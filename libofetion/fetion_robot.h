#include <sys/socket.h>
#define BUFLEN 1024

/* 使用命令对消息进行处理时才用到，保存命令值 */
char Command[BUFLEN];

/* 封装的登陆过程 */
int fx_login(User **user, const char *mobileno, const char *password);

/* 避免利用命令行参数执行其他命令 */
int check_command(char *command, char *safe_command);

/* 飞信消息作为命令参数执行  */
int execute_command_with_args(User *user, Message *sip_msg, char out_message[]);

/* 飞信消息作为命令执行(不安全，仅供私人使用)，适合系统管理员 */
int execute_command(User *user, Message *sip_msg, char out_message[]);

/* 封装的发送消息 */
int fetion_robot_send_msg(User *user, char *sipuri, char out_message[]);

int process_notification(User *user, const char* sipmsg, int (**process_function)(User *, Message *, char *), char *out_message);

/* 封装的消息处理，消息在process_function*/
int fetion_robot_daemon(int argc, char *argv[], User **user, int (**process_function)(User *, Message *, char *));

/* 邀请处理 */
void process_invitation(User *user, SipMsg *msg, char* out_message);

void usage(char *command);
