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

char buffer[1025];
int callid = 1;

FetionSip* fetion_sip_new(FetionConnection* tcp , const char* sid)
{
	FetionSip* sip = (FetionSip*)malloc(sizeof(FetionSip));
	memset(sip , 0 , sizeof(FetionSip));
	strcpy(sip->from , sid);
	sip->sequence = 2;
	sip->tcp = tcp;
	sip->header = NULL;
	return sip;
}
FetionSip* fetion_sip_clone(FetionSip* sip)
{
	FetionSip* res = (FetionSip*)malloc(sizeof(FetionSip));
	memset(res , 0 , sizeof(FetionSip));
	memcpy(res , sip , sizeof(FetionSip));
	sip->header = NULL;
	return res;
}
SipHeader* fetion_sip_header_new(const char* name , const char* value)
{
	SipHeader* header = (SipHeader*)malloc(sizeof(SipHeader));
	memset(header , 0 , sizeof(SipHeader));
	strcpy(header->name , name);
	header->value = (char*)malloc(strlen(value) + 2 );
	memset(header->value , 0 , strlen(value) + 2);
	strcpy(header->value , value);
	header->next = NULL;
	return header;
}

void fetion_sip_set_type(FetionSip* sip , SipType type)
{
	sip->type = type;
}
SipHeader* fetion_sip_authentication_header_new(const char* response)
{
	int len;
	char* res;
	char start[] = "Digest response=\"";
	char end[]   = "\",algorithm=\"SHA1-sess-v4\"";
	SipHeader* header;
	
	len = strlen(start) + strlen(end) + strlen(response) + 1;
	res = (char*)malloc(len);
	memset(res , 0 , len);
	sprintf(res , "%s%s%s" , start , response , end);
	header = (SipHeader*)malloc(sizeof(SipHeader));
	memset(header , 0 , sizeof(SipHeader));
	strcpy(header->name , "A");
	header->value = res;
	return header;
}
SipHeader* fetion_sip_ack_header_new(const char* code , const char* algorithm , const char* type , const char* guid)
{
	char ack[512];
	sprintf(ack , "Verify response=\"%s\",algorithm=\"%s\",type=\"%s\",chid=\"%s\""
			 	, code , algorithm , type , guid);
	return fetion_sip_header_new("A" , ack);
}
SipHeader* fetion_sip_event_header_new(int eventType)
{
	char event[48];
	bzero(event , sizeof(event));
	switch(eventType)
	{
		case SIP_EVENT_PRESENCE :
			strcpy(event , "PresenceV4");
			break;
		case SIP_EVENT_SETPRESENCE :
			strcpy(event , "SetPresenceV4");
			break;
		case SIP_EVENT_CATMESSAGE :
			strcpy(event , "CatMsg");
			break;
		case SIP_EVENT_SENDCATMESSAGE :
			strcpy(event , "SendCatSMS");
			break;
		case SIP_EVENT_STARTCHAT :
			strcpy(event , "StartChat");
			break;
		case SIP_EVENT_GETCONTACTINFO :
			strcpy(event , "GetContactInfoV4");
			break;
		case SIP_EVENT_CONVERSATION :
			strcpy(event , "Conversation");
			break;
		case SIP_EVENT_INVITEBUDDY :
			strcpy(event , "InviteBuddy");
			break;
		case SIP_EVENT_CREATEBUDDYLIST :
			strcpy(event , "CreateBuddyList");
			break;
		case SIP_EVENT_DELETEBUDDYLIST :
			strcpy(event , "DeleteBuddyList");
			break;
		case SIP_EVENT_SETCONTACTINFO :
			strcpy(event , "SetContactInfoV4");
			break;
		case SIP_EVENT_SETUSERINFO :
			strcpy(event , "SetUserInfoV4");
			break;
		case SIP_EVENT_SETBUDDYLISTINFO :
			strcpy(event , "SetBuddyListInfo");
			break;
		case SIP_EVENT_DELETEBUDDY :
			strcpy(event , "DeleteBuddyV4");
			break;
		case SIP_EVENT_ADDBUDDY :
			strcpy(event , "AddBuddyV4");
			break;
		case SIP_EVENT_KEEPALIVE :
			strcpy(event , "KeepAlive");
			break;
		case SIP_EVENT_HANDLECONTACTREQUEST :
			strcpy(event , "HandleContactRequestV4");
			break;
		default:
			break;
	}
	return fetion_sip_header_new("N" , event);
}
SipHeader* fetion_sip_credential_header_new(const char* credential)
{
	char value[64];
	bzero(value , sizeof(value));
	sprintf(value , "TICKS auth=\"%s\"" , credential);
	return fetion_sip_header_new("A" , value);
}
void fetion_sip_add_header(FetionSip* sip , SipHeader* header)
{
	SipHeader* pos = sip->header;
	if(pos == NULL)
	{
		sip->header = header;
		return;
	}
	while(pos != NULL)
	{
		if(pos->next == NULL)
		{
			pos->next = header;
			break;
		}
		pos = pos->next;
	}
}

