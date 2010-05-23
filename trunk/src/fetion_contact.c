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
Contact* fetion_contact_new()
{
	Contact* list = (Contact*)malloc(sizeof(Contact));
	memset(list , 0 , sizeof(Contact));
	list->imageChanged = IMAGE_NOT_INITIALIZED;
	list->state = P_HIDDEN;
	list->preNode = NULL;
	list->nextNode = NULL;
	return list;
}
void fetion_contact_list_append(Contact* contactlist , Contact* contact)
{
	Contact* pos = contactlist;
	if(contactlist == NULL)
	{
	debug_info("Try to append a contact to a NULL contactlist at fetion_contact_list_append");
		return;
	}
	while(1)
	{
		if(pos->nextNode == NULL)
		{
			pos->nextNode = contact;
			contact->preNode = pos;
			contact->nextNode = NULL;
			break;
		}
		pos = pos->nextNode;
	}
}
Contact* fetion_contact_list_find_by_userid(Contact* contactlist , const char* userid)
{
	Contact* pos = contactlist;
	while(pos != NULL)
	{
		if(strcmp(pos->userId , userid) == 0)
			return pos;
		pos = pos->nextNode;
	}
	return NULL;
}
Contact* fetion_contact_list_find_by_sipuri(Contact* contactlist , const char* sipuri)
{
	Contact* pos = contactlist;
	while(pos != NULL)
	{
		if(strcmp(pos->sipuri , sipuri) == 0)
			return pos;
		pos = pos->nextNode;
	}
	return NULL;
}
void fetion_contact_list_remove_by_userid(Contact** contactlist , const char* userid)
{
	Contact* pos = *contactlist;
	if(strcmp((*contactlist)->userId , userid) == 0)
	{
		*contactlist = (*contactlist)->nextNode;
		return;
	}
	while(pos != NULL)
	{
		if(strcmp(pos->userId , userid) == 0)	
		{
			pos->preNode->nextNode = pos->nextNode;
			if(pos->nextNode != NULL)
				pos->nextNode->preNode = pos->preNode;
			break;
		}
		pos = pos->nextNode;
	}
}
void fetion_contact_list_free(Contact* contact)
{
	Contact* pos = contact;
	debug_info("Free contact list");
	while(contact != NULL)
	{ 
		contact = contact->nextNode;
	}
}
int fetion_contact_subscribe_only(User* user)
{
	char *res, *body;
	FetionSip* sip;
	SipHeader* eheader;

	sip = user->sip;
	fetion_sip_set_type(sip , SIP_SUBSCRIPTION);
	eheader = fetion_sip_event_header_new(SIP_EVENT_PRESENCE);
	fetion_sip_add_header(sip , eheader);
	body = generate_subscribe_body("0");
	res = fetion_sip_to_string(sip , body);
	free(body);
	debug_info("Start subscribe contact list");
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
	return 1;
}
int fetion_contact_subscribe(User* user)
{

	FetionSip* sip;
	SipMsg *msg , *pos;
	int messageType , notificationEvent , notificationType;
	char* notificationBody;
	fetion_contact_subscribe_only(user);
	while(1)
	{
		if(tcp_connection_select_read(sip->tcp) > 0)
			msg = fetion_sip_listen(sip);
		else
			break;
		pos = msg;
		while(pos != NULL)
		{
			messageType = fetion_sip_get_type(pos->message);
			if(messageType != SIP_NOTIFICATION )
				goto gotonext;
			fetion_sip_parse_notification(pos->message
										, &notificationType
										, &notificationEvent
										, &notificationBody);
		   	if(notificationType == NOTIFICATION_TYPE_PRESENCE && notificationEvent == NOTIFICATION_EVENT_PRESENCECHANGED)
				fetion_user_parse_presence_body(notificationBody , user);
		gotonext:
			pos = pos->next;
		}
		if(msg != NULL)
			fetion_sip_message_free(msg);
	}
	debug_info("Subscribe contact list success");
	return 1;
}
Contact* fetion_contact_get_contact_info(User* user , const char* userid)
{
	FetionSip* sip = user->sip;
	char *res , *body , *pos;
	Contact* contact;
	xmlChar* cs;
	xmlDocPtr doc;
	xmlNodePtr node;
	int n;
	contact = fetion_contact_list_find_by_userid(user->contactList , userid);
	body = generate_contact_info_body(userid);
	fetion_sip_set_type(sip , SIP_SERVICE);
	SipHeader* eheader = fetion_sip_event_header_new(SIP_EVENT_GETCONTACTINFO);
	fetion_sip_add_header(sip , eheader);
	res = fetion_sip_to_string(sip , body);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
	res = fetion_sip_get_response(sip);
	printf("%s\n" , res);
	pos = strstr(res , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	if(xmlHasProp(node , BAD_CAST "carrier-region"))
	{
		cs = xmlGetProp(node , BAD_CAST "carrier-region");
		pos = (char*)cs;
		n = strlen(pos) - strlen(strstr(pos , "."));
		strncpy(contact->country , pos , n);
		pos = strstr(pos , ".") + 1;
		n = strlen(pos) - strlen(strstr(pos , "."));
		strncpy(contact->province , pos , n);
		pos = strstr(pos , ".") + 1;
		n = strlen(pos) - strlen(strstr(pos , "."));
		strncpy(contact->city , pos , n);
		contact->city[n] = 0;
		xmlFree(cs);
		free(res);
	}
	return contact;
}
Contact* fetion_contact_get_contact_info_by_no(User* user , const char* no , NumberType nt)
{
	FetionSip* sip = user->sip;
	SipHeader* eheader;
	char *res , *body;
	Contact* contact;
	int ret;
	fetion_sip_set_type(sip , SIP_SERVICE);
	eheader = fetion_sip_event_header_new(SIP_EVENT_GETCONTACTINFO);
	fetion_sip_add_header(sip , eheader);
	body = generate_contact_info_by_no_body(no , nt);
	res = fetion_sip_to_string(sip , body);
	free(body);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res); res = NULL; 
	res = fetion_sip_get_response(sip);
	ret = fetion_sip_get_code(res);
	if(ret == 200)
	{
		contact = parse_contact_info_by_no_response(res);
		free(res);
		debug_info("Get user information by mobile number success");
		return contact;
	}
	else
	{
		free(res);
		debug_error("Get user information by mobile number failed , errno :" , ret);
		return NULL;
	}
}
int fetion_contact_set_mobileno_permission(User* user , const char* userid , int show)
{
	FetionSip* sip = user->sip;
	SipHeader* eheader;
	char *res , *body;
	int ret;
	fetion_sip_set_type(sip , SIP_SERVICE);
	eheader = fetion_sip_event_header_new(SIP_EVENT_SETCONTACTINFO);
	fetion_sip_add_header(sip , eheader);
	body = generate_set_mobileno_perssion(userid , show);
	res = fetion_sip_to_string(sip , body);
	free(body);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res) ; 
	res = fetion_sip_get_response(sip);
	ret = fetion_sip_get_code(res);
	if(ret == 200)
	{
		parse_set_mobileno_permission_response(user , res);
		free(res);
		debug_info("Get user information by mobile number success");
		return 1;
	}
	else
	{
		free(res);
		debug_error("Get user information by mobile number failed , errno :" , ret);
		return -1;
	}

}
int fetion_contact_set_displayname(User* user , const char* userid , const char* name)
{
	FetionSip* sip = user->sip;
	SipHeader* eheader;
	char *res , *body;
	int ret;
	fetion_sip_set_type(sip , SIP_SERVICE);
	eheader = fetion_sip_event_header_new(SIP_EVENT_SETCONTACTINFO);
	fetion_sip_add_header(sip , eheader);
	body = generate_set_displayname_body(userid , name);
	res = fetion_sip_to_string(sip , body);
	free(body);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
	res = fetion_sip_get_response(sip);
	ret = fetion_sip_get_code(res);
	free(res);
	if(ret == 200)
	{
		debug_info("Set buddy(%s)`s localname to %s success" , userid , name);
		return 1;
	}
	else
	{
		debug_info("Set buddy(%s)`s localname to %s failed" , userid , name);
		return -1;
	}
}
int fetion_contact_move_to_group(User* user , const char* userid , int buddylist)
{
	FetionSip* sip = user->sip;
	SipHeader* eheader;
	char *res , *body;
	int ret;
	fetion_sip_set_type(sip , SIP_SERVICE);
	eheader = fetion_sip_event_header_new(SIP_EVENT_SETCONTACTINFO);
	fetion_sip_add_header(sip , eheader);
	body = generate_move_to_group_body(userid , buddylist);
	res = fetion_sip_to_string(sip , body);
	free(body);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
	res = fetion_sip_get_response(sip);
	ret = fetion_sip_get_code(res);
	free(res);
	if(ret == 200)
	{
		debug_info("Move buddy(%s) to group %d success" , userid , buddylist);
		return 1;
	}
	else
	{
		debug_info("Move buddy(%s) to group %d failed" , userid , buddylist);
		return -1;
	}
}
int fetion_contact_delete_buddy(User* user , const char* userid)
{
	FetionSip* sip = user->sip;
	SipHeader* eheader;
	char *res , *body;
	int ret;
	fetion_sip_set_type(sip , SIP_SERVICE);
	eheader = fetion_sip_event_header_new(SIP_EVENT_DELETEBUDDY);
	fetion_sip_add_header(sip , eheader);
	body = generate_delete_buddy_body(userid);
	res = fetion_sip_to_string(sip , body);
	free(body);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
	res = fetion_sip_get_response(sip);
	ret = fetion_sip_get_code(res);
	free(res);
	if(ret == 200)
	{
		fetion_contact_list_remove_by_userid(&(user->contactList) , userid);
		debug_info("Delete buddy(%s) success" , userid);
		return 1;
	}
	else
	{
		debug_info("Delete buddy(%s) failed" , userid);
		return -1;
	}
}
Contact* fetion_contact_add_buddy(User* user , const char* no
								, NumberType notype , int buddylist
								, const char* localname , const char* desc
								, int phraseid , int* statuscode)
{
	FetionSip* sip = user->sip;
	SipHeader* eheader = NULL;
	SipHeader* ackheader = NULL;
	char *res = NULL;
	char *body = NULL;
	int ret;
	Contact* contact;
	fetion_sip_set_type(sip , SIP_SERVICE);
	eheader = fetion_sip_event_header_new(SIP_EVENT_ADDBUDDY);
	fetion_sip_add_header(sip , eheader);
	if(user->verification != NULL && user->verification->algorithm != NULL)	
	{
		ackheader = fetion_sip_ack_header_new(user->verification->code
											, user->verification->algorithm
											, user->verification->type
											, user->verification->guid);
		fetion_sip_add_header(sip , ackheader);
	}
	body = generate_add_buddy_body(no , notype , buddylist , localname , desc , phraseid);

	res = fetion_sip_to_string(sip , body);
	free(body);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
	res = fetion_sip_get_response(sip);
	ret = fetion_sip_get_code(res);
	*statuscode = ret;
	switch(ret)
	{
		case 200 :
			contact = parse_add_buddy_response(res , statuscode);
			fetion_verification_free(user->verification);
			user->verification = NULL;
			free(res);
			fetion_contact_list_append(user->contactList , contact);
			debug_info("Add buddy(%s) success" , no);
			return contact;
		case 421 : 
		case 420 :
			parse_add_buddy_verification(user , res);
			debug_info("Add buddy(%s) falied , need verification" , no);
			return NULL;
		default:
			free(res);
			debug_info("Add buddy(%s) failed" , no);
			return NULL;
	}
}

