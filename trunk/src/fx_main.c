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
#ifdef USE_GSTREAMER
#include <gst/gst.h>
#endif
#include <glib/gi18n.h>
#include <sys/select.h>
#include <locale.h>
#include <glib.h>

fd_set  fd_read;
gint presence_count = 0;
gint window_pos_x;
gint window_pos_y;
gint window_pos_x_old = 0;
gint window_pos_y_old = 0;
gint start_popup_presence = 0;
extern struct unacked_list *unackedlist;
GStaticMutex mutex = G_STATIC_MUTEX_INIT;
GdkScreen *current_screen;

static void fx_main_process_pggetgroupinfo(FxMain *fxmain , const gchar *sipmsg);
static void fx_main_process_pgpresencechanged(FxMain *fxmain , const gchar *sipmsg);
static gboolean key_press_func(GtkWidget *widget , GdkEventKey *event
		, gpointer data);

FxMain* fx_main_new()
{
	FxMain* fxmain = (FxMain*)malloc(sizeof(FxMain));

	memset(fxmain , 0 , sizeof(FxMain));
	fxmain->clist = fx_list_new(NULL);
	fxmain->mlist = fx_list_new(NULL);
	fxmain->slist = fx_list_new(NULL);
	fxmain->tlist = fx_list_new(NULL);
	fxmain->shlist = fx_list_new(NULL);
	fxmain->pglist = fx_list_new(NULL);
	return fxmain;
}

#if 0
static void fx_main_position_func(GtkWidget *UNUSED(widget) , GdkEventConfigure *event ,
                                    gpointer UNUSED(user_data)){
	window_pos_x = event->x;
	window_pos_y = event->y;
}
#endif

void fx_main_initialize(FxMain* fxmain)
{
	int window_width , window_height;
	Config    *config;

	fxmain->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_name(fxmain->window , "mainwindow");
	gtk_window_set_title(GTK_WINDOW(fxmain->window) , "OpenFetion");


	current_screen = gdk_screen_get_default();
	config = fetion_config_new();
	fetion_config_load_size(config);
	if(config->window_width == 0){
		window_width = gdk_screen_get_width(current_screen);
		window_height = gdk_screen_get_height(current_screen);
		window_pos_x = window_width - WINDOW_WIDTH - 200;
		window_pos_y = (window_height - WINDOW_HEIGHT) / 2;
	}else{
		window_pos_x = config->window_pos_x;
		window_pos_y = config->window_pos_y;
	}
	fetion_config_free(config);

	gtk_window_move(GTK_WINDOW(fxmain->window) , window_pos_x , window_pos_y);
	gtk_container_set_border_width(GTK_CONTAINER(fxmain->window) , 0);
	g_signal_connect(G_OBJECT(fxmain->window)
				  , "delete-event"
				  , G_CALLBACK(fx_main_delete)
				  , fxmain);
	g_signal_connect(G_OBJECT(fxmain->window)
				  , "key-press-event"
				  , G_CALLBACK(key_press_func)
				  , fxmain);
	g_signal_connect(G_OBJECT(fxmain->window)
				  , "destroy"
				  , G_CALLBACK(fx_main_destroy)
				  , fxmain);
	g_signal_connect(G_OBJECT(fxmain->window)
				  , "window-state-event"
				  , G_CALLBACK(fx_main_window_state_func)
				  , fxmain);
#if 0
	g_signal_connect(G_OBJECT(fxmain->window)
				  , "configure-event"
				  , G_CALLBACK(fx_main_position_func)
				  , NULL);
#endif

	gtk_window_set_default_size(
			GTK_WINDOW(fxmain->window),
			WINDOW_WIDTH , WINDOW_HEIGHT);
	GdkPixbuf* icon = gdk_pixbuf_new_from_file_at_size(
			SKIN_DIR"fetion.svg" , 48 , 48 , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxmain->window) , icon);
	fxmain->trayIcon = gtk_status_icon_new_from_file(
			SKIN_DIR"offline.svg");
	gtk_status_icon_set_tooltip(fxmain->trayIcon, "OpenFetion");
#ifdef USE_LIBNOTIFY
	fxmain->notify = notify_notification_new_with_status_icon("welcome"
			, "" , NULL , fxmain->trayIcon);
	notify_notification_set_timeout(fxmain->notify , 2500);
#endif

	fxmain->iconConnectId = g_signal_connect(
						GTK_STATUS_ICON(fxmain->trayIcon),
						"activate",
						GTK_SIGNAL_FUNC(fx_main_tray_activate_func),
						fxmain);
	g_signal_connect(GTK_STATUS_ICON(fxmain->trayIcon),
				   	"popup-menu",
				    GTK_SIGNAL_FUNC(fx_main_tray_popmenu_func),
				    fxmain);

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
	if(fxmain->user != NULL)
		fetion_user_free(fxmain->user);
	free(fxmain);
}

void fx_main_set_user(FxMain* fxmain , User* user)
{
	fxmain->user = user;
}

void fx_main_history_init(FxMain *fxmain)
{
	fxmain->history = fetion_history_new(fxmain->user);
}

void update()
{
	g_usleep(1);
	while(gtk_events_pending()){
		 gtk_main_iteration();
	}
}
TimeOutArgs* timeout_args_new(FxMain *fxmain , FetionSip *sip , const gchar *sipuri)
{
	TimeOutArgs *args = (TimeOutArgs*)malloc(sizeof(TimeOutArgs));
	memset(args , 0 , sizeof(TimeOutArgs));
	args->fxmain = fxmain;
	args->sip = sip;
	args->terminated = FALSE;
	strcpy(args->sipuri , sipuri);
	return args;
}
GtkWidget* fx_main_create_menu(const gchar* name
							 , const gchar* iconpath
							 , GtkWidget* parent
							 , void (*func)(GtkWidget* item , gpointer data)
							 , gpointer data
							 )
{
	GtkWidget *item = gtk_image_menu_item_new_with_label(name);
	GdkPixbuf *pb = gdk_pixbuf_new_from_file_at_size(iconpath , 16 , 16 , NULL);
	GtkWidget *img = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item) , img);
	gtk_menu_shell_append(GTK_MENU_SHELL(parent) , item);
	if(func != NULL)
		g_signal_connect(item , "activate" , G_CALLBACK(func) , data);
	return item;
}

GtkWidget* fx_main_create_menu1(const gchar* name
							 , const gchar *stockid
							 , GtkWidget* parent
							 , void (*func)(GtkWidget* item , gpointer data)
							 , gpointer data
							 )
{
	GtkWidget *item = gtk_image_menu_item_new_with_label(name);
#if 0
	GtkWidget *img = gtk_image_new_from_stock(stockid , GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item) , img);
#endif
	gtk_menu_shell_append(GTK_MENU_SHELL(parent) , item);
	if(func != NULL)
		g_signal_connect(item , "activate" , G_CALLBACK(func) , data);
	return item;
}