char* fetion_sip_to_string(FetionSip* sip , const char* body)
{
	char *res , *head , buf[40] , type[5];
	SipHeader *pos , *tmp;
	int len = 0;

	pos = sip->header;
	while(pos != NULL)
	{
		len += (strlen(pos->value) + strlen(pos->name) + 5);
		pos = pos->next;
	}
	len += (body == NULL ? 100 : strlen(body) + 100 );
	res = (char*)malloc(len + 1);
	memset(res , 0 , len + 1);
	bzero(type , sizeof(type));
	switch(sip->type)
	{
		case SIP_REGISTER     : strcpy(type , "R");
			break;
		case SIP_SUBSCRIPTION :	strcpy(type , "SUB");
			break;
		case SIP_SERVICE 	  : strcpy(type , "S");
			break;
		case SIP_MESSAGE      : strcpy(type , "M");
			break;
		case SIP_INCOMING	  : strcpy(type , "IN");
			break;
		default:
			break;
	};
	if(strlen(type) == 0)
		return NULL;
	bzero(buf , sizeof(buf));
	sprintf(buf , "%s fetion.com.cn SIP-C/4.0\r\n" , type);
	strcpy(res , buf);
	bzero(buf , sizeof(buf));
	sprintf(buf , "F: %s\r\n" , sip->from);
	strcat(res , buf);
	bzero(buf , sizeof(buf));
	sprintf(buf , "I: %d\r\n" , callid);
	strcat(res , buf);
	bzero(buf , sizeof(buf));
	sprintf(buf , "Q: 2 %s\r\n" , type);
	strcat(res , buf);

	pos = sip->header;
	while(pos != NULL)
	{
		len = strlen(pos->value) + strlen(pos->name) + 5;
		head = (char*)malloc(len);
		memset(head , 0 , len);
		sprintf(head , "%s: %s\r\n" , pos->name , pos->value);
		strcat(res , head);
		tmp = pos;
		pos = pos->next;
		free(head);
		free(tmp->value);
		free(tmp);
	}
	if(body != NULL)
	{
		bzero(buf , sizeof(buf));
		sprintf(buf , "L: %d\r\n\r\n" , strlen(body));
		strcat(res , buf);
		strcat(res , body);
	}
	else
	{
		strcat(res , "\r\n");
	}
	callid ++;
	sip->header = NULL;
	return res;
}
void fetion_sip_free(FetionSip* sip)
{
	debug_info("Free sip struct and close socket");
	if(sip != NULL)
		tcp_connection_free(sip->tcp);
	free(sip);
}
char* fetion_sip_get_sid_by_sipuri(const char* sipuri)
{
	char *res , *pos;
	int n;
	pos = strstr(sipuri , ":") + 1;
	n = strlen(pos) - (strstr(pos , "@") == 0 ? 0 : strlen(strstr(pos , "@"))) ;
	res = (char*)malloc(n + 1);
	memset(res , 0 , n + 1);
	strncpy(res , pos , n);
	return res;
}
int fetion_sip_get_attr(const char* sip , const char* name , char* result)
{
	char m_name[5];
	char* pos;
	int n;
	bzero(m_name , sizeof(m_name));
	sprintf(m_name , "%s: " , name);
	if(strstr(sip , m_name) == NULL)
		return -1;
	pos = strstr(sip , m_name) + strlen(m_name);
	if(strstr(pos , "\r\n") == NULL)
		n = strlen(pos);
	else
		n = strlen(pos) - strlen(strstr(pos , "\r\n"));
	strncpy(result , pos , n);
	return 1;
}

