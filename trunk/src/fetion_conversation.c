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

#include "fetion_include.h"

Conversation* fetion_conversation_new(User* user , const char* sipuri , FetionSip* sip)
{
	Conversation* conversation = (Conversation*)malloc(sizeof(Conversation));
	memset(conversation , 0 , sizeof(Conversation));
	conversation->currentUser = user;
	if(sipuri != NULL)
		conversation->currentContact = fetion_contact_list_find_by_sipuri(user->contactList , sipuri);
	else
		conversation->currentContact = NULL;
	if(sipuri != NULL && conversation->currentContact == NULL)
		return NULL;
	conversation->currentSip = sip;
	return conversation;
}

void fetion_conversation_send_sms(Conversation* conversation , const char* msg)
{
	FetionSip* sip = conversation->currentSip == NULL ? conversation->currentUser->sip : conversation->currentSip;
	SipHeader *toheader , *cheader , *kheader , *nheader;
	char* res;
	fetion_sip_set_type(sip , SIP_MESSAGE);
	nheader  = fetion_sip_event_header_new(SIP_EVENT_CATMESSAGE);
	toheader = fetion_sip_header_new("T" , conversation->currentContact->sipuri);
	cheader  = fetion_sip_header_new("C" , "text/plain");
	kheader  = fetion_sip_header_new("K" , "SaveHistory");
	fetion_sip_add_header(sip , toheader);
	fetion_sip_add_header(sip , cheader);
	fetion_sip_add_header(sip , kheader);
	fetion_sip_add_header(sip , nheader);
	res = fetion_sip_to_string(sip , msg);
	debug_info("Sent a message to %s" , conversation->currentContact->sipuri);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
}
void fetion_conversation_send_sms_to_myself(Conversation* conversation , const char* message)
{
	SipHeader *toheader = NULL;
	SipHeader *eheader = NULL;
	char* res = NULL;
	FetionSip* sip = conversation->currentUser->sip;

	fetion_sip_set_type(sip , SIP_MESSAGE);
	toheader = fetion_sip_header_new("T" , conversation->currentUser->sipuri);
	eheader  = fetion_sip_event_header_new(SIP_EVENT_SENDCATMESSAGE);
	fetion_sip_add_header(sip , toheader);
	fetion_sip_add_header(sip , eheader);
	res = fetion_sip_to_string(sip , message);
	debug_info("Sent a message to myself" , conversation->currentContact->sipuri);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
	res = fetion_sip_get_response(sip);
	free(res);
}
void fetion_conversation_send_sms_to_phone(Conversation* conversation , const char* message)
{
	
	SipHeader *toheader = NULL;
	SipHeader *eheader = NULL;
	char* res = NULL;
	FetionSip* sip = conversation->currentUser->sip;
	char* sipuri = conversation->currentContact->sipuri;
	fetion_sip_set_type(sip , SIP_MESSAGE);
	toheader = fetion_sip_header_new("T" , sipuri);
	eheader  = fetion_sip_event_header_new(SIP_EVENT_SENDCATMESSAGE);
	fetion_sip_add_header(sip , toheader);
	fetion_sip_add_header(sip , eheader);
	res = fetion_sip_to_string(sip , message);
	debug_info("Sent a message to (%s)`s mobile phone" , sipuri);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
}
int fetion_conversation_send_sms_to_phone_with_reply(Conversation* conversation , const char* message , int* daycount , int* monthcount)
{
	
	SipHeader *toheader , *eheader;
	char* res;
	char* xml;
	FetionSip* sip = conversation->currentUser->sip;
	char* sipuri = conversation->currentContact->sipuri;
	fetion_sip_set_type(sip , SIP_MESSAGE);
	toheader = fetion_sip_header_new("T" , sipuri);
	eheader  = fetion_sip_event_header_new(SIP_EVENT_SENDCATMESSAGE);
	fetion_sip_add_header(sip , toheader);
	fetion_sip_add_header(sip , eheader);
	res = fetion_sip_to_string(sip , message);
	debug_info("Sent a message to (%s)`s mobile phone" , sipuri);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
	res = fetion_sip_get_response(sip);
	if(fetion_sip_get_code(res) == 280)
	{
		xml = strstr(res , "\r\n\r\n") + 4;
		fetion_conversation_parse_send_sms(xml , daycount , monthcount);
		free(res);
		return 1;
	}
	else
	{
		free(res);
		debug_error("Send a message to (%s)`s mobile phone failed");
		return -1;
	}
}
int fetion_conversation_invite_friend(Conversation* conversation)
{
	FetionSip* sip = conversation->currentUser->sip;
	char *res , *ip , *credential , auth[256] , *body;
	int port;
	FetionConnection* conn;
	SipHeader *eheader , *theader , *mheader , *nheader , *aheader;
	/*start chat*/
	fetion_sip_set_type(sip , SIP_SERVICE);
	eheader = fetion_sip_event_header_new(SIP_EVENT_STARTCHAT);
	fetion_sip_add_header(sip , eheader);
	res = fetion_sip_to_string(sip , NULL);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res); res = NULL;
	res = fetion_sip_get_response(sip);
	bzero(auth , sizeof(auth));
	fetion_sip_get_attr(res , "A" , auth);
	fetion_sip_get_auth_attr(auth , &ip , &port , &credential);
	free(res); res = NULL;
	conn = tcp_connection_new();
	tcp_connection_connect(conn , ip , port);
	/*clone sip*/
	conversation->currentSip = fetion_sip_clone(conversation->currentUser->sip);
	bzero(conversation->currentSip->sipuri , sizeof(conversation->currentSip->sipuri));
	strcpy(conversation->currentSip->sipuri , conversation->currentContact->sipuri);
	fetion_sip_set_connection(conversation->currentSip , conn);
	free(ip); ip = NULL;
	/*register*/
	sip = conversation->currentSip;
	fetion_sip_set_type(sip , SIP_REGISTER);
	aheader = fetion_sip_credential_header_new(credential);
	theader = fetion_sip_header_new("K" , "text/html-fragment");
	mheader = fetion_sip_header_new("K" , "multiparty");
	nheader = fetion_sip_header_new("K" , "nudge");
	fetion_sip_add_header(sip , aheader);
	fetion_sip_add_header(sip , theader);
	fetion_sip_add_header(sip , mheader);
	fetion_sip_add_header(sip , nheader);
	res = fetion_sip_to_string(sip , NULL);
	tcp_connection_send(conn , res , strlen(res));
	free(res);res = NULL;
	free(credential); credential = NULL;
	res = fetion_sip_get_response(sip);
	free(res); res = NULL;
	/*invite buddy*/
	fetion_sip_set_type(sip , SIP_SERVICE);
	eheader = fetion_sip_event_header_new(SIP_EVENT_INVITEBUDDY);
	fetion_sip_add_header(sip , eheader);
	body = generate_invite_friend_body(conversation->currentContact->sipuri);
	res = fetion_sip_to_string(sip , body);	
	free(body); body = NULL;
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res); res = NULL;
	res = fetion_sip_get_response(sip);
	if(fetion_sip_get_code(res) == 200)
	{
		free(res);
		res = (char*)malloc(2048);
		memset(res , 0 , 2048);
		tcp_connection_recv(sip->tcp , res , 2048);
		//res = fetion_sip_get_response(sip);
		free(res);
		return 1;
	}
	else
	{
		free(res);
		return -1;
	}
}
int fetion_conversation_send_nudge(Conversation* conversation)
{
	SipHeader *toheader = NULL;
	char* res = NULL;
	char* body = NULL;
	FetionSip* sip = conversation->currentSip;
	if(sip == NULL)
	{
		debug_error("Did not start a chat chanel , can not send a nudge");
		return -1;
	}
	char* sipuri = conversation->currentContact->sipuri;
	fetion_sip_set_type(sip , SIP_INCOMING);
	toheader = fetion_sip_header_new("T" , sipuri);
	fetion_sip_add_header(sip , toheader);
	body = generate_send_nudge_body();
	res = fetion_sip_to_string(sip , body);
	free(body);
	debug_info("Sent a nudge to (%s)" , sipuri);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
/*	res = fetion_sip_get_response(sip);
	if(fetion_sip_get_code(res) == 280)
	{
		free(res);
		return 1;
	}
	else
	{
		printf("%s\n" , res);
		free(res);
		debug_error("Send nuge failed");
		return -1;
	}*/
	return 1;

}
char* generate_invite_friend_body(const char* sipuri)
{
	xmlChar *buf;
	char *pos , *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	char body[] = "<args></args>";
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);
	xmlNewProp(node , BAD_CAST "uri" , BAD_CAST sipuri);
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	pos = strstr((char*)buf , "?>") + 2;
	res = (char*)malloc(strlen(pos));
	memset(res , 0 , strlen(pos));
	strcpy(res , pos);
	xmlFree(buf);
	return res;
}
char* generate_send_nudge_body()
{
	xmlChar *buf;
	char *pos , *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	char body[] = "<is-composing></is-composing>";
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "state" , NULL);
	xmlNodeSetContent(node , BAD_CAST "nudge");
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	pos = strstr((char*)buf , "?>") + 2;
	res = (char*)malloc(strlen(pos));
	memset(res , 0 , strlen(pos));
	strcpy(res , pos);
	xmlFree(buf);
	return res;

}
void fetion_conversation_parse_send_sms(const char* xml , int* daycount , int* mountcount)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar* res;
	doc = xmlParseMemory(xml , strlen(xml));
	node = xmlDocGetRootElement(doc);
	node = xml_goto_node(node , "frequency");
	res = xmlGetProp(node , BAD_CAST "day-count");
	*daycount = atoi((char*)res);
	xmlFree(res);
	res = xmlGetProp(node , BAD_CAST "month-count");
	*mountcount = atoi((char*)res);
	xmlFree(res);
	xmlFreeDoc(doc);
}