void fx_main_process_notification(FxMain* fxmain , const gchar* sipmsg)
{
	gint   event;
	gint   notification_type;
	gchar  *xml;

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
			if(event == NOTIFICATION_EVENT_USERLEFT){
				fx_main_process_user_left(fxmain , sipmsg);
				break;
			}
			break;
		case NOTIFICATION_TYPE_REGISTRATION :
			if(event == NOTIFICATION_EVENT_DEREGISTRATION){
				fx_main_process_deregistration(fxmain);
				break;
			}
			break;
		case NOTIFICATION_TYPE_SYNCUSERINFO :
			if(event == NOTIFICATION_EVENT_SYNCUSERINFO){
				fx_main_process_syncuserinfo(fxmain , xml);
				break;
			}
			break;
		case NOTIFICATION_TYPE_CONTACT :
			if(event == NOTIFICATION_EVENT_ADDBUDDYAPPLICATION){
				fx_main_process_addbuddyapplication(fxmain , sipmsg);
				break;
			}
			break;
		case NOTIFICATION_TYPE_PGGROUP :
			if(event == NOTIFICATION_EVENT_PGGETGROUPINFO){
				fx_main_process_pggetgroupinfo(fxmain , sipmsg);
				break;
			}
			if(event == NOTIFICATION_EVENT_PRESENCECHANGED){
				fx_main_process_pgpresencechanged(fxmain , sipmsg);
				break;
			}
			break;
		default:
			break;
	}
	g_free(xml);

}

static void *update_data(void *data)
{
	FxMain *fxmain = (FxMain*)data;
	User   *user = fxmain->user;

	fetion_user_save(user);
	fetion_contact_save(user);
	fx_tree_update_portrait(data);

	return NULL;
}

static void popup_online_notify(FxMain *fxmain, Contact *contact)
{
#ifdef USE_LIBNOTIFY
	gchar         notifySummary[256];
	gchar         notifyText[1024];
	gchar         iconPath[256];
	GdkPixbuf    *pixbuf;
	Config       *config;

	config = fxmain->user->config;

	if(start_popup_presence &&
		presence_count > fxmain->user->contactCount &&
		config->onlineNotify == ONLINE_NOTIFY_ENABLE){

		sprintf(iconPath , "%s/%s.jpg",
				config->iconPath , contact->sId);
		sprintf(notifySummary,
				_("%s , now ONLINE") , contact->nickname);
		sprintf(notifyText ,
				_("Phone Number: %s\n"
				  "Fetion Number: %s\n"
				  "Signature: %s")
				, contact->mobileno == NULL ||
					strlen(contact->mobileno) == 0 ?
					"未知" : contact->mobileno
				, contact->sId
				, contact->impression );
		pixbuf = gdk_pixbuf_new_from_file_at_size(
				iconPath,
				NOTIFY_IMAGE_SIZE,
				NOTIFY_IMAGE_SIZE , NULL);
		if(!pixbuf)
			pixbuf = gdk_pixbuf_new_from_file_at_size(
					SKIN_DIR"fetion.svg",
					NOTIFY_IMAGE_SIZE,
					NOTIFY_IMAGE_SIZE , NULL);

		notify_notification_update(fxmain->notify , notifySummary
				, notifyText , NULL);
		notify_notification_set_icon_from_pixbuf(fxmain->notify , pixbuf);
		notify_notification_show(fxmain->notify , NULL);
		g_object_unref(pixbuf);
	}
#endif
}

void fx_main_process_presence(FxMain* fxmain , const gchar* xml)
{
	gchar        *crc = NULL;
	gchar        *name;
	gint          oldstate , count;
	Contact      *contactlist;
	Contact      *contact;
	User         *user = fxmain->user;
	GtkWidget    *treeView = fxmain->mainPanel->treeView;
	GtkTreeModel *model;
	FxChat       *fxchat;
	GtkTreeIter   iter;
	GtkTreeIter   parentIter;

	contactlist = fetion_user_parse_presence_body(xml , user);
	contact = contactlist;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView));
	foreach_contactlist(contactlist , contact){

		if(fx_tree_get_buddy_iter_by_userid(model , contact->userId , &iter) == -1)
				continue;

		presence_count ++;

		/* all presence information has been pushed
		 * then start update local data and buddy portrait */
		if(presence_count == user->contactCount)
			g_thread_create(update_data, fxmain, FALSE, NULL);

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

			popup_online_notify(fxmain, contact);

		}
		if(fxchat)
			fxchat->state = contact->state;

		name = (contact->nickname == NULL || strlen(contact->localname) == 0)?
				contact->nickname : contact->localname;
		gtk_tree_store_set(GTK_TREE_STORE(model) , &iter
						 , B_NAME_COL            , g_markup_escape_text(name, -1)
						 , B_SIPURI_COL			 , contact->sipuri
						 , B_IMPRESSION_COL		 , g_markup_escape_text(contact->impression, -1)
						 , B_PHONENUM_COL		 , contact->mobileno
						 , B_USERID_COL			 , contact->userId
						 , B_STATE_COL			 , contact->state
						 , B_IDENTITY_COL		 , contact->identity
						 , B_DEVICE_COL			 , contact->devicetype
						 , B_RELATIONSTATUS_COL  , contact->relationStatus
						 , B_SERVICESTATUS_COL	 , contact->serviceStatus
						 , B_CARRIERSTATUS_COL   , contact->carrierStatus
						 , B_CARRIER_COL		 , contact->carrier
						 , B_CRC_COL			 , contact->portraitCrc
						 , B_IMAGE_CHANGED_COL	 , crc == NULL ? IMAGE_CHANGED :
						 (strcmp(crc , contact->portraitCrc) == 0 ? IMAGE_NOT_CHANGED : IMAGE_CHANGED)
						 , -1);
		g_free(crc);
		gdk_threads_leave();
	}
}

static void process_system_message(const char *sipmsg)
{

	gint     showonce;
	gint     type;
	gchar    *msg;
	gchar    *url;
	FxSysmsg *sysmsg;

	fetion_sip_parse_sysmsg(sipmsg , &type , &showonce
			, &msg , &url);

	if(type == 0){
		sysmsg = fx_sysmsg_new();
		fx_sysmsg_initialize(sysmsg);
		fx_sysmsg_bind(sysmsg , msg , url);
		gtk_dialog_run(GTK_DIALOG(sysmsg->dialog));

		gtk_widget_destroy(sysmsg->dialog);
		g_free(sysmsg);

	}
}

