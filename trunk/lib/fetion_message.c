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


Message* fetion_message_new()
{
	Message* msg = (Message*)malloc(sizeof(Message));
	memset(msg , 0 , sizeof(Message));
	msg->sipuri = NULL;
	msg->pguri = NULL;
	msg->message = NULL;
	return msg;
}

void fetion_message_set_pguri(Message* msg , const char* sipuri)
{
	if(msg == NULL){
		debug_error("Message is NULL , at(fetion_message_set_sipuri)");
		return;
	}
	msg->pguri = (char*)malloc(strlen(sipuri) + 1);
	memset(msg->pguri , 0 , strlen(sipuri) + 1);
	strcpy(msg->pguri , sipuri);
}

void fetion_message_set_sipuri(Message* msg , const char* sipuri)
{
	if(msg == NULL){
		debug_error("Message is NULL , at(fetion_message_set_sipuri)");
		return;
	}
	msg->sipuri = (char*)malloc(strlen(sipuri) + 1);
	memset(msg->sipuri , 0 , strlen(sipuri) + 1);
	strcpy(msg->sipuri , sipuri);
}

void fetion_message_set_message(Message* msg , const char* message)
{
	if(msg == NULL)
	{
		debug_error("Message is NULL , at(fetion_message_set_message)");
		return;
	}
	msg->message = (char*)malloc(strlen(message) + 1);
	memset(msg->message , 0 , strlen(message) + 1);
	strcpy(msg->message , message);
}

void fetion_message_set_time(Message* msg , struct tm sendtime)
{
	if(msg == NULL)
	{
		debug_error("Message is NULL , at(fetion_message_set_time)");
		return;
	}
	msg->sendtime = sendtime;
}

void fetion_message_set_callid(Message* msg , int callid)
{
	if(msg == NULL)
	{
		debug_error("Message is NULL , at(fetion_message_set_callid)");
		return;
	}
	msg->callid = callid;
}

void fetion_message_free(Message* msg)
{
	if(msg != NULL)
	{
		if(msg->sipuri != NULL)
			free(msg->sipuri);
		if(msg->message != NULL)
			free(msg->message);
		free(msg);
	}
}

struct unacked_list *unacked_list_new(Message *message)
{
	struct unacked_list *list =
			(struct unacked_list*)malloc(sizeof(struct unacked_list));
	list->timeout = 0;
	list->message = message;
	list->next = list->pre = list;
	return list;
}

void unacked_list_append(struct unacked_list *head
			   	, struct unacked_list *newnode)
{
	head->next->pre = newnode;
	newnode->next = head->next;
	newnode->pre = head;
	head->next = newnode;
}

void unacked_list_remove(struct unacked_list *head,
				struct unacked_list *delnode)
{
	delnode->next->pre = delnode->pre;
	delnode->pre->next = delnode->next;
	if(delnode->timeout && head->timeout)
		head->timeout --;
}

char* contruct_message_sip(const char *sid, Message *msg)
{
	char *res;
	char time[128];
	char buffer[2048];
	struct tm st = msg->sendtime;
	
	memset(time , 0 , sizeof(time));
	st.tm_hour -= 8;
	strftime(time , sizeof(time),
			", %d Sep %Y %T GMT" , &st);	

	snprintf(buffer , 2047 , "M %s SIP-C/4.0\r\n"
		"I: 15\r\n"
		"Q: 5 M\r\n"
		"F: %s\r\n"
		"C: text/html-fragment\r\n"
		"K: SaveHistory\r\n"
		"D: %s\r\n"
		"BK: 1\r\n"
		"XI: BB6EE2B50BB01CA526C194D0C99B99FE\r\n\r\n%s",
		sid , msg->sipuri , time,
	   	msg->message);

	res = (char*)malloc(strlen(buffer) + 1);
	memset(res , 0 , strlen(buffer) + 1);
	strcpy(res , buffer);

	return res;

}
