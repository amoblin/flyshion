#include "fetion_include.h"
#include <openssl/md5.h>

#define GUID "9741dc51-43d3-448b-bfc2-dbf4661a27f6"
#define SESSIONID "xz4BBcV9741dc5143d3448bbfc2dbf4661a27f6"

Share *fetion_share_new(const char *sipuri , const char *absolutePath)
{
	Share *share = (Share*)malloc(sizeof(Share));
	char *name = NULL;
	char *md5 = NULL;

	memset(share , 0 , sizeof(Share));

	strcpy(share->guid , GUID);
	strcpy(share->sessionid , SESSIONID);	
	strcpy(share->sipuri , sipuri);

	name = (char*)basename(absolutePath);
	strcpy(share->filename , name);
	share->filesize = fetion_share_get_filesize(absolutePath);
	md5 = fetion_share_compute_md5(absolutePath);
	strcpy(share->md5 , md5);
	free(md5);
	return share;
}

static char* generate_share_request_body(Share *share)
{
	xmlChar *buf = NULL;
	xmlDocPtr doc;
	xmlNodePtr node , fnode , root;
	char size[16];
	char body[] = "<share-content></share-content>";
	doc = xmlParseMemory(body , strlen(body));
	root = xmlDocGetRootElement(doc);
	xmlNewProp(root , BAD_CAST "id" , BAD_CAST share->guid);
	node = xmlNewChild(root , NULL , BAD_CAST "caps" , NULL);
	xmlNewProp(node , BAD_CAST "modes" , BAD_CAST "block;relay;p2p;p2pV2;relayV2;p2pV3;scV2");
	xmlNewProp(node , BAD_CAST "max-size" , BAD_CAST "2097151");
	node = xmlNewChild(root , NULL , BAD_CAST "client" , NULL);
	xmlNewProp(node , BAD_CAST "outer-ip" , BAD_CAST "");
	xmlNewProp(node , BAD_CAST "inner-ip" , BAD_CAST "59.64.129.143:1519;");
	xmlNewProp(node , BAD_CAST "port" , BAD_CAST "443");
	node = xmlNewChild(root , NULL , BAD_CAST "fileinfo" , NULL);
	fnode = xmlNewChild(node , NULL , BAD_CAST "transmit" , NULL);
	xmlNewProp(fnode , BAD_CAST "type" , BAD_CAST "p2p");
	xmlNewProp(fnode , BAD_CAST "session-id" , BAD_CAST share->sessionid);
	fnode = xmlNewChild(node , NULL , BAD_CAST "file" , NULL);
	xmlNewProp(fnode , BAD_CAST "name" , BAD_CAST share->filename);
	bzero(size , sizeof(size));
	sprintf(size , "%ld" , share->filesize);
	xmlNewProp(fnode , BAD_CAST "size" , BAD_CAST size);
	xmlNewProp(fnode , BAD_CAST "url" , BAD_CAST "");
	xmlNewProp(fnode , BAD_CAST "md5" , BAD_CAST share->md5);
	xmlNewProp(fnode , BAD_CAST "id" , BAD_CAST share->guid);
	xmlNewProp(fnode , BAD_CAST "file-type" , BAD_CAST "unknown");
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return xml_convert(buf);
}

void fetion_share_request(FetionSip *sip , Share *share)
{
	SipHeader *kheader = NULL;
	SipHeader *theader = NULL;
	char *res = NULL;
	char *body = NULL;
	char buf[2048];
	fetion_sip_set_type(sip , SIP_OPTION);
	
	kheader = fetion_sip_header_new("K" , "ShareContent");
	theader = fetion_sip_header_new("T" , share->sipuri);
	fetion_sip_add_header(sip , kheader);
	fetion_sip_add_header(sip , theader);
	body = generate_share_request_body(share);
	res = fetion_sip_to_string(sip , body);
	tcp_connection_send(sip->tcp , res , strlen(res));
	free(res);
	bzero(buf , sizeof(buf));
	tcp_connection_recv(sip->tcp , buf , sizeof(buf));
	printf("%s\n" , buf);
}

char* fetion_share_compute_md5(const char *absolutePath)
{
	MD5_CTX ctx;
	FILE *file;
	unsigned char input[1024];
	unsigned char output[16];
	int n , i = 0;
	char* res = (char*)malloc(33);

	file = fopen(absolutePath , "r");
	MD5_Init(&ctx);
	while(1)
	{
		n = fread(input ,  1 , sizeof(input) , file);
		if(n == 0)
			break;
		MD5_Update(&ctx , input , n);
	}
	MD5_Final(output , &ctx);
	bzero(res , 33);
	while(i < 16)
	{
		sprintf(res + i * 2 , "%02x" , output[i]);
		i ++;
	};
	return res;
}

long long fetion_share_get_filesize(const char *absolutePath)
{
	struct stat sb;

	if(stat(absolutePath , &sb) == -1)
	{
		debug_error("Can not get the file size");
		return -1;
	}

	return (long long)(sb.st_size);

}