static void process_group_message(FxMain *fxmain , Message *message)
{
	FxPGGroup     *fxpgcur;
	FxPGGroup     *fxpg;
	PGGroupMember *memcur;
	FxList        *pglist;
	FxList        *cur;
	User          *user;
	Config        *config;
	GdkPixbuf     *pixbuf;
	gchar         path[1024];
	gchar         *sid;

 	user = fxmain->user;
	config = user->config;

	pglist = fxmain->pglist;

	foreach_list(pglist , cur){
		fxpgcur = (FxPGGroup*)(cur->data);
		if(strcmp(fxpgcur->pggroup->pguri,
					message->pguri) == 0){
			fxpg = fxpgcur;
		       	break;
		}
	}

	gdk_threads_enter();
	if(fxpg == NULL || fxpg->hasFocus == CHAT_DIALOG_NOT_FOCUSED ){
	    if(fxpg == NULL){
		    if(config->autoPopup == AUTO_POPUP_ENABLE){
			    fxpg = pg_create_window(fxmain , message->pguri);
			    foreach_pg_member(fxpg->pggroup->member , memcur){
				if(strcmp(memcur->sipuri , message->sipuri) == 0)
				    pg_add_message(fxpg , message->message
					   , &(message->sendtime) , memcur);
			    }

		    }else{
			    cur = fx_list_new(message);
			    fx_list_append(fxmain->mlist , cur);
		    }
		}else{
		    foreach_pg_member(fxpg->pggroup->member , memcur){
			if(strcmp(memcur->sipuri , message->sipuri) == 0)
			    pg_add_message(fxpg , message->message , &(message->sendtime) , memcur);
		    }
		}
		sid = fetion_sip_get_pgid_by_sipuri(message->pguri);
		snprintf(path , sizeof(path) - 1 , "%s/PG%s.jpg" , config->iconPath , sid);
		g_free(sid);
		pixbuf = gdk_pixbuf_new_from_file(path , NULL);
		if(pixbuf == NULL)
			pixbuf = gdk_pixbuf_new_from_file(SKIN_DIR"online.svg" , NULL);
		gtk_status_icon_set_from_pixbuf(GTK_STATUS_ICON(fxmain->trayIcon) , pixbuf);
		g_object_unref(pixbuf);
		gtk_status_icon_set_blinking(GTK_STATUS_ICON(fxmain->trayIcon) , TRUE);
		g_signal_handler_disconnect(fxmain->trayIcon , fxmain->iconConnectId);
		fxmain->iconConnectId = g_signal_connect(G_OBJECT(fxmain->trayIcon)
							, "activate"
							, GTK_SIGNAL_FUNC(fx_main_message_func)
							, fxmain);
	}else{
		foreach_pg_member(fxpg->pggroup->member , memcur){
		    if(strcmp(memcur->sipuri , message->sipuri) == 0)
			pg_add_message(fxpg , message->message , &(message->sendtime) , memcur);
		}
	}
	gdk_threads_leave();
	if(config->isMute == MUTE_DISABLE)
		fx_sound_play_file(RESOURCE_DIR"newmessage.wav");
}

static void popup_msg_notify(FxMain *fxmain, Contact *senderContact, Message *msg)
{
#ifdef USE_LIBNOTIFY
	Config    *config = fxmain->user->config;
	GdkPixbuf *notifyIcon;
	gchar     *senderSid;
	gchar      iconPath[256];
	gchar      notifySum[256];

	if(config->msgAlert == MSG_ALERT_ENABLE){
		if(senderContact){
			sprintf(iconPath, "%s/%s.jpg",
					config->iconPath, senderContact->sId);
			senderSid = fetion_sip_get_sid_by_sipuri(msg->sipuri);
			sprintf(notifySum, _("%s(%s) said:"),
					senderContact->nickname , senderContact->sId);
			g_free(senderSid);

			notifyIcon = gdk_pixbuf_new_from_file_at_size(
					iconPath , 48 , 48 , NULL);
			notify_notification_update(fxmain->notify,
					notifySum, msg->message , NULL);

			if(!notifyIcon)
				notifyIcon = gdk_pixbuf_new_from_file_at_size(
						SKIN_DIR"fetion.svg", 48, 48, NULL);

			notify_notification_set_icon_from_pixbuf(
						fxmain->notify , notifyIcon);
			notify_notification_show(fxmain->notify , NULL);
			g_object_unref(notifyIcon);
		}
	}
#endif
}

