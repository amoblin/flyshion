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

#ifndef FETION_SIP_H
#define FETION_SIP_H
#define SIP_BUFFER_SIZE 2048

typedef enum 
{
	SIP_REGISTER = 1 ,
	SIP_SERVICE ,
 	SIP_SUBSCRIPTION , 
	SIP_NOTIFICATION ,
	SIP_INVITATION , 
	SIP_INCOMING , 
	SIP_MESSAGE ,
	SIP_SIPC_4_0 ,
	SIP_UNKNOWN
} SipType;

typedef enum
{
	NOTIFICATION_TYPE_PRESENCE ,
	NOTIFICATION_TYPE_CONTACT ,
	NOTIFICATION_TYPE_CONVERSATION ,
	NOTIFICATION_TYPE_REGISTRATION ,
	NOTIFICATION_TYPE_SYNCUSERINFO ,
	NOTIFICATION_TYPE_UNKNOWN
} NotificationType;

typedef enum
{
	NOTIFICATION_EVENT_PRESENCECHANGED ,
	NOTIFICATION_EVENT_ADDBUDDYAPPLICATION ,
	NOTIFICATION_EVENT_USERLEFT ,
	NOTIFICATION_EVENT_DEREGISTRATION , 
	NOTIFICATION_EVENT_SYNCUSERINFO ,
	NOTIFICATION_EVENT_UNKNOWN
} NotificationEvent;

typedef enum
{
	SIP_EVENT_PRESENCE = 0,
	SIP_EVENT_SETPRESENCE ,
	SIP_EVENT_CONTACT ,
	SIP_EVENT_CONVERSATION ,
	SIP_EVENT_CATMESSAGE ,
	SIP_EVENT_SENDCATMESSAGE ,
	SIP_EVENT_STARTCHAT ,
	SIP_EVENT_INVITEBUDDY ,
	SIP_EVENT_GETCONTACTINFO ,
	SIP_EVENT_CREATEBUDDYLIST ,
	SIP_EVENT_DELETEBUDDYLIST ,
	SIP_EVENT_SETCONTACTINFO ,
	SIP_EVENT_SETUSERINFO ,
	SIP_EVENT_SETBUDDYLISTINFO ,
	SIP_EVENT_DELETEBUDDY ,
	SIP_EVENT_ADDBUDDY ,
	SIP_EVENT_KEEPALIVE ,
	SIP_EVENT_HANDLECONTACTREQUEST
} SipEvent;

typedef enum
{
	INCOMING_NUDGE ,
	INCOMING_UNKNOWN
} IncomingType;

extern FetionSip* fetion_sip_new(FetionConnection* tcp , const char* sid);

extern FetionSip* fetion_sip_clone(FetionSip* sip);

extern SipHeader* fetion_sip_header_new(const char* name , const char* value);

extern void fetion_sip_set_type(FetionSip* sip , SipType type);

extern SipHeader* fetion_sip_authentication_header_new(const char* response);

extern SipHeader* fetion_sip_ack_header_new(const char* code
		, const char* algorithm , const char* type , const char* guid);

extern SipHeader* fetion_sip_event_header_new(int eventType);

extern SipHeader* fetion_sip_credential_header_new(const char* credential);

extern void fetion_sip_add_header(FetionSip* sip , SipHeader* header);

extern char* fetion_sip_to_string(FetionSip* sip , const char* body);

extern void fetion_sip_free(FetionSip* sip);

extern char* fetion_sip_get_sid_by_sipuri(const char* sipuri);

extern int fetion_sip_get_attr(const char* sip , const char* name , char* result);

extern int fetion_sip_get_length(const char* sip);

extern int fetion_sip_get_code(const char* sip);

extern int fetion_sip_get_type(const char* sip);

extern void fetion_sip_get_auth_attr(const char* auth , char** ipaddress
		, int* port , char** credential);

extern char* fetion_sip_get_response(FetionSip* sip);

extern void fetion_sip_set_connection(FetionSip* sip , FetionConnection* conn);

extern SipMsg* fetion_sip_listen(FetionSip* sip);

extern void fetion_sip_keep_alive(FetionSip* sip);

extern void fetion_sip_message_free(SipMsg* msg);

extern void fetion_sip_message_append(SipMsg* msglist , SipMsg* msg);

extern void fetion_sip_parse_notification(const char* sip 
		, int* type , int* event , char** xml);

extern void fetion_sip_parse_message(FetionSip* sip , const char* sipmsg , Message** msg);

extern void fetion_sip_parse_invitation(FetionSip* sip  , Proxy *proxy, const char* sipmsg
		, FetionSip** conversionSip , char** sipuri);

extern void fetion_sip_parse_addbuddyapplication(const char* sipmsg , char** sipuri
		, char** userid , char** desc , int* phrase);

extern void fetion_sip_parse_incoming(FetionSip* sip , const char* sipmsg
		, char** sipuri , IncomingType* type);

extern void fetion_sip_parse_userleft(const char* sipmsg , char** sipuri);

extern struct tm convert_date(const char* date);
#endif
