#include "openfetion.h"

/* 避免利用命令行参数执行其他命令 */
int check_command(char *command, char *safe_command);

/* 处理收到的信息 */
int process_new_message(User *user, Message *sip_msg, char out_message[], char command[]);

/* 改变状态时的消息处理 */
int process_presence(User *user);