void fx_main_process_message(FxMain* fxmain , FetionSip* sip , const gchar* sipmsg)
{
	Message   *msg;
	FxList    *clist;
	FxList    *mitem;
	FxChat    *fxchat;
	User      *user;
	Config    *config;
	gchar      path[256];
	gchar     *sid;
	GdkPixbuf *pb;
	Contact *senderContact;

	clist = fxmain->clist;
	user = fxmain->user;
	config = user->config;

	fetion_sip_parse_message(sip , sipmsg , &msg);

	/* group message */
	if(msg->pguri){
	    process_group_message(fxmain , msg);
	    return;
	}

	fxchat = fx_list_find_chat_by_sipuri(clist , msg->sipuri);
	sid = fetion_sip_get_sid_by_sipuri(msg->sipuri);

	senderContact = fetion_contact_list_find_by_sipuri(
						user->contactList , msg->sipuri);

	/* system message */
	if(strlen(sid) < 5 || strcmp(sid , "10000") == 0){
		g_free(sid);
		if(config->closeSysMsg == CLOSE_SYSMSG_ENABLE)
			return;
		gdk_threads_enter();
		process_system_message(sipmsg);
		gdk_threads_leave();
		fetion_message_free(msg);
		return;
	}

	if(senderContact)
		fx_main_add_history(fxmain, senderContact->nickname,
				senderContact->userId, msg->message, 0);


	gdk_threads_enter();

	if(!fxchat || fxchat->hasFocus == CHAT_DIALOG_NOT_FOCUSED){
		/* chat window does not exist */
		if(!fxchat){

			/* auto popup enabled */
			if(config->autoPopup == AUTO_POPUP_ENABLE){
				fxchat = fx_main_create_chat_window(fxmain , msg->sipuri);
				if(!fxchat){
					fetion_message_free(msg);
					gdk_threads_leave();
					return;
				}
				fx_chat_add_message(fxchat , msg->message,
								&(msg->sendtime) , 0 , msg->sysback);
				fetion_message_free(msg);
				gdk_threads_leave();
				return;
			}else{
				/* chat window doesn`t exist and auto-pupup wasn`t enabled
				 * just push message into message queue,wait for user action */
				mitem = fx_list_new(msg);
				fx_list_append(fxmain->mlist , mitem );
				popup_msg_notify(fxmain, senderContact, msg);
			}
		/* chat window exist,but not focused */
		}else{
			fx_chat_add_message(fxchat , msg->message,
						 &(msg->sendtime) , 0 , msg->sysback);
			fxchat->unreadMsgCount ++;
			fx_chat_update_window(fxchat);
		}

		sprintf(path , "%s/%s.jpg" , config->iconPath , sid);
		g_free(sid);
		pb = gdk_pixbuf_new_from_file(path , NULL);
		if(!pb)
			pb = gdk_pixbuf_new_from_file(SKIN_DIR"online.svg" , NULL);
		gtk_status_icon_set_from_pixbuf(GTK_STATUS_ICON(fxmain->trayIcon) , pb);
		g_object_unref(pb);

		gtk_status_icon_set_blinking(GTK_STATUS_ICON(fxmain->trayIcon) , TRUE);
		g_signal_handler_disconnect(fxmain->trayIcon , fxmain->iconConnectId);

		fxmain->iconConnectId = g_signal_connect(G_OBJECT(fxmain->trayIcon)
							, "activate"
							, GTK_SIGNAL_FUNC(fx_main_message_func)
							, fxmain);

		/* no message was pushed into the message queue,just free it */
		if(fxchat && fxchat->hasFocus == CHAT_DIALOG_NOT_FOCUSED)
			fetion_message_free(msg);
	}else{
		fx_chat_add_message(fxchat , msg->message,
					   	&(msg->sendtime) , 0, msg->sysback);

		/* message was showed in the chat dialog directly,just free it */
		fetion_message_free(msg);
	}
	gdk_threads_leave();
	if(config->isMute == MUTE_DISABLE)
		fx_sound_play_file(RESOURCE_DIR"newmessage.wav");
}
void fx_main_process_user_left(FxMain* fxmain , const gchar* msg)
{
	gchar         *sipuri;
	FxList       *clist;
	FxChat       *fxchat;
	Conversation *conv;

	clist = fxmain->clist;

	fetion_sip_parse_userleft(msg , &sipuri);
	/* remove sip struct from stack	 */
	fx_list_remove_sip_by_sipuri(fxmain->slist , sipuri);

	/* if fxchat exist , set current sip struct to NULL ,
	 * and exit thread, orelse just exit current thread	 */
	fxchat = fx_list_find_chat_by_sipuri(clist , sipuri);
	if(!fxchat) {
		debug_info("User %s left conversation" , sipuri);
		debug_info("Thread exit");
		g_free(sipuri);
		g_thread_exit(0);
	}
	conv = fxchat->conv;
	conv->currentSip = NULL;

	debug_info("User %s left conversation" , sipuri);
	debug_info("Thread exit");
	g_free(sipuri);
	g_thread_exit(0);

}
FxChat* fx_main_create_chat_window(FxMain* fxmain , const gchar* sipuri)
{
	Conversation *conv;
	FxChat       *fxchat;
	FxList       *citem;
	Contact      *contact;
	gchar        *sid;

	if((fxchat = fx_list_find_chat_by_sipuri(fxmain->clist , sipuri)) != NULL)
		return fxchat;
	conv = fetion_conversation_new(fxmain->user , sipuri , NULL);

	/* this buddy is not in the friend list*/
	if(!conv) {
		sid = fetion_sip_get_sid_by_sipuri(sipuri);
		if(strlen(sid) < 8 || strcmp(sid , "10000") == 0){
			g_free(sid);
			return NULL;
		}

		contact = fetion_contact_get_contact_info_by_no(fxmain->user , sid , FETION_NO);

		if(!contact)
			return NULL;
		/* replace the sipuri*/
		memset(contact->sipuri, 0, sizeof(contact->sipuri));
		strcpy(contact->sipuri , sipuri);
		fetion_contact_list_append(fxmain->user->contactList , contact);
		conv = fetion_conversation_new(fxmain->user , sipuri , NULL);
	}
	if(!conv)
		return NULL;
	fxchat = fx_chat_new(fxmain , conv);
	fx_chat_initialize(fxchat);
	citem = fx_list_new(fxchat);
	fx_list_append(fxmain->clist , citem);

	return fxchat;
}
void fx_main_process_invitation(FxMain* fxmain , const gchar* sipmsg)
{
	gchar       *sipuri;
	FetionSip   *osip;
	FetionSip   *sip;
	FxList      *list;
	TimeOutArgs *timeout;
	gchar        event[16];
	ThreadArgs  *args;

	args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
	sip = fxmain->user->sip;

	memset(event, 0, sizeof(event));
	if(fetion_sip_get_attr(sipmsg , "N" , event) != -1)
		return;

	fetion_sip_parse_invitation(sip,
			fxmain->user->config->proxy,
			sipmsg , &osip , &sipuri);

	list = fx_list_new(osip);
	fx_list_append(fxmain->slist , list);

	args->fxmain = fxmain;
	args->sip = osip;

	/* create a thread to listen in this channel */
	g_thread_create(fx_main_listen_thread_func,
			args , FALSE , NULL);

	/* start send keep alive message throuth chat chanel
	 * and put the timeout information into stack */
	debug_info("Start periodically sending keep alive request");
	timeout = timeout_args_new(fxmain , osip , sipuri);
	list = fx_list_new(timeout);
	fx_list_append(fxmain->tlist , list);
}
#if 0
static void process_share_action_accept(FxMain *fxmain
		, FetionSip *sip , const char *sipmsg , const char *sipuri){

	return;
}

