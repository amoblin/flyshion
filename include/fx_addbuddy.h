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
