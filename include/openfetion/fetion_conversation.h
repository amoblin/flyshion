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

/**
 * construct a conversation object
 * @uesr Global User object
 * @sipuri To specify the user in the conversation
 */
extern Conversation* fetion_conversation_new(User* user , const char* sipuri , FetionSip* sip);

/**
 * send a message to the user in this conversation
 * @note before you send a message to an online buddy , in other word ,
 * the buddy`s state is larger than 0 , then you need to send an invitation 
 * to the buddy first using function "fetion_conversation_invite_friend()"
 * or else the message will fail to send
 * @param conv To specify the conversation to send a sms to 
 * @param msg the message to be sent
 */
extern void fetion_conversation_send_sms(Conversation* conv , const char* msg);

/**
 *  send a sms to yourself
 */
extern void fetion_conversation_send_sms_to_myself(Conversation* conversation , const char* message);

/**
 * send a message directly to the user`s phone in the specified conversation
 * @param conversation To specify the user to whom the message will be sent
 * @param message The message body
 * @return 1 if success , or else -1
 */
extern int fetion_conversation_send_sms_to_phone(Conversation* conversation , const char* message);

/**
 * send a message directly to the user`s phone in the specified conversation
 * and parse the response message.
 * @param conversation To specify the buddy to whom the message will be sent
 * @param message The message body
 * @param daycount Will be filled with the count of messages you send in the current day
 * @param mountcount Will be filled with the count of messages you send in the current month
 * @return 1 if success , or else -1
 */
extern int fetion_conversation_send_sms_to_phone_with_reply(Conversation* conversation
       	, const char* message , int* daycount , int* monthcount);

/**
 * invite an online buddy to a conversation
 * @param conversation To specify the buddy to whom the message will be sent
 * @return 1 if success , or else -1
 */
extern int fetion_conversation_invite_friend(Conversation* conversation);

/**
 * send a window nudge to a buddy
 * @param conversation To specify the buddy to whom the nudge will be sent
 * @return 1 if success , or else -1
 */
extern int fetion_conversation_send_nudge(Conversation* conversation);


#endif
