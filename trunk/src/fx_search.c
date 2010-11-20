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

FxSearch *fx_search_new(FxMain *fxmain)
{
	FxSearch *search = (FxSearch*)malloc(sizeof(FxSearch));
	search->fxmain = fxmain;
	return search;
}
static void row_activate_func(GtkTreeView *view , GtkTreePath *path
		, GtkTreeViewColumn *UNUSED(column) , gpointer data)
{
	FxSearch *search = (FxSearch*)data;
	FxMain* fxmain = search->fxmain;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(view));
	GtkTreeIter iter;
	char *sipuri , *mobileno , *carrier;
	int serviceStatus , relationStatus , carrierStatus;

	if(path == NULL){
		gtk_widget_destroy(search->window);
		free(search);
		return;
	}

	if(!gtk_tree_model_get_iter(model , &iter , path))
		return;

	gtk_tree_model_get(model      , &iter
					 , B_SIPURI_COL , &sipuri
					 , B_PHONENUM_COL	 , &mobileno
					 , B_SERVICESTATUS_COL , &serviceStatus
					 , B_CARRIER_COL	   , &carrier
					 , B_CARRIERSTATUS_COL , &carrierStatus
					 , B_RELATIONSTATUS_COL , &relationStatus
					 , -1);
	if(relationStatus == RELATION_STATUS_UNAUTHENTICATED)
	{
		fx_util_popup_warning(fxmain , _("You can not send messages to him(her)"
			", because he(she) is not in your contact list"));
		return;
	}
	if(serviceStatus == BASIC_SERVICE_ABNORMAL && 
		(carrierStatus == CARRIER_STATUS_CLOSED ||
		 (carrier != NULL && mobileno == NULL)))
	{
		fx_util_popup_warning(fxmain , _("User has shut his fetion service"
			", so you can not send a message to him"));
		return;
	}
	fx_main_create_chat_window(fxmain , sipuri);
	free(sipuri);
	free(mobileno);
	free(carrier);
}

static gboolean key_press_func(GtkWidget *UNUSED(tree)
		, GdkEventKey *event , gpointer data)
{
	FxSearch *search = (FxSearch*)data;
	if(event->keyval == GDK_q){
		gtk_widget_destroy(search->window);
		free(search);
		return TRUE;
	}

	return FALSE;
}

static gboolean button_press_func(GtkWidget* tree
		, GdkEventButton* event , gpointer data)
{
	GtkTreePath* path = NULL;
	GtkTreeModel* model = NULL;

	if(event->type == GDK_BUTTON_PRESS && event->button == 1)
	{

		model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
		gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(tree) 
				, (gint)event->x , (gint)event->y , &path , NULL , NULL , NULL);
		row_activate_func(GTK_TREE_VIEW(tree) , path , NULL , data);
		return TRUE;
	}
	return FALSE;
}

static gboolean window_focus_out(GtkWidget *UNUSED(widget)
		, GdkEventFocus *UNUSED(event) , gpointer data)
{
	FxSearch *search = (FxSearch*)data;
	gtk_widget_destroy(search->window);
	free(search);
	return TRUE;
}

