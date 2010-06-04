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

Config* fetion_config_new()
{
	char* homepath = NULL;
	Config* config = NULL;
	int n , e;
	DIR *dir = NULL;

	homepath = getenv("HOME");
	config = (Config*)malloc(sizeof(Config));
	memset(config , 0 , sizeof(Config));

	sprintf(config->globalPath , "%s/.openfetion" , homepath);
	dir = opendir(config->globalPath);
	if(dir == NULL){
		e = mkdir(config->globalPath , S_IRWXU|S_IRWXO|S_IRWXG);
		if(e == -1){
			debug_error("Create directory: %s failed" , config->globalPath);
			return NULL;
		}
	}
	config->ul = NULL;
	config->iconSize = 25;
	return config;
}

FxList* fetion_config_get_phrase(Config* config)
{
	
	char path[256] ;
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar *res;
	FxList *list;
	FxList *pos;
	Phrase *phrase;
	sprintf(path , "%s/configuration.xml" , config->userPath);
	
	doc = xmlParseFile(path);
	node = xmlDocGetRootElement(doc);
	node = xml_goto_node(node , "addbuddy-phrases");
	node = node->xmlChildrenNode;
	list = fx_list_new(NULL);
	while(node != NULL)
	{
		phrase = (Phrase*)malloc(sizeof(Phrase));
		res = xmlNodeGetContent(node);
		phrase->content = (char*)res;
		res = xmlGetProp(node , BAD_CAST "id");
		phrase->phraseid = atoi((char*)res);
		pos = fx_list_new(phrase);
		fx_list_append(list , pos);
		node = node->next;
	}
	return list;
}
void fetion_phrase_free(Phrase* phrase)
{
	if(phrase != NULL)
		free(phrase->content);
	free(phrase);
}
struct userlist* fetion_user_list_new(const char* no , const char* password
		, int laststate , int islastuser)
{
	struct userlist* ul;

	ul = (struct userlist*)malloc(sizeof(struct userlist));
	memset(ul , 0 , sizeof(struct userlist));
	if(no != NULL)
		strcpy(ul->no , no);
	if(password != NULL)
		strcpy(ul->password , password);
	ul->laststate = laststate;
	ul->islastuser = islastuser;
	ul->next = ul;
	ul->pre = ul;
	return ul;
}
void fetion_user_list_append(struct userlist* head , struct userlist* ul)
{
	head->next->pre = ul;
	ul->next = head->next;
	ul->pre = head;
	head->next = ul;
}
void fetion_user_list_save(Config* config , struct userlist* ul)
{	
	char path[256];
	FILE* file;
	struct userlist* pos;
	bzero(path , sizeof(path));
	sprintf(path , "%s/global.dat" , config->globalPath);
	file = fopen(path , "wb+");
	foreach_userlist(ul , pos){
		fwrite(pos , sizeof(struct userlist) , 1 , file);
	}
	fclose(file);
}

void fetion_user_list_set_lastuser_by_no(struct userlist* ul , const char* no)
{
	struct userlist* pos;
	foreach_userlist(ul , pos){
		if(strcmp(pos->no , no) == 0)
			pos->islastuser = 1;
		else
			pos->islastuser = 0;
	}
}
struct userlist* fetion_user_list_find_by_no(struct userlist* list , const char* no)
{
	struct userlist* pos;
	foreach_userlist(list , pos){
		if(strcmp(pos->no , no) == 0)
			return pos;
	}
	return NULL;
}

struct userlist* fetion_user_list_load(Config* config)
{
	char path[256];
	FILE* file;
	struct userlist *res = NULL , *pos;
	int n;

	res = fetion_user_list_new(NULL , NULL , 0 , 0);

	bzero(path , sizeof(path));
	sprintf(path , "%s/global.dat" , config->globalPath);

