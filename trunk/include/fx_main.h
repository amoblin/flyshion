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

#ifndef FX_MAIN_H
#define FX_MAIN_H

#define NOTIFY_IMAGE_SIZE 48

typedef struct{
	FxMain    *fxmain;
	FetionSip *sip;
	gboolean   terminated;
	gchar      sipuri[50];
} TimeOutArgs;

extern FxMain* fx_main_new();

extern void fx_main_free(FxMain* main);

extern void fx_main_set_user(FxMain* fxmain , User* user);

extern void fx_main_history_init(FxMain *fxmain);

extern void update();

/**
 * time out args function
 */
extern TimeOutArgs* timeout_args_new(FxMain* fxmain , FetionSip *sip , const char *sipuri);

/**
 * private function
 */
extern GtkWidget* fx_main_create_menu(const char* name , const char* iconpath
		, GtkWidget* parent , void (*func)(GtkWidget* item , gpointer data) , gpointer data);

extern void fx_main_process_notification(FxMain* fxmain , const char* sipmsg);

extern void fx_main_process_presence(FxMain* fxmain , const char* xml);

extern void fx_main_process_message(FxMain* fxmain , FetionSip* sip , const char* sipmsg);

extern FxChat* fx_main_create_chat_window(FxMain* fxmain , const char* sipuri);

extern void fx_main_process_invitation(FxMain* fxmain , const char* sipmsg);

extern void fx_main_process_incoming(FxMain* fxmain , FetionSip* sip , const char* sipmsg);

extern void fx_main_process_sipc(FxMain* fxmain , const char* sipmsg);

extern void	fx_main_process_deregistration(FxMain* fxmain);

extern void fx_main_process_syncuserinfo(FxMain* fxmain , const char* xml);

extern void fx_main_process_addbuddyapplication(FxMain* fxmain , const char* sipmsg);

extern void fx_main_process_user_left(FxMain* fxmain , const char* sipmsg);

extern void fx_main_add_history(FxMain *fxmain, const char *name,
		const char *userid, const char *msg, int issend);


/**
 * signal function
 */
extern void fx_main_destroy(GtkWidget* UNUSED(widget) , gpointer UNUSED(data));

extern gboolean fx_main_delete(GtkWidget *widget , GdkEvent *UNUSED(event) , gpointer data);

extern gboolean fx_main_window_state_func(GtkWidget *widget
		, GdkEventWindowState *event , gpointer data);

extern void fx_main_tray_activate_func(GtkWidget *UNUSED(widget) , gpointer data);

extern void fx_main_tray_popmenu_func(GtkWidget* UNUSED(widget)
		, guint button , guint activate_time , gpointer data);

extern void* fx_main_listen_thread_func(void* data);

extern void fx_main_message_func(GtkWidget *UNUSED(widget) , gpointer data);

extern gboolean fx_main_register_func(User* user);

extern gboolean fx_main_check_func(FxMain* fxmain);

extern gboolean fx_main_chat_keep_alive_func(TimeOutArgs* user);

extern void fx_main_about_author_clicked(GtkWidget *UNUSED(widget) , gpointer UNUSED(data));

extern void fx_main_about_fetion_clicked(GtkWidget *UNUSED(widget) , gpointer UNUSED(data));

extern void fx_main_check_update_clicked(GtkWidget* widget , gpointer data);

extern void fx_main_send_to_myself_clicked(GtkWidget* widget , gpointer data);

extern void fx_main_send_to_many_clicked(GtkWidget* widget , gpointer data);

extern void fx_main_personal_setting_clicked(GtkWidget* widget , gpointer data);

extern void fx_main_system_setting_clicked(GtkWidget* widget , gpointer data);

extern void fx_main_add_buddy_clicked(GtkWidget* widget , gpointer data);

extern void fx_main_set_state_clicked(GtkWidget* widget , gpointer data);

extern void fx_main_info_lookup_clicked(GtkWidget* widget , gpointer data);

extern FetionSip* fx_list_find_sip_by_sipuri(FxList* fxlist , const char* sipuri);

extern void fx_list_remove_sip_by_sipuri(FxList* fxlist , const char* sipuri);

extern void fx_list_remove_pg_by_sipuri(FxList* fxlist , const char* sipuri);

extern FxChat* fx_list_find_chat_by_sipuri(FxList* clist , const char* sipuri);

extern void fx_list_remove_chat_by_sipuri(FxList* fxlist , const char* sipuri);

extern TimeOutArgs* fx_list_find_timeout_by_sipuri(FxList* fxlist , const char* sipuri);

extern void fx_list_remove_timeout_by_sipuri(FxList* fxlist , const char* sipuri);
#endif
