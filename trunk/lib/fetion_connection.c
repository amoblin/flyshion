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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netdb.h>

const char* global_ssi_uri = "https://uid.fetion.com.cn/ssiportal/SSIAppSignInV4.aspx";

int tcp_keep_alive(int socketfd)
{
	int keepAlive = 1;
	int keepIdle = 10;
	int keepInterval = 5;
	int keepCount = 5;

	if(setsockopt(socketfd , SOL_SOCKET , SO_KEEPALIVE 
				,(void*)&keepAlive,sizeof(keepAlive)) == -1){
		debug_info("set SO_KEEPALIVE failed\n");
		return -1;
	}

	if(setsockopt(socketfd , SOL_TCP , TCP_KEEPIDLE 
				,(void *)&keepIdle,sizeof(keepIdle)) == -1){
		debug_info("set TCP_KEEPIDEL failed\n");
		return -1;
	}

	if(setsockopt(socketfd , SOL_TCP , TCP_KEEPINTVL
				,(void *)&keepInterval,sizeof(keepInterval)) == -1){
		debug_info("set TCP_KEEPINTVL failed\n");
		return -1;
	}

	if(setsockopt(socketfd , SOL_TCP , TCP_KEEPCNT
				,(void *)&keepCount,sizeof(keepCount)) == -1){
		debug_info("set TCP_KEEPCNT failed\n");
		return -1;
	}
	return 1;
}

FetionConnection* tcp_connection_new(void)
{
	FetionConnection* conn = (FetionConnection*)malloc(sizeof(FetionConnection));
	memset(conn , 0 , sizeof(FetionConnection));
	conn->socketfd = socket(AF_INET , SOCK_STREAM , 0);
	if(tcp_keep_alive(conn->socketfd) == -1)
		return NULL;
	conn->ssl = NULL;
	conn->ssl_ctx = NULL;
	return conn;
}

FetionConnection* tcp_connection_new_with_port(const int port)
{
	int ret;
	struct sockaddr_in addr;
	FetionConnection* conn = NULL;
	
	conn = (FetionConnection*)malloc(sizeof(FetionConnection));
	memset(conn , 0 , sizeof(FetionConnection));
	conn->socketfd = socket(AF_INET , SOCK_STREAM , 0);
	if(tcp_keep_alive(conn->socketfd) == -1)
		return NULL;
	conn->local_port = port;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	ret = bind(conn->socketfd , (struct sockaddr*)(&addr) , sizeof(struct sockaddr));
	if(ret == -1){
		printf("Failed to bind");
		return NULL;
	}
	return conn;
}

FetionConnection* tcp_connection_new_with_ip_and_port(const char* ipaddress , const int port)
{
	int ret;
	struct sockaddr_in addr;
	FetionConnection* conn = NULL;
	
	conn = (FetionConnection*)malloc(sizeof(FetionConnection));
	memset(conn , 0 , sizeof(FetionConnection));
	conn->socketfd = socket(AF_INET , SOCK_STREAM , 0);
	if(tcp_keep_alive(conn->socketfd) == -1)
		return NULL;
	strcpy(conn->local_ipaddress , ipaddress);
	conn->local_port = port;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ipaddress);
	addr.sin_port = htons(port);
	ret = bind(conn->socketfd , (struct sockaddr*)(&addr) , sizeof(struct sockaddr));
	if(ret == -1)
	{
		printf("Failed to bind");
		return NULL;
	}
	return conn;
}
int tcp_connection_connect(FetionConnection* connection , const char* ipaddress , const int port)
{
	struct sockaddr_in addr;
	int n;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ipaddress);
	addr.sin_port = htons(port);
	strcpy(connection->remote_ipaddress , ipaddress);
	connection->remote_port = port;

	n = MAX_RECV_BUF_SIZE;
	setsockopt(connection->socketfd , SOL_SOCKET , SO_RCVBUF , (const char*)&n , sizeof(n));
	return connect(connection->socketfd , (struct sockaddr*)&addr , sizeof(struct sockaddr));
}

