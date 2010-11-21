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

#include <fx_include.h>

static GStaticMutex pg_mutex = G_STATIC_MUTEX_INIT;

static void on_close_clicked(GtkWidget *UNUSED(widget) , gpointer data);
static void on_send_clicked(GtkWidget *UNUSED(widget) , gpointer data);
static GtkTreeModel *create_model(PGGroup *pggroup);
static void create_column(GtkWidget* tree);
static void text_cell_data_func(GtkTreeViewColumn *UNUSED(col),
	   			 GtkCellRenderer   *renderer,
				 GtkTreeModel      *model,
				 GtkTreeIter       *iter,
				 gpointer           UNUSED(user_data));
static void pixbuf_cell_data_func(GtkTreeViewColumn *UNUSED(col),
	   			 GtkCellRenderer   *renderer,
				 GtkTreeModel      *model,
				 GtkTreeIter       *iter,
				 gpointer           UNUSED(user_data));
static void add_information(FxPGGroup* fxpggroup , const char* msg);
static gboolean pggroup_on_key_pressed(GtkWidget* UNUSED(widget)
		, GdkEventKey* event , gpointer data);
static void pggroup_send_message(FxPGGroup *fxpg);
static void pggroup_on_sendsms_clicked(GtkWidget *widget , gpointer data);
static void pggroup_on_space_clicked(GtkWidget *UNUSED(widget) , gpointer data);
static void *pggroup_update_portrait_thread(void *data);
static gboolean on_show_tooltip(GtkWidget* widget
		, int x , int y , gboolean keybord_mode
		, GtkTooltip* tip , gpointer data);
static void on_double_click(GtkTreeView *treeview
		, GtkTreePath *path , GtkTreeViewColumn  *UNUSED(col)
		, gpointer data);
static void *on_contact_show(void *data);
static void pg_window_destroy(GtkWidget *UNUSED(widget) , gpointer data);
static gboolean key_press_func(GtkWidget *UNUSED(widget) , GdkEventKey *event
		, gpointer data);

FxPGGroup *fx_pggroup_new(FxMain *fxmain , PGGroup *pggroup)
{
	FxPGGroup *fxpggroup = (FxPGGroup*)malloc(sizeof(FxPGGroup));

	memset(fxpggroup , 0 , sizeof(FxPGGroup));
	fxpggroup->pggroup = pggroup;
	fxpggroup->fxmain = fxmain;

	return fxpggroup;
}

