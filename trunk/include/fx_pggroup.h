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

#ifndef FX_PGGROUP_H
#define FX_PGGROUP_H

#define MEMBER_PROTRAIT_SIZED 30

enum{
    	M_PIXBUF_COL = 0 ,
	M_TEXT_COL ,
	M_NICKNAME_COL ,
	M_SIPURI_COL ,
	M_STATE_COL , 
	M_IDENTITY_COL ,
	M_COL_NUM
};

extern FxPGGroup *fx_pggroup_new(FxMain *fxmain , PGGroup *pggroup);

extern void fx_pggroup_initialize(FxPGGroup *fxpggroup);

extern void pg_add_message(FxPGGroup* fxpggroup , const char* msg
	       	, const struct tm* datetime , PGGroupMember *contact);

#endif