static void process_share_action_cancel(FxMain *fxmain
		, FetionSip *sip , const char *sipmsg , const char *sipuri){

	return;

}
#endif
void fx_main_process_incoming(FxMain* fxmain
		, FetionSip* sip , const gchar* sipmsg)
{
	IncomingType       type;
	IncomingActionType action;
	gchar              *sipuri;
	FxChat             *fxchat;

	fetion_sip_parse_incoming(sip , sipmsg , &sipuri , &type , &action);
	switch(type) {
		case INCOMING_NUDGE :
			{
				gdk_threads_enter();
				fxchat = fx_main_create_chat_window(fxmain , sipuri);
				gdk_threads_leave();

				fx_chat_nudge_in_thread(fxchat);

				gdk_threads_enter();
				fx_chat_add_information(fxchat , _("Receive a window jitter"));
				gdk_threads_leave();
				debug_info("Received a nudge from %s" , sipuri);
				break;
			}
		case INCOMING_SHARE_CONTENT :
			{
				switch(action){
					case INCOMING_ACTION_ACCEPT :
						//process_share_action_accept(fxmain , sip , sipmsg , sipuri);
						break;
					case INCOMING_ACTION_CANCEL :
						//process_share_action_cancel(fxmain , sip , sipmsg , sipuri);
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

static void fx_main_process_group(FxMain *fxmain , const gchar *xml)
{
	User *user = fxmain->user;
	PGGroup *pgcur;

	user->pggroup = pg_group_parse_list(xml);
	if(user->pggroup == NULL)
	    return;
	pg_group_get_info(user , user->pggroup);
	foreach_pg_group(user->pggroup , pgcur){
		pg_group_send_invitation(user , pgcur);
		pg_group_get_group_members(user , pgcur);
	}
}

void fx_main_process_sipc(FxMain* fxmain , const gchar* sipmsg)
{
	int callid;
	int code;
	char *xml = NULL;
	User *user = fxmain->user;

	struct unacked_list *ulist;

	PGGroup *pggroup = user->pggroup;
	PGGroup *pgcur;

	code = fetion_sip_parse_sipc(sipmsg , &callid , &xml);

	/* get group info response */
	if(callid == user->pgGroupCallId){
		gdk_threads_enter();
		fx_main_process_group(fxmain , xml);
		gdk_threads_leave();
		return;
	}

	foreach_unacked_list(unackedlist , ulist) {
		if(callid == ulist->message->callid){
			unacked_list_remove(unackedlist , ulist);
			fetion_message_free(ulist->message);
			free(ulist);
			return;
		}
	}

	if(pggroup != NULL){
		foreach_pg_group(pggroup , pgcur){
#if 0
		    	/* get member contact info response */
			foreach_pg_member(pgcur->member , memcur){
				if(memcur->getContactInfoCallId == callid){
					memcur->contact = pg_group_parse_contact_info(xml);
					return;
				}
			}
#endif
			/* group invitation response */
			if(pgcur->inviteCallId == callid && code == 200){
				if(pgcur->hasAcked == 0){
				    pg_group_send_invite_ack(user , sipmsg);
				    pgcur->hasAcked = 1;
				    return;
				}

			}
			/* get group member response */
			if(pgcur->getMembersCallId == callid){
				pg_group_parse_member_list(pggroup , sipmsg);
				pg_group_subscribe(user , pgcur);
				return;
			}
		}
	}

	free(xml);
}

void fx_main_process_deregistration(FxMain* fxmain)
{

	gdk_threads_enter();
	fx_util_popup_warning(fxmain , _("Your fetion login elsewhere. You are forced quit."));
	gdk_threads_leave();
	gtk_main_quit();
}
void fx_main_process_syncuserinfo(FxMain* fxmain , const gchar* xml)
{
	Contact*      contact;
	GtkTreeIter   iter;
	GtkTreeIter   cIter;
	gchar         *userid;
	GtkTreeModel* model = gtk_tree_view_get_model(
			GTK_TREE_VIEW(fxmain->mainPanel->treeView));

	contact = fetion_user_parse_syncuserinfo_body(xml , fxmain->user);

	if(!contact)
		return;

	gtk_tree_model_get_iter_root(model , &iter);

	do {
		if(gtk_tree_model_iter_children(model , &cIter , &iter)){
			do{
				gtk_tree_model_get(model , &cIter , B_USERID_COL , &userid , -1);
				if(strcmp(userid , contact->userId) == 0){

					gdk_threads_enter();
					gtk_tree_store_set(GTK_TREE_STORE(model) , &cIter
									 , B_SIPURI_COL			 , contact->sipuri
									 , B_RELATIONSTATUS_COL	 , contact->relationStatus
									 , -1);
					gdk_threads_leave();
					g_free(userid);
					goto end;
				}
				g_free(userid);
			}
			while(gtk_tree_model_iter_next(model , &cIter));
		}
	}
	while(gtk_tree_model_iter_next(model , &iter));
end:
	return;
}
void* fx_main_process_addbuddyapplication_thread(void* data)
{
	struct Args {
		FxMain *fxmain;
		gchar   sipmsg[2048];
	} *args = (struct Args*)data;
	gchar *userid;
	gchar *sipuri;
	gchar *desc;
	gint   phrase;
	FxApp *fxapp;


	fetion_sip_parse_addbuddyapplication(args->sipmsg,
			&sipuri , &userid , &desc , &phrase);

	fxapp = fx_app_new(args->fxmain, sipuri,
			userid , desc , phrase);

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

	memset(args , 0 , sizeof(struct args));
	args->fxmain = fxmain;
	strcpy(args->sipmsg , sipmsg);
	g_thread_create(fx_main_process_addbuddyapplication_thread , args , FALSE , NULL);
}
void fx_main_destroy(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	User   *user = fxmain->user;
	if(user){
		Config *config = user->config;
		fetion_config_save_size(config);
	}
	gtk_main_quit();
}
gboolean fx_main_delete(GtkWidget *widget , GdkEvent *UNUSED(event) , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	FxClose *fxclose;
	Config *config;

	int ret;

	int     window_width;
	int     window_height;
	int     window_x;
	int     window_y;

	if(fxmain->user){
		config = fxmain->user->config;
		gtk_window_get_position(GTK_WINDOW(fxmain->window),
				&window_x, &window_y);
		config->window_pos_x = window_x;
		config->window_pos_y = window_y;
		gtk_window_get_size(GTK_WINDOW(fxmain->window),
				&window_width, &window_height);
		config->window_width = window_width;
		config->window_height = window_height;
	}

	if(fxmain->user){
		if(config->closeAlert == CLOSE_ALERT_ENABLE){
			fxclose = fx_close_new(fxmain);
			fx_close_initialize(fxclose);
			ret = gtk_dialog_run(GTK_DIALOG(fxclose->dialog));
			if(ret == GTK_RESPONSE_OK){
				if(fx_close_alert(fxclose) == CLOSE_ALERT_DISABLE){
					config->closeMode = fx_close_get_action(fxclose);
					config->closeAlert = CLOSE_ALERT_DISABLE;
					fetion_config_save(fxmain->user);
				}
				if(fx_close_get_action(fxclose) == CLOSE_DESTROY_MODE){
					gtk_widget_destroy(fxclose->dialog);
					free(fxclose);
					gtk_widget_destroy(widget);
					return FALSE;
				}else{
					gtk_widget_destroy(fxclose->dialog);
					gtk_widget_hide_on_delete(widget);
					return TRUE;
				}
			}else{
				gtk_widget_destroy(fxclose->dialog);
				return TRUE;
			}
		}
	}else{
		fx_main_destroy(widget , fxmain);
		return FALSE;
	}

	if(fxmain->user != NULL && fxmain->user->loginStatus != -1){
		config = fxmain->user->config;
		if(config->closeMode == CLOSE_ICON_MODE){
			gtk_widget_hide_on_delete(widget);
			return TRUE;
		}else{
			gtk_widget_destroy(widget);
			return FALSE;
		}
	}
	return FALSE;
}
gboolean fx_main_window_state_func(GtkWidget *widget
		, GdkEventWindowState *event , gpointer data)
{
	FxMain *fxmain;
	Config *config;

	fxmain = (FxMain*)data;
	config = fxmain->user == NULL ? NULL: fxmain->user->config;

	if(config){
			if(event->changed_mask == GDK_WINDOW_STATE_ICONIFIED && 
				event->new_window_state == GDK_WINDOW_STATE_ICONIFIED){
			
			if(config->canIconify == ICON_CAN){
				gtk_window_get_position(GTK_WINDOW(widget)
						, &window_pos_x_old , &window_pos_y_old);
				return TRUE;
			}else{
				return FALSE;
			}
		}
	}
	return FALSE;
}
void fx_main_tray_activate_func(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxMain *fxmain;
	Config *config = NULL;

	fxmain = (FxMain*)data;

	if(fxmain->user && fxmain->user->config)
		config = fxmain->user->config;

	if(!config)
		return;

	gtk_window_deiconify(GTK_WINDOW(fxmain->window));
    if(GTK_WIDGET_VISIBLE(fxmain->window)){
		if(config->canIconify == ICON_CAN){
			gtk_window_iconify(GTK_WINDOW(fxmain->window));
			gtk_window_get_position(GTK_WINDOW(fxmain->window)
					, &window_pos_x_old , &window_pos_y_old);
			gtk_widget_hide(fxmain->window);
		}
	}
    else{
        //gtk_window_deiconify(GTK_WINDOW(fxmain->window));
		gtk_widget_show(fxmain->window);
	}

	if(window_pos_x_old == 0 && window_pos_y_old == 0){
		window_pos_x_old = window_pos_x;
		window_pos_y_old = window_pos_y;
	}
	gtk_window_move(GTK_WINDOW(fxmain->window),
			window_pos_x_old , window_pos_y_old);

}

static void fx_main_mute_clicked(
		GtkWidget *UNUSED(widget) , gpointer data)
{
	FxMain *fxmain;
	User   *user;
	Config *config;

	fxmain = (FxMain*)data;
	user = fxmain->user;
	config = user->config;

	if(config->isMute == MUTE_ENABLE)
		config->isMute = MUTE_DISABLE;
	else
		config->isMute = MUTE_ENABLE;

	fetion_config_save(user);

}

static void fx_main_direct_sms_clicked(
		GtkWidget *UNUSED(widget) , gpointer data)
{
	FxMain *fxmain;
	FxDSMS *fxdsms;

	fxmain = (FxMain*)data;
	fxdsms = fx_dsms_new(fxmain);

	if(fxmain->user->carrierStatus == CARRIER_STATUS_DOWN){
		fx_util_popup_warning(fxmain,
				SERVICE_DOWN_MESSAGE);
		return;
	}
	fx_dsms_initialize(fxdsms);
}

void fx_main_tray_popmenu_func(
		GtkWidget* UNUSED(widget),
		guint button , guint activate_time,
		gpointer data)
{
	FxMain    *fxmain;
	User      *user;
	Config    *config;
	GtkWidget *item;
	GtkWidget *menu;
	GtkWidget *statemenu;
	GtkWidget *submenu;
	gchar     stateMenu[48];
	gint      i;
	typedef struct {
		FxMain* fxmain;
		StateType type;
	} Args;
	Args      *args;

	fxmain = (FxMain*)data;
	user = fxmain->user;

	struct {
		const gchar* name;
		const gchar* icon;
		int type;
	} presence[] = {
		{ N_("Online")	 , SKIN_DIR"online.svg" , P_ONLINE } ,
		{ N_("Leave")	 , SKIN_DIR"away.svg" , P_AWAY } ,
		{ N_("Busy")	 , SKIN_DIR"busy.svg" , P_BUSY } ,
		{ N_("Hide")	 , SKIN_DIR"invisible.svg" , P_HIDDEN } ,
		{ N_("Eating out") , SKIN_DIR"away.svg" , P_OUTFORLUNCH } ,
		{ N_("Do Not Disturb") , SKIN_DIR"away.svg" , P_DONOTDISTURB } ,
		{ N_("Back Soon") , SKIN_DIR"away.svg" , P_RIGHTBACK } ,
		{ N_("Meeting")	 , SKIN_DIR"away.svg" , P_MEETING } ,
		{ N_("Calling")	 , SKIN_DIR"away.svg" , P_ONTHEPHONE} ,
		{ NULL		 , NULL 			   , -1}
	};

	menu = gtk_menu_new();
	fx_main_create_menu1(_("About OpenFetion") , GTK_STOCK_ABOUT
					 , menu , fx_main_about_fetion_clicked , NULL);

	if(fxmain->user && fxmain->user->loginStatus != -1)
	{
		config = fxmain->user->config;

		item = gtk_check_menu_item_new_with_label(_("Close sound"));

		if(config->isMute == MUTE_ENABLE)
			gtk_check_menu_item_set_active(
					GTK_CHECK_MENU_ITEM(item) , TRUE);
		else
			gtk_check_menu_item_set_active(
					GTK_CHECK_MENU_ITEM(item) , FALSE);

		g_signal_connect(item, "activate",
				G_CALLBACK(fx_main_mute_clicked) , fxmain);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu) , item);

		item = gtk_check_menu_item_new_with_label(_("Receive SMS"));

		if(strcmp(user->smsOnLineStatus , "0.00:00:00") &&
			strcmp(user->smsOnLineStatus , "0.0:0:0"))
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item) , FALSE);
		else
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item) , TRUE);

		gtk_menu_shell_append(GTK_MENU_SHELL(menu) , item);
		g_signal_connect(item, "activate",
				G_CALLBACK(fx_head_set_sms_clicked) , fxmain);

		fx_main_create_menu1(_("Information query") , GTK_STOCK_FIND
						 , menu , fx_main_info_lookup_clicked , fxmain);


		statemenu = fx_main_create_menu1(_("Edit statement"),
				GTK_STOCK_INFO , menu , NULL , NULL);

		submenu = gtk_menu_new();
		for(i = 0 ; presence[i].type != -1 ; i++){
			args = (Args*)malloc(sizeof(Args));
			args->fxmain = fxmain;
			args->type = presence[i].type;

			sprintf(stateMenu , "%s      " , _(presence[i].name));
			fx_main_create_menu(stateMenu , presence[i].icon
							 , submenu , fx_main_set_state_clicked , args);
		}

		gtk_menu_item_set_submenu(GTK_MENU_ITEM(statemenu) , submenu);

		statemenu = fx_main_create_menu1(_("Message Function"),
				GTK_STOCK_INFO , menu , NULL , NULL);
		submenu = gtk_menu_new();
		fx_main_create_menu(_("SMS to many") , SKIN_DIR"groupsend.png" 
						 , submenu , fx_main_send_to_many_clicked , fxmain);
		fx_main_create_menu(_("SMS directly") , SKIN_DIR"directsms.png"
						 , submenu , fx_main_direct_sms_clicked , fxmain);
		if(fxmain->user->boundToMobile == BOUND_MOBILE_ENABLE)
			fx_main_create_menu(_("SMS myself") , SKIN_DIR"myselfsms.png"
							 , submenu , fx_main_send_to_myself_clicked , fxmain);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(statemenu) , submenu);

		fx_main_create_menu1(_("Add contact") , GTK_STOCK_ADD
						 , menu , fx_main_add_buddy_clicked , fxmain);
		fx_main_create_menu1(_("Personal Settings") ,  GTK_STOCK_EDIT
						 , menu , fx_main_personal_setting_clicked , fxmain);
		fx_main_create_menu1(_("System Settings") , GTK_STOCK_PREFERENCES
						 , menu , fx_main_system_setting_clicked , fxmain);
	}

	fx_main_create_menu1(_("Exit OpenFetion ") , GTK_STOCK_QUIT
					 , menu , fx_main_destroy , fxmain);
	gtk_widget_show_all(menu);
	gtk_menu_popup(GTK_MENU(menu) , NULL , NULL , NULL , NULL
				, button , activate_time);
}
int main(int argc , char* argv[])
{

	FxMain* fxmain = fx_main_new();

	setlocale(LC_ALL, "");
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	bindtextdomain(GETTEXT_PACKAGE , LOCALE_DIR);
	textdomain(GETTEXT_PACKAGE);

	if(!g_thread_supported())
		g_thread_init(NULL);
	gdk_threads_init();

 	struct sigaction sa;
 	sa.sa_handler = SIG_IGN;
 	sigaction(SIGPIPE, &sa, 0 );

#ifdef USE_GSTREAMER
	gst_init(&argc , &argv);
#endif

#ifdef USE_LIBNOTIFY
	notify_init("Openfetion");
#endif
	gtk_init(&argc , &argv);

	fx_conn_init(fxmain);

	fx_main_initialize(fxmain);

	return 0;
}

