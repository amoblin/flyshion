#include "openfetion.h"

int main(int argc, char *argv[])
{
    User *user;
    int (*process_function[10])(User *user, Message *sip_msg, char *);
    process_function[0] = execute_command;
    process_function[9] = execute_command_with_args;
    fetion_robot_daemon(argc, argv, &user, process_function);
    return 0;
}
