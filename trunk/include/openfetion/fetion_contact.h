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

#ifndef FETION_CONTACT_H
#define FETION_CONTACT_H

typedef enum
{
	FETION_NO = 1,
	MOBILE_NO 
} NumberType;

typedef enum
{
	BUDDY_OK = 200 ,
	BUDDY_SAME_USER_DAILY_LIMIT = 486 ,
	BUDDY_USER_EXIST = 521 ,
	BUDDY_BAD_REQUEST = 400
} AddBuddyType;

#define foreach_contactlist(head , cl) \
	for(cl = head ; (cl = cl->next) != head ;)

/**
 * constuct a Contact object
 */
extern Contact* fetion_contact_new();

/**
 * append a contact object to the contact list
 */
extern void fetion_contact_list_append(Contact* cl , Contact* contact);

/**
 * find the contact in the contact list by the specified userid
 */
extern Contact* fetion_contact_list_find_by_userid(Contact* contactlist , const char* userid);

/**
 * find the contact in the contact list by the specified sipuri
 */
extern Contact* fetion_contact_list_find_by_sipuri(Contact* contactlist , const char* sipuri);

/**
 * remove a contact with the specified userid from the contact list
 */
extern void fetion_contact_list_remove_by_userid(Contact* contactlist , const char* userid);

/**
 * free the resource of the while contact list
 * @param contactlist the head of the contact list
 */
extern void fetion_contact_list_free(Contact* contactlist);

/**
 * test whether there is an ungrouped contact in the contact list
 * @param contactlist the head of the contact list
 * @return 1 if there is ungrouped contact , or else -1
 */
extern int fetion_contact_has_ungrouped(Contact *contactlist);

/**
 * test whether there is a stranger contact in the contact list
 * @param contactlist the head of the contact list
 * @return 1 if there is a stranger contact , or else -1
 */
extern int fetion_contact_has_strangers(Contact *contactlist);

/**
 * subscribe the contact`s information after login,and then 
 * the presence information of contacts will be pushed from the server
 * @return 1 if success , or else -1
 */
extern int fetion_contact_subscribe_only(User* user);

/**
 * get the contact`s detail information from the sipc server
 * @param user Global User object
 * @param userid The user-id of the contact for which to get information
 * @return The Contact object with detail information if success , or else NULL
 */
extern Contact* fetion_contact_get_contact_info(User* user , const char* userid);

/**
 * get the contact`s detail information from the sipc server with the specified number type
 * @param user Global User object
 * @param userid The user-id of the contact for which to get information
 * @param no Fetion number if 'nt' is FETION_NO , or phone number if 'nt' is MOBILE_NO
 * @param nt Number type to specify the type of the second argument
 * @return The Contact object with detail information if success , or else NULL
 */
extern Contact* fetion_contact_get_contact_info_by_no(User* user , const char* no , NumberType nt);

/**
 * modify whether to show your mobile phone number to the user specified by userid
 * @param user Global User object
 * @param userid The user-id of the user for whom to set permission
 * @param show If set to 1,then show phone number to this user , while 0 not show
 * @return 1 if success , or else -1
 */
extern int fetion_contact_set_mobileno_permission(User* user , const char* userid , int show);

/**
 * modify the display name of the user specified by the userid
 * @return 1 if success , or else -1
 */
extern int fetion_contact_set_displayname(User* user , const char* userid , const char* name);

/**
 * move the user specified by userid to the new user group specified by the buddylist
 * @param user Global User object
 * @param userid To specify the user to be moved
 * @param buddylist To specify the user group to be moved into
 * @return 1 if success , of else -1
 */
extern int fetion_contact_move_to_group(User* user , const char* userid , int buddylist);

/**
 * delete a user specified by userid from your contact
 * @param user Global User object
 * @param userid To specify the user to be deleted
 */
extern int fetion_contact_delete_buddy(User* user , const char* userid);

/**
 * send an Add-Buddy request to sipc server
 * @param user Global User object
 * @param no Fetion number is 'notype' is FETION_NO , mobile number if 'notype' it MOBILE_NO
 * @param notype To specify the number type
 * @param buddylist To specify which user group the new user will be located
 * @param localname To specify the local name of the new user
 * @param desc To specify your display name that will be contained in the request sent to the new uesr
 * @param phraseid To specify which Add-Buddy phrase to use
 * @return the contact contains the basic information of the new uesr
 */
extern Contact* fetion_contact_add_buddy(User* user , const char* no
					 , NumberType notype , int buddylist
					  , const char* localname , const char* desc
					  , int phraseid , int* statuscode);

extern Contact* fetion_contact_handle_contact_request(User* user, const char* sipuri
					   , const char* userid , const char* localname
					   , int buddylist , int result);


extern void parse_set_displayname_response(User* user , const char* userid , const char* sipmsg);

extern void parse_set_mobileno_permission_response(User* user , const char* sipmsg);

extern Contact* parse_contact_info_by_no_response(const char* sipmsg);

extern void parse_add_buddy_verification(User* user , const char* str);

#endif
