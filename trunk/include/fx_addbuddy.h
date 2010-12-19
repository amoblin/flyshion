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

#ifndef FX_ADDBUDDY_H
#define FX_ADDBUDDY_H

enum groupcolumn
{
	GROUP_NAME_COL = 0,
	GROUP_ID_COL = 1,
	GROUP_COL_NUMS = 2
};

extern FxAddbuddy* fx_addbuddy_new(FxMain* fxmain);

extern void fx_addbuddy_initialize(FxAddbuddy* fxaddbuddy);

extern GtkTreeModel* fx_addbuddy_create_group_model(FxAddbuddy* fxaddnuddy);

extern void fx_addbuddy_bind(FxAddbuddy* fxaddbuddy);

extern void fx_addbuddy_on_ok_clicked(GtkWidget *UNUSED(widget) , gpointer data);

extern void fx_addbuddy_on_cancel_clicked(GtkWidget *UNUSED(widget) , gpointer data);

extern void fx_addbuddy_on_phrase_change(GtkWidget *widget , gpointer data);

extern void fx_addbuddy_no_type_change(GtkWidget *UNUSED(widget) , gpointer data);
#endif