	file = fopen(path , "rb");
	if(file == NULL)
		return res;
	debug_info("Loading user list store in local data file");
	while(!feof(file)){
		pos = (struct userlist*)malloc(sizeof(struct userlist));
		n = fread(pos , sizeof(struct userlist) , 1 , file);
		pos->next = NULL;
		if(n > 0 ){
			fetion_user_list_append(res , pos);
		}else{
			free(pos);
			break;
		}
	}
	fclose(file);
	return res;
}
void fetion_config_download_configuration(User* user)
{
	char http[1025] , path[256] , *body , *res;
	FetionConnection* conn = NULL;
	Config *config = user->config;
	char uri[] = "nav.fetion.com.cn";
	char* ip;
	sprintf(path , "%s/configuration.xml" , user->config->userPath);
	ip = get_ip_by_name(uri);
	if(ip == NULL)
	{
		debug_error("Parse configuration uri (%s) failed!!!");
		return;
	}
	conn = tcp_connection_new();
	if(config->proxy != NULL && config->proxy->proxyEnabled)
		tcp_connection_connect_with_proxy(conn , ip , 80 , config->proxy);
	else
		tcp_connection_connect(conn , ip , 80);
	body = generate_configuration_body(user);
	sprintf(http , "POST /nav/getsystemconfig.aspx HTTP/1.1\r\n"
				   "User-Agent: IIC2.0/PC 3.6.1900\r\n"
				   "Host: %s\r\n"
				   "Connection: Close\r\n"
				   "Content-Length: %d\r\n\r\n%s"
				 , uri , strlen(body) , body);
	printf("%s\n" , http);
	tcp_connection_send(conn , http , strlen(http));
	res = http_connection_get_response(conn);
	refresh_configuration_xml(res , path , user);
	free(res);
}
int fetion_config_initialize(Config* config , const char* userid)
{

	DIR *userdir , *icondir;

	sprintf(config->userPath , "%s/%s" , config->globalPath , userid);
	sprintf(config->iconPath , "%s/icons" , config->userPath );

	userdir = opendir(config->userPath);

	if(userdir == NULL){
		int e = mkdir(config->userPath , S_IRWXU|S_IRWXO|S_IRWXG);
		if(e == -1){
			debug_error("Create directory: %s failed" , config->userPath);
			return e;
		}
	}
	icondir = opendir(config->iconPath);
	if(icondir == NULL){
		int e = mkdir(config->iconPath , S_IRWXU|S_IRWXO|S_IRWXG);
		if(e == -1){
			debug_error("Create directory: %s failed" , config->iconPath);
			return e;
		}
	}
	return 0;
}
int fetion_config_load_xml(User* user)
{
	Config* config = user->config;
	char configFilePath[256];
	char sipcIP[20] , sipcPort[6]; 
	char* pos;
	int n;
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlNodePtr cnode;

	bzero(configFilePath , sizeof(configFilePath));
	bzero(sipcIP , sizeof(sipcIP));
	bzero(sipcPort , sizeof(sipcPort));
	sprintf(configFilePath , "%s/configuration.xml" , config->userPath);

	doc = xmlParseFile(configFilePath);
	if(doc == NULL)
	{
		debug_error("Can not fild configuration file");
		return -1;
	}
	node = xmlDocGetRootElement(doc);
	cnode = xml_goto_node(node , "sipc-proxy");
	res = xmlNodeGetContent(cnode);
	n = strlen((char*)res) - strlen(strstr((char*)res , ":"));
	strncpy(config->sipcProxyIP , (char*)res , n);
	pos = strstr((char*)res , ":") + 1;
	config->sipcProxyPort = atoi(pos);
	xmlFree(res);
	cnode = xml_goto_node(node , "get-uri");
	res = xmlNodeGetContent(cnode);
	pos = strstr((char*)res , "//") + 2;
	n = strlen(pos) - strlen(strstr(pos , "/"));
	strncpy(config->portraitServerName , pos , n);
	pos = strstr(pos , "/") + 1;
	n = strlen(pos) - strlen(strstr(pos , "/"));
	strncpy(config->portraitServerPath , pos , n);
	xmlFree(res);
	return 1;
}
int fetion_config_load_data(User* user)
{
	char path[256];
	FILE *file;
	Config config;
	Config *cfg = user->config;

	bzero(path , sizeof(path));
	sprintf(path , "%s/config.dat" , cfg->userPath);

	file = fopen(path , "rb");

	if(file == NULL)
		return -1;

	debug_info("Loading config data file");
	fread(&config , sizeof(Config) , 1 , file);

	cfg->iconSize = config.iconSize;
	cfg->autoReply = config.autoReply;
	bzero(cfg->autoReplyMessage , sizeof(cfg->autoReplyMessage));
	strcpy(cfg->autoReplyMessage , config.autoReplyMessage);
	cfg->autoPopup = config.autoPopup;
	cfg->sendMode = config.sendMode;
	cfg->closeMode = config.closeMode;
	cfg->iconSize = config.iconSize;
	cfg->closeAlert = config.closeAlert;
	cfg->msgAlert = config.msgAlert;
	return 1;
}

