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


User* fetion_user_new(const char* no , const char* password)
{
	User* user = (User*)malloc(sizeof(User));
	memset(user , 0 , sizeof(User));
	if(strlen(no) == 11)
	{
		strcpy(user->mobileno , no);
		user->loginType = 1;
	}
	else
	{
		strcpy(user->sId , no);
		user->loginType = 2;
	}
	strcpy(user->password , password);
	strcpy(user->personalVersion , "0");
	strcpy(user->contactVersion , "0");
	strcpy(user->configServersVersion , "0");
	strcpy(user->configHintsVersion , "0");
	strcpy(user->customConfigVersion , "0");
	user->groupList = NULL;
	user->contactList = NULL;
	user->sip = NULL;
	user->verification = NULL;
	user->customConfig = NULL;
	user->ssic = NULL;
	user->config = NULL;
	return user;
}

void fetion_user_set_userid(User* user , const char* userid1)
{
	strcpy(user->userId , userid1);
}

void fetion_user_set_sid(User* user , const char* sId1)
{
	strcpy(user->sId , sId1);
}

void fetion_user_set_mobileno(User* user , const char* mobileno1)
{
	strcpy(user->mobileno , mobileno1);
}
void fetion_user_set_sip(User* user , FetionSip* sip1)
{
	debug_info("Set a initialized Sip Struct to User");
	user->sip = sip1;
}
void fetion_user_set_config(User* user , Config* config1)
{
	debug_info("Set a initialized Config Struct to User");
	user->config = config1;
}
void fetion_user_set_verification_code(User* user , const char* code)
{
	user->verification->code = (char*)malloc(12);
	memset(user->verification->code , 0 , 12);
	strcpy(user->verification->code , code);
}
void fetion_user_free(User* user)
{
	if(user->ssic != NULL)
		free(user->ssic);
	if(user->customConfig != NULL)
		free(user->customConfig);
	if(user->verification != NULL)
		free(user->verification);
	free(user);
}
void fetion_user_save(User* user)
{
#if 0
	Config* config = user->config;
	char filename[] = "personal.dat";
	char* filepath;
	FILE* file;
	int n;
	n = strlen(config->userPath) + strlen(filename) + 2;
	filepath = (char*)malloc(n);
	bzero(filepath , n);
	strcpy(filepath , config->userPath);
	strcat(filepath , "/");
	strcat(filepath , filename);
	file = fopen(filepath , "wb+");
	debug_info("Storing user information to local disk");
	fwrite(user , sizeof(User) , 1 , file);
	fclose(file);
	free(filepath);
	fetion_contact_save(user);
	fetion_buddylist_save(user);
#endif
}
void fetion_user_load(User* user)
{
#if 0
	debug_info("Storing some pointers to memory");
	Config* config = user->config;
	Verification* vtmp = user->verification;
	char *ssictmp = user->ssic;
	char *customtmp = user->customConfig;
	Contact* cltmp = user->contactList;
	Group* gltmp = user->groupList;
	FetionSip* fstmp = user->sip;
	char filename[] = "personal.dat";
	char* filepath;
	User fu;
	FILE* file;
	int n;
	n = strlen(config->userPath) + strlen(filename) + 2;
	filepath = (char*)malloc(n);
	bzero(filepath , n);
	strcpy(filepath , config->userPath);
	strcat(filepath , "/");
	strcat(filepath , filename);
	file = fopen(filepath , "r");
	debug_info("Reading user information from local disk");
	if(file == NULL)
	{
		debug_info("Reading user information from local disk failed!");
		return;
	}
	fread(&fu , sizeof(User) , 1 , file);
	memcpy(user , &fu , sizeof(User));
	fclose(file);

	debug_info("Reading pointers from memory to struct User");
	user->verification = vtmp;
	user->contactList = cltmp;
	user->ssic = ssictmp;
	user->groupList = gltmp;
	user->sip = fstmp;
	user->customConfig = customtmp;
	user->config = config;
	fetion_contact_load(user);
	fetion_buddylist_load(user);
	free(filepath);
#endif
}
int fetion_user_set_state(User* user , StateType state)
{
	SipHeader* eheader;
	FetionSip* sip = user->sip;
	char* body;
	char* res;

	fetion_sip_set_type(sip , SIP_SERVICE);
	eheader = fetion_sip_event_header_new(SIP_EVENT_SETPRESENCE);
	fetion_sip_add_header(sip , eheader);
	body = generate_set_state_body(state);
	res = fetion_sip_to_string(sip , body);
	tcp_connection_send(sip->tcp , res , strlen(res));
	user->state = state;
	free(body);
	free(res);
	debug_info("User state changed to %d" , state);
	return 1;
}
int fetion_user_set_moodphrase(User* user , const char* moodphrase)
{
	FetionSip* sip = user->sip;
	SipHeader* eheader;
	char *res , *body;
	int ret;
	fetion_sip_set_type(sip , SIP_SERVICE);
	debug_info("Start seting moodphrase");
	eheader = fetion_sip_event_header_new(SIP_EVENT_SETUSERINFO);
	fetion_sip_add_header(sip , eheader);
	body = generate_set_moodphrase_body(user->customConfigVersion
									  , user->customConfig
									  , user->personalVersion
									  , moodphrase);
	res = fetion_sip_to_string(sip , body);
	free(body);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res) ; 
	res = fetion_sip_get_response(sip);
	ret = fetion_sip_get_code(res);
	if(ret == 200)
	{
		parse_set_moodphrase_response(user , res);
		free(res);
		debug_info("Set moodphrase success");
		return 1;
	}
	else
	{
		free(res);
		debug_error("Set moodphrase failed , errno :" , ret);
		return -1;
	}

}
int fetion_user_update_info(User* user)
{
	FetionSip* sip = user->sip;
	SipHeader* eheader = NULL;
	char *res , *body;
	int ret;
	fetion_sip_set_type(sip , SIP_SERVICE);
	debug_info("Start Updating User Information");
	eheader = fetion_sip_event_header_new(SIP_EVENT_SETUSERINFO);
	fetion_sip_add_header(sip , eheader);
	body = generate_update_information_body(user);
	res = fetion_sip_to_string(sip , body);
	free(body);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res) ; 
	res = fetion_sip_get_response(sip);
	ret = fetion_sip_get_code(res);
	if(ret == 200)
	{
		free(res);
		debug_info("Update information success");
		return 1;
	}
	else
	{
		free(res);
		debug_error("Update information failed , errno :" , ret);
		return -1;
	}
}
int fetion_user_keep_alive(User* user)
{
	FetionSip* sip = user->sip;
	SipHeader* eheader = NULL;
	char *res = NULL , *body = NULL;
	fetion_sip_set_type(sip , SIP_REGISTER);
	debug_info("send a keep alive request");
	eheader = fetion_sip_event_header_new(SIP_EVENT_KEEPALIVE);
	fetion_sip_add_header(sip , eheader);
	body = generate_keep_alive_body();
	res = fetion_sip_to_string(sip , body);
	free(body);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res); 
	return 1;
}
Group* fetion_group_new()
{
	Group* list = (Group*)malloc(sizeof(Group));
	memset(list , 0 , sizeof(Group));
	list->preNode = NULL;
	list->nextNode = NULL;
	return list;
}
void fetion_group_list_append(Group* grouplist , Group* group)
{
	Group* pos = grouplist;
	if(grouplist == NULL)
	{
		debug_error("Try to append a group to a NULL grouplist(fetion_group_list_append)");
		return;
	}
	while(1)
	{
		if(pos->nextNode == NULL)
		{
			pos->nextNode = group;
			group->preNode = pos;
			break;
		}
		pos = pos->nextNode;
	}
}
void fetion_group_remove(Group** grouplist , int groupid)
{
	Group* pos = *grouplist;
	while(1)
	{
		if(pos->groupid == groupid)
		{
			if(pos == *grouplist)
			{
				*grouplist = pos->nextNode;
				(*grouplist)->preNode = NULL;
			}
			else
			{
				pos->preNode->nextNode = pos->nextNode;
				if(pos->nextNode != NULL)
					pos->nextNode->preNode = pos->preNode;
				free(pos);
			}
			return;
		}
		pos = pos->nextNode;
	}
}
Group* fetion_group_list_find_by_id(Group* grouplist , int id)
{
	Group* pos = grouplist;
	while(pos != NULL)
	{
		if(pos->groupid == id)
			return pos;
		pos = pos->nextNode;
	}
	return NULL;
}
Verification* fetion_verification_new()
{
	Verification* ver = (Verification*)malloc(sizeof(Verification));
	memset(ver , 0 , sizeof(Verification));
	ver->algorithm = NULL;
	ver->type = NULL;
	ver->text = NULL;
	ver->tips = NULL;
	return ver;
}
void fetion_verification_free(Verification* ver)
{
	if(ver != NULL)
	{
		free(ver->algorithm);
		free(ver->type);
		free(ver->text);
		free(ver->tips);
	}
	free(ver);
}

