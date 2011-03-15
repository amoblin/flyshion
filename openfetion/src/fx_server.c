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
 *   51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA.            *
 *                                                                         *
 *   OpenSSL linking exception                                             *
 *   --------------------------                                            *
 *   If you modify this Program, or any covered work, by linking or        *
 *   combining it with the OpenSSL project's "OpenSSL" library (or a       *
 *   modified version of that library), containing parts covered by        *
 *   the terms of OpenSSL/SSLeay license, the licensors of this            *
 *   Program grant you additional permission to convey the resulting       *
 *   work. Corresponding Source for a non-source form of such a            *
 *   combination shall include the source code for the parts of the        *
 *   OpenSSL library used as well as that of the covered work.             *
 ***************************************************************************/

#include <errno.h>
#include <openfetion.h>
#include <fx_server.h>

int   idlefifo;

static int process_request(FxMain *fxmain, struct fifo_mesg mesg, const char *body);
static int process_send_message(FxMain *fxmain, int client_fifo, xmlNodePtr node);
static int process_get_contact_info(FxMain *fxmain, int client_fifo, xmlNodePtr node);
static int server_send_message_to_friend(FxMain *fxmain, const char *no, const char *body, int send_to_phone, char *errbuf);
static int send_error(int client_fifo, const char *msg);
static int send_success(int client_fifo, const char *msg);

int init_server(FxMain *fxmain)
{
	int   fifo;
	User *user = fxmain->user;

	char server_fifo[128];
	snprintf(server_fifo, sizeof(server_fifo) - 1, OPENFETION_FIFO_FILE, user->mobileno);

	if(mkfifo(server_fifo, FIFO_FILE_MODE) == -1
			&& errno != EEXIST) {
		debug_error("create fifo %s:%s\n", server_fifo, strerror(errno));
		return -1;
	}

	if((fifo = open(server_fifo, O_RDONLY, 0)) == -1) {
		debug_error("open fifo %s:%s\n", server_fifo, strerror(errno));
		return -1;
	}

	if((idlefifo = open(server_fifo, O_WRONLY, 0)) == -1) {
		debug_error("open fifo %s:%s\n", server_fifo, strerror(errno));
		close(fifo);
		return -1;
	}

	return fifo;
}

int io_read(int fifo, void *buf, int len)
{
	int n, ret;
	if(len == 0) return 0;
	for(n=0;n!=len && (ret = read(fifo, buf+n, len-n)) != -1 && ret;n += ret);
	return (n!=len)? -1:n;
}

int start_server(FxMain *fxmain, int fifo)
{
	char buf[BUFLEN];
	struct fifo_mesg mesg;
	int  n;

	for(;;) {
		memset(buf, 0, sizeof(buf));
		n = read(fifo, &mesg, FIFO_HEAD_SIZE);
		if(n != FIFO_HEAD_SIZE) {
			debug_error("fifo message error\n");
			continue;
		}

		memset(buf, 0, sizeof(buf));
		if((n = io_read(fifo, buf, mesg.length)) == -1) {
			continue;
		}

		process_request(fxmain, mesg, buf);

	}

	return 0;
}

static int process_request(FxMain *fxmain, struct fifo_mesg mesg, const char *body)
{
	xmlDocPtr     doc;
	xmlNodePtr    node;
	char          client_fifo_file[BUFLEN];
	int           client_fifo;

	sprintf(client_fifo_file, OPENFETION_CLIENT_FIFO, mesg.pid);

	if((client_fifo = open(client_fifo_file, O_WRONLY, 0)) == -1) {
		debug_error("open %s:%s", client_fifo_file, strerror(errno));
		return -1;
	}


	if(!(doc = xmlParseMemory(body, mesg.length))) {
		debug_error("parse cli send message request failed");
		send_error(client_fifo, "unknown request message");
		close(client_fifo);
		return -1;
	}

	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;

	if(strcmp((char*)node->name, "m") == 0) {
		switch(mesg.type) {
			case CLI_SEND_MESSAGE:
				process_send_message(fxmain, client_fifo, node);
				break;
			case CLI_GET_INFORMATION:
				process_get_contact_info(fxmain, client_fifo, node);
				break;
			default:
				debug_error("unknown request type");
				send_error(client_fifo, "unknown request type");
				xmlFreeDoc(doc);
				return -1;
				break;
		}
	}else{
		debug_error("parse cli send message request failed");
		send_error(client_fifo, "unknown request message");
		xmlFreeDoc(doc);
		return -1;
	}

	xmlFreeDoc(doc);
	return 0;

}

