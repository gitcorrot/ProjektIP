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

#define SAVE_TIME 200 // Time in miliseconds

void printAllSharedMemoryObjects();
int readValueFromSharedMemory();

/* the SHARED_MEMORY_SIZE (in bytes) of shared memory object */
int SHARED_MEMORY_SIZE;

/* names of the shared memory objects */
const char *names[] = {"liquid_level", "current_temperature", "nominal_temperature",
                       "current_flow", "nominal_flow", "work", "next", "heater",
                       "cooler", "mixer", "release", "joghurt_valve", "measure_valve"};

/* shared memory file descriptor */
int sharedMemory_descriptor;

/* pointer to shared memory object */
void *pointer;

int main()
{
    SHARED_MEMORY_SIZE = sysconf(_SC_PAGESIZE);

    printf("Starting saving data!\n\n");

    while (1)
    {
        printf("Saving...\n");
        fflush(stdout);
        usleep(SAVE_TIME * 1000);

        FILE *f = fopen("data.txt", "w");

        for (int i = 0; i < sizeof(names) / sizeof(names[0]); i++)
        {
            /* create the shared memory object */
            sharedMemory_descriptor = shm_open(names[i], O_RDONLY, 0666);

            /* configure the SHARED_MEMORY_SIZE of the shared memory object */
            ftruncate(sharedMemory_descriptor, SHARED_MEMORY_SIZE);

            /* memory map the shared memory object */
            pointer = mmap(0, SHARED_MEMORY_SIZE, PROT_READ, MAP_SHARED, sharedMemory_descriptor, 0);

            /* create a semaphore in locked state */

            sem_t *sem_des = sem_open(names[i], O_CREAT, 0644, 0);

            /* save data to txt file */
            fprintf(f, "%s %s\n", names[i], (char *)pointer);

            /* Release the semaphore lock */
            sem_post(sem_des);
            munmap(pointer, SHARED_MEMORY_SIZE);

            /* Close the shared memory object */
            close(sharedMemory_descriptor);

            /* Close the Semaphore */
            sem_close(sem_des);
        }
        fclose(f);
    }
    return 0;
}
