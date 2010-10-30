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

static void fx_many_add_information(FxMany* fxmany , const char* text);
static void fx_many_on_close_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_many_on_send_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void* fx_many_sms_send_func(void* data);
static GtkTreeModel* fx_many_create_all_model(FxMany* fxmany);
static gboolean key_press_func(GtkWidget *UNUSED(widget) , GdkEventKey *event
		, gpointer data);

FxMany* fx_many_new(FxMain* fxmain)
{
	FxMany* fxmany = (FxMany*)malloc(sizeof(FxMany));

	memset(fxmany , 0 , sizeof(FxMany));
	fxmany->fxmain = fxmain;
	return fxmany;
}
static GtkTreeModel* fx_many_create_all_model(FxMany* fxmany)
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

	gtk_tree_model_get_iter_root(model , &iter);
	do
	{
		gtk_tree_model_get(model      , &iter
						 , G_NAME_COL   , &name
						 , -1);
		gtk_tree_store_append(store , &inIter , NULL);
		gtk_tree_store_set(store , &inIter
						 , S_CHECK_COL, FALSE
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
				g_object_unref(pb);
				free(name);
				free(sipuri);
			}
			while(gtk_tree_model_iter_next(model , &childIter));
		}
	}
	while(gtk_tree_model_iter_next(model , &iter));
	return GTK_TREE_MODEL(store);
}

static GtkTreeModel* fx_many_create_choosed_model()
{
	GtkTreeStore* store = gtk_tree_store_new(C_COLS_NUM
									   , GDK_TYPE_PIXBUF
									   , G_TYPE_STRING
									   , G_TYPE_STRING
									   );

	return GTK_TREE_MODEL(store);
}
static void fx_many_item_toggled(GtkCellRendererToggle *UNUSED(cell)
		, char* path_str , gpointer data)
{
	FxMany* fxmany = (FxMany*)data;
	GtkTreeView* tree = GTK_TREE_VIEW(fxmany->tree);
	GtkTreeModel* model = gtk_tree_view_get_model(tree);
	GtkTreeView* ctree = GTK_TREE_VIEW(fxmany->selected);
	GtkTreeModel* cmodel = gtk_tree_view_get_model(ctree);
	GtkTreePath* path = gtk_tree_path_new_from_string(path_str);
	GtkTreeIter iter;
	gboolean checked , checked1;
	char* name;
	char* sipuri;
	char* uri;
	char labeltext[128];
	GdkPixbuf* pb;

	GtkTreeIter cIter;
	GtkTreeIter childiter;

	gtk_tree_model_get_iter(model , &iter , path);
	gtk_tree_model_get(model , &iter
					 , S_SIPURI_COL , &sipuri
					 , S_CHECK_COL  , &checked
					 , -1);
	checked ^= 1;

	if(checked)
	{
		if(gtk_tree_path_get_depth(path) > 1){
			gtk_tree_model_get(model , &iter
							 , S_NAME_COL   , &name
							 , S_PIXBUF_COL , &pb
							 , -1);
			gtk_tree_store_append(GTK_TREE_STORE(cmodel) , &cIter , NULL);
			gtk_tree_store_set(GTK_TREE_STORE(cmodel) , &cIter 
							 , C_PIXBUF_COL           , pb
							 , C_NAME_COL             , name
							 , C_SIPURI_COL           , sipuri
							 , -1);
			fxmany->chooseCount ++;

			free(name);
			free(sipuri);
			g_object_unref(pb);
		}else{
			if(gtk_tree_model_iter_children(model , &childiter , &iter)){
				do{
					gtk_tree_model_get(model , &childiter
									 , S_CHECK_COL  , &checked1
									 , S_NAME_COL   , &name
									 , S_SIPURI_COL , &sipuri
									 , S_PIXBUF_COL , &pb
									 , -1);
					if(!checked1){
						gtk_tree_store_append(GTK_TREE_STORE(cmodel) , &cIter , NULL);
						gtk_tree_store_set(GTK_TREE_STORE(cmodel) , &cIter 
										 , C_PIXBUF_COL           , pb
										 , C_NAME_COL             , name
										 , C_SIPURI_COL           , sipuri
										 , -1);
						fxmany->chooseCount ++;
						gtk_tree_store_set(GTK_TREE_STORE(model) , &childiter
								, S_CHECK_COL , TRUE , -1);
					}
					free(name);
					free(sipuri);
					g_object_unref(pb);
				}while(gtk_tree_model_iter_next(model , &childiter));
			}
		}
	}
	else
	{
		if(gtk_tree_path_get_depth(path) > 1){
			gtk_tree_model_get_iter_root(cmodel , &cIter);
			do{
				gtk_tree_model_get(cmodel       , &cIter 
								 , C_SIPURI_COL , &uri
								 , -1);
				if(strcmp(sipuri , uri) == 0){
					free(uri);
					break;
				}
			}while(gtk_tree_model_iter_next(cmodel , &cIter));
			gtk_tree_store_remove(GTK_TREE_STORE(cmodel) , &cIter);
			fxmany->chooseCount --;
		}else{
			if(gtk_tree_model_iter_children(model , &childiter , &iter)){
				do{
					gtk_tree_model_get(model , &childiter 
							, S_CHECK_COL , &checked1
							, S_SIPURI_COL , &sipuri , -1);

					if(!checked1)
						continue;

					gtk_tree_model_get_iter_root(cmodel , &cIter);

					do{
						gtk_tree_model_get(cmodel       , &cIter 
										 , C_SIPURI_COL , &uri
										 , -1);
						if(strcmp(sipuri , uri) == 0){
							free(uri);
							break;
						}
					}while(gtk_tree_model_iter_next(cmodel , &cIter));

					gtk_tree_store_remove(GTK_TREE_STORE(cmodel) , &cIter);
					fxmany->chooseCount --;
					gtk_tree_store_set(GTK_TREE_STORE(model) , &childiter
							, S_CHECK_COL , FALSE , -1);

					free(sipuri);

				}while(gtk_tree_model_iter_next(model , &childiter));
			}
		}
	}
	gtk_tree_store_set(GTK_TREE_STORE(model) , &iter
					 , S_CHECK_COL , checked 
					 , -1);

	gtk_tree_path_free(path);
	bzero(labeltext , sizeof(labeltext));
	sprintf(labeltext , _("Choosed [<span color=\"red\">%d</span>] contacts\n"
						"[<span color=\"red\">%d</span>] more available")
					  , fxmany->chooseCount , 10000 - fxmany->chooseCount);
	gtk_label_set_markup(GTK_LABEL(fxmany->label) , labeltext);

}

