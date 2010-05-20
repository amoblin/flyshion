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

FxChat* fx_chat_new(FxMain* fxmain , Conversation* conv)
{
	FxChat* fxchat = (FxChat*)malloc(sizeof(FxChat));

	DEBUG_FOOTPRINT();

	memset(fxchat , 0 , sizeof(FxChat));
	fxchat->fxmain = fxmain;
	fxchat->conv = conv;
	fxchat->fhistory = fetion_history_new(fxmain->user);
	fxchat->sendtophone = FALSE;
	return fxchat;
}
void fx_chat_add_message(FxChat* fxchat , const char* msg , const struct tm* datetime , const int issendmsg)
{
	GtkTextIter iter;
	GtkTextBuffer* buffer;
	FxMain* fxmain = fxchat->fxmain;
	User* user = fxmain->user;
	History* history;
	Contact* contact = fxchat->conv->currentContact;
	char* usid;
	char text[200] = { 0 };
	char color[10] = { 0 };
	char time[30] = { 0 };

	int i = 0 , p = 0 , n = 0;
	GtkWidget *pb = NULL;
	GtkTextChildAnchor *anchor; 
	char path[1024];

	DEBUG_FOOTPRINT();

	if(issendmsg == 1)
		strcpy(color , "blue");
	else
		strcpy(color , "red");

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxchat->recv_text));
	strftime(time , sizeof(time) , "%H:%M:%S" , datetime);

	usid = fetion_sip_get_sid_by_sipuri(contact->sipuri);
	if(issendmsg == 0){
		sprintf(text , "%s(%s) %s\n" , contact->nickname , usid , time);
		history = fetion_history_message_new(contact->nickname , contact->userId , *datetime , msg , issendmsg);
	}else{
		sprintf(text , "%s(%s) %s\n" , user->nickname , user->sId , time);
		history = fetion_history_message_new(user->nickname , contact->userId , *datetime , msg , issendmsg);
	}
	fetion_history_add(fxchat->fhistory , history);
	fetion_history_message_free(history);
	gtk_text_buffer_get_end_iter(buffer , &iter );
	gtk_text_buffer_insert_with_tags_by_name(buffer
					, &iter , text , -1 , color , NULL);
	char *msgE = fx_util_replace_emotion((char*)msg);
	char *pos , num[4];
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
						bzero(path , sizeof(path));
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
	GtkTextIter iter;

	GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxchat->recv_text));

	DEBUG_FOOTPRINT();

	gtk_text_buffer_get_end_iter(buffer , &iter );

	gtk_text_buffer_insert(buffer , &iter , "" , -1);

	gtk_text_buffer_insert_with_tags_by_name(buffer
					, &iter, msg , -1 , "grey" , "lm10" , "small" , NULL);
	
	gtk_text_buffer_insert(buffer , &iter , "\n" , -1);

	gtk_text_iter_set_line_offset (&iter, 0);
	
	fxchat->mark = gtk_text_buffer_get_mark (buffer, "scroll");

	gtk_text_buffer_move_mark (buffer, fxchat->mark, &iter);

	gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW(fxchat->recv_text), fxchat->mark);
}
gboolean fx_chat_focus_in_func(GtkWidget *widget , GdkEventFocus *event , gpointer data)  
{ 
	FxChat* fxchat = (FxChat*)data;
	FxMain* fxmain = fxchat->fxmain;
	FxList* ctlist = fxmain->mlist;
	Config* config = fxmain->user->config;
	Message* message = NULL;
	GdkPixbuf* pb = NULL;
	char path[128];
	char* sipuri = NULL;
	char* sid = NULL;

	DEBUG_FOOTPRINT();

	if(ctlist == NULL)
	{
		fx_head_set_state_image(fxmain , fxmain->user->state);
		gtk_status_icon_set_blinking(GTK_STATUS_ICON(fxmain->trayIcon) , FALSE);
		g_signal_handler_disconnect(fxmain->trayIcon , fxmain->iconConnectId);
		fxmain->iconConnectId = g_signal_connect(G_OBJECT(fxmain->trayIcon) 
												 , "activate"
												 , GTK_SIGNAL_FUNC(fx_main_tray_activate_func)
												 , fxmain);
	}
	else
	{
		message = (Message*)(ctlist->data);
		sipuri = message->sipuri;
		sid = fetion_sip_get_sid_by_sipuri(sipuri);
		bzero(path , sizeof(path));
		sprintf(path , "%s/%s.jpg" , config->iconPath , sid);
		free(sid);
		pb = gdk_pixbuf_new_from_file(path , NULL);
		if(pb == NULL)
			pb = gdk_pixbuf_new_from_file(SKIN_DIR"user_online.png" , NULL);
		gtk_status_icon_set_blinking(GTK_STATUS_ICON(fxmain->trayIcon) , TRUE);
		gtk_status_icon_set_from_pixbuf(GTK_STATUS_ICON(fxmain->trayIcon) , pb);
		g_signal_handler_disconnect(fxmain->trayIcon , fxmain->iconConnectId);
		fxmain->iconConnectId = g_signal_connect(G_OBJECT(fxmain->trayIcon) 
												 , "activate"
												 , GTK_SIGNAL_FUNC(fx_main_message_func)
												 , fxmain);
	}
	fxchat->hasFocus = CHAT_DIALOG_FOCUSED;
	return FALSE;
}
gboolean fx_chat_focus_out_func(GtkWidget *widget , GdkEventFocus *event , gpointer data)  
{
	FxChat* fxchat = (FxChat*)data;

	DEBUG_FOOTPRINT();

	fxchat->hasFocus = CHAT_DIALOG_NOT_FOCUSED;
	return FALSE;
}
void fx_chat_bind(FxChat* fxchat)
{
	
	FxMain* fxmain = fxchat->fxmain;
	Config* config = fxmain->user->config;
	char text[100];
	GdkPixbuf* pb;

	DEBUG_FOOTPRINT();

	bzero(text , sizeof(text));
	sprintf(text , "%s/%s.jpg" , config->iconPath , fxchat->conv->currentContact->sId);

	pb = gdk_pixbuf_new_from_file_at_size(text , 30 , 30 , NULL);
	if(pb != NULL)
	{
		gtk_image_set_from_pixbuf(GTK_IMAGE(fxchat->headimage) , pb);
		gtk_window_set_icon(GTK_WINDOW(fxchat->dialog) , pb);
	}
	bzero(text , sizeof(text));
	sprintf(text , "和[%s]聊天中" , fxchat->conv->currentContact->nickname);
	gtk_window_set_title(GTK_WINDOW(fxchat->dialog) , text);


}

