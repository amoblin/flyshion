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

extern Contact* fetion_contact_new();

extern void fetion_contact_list_append(Contact* contactlist , Contact* contact);

extern Contact* fetion_contact_list_find_by_userid(Contact* contactlist , const char* userid);

extern Contact* fetion_contact_list_find_by_sipuri(Contact* contactlist , const char* sipuri);

extern void fetion_contact_list_remove_by_userid(Contact** contactlist , const char* userid);

extern void fetion_contact_list_free(Contact* contactlist);

extern int fetion_contact_subscribe_only(User* user);

extern int fetion_contact_subscribe(User* user);

extern Contact* fetion_contact_get_contact_info(User* user , const char* userid);

extern Contact* fetion_contact_get_contact_info_by_no(User* user , const char* no , NumberType nt);

extern int fetion_contact_set_mobileno_permission(User* user , const char* userid , int show);

extern int fetion_contact_set_displayname(User* user , const char* userid , const char* name);

extern int fetion_contact_move_to_group(User* user , const char* userid , int buddylist);

extern int fetion_contact_delete_buddy(User* user , const char* userid);

extern Contact* fetion_contact_add_buddy(User* user , const char* no
									   , NumberType notype , int buddylist
									   , const char* localname , const char* desc
									   , int phraseid , int* statuscode);

extern Contact* fetion_contact_handle_contact_request(User* user, const char* sipuri
									   , const char* userid , const char* localname
									   , int buddylist , int result);

extern int fetion_contact_save(User* user);

extern void fetion_contact_load(User* user);

/*private */

extern char* generate_subscribe_body(const char* version);

extern char* generate_contact_info_body(const char* userid);

extern char* generate_contact_info_by_no_body(const char* no , NumberType nt);

extern char* generate_set_mobileno_perssion(const char* userid , int show);

extern char* generate_set_displayname_body(const char* userid , const char* name);

extern char* generate_move_to_group_body(const char* userid , int buddylist);

extern char* generate_delete_buddy_body(const char* userid);

extern char* generate_add_buddy_body(const char* no , NumberType notype
								, int buddylist , const char* localname
								, const char* desc , int phraseid);

extern char* generate_handle_contact_request_body(const char* sipuri
								, const char* userid , const char* localname
								, int buddylist , int result );

extern void parse_set_displayname_response(User* user , const char* userid , const char* sipmsg);

extern void parse_set_mobileno_permission_response(User* user , const char* sipmsg);

extern Contact* parse_contact_info_by_no_response(const char* sipmsg);

extern Contact* parse_add_buddy_response(const char* sipmsg , int* statuscode);

extern Contact* parse_handle_contact_request_response(const char* sipmsg);

extern void parse_add_buddy_verification(User* user , const char* str);

#endif
