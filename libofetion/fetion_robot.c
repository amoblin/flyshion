/***************************************************************************
 *   Copyright (C) 2010 by lwp                                             *
 *   levin108@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#include "openfetion.h"

int fx_login(User **user_p, const char *mobileno, const char *password)
{
	Config           *config;
	FetionConnection *tcp;
    FetionSip        *sip;
	char             *res;
	char             *nonce;
	char             *key;
	char             *aeskey;
	char             *response;
	int               local_group_count;
	int               local_buddy_count;
	int               group_count;
	int               buddy_count;
	int               ret;
 
	/* construct a user object */
    User *user = fetion_user_new(mobileno, password);
    *user_p = user;
	/* construct a config object */
	config = fetion_config_new();
	/* attach config to user */
	fetion_user_set_config(user, config);
 
login:
	/* start ssi authencation,result string needs to be freed after use */
	res = ssi_auth_action(user);
	/* parse the ssi authencation result,if success,user's sipuri and userid
	 * are stored in user object,orelse user->loginStatus was marked failed */
	parse_ssi_auth_response(res, user);
	free(res);

	/* whether needs to input a confirm code,or login failed
	 * for other reason like password error */
	if(USER_AUTH_NEED_CONFIRM(user) || USER_AUTH_ERROR(user)) {
		debug_error("authencation failed");
		return 1;
	}
 
	/* initialize configuration for current user */
	if(fetion_user_init_config(user) == -1) {
		debug_error("initialize configuration");
		return 1;
	}
 
	if(fetion_config_download_configuration(user) == -1) {
		debug_error("download configuration");
		return 1;
	}
 
	/* set user's login state to be online*/
	fetion_user_set_st(user, P_ONLINE);

	/* load user information and contact list information from local host */
	fetion_user_load(user);
	fetion_contact_load(user, &local_group_count, &local_buddy_count);
 
	/* construct a tcp object and connect to the sipc proxy server */
	tcp = tcp_connection_new();
	if((ret = tcp_connection_connect(tcp, config->sipcProxyIP, config->sipcProxyPort)) == -1) {
		debug_error("connect sipc server %s:%d\n", config->sipcProxyIP, config->sipcProxyPort);
		return 1;
	}
 
	/* construct a sip object with the tcp object and attach it to user object */
	sip = fetion_sip_new(tcp, user->sId);
	fetion_user_set_sip(user, sip);
 
	/* register to sipc server */
	if(!(res = sipc_reg_action(user))) {
		debug_error("register to sipc server");
		return 1;
	}
 
	parse_sipc_reg_response(res, &nonce, &key);
	free(res);
	aeskey = generate_aes_key();
 
	response = generate_response(nonce, user->userId, user->password, key, aeskey);
	free(nonce);
	free(key);
	free(aeskey);
 
	/* sipc authencation,you can printf res to see what you received */
	if(!(res = sipc_aut_action(user, response))) {
		debug_error("sipc authencation");
		return 1;
	}
 
	if(parse_sipc_auth_response(res, user, &group_count, &buddy_count) == -1) {
		debug_error("authencation failed");
		return 1;
	}
 
	free(res);
	free(response);
 
	if(USER_AUTH_ERROR(user)) {
		debug_error("Password error , login failed!!!");
		return 1;
	}
	if(USER_AUTH_NEED_CONFIRM(user)) {
        char codePath[256];
        char code[16];
        memset(codePath, 0, sizeof(codePath));
        memset(code, 0, sizeof(code));

        generate_pic_code(user);
        sprintf(codePath , "%s/code.gif" , user->config->globalPath);
        debug_info("saved in:%s", codePath);
		debug_info(user->verification->text);
		//debug_info(user->verification->tips);
        int i=0;
        char ch;
        while((ch=getchar())!='\n') {
            code[i++] = ch;
        }
		debug_info("Input verfication code:%s" , code);
        fetion_user_set_verification_code(user , code);
        goto login;
	}
 
	/* save the user information and contact list information back to the local database */
	fetion_user_save(user);
	fetion_contact_save(user);
 
	/* these... fuck the fetion protocol */
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	char buf[1024];
	if(setsockopt(user->sip->tcp->socketfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
		debug_error("settimeout");
		return 1;
	}
	tcp_connection_recv(user->sip->tcp, buf, sizeof(buf));
 
	return 0;
}
 
