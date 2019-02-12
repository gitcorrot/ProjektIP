#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h> 
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> //Contains tolower()

void get_salt(char *s);
int find_username(char *username);

int main()
{
    FILE *f = fopen("passwords.txt", "a"); //Creates an empty file if there isn't one.
    fclose(f);                             //Does nothing if the file already exist.

    time_t tt;
    int seed = time(&tt);
    srand(seed);

    char username[80];
    char user_password[80];
    char salt[3];
    printf("Enter username: ");
    fgets(username, 80, stdin);
    username[strcspn(username, "\n")] = 0;

    for (int i = 0; username[i]; i++)
    {
        username[i] = tolower(username[i]);
    }

    while (find_username(username) != 0)
    {
        fgets(username, 80, stdin);
        username[strcspn(username, "\n")] = 0;

        for (int i = 0; username[i]; i++)
        {
            username[i] = tolower(username[i]);
        }
    }

    printf("Enter password: ");
    fgets(user_password, 80, stdin);
    user_password[strcspn(user_password, "\n")] = 0;

    get_salt(salt);
    char *hashed = crypt(user_password, salt);
    f = fopen("passwords.txt", "a");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    fprintf(f, "%s %s\n", username, hashed);

    fclose(f);
}

void get_salt(char *s)
{
    static const char available_characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789./";

    for (int i = 0; i < 2; i++)
    {
        s[i] = available_characters[rand() % (sizeof(available_characters) - 1)];
    }

    s[2] = 0;
}

int find_username(char *username)
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
        if (strcmp(username_in_file, username) == 0)
        {
            printf("Username already exists.\n");
            printf("Enter username: ");
            return (-1);
        }
    }
    return (0);
}
