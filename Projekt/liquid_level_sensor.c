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
void saveValueToSharedMemory();
int readValueFromSharedMemory();
long int strtoint();

const char NAME[] = "liquid_level";
int SHARED_MEMORY_SIZE;
int sharedMemory_descriptor;
void *pointer;

int liquid_level;

// THIS PROCESS IS RESPONSIBLE FOR READING LIQUID LEVEL SENSOR DATA AND
// CONTROLL MEASURE AND JOGHURT VALVES

// ( liquid_level == 100 -> joghurt_valve = false )

int main()
{
    initiate();

    while (1)
    {
        usleep(200 * 1000);
        printf("----------------------------------------\n");

        liquid_level = readValueFromSharedMemory(NAME);

        if (liquid_level == 100)
        {
            saveValueToSharedMemory("joghurt_valve", 0);
        }
        else if (liquid_level > 100)
        {
            saveValueToSharedMemory("liquid_level", 100);
        }
        else if (liquid_level < 0)
        {
            saveValueToSharedMemory("liquid_level", 0);
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

    sprintf(pointer, "%d", sm_value);
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

long int strtoint(char *str)
{
    return (strtol(str, NULL, 10));
}
