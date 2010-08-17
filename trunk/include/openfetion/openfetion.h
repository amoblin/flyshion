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

#ifndef OPENFETION_H
#define OPENFETION_H
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <libxml/parser.h>
#include <pthread.h>
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <fetion_types.h>
#include <fetion_list.h>
#include <fetion_debug.h>
#include <fetion_message.h>
#include <fetion_connection.h>
#include <fetion_sip.h>
#include <fetion_user.h>
#include <fetion_contact.h>
#include <fetion_login.h>
#include <fetion_config.h>
#include <fetion_conversation.h>
#include <fetion_buddylist.h>
#include <fetion_history.h>
#include <fetion_share.h>
#include <fetion_directsms.h>
#include <fetion_group.h>
#endif