static void fx_chat_name_box_func(GtkWidget *widget
		, GdkEventButton *event , gpointer data)
{
	FxChat *fxchat = (FxChat*)data;
	char *name = NULL , *sid = NULL , nametext[1024];
	Contact *contact = fxchat->conv->currentContact;
	FxProfile* fxprofile = NULL;

	DEBUG_FOOTPRINT();

	name = (contact->localname == NULL ||
		strlen(contact->localname) == 0)
		? contact->nickname : contact->localname;
	sid = fetion_sip_get_sid_by_sipuri(contact->sipuri);
	bzero(nametext , sizeof(nametext));
	switch(event->type)
	{
		case GDK_ENTER_NOTIFY :
			sprintf(nametext , "<span underline='low'><b>%s(%s)</b></span>" , name , sid);
			free(sid);
			gtk_label_set_markup(GTK_LABEL(fxchat->name_label) , nametext);
			break;
		case GDK_LEAVE_NOTIFY :
			sprintf(nametext , "<b>%s(%s)</b>" , name , sid);
			free(sid);
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

	DEBUG_FOOTPRINT();

	gtk_widget_translate_coordinates(widget , fxchat->dialog , 0 , 0 , &ex , &ey );
	gtk_window_get_position(GTK_WINDOW(fxchat->dialog) , &root_x , &root_y);
	x = root_x + ex + 3;
	y = root_y + ey + 46;

	fxemotion = fx_emotion_new(fxchat);
	fx_emotion_initialize(fxemotion , x , y);
}
void fx_chat_initialize(FxChat* fxchat)
{
	GtkWidget *vbox , *halign , *halign1 , *tophone_icon;
	GtkWidget *history_icon , *nouge_icon , *label , *action_area ;
	GtkWidget *send_button , *close_button;
	char nametext[512] , *sid , *name;
	Contact* contact = fxchat->conv->currentContact;
	GdkPixbuf *pb;
	GtkToolItem *emotionBtn;

	DEBUG_FOOTPRINT();

	fxchat->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_name(fxchat->dialog , "mainwindow");
	gtk_window_set_modal(GTK_WINDOW(fxchat->dialog) , FALSE);
	gtk_window_set_default_size(GTK_WINDOW(fxchat->dialog) , 550 , 490);
	gtk_widget_set_size_request(fxchat->dialog , 550 , 0);
	name = (contact->localname == NULL || strlen(contact->localname) == 0) ? contact->nickname : contact->localname;
	bzero(nametext , sizeof(nametext));
	sprintf(nametext , "与[%s]聊天中" , name);
	gtk_window_set_title(GTK_WINDOW(fxchat->dialog) , nametext);
	gtk_container_set_border_width(GTK_CONTAINER(fxchat->dialog) , 10);

	g_signal_connect(fxchat->dialog , "destroy" , G_CALLBACK(fx_chat_destroy) , fxchat);

	vbox = gtk_vbox_new(FALSE , 0);
	gtk_container_add(GTK_CONTAINER(fxchat->dialog) , vbox);
	action_area = gtk_hbox_new(FALSE , 0);

	fxchat->headbox = gtk_table_new(2 , 10 , FALSE );

	fxchat->headpix = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.jpg" , 40 , 40 , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxchat->dialog) , fxchat->headpix);
	fxchat->headimage = gtk_image_new_from_pixbuf(fxchat->headpix);
	gtk_table_attach(GTK_TABLE(fxchat->headbox) , fxchat->headimage
								, 0 , 1 , 0 , 2
								, GTK_FILL , GTK_FILL , 3 , 0);

	fxchat->name_label = gtk_label_new(NULL);
	sid = fetion_sip_get_sid_by_sipuri(contact->sipuri);
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


	fxchat->impre_label = gtk_label_new(contact->impression);
	gtk_label_set_justify(GTK_LABEL(fxchat->impre_label) , GTK_JUSTIFY_LEFT);
	gtk_table_attach(GTK_TABLE(fxchat->headbox) , fxchat->impre_label  
								, 1 , 10 , 1 , 2
								, GTK_FILL , GTK_FILL , 0 , 0);

	halign = gtk_alignment_new( 0 , 0 , 0 , 0);
	gtk_container_add(GTK_CONTAINER(halign) , fxchat->headbox);

	gtk_box_pack_start(GTK_BOX(vbox) , halign , FALSE , TRUE , 5);

	fxchat->recv_scroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_box_pack_start(GTK_BOX(vbox) , fxchat->recv_scroll , TRUE , TRUE , 0);
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
	gtk_text_buffer_create_tag(fxchat->recv_buffer , "blue" , "foreground" , "blue" , NULL);
	gtk_text_buffer_create_tag(fxchat->recv_buffer , "grey" , "foreground" , "grey" , NULL);
	gtk_text_buffer_create_tag(fxchat->recv_buffer , "green" , "foreground" , "green" , NULL);
	gtk_text_buffer_create_tag(fxchat->recv_buffer , "red" , "foreground" , "red" , NULL);
	gtk_text_buffer_create_tag(fxchat->recv_buffer , "lm10" , "left_margin" , 10 , NULL);
	gtk_text_buffer_create_tag(fxchat->recv_buffer , "small" , "left_margin" , 5 , NULL);
	gtk_text_buffer_get_end_iter(fxchat->recv_buffer , &(fxchat->recv_iter));
	gtk_text_buffer_create_mark(fxchat->recv_buffer , "scroll" , &(fxchat->recv_iter) , FALSE);
	
	/*toolbar begin*/
	fxchat->toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(fxchat->toolbar) , GTK_TOOLBAR_ICONS);
	gtk_box_pack_start(GTK_BOX(vbox) , fxchat->toolbar , FALSE , FALSE , 0);

	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"face_images/3.gif" , 16 , 16 , NULL);
	nouge_icon = gtk_image_new_from_pixbuf(pb);
	fxchat->nouge = gtk_toolbar_append_item(GTK_TOOLBAR(fxchat->toolbar)
					 						   , "表情" , "" , NULL , nouge_icon
					  						   , G_CALLBACK(fx_chat_on_emotion_clicked)
											   , fxchat );									   
	gtk_toolbar_append_space(GTK_TOOLBAR(fxchat->toolbar));
	tophone_icon = gtk_image_new_from_file(SKIN_DIR"phone.png");
	fxchat->tophone = gtk_toolbar_append_element(GTK_TOOLBAR(fxchat->toolbar)
					 						   , GTK_TOOLBAR_CHILD_TOGGLEBUTTON , NULL
											   , "对方手机" , "消息将以长短信的方式发送到对方手机" , NULL , tophone_icon
					  						   , G_CALLBACK(fx_chat_on_tophone_clicked)
											   , fxchat );
	gtk_toolbar_append_space(GTK_TOOLBAR(fxchat->toolbar));
	history_icon = gtk_image_new_from_file(SKIN_DIR"history.png");
	fxchat->historybutton = gtk_toolbar_append_item(GTK_TOOLBAR(fxchat->toolbar)
					 						   , " 聊天记录" , "查看聊天记录" , NULL , history_icon
					  						   , G_CALLBACK(fx_chat_on_history_clicked)
												,fxchat );
	gtk_toolbar_append_space(GTK_TOOLBAR(fxchat->toolbar));
	nouge_icon = gtk_image_new_from_file(SKIN_DIR"nudge.png");
	fxchat->nouge = gtk_toolbar_append_item(GTK_TOOLBAR(fxchat->toolbar)
					 						   , " 屏幕抖动" , "发送一个屏幕抖动" , NULL , nouge_icon
					  						   , G_CALLBACK(fx_chat_on_nudge_clicked)
											   , fxchat );									   
	gtk_toolbar_append_space(GTK_TOOLBAR(fxchat->toolbar));



	label = gtk_label_new("共可输入180个字 , 还可输入");
	fxchat->countLabel = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(fxchat->countLabel) , "[<span color='#0099ff'>180</span>]个字");
	gtk_container_add(GTK_CONTAINER(fxchat->toolbar) , label);
	gtk_container_add(GTK_CONTAINER(fxchat->toolbar) , fxchat->countLabel);
	/*toolbar end*/
	fxchat->send_scroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_box_pack_start(GTK_BOX(vbox) , fxchat->send_scroll , TRUE , TRUE , 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fxchat->send_scroll)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(fxchat->send_scroll)
									  , GTK_SHADOW_ETCHED_IN);
	fxchat->send_text = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxchat->send_text) , GTK_WRAP_WORD_CHAR);
	g_signal_connect(fxchat->send_text , "key_press_event" , G_CALLBACK(fx_chat_on_key_pressed) , fxchat);
	gtk_container_add(GTK_CONTAINER(fxchat->send_scroll) , fxchat->send_text);

 	fxchat->send_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxchat->send_text));
	gtk_text_buffer_get_iter_at_offset(fxchat->send_buffer , &(fxchat->send_iter) , 0);

	halign1 = gtk_alignment_new( 1 , 0 , 0 , 0);
	gtk_container_add(GTK_CONTAINER(halign1) , action_area);
	gtk_box_pack_start(GTK_BOX(vbox) , halign1 , FALSE , FALSE , 0);
	
	close_button = gtk_button_new_with_label("关闭");
	gtk_widget_set_usize(close_button , 100 , 30);
	gtk_box_pack_start(GTK_BOX(action_area) , close_button , FALSE , TRUE , 2);
	g_signal_connect(close_button , "clicked" , G_CALLBACK(fx_chat_on_close_clicked) , fxchat);

	send_button = gtk_button_new_with_label("发送");
	gtk_widget_set_usize(send_button , 100 , 30);
	gtk_box_pack_start(GTK_BOX(action_area) , send_button , FALSE , TRUE , 2);
	g_signal_connect(send_button , "clicked" , G_CALLBACK(fx_chat_on_send_clicked) , fxchat);

	gtk_window_set_position(GTK_WINDOW(fxchat->dialog) , GTK_WIN_POS_CENTER);
	gtk_window_set_opacity(GTK_WINDOW(fxchat->dialog) , 0.9);

	fx_chat_bind(fxchat);

	g_signal_connect(fxchat->dialog , "focus-in-event" , GTK_SIGNAL_FUNC(fx_chat_focus_in_func) , fxchat );

	g_signal_connect(fxchat->dialog , "focus-out-event" , GTK_SIGNAL_FUNC(fx_chat_focus_out_func) , fxchat );

	g_signal_connect(fxchat->send_buffer , "changed" , G_CALLBACK(fx_chat_on_text_buffer_changed) , fxchat);

	gtk_widget_show_all (vbox);
	gtk_widget_show(fxchat->dialog);
}

