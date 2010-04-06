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

#ifndef FETION_LOGIN_H
#define FETION_LOGIN_H
enum
{
	PASSWORD_ERROR_MAX = 0	
};
/*public method*/
extern char* ssi_auth_action(User* user);

extern char* sipc_reg_action(User* user);

extern void parse_ssi_auth_response(const char* ssi_response , User* user);

extern void parse_sipc_reg_response(const char* reg_response , char** nouce , char** key) ;

extern void parse_sipc_auth_response(const char* auth_response , User* user);

extern char* sipc_aut_action(User* sip , const char* response);

extern char* generate_response(const char* nouce , const char* userid , const char* password , const char* publickey , const char* key) ;

extern void generate_pic_code(User* user);

/*private method*/
extern char* generate_auth_body(User* user);

extern void parse_personal_info(xmlNodePtr node , User* user);

extern void parse_contact_list(xmlNodePtr node , User* user);

extern void parse_stranger_list(xmlNodePtr node , User* user);

extern void parse_ssi_auth_success(xmlNodePtr node , User* user);

extern void parse_ssi_auth_failed(xmlNodePtr node , User* user);

extern unsigned char* strtohex(const char* in , int* len) ;

extern char* hextostr(const unsigned char* in , int len) ;

extern char* hash_password_v1(const unsigned char* b0 , int b0len , const unsigned char* password , int psdlen);

extern char* hash_password_v2(const char* userid , const char* passwordhex) ;

extern char* hash_password_v4(const char* userid , const char* password) ;

extern char* generate_aes_key() ;

extern char* generate_cnouce() ;

extern unsigned char* decode_base64(const char* in , int* len);
#endif
