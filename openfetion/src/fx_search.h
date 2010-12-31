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
 *   51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA.            *
 *                                                                         *
 *   OpenSSL linking exception                                             *
 *   --------------------------                                            *
 *   If you modify this Program, or any covered work, by linking or        *
 *   combining it with the OpenSSL project's "OpenSSL" library (or a       *
 *   modified version of that library), containing parts covered by        *
 *   the terms of OpenSSL/SSLeay license, the licensors of this            *
 *   Program grant you additional permission to convey the resulting       *
 *   work. Corresponding Source for a non-source form of such a            *
 *   combination shall include the source code for the parts of the        *
 *   OpenSSL library used as well as that of the covered work.             *
 ***************************************************************************/

#ifndef FX_SEARCH_H
#define FX_SEARCH_H

enum
{
	H_PIXBUF_COL = 0 ,
	H_TEXT_COL ,
	H_SIPURI_COL , 
	H_USERID_COL ,
	H_NAME_COL ,
	H_IMPRESSION_COL ,
	H_PHONENUM_COL ,
	H_CARRIER_COL ,
	H_STATE_COL , 
	H_IDENTITY_COL ,
	H_RELATIONSTATUS_COL ,
	H_CARRIERSTATUS_COL , 
	H_SERVICESTATUS_COL ,
	H_COL_NUM
};

extern FxSearch *fx_search_new(FxMain *fxmain);

extern void fx_search_initialize(FxSearch *searcn , GtkTreeModel *model
		, const char *str , int x , int y);

#endif
