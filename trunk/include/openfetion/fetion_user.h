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

#ifndef FETION_USER_H
#define FETION_USER_H

#define STATE_ONLINE 400 
#define STATE_RIGHTBACK 300
#define STATE_AWAY 100
#define STATE_BUSY   600
#define STATE_OUTFORLUNCH 500
#define STATE_ONTHEPHONE 150
#define STATE_MEETING 850
#define STATE_DONOTDISTURB 800
#define STATE_HIDDEN 0
#define STATE_NOTAPRESENCE -1


extern User* fetion_user_new(const char* no , const char* password);

extern void fetion_user_set_userid(User* user , const char* userid);

extern void fetion_user_set_sid(User* user , const char* sId);

extern void fetion_user_set_mobileno(User* user , const char* mobileno);

extern void fetion_user_set_sip(User* user , FetionSip* sip);

extern void fetion_user_set_config(User* user , Config* config);

extern void fetion_user_set_verification_code(User* user , const char* code);

extern void fetion_user_free(User* user);

extern void fetion_user_save(User* user);

extern void fetion_user_load(User* user);

extern int fetion_user_upload_portrait(User* user , const char* filename);

extern int fetion_user_download_portrait(User* user , const char* sipuri);

extern int fetion_user_set_state(User* user , StateType state);

extern int fetion_user_set_moodphrase(User* user , const char* moodphrase);

extern int fetion_user_update_info(User* user);

extern int fetion_user_keep_alive(User* user);

extern Group* fetion_group_new();

extern void fetion_group_list_append(Group* grouplist , Group* group);

extern void fetion_group_remove(Group** grouplist , int groupid);

extern Group* fetion_group_list_find_by_id(Group* grouplist , int id);

extern void fetion_group_list_free(Contact* grouplist);

extern Verification* fetion_verification_new();

extern void fetion_verification_free(Verification* ver);

/*private*/
extern int fetion_user_download_portrait_again(const char* filepath , const char* buf , Proxy *proxy);

extern Contact* fetion_user_parse_presence_body(const char* body , User* user);

extern Contact* fetion_user_parse_syncuserinfo_body(const char* body , User* user);

extern char* generate_set_state_body(StateType state);

extern char* generate_set_moodphrase_body(const char* customConfigVersion
				, const char* customConfig , const char* personalVersion ,  const char* moodphrase);

extern char* generate_update_information_body(User* user);

extern char* generate_keep_alive_body();

extern void parse_set_moodphrase_response(User* user , const char* sipmsg);
#endif