static void chat_listen_thread_end(FxMain *fxmain, const char *sipuri)
{
	FxList       *clist;
	FxChat       *fxchat;
	Conversation *conv;

	clist = fxmain->clist;

	if(!sipuri || strlen(sipuri) == 0)
		return;

	fx_list_remove_sip_by_sipuri(fxmain->slist , sipuri);

	fxchat = fx_list_find_chat_by_sipuri(clist , sipuri);
	if(!fxchat) {
		debug_info("User %s left conversation" , sipuri);
		debug_info("Thread exit");
		g_thread_exit(0);
	}
	conv = fxchat->conv;
	conv->currentSip = NULL;

	debug_info("User %s left conversation" , sipuri);
	debug_info("Thread exit");
	g_thread_exit(0);
}

void* fx_main_listen_thread_func(void* data)
{
	ThreadArgs *args;
	FxMain     *fxmain;
	FetionSip  *sip;
	SipMsg     *msg;
	SipMsg     *pos;
	User       *user;
	gint        type;
	gint        ret;
	gint        error;

	args = (ThreadArgs*)data;
	fxmain = args->fxmain;
	user = fxmain->user;
	sip = args->sip;

	struct timeval tv;

	debug_info("A new thread entered");

	sip = (sip == NULL ? fxmain->user->sip : sip);

	for(;;){

		if(!fxmain)
			g_thread_exit(0);

		FD_ZERO(&fd_read);

		g_static_mutex_lock(&mutex);
		if(!sip || !sip->tcp){
			debug_info("thread exited");
			g_thread_exit(0);
		}
		FD_SET(sip->tcp->socketfd, &fd_read);
		g_static_mutex_unlock(&mutex);

		tv.tv_sec = 13;
		tv.tv_usec = 0;

		ret = select(sip->tcp->socketfd+1, &fd_read, NULL, NULL, &tv);

		if(ret == 0)
			continue;

		if (ret == -1) {
			debug_info ("Error.. to read socket %d,exit thread",
					sip->tcp->socketfd);
			if(sip != user->sip){
				debug_info("Error.. thread sip freed\n");
				g_free(sip);
			}
			g_thread_exit(0);
		}

		if (!FD_ISSET (sip->tcp->socketfd, &fd_read)) {
			g_usleep (100);
			continue;
		}

		msg = fetion_sip_listen(sip, &error);

		if(!msg && error){
			/* if it is the main listening thread */
			if(sip == user->sip){
				gdk_threads_enter();
				printf("\n\nError ...... break out...\n\n");
				fx_conn_offline(fxmain);
				gdk_threads_leave();
				g_thread_exit(0);
			}else{
				printf("\n\n Error ... user listen thread break out\n\n");
				chat_listen_thread_end(fxmain, sip->sipuri);
				tcp_connection_free(sip->tcp);
				g_thread_exit(0);
			}
		}

		pos = msg;
		while(pos){

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
					break;
				case SIP_SIPC_4_0:
					fx_main_process_sipc(fxmain , pos->message);
					break;
				default:
					//printf("%s\n" , pos->message);
					break;
			}
			pos = pos->next;
		}

		if(msg)
			fetion_sip_message_free(msg);
	}

	return NULL;
}
void fx_main_message_func(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxMain        *fxmain;
	FxList        *mlist;
	FxList        *cur;
	FxList        *tmp;
	FxChat        *fxchat;
	FxPGGroup     *fxpg;
	PGGroupMember *memcur;
	Message       *msg;

	fxmain = (FxMain*)data;
	mlist = fxmain->mlist;
	cur = mlist->pre;

#ifdef USE_LIBNOTIFY
	notify_notification_close(fxmain->notify , NULL);
#endif

	while(cur != fxmain->mlist){
		msg = (Message*)(cur->data);

		/* ordinary message */
		if(!msg->pguri){
		    fxchat = fx_main_create_chat_window(fxmain , msg->sipuri);

		    if(!fxchat){
			    g_print("Unknow Message\n");
			    g_print("%s:%s\n" , msg->sipuri ,  msg->message);
			    continue;
		    }
		    fx_chat_add_message(fxchat , msg->message,
					&(msg->sendtime) , 0 , msg->sysback);
		}else{
			/*group message*/
		    fxpg = pg_create_window(fxmain , msg->pguri);

		    foreach_pg_member(fxpg->pggroup->member , memcur){
				if(strcmp(memcur->sipuri , msg->sipuri) == 0){
			    	pg_add_message(fxpg, msg->message,
							&(msg->sendtime) , memcur );
					break;
				}
		    }
		}
		fetion_message_free(msg);
		tmp = cur;
		cur = cur->pre;
		fx_list_remove(tmp);
		free(tmp);
	}
	fx_head_set_state_image(fxmain , fxmain->user->state);
	gtk_status_icon_set_blinking(
			GTK_STATUS_ICON(fxmain->trayIcon) , FALSE);
	g_signal_handler_disconnect(fxmain->trayIcon,
			fxmain->iconConnectId);
	fxmain->iconConnectId = g_signal_connect(
						G_OBJECT(fxmain->trayIcon)
						 , "activate"
						 , GTK_SIGNAL_FUNC(fx_main_tray_activate_func)
						 , fxmain);

	foreach_list(fxmain->clist, cur){
		fxchat = (FxChat*)cur->data;
		if(fxchat->unreadMsgCount != 0)
			gtk_window_present(GTK_WINDOW(fxchat->dialog));

	}
}
gboolean fx_main_register_func(User* user)
{
	if(fetion_user_keep_alive(user) < 0){
		debug_info("keep alive terminated");
		return FALSE;
	}
	return TRUE;
}

