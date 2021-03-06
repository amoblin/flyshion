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

#include "fx_include.h"

static gboolean key_press_func(GtkWidget *UNUSED(widget) , GdkEventKey *event
		, gpointer data);

FxMyself* fx_myself_new(FxMain* fxmain)
{
	FxMyself* fxmyself = (FxMyself*)malloc(sizeof(FxMyself));

	memset(fxmyself , 0 , sizeof(FxMyself));
	fxmyself->fxmain = fxmain;
	fxmyself->conv = fetion_conversation_new(fxmain->user , NULL , NULL);
	return fxmyself;
}

void fx_myself_bind(FxMyself* fxmyself)
{
	FxMain* fxmain = fxmyself->fxmain;
	User* user = fxmain->user;
	Config* config = user->config;
	GdkPixbuf* pb;
	char name[128];

	sprintf(name , "%s/%s.jpg" , config->iconPath ,
			user->sId );
	pb = gdk_pixbuf_new_from_file_at_size(name , 40 , 40 ,
				NULL); 
	if(pb != NULL) {
		gtk_image_set_from_pixbuf(GTK_IMAGE(fxmyself->headimage) , pb);
		gtk_window_set_icon(GTK_WINDOW(fxmyself->window) , pb);
		g_object_unref(pb);
	}
	gtk_window_set_title(GTK_WINDOW(fxmyself->window) ,
			_("Sending sms to myself"));

   	sprintf(name , "%s(%s)" , user->nickname ,
			user->sId);
	gtk_label_set_markup(GTK_LABEL(fxmyself->name_label) ,
				name);

	gtk_label_set_text(GTK_LABEL(fxmyself->impre_label) , user->impression);
}

void fx_myself_add_message(FxMyself* fxmyself , const char* message)
{
	GtkTextIter iter;

	User* user = fxmyself->fxmain->user;
	char text[4096];
	char time[30] = { 0 };
	char color[] = "blue";

	struct tm* sendtime = get_currenttime();
	GtkTextBuffer* buffer =	gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxmyself->recv_text));

	strftime(time , sizeof(time) , "%H:%M:%S" , sendtime);

	sprintf(text , "%s(%s) %s\n" , user->nickname , user->sId , time);

	gtk_text_buffer_get_end_iter(buffer , &iter );

	gtk_text_buffer_insert_with_tags_by_name(buffer , &iter , text , -1 , color
			, NULL);

	gtk_text_buffer_insert_with_tags_by_name(buffer , &iter, message , -1 ,
			"lm10" , NULL);
	
	gtk_text_buffer_insert(buffer , &iter , "\n" , -1);

	gtk_text_iter_set_line_offset (&iter, 0);
	
	fxmyself->mark = gtk_text_buffer_get_mark (buffer, "scroll");

	gtk_text_buffer_move_mark (buffer, fxmyself->mark, &iter);

	gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW(fxmyself->recv_text),
			fxmyself->mark);
} 
void fx_myself_initialize(FxMyself* fxmyself)
{
	GtkWidget* close_button;
	GtkWidget* send_button;
	GtkWidget* vbox;
	GtkWidget *action_area;

	fxmyself->window = gtk_window_new(GTK_WINDOW_TOPLEVEL); 
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (fxmyself->window), vbox);
	gtk_window_set_modal(GTK_WINDOW(fxmyself->window) , FALSE);
	gtk_widget_set_usize(fxmyself->window , 550 , 490);
	g_signal_connect(fxmyself->window , "key-press-event"
			, G_CALLBACK(key_press_func) , fxmyself);
	gtk_container_set_border_width(GTK_CONTAINER(fxmyself->window) , 10);

	fxmyself->headbox = gtk_table_new(2 , 10 , FALSE );

	fxmyself->headpix = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.svg" ,
			40 , 40 , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxmyself->window) , fxmyself->headpix);
	fxmyself->headimage = gtk_image_new_from_pixbuf(fxmyself->headpix);
	gtk_table_attach(GTK_TABLE(fxmyself->headbox) , fxmyself->headimage
								, 0 , 1 , 0 , 2
								, GTK_FILL , GTK_FILL , 3 , 0);

	fxmyself->name_label = gtk_label_new(NULL);

	gtk_label_set_justify(GTK_LABEL(fxmyself->name_label) , GTK_JUSTIFY_LEFT);
	gtk_table_attach(GTK_TABLE(fxmyself->headbox) , fxmyself->name_label 
								, 1 , 2 , 0 , 1 
								, GTK_FILL , GTK_FILL , 0 , 0);

	fxmyself->impre_label = gtk_label_new(NULL);
	gtk_label_set_justify(GTK_LABEL(fxmyself->impre_label) , GTK_JUSTIFY_LEFT);
	gtk_table_attach(GTK_TABLE(fxmyself->headbox) , fxmyself->impre_label  
								, 1 , 10 , 1 , 2
								, GTK_FILL , GTK_FILL , 0 , 0);

	GtkWidget* halign = gtk_alignment_new( 0 , 0 , 0 , 0);
	gtk_container_add(GTK_CONTAINER(halign) , fxmyself->headbox);

	gtk_box_pack_start(GTK_BOX(vbox) , halign , FALSE , FALSE , 0);

	fxmyself->recv_scroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_box_pack_start(GTK_BOX(vbox) , fxmyself->recv_scroll , TRUE , TRUE , 10);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fxmyself->recv_scroll)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);

	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(fxmyself->recv_scroll)
									  , GTK_SHADOW_ETCHED_IN);
	fxmyself->recv_text = gtk_text_view_new();
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(fxmyself->recv_text) , FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxmyself->recv_text) , GTK_WRAP_CHAR);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(fxmyself->recv_text) , FALSE);
	gtk_container_add(GTK_CONTAINER(fxmyself->recv_scroll) , fxmyself->recv_text);

	fxmyself->recv_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxmyself->recv_text));
	gtk_text_buffer_create_tag(fxmyself->recv_buffer , "blue" , "foreground" , "blue" , NULL);
	gtk_text_buffer_create_tag(fxmyself->recv_buffer , "red" , "foreground" , "red" , NULL);
	gtk_text_buffer_create_tag(fxmyself->recv_buffer , "lm10" , "left_margin" , 10 , NULL);
	gtk_text_buffer_get_end_iter(fxmyself->recv_buffer , &(fxmyself->recv_iter));
	gtk_text_buffer_create_mark(fxmyself->recv_buffer , "scroll" , &(fxmyself->recv_iter) , FALSE);
	
	fxmyself->send_scroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_widget_set_usize(fxmyself->send_scroll , 0 , 100);
	gtk_box_pack_start(GTK_BOX(vbox) , fxmyself->send_scroll , FALSE , FALSE , 10);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fxmyself->send_scroll)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);

	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(fxmyself->send_scroll)
									  , GTK_SHADOW_ETCHED_IN);
	fxmyself->send_text = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxmyself->send_text) , GTK_WRAP_CHAR);
	g_signal_connect(fxmyself->send_text , "key_press_event" , G_CALLBACK(fx_myself_on_enter_pressed) , fxmyself);
	gtk_container_add(GTK_CONTAINER(fxmyself->send_scroll) , fxmyself->send_text);

 	fxmyself->send_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxmyself->send_text));
	gtk_text_buffer_get_iter_at_offset(fxmyself->send_buffer , &(fxmyself->send_iter) , 0);
	
	action_area = gtk_hbox_new(FALSE , 0);
	gtk_box_pack_start(GTK_BOX(vbox) , action_area , FALSE , FALSE , 10);
	close_button = gtk_button_new_with_label(_("Close"));
	gtk_widget_set_usize(close_button , 100 , 30);
	gtk_box_pack_end(GTK_BOX(action_area) , close_button , FALSE , TRUE , 2);
	g_signal_connect(close_button , "clicked" , G_CALLBACK(fx_myself_on_close_clicked) , fxmyself);

	send_button = gtk_button_new_with_label(_("Send"));
	gtk_widget_set_usize(send_button , 100 , 30);
	gtk_box_pack_end(GTK_BOX(action_area) , send_button , FALSE , TRUE , 2);
	g_signal_connect(send_button , "clicked" , G_CALLBACK(fx_myself_on_send_clicked) , fxmyself);

	gtk_window_set_position(GTK_WINDOW(fxmyself->window) , GTK_WIN_POS_CENTER);
	fx_myself_bind(fxmyself);

	GTK_WIDGET_SET_FLAGS(fxmyself->send_text, GTK_CAN_FOCUS);
	gtk_widget_grab_focus(fxmyself->send_text);

	gtk_widget_show_all(fxmyself->window);
}
void fx_myself_on_close_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxMyself* fxmyself = (FxMyself*)data;

	gtk_widget_destroy(fxmyself->window);
}

