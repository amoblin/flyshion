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

#ifndef FX_CODE_H
#define FX_CODE_H

extern FxCode* fx_code_new(FxMain* fxmain , const char* reason , const char* tips , ErrorType error);

extern void fx_code_initialize(FxCode* fxcode);

extern void fx_code_code_event_func(GtkWidget* UNUSED(widget) , GdkEventButton* event , gpointer data);

extern void fx_code_on_ok_clicked(GtkWidget* UNUSED(widget) , gpointer data);

extern void fx_code_on_cancel_clicked(GtkWidget* UNUSED(widget) , gpointer data);

extern gboolean fx_code_on_key_pressed(GtkWidget* UNUSED(widget), GdkEventKey* event, gpointer data);

#endif
