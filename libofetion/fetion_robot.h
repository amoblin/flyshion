#include <sys/socket.h>
#define BUFLEN 1024

int init_arg(int argc, char *argv[], char **_mobileno, char **_password, char **_command);

/* 避免利用命令行参数执行其他命令 */
int check_command(char *command, char *safe_command);

/*  */
int fx_login(User *user, const char *mobileno, const char *password);

/* 飞信机器人守护进程 */
int fetion_robot_daemon(User *user);

void usage(char *command);
