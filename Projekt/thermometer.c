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

int cooler;
int heater;
int current_temperature;
int nominal_temperature;

// THIS PROCESS IS RESPONSIBLE FOR READING THERMOMETER'S DATA
// AND CONTROLING THERMOMETER

int main()
{
    initiate();

    while (1)
    {
        // execute once per 3 seconds
        sleep(3);
        printf("----------------------------------------\n");

        // update (get form shared memory) variables
        updateData();

        if (current_temperature == nominal_temperature)
        {
            saveValueToSharedMemory("cooler", 0);
            saveValueToSharedMemory("heater", 0);
        }

        //current_temperature > nominal_temperature -> cooler = true
        else if (current_temperature > nominal_temperature)
        {
            saveValueToSharedMemory("cooler", 1);
            saveValueToSharedMemory("heater", 0);
        }

        //current_temperature < nominal_temperature -> heater = false
        else if (current_temperature < nominal_temperature)
        {
            saveValueToSharedMemory("heater", 1);
            saveValueToSharedMemory("cooler", 0);
        }

        // update (get form shared memory) variables
        updateData();

        if (heater == 1)
        {
            // increase current_temperature
            saveValueToSharedMemory("current_temperature", current_temperature + 1);
        }

        else if (cooler == 1)
        {
            // decrease current_temperature
            saveValueToSharedMemory("current_temperature", current_temperature - 1);
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
    cooler = readValueFromSharedMemory("cooler");
    heater = readValueFromSharedMemory("heater");
    current_temperature = readValueFromSharedMemory("current_temperature");
    nominal_temperature = readValueFromSharedMemory("nominal_temperature");
}

long int strtoint(char *str)
{
    return (strtol(str, NULL, 10));
}
