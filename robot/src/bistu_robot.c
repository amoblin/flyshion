#include "openfetion.h"

int bistu_process_message(User *user, Message *sip_msg, char *out_message)
{
    char master_sip[] = "sip:856882346@fetion.com.cn;p=5911";
    if(strcmp(sip_msg->sipuri, master_sip ) != 0) {
        execute_command_with_args(user, sip_msg, out_message);
        return 0;
    }
    /* -b message 广播消息 */
    if(sip_msg->message[0] == '-' && sip_msg->message[1] == 'b' && sip_msg->message[2] == ' ') {
        Contact *contactlist;
        strcpy(out_message, sip_msg->message+3);
        debug_info(out_message);
        foreach_contactlist(user->contactList, contactlist) {
            fetion_robot_send_msg(user, contactlist->sipuri, out_message);
        }
        return 0;
    }
    execute_command_with_args(user, sip_msg, out_message);
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
