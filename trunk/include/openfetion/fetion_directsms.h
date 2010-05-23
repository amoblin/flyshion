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

#ifndef FETION_DIRECTSMS_H
#define FETION_DIRECTSMS_H

#define PIC_SUCCESS 1
#define PIC_ERROR -1
#define UNKNOW_ERROR -2

#define SEND_SMS_SUCCESS 1
#define SEND_SMS_NEED_AUTHENTICATION -1
#define SEND_SMS_OTHER_ERROR -2

#define DSMS_OPTION_SUCCESS 1
#define DSMS_OPTION_FAILED -1

extern int fetion_directsms_send_option(User *user
		, const char *response);

extern int fetion_directsms_send_subscribe(User *user
		, const char *code , char **error);

extern int fetion_directsms_send_sms(User *user
		, const char *to , const char *msg);
#endif