int tcp_connection_connect_with_proxy(FetionConnection* connection 
		, const char* ipaddress , const int port , Proxy *proxy)
{
	struct sockaddr_in addr;
	int n;
	char http[1024] , code[5] , *pos = NULL;
	unsigned char authentication[1024];
	char authen[1024];
	char authorization[1024];
	char *ip = get_ip_by_name(proxy->proxyHost);

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(proxy->proxyPort);
	strcpy(connection->remote_ipaddress , ipaddress);
	connection->remote_port = port;

	n = MAX_RECV_BUF_SIZE;
	setsockopt(connection->socketfd , SOL_SOCKET , SO_RCVBUF , (const char*)&n , sizeof(n));
	connect(connection->socketfd , (struct sockaddr*)&addr , sizeof(struct sockaddr));

	bzero(authorization , sizeof(authorization));
	if(strlen(proxy->proxyUser) != 0 && strlen(proxy->proxyPass) != 0)
	{
		bzero(authen , sizeof(authen));
		sprintf(authen , "%s:%s" , proxy->proxyUser , proxy->proxyPass);
		EVP_EncodeBlock(authentication , (unsigned char*)authen , strlen(authen));
		sprintf(authorization , "Proxy-Authorization: Basic %s\r\n" , (char*)authentication);
	}

	bzero(http , sizeof(http));
	snprintf(http , 1023 , "CONNECT %s:%d HTTP/1.1\r\n"
				   "Host: %s:%d\r\n%s"
				   "User-Agent: OpenFetion\r\n\r\n"
				 , ipaddress , port , ipaddress , port , authorization);

#if 0
	debug_info("Connecting to %s:%d through proxy server %s:%d"
			, ipaddress , port , ip , proxy->proxyPort);
#endif

	tcp_connection_send(connection , http , strlen(http));

	bzero(http , sizeof(http));

	tcp_connection_recv(connection , http , sizeof(http));

	pos = strstr(http , " ") + 1;
	n = strlen(pos) - strlen(strstr(pos , " "));
	bzero(code , sizeof(code));
	strncpy(code , pos , n);
	free(ip); ip = NULL;

	if(strcmp(code , "200") != 0)
		return -1;

	return 1;
}
int tcp_connection_select_read(FetionConnection* connection)
{
	fd_set fs ; 
	struct timeval tv ; 
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	FD_ZERO(&fs);
	FD_SET(connection->socketfd , &fs);
	return select(connection->socketfd + 1 , &fs , NULL , NULL , &tv );
}
int tcp_connection_send(FetionConnection* connection , const void* buf , int len)
{
	return send(connection->socketfd , buf , len , 0);
}

int tcp_connection_recv(FetionConnection* connection , void* buf , int len)
{
	return recv(connection->socketfd , buf , len , 0);
}

int tcp_connection_recv_dont_wait(FetionConnection* connection , void* buf , int len)
{
	return recv(connection->socketfd , buf , len , MSG_DONTWAIT);
}

int tcp_connection_getname(FetionConnection* connection , char **ip , int *port)
{
	struct sockaddr_in addr;
	int ret;
	socklen_t len = 0;

	len = sizeof(struct sockaddr);

	ret = getsockname(connection->socketfd
			, (struct sockaddr*)&addr , &len);

	*ip = inet_ntoa(addr.sin_addr);
	*port = ntohs(addr.sin_port);

	return ret;
}
int ssl_connection_start(FetionConnection* conn)
{
	int ret;
	SSL_load_error_strings();
	SSL_library_init();
	conn->ssl_ctx = SSL_CTX_new(SSLv23_client_method());

	if ( conn->ssl_ctx == NULL ){
		debug_info("Init SSL CTX failed");
		return -1;
	}
	conn->ssl = SSL_new(conn->ssl_ctx);

	if ( conn->ssl == NULL ){
		debug_info("New SSL with created CTX failed");
		return -1;
	}
	ret = SSL_set_fd(conn->ssl, conn->socketfd);

	if ( ret == 0 ){
		debug_info("Add ssl to tcp socket failed");
		return -1;
	}
	RAND_poll();
	while ( RAND_status() == 0 )
	{
		unsigned short rand_ret = rand() % 65536;
		RAND_seed(&rand_ret, sizeof(rand_ret));
	} 
	ret = SSL_connect(conn->ssl);
	if( ret != 1 )
	{
		debug_info("SSL connection failed");
		return -1;
	}
	return 0;
}
char* ssl_connection_get(FetionConnection* conn , const char* buf)
{
	int n;
	char* ret;
	
	ret = (char*)malloc(1025);
	memset(ret , 0 , 1025);

	SSL_write(conn->ssl, buf, strlen(buf));
	n = SSL_read(conn->ssl, ret , 1024);
	return ret; 
}

