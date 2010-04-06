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

#ifndef FETION_CONVERSION_H
#define FETION_CONVERSION_h


extern Conversation* fetion_conversation_new(User* user , const char* sipuri , FetionSip* sip);

extern void fetion_conversation_send_sms(Conversation* user , const char* msg);

extern void fetion_conversation_send_sms_to_myself(Conversation* conversation , const char* message);

extern void fetion_conversation_send_sms_to_phone(Conversation* conversation , const char* message);

extern int fetion_conversation_send_sms_to_phone_with_reply(Conversation* conversation , const char* message , int* daycount , int* monthcount);

extern int fetion_conversation_invite_friend(Conversation* conversation);

extern int fetion_conversation_send_nudge(Conversation* conversation);

/*private*/

extern char* generate_invite_friend_body(const char* sipuri);

extern char* generate_send_nudge_body();

extern void fetion_conversation_parse_send_sms(const char* xml , int* daycount , int* mountcount);

#endif
