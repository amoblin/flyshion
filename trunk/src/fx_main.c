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
#include "libnotify/notify.h"
int window_pos_x;
int window_pos_y;

FxMain* fx_main_new()
{
	FxMain* fxmain = (FxMain*)malloc(sizeof(FxMain));

	DEBUG_FOOTPRINT();

	memset(fxmain , 0 , sizeof(FxMain));
	return fxmain;
}
void fx_main_initialize(FxMain* fxmain)
{
	int window_width , window_height;
	GdkScreen *screen;

	DEBUG_FOOTPRINT();
	fxmain->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_name(fxmain->window , "mainwindow");
	gtk_window_set_title(GTK_WINDOW(fxmain->window) , "OpenFetion");


	screen = gdk_screen_get_default();
	window_width = gdk_screen_get_width(screen);
	window_height = gdk_screen_get_height(screen);
	window_pos_x = window_width - WINDOW_WIDTH - 200;
	window_pos_y = (window_height - WINDOW_HEIGHT) / 2;

	gtk_window_move(GTK_WINDOW(fxmain->window) , window_pos_x , window_pos_y);
	gtk_container_set_border_width(GTK_CONTAINER(fxmain->window) , 0);
	g_signal_connect(G_OBJECT(fxmain->window)
				  , "delete-event"
				  , G_CALLBACK(fx_main_delete)
				  , fxmain);
	g_signal_connect(G_OBJECT(fxmain->window)
				  , "destroy"
				  , G_CALLBACK(fx_main_destroy)
				  , fxmain);
	g_signal_connect(G_OBJECT(fxmain->window)
				  , "window-state-event"
				  , G_CALLBACK(fx_main_window_state_func)
				  , NULL);
	gtk_window_set_default_size(GTK_WINDOW(fxmain->window) , WINDOW_WIDTH , WINDOW_HEIGHT);
	GdkPixbuf* icon = gdk_pixbuf_new_from_file(SKIN_DIR"user_online.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxmain->window) , icon);
	fxmain->trayIcon = gtk_status_icon_new_from_file(SKIN_DIR"user_offline.png");
	gtk_status_icon_set_tooltip(fxmain->trayIcon , "OpenFetion");
#ifdef HAVE_LIBNOTIFY
	fxmain->notify = notify_notification_new_with_status_icon("welcome"
			, "hehe" , NULL , fxmain->trayIcon);
#endif

	fxmain->iconConnectId = g_signal_connect(GTK_STATUS_ICON(fxmain->trayIcon)
									, "activate"
									, GTK_SIGNAL_FUNC(fx_main_tray_activate_func)
									, fxmain);
	g_signal_connect(GTK_STATUS_ICON(fxmain->trayIcon)
				   , "popup-menu"
				   , GTK_SIGNAL_FUNC(fx_main_tray_popmenu_func)
				   , fxmain);

	fxmain->mainbox = gtk_vbox_new(FALSE , 4);
	gtk_container_add(GTK_CONTAINER(fxmain->window) , fxmain->mainbox);
	
	fxmain->loginPanel = fx_login_new();
	fx_login_initialize(fxmain);
	gtk_widget_show_all(fxmain->window);

	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();
}
void fx_main_free(FxMain* fxmain)
{
	DEBUG_FOOTPRINT();

	if(fxmain->user != NULL)
		fetion_user_free(fxmain->user);
	free(fxmain);
}
void fx_main_set_user(FxMain* fxmain , User* user)
{
	DEBUG_FOOTPRINT();

	fxmain->user = user;
}
void update()
{
	g_usleep(1);
	while(gtk_events_pending())
	{ 
		 gtk_main_iteration();         
	};        
}
TimeOutArgs* timeout_args_new(FxMain *fxmain , FetionSip *sip , const char *sipuri)
{
	TimeOutArgs *args = (TimeOutArgs*)malloc(sizeof(TimeOutArgs));
	memset(args , 0 , sizeof(TimeOutArgs));
	args->fxmain = fxmain;
	args->sip = sip;
	args->terminated = FALSE;
	strcpy(args->sipuri , sipuri);
	return args;
}
GtkWidget* fx_main_create_menu(const char* name
							 , const char* iconpath
							 , GtkWidget* parent
							 , void (*func)(GtkWidget* item , gpointer data)
							 , gpointer data
							 )
{
	GtkWidget* item = gtk_image_menu_item_new_with_label(name);
	GtkWidget* img = gtk_image_new_from_file(iconpath);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item) , img);
	gtk_menu_shell_append(GTK_MENU_SHELL(parent) , item);
	if(func != NULL)
		g_signal_connect(item , "activate" , G_CALLBACK(func) , data);
	return item;
}
void fx_main_process_notification(FxMain* fxmain , const char* sipmsg)
{
	int event , notification_type;
	char* xml = NULL;
	fetion_sip_parse_notification(sipmsg , &notification_type , &event , &xml);
	g_usleep(1);
	switch(notification_type)
	{
		case NOTIFICATION_TYPE_PRESENCE:
			switch(event)
			{
				case NOTIFICATION_EVENT_PRESENCECHANGED :
					fx_main_process_presence(fxmain , xml);
					break;
				default:
					break;
			}
		case NOTIFICATION_TYPE_CONVERSATION :
			if(event == NOTIFICATION_EVENT_USERLEFT)
			{
				fx_main_process_user_left(fxmain , sipmsg);
				break;
			}
			break;
		case NOTIFICATION_TYPE_REGISTRATION :
			if(event == NOTIFICATION_EVENT_DEREGISTRATION)
			{
				fx_main_process_deregistration(fxmain);
				break;
			}
			break;
		case NOTIFICATION_TYPE_SYNCUSERINFO :
			if(event == NOTIFICATION_EVENT_SYNCUSERINFO)
			{
				fx_main_process_syncuserinfo(fxmain , xml);
				break;
			}
			break;
		case NOTIFICATION_TYPE_CONTACT :
			if(event == NOTIFICATION_EVENT_ADDBUDDYAPPLICATION)
			{
				fx_main_process_addbuddyapplication(fxmain , sipmsg);
				break;
			}
			break;
		default:
			break;
	}
	free(xml);

}
void fx_main_process_presence(FxMain* fxmain , const char* xml)
{
	char *crc = NULL;
	char *name = NULL;
	int oldstate , count;
	Contact *contactlist = NULL;
	Contact *contact , *contactToDelete;
	User* user = fxmain->user;
	GtkWidget* treeView = fxmain->mainPanel->treeView;
	GtkTreeModel* model = NULL;
	FxChat *fxchat = NULL;
	GtkTreeIter iter;
	GtkTreeIter parentIter;
	contactlist = fetion_user_parse_presence_body(xml , user);
	contact = contactlist;
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView));
	foreach_contactlist(contactlist , contact){
		fx_tree_get_buddy_iter_by_userid(model , contact->userId , &iter);
		gtk_tree_model_get(model , &iter
						 , B_CRC_COL    , &crc
						 , B_STATE_COL  , &oldstate
						 , -1);
		gdk_threads_enter();
		fxchat = fx_list_find_chat_by_sipuri(fxmain->clist , contact->sipuri);
		if(oldstate > 0 && contact->state <= 0 && contact->serviceStatus == STATUS_NORMAL)
		{
			gtk_tree_model_iter_parent(model , &parentIter , &iter);
			gtk_tree_model_get(model , &parentIter
							 , G_ONLINE_COUNT_COL	, &count
							 , -1);
			count --;
			gtk_tree_store_set(GTK_TREE_STORE(model) , &parentIter
							 , G_ONLINE_COUNT_COL	, count
							 , -1);
			fx_tree_move_to_the_last(model , &iter);
		}
		if(oldstate <= 0 && contact->state > 0 && contact->serviceStatus == STATUS_NORMAL)
		{
			gtk_tree_model_iter_parent(model , &parentIter , &iter);
			gtk_tree_model_get(model , &parentIter
							 , G_ONLINE_COUNT_COL	, &count
							 , -1);
			count ++;
			gtk_tree_store_set(GTK_TREE_STORE(model) , &parentIter
							 , G_ONLINE_COUNT_COL	, count
							 , -1);
			fx_tree_move_to_the_last(model , &iter);
			fx_tree_move_to_the_first(model , &iter);
		}
		if(fxchat != NULL)
		{
			fxchat->state = contact->state;
		}
		name = (contact->nickname == NULL || strlen(contact->localname) == 0) ? contact->nickname : contact->localname;
		gtk_tree_store_set(GTK_TREE_STORE(model) , &iter
						 , B_NAME_COL            , g_markup_escape_text(name , strlen(name))
						 , B_SIPURI_COL			 , contact->sipuri
						 , B_IMPRESSION_COL		 , contact->impression
						 , B_PHONENUM_COL		 , contact->mobileno
						 , B_USERID_COL			 , contact->userId
						 , B_STATE_COL			 , contact->state
						 , B_IDENTITY_COL		 , contact->identity
						 , B_DEVICE_COL			 , contact->devicetype
						 , B_RELATIONSTATUS_COL  , contact->relationStatus
						 , B_SERVICESTATUS_COL	 , contact->serviceStatus
						 , B_CARRIERSTATUS_COL   , contact->carrierStatus
						 , B_CARRIER_COL		 , contact->carrier
						 , B_IMAGE_CHANGED_COL	 , crc == NULL ? IMAGE_CHANGED :
						 (strcmp(crc , contact->portraitCrc) == 0 ? IMAGE_NOT_CHANGED : IMAGE_CHANGED)
						 , -1);

		gdk_threads_leave();
	}
}

