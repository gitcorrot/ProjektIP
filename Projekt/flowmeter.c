#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void initiate();
void updateData();
void saveValueToSharedMemory();
int readValueFromSharedMemory();
long int strtoint();

int SHARED_MEMORY_SIZE;
int sharedMemory_descriptor;
void *pointer;

int current_flow;
int nominal_flow;
int mixer;

// THIS PROCESS IS RESPONSIBLE FOR READING FLOWMETER'S DATA
// AND CONTROLING FLOWMETER

int main()
{
    initiate();

    while (1)
    {
        // execute once per 5 seconds
        sleep(5);
        printf("----------------------------------------\n");

        // update (get form shared memory) variables
        updateData();

        if (current_flow < 0)
        {
            saveValueToSharedMemory("current_flow", 0);
        }

        // current_flow < nominal_flow -> mixer = true
        if (current_flow < nominal_flow)
        {
            saveValueToSharedMemory("mixer", 1);
        }

        // current_flow >= nominal_flow -> mixer = false
        else if (current_flow >= nominal_flow)
        {
            saveValueToSharedMemory("mixer", 0);
        }

        // update (get form shared memory) variables
        updateData();

        if (mixer == 1 && current_flow != nominal_flow)
        {
            // increase current_flow
            saveValueToSharedMemory("current_flow", current_flow + 1);
        }

        else if (mixer == 0 && current_flow != nominal_flow)
        {
            // decrease current_flow
            saveValueToSharedMemory("current_flow", current_flow - 1);
        }
    }

    return 0;
}

void initiate()
{
    SHARED_MEMORY_SIZE = sysconf(_SC_PAGESIZE);
}

void saveValueToSharedMemory(char sm_name[], int sm_value)
{

    sharedMemory_descriptor = shm_open(sm_name, O_RDWR, 0666);

    sem_t *sem_des = sem_open(sm_name, O_CREAT, 0644, 0);

    pointer = mmap(0, SHARED_MEMORY_SIZE, PROT_WRITE, MAP_SHARED, sharedMemory_descriptor, 0);

    sprintf(pointer, "%d\n", sm_value);
    printf("Writing %d to \"%s\"\n", sm_value, sm_name);

    sem_post(sem_des);
    munmap(pointer, SHARED_MEMORY_SIZE);

    close(sharedMemory_descriptor);

    sem_close(sem_des);
}

int readValueFromSharedMemory(char sm_name[])
{

    int sm_value = -1;

    /* create the shared memory object */
    sharedMemory_descriptor = shm_open(sm_name, O_RDONLY, 0666);

    /* configure the size of the shared memory object */
    ftruncate(sharedMemory_descriptor, SHARED_MEMORY_SIZE);

    /* memory map the shared memory object */
    pointer = mmap(0, SHARED_MEMORY_SIZE, PROT_READ, MAP_SHARED, sharedMemory_descriptor, 0);

    /* create a semaphore in locked state */
    sem_t *sem_des = sem_open(sm_name, O_CREAT, 0644, 0);

    /* read value and save to sm_value as integer */

    sm_value = strtoint((char *)pointer);
    printf("%s : %d\n", sm_name, sm_value);

    /* Release the semaphore lock */
    sem_post(sem_des);
    munmap(pointer, SHARED_MEMORY_SIZE);

    /* Close the shared memory object */
    close(sharedMemory_descriptor);

    /* Close the Semaphore */
    sem_close(sem_des);

    return sm_value;
}

void updateData()
{
    current_flow = readValueFromSharedMemory("current_flow");
    nominal_flow = readValueFromSharedMemory("nominal_flow");
    mixer = readValueFromSharedMemory("mixer");
}

long int strtoint(char *str)
{
    return (strtol(str, NULL, 10));
}