void fx_chat_free(FxChat* fxchat)
{

	fetion_history_free(fxchat->fhistory);

	DEBUG_FOOTPRINT();

	free(fxchat);
}

void fx_chat_destroy(GtkWidget* widget , gpointer data)
{
	FxChat* fxchat = (FxChat*)data;

	DEBUG_FOOTPRINT();

	fx_list_remove_chat_by_sipuri(&(fxchat->fxmain->clist)
								, fxchat->conv->currentContact->sipuri);
	fx_chat_free(fxchat);

}

void* fx_chat_send_message_thread(void* data)
{
	FxChat *fxchat = (FxChat*)data;
	Conversation *conv = fxchat->conv;
	GtkTextIter begin , end;
	char *text = NULL;
	struct tm *now = NULL;
	ThreadArgs *args = NULL;
	TimeOutArgs *targs = NULL;
	FxList *fxlist = NULL;

	DEBUG_FOOTPRINT();

	gdk_threads_enter();
	gtk_text_buffer_get_start_iter(fxchat->send_buffer , &begin);
	gtk_text_buffer_get_end_iter(fxchat->send_buffer , &end);
	text = gtk_text_buffer_get_text(fxchat->send_buffer , &begin , &end , TRUE);
	if(strlen(text) == 0)
	{
		fx_chat_add_information(fxchat , "不允许发送空信息");
		return NULL;
	}
	now = get_currenttime();
	fx_chat_add_message(fxchat , text , now , 1);
	gtk_text_buffer_delete(fxchat->send_buffer , &begin , &end);

	gdk_threads_leave();

	if(fetion_conversation_invite_friend(conv) > 0)
	{
		args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
		args->fxmain = fxchat->fxmain;
		args->sip = conv->currentSip;

		fxlist = fx_list_new(conv->currentSip);
		fx_list_append(&(fxchat->fxmain->slist) , fxlist);

		g_thread_create(fx_main_listen_thread_func , args , FALSE , NULL);

		g_usleep(100);
		fetion_conversation_send_sms(conv , text);

		/**
		 * start send keep alive message throuth chat chanel
		 * and put the timeout information into stack
		 */
		debug_info("Start periodically sending keep alive request");
		targs = timeout_args_new(fxchat->fxmain , conv->currentSip , conv->currentContact->sipuri);
		fxlist = fx_list_new(targs);
		fx_list_append(&(fxchat->fxmain->tlist) , fxlist);
		g_timeout_add_seconds(120 , (GSourceFunc)fx_main_chat_keep_alive_func , targs);
	}
	return NULL;
}

