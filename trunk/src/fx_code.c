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

#include "fx_include.h"


FxCode* fx_code_new(FxMain* fxmain , const char* reason , const char* tips , ErrorType error)
{
	FxCode* fxcode = (FxCode*)malloc(sizeof(FxCode));

	DEBUG_FOOTPRINT();

	memset(fxcode , 0 , sizeof(FxCode));
	fxcode->fxmain = fxmain;
	strcpy(fxcode->reason , reason);
	strcpy(fxcode->tip , tips);
	fxcode->error = error;
	return fxcode;
}

void fx_code_initialize(FxCode *fxcode)
{
	GtkWidget *fixed , *warningpic;
	GtkWidget *vbox , *action_area;
	GtkWidget *ok_button , *cancel_button;
	GdkPixbuf *pb , *icon;
	char codePath[128];

	DEBUG_FOOTPRINT();

	fxcode->dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(fxcode->dialog) , "请输入验证码");
	icon = gdk_pixbuf_new_from_file(SKIN_DIR"warning.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxcode->dialog) , icon);
	vbox = GTK_DIALOG(fxcode->dialog)->vbox;
	action_area = GTK_DIALOG(fxcode->dialog)->action_area;
	fixed = gtk_fixed_new();
	gtk_widget_set_usize(fxcode->dialog , 460 , 250);
	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"warning_icon.png" , 96 , 96 , NULL);
	warningpic = gtk_image_new_from_pixbuf(pb);
	gtk_fixed_put(GTK_FIXED(fixed) , warningpic , 20 , 20 );
	
	fxcode->reasonlabel = gtk_label_new(fxcode->reason);
	gtk_widget_set_usize(fxcode->reasonlabel , 300 , 40);
	gtk_label_set_line_wrap(GTK_LABEL(fxcode->reasonlabel) , TRUE );
	gtk_label_set_justify(GTK_LABEL(fxcode->reasonlabel), GTK_JUSTIFY_FILL);
	gtk_fixed_put(GTK_FIXED(fixed) , fxcode->reasonlabel , 150 , 20);

	fxcode->codeentry = gtk_entry_new();
	gtk_widget_set_usize(fxcode->codeentry , 300 , 30);
	gtk_fixed_put(GTK_FIXED(fixed)  , fxcode->codeentry , 150 , 70);

	bzero(codePath , sizeof(codePath));
	sprintf(codePath , "%s/code.gif" , fxcode->fxmain->user->config->globalPath);
	pb = gdk_pixbuf_new_from_file_at_size(codePath , 130 , 36 , NULL);
	fxcode->codepic = gtk_image_new_from_pixbuf(pb);
	fxcode->codebox = gtk_event_box_new();
	g_signal_connect(G_OBJECT(fxcode->codebox)
					, "button_press_event"
					, GTK_SIGNAL_FUNC(fx_code_code_event_func)
				   , NULL);
	g_signal_connect(G_OBJECT(fxcode->codebox)
				   , "enter_notify_event"
				   , GTK_SIGNAL_FUNC(fx_code_code_event_func)
				   , fxcode->dialog);
	g_signal_connect(G_OBJECT(fxcode->codebox)
				   , "leave_notify_event"
				   , GTK_SIGNAL_FUNC(fx_code_code_event_func)
				   , fxcode->dialog);
	gtk_container_add(GTK_CONTAINER(fxcode->codebox) , fxcode->codepic);
	gtk_fixed_put(GTK_FIXED(fixed) , fxcode->codebox , 150 , 120);


	fxcode->tiplabel = gtk_label_new(fxcode->tip);
	gtk_widget_set_usize(fxcode->tiplabel , 450 , 40);
	gtk_label_set_line_wrap(GTK_LABEL(fxcode->tiplabel) , TRUE );
	gtk_label_set_justify(GTK_LABEL(fxcode->tiplabel), GTK_JUSTIFY_FILL);
	gtk_fixed_put(GTK_FIXED(fixed) , fxcode->tiplabel , 20 , 160);

	ok_button = gtk_button_new_with_label("确定");
	g_signal_connect(ok_button , "clicked" , G_CALLBACK(fx_code_on_ok_clicked) , fxcode->dialog);
	gtk_box_pack_start_defaults(GTK_BOX(action_area) , ok_button);

	cancel_button = gtk_button_new_with_label("取消");
	g_signal_connect(cancel_button , "clicked" , G_CALLBACK(fx_code_on_cancel_clicked) , fxcode->dialog);
	gtk_box_pack_start_defaults(GTK_BOX(action_area) , cancel_button);

	gtk_box_pack_start_defaults(GTK_BOX(vbox) , fixed);
	gtk_widget_show_all(fxcode->dialog);
	gtk_widget_hide(fxcode->dialog);
}
void fx_code_code_event_func(GtkWidget* widget , GdkEventButton* event , gpointer data)
{
	GtkWidget* dialog = (GtkWidget*)data;
	GdkCursor*  cursor;

	DEBUG_FOOTPRINT();

	switch(event->type)
	{
		case GDK_BUTTON_PRESS :
			break;
		case GDK_ENTER_NOTIFY :
			cursor = gdk_cursor_new (GDK_HAND2);
			gdk_window_set_cursor(dialog->window,cursor);
			break;
		case GDK_LEAVE_NOTIFY :
			cursor = gdk_cursor_new (GDK_LEFT_PTR);
			gdk_window_set_cursor(dialog->window,cursor);
			break;
		default:
			break;
	};
}
void fx_code_on_ok_clicked(GtkWidget* widget , gpointer data)
{
	DEBUG_FOOTPRINT();

	gtk_dialog_response(GTK_DIALOG(data) , GTK_RESPONSE_OK);
}

void fx_code_on_cancel_clicked(GtkWidget* widget , gpointer data)
{
	DEBUG_FOOTPRINT();

	gtk_dialog_response(GTK_DIALOG(data) , GTK_RESPONSE_CANCEL);
}