gboolean fx_main_check_func(FxMain* fxmain)
{
	struct unacked_list *list;
	struct tm           *now;
	char                *msg;
	time_t               now_t;
	time_t               msg_time_t;
	long                 seconds;

	start_popup_presence = 1;

	if(fxmain->user->state == P_OFFLINE){
		debug_info("Error.. check function exited");
		return FALSE;
	}

	now = get_currenttime();
	foreach_unacked_list(unackedlist , list){
		now_t = mktime(now);
		msg_time_t = mktime(&(list->message->sendtime));
		seconds = (long)now_t - (long)msg_time_t;
		if(seconds > 20) {
			unacked_list_remove(unackedlist , list);
			msg = contruct_message_sip(fxmain->user->sId , list->message);
			fx_main_process_message(fxmain , fxmain->user->sip , msg);
			free(msg);
			fetion_message_free(list->message);
			free(list);
		}
	}

	idle_autoaway(fxmain);

	return TRUE;
}

void fx_main_about_fetion_clicked(GtkWidget *UNUSED(widget) , gpointer UNUSED(data))
{
	show_about();
}

void fx_main_send_to_myself_clicked(GtkWidget *widget , gpointer data)
{
	fx_bottom_on_sendtome_clicked(widget , data);
}

void fx_main_send_to_many_clicked(GtkWidget *widget , gpointer data)
{
	fx_bottom_on_sendtomany_clicked(widget , data);
}

void fx_main_personal_setting_clicked(GtkWidget *widget , gpointer data)
{
	fx_bottom_on_setting_clicked(widget , data);
}

void fx_main_system_setting_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	FxSet *fxset = fx_set_new(fxmain);
	fx_set_initialize(fxset);
	g_object_set(fxset->notebook , "page" , 1 , NULL);
	gtk_window_set_position(GTK_WINDOW(fxset->dialog) , GTK_WIN_POS_CENTER);
	gtk_dialog_run(GTK_DIALOG(fxset->dialog));
	gtk_widget_destroy(fxset->dialog);
}

void fx_main_set_state_clicked(GtkWidget *widget , gpointer data)
{
	fx_head_change_state_func(widget , data);
}

void fx_main_add_buddy_clicked(GtkWidget *widget , gpointer data)
{
	fx_bottom_on_addfriend_clicked(widget , data);
}

void fx_main_info_lookup_clicked(GtkWidget *widget , gpointer data)
{
	fx_bottom_on_lookup_clicked(widget , data);
}

