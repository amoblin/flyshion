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

extern int old_state;

FxHead* fx_head_new()
{
	FxHead* fxhead = (FxHead*)malloc(sizeof(FxHead));

	memset(fxhead , 0 , sizeof(FxHead));
	return fxhead;
}

void fx_head_free(FxHead* fxhead)
{
	if(fxhead != NULL)
		free(fxhead);
}

void fx_head_initialize(FxMain* fxmain)
{
	
	GdkPixbuf* pb;
	GtkWidget *vbox , *headbox , *alignment , *halign;
	GtkWidget* hbox = gtk_hbox_new(FALSE , 0);
	FxHead* fxhead = fx_head_new();
	fxmain->headPanel = fxhead;
	halign = gtk_alignment_new(0, 0 , 0 , 0);
	gtk_container_add(GTK_CONTAINER(halign) , hbox);
	
	pb = gdk_pixbuf_new_from_file_at_scale(SKIN_DIR"fetion.svg",
			USER_PORTRAIT_SIZE, USER_PORTRAIT_SIZE, TRUE, NULL);
	fxhead->portrait = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);

	GtkWidget *frame;
	frame = gtk_frame_new(NULL);
	gtk_widget_set_usize(frame , 55 , 55);
	fxhead->portraitbox = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(fxhead->portraitbox) , fxhead->portrait);
	gtk_container_add(GTK_CONTAINER(frame) , fxhead->portraitbox);
	gtk_box_pack_start(GTK_BOX(hbox) , frame , FALSE , FALSE , 10 );

	g_signal_connect(G_OBJECT(fxhead->portraitbox)
				   , "button_press_event"
				   , GTK_SIGNAL_FUNC(fx_head_change_portrait_func)
				   , fxmain);
				 
	g_signal_connect(G_OBJECT(fxhead->portraitbox)
				   , "enter_notify_event"
				   , GTK_SIGNAL_FUNC(fx_head_change_portrait_func)
				   , fxmain);

	g_signal_connect(G_OBJECT(fxhead->portraitbox)
				   , "leave_notify_event"
				   , GTK_SIGNAL_FUNC(fx_head_change_portrait_func)
				   , fxmain);

	vbox = gtk_vbox_new(TRUE , 0);

	headbox = gtk_hbox_new(FALSE , 0);

	fxhead->state_button = gtk_event_box_new();
	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"online.svg" , 20 , 20 , FALSE);
	fxhead->state_img = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_container_add(GTK_CONTAINER(fxhead->state_button) , fxhead->state_img);
	gtk_widget_set_events(fxhead->state_button
						, GDK_MOTION_NOTIFY
						| GDK_BUTTON_PRESS
						| GDK_BUTTON_RELEASE
						| GDK_ENTER_NOTIFY
						| GDK_LEAVE_NOTIFY);
	g_signal_connect(G_OBJECT(fxhead->state_button)
					 , "button_press_event"
					 , GTK_SIGNAL_FUNC(fx_head_popup_statemenu_func)
					 , fxmain);
	g_signal_connect(G_OBJECT(fxhead->state_button)
					 , "enter_notify_event"
					 , GTK_SIGNAL_FUNC(fx_head_popup_statemenu_func)
					 , fxmain);
	g_signal_connect(G_OBJECT(fxhead->state_button)
					 , "leave_notify_event"
					 , GTK_SIGNAL_FUNC(fx_head_popup_statemenu_func)
					 , fxmain);

	g_signal_connect(G_OBJECT(fxhead->portraitbox)
				   , "leave_notify_event"
				   , GTK_SIGNAL_FUNC(fx_head_change_portrait_func)
				   , fxmain);

	gtk_box_pack_start(GTK_BOX(headbox) , fxhead->state_button , TRUE , TRUE , 0);

	fxhead->name_label = gtk_label_new(NULL);
	fxhead->state_label = gtk_label_new(NULL);
	gtk_box_pack_start(GTK_BOX(headbox) , fxhead->name_label , TRUE , TRUE , 0);
	gtk_box_pack_start(GTK_BOX(headbox) , fxhead->state_label , TRUE , TRUE , 0);

	fxhead->impre_box = gtk_event_box_new();
	g_signal_connect(G_OBJECT(fxhead->impre_box)
				   , "button_press_event"
				   , GTK_SIGNAL_FUNC(fx_head_impre_event_func)
				   , fxmain);
	g_signal_connect(G_OBJECT(fxhead->impre_box)
				   , "button_release_event"
				   , GTK_SIGNAL_FUNC(fx_head_impre_event_func)
				   , fxmain);
	g_signal_connect(G_OBJECT(fxhead->impre_box)
				   , "enter_notify_event"
				   , GTK_SIGNAL_FUNC(fx_head_impre_event_func)
				   , fxmain);
	g_signal_connect(G_OBJECT(fxhead->impre_box)
				   , "leave_notify_event"
				   , GTK_SIGNAL_FUNC(fx_head_impre_event_func)
				   , fxmain);
	fxhead->impre_entry = gtk_entry_new();
	g_signal_connect(G_OBJECT(fxhead->impre_entry)
				   , "focus-out-event"
				   , GTK_SIGNAL_FUNC(fx_head_impre_focus_out_func)
				   , fxmain);
	g_signal_connect(G_OBJECT(fxhead->impre_entry)
				   , "activate"
				   , G_CALLBACK(fx_head_impre_activate_func)
				   , fxmain);
	fxhead->impre_label = gtk_label_new(NULL);
	gtk_label_set_width_chars(GTK_LABEL(fxhead->impre_label) , 15);
	gtk_misc_set_alignment(GTK_MISC(fxhead->impre_label) , 0 , 0);

	alignment = gtk_alignment_new(0 , 0 , 0 , 0);
	gtk_container_add(GTK_CONTAINER(alignment) , headbox);
	gtk_box_pack_start(GTK_BOX(vbox) , alignment , FALSE , TRUE , 0);

	gtk_container_add(GTK_CONTAINER(fxhead->impre_box) , fxhead->impre_label);
	gtk_box_pack_start(GTK_BOX(vbox) , fxhead->impre_box , FALSE , FALSE , 0);
	gtk_box_pack_start(GTK_BOX(vbox) , fxhead->impre_entry , FALSE , FALSE , 0);
	gtk_box_pack_start(GTK_BOX(hbox) , vbox , FALSE , TRUE , 10);

	fxhead->topbox = gtk_hbox_new(FALSE , 0);
	gtk_box_pack_start(GTK_BOX(fxhead->topbox) , halign , FALSE , FALSE , 0);
	gtk_box_pack_start(GTK_BOX(fxmain->mainbox) , fxhead->topbox , FALSE , FALSE , 10);
	fx_head_bind(fxmain);	
}

