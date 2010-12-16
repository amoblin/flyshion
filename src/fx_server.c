#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <openfetion.h>
#include <fx_server.h>

static int process_send_message(FxMain *fxmain, struct fifo_mesg mesg, const char *body);
static int server_send_message_to_friend(FxMain *fxmain, const char *no, const char *body, char *errbuf);

int init_server(FxMain *fxmain)
{
	int   fifo;
	int   idlefifo;
	User *user = fxmain->user;

	char server_fifo[128];
	snprintf(server_fifo, sizeof(server_fifo) - 1, OPENFETION_FIFO_FILE, user->mobileno);

	if(mkfifo(server_fifo, FIFO_FILE_MODE) == -1
			&& errno != EEXIST) {
		fprintf(stderr, "create fifo %s:%s\n", server_fifo, strerror(errno));
		return -1;
	}

	if((fifo = open(server_fifo, O_RDONLY, 0)) == -1) {
		fprintf(stderr, "open fifo %s:%s\n", server_fifo, strerror(errno));
		return -1;
	}

	if((idlefifo = open(server_fifo, O_WRONLY, 0)) == -1) {
		fprintf(stderr, "open fifo %s:%s\n", server_fifo, strerror(errno));
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
			fprintf(stderr, "fifo message error\n");
			continue;
		}

		memset(buf, 0, sizeof(buf));
		if((n = io_read(fifo, buf, mesg.length)) == -1) {
			continue;
		}

		switch(mesg.type) {
			case CLI_SEND_MESSAGE:
				process_send_message(fxmain, mesg, buf);
				break;
			default:
				break;
		}

	}

	return 0;
}

static int process_send_message(FxMain *fxmain, struct fifo_mesg mesg, const char *body)
{
	xmlDocPtr     doc;
	xmlNodePtr    node;
	xmlChar      *res;
	char          msg_no[BUFLEN];
	char          msg_body[BUFLEN];
	User         *user = fxmain->user;
	Conversation *conv;

	char          client_fifo_file[BUFLEN];
	int           client_fifo;
	char          client_buf[BUFLEN];
	char          errbuf[BUFLEN];
	struct fifo_resp *resp;

	int           n;

	sprintf(client_fifo_file, OPENFETION_CLIENT_FIFO, mesg.pid);

	if((client_fifo = open(client_fifo_file, O_WRONLY, 0)) == -1) {
		debug_error("open %s:%s\n", client_fifo_file, strerror(errno));
		return -1;
	}


	if(!(doc = xmlParseMemory(body, mesg.length))) {
		debug_error("parse cli send message request fail\n");
		return -1;
	}

	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	if(strcmp((char*)node->name, "c") != 0) {
		debug_error("parse cli send message request fail\n");
		return -1;
	}

	if(!xmlHasProp(node, BAD_CAST "no")) {
		debug_error("parse cli send message request fail\n");
		return -1;
	}

	if(!xmlHasProp(node, BAD_CAST "bd")) {
		debug_error("parse cli send message request fail\n");
		return -1;
	}

	res = xmlGetProp(node, BAD_CAST "no");
	sprintf(msg_no, "%s", (char*)res);
	xmlFree(res);

	res = xmlGetProp(node, BAD_CAST "bd");
	sprintf(msg_body, "%s", (char*)res);
	xmlFree(res);

	resp = (struct fifo_resp*)client_buf;
	if(strcmp(msg_no, user->mobileno) == 0) {
		/* send it to myself  */
		conv = fetion_conversation_new(user, NULL, NULL);
		fetion_conversation_send_sms_to_myself(conv, msg_body);
		free(conv);

	}else{
		memset(errbuf, 0, sizeof(errbuf));
		if(server_send_message_to_friend(fxmain, msg_no, msg_body, errbuf) == -1) {
			resp->code = 400;
			resp->length = strlen(errbuf);
			memcpy(client_buf + RESP_HEAD_SIZE, errbuf, strlen(errbuf));
		}else{
			snprintf(errbuf, sizeof(errbuf) - 1, "send sms success to %s", msg_no);
			resp->code = 200;
			resp->length = strlen(errbuf);
			memcpy(client_buf + RESP_HEAD_SIZE, errbuf, strlen(errbuf));
		}

		n = write(client_fifo, client_buf, strlen(errbuf) + RESP_HEAD_SIZE);
		if(n != strlen(errbuf) + RESP_HEAD_SIZE) {
			debug_error("send back response failed\n");
			return -1;
		}

	}

	xmlFreeDoc(doc);

	return 0;
}

static int server_send_message_to_friend(FxMain *fxmain, const char *no, const char *body, char *errbuf)
{
	FetionSip    *sip;
	ThreadArgs   *args = NULL;
	FxList       *fxlist = NULL;
	Contact      *contact;
	User         *user = fxmain->user;
	Conversation *conv;
	
	contact = fetion_contact_list_find_by_mobileno(user->contactList, no);
	if(!contact) {
		sprintf(errbuf, "couldn't find %s in buddy list", no);
		debug_error(errbuf);
		return -1;
	}
	conv = fetion_conversation_new(user, contact->sipuri, NULL);
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

				fetion_conversation_send_sms(conv , body);
			}else{
				sprintf(errbuf, "user with mobile number %s is now online, but couldn't invite it", no);
				debug_error(errbuf);
				return -1;
			}
		}else{
			conv->currentSip = sip;
			if(fetion_conversation_send_sms_with_reply(conv, body) == -1) {
				sprintf(errbuf, "send sms failed\n");
				debug_error(errbuf);
				return -1;
			}
		}
	}else{
		if(fetion_conversation_send_sms_with_reply(conv, body) == -1) {
			sprintf(errbuf, "send sms failed\n");
			debug_error(errbuf);
			return -1;
		}
	}
	return 0;
}