int fetion_sip_get_length(const char* sip)
{
	char res[6];
	char name[] = "L";
	bzero(res , sizeof(res));
	if(fetion_sip_get_attr(sip , name , res) == -1)
		return 0;
	return atoi(res);
}
int fetion_sip_get_code(const char* sip)
{
	char *pos , res[4];
	int n;
	pos = strstr(sip , " ") + 1;
	n = strlen(pos) - strlen(strstr(pos , " "));
	strncpy(res , pos , n);
	return atoi(res);
}
int fetion_sip_get_type(const char* sip)
{
	char res[10];
	int n;
	n = strlen(sip) - strlen(strstr(sip , " "));
	bzero(res , sizeof(res));
	strncpy(res , sip , n);
	if(strcmp(res , "I") == 0 )
		return SIP_INVITATION;
	if(strcmp(res , "M") == 0 )
		return SIP_MESSAGE;
	if(strcmp(res , "BN") == 0)
		return SIP_NOTIFICATION;
	if(strcmp(res , "SIP-C/4.0") == 0)
		return SIP_SIPC_4_0;
	if(strcmp(res , "IN") == 0)
		return SIP_INCOMING;
	return SIP_UNKNOWN;
		
}
char* fetion_sip_get_response(FetionSip* sip)
{
	char *res;
	int len , n , c , c1;
	char buf[2048];
	bzero(buf , sizeof(buf));
	c = tcp_connection_recv(sip->tcp , buf , sizeof(buf) - 2);
	len = fetion_sip_get_length(buf);
	if(strstr(buf , "\r\n\r\n") == NULL)
		return NULL;
	n = strlen(buf) - strlen(strstr(buf , "\r\n\r\n")) + 4;
	len += n;
	res = (char*)malloc(len + 1);
	memset(res , 0 , len + 1);
	if(res == NULL)
	{
		return NULL;
	}
	strcpy(res , buf);
	if(c < len)
		while(1)
		{
			bzero(buf , sizeof(buf));
			c1 = tcp_connection_recv(sip->tcp , buf , sizeof(buf) - 2 );
			strcpy(res + c, buf);
			c += c1;
			if(c == len)
				break;
		}
	return res;
}
void fetion_sip_set_connection(FetionSip* sip , FetionConnection* conn)
{
	sip->tcp = conn;
}
SipMsg* fetion_sip_listen(FetionSip* sip)
{
	char *buf , *pos , *missingStr = NULL , *tmp = NULL;
	char *strBeforeSpt = NULL;
	int strBeforeSptLen = 0;
	int bodyLen , len , msglen , missingLen , rereceiveLen , msgCurrentLen;
	SipMsg *msglist = NULL;
	SipMsg* msg; 
	bzero(buffer , sizeof(buffer));
	sleep(0.1);
	bodyLen = tcp_connection_recv_dont_wait(sip->tcp , buffer , 1024);
	buf = buffer;
	if( bodyLen == 0 || bodyLen == -1 )
		return NULL;
	while(1)
	{	
		msg = (SipMsg*)malloc(sizeof(SipMsg));
		memset(msg , 0 , sizeof(SipMsg));
		msg->next = NULL;
		bodyLen = fetion_sip_get_length(buf);
		pos = strstr(buf , "\r\n\r\n");
		/* ensure the bodyLength is read from 
		 * the first chunk before '\r\n\r\n' */
		if(pos != NULL)
		{
			strBeforeSptLen = strlen(buf) - strlen(pos) + 1;
			strBeforeSpt = (char*)malloc(strBeforeSptLen);
			memset(strBeforeSpt , 0 , strBeforeSptLen);
			strncpy(strBeforeSpt , buf , strBeforeSptLen - 1);
			bodyLen = fetion_sip_get_length(strBeforeSpt);
			free(strBeforeSpt);
		}
		/* if read to the end of buffer , just return */
		if(buf == NULL || strlen(buf) == 0 )
			return msglist;

		/************************************
		 * chunk contains an entire sip head
		 * but has no attribute of length 
		 * such as a reply message :
		 *
		 * SIP-C/4.0 200 OK
		 * I: 3
		 * Q: 2 SUB
		 ***********************************/
		if(bodyLen == 0 && pos != NULL)
		{
			len = strlen(buf) - strlen(pos);
			msg->message = (char*)malloc(len + 5);
			bzero(msg->message , len + 5);
			strncpy(msg->message , buf , len + 4);
			if(msglist == NULL)
				msglist = msg;
			else
				fetion_sip_message_append(msglist , msg);
			buf = pos + 4;
			continue;
		}
		/*************************************
		 * chunk does not contan an entire sip header
		 * and even has no attribute of length , such as:
		 *
		 * BN 572003969 SIP-C/4.0
		 * N: PresenceV4
		 * I: 2\r\n
		 *************************************/
		if(bodyLen == 0 && pos == NULL)
		{
			if(tmp != NULL && strlen(tmp) != 0)
			{
				free(tmp);
				tmp = NULL;
			}
			tmp = (char*)malloc(strlen(buf) + sizeof(buffer));
			memset(tmp , 0 , strlen(buf) + sizeof(buffer));
			strcpy(tmp , buf);
			memset(buffer , 0 , sizeof(buffer));
			tcp_connection_recv(sip->tcp , buffer , 1024);
			strcat(tmp , buffer);
			buf = tmp;
			continue;
		}
		/*****************************************
		 * chunk contanns an entire sip header at least
		 * and has attribute of length
		 * ***************************************/
		if(bodyLen != 0 && pos == NULL)
		{
			if(tmp != NULL && strlen(tmp) != 0)
			{
				free(tmp);
				tmp = NULL;
			}
			tmp = (char*)malloc(strlen(buf) + sizeof(buffer));
			memset(tmp , 0 , strlen(buf) + sizeof(buffer));
			strcpy(tmp , buf);
			memset(buffer , 0 , sizeof(buffer));
			tcp_connection_recv(sip->tcp , buffer , 1024);
			strcat(tmp , buffer);
			buf = tmp;
			continue;
		}
		else
		{
			len = strlen(buf) - strlen(pos);
			msglen = len + 4 + bodyLen;
			msg->message = (char*)malloc(msglen + 1);
			bzero(msg->message , msglen + 1);
			strncpy(msg->message , buf , len + 4);
			pos += 4;
			if(strlen(pos) == bodyLen)
			{
				strcpy(msg->message + len + 4 , pos);
				if(msglist == NULL)
					msglist = msg;
				else
					fetion_sip_message_append(msglist , msg);
				if(tmp != NULL && strlen(tmp) != 0)
				{
					free(tmp);
					tmp = NULL;
				}
				return msglist;
			}
			if(strlen(pos) < bodyLen)
			{
				msgCurrentLen = len + 4;
				strcpy(msg->message + msgCurrentLen , pos);
				msgCurrentLen += strlen(pos);
				missingLen = bodyLen - strlen(pos);

			readmissing:	
				missingStr = (char*)malloc(missingLen + 1);
				bzero(missingStr , missingLen + 1);
				rereceiveLen = tcp_connection_recv(sip->tcp , missingStr , missingLen);
				strcpy(msg->message + msgCurrentLen , missingStr);
				free(missingStr);
				msgCurrentLen += rereceiveLen;
				if(rereceiveLen < missingLen)
				{
					missingLen = missingLen - rereceiveLen;
					goto readmissing;
				}
				if(msglist == NULL)
					msglist = msg;
				else
					fetion_sip_message_append(msglist , msg);
				if(tmp != NULL && strlen(tmp) != 0)
				{
					free(tmp);
					tmp = NULL;
				}
				return msglist;
			}
			else
			{
				strncpy(msg->message + len + 4 , pos , bodyLen);
				if(msglist == NULL)
					msglist = msg;
				else
					fetion_sip_message_append(msglist , msg);
				pos += bodyLen;
				buf = pos;
			}
		}
	}
}
void fetion_sip_keep_alive(FetionSip* sip)
{
	char *res = NULL;

	debug_info("Send a periodical chat keep alive request");

	fetion_sip_set_type(sip , SIP_REGISTER);
	res = fetion_sip_to_string(sip , NULL);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
}
void fetion_sip_message_free(SipMsg* msg)
{
	SipMsg* pos = msg;
	SipMsg* pot;
	while(pos != NULL)
	{
		pot = pos;
		pos = pos->next;
		if(pot != NULL && pot->message != NULL)
		{
			free(pot->message);
		}
		free(pot);
		pot = NULL;
	}
}
void fetion_sip_message_append(SipMsg* msglist , SipMsg* msg)
{
	SipMsg* pos = msglist;
	while(pos != NULL)
	{
		if(pos->next == NULL)
		{
			pos->next = msg;
			break;
		}
		pos = pos->next;
	}
}
void fetion_sip_parse_notification(const char* sip , int* type , int* event , char** xml)
{
	char type1[16] , *pos;
	xmlChar *event1;
	xmlDocPtr doc;
	xmlNodePtr node;
	bzero(type1 , sizeof(type1));
	fetion_sip_get_attr(sip , "N" , type1);
	if(strcmp(type1 , "PresenceV4") == 0)
		*type = NOTIFICATION_TYPE_PRESENCE;
	else if(strcmp(type1 , "Conversation") == 0)
		*type = NOTIFICATION_TYPE_CONVERSATION;
	else if(strcmp(type1 , "contact") == 0)
		*type = NOTIFICATION_TYPE_CONTACT;
	else if(strcmp(type1 , "registration") == 0)
		*type = NOTIFICATION_TYPE_REGISTRATION;
	else if(strcmp(type1 , "SyncUserInfoV4") == 0)
		*type = NOTIFICATION_TYPE_SYNCUSERINFO;
	else
		*type = NOTIFICATION_TYPE_UNKNOWN;

	pos = strstr(sip , "\r\n\r\n") + 4;
	*xml = (char*)malloc(strlen(pos) + 1);
	memset(*xml , 0 , strlen(pos) + 1);
	strcpy(*xml , pos);
	doc = xmlReadMemory(*xml , strlen(*xml) , NULL , "UTF-8" , XML_PARSE_RECOVER);
	node = xmlDocGetRootElement(doc);
	node = xml_goto_node(node , "event");
	event1 = xmlGetProp(node ,  BAD_CAST "type");
	if(xmlStrcmp(event1 , BAD_CAST "PresenceChanged") == 0)
		*event = NOTIFICATION_EVENT_PRESENCECHANGED;
	else if(xmlStrcmp(event1 , BAD_CAST "UserLeft") == 0)
		*event = NOTIFICATION_EVENT_USERLEFT;
	else if(xmlStrcmp(event1 , BAD_CAST "deregistered") == 0)
		*event = NOTIFICATION_EVENT_DEREGISTRATION;
	else if(xmlStrcmp(event1 , BAD_CAST "SyncUserInfo") == 0)
		*event = NOTIFICATION_EVENT_SYNCUSERINFO;
	else if(xmlStrcmp(event1 , BAD_CAST "AddBuddyApplication") == 0)
		*event = NOTIFICATION_EVENT_ADDBUDDYAPPLICATION;
	else
		*event = NOTIFICATION_EVENT_UNKNOWN;
	xmlFree(event1);
	xmlFreeDoc(doc);
}
void fetion_sip_parse_message(FetionSip* sip , const char* sipmsg , Message** msg)
{
	char len[10] , callid[10] , sequence[10] , sendtime[32] , from[50] , rep[256];
	char *pos = NULL;
	xmlDocPtr doc;
	xmlNodePtr node;
	bzero(len , sizeof(len));
	bzero(callid , sizeof(callid));
	bzero(sequence , sizeof(sequence));
	bzero(sendtime , sizeof(sendtime));
	bzero(from , sizeof(from));
	fetion_sip_get_attr(sipmsg , "F" , from);
	fetion_sip_get_attr(sipmsg , "L" , len);
	fetion_sip_get_attr(sipmsg , "I" , callid);
	fetion_sip_get_attr(sipmsg , "Q" , sequence);
	fetion_sip_get_attr(sipmsg , "D" , sendtime);	

	*msg = fetion_message_new();

	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlReadMemory(pos , strlen(pos) , "UTF-8" , NULL , XML_PARSE_NOERROR);
	fetion_message_set_sipuri(*msg , from);
	fetion_message_set_time(*msg , convert_date(sendtime));
	if(doc != NULL)
	{
		node = xmlDocGetRootElement(doc);
		pos = (char*)xmlNodeGetContent(node);
		fetion_message_set_message(*msg , pos);
		free(pos);
		xmlFreeDoc(doc);
	}
	else
	{
		fetion_message_set_message(*msg , pos);
	}

	bzero(rep , sizeof(rep));
	sprintf(rep ,"SIP-C/4.0 200 OK\r\nF: %s\r\nI: %s\r\nQ: %s\r\n\r\n"
				, from , callid , sequence );
	tcp_connection_send(sip->tcp , rep , strlen(rep));
}
void fetion_sip_parse_invitation(FetionSip* sip , const char* sipmsg , FetionSip** conversionSip , char** sipuri)
{
	char from[50];
	char auth[128];
	char* ipaddress;
	char buf[1024];
	int port;
	char* credential;
	FetionConnection* conn;
	SipHeader* aheader;
	SipHeader* theader;
	SipHeader* mheader;
	SipHeader* nheader;
	char* sipres;
	bzero(from , sizeof(from));
	bzero(auth , sizeof(auth));
	bzero(buf , sizeof(buf));
	fetion_sip_get_attr(sipmsg , "F" , from);
	fetion_sip_get_attr(sipmsg , "A" , auth);
	fetion_sip_get_auth_attr(auth , &ipaddress , &port , &credential);
	conn = tcp_connection_new();
	tcp_connection_connect(conn , ipaddress , port);
	*conversionSip = fetion_sip_clone(sip);
	fetion_sip_set_connection(*conversionSip , conn);
	debug_info("Received a conversation invitation");
	sprintf(buf , "SIP-C/4.0 200 OK\r\nF: %s\r\nI: -61\r\nQ: 200002 I\r\n\r\n"
				, from);
	*sipuri = (char*)malloc(48);
	memset(*sipuri , 0 , 48);
	strcpy(*sipuri , from);
	tcp_connection_send(sip->tcp , buf , strlen(buf));

	fetion_sip_set_type(sip , SIP_REGISTER);
	aheader = fetion_sip_credential_header_new(credential);
	theader = fetion_sip_header_new("K" , "text/html-fragment");
	mheader = fetion_sip_header_new("K" , "multiparty");
	nheader = fetion_sip_header_new("K" , "nudge");
	fetion_sip_add_header(sip , aheader);
	fetion_sip_add_header(sip , theader);
	fetion_sip_add_header(sip , mheader);
	fetion_sip_add_header(sip , nheader);
	sipres = fetion_sip_to_string(sip , NULL);
	debug_info("Register to conversation server %s:%d" , ipaddress , port);
	tcp_connection_send(conn , sipres , strlen(sipres));
	free(sipres);
	free(ipaddress);
	memset(buf , 0 , sizeof(buf));
	port = tcp_connection_recv(conn , buf , sizeof(buf));

	bzero((*conversionSip)->sipuri , sizeof((*conversionSip)->sipuri));
	strcpy((*conversionSip)->sipuri , *sipuri);

}
void fetion_sip_parse_addbuddyapplication(const char* sipmsg , char** sipuri , char** userid , char** desc , int* phrase)
{
	char *pos = NULL;
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar *res = NULL;
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlReadMemory(pos , strlen(pos) , NULL , "UTF-8" , XML_PARSE_RECOVER);
	node = xmlDocGetRootElement(doc);
	node = xml_goto_node(node , "application");

	res = xmlGetProp(node , BAD_CAST "uri");
	*sipuri = (char*)malloc(strlen((char*)res) + 1);
	bzero(*sipuri , strlen((char*)res) + 1);
	strcpy(*sipuri , (char*)res);
	xmlFree(res);

	res = xmlGetProp(node , BAD_CAST "user-id");
	*userid = (char*)malloc(strlen((char*)res) + 1);
	bzero(*userid , strlen((char*)res) + 1);
	strcpy(*userid , (char*)res);
	xmlFree(res);

	res = xmlGetProp(node , BAD_CAST "desc");
	*desc = (char*)malloc(strlen((char*)res) + 1);
	bzero(*desc , strlen((char*)res) + 1);
	strcpy(*desc , (char*)res);
	xmlFree(res);

	res = xmlGetProp(node , BAD_CAST "addbuddy-phrase-id");
	*phrase = atoi((char*)res);
	xmlFree(res);

	xmlFreeDoc(doc);

}

