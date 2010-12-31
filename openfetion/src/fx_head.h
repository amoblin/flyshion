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

#ifndef FX_HEAD_H
#define FX_HEAD_H

#define USER_PORTRAIT_SIZE 50

extern FxHead* fx_head_new();

extern void fx_head_free(FxHead* fxhead);

extern void fx_head_initialize(FxMain* fxmain);

extern void fx_head_show(FxMain *fxmain);

extern void fx_head_bind(FxMain* fxmain);

void fx_head_set_state_image(FxMain* fxmain , StateType type);

extern void fx_head_popup_statemenu_func(GtkWidget* UNUSED(widget),
		GdkEventButton* event , gpointer data);

void fx_head_create_presence_item(int type,
		const char* message, GtkWidget* menu, FxMain* fxmain);

extern void fx_head_impre_event_func(GtkWidget* widget,
		GdkEventButton* event , gpointer data);

extern gboolean fx_head_impre_focus_out_func(GtkWidget* UNUSED(widget),
		GdkEventFocus* UNUSED(event) , gpointer data);

extern gboolean fx_head_impre_activate_func(GtkWidget* widget,
		gpointer data);

extern void fx_head_change_state_func(GtkWidget* UNUSED(widget),
		gpointer data);

extern void fx_head_change_portrait_func(GtkWidget* widget,
		GdkEventButton* event , gpointer data);

extern void* fx_head_change_portrait_thread(void* data);

extern gboolean fx_head_set_sms_clicked(GtkWidget *widget,
		gpointer data);
#endif