int fetion_user_upload_portrait(User* user , const char* filename)
{
	char http[1024];
	unsigned char buf[1024];
	char res[1024];
	char code[4];
	char* ip = NULL;
	FILE* f = NULL;
	char* server = user->config->portraitServerName;
	char* portraitPath = user->config->portraitServerPath;
	long filelength;
	int n;
	FetionConnection* tcp;

	ip = get_ip_by_name(server);
	if(ip == NULL)
	{
		debug_error("Parse server ip address failed , %s" , server);
		return -1;
	}

	f = fopen(filename , "r");
	fseek(f , 0 , SEEK_END);
	filelength = ftell(f);
	rewind(f);
	debug_info("uploading portrait....");
	bzero(http , sizeof(http));
	sprintf(http , "POST /%s/setportrait.aspx HTTP/1.1\r\n"
		    	   "User-Agent: IIC2.0/PC 3.6.1900\r\n"
		    	   "Content-Type: image/jpeg\r\n"
		    	   "Host: %s\r\n"
		    	   "Cookie: ssic=%s\r\n"
		    	   "Content-Length: %ld\r\n"
				   "Connection: Keep-Alive\r\n\r\n"
		  		  , portraitPath , server , user->ssic , filelength);

	tcp = tcp_connection_new();
	tcp_connection_connect(tcp , ip , 80);

	tcp_connection_send(tcp , http , strlen(http));

	memset(buf , 0 , sizeof(buf));
	while((n = fread(buf , 1 , sizeof(buf) , f)))
	{
		fflush(f);
		tcp_connection_send(tcp , buf , n) ;
		memset(buf , 0 , sizeof(buf));
	}
	fclose(f);

	bzero(res , sizeof(res));
	tcp_connection_recv(tcp , res , sizeof(res));
	bzero(code , sizeof(code));
	strncpy(code , res + 9 , 3);
	if(strcmp(code , "200") == 0)
	{
		debug_info("Upload portrait success");
		return 1;
	}
	else
	{
		debug_error("Upload portrait failed");
		return -1;
	}
}