static void text_cell_data_func(GtkTreeViewColumn *UNUSED(col),
	   							 GtkCellRenderer   *renderer,
								 GtkTreeModel      *model,
								 GtkTreeIter       *iter,
								 gpointer           UNUSED(user_data))
{
	GtkTreePath* path = gtk_tree_model_get_path(model , iter);
	char text[1024];
	/*contact data*/
	char *name , *impression , *sipuri , *sid , *stateStr , *mobileno , *device , *carrier;
	char stateStr1[96];
	char statusStr[256];
	int presence , size;
	int carrierStatus , relationStatus , serviceStatus;

	/* render friend list text*/
	bzero(text , sizeof(text));
	gtk_tree_model_get(model         , iter
					, B_NAME_COL       , &name
					, B_SIPURI_COL     , &sipuri
					, B_STATE_COL   , &presence
					, B_IMPRESSION_COL , &impression
					, B_PHONENUM_COL	, &mobileno
					, B_CARRIER_COL     , &carrier
					, B_CARRIERSTATUS_COL , &carrierStatus
					, B_RELATIONSTATUS_COL , &relationStatus
					, B_SERVICESTATUS_COL , &serviceStatus
					, B_SIZE_COL       , &size
					, B_DEVICE_COL	   , &device
					, -1);

	stateStr = fx_util_get_state_name(presence);
	bzero(statusStr , sizeof(statusStr));
	if(relationStatus == RELATION_STATUS_UNAUTHENTICATED){
		sprintf(statusStr , "<span color='#d4b4b4'>[Unverified]</span>");
	}else if(serviceStatus == BASIC_SERVICE_ABNORMAL){
		if(carrierStatus == CARRIER_STATUS_CLOSED){
			sprintf(statusStr , "<span color='#d4b4b4'>[Has shut fetion service]</span>");
		}else{
			if(carrier != NULL || strlen(carrier) != 0){
				sprintf(statusStr , "<span color='#d4b4b4'>[Online with SMS]</span>");
				if(mobileno == NULL){
					sprintf(statusStr , "<span color='#d4b4b4'>[Has shut fetion service]</span>");
				}
			}else{
				sprintf(statusStr , "<span color='#d4b4b4'>[Has shut fetion service]</span>");
			}
		}
	}else if(carrierStatus == CARRIER_STATUS_DOWN){
		if(strlen(carrier) != 0){
			sprintf(statusStr , "<span color='#d4b4b4'>[Out of service]</span>");
		}
	}
	if(sipuri == NULL){
		return;
	}
	sid = fetion_sip_get_sid_by_sipuri(sipuri);
	bzero(stateStr1 , sizeof(stateStr1));
	sprintf(stateStr1 , "<span color='#0099FF'>%s</span>" , stateStr);
	if( size < 30)
	{
		sprintf(text , "<b>%s</b>%s%s"
					   "(%s)  <span color='#838383'>%s</span>"
					   , name == NULL ? "" : g_markup_escape_text(name , strlen(name))
					   , (statusStr == NULL ? (presence == 0 ? "" : stateStr1) : statusStr)
					   , (device != NULL && strcmp(device , "PC") != 0) ? "[Login with cell phone]" : "" , sid
					   , impression == NULL ? "" : g_markup_escape_text(impression , strlen(impression)));
	}
	else
	{
		sprintf(text , "<b>%s</b>%s%s"
					   "(%s) \n <span color='#838383'>%s</span>"
					   , name == NULL ? "" : g_markup_escape_text(name , strlen(name))
					   , (statusStr == NULL ? (presence == 0 ? "" : stateStr1) : statusStr)
					   , (device != NULL &&strcmp(device , "PC") != 0) ? "[Login with cell phone]" : "" , sid
					   , impression == NULL ? "" : g_markup_escape_text(impression , strlen(impression)));
	}

	g_object_set(renderer , "markup" , text , NULL);

	free(name);
	free(stateStr);
	free(impression);
	free(sipuri);
	free(mobileno);
	free(sid);
	free(device);
	free(carrier);

	gtk_tree_path_free(path);
}