static void fx_many_text_cell_data_func(GtkTreeViewColumn *UNUSED(col)
		, GtkCellRenderer   *renderer, GtkTreeModel *model
		, GtkTreeIter *iter , gpointer UNUSED(user_data))
{
	GtkTreePath* path = gtk_tree_model_get_path(model , iter);
	gchar* name;
	gchar* sipuri;
	gchar* sid;
	gchar text[128];

	/* render friend list text*/
	if(gtk_tree_path_get_depth(path) > 1){
		gtk_tree_model_get(model            , iter
						,  S_NAME_COL       , &name
						,  S_SIPURI_COL     , &sipuri
						, -1);
		sid = fetion_sip_get_sid_by_sipuri(sipuri);
		sprintf(text , "%s"
					 , name == NULL ? sid : name);
		g_object_set(renderer 
				   , "markup" , text
				   , "width" , 100
				   , NULL);
		g_free(sid);
		g_free(sipuri);
		g_free(name);
	}/*render group list text*/
	else{
		gtk_tree_model_get(model      , iter
						 , S_NAME_COL , &name
						 , -1);
		sprintf(text , "%s" , name );
		g_object_set(renderer
				   , "text" , text
				   , "width" , 100
				   , NULL);
		g_free(name);
	}
}
static void fx_many_create_all_column(FxMany* fxmany)
{
	GtkCellRenderer* renderer;
	GtkTreeViewColumn *col , *col1;
	GtkWidget* tree = fxmany->tree;

	renderer = gtk_cell_renderer_toggle_new();
	g_signal_connect(renderer , "toggled" , G_CALLBACK(fx_many_item_toggled) , fxmany);
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_pack_start(col , renderer , TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "active", S_CHECK_COL);
	gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (col)
								   , GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (col), 40);
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