static void process_system_message(FxMain *fxmain , const char *sipmsg){
	
	int showonce , type;
	char *msg , *url;
	FxSysmsg *sysmsg;
	fetion_sip_parse_sysmsg(sipmsg , &type , &showonce
			, &msg , &url);
	if(type == 0)
	{
		sysmsg = fx_sysmsg_new();
		fx_sysmsg_initialize(sysmsg);
		fx_sysmsg_bind(sysmsg , msg , url);
		gtk_dialog_run(GTK_DIALOG(sysmsg->dialog));

		gtk_widget_destroy(sysmsg->dialog);
		free(sysmsg);

	}
}

void fx_main_process_message(FxMain* fxmain , FetionSip* sip , const char* sipmsg)
{
	Message* msg = NULL;
	FxList* clist = fxmain->clist;
	FxList* mitem = NULL;
	FxChat* fxchat = NULL;
	User *user = fxmain->user;
	Config* config = user->config;
	char path[256] , *sid = NULL;
	GdkPixbuf *pb = NULL;

	DEBUG_FOOTPRINT();

	fetion_sip_parse_message(sip , sipmsg , &msg);
	fxchat = fx_list_find_chat_by_sipuri(clist , msg->sipuri);

	gdk_threads_enter();

	if(fxchat == NULL || fxchat->hasFocus == CHAT_DIALOG_NOT_FOCUSED)
	{
		if(fxchat == NULL)
		{
			sid = fetion_sip_get_sid_by_sipuri(msg->sipuri);
			if(strcmp(sid , "100") == 0)
			{
				free(sid);
				sid = NULL;
				process_system_message(fxmain , sipmsg);
				gdk_threads_leave();
				return;
			}
			if(config->autoPopup == AUTO_POPUP_ENABLE)
			{
				fxchat = fx_main_create_chat_window(fxmain , msg->sipuri);
				if(fxchat == NULL){
					gdk_threads_leave();
					return;
				}
				fx_chat_add_message(fxchat , msg->message , &(msg->sendtime) , 0);
			}
			else
			{
				mitem = fx_list_new(msg);
				fx_list_append(&(fxmain->mlist) , mitem );

#ifdef HAVE_LIBNOTIFY
				char iconPath[256];
				char notifySum[256];
				char *senderSid;
				GdkPixbuf *notifyIcon;
				Contact *senderContact;

				senderContact = fetion_contact_list_find_by_sipuri(user->contactList , msg->sipuri);
				bzero(notifySum , sizeof(notifySum));
				bzero(iconPath , sizeof(iconPath));
				sprintf(iconPath , "%s/%s.jpg" , config->iconPath , senderContact->sId);
				senderSid = fetion_sip_get_sid_by_sipuri(msg->sipuri);
				sprintf(notifySum , "%s(%s) 说:" , senderContact->nickname , senderContact->sId);
				notifyIcon = gdk_pixbuf_new_from_file_at_size(iconPath , 48 , 48 , NULL);
				notify_notification_update(fxmain->notify , notifySum
						, msg->message , NULL);
				notify_notification_set_icon_from_pixbuf(fxmain->notify , notifyIcon);
				notify_notification_show(fxmain->notify , NULL);
				g_object_unref(notifyIcon);
#endif
			}
		}
		else
		{
			fx_chat_add_message(fxchat , msg->message , &(msg->sendtime) , 0);
		}
		bzero(path , sizeof(path));
		sid = fetion_sip_get_sid_by_sipuri(msg->sipuri);
		sprintf(path , "%s/%s.jpg" , config->iconPath , sid);
		free(sid);
		sid = NULL;
		pb = gdk_pixbuf_new_from_file(path , NULL);
		if(pb == NULL)
			pb = gdk_pixbuf_new_from_file(SKIN_DIR"user_online.png" , NULL);
		gtk_status_icon_set_from_pixbuf(GTK_STATUS_ICON(fxmain->trayIcon) , pb);
		gtk_status_icon_set_blinking(GTK_STATUS_ICON(fxmain->trayIcon) , TRUE);
		g_signal_handler_disconnect(fxmain->trayIcon , fxmain->iconConnectId);
		
		fxmain->iconConnectId = g_signal_connect(G_OBJECT(fxmain->trayIcon) 
												 , "activate"
												 , GTK_SIGNAL_FUNC(fx_main_message_func)
												 , fxmain);
	}
	else
	{
		fx_chat_add_message(fxchat , msg->message , &(msg->sendtime) , 0);
	}
	gdk_threads_leave();
	if(config->isMute == MUTE_DISABLE)
		fx_sound_play_file(RESOURCE_DIR"newmessage.wav");
}
void fx_main_process_user_left(FxMain* fxmain , const char* msg)
{
	char *sipuri = NULL;
	FxList *clist = fxmain->clist;
	FxChat *fxchat = NULL;
	TimeOutArgs *timeout = NULL;
	Conversation *conv = NULL;

	DEBUG_FOOTPRINT();

	fetion_sip_parse_userleft(msg , &sipuri);
	/**
	 * remove sip struct from stack
	 */
	fx_list_remove_sip_by_sipuri(&(fxmain->slist) , sipuri);

	/**
	 * mark timeout struct to TERMINATED
	 * and the struct is to be remove from stack in timeout thread
	 */
	timeout = fx_list_find_timeout_by_sipuri(fxmain->tlist , sipuri);
	if(timeout != NULL)
		timeout->terminated = TRUE;
	
	/**
	 * if fxchat exist , set current sip struct to NULL , and exit thread
	 * else just exit current thread
	 */
	fxchat = fx_list_find_chat_by_sipuri(clist , sipuri);
	if(fxchat == NULL)
	{
		debug_info("User %s left conversation" , sipuri);
		debug_info("Thread exit");
		free(sipuri);
		g_thread_exit(0);
	}
	conv = fxchat->conv;
	conv->currentSip = NULL;

	debug_info("User %s left conversation" , sipuri);
	debug_info("Thread exit");
	free(sipuri);
	g_thread_exit(0);

}
FxChat* fx_main_create_chat_window(FxMain* fxmain , const char* sipuri)
{
	Conversation* conv = NULL;
	FxChat* fxchat = NULL;
	FxList* citem = NULL;
	Contact* contact = NULL;
	char* sid = NULL;

	DEBUG_FOOTPRINT();

	if((fxchat = fx_list_find_chat_by_sipuri(fxmain->clist , sipuri)) != NULL)
		return fxchat;
	conv = fetion_conversation_new(fxmain->user , sipuri , NULL);
	
	/* this buddy is not in the friend list*/
	if(conv == NULL)
	{
		sid = fetion_sip_get_sid_by_sipuri(sipuri);
		if(strcmp(sid , "100") == 0)
			return NULL;
		contact = fetion_contact_get_contact_info_by_no(fxmain->user , sid , FETION_NO);
		if(contact == NULL)
			return NULL;
		/* replace the sipuri*/
		bzero(contact->sipuri , sizeof(contact->sipuri));
		strcpy(contact->sipuri , sipuri);
		fetion_contact_list_append(fxmain->user->contactList , contact);
		conv = fetion_conversation_new(fxmain->user , sipuri , NULL);
	}
	if(conv == NULL)
		return NULL;
	fxchat = fx_chat_new(fxmain , conv);
	fx_chat_initialize(fxchat);
	citem = fx_list_new(fxchat);
	fx_list_append(&(fxmain->clist) , citem);
	return fxchat;
}
void fx_main_process_invitation(FxMain* fxmain , const char* sipmsg)
{
	char* sipuri = NULL;
	FetionSip* osip = NULL;
	FetionSip* sip = fxmain->user->sip;
	FxList *list = NULL;
	TimeOutArgs *timeout = NULL;
	char event[10];
	ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));

	DEBUG_FOOTPRINT();
	bzero(event , sizeof(event));
	if(fetion_sip_get_attr(sipmsg , "N" , event) != -1){
		return;
	}

	fetion_sip_parse_invitation(sip , fxmain->user->config->proxy , sipmsg , &osip , &sipuri);

	list = fx_list_new(osip);
	fx_list_append(&(fxmain->slist) , list);

	args->fxmain = fxmain;
	args->sip = osip;
	/**
	 * create a thread to listen in this channel
	 */
	g_thread_create(fx_main_listen_thread_func , args , FALSE , NULL);
	/**
	 * start send keep alive message throuth chat chanel
	 * and put the timeout information into stack
	 */
	debug_info("Start periodically sending keep alive request");
	timeout = timeout_args_new(fxmain , osip , sipuri);
	list = fx_list_new(timeout);
	fx_list_append(&(fxmain->tlist) , list);
	g_timeout_add_seconds(120 , (GSourceFunc)fx_main_chat_keep_alive_func , timeout);
} 

