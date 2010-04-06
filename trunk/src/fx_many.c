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

FxMany* fx_many_new(FxMain* fxmain)
{
	FxMany* fxmany = (FxMany*)malloc(sizeof(FxMany));

	DEBUG_FOOTPRINT();

	memset(fxmany , 0 , sizeof(FxMany));
	fxmany->fxmain = fxmain;
	return fxmany;
}
GtkTreeModel* fx_many_create_all_model(FxMany* fxmany)
{
	char *name , *sipuri;
	GdkPixbuf* pb;
	GtkTreeIter iter , inIter , childIter;
	GtkWidget* treeview = fxmany->fxmain->mainPanel->treeView;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	GtkTreeStore* store = gtk_tree_store_new(S_COLS_NUM
										   , G_TYPE_BOOLEAN
										   , GDK_TYPE_PIXBUF
										   , G_TYPE_STRING
										   , G_TYPE_STRING
										   , G_TYPE_STRING
										   );

	DEBUG_FOOTPRINT();

	gtk_tree_model_get_iter_root(model , &iter);
	do
	{
		gtk_tree_model_get(model      , &iter
						 , G_NAME_COL   , &name
						 , -1);
		gtk_tree_store_append(store , &inIter , NULL);
		gtk_tree_store_set(store , &inIter
						 , S_CHECK_COL, TRUE
						 , S_NAME_COL , name
						 , -1);
		free(name);
		if(gtk_tree_model_iter_children(model , &childIter , &iter))
		{
			do
			{
				gtk_tree_model_get(model      , &childIter
								 , B_PIXBUF_COL , &pb
								 , B_NAME_COL   , &name
								 , B_SIPURI_COL , &sipuri
								 , -1);
				GtkTreeIter inChildIter;
				pb = gdk_pixbuf_scale_simple(pb , 16 , 16 , GDK_INTERP_NEAREST);
				gtk_tree_store_append(store , &inChildIter , &inIter);
				gtk_tree_store_set(store        , &inChildIter
								 , S_CHECK_COL  , FALSE
								 , S_PIXBUF_COL , pb
								 , S_NAME_COL   , name
								 , S_SIPURI_COL , sipuri
								 , -1);
				free(name);
				free(sipuri);
			}
			while(gtk_tree_model_iter_next(model , &childIter));
		}
	}
	while(gtk_tree_model_iter_next(model , &iter));
	return GTK_TREE_MODEL(store);
}