FetionSip* fx_list_find_sip_by_sipuri(FxList *fxlist , const char *sipuri)
{
	FxList    *cur;
	FetionSip *sip;
	gchar     *sid;
	gchar     *sid1;

	foreach_list(fxlist , cur){
		sip = (FetionSip*)(cur->data);
		sid = fetion_sip_get_sid_by_sipuri(sip->sipuri);
		sid1 = fetion_sip_get_sid_by_sipuri(sipuri);
		if(strcmp(sid , sid1) == 0){
			g_free(sid); sid = NULL;
			g_free(sid1); sid1 = NULL;
			return sip;
		}
		g_free(sid); sid = NULL;
		g_free(sid1); sid1 = NULL;
	}
	return NULL;
}
void fx_list_remove_sip_by_sipuri(FxList *fxlist , const char *sipuri)
{
	FxList    *cur;
	FetionSip *sip;
	gchar     *sid;
	gchar     *sid1;

	foreach_list(fxlist , cur){
		sip = (FetionSip*)(cur->data);
		sid = fetion_sip_get_sid_by_sipuri(sip->sipuri);
		sid1 = fetion_sip_get_sid_by_sipuri(sipuri);
		if(strcmp(sid , sid1) == 0)	{
			debug_info("Removing sip from sip list");
			cur->next->pre = cur->pre;
			cur->pre->next = cur->next;
			g_free(sid);sid = NULL;
			g_free(sid1);sid1 = NULL;
			g_free(cur);
			break;
		}
		g_free(sid);sid = NULL;
		g_free(sid1);sid1 = NULL;
	}
}

FxChat* fx_list_find_chat_by_sipuri(FxList *fxlist , const char *sipuri)
{
	FxChat  *fxchat;
	FxList  *cur;
	Contact *contact;
	gchar   *sid;
	gchar   *sid1;

	foreach_list(fxlist , cur){
		fxchat = (FxChat*)(cur->data);
		contact = fxchat->conv->currentContact;
		sid = fetion_sip_get_sid_by_sipuri(contact->sipuri);
		sid1 = fetion_sip_get_sid_by_sipuri(sipuri);
		if(strcmp(sid , sid1) == 0){
			g_free(sid);
			g_free(sid1);
			return fxchat;
		}
		g_free(sid);
		g_free(sid1);
	}
	return NULL;
}
void fx_list_remove_chat_by_sipuri(FxList *fxlist , const char *sipuri)
{
	FxList  *cur;
	Contact *contact;
	FxChat  *fxchat;
	gchar   *sid;
	gchar   *sid1;

	foreach_list(fxlist , cur){
		fxchat = (FxChat*)(cur->data);
		contact = fxchat->conv->currentContact;
		if(!contact){
			printf("Unknown FxChat\n");
			continue;
		}
		sid = fetion_sip_get_sid_by_sipuri(contact->sipuri);
		sid1 = fetion_sip_get_sid_by_sipuri(sipuri);
		if(strcmp(sid , sid1) == 0){
			debug_info("Removing chat struct from chat list");
			g_free(sid);sid = NULL;
			g_free(sid1);sid1 = NULL;
			cur->next->pre = cur->pre;
			cur->pre->next = cur->next;
			break;
		}
		g_free(sid);sid = NULL;
		g_free(sid1);sid1 = NULL;
	}
}

TimeOutArgs* fx_list_find_timeout_by_sipuri(FxList* fxlist , const char* sipuri)
{
	TimeOutArgs *args;
	FxList      *cur;
	gchar       *sid;
	gchar       *sid1;

	foreach_list(fxlist , cur){
		args = (TimeOutArgs*)(cur->data);
		sid = fetion_sip_get_sid_by_sipuri(args->sipuri);
		sid1 = fetion_sip_get_sid_by_sipuri(sipuri);
		if(strcmp(sid , sid1) == 0){
			g_free(sid);sid = NULL;
			g_free(sid1);sid1 = NULL;
			return args;
		}
		g_free(sid);sid = NULL;
		g_free(sid1);sid1 = NULL;
	}
	return NULL;
}

void fx_list_remove_timeout_by_sipuri(FxList* fxlist , const char* sipuri)
{
	FxList      *cur;
	TimeOutArgs *args;
	gchar       *sid;
	gchar       *sid1;

	foreach_list(fxlist , cur){
		args = (TimeOutArgs*)(cur->data);
		sid = fetion_sip_get_sid_by_sipuri(args->sipuri);
		sid1 = fetion_sip_get_sid_by_sipuri(sipuri);
		if(strcmp(sid , sid1) == 0){
			cur->next->pre = cur->pre;
			cur->pre->next = cur->next;
			g_free(sid);sid = NULL;
			g_free(sid1);sid1 = NULL;
			g_free(cur);
			break;
		}
		g_free(sid);sid = NULL;
		g_free(sid1);sid1 = NULL;
	}

}

void fx_list_remove_pg_by_sipuri(FxList* fxlist , const char* sipuri)
{
	FxList    *cur;
	FxPGGroup *fxpg;

	foreach_list(fxlist , cur){
		fxpg = (FxPGGroup*)(cur->data);
		if(strcmp(fxpg->pggroup->pguri , sipuri) == 0){
			cur->next->pre = cur->pre;
			cur->pre->next = cur->next;
			g_free(cur);
			break;
		}
	}

}

void fx_main_add_history(FxMain *fxmain, const char *name,
		const char *sid, const char *msg, int issend)
{
	History   *history;
	struct tm *now;
	User      *user;

	user = fxmain->user;
	now = get_currenttime();

	history = fetion_history_message_new(name
				, sid , *now , msg , issend);

	g_static_mutex_lock(&mutex);
	fetion_history_add(fxmain->history , history);
	fetion_history_message_free(history);
	g_static_mutex_unlock(&mutex);
}

static void fx_main_process_pggetgroupinfo(FxMain *fxmain , const char *sipmsg)
{
	PGGroup *pggroup = fxmain->user->pggroup;
	pg_group_parse_info(pggroup , sipmsg);
	gdk_threads_enter();
	fx_tree_bind_pg_data(fxmain);
	gdk_threads_leave();

}
static void fx_main_process_pgpresencechanged(FxMain *fxmain , const char *sipmsg)
{
	PGGroup *pggroup = fxmain->user->pggroup;

	pg_group_parse_member(pggroup , sipmsg);

#if 0
	PGGroup *pgcur;
	/* get member contact info of current group */
	foreach_pg_group(pggroup , pgcur){
		if(pgcur->hasDetails == 0)
			pg_group_update_group_info(fxmain->user , pgcur);
	}
#endif
}

static gboolean key_press_func(GtkWidget *widget , GdkEventKey *event
		, gpointer data)
{
	if(event->keyval == GDK_w){
		if(event->state & GDK_CONTROL_MASK){
			gtk_window_iconify(GTK_WINDOW(widget));
			return TRUE;
		}else{
			return FALSE;
		}
	}
	if(event->keyval == GDK_q){
		if(event->state & GDK_CONTROL_MASK){
			fx_main_delete(widget , NULL , data);
			return TRUE;
		}else{
			return FALSE;
		}
	}

	return FALSE;
}