void fx_myself_on_send_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{

	FxMyself* fxmyself = (FxMyself*)data;
	GtkTextIter begin , end;
	char* text;

	gtk_text_buffer_get_start_iter(fxmyself->send_buffer , &begin);
	gtk_text_buffer_get_end_iter(fxmyself->send_buffer , &end);
	text = gtk_text_buffer_get_text(fxmyself->send_buffer , &begin , &end , TRUE);

	if(*text == '\0' )
		return;
	fetion_conversation_send_sms_to_myself(fxmyself->conv , text);
	fx_myself_add_message(fxmyself , text);
	gtk_text_buffer_delete(fxmyself->send_buffer , &begin , &end);
}
gboolean fx_myself_on_enter_pressed(GtkWidget* widget , GdkEventKey* event , gpointer data)
{
	FxMyself* fxmyself = NULL;
	Config *config = NULL;
	
	if(event->keyval == GDK_Return || event->keyval == GDK_ISO_Enter || event->keyval == GDK_KP_Enter)
	{
		fxmyself = (FxMyself*)data;
		config = fxmyself->fxmain->user->config;
		if(config->sendMode == SEND_MODE_ENTER)
		{
			if(event->state & GDK_CONTROL_MASK || event->state & GDK_SHIFT_MASK){
				return FALSE;
			}else{
 				if (gtk_im_context_filter_keypress (GTK_TEXT_VIEW(fxmyself->send_text)->im_context, event)) {
 					GTK_TEXT_VIEW (fxmyself->send_text)->need_im_reset = TRUE;
 					return TRUE;
 				}
				fx_myself_on_send_clicked(widget, data);
				return TRUE;
			}
		}
		else
		{
			if(event->state & GDK_CONTROL_MASK)	{
				fx_myself_on_send_clicked(widget, data);
				return TRUE;
			}else{
				return FALSE;
			}
		}		
	}
	return FALSE;
}

static gboolean key_press_func(GtkWidget *UNUSED(widget), GdkEventKey *event
		, gpointer data)
{
	FxMyself *fxmyself;
	if(event->keyval == GDK_w){
		if(event->state & GDK_CONTROL_MASK){
			fxmyself = (FxMyself*)data;
			gtk_widget_destroy(fxmyself->window);
			return TRUE;
		}else{
			return FALSE;
		}
	}

	return FALSE;
}
