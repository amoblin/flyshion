#include "openfetion.h"

int bistu_process_message(User *user, Message *sip_msg, char *out_message)
{
    if(strcmp(sip_msg->sipuri, "856882346@fetion.com.cn;p=5911") != 0) {
        execute_command_with_args(user, sip_msg, out_message);
        return 0;
    }
    /* -b message 群发消息message */
    if(sip_msg->message[0] == '-' && sip_msg->message[1] == 'b') {
        Contact *contactlist;
        debug_info("%p", sip_msg->message+2);
        strcpy(out_message, sip_msg->message+2);
        foreach_contactlist(user->contactList, contactlist) {
            debug_info("send message");
            //fetion_robot_send_msg(user, contactlist->sipuri, out_message);
        }
        return 0;
    }
    debug_info("here");
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
