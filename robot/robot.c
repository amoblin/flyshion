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
 
#include <openfetion.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define BUFLEN 1024
 
int   password_inputed = 0;
int   mobileno_inputed = 0;
int   command_inputed = 0;
User *user;
pthread_t th;
 
static void usage(char *argv[]);
 
int fx_login(const char *mobileno, const char *password)
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
 	user = fetion_user_new(mobileno, password);
	/* construct a config object */
	config = fetion_config_new();
	/* attach config to user */
	fetion_user_set_config(user, config);
 
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
 
	if(USER_AUTH_ERROR(user) || USER_AUTH_NEED_CONFIRM(user)) {
		debug_error("login failed");
		return 1;
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

/* 内部函数绑定的话，修改这里 */
int auto_reply(Message *sip_msg, char out_message[], char command[])
{
    char command_str[BUFLEN];
    memset(command_str, 0, BUFLEN);
    strncpy(command_str, command, BUFLEN);
    strcat(command_str, "\"");
    strcat(command_str, sip_msg->message);
    strcat(command_str, "\"");
    debug_info(command_str);
    FILE *pp;
    debug_info(command);
    if( (pp = popen(command_str, "r")) == NULL) {
        debug_info("Error! popen() failed!");
        return 1;
    }
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
};
 
int main(int argc, char *argv[])
{
	int ch;
	char mobileno[BUFLEN];
	char password[BUFLEN];
    char command[BUFLEN];
    char out_message[BUFLEN];
	FetionSip  *sip;
    SipMsg *msg;
    SipMsg *pos;
	Message *sip_msg;
	int error;
    int type;
 
	memset(mobileno, 0, sizeof(mobileno));
	memset(password, 0, sizeof(password));
    memset(command, 0, sizeof(command));
    memset(out_message, 0, sizeof(out_message));
 
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
				command_inputed = 1;
				strncpy(command, optarg, sizeof(command) - 1);
				break;
			default:
				break;
		}
	}
 
	if(!mobileno_inputed || !password_inputed || !command_inputed) {
		usage(argv);
		return 1;
	}
 
	if(fx_login(mobileno, password))
		return 1;

    /*
    char receiveno[BUFLEN]="13811495947";
    char message[BUFLEN]="好，熄灯我就睡了。";
    send_message(mobileno, receiveno, message);
    */

    sip = user->sip;
    /* 后台守候 */
    while(1) {
        /* keep alive */
        fetion_sip_keep_alive(sip);
        /* get receive */
        msg = fetion_sip_listen(sip, &error);
        pos = msg;
        while(pos){
            type = fetion_sip_get_type(pos->message);
            switch(type){
                case SIP_NOTIFICATION :
                    debug_info("notification");
                    debug_info(pos->message);
                    break;
                case SIP_MESSAGE:
                    fetion_sip_parse_message(sip , pos->message, &sip_msg);
                    //debug_info("%s: %s", sip_msg->sipuri, sip_msg->message);
                    auto_reply(sip_msg, out_message, command);
                    break;
                case SIP_INVITATION:
                    debug_info("invitation");
                    debug_info(pos->message);
                    break;
                case SIP_INCOMING :
                    debug_info("incoming");
                    debug_info(pos->message);
                    break;
                case SIP_SIPC_4_0:
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
        sleep(2);
    }
 
	fetion_user_free(user);
	return 0;
 
}
 
static void usage(char *argv[])
{
	fprintf(stderr, "Usage:%s -f mobileno -p password -c command\n", argv[0]);
}
