#include "openfetion.h"

int main(int argc, char *argv[])
{
    User *user;
    int (*process_function[10])(User *user, Message *sip_msg, char *);
    process_function[0] = execute_command;
    fetion_robot_daemon(argc, argv, &user, process_function);
    return 0;
}
