#include "fetion_robot.h"
#define BUFLEN 1024

/* 避免利用命令行参数执行其他命令 */
int check_command(char *command, char *safe_command)
{
    int i;
    int j = 0;
    for(i=0; i<strlen(command);i++) {
        switch(command[i]) {
            case ';':
                safe_command[j++] = ':';
                break;
            case '"':
                safe_command[j++] = '\\';
                safe_command[j++] = '"';
                break;
            case '`':
                safe_command[j++] = '\'';
                break;
            default:
                safe_command[j++] = command[i];
        }
    }
}

int process_new_message(User *user, Message *sip_msg, char out_message[], char command[])
{
    char command_str[BUFLEN];
    memset(command_str, 0, BUFLEN);
    strncpy(command_str, command, BUFLEN);
    char safe_command[BUFLEN];
    memset(safe_command, 0, sizeof(safe_command));
    check_command(sip_msg->message, safe_command);
    strcat(command_str, safe_command);
    FILE *pp;
    if( (pp = popen(command_str, "r")) == NULL) {
        debug_info("Error! popen() failed!");
        return 1;
    }
    //debug_info("execute: %s", command_str);
    fread(out_message, sizeof(char), BUFLEN, pp);
    pclose(pp);
    /* 发送消息 */
    Conversation *conv = fetion_conversation_new(user, sip_msg->sipuri, NULL);
    //if(fetion_conversation_send_sms_with_reply(conv, out_message) == -1) {
    if(fetion_conversation_send_sms(conv, out_message) == -1) {
        debug_info("Error! reply to %s failed!", sip_msg->sipuri);
    } else {
        debug_info(out_message);
    }
    memset(out_message, 0, BUFLEN);
    return 0;
}

int process_presence(User *user)
{
	Contact      *contactlist;
	Contact      *contact;
	//contactlist = fetion_user_parse_presence_body(xml , user);
	//contact = contactlist;
	//foreach_contactlist(contactlist , contact){
    //}
}

