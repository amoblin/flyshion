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

static GStaticMutex mutex;
static void fx_chat_on_tophone_clicked(GtkWidget* widget , gpointer data);
static void fx_chat_on_close_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_chat_on_send_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_chat_on_history_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static gboolean fx_chat_on_key_pressed(GtkWidget* UNUSED(widget)
		, GdkEventKey* event , gpointer data);
static void fx_chat_on_nudge_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static gboolean fx_chat_on_text_buffer_changed(GtkTextBuffer* buffer , gpointer data);

FxChat* fx_chat_new(FxMain* fxmain , Conversation* conv)
{
	FxChat* fxchat = (FxChat*)malloc(sizeof(FxChat));

	DEBUG_FOOTPRINT();

	memset(fxchat , 0 , sizeof(FxChat));
	fxchat->fxmain = fxmain;
	fxchat->conv = conv;
	fxchat->sendtophone = FALSE;
	return fxchat;
}
void fx_chat_add_message(FxChat* fxchat , const char* msg 
	, const struct tm* datetime , int issendmsg,
	int issysback)
{
	GtkTextChildAnchor *anchor; 
	GtkTextIter         iter;
	GtkWidget          *pb;
	GtkTextBuffer      *buffer;
	FxMain             *fxmain;
	User               *user;
	Contact            *contact;
	gchar              *usid;
	gchar              text[4096];
	gchar              color[10];
	gchar              time[30];
	gchar              path[1024];
	struct tm         *now;

	unsigned int i = 0 , p = 0 , n = 0;

	fxmain = fxchat->fxmain;
	user = fxmain->user;
	contact = fxchat->conv->currentContact;

	if(issendmsg == 1)
		strcpy(color , "blue");
	else
		strcpy(color , "red");

	buffer = gtk_text_view_get_buffer(
			GTK_TEXT_VIEW(fxchat->recv_text));

	/* timeout alert msg */
	if(issysback) {
		strftime(time, sizeof(time), "%H:%M:%S", datetime);
		snprintf(text ,sizeof(text) - 1,
				_("The message \"%s\" sent"
				" at [%s] send failed,please resend it"),
				time , msg);
		fx_chat_add_information(fxchat , text);
		return;
	}

	usid = fetion_sip_get_sid_by_sipuri(contact->sipuri);

	if(issendmsg == 0){
		strftime(time, sizeof(time), "%H:%M:%S", datetime);
		snprintf(text, sizeof(text) - 1,
				_("%s said: (%s):\n"), contact->nickname, time);
	}else{
		now = get_currenttime();
		strftime(time, sizeof(time), "%H:%M:%S", now);
		snprintf(text, sizeof(text) - 1,
				_("%s said: (%s):\n"), user->nickname, time);
		fx_main_add_history(fxmain, user->nickname,	contact->userId,
				msg, issendmsg);
	}
	g_free(usid);

	gtk_text_buffer_get_end_iter(buffer , &iter );
	gtk_text_buffer_insert_with_tags_by_name(buffer
					, &iter , text , -1 , color , NULL);
	gchar *msgE = fx_util_replace_emotion((char*)msg);
	gchar *pos , num[4];

	while(msgE[i] != '\0'){
		if(msgE[i] == '#'){
			pos = msgE + i + 1;
			if(strstr(pos , "#") != NULL){
				n = strlen(pos) - strlen(strstr(pos , "#"));
				if(n == 1 || n == 2 ){
					bzero(num , sizeof(num));
					strncpy(num , pos , n);
					if(atoi(num) > 0 && atoi(num) < 53){
						gtk_text_buffer_insert_with_tags_by_name(buffer
										, &iter, msgE + p , i - p , "lm10" , NULL);
						sprintf(path , SKIN_DIR"face_images/big_%s.gif" , num);
						pb = gtk_image_new_from_file(path);
						gtk_widget_show(pb);
						anchor = gtk_text_buffer_create_child_anchor(buffer , &iter);
						gtk_text_view_add_child_at_anchor(GTK_TEXT_VIEW(fxchat->recv_text)
								, pb , anchor); 
						i += n + 2;
						p = i;
						continue;
					}
				}
			}
		}
		i ++;
	}
	if( p < strlen(msgE)){
		gtk_text_buffer_insert_with_tags_by_name(buffer
						, &iter, msgE + p , strlen(msgE) - p , "lm10" , NULL);
	}
	gtk_text_buffer_insert(buffer , &iter , "\n" , -1);
	gtk_text_iter_set_line_offset (&iter, 0);
	fxchat->mark = gtk_text_buffer_get_mark (buffer, "scroll");
	gtk_text_buffer_move_mark (buffer, fxchat->mark, &iter);
	gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW(fxchat->recv_text), fxchat->mark);
}
void fx_chat_add_information(FxChat* fxchat , const char* msg)
{
	GtkTextIter    iter;
	GtkTextBuffer *buffer;
	
	buffer = gtk_text_view_get_buffer(
			GTK_TEXT_VIEW(fxchat->recv_text));

	gtk_text_buffer_get_end_iter(buffer , &iter );
	gtk_text_buffer_insert(buffer , &iter , "" , -1);
	gtk_text_buffer_insert_with_tags_by_name(buffer,
					&iter, msg , -1 , "grey",
					"lm10" , "small" , NULL);
	gtk_text_buffer_insert(buffer , &iter , "\n" , -1);

	gtk_text_iter_set_line_offset (&iter, 0);
	fxchat->mark = gtk_text_buffer_get_mark (buffer, "scroll");
	gtk_text_buffer_move_mark(buffer, fxchat->mark, &iter);
	gtk_text_view_scroll_mark_onscreen(
			GTK_TEXT_VIEW(fxchat->recv_text), fxchat->mark);
}
gboolean fx_chat_focus_in_func(GtkWidget *UNUSED(widget)
		, GdkEventFocus *UNUSED(event) , gpointer data)  
{ 
	FxChat    *fxchat;
	FxMain    *fxmain;
	FxList    *ctlist;
	Config    *config;
	Message   *message;
	GdkPixbuf *pb;
	gchar     *sipuri;
	gchar     *sid;
	gchar      path[128];

	fxchat = (FxChat*)data;
	fxmain = fxchat->fxmain;
	ctlist = fxmain->mlist;
	config = fxmain->user->config;

	fxchat->hasFocus = CHAT_DIALOG_FOCUSED;

	if(fxchat->unreadMsgCount > 0){
		fxchat->unreadMsgCount = 0;
		fx_chat_update_window(fxchat);
	}

	if(list_empty(ctlist)){
		/* if the message queue is empty,and the status icon is not blinking 
		 * and there`s no unread mesasge for the current chat window,do nothing */
		if(!gtk_status_icon_get_blinking(
				GTK_STATUS_ICON(fxmain->trayIcon))
			 && fxchat->unreadMsgCount == 0)
			return FALSE;

		fx_head_set_state_image(fxmain,
				fxmain->user->state);
		gtk_status_icon_set_blinking(
				GTK_STATUS_ICON(fxmain->trayIcon) , FALSE);
		g_signal_handler_disconnect(fxmain->trayIcon,
				fxmain->iconConnectId);

		fxmain->iconConnectId = g_signal_connect(
							G_OBJECT(fxmain->trayIcon) 
							 , "activate"
							 , GTK_SIGNAL_FUNC(fx_main_tray_activate_func)
							 , fxmain);
	}else{
		message = (Message*)(ctlist->next->data);

		sipuri = message->sipuri;
		sid = fetion_sip_get_sid_by_sipuri(sipuri);
		sprintf(path , "%s/%s.jpg" , config->iconPath , sid);
		g_free(sid);

		pb = gdk_pixbuf_new_from_file(path , NULL);
		if(!pb)
			pb = gdk_pixbuf_new_from_file(SKIN_DIR"online.svg" , NULL);

		gtk_status_icon_set_blinking(
				GTK_STATUS_ICON(fxmain->trayIcon) , TRUE);
		gtk_status_icon_set_from_pixbuf(
				GTK_STATUS_ICON(fxmain->trayIcon) , pb);
		g_signal_handler_disconnect(fxmain->trayIcon,
				fxmain->iconConnectId);

		fxmain->iconConnectId = g_signal_connect(
							G_OBJECT(fxmain->trayIcon) 
							 , "activate"
							 , GTK_SIGNAL_FUNC(fx_main_message_func)
							, fxmain);

		g_object_unref(pb);
	}

	return FALSE;
}
gboolean fx_chat_focus_out_func(GtkWidget *UNUSED(widget)
		, GdkEventFocus *UNUSED(event) , gpointer data)  
{
	FxChat* fxchat = (FxChat*)data;

	fxchat->hasFocus = CHAT_DIALOG_NOT_FOCUSED;
	return FALSE;
}
void fx_chat_bind(FxChat* fxchat)
{
	FxMain    *fxmain;
	Config    *config;
	gchar      text[128];
	GdkPixbuf *pixbuf;

	fxmain = fxchat->fxmain;
 	config = fxmain->user->config;

	sprintf(text , "%s/%s.jpg",
			config->iconPath,
			fxchat->conv->currentContact->sId);

	pixbuf = gdk_pixbuf_new_from_file_at_size(
			text , 30 , 30 , NULL);

	if(pixbuf){
		gtk_image_set_from_pixbuf(
				GTK_IMAGE(fxchat->headimage),
				pixbuf);
		gtk_window_set_icon(
				GTK_WINDOW(fxchat->dialog),
				pixbuf);
		g_object_unref(pixbuf);
	}
}