/* 对参数进行检查，避免利用命令行参数执行其他命令 */
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

int execute_command_with_args(User *user, Message *sip_msg, char out_message[])
{
    char command_str[BUFLEN];
    memset(command_str, 0, BUFLEN);
    strncpy(command_str, Command, BUFLEN);
    char safe_command[BUFLEN];
    memset(safe_command, 0, sizeof(safe_command));
    check_command(sip_msg->message, safe_command);
    strcat(command_str, safe_command);
    FILE *pp;
    if( (pp = popen(command_str, "r")) == NULL) {
        debug_info("Error! popen() failed!");
        return 1;
    }
    debug_info("execute: %s", command_str);
    fread(out_message, sizeof(char), BUFLEN, pp);
    pclose(pp);
    return 0;
}

int execute_command(User *user, Message *sip_msg, char out_message[])
{
    FILE *pp;
    if( (pp = popen(sip_msg->message, "r")) == NULL) {
        debug_info("Error! popen() failed!");
        return 1;
    }
    //debug_info("execute: %s", command_str);
    fread(out_message, sizeof(char), BUFLEN, pp);
    pclose(pp);
    return 0;
}

int fetion_robot_send_msg(User *user, char *sipuri, char out_message[])
{
    /* 发送消息 */
    Conversation *conv = fetion_conversation_new(user, sipuri, NULL);
    if(fetion_conversation_send_sms(conv, out_message) == -1) {
        debug_info("Error! reply to %s failed!", sipuri);
        return 1;
    } else {
        debug_info("Me --> %s : %s", sipuri, out_message);
    }
    memset(out_message, 0, BUFLEN);
    return 0;
}

