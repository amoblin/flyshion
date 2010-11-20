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


FxCode* fx_code_new(FxMain* fxmain , const char* reason
		, const char* tips , ErrorType error)
{
	FxCode* fxcode = (FxCode*)malloc(sizeof(FxCode));

	memset(fxcode , 0 , sizeof(FxCode));
	fxcode->fxmain = fxmain;
	if(reason != NULL)
		strcpy(fxcode->reason , reason);
	if(tips != NULL)
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
	char codePath[256];

	fxcode->dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(fxcode->dialog) , _("Please entry your verification code"));
	icon = gdk_pixbuf_new_from_file(SKIN_DIR"warning.svg" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxcode->dialog) , icon);
	vbox = GTK_DIALOG(fxcode->dialog)->vbox;
	action_area = GTK_DIALOG(fxcode->dialog)->action_area;
	fixed = gtk_fixed_new();
	gtk_widget_set_usize(fxcode->dialog , 460 , 250);
	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"warning.svg" , 96 , 96 , NULL);
	warningpic = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_fixed_put(GTK_FIXED(fixed) , warningpic , 20 , 20 );
	
	fxcode->reasonlabel = gtk_label_new(fxcode->reason);
	gtk_widget_set_usize(fxcode->reasonlabel , 300 , 40);
	gtk_label_set_line_wrap(GTK_LABEL(fxcode->reasonlabel) , TRUE );
	gtk_label_set_justify(GTK_LABEL(fxcode->reasonlabel), GTK_JUSTIFY_FILL);
	gtk_fixed_put(GTK_FIXED(fixed) , fxcode->reasonlabel , 150 , 20);

	fxcode->codeentry = gtk_entry_new();
	gtk_widget_set_usize(fxcode->codeentry , 300 , 30);
	gtk_fixed_put(GTK_FIXED(fixed)  , fxcode->codeentry , 150 , 70);
	g_signal_connect(fxcode->codeentry, "key_press_event" , G_CALLBACK(fx_code_on_key_pressed), fxcode);

	memset(codePath , 0 , sizeof(codePath));
	sprintf(codePath , "%s/code.gif" , fxcode->fxmain->user->config->globalPath);
	pb = gdk_pixbuf_new_from_file_at_size(codePath , 130 , 36 , NULL);
	fxcode->codepic = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_widget_set_tooltip_markup(fxcode->codepic , _("Click here getting new verification code"));
	fxcode->codebox = gtk_event_box_new();
	g_signal_connect(G_OBJECT(fxcode->codebox)
					, "button_press_event"
					, GTK_SIGNAL_FUNC(fx_code_code_event_func)
				   , fxcode);
	g_signal_connect(G_OBJECT(fxcode->codebox)
				   , "enter_notify_event"
				   , GTK_SIGNAL_FUNC(fx_code_code_event_func)
				   , fxcode);
	g_signal_connect(G_OBJECT(fxcode->codebox)
				   , "leave_notify_event"
				   , GTK_SIGNAL_FUNC(fx_code_code_event_func)
				   , fxcode);
	gtk_container_add(GTK_CONTAINER(fxcode->codebox) , fxcode->codepic);
	gtk_fixed_put(GTK_FIXED(fixed) , fxcode->codebox , 150 , 120);


	fxcode->tiplabel = gtk_label_new(fxcode->tip);
	gtk_widget_set_usize(fxcode->tiplabel , 450 , 40);
	gtk_label_set_line_wrap(GTK_LABEL(fxcode->tiplabel) , TRUE );
	gtk_label_set_justify(GTK_LABEL(fxcode->tiplabel), GTK_JUSTIFY_FILL);
	gtk_fixed_put(GTK_FIXED(fixed) , fxcode->tiplabel , 20 , 160);

	ok_button = gtk_button_new_with_label(_("OK"));
	GTK_WIDGET_SET_FLAGS(ok_button , GTK_CAN_FOCUS);
	gtk_widget_grab_focus(ok_button);
	g_signal_connect(ok_button , "clicked" , G_CALLBACK(fx_code_on_ok_clicked) , fxcode->dialog);
	gtk_box_pack_start_defaults(GTK_BOX(action_area) , ok_button);

	cancel_button = gtk_button_new_with_label(_("Cancel"));
	g_signal_connect(cancel_button , "clicked" , G_CALLBACK(fx_code_on_cancel_clicked) , fxcode->dialog);
	gtk_box_pack_start_defaults(GTK_BOX(action_area) , cancel_button);

	gtk_box_pack_start_defaults(GTK_BOX(vbox) , fixed);
	gtk_widget_show_all(fxcode->dialog);
}

void fx_code_code_event_func(GtkWidget* UNUSED(widget) , GdkEventButton* event , gpointer data)
{
	FxCode *fxcode = (FxCode*)data;
	User *user = fxcode->fxmain->user;
	GdkCursor*  cursor;
	char codePath[256];
	GdkPixbuf *pb;

	switch(event->type)
	{
		case GDK_BUTTON_PRESS :
			generate_pic_code(user);
			bzero(codePath , sizeof(codePath));
			sprintf(codePath , "%s/code.gif" , fxcode->fxmain->user->config->globalPath);
			pb = gdk_pixbuf_new_from_file_at_size(codePath , 130 , 36 , NULL);
			gtk_image_set_from_pixbuf(GTK_IMAGE(fxcode->codepic) , pb);
			g_object_unref(pb);
			break;
		case GDK_ENTER_NOTIFY :
			cursor = gdk_cursor_new (GDK_HAND2);
			gdk_window_set_cursor(fxcode->dialog->window,cursor);
			break;
		case GDK_LEAVE_NOTIFY :
			cursor = gdk_cursor_new (GDK_LEFT_PTR);
			gdk_window_set_cursor(fxcode->dialog->window,cursor);
			break;
		default:
			break;
	};
}

void fx_code_on_ok_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	gtk_dialog_response(GTK_DIALOG(data) , GTK_RESPONSE_OK);
}

void fx_code_on_cancel_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	gtk_dialog_response(GTK_DIALOG(data) , GTK_RESPONSE_CANCEL);
}

gboolean fx_code_on_key_pressed(GtkWidget* UNUSED(widget), GdkEventKey* event, gpointer data)
{
	FxCode *fxcode = (FxCode*)data;

	if(event->keyval == GDK_Return || event->keyval == GDK_ISO_Enter || event->keyval == GDK_KP_Enter)
	{
		fx_code_on_ok_clicked(NULL, fxcode->dialog);
		return TRUE;
	}
	return FALSE;
}