static void fx_chat_name_box_func(GtkWidget *UNUSED(widget)
		, GdkEventButton *event , gpointer data)
{
	FxChat     *fxchat;
	gchar      *name;
    gchar      *sid;
    gchar       nametext[1024];
	Contact    *contact;
	FxProfile  *fxprofile;

	fxchat = (FxChat*)data;
	contact = fxchat->conv->currentContact;

	name = (contact->localname == NULL ||
		strlen(contact->localname) == 0)
		? contact->nickname : contact->localname;

	sid = fetion_sip_get_sid_by_sipuri(contact->sipuri);

	switch(event->type){
		case GDK_ENTER_NOTIFY :
			snprintf(nametext, 1023,
					"<span underline='low'><b>%s(%s)</b></span>",
					name , sid);
			g_free(sid);
			gtk_label_set_markup(GTK_LABEL(fxchat->name_label) , nametext);
			break;
		case GDK_LEAVE_NOTIFY :
			snprintf(nametext , 1023 , "<b>%s(%s)</b>" , name , sid);
			g_free(sid);
			gtk_label_set_markup(GTK_LABEL(fxchat->name_label) , nametext);
			break;
		case GDK_BUTTON_PRESS :
			fxprofile = fx_profile_new(fxchat->fxmain , contact->userId);
			fx_profile_initialize(fxprofile);
			gtk_dialog_run(GTK_DIALOG(fxprofile->dialog));
			gtk_widget_destroy(fxprofile->dialog);
			free(fxprofile);
			break;
		default:
			break;
	}
}
void fx_chat_on_emotion_clicked(GtkWidget *widget , gpointer data)
{
	FxChat *fxchat = (FxChat*)data;
	FxEmotion *fxemotion = NULL;
	int x , y , ex , ey , root_x , root_y;

	gtk_widget_translate_coordinates(widget , fxchat->dialog , 0 , 0 , &ex , &ey );
	gtk_window_get_position(GTK_WINDOW(fxchat->dialog) , &root_x , &root_y);
	x = root_x + ex + 3;
	y = root_y + ey + 46;

	fxemotion = fx_emotion_new(fxchat);
	fx_emotion_initialize(fxemotion , x , y);
}