Proxy* fetion_config_load_proxy()
{
	Proxy *proxy = (Proxy*)malloc(sizeof(Proxy));
	FILE *file = NULL;
	char path[1024];
	
	bzero(path , sizeof(path));

	sprintf(path , "%s/.openfetion/proxy.dat" , getenv("HOME"));

	file = fopen(path , "rb");

	if(file == NULL)
	{
		free(proxy);
		return NULL;
	}

	debug_info("Read proxy information");

	if(fread(proxy , sizeof(Proxy) , 1 , file) > 0)
	{
		fclose(file);
		return proxy;
	}
	free(proxy);
	fclose(file);
	return NULL;
}

void fetion_config_save_proxy(Proxy *proxy)
{
	FILE *file = NULL;
	char path[1024];
	
	bzero(path , sizeof(path));

	sprintf(path , "%s/.openfetion/proxy.dat" , getenv("HOME"));

	file = fopen(path , "wb+");

	fwrite(proxy , sizeof(Proxy) , 1 , file);

	fclose(file);
}

int fetion_config_save(User* user)
{
	char path[256];
	Config *config = user->config;
	FILE *file;

	bzero(path , sizeof(path));
	sprintf(path , "%s/config.dat" , config->userPath);

	file = fopen(path , "wb+");

	debug_info("Save configration file");
	fwrite(config , sizeof(Config) , 1 , file);

	fclose(file);
	
	return 1;
}

char* generate_configuration_body(User* user)
{
	xmlChar* buf;
	char *res , *pos;
	xmlDocPtr doc;
	xmlNodePtr node , cnode;
	char body[] = "<config></config>";
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	cnode = xmlNewChild(node , NULL , BAD_CAST "user" , NULL);
	if(user->loginType == LOGIN_TYPE_FETIONNO){
		xmlNewProp(cnode , BAD_CAST "sid" , BAD_CAST user->sId);
	}else{
		xmlNewProp(cnode , BAD_CAST "mobile-no" , BAD_CAST user->mobileno);
	}
	cnode = xmlNewChild(node , NULL , BAD_CAST "client" , NULL);
	xmlNewProp(cnode , BAD_CAST "type" , BAD_CAST "PC");
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST PROTO_VERSION);
	xmlNewProp(cnode , BAD_CAST "platform" , BAD_CAST "W5.1");
	cnode = xmlNewChild(node , NULL , BAD_CAST "servers" , NULL);
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST user->configServersVersion);
/*	cnode = xmlNewChild(node , NULL , BAD_CAST "service-no" , NULL);
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST "0");*/
	cnode = xmlNewChild(node , NULL , BAD_CAST "parameters" , NULL);
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST "0");
	cnode = xmlNewChild(node , NULL , BAD_CAST "hints" , NULL);
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST user->configServersVersion);
/*	cnode = xmlNewChild(node , NULL , BAD_CAST "http-applications" , NULL);
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST "0");
	cnode = xmlNewChild(node , NULL , BAD_CAST "client-config" , NULL);
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST "0");
	cnode = xmlNewChild(node , NULL , BAD_CAST "services" , NULL);
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST "0");*/
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);	
	return xml_convert(buf);
}
void refresh_configuration_xml(const char* xml , const char* xmlPath , User* user)
{
	FILE* xmlfd;
	xmlDocPtr olddoc , newdoc;
	xmlNodePtr oldnode , newnode , oldpos , newpos , newpos1;
	xmlChar* c;

	if(xml == NULL)
		return;
	xmlfd = fopen(xmlPath , "r");

	if(xmlfd == NULL)
	{
		xmlfd = fopen(xmlPath , "wb+");
		fwrite(xml , strlen(xml) , 1 , xmlfd);
		fclose(xmlfd);
	}
	else
	{
		fclose(xmlfd);
		olddoc = xmlParseFile(xmlPath);
		newdoc = xmlParseMemory(xml , strlen(xml));
		oldnode = xmlDocGetRootElement(olddoc);
		newnode = xmlDocGetRootElement(newdoc);
		newpos = xml_goto_node(newnode , "servers");
		if(newpos != NULL)
		{
			c = xmlGetProp(newpos , BAD_CAST "version");
			strcpy(user->configServersVersion , (char*)c);
			xmlFree(c);
			oldpos = xml_goto_node(oldnode , "servers");
			if(oldpos != NULL)
				oldpos->xmlChildrenNode = newpos->xmlChildrenNode;
			else
				xmlAddChild(oldnode , newpos);
		}
		newpos1 = xml_goto_node(newnode , "hints");
		if(newpos1 != NULL)
		{
			c = xmlGetProp(newpos1 , BAD_CAST "version");
			strcpy(user->configHintsVersion , (char*)c);
			xmlFree(c);
			oldpos = xml_goto_node(oldnode , "hints");
			if(oldpos != NULL)
				oldpos->xmlChildrenNode = newpos1->xmlChildrenNode;
			else
				xmlAddChild(oldnode , newpos1);
		}
		xmlSaveFile(xmlPath , olddoc);
	}
}

