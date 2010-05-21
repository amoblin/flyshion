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

static void 
parse_option_verification(User *user , const char *in)
{
	int n;
	char *pos , *pos1 , w[256];
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar *res;

	user->verification = fetion_verification_new();
	bzero(w , sizeof(w));
	fetion_sip_get_attr(in , "W" , w);
	pos = strstr(w , "thm=\"") + 5;
	n = strlen(pos) - strlen(strstr(pos , "\""));
	user->verification->algorithm = (char*)malloc(n + 1);
	bzero(user->verification->algorithm , n + 1);
	strncpy(user->verification->algorithm , pos , n);
	pos = strstr(pos , "type=\"") + 6;
	n = strlen(pos) - strlen(strstr(pos , "\""));
	user->verification->type = (char*)malloc(n + 1);
	bzero(user->verification->type , n + 1);
	strncpy(user->verification->type , pos , n);

	pos = strstr(in , "\r\n\r\n") + 4;
	doc = xmlReadMemory(pos , strlen(pos)
			, NULL , "UTF-8" , XML_PARSE_RECOVER);
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	if(xmlHasProp(node , BAD_CAST "text")){
		res = xmlGetProp(node , BAD_CAST "text");
		user->verification->text = (char*)malloc(xmlStrlen(res) + 1);
		bzero(user->verification->text , xmlStrlen(res) + 1);
		strcpy(user->verification->text , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "tips")){
		res = xmlGetProp(node , BAD_CAST "tips");
		if(strstr((char*)res , "，<a")){
			n = xmlStrlen(res) - strlen(strstr((char*)res , "，<a"));
			user->verification->tips = (char*)malloc(n + 1);
			bzero(user->verification->tips , n + 1);
			strncpy(user->verification->tips , (char*)res , n);
		}
		xmlFree(res);
	}
}

void fetion_directsms_send_option(User *user)
{
	FetionSip *sip = user->sip;
	SipHeader *eheader;
	char *res;
	
	fetion_sip_set_type(sip , SIP_OPTION);
	eheader = fetion_sip_event_header_new(SIP_EVENT_DIRECTSMS);	
	fetion_sip_add_header(sip , eheader);
	res = fetion_sip_to_string(sip , NULL);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
	res = fetion_sip_get_response(sip);
	parse_option_verification(user , res);
	free(res);
}

static int 
parse_subscribe_response(const char *in , char **error)
{
	char *pos , c[4];
	int code , n;
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar *res;

	pos = strstr(in , " ") + 1;
	n = strlen(pos) - strlen(strstr(pos , " "));
	bzero(c , sizeof(c));
	strncpy(c , pos , n);
	if(strcmp(c , "200") == 0){
		*error = NULL;
		return PIC_SUCCESS;
	}
	pos = strstr(in , "\r\n\r\n") + 4;
	doc = xmlReadMemory(pos , strlen(pos)
			, NULL , "UTF-8" , XML_PARSE_RECOVER );
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	if(xmlStrcmp(node->name , BAD_CAST "error") == 0){
		if(xmlHasProp(node , BAD_CAST "user-msg")){
			res = xmlGetProp(node , BAD_CAST "user-msg");
			*error = (char*)malloc(xmlStrlen(res) + 1);
			strcpy(*error , (char*)res);
			xmlFree(res);
			return PIC_ERROR;
		}else{
			*error = NULL;
			return UNKNOW_ERROR;
		}
	}else{
		*error = NULL;
		return UNKNOW_ERROR;
	}
}

int fetion_directsms_send_subscribe(User *user , const char *code , char **error)
{
	char body[256];
	char http[2048];
	char *ip;
	FetionConnection *tcp;

	ip = get_ip_by_name(NAVIGATION_URI);
	bzero(body , sizeof(body));
	sprintf(body , "PicCertSessionId=%s&PicCertCode=%s&MobileNo=%s"
			, user->verification->guid , code , user->mobileno);

	bzero(http , sizeof(http));
	sprintf(http , "POST /nav/ApplySubscribe.aspx HTTP/1.1\r\n"
				   "Cookie: ssic=%s\r\n"
				   "Accept: */*\r\n"
				   "Host: %s\r\n"
				   "Content-Length: %d\r\n"
				   "Content-Type: application/x-www-form-urlencoded;"
				   "charset=utf-8\r\n"
				   "User-Agent: IIC2.0/PC "PROTO_VERSION"\r\n"
				   "Connection: Keep-Alive\r\n"
				   "Cache-Control: no-cache\r\n\r\n%s"
				 , user->ssic , NAVIGATION_URI , strlen(body) , body);

	printf("%s\n" , http);
	tcp = tcp_connection_new();
	tcp_connection_connect(tcp , ip , 80);
	tcp_connection_send(tcp , http , strlen(http));
	bzero(http , sizeof(http));
	tcp_connection_recv(tcp , http , sizeof(http));
	printf("%s\n" , http);
	return parse_subscribe_response(http , error);

}