void fx_pggroup_initialize(FxPGGroup *fxpggroup )
{

	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *lbox;
	GtkWidget *rbox;
	GdkPixbuf *pixbuf;
	GtkWidget *halign1;
	GtkWidget *action_area;
	GtkWidget *close_button;
	GtkWidget *send_button;
	GtkWidget *frame;
	GtkWidget *sendScroll;
	GtkWidget *recvScroll;
	GtkWidget *noticeScroll;
	GtkWidget *listScroll;
	GtkWidget *toolImage;
	GtkTextBuffer *buffer;
	GtkTextIter iter;
	Config *config = fxpggroup->fxmain->user->config;
	char text[1024];
	const char *nonotice = _("no notice!");
	char *pgsid;

	PGGroup *pgcur = fxpggroup->pggroup;

	fxpggroup->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_name(fxpggroup->window , "mainwindow");
	sprintf(text , _("Fetion Group [%s]") , pgcur->name);
	gtk_window_set_title(GTK_WINDOW(fxpggroup->window) , text);
	g_signal_connect(fxpggroup->window , "destroy"
	       	, G_CALLBACK(pg_window_destroy) , fxpggroup);
	g_signal_connect(fxpggroup->window , "key-press-event"
			, G_CALLBACK(key_press_func) , fxpggroup);
	pixbuf = gdk_pixbuf_new_from_file(SKIN_DIR"pggroup.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxpggroup->window) , pixbuf);
	gtk_widget_set_size_request(fxpggroup->window , 650 , 490);
	g_object_unref(pixbuf);

	vbox = gtk_vbox_new(FALSE , 2);
	gtk_container_add(GTK_CONTAINER(fxpggroup->window) , vbox);

	/* top */
	hbox = gtk_hbox_new(FALSE , 5);
	gtk_box_pack_start(GTK_BOX(vbox) , hbox , FALSE , FALSE , 5);

	pgsid = fetion_sip_get_pgid_by_sipuri(fxpggroup->pggroup->pguri);
	snprintf(text , 1023 , "%s/PG%s.jpg" , config->iconPath , pgsid);
	pixbuf = gdk_pixbuf_new_from_file_at_size(text , 40 , 40 , NULL);
	fxpggroup->image = gtk_image_new_from_pixbuf(pixbuf);
	gtk_box_pack_start(GTK_BOX(hbox) , fxpggroup->image , FALSE , FALSE , 10);

	fxpggroup->nameLabel = gtk_label_new(NULL);
	sprintf(text , "<b>%s</b>(%s)" , pgcur->name , pgsid);
	free(pgsid);
	gtk_label_set_markup(GTK_LABEL(fxpggroup->nameLabel) , text);
	gtk_box_pack_start(GTK_BOX(hbox) , fxpggroup->nameLabel , FALSE , FALSE , 2);

	fxpggroup->otherLabel = gtk_label_new(NULL);
	gtk_box_pack_start(GTK_BOX(hbox) , fxpggroup->otherLabel , FALSE , FALSE , 2);

	/* middle */
	hbox = gtk_hbox_new(FALSE , 5);
	gtk_box_pack_start(GTK_BOX(vbox) , hbox , TRUE , TRUE , 0);
	
	/* middle left */
	lbox = gtk_vbox_new(FALSE , 5);
	gtk_box_pack_start(GTK_BOX(hbox) , lbox , TRUE , TRUE , 5);
	
	recvScroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_box_pack_start(GTK_BOX(lbox) , recvScroll , TRUE , TRUE , 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(recvScroll)
					, GTK_POLICY_NEVER
					, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(recvScroll)
					, GTK_SHADOW_ETCHED_IN);
	fxpggroup->recvView = gtk_text_view_new();
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(fxpggroup->recvView) , FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxpggroup->recvView) , GTK_WRAP_CHAR);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(fxpggroup->recvView) , FALSE);
	gtk_container_add(GTK_CONTAINER(recvScroll) , fxpggroup->recvView);

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxpggroup->recvView));
	gtk_text_buffer_create_tag(buffer , "blue" , "foreground" , "#639900" , NULL);
	gtk_text_buffer_create_tag(buffer , "grey" , "foreground" , "#808080" , NULL);
	gtk_text_buffer_create_tag(buffer , "green" , "foreground" , "green" , NULL);
	gtk_text_buffer_create_tag(buffer , "red" , "foreground" , "#0088bf" , NULL);
	gtk_text_buffer_create_tag(buffer , "lm10" , "left_margin" , 10 , NULL);
	gtk_text_buffer_create_tag(buffer , "small" , "left_margin" , 5 , NULL);
	gtk_text_buffer_get_end_iter(buffer , &iter);
	gtk_text_buffer_create_mark(buffer , "scroll" , &iter , FALSE);
	
	/* toolbar begin*/
	fxpggroup->toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(fxpggroup->toolbar) , GTK_TOOLBAR_ICONS);
	gtk_box_pack_start(GTK_BOX(lbox) , fxpggroup->toolbar , FALSE , FALSE , 0);

	pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"myselfsms.png" , 16 , 16 , NULL);
	toolImage = gtk_image_new_from_pixbuf(pixbuf);
	g_object_unref(pixbuf);
	fxpggroup->phoneButton = gtk_toolbar_append_element(GTK_TOOLBAR(fxpggroup->toolbar)
					, GTK_TOOLBAR_CHILD_TOGGLEBUTTON , NULL
					   , _("Send group message") , _("Send group message") , NULL , toolImage
					   , G_CALLBACK(pggroup_on_sendsms_clicked)
					   , fxpggroup );									   
	if(fxpggroup->pggroup->identity != 1)
	    gtk_widget_set_sensitive(fxpggroup->phoneButton , FALSE);

	pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"home.png" , 16 , 16 , NULL);
	toolImage = gtk_image_new_from_pixbuf(pixbuf);
	g_object_unref(pixbuf);
	gtk_toolbar_append_item(GTK_TOOLBAR(fxpggroup->toolbar)
					, _("Access group space") , _("Access group space") , NULL , toolImage
					, G_CALLBACK(pggroup_on_space_clicked) , fxpggroup);
	gtk_toolbar_append_space(GTK_TOOLBAR(fxpggroup->toolbar));

	/* toolbar end*/
	sendScroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_box_pack_start(GTK_BOX(lbox) , sendScroll , FALSE , FALSE , 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sendScroll)
				 , GTK_POLICY_NEVER
				 , GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sendScroll)
				 , GTK_SHADOW_ETCHED_IN);
	fxpggroup->sendView = gtk_text_view_new();
	gtk_widget_set_usize(fxpggroup->sendView , 0 , 100);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxpggroup->sendView) , GTK_WRAP_WORD_CHAR);
	g_signal_connect(fxpggroup->sendView , "key_press_event" , G_CALLBACK(pggroup_on_key_pressed) , fxpggroup);
	gtk_container_add(GTK_CONTAINER(sendScroll) , fxpggroup->sendView);


	/* action area */
	action_area = gtk_hbox_new(FALSE , 0);
	halign1 = gtk_alignment_new( 1 , 0 , 0 , 0);
	gtk_container_add(GTK_CONTAINER(halign1) , action_area);
	gtk_box_pack_start(GTK_BOX(vbox) , halign1 , FALSE , FALSE , 5);
	
	close_button = gtk_button_new_with_label(_("Close"));
	gtk_widget_set_usize(close_button , 100 , 30);
	gtk_box_pack_start(GTK_BOX(action_area) , close_button , FALSE , TRUE , 5);
	g_signal_connect(close_button , "clicked" , G_CALLBACK(on_close_clicked) , fxpggroup);

	send_button = gtk_button_new_with_label(_("Send"));
	gtk_widget_set_usize(send_button , 100 , 30);
	gtk_box_pack_start(GTK_BOX(action_area) , send_button , FALSE , TRUE , 5);
	g_signal_connect(send_button , "clicked" , G_CALLBACK(on_send_clicked) , fxpggroup);

	gtk_window_set_position(GTK_WINDOW(fxpggroup->window) , GTK_WIN_POS_CENTER);

	/*right box */
	rbox = gtk_vbox_new(FALSE , 0);
	gtk_box_pack_start(GTK_BOX(hbox) , rbox , FALSE , FALSE , 5);

	/* notice */
	frame = gtk_frame_new(_("Group Notice"));
	gtk_frame_set_shadow_type(GTK_FRAME(frame) , GTK_SHADOW_NONE);
	gtk_widget_set_usize(frame , 200 , 0);
	gtk_box_pack_start(GTK_BOX(rbox) , frame , FALSE , FALSE , 0);

	noticeScroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_container_add(GTK_CONTAINER(frame) , noticeScroll);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(noticeScroll)
				 , GTK_POLICY_NEVER
				 , GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(noticeScroll)
				 , GTK_SHADOW_ETCHED_IN);
	fxpggroup->noticeView = gtk_text_view_new();
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxpggroup->noticeView));
	gtk_text_buffer_get_end_iter(buffer , &iter);
	if(strlen(pgcur->bulletin) == 0){
		gtk_text_buffer_insert(buffer , &iter , nonotice , strlen(nonotice));
	}
	else
		gtk_text_buffer_insert(buffer , &iter , pgcur->bulletin , strlen(pgcur->bulletin));
	gtk_widget_set_usize(fxpggroup->noticeView , 0 , 100);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxpggroup->noticeView) , GTK_WRAP_WORD_CHAR);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(fxpggroup->noticeView) , FALSE);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(fxpggroup->noticeView) , FALSE);
	gtk_container_add(GTK_CONTAINER(noticeScroll) , fxpggroup->noticeView);

	/* contact list */
	frame = gtk_frame_new(_("Group Members"));
	gtk_frame_set_shadow_type(GTK_FRAME(frame) , GTK_SHADOW_NONE);
	gtk_box_pack_start(GTK_BOX(rbox) , frame , TRUE , TRUE , 0);
	listScroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_widget_set_size_request(listScroll , 225 , 0);
	gtk_container_add(GTK_CONTAINER(frame) , listScroll);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(listScroll)
				 , GTK_POLICY_NEVER
				 , GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(listScroll)
				 , GTK_SHADOW_ETCHED_IN);
	fxpggroup->treeView = gtk_tree_view_new_with_model(create_model(pgcur));
	g_object_set(fxpggroup->treeView , "has-tooltip" , TRUE , NULL);
	g_signal_connect(fxpggroup->treeView , "query-tooltip" , G_CALLBACK(on_show_tooltip) , fxpggroup);
	g_signal_connect(fxpggroup->treeView
			   , "row-activated"
			   , G_CALLBACK(on_double_click)
			   , fxpggroup);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(fxpggroup->treeView) , FALSE);
 	//gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (fxpggroup->treeView), TRUE);
	gtk_tree_view_set_level_indentation(GTK_TREE_VIEW(fxpggroup->treeView) , -30);
	gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(fxpggroup->treeView) , TRUE);
	create_column(fxpggroup->treeView);
	gtk_container_add(GTK_CONTAINER(listScroll) , fxpggroup->treeView);

	g_thread_create(pggroup_update_portrait_thread , fxpggroup , FALSE , NULL);

	gtk_widget_show_all(fxpggroup->window);
}