static gboolean key_press_func(GtkWidget *UNUSED(widget) , GdkEventKey *event
		, gpointer data)
{
	FxChat *fxchat;

	if(event->keyval == GDK_w){
		fxchat = (FxChat*)data;
		if(event->state & GDK_CONTROL_MASK){
			gtk_widget_destroy(fxchat->dialog);
			return TRUE;
		}else{
			return FALSE;
		}
	}

	return FALSE;
}

void fx_chat_initialize(FxChat* fxchat)
{
	GtkWidget *vbox;
	GtkWidget *halign;
	GtkWidget *halign1;
	GtkWidget *tophone_icon;
	GtkWidget *hbox;
	GtkWidget *lvbox;
	GtkWidget *rvbox;
	GtkWidget *history_icon;
	GtkWidget *nouge_icon;
	GtkWidget *label;
	GtkWidget *action_area;
	GtkWidget *send_button;
	GtkWidget *close_button;
	GtkWidget *frame;
	GtkWidget *img;
	GdkPixbuf *pb;
	gchar      nametext[512];
	gchar      portraitPath[512];
	gchar     *sid;
	gchar     *name;
	Contact   *contact;
	
	contact = fxchat->conv->currentContact;
	FxMain    *fxmain = fxchat->fxmain;
	User      *user = fxmain->user;
	Config    *config = user->config;

	fxchat->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_modal(GTK_WINDOW(fxchat->dialog) , FALSE);
	gtk_window_set_default_size(GTK_WINDOW(fxchat->dialog) , 600 , 430);
	gtk_widget_set_size_request(fxchat->dialog , 550 , 0);
	gtk_container_set_border_width(GTK_CONTAINER(fxchat->dialog) , 10);
	fx_chat_update_window(fxchat);

	g_signal_connect(fxchat->dialog , "destroy" , G_CALLBACK(fx_chat_destroy) , fxchat);

	vbox = gtk_vbox_new(FALSE , 2);
	hbox = gtk_hbox_new(FALSE , 6);
	gtk_container_add(GTK_CONTAINER(fxchat->dialog) , vbox);
	action_area = gtk_hbox_new(FALSE , 0);

	fxchat->headbox = gtk_table_new(2 , 10 , FALSE );

	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.svg" , 40 , 40 , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxchat->dialog), pb);
	fxchat->headimage = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);

	gtk_table_attach(GTK_TABLE(fxchat->headbox) , fxchat->headimage
								, 0 , 1 , 0 , 2
								, GTK_FILL , GTK_FILL , 3 , 0);

	fxchat->name_label = gtk_label_new(NULL);
	sid = fetion_sip_get_sid_by_sipuri(contact->sipuri);
	name = (contact->localname == NULL || strlen(contact->localname) == 0) ?
				contact->nickname : contact->localname;
	sprintf(nametext , "<b>%s(%s)</b>" , name , sid);
	free(sid);
	gtk_label_set_markup(GTK_LABEL(fxchat->name_label) , nametext);
	gtk_label_set_justify(GTK_LABEL(fxchat->name_label) , GTK_JUSTIFY_LEFT);

	fxchat->name_box = gtk_event_box_new();

	g_signal_connect(fxchat->name_box , "enter-notify-event"
			, GTK_SIGNAL_FUNC(fx_chat_name_box_func) , fxchat);
	g_signal_connect(fxchat->name_box , "leave-notify-event"
			, GTK_SIGNAL_FUNC(fx_chat_name_box_func) , fxchat);
	g_signal_connect(fxchat->name_box , "button-press-event"
			, GTK_SIGNAL_FUNC(fx_chat_name_box_func) , fxchat);

	gtk_container_add(GTK_CONTAINER(fxchat->name_box) , fxchat->name_label);

	gtk_table_attach(GTK_TABLE(fxchat->headbox) , fxchat->name_box
								, 1 , 2 , 0 , 1 
								, GTK_FILL , GTK_FILL , 0 , 0);

	snprintf(nametext , 510 , "%s" , contact->impression);
	escape_impression(nametext);
	fxchat->impre_label = gtk_label_new(nametext);
	gtk_label_set_justify(GTK_LABEL(fxchat->impre_label) , GTK_JUSTIFY_LEFT);
	gtk_table_attach(GTK_TABLE(fxchat->headbox) , fxchat->impre_label  
								, 1 , 10 , 1 , 2
								, GTK_FILL , GTK_FILL , 0 , 0);

	halign = gtk_alignment_new( 0 , 0 , 0 , 0);
	gtk_container_add(GTK_CONTAINER(halign) , fxchat->headbox);

	gtk_box_pack_start(GTK_BOX(vbox) , halign , FALSE , FALSE , 5);
	gtk_box_pack_start(GTK_BOX(vbox) , hbox , TRUE , TRUE , 0);
	lvbox = gtk_vbox_new(FALSE , 0);
	rvbox = gtk_vbox_new(FALSE , 0);
	gtk_box_pack_start(GTK_BOX(hbox) , lvbox , TRUE , TRUE , 0);
	gtk_box_pack_start(GTK_BOX(hbox) , rvbox , FALSE , FALSE , 0);
	fxchat->recv_scroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_box_pack_start(GTK_BOX(lvbox) , fxchat->recv_scroll , TRUE , TRUE , 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fxchat->recv_scroll)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(fxchat->recv_scroll)
									  , GTK_SHADOW_ETCHED_IN);
	fxchat->recv_text = gtk_text_view_new();
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(fxchat->recv_text) , FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxchat->recv_text) , GTK_WRAP_CHAR);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(fxchat->recv_text) , FALSE);
	gtk_container_add(GTK_CONTAINER(fxchat->recv_scroll) , fxchat->recv_text);

	fxchat->recv_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxchat->recv_text));
	gtk_text_buffer_create_tag(fxchat->recv_buffer , "blue" , "foreground" , "#639900" , NULL);
	gtk_text_buffer_create_tag(fxchat->recv_buffer , "grey" , "foreground" , "#808080" , NULL);
	gtk_text_buffer_create_tag(fxchat->recv_buffer , "green" , "foreground" , "green" , NULL);
	gtk_text_buffer_create_tag(fxchat->recv_buffer , "red" , "foreground" , "#0088bf" , NULL);
	gtk_text_buffer_create_tag(fxchat->recv_buffer , "lm10" , "left_margin" , 10 , NULL);
	gtk_text_buffer_create_tag(fxchat->recv_buffer , "small" , "left_margin" , 5 , NULL);
	gtk_text_buffer_get_end_iter(fxchat->recv_buffer , &(fxchat->recv_iter));
	gtk_text_buffer_create_mark(fxchat->recv_buffer , "scroll" , &(fxchat->recv_iter) , FALSE);
	
	/*toolbar begin*/
	fxchat->toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(fxchat->toolbar) , GTK_TOOLBAR_ICONS);
	gtk_box_pack_start(GTK_BOX(lvbox) , fxchat->toolbar , FALSE , FALSE , 0);

	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"emotion.png" , 16 , 16 , NULL);
	nouge_icon = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	fxchat->nouge = gtk_toolbar_append_item(GTK_TOOLBAR(fxchat->toolbar)
					    , _("Emotion") , "" , NULL , nouge_icon
					    , G_CALLBACK(fx_chat_on_emotion_clicked)
					   , fxchat );									   
	gtk_toolbar_append_space(GTK_TOOLBAR(fxchat->toolbar));
	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"phone.png" , 16 , 16 , NULL);
	tophone_icon = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	fxchat->tophone = gtk_toolbar_append_element(GTK_TOOLBAR(fxchat->toolbar)
					 	, GTK_TOOLBAR_CHILD_TOGGLEBUTTON , NULL
						, _("Contact's cell phone")
					       	, _("Mesage will be send to Contact's cell phone in long SMS format") 
						, NULL , tophone_icon
					     , G_CALLBACK(fx_chat_on_tophone_clicked)
						   , fxchat );
	gtk_toolbar_append_space(GTK_TOOLBAR(fxchat->toolbar));
	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"history.png" , 16 , 16 , NULL);
	history_icon = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	fxchat->historybutton = gtk_toolbar_append_item(GTK_TOOLBAR(fxchat->toolbar)
					 	, _("Chat logs") , _("View chat logs") , NULL , history_icon
					  	, G_CALLBACK(fx_chat_on_history_clicked)
						,fxchat );
	gtk_toolbar_append_space(GTK_TOOLBAR(fxchat->toolbar));
	nouge_icon = gtk_image_new_from_file(SKIN_DIR"nudge.png");
	fxchat->nouge = gtk_toolbar_append_item(GTK_TOOLBAR(fxchat->toolbar)
					 	  , _("Screen jitter") , _("Send a screen jitter") , NULL , nouge_icon
					  	  , G_CALLBACK(fx_chat_on_nudge_clicked)
											   , fxchat );									   
	gtk_toolbar_append_space(GTK_TOOLBAR(fxchat->toolbar));

	label = gtk_label_new(_("total 180 character left"));
	fxchat->countLabel = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(fxchat->countLabel) , _("[<span color='#0099ff'>180</span>] characters"));
	gtk_container_add(GTK_CONTAINER(fxchat->toolbar) , label);
	gtk_container_add(GTK_CONTAINER(fxchat->toolbar) , fxchat->countLabel);
	/*toolbar end*/
	fxchat->send_scroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_box_pack_start(GTK_BOX(lvbox) , fxchat->send_scroll , FALSE , FALSE , 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fxchat->send_scroll)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(fxchat->send_scroll)
									  , GTK_SHADOW_ETCHED_IN);
	fxchat->send_text = gtk_text_view_new();
	gtk_widget_set_usize(fxchat->send_text , 0 , 100);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxchat->send_text) , GTK_WRAP_WORD_CHAR);
	g_signal_connect(fxchat->send_text , "key_press_event" , G_CALLBACK(fx_chat_on_key_pressed) , fxchat);
	gtk_container_add(GTK_CONTAINER(fxchat->send_scroll) , fxchat->send_text);

 	fxchat->send_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxchat->send_text));
	gtk_text_buffer_get_iter_at_offset(fxchat->send_buffer , &(fxchat->send_iter) , 0);

	halign1 = gtk_alignment_new( 1 , 0 , 0 , 0);
	gtk_container_add(GTK_CONTAINER(halign1) , action_area);
	gtk_box_pack_start(GTK_BOX(vbox) , halign1 , FALSE , FALSE , 0);
	
	close_button = gtk_button_new_with_label(_("Close"));
	gtk_widget_set_usize(close_button , 100 , 30);
	gtk_box_pack_start(GTK_BOX(action_area) , close_button , FALSE , TRUE , 2);
	g_signal_connect(close_button , "clicked" , G_CALLBACK(fx_chat_on_close_clicked) , fxchat);

	send_button = gtk_button_new_with_label(_("Send"));
	gtk_widget_set_usize(send_button , 100 , 30);
	gtk_box_pack_start(GTK_BOX(action_area) , send_button , FALSE , TRUE , 2);
	g_signal_connect(send_button , "clicked" , G_CALLBACK(fx_chat_on_send_clicked) , fxchat);

	gtk_window_set_position(GTK_WINDOW(fxchat->dialog) , GTK_WIN_POS_CENTER);
	//gtk_window_set_opacity(GTK_WINDOW(fxchat->dialog) , 0.9);

	fx_chat_bind(fxchat);

	g_signal_connect(fxchat->dialog , "focus-in-event"
			, GTK_SIGNAL_FUNC(fx_chat_focus_in_func) , fxchat );

	g_signal_connect(fxchat->dialog , "focus-out-event"
			, GTK_SIGNAL_FUNC(fx_chat_focus_out_func) , fxchat );

	g_signal_connect(fxchat->send_buffer , "changed"
			, G_CALLBACK(fx_chat_on_text_buffer_changed) , fxchat);

	g_signal_connect(fxchat->dialog , "key-press-event" , G_CALLBACK(key_press_func) , fxchat);
	/*right box */

	frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame) , GTK_SHADOW_ETCHED_IN);
	gtk_widget_set_usize(frame , 160 , 160);

	sprintf(portraitPath , "%s/%s.jpg" , config->iconPath , contact->sId);
	pb = gdk_pixbuf_new_from_file_at_size(portraitPath , 140 , 140 , NULL);
	if(!pb)
		pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.svg" , 140 , 140 , NULL);
	
	img = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_container_add(GTK_CONTAINER(frame) , img);
	gtk_box_pack_start(GTK_BOX(rvbox) , frame , FALSE , FALSE , 0);

	GtkWidget *spliter = gtk_label_new(NULL);
	gtk_box_pack_start(GTK_BOX(rvbox) , spliter , TRUE , TRUE , 0);
	
	frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame) , GTK_SHADOW_ETCHED_IN);
	gtk_widget_set_usize(frame , 160 , 160);
	bzero(portraitPath , sizeof(portraitPath));
	sprintf(portraitPath , "%s/%s.jpg" , config->iconPath , user->sId);
	pb = gdk_pixbuf_new_from_file_at_size(portraitPath , 140 , 140 , NULL);
	if(!pb){
		pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.svg" , 140 , 140 , NULL);
	}
	img = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_container_add(GTK_CONTAINER(frame) , img);
	gtk_box_pack_start(GTK_BOX(rvbox) , frame , FALSE , FALSE , 0);

	gtk_widget_show_all (vbox);
	gtk_widget_show(fxchat->dialog);
}