static int process_send_message(FxMain *fxmain, int client_fifo, xmlNodePtr node)
{
	xmlChar      *res;
	char          msg_no[BUFLEN];
	char          msg_body[BUFLEN];
	User         *user = fxmain->user;
	Conversation *conv;

	char          errbuf[BUFLEN];
	int           send_to_phone = 0;

	if(!xmlHasProp(node, BAD_CAST "no")) {
		debug_error("parse cli send message request failed");
		send_error(client_fifo, "unknown request message");
		return -1;
	}

	if(!xmlHasProp(node, BAD_CAST "bd")) {
		debug_error("parse cli send message request failed");
		send_error(client_fifo, "unknown request message");
		return -1;
	}

	if(xmlHasProp(node, BAD_CAST "p")) {
		res = xmlGetProp(node, BAD_CAST "p");
		send_to_phone = atoi((char*)res);
		xmlFree(res);
	}

	res = xmlGetProp(node, BAD_CAST "no");
	sprintf(msg_no, "%s", (char*)res);
	xmlFree(res);

	res = xmlGetProp(node, BAD_CAST "bd");
	sprintf(msg_body, "%s", (char*)res);
	xmlFree(res);

	if(strcmp(msg_no, user->mobileno) == 0) {
		/* send it to myself  */
		conv = fetion_conversation_new(user, NULL, NULL);
		if(fetion_conversation_send_sms_to_myself_with_reply(conv, msg_body) == -1) {
			snprintf(errbuf, sizeof(errbuf) - 1, "send sms to %s failed", msg_no);
			send_error(client_fifo, errbuf);
			return -1;
		}else{
			snprintf(errbuf, sizeof(errbuf) - 1, "send sms to %s", msg_no);
			send_success(client_fifo, errbuf);
			return 0;
		}
		free(conv);

	}else{
		memset(errbuf, 0, sizeof(errbuf));
		if(server_send_message_to_friend(fxmain, msg_no, msg_body, send_to_phone, errbuf) == -1) {
			send_error(client_fifo, errbuf);
			return -1;
		}else{
			send_success(client_fifo, errbuf);
			return 0;
		}
	}

	return 0;
}

static int process_get_contact_info(FxMain *fxmain, int client_fifo, xmlNodePtr node)
{
	Contact          *contact;
	xmlChar          *res;
	User             *user = fxmain->user;
	char             *city;
	char             *province;
	char             *sid;
	char              body_buf[BUFLEN];
	char              mobileno[BUFLEN];

	if(!xmlHasProp(node, BAD_CAST "no")) {
		send_error(client_fifo, "unknown request message");
		return -1;
	}

	res = xmlGetProp(node, BAD_CAST "no");
	strcpy(mobileno, (char*)res);
	xmlFree(res);

	contact = fetion_contact_get_contact_info_by_no(user, mobileno, MOBILE_NO);


	if(contact) {
		province = fetion_config_get_province_name(contact->province);
		city = fetion_config_get_city_name(contact->province, contact->city);
		sid = fetion_sip_get_sid_by_sipuri(contact->sipuri);
		snprintf(body_buf, sizeof(body_buf) - 1, "get information successfully\n"
				"Mobile No:\t%s\n"
				"Sex:\t\t%s\n"
				"NickName:\t%s\n"
				"Impression:\t%s\n"
				"Birthday:\t%s\n"
				"City:\t\t%s\n"
				"Province\t%s",
				contact->mobileno,
				contact->gender == 1 ? _("Male") : ( contact->gender == 2 ? _("Female") : _("Secrecy")),
				contact->nickname, contact->impression, contact->birthday,
				city, province);
		free(city);
		free(province);
		free(sid);
		
		if(send_success(client_fifo, body_buf) == -1) {
			debug_info("send response successfully\n%s", body_buf);
			return -1;
		}else{
			return 0;
		}

	}else{
		snprintf(body_buf, sizeof(body_buf) - 1, "get contact information of %s failed", mobileno);
		send_error(client_fifo, body_buf);
		return -1;
	}

	return 0;
}