void fx_head_show(FxMain *fxmain)
{
	FxHead *fxhead = fxmain->headPanel;
	gtk_widget_show_all(fxhead->topbox);
	gtk_widget_hide(fxhead->impre_entry);
}

void fx_head_bind(FxMain* fxmain)
{
	FxHead* fxhead = fxmain->headPanel;
	User* user = fxmain->user;
	Config* config = user->config;
	char name[256];
	char tooltip[1024];
	char* statename = NULL;
	GdkPixbuf* portrait_pix = NULL;

	memset(fxhead->oldimpression, 0, sizeof(fxhead->oldimpression));

	snprintf(name, sizeof(name) - 1 , "<b>%s</b>"
			, user->nickname == NULL ? user->sId : g_markup_escape_text(user->nickname, -1) );

	gtk_label_set_markup(GTK_LABEL(fxhead->name_label), name );
	

	strcpy(fxhead->oldimpression
		, (strlen(user->impression) == 0 || user->impression == NULL)
		? _("Click here to input signature") : user->impression);

	sprintf(tooltip, "<b>%s</b>", g_markup_escape_text(user->impression, strlen(user->impression)));
	gtk_widget_set_tooltip_markup(fxhead->impre_label, tooltip);
	escape_impression(fxhead->oldimpression);
	gtk_label_set_text(GTK_LABEL(fxhead->impre_label), fxhead->oldimpression);

	sprintf(name , "%s/%s.jpg" , config->iconPath , user->sId);

	portrait_pix = gdk_pixbuf_new_from_file_at_size(name,
			USER_PORTRAIT_SIZE, USER_PORTRAIT_SIZE, NULL);
	if(! portrait_pix)
		portrait_pix = gdk_pixbuf_new_from_file_at_size(name,
				USER_PORTRAIT_SIZE, USER_PORTRAIT_SIZE, NULL);
	if(portrait_pix)
		gtk_image_set_from_pixbuf(GTK_IMAGE(fxhead->portrait) , portrait_pix);

	statename = fx_util_get_state_name(user->state);
	gtk_label_set_markup(GTK_LABEL(fxhead->state_label) , statename);
	fx_head_set_state_image(fxmain , user->state);

	free(statename);
}
void fx_head_set_state_image(FxMain* fxmain , StateType type)
{
	FxHead* fxhead = fxmain->headPanel;
	GdkPixbuf *pixbuf;
	char* statename = fx_util_get_state_name(type);

	gtk_label_set_markup(GTK_LABEL(fxhead->state_label) , statename);
	switch(type)
	{
		case P_ONLINE :
			pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"online.svg" , 20 , 20 , NULL);
			gtk_status_icon_set_from_file(fxmain->trayIcon
										, SKIN_DIR"online.svg");
			break;
		case P_BUSY :
			pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"busy.svg" , 20 , 20 , NULL);
			gtk_status_icon_set_from_file(fxmain->trayIcon
										, SKIN_DIR"busy.svg");
			break;
		case P_HIDDEN :
			pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"invisible.svg" , 20 , 20 , NULL);
			gtk_status_icon_set_from_file(fxmain->trayIcon
										, SKIN_DIR"invisible.svg");
			break;
		case P_OFFLINE :
			pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"offline.svg" , 20 , 20 , NULL);
			gtk_status_icon_set_from_file(fxmain->trayIcon
										, SKIN_DIR"offline.svg");
			gtk_widget_set_sensitive(fxhead->portrait, FALSE);
			break;
		default :
			pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"away.svg" , 20 , 20 , NULL);
			gtk_status_icon_set_from_file(fxmain->trayIcon
										, SKIN_DIR"away.svg");
			break;
	}
	gtk_image_set_from_pixbuf(GTK_IMAGE(fxhead->state_img) , pixbuf);
	g_object_unref(pixbuf);
	free(statename);
}
void fx_head_popup_statemenu_func(GtkWidget* widget
		, GdkEventButton* event , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	User *user = fxmain->user;
	GtkWidget *item;
	GtkWidget *img;
	GtkWidget *separator;
	GdkCursor *cursor;
	GtkWidget *presence_menu = gtk_menu_new();

	if(event->type == GDK_ENTER_NOTIFY) {
		cursor = gdk_cursor_new (GDK_HAND2);
		gdk_window_set_cursor(widget->window , cursor);
		return;
	}

	if(event->type == GDK_LEAVE_NOTIFY)	{
		cursor = gdk_cursor_new (GDK_LEFT_PTR);
		gdk_window_set_cursor(widget->window , cursor);
		return;
	}

	fx_head_create_presence_item(P_ONLINE , _("Online") , presence_menu , fxmain);
	fx_head_create_presence_item(P_AWAY   , _("Leave") , presence_menu , fxmain);
	fx_head_create_presence_item(P_BUSY   , _("Busy") , presence_menu , fxmain);
	fx_head_create_presence_item(P_HIDDEN , _("Hide") , presence_menu , fxmain);
	fx_head_create_presence_item(P_OFFLINE , _("Offline") , presence_menu , fxmain);

	separator = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(presence_menu) , separator);

	fx_head_create_presence_item(P_OUTFORLUNCH  , _("Eating out") , presence_menu , fxmain);
	fx_head_create_presence_item(P_DONOTDISTURB , _("Do Not Disturb") , presence_menu , fxmain);
	fx_head_create_presence_item(P_MEETING      , _("Meeting") , presence_menu , fxmain);
	fx_head_create_presence_item(P_ONTHEPHONE   , _("Calling") , presence_menu , fxmain);

	separator = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(presence_menu) , separator);

	item = gtk_check_menu_item_new_with_label(_("Receive SMS"));
	if(strcmp(user->smsOnLineStatus , "0.00:00:00") &&
		strcmp(user->smsOnLineStatus , "0.0:0:0"))
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item) , FALSE);
	else
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item) , TRUE);
	gtk_menu_shell_append(GTK_MENU_SHELL(presence_menu) , item);
	g_signal_connect(item , "activate" , G_CALLBACK(fx_head_set_sms_clicked) , fxmain);

	item = gtk_image_menu_item_new_with_label(_("Modify Profile"));
	img = gtk_image_new_from_file(SKIN_DIR"edit.png");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item) , img);
	gtk_menu_shell_append(GTK_MENU_SHELL(presence_menu) , item);
	g_signal_connect(item , "activate" , G_CALLBACK(fx_bottom_on_setting_clicked) , fxmain);

	item = gtk_image_menu_item_new_with_label(_("Exit OpenFetion "));
	img = gtk_image_new_from_file(SKIN_DIR"exit.png");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item) , img);
	gtk_menu_shell_append(GTK_MENU_SHELL(presence_menu) , item);
	g_signal_connect(item , "activate" , G_CALLBACK(fx_main_destroy) , fxmain);


	gtk_widget_show_all(presence_menu);
	
	gtk_menu_popup(GTK_MENU(presence_menu) , NULL , NULL , NULL , NULL 
				, (event != NULL) ? event->button : 0 , gdk_event_get_time((GdkEvent*)event));
}
void fx_head_create_presence_item(int type , const char* message , GtkWidget* menu , FxMain* fxmain)
{
	typedef struct {
		FxMain    *fxmain;
		StateType  type;
	} Args1;
	GtkWidget* item = gtk_image_menu_item_new_with_label(message);
	GtkWidget* item_img = NULL;
	GdkPixbuf *pixbuf;
	Args1 *args = (Args1*)malloc(sizeof(Args1));
	args->fxmain = fxmain;
	args->type = type;
	g_signal_connect(item , "activate" , G_CALLBACK(fx_head_change_state_func) , args);
	switch(type)
	{
		case P_ONLINE :
			pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"online.svg" , 20 , 20 , NULL);
			break;
		case P_BUSY :
			pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"busy.svg" , 20 , 20 , NULL);
			break;
		case P_HIDDEN :
			pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"invisible.svg" , 20 , 20 , NULL);
			break;
		case P_OFFLINE :
			pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"offline.svg", 20, 20, NULL);
			break;
		default :
			pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"away.svg" , 20 , 20 , NULL);
			break;
	}
	item_img = gtk_image_new_from_pixbuf(pixbuf);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item) , item_img);
	g_object_unref(pixbuf);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu) , item);
}
void fx_head_impre_event_func(GtkWidget* widget , GdkEventButton* event , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxHead* fxhead = fxmain->headPanel;
	GdkColor color;
	gdk_color_parse ("white", &color);
	GdkCursor* cursor = NULL;
	const char* text = NULL;

	switch(event->type)
	{
		case GDK_BUTTON_PRESS :
			if(fx_conn_check_action(fxmain)){
				gtk_widget_show(fxhead->impre_entry);
				text = gtk_label_get_text(GTK_LABEL(fxhead->impre_label));
				gtk_entry_set_text(GTK_ENTRY(fxhead->impre_entry) , text);
				gtk_entry_select_region(GTK_ENTRY(fxhead->impre_entry) , 0 , strlen(text));
				gtk_widget_grab_focus(fxhead->impre_entry);
				gtk_widget_hide(widget);
			}
			break;
		case GDK_ENTER_NOTIFY :
			gtk_widget_modify_bg(fxhead->impre_box, GTK_STATE_NORMAL, &color);
			cursor = gdk_cursor_new (GDK_HAND2);
			gdk_window_set_cursor(widget->window , cursor);
			break;
		case GDK_LEAVE_NOTIFY :
			gtk_widget_modify_bg(fxhead->impre_box, GTK_STATE_NORMAL, NULL);
			cursor = gdk_cursor_new (GDK_LEFT_PTR);
			gdk_window_set_cursor(widget->window , cursor);
			break;
		default:
			break;
	};
}
gboolean fx_head_impre_focus_out_func(GtkWidget* UNUSED(widget)
		, GdkEventFocus* UNUSED(event) , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxHead* fxhead = fxmain->headPanel;

	gtk_widget_show(fxhead->impre_box);
	gtk_widget_hide(fxhead->impre_entry);
	return FALSE;
}