void fx_chat_free(FxChat* fxchat)
{
	if(fxchat)
		free(fxchat->conv);

	free(fxchat);
}

void fx_chat_destroy(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxChat* fxchat = (FxChat*)data;

	fx_list_remove_chat_by_sipuri(fxchat->fxmain->clist
				, fxchat->conv->currentContact->sipuri);
	fx_chat_free(fxchat);
}

void* fx_chat_send_message_thread(void *data)
{
	FxChat       *fxchat;
	Conversation *conv;
	GtkTextIter   begin;
	GtkTextIter   end;
	gchar         *text = NULL;
	struct tm    *now = NULL;
	ThreadArgs   *args = NULL;
	FxList       *fxlist = NULL;

	fxchat = (FxChat*)data;
	conv = fxchat->conv;

	gtk_text_buffer_get_start_iter(fxchat->send_buffer , &begin);
	gtk_text_buffer_get_end_iter(fxchat->send_buffer , &end);
	text = gtk_text_buffer_get_text(fxchat->send_buffer , &begin , &end , TRUE);

	if(strlen(text) == 0){
		fx_chat_add_information(fxchat,
				_("Empty messages are not allowed."));
		return NULL;
	}

	now = get_currenttime();

	gdk_threads_enter();
	fx_chat_add_message(fxchat , text , now , 1 , 0);
	gtk_text_buffer_delete(fxchat->send_buffer , &begin , &end);
	gdk_threads_leave();

	if(fetion_conversation_invite_friend(conv) > 0){

		fx_conn_append(conv->currentSip->tcp);

		args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
		args->fxmain = fxchat->fxmain;
		args->sip = conv->currentSip;

		fxlist = fx_list_new(conv->currentSip);
		fx_list_append(fxchat->fxmain->slist , fxlist);

		g_thread_create(fx_main_listen_thread_func , args , FALSE , NULL);

		fetion_conversation_send_sms(conv , text);
	}

	return NULL;
}

