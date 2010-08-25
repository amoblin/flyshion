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
#include <openssl/rsa.h>
#include <openssl/sha.h>

/*private method*/
static char* generate_auth_body(User* user);
static void parse_personal_info(xmlNodePtr node , User* user);
static void parse_contact_list(xmlNodePtr node , User* user);
static void parse_stranger_list(xmlNodePtr node , User* user);
static void parse_ssi_auth_success(xmlNodePtr node , User* user);
static void parse_ssi_auth_failed(xmlNodePtr node , User* user);
static unsigned char* strtohex(const char* in , int* len) ;
static char* hextostr(const unsigned char* in , int len) ;
static char* hash_password_v1(const unsigned char* b0 , int b0len , const unsigned char* password , int psdlen);
static char* hash_password_v2(const char* userid , const char* passwordhex);
static char* hash_password_v4(const char* userid , const char* password);
static char* generate_cnouce() ;
static unsigned char* decode_base64(const char* in , int* len);


char* generate_response(const char* nouce , const char* userid 
		, const char* password , const char* publickey , const char* key)
{
	char* psdhex = hash_password_v4(userid , password);
	char modulus[257];
	char exponent[7];
	int ret, flen;
	BIGNUM *bnn, *bne;
	unsigned char *out;
	unsigned char *nonce , *aeskey , *psd , *res;
	int nonce_len , aeskey_len , psd_len;
	RSA *r = RSA_new();

	DEBUG_FOOTPRINT();

	key = NULL;

	bzero(modulus , sizeof(modulus));
	bzero(exponent , sizeof(exponent));

	memcpy(modulus , publickey , 256);
	memcpy(exponent , publickey + 256 , 6);
	nonce = (unsigned char*)malloc(strlen(nouce) + 1);
	memset(nonce , 0 , strlen(nouce) + 1);
	memcpy(nonce , (unsigned char*)nouce , strlen(nouce));
	nonce_len = strlen(nouce);
	psd = strtohex(psdhex , &psd_len);
	aeskey = strtohex(generate_aes_key() , &aeskey_len);
	res = (unsigned char*)malloc(nonce_len + aeskey_len + psd_len + 1);
	memset(res , 0 , nonce_len + aeskey_len + psd_len + 1);
	memcpy(res , nonce , nonce_len);
	memcpy(res + nonce_len , psd , psd_len );
	memcpy(res + nonce_len + psd_len , aeskey , aeskey_len);

	bnn = BN_new();
	bne = BN_new();
	BN_hex2bn(&bnn, modulus);
	BN_hex2bn(&bne, exponent);
	r->n = bnn;	r->e = bne;	r->d = NULL;
	RSA_print_fp(stdout, r, 5);
	flen = RSA_size(r);
	out =  (unsigned char*)malloc(flen);
	memset(out , 0 , flen);
	debug_info("Start encrypting response");
	ret = RSA_public_encrypt(nonce_len + aeskey_len + psd_len, res , out, r, RSA_PKCS1_PADDING);
	if (ret < 0)
	{
		debug_info("Encrypt response failed!");
		return NULL;
	}
	RSA_free(r);
	debug_info("Encrypting reponse success");
	free(res); 
	free(aeskey);
	free(psd);
	free(nonce);
	return hextostr(out , ret);
}
void generate_pic_code(User* user)
{
	char buf[1024] , *res , *code;
	char codePath[128];
	char cookie[1024];
	char* ip;

	FILE* picfd;
	int piclen;
	unsigned char* pic;
	int n;

	xmlDocPtr doc;
	xmlNodePtr node;

	DEBUG_FOOTPRINT();

	bzero(buf , sizeof(buf));
	ip = get_ip_by_name(NAVIGATION_URI);
	FetionConnection* con = tcp_connection_new();
	tcp_connection_connect(con , ip , 80);
	bzero(cookie , sizeof(cookie));
	if(user->ssic){
		sprintf(cookie , "Cookie: ssic=%s\r\n" , user->ssic);
	}
	sprintf(buf , "GET /nav/GetPicCodeV4.aspx?algorithm=%s HTTP/1.1\r\n"
				  "%sHost: %s\r\n"
				  "User-Agent: IIC2.0/PC "PROTO_VERSION"\r\n"
				  "Connection: close\r\n\r\n"
				, user->verification->algorithm == NULL ? "" : user->verification->algorithm
				, user->ssic == NULL ? "" : cookie , NAVIGATION_URI);
	tcp_connection_send(con , buf , strlen(buf));
	res = http_connection_get_response(con);
	tcp_connection_free(con);
	doc = xmlParseMemory(res , strlen(res));
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	user->verification->guid = (char*)xmlGetProp(node , BAD_CAST "id");
	code = (char*)xmlGetProp(node , BAD_CAST "pic");
	xmlFreeDoc(doc);
	debug_info("Generating verification code picture");
	pic = decode_base64(code , &piclen);
	free(code);
	bzero(codePath , sizeof(codePath));
	sprintf(codePath , "%s/code.gif" , user->config->globalPath);
	picfd = fopen(codePath , "wb+");
	n = 0;
	for(; n != piclen ;){
		n += fwrite(pic + n , 1 , piclen - n , picfd);
	}
	fclose(picfd);
	free(res);
}
char* ssi_auth_action(User* user)
{
	char sslbuf[2048] = { 0 };
	const char ssiName[] = "uid.fetion.com.cn";
	char noUri[128];
	char verifyUri[256];
	char *password , *ssi_ip , *res;
	int passwordType;
	int ret;
	
	DEBUG_FOOTPRINT();

	debug_info("Initialize ssi authentication action");
	password = hash_password_v4(user->userId , user->password);
	bzero(noUri , sizeof(noUri));
	if(user->loginType == LOGIN_TYPE_MOBILENO)
		sprintf(noUri , "mobileno=%s" , user->mobileno);
	else
		sprintf(noUri , "sid=%s" , user->sId);
	bzero(verifyUri , sizeof(verifyUri));
	if(user->verification != NULL && user->verification->code != NULL)
	{
		sprintf(verifyUri , "&pid=%s&pic=%s&algorithm=%s"
						  , user->verification->guid
						  , user->verification->code
						  , user->verification->algorithm);
	}
	passwordType = (strlen(user->userId) == 0 ? 1 : 2);
	sprintf(sslbuf, "GET /ssiportal/SSIAppSignInV4.aspx?%s"
				    "&domains=fetion.com.cn%s&v4digest-type=%d&v4digest=%s\r\n"
				    "User-Agent: IIC2.0/pc "PROTO_VERSION"\r\n"
					"Host: %s\r\n"
				    "Cache-Control: private\r\n"
				    "Connection: Keep-Alive\r\n\r\n",
				    noUri , verifyUri , passwordType , password , "uid.fetion.com.cn");
	ssi_ip = get_ip_by_name(ssiName);
	FetionConnection* ssl;
	ssl = tcp_connection_new();

	if(user->config->proxy != NULL && user->config->proxy->proxyEnabled){
		ret = tcp_connection_connect_with_proxy(ssl , ssi_ip , 443 , user->config->proxy);
		if(ret < 0)
			return NULL;
	}else{
		if(tcp_connection_connect(ssl , ssi_ip , 443) < 0)
			return NULL;
	}

	debug_info("Start ssi login with %s password , user number %s"
			, passwordType == 1 ? "v3Temp" : "v4"
			, user->loginType == LOGIN_TYPE_MOBILENO ? user->mobileno : user->sId);
	if(ssl_connection_start(ssl) == -1){
		debug_error("Initialize ssl failed,please retry or check your system`s configuration");
		return NULL;
	}
	res = ssl_connection_get(ssl , sslbuf);
	tcp_connection_free(ssl);
	free(password);
	free(ssi_ip);
	return res;
}
char* sipc_reg_action(User* user)
{
	char* sipmsg;
	char* cnouce = generate_cnouce();

	FetionSip* sip = user->sip;

	DEBUG_FOOTPRINT();
	debug_info("Initialize sipc registeration action");

	fetion_sip_set_type(sip , SIP_REGISTER);
	SipHeader* cheader = fetion_sip_header_new("CN" , cnouce);
	SipHeader* client = fetion_sip_header_new("CL" , "type=\"pc\" ,version=\"PROTO_VERSION\"");
	fetion_sip_add_header(sip , cheader);
	fetion_sip_add_header(sip , client);
	free(cnouce);
	sipmsg = fetion_sip_to_string(sip , NULL);
	debug_info("Start registering to sip server(%s:%d)"
			 , sip->tcp->remote_ipaddress , sip->tcp->remote_port);
	tcp_connection_send(sip->tcp , sipmsg , strlen(sipmsg));
	free(sipmsg);
	sipmsg = (char*)malloc(1024);
	bzero(sipmsg , 1024);
	if(tcp_connection_recv(sip->tcp , sipmsg , 1024) <= 0){
		debug_info("Net work error occured here");
		return NULL;
	}
	return sipmsg;
}
char* sipc_aut_action(User* user , const char* response)
{
	char* sipmsg;
	char* xml;
	char* res;
	SipHeader* aheader = NULL;
	SipHeader* akheader = NULL;
	SipHeader* ackheader = NULL;
	FetionSip* sip = user->sip;

	DEBUG_FOOTPRINT();
	debug_info("Initialize sipc authencation action");

	xml = generate_auth_body(user);
	fetion_sip_set_type(sip , SIP_REGISTER);
	aheader = fetion_sip_authentication_header_new(response);
	akheader = fetion_sip_header_new("AK" , "ak-value");
	fetion_sip_add_header(sip , aheader);
	fetion_sip_add_header(sip , akheader);
	if(user->verification != NULL && user->verification->algorithm != NULL)	
	{
		ackheader = fetion_sip_ack_header_new(user->verification->code
											, user->verification->algorithm
											, user->verification->type
											, user->verification->guid);
		fetion_sip_add_header(sip , ackheader);
	}
	sipmsg = fetion_sip_to_string(sip , xml);
	debug_info("Start sipc authentication , with ak-value");
	debug_info("contact-version : %s , personal-version %s"
			 , user->contactVersion , user->personalVersion);
	tcp_connection_send(sip->tcp , sipmsg , strlen(sipmsg));
	res = fetion_sip_get_response(sip);
	free(sipmsg);
	return res;
}

