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

#ifndef P_CONNECTION_H
#define P_CONNECTION_H

#define MAX_RECV_BUF_SIZE 61140


extern FetionConnection* tcp_connection_new(void);

extern FetionConnection* tcp_connection_new_with_port(const int port);

extern FetionConnection* tcp_connection_new_with_ip_and_port(const char* ipaddress , const int port);

extern int tcp_connection_connect(FetionConnection* connection
		, const char* ipaddress , const int port);

extern int tcp_connection_connect_with_proxy(FetionConnection* connection 
		, const char* ipaddress , const int port , Proxy *proxy);

extern int tcp_connection_send(FetionConnection* connetion , const void* buf , int len);

extern int tcp_connection_recv(FetionConnection* connection , void* recv , int len);

extern int tcp_connection_select_read(FetionConnection* connection);

extern int tcp_connection_recv_dont_wait(FetionConnection* connection
		, void* recv , int len);

extern int tcp_connection_getname(FetionConnection* connection
		, char **ip , int *port);

extern int ssl_connection_start(FetionConnection* conn);

extern char* ssl_connection_get(FetionConnection* conn , const char* buf);

extern char* http_connection_get_response(FetionConnection* conn);

extern int http_connection_get_body_length(const char* http);

extern int http_connection_get_head_length(const char* http);

extern char* http_connection_encode_url(const char* url);

extern void tcp_connection_free(FetionConnection* conn);

extern char* get_ip_by_name(const char* hostname);

extern char *get_loacl_ip();

extern char *hexip_to_dotip(const char *ip);

#endif