static void process_share_action_accept(FxMain *fxmain
		, FetionSip *sip , const char *sipmsg , const char *sipuri){
	FxShare *fxshare;
	Share *share;

	DEBUG_FOOTPRINT();

	fxshare = fx_share_find_by_sipuri(fxmain->shlist , sipuri);
	if(fxshare == NULL){
		return;
	}

	share = fxshare->share;
	fetion_sip_parse_shareaccept(sip , sipmsg , share);

	gdk_threads_enter();
	fx_share_start_transfer(fxshare);
	gdk_threads_leave();


}

static void process_share_action_cancel(FxMain *fxmain
		, FetionSip *sip , const char *sipmsg , const char *sipuri){
	FxShare *fxshare;
	Share *share;

	DEBUG_FOOTPRINT();

	fxshare = fx_share_find_by_sipuri(fxmain->shlist , sipuri);
	if(fxshare == NULL){
		return;
	}

	share = fxshare->share;
	fetion_sip_parse_shareaccept(sip , sipmsg , share);

	gdk_threads_enter();
	fx_share_start_transfer(fxshare);
	gdk_threads_leave();


}
void fx_main_process_incoming(FxMain* fxmain
		, FetionSip* sip , const char* sipmsg)
{
	IncomingType type;
	IncomingActionType action;
	char *sipuri = NULL;
	FxChat *fxchat = NULL;

	DEBUG_FOOTPRINT();

	fetion_sip_parse_incoming(sip , sipmsg , &sipuri , &type , &action);
	switch(type)
	{
		case INCOMING_NUDGE :
			{
				gdk_threads_enter();
				fxchat = fx_main_create_chat_window(fxmain , sipuri);
				gdk_threads_leave();

				fx_chat_nudge_in_thread(fxchat);

				gdk_threads_enter();
				fx_chat_add_information(fxchat , "收到一个窗口抖动");
				gdk_threads_leave();
				debug_info("Received a nudge from %s" , sipuri);
				break;
			}
		case INCOMING_SHARE_CONTENT :
			{
				switch(action){
					case INCOMING_ACTION_ACCEPT :
						process_share_action_accept(fxmain , sip , sipmsg , sipuri);
						break;
					case INCOMING_ACTION_CANCEL :
						process_share_action_cancel(fxmain , sip , sipmsg , sipuri);
						break;
					default:
						break;
				}
				break;
			}
		default:
			break;
	}
}

