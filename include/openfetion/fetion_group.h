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

#ifndef FETION_GROUP_H
#define FETION_GROUP_H

#define foreach_pg_group(head , cur) \
    for(cur = head; (cur = cur->next) != head ;)

/**
 * send get group list request to server
 * @param user Global User object
 */
extern void pg_group_get_list(User *user);

/**
 * parse the response string for get group list request
 * @param sipmsg The response string
 * @return head of group list
 */
extern PGGroup *pg_group_parse_list(const char *sipmsg);

/**
 * send a get group information request to server
 * @param user Global User object
 * @param pg To specify which group to get information for
 */
extern void pg_group_get_info(User *user , PGGroup *pg);

/**
 * send a group subscribtion request to server
 * @param user Global User object
 * @param pg To specify which group to subscribe
 */
extern void pg_group_subscribe(User *user , PGGroup *pg);

/**
 * parse the response string of get group info request
 * @param pg The head of the group list
 * @param sipmsg The response string to be parsed
 */
extern void pg_group_parse_info(PGGroup *pg , const char *sipmsg );

/**
 * send a get group members request to server
 * @param user Global User object
 * @param pg To specify which group to get members for
 */
extern void pg_group_get_group_members(User *user , PGGroup *pg);

/**
 * parse the response string of get members request
 * @param pggroup The head of group list
 * @param sipmsg The response string
 */
extern void pg_group_parse_member_list(PGGroup *pggroup , const char *sipmsg);


#define foreach_pg_member(head , cur) \
    for(cur = head; (cur = cur->next)!= head ;)

/**
 * Construct a PGGroupMember object
 */
extern PGGroupMember *pg_group_member_new();

/**
 * parse the member presence information pushed from the server
 * @param pg The head of the group list
 * @param the notification sip message
 */
extern void pg_group_parse_member(PGGroup *pg , const char *sipmsg);

/**
 * send update group member information request to server
 * and sip message containing user information will be pushed from the server
 * @param user Global User object
 * @param To specify which group will update its member information
 */
extern void pg_group_update_group_info(User *user , PGGroup *pg);

/**
 * get member count of a specified group
 * @param pg The group to be calculated
 * @return the member count of the specified group
 */
extern int pg_group_get_member_count(PGGroup *pg);

/**
 * parse the body of respones string pushed from the server after function 
 * "pg_group_update_group_info()" was called
 * @param the information sip message
 * @return A new contact parsed from the xml
 */
extern Contact* pg_group_parse_contact_info(const char* xml);

/**
 * send a dialog invitation to a specified group
 * @param user Global User object
 * @param pg To specfy which group the invitation will be sent to 
 */
extern void pg_group_send_invitation(User *user , PGGroup *pg);

/**
 * send a group message to a specified group
 * @param user Global User object
 * @param pg To specify whicl group the message will be sent to
 * @param message The message body
 */
extern void pg_group_send_message(User *user , PGGroup *pg , const char *message);

/**
 * send a group sms to a specified group
 * @param user Global User object
 * @param pg To specify whicl group the message will be sent to
 * @param message The message body
 */
extern void pg_group_send_sms(User *user , PGGroup *pg , const char *message);

/**
 * send an acknowledge message after receiving the dialog invitation response message
 * @param user Global User object
 * @param sipmsg The dialog invitation response message
 */
extern void pg_group_send_invite_ack(User *user , const char *sipmsg);


#endif