gboolean fx_head_impre_activate_func(GtkWidget* widget , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxHead* fxhead = fxmain->headPanel;

	gchar tooltip[1024];
	const gchar* impression = gtk_entry_get_text(GTK_ENTRY(widget));
	
	gtk_widget_show(fxhead->impre_box);
	gtk_widget_hide(widget);
	if(strcmp(impression , fxhead->oldimpression) == 0)
		return FALSE;
	if(fetion_user_set_moodphrase(fxmain->user , impression) > 0)
	{
		gtk_label_set_text(GTK_LABEL(fxhead->impre_label) , impression);
		memset(fxhead->oldimpression, 0, sizeof(fxhead->oldimpression));
		strcpy(fxhead->oldimpression , impression);
		memset(tooltip, 0, sizeof(tooltip));
		sprintf(tooltip , "<b>%s</b>" , g_markup_escape_text(impression, -1));
		gtk_widget_set_tooltip_markup(fxhead->impre_label , tooltip);
	}
	return TRUE;
}

static void* reconnection_func(void *data)
{
	typedef struct {
		FxMain    *fxmain;
		StateType  type;
	} Args1;
	Args1    *args = (Args1*)data;
	FxMain   *fxmain = args->fxmain;
	FxHead   *fxhead = fxmain->headPanel;

	gdk_threads_enter();
	gtk_image_set_from_file(GTK_IMAGE(fxhead->portrait),
			SKIN_DIR"reconnecting.gif");
	gdk_threads_leave();

	fx_conn_reconnect(fxmain, args->type);

	return NULL;
}