GtkTreeModel* fx_many_create_choosed_model(FxMany* fxmany)
{
	GtkTreeStore* store = gtk_tree_store_new(C_COLS_NUM
									   , GDK_TYPE_PIXBUF
									   , G_TYPE_STRING
									   , G_TYPE_STRING
									   );

	DEBUG_FOOTPRINT();

	return GTK_TREE_MODEL(store);
}
void fx_many_item_toggled(GtkCellRendererToggle *cell , char* path_str , gpointer data)
{
	FxMany* fxmany = (FxMany*)data;
	GtkTreeView* tree = GTK_TREE_VIEW(fxmany->tree);
	GtkTreeModel* model = gtk_tree_view_get_model(tree);
	GtkTreeView* ctree = GTK_TREE_VIEW(fxmany->selected);
	GtkTreeModel* cmodel = gtk_tree_view_get_model(ctree);
	GtkTreePath* path = gtk_tree_path_new_from_string(path_str);
	GtkTreeIter iter;
	gboolean checked;
	char* name;
	char* sipuri;
	char* uri;
	char labeltext[128];
	GdkPixbuf* pb;

	GtkWidget *dialog;
	GtkTreeIter cIter;

	DEBUG_FOOTPRINT();

	gtk_tree_model_get_iter(model , &iter , path);
	gtk_tree_model_get(model , &iter
					 , S_CHECK_COL  , &checked
					 , S_NAME_COL   , &name
					 , S_SIPURI_COL , &sipuri
					 , S_PIXBUF_COL , &pb
					 , -1);
	checked ^= 1;
	if(checked)
	{
		if(fxmany->chooseCount == 10000)
		{
			dialog = gtk_message_dialog_new(GTK_WINDOW(fxmany->dialog),
											GTK_DIALOG_DESTROY_WITH_PARENT,
											GTK_MESSAGE_WARNING,
											GTK_BUTTONS_OK,
											"最多选择10000个好友！");
			gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			return;
		}
		gtk_tree_store_append(GTK_TREE_STORE(cmodel) , &cIter , NULL);
		gtk_tree_store_set(GTK_TREE_STORE(cmodel) , &cIter 
						 , C_PIXBUF_COL           , pb
						 , C_NAME_COL             , name
						 , C_SIPURI_COL           , sipuri
						 , -1);
		fxmany->chooseCount ++;
	}
	else
	{
		gtk_tree_model_get_iter_root(cmodel , &cIter);
		do
		{
			gtk_tree_model_get(cmodel       , &cIter 
							 , C_SIPURI_COL , &uri
							 , -1);
			if(strcmp(sipuri , uri) == 0)
			{
				free(uri);
				break;
			}
		}
		while(gtk_tree_model_iter_next(cmodel , &cIter));
		gtk_tree_store_remove(GTK_TREE_STORE(cmodel) , &cIter);
		fxmany->chooseCount --;
	}
	gtk_tree_store_set(GTK_TREE_STORE(model) , &iter
					 , S_CHECK_COL , checked 
					 , -1);

	gtk_tree_path_free(path);
	bzero(labeltext , sizeof(labeltext));
	sprintf(labeltext , "已选择好友[<span color=\"red\">%d</span>]人"
					    "还可选择[<span color=\"red\">%d</span>]人"
					  , fxmany->chooseCount , 10000 - fxmany->chooseCount);
	gtk_label_set_markup(GTK_LABEL(fxmany->label) , labeltext);

	free(name);
	free(sipuri);
}

void fx_many_text_cell_data_func(GtkTreeViewColumn *col , GtkCellRenderer   *renderer, GtkTreeModel *model , GtkTreeIter *iter , gpointer user_data)
{
	GtkTreePath* path = gtk_tree_model_get_path(model , iter);
	char* name;
	char* sipuri;
	char* sid;
	char text[128];

	DEBUG_FOOTPRINT();

	bzero(text , sizeof(text));
	/* render friend list text*/
	if(gtk_tree_path_get_depth(path) > 1)
	{
		gtk_tree_model_get(model            , iter
						,  S_NAME_COL       , &name
						,  S_SIPURI_COL     , &sipuri
						, -1);
		sid = fetion_sip_get_sid_by_sipuri(sipuri);
		sprintf(text , "<b>%s</b>"
					 , name == NULL ? sid : name);
		g_object_set(renderer 
				   , "markup" , text
				   , "width" , 100
				   , NULL);
		free(sid);
		free(sipuri);
		free(name);
	}
	/*render group list text*/
	else
	{
		gtk_tree_model_get(model      , iter
						 , S_NAME_COL , &name
						 , -1);
		sprintf(text , "%s" , name );
		g_object_set(renderer
				   , "text" , text
				   , "width" , 100
				   , NULL);
		free(name);
	}
}
void fx_many_create_all_column(FxMany* fxmany)
{
	GtkCellRenderer* renderer;
	GtkTreeViewColumn *col , *col1;
	GtkWidget* tree = fxmany->tree;

	DEBUG_FOOTPRINT();

	renderer = gtk_cell_renderer_toggle_new();
	g_signal_connect(renderer , "toggled" , G_CALLBACK(fx_many_item_toggled) , fxmany);
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_pack_start(col , renderer , FALSE);
	gtk_tree_view_column_add_attribute(col, renderer, "active", S_CHECK_COL);
	gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (col)
								   , GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (col), 20);
	gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (col), TRUE);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree) , col);

	renderer = gtk_cell_renderer_pixbuf_new();

	GtkTreeViewColumn* col0 = gtk_tree_view_column_new(); 
    gtk_tree_view_column_pack_start(col0, renderer , FALSE);
	gtk_tree_view_column_add_attribute(col0, renderer, "pixbuf", S_PIXBUF_COL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col0);

    renderer = gtk_cell_renderer_text_new();
    col1 = gtk_tree_view_column_new(); 
    gtk_tree_view_column_pack_start(col1, renderer , FALSE);
	gtk_tree_view_column_set_cell_data_func(col1
										  , renderer
										  , fx_many_text_cell_data_func
										  , NULL
										  , NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col1);
	gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (col)
								   , GTK_TREE_VIEW_COLUMN_FIXED);

}