void fx_chat_send_message(FxChat* fxchat)
{
	Conversation    *conv;
	Contact         *contact;
	User            *user;
	GtkTextIter      begin;
	GtkTextIter      end;
	gchar           *text;
	FxCode          *fxcode;
	gchar            reason[512];
	gchar            tips[512];
	const gchar     *code = NULL;
	gint             ret;
	gint             daycount;
	gint             monthcount;


	if(!fx_conn_check_action(fxchat->fxmain))
		return;

	conv = fxchat->conv;
	contact = conv->currentContact;
	user = fxchat->fxmain->user;

	gtk_text_buffer_get_start_iter(fxchat->send_buffer , &begin);
	gtk_text_buffer_get_end_iter(fxchat->send_buffer , &end);

	if(!contact)
		return;

	text = gtk_text_buffer_get_text(fxchat->send_buffer,
			&begin, &end, TRUE);

	if(strlen(text) == 0)
		return;


	/* check whether 'send to phone' button is clicked ,
	 * if true , just send to phone and return	 */
	if(fxchat->sendtophone)	{
		/* show message sent,and truncate the send text area */
		text = gtk_text_buffer_get_text(fxchat->send_buffer,
				&begin, &end, TRUE);

		if(strlen(text) == 0){
			fx_chat_add_information(fxchat,
					_("Empty messages are not allowed."));
			return;
		}

		if(user->boundToMobile == BOUND_MOBILE_DISABLE){
			user->verification = fetion_verification_new();
send:
			generate_pic_code(user);
			if(user->smsDayLimit == user->smsDayCount){
				fx_chat_add_information(fxchat,
						_("Sorry, you have reached"
						" the quota of free SMS today, SMS"
						" messages cannot be sent any more today."));
				return;
			}
			sprintf(reason , _("You have %d free SMS can send "
					" (include this one). Free SMS: %d per month")
					, user->smsDayLimit - user->smsMonthCount
					, user->smsMonthLimit );
			sprintf(tips,
				_("tips:if you wnat to send more"
				" free sms,please bound your mobileno\n"
				"at once ,no authentication code needed any more."));

			fxcode = fx_code_new(fxchat->fxmain , reason
					, tips , CODE_NOT_ERROR);
			fx_code_initialize(fxcode);
			ret = gtk_dialog_run(GTK_DIALOG(fxcode->dialog));

			if(ret == GTK_RESPONSE_OK){
				code = gtk_entry_get_text(GTK_ENTRY(fxcode->codeentry));
				user->verification->code = (char*)malloc(strlen(code) + 1);
				bzero(user->verification->code , strlen(code) + 1);
				strcpy(user->verification->code , code);
				gtk_widget_destroy(fxcode->dialog);
			}else{
				gtk_widget_destroy(fxcode->dialog);
				return;
			}

			if(fetion_conversation_send_sms_to_phone_with_reply(conv
						, text , &daycount , &monthcount) == -1){
				goto send;
			}else{
				fetion_verification_free(user->verification);
				user->verification = NULL;
			}

		}else{
			fetion_conversation_send_sms_to_phone_with_reply(conv
					, text , &daycount , &monthcount);
		}

		fx_chat_add_message(fxchat , text , NULL , 1 , 0);

		sprintf(tips , _("Message sent successfully."
					" You have sent %d SMS, and %d left.")
				, daycount , user->smsDayLimit - daycount);
		fx_chat_add_information(fxchat , tips);

		gtk_text_buffer_delete(fxchat->send_buffer , &begin , &end);
		return;
	}
	/* if user`s online and has not started a thread for chating ,
	 * just created a thread for listening ,and send message
	 * throuch the chat channel	 */
	if(contact->state > 0 && conv->currentSip == NULL){
		conv->currentSip = fx_list_find_sip_by_sipuri(
				fxchat->fxmain->slist , contact->sipuri);

		if(!conv->currentSip){
			debug_info("CREATE A NEW MESSAGE THREAD");
			g_thread_create(fx_chat_send_message_thread, fxchat, FALSE, NULL);
			return;
		}

		debug_info("MESSAGE THREAD ALREADY EXIST");
	}

	/* show message sent,and truncate the send text area */

	fx_chat_add_message(fxchat , text , NULL , 1 , 0);

	gtk_text_buffer_delete(fxchat->send_buffer , &begin , &end);

	fetion_conversation_send_sms(conv , text);
} 