void fx_main_process_deregistration(FxMain* fxmain)
{
	GtkWidget *dialog;

	DEBUG_FOOTPRINT();

	gdk_threads_enter();
	fx_util_popup_warning(fxmain , "您的飞信已经别处登录，您被强制退出");
	gdk_threads_leave();
	gtk_main_quit();
}
void fx_main_process_syncuserinfo(FxMain* fxmain , const char* xml)
{
	Contact* contact = NULL;
	GtkTreeIter iter , cIter;
	char *userid = NULL;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(fxmain->mainPanel->treeView));

	DEBUG_FOOTPRINT();

	contact = fetion_user_parse_syncuserinfo_body(xml , fxmain->user);
	if(contact == NULL)
		return;
	gtk_tree_model_get_iter_root(model , &iter);
	do
	{
		if(gtk_tree_model_iter_children(model , &cIter , &iter))
		{
			do
			{
				gtk_tree_model_get(model , &cIter , B_USERID_COL , &userid , -1);
				if(strcmp(userid , contact->userId) == 0)
				{

					gtk_tree_store_set(GTK_TREE_STORE(model) , &cIter
									 , B_SIPURI_COL			 , contact->sipuri
									 , B_RELATIONSTATUS_COL	 , contact->relationStatus
									 , -1);
					free(userid);
					break;
				}
				free(userid);
			}
			while(gtk_tree_model_iter_next(model , &cIter));
		}
	}
	while(gtk_tree_model_iter_next(model , &iter));
}
void* fx_main_process_addbuddyapplication_thread(void* data)
{
	struct Args{
		FxMain* fxmain;
		char sipmsg[2048];
	} *args = (struct Args*)data;
	char *userid = NULL;
	char *sipuri = NULL;
	char *desc = NULL;
	int phrase;
	FxApp *fxapp = NULL;

	DEBUG_FOOTPRINT();

	fetion_sip_parse_addbuddyapplication(args->sipmsg , &sipuri , &userid , &desc , &phrase);

	fxapp = fx_app_new(args->fxmain , sipuri , userid , desc , phrase);
	gdk_threads_enter();
	fx_app_initialize(fxapp);
	gtk_dialog_run(GTK_DIALOG(fxapp->dialog));
	gtk_widget_destroy(fxapp->dialog);
	gdk_threads_leave();
	return NULL;
}
void fx_main_process_addbuddyapplication(FxMain* fxmain , const char* sipmsg)
{
	struct Args{
		FxMain* fxmain;
		char sipmsg[2048];
	} *args = (struct Args*)malloc(sizeof(struct Args));

	DEBUG_FOOTPRINT();

	memset(args , 0 , sizeof(struct args));
	args->fxmain = fxmain;
	strcpy(args->sipmsg , sipmsg);
	g_thread_create(fx_main_process_addbuddyapplication_thread , args , FALSE , NULL);
}
void fx_main_destroy(GtkWidget* widget , gpointer data)
{
	gtk_main_quit();
}
gboolean fx_main_delete(GtkWidget* widget , GdkEvent* event , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	Config *config = NULL;

	DEBUG_FOOTPRINT();

	if(fxmain->user != NULL && fxmain->user->loginStatus != -1)
	{
		config = fxmain->user->config;
		if(config->closeMode == CLOSE_ICON_MODE)
		{
			gtk_widget_hide_on_delete(widget);
			return TRUE;
		}
		else
		{
			gtk_widget_destroy(widget);
			return FALSE;
		}
	}
}
gboolean fx_main_window_state_func(GtkWidget* widget , GdkEventWindowState* event , gpointer data)
{

	DEBUG_FOOTPRINT();

	if(event->new_window_state == GDK_WINDOW_STATE_ICONIFIED)
	{
		gtk_widget_hide(widget);
		return TRUE;
	}
	return FALSE;
}
void fx_main_tray_activate_func(GtkWidget* widget , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;

	DEBUG_FOOTPRINT();

	gtk_window_deiconify(GTK_WINDOW(fxmain->window));
	gtk_window_move(GTK_WINDOW(fxmain->window) , window_pos_x , window_pos_y);
	gtk_widget_show(fxmain->window);

}