Contact* fetion_contact_handle_contact_request(User* user
		, const char* sipuri , const char* userid
		, const char* localname , int buddylist , int result)
{
	FetionSip* sip = user->sip;
	SipHeader* eheader;
	char *res , *body;
	int ret;
	Contact* contact;
	fetion_sip_set_type(sip , SIP_SERVICE);
	eheader = fetion_sip_event_header_new(SIP_EVENT_HANDLECONTACTREQUEST);
	fetion_sip_add_header(sip , eheader);
	body = generate_handle_contact_request_body(sipuri , userid , localname , buddylist , result);
	res = fetion_sip_to_string(sip , body);
	free(body);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
	res = fetion_sip_get_response(sip);
	ret = fetion_sip_get_code(res);
	switch(ret)
	{
		case 200 :
			contact = parse_handle_contact_request_response(res);
			free(res);
			fetion_contact_list_append(user->contactList , contact);
			debug_info("handle contact request from (%s) success" , userid);
			return contact;
		default:
			free(res);
			debug_info("handle contact request from (%s) failed" , userid);
			return NULL;
	}
	return NULL;
}

int fetion_contact_save(User* user)
{
#if 0
	FILE* fd;
	char filename[] = "contact.dat";
	char* filepath;
	Contact* pos = user->contactList;	
	Config* config = user->config;
	int n;

	n = strlen(config->userPath) + strlen(filename) + 2;
	filepath = (char*)malloc(n);
	bzero(filepath , n);
	strcpy(filepath , config->userPath);
	strcat(filepath , "/");
	strcat(filepath , filename);
	
	fd = fopen(filepath , "w+");
	debug_info("Storing contact list to local disk");
	if(fd == NULL)
	{
		debug_info("Store contact list to local disk failed");
		return -1;
	}
	while(pos != NULL)
	{
		fwrite(pos , sizeof(Contact) , 1 , fd);
		pos = pos->nextNode;
	}
	fclose(fd);
	free(filepath);
	return 1;
#endif
}
void fetion_contact_load(User* user)
{
#if 0
	FILE* fd;
	char filename[] = "contact.dat";
	char* filepath;
	Contact *pos , *contactlist = NULL;	
	Config* config = user->config;
	int n ;
	n = strlen(config->userPath) + strlen(filename) + 2;
	filepath = (char*)malloc(n);
	bzero(filepath , n);
	strcpy(filepath , config->userPath);
	strcat(filepath , "/");
	strcat(filepath , filename);
	fd = fopen(filepath , "r");
	debug_info("Reading contact list from local disk");
	if(fd == NULL)
	{
		debug_info("Reading contact list from local disk failed");
		return;
	}
	while(!feof(fd))
	{
		pos = fetion_contact_new();
		n = fread(pos , sizeof(Contact) , 1 , fd);
		pos->preNode = NULL;
		pos->nextNode = NULL;
		pos->imageChanged = IMAGE_NOT_INITIALIZED;
		pos->state = 0;
		if(n > 0)
			if(contactlist == NULL)
				contactlist = pos;
			else
				fetion_contact_list_append(contactlist , pos);
		else
			free(pos);
	}
	user->contactList = contactlist;
	fclose(fd);
	free(filepath);
#endif
}
char* generate_subscribe_body(const char* version)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	char body[] = "<args></args>";
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "subscription" , NULL);
	xmlNewProp(node , BAD_CAST "self" , BAD_CAST "v4default;mail-count");
	xmlNewProp(node , BAD_CAST "buddy" , BAD_CAST "v4default");
	xmlNewProp(node , BAD_CAST "version" , BAD_CAST version);
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return xml_convert(buf);
}
char* generate_contact_info_body(const char* userid)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	char body[] = "<args></args>";
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);
	xmlNewProp(node , BAD_CAST "user-id" , BAD_CAST userid);
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return xml_convert(buf);
	
}
char* generate_contact_info_by_no_body(const char* no , NumberType nt)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	char uri[32];
	char body[] = "<args></args>";
	bzero(uri , sizeof(uri));
	if(nt == MOBILE_NO)
		sprintf(uri , "tel:%s" , no);
	else
		sprintf(uri , "sip:%s" , no);
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);
	xmlNewProp(node , BAD_CAST "uri" , BAD_CAST uri);
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return xml_convert(buf);
}
char* generate_set_mobileno_perssion(const char* userid , int show)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	char permission[32];
	char body[] = "<args></args>";
	bzero(permission , sizeof(permission));
	sprintf(permission , "identity=%d" , show);
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);
	xmlNewProp(node , BAD_CAST "user-id" , BAD_CAST userid);
	xmlNewProp(node , BAD_CAST "permission" , BAD_CAST permission);
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return xml_convert(buf);
}
char* generate_handle_contact_request_body(const char* sipuri
		, const char* userid , const char* localname
		, int buddylist , int result )
{
	char args[] = "<args></args>";
	char result_s[4];
	char buddylist_s[4];
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlReadMemory(args , strlen(args) , NULL , "UTF-8" , XML_PARSE_RECOVER);
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddies" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddy" , NULL);
	xmlNewProp(node , BAD_CAST "user-id" , BAD_CAST userid);
	xmlNewProp(node , BAD_CAST "uri" , BAD_CAST sipuri);
	bzero(result_s , sizeof(result_s));
	sprintf(result_s , "%d" , result);
	bzero(buddylist_s , sizeof(buddylist_s));
	sprintf(buddylist_s , "%d" , buddylist);
	xmlNewProp(node , BAD_CAST "result" , BAD_CAST result_s);
	xmlNewProp(node , BAD_CAST "buddy-lists" , BAD_CAST buddylist_s);
	xmlNewProp(node , BAD_CAST "expose-mobile-no" , BAD_CAST "1");
	xmlNewProp(node , BAD_CAST "expose-name" , BAD_CAST "1");
	xmlNewProp(node , BAD_CAST "local-name" , BAD_CAST localname);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return xml_convert(res);
}
char* generate_set_displayname_body(const char* userid , const char* name)
{
	char args[] = "<args></args>";
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);
	xmlNewProp(node , BAD_CAST "user-id" , BAD_CAST userid);
	xmlNewProp(node , BAD_CAST "local-name" , BAD_CAST name);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return xml_convert(res);
}
char* generate_move_to_group_body(const char* userid , int buddylist)
{
	char args[] = "<args></args>";
	char bl[5];
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);
	xmlNewProp(node , BAD_CAST "user-id" , BAD_CAST userid);
	bzero(bl , sizeof(bl));
	sprintf(bl , "%d" , buddylist);
	xmlNewProp(node , BAD_CAST "buddy-lists" , BAD_CAST bl);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return xml_convert(res);
}
char* generate_delete_buddy_body(const char* userid)
{
	char args[] = "<args></args>";
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddies" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddy" , NULL);
	xmlNewProp(node , BAD_CAST "user-id" , BAD_CAST userid);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return xml_convert(res);
}
char* generate_add_buddy_body(const char* no 
		, NumberType notype , int buddylist 
		, const char* localname , const char* desc , int phraseid)
{
	char args[] = "<args></args>";
	char uri[48];
	char phrase[4];
	char groupid[4];
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddies" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddy" , NULL);
	bzero(uri , sizeof(uri));
	bzero(phrase , sizeof(phrase));
	bzero(groupid , sizeof(groupid));
	if(notype == FETION_NO)
		sprintf(uri , "sip:%s" , no);
	else
		sprintf(uri , "tel:%s" , no);
	sprintf(phrase , "%d" , phraseid);
	sprintf(groupid , "%d" , buddylist);
	xmlNewProp(node , BAD_CAST "uri" , BAD_CAST uri);
	xmlNewProp(node , BAD_CAST "local-name" , BAD_CAST localname);
	xmlNewProp(node , BAD_CAST "buddy-lists" , BAD_CAST groupid);
	xmlNewProp(node , BAD_CAST "desc" , BAD_CAST desc);
	xmlNewProp(node , BAD_CAST "expose-mobile-no" , BAD_CAST "1");
	xmlNewProp(node , BAD_CAST "expose-name" , BAD_CAST "1");
	xmlNewProp(node , BAD_CAST "addbuddy-phrase-id" , BAD_CAST phrase);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return xml_convert(res);
}
void parse_set_displayname_response(User* user , const char* userid , const char* sipmsg)
{
	char *pos;
	Contact* contact;
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	contact = fetion_contact_list_find_by_userid(user->contactList , userid);
	if(contact == NULL)
	{
		debug_error("Can not find user(%s) in contact list" , userid);
		return;
	}
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	res = xmlGetProp(node , BAD_CAST "version");
	bzero(user->contactVersion , sizeof(user->contactVersion));
	strcpy(user->contactVersion , (char*)res);
	xmlFree(res);
	node = node->xmlChildrenNode;
	res = xmlGetProp(node , BAD_CAST "local-name");
	strcpy(contact->localname , (char*)res);
	xmlFree(res);
	xmlFreeDoc(doc);
}
void parse_set_mobileno_permission_response(User* user , const char* sipmsg)
{
	char *pos;
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	res = xmlGetProp(node , BAD_CAST "contact-list-version");
	bzero(user->contactVersion , sizeof(user->contactVersion));
	strcpy(user->contactVersion , (char*)res);
	xmlFree(res);
	xmlFreeDoc(doc);
}
Contact* parse_contact_info_by_no_response(const char* sipmsg)
{
	char *pos;
	Contact* contact;
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	int n;
	contact = fetion_contact_new();
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	if(xmlHasProp(node , BAD_CAST "uri"))
	{
		res = xmlGetProp(node , BAD_CAST "uri");
		strcpy(contact->sipuri , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "user-id"))
	{
		res = xmlGetProp(node , BAD_CAST "user-id");
		strcpy(contact->userId , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "sid"))
	{
		res = xmlGetProp(node , BAD_CAST "sid");
		strcpy(contact->sId , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "nickname"))
	{
		res = xmlGetProp(node , BAD_CAST "nickname");
		strcpy(contact->nickname , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "gender"))
	{
		res = xmlGetProp(node , BAD_CAST "gender");
		contact->gender = atoi((char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "birth-date"))
	{
		res = xmlGetProp(node , BAD_CAST "birth-date");
		strcpy(contact->birthday , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "impresa"))
	{
		res = xmlGetProp(node , BAD_CAST "impresa");
		strcpy(contact->impression , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "mobile-no"))
	{
		res = xmlGetProp(node , BAD_CAST "mobile-no");
		strcpy(contact->mobileno , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "carrier-region"))
	{
		res = xmlGetProp(node , BAD_CAST "carrier-region");
		pos = (char*)res;
		n = strlen(pos) - strlen(strstr(pos , "."));
		strncpy(contact->country , pos , n);
		pos = strstr(pos , ".") + 1;
		n = strlen(pos) - strlen(strstr(pos , "."));
		strncpy(contact->province , pos , n);
		pos = strstr(pos , ".") + 1;
		n = strlen(pos) - strlen(strstr(pos , "."));
		strncpy(contact->city , pos , n);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "portrait-crc"))
	{
		res = xmlGetProp(node , BAD_CAST "portrait-crc");
		strcpy(contact->portraitCrc , (char*)res);
		xmlFree(res);
	}
	xmlFreeDoc(doc);
	return contact;
}
Contact* parse_add_buddy_response(const char* sipmsg , int* statuscode)
{
	char *pos;
	Contact* contact;
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	contact = fetion_contact_new();
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = xml_goto_node(node , "buddy");
	if(node == NULL)
	{
		*statuscode = 400;
		return NULL;
	}
	if(xmlHasProp(node , BAD_CAST "uri"))
	{
		res = xmlGetProp(node , BAD_CAST "uri");
		strcpy(contact->sipuri , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "user-id"))
	{
		res = xmlGetProp(node , BAD_CAST "user-id");
		strcpy(contact->userId , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "mobile-no"))
	{
		res = xmlGetProp(node , BAD_CAST "mobile-no");
		strcpy(contact->mobileno , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "local-name"))
	{
		res = xmlGetProp(node , BAD_CAST "local-name");
		strcpy(contact->localname , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "buddy-lists"))
	{
		res = xmlGetProp(node , BAD_CAST "buddy-lists");
		contact->groupid = atoi((char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "status-code"))
	{
		res = xmlGetProp(node , BAD_CAST "status-code");
		*statuscode = atoi((char*)res);
	}
	else
	{
		*statuscode = 200;
	}
	contact->serviceStatus = STATUS_NOT_AUTHENTICATED;
	xmlFreeDoc(doc);
	return contact;
}
Contact* parse_handle_contact_request_response(const char* sipmsg)
{
	char *pos = NULL;
	Contact* contact = NULL;
	xmlChar* res = NULL;
	xmlDocPtr doc;
	xmlNodePtr node;
	contact = fetion_contact_new();
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = xml_goto_node(node , "buddy");
	if(xmlHasProp(node , BAD_CAST "uri"))
	{
		res = xmlGetProp(node , BAD_CAST "uri");
		strcpy(contact->sipuri , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "user-id"))
	{
		res = xmlGetProp(node , BAD_CAST "user-id");
		strcpy(contact->userId , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "local-name"))
	{
		res = xmlGetProp(node , BAD_CAST "local-name");
		strcpy(contact->localname , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "buddy-lists"))
	{
		res = xmlGetProp(node , BAD_CAST "buddy-lists");
		contact->groupid = atoi((char*)res);
		xmlFree(res);
	}
	contact->serviceStatus = RELATION_STATUS_UNAUTHENTICATED;
	xmlFreeDoc(doc);
	return contact;
}
void parse_add_buddy_verification(User* user , const char* str)
{
	char* xml = NULL;
	char w[128];
	int n = 0;
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar *res = NULL;
	Verification *ver = NULL;

	ver = (Verification*)malloc(sizeof(Verification));
	memset(ver , 0 , sizeof(sizeof(Verification)));

	bzero(w , sizeof(w));
	fetion_sip_get_attr(str , "W" , w);
	xml = strstr(w , "algorithm=") + 11;
	n = strlen(xml) - strlen(strstr(xml , "\""));
	ver->algorithm = (char*)malloc(n + 1);
	bzero(ver->algorithm , n + 1);
	strncpy(ver->algorithm , xml , n);
	xml = strstr(w , "type=") + 6;
	n = strlen(xml) - strlen(strstr(xml , "\""));
	ver->type = (char*)malloc(n + 1);
	bzero(ver->type , n +1);
	strncpy(ver->type , xml , n);

	xml = strstr(str , "\r\n\r\n");
	doc = xmlParseMemory(xml , strlen(xml));
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	res = xmlGetProp(node , BAD_CAST "text");
	n = xmlStrlen(res) + 1;
	ver->text = (char*)malloc(n);
	bzero(ver->text , n);
	strncpy(ver->text , (char*)res , n - 1);
	xmlFree(res);
	res = xmlGetProp(node , BAD_CAST "tips");
	n = xmlStrlen(res) + 1;
	ver->tips = (char*)malloc(n);
	bzero(ver->tips , n);
	strncpy(ver->tips , (char*)res , n - 1);
	xmlFree(res);
	user->verification = ver;
}