void* fx_chat_send_nudge_thread(void* data)
{
	FxChat       *fxchat;
	Conversation *conv;
	ThreadArgs   *args = NULL;
	FxList       *fxlist = NULL;

	fxchat = (FxChat*)data;
	conv = fxchat->conv;

	g_static_mutex_lock(&mutex);
	if(fetion_conversation_invite_friend(conv) > 0){

		fx_conn_append(conv->currentSip->tcp);

		args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
		args->fxmain = fxchat->fxmain;
		args->sip = conv->currentSip;

		fxlist = fx_list_new(conv->currentSip);
		fx_list_append(fxchat->fxmain->slist , fxlist);

		gdk_threads_enter();

		g_thread_create(fx_main_listen_thread_func , args , FALSE , NULL);
		fetion_conversation_send_nudge(conv);

		gdk_threads_leave();
	}
	g_static_mutex_unlock(&mutex);
	return NULL;
}
static void fx_chat_on_nudge_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxChat       *fxchat;
	Conversation *conv;
	Contact      *contact;

	fxchat = (FxChat*)data;
	conv = fxchat->conv;
	contact = conv->currentContact;

	if(!fx_conn_check_action(fxchat->fxmain))
		return;

	if(contact->state <= 0){
		fx_chat_add_information(fxchat,
			_("Contact is not online. Cannot send screen jitter"));
		return;
	}

	if(contact->state > 0 && conv->currentSip == NULL){

		conv->currentSip = fx_list_find_sip_by_sipuri(
				fxchat->fxmain->slist , contact->sipuri);

		if(!conv->currentSip){
			debug_info("CREATE A NEW MESSAGE THREAD");
			g_thread_create(fx_chat_send_nudge_thread , fxchat , FALSE , NULL);
			fx_chat_nudge(fxchat);
			return;
		}
		debug_info("MESSAGE THREAD ALREADY EXIST");
	}
	fetion_conversation_send_nudge(conv);
	fx_chat_nudge(fxchat);
	
}
void fx_chat_nudge(FxChat* fxchat)
{
	int x , y , i;
	
	gtk_window_get_position(GTK_WINDOW(fxchat->dialog) , &x , &y);

	for(i = 0 ; i < 4 ; i++){
		g_usleep(70000);
		gtk_window_move(GTK_WINDOW(fxchat->dialog) , x + 7 , y + 7);
		update();
		g_usleep(70000);
		gtk_window_move(GTK_WINDOW(fxchat->dialog) , x - 7 , y - 7);
		update();
	}
}

