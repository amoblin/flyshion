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

#ifndef FETION_MESSAGE_H
#define FETION_MESSAGE_H

extern Message* fetion_message_new();

extern void fetion_message_set_sipuri(Message* msg , const char* sipuri);

extern void fetion_message_set_pguri(Message* msg , const char* sipuri);

extern void fetion_message_set_message(Message* msg , const char* message);

extern void fetion_message_set_time(Message* msg , struct tm sendtime);

extern void fetion_message_free(Message* msg);

#endif
