#include <sys/socket.h>
#define BUFLEN 1024

/* 封装的登陆过程 */
int fx_login(User **user, const char *mobileno, const char *password);

/* 避免利用命令行参数执行其他命令 */
int check_command(char *command, char *safe_command);

/* 飞信消息作为命令参数执行  */
int execute_commandk_with_args(User *user, Message *sip_msg, char out_message[], char command[]);

/* 飞信消息作为命令执行(不安全，仅供私人使用)，适合系统管理员 */
int execute_command(User *user, Message *sip_msg, char out_message[]);

/* 封装的发送消息 */
int fetion_robot_send_msg(User *user, Message *sip_msg, char out_message[]);

/* 封装的消息处理，消息在process_function*/
int fetion_robot_daemon(int argc, char *argv[], User **user, int (**process_function)(User *, Message *, char *));

void usage(char *command);
