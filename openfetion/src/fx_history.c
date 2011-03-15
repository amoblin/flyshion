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

static GtkTreeModel* fx_history_create_count_model();
static void fx_history_on_close_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_history_on_refresh_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_history_on_today_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_history_on_yestorday_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_history_on_month_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_history_on_all_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_history_on_export_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_history_on_cleanup_clicked(GtkWidget* UNUSED(widget) , gpointer data);


FxHistory* fx_history_new(FxMain* fxmain , const char* userid , const char* name)
{
	FxHistory* fxhistory = (FxHistory*)malloc(sizeof(FxHistory));
	memset(fxhistory , 0 , sizeof(FxHistory));
	strcpy(fxhistory->userid , userid);
	strcpy(fxhistory->name , name);
	fxhistory->fxmain = fxmain;
	return fxhistory;
}

void fx_history_initialize(FxHistory* fxhistory)
{
	char title[128];
	GdkPixbuf* pb;
	GtkWidget *label1 , *label2 , *refresh_icon , *today_icon;
	GtkWidget *yest_icon , *month_icon , *all_icon , *exp_icon;
	GtkWidget *scrollwindow;
	GtkTextBuffer* buffer;
	GtkTreeModel* model;
	GtkBox* vbox;
	FxList *list;
	Config* config = fxhistory->fxmain->user->config;

	fxhistory->dialog = gtk_dialog_new();
	snprintf(title, sizeof(title) - 1 , _("View logs with [%s]") , fxhistory->name );
	gtk_window_set_title(GTK_WINDOW(fxhistory->dialog) , title);
	gtk_widget_set_usize(fxhistory->dialog , 750 , 550);
	pb = gdk_pixbuf_new_from_file(SKIN_DIR"history.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxhistory->dialog) , pb);
	g_object_unref(pb);
	gtk_dialog_set_has_separator(GTK_DIALOG(fxhistory->dialog) , FALSE);
	vbox = GTK_BOX(GTK_DIALOG(fxhistory->dialog)->vbox);
	fxhistory->toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(fxhistory->toolbar) , GTK_TOOLBAR_BOTH_HORIZ);
	gtk_box_pack_start(vbox , fxhistory->toolbar , FALSE , TRUE , 0);
	label1 = gtk_label_new(_("Show the most recent"));
	gtk_container_add(GTK_CONTAINER(fxhistory->toolbar) , label1);
	model = fx_history_create_count_model();
	fxhistory->daycombo = gtk_combo_box_entry_new_with_model(model , 0);
	gtk_combo_box_set_active(GTK_COMBO_BOX(fxhistory->daycombo) , 1);
	gtk_widget_set_usize(fxhistory->daycombo , 60 , 20);
	gtk_container_add(GTK_CONTAINER(fxhistory->toolbar) , fxhistory->daycombo);
	label2 = gtk_label_new(_("mesage"));
	gtk_container_add(GTK_CONTAINER(fxhistory->toolbar) , label2);
	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"refresh.png" , 16 , 16 , NULL);
	refresh_icon = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_toolbar_append_item(GTK_TOOLBAR(fxhistory->toolbar)
						  , _("Refresh") , NULL , NULL , refresh_icon
						  , G_CALLBACK(fx_history_on_refresh_clicked)
						  , fxhistory);
	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"login.png" , 16 , 16 , NULL);
	today_icon = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_toolbar_append_item(GTK_TOOLBAR(fxhistory->toolbar)
						  , _("Today") , NULL , NULL , today_icon
						  , G_CALLBACK(fx_history_on_today_clicked)
						  , fxhistory);

	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"login.png" , 16 , 16 , NULL);
	yest_icon = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_toolbar_append_item(GTK_TOOLBAR(fxhistory->toolbar)
						  , _("This Week") , NULL , NULL , yest_icon
						  , G_CALLBACK(fx_history_on_yestorday_clicked)
						  , fxhistory);
	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"login.png" , 16 , 16 , NULL);
	month_icon = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_toolbar_append_item(GTK_TOOLBAR(fxhistory->toolbar)
						  , _("This Month") , NULL , NULL , month_icon
						  , G_CALLBACK(fx_history_on_month_clicked)
						  , fxhistory);
	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"login.png" , 16 , 16 , NULL);
	all_icon = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_toolbar_append_item(GTK_TOOLBAR(fxhistory->toolbar)
						  , _("All") , NULL , NULL , all_icon
						  , G_CALLBACK(fx_history_on_all_clicked)
						  , fxhistory);

	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"login.png" , 16 , 16 , NULL);
	exp_icon = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_toolbar_append_item(GTK_TOOLBAR(fxhistory->toolbar)
						  , _("Export") , NULL , NULL , exp_icon
						  , G_CALLBACK(fx_history_on_export_clicked)
						  , fxhistory);
	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"login.png" , 16 , 16 , NULL);
	exp_icon = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_toolbar_append_item(GTK_TOOLBAR(fxhistory->toolbar)
						  , _("Cleanup") , NULL , NULL , exp_icon
						  , G_CALLBACK(fx_history_on_cleanup_clicked)
						  , fxhistory);
	fxhistory->textview = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxhistory->textview) , GTK_WRAP_CHAR);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(fxhistory->textview) , FALSE);
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxhistory->textview));
	gtk_text_buffer_create_tag(buffer , "blue" , "foreground" , "blue" , NULL);
	gtk_text_buffer_create_tag(buffer , "green" , "foreground" , "green" , NULL);
	gtk_text_buffer_create_tag(buffer , "red" , "foreground" , "red" , NULL);
	gtk_text_buffer_create_tag(buffer , "lm10" , "left_margin" , 10 , NULL);
	scrollwindow = gtk_scrolled_window_new(NULL , NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollwindow)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrollwindow)
									  , GTK_SHADOW_ETCHED_IN);
	gtk_container_add(GTK_CONTAINER(scrollwindow) , fxhistory->textview);
	gtk_box_pack_start(vbox , scrollwindow , TRUE , TRUE , 5);
	fxhistory->closebtn = gtk_button_new();
	gtk_button_set_label(GTK_BUTTON(fxhistory->closebtn) , _("Close"));
	g_signal_connect(fxhistory->closebtn, "clicked",
			G_CALLBACK(fx_history_on_close_clicked),
			fxhistory);
	gtk_box_pack_start_defaults(
			GTK_BOX(GTK_DIALOG(fxhistory->dialog)->action_area),
			fxhistory->closebtn);
	list = fetion_history_get_list(config , fxhistory->userid , 20);
	fx_history_bind(fxhistory , list);
	gtk_widget_show_all(fxhistory->dialog);
}
void fx_history_bind(FxHistory* fxhistory , FxList *list)
{
	char text[2048];
	FxList *cur;
	History *history;
	GtkTextBuffer *buffer;
	GtkTextIter begin , end;
	
	buffer = gtk_text_view_get_buffer(
			GTK_TEXT_VIEW(fxhistory->textview));
	gtk_text_buffer_get_start_iter(buffer , &begin);
	gtk_text_buffer_get_end_iter(buffer , &end);
	gtk_text_buffer_delete(buffer , &begin , &end);

	if(!list)
		return;

	foreach_list_back(list , cur){
		history = (History*)(cur->data);
		if(history->issend){
			snprintf(text, sizeof(text) -1,
					"%s(%s) ：", history->name,
					history->sendtime );
			gtk_text_buffer_insert_with_tags_by_name(buffer
							, &end , text , -1 , "blue" , NULL);
		}else{
			snprintf(text, sizeof(text) -1,
					"%s(%s) :", history->name, 
					history->sendtime );
			gtk_text_buffer_insert_with_tags_by_name(buffer
							, &end , text , -1 , "red" , NULL);
		}
		gtk_text_buffer_insert(buffer , &end , "\n" , -1);
		gtk_text_buffer_insert_with_tags_by_name(buffer
						, &end , history->message
						, -1 , "lm10" , NULL);
		gtk_text_buffer_insert(buffer , &end , "\n" , -1);
	}
}
static GtkTreeModel* fx_history_create_count_model()
{
	GtkTreeStore* store = gtk_tree_store_new(1 , G_TYPE_STRING);
	char count[5];
	int i;
	for(i = 10 ; i != 100 ; i += 10){
		GtkTreeIter iter;
		gtk_tree_store_append(store , &iter , NULL);
		sprintf(count , "%d" , i);
		gtk_tree_store_set(store , &iter , 0 , count , -1);
	}
	return GTK_TREE_MODEL(store);
}
static void fx_history_on_close_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxHistory *fxhistory = (FxHistory*)data;
	gtk_dialog_response(GTK_DIALOG(fxhistory->dialog)
			, GTK_RESPONSE_OK);
}
static void fx_history_on_refresh_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxHistory *fxhistory = (FxHistory*)data;
	FxList *list;
	GtkWidget* entry = gtk_bin_get_child(GTK_BIN(fxhistory->daycombo));
	const char* count = gtk_entry_get_text(GTK_ENTRY(entry));
	Config* config = fxhistory->fxmain->user->config;

	list = fetion_history_get_list(config , fxhistory->userid , atoi(count));
	fx_history_bind(fxhistory , list);
}