static void fx_main_mute_clicked(GtkWidget *widget , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	User *user = fxmain->user;
	Config *config = user->config;

	DEBUG_FOOTPRINT();

	if(config->isMute == MUTE_ENABLE)
		config->isMute = MUTE_DISABLE;
	else
		config->isMute = MUTE_ENABLE;

	fetion_config_save(user);

}

static void fx_main_direct_sms_clicked(GtkWidget *widget , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	FxDSMS *fxdsms = fx_dsms_new(fxmain);

	DEBUG_FOOTPRINT();

	if(fxmain->user->carrierStatus == CARRIER_STATUS_DOWN){
		fx_util_popup_warning(fxmain , SERVICE_DOWN_MESSAGE);
		return;
	}
	fx_dsms_initialize(fxdsms);
}

void fx_main_tray_popmenu_func(GtkWidget* widget , guint button , guint activate_time , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	Config *config = NULL;
	GtkWidget *muteItem;
	char stateMenu[48];
	int i;
	typedef struct 
	{
		FxMain* fxmain;
		StateType type;
	} Args;
	Args *args = NULL;
	GtkWidget* menu = NULL;
	GtkWidget* statemenu = NULL;
	GtkWidget* submenu = NULL;
	struct 
	{
		const gchar* name;
		const gchar* icon;
		int type;
	} presence[] = {
		{ "上线"	 , SKIN_DIR"user_online.png" , P_ONLINE } , 
		{ "离开"	 , SKIN_DIR"user_away.png" , P_AWAY } , 
		{ "忙碌"	 , SKIN_DIR"user_busy.png" , P_BUSY } ,
		{ "隐身"	 , SKIN_DIR"user_invisible.png" , P_HIDDEN } , 
		{ "外出就餐" , SKIN_DIR"user_away.png" , P_OUTFORLUNCH } ,
		{ "请勿打扰" , SKIN_DIR"user_away.png" , P_DONOTDISTURB } , 
		{ "马上回来" , SKIN_DIR"user_away.png" , P_RIGHTBACK } , 
		{ "会议中"	 , SKIN_DIR"user_away.png" , P_MEETING } , 
		{ "电话中"	 , SKIN_DIR"user_away.png" , P_ONTHEPHONE} ,
		{ NULL		 , NULL 			   , -1}
	};

	DEBUG_FOOTPRINT();

	menu = gtk_menu_new();
	fx_main_create_menu("关于OpenFetion" , SKIN_DIR"about.png"
					 , menu , fx_main_about_fetion_clicked , NULL);
	fx_main_create_menu("查找更新" , SKIN_DIR"about.png"
					 , menu , fx_main_check_update_clicked , NULL);
	fx_main_create_menu("关于作者" , SKIN_DIR"about.png"
					 , menu , fx_main_about_author_clicked , NULL);
	if(fxmain->user != NULL && fxmain->user->loginStatus != -1)
	{
		config = fxmain->user->config;
		muteItem = gtk_check_menu_item_new_with_label("关闭声音");
		if(config->isMute == MUTE_ENABLE)
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(muteItem) , TRUE);
		else
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(muteItem) , FALSE);
		g_signal_connect(muteItem , "activate" , G_CALLBACK(fx_main_mute_clicked) , fxmain);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu) , muteItem);
		fx_main_create_menu("信息查询" , SKIN_DIR"find.png"
						 , menu , fx_main_info_lookup_clicked , fxmain);
		if(fxmain->user->boundToMobile == BOUND_MOBILE_ENABLE){
			fx_main_create_menu("短信自己" , SKIN_DIR"myselfsms.png"
							 , menu , fx_main_send_to_myself_clicked , fxmain);
		}
		statemenu = fx_main_create_menu("修改状态" , SKIN_DIR"user_online.png" , menu , NULL , NULL);
		submenu = gtk_menu_new();
		for(i = 0 ; presence[i].type != -1 ; i++)
		{
			args = (Args*)malloc(sizeof(Args));
			args->fxmain = fxmain;
			args->type = presence[i].type;
			bzero(stateMenu , sizeof(stateMenu));
			sprintf(stateMenu , "%s      " , presence[i].name);
			fx_main_create_menu(stateMenu , presence[i].icon
							 , submenu , fx_main_set_state_clicked , args);
		}
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(statemenu) , submenu);
		fx_main_create_menu("短信群发" , SKIN_DIR"groupsend.png"
						 , menu , fx_main_send_to_many_clicked , fxmain);
		fx_main_create_menu("直接短信" , SKIN_DIR"directsms.png"
						 , menu , fx_main_direct_sms_clicked , fxmain);
		fx_main_create_menu("添加好友" , SKIN_DIR"addbuddy.png"
						 , menu , fx_main_add_buddy_clicked , fxmain);
		fx_main_create_menu("个人设置" , SKIN_DIR"personal.png"
						 , menu , fx_main_personal_setting_clicked , fxmain);
		fx_main_create_menu("系统设置" , SKIN_DIR"options.png"
						 , menu , fx_main_system_setting_clicked , fxmain);
	}
	fx_main_create_menu("退出Openfetion   " , SKIN_DIR"exit.png"
					 , menu , fx_main_destroy , fxmain);
	gtk_widget_show_all(menu);
	gtk_menu_popup(GTK_MENU(menu) , NULL , NULL , NULL , NULL 
				, button , activate_time);
}
int main(int argc , char* argv[])
{

	FxMain* fxmain = fx_main_new();

	if(!g_thread_supported())
		g_thread_init(NULL);
	gdk_threads_init();

#ifdef USE_GSTREAMER
	gst_init(&argc , &argv);
#endif

#ifdef HAVE_LIBNOTIFY
	notify_init("Openfetion");
#endif
	gtk_init(&argc , &argv);

	gtk_rc_parse(RESOURCE_DIR"style.rc");

	DEBUG_FOOTPRINT();

	fx_main_initialize(fxmain);

	return 0;
}
void* fx_main_listen_thread_func(void* data)
{
	ThreadArgs* args = (ThreadArgs*)data;
	FxMain* fxmain = args->fxmain;
	FetionSip* sip = args->sip;
	int type;
	SipMsg *msg , *pos;

	DEBUG_FOOTPRINT();

	debug_info("A new thread entered");

	sip = (sip == NULL ? fxmain->user->sip : sip);
	while(1){
		
		if(fxmain == NULL)
			g_thread_exit(0);
		g_usleep(10);
		gdk_threads_enter();
		msg = fetion_sip_listen(sip);
		gdk_threads_leave();
		pos = msg;
		while(pos != NULL){
			type = fetion_sip_get_type(pos->message);
			switch(type){
				case SIP_NOTIFICATION :
					fx_main_process_notification(fxmain , pos->message);
					break;
				case SIP_MESSAGE:
					fx_main_process_message(fxmain , sip , pos->message);
					break;
				case SIP_INVITATION:
					fx_main_process_invitation(fxmain , pos->message);
					break;
				case SIP_INCOMING :
					fx_main_process_incoming(fxmain , sip , pos->message);
				case SIP_SIPC_4_0:
					break;
				default:
					printf("%s\n" , pos->message);
					break;
			}
			pos = pos->next;
		}
		if(msg != NULL)
			fetion_sip_message_free(msg);
	}
	g_thread_exit(0);
}
void fx_main_message_func(GtkWidget* widget , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxList* mlist = fxmain->mlist;
	FxList* pos = mlist;
	FxList* tmp = NULL;
	FxChat* fxchat = NULL;
	Message* msg = NULL;
	char *sid = NULL;

	DEBUG_FOOTPRINT();

	while(pos != NULL){
		msg = (Message*)(pos->data);
		fxchat = fx_main_create_chat_window(fxmain , msg->sipuri);
		if(fxchat == NULL){
			sid = fetion_sip_get_sid_by_sipuri(msg->sipuri);
			if(strcmp(sid , "100") == 0){
				pos = pos->next;
				continue;
			}else{
				debug_info("Received a stranger,ignored");
				pos = pos->next;
				continue;
			}
		}
		fx_chat_add_message(fxchat , msg->message , &(msg->sendtime) , 0);
		free(msg);
		tmp = pos;
		pos = pos->next;
		free(tmp);
	}
	fxmain->mlist = NULL;
	fx_head_set_state_image(fxmain , fxmain->user->state);
	gtk_status_icon_set_blinking(GTK_STATUS_ICON(fxmain->trayIcon) , FALSE);
	g_signal_handler_disconnect(fxmain->trayIcon , fxmain->iconConnectId);
	fxmain->iconConnectId = g_signal_connect(G_OBJECT(fxmain->trayIcon) 
											 , "activate"
											 , GTK_SIGNAL_FUNC(fx_main_tray_activate_func)
											 , fxmain);
}
gboolean fx_main_register_func(User* user)
{
	if(fetion_user_keep_alive(user) < 0){
		debug_info("网络连接已断开,请重新登录");
		gtk_main_quit();
		return;
	}
	return TRUE;
}

