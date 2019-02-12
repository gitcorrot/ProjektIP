#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h> //Needed to open files
#include <string.h>
#include <ctype.h> //Contains tolower()
#include <stdlib.h>

int find_username(char *username, char *hashed_password_from_file, char *salt);
int compare_passwords(char *user_password, char *hashed_password_from_file, char *salt);

int main()
{
    char username[80];
    char user_password[80];
    char salt[3];
    char hash_and_salt[14];

    
    do
    {
        printf("Enter username: ");
        fgets(username,80,stdin);
        username[strcspn(username, "\n")] = 0;

        for (int i = 0; username[i]; i++)
        {
            username[i] = tolower(username[i]);
        }
    }
    while(find_username(username, hash_and_salt, salt) != 0);

    do
    {
        printf("Enter password: ");
        fgets(user_password,80,stdin);
        user_password[strcspn(user_password, "\n")] = 0;

    }
    while(compare_passwords(user_password, hash_and_salt, salt) != 0);
    
    
}

int compare_passwords(char *user_password, char *hash_and_salt, char *salt)
    {
        char *hashed_user_password = crypt(user_password, salt);
        hashed_user_password[14] = 0;
        if (strcmp(hash_and_salt, hashed_user_password) == 0)
        {
            printf("Logged in.");
            return(0);
        }
        printf("Password is incorrect.\n");
        return(-1);
    }


int find_username(char *username, char *hash_and_salt, char *salt){
    char line[94];
    char username_in_file[80];
    char *space;
    int length;

    FILE* fp = fopen("passwords.txt", "r");
    if (fp == NULL)
        {
            printf("Error opening file!\n");
            exit(1);
        }

    while (fgets(line, 94, fp)!= NULL)
    {
        space = strchr(line, ' ');
        length = space - line;
        memcpy(username_in_file, line, length);
        username_in_file[length] = 0;
        if (strcmp(username_in_file, username) == 0)
        {
            //User exists
            char *space = strchr(line, ' ');
            memcpy(salt, space+1, 2);
            salt[3] = 0;
            memcpy(hash_and_salt, space+1, 13);
            hash_and_salt[14] = 0;
            return(0);
        }
    }
    printf("User does not exist.\n");
    return(-1);
}
