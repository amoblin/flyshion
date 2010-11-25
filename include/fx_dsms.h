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

#ifndef FX_DSMS_H
#define FX_DSMS_H

enum{
	PIXBUF_COL ,
	NUMBER_COL ,
	COL_SIZE
};

struct deleteargs{
	FxDSMS *fxdsms;
	GtkTreeIter iter;
};

extern FxConfirm* fx_confirm_new(FxMain *fxmain); 

extern void fx_confirm_initialize(FxConfirm *fxconfirm);

extern FxDSMS* fx_dsms_new(FxMain *fxmain);

extern void fx_dsms_initialize(FxDSMS *fxdsms);

#endif
