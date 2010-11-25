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

#ifndef FX_MANY_H
#define FX_MANY_H
enum
{
	S_CHECK_COL = 0 ,
	S_PIXBUF_COL ,
	S_TEXT_COL ,
	S_SIPURI_COL ,
	S_NAME_COL ,
	S_COLS_NUM
};
enum
{
	C_PIXBUF_COL = 0 ,
	C_NAME_COL , 
	C_SIPURI_COL , 
	C_COLS_NUM
};
extern FxMany* fx_many_new(FxMain* fxmain);

extern void fx_many_initialize(FxMany* fxmany);

#endif