static int server_send_message_to_friend(FxMain *fxmain, const char *no, const char *body, int send_to_phone, char *resbuf)
{
	FetionSip    *sip;
	ThreadArgs   *args = NULL;
	FxList       *fxlist = NULL;
	Contact      *contact;
	User         *user = fxmain->user;
	Conversation *conv;
	
	contact = fetion_contact_list_find_by_mobileno(user->contactList, no);
	if(!contact) {
		sprintf(resbuf, "couldn't find %s in buddy list", no);
		debug_error(resbuf);
		return -1;
	}
	conv = fetion_conversation_new(user, contact->sipuri, NULL);

	/* send message directly to mobile phone */
	if(send_to_phone) {
		int daycount = 0, monthcount = 0;
		if(fetion_conversation_send_sms_to_phone_with_reply(conv, body, &daycount, &monthcount) == -1) {
			sprintf(resbuf, "send message directly to mobile phone with number %s failed", no);
			debug_error(resbuf);
			return -1;
		}else{
			sprintf(resbuf, "send message to %s\n%d messages sent today\n%d messages sent this month\n"
					"%d messages left for today\n%d messages left for this month",
					no, daycount, monthcount, user->smsDayLimit - daycount, user->smsMonthLimit - monthcount);
			debug_info(resbuf);
			return 0;
		}
	}

	if(contact->state == P_ONLINE) {

		sip = fx_list_find_sip_by_sipuri(fxmain->slist, contact->sipuri);
		if(!sip) {
			if(fetion_conversation_invite_friend(conv) > 0){

				fx_conn_append(conv->currentSip->tcp);

				args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
				args->fxmain = fxmain;
				args->sip = conv->currentSip;

				fxlist = fx_list_new(conv->currentSip);
				fx_list_append(fxmain->slist , fxlist);

				g_thread_create(fx_main_listen_thread_func , args , FALSE , NULL);

				if(fetion_conversation_send_sms_with_reply(conv , body) == -1) {
					sprintf(resbuf, "send sms failed");
					debug_error(resbuf);
					return -1;
				}else{
					sprintf(resbuf, "send sms successfully");
					debug_info(resbuf);
				}
			}else{
				sprintf(resbuf, "user with mobile number %s is now online, but couldn't invite it", no);
				debug_error(resbuf);
				return -1;
			}
		}else{
			conv->currentSip = sip;
			if(fetion_conversation_send_sms_with_reply(conv, body) == -1) {
				sprintf(resbuf, "send sms failed");
				debug_error(resbuf);
				return -1;
			}else{
				sprintf(resbuf, "successfully send message to %s", no);
				debug_info(resbuf);
				return 0;
			}
		}
	}else{
		if(fetion_conversation_send_sms_with_reply(conv, body) == -1) {
			sprintf(resbuf, "send sms failed");
			debug_error(resbuf);
			return -1;
		}else{
			sprintf(resbuf, "successfully send message to %s", no);
			debug_info(resbuf);
			return 0;
		}
	}
	return 0;
}

static int send_error(int client_fifo, const char *msg)
{
	struct fifo_resp *resp;
	char              client_buf[BUFLEN];
	unsigned int      n;

	resp = (struct fifo_resp*)client_buf;
	resp->code = CLI_EXEC_FAIL;
	resp->length = strlen(msg);
	memcpy(client_buf + RESP_HEAD_SIZE, msg, strlen(msg));

	n = write(client_fifo, client_buf, resp->length + RESP_HEAD_SIZE);
	if(n != resp->length + RESP_HEAD_SIZE) {
		debug_error("send back response failed");
		return -1;
	}
	return 0;
}

static int send_success(int client_fifo, const char *msg)
{
	struct fifo_resp *resp;
	char              client_buf[BUFLEN];
	unsigned int      n;

	resp = (struct fifo_resp*)client_buf;
	resp->code = CLI_EXEC_OK;
	resp->length = strlen(msg);
	memcpy(client_buf + RESP_HEAD_SIZE, msg, strlen(msg));

	n = write(client_fifo, client_buf, resp->length + RESP_HEAD_SIZE);
	if(n != resp->length + RESP_HEAD_SIZE) {
		debug_error("send back response failed");
		return -1;
	}
	return 0;
}