void fx_chat_nudge_in_thread(FxChat* fxchat)
{
	int x , y , i;
	
	gdk_threads_enter();
	gtk_window_get_position(GTK_WINDOW(fxchat->dialog) , &x , &y);
	gdk_threads_leave();

	for(i = 0 ; i < 4 ; i++){
		g_usleep(100000);
		gdk_threads_enter();
		gtk_window_move(GTK_WINDOW(fxchat->dialog) , x + 7 , y + 7);
		update();
		gdk_threads_leave();
		g_usleep(100000);
		gdk_threads_enter();
		gtk_window_move(GTK_WINDOW(fxchat->dialog) , x - 7 , y - 7);
		update();
		gdk_threads_leave();
	}
}

static void fx_chat_on_tophone_clicked(GtkWidget* widget , gpointer data)
{
	GtkToggleButton *btn;
	FxChat          *fxchat;
	User            *user;
	gchar            text[1024];

	btn = GTK_TOGGLE_BUTTON(widget);
 	fxchat = (FxChat*)data;
	user = fxchat->fxmain->user;

	if(gtk_toggle_button_get_active(btn)){

		fxchat->sendtophone = TRUE;

		if(user->boundToMobile == BOUND_MOBILE_DISABLE){
			bzero(text , sizeof(text));
			if(user->smsDayLimit == user->smsDayCount 
			|| user->smsMonthLimit == user->smsMonthCount){
				strcpy(text , _("Run out of your quota,"
							" you can still send IM mesages."
							" If want to send more free SMS,"
							" bind you cell phone number."));
				gtk_widget_set_sensitive(fxchat->send_text , FALSE);
			}else{
				sprintf(text , _("Mesage will be sent to "
							"contact's cell phone. You have"
							" %d free SMS left. If want to send"
							" more free SMS, bind your cell phone,please.") 
						, user->smsDayLimit - user->smsDayCount);
			}
			fx_chat_add_information(fxchat , text);
		}else{
			sprintf(text , _("Mesage will be sent to you phone as long SMS."
						" You have sent %d, and %d left.")
					, user->smsDayCount , user->smsDayLimit - user->smsDayCount);
			fx_chat_add_information(fxchat , text);
		}
	}else{
		fxchat->sendtophone = FALSE;
		gtk_widget_set_sensitive(fxchat->send_text , TRUE);
		fx_chat_add_information(fxchat , _("Message will be sent to contact's fetion"));
	}
}
static void fx_chat_on_close_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxChat* fxchat = (FxChat*)data;

	gtk_widget_destroy(fxchat->dialog);
}