static void on_close_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxPGGroup *fxpg = (FxPGGroup*)data;

	gtk_widget_destroy(fxpg->window);
}

static void on_send_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxPGGroup *fxpg = (FxPGGroup*)data;
	pggroup_send_message(fxpg);
}

static GtkTreeModel *create_model(PGGroup *pggroup)
{
    	PGGroupMember *member = pggroup->member;
	PGGroupMember *mcur;
	GtkTreeIter iter;
	GdkPixbuf *pixbuf;
	GtkTreeStore *store = gtk_tree_store_new(M_COL_NUM
		, GDK_TYPE_PIXBUF
		, G_TYPE_STRING
		, G_TYPE_STRING
		, G_TYPE_STRING
		, G_TYPE_INT
		, G_TYPE_INT);

	foreach_pg_member(member , mcur){
		if(mcur->state <= 0){
		    gtk_tree_store_append(store , &iter , NULL);
		    pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"invisible.svg"
			    , MEMBER_PROTRAIT_SIZED , MEMBER_PROTRAIT_SIZED , NULL);
		}
		else{
		    gtk_tree_store_prepend(store , &iter , NULL);
		    switch(mcur->state){
			case P_ONLINE :
				pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"online.svg"
				       	, MEMBER_PROTRAIT_SIZED , MEMBER_PROTRAIT_SIZED , NULL);
				break;
			case P_BUSY :
				pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"busy.svg"
				       	, MEMBER_PROTRAIT_SIZED , MEMBER_PROTRAIT_SIZED , NULL);
				break;
			default:
				pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"away.svg"
				       	, MEMBER_PROTRAIT_SIZED , MEMBER_PROTRAIT_SIZED , NULL);
				break;
		    }
			
		}
		gtk_tree_store_set(store , &iter
			, M_PIXBUF_COL , pixbuf
			, M_NICKNAME_COL , g_markup_escape_text(mcur->nickname , strlen(mcur->nickname))
			, M_SIPURI_COL , mcur->sipuri
			, M_IDENTITY_COL , mcur->identity
			, M_STATE_COL , mcur->state
			, -1);
		g_object_unref(pixbuf);

	}

	return GTK_TREE_MODEL(store);
}

