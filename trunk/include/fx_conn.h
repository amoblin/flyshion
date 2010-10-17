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
#ifndef FX_CONN_H
#define FX_CONN_H
#include <fx_include.h>

struct conn_list {
	FetionConnection *conn;
	struct conn_list *next;
	struct conn_list *pre;
};

#define foreach_conn_list(head,cur) \
	for(cur=head;(cur=cur->next)!=head;)

struct conn_list *connlst;

extern void fx_conn_init(FxMain *fxmain);

extern void fx_conn_append(FetionConnection *conn);

extern void fx_conn_offline(FxMain *fxmain);

extern int fx_conn_check_action(FxMain *fxmain);

extern int fx_conn_connect(FxMain *fxmain);

extern int fx_conn_reconnect(FxMain *fxmain, int state);

extern int fx_conn_offline_login(FxMain *fxmain);
#endif