static void fx_chat_on_send_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	fx_chat_send_message((FxChat*)data);
}
static void fx_chat_on_history_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxChat       *fxchat;
	Conversation *conv;
	Contact      *contact;
	FxHistory    *fxhistory;
 	
	fxchat = (FxChat*)data;
 	conv = fxchat->conv;
 	contact = conv->currentContact;

	fxhistory = fx_history_new(fxchat->fxmain,
			contact->userId , contact->nickname);
	fx_history_initialize(fxhistory);
	gtk_dialog_run(GTK_DIALOG(fxhistory->dialog));
	gtk_widget_destroy(fxhistory->dialog);
	free(fxhistory);
}

static gboolean fx_chat_on_key_pressed(GtkWidget* UNUSED(widget)
		, GdkEventKey* event , gpointer data)
{
	FxChat *fxchat = NULL;
	Config *config = NULL;

	DEBUG_FOOTPRINT();

	if(event->keyval == GDK_Return || event->keyval == GDK_ISO_Enter || event->keyval == GDK_KP_Enter){
		fxchat = (FxChat*)data;
		config = fxchat->fxmain->user->config;
		if(config->sendMode == SEND_MODE_ENTER){
			if(event->state & GDK_CONTROL_MASK || event->state & GDK_SHIFT_MASK){
				return FALSE;
			}else{
 				if (gtk_im_context_filter_keypress (GTK_TEXT_VIEW(fxchat->send_text)->im_context, event)) {
					GTK_TEXT_VIEW (fxchat->send_text)->need_im_reset = TRUE;
 					return TRUE;
 				}
				fx_chat_send_message(fxchat);
				return TRUE;
			}
		}else{
			if(event->state & GDK_CONTROL_MASK)	{
				fx_chat_send_message(fxchat);
				return TRUE;
			}else{
				return FALSE;
			}
		}
	}
	return FALSE;
}

static gboolean fx_chat_on_text_buffer_changed(GtkTextBuffer* buffer , gpointer data)
{
	FxChat* fxchat = (FxChat*)data;
	GtkTextIter  startIter;
	GtkTextIter  endIter;
	gchar        text[48];
	const gchar *res = NULL;
	gint         count;  
	
	count = gtk_text_buffer_get_char_count(buffer);

	if(count <= 180){
		sprintf(text , _("[<span color='#0099ff'>%d</span>] character") , 180 - count);
		gtk_label_set_markup(GTK_LABEL(fxchat->countLabel) , text);
	}else{
		gtk_text_buffer_get_start_iter(buffer , &startIter);
		gtk_text_buffer_get_iter_at_offset(buffer , &endIter , 180);
		res = gtk_text_buffer_get_text(buffer , &startIter , &endIter , 180);
		gtk_text_buffer_set_text(buffer , res , strlen(res));
	}
	return FALSE;
}

void fx_chat_update_window(FxChat *fxchat)
{
	Contact *cont;
	char    markup[1024];
	char    *name;

	cont = fxchat->conv->currentContact;
	name = (cont->localname == NULL || strlen(cont->localname) == 0) ?
				cont->nickname : cont->localname;

	if(fxchat->unreadMsgCount > 0)
		sprintf(markup, _("(%d)unread [%s]"),
				fxchat->unreadMsgCount, name);
	else
		snprintf(markup , sizeof(markup) - 1,
				_("Chatting with [%s]"), name);
	

	gtk_window_set_title(GTK_WINDOW(fxchat->dialog), markup);
}
