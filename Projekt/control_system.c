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

int liquid_level;
int current_temperature;
int current_flow;
int work;
int next;
int heater;
int cooler;
int mixer;
int release;
int joghurt_valve;
int measure_valve;

// THIS PROCESS EXECUTE ALGORTRHM'S ACTIONS.

int main()
{
    initiate();

    while (1)
    {
        usleep(200 * 1000);
        printf("--------------algorithm--------------\n");

        updateData();

        // next && !work -> joghurt_valve = true && work = true && next = false
        if (next == 1 && work == 0)
        {
            saveValueToSharedMemory("joghurt_valve", 1);
            saveValueToSharedMemory("work", 1);
            saveValueToSharedMemory("next", 0);
        }

        if (joghurt_valve == 1 && liquid_level < 100)
        {
            // increase liquid_level
            saveValueToSharedMemory("liquid_level", liquid_level + current_flow);
        }

        if (release == 1 && liquid_level > 0)
        {
            // decrease liquid_level
            saveValueToSharedMemory("liquid_level", liquid_level - current_flow);

            // if next while release -> !next
            saveValueToSharedMemory("next", 0);
        }

        // liquid_level == 100 && !joghurt_valve-> measure_valve = true && release = true
        if (liquid_level == 100 && joghurt_valve == 0)
        {
            saveValueToSharedMemory("measure_valve", 1);
            saveValueToSharedMemory("release", 1);
        }

        // liquid_level == 0 && release-> measure_valve = false && work = false && release = false
        if (liquid_level == 0 && release == 1)
        {
            saveValueToSharedMemory("measure_valve", 0);
            saveValueToSharedMemory("work", 0);
            saveValueToSharedMemory("release", 0);
        }

        // w mainie upchnąć obniżanie temperatury co 20 sekund (jeżeli nie działa heater)
        // polecenie zmniejszania prędkości żeby flowmeter wykrył i włączył mieszadło
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

void updateData()
{
    liquid_level = readValueFromSharedMemory("liquid_level");
    current_temperature = readValueFromSharedMemory("current_temperature");
    current_flow = readValueFromSharedMemory("current_flow");
    work = readValueFromSharedMemory("work");
    next = readValueFromSharedMemory("next");
    heater = readValueFromSharedMemory("heater");
    cooler = readValueFromSharedMemory("cooler");
    mixer = readValueFromSharedMemory("mixer");
    release = readValueFromSharedMemory("release");
    joghurt_valve = readValueFromSharedMemory("joghurt_valve");
    measure_valve = readValueFromSharedMemory("measure_valve");
}

long int strtoint(char *str)
{
    return (strtol(str, NULL, 10));
}