static void fx_history_on_today_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxHistory *fxhistory = (FxHistory*)data;
	Config* config = fxhistory->fxmain->user->config;
	FxList *list = fetion_history_get_e_list(config , fxhistory->userid , HISTORY_TODAY);
	fx_history_bind(fxhistory , list);
}

static void fx_history_on_yestorday_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxHistory *fxhistory = (FxHistory*)data;
	Config* config = fxhistory->fxmain->user->config;
	FxList *list = fetion_history_get_e_list(config,
			fxhistory->userid , HISTORY_WEEK);
	fx_history_bind(fxhistory , list);
}

static void fx_history_on_month_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxHistory *fxhistory = (FxHistory*)data;
	Config* config = fxhistory->fxmain->user->config;
	FxList *list = fetion_history_get_e_list(config , fxhistory->userid , HISTORY_MONTH);
	fx_history_bind(fxhistory , list);
}

static void fx_history_on_all_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxHistory *fxhistory = (FxHistory*)data;
	Config* config = fxhistory->fxmain->user->config;
	FxList *list = fetion_history_get_e_list(config , fxhistory->userid , HISTORY_ALL);
	fx_history_bind(fxhistory , list);
}

static void fx_history_on_export_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxHistory *fxhistory = (FxHistory*)data;
	FxMain *fxmain = fxhistory->fxmain;
	User *user = fxmain->user;
	Config *config = user->config;
	GtkWidget *dialog;
	char text[1024];

	dialog = gtk_file_chooser_dialog_new (_("Export File"),
						  GTK_WINDOW(fxhistory->fxmain->window),
						  GTK_FILE_CHOOSER_ACTION_SAVE,
						  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						  GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
						  NULL);

	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER (dialog), TRUE);
	gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), getenv("HOME"));
	sprintf(text , _("logs with %s.txt") , fxhistory->name);
	gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), text);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		fetion_history_export(config , user->sId , fxhistory->userid , filename);
		g_free (filename);
	}
	gtk_widget_destroy (dialog);

}

static void fx_history_on_cleanup_clicked(GtkWidget *UNUSED(widget), gpointer data)
{
	FxHistory *fxhistory = (FxHistory*)data;
	FxMain *fxmain = fxhistory->fxmain;
	User *user = fxmain->user;
	Config *config = user->config;
	GtkWidget *dialog;
	GtkTextBuffer *buffer;
	GtkTextIter begin , end;
	int ret;

	dialog = gtk_message_dialog_new (GTK_WINDOW(fxhistory->dialog),
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_WARNING,
                                 GTK_BUTTONS_OK_CANCEL,
                                 _("The action can not be undone.\n"
								 "Are you sure you want to continue?"));
	ret = gtk_dialog_run (GTK_DIALOG (dialog));
	if(ret == GTK_RESPONSE_OK){

		fetion_history_delete(config, fxhistory->userid);
	
		buffer = gtk_text_view_get_buffer(
				GTK_TEXT_VIEW(fxhistory->textview));
		gtk_text_buffer_get_start_iter(buffer , &begin);
		gtk_text_buffer_get_end_iter(buffer , &end);
		gtk_text_buffer_delete(buffer , &begin , &end);
	}
	gtk_widget_destroy (dialog);
}