static void create_column(GtkWidget* tree)
{
	GtkCellRenderer* renderer;
	GtkTreeViewColumn *col , *col0;

	renderer = gtk_cell_renderer_pixbuf_new();
	col = gtk_tree_view_column_new(); 
	gtk_tree_view_column_pack_start(col, renderer , FALSE);
	gtk_tree_view_column_add_attribute(col, renderer, "pixbuf", M_PIXBUF_COL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col);
	gtk_tree_view_column_set_cell_data_func(col
					  , renderer
					  , pixbuf_cell_data_func
					  , NULL
					  , NULL);

	renderer = gtk_cell_renderer_text_new();
	col0 = gtk_tree_view_column_new(); 
	gtk_tree_view_column_pack_start(col0, renderer , FALSE);
	gtk_tree_view_column_set_cell_data_func(col0
					  , renderer
					  , text_cell_data_func
					  , NULL
					  , NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col0);

}

static void text_cell_data_func(GtkTreeViewColumn *UNUSED(col),
	   			 GtkCellRenderer   *renderer,
				 GtkTreeModel      *model,
				 GtkTreeIter       *iter,
				 gpointer           UNUSED(user_data))
{
	char *sipuri;
	char *nickname;
	char *sid;
	char text[1024];
	int identity;
	int state;

	gtk_tree_model_get(model , iter
		, M_SIPURI_COL , &sipuri
		, M_NICKNAME_COL , &nickname
		, M_IDENTITY_COL , &identity
		, M_STATE_COL , &state
		, -1);

	sid = fetion_sip_get_sid_by_sipuri(sipuri);

	sprintf(text , "<b>%s</b> %s(%s)"
	       	, *nickname == '\0' ? sid : g_markup_escape_text(nickname , strlen(nickname))
		, identity == 2 ? _("<span color='#0099ff'>[Administrator]</span>")
	       		: ( identity == 1 ?_("<span color='#0099ff'>[Super Administrator]</span>") : "")
		, sid);
		
	g_object_set(renderer , "markup" , text , NULL);

	free(sipuri);
	free(nickname);
	free(sid);
}

