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

FxHead* fx_head_new()
{
	FxHead* fxhead = (FxHead*)malloc(sizeof(FxHead));

	DEBUG_FOOTPRINT();

	memset(fxhead , 0 , sizeof(FxHead));
	return fxhead;
}

void fx_head_free(FxHead* fxhead)
{
	DEBUG_FOOTPRINT();

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
	
	DEBUG_FOOTPRINT();

	pb = gdk_pixbuf_new_from_file_at_scale(SKIN_DIR"fetion.svg" , 50 , 50 , TRUE , NULL);
	fxhead->portrait = gtk_image_new_from_pixbuf(pb);
	fxhead->portraitbox = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(fxhead->portraitbox) , fxhead->portrait);
	gtk_box_pack_start(GTK_BOX(hbox) , fxhead->portraitbox , FALSE , FALSE , 10 );

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
	pb = gdk_pixbuf_new_from_file(SKIN_DIR"user_online.png" , FALSE);
	fxhead->state_img = gtk_image_new_from_pixbuf(pb);
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

	DEBUG_FOOTPRINT();

	bzero(name , sizeof(name));
	bzero(fxhead->oldimpression , sizeof(fxhead->oldimpression));

	sprintf(name , "<b>%s</b>"
			, user->nickname == NULL ? user->sId : user->nickname );

	gtk_label_set_markup(GTK_LABEL(fxhead->name_label) , name );

	strcpy(fxhead->oldimpression
		, (strlen(user->impression) == 0 || user->impression == NULL)
		? "Click here to input signature" : user->impression);
	bzero(tooltip , sizeof(tooltip));
	sprintf(tooltip , "<b>%s</b>" , user->impression);
	gtk_widget_set_tooltip_markup(fxhead->impre_label
			, tooltip);
	gtk_label_set_text(GTK_LABEL(fxhead->impre_label) , fxhead->oldimpression);

	bzero(name , sizeof(name));

	sprintf(name , "%s/%s.jpg" , config->iconPath , user->sId);

	fetion_user_download_portrait(user , user->sipuri);
	portrait_pix = gdk_pixbuf_new_from_file_at_size(name , 50 , 50 , NULL);
	if(! portrait_pix)
		portrait_pix = gdk_pixbuf_new_from_file_at_size(name , 50 , 50 , NULL);
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
	char* statename = fx_util_get_state_name(type);

	DEBUG_FOOTPRINT();

	gtk_label_set_markup(GTK_LABEL(fxhead->state_label) , statename);
	switch(type)
	{
		case P_ONLINE :
			gtk_image_set_from_file(GTK_IMAGE(fxhead->state_img)
								  , SKIN_DIR"user_online.png");
			gtk_status_icon_set_from_file(fxmain->trayIcon
										, SKIN_DIR"online.svg");
			break;
		case P_BUSY :
			gtk_image_set_from_file(GTK_IMAGE(fxhead->state_img)
					              , SKIN_DIR"user_busy.png");
			gtk_status_icon_set_from_file(fxmain->trayIcon
										, SKIN_DIR"busy.svg");
			break;
		case P_HIDDEN :
			gtk_image_set_from_file(GTK_IMAGE(fxhead->state_img)
								  , SKIN_DIR"user_invisible.png");
			gtk_status_icon_set_from_file(fxmain->trayIcon
										, SKIN_DIR"invisible.svg");
			break;
		default :
			gtk_image_set_from_file(GTK_IMAGE(fxhead->state_img)
								  , SKIN_DIR"user_away.png");
			gtk_status_icon_set_from_file(fxmain->trayIcon
										, SKIN_DIR"away.svg");
			break;
	}
	free(statename);
}
void fx_head_popup_statemenu_func(GtkWidget* UNUSED(widget)
		, GdkEventButton* event , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	GtkWidget* separator;
	GtkWidget* presence_menu = gtk_menu_new();

	DEBUG_FOOTPRINT();

	fx_head_create_presence_item(P_ONLINE , _("Online") , presence_menu , fxmain);
	fx_head_create_presence_item(P_AWAY   , _("Leave") , presence_menu , fxmain);
	fx_head_create_presence_item(P_BUSY   , _("Busy") , presence_menu , fxmain);
	fx_head_create_presence_item(P_HIDDEN , _("Hide") , presence_menu , fxmain);

	separator = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(presence_menu) , separator);

	fx_head_create_presence_item(P_OUTFORLUNCH  , _("Eating out") , presence_menu , fxmain);
	fx_head_create_presence_item(P_DONOTDISTURB , _("Do Not Disturb") , presence_menu , fxmain);
	fx_head_create_presence_item(P_MEETING      , _("Meeting") , presence_menu , fxmain);
	fx_head_create_presence_item(P_ONTHEPHONE   , _("Calling") , presence_menu , fxmain);

	gtk_widget_show_all(presence_menu);
	
	gtk_menu_popup(GTK_MENU(presence_menu) , NULL , NULL , NULL , NULL 
				, (event != NULL) ? event->button : 0 , gdk_event_get_time((GdkEvent*)event));
}
void fx_head_create_presence_item(int type , const char* message , GtkWidget* menu , FxMain* fxmain)
{
	typedef struct 
	{
		FxMain* fxmain;
		StateType type;
	} Args;
	GtkWidget* item = gtk_image_menu_item_new_with_label(message);
	GtkWidget* item_img = NULL;
	Args *args = (Args*)malloc(sizeof(Args));
	args->fxmain = fxmain;
	args->type = type;
	g_signal_connect(item , "activate" , G_CALLBACK(fx_head_change_state_func) , args);
	switch(type)
	{
		case P_ONLINE :
			item_img = gtk_image_new_from_file(SKIN_DIR"user_online.png");
			break;
		case P_BUSY :
			item_img = gtk_image_new_from_file(SKIN_DIR"user_busy.png");
			break;
		case P_HIDDEN :
			item_img = gtk_image_new_from_file(SKIN_DIR"user_invisible.png");
			break;
		default :
			item_img = gtk_image_new_from_file(SKIN_DIR"user_away.png");
			break;
	}
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item) , item_img);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu) , item);
}
void fx_head_impre_event_func(GtkWidget* widget , GdkEventButton* event , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxHead* fxhead = fxmain->headPanel;
	const char* text = NULL;

	DEBUG_FOOTPRINT();

	switch(event->type)
	{
		case GDK_BUTTON_PRESS :
			gtk_widget_show(fxhead->impre_entry);
			text = gtk_label_get_text(GTK_LABEL(fxhead->impre_label));
			gtk_entry_set_text(GTK_ENTRY(fxhead->impre_entry) , text);
			gtk_entry_select_region(GTK_ENTRY(fxhead->impre_entry) , 0 , strlen(text));
			gtk_widget_grab_focus(fxhead->impre_entry);
			gtk_widget_hide(widget);
			break;
		case GDK_ENTER_NOTIFY :
			gtk_container_set_border_width(GTK_CONTAINER(fxhead->impre_box) , 2);
			break;
		case GDK_LEAVE_NOTIFY :
			gtk_container_set_border_width(GTK_CONTAINER(fxhead->impre_box) , 0);
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

	DEBUG_FOOTPRINT();

	gtk_widget_show(fxhead->impre_box);
	gtk_widget_hide(fxhead->impre_entry);
	return FALSE;
}

gboolean fx_head_impre_activate_func(GtkWidget* widget , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxHead* fxhead = fxmain->headPanel;

	char tooltip[1024];
	const char* impression = gtk_entry_get_text(GTK_ENTRY(widget));
	
	DEBUG_FOOTPRINT();

	gtk_widget_show(fxhead->impre_box);
	gtk_widget_hide(widget);
	if(strcmp(impression , fxhead->oldimpression) == 0)
		return FALSE;
	if(fetion_user_set_moodphrase(fxmain->user , impression) > 0)
	{
		gtk_label_set_text(GTK_LABEL(fxhead->impre_label) , impression);
		bzero(fxhead->oldimpression , sizeof(fxhead->oldimpression));
		strcpy(fxhead->oldimpression , impression);
		bzero(tooltip , sizeof(tooltip));
		sprintf(tooltip , "<b>%s</b>" , impression);
		gtk_widget_set_tooltip_markup(fxhead->impre_label , tooltip);
	}
	return TRUE;
}
void fx_head_change_state_func(GtkWidget* UNUSED(widget) , gpointer data)
{
	typedef struct 
	{
		FxMain* fxmain;
		StateType type;
	} Args;
	Args *args = (Args*)data;
	FxMain* fxmain = args->fxmain;
	User* user = fxmain->user;

	DEBUG_FOOTPRINT();

	if(fetion_user_set_state(user , args->type) > 0)
	{
		fx_head_set_state_image(fxmain , args->type);
	}
}
void fx_head_change_portrait_func(GtkWidget* widget , GdkEventButton* event , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	char *filename = NULL;
	GdkPixbufAnimation *anim_pixbuf = NULL;
	GtkWidget* filechooser = NULL;
	GdkCursor* cursor = NULL;
	struct Args{
		FxMain* fxmain;
		char filename[1024];
	} *args = (struct Args*)malloc(sizeof(struct Args));
	int response;

	DEBUG_FOOTPRINT();

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

	filechooser = gtk_file_chooser_dialog_new(_("Choose the avatar file to upload")
							   , NULL , GTK_FILE_CHOOSER_ACTION_OPEN
							   , _("Upload") , 1 , _("Cancel") , 2 , NULL);
	response = gtk_dialog_run(GTK_DIALOG(filechooser));

	if(response == 1)
	{
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser));
		anim_pixbuf = gdk_pixbuf_animation_new_from_file(SKIN_DIR"LoadingImage.gif", NULL); 
		gtk_image_set_from_animation(GTK_IMAGE(fxmain->headPanel->portrait) , anim_pixbuf);
		args->fxmain = fxmain;
		bzero(args->filename , sizeof(args->filename));
		strcpy(args->filename , filename);
		g_thread_create(fx_head_change_portrait_thread , args , FALSE , NULL);
	}
	
	gtk_widget_destroy(filechooser);
}
void* fx_head_change_portrait_thread(void* data)
{
	struct Args {
		FxMain* fxmain;
		char filename[1024];
	} *args = (struct Args*)data;
	FxMain* fxmain = args->fxmain;
	FxHead* fxhead = fxmain->headPanel;
	Config* config = fxmain->user->config;
	char filepath[128];
	GtkWidget *dialog = NULL;
	GdkPixbuf* pb = NULL;
	
	DEBUG_FOOTPRINT();

	bzero(filepath , sizeof(filepath));
	if(fetion_user_upload_portrait(fxmain->user , args->filename) > 0)
	{
		
		gdk_threads_enter();
		fetion_user_download_portrait(fxmain->user , fxmain->user->sipuri);
		sprintf(filepath , "%s/%s.jpg" , config->iconPath , fxmain->user->sId);
		pb = gdk_pixbuf_new_from_file_at_size(filepath , 50 , 50 , NULL);
		if(pb == NULL)
			pb = gdk_pixbuf_new_from_file_at_size(filepath , 50 , 50 , NULL);
		gtk_image_set_from_pixbuf(GTK_IMAGE(fxhead->portrait) , pb);
		gdk_threads_leave();
	}
	else
	{
		gdk_threads_enter();
		dialog = gtk_message_dialog_new(GTK_WINDOW(fxmain->window),
										GTK_DIALOG_DESTROY_WITH_PARENT,
										GTK_MESSAGE_WARNING,
										GTK_BUTTONS_OK,
										_("Upload avatar failed. Note: This appliaction don't support autocompress "
										"of image. Please be sure that the avater is in 150px × 150px."));
		gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		gdk_threads_leave();

		gdk_threads_enter();
		bzero(filepath , sizeof(filepath));
		sprintf(filepath , "%s/%s.jpg" , config->iconPath , fxmain->user->sId);
		pb = gdk_pixbuf_new_from_file_at_size(filepath , 50 , 50 , NULL);
		if(pb == NULL)
			pb = gdk_pixbuf_new_from_file_at_size(filepath , 50 , 50 , NULL);
		gtk_image_set_from_pixbuf(GTK_IMAGE(fxhead->portrait) , pb);
		gdk_threads_leave();
	}
	free(args);
	return NULL;
}