void parse_ssi_auth_response(const char* ssi_response , User* user)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	char* pos;
	char* xml = strstr(ssi_response , "\r\n\r\n") + 4;
	int n;

	DEBUG_FOOTPRINT();
	if(strstr(ssi_response , "ssic=")){
		pos = strstr(ssi_response , "ssic=") + 5;
		n = strlen(pos) - strlen(strstr(pos , ";"));
		user->ssic = (char*)malloc(n + 1);
		bzero(user->ssic , n + 1);
		strncpy(user->ssic , pos , n);
	}

	doc = xmlReadMemory(xml , strlen(xml) , NULL , "UTF-8" , XML_PARSE_RECOVER);
	node = xmlDocGetRootElement(doc);
	pos = (char*)xmlGetProp(node , BAD_CAST "status-code");
	user->loginStatus = atoi(pos);
	node = node->xmlChildrenNode;
	if(atoi(pos) == 200)
	{
		debug_info("SSI login success");
		parse_ssi_auth_success(node , user);
	}
	else
	{
		debug_info("SSI login failed , status-code :%s" , pos);
		parse_ssi_auth_failed(node , user);
	}
	free(pos);
	xmlFreeDoc(doc);
}
void parse_sipc_reg_response(const char* reg_response , char** nouce , char** key)
{
	char digest[2048] = { 0 };
	char* pos;
	int n;

	DEBUG_FOOTPRINT();

	fetion_sip_get_attr(reg_response , "W" , digest);

	pos = strstr(digest , "nonce") + 7;
	n = strlen(pos) - strlen(strstr(pos , "\","));
	*nouce = (char*)malloc(n + 1);
	strncpy(*nouce , pos , n);
	(*nouce)[n] = '\0';

	pos = strstr(pos , "key") + 5;
	n = strlen(pos) - strlen(strstr(pos , "\","));
	*key = (char*)malloc(n + 1);
	strncpy(*key , pos , n);
	(*key)[n] = '\0';
	debug_info("Register to sip server success");
	debug_info("nonce:%s" , *nouce);
}
static void parse_sms_frequency(xmlNodePtr node , User *user)
{
	xmlChar *res;

	node = node->xmlChildrenNode;
	if(xmlHasProp(node , BAD_CAST "day-limit")){
		res = xmlGetProp(node , BAD_CAST "day-limit");
		user->smsDayLimit = atoi((char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "day-count")){
		res = xmlGetProp(node , BAD_CAST "day-count");
		user->smsDayCount = atoi((char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "month-limit")){
		res = xmlGetProp(node , BAD_CAST "month-limit");
		user->smsMonthLimit = atoi((char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "month-count")){
		res = xmlGetProp(node , BAD_CAST "month-count");
		user->smsMonthCount = atoi((char*)res);
		xmlFree(res);
	}
}

int parse_sipc_auth_response(const char* auth_response , User* user)
{
	char *pos;
	xmlChar* buf = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr rootnode = NULL;
	xmlNodePtr node = NULL;
	xmlNodePtr node1 = NULL;
	int code;

	DEBUG_FOOTPRINT();

	code = fetion_sip_get_code(auth_response);
	user->loginStatus = code;
	if(code == 200)
	{
		fetion_verification_free(user->verification);
		user->verification = NULL;
		debug_info("Sipc authentication success");
	}
	else if(code == 421 || code == 420)
	{
		parse_add_buddy_verification(user , auth_response);
		return 2;
	}
	else
	{
		debug_error("Sipc authentication failed");
		return -1;
	}
	pos = strstr(auth_response , "\r\n\r\n") + 4;
	doc = xmlReadMemory( pos , strlen(pos) , NULL , "UTF-8" , XML_PARSE_RECOVER);
	rootnode = xmlDocGetRootElement(doc); 
	node = rootnode->xmlChildrenNode;
	buf = xmlGetProp(node , BAD_CAST "public-ip");
	strcpy(user->publicIp , (char*)buf);
	xmlFree(buf);
	buf = xmlGetProp(node , BAD_CAST "last-login-ip");
	strcpy(user->lastLoginIp , (char*)buf);
	xmlFree(buf);
	buf = xmlGetProp(node , BAD_CAST "last-login-time");
	strcpy(user->lastLoginTime , (char*)buf);
	xmlFree(buf);
	node = node->next;
	node1 = node->xmlChildrenNode;
	parse_personal_info(node1 , user);
	node1 = xml_goto_node(node , "custom-config");
	buf = xmlGetProp(node1 , BAD_CAST "version");
	strcpy(user->customConfigVersion , (char*)buf);
	xmlFree(buf);
	buf = xmlNodeGetContent(node1);
	user->customConfig = malloc(strlen((char*)buf) + 1);
	memset(user->customConfig , 0 , strlen((char*)buf) + 1);
	strcpy(user->customConfig , (char*)buf);
	xmlFree(buf);
	node1 = xml_goto_node(node , "contact-list");
	parse_contact_list(node1 , user);
	node1 = xml_goto_node(node , "chat-friends");
	if(node1 != NULL){
		parse_stranger_list(node1 , user);
	}
	node1 = xml_goto_node(node , "quota-frequency");
	if(node1 != NULL){
		parse_sms_frequency(node1 , user);
	}
	xmlFreeDoc(doc);
	return 1;
}

char* generate_aes_key()
{
	char* key = (char*)malloc(65);
	memset( key , 0 , 65 );
	sprintf( key , "%04x%04x%04x%04x%04x%04x%04x"
			"%04x%04x%04x%04x%04x%04x%04x%04x%04x" , 
			rand() & 0xFFFF , rand() & 0xFFFF , 
			rand() & 0xFFFF , rand() & 0xFFFF , 
			rand() & 0xFFFF , rand() & 0xFFFF , 
			rand() & 0xFFFF , rand() & 0xFFFF , 
			rand() & 0xFFFF , rand() & 0xFFFF , 
			rand() & 0xFFFF , rand() & 0xFFFF ,
			rand() & 0xFFFF , rand() & 0xFFFF,
			rand() & 0xFFFF , rand() & 0xFFFF );
	return key;
}
static char* generate_auth_body(User* user)
{
	char basexml[] = "<args></args>";
	char state[5];
	xmlChar* buf = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr rootnode = NULL;
	xmlNodePtr node = NULL;
	xmlNodePtr node1 = NULL;

	DEBUG_FOOTPRINT();

	doc = xmlParseMemory( basexml , strlen(basexml));
	rootnode = xmlDocGetRootElement(doc); 
	node = xmlNewChild(rootnode , NULL , BAD_CAST "device" , NULL);
	xmlNewProp(node , BAD_CAST "machine-code" , BAD_CAST "001676C0E351");
	node = xmlNewChild(rootnode , NULL , BAD_CAST "caps" , NULL);
	xmlNewProp(node , BAD_CAST "value" , BAD_CAST "1ff");
	node = xmlNewChild(rootnode , NULL , BAD_CAST "events" , NULL);
	xmlNewProp(node , BAD_CAST "value" , BAD_CAST "7f");
	node = xmlNewChild(rootnode , NULL , BAD_CAST "user-info" , NULL);
	xmlNewProp(node , BAD_CAST "mobile-no" , BAD_CAST user->mobileno);
	xmlNewProp(node , BAD_CAST "user-id" , BAD_CAST user->userId);
	node1 = xmlNewChild(node , NULL , BAD_CAST "personal" , NULL);
	xmlNewProp(node1 , BAD_CAST "version" , BAD_CAST "0");//user->personalVersion);
	xmlNewProp(node1 , BAD_CAST "attributes" , BAD_CAST "v4default");
	node1 = xmlNewChild(node , NULL , BAD_CAST "custom-config" , NULL);
	xmlNewProp(node1 , BAD_CAST "version" , BAD_CAST "0");// user->customConfigVersion);
	node1 = xmlNewChild(node , NULL , BAD_CAST "contact-list" , NULL);
	xmlNewProp(node1 , BAD_CAST "version" , BAD_CAST "0");//user->contactVersion);
	xmlNewProp(node1 , BAD_CAST "buddy-attributes" , BAD_CAST "v4default");
	node = xmlNewChild(rootnode , NULL , BAD_CAST "credentials" , NULL);
	xmlNewProp(node , BAD_CAST "domains" , BAD_CAST "fetion.com.cn");
	node = xmlNewChild(rootnode , NULL , BAD_CAST "presence" , NULL);
	node1 = xmlNewChild(node , NULL , BAD_CAST "basic" , NULL);
	bzero(state , sizeof(state));
	sprintf(state , "%d" , user->state);
	xmlNewProp(node1 , BAD_CAST "value" , BAD_CAST state);
	xmlNewProp(node1 , BAD_CAST "desc" , BAD_CAST "");
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return xml_convert(buf);
}
static void parse_personal_info(xmlNodePtr node , User* user)
{
	xmlChar *buf;
	char *pos;
	int n;
	
	DEBUG_FOOTPRINT();

	buf = xmlGetProp(node , BAD_CAST "version");
	strcpy(user->personalVersion , (char*)buf);
	xmlFree(buf);
	if(xmlHasProp(node , BAD_CAST "sid"))
	{
		buf = xmlGetProp(node , BAD_CAST "sid");
		strcpy(user->sId , (char*)buf);
		xmlFree(buf);
	}
	if(xmlHasProp(node , BAD_CAST "mobile-no"))
	{
		buf = xmlGetProp(node , BAD_CAST "mobile-no");
		if(xmlStrlen(buf)){
			user->boundToMobile = BOUND_MOBILE_ENABLE;
		}else{
			user->boundToMobile = BOUND_MOBILE_DISABLE;
		}
		strcpy(user->mobileno , (char*)buf);
		xmlFree(buf);
	}
	if(xmlHasProp(node , BAD_CAST "carrier-status"))
	{
		buf = xmlGetProp(node , BAD_CAST "carrier-status");
		user->carrierStatus = atoi((char*)buf);
		xmlFree(buf);
	}
	if(xmlHasProp(node , BAD_CAST "nickname"))
	{
		buf = xmlGetProp(node , BAD_CAST "nickname");
		strcpy(user->nickname , (char*)buf);
		xmlFree(buf);
	}
	if(xmlHasProp(node , BAD_CAST "gender"))
	{
		buf = xmlGetProp(node , BAD_CAST "gender");
		user->gender = atoi((char*)buf);
		xmlFree(buf);
	}
	if(xmlHasProp(node , BAD_CAST "impresa"))
	{
		buf = xmlGetProp(node , BAD_CAST "impresa");
		strcpy(user->impression , (char*)buf);
		xmlFree(buf);
	}
	if(xmlHasProp(node , BAD_CAST "carrier-region"))
	{
		buf = xmlGetProp(node , BAD_CAST "carrier-region");
		pos = (char*)buf;
		n = strlen(pos) - strlen(strstr(pos , "."));
		strncpy(user->country , pos , n);
		pos = strstr(pos , ".") + 1;
		n = strlen(pos) - strlen(strstr(pos , "."));
		strncpy(user->province , pos , n);
		pos = strstr(pos , ".") + 1;
		n = strlen(pos) - strlen(strstr(pos , "."));
		strncpy(user->city , pos , n);
		xmlFree(buf);
	}
}
static void parse_contact_list(xmlNodePtr node , User* user)
{
	xmlChar* buf = NULL;
	xmlNodePtr node1 , node2;
	Group* group = NULL;
	Contact* contact = NULL;
	int hasGroup = 1 , hasBuddy = 1;

	DEBUG_FOOTPRINT();

//	buf = xmlGetProp(node , BAD_CAST "version");
	debug_info("Start reading contact list ");
//	if(strcmp(user->contactVersion , (char*) buf) == 0){
//		debug_info("Contact list is the same as that stored in the local disk!");
//		return ;
//	}
//	strcpy(user->contactVersion , (char*)buf);
//	xmlFree(buf);
	node1 = xml_goto_node(node , "buddy-lists");
	node2 = node1->xmlChildrenNode;
	while(node2 != NULL){
		buf = xmlGetProp(node2 , BAD_CAST "id");
		group = fetion_group_list_find_by_id(user->groupList , atoi((char*)buf));
		if(group == NULL){
			hasGroup = 0;
			group = fetion_group_new();
		}
		group->groupid = atoi((char*)buf);
		xmlFree(buf);
		buf = xmlGetProp(node2 , BAD_CAST "name");
		strcpy(group->groupname , (char*)buf);
		xmlFree(buf);
		
		if(hasGroup == 0){
			fetion_group_list_append(user->groupList , group);
			hasGroup = 1;
		}
		node2 = node2->next;
	}
	node1 = xml_goto_node(node , "buddies");
	node1 = node1->xmlChildrenNode;
	while(node1 != NULL){
		if(! xmlHasProp(node1 , BAD_CAST "i")){
			node1 = node1->next;
			continue;
		}
		buf = xmlGetProp(node1 , BAD_CAST "i");
		contact = fetion_contact_list_find_by_userid(user->contactList , (char*)buf);
		if(contact == NULL){
			hasBuddy = 0;
			contact = fetion_contact_new();
		}
		strcpy(contact->userId , (char*)buf);
		xmlFree(buf);
		if(xmlHasProp(node1 , BAD_CAST "n")){
			buf = xmlGetProp(node1 , BAD_CAST "n");
			strcpy(contact->localname , (char*)buf);
			xmlFree(buf);
		}
		if(xmlHasProp(node1 , BAD_CAST "l")){
			buf = xmlGetProp(node1 , BAD_CAST "l");
			contact->groupid = atoi((char*)buf);
			if(xmlStrstr(buf , BAD_CAST ";") != NULL
					|| contact->groupid < 0)
					contact->groupid = 0;
			xmlFree(buf);
		}
		if(xmlHasProp(node1 , BAD_CAST "p")){
			buf = xmlGetProp(node1 , BAD_CAST "p");
			if(strstr((char*)buf , "identity=1") != NULL)
				contact->identity = 1;
			else
				contact->identity = 0;
			xmlFree(buf);
		}
		if(xmlHasProp(node1 , BAD_CAST "r")){
			buf = xmlGetProp(node1 , BAD_CAST "r");
			contact->relationStatus = atoi((char*)buf);
			xmlFree(buf);
		}

		if(xmlHasProp(node1 , BAD_CAST "u")){
			buf = xmlGetProp(node1 , BAD_CAST "u");
			strcpy(contact->sipuri , (char*)buf);
			//if(strstr((char*)buf , "tel") != NULL)
			//	contact->serviceStatus = STATUS_SMS_ONLINE;
			xmlFree(buf);
		}

		strcpy(contact->portraitCrc , "unlogin");

		if(hasBuddy == 0){
			fetion_contact_list_append(user->contactList , contact);
			hasBuddy = 1;
		}
		node1 = node1->next;
	}
	debug_info("Read contact list complete");
}
static void parse_stranger_list(xmlNodePtr node , User* user)
{
	xmlNodePtr node1 = node->xmlChildrenNode;
	xmlChar *buf = NULL;
	Contact *contact = NULL;
	int hasBuddy = 1;
	while(node1 != NULL)
	{
		buf = xmlGetProp(node1 , BAD_CAST "u");
		contact = fetion_contact_list_find_by_userid(user->contactList , (char*)buf);
		if(contact == NULL){
			hasBuddy = 0;
			contact = fetion_contact_new();
		}
		strcpy(contact->sipuri , (char*)buf);
		xmlFree(buf);
		buf = xmlGetProp(node1 , BAD_CAST "i");
		strcpy(contact->userId , (char*)buf);
		contact->groupid = BUDDY_LIST_STRANGER;
		if(hasBuddy == 0){
			fetion_contact_list_append(user->contactList , contact);
		}
		node1 = node1->next;
	}
}
static void parse_ssi_auth_success(xmlNodePtr node , User* user)
{
	char* pos;
	pos = (char*)xmlGetProp(node , BAD_CAST "uri");
	strcpy(user->sipuri , pos);
	free(pos);
	pos = fetion_sip_get_sid_by_sipuri(user->sipuri);
	strcpy(user->sId , pos);
	free(pos);
	pos = (char*)xmlGetProp(node , BAD_CAST "mobile-no");
	strcpy(user->mobileno , pos);
	free(pos);
	pos = (char*)xmlGetProp(node , BAD_CAST "user-id");
	strcpy(user->userId , pos);
	free(pos);
}
static void parse_ssi_auth_failed(xmlNodePtr node , User* user)
{
	Verification* ver = fetion_verification_new();
	ver->algorithm = (char*)xmlGetProp(node , BAD_CAST "algorithm");
	ver->type      = (char*)xmlGetProp(node , BAD_CAST "type");
	ver->text      = (char*)xmlGetProp(node , BAD_CAST "text");
	ver->tips	   = (char*)xmlGetProp(node , BAD_CAST "tips");
	user->verification = ver;
}
static unsigned char* strtohex(const char* in , int* len) 
{
	unsigned char* out = (unsigned char*)malloc(strlen(in)/2 );
	int i = 0 , j = 0 , k = 0 ,length = 0;
	char tmp[3] = { 0 };
	memset(out , 0 , strlen(in) / 2);
	while(i < (int)strlen(in))
	{
		tmp[k++] = in[i++];
		tmp[k] = '\0';
		if(k == 2)
		{
			out[j++] = (unsigned char)strtol(tmp , (char**)NULL , 16);
			k = 0;
			length ++;
		}
	}
	if(len != NULL )
		*len = length;
	return out;
}
static char* hextostr(const unsigned char* in , int len) 
{
	char* res = (char*)malloc(len * 2 + 1);
	int i = 0;
	memset(res , 0 , len * 2 + 1);
	while(i < len)
	{
		sprintf(res + i * 2 , "%02x" , in[i]);
		i ++;
	};
	i = 0;
	while(i < (int)strlen(res))
	{
		res[i] = toupper(res[i]);
		i ++;
	};
	return res;
}
static char* hash_password_v1(const unsigned char* b0 , int b0len , const unsigned char* password , int psdlen) 
{
	unsigned char* dst = (unsigned char*)malloc(b0len + psdlen + 1);
	unsigned char tmp[20];
	char* res;
	memset(tmp , 0 , sizeof(tmp));
	memset(dst , 0 , b0len + psdlen + 1);
	memcpy(dst , b0 , b0len);
	memcpy(dst + b0len , password , psdlen);
	SHA_CTX ctx;
	SHA1_Init(&ctx);
	SHA1_Update(&ctx , dst , b0len + psdlen );
	SHA1_Final(tmp , &ctx);
	free(dst);
	res = hextostr(tmp , 20);
	return res;
}
static char* hash_password_v2(const char* userid , const char* passwordhex) 
{
	int id = atoi(userid);
	char* res;
	unsigned char* bid = (unsigned char*)(&id);
	unsigned char ubid[4];
	int bpsd_len;
	unsigned char* bpsd = strtohex(passwordhex , &bpsd_len);
	memcpy(ubid , bid , 4);
	res = hash_password_v1(ubid , sizeof(id) , bpsd , bpsd_len);
	free(bpsd);
	return res;
}
static char* hash_password_v4(const char* userid , const char* password)
{
	const char* domain = "fetion.com.cn:";
	char *res , *dst;
	unsigned char* udomain = (unsigned char*)malloc(strlen(domain));
	unsigned char* upassword = (unsigned char*)malloc(strlen(password));
	memset(udomain , 0 , strlen(domain));
	memcpy(udomain , (unsigned char*)domain , strlen(domain));
	memset(upassword , 0 , strlen(password));
	memcpy(upassword , (unsigned char*)password , strlen(password));
	res = hash_password_v1(udomain , strlen(domain) , upassword , strlen(password));
	free(udomain);
	free(upassword);
	if(userid == NULL || strlen(userid) == 0)
	{
		return res;
	}
	dst = hash_password_v2(userid , res);
	free(res);
	return dst;
}
static char* generate_cnouce()
{
	char* cnouce = (char*)malloc(33);
	memset( cnouce , 0 , 33 );
	sprintf( cnouce , "%04X%04X%04X%04X%04X%04X%04X%04X" , 
			rand() & 0xFFFF , rand() & 0xFFFF , 
			rand() & 0xFFFF , rand() & 0xFFFF ,
			rand() & 0xFFFF , rand() & 0xFFFF,
			rand() & 0xFFFF , rand() & 0xFFFF );
	return cnouce;
}

static unsigned char* decode_base64(const char* in , int* len)
{
 	unsigned int n , t = 0 , c = 0;
	unsigned char* res;
	unsigned char out[3];
	unsigned char inp[4];

	n = strlen(in);
	if(n % 4 != 0)
	{
		debug_error("Try to decode a string which is not a base64 string(decode_base64)");
		return NULL;
	}
	n = n / 4 * 3;
	if(len != NULL)
		*len = n;
	res = (unsigned char*)malloc(n);
	memset(res , 0 , n);
	while(1)
	{
		memset(inp , 0 , 4);
		memset(out , 0 , 3);
		memcpy(inp , in + c , 4);
		c += 4;
		n = EVP_DecodeBlock(out , inp , 4 );
		memcpy(res + t , out , n);
		t += n;
		if(c >= strlen(in))
			break;
	}
	return res;
}