static void create_column(GtkWidget* tree , FxMain* UNUSED(fxmain))
{
	GtkCellRenderer* renderer;
	GtkTreeViewColumn *col , *col0;

	renderer = gtk_cell_renderer_pixbuf_new();
	col = gtk_tree_view_column_new(); 
    gtk_tree_view_column_pack_start(col, renderer , FALSE);
	gtk_tree_view_column_add_attribute(col, renderer, "pixbuf", B_PIXBUF_COL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col);

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

static GtkTreeModel* create_model(GtkTreeModel *model , const char *str)
{
	GtkTreeIter iter , citer , niter;
	GdkPixbuf *pixbuf;
	char *name , *impression , *sipuri;
	char *mobileno , *device , *carrier;
	int presence , size;
	char text[1024];
	int carrierStatus , relationStatus , serviceStatus;

	char *pyname;


	GtkTreeStore *store = gtk_tree_store_new(COL_NUM
							 , GDK_TYPE_PIXBUF
							 , G_TYPE_STRING
							 , G_TYPE_STRING
							 , G_TYPE_STRING
							 , G_TYPE_STRING
							 , G_TYPE_STRING
							 , G_TYPE_STRING
							 , G_TYPE_STRING
							 , G_TYPE_STRING
							 , G_TYPE_STRING
							 , G_TYPE_INT
							 , G_TYPE_INT
							 , G_TYPE_INT
							 , G_TYPE_INT
							 , G_TYPE_INT
							 , G_TYPE_INT
							 , G_TYPE_INT
							 , G_TYPE_INT);

	if(str == NULL)
		return GTK_TREE_MODEL(store);

	gtk_tree_model_get_iter_first(model , &iter);
	do{
		if(gtk_tree_model_iter_children(model , &citer , &iter)){

			do{
				gtk_tree_model_get(model         , &citer
								, B_PIXBUF_COL   , &pixbuf
								, B_NAME_COL       , &name
								, B_SIPURI_COL     , &sipuri
								, B_STATE_COL   , &presence
								, B_IMPRESSION_COL , &impression
								, B_PHONENUM_COL	, &mobileno
								, B_CARRIER_COL     , &carrier
								, B_CARRIERSTATUS_COL , &carrierStatus
								, B_RELATIONSTATUS_COL , &relationStatus
								, B_SERVICESTATUS_COL , &serviceStatus
								, B_SIZE_COL       , &size
								, B_DEVICE_COL	   , &device
								, -1);

				size_t j;
				strcpy(text , str);
				for(j = 0; j < strlen(text) ; j++)
					text[j] = toupper(text[j]);

				pyname = get_pystring(name);

				if(strstr(pyname , text) != NULL){

					gtk_tree_store_append(store , &niter , NULL);

					gtk_tree_store_set(store         , &niter
									, B_PIXBUF_COL     , pixbuf
									, B_NAME_COL       , name
									, B_SIPURI_COL     , sipuri
									, B_STATE_COL   , presence
									, B_IMPRESSION_COL , impression
									, B_PHONENUM_COL	, mobileno
									, B_CARRIER_COL     , carrier
									, B_CARRIERSTATUS_COL , carrierStatus
									, B_RELATIONSTATUS_COL , relationStatus
									, B_SERVICESTATUS_COL , serviceStatus
									, B_SIZE_COL       , size
									, B_DEVICE_COL	   , device
									, -1);
				}
				free(pyname);
				g_object_unref(pixbuf);
				free(name);
				free(sipuri);
				free(impression);
				free(mobileno);
				free(carrier);
				free(device);

			}while(gtk_tree_model_iter_next(model , &citer));
		}
	}while(gtk_tree_model_iter_next(model , &iter));

	return GTK_TREE_MODEL(store);
}

void fx_search_initialize(FxSearch *search , GtkTreeModel *model
		, const char *str , int x , int y)
{
	GtkWidget *frame;

	search->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_decorated(GTK_WINDOW(search->window) , FALSE);
	gtk_window_set_type_hint(GTK_WINDOW(search->window), GDK_WINDOW_TYPE_HINT_DIALOG);
//	gtk_widget_set_name(search->window , "mainwindow");
	gtk_window_set_default_size(GTK_WINDOW(search->window) , 240 , 60);
        gtk_window_set_skip_taskbar_hint (GTK_WINDOW(search->window), TRUE);
	gtk_window_move(GTK_WINDOW(search->window) , x , y);
	g_signal_connect(search->window , "focus-out-event"
			, GTK_SIGNAL_FUNC(window_focus_out) , search);

	frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame) , GTK_SHADOW_ETCHED_IN);

	GtkTreeModel *nmodel = create_model(model , str);
	search->view = gtk_tree_view_new_with_model(nmodel);

	gtk_widget_set_usize(search->view , 320 , 0);
	create_column(search->view , search->fxmain);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(search->view) , FALSE);
 	//gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (search->view), TRUE);
	gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(search->view) , TRUE);
	gtk_container_add(GTK_CONTAINER(search->window) , frame);
	gtk_container_add(GTK_CONTAINER(frame) , search->view);

	g_signal_connect(search->view , "row-activated" , G_CALLBACK(row_activate_func) , search);
	g_signal_connect(search->view , "button-press-event" , G_CALLBACK(button_press_func) , search);
	g_signal_connect(search->view , "key-press-event" , G_CALLBACK(key_press_func) , search);

	gtk_widget_show_all(search->window);
}