void fx_many_create_selected_column(FxMany* fxmany)
{
	GtkCellRenderer* renderer;
	GtkTreeViewColumn *col0 , *col1;

	DEBUG_FOOTPRINT();

	renderer = gtk_cell_renderer_pixbuf_new();

	col0 = gtk_tree_view_column_new(); 
    gtk_tree_view_column_pack_start(col0, renderer , FALSE);
	gtk_tree_view_column_add_attribute(col0, renderer, "pixbuf", C_PIXBUF_COL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(fxmany->selected), col0);

    renderer = gtk_cell_renderer_text_new();
    col1 = gtk_tree_view_column_new(); 
    gtk_tree_view_column_pack_start(col1, renderer , FALSE);
	gtk_tree_view_column_add_attribute(col1, renderer, "text", C_NAME_COL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(fxmany->selected), col1);
	gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (col1)
								   , GTK_TREE_VIEW_COLUMN_FIXED);


}
void fx_many_add_information(FxMany* fxmany , const char* text)
{
	GtkTextIter iter;

	GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxmany->recv_text));

	DEBUG_FOOTPRINT();

	gtk_text_buffer_get_end_iter(buffer , &iter );

	gtk_text_buffer_insert_with_tags_by_name(buffer
					, &iter , text , -1 , "blue" , NULL);

	gtk_text_buffer_insert(buffer , &iter , "\n" , -1);

	gtk_text_iter_set_line_offset (&iter, 0);
	
	fxmany->mark = gtk_text_buffer_get_mark (buffer, "scroll");

	gtk_text_buffer_move_mark (buffer, fxmany->mark, &iter);

	gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW(fxmany->recv_text), fxmany->mark);

}
void fx_many_initialize(FxMany* fxmany)
{
	
	GtkWidget *lt_frame , *lb_frame , *scrollwindow , *scrollwindow1 , *rbox , *lbox;
	GtkWidget *close_button , *send_button;
	GtkTreeModel *model , *model1;
	GtkWidget *action_area = NULL;
	GdkPixbuf *pb = NULL;

	DEBUG_FOOTPRINT();

	fxmany->dialog = gtk_dialog_new();
	pb = gdk_pixbuf_new_from_file(SKIN_DIR"user_online.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxmany->dialog) , pb);
	gtk_window_set_title(GTK_WINDOW(fxmany->dialog) , "短信群发");
	gtk_widget_set_usize(fxmany->dialog , 560 , 520);
	gtk_container_set_border_width(GTK_CONTAINER(fxmany->dialog) , 5);

	fxmany->hbox = gtk_hbox_new(FALSE , 0);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(fxmany->dialog)->vbox) , fxmany->hbox , TRUE , TRUE , 0);
	rbox = gtk_vbox_new(FALSE , 0);
	lbox = gtk_vbox_new(FALSE , 0);
	gtk_box_pack_start(GTK_BOX(fxmany->hbox) , lbox , TRUE , TRUE , 5);
	gtk_box_pack_start(GTK_BOX(fxmany->hbox) , rbox , TRUE , TRUE , 5);
	action_area = GTK_DIALOG(fxmany->dialog)->action_area;
	/*left top area*/
	lt_frame = gtk_frame_new("请选择好友");
	model = fx_many_create_all_model(fxmany);
	fxmany->tree = gtk_tree_view_new_with_model(model);
	scrollwindow = gtk_scrolled_window_new(NULL , NULL);
	gtk_container_add(GTK_CONTAINER(scrollwindow) , fxmany->tree);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollwindow)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(fxmany->tree) , FALSE);
   	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (fxmany->tree), TRUE);
	gtk_tree_view_set_level_indentation(GTK_TREE_VIEW(fxmany->tree) , -30);
	gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(fxmany->tree) , TRUE);
	gtk_tree_view_columns_autosize(GTK_TREE_VIEW(fxmany->tree));

	fx_many_create_all_column(fxmany);
	gtk_box_pack_start(GTK_BOX(lbox) , lt_frame , TRUE , TRUE , 5);
	gtk_container_add(GTK_CONTAINER(lt_frame) , scrollwindow);
	gtk_frame_set_shadow_type(GTK_FRAME(lt_frame) , GTK_SHADOW_IN);
	/*left bottom area*/
	model1 = fx_many_create_choosed_model(fxmany);
	fxmany->selected = gtk_tree_view_new_with_model(model1);
	scrollwindow1 = gtk_scrolled_window_new(NULL , NULL);
	gtk_container_add(GTK_CONTAINER(scrollwindow1) , fxmany->selected);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollwindow1)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(fxmany->selected) , FALSE);
   	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (fxmany->selected), TRUE);
	gtk_tree_view_set_level_indentation(GTK_TREE_VIEW(fxmany->selected) , -30);
	gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(fxmany->selected) , TRUE);
	gtk_tree_view_columns_autosize(GTK_TREE_VIEW(fxmany->selected));
	
	fx_many_create_selected_column(fxmany);
	lb_frame = gtk_frame_new("已选择的好友");
	gtk_box_pack_start(GTK_BOX(lbox) , lb_frame , TRUE , TRUE , 5);
	gtk_container_add(GTK_CONTAINER(lb_frame) , scrollwindow1);
	/*right area*/
	fxmany->label = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(fxmany->label)
					  , "已选择好友[<span color=\"red\">0</span>]人"
					    "还可选择[<span color=\"red\">10000</span>]人");
	gtk_box_pack_start(GTK_BOX(rbox) , fxmany->label , FALSE , FALSE , 5);
	gtk_widget_set_usize(fxmany->label , 200 , 20);
	fxmany->recv_scroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_box_pack_start(GTK_BOX(rbox) , fxmany->recv_scroll , TRUE , TRUE , 5);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fxmany->recv_scroll)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);

	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(fxmany->recv_scroll)
									  , GTK_SHADOW_ETCHED_IN);
	fxmany->recv_text = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxmany->recv_text) , GTK_WRAP_CHAR);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(fxmany->recv_text) , FALSE);
	gtk_container_add(GTK_CONTAINER(fxmany->recv_scroll) , fxmany->recv_text);

	fxmany->recv_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxmany->recv_text));
	gtk_text_buffer_create_tag(fxmany->recv_buffer , "blue" , "foreground" , "blue" , NULL);
	gtk_text_buffer_create_tag(fxmany->recv_buffer , "red" , "foreground" , "red" , NULL);
	gtk_text_buffer_create_tag(fxmany->recv_buffer , "lm10" , "left_margin" , 10 , NULL);
	gtk_text_buffer_get_end_iter(fxmany->recv_buffer , &fxmany->recv_iter);
	gtk_text_buffer_create_mark(fxmany->recv_buffer , "scroll" , &fxmany->recv_iter , FALSE);
	
	fxmany->send_scroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_box_pack_start(GTK_BOX(rbox) , fxmany->send_scroll , TRUE , TRUE , 5);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fxmany->send_scroll)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(fxmany->send_scroll)
									  , GTK_SHADOW_ETCHED_IN);
	fxmany->send_text = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxmany->recv_text) , GTK_WRAP_CHAR);