static void pixbuf_cell_data_func(GtkTreeViewColumn *UNUSED(col),
	   			 GtkCellRenderer   *renderer,
				 GtkTreeModel      *model,
				 GtkTreeIter       *iter,
				 gpointer           UNUSED(user_data))
{
	int state;
	gtk_tree_model_get(model , iter
		, M_STATE_COL , &state , -1);
	if(state > 0)
		g_object_set(renderer , "sensitive" , TRUE , NULL);
	else
		g_object_set(renderer , "sensitive" , FALSE , NULL);

}

void pg_add_message(FxPGGroup* fxpggroup , const char* msg
	       	, const struct tm* datetime , PGGroupMember *contact)
{
	GtkTextIter iter;
	GtkTextBuffer* buffer;
	GtkTextMark *mark;
	FxMain* fxmain = fxpggroup->fxmain;
	User* user = fxmain->user;
	char time[256];
	char text[2048];
	char color[16];
	//char *usid;

	if(contact == NULL)
		strcpy(color , "blue");
	else
		strcpy(color , "red");

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxpggroup->recvView));
	strftime(time , sizeof(time) , "%H:%M:%S" , datetime);

	//usid = fetion_sip_get_sid_by_sipuri(contact->sipuri);
	if(contact != NULL){
		sprintf(text , _("%s says: (%s):\n") , contact->nickname , time);
	}else{
		sprintf(text , _("%s says: (%s):\n") , user->nickname , time);
	}
	gtk_text_buffer_get_end_iter(buffer , &iter );
	gtk_text_buffer_insert_with_tags_by_name(buffer
					, &iter , text , -1 , color , NULL);
	gtk_text_buffer_insert_with_tags_by_name(buffer
					, &iter, msg , strlen(msg) , "lm10" , NULL);
	gtk_text_buffer_insert(buffer , &iter , "\n" , -1);
	gtk_text_iter_set_line_offset (&iter, 0);
	mark = gtk_text_buffer_get_mark (buffer, "scroll");
	gtk_text_buffer_move_mark (buffer, mark, &iter);
	gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW(fxpggroup->recvView), mark);
}

static void add_information(FxPGGroup* fxpggroup , const char* msg)
{
	GtkTextIter iter;
	GtkTextBuffer* buffer;
	GtkTextMark *mark;

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxpggroup->recvView));
	gtk_text_buffer_get_end_iter(buffer , &iter );
	gtk_text_buffer_insert_with_tags_by_name(buffer
					, &iter, msg , strlen(msg) , "grey" , NULL);
	gtk_text_buffer_insert(buffer , &iter , "\n" , -1);
	gtk_text_iter_set_line_offset (&iter, 0);
	mark = gtk_text_buffer_get_mark (buffer, "scroll");
	gtk_text_buffer_move_mark (buffer, mark, &iter);
	gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW(fxpggroup->recvView), mark);
}

