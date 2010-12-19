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

#ifndef FETION_BUDDYLIST_H
#define FETION_BUDDYLIST_H

extern int fetion_buddylist_create(User* user , const char* name);

extern int fetion_buddylist_delete(User* user , int id);

extern int fetion_buddylist_edit(User* user , int id , const char* name);

/*private*/
extern char* generate_create_buddylist_body(const char* name);

extern char* generate_delete_buddylist_body(int id);

extern char* generate_edit_buddylist_body(int id , const char* name);

extern int parse_create_buddylist_response(User* user , const char* sipmsg);

extern void parse_delete_buddylist_response(User* user , const char* sipmsg);

#endif
