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

static char* generate_contact_info_by_no_body(const char* no);
static char* generate_get_members_body(const char *pguri);
static void pg_group_member_append(PGGroupMember *head , PGGroupMember *newmem);
static void pg_group_append(PGGroup *head , PGGroup *n);
static void pg_group_prepend(PGGroup *head , PGGroup *n);

static PGGroup *pg_group_new(const char *pguri , int identity)
{
	PGGroup *pggroup = (PGGroup*)malloc(sizeof(PGGroup));

	memset(pggroup , 0 , sizeof(PGGroup));
	if(pguri)
	    strcpy(pggroup->pguri , pguri);
	pggroup->member = pg_group_member_new();
	pggroup->identity = identity;
	pggroup->next = pggroup->pre = pggroup;

	return pggroup;
}

static void pg_group_append(PGGroup *head , PGGroup *n)
{
	head->next->pre = n;
	n->pre = head;
	n->next = head->next;
	head->next = n;
}

static void pg_group_prepend(PGGroup *head , PGGroup *n)
{
	head->pre->next = n;
	n->pre = head->pre;
	n->next = head;
	head->pre = n;
}

PGGroup *pg_group_parse_list(const char *in)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar *res;
	PGGroup *pggroup , *newpg;
	
	doc = xmlReadMemory(in , strlen(in) , NULL , "UTF-8" , XML_PARSE_RECOVER);
	node = xmlDocGetRootElement(doc);
	node = xml_goto_node(node , "group");
	if(!node)
	    return NULL;

	pggroup = pg_group_new(NULL , 0);
	while(node){
	    	newpg = pg_group_new(NULL , 0);
		res = xmlGetProp(node , BAD_CAST "uri");
		strcpy(newpg->pguri , (char*)res);
		xmlFree(res);
		if(xmlHasProp(node , BAD_CAST "identity")){
			res = xmlGetProp(node , BAD_CAST "identity");
			newpg->identity = atoi((char*)res);
			xmlFree(res);
		}
		if(newpg->identity == 1)
		    pg_group_prepend(pggroup , newpg);
		else
		    pg_group_append(pggroup , newpg);
		node = node->next;
	}
	xmlFreeDoc(doc);

	return pggroup;
}

void pg_group_get_list(User *user)
{
	FetionSip *sip;
	SipHeader *eheader;
	const char *body = "<args><group-list /></args>";
	char *res;
	extern int callid;

	sip = user->sip;

	fetion_sip_set_type(sip , SIP_SERVICE);	
	eheader = fetion_sip_event_header_new(SIP_EVENT_PGGETGROUPLIST);
	fetion_sip_add_header(sip , eheader);

	user->pgGroupCallId = callid;

	res = fetion_sip_to_string(sip , body);

	tcp_connection_send(sip->tcp , res , strlen(res));

	free(res);
}

static char *generate_get_info_body(PGGroup *pg)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlNodePtr node1;
	PGGroup *cur;
	char body[] = "<args></args>";


	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "groups" , NULL);
	xmlNewProp(node , BAD_CAST "attributes" , BAD_CAST "all");
	foreach_pg_group(pg , cur){
		node1 = xmlNewChild(node , NULL , BAD_CAST "group" , NULL);
		xmlNewProp(node1 , BAD_CAST "uri" , BAD_CAST cur->pguri);
	}
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return xml_convert(buf);
}

void pg_group_get_info(User *user , PGGroup *pg)
{
	FetionSip *sip;
	SipHeader *eheader;
	char *res;
	char *body;
	extern int callid;

	sip = user->sip;

	fetion_sip_set_type(sip , SIP_SERVICE);
	eheader = fetion_sip_event_header_new(SIP_EVENT_PGGETGROUPINFO);
	fetion_sip_add_header(sip , eheader);

	user->groupInfoCallId = callid;

	body = generate_get_info_body(pg);
	res = fetion_sip_to_string(sip , body);
	free(body);

	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
	
}