int fetion_robot_add_buddy(User *user, char *sipmsg, char out_message[])
{
    char *userid;
    char *sipuri;
    char *desc;
    int   phrase;
    //int ret;
    /* 解析添加好友请求 */
    fetion_sip_parse_addbuddyapplication(sipmsg,
            &sipuri , &userid , &desc , &phrase);
    /* 同意添加好友 */
    /*
       char *res = NULL;
       int rtv = parse_add_buddy_verification(user , res);
       free(res);
       if(rtv != 0){
       debug_info("Add buddy(%s) falied , need verification, but parse error" , no);
       return NULL;
       }
       debug_info("Add buddy(%s) falied , need verification" , no);
       return NULL;
     */
    //fetion_contact_add_buddy(user, sipuri, FETION_NO, 0, NULL, "robot", phrase, &ret);
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

int process_notification(User *user, const char* sipmsg, int (**process_function)(User *, Message *, char *), char *out_message)
{
    int   event;
    int   notification_type;
    char  *xml;
    debug_info(sipmsg);
    fetion_sip_parse_notification(sipmsg , &notification_type , &event , &xml);
    debug_info("通知类型：%s", notification_type);
    debug_info(xml);
	switch(notification_type)
	{
		case NOTIFICATION_TYPE_PRESENCE:
			switch(event)
			{
				case NOTIFICATION_EVENT_PRESENCECHANGED :
					//process_presence(user);
                    //process_function[1];
					break;
				default:
					break;
			}
		case NOTIFICATION_TYPE_CONVERSATION :
			if(event == NOTIFICATION_EVENT_USERLEFT){
				//fx_main_process_user_left(fxmain , sipmsg);
				break;
			}
			break;
		case NOTIFICATION_TYPE_REGISTRATION :
			if(event == NOTIFICATION_EVENT_DEREGISTRATION){
				//fx_main_process_deregistration(fxmain);
				break;
			}
			break;
		case NOTIFICATION_TYPE_SYNCUSERINFO :
			if(event == NOTIFICATION_EVENT_SYNCUSERINFO){
				//fx_main_process_syncuserinfo(fxmain , xml);
				break;
			}
			break;
		case NOTIFICATION_TYPE_CONTACT :
			if(event == NOTIFICATION_EVENT_ADDBUDDYAPPLICATION){
                fetion_robot_add_buddy(user, sipmsg, out_message);
				break;
			}
			break;
		case NOTIFICATION_TYPE_PGGROUP :
			if(event == NOTIFICATION_EVENT_PGGETGROUPINFO){
                debug_info("pggroup notification");
                PGGroup *pggroup = user->pggroup;
                pg_group_parse_info(pggroup , sipmsg);
				break;
			}
			if(event == NOTIFICATION_EVENT_PRESENCECHANGED){
				//fx_main_process_pgpresencechanged(fxmain , sipmsg);
				break;
			}
			break;
		default:
			break;
	}
	free(xml);
    return 0;
}

int fetion_robot_daemon(int argc, char *argv[], User **user_p, int (**process_function)(User *, Message *, char *))
{
    User *user;
    int   password_inputed = 0;
    int   mobileno_inputed = 0;

	char mobileno[BUFLEN];
	char password[BUFLEN];
    char out_message[BUFLEN];
	FetionSip  *sip;
    SipMsg *msg;
    SipMsg *pos;
	Message *sip_msg;
	int error;
    int type;
	int ch;

	memset(mobileno, 0, sizeof(mobileno));
	memset(password, 0, sizeof(password));
    memset(out_message, 0, sizeof(out_message));
    memset(Command, 0, sizeof(Command));

	while((ch = getopt(argc, argv, "f:p:c:")) != -1) {
		switch(ch) {
			case 'f':
				mobileno_inputed = 1;
				strncpy(mobileno, optarg, sizeof(mobileno) - 1);
				break;
			case 'p':
				password_inputed = 1;
				strncpy(password, optarg, sizeof(password) - 1);
				break;
			case 'c':
                /* 调用外部命令来处理消息*/
				strncpy(Command, optarg, sizeof(Command) - 1);
                process_function[0] = execute_command_with_args;
				break;
			default:
				break;
		}
	}

	if(!mobileno_inputed || !password_inputed) {
		usage(argv[0]);
		return 1;
	}

	if(fx_login(user_p, mobileno, password)) {
		return 1;
    }
    user = *user_p;
 
    /*
    //pg_group_get_list(user);
    Group *group_pos;
    PGGroup *pggroup_pos;
    foreach_grouplist(user->groupList, group_pos) {
        debug_info(group_pos->groupname);
    }
    if (user->pggroup != NULL) {
        foreach_pg_group(user->pggroup, pggroup_pos) {
            debug_info(pggroup_pos->pguri);
            debug_info(pggroup_pos->name);
        }
    }
    */

	fetion_user_set_state(user, P_ONLINE);
    sip = user->sip;
    /* 后台守候 */
    int sleep_time=2000000;
    while(1) {
        /* keep alive */
        fetion_user_keep_alive(user);
        sleep_time = 2000000;
        /* get receive */
        msg = fetion_sip_listen(sip, &error);
        pos = msg;
        while(pos){
            type = fetion_sip_get_type(pos->message);
            switch(type){
                case SIP_NOTIFICATION :
                    /* 处理添加好友请求 */
                    process_notification(user, pos->message, process_function, out_message);
                    break;
                case SIP_MESSAGE:
                    fetion_sip_parse_message(sip , pos->message, &sip_msg);
                    /* 忽略移动的消息  */
                    if(strcmp(sip_msg->sipuri,"sip:10000@fetion.com.cn;p=100") == 0){
                        break;
                    }
                    debug_info("%s: %s", sip_msg->sipuri, sip_msg->message);
                    process_function[0](user, sip_msg, out_message);
                    fetion_robot_send_msg(user, sip_msg->sipuri, out_message);

                    break;
                case SIP_INVITATION:
                    debug_info("invitation");
                    /* 初次会话 */
                    sleep_time = 1;
                    debug_info(pos->message);
                    break;
                case SIP_INCOMING :
                    debug_info("incoming");
                    debug_info(pos->message);
                    break;
                case SIP_SIPC_4_0:
                    /* 这个一直有 */
                    break;
                default:
                    debug_info("unknown type");
                    debug_info(pos->message);
                    break;
            }
            pos = pos->next;
        }
        if(msg)
            fetion_sip_message_free(msg);
        usleep(sleep_time);
    }
 
	fetion_user_free(user);
	return 0;
 
}

void usage(char *command)
{
	fprintf(stdout, "Usage:%s -f mobileno -p password [-c command]\n", command);
}
