#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int parse_command(char *str, int *broadcast, char sid[], char message[])
{
    int ch;
    char *command[7];
    char *p;
    char new_str[1024];
    strcat(new_str, "./a.out ");
    strcat(new_str, str);
    p = strtok(new_str, " ");
    int i=0;
    while(p) {
        command[i] = malloc(strlen(p));
        strcpy(command[i++], p);
        p = strtok(NULL, " ");
    }
    while((ch = getopt(i, command, "bf:m:")) != -1) {
        switch(ch) {
            case 'b':
                *broadcast = 1;
                break;
            case 'f':
                *broadcast = 0;
                strncpy(sid, optarg, strlen(optarg));
                break;
            case 'm':
                strncpy(message, optarg, strlen(optarg));
                break;
            default:
                break;
        }
    }
    return 0;
}

int main()
{
    char str[] = "-b -m hello world";
    int broadcast;
    char sid[12];
    char message[1024];
    memset(sid, 0, 12);
    memset(message, 0, 1024);
    parse_command(str, &broadcast, sid, message);
    printf("%d\n", broadcast);
    printf("%s\n", sid);
    printf("%s\n", message);
}
