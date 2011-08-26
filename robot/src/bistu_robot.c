#include "openfetion.h"

char master_sip[] = "sip:856882346@fetion.com.cn;p=5911";

typedef enum {
BROADCAST,
UNICAST,
COMMON_MESSAGE
} CastType;

int parse_command(char *str, CastType *type, char sid[], char message[])
{
    int ch;
    char *command[7];
    char *p;
    char new_str[1024];
    memset(new_str, 0, sizeof(new_str));
    strcpy(new_str, "./a.out ");
    strcat(new_str, str);
    p = strtok(new_str, " ");
    int i=0;
    while(p) {
        command[i] = malloc(strlen(p));
        strcpy(command[i++], p);
        p = strtok(NULL, " ");
    }
    optind = 1;
    while((ch = getopt(i, command, "bf:m:")) != -1) {
        switch(ch) {
            case 'b':
                *type = BROADCAST;
                break;
            case 'f':
                *type = UNICAST;
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

int bistu_process_message(User *user, Message *sip_msg, char *out_message)
{
    if(strcmp(sip_msg->sipuri, master_sip ) != 0) {
        execute_command_with_args(user, sip_msg, out_message);
        return 0;
    }
    CastType type=COMMON_MESSAGE;
    char no[16];
    char cast_message[BUFLEN];
    parse_command(sip_msg->message, &type, no, cast_message);
    Contact *contact;
    switch(type) {
        case UNICAST:
            /* 特定用户发消息 */
            strcpy(out_message, cast_message);
            contact=fetion_contact_list_find_by_sipuri(user->contactList, no);
            if(contact) {
                fetion_robot_send_msg(user, contact->sipuri, out_message);
                sprintf(out_message, "单播%s：", no);
                strcat(out_message, cast_message);
            } else {
                strcpy(out_message, "sipuri does not exist.");
            }
            break;
        case BROADCAST:
            /* -b message 广播消息 */
            strcpy(out_message, cast_message);
            foreach_contactlist(user->contactList, contact) {
                strcpy(out_message, cast_message);
                //fetion_robot_send_msg(user, contact->sipuri, out_message);
            }
            strcpy(out_message, "广播消息：");
            strcat(out_message, cast_message);
            break;
        case COMMON_MESSAGE:
            execute_command_with_args(user, sip_msg, out_message);
            break;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    User *user;
    int (*process_function[10])(User *user, Message *sip_msg, char *);
    process_function[9] = bistu_process_message;
    fetion_robot_daemon(argc, argv, &user, process_function);
    return 0;
}