static gboolean pggroup_on_key_pressed(GtkWidget* UNUSED(widget)
		, GdkEventKey* event , gpointer data)
{

	FxPGGroup *fxpg;
	Config *config = NULL;

	if(event->keyval == GDK_Return || event->keyval == GDK_ISO_Enter || event->keyval == GDK_KP_Enter)
	{
		fxpg = (FxPGGroup*)data;
		config = fxpg->fxmain->user->config;
		if(config->sendMode == SEND_MODE_ENTER)
		{
			if(event->state & GDK_CONTROL_MASK || event->state & GDK_SHIFT_MASK){
				return FALSE;
			}else{
 				if (gtk_im_context_filter_keypress (GTK_TEXT_VIEW(fxpg->sendView)->im_context, event)) {
					GTK_TEXT_VIEW (fxpg->sendView)->need_im_reset = TRUE;
 					return TRUE;
 				}
				pggroup_send_message(fxpg);
				return TRUE;
			}
		}
		else
		{
			if(event->state & GDK_CONTROL_MASK)	{
				pggroup_send_message(fxpg);
				return TRUE;
			}else{
				return FALSE;
			}
		}
	}
	return FALSE;
}
static void pggroup_send_message(FxPGGroup *fxpg)
{
	User *user = fxpg->fxmain->user;
	GtkTextView *text = GTK_TEXT_VIEW(fxpg->sendView);
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(text);
	GtkTextIter biter;
	GtkTextIter eiter;
	gchar *value;
	struct tm *now;
	now = get_currenttime();

	gtk_text_buffer_get_start_iter(buffer , &biter);
	gtk_text_buffer_get_end_iter(buffer , &eiter);
	value = gtk_text_buffer_get_text(buffer , &biter , &eiter , TRUE);

	if(*value == '\0')
	    return;
	pg_add_message(fxpg , value , now , NULL);
	gtk_text_buffer_delete(buffer , &biter , &eiter);
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxpg->phoneButton)))
	    pg_group_send_message(user , fxpg->pggroup , value);
	else
	    pg_group_send_sms(user , fxpg->pggroup , value);
}

void pggroup_on_sendsms_clicked(GtkWidget *widget , gpointer data)
{
    	FxPGGroup *fxpg = (FxPGGroup*)data;
	GtkToggleButton *btn = GTK_TOGGLE_BUTTON(widget);
	if(gtk_toggle_button_get_active(btn)){
		add_information(fxpg , _("Group member who didn't bind fetion number"
			    " to mobile number can't receive the group sms you send"));
	}
}

void pggroup_on_space_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
    	FxPGGroup *fxpg = (FxPGGroup*)data;
	char *pgsid;
	char text[1024];

	if(fork() == 0){
		pgsid = fetion_sip_get_pgid_by_sipuri(fxpg->pggroup->pguri);
		sprintf(text , "%s?GroupID=%s" , PGGROUP_SPACE_URI , pgsid);
		execlp("xdg-open" , "xdg-open" , text , (char**)NULL);
	}
}
static void *pggroup_update_portrait_thread(void *data)
{
	FxPGGroup *fxpg = (FxPGGroup*)data;
	User *user = fxpg->fxmain->user;
	Config *config = user->config;
	GtkTreeView *tree = GTK_TREE_VIEW(fxpg->treeView);
	GtkTreeModel *model = gtk_tree_view_get_model(tree);
	GtkTreeIter iter;
	GdkPixbuf *pixbuf;
	char portraitPath[256];
	char path[1024];
	char *sipuri;
	char *sid;

	if(gtk_tree_model_get_iter_first(model , &iter)){
		do{
			if(!fxpg || !fxpg->pggroup)
				return NULL;
			gtk_tree_model_get(model , &iter , M_SIPURI_COL , &sipuri , -1);
			sid = fetion_sip_get_sid_by_sipuri(sipuri);
			snprintf(path , 1023 , "%s/%s.jpg" , config->iconPath , sid);
			pixbuf = gdk_pixbuf_new_from_file_at_size(path
			       	, MEMBER_PROTRAIT_SIZED , MEMBER_PROTRAIT_SIZED , NULL);

			if(!fxpg || !fxpg->pggroup)
				return NULL;

			g_static_mutex_lock(&pg_mutex);
			if(pixbuf == NULL && fxpg->pggroup->hasImage == 0){
			    snprintf(portraitPath , 255 , "/%s/getportrait.aspx" , config->portraitServerPath );
			    fetion_user_download_portrait_with_uri(user , sipuri
				    , config->portraitServerName , portraitPath);
			    snprintf(path , 1023 , "%s/%s.jpg" , config->iconPath , sid);
			    pixbuf = gdk_pixbuf_new_from_file_at_size(path
				    , MEMBER_PROTRAIT_SIZED , MEMBER_PROTRAIT_SIZED , NULL);
			}
			g_static_mutex_unlock(&pg_mutex);

			if(!fxpg || !fxpg->pggroup)
				return NULL;

			if(pixbuf != NULL){
				gdk_threads_enter();
				gtk_tree_store_set(GTK_TREE_STORE(model) , &iter
					, PG_PIXBUF_COL , pixbuf , -1);
				gdk_threads_leave();
				g_object_unref(pixbuf);
			}
			free(sipuri);
			free(sid);

		}while(gtk_tree_model_iter_next(model , &iter));
		if(!fxpg)
			return NULL;
		fxpg->pggroup->hasImage = 1;
	}
	return NULL;
}