int fetion_user_download_portrait(User* user , const char* sipuri)
{
	char buf[2048] = { 0 };
	char* ip = NULL;
	char* pos = NULL;
	FILE* f = NULL;
	char filename[100] = { 0 };
	char* server = user->config->portraitServerName;
	char* portraitPath = user->config->portraitServerPath;
	char *encodedSipuri , *encodedSsic , replyCode[4] = { 0 };
	char *friendSid = NULL;
	FetionConnection* tcp = NULL;
	int i = 0 , isFirstChunk = 0 , chunkLength = 0 , imageLength = 0 , receivedLength = 0;
	ip = get_ip_by_name(server);
	if(ip == NULL)
	{
		debug_error("Parse server ip address failed , %s" , server);
		return -1;
	}
	friendSid = fetion_sip_get_sid_by_sipuri(sipuri);
	if(friendSid == NULL)
		return -1;
/*	open a file to write ,if not exist then create one*/
	sprintf(filename , "%s/%s.jpg" , user->config->iconPath ,  friendSid);
	free(friendSid);
	encodedSipuri = http_connection_encode_url(sipuri);
	encodedSsic = http_connection_encode_url(user->ssic);
	sprintf(buf , "GET /%s/getportrait.aspx?Uri=%s"
				  "&Size=120&c=%s HTTP/1.1\r\n"
				  "User-Agent: IIC2.0/PC 3.6.1900\r\n"
				  "Accept: image/pjpeg;image/jpeg;image/bmp;"
				  "image/x-windows-bmp;image/png;image/gif\r\n"
				  "Host: %s\r\nConnection: Keep-Alive\r\n\r\n"
				  , portraitPath , encodedSipuri , encodedSsic , server);
	tcp = tcp_connection_new();
	tcp_connection_connect(tcp , ip , 80);
	free(ip);
	tcp_connection_send(tcp , buf , strlen(buf));	
	memset( buf , 0 , sizeof(buf)); 
	//read reply

	/* 200 OK begin to download protrait ,
	 * 302 need to redirect ,404 not found */
	while((chunkLength = tcp_connection_recv(tcp , buf , sizeof(buf) -1)) > 0 )
	{
		buf[chunkLength] = '\0';
		if(isFirstChunk == 0)
		{	
			/* check the code num for the first segment*/
			memcpy(replyCode , buf + 9 , 3);
			switch(atoi(replyCode))
			{   
				/*	no protrait for current user found
				 * ,just return a error */
				case 404:
					goto end;
					break;
				/*write the image bytes of the first segment into file*/
				case 200:
					f = fopen(filename , "wb+");
					if( f == NULL )
					{
						debug_error("Write user portrait to local disk failed");
						return -1;
					}
					pos = (char*)buf;
					imageLength = http_connection_get_body_length(pos);
					receivedLength = chunkLength - http_connection_get_head_length(pos) -4;
					for(i = 0 ; i < chunkLength ; i++ )
						if( buf[i] == '\r' && buf[i+1] == '\n'
							&&buf[i+2] == '\r' && buf[i+3] == '\n' )
						{
							fwrite(buf + i + 4 , chunkLength - i -4 , 1 ,f);
							fflush(f);
							break;
						}
					if(receivedLength == imageLength)
						goto end;
					break;
				default:
					goto redirect;
					break;
			};
			isFirstChunk ++;
		}
		else
		{
			if(strcmp(replyCode , "200") == 0){
				fwrite(buf , chunkLength , 1 , f);
				fflush(f);
			}
			receivedLength += chunkLength;
			if(receivedLength == imageLength)
				break;
		}
	}
	if(strcmp(replyCode , "200") == 0)
	{
		fclose(f);
		tcp_connection_free(tcp);
		tcp = NULL;
		return 0;
	}
redirect:
	if(strcmp(replyCode , "302") == 0)
		fetion_user_download_portrait_again(filename , buf);
end:
	if(f != NULL)
		fclose(f);
	tcp_connection_free(tcp);
	tcp = NULL;
	return 0;
}
int fetion_user_download_portrait_again(const char* filepath , const char* buf)
{
	char location[512] = { 0 };
	char httpHost[50] = { 0 };
	char httpPath[512] = { 0 };
	char http[1024] = { 0 };
	char replyCode[5] = { 0 };
	FILE* f;
	FetionConnection* tcp;
	char* ip = NULL;
	char* pos = strstr(buf , "Location: ") ;
	int chunkLength = 0 , imageLength = 0 , receivedLength = 0;
	int i , n = 0;
	
	int isFirstChunk = 0;
	unsigned char img[2048] = { 0 };

	if(pos == NULL)
		return -1;
	pos += 10;
	n = strlen(pos) - strlen(strstr(pos , "\r\n"));
	strncpy(location , pos , n );
	pos = location + 7;
	n = strlen(pos) - strlen(strstr(pos , "/"));
	strncpy(httpHost , pos , n);
	pos += n;
	strcpy(httpPath , pos);
	sprintf(http , "GET %s HTTP/1.1\r\n"
				   "User-Agent: IIC2.0/PC 3.3.0370\r\n"
			 	   "Accept: image/pjpeg;image/jpeg;image/bmp;image/x-windows-bmp;image/png;image/gif\r\n"
				   "Cache-Control: private\r\n"
				   "Host: %s\r\n"
				   "Connection: Keep-Alive\r\n\r\n" , httpPath , httpHost);
	ip = get_ip_by_name(httpHost);
	if(ip == NULL)
	{
		debug_error("Parse portrait server ip address failed , %s" , httpHost);
		return -1;
	}
	tcp = tcp_connection_new();
	tcp_connection_connect(tcp , ip , 80);
	free(ip);
	tcp_connection_send(tcp , http , strlen(http));
	//read portrait data
	while((chunkLength = tcp_connection_recv(tcp , img , sizeof(img)-1)) > 0)
	{
		img[chunkLength] = '\0';
		if(isFirstChunk ++ == 0)
		{
			char* pos = (char*)(img);
			strncpy(replyCode , pos + 9 , 3 );
			if(strcmp(replyCode , "404") == 0)
				goto end;
			f = fopen(filepath , "wb+");
			imageLength = http_connection_get_body_length(pos);
			receivedLength = chunkLength - http_connection_get_head_length(pos) - 4;
			for(i = 0 ; i < chunkLength ; i ++)
				if( img[i] == '\r' && img[i+1] == '\n'
					&&img[i+2] == '\r' && img[i+3] == '\n' )
				{
					fwrite(img + i +4 , chunkLength - i - 4 , 1 ,f);
					fflush(f);
					break;
				}
			if(receivedLength == imageLength)
				goto end;
		}
		else
		{
			fwrite(img , chunkLength , 1 , f);
			fflush(f);
			receivedLength += chunkLength;
			if(receivedLength == imageLength)
				break;
		}
		memset(img , 0 , sizeof(img));
	}
	fclose(f);
end:
	tcp_connection_free(tcp);
	tcp = NULL;
	return 0;
}
Contact* fetion_user_parse_presence_body(const char* body , User* user)
{
	xmlDocPtr doc;
	xmlNodePtr node , cnode;
	xmlChar* pos;
	Contact* contact = NULL;
	Contact* contactres = NULL;
	Contact* contactlist = user->contactList;
	Contact* currentContact;

	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xml_goto_node(node , "c");
	while(node != NULL)
	{
		pos = xmlGetProp(node , BAD_CAST "id");
		currentContact = fetion_contact_list_find_by_userid(contactlist , (char*)pos);
		if(currentContact == NULL)
		{
			/*not a valid information*/
			/*debug_error("User %s is not a valid user" , (char*)pos);*/
			node = node->next;
			continue;
		}
		cnode = node->xmlChildrenNode;
		if(xmlHasProp(cnode , BAD_CAST "sid"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "sid");
			strcpy(currentContact->sId ,  (char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "m"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "m");
			strcpy(currentContact->mobileno ,  (char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "l"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "l");
			currentContact->scoreLevel = atoi((char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "n"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "n");
			strcpy(currentContact->nickname ,  (char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "i"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "i");
			strcpy(currentContact->impression ,  (char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "p"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "p");
			strcpy(currentContact->portraitCrc ,  (char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "sms"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "sms");
			if(strstr((char*)pos , "365") != NULL)
				currentContact->serviceStatus = STATUS_OFFLINE;
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "cs"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "cs");
			if(atoi((char*)pos) == 2)
				currentContact->serviceStatus = STATUS_SERVICE_CLOSED;
			xmlFree(pos);
		}
		cnode = xml_goto_node(node , "pr");
		if(xmlHasProp(cnode , BAD_CAST "dt"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "dt");
			strcpy(currentContact->devicetype ,  strlen((char*)pos) == 0 ? "PC" : (char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "b"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "b");
			currentContact->state = atoi((char*)pos);
			xmlFree(pos);
		}
		contact = fetion_contact_new();
		memset(contact , 0 , sizeof(contact));
		memcpy(contact , currentContact , sizeof(Contact));
		contact->preNode = NULL;
		contact->nextNode = NULL;
		if(contactres == NULL)
			contactres = contact;
		else
			fetion_contact_list_append(contactres , contact);
		node = node->next;
	}
	xmlFreeDoc(doc);
	return contactres;
}
Contact* fetion_user_parse_syncuserinfo_body(const char* body , User* user)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar* pos;
	Contact* contactlist = user->contactList;
	Contact* currentContact;

	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xml_goto_node(node , "buddy");

	pos = xmlGetProp(node , BAD_CAST "user-id");
	currentContact = fetion_contact_list_find_by_userid(contactlist , (char*)pos);
	debug_info("synchronize user information");
	if(currentContact == NULL)
	{
		/*not a valid information*/
		debug_error("User %s is not a valid user" , (char*)pos);
		return NULL;
	}
	if(xmlHasProp(node , BAD_CAST "uri"))
	{
		pos = xmlGetProp(node , BAD_CAST "uri");
		strcpy(currentContact->sipuri ,  (char*)pos);
		xmlFree(pos);
	}
	if(xmlHasProp(node , BAD_CAST "relation-status"))
	{
		pos = xmlGetProp(node , BAD_CAST "relation-status");
		if(atoi((char*)pos) == 1)
		{
			debug_info("User %s accepted your request" , currentContact->userId);
			currentContact->serviceStatus = STATUS_NORMAL;
		}
		else
		{
			debug_info("User %s refused your request" , currentContact->userId);
			currentContact->serviceStatus = STATUS_REJECTED;
		}
		xmlFree(pos);
	}
	xmlFreeDoc(doc);
	return currentContact;
}
char* generate_set_state_body(StateType state)	
{
	char s[5];
	char data[] = "<args></args>";
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(data , strlen(data));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "presence" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "basic" , NULL);
	bzero(s , sizeof(s));
	sprintf(s , "%d" , state);
	xmlNewProp(node , BAD_CAST "value" , BAD_CAST s);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return xml_convert(res);
}
char* generate_set_moodphrase_body(const char* customConfigVersion , const char* customConfig , const char* personalVersion ,  const char* moodphrase)
{
	char args[] = "<args></args>";
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node , cnode;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "userinfo" , NULL);
	cnode = xmlNewChild(node , NULL , BAD_CAST "personal" , NULL);
	xmlNewProp(cnode , BAD_CAST "impresa" , BAD_CAST moodphrase);
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST personalVersion);
	cnode = xmlNewChild(node , NULL , BAD_CAST "custom-config" , BAD_CAST customConfig);
	xmlNewProp(cnode , BAD_CAST "type" , BAD_CAST "PC");
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST customConfigVersion);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return xml_convert(res);
}
char* generate_update_information_body(User* user)
{
	char args[] = "<args></args>";
	char gender[5];
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node , cnode;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "userinfo" , NULL);
	cnode = xmlNewChild(node , NULL , BAD_CAST "personal" , NULL);
	xmlNewProp(cnode , BAD_CAST "impresa" , BAD_CAST user->impression);
	xmlNewProp(cnode , BAD_CAST "nickname" , BAD_CAST user->nickname);
	bzero(gender , sizeof(gender));
	sprintf(gender , "%d" , user->gender);
	xmlNewProp(cnode , BAD_CAST "gender" , BAD_CAST gender);
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST "0");
	cnode = xmlNewChild(node , NULL , BAD_CAST "custom-config" , BAD_CAST user->customConfig);
	xmlNewProp(cnode , BAD_CAST "type" , BAD_CAST "PC");
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST user->customConfigVersion);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return xml_convert(res);
}
char* generate_keep_alive_body()
{
	char args[] = "<args></args>";
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "credentials" , NULL);
	xmlNewProp(node , BAD_CAST "domains" , BAD_CAST "fetion.com.cn");
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return xml_convert(res);
}
void parse_set_moodphrase_response(User* user , const char* sipmsg)
{
	char *pos;
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode->xmlChildrenNode;
	res = xmlGetProp(node , BAD_CAST "version");
	bzero(user->personalVersion , sizeof(user->personalVersion));
	strcpy(user->personalVersion , (char*)res);
	xmlFree(res);
	res = xmlGetProp(node , BAD_CAST "impresa");
	bzero(user->impression , sizeof(user->impression));
	strcpy(user->impression , (char*)res);
	xmlFree(res);
	node = node->next;
	res = xmlGetProp(node , BAD_CAST "version");
	bzero(user->customConfigVersion , sizeof(user->customConfigVersion));
	strcpy(user->customConfigVersion , (char*)res);	
	xmlFree(res);
	res = xmlNodeGetContent(node);
	free(user->customConfig);
	user->customConfig = (char*)malloc(strlen((char*)res) + 1);
	bzero(user->customConfig , strlen((char*)res) + 1);
	strcpy(user->customConfig , (char*)res);
	xmlFree(res);
	xmlFreeDoc(doc);
}