gboolean fx_main_chat_keep_alive_func(TimeOutArgs* args)
{
	if(fetion_sip_keep_alive(args->sip) < 0)
		return FALSE;
	if(args->terminated == TRUE)
	{
		fx_list_remove_timeout_by_sipuri(&(args->fxmain->tlist) , args->sipuri);

		fetion_sip_free(args->sip);
		free(args);
		return FALSE;
	}
	return TRUE;
}
void fx_main_about_fetion_clicked(GtkWidget* widget , gpointer data)
{
	GtkWidget *dialog = gtk_about_dialog_new();
	GdkPixbuf *logo = NULL;

	DEBUG_FOOTPRINT();

	gtk_window_set_modal(GTK_WINDOW(dialog) , TRUE);
	gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "OpenFetion");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), FETION_VERSION); 
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), 
	"(c) Li Wenpeng");
	logo = gdk_pixbuf_new_from_file(SKIN_DIR"warning_icon.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(dialog) , logo);
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
	"OpenFetion 属于非盈利性软件,旨在方便Linux用户使用飞信.");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), 
	"http://www.basiccoder.com/openfetion");
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}

void fx_main_check_update_clicked(GtkWidget* widget , gpointer data)
{
	if(fork() == 0)
	{
		execlp("firefox" , "firefox" , "http://basiccoder.com/openfetion" , (char**)NULL);
	}
}