void fetion_sip_parse_incoming(FetionSip* sip , const char* sipmsg , char** sipuri, IncomingType* type)
{
	char *pos = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr node = NULL;
	xmlChar *res = NULL;
	char replyMsg[128];
	char callid[10];
	char seq[10];

	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	if(xmlStrcmp(node->name , BAD_CAST "is-composing") != 0)
	{
		debug_info("Received a unhandled sip message , thanks for sending it to the author");
		printf("%s\n" , sipmsg);
		*type = INCOMING_UNKNOWN;
		return;
	}
	node = node->xmlChildrenNode;
	res = xmlNodeGetContent(node);
	if(xmlStrcmp(res , BAD_CAST "nudge") != 0)
	{
		debug_info("Received a unhandled sip message , thanks for sending it to the author");
		printf("%s\n" , sipmsg);
		*type = INCOMING_UNKNOWN;
		xmlFree(res);
		return;
	}
	xmlFree(res);
	*sipuri = (char*)malloc(50);
	bzero(replyMsg , sizeof(replyMsg));
	bzero(callid   , sizeof(callid));
	bzero(seq 	   , sizeof(seq));
	bzero(*sipuri  , 50);

	fetion_sip_get_attr(sipmsg , "I" , callid);
	fetion_sip_get_attr(sipmsg , "Q" , seq);
	fetion_sip_get_attr(sipmsg , "F" , *sipuri);
	sprintf(replyMsg , "SIP-C/4.0 200 OK\r\n"
					   "F: %s\r\n"
					   "I: %s \r\n"
					   "Q: %s\r\n\r\n"
					 , *sipuri , callid , seq);
	tcp_connection_send(sip->tcp , replyMsg , strlen(replyMsg));
	
	*type = INCOMING_NUDGE;
}

