#define _XOPEN_SOURCE

#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h> // Contains tolower()
#include <stdlib.h>

// LOGGING

int login();
int find_username(char *username, char *hashed_password_from_file, char *salt);
int compare_passwords(char *user_password, char *hashed_password_from_file, char *salt);

// OPERATOR STUFF

void menu();
void createAllSharedMemoryObjects();
void initiateAllSharedMemoryObjects();
void saveValueToSharedMemory();
void deleteAllSharedMemoryObjects();
void initiate();

// VARIABLES

int logged = 0;
int sharedMemory_descriptor;
void *pointer;
int SHARED_MEMORY_SIZE;
const char *sm_names[] = {"liquid_level", "current_temperature", "nominal_temperature",
                          "current_flow", "nominal_flow", "work", "next", "heater",
                          "cooler", "mixer", "release", "joghurt_valve", "measure_valve"};

int main()
{
    printf("Welcome to Joghturt Bottler!\n\n");

    logged = login();

    if (logged == 1)
    {

        initiate();

        menu();

        deleteAllSharedMemoryObjects();
    }

    return 0;
}

/**
***
***     LOGGING METHODS
***
**/

int login()
{
    char username[80];
    char user_password[80];
    char salt[3];
    char hash_and_salt[14];

    do
    {
        printf("Enter username: ");
        fgets(username, 80, stdin);
        username[strcspn(username, "\n")] = 0;

        for (int i = 0; username[i]; i++)
        {
            username[i] = tolower(username[i]);
        }
    } while (find_username(username, hash_and_salt, salt) != 0);

    do
    {
        printf("Enter password: ");
        fgets(user_password, 80, stdin);
        user_password[strcspn(user_password, "\n")] = 0;
    } while (compare_passwords(user_password, hash_and_salt, salt) != 0);

    return 1;
}

int compare_passwords(char *user_password, char *hash_and_salt, char *salt)
{
    char *hashed_user_password = crypt(user_password, salt);
    hashed_user_password[14] = 0;
    if (strcmp(hash_and_salt, hashed_user_password) == 0)
    {
        printf("Logged in.\n");
        return (0);
    }
    printf("Password is incorrect.\n");
    return (-1);
}

int find_username(char *username, char *hash_and_salt, char *salt)
{
    char line[94];
    char username_in_file[80];
    char *space;
    int length;

    FILE *fp = fopen("passwords.txt", "r");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    while (fgets(line, 94, fp) != NULL)
    {
        space = strchr(line, ' ');
        length = space - line;
        memcpy(username_in_file, line, length);
        username_in_file[length] = 0;
        if (strcmp(username_in_file, username) == 0)
        {
            //User exists
            char *space = strchr(line, ' ');
            memcpy(salt, space + 1, 2);
            salt[3] = 0;
            memcpy(hash_and_salt, space + 1, 13);
            hash_and_salt[14] = 0;
            return (0);
        }
    }
    printf("User does not exist.\n");
    return (-1);
}

/**
***
***     OPERATOR'S METHODS
***
**/

void initiate()
{
    SHARED_MEMORY_SIZE = sysconf(_SC_PAGESIZE);

    createAllSharedMemoryObjects();

    // assign initial values to sm objects
    initiateAllSharedMemoryObjects();
}

// This function generate operator command prompt

void menu()
{

    while (1)
    {

        printf("Please enter command: ");

        char command[24] = "";
        scanf("%s", command);

        // compare string to strings from command list

        if (strcmp(command, "shut_down") == 0)
        {
            system("clear");
            printf("Bye!");
            fflush(stdout);
            sleep(1);
            deleteAllSharedMemoryObjects();

            return;
        }
        else if (strcmp(command, "set_temperature") == 0)
        {

            int temperature;

            do
            {
                printf("\nPut temperature: ");

                scanf("%d", &temperature);
                if (temperature < 0)
                {
                    system("clear");
                    printf("That's too cold!");

                    fflush(stdout);
                    sleep(2);
                                        system("clear");
                }
            } while (temperature < 0);

            printf("\nAre you sure, that you want to set temperature to %d?", temperature);
            printf("\n\n[y,n]\n\n");

            char c;
            scanf(" %c", &c);

            if (c == 'y')
            {

                // assign read temperature as nominal_temperature
                saveValueToSharedMemory("nominal_temperature", temperature);

                system("clear");
                printf("Nominal temperature set to %d C\n", temperature);
                fflush(stdout);
                sleep(2);
            }
            else if (c == 'n')
            {
            }
            else
            {
                system("clear");
                printf("Something went wrong. Try again!");
                fflush(stdout);
                sleep(2);
            }
        }
        else if (strcmp(command, "set_flow") == 0)
        {

            int flow;
            printf("\nPut flow: ");
            scanf("%d", &flow);

            printf("\nAre you sure, that you want to set flow to %d?", flow);
            printf("\n\n[y,n]\n\n");

            char c;
            scanf(" %c", &c);

            if (c == 'y')
            {

                // assign read flow as nominal_flow
                saveValueToSharedMemory("nominal_flow", flow);

                system("clear");
                printf("Nominal flow set to %d C\n", flow);
                fflush(stdout);
                sleep(2);
            }
            else if (c == 'n')
            {
            }
            else
            {
                system("clear");
                printf("Something went wrong. Try again!");
                fflush(stdout);
                sleep(2);
            }
        }
        else if (strcmp(command, "next") == 0)
        {

            // WRITIE NEXT TO SHARED MEMORY
            saveValueToSharedMemory("next", 1);
        }
        else if (strcmp(command, "help") == 0)
        {
            system("clear");
            printf("Commands you can use:\n\nnext - put next cup\nset_temperature\nset_flow\nshut_down - unlinks semaphores and shared memory objects\n\nPress ENTER to continue");
            fflush(stdout);
            while (getchar() != '\n')
                ; // clean stdin
            getchar();
        }
        else
        {
            printf("\n%s is wrong command! Try again.", command);
            fflush(stdout);
            sleep(2);
        }

        fflush(stdout);
        system("clear");
    }
}