void fx_main_about_author_clicked(GtkWidget* widget , gpointer data)
{
	if(fork() == 0)
	{
		execlp("firefox" , "firefox" , "http://www.basiccoder.com" , (char**)NULL);
	}
}

void fx_main_send_to_myself_clicked(GtkWidget* widget , gpointer data)
{
	fx_bottom_on_sendtome_clicked(widget , data);
}

void fx_main_send_to_many_clicked(GtkWidget* widget , gpointer data)
{
	fx_bottom_on_sendtomany_clicked(widget , data);
}

void fx_main_personal_setting_clicked(GtkWidget* widget , gpointer data)
{
	fx_bottom_on_setting_clicked(widget , data);
}

void fx_main_system_setting_clicked(GtkWidget* widget , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	FxSet *fxset = fx_set_new(fxmain);
	fx_set_initialize(fxset);
	g_object_set(fxset->notebook , "page" , 1 , NULL);
	gtk_window_set_position(GTK_WINDOW(fxset->dialog) , GTK_WIN_POS_CENTER);
	gtk_dialog_run(GTK_DIALOG(fxset->dialog));
	gtk_widget_destroy(fxset->dialog);
}

void fx_main_set_state_clicked(GtkWidget* widget , gpointer data)
{
	fx_head_change_state_func(widget , data);
}