void fetion_sip_parse_userleft(const char* sipmsg , char** sipuri)
{
	char *pos = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr node = NULL;
	xmlChar *res;

	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = xml_goto_node(node , "member");
	res = xmlGetProp(node , BAD_CAST "uri");
	*sipuri = (char*)malloc(xmlStrlen(res) + 1);
	bzero(*sipuri , xmlStrlen(res) + 1);
	strcpy(*sipuri , (char*)res);
	xmlFreeDoc(doc);
}

struct tm convert_date(const char* date)
{
	char* pos = strstr(date , ",") + 2;
	struct tm dstr;

	strptime(pos , "%d %b %Y %T %Z" , &dstr);
	dstr.tm_hour += 8;
	if(dstr.tm_hour > 23)
		dstr.tm_hour -= 24;
	return dstr;
	
}
void fetion_sip_get_auth_attr(const char* auth , char** ipaddress , int* port , char** credential)
{
	char* pos = strstr(auth , "address=\"") + 9;
	int n = strlen(pos) - strlen(strstr(pos , ":"));
	char port_str[6] = { 0 };
	*credential = (char*)malloc(48);
	memset(*credential , 0 , 48);
	*ipaddress = (char*)malloc(18);
	memset(*ipaddress , 0 , 18);
	strncpy(*ipaddress , pos , n);
	pos = strstr(pos , ":") + 1;
	n = strlen(pos) - strlen(strstr(pos , ";"));
	strncpy(port_str , pos , n);
	*port = atoi(port_str);
	pos = strstr(pos , "credential=\"") + 12;
	strncpy(*credential , pos , strlen(pos) - 1);
}