void fx_head_change_state_func(GtkWidget* UNUSED(widget) , gpointer data)
{
	typedef struct {
		FxMain    *fxmain;
		StateType  type;
	} Args1;
	Args1  *args = (Args1*)data;
	FxMain *fxmain = args->fxmain;
	User   *user = fxmain->user;

	if(args->type == P_OFFLINE){
		fx_conn_offline(fxmain);
		g_free(args);
		return;
	}

	if(user->state == P_OFFLINE){
		g_thread_create(reconnection_func,
				args, FALSE, NULL);
		return;
	}

	if(fetion_user_set_state(user , args->type) > 0){
		fx_head_set_state_image(fxmain , args->type);
		old_state = args->type;
	}

	g_free(args);

}
void fx_head_change_portrait_func(GtkWidget* widget , GdkEventButton* event , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	char *filename = NULL;
	GdkPixbufAnimation *anim_pixbuf = NULL;
	GtkWidget* filechooser = NULL;
	GdkCursor* cursor = NULL;
	struct Args1{
		FxMain* fxmain;
		char    filename[1024];
	} *args = (struct Args1*)malloc(sizeof(struct Args1));
	int response;

	if(event->type == GDK_ENTER_NOTIFY)
	{
		cursor = gdk_cursor_new (GDK_HAND2);
		gdk_window_set_cursor(widget->window , cursor);
		return;
	}

	if(event->type == GDK_LEAVE_NOTIFY)
	{
		cursor = gdk_cursor_new (GDK_LEFT_PTR);
		gdk_window_set_cursor(widget->window , cursor);
		return;
	}

	if(!fx_conn_check_action(fxmain))
		return;

	filechooser = gtk_file_chooser_dialog_new(_("Choose the avatar file to upload")
							   , NULL , GTK_FILE_CHOOSER_ACTION_OPEN
							   , _("Upload") , 1 , _("Cancel") , 2 , NULL);
	response = gtk_dialog_run(GTK_DIALOG(filechooser));

	if(response == 1)
	{
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser));
		if (filename)
		{
			anim_pixbuf = gdk_pixbuf_animation_new_from_file(SKIN_DIR"LoadingImage.gif", NULL); 
			gtk_image_set_from_animation(GTK_IMAGE(fxmain->headPanel->portrait) , anim_pixbuf);
			args->fxmain = fxmain;
			memset(args->filename, 0, sizeof(args->filename));
			strcpy(args->filename , filename);
			g_thread_create(fx_head_change_portrait_thread , args , FALSE , NULL);
		}
	}
	
	gtk_widget_destroy(filechooser);
}
void* fx_head_change_portrait_thread(void* data)
{
	struct Args1 {
		FxMain* fxmain;
		char filename[1024];
	} *args = (struct Args1*)data;
	FxMain     *fxmain;
	FxHead     *fxhead;
	Config     *config;
	gchar       filepath[128];
	GtkWidget  *dialog;
	GdkPixbuf  *pb;

	fxmain = args->fxmain;
	fxhead = fxmain->headPanel;
	config = fxmain->user->config;
	
	if(fetion_user_upload_portrait(fxmain->user , args->filename) > 0){
		fetion_user_download_portrait(fxmain->user , fxmain->user->sipuri);
		sprintf(filepath , "%s/%s.jpg" , config->iconPath , fxmain->user->sId);
		pb = gdk_pixbuf_new_from_file_at_size(filepath,
				USER_PORTRAIT_SIZE , USER_PORTRAIT_SIZE , NULL);

		if(pb == NULL)
			pb = gdk_pixbuf_new_from_file_at_size(filepath,
					USER_PORTRAIT_SIZE, USER_PORTRAIT_SIZE, NULL);

		gdk_threads_enter();
		gtk_image_set_from_pixbuf(GTK_IMAGE(fxhead->portrait) , pb);
		g_object_unref(pb);
		gdk_threads_leave();
	}else{
		gdk_threads_enter();
		dialog = gtk_message_dialog_new(GTK_WINDOW(fxmain->window),
										GTK_DIALOG_DESTROY_WITH_PARENT,
										GTK_MESSAGE_WARNING,
										GTK_BUTTONS_OK,
										_("Upload avatar failed. Note: This appliaction doesn't support auto-compression "
										"of image. Please be sure that the avater is in 150px × 150px."));
		gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		gdk_threads_leave();

		gdk_threads_enter();
		sprintf(filepath , "%s/%s.jpg" , config->iconPath , fxmain->user->sId);
		pb = gdk_pixbuf_new_from_file_at_size(filepath ,
				USER_PORTRAIT_SIZE , USER_PORTRAIT_SIZE , NULL);

		if(pb == NULL)
			pb = gdk_pixbuf_new_from_file_at_size(filepath,
					USER_PORTRAIT_SIZE , USER_PORTRAIT_SIZE , NULL);

		gtk_image_set_from_pixbuf(GTK_IMAGE(fxhead->portrait) , pb);
		g_object_unref(pb);
		gdk_threads_leave();
	}
	free(args);
	return NULL;
}

gboolean fx_head_set_sms_clicked(GtkWidget *widget , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	User *user = fxmain->user;
	FxSmsstat *fxst;

	if(strcmp(user->smsOnLineStatus , "0.00:00:00") == 0 || 
		strcmp(user->smsOnLineStatus , "0.0:0:0") == 0){

		fxst = fx_smsstat_new(fxmain);
		fx_smsstat_initialize(fxst);
		gtk_dialog_run(GTK_DIALOG(fxst->dialog));	

		gtk_widget_destroy(fxst->dialog);
		free(fxst);
	}else{
		fetion_user_set_sms_status(user , 0);
	}
	return FALSE;
}