void fx_main_add_buddy_clicked(GtkWidget* widget , gpointer data)
{
	fx_bottom_on_addfriend_clicked(widget , data);
}

void fx_main_info_lookup_clicked(GtkWidget* widget , gpointer data)
{
	fx_bottom_on_lookup_clicked(widget , data);
}

FetionSip* fx_list_find_sip_by_sipuri(FxList* fxlist , const char* sipuri)
{
	FxList *pos = fxlist;
	FetionSip *sip = NULL;
	while(pos != NULL)
	{
		sip = (FetionSip*)(pos->data);
		if(strcmp(sip->sipuri , sipuri) == 0)
		{
			return sip;
		}
		pos = pos->next;
	}
	return NULL;
}
void fx_list_remove_sip_by_sipuri(FxList** fxlist , const char* sipuri)
{
	FxList* pos = *fxlist;
	FetionSip* sip = NULL;

	DEBUG_FOOTPRINT();

	while(pos != NULL)
	{
		sip = (FetionSip*)(pos->data);
		if(strcmp(sip->sipuri , sipuri) == 0)
		{	
			debug_info("Removing sip from sip list");
			if(pos == *fxlist)
			{
				*fxlist = pos->next;
			}
			else
			{
				pos->pre->next = pos->next;
				if(pos->next != NULL)
				{
					pos->next->pre = pos->pre;
				}
			}
			free(pos);
			break;
		}	
		pos = pos->next;
	}
}

FxChat* fx_list_find_chat_by_sipuri(FxList* fxlist , const char* sipuri)
{
	FxChat* fxchat;
	Contact* contact;
	FxList* pos = fxlist;

	DEBUG_FOOTPRINT();

	while(pos != NULL)
	{
		fxchat = (FxChat*)(pos->data);
		contact = fxchat->conv->currentContact;
		if(strcmp(contact->sipuri , sipuri) == 0)
			return fxchat;
		pos = pos->next;
	}
	return NULL;
}
void fx_list_remove_chat_by_sipuri(FxList** fxlist , const char* sipuri)
{
	FxList* pos = *fxlist;
	Contact* contact;
	FxChat* fxchat;

	DEBUG_FOOTPRINT();

	while(pos != NULL)
	{
		fxchat = (FxChat*)(pos->data);
		contact = fxchat->conv->currentContact;
		if(strcmp(contact->sipuri , sipuri) == 0)
		{	
			debug_info("Removing chat struct from chat list");
			if(pos == *fxlist)
			{
				*fxlist = pos->next;
			}
			else
			{
				pos->pre->next = pos->next;
				if(pos->next != NULL)
				{
					pos->next->pre = pos->pre;
				}
			}
			free(pos);
			break;
		}	
		pos = pos->next;
	}
}

TimeOutArgs* fx_list_find_timeout_by_sipuri(FxList* fxlist , const char* sipuri)
{
	TimeOutArgs *args = NULL;
	FxList* pos = fxlist;

	DEBUG_FOOTPRINT();

	while(pos != NULL)
	{
		args = (TimeOutArgs*)(pos->data);
		if(strcmp(args->sipuri , sipuri) == 0)
			return args;
		pos = pos->next;
	}
	return NULL;
}

void fx_list_remove_timeout_by_sipuri(FxList** fxlist , const char* sipuri)
{
	FxList* pos = *fxlist;
	TimeOutArgs *args = NULL;

	DEBUG_FOOTPRINT();

	while(pos != NULL)
	{
		args = (TimeOutArgs*)(pos->data);
		if(strcmp(args->sipuri , sipuri) == 0)
		{	
			debug_info("Removing timeout struct from timeout list");
			if(pos == *fxlist)
			{
				*fxlist = pos->next;
			}
			else
			{
				pos->pre->next = pos->next;
				if(pos->next != NULL)
				{
					pos->next->pre = pos->pre;
				}
			}
			free(pos);
			break;
		}	
		pos = pos->next;
	}

}