xmlNodePtr xml_goto_node(xmlNodePtr node , const char* name)
{
	xmlNodePtr pos = node;
	xmlNodePtr tmp = NULL;
	while(pos != NULL)
	{
		if(strcmp(name , (char*)pos->name) == 0)
			return pos;
		tmp = pos->xmlChildrenNode;
		if(tmp != NULL
		   &&tmp->type == XML_ELEMENT_NODE
		   && (tmp = xml_goto_node(tmp , name)) != NULL )
			return tmp;
		pos = pos->next;
	};
	return NULL;
}
char* xml_convert(xmlChar* in)
{
	char *res , *pos ;
	pos = strstr((char*)in , "?>") + 2;
	res = (char*)malloc(strlen(pos) + 1);
	memset(res , 0 , strlen(pos) + 1);
	memcpy(res , pos , strlen(pos));
	xmlFree(in);
	return res;
}
char* fetion_config_get_city_name(const char* province , const char* city)
{
	char path[] = RESOURCE_DIR"city.xml"; 
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseFile(path);
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	while(node != NULL)
	{
		if(node->type != XML_ELEMENT_NODE)
		{
			node = node->next;
			continue;
		}
		res = xmlGetProp(node , BAD_CAST "id");
		if(xmlStrcmp(res , BAD_CAST province) == 0)
		{
			node = node->xmlChildrenNode;
			while(node != NULL)
			{
				if(node->type != XML_ELEMENT_NODE)
				{
					node = node->next;
					continue;
				}
				xmlFree(res);
				res = xmlGetProp(node , BAD_CAST "id");
				if(xmlStrcmp(res , BAD_CAST city) == 0)
				{
					xmlFree(res);
					return (char*)xmlNodeGetContent(node);
					break;
				}
				node = node->next;
			}
			break;
		}
		xmlFree(res);
	
		node = node->next;
	}
	return NULL;
}

char* fetion_config_get_province_name(const char* province)
{
	char path[] = RESOURCE_DIR"province.xml"; 
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlReadFile(path , "UTF-8" , XML_PARSE_RECOVER);
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	while(node != NULL)
	{
		res = xmlGetProp(node , BAD_CAST "id");
		if(xmlStrcmp(res , BAD_CAST province) == 0)
		{
			return (char*)xmlNodeGetContent(node);
			xmlFree(res);
			break;
		}
		xmlFree(res);
		node = node->next;
	}
	xmlFreeDoc(doc);
	return NULL;
}