void fx_chat_send_message(FxChat* fxchat)
{
	Conversation* conv = fxchat->conv;
	Contact* contact = conv->currentContact;
	User *user = fxchat->fxmain->user;
	Config *config = user->config;
	GtkTextIter begin , end;
	char* text;
	struct tm* now;
	FxCode *fxcode;
	char reason[512];
	char tips[512];
	const char *code = NULL;
	int ret;
	int daycount , monthcount;

	DEBUG_FOOTPRINT();

	gtk_text_buffer_get_start_iter(fxchat->send_buffer , &begin);
	gtk_text_buffer_get_end_iter(fxchat->send_buffer , &end);

	if(contact == NULL)
		return;

	/**
	 * check whether 'send to phone' button is clicked ,
	 * if true , just send to phone and return
	 */
	if(fxchat->sendtophone == TRUE)
	{
		/***
		 * show message sent,and truncate the send text area
		 */
		text = gtk_text_buffer_get_text(fxchat->send_buffer , &begin , &end , TRUE);
		if(strlen(text) == 0)
		{
			fx_chat_add_information(fxchat , "不允许发送空信息");
			return;
		}
		if(user->boundToMobile == BOUND_MOBILE_DISABLE){

			user->verification = fetion_verification_new();
send:
			generate_pic_code(user);
			bzero(reason , sizeof(reason));
			sprintf(reason , "您还可以发送%d条免费短信（含本条），"
					"免费短信限额：每月%d条" , user->smsDayLimit - user->smsMonthCount
					, user->smsMonthLimit );
			bzero(tips , sizeof(tips));
			sprintf(tips , "温馨提示：发送更多免费短信，请立即绑定手机号，无需输入验证码");
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
		now = get_currenttime();
		fx_chat_add_message(fxchat , text , now , 1);

		bzero(tips , sizeof(tips));
		sprintf(tips , "发送成功，今天已发送%d条短信息，还可以发送%d条。"
				, daycount , user->smsDayLimit - daycount);
		fx_chat_add_information(fxchat , tips);
		gtk_text_buffer_delete(fxchat->send_buffer , &begin , &end);
		return;
	}
	/**
	 * if user`s online and has not started a thread for chating ,
	 * just created a thread for listening ,and send message throuch the chat channel
	 */
	if(contact->state > 0 && conv->currentSip == NULL)
	{
		conv->currentSip = fx_list_find_sip_by_sipuri(fxchat->fxmain->slist , contact->sipuri);
		if(conv->currentSip == NULL)
		{
			debug_info("CREATE A NEW MESSAGE THREAD");
			g_thread_create(fx_chat_send_message_thread , fxchat , FALSE , NULL);
			return;
		}
		debug_info("MESSAGE THREAD ALREADY EXIST");
	}

	/***
	 * show message sent,and truncate the send text area
	 */
	text = gtk_text_buffer_get_text(fxchat->send_buffer , &begin , &end , TRUE);
	if(strlen(text) == 0)
	{
		fx_chat_add_information(fxchat , "不允许发送空信息");
		return;
	}
	now = get_currenttime();
	fx_chat_add_message(fxchat , text , now , 1);

	gtk_text_buffer_delete(fxchat->send_buffer , &begin , &end);

	fetion_conversation_send_sms(conv , text);
} 

void* fx_chat_send_nudge_thread(void* data)
{
	FxChat *fxchat = (FxChat*)data;
	Conversation *conv = fxchat->conv;
	ThreadArgs *args = NULL;
	TimeOutArgs *targs = NULL;
	FxList *fxlist = NULL;

	DEBUG_FOOTPRINT();
	if(fetion_conversation_invite_friend(conv) > 0)
	{
		gdk_threads_enter();
		args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
		args->fxmain = fxchat->fxmain;
		args->sip = conv->currentSip;

		fxlist = fx_list_new(conv->currentSip);
		fx_list_append(&(fxchat->fxmain->slist) , fxlist);

		g_thread_create(fx_main_listen_thread_func , args , FALSE , NULL);
		fetion_conversation_send_nudge(conv);
		gdk_threads_leave();
		/**
		 * start send keep alive message throuth chat chanel
		 * and put the timeout information into stack
		 */
		targs = timeout_args_new(fxchat->fxmain , conv->currentSip , conv->currentContact->sipuri);
		fxlist = fx_list_new(targs);
		fx_list_append(&(fxchat->fxmain->tlist) , fxlist);
		debug_info("Start periodically sending keep alive request");
		g_timeout_add_seconds(120 , (GSourceFunc)fx_main_chat_keep_alive_func , targs);
	}
	return NULL;
}
void fx_chat_on_nudge_clicked(GtkWidget* widget , gpointer data)
{
	FxChat* fxchat = (FxChat*)data;
	Conversation* conv = fxchat->conv;
	Contact* contact = conv->currentContact;

	DEBUG_FOOTPRINT();

	if(contact->state <= 0)
	{
		fx_chat_add_information(fxchat , "对方不在线，不能发送窗口抖动");
		return;
	}
	if(contact->state > 0 && conv->currentSip == NULL)
	{
		conv->currentSip = fx_list_find_sip_by_sipuri(fxchat->fxmain->slist , contact->sipuri);
		if(conv->currentSip == NULL)
		{
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
	
	DEBUG_FOOTPRINT();

	gtk_window_get_position(GTK_WINDOW(fxchat->dialog) , &x , &y);
	for(i = 0 ; i < 4 ; i++)
	{
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
	
	DEBUG_FOOTPRINT();

	gdk_threads_enter();
	gtk_window_get_position(GTK_WINDOW(fxchat->dialog) , &x , &y);
	gdk_threads_leave();
	for(i = 0 ; i < 4 ; i++)
	{
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

void fx_chat_on_tophone_clicked(GtkWidget* widget , gpointer data)
{
	GtkToggleButton* btn = GTK_TOGGLE_BUTTON(widget);
	FxChat* fxchat = (FxChat*)data;
	User *user = fxchat->fxmain->user;
	char text[1024];

	DEBUG_FOOTPRINT();

	if(gtk_toggle_button_get_active(btn))
	{
		fxchat->sendtophone = TRUE;
		if(user->boundToMobile == BOUND_MOBILE_DISABLE){
			bzero(text , sizeof(text));
			sprintf(text , "消息将直接发送到对方手机。您还可以发送%d条免费短信。如果您还想"
					"发送更多免费短信，请绑定手机号，成为飞信移动用户。"
					, user->smsDayLimit - user->smsDayCount);
			fx_chat_add_information(fxchat , text);
		}else{
			fx_chat_add_information(fxchat , "信息将以长短信的方式发送到对方的手机上");
		}
	}
	else
	{
		fxchat->sendtophone = FALSE;
		fx_chat_add_information(fxchat , "消息将直接发送到对方飞信");
	}
}
void fx_chat_on_close_clicked(GtkWidget* widget , gpointer data)
{
	FxChat* fxchat = (FxChat*)data;

	DEBUG_FOOTPRINT();

	gtk_widget_destroy(fxchat->dialog);
}

void fx_chat_on_send_clicked(GtkWidget* widget , gpointer data)
{
	DEBUG_FOOTPRINT();

	fx_chat_send_message((FxChat*)data);
}
void fx_chat_on_history_clicked(GtkWidget* widget , gpointer data)
{
	FxChat* fxchat = (FxChat*)data;
	Conversation* conv = fxchat->conv;
	Contact* contact = conv->currentContact;
	FxHistory* fxhistory;

	DEBUG_FOOTPRINT();

	fxhistory = fx_history_new(fxchat->fxmain , contact->userId , contact->nickname);
	fx_history_initialize(fxhistory);
	gtk_dialog_run(GTK_DIALOG(fxhistory->dialog));
	gtk_widget_destroy(fxhistory->dialog);
	free(fxhistory);
}

gboolean fx_chat_on_key_pressed(GtkWidget* widget , GdkEventKey* event , gpointer data)
{

	FxChat *fxchat = NULL;
	Config *config = NULL;

	DEBUG_FOOTPRINT();

	if(event->keyval == GDK_Return)
	{
		fxchat = (FxChat*)data;
		config = fxchat->fxmain->user->config;
		if(config->sendMode == SEND_MODE_ENTER)
		{
			if(event->state & GDK_CONTROL_MASK){
				return FALSE;
			}else{
				fx_chat_send_message(fxchat);
				return TRUE;
			}
		}
		else
		{
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

gboolean fx_chat_on_text_buffer_changed(GtkTextBuffer* buffer , gpointer data)
{
	FxChat* fxchat = (FxChat*)data;
	GtkTextIter startIter , endIter;
	char text[48];
	const char *res = NULL;
	int count = gtk_text_buffer_get_char_count(buffer);

	DEBUG_FOOTPRINT();

	if(count <= 180)
	{
		bzero(text , sizeof(text));
		sprintf(text , "[<span color='#0099ff'>%d</span>]个字" , 180 - count);
		gtk_label_set_markup(GTK_LABEL(fxchat->countLabel) , text);
	}
	else
	{
		gtk_text_buffer_get_start_iter(buffer , &startIter);
		gtk_text_buffer_get_iter_at_offset(buffer , &endIter , 180);
		res = gtk_text_buffer_get_text(buffer , &startIter , &endIter , 180);
		gtk_text_buffer_set_text(buffer , res , strlen(res));
	}
	return FALSE;
}