void pg_group_parse_info(PGGroup *pg , const char *sipmsg)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar *res;
	PGGroup *pgcur;
	char *pos;

	pos = strstr(sipmsg , "\r\n\r\n") + 4;

	doc = xmlReadMemory(pos , strlen(pos)
		, NULL , "UTF-8" , XML_PARSE_RECOVER);
	node = xmlDocGetRootElement(doc);
	node = xml_goto_node(node , "group");
	if(!node)
	    return;
	while(node != NULL){
		res = xmlGetProp(node , BAD_CAST "uri");		
		foreach_pg_group(pg , pgcur){
			if(xmlStrcmp(res , BAD_CAST pgcur->pguri) == 0)
			    break;
		}
		if(xmlHasProp(node , BAD_CAST "status-code")){
			res = xmlGetProp(node , BAD_CAST "status-code");
			pgcur->statusCode = atoi((char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "name")){
			res = xmlGetProp(node , BAD_CAST "name");
			strcpy(pgcur->name , (char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "category")){
			res = xmlGetProp(node , BAD_CAST "category");
			pgcur->category = atoi((char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "current-member-count")){
			res = xmlGetProp(node , BAD_CAST "current-member-count");
			pgcur->currentMemberCount = atoi((char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "limit-member-count")){
			res = xmlGetProp(node , BAD_CAST "limit-member-count");
			pgcur->limitMemberCount = atoi((char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "group-rank")){
			res = xmlGetProp(node , BAD_CAST "group-rank");
			pgcur->groupRank = atoi((char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "max-rank")){
			res = xmlGetProp(node , BAD_CAST "max-rank");
			pgcur->maxRank = atoi((char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "bulletin")){
			res = xmlGetProp(node , BAD_CAST "bulletin");
			strcpy(pgcur->bulletin , (char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "introduce")){
			res = xmlGetProp(node , BAD_CAST "introduce");
			strcpy(pgcur->summary , (char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "create-time")){
			res = xmlGetProp(node , BAD_CAST "create-time");
			strcpy(pgcur->createTime , (char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "get-group-portrait-hds")){
			res = xmlGetProp(node , BAD_CAST "get-group-portrait-hds");
			strcpy(pgcur->getProtraitUri , (char*)res);
			xmlFree(res);
		}
		node = node->next;
	}
}

static char *generate_pg_subscribe_body(PGGroup *pg)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node0;
	xmlNodePtr node;
	xmlNodePtr node1;
	char body[] = "<args></args>";


	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node0 = xmlNewChild(node , NULL , BAD_CAST "subscription" , NULL);
	node = xmlNewChild(node0 , NULL , BAD_CAST "groups" , NULL);
	node1 = xmlNewChild(node , NULL , BAD_CAST "group" , NULL);
	xmlNewProp(node1 , BAD_CAST "uri" , BAD_CAST pg->pguri);
	node = xmlNewChild(node0 , NULL , BAD_CAST "presence" , NULL);

	node1 = xmlNewChild(node , NULL , BAD_CAST "basic" , NULL);
	xmlNewProp(node1 , BAD_CAST "attributes" , BAD_CAST "all");

	node1 = xmlNewChild(node , NULL , BAD_CAST "member" , NULL);
	xmlNewProp(node1 , BAD_CAST "attributes" , BAD_CAST "identity");

	node1 = xmlNewChild(node , NULL , BAD_CAST "management" , NULL);
	xmlNewProp(node1 , BAD_CAST "attributes" , BAD_CAST "all");
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return xml_convert(buf);
}

void pg_group_subscribe(User *user , PGGroup *pg)
{
	FetionSip *sip;
	SipHeader *eheader;
	char *res;
	char *body;

	sip = user->sip;

	fetion_sip_set_type(sip , SIP_SUBSCRIPTION);
	eheader = fetion_sip_event_header_new(SIP_EVENT_PGPRESENCE);
	fetion_sip_add_header(sip , eheader);

	body = generate_pg_subscribe_body(pg);
	res = fetion_sip_to_string(sip , body);
	free(body);
	
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);

}

void pg_group_get_group_members(User *user , PGGroup *pg)
{
	FetionSip *sip;
	SipHeader *nheader;
	char *body;
	char *res;

	sip = user->sip;

	fetion_sip_set_type(sip , SIP_SERVICE);
	nheader = fetion_sip_event_header_new(SIP_EVENT_PGGETGROUPMEMBERS);
	fetion_sip_add_header(sip , nheader);
	pg->getMembersCallId = sip->callid;
	body = generate_get_members_body(pg->pguri);
	res = fetion_sip_to_string(sip , body);

	tcp_connection_send(sip->tcp , res , strlen(res));

	free(res);
}

void pg_group_parse_member_list(PGGroup *pggroup , const char *sipmsg)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlNodePtr cnode;
	xmlChar *res;
	PGGroup *pgcur;
	PGGroupMember *member;
	char *pos;

	if(strstr(sipmsg , "\r\n\r\n") == NULL){
		fprintf(stderr , "FATAL ERROR\n");
		return;
	}

	pos = strstr(sipmsg , "\r\n\r\n") + 4;

	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode->xmlChildrenNode;
	while(node != NULL){
		if(xmlHasProp(node , BAD_CAST "uri")){
			res = xmlGetProp(node , BAD_CAST "uri");
			foreach_pg_group(pggroup , pgcur){
				if(xmlStrcmp(res , BAD_CAST pgcur->pguri) == 0)
				    break;
			}	    
		}else{
			return;
		}
		cnode = node->xmlChildrenNode;
		while(cnode != NULL){
		    	member = pg_group_member_new();
			if(xmlHasProp(cnode , BAD_CAST "uri")){
				res = xmlGetProp(cnode , BAD_CAST "uri");
				strcpy(member->sipuri , (char*)res);
				xmlFree(res);
			}
			if(xmlHasProp(cnode , BAD_CAST "iicnickname")){
				res = xmlGetProp(cnode , BAD_CAST "iicnickname");
				strcpy(member->nickname , (char*)res);
				xmlFree(res);
			}
			if(xmlHasProp(cnode , BAD_CAST "identity")){
				res = xmlGetProp(cnode , BAD_CAST "identity");
				member->identity = atoi((char*)res);
				xmlFree(res);
			}
			if(xmlHasProp(cnode , BAD_CAST "user-id")){
				res = xmlGetProp(cnode , BAD_CAST "user-id");
				strcpy(member->userId , (char*)res);
				xmlFree(res);
			}
			pg_group_member_append(pgcur->member , member);
			cnode = cnode->next;
		}
		node = node->next;
	}
	xmlFreeDoc(doc);
}

PGGroupMember *pg_group_member_new()
{
	PGGroupMember *pgmem = (PGGroupMember*)malloc(sizeof(PGGroupMember));

	memset(pgmem , 0 , sizeof(PGGroupMember));
	pgmem->contact = NULL;
	pgmem->next = pgmem->pre = pgmem;

	return pgmem;
}

static void pg_group_member_append(PGGroupMember *head , PGGroupMember *newmem)
{
	head->next->pre = newmem;
	newmem->next = head->next;
	newmem->pre = head;
	head->next = newmem;
}
#if 0
static void pg_group_member_prepend(PGGroupMember *head , PGGroupMember *newmem)
{
	head->pre->next = newmem;
	newmem->pre = head->pre;
	newmem->next = head;
	head->pre = newmem;
}
#endif

void pg_group_parse_member(PGGroup *pg , const char *sipmsg)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlNodePtr mnode;
	xmlChar *res;
	PGGroup *pgcur;
	PGGroupMember *member;
	char *pos;

	pos = strstr(sipmsg , "\r\n\r\n") + 4;

	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = xml_goto_node(node , "group");
	while(node != NULL){
		res = xmlGetProp(node , BAD_CAST "uri");
		foreach_pg_group(pg , pgcur){
			if(xmlStrcmp(res , BAD_CAST pgcur->pguri) == 0)
			    break;
		}
		mnode = node->xmlChildrenNode;
		if(!mnode){
			node = node->next;
			continue;
		}
		while(mnode != NULL){
			res = xmlGetProp(mnode , BAD_CAST "uri");
		    	foreach_pg_member(pgcur->member , member){
				if(xmlStrcmp(res , BAD_CAST member->sipuri) == 0)
				    break;
			}
			if(xmlHasProp(mnode , BAD_CAST "identity")){
				res = xmlGetProp(mnode , BAD_CAST "identity");
				member->identity = atoi((char*)res);
				xmlFree(res);
			}
			if(xmlHasProp(mnode , BAD_CAST "state")){
				res = xmlGetProp(mnode , BAD_CAST "state");
				member->state = atoi((char*)res);
				xmlFree(res);
			}
			if(xmlHasProp(mnode , BAD_CAST "client-type")){
				res = xmlGetProp(mnode , BAD_CAST "client-type");
				strcpy(member->clientType , (char*)res);
				xmlFree(res);
			}
	    		mnode = mnode->next;	    
		}

		node = node->next;
	}
	xmlFreeDoc(doc);
}

void pg_group_update_group_info(User *user , PGGroup *pg)
{
	FetionSip* sip = user->sip;
	SipHeader* eheader;
	char *res , *body;
	char *sid;
	extern int callid;
	PGGroupMember *memcur;

	if(pg == NULL || pg_group_get_member_count(pg) == 0)
		return;
	pg->hasDetails = 1;
	foreach_pg_member(pg->member , memcur){
		
		fetion_sip_set_type(sip , SIP_SERVICE);
		eheader = fetion_sip_event_header_new(SIP_EVENT_GETCONTACTINFO);
		fetion_sip_add_header(sip , eheader);
		memcur->getContactInfoCallId = callid;
		sid = fetion_sip_get_sid_by_sipuri(memcur->sipuri);
		body = generate_contact_info_by_no_body(sid);
		free(sid);
		res = fetion_sip_to_string(sip , body);
		free(body);
		tcp_connection_send(sip->tcp , res , strlen(res));
		free(res); res = NULL; 
	}
}

int pg_group_get_member_count(PGGroup *pg)
{
	int count = 0;
	PGGroupMember *memcur;
	foreach_pg_member(pg->member , memcur){
		count ++;
	}
	return count;
}
Contact* pg_group_parse_contact_info(const char* xml)
{
	Contact* contact;
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	char *pos;
	contact = fetion_contact_new();
	doc = xmlParseMemory(xml , strlen(xml));
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
		int n;
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

void pg_group_send_invitation(User *user , PGGroup *pg)
{
	FetionSip *sip = user->sip;
	SipHeader *theader;
	SipHeader *kheader1;
	SipHeader *kheader2;
	SipHeader *kheader3;
	SipHeader *kheader4;
	SipHeader *kheader5;
	extern int callid;
	const char *body = "s=session m=message";
	char *res;
	
	fetion_sip_set_type(sip , SIP_INVITATION);
	theader = fetion_sip_header_new("T" , pg->pguri);
	kheader1 = fetion_sip_header_new("K" , "text/html-fragment");
	kheader2 = fetion_sip_header_new("K" , "multiparty");
	kheader3 = fetion_sip_header_new("K" , "nudge");
	kheader4 = fetion_sip_header_new("K" , "share-background");
	kheader5 = fetion_sip_header_new("K" , "fetion-show");

	pg->inviteCallId = callid;	

	fetion_sip_add_header(sip , theader);
	fetion_sip_add_header(sip , kheader1);
	fetion_sip_add_header(sip , kheader2);
	fetion_sip_add_header(sip , kheader3);
	fetion_sip_add_header(sip , kheader4);
	fetion_sip_add_header(sip , kheader5);

	res = fetion_sip_to_string(sip , body);

	tcp_connection_send(sip->tcp , res , strlen(res));

}

void pg_group_send_invite_ack(User *user , const char *sipmsg)
{
	FetionSip *sip = user->sip;
	SipHeader *theader;
	char callid[16];
	char touri[64];
	char *res;

	memset(callid , 0 , sizeof(callid));
	memset(touri , 0 , sizeof(touri));
	fetion_sip_get_attr(sipmsg , "I" , callid);
	fetion_sip_get_attr(sipmsg , "T" , touri);

	fetion_sip_set_type(sip , SIP_ACKNOWLEDGE);
	theader = fetion_sip_header_new("T" , touri);
	fetion_sip_set_callid(sip , atoi(callid));
	fetion_sip_add_header(sip , theader);

	res = fetion_sip_to_string(sip , NULL);

	tcp_connection_send(sip->tcp , res , strlen(res));

	free(res);

}

void pg_group_send_message(User *user , PGGroup *pg , const char *message)
{
	FetionSip *sip = user->sip;
	SipHeader *theader;
	SipHeader *cheader;
	SipHeader *kheader;
	char *res;


	fetion_sip_set_type(sip , SIP_MESSAGE);
	theader = fetion_sip_header_new("T" , pg->pguri);
	cheader = fetion_sip_header_new("C" , "text/html-fragment");
	kheader = fetion_sip_header_new("K" , "SaveHistory");

	fetion_sip_add_header(sip , theader);
	fetion_sip_add_header(sip , cheader);
	fetion_sip_add_header(sip , kheader);

	fetion_sip_set_callid(sip , pg->inviteCallId);

	res = fetion_sip_to_string(sip , message);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);

}

void pg_group_send_sms(User *user , PGGroup *pg , const char *message)
{
	FetionSip *sip = user->sip;
	SipHeader *theader;
	SipHeader *eheader;
	char *res;


	fetion_sip_set_type(sip , SIP_MESSAGE);
	theader = fetion_sip_header_new("T" , pg->pguri);
	eheader = fetion_sip_event_header_new(SIP_EVENT_PGSENDCATSMS);

	fetion_sip_add_header(sip , theader);
	fetion_sip_add_header(sip , eheader);

	res = fetion_sip_to_string(sip , message);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
}

static char* generate_get_members_body(const char *pguri)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	char body[] = "<args></args>";
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "groups" , NULL);
	xmlNewProp(node , BAD_CAST "attributes" , BAD_CAST "member-uri;member-nickname;member-iicnickname;member-identity;member-t6svcid");
	node = xmlNewChild(node , NULL , BAD_CAST "group" , NULL);
	xmlNewProp(node , BAD_CAST "uri" , BAD_CAST pguri);
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return xml_convert(buf);
}

static char* generate_contact_info_by_no_body(const char* no)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	char uri[32];
	char body[] = "<args></args>";
	memset(uri, 0, sizeof(uri));
	sprintf(uri , "sip:%s" , no);
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);
	xmlNewProp(node , BAD_CAST "uri" , BAD_CAST uri);
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return xml_convert(buf);
}