static gboolean on_show_tooltip(GtkWidget* widget
		, int x , int y , gboolean keybord_mode
		, GtkTooltip* tip , gpointer data)
{
    	FxPGGroup *fxpg = (FxPGGroup*)data;
	FxMain *fxmain = fxpg->fxmain;
	Config *config = fxmain->user->config;
	GtkTreePath *path;
	GtkTreeIter iter;
	GtkTreeView *tree;
	GtkTreeModel *model;
	GdkPixbuf *pixbuf;
	char *sipuri;
	char *nickname;
	char *sid;
	char text[2048];
	char iconpath[256];

	tree = GTK_TREE_VIEW(widget);
	model = gtk_tree_view_get_model(tree);

	if(!gtk_tree_view_get_tooltip_context(tree , &x , &y , keybord_mode 
					, &model , &path , &iter)){
		return FALSE;
	}
	gtk_tree_model_get(model , &iter
		, M_SIPURI_COL , &sipuri
	       	, M_NICKNAME_COL , &nickname , -1);

	sid = fetion_sip_get_sid_by_sipuri(sipuri);

	snprintf(text , 2047 , _("<span color='#808080'>NickName :</span> <b>%s</b>\t\n"
			"<span color='#808080'>FetionNumber :</span> %s\t\n")
		, g_markup_escape_text(nickname , strlen(nickname)) , sid);
	gtk_tooltip_set_markup(tip , text);
	sprintf(iconpath , "%s/%s.jpg" , config->iconPath , sid);
	pixbuf = gdk_pixbuf_new_from_file_at_size(iconpath , 80 , 80 , NULL);
	if(pixbuf == NULL)
	    pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"online.svg" , 80 , 80 , NULL);
	gtk_tooltip_set_icon(tip , pixbuf);	
	g_object_unref(pixbuf);
	gtk_tree_view_set_tooltip_row(tree , tip , path);
	gtk_tree_path_free(path);

	free(nickname);
	free(sipuri);
	free(sid);
	return TRUE;
}

static void on_double_click(GtkTreeView *treeview
		, GtkTreePath *path , GtkTreeViewColumn  *UNUSED(col)
		, gpointer data)
{
    	struct args{
		FxPGGroup *fxpg;
		char *sipuri;
	} *arg = (struct args*)malloc(sizeof(struct args));
	FxPGGroup *fxpg = (FxPGGroup*)data;
	GtkTreeModel *model = gtk_tree_view_get_model(treeview);
	GtkTreeIter iter;
	char *sipuri;

	gtk_tree_model_get_iter(model , &iter , path);
	gtk_tree_model_get(model , &iter , M_SIPURI_COL , &sipuri , -1);

	arg->fxpg = fxpg;
       	arg->sipuri = sipuri;	

	g_thread_create(on_contact_show , arg , FALSE , NULL);

}

static void *on_contact_show(void *data)
{
    	struct args{
		FxPGGroup *fxpg;
		char *sipuri;
	} *arg = (struct args*)data;
	FxPGGroup *fxpg = arg->fxpg;
	char *sipuri = arg->sipuri;
	char *sid;
	Contact *contact;
	User *user = fxpg->fxmain->user;
	FxLookupres *lookup;


	sid = fetion_sip_get_sid_by_sipuri(sipuri);
	contact = fetion_contact_get_contact_info_by_no(user , sid , FETION_NO);

	lookup = fx_lookupres_new(fxpg->fxmain , contact);
	gdk_threads_enter();
	fx_lookupres_initialize(lookup);
	gtk_dialog_run(GTK_DIALOG(lookup->dialog));
	gtk_widget_destroy(lookup->dialog);
	gdk_threads_leave();

	free(sipuri);
	free(sid);
	free(arg);

	return NULL;
}

static void pg_window_destroy(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxPGGroup *fxpg = (FxPGGroup*)data;

	fx_list_remove_pg_by_sipuri(fxpg->fxmain->pglist , fxpg->pggroup->pguri);

	fxpg->pggroup = NULL;
}

static gboolean key_press_func(GtkWidget *widget , GdkEventKey *event
		, gpointer UNUSED(data))
{
	if(event->keyval == GDK_w){
		if(event->state & GDK_CONTROL_MASK){
			gtk_widget_destroy(widget);
			return TRUE;
		}else{
			return FALSE;
		}
	}

	return FALSE;
}
