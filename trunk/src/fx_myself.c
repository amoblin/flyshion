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

FxMyself* fx_myself_new(FxMain* fxmain)
{
	FxMyself* fxmyself = (FxMyself*)malloc(sizeof(FxMyself));

	DEBUG_FOOTPRINT();

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

	DEBUG_FOOTPRINT();

	bzero(name , sizeof(name));
	sprintf(name , "%s/%s.jpg" , config->iconPath ,
			user->sId );
	pb = gdk_pixbuf_new_from_file_at_size(name , 40 , 40 ,
				NULL); 
	if(pb != NULL) {
		gtk_image_set_from_pixbuf(GTK_IMAGE(fxmyself->headimage) , pb);
		gtk_window_set_icon(GTK_WINDOW(fxmyself->dialog) , pb);
	}
	gtk_window_set_title(GTK_WINDOW(fxmyself->dialog) ,
			_("Sending sms to myself"));

	bzero(name , sizeof(name)); sprintf(name , "%s(%s)" , user->nickname ,
			user->sId);
	gtk_label_set_markup(GTK_LABEL(fxmyself->name_label) ,
				name);

	gtk_label_set_text(GTK_LABEL(fxmyself->impre_label) , user->impression);
}

void fx_myself_add_message(FxMyself* fxmyself , const char* message)
{
	GtkTextIter iter;

	User* user = fxmyself->fxmain->user;
	char text[200] = { 0 };
	char time[30] = { 0 };
	char color[] = "blue";

	struct tm* sendtime = get_currenttime();
	GtkTextBuffer* buffer =	gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxmyself->recv_text));

	DEBUG_FOOTPRINT();

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
	GtkWidget* action_area;

	DEBUG_FOOTPRINT();

	fxmyself->dialog = gtk_dialog_new(); 
	vbox = GTK_DIALOG(fxmyself->dialog)->vbox;
	action_area = GTK_DIALOG(fxmyself->dialog)->action_area;
	gtk_window_set_modal(GTK_WINDOW(fxmyself->dialog) , FALSE);
	gtk_widget_set_usize(fxmyself->dialog , 550 , 490);
	gtk_dialog_set_has_separator(GTK_DIALOG(fxmyself->dialog) , FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(fxmyself->dialog) , 10);

	fxmyself->headbox = gtk_table_new(2 , 10 , FALSE );

	fxmyself->headpix = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.png" ,
			40 , 40 , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxmyself->dialog) , fxmyself->headpix);
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

	gtk_box_pack_start(GTK_BOX(vbox) , halign , FALSE , TRUE , 0);

	fxmyself->recv_scroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_box_pack_start(GTK_BOX(vbox) , fxmyself->recv_scroll , TRUE , TRUE , 10);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fxmyself->recv_scroll)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);

	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(fxmyself->recv_scroll)
									  , GTK_SHADOW_ETCHED_IN);
	fxmyself->recv_text = gtk_text_view_new();
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
	gtk_box_pack_start(GTK_BOX(vbox) , fxmyself->send_scroll , TRUE , TRUE , 10);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fxmyself->send_scroll)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);

	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(fxmyself->send_scroll)
									  , GTK_SHADOW_ETCHED_IN);
	fxmyself->send_text = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxmyself->recv_text) , GTK_WRAP_CHAR);
	g_signal_connect(fxmyself->send_text , "key_press_event" , G_CALLBACK(fx_myself_on_enter_pressed) , fxmyself);
	gtk_container_add(GTK_CONTAINER(fxmyself->send_scroll) , fxmyself->send_text);

 	fxmyself->send_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxmyself->send_text));
	gtk_text_buffer_get_iter_at_offset(fxmyself->send_buffer , &(fxmyself->send_iter) , 0);

	close_button = gtk_button_new_with_label(_("Close"));
	gtk_widget_set_usize(close_button , 100 , 30);
	gtk_box_pack_start(GTK_BOX(action_area) , close_button , FALSE , TRUE , 2);
	g_signal_connect(close_button , "clicked" , G_CALLBACK(fx_myself_on_close_clicked) , fxmyself);

	send_button = gtk_button_new_with_label(_("Send"));
	gtk_widget_set_usize(send_button , 100 , 30);
	gtk_box_pack_start(GTK_BOX(action_area) , send_button , FALSE , TRUE , 2);
	g_signal_connect(send_button , "clicked" , G_CALLBACK(fx_myself_on_send_clicked) , fxmyself);

	gtk_window_set_position(GTK_WINDOW(fxmyself->dialog) , GTK_WIN_POS_CENTER);
	gtk_window_set_opacity(GTK_WINDOW(fxmyself->dialog) , 0.9);
	fx_myself_bind(fxmyself);
	gtk_widget_show_all(fxmyself->dialog);
	gtk_widget_hide(fxmyself->dialog);
}
void fx_myself_on_close_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxMyself* fxmyself = (FxMyself*)data;

	DEBUG_FOOTPRINT();

	gtk_dialog_response(GTK_DIALOG(fxmyself->dialog) , GTK_RESPONSE_OK);
}

void fx_myself_on_send_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{

	FxMyself* fxmyself = (FxMyself*)data;
	GtkTextIter begin , end;
	char* text;

	DEBUG_FOOTPRINT();

	gtk_text_buffer_get_start_iter(fxmyself->send_buffer , &begin);
	gtk_text_buffer_get_end_iter(fxmyself->send_buffer , &end);
	text = gtk_text_buffer_get_text(fxmyself->send_buffer , &begin , &end , TRUE);

	if(strlen(text) == 0 )
		return;
	fetion_conversation_send_sms_to_myself(fxmyself->conv , text);
	fx_myself_add_message(fxmyself , text);
	gtk_text_buffer_delete(fxmyself->send_buffer , &begin , &end);
}
gboolean fx_myself_on_enter_pressed(GtkWidget* widget , GdkEventKey* event , gpointer data)
{
	FxMyself* fxmyself = NULL;
	Config *config = NULL;
	
	DEBUG_FOOTPRINT();

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