char* http_connection_get_response(FetionConnection* conn)
{
	char buf[1024*4];
	char ls[10];
	char *pos;
	char *res;
	int  n = 0 , c , len;

	memset(buf, 0, sizeof(buf));

	do{
		c = tcp_connection_recv(conn
			, buf + n , sizeof(buf) - n - 1);
		n += c;
		if(n >= sizeof(buf))
			return NULL;
	}while(!strstr(buf , "\r\n\r\n"));

	pos = strstr(buf , "Content-Length: ") + 16;
	len = strlen(pos) - strlen(strstr(pos , "\r\n"));
	memset(ls , 0 , sizeof(ls));
	strncpy(ls , pos , len);
	len = atoi(ls);

	pos = strstr(pos , "\r\n\r\n") + 4;
	n = strlen(pos);

	res = (char*)malloc(len + 1);
	memset(res , 0 , len + 1);
	strcpy(res , pos);

	for(;;){
		memset(buf , 0 , sizeof(buf));
		c = tcp_connection_recv(conn , buf , sizeof(buf) - 1);
		if(c <= 0)
			break;
		strcpy(res + n, buf);
		n += c;
		if(n >= len)
			break;
	}

	return res;
}
int http_connection_get_body_length(const char* http)
{
	char *pos , length[10];
	int len;
	
	pos = strstr(http , "Content-Length: ");
	if(pos == NULL)
		return 0;
	pos += 16;
	len = strlen(pos) - strlen(strstr(pos , "\r\n"));
	bzero(length , sizeof(length));
	strncpy(length , pos , len);
	return atoi(length);
}
int http_connection_get_head_length(const char* http)
{
	if(strstr(http , "\r\n\r\n") == NULL)
		return -1;
	return strlen(http) - strlen(strstr(http , "\r\n\r\n"));
}
void tcp_connection_free(FetionConnection* conn)
{
	if(conn->ssl != NULL && conn->ssl_ctx != NULL)
	{
		SSL_free(conn->ssl);
		SSL_CTX_free(conn->ssl_ctx);
		ERR_free_strings();
	}
	close(conn->socketfd);
	free(conn);
}
char* get_ip_by_name(const char* hostname)
{
	struct hostent *hst;
	char *name , *pos;
	int len;
	char* ip = (char*)malloc(20);
	memset(ip , 0 , 20);
	pos = strstr(hostname , "//");
	if(pos != NULL)
		pos += 2;
	else
		pos = (char*)hostname;
	if(strstr(pos , "/") != NULL)
		len = strlen(pos) - strlen(strstr(pos , "/"));
	else
		len = strlen(pos);
	name = (char*)malloc(len + 1);
	memset(name , 0 , len + 1);
	strncpy(name , pos , len);
reget:
	hst = gethostbyname(name);
	if(hst == NULL)
		goto reget;
	inet_ntop(AF_INET , hst->h_addr , ip , 16);
	free(name);
	return ip;
}
char* http_connection_encode_url(const char* url)
{
	char pos;
	char* res;
	char tmp[2];
	int i = 1;
	res = (char*)malloc(2048);
	pos = url[0];
	memset(res , 0 , 2048);
	while(pos != '\0')
	{
		if(pos == '/')
			strcat(res , "%2f");
		else if(pos == '@')
			strcat(res , "%40");
		else if(pos == '=')
			strcat(res , "%3d");
		else if(pos == ':')
			strcat(res , "%3a");
		else if(pos == ';')
			strcat(res , "%3b");
		else if(pos == '+'){
			strcat(res , "%2b");
		}else{
			bzero(tmp , sizeof(tmp));
			sprintf(tmp , "%c" , pos);
			strcat(res , tmp);
		}
		pos = (url + (i ++))[0];
	}
	return res;
}

char *get_loacl_ip(){
#if 0
    struct ifreq req;
    int sock;
	char *ip = NULL;

	DEBUG_FOOTPRINT();

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(req.ifr_name, "eth0", IFNAMSIZ);

	if ( ioctl(sock, SIOCGIFADDR, &req) < 0 ) {
		debug_error("Get local ipaddress failed");
		return NULL;
	}

	ip = (char *)inet_ntoa(*(struct in_addr *) &((struct sockaddr_in *) &req.ifr_addr)->sin_addr);
	shutdown(sock, 2);
	close(sock);
#endif
    return "";
}

char *hexip_to_dotip(const char *ip){
	
	char *out;
	char tmp[3];
	char tmp1[4];
	unsigned int i , j = 0;
	long res;

	out = (char*)malloc(18);
	bzero(out , 18);
	bzero(tmp , sizeof(tmp));

	for(i = 0 ; i < strlen(ip) ; i ++){
		tmp[j++] = ip[i]; 	
		if(j == 2){
			res = strtol(tmp , NULL , 16);
			bzero(tmp1 , sizeof(tmp1));
			sprintf(tmp1 , "%ld" , res);
			strcat(out , tmp1);
			if(i != 7){
				strcat(out , ".");
			}
			j = 0;
		}
	}
	return out;
}
