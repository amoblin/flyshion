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
	char title[96];
	GdkPixbuf* pb;
	GtkWidget *label1 , *label2 , *refresh_icon , *scrollwindow;
	GtkTextBuffer* buffer;
	GtkTreeModel* model;
	GtkBox* vbox;
	bzero(title , sizeof(title));
	fxhistory->dialog = gtk_dialog_new();
	sprintf(title , "查看与[%s]的聊天记录" , fxhistory->name );
	gtk_window_set_title(GTK_WINDOW(fxhistory->dialog) , title);
	gtk_widget_set_usize(fxhistory->dialog , 450 , 350);
	pb = gdk_pixbuf_new_from_file(SKIN_DIR"history.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxhistory->dialog) , pb);
	gtk_dialog_set_has_separator(GTK_DIALOG(fxhistory->dialog) , FALSE);
	vbox = GTK_BOX(GTK_DIALOG(fxhistory->dialog)->vbox);
	fxhistory->toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(fxhistory->toolbar) , GTK_TOOLBAR_BOTH_HORIZ);
	gtk_box_pack_start(vbox , fxhistory->toolbar , FALSE , TRUE , 0);
	label1 = gtk_label_new("显示最新的");
	gtk_container_add(GTK_CONTAINER(fxhistory->toolbar) , label1);
	model = fx_history_create_count_model();
	fxhistory->daycombo = gtk_combo_box_entry_new_with_model(model , 0);
	gtk_combo_box_set_active(GTK_COMBO_BOX(fxhistory->daycombo) , 1);
	gtk_widget_set_usize(fxhistory->daycombo , 60 , 20);
	gtk_container_add(GTK_CONTAINER(fxhistory->toolbar) , fxhistory->daycombo);
	label2 = gtk_label_new("条聊天记录");
	gtk_container_add(GTK_CONTAINER(fxhistory->toolbar) , label2);
	refresh_icon = gtk_image_new_from_file(SKIN_DIR"refresh.gif");
	gtk_toolbar_append_item(GTK_TOOLBAR(fxhistory->toolbar)
						  , "刷新" , NULL , NULL , refresh_icon
						  , G_CALLBACK(fx_history_on_refresh_clicked)
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
	gtk_button_set_label(GTK_BUTTON(fxhistory->closebtn) , "关闭");
	g_signal_connect(fxhistory->closebtn , "clicked" , G_CALLBACK(fx_history_on_close_clicked) , fxhistory->dialog);
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxhistory->dialog)->action_area) , fxhistory->closebtn);
	fx_history_bind(fxhistory , 20);
	gtk_widget_show_all(fxhistory->dialog);
}
void fx_history_bind(FxHistory* fxhistory , int count)
{
	FxMain* fxmain = fxhistory->fxmain;
	Config* config = fxmain->user->config;
	char text[100];
	char time[30];
	FxList *list , *pos;
	History* history;
	GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxhistory->textview));
	GtkTextIter begin , end;
	
	list = fetion_history_get_list(config , fxhistory->userid , count);
	pos = list;
	gtk_text_buffer_get_start_iter(buffer , &begin);
	gtk_text_buffer_get_end_iter(buffer , &end);
	gtk_text_buffer_delete(buffer , &begin , &end);

	while(pos != NULL)
	{
		bzero(text , sizeof(text));
		bzero(time , sizeof(time));
		history = (History*)(pos->data);
		strftime(time , sizeof(time) , "%m月%d日 %H:%M:%S" , &(history->sendtime));
		if(history->issend)
		{
			sprintf(text , "%s(%s) %s" , history->name , fxmain->user->sId , time );
			gtk_text_buffer_insert_with_tags_by_name(buffer
							, &end , text , -1 , "blue" , NULL);
		}
		else
		{
			sprintf(text , "%s(%s) %s" , history->name , fxmain->user->sId , time );
			gtk_text_buffer_insert_with_tags_by_name(buffer
							, &end , text , -1 , "red" , NULL);
		}
		gtk_text_buffer_insert(buffer , &end , "\n" , -1);
		gtk_text_buffer_insert_with_tags_by_name(buffer
						, &end , history->message , -1 , "lm10" , NULL);
		gtk_text_buffer_insert(buffer , &end , "\n" , -1);
		pos = pos->next;
	}
}
GtkTreeModel* fx_history_create_count_model()
{
	GtkTreeStore* store = gtk_tree_store_new(1 , G_TYPE_STRING);
	char count[5];
	int i;
	for(i = 10 ; i != 100 ; i += 10)
	{
		GtkTreeIter iter;
		gtk_tree_store_append(store , &iter , NULL);
		bzero(count , sizeof(count));
		sprintf(count , "%d" , i);
		gtk_tree_store_set(store , &iter , 0 , count , -1);
	}
	return GTK_TREE_MODEL(store);
}
void fx_history_on_close_clicked(GtkWidget* widget , gpointer data)
{
	GtkDialog* dialog = GTK_DIALOG(data);
	gtk_dialog_response(dialog , GTK_RESPONSE_OK);
}
void fx_history_on_refresh_clicked(GtkWidget* widget , gpointer data)
{
	FxHistory *fxhistory = (FxHistory*)data;
	GtkWidget* entry = gtk_bin_get_child(GTK_BIN(fxhistory->daycombo));
	const char* count = gtk_entry_get_text(GTK_ENTRY(entry));
	fx_history_bind(fxhistory , atoi(count));
}