void createAllSharedMemoryObjects()
{
    for (int i = 0; i < sizeof(sm_names) / sizeof(sm_names[0]); i++)
    {

        if ((sharedMemory_descriptor = shm_open(sm_names[i], O_CREAT | O_RDWR, 0666)) == -1)
        {
            perror("shm_open failure while creating shared memory objects");
            _exit(-1);
        }
        if ((ftruncate(sharedMemory_descriptor, SHARED_MEMORY_SIZE)) == -1)
        {
            perror("ftruncate failure while creating shared memory objects");
            _exit(-1);
        }
        pointer = mmap(0, SHARED_MEMORY_SIZE, PROT_READ, MAP_SHARED, sharedMemory_descriptor, 0);
    }
}

void initiateAllSharedMemoryObjects()
{

    /**
    ***
    ***   SET THIS VARIABLES' INITIAL STATES TO SHARED MEMORY
    ***
    *** "liquid_level", "temperature", "flow", "work", "next", "cooler", "heater", "release" ...
    ***
    ***       0              32         10       0       0        0         0          0`
    **/

    int VALUE;

    for (int i = 0; i < sizeof(sm_names) / sizeof(sm_names[0]); i++)
    {
        if (strcmp(sm_names[i], "liquid_level") == 0)
            VALUE = 0;
        else if (strcmp(sm_names[i], "current_temperature") == 0)
            VALUE = 26;
        else if (strcmp(sm_names[i], "nominal_temperature") == 0)
            VALUE = 26;
        else if (strcmp(sm_names[i], "current_flow") == 0)
            VALUE = 10;
        else if (strcmp(sm_names[i], "nominal_flow") == 0)
            VALUE = 10;
        else if (strcmp(sm_names[i], "work") == 0)
            VALUE = 0;
        else if (strcmp(sm_names[i], "next") == 0)
            VALUE = 0;
        else if (strcmp(sm_names[i], "heater") == 0)
            VALUE = 0;
        else if (strcmp(sm_names[i], "cooler") == 0)
            VALUE = 0;
        else if (strcmp(sm_names[i], "mixer") == 0)
            VALUE = 0;
        else if (strcmp(sm_names[i], "release") == 0)
            VALUE = 0;
        else if (strcmp(sm_names[i], "joghurt_valve") == 0)
            VALUE = 0;
        else if (strcmp(sm_names[i], "measure_valve") == 0)
            VALUE = 0;
        else
        {
            printf("\nError while assigning %s!", sm_names[i]);
            return;
        }

        saveValueToSharedMemory(sm_names[i], VALUE);
    }
}

void saveValueToSharedMemory(char sm_name[], int sm_value)
{

    sharedMemory_descriptor = shm_open(sm_name, O_RDWR, 0666);

    // Open a semaphore in locked state

    sem_t *sem_des = sem_open(sm_name, O_CREAT, 0644, 0);

    // Map a memory region into a process's address space

    pointer = mmap(0, SHARED_MEMORY_SIZE, PROT_WRITE, MAP_SHARED, sharedMemory_descriptor, 0);

    // Write to shared memory

    sprintf(pointer, "%d", sm_value);
    printf("Writing %d to \"%s\"\n", sm_value, sm_name);

    // Release the semaphore lock

    sem_post(sem_des);

    // Unmap a memory region from a process's address space

    munmap(pointer, SHARED_MEMORY_SIZE);

    // Close the shared memory object

    close(sharedMemory_descriptor);

    // Close the Semaphore

    sem_close(sem_des);
}

// This method deletes all shared memory objects and its semaphores

void deleteAllSharedMemoryObjects()
{
    for (int i = 0; i < sizeof(sm_names) / sizeof(sm_names[0]); i++)
    {
        shm_unlink(sm_names[i]);
        sem_unlink(sm_names[i]);
    }
}