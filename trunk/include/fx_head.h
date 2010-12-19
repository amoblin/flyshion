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