static void fx_many_create_selected_column(FxMany* fxmany)
{
	GtkCellRenderer* renderer;
	GtkTreeViewColumn *col0 , *col1;

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

	fxmany->dialog = gtk_dialog_new();
	pb = gdk_pixbuf_new_from_file(SKIN_DIR"groupsend.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxmany->dialog) , pb);
	g_object_unref(pb);
	gtk_window_set_title(GTK_WINDOW(fxmany->dialog) , _("SMS To Many"));
	gtk_widget_set_usize(fxmany->dialog , 660 , 520);
	g_signal_connect(fxmany->dialog , "key-press-event"
			, G_CALLBACK(key_press_func) , fxmany);
	gtk_container_set_border_width(GTK_CONTAINER(fxmany->dialog) , 5);

	fxmany->hbox = gtk_hbox_new(FALSE , 0);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(fxmany->dialog)->vbox) , fxmany->hbox , TRUE , TRUE , 0);
	rbox = gtk_vbox_new(FALSE , 0);
	lbox = gtk_vbox_new(FALSE , 0);
	gtk_box_pack_start(GTK_BOX(fxmany->hbox) , rbox , TRUE , TRUE , 5);
	gtk_box_pack_start(GTK_BOX(fxmany->hbox) , lbox , FALSE , FALSE , 5);
	action_area = GTK_DIALOG(fxmany->dialog)->action_area;
	/*left top area*/
	lt_frame = gtk_frame_new(_("Choose Contacts"));
	gtk_widget_set_usize(lt_frame , 180 , 0);
	model = fx_many_create_all_model(fxmany);
	fxmany->tree = gtk_tree_view_new_with_model(model);
	scrollwindow = gtk_scrolled_window_new(NULL , NULL);
	gtk_container_add(GTK_CONTAINER(scrollwindow) , fxmany->tree);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollwindow)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(fxmany->tree) , FALSE);
   	//gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (fxmany->tree), TRUE);
	gtk_tree_view_set_level_indentation(GTK_TREE_VIEW(fxmany->tree) , -35);
	gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(fxmany->tree) , TRUE);
	gtk_tree_view_columns_autosize(GTK_TREE_VIEW(fxmany->tree));

	fx_many_create_all_column(fxmany);
	gtk_box_pack_start(GTK_BOX(lbox) , lt_frame , TRUE , TRUE , 5);
	gtk_container_add(GTK_CONTAINER(lt_frame) , scrollwindow);
	gtk_frame_set_shadow_type(GTK_FRAME(lt_frame) , GTK_SHADOW_IN);
	/*left bottom area*/
	model1 = fx_many_create_choosed_model();
	fxmany->selected = gtk_tree_view_new_with_model(model1);
	scrollwindow1 = gtk_scrolled_window_new(NULL , NULL);
	gtk_container_add(GTK_CONTAINER(scrollwindow1) , fxmany->selected);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollwindow1)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(fxmany->selected) , FALSE);
   	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (fxmany->selected), TRUE);
	gtk_tree_view_set_level_indentation(GTK_TREE_VIEW(fxmany->selected) , 0);
	gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(fxmany->selected) , TRUE);
	gtk_tree_view_columns_autosize(GTK_TREE_VIEW(fxmany->selected));
	
	fx_many_create_selected_column(fxmany);
	lb_frame = gtk_frame_new(_("Contacts Selected"));
	gtk_widget_set_usize(lb_frame , 0 , 160);
	gtk_box_pack_start(GTK_BOX(lbox) , lb_frame , FALSE , FALSE , 5);
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
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(fxmany->recv_text) , FALSE);
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
	gtk_box_pack_start(GTK_BOX(rbox) , fxmany->send_scroll , FALSE , FALSE , 5);
	gtk_widget_set_usize(fxmany->send_scroll , 0 , 120);

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

	close_button = gtk_button_new_with_label(_("Close"));
	gtk_widget_set_usize(close_button , 100 , 30);
	gtk_box_pack_start(GTK_BOX(action_area) , close_button , FALSE , TRUE , 2);
	g_signal_connect(close_button , "clicked" , G_CALLBACK(fx_many_on_close_clicked) , fxmany->dialog);

	send_button = gtk_button_new_with_label(_("Send"));
	gtk_widget_set_usize(send_button , 100 , 30);
	gtk_box_pack_start(GTK_BOX(action_area) , send_button , FALSE , TRUE , 2);
	g_signal_connect(send_button , "clicked" , G_CALLBACK(fx_many_on_send_clicked) , fxmany);

	gtk_window_set_position(GTK_WINDOW(fxmany->dialog) , GTK_WIN_POS_CENTER);

	GTK_WIDGET_SET_FLAGS(fxmany->send_text, GTK_CAN_FOCUS);
	gtk_widget_grab_focus(fxmany->send_text);

	gtk_widget_show_all(fxmany->dialog);
	gtk_widget_hide(fxmany->dialog);
}
static void fx_many_on_close_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	gtk_dialog_response(GTK_DIALOG(data) , GTK_RESPONSE_CANCEL);
}
static void fx_many_on_send_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxMany* fxmany = (FxMany*)data;

	if(fxmany->chooseCount == 0){
		fx_many_add_information(fxmany,
				_("You have not selected contacts"));
		return;
	}	
	
	g_thread_create(fx_many_sms_send_func , fxmany , FALSE , NULL);
}
static void* fx_many_sms_send_func(void* data)
{
	FxMany* fxmany = (FxMany*)data;
	GtkTreeModel  *model;
	GtkTreeIter    iter;
	gchar         *sipuri;
	gchar         *name;
	GtkTextIter    begin;
	GtkTextIter    end;
	const gchar   *text;
	gchar          alertmsg[1024];
	Conversation  *conv;
	gint           daycount;
	gint           monthcount;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(fxmany->selected));

	gtk_text_buffer_get_start_iter(fxmany->send_buffer , &begin);
	gtk_text_buffer_get_end_iter(fxmany->send_buffer , &end);
	text = gtk_text_buffer_get_text(fxmany->send_buffer , &begin , &end , TRUE);

	if(strlen(text) == 0){
		gdk_threads_enter();
		fx_many_add_information(fxmany , _("Please input the contents of message"));
		gdk_threads_leave();
		g_thread_exit(0);
	}

	gdk_threads_enter();
	gtk_text_buffer_delete(fxmany->send_buffer , &begin , &end);
	gdk_threads_leave();
	
	if(gtk_tree_model_get_iter_root(model , &iter)){
		do{
			gtk_tree_model_get(model, &iter, C_NAME_COL
					, &name, C_SIPURI_COL, &sipuri, -1);

			conv = fetion_conversation_new(fxmany->fxmain->user,
					sipuri, NULL);

			if(fetion_conversation_send_sms_to_phone_with_reply(conv,
						 text, &daycount, &monthcount) > 0)
				sprintf(alertmsg , _("Mesage \"%s\" has been"
							" sent to \"%s\". You have sent %d today and "
						    "%d this month\n"),
							text , name , daycount , monthcount);
			else
				sprintf(alertmsg,
						_("Mesage \"%s\" did not send to \"%s\"\n"),
						text , name);
			
			gdk_threads_enter();
			fx_many_add_information(fxmany , alertmsg);
			gdk_threads_leave();

			g_free(conv);
			g_free(name);
			g_free(sipuri);
		}while(gtk_tree_model_iter_next(model , &iter));
	}
	return NULL;
}

static gboolean key_press_func(GtkWidget *UNUSED(widget) , GdkEventKey *event
		, gpointer data)
{
	FxMany *fxmany ;
	if(event->keyval == GDK_w){
		if(event->state & GDK_CONTROL_MASK){
			fxmany = (FxMany*)data;
			gtk_dialog_response(GTK_DIALOG(fxmany->dialog),
					GTK_RESPONSE_OK);
			return TRUE;
		}else{
			return FALSE;
		}
	}

	return FALSE;
}