//	g_signal_connect(send_text , "key_press_event" , G_CALLBACK(ctrlpressed) , pthis);
	gtk_container_add(GTK_CONTAINER(fxmany->send_scroll) , fxmany->send_text);

 	fxmany->send_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxmany->send_text));
	gtk_text_buffer_get_iter_at_offset(fxmany->send_buffer , &fxmany->send_iter , 0);

	close_button = gtk_button_new_with_label("关闭");
	gtk_widget_set_usize(close_button , 100 , 30);
	gtk_box_pack_start(GTK_BOX(action_area) , close_button , FALSE , TRUE , 2);
	g_signal_connect(close_button , "clicked" , G_CALLBACK(fx_many_on_close_clicked) , fxmany->dialog);

	send_button = gtk_button_new_with_label("发送");
	gtk_widget_set_usize(send_button , 100 , 30);
	gtk_box_pack_start(GTK_BOX(action_area) , send_button , FALSE , TRUE , 2);
	g_signal_connect(send_button , "clicked" , G_CALLBACK(fx_many_on_send_clicked) , fxmany);

	gtk_window_set_position(GTK_WINDOW(fxmany->dialog) , GTK_WIN_POS_CENTER);
	gtk_window_set_opacity(GTK_WINDOW(fxmany->dialog) , 0.9);

	gtk_widget_show_all(fxmany->dialog);
	gtk_widget_hide(fxmany->dialog);
}
void fx_many_on_close_clicked(GtkWidget* widget , gpointer data)
{

	DEBUG_FOOTPRINT();

	gtk_dialog_response(GTK_DIALOG(data) , GTK_RESPONSE_CANCEL);
}
void fx_many_on_send_clicked(GtkWidget* widget , gpointer data)
{
	FxMany* fxmany = (FxMany*)data;

	DEBUG_FOOTPRINT();

	if(fxmany->chooseCount == 0)
	{
		fx_many_add_information(fxmany , "您还没有选择好友");
		return;
	}	
	
	g_thread_create(fx_many_sms_send_func , fxmany , FALSE , NULL);
}
void* fx_many_sms_send_func(void* data)
{
	FxMany* fxmany = (FxMany*)data;
	GtkTreeModel* model;
	GtkTreeIter iter;
	char *sipuri , *name;
	GtkTextIter begin , end;
	const char* text;
	char alertmsg[1024];
	Conversation *conv;
	int daycount , monthcount;

	DEBUG_FOOTPRINT();

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(fxmany->selected));

	gtk_text_buffer_get_start_iter(fxmany->send_buffer , &begin);
	gtk_text_buffer_get_end_iter(fxmany->send_buffer , &end);
	text = gtk_text_buffer_get_text(fxmany->send_buffer , &begin , &end , TRUE);
	if(strlen(text) == 0)
	{
		fx_many_add_information(fxmany , "请输入消息内容");
		return;
	}
	gtk_text_buffer_delete(fxmany->send_buffer , &begin , &end);
	
	if(gtk_tree_model_get_iter_root(model , &iter))
	{
		do
		{
			gtk_tree_model_get(model , &iter , C_NAME_COL , &name , C_SIPURI_COL , &sipuri , -1);
			conv = fetion_conversation_new(fxmany->fxmain->user , sipuri , NULL);
			if(fetion_conversation_send_sms_to_phone_with_reply(conv , text , &daycount , &monthcount) > 0)
			{
				bzero(alertmsg , sizeof(alertmsg));
				sprintf(alertmsg , "消息“%s”成功发送至“%s”, 今天已发送短信息%d条，本月已发送%d条\n" , text , name , daycount , monthcount);
			}
			else
			{
				sprintf(alertmsg , "消息“%s”未发送至“%s”\n" , text , name);
			}
			fx_many_add_information(fxmany , alertmsg);
			free(conv);
			free(name);
			free(sipuri);
		}
		while(gtk_tree_model_iter_next(model , &iter));
	}
	return NULL;
}
