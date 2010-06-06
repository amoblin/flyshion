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

int all_light = 0;

Args* fx_args_new(FxMain* fxmain , GtkTreeIter iter , const char* ss , int ii)
{
	Args* args = (Args*)malloc(sizeof(Args));

	DEBUG_FOOTPRINT();

	memset(args , 0 , sizeof(args));
	args->fxmain = fxmain;
	if(ss != NULL)
		strcpy(args->s , ss);
	args->i = ii;
	args->iter = iter;
	return args;
}
FxTree* fx_tree_new()
{
	FxTree* fxtree = (FxTree*)malloc(sizeof(FxTree));

	DEBUG_FOOTPRINT();

	memset(fxtree , 0 , sizeof(FxTree));
	return fxtree;
}
void fx_tree_initilize(FxMain* fxmain)
{
	GtkWidget* mainbox = fxmain->mainbox;	
	FxTree* fxtree;
	GtkTreeModel* treeModel;
	ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
	args->fxmain = fxmain;
	args->sip = NULL;

	DEBUG_FOOTPRINT();

	fxtree = fxmain->mainPanel;
	fxtree->scrollWindow = gtk_scrolled_window_new(NULL , NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fxtree->scrollWindow)
								 , GTK_POLICY_NEVER , GTK_POLICY_AUTOMATIC);
	
	treeModel = fx_tree_create_model(fxmain->user);
	fxtree->treeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(treeModel));
	gtk_widget_set_usize(fxtree->treeView , 100 , 0);
	g_object_set(fxtree->treeView , "has-tooltip" , TRUE , NULL);
	g_signal_connect(fxtree->treeView , "query-tooltip" , G_CALLBACK(fx_tree_on_show_tooltip) , fxmain);

	fx_tree_create_column(fxtree->treeView , fxmain);

	g_signal_connect(fxtree->treeView
				   , "button_press_event"
				   , G_CALLBACK(fx_tree_on_rightbutton_click)
				   , fxmain);
	g_signal_connect(fxtree->treeView
				   , "row-activated"
				   , G_CALLBACK(fx_tree_on_double_click)
				   , fxmain);

	gtk_container_set_border_width(GTK_CONTAINER(fxtree->scrollWindow) , 0);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(fxtree->scrollWindow) , fxtree->treeView);
	gtk_box_pack_start(GTK_BOX(mainbox) , fxtree->scrollWindow , TRUE , TRUE , 0);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(fxtree->treeView) , FALSE);
 	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (fxtree->treeView), TRUE);
	gtk_tree_view_set_level_indentation(GTK_TREE_VIEW(fxtree->treeView) , -30);
	gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(fxtree->treeView) , TRUE);
	fetion_contact_subscribe_only(fxmain->user);
	g_thread_create(fx_tree_update_portrait_thread_func , fxmain , FALSE , NULL);

	g_thread_create(fx_main_listen_thread_func , args , FALSE , NULL);
	gtk_widget_show_all(fxtree->scrollWindow);
}
void fx_tree_free(FxTree* fxtree)
{
	DEBUG_FOOTPRINT();

	free(fxtree);
}
void fx_tree_move_to_the_first(GtkTreeModel* model , GtkTreeIter* iter)
{
	DEBUG_FOOTPRINT();

	gtk_tree_store_move_after(GTK_TREE_STORE(model) , iter , NULL);
}
void fx_tree_move_to_the_last(GtkTreeModel* model , GtkTreeIter* iter)
{
	DEBUG_FOOTPRINT();

	gtk_tree_store_move_before(GTK_TREE_STORE(model) , iter , NULL);
}
GtkTreeModel* fx_tree_create_model(User* user)
{
	Group *group = NULL;
	Contact *contact;
	Config *config = NULL;
	GdkPixbuf* pb = NULL;
	GtkTreeStore* store = NULL;
	GtkTreeIter iter;
	GtkTreeIter iter1;
	char *name;
	int count , count1;

	DEBUG_FOOTPRINT();

	group = user->groupList;
	config = user->config;	

	store = gtk_tree_store_new(COL_NUM
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
							 , G_TYPE_INT);
	
	foreach_grouplist(user->groupList , group){
		gtk_tree_store_append(store , &iter , NULL);
		gtk_tree_store_set(store , &iter
						 , G_NAME_COL 		  , group->groupname
						 , G_ID_COL           , group->groupid
						 , G_ALL_COUNT_COL    , 0
						 , G_ONLINE_COUNT_COL , 0 , -1);
	}
	foreach_contactlist(user->contactList , contact){
		pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.jpg"
				, config->iconSize , config->iconSize , NULL);
		fx_tree_get_group_iter_by_id(GTK_TREE_MODEL(store) , contact->groupid , &iter );
		if(contact->state > 0)
			gtk_tree_store_prepend(store , &iter1 , &iter);
		else
			gtk_tree_store_append(store , &iter1 , &iter);
		name = (contact->nickname == NULL || strlen(contact->localname) == 0) ? contact->nickname : contact->localname;

		gtk_tree_store_set(store , &iter1
						 , B_PIXBUF_COL 	, pb
						 , B_SIPURI_COL 	, contact->sipuri ? contact->sipuri : ""
						 , B_USERID_COL 	, contact->userId ? contact->userId : ""
						 , B_NAME_COL 	    , name ? g_markup_escape_text(name , strlen(name)) : ""
						 , B_IMPRESSION_COL , contact->impression
						 , B_PHONENUM_COL   , contact->mobileno
						 , B_DEVICE_COL     , contact->devicetype
						 , B_CRC_COL        , contact->portraitCrc
						 , B_CARRIER_COL	, contact->carrier
						 , B_STATE_COL     , contact->state
						 , B_IDENTITY_COL	, contact->identity
						 , B_SERVICESTATUS_COL	, contact->serviceStatus
						 , B_CARRIERSTATUS_COL  , contact->carrierStatus
						 , B_RELATIONSTATUS_COL , contact->relationStatus
						 , B_CARRIERSTATUS_COL  , contact->carrier
						 , B_SIZE_COL		, config->iconSize
						 , -1);
		g_object_unref(pb);
		gtk_tree_model_get(GTK_TREE_MODEL(store) , &iter
						 , G_ALL_COUNT_COL		, &count
						 , G_ONLINE_COUNT_COL	, &count1
						 , -1);
		count ++;
		if(contact->state > 0)
			count1 ++;
		gtk_tree_store_set(store , &iter
						 , G_ALL_COUNT_COL		, count
						 , G_ONLINE_COUNT_COL	, count1
						 , -1);
	}
	return GTK_TREE_MODEL(store);


}
void fx_tree_get_group_iter_by_id(GtkTreeModel* model , int id , GtkTreeIter* iter)
{
	int idt;

	if(gtk_tree_model_get_iter_root(model , iter)){
		do{
			gtk_tree_model_get(model , iter , G_ID_COL , &idt , -1);
			if(idt == id)
				break;
		}while(gtk_tree_model_iter_next(model , iter));
	}
}
int fx_tree_get_buddy_iter_by_userid(GtkTreeModel* model , const char* userid , GtkTreeIter* iter)
{
	char *id = NULL;
	GtkTreeIter pos;

	if(gtk_tree_model_get_iter_root(model , &pos)){
		do{
			if(gtk_tree_model_iter_children(model , iter , &pos)){
				do{
					gtk_tree_model_get(model , iter , B_USERID_COL , &id , -1);
					if(id == NULL)
						continue;
					if(strcmp(userid , id) == 0)
						return 1;
				}while(gtk_tree_model_iter_next(model , iter));
			}
		}while(gtk_tree_model_iter_next(model , &pos));
	}
	return -1;
}
static void fx_tree_on_hightlight_clicked(GtkWidget *widget , gpointer data)
{
	if(all_light)
		all_light = 0;
	else
		all_light = 1;
}
void fx_tree_create_buddy_menu(FxMain* fxmain , GtkWidget* tree
		, GtkTreePath* path , GdkEventButton* event , GtkTreeIter iter)
{
	char *sipuri , *groupname , *userid , *mobileno , *carrier;
	int groupid , iconsize;
	int serviceStatus , relationStatus , carrierStatus;
	GtkWidget* menu = NULL;
	GtkWidget* groupSubmenu = NULL;
	GtkWidget* moveItem = NULL;
	GtkTreeSelection* selection = NULL;
	GtkTreeModel* model = NULL;
	GtkTreeIter groupiter;
	Args *profileargs , *moveargs , *chatargs;
	FxTree* fxtree = fxmain->mainPanel;

	DEBUG_FOOTPRINT();

	menu = gtk_menu_new();
	groupSubmenu = gtk_menu_new();
	/*select row*/
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(fxtree->treeView));
	if(gtk_tree_selection_count_selected_rows(selection) <= 1)
	{
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(fxtree->treeView));
		gtk_tree_selection_unselect_all(selection);
		gtk_tree_selection_select_path(selection , path);

		gtk_tree_model_get(GTK_TREE_MODEL(model) , &iter
						 , B_SIPURI_COL			 , &sipuri
						 , B_PHONENUM_COL		 , &mobileno
						 , B_USERID_COL		     , &userid
						 , B_SERVICESTATUS_COL	 , &serviceStatus
						 , B_RELATIONSTATUS_COL  , &relationStatus
						 , B_CARRIER_COL		 , &carrier
						 , B_CARRIERSTATUS_COL	 , &carrierStatus
						 , B_SIZE_COL			 , &iconsize
						 , -1);
		profileargs = fx_args_new(fxmain , iter , userid , 0);
		chatargs = fx_args_new(fxmain , iter , sipuri , 0);
		fx_tree_create_menu("发送即时消息" , SKIN_DIR"im.gif" , menu
						  , ((serviceStatus == BASIC_SERVICE_ABNORMAL
								  && (carrierStatus == CARRIER_STATUS_CLOSED || (strlen(carrier)!= 0 && strlen(mobileno) == 0)))
						  || relationStatus == RELATION_STATUS_UNAUTHENTICATED)
						  ? FALSE : TRUE
						  , fx_tree_on_chatmenu_clicked , chatargs);

		fx_tree_create_menu("查看好友信息" , SKIN_DIR"profile.gif"
						, menu , TRUE , fx_tree_on_profilemenu_clicked , profileargs);
#if 0
		fx_tree_create_menu("发送文件" , SKIN_DIR"sendfile.png"
						, menu , TRUE , fx_tree_on_sendfile_clicked , profileargs);
#endif
		fx_tree_create_menu("查看聊天记录" , SKIN_DIR"history.png"
						, menu , TRUE ,  fx_tree_on_historymenu_clicked , profileargs);

		fx_tree_create_menu("刷新好友信息" , SKIN_DIR"refresh.gif"
						, menu , TRUE ,  fx_tree_on_reload_clicked , profileargs);

		fx_tree_create_menu("修改备注名称" , SKIN_DIR"rename.gif"
						, menu , TRUE , fx_tree_on_editmenu_clicked , profileargs);

		fx_tree_create_menu( !all_light ? "所有好友高亮" : "在线友好高亮" , SKIN_DIR"sendfile.png"
						, menu , TRUE , fx_tree_on_hightlight_clicked , NULL);

		fx_tree_create_menu(iconsize > 30 ? "使用小图标" : "使用大图标"
						, SKIN_DIR"bigimage.png" , menu , TRUE , fx_tree_on_iconchange_clicked , fxmain);

		fx_tree_create_menu("删除该好友" , SKIN_DIR"delete.png"
						, menu , TRUE , fx_tree_on_deletemenu_clicked , profileargs);

		gtk_tree_model_iter_parent(model , &groupiter , &iter);
		gtk_tree_model_get(model , &groupiter , G_ID_COL , &groupid , -1);

		if(groupid != BUDDY_LIST_STRANGER)
		{

			moveItem = fx_tree_create_menu("将好友移动到" , SKIN_DIR"move.gif"
							, menu , TRUE , NULL , NULL);
			/*add group child menu*/
			gtk_tree_model_get_iter_root(GTK_TREE_MODEL(model) , &groupiter);
			do
			{
				gtk_tree_model_get(GTK_TREE_MODEL(model) , &groupiter
								 , G_NAME_COL 			 , &groupname
								 , G_ID_COL 			 , &groupid , -1);
				moveargs = fx_args_new(fxmain , iter , userid , groupid);
				fx_tree_create_menu(groupname 	  , SKIN_DIR"group.png"
								  , groupSubmenu , TRUE , fx_tree_on_movemenu_clicked
								  , moveargs);
				free(groupname);
			}
			while(gtk_tree_model_iter_next(GTK_TREE_MODEL(model) , &groupiter));
			gtk_menu_item_set_submenu(GTK_MENU_ITEM(moveItem) , groupSubmenu);
		}
	}
	gtk_widget_show_all(menu);
	free(userid);
	free(sipuri);
	free(mobileno);
	free(carrier);
	gtk_menu_popup(GTK_MENU(menu) , NULL , NULL , NULL , NULL 
			, (event != NULL) ? event->button : 0 , gdk_event_get_time((GdkEvent*)event));
}

void fx_tree_create_group_menu(FxMain* fxmain , GtkWidget* tree
		, GtkTreePath* path , GdkEventButton* event , GtkTreeIter iter)
{
	GtkWidget* menu = NULL;
	Args* args = fx_args_new(fxmain , iter ,  NULL , 0);
	int count , groupid;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));

	DEBUG_FOOTPRINT();

	menu = gtk_menu_new();
	gtk_tree_model_get(model ,  &iter
					 , G_ALL_COUNT_COL , &count
					 , G_ID_COL		   , &groupid , -1);
	fx_tree_create_menu("添加新分组" , SKIN_DIR"myselfsms.png"
					, menu , TRUE , fx_tree_on_gaddmenu_clicked , fxmain);
	fx_tree_create_menu("修改分组名称" , SKIN_DIR"rename.gif"
					, menu , (groupid == BUDDY_LIST_NOT_GROUPED || groupid == BUDDY_LIST_STRANGER) ? FALSE : TRUE
					, fx_tree_on_geditmenu_clicked , args);
	fx_tree_create_menu("删除该分组" , SKIN_DIR"delete.png"
					, menu , (count == 0 && groupid != BUDDY_LIST_NOT_GROUPED && groupid != BUDDY_LIST_STRANGER) ? TRUE : FALSE
					, fx_tree_on_gdeletemenu_clicked , args);

	gtk_widget_show_all(menu);
	gtk_menu_popup(GTK_MENU(menu) , NULL , NULL , NULL , NULL 
			, (event != NULL) ? event->button : 0 , gdk_event_get_time((GdkEvent*)event));
}
GtkWidget* fx_tree_create_menu(const char* name
							 , const char* iconpath
							 , GtkWidget* parent
							 , gboolean sensitive
							 , void (*func)(GtkWidget* item , gpointer data)
							 , gpointer data)
{
	GtkWidget* item = gtk_image_menu_item_new_with_label(name);
	GtkWidget* img = gtk_image_new_from_file(iconpath);

	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item) , img);
	gtk_menu_shell_append(GTK_MENU_SHELL(parent) , item);
	if(sensitive == FALSE)
	{
		gtk_widget_set_sensitive(item , FALSE);
		return item;
	}
	if(func != NULL)
		g_signal_connect(item , "activate" , G_CALLBACK(func) , data);
	return item;
}

void fx_tree_add_new_buddy(FxMain* fxmain , Contact* contact)
{
	GtkTreeView *tree = GTK_TREE_VIEW(fxmain->mainPanel->treeView);
	GtkTreeModel *model = gtk_tree_view_get_model(tree);
	GtkTreeIter oIter , nIter;
	GdkPixbuf *pb = NULL;
	Config *config = fxmain->user->config;
	int groupid;

	DEBUG_FOOTPRINT();

	gtk_tree_model_get_iter_root(model , &oIter);
	do
	{
		gtk_tree_model_get(model		, &oIter
						 , G_ID_COL		, &groupid
						 , -1);
		if(groupid == contact->groupid)
		{
			pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.jpg" , config->iconSize , config->iconSize , NULL);
			gtk_tree_store_append(GTK_TREE_STORE(model) , &nIter , &oIter);
			gtk_tree_store_set(GTK_TREE_STORE(model), &nIter
							, B_PIXBUF_COL		, pb
							, B_NAME_COL		, contact->localname
							, B_SIPURI_COL		, contact->sipuri
							, B_USERID_COL		, contact->userId
							, B_SERVICESTATUS_COL	, contact->serviceStatus
							, B_RELATIONSTATUS_COL  , contact->relationStatus
							, B_CARRIERSTATUS_COL   , contact->carrierStatus
							, B_CARRIER_COL , "CMCC"
							, -1);		
		}
	}
	while(gtk_tree_model_iter_next(model , &oIter));
}

void fx_tree_create_column(GtkWidget* tree , FxMain* fxmain)
{
	GtkCellRenderer* renderer;
	GtkTreeViewColumn *col , *col0;

	DEBUG_FOOTPRINT();

	renderer = gtk_cell_renderer_pixbuf_new();
	col = gtk_tree_view_column_new(); 
    gtk_tree_view_column_pack_start(col, renderer , FALSE);
	gtk_tree_view_column_add_attribute(col, renderer, "pixbuf", B_PIXBUF_COL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col);
	gtk_tree_view_column_set_cell_data_func(col
										  , renderer
										  , fx_tree_pixbuf_cell_data_func
										  , fxmain
										  , NULL);

    renderer = gtk_cell_renderer_text_new();
    col0 = gtk_tree_view_column_new(); 
    gtk_tree_view_column_pack_start(col0, renderer , FALSE);
	gtk_tree_view_column_set_cell_data_func(col0
										  , renderer
										  , fx_tree_text_cell_data_func
										  , NULL
										  , NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col0);

}
void fx_tree_pixbuf_cell_data_func(GtkTreeViewColumn *col
								 , GtkCellRenderer *renderer
								 , GtkTreeModel *model
								 , GtkTreeIter *iter
								 , gpointer user_data)
{
	int state;
	GtkTreePath* path = gtk_tree_model_get_path(model , iter);

	if(gtk_tree_path_get_depth(path) > 1)
	{
		gtk_tree_model_get(model , iter , B_STATE_COL , &state , -1);
		g_object_set(renderer , "visible" , TRUE , NULL);
		g_object_set(renderer , "sensitive" , all_light ? TRUE : (state > 0 ? TRUE : FALSE) , NULL);

	}
	else
	{
		g_object_set(renderer , "visible" , FALSE , NULL);
	}
	gtk_tree_path_free(path);
}
void fx_tree_text_cell_data_func(GtkTreeViewColumn *col,
	   							 GtkCellRenderer   *renderer,
								 GtkTreeModel      *model,
								 GtkTreeIter       *iter,
								 gpointer           user_data)
{
	GtkTreePath* path = gtk_tree_model_get_path(model , iter);
	char text[1024];
	/*contact data*/
	char *name , *impression , *sipuri , *sid , *stateStr , *mobileno , *device , *carrier;
	char stateStr1[96];
	char statusStr[256];
	int presence , status , size;
	int carrierStatus , relationStatus , serviceStatus;
	/*buddylist data*/
	int allCount , onlineCount;
	char* buddylistName = NULL;

	/* render friend list text*/
	if(gtk_tree_path_get_depth(path) > 1)
	{
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
			sprintf(statusStr , "<span color='#d4b4b4'>[等待验证]</span>");
		}else if(serviceStatus == BASIC_SERVICE_ABNORMAL){
			if(carrierStatus == CARRIER_STATUS_CLOSED){
				sprintf(statusStr , "<span color='#d4b4b4'>[已关闭飞信服务]</span>");
			}else{
				if(carrier != NULL || strlen(carrier) != 0){
					sprintf(statusStr , "<span color='#d4b4b4'>[短信在线]</span>");
					if(mobileno == NULL || strlen(mobileno) == 0){
						sprintf(statusStr , "<span color='#d4b4b4'>[已关闭飞信服务]</span>");
					}
				}else{
					sprintf(statusStr , "<span color='#d4b4b4'>[已关闭飞信服务]</span>");
				}
			}
		}else if(carrierStatus == CARRIER_STATUS_DOWN){
			if(strlen(carrier) != 0){
				sprintf(statusStr , "<span color='#d4b4b4'>[停机]</span>");
			}
		}
		if(sipuri == NULL){
			printf("NULL\n");
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
						   , (strlen(statusStr) == 0 ? (presence == 0 ? "" : stateStr1) : statusStr)
						   , (device != NULL && strcmp(device , "PC") != 0) ? "[手机登录]" : "" , sid
						   , impression == NULL ? "" : g_markup_escape_text(impression , strlen(impression)));
		}
		else
		{
			sprintf(text , "<b>%s</b>%s%s"
						   "(%s) \n <span color='#838383'>%s</span>"
						   , name == NULL ? "" : g_markup_escape_text(name , strlen(name))
						   , (strlen(statusStr) == 0 ? (presence == 0 ? "" : stateStr1) : statusStr)
						   , (device != NULL &&strcmp(device , "PC") != 0) ? "[手机登录]" : "" , sid
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
	}
	/*render group list text*/
	else
	{
		bzero(text , sizeof(text));
		gtk_tree_model_get(model              , iter
						 , G_NAME_COL         , &buddylistName
						 , G_ALL_COUNT_COL    , &allCount
						 , G_ONLINE_COUNT_COL , &onlineCount ,-1);
		sprintf(text , "%s [%d/%d]" , buddylistName , onlineCount , allCount );
		g_object_set(renderer , "text" , text , NULL);
		free(buddylistName);
	}
	gtk_tree_path_free(path);
}
void* fx_tree_update_portrait_thread_func(void* data)
{
	FxMain* fxmain = (FxMain*)data;
	GtkTreeModel* model = NULL;
	GtkTreeIter iter , pos;
	GdkPixbuf* pb;
	User* user = fxmain->user;
	Config* config = user->config;
	int size;
	char portraitPath[256] , *sipuri , *sid;

	DEBUG_FOOTPRINT();
	sleep(1);

	fetion_user_save(user);
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(fxmain->mainPanel->treeView));
	gtk_tree_model_get_iter_root(model , &iter);
	do{
		if(gtk_tree_model_iter_children(model , &pos , &iter))
		{
			do
			{
				bzero(portraitPath , sizeof(portraitPath));
				gtk_tree_model_get(model , &pos
								 , B_SIPURI_COL , &sipuri
								 , B_SIZE_COL   , &size  ,-1);
				sid = fetion_sip_get_sid_by_sipuri(sipuri);
				sprintf(portraitPath , "%s/%s.jpg" , config->iconPath , sid);
				pb = gdk_pixbuf_new_from_file_at_size(portraitPath , size , size , NULL);
				if(pb == NULL){
					fetion_user_download_portrait(user , sipuri);
					pb = gdk_pixbuf_new_from_file_at_size(portraitPath , size , size , NULL);
				}
				if(pb != NULL){
					gdk_threads_enter();
					gtk_tree_store_set(GTK_TREE_STORE(model) , &pos , B_PIXBUF_COL , pb , -1);
					g_object_unref(pb);
					gdk_threads_leave();
				}
				gdk_threads_enter();
				gtk_tree_store_set(GTK_TREE_STORE(model) , &pos , B_IMAGE_CHANGED_COL , IMAGE_NOT_CHANGED , -1);
				gdk_threads_leave();
				free(sipuri);
				free(sid);
			}
			while(gtk_tree_model_iter_next(model , &pos));
		}
	}
	while(gtk_tree_model_iter_next(model , &iter));
	return NULL;
}
void fx_tree_on_double_click(GtkTreeView *treeview
		, GtkTreePath *path , GtkTreeViewColumn  *col , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	GtkTreeIter iter;
	gtk_tree_model_get_iter(model , &iter , path);
	int depth = gtk_tree_path_get_depth(path);
	char *sipuri , *mobileno , *carrier;
	int serviceStatus , relationStatus , carrierStatus;

	DEBUG_FOOTPRINT();

	if(depth > 1)
	{
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
			fx_util_popup_warning(fxmain , "由于对方尚未成为你的飞信好友"
					",所以您不能给对方发送消息");
			return;
		}
		if(serviceStatus == BASIC_SERVICE_ABNORMAL && 
			(carrierStatus == CARRIER_STATUS_CLOSED ||
			 (strlen(carrier) != 0 && strlen(mobileno) == 0)))
		{
			fx_util_popup_warning(fxmain , "该用户已关闭飞信服务，"
					"不能给他发消息");
			return;
		}
		fx_main_create_chat_window(fxmain , sipuri);
		free(sipuri);
		free(mobileno);
		free(carrier);
	}
	else
	{
		if(gtk_tree_view_row_expanded(treeview , path))
			gtk_tree_view_collapse_row(treeview , path);
		else
			gtk_tree_view_expand_row(treeview , path , TRUE);
	}
}
gboolean fx_tree_on_rightbutton_click(GtkWidget* tree
		, GdkEventButton* event , gpointer data)
{
	GtkTreeIter iter;
	GtkTreePath* path = NULL;
	GtkTreeModel* model = NULL;
	FxMain* fxmain = NULL;
	FxTree* fxtree = NULL;
	int depth;

	if(event->type == GDK_BUTTON_PRESS && event->button == 3)
	{

		DEBUG_FOOTPRINT();
		fxmain = (FxMain*)data;
		fxtree = fxmain->mainPanel;
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(fxtree->treeView));
		gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(fxtree->treeView) 
				, (gint)event->x , (gint)event->y , &path , NULL , NULL , NULL);
		if(path == NULL)
			return FALSE;

		gtk_tree_model_get_iter(GTK_TREE_MODEL(model) , &iter , path);

		depth = gtk_tree_path_get_depth(path);
		if(depth == 2){
			fx_tree_create_buddy_menu(fxmain , fxtree->treeView , path , event , iter);
		}else{
			fx_tree_create_group_menu(fxmain , fxtree->treeView , path , event , iter);
		}
		gtk_tree_path_free(path);
		return TRUE;
	}
	return FALSE;
}
void fx_tree_on_chatmenu_clicked(GtkWidget* widget , gpointer data)
{
	Args* args = (Args*)data;
	FxMain* fxmain = args->fxmain;
	char* sipuri = args->s;

	DEBUG_FOOTPRINT();

	fx_main_create_chat_window(fxmain , sipuri);
	free(args);
}

void fx_tree_on_profilemenu_clicked(GtkWidget* widget , gpointer data)
{
	Args* args = (Args*)data;
	FxMain* fxmain = args->fxmain;
	char* userid = args->s;
	FxProfile* fxprofile = fx_profile_new(fxmain , userid);

	DEBUG_FOOTPRINT();

	fx_profile_initialize(fxprofile);
	gtk_dialog_run(GTK_DIALOG(fxprofile->dialog));
	gtk_widget_destroy(fxprofile->dialog);
	free(fxprofile);
	free(args);
}

static void* fx_tree_on_send_thread(void *data){

	struct sendargs{
		FxMain *fxmain;FxShare *share;
	} *threadargs = (struct sendargs*)data;

	Conversation *conv = NULL;
	ThreadArgs *targs = NULL;
	TimeOutArgs *oargs = NULL;
	FxMain *fxmain = threadargs->fxmain;
	FxShare *fxshare = threadargs->share;
	User *user = fxmain->user;
	Share *share = fxshare->share;
	char *sipuri = fxshare->contact->sipuri;
	FetionSip *sip ;
	FxList *fxlist;

	DEBUG_FOOTPRINT();

	conv = fetion_conversation_new(user , sipuri , NULL);
	sip = fx_list_find_sip_by_sipuri(fxmain->slist , sipuri);
	if(sip != NULL){
		conv->currentSip = sip;
	}else{
		if(fetion_conversation_invite_friend(conv) > 0){
			targs = (ThreadArgs*)malloc(sizeof(ThreadArgs));
			targs->fxmain = fxmain;
			targs->sip = conv->currentSip;

			fxlist = fx_list_new(conv->currentSip);
			fx_list_append(fxmain->slist , fxlist);

			g_thread_create(fx_main_listen_thread_func , targs , FALSE , NULL);

			debug_info("Start periodically sending keep alive request");
			oargs = timeout_args_new(fxmain , conv->currentSip , sipuri);
			fxlist = fx_list_new(oargs);
			fx_list_append(fxmain->tlist , fxlist);
			g_timeout_add_seconds(120 , (GSourceFunc)fx_main_chat_keep_alive_func , oargs);
		}
	}
	sip = conv->currentSip;
	free(conv);

	fetion_share_request(sip , share);

	if(share == NULL){
		return NULL;
	}

	gdk_threads_enter();
	gtk_label_set_markup(GTK_LABEL(fxshare->iLabel)
			, "<span color='#838383'>连接已经建立，正在等待对方接收文件...</span>");
	gdk_threads_leave();

	return NULL;
}

void fx_tree_on_sendfile_clicked(GtkWidget* widget , gpointer data)
{
	Args *args = (Args*)data;
	FxMain *fxmain = args->fxmain;
	FxTree *fxtree = fxmain->mainPanel;
	FxShare *fxshare = NULL;
	GtkTreeView *treeview = GTK_TREE_VIEW(fxtree->treeView);
	GtkTreeModel *model = gtk_tree_view_get_model(treeview);
	GtkTreeIter iter = args->iter;

	GtkWidget *filechooser = NULL;
	char *filename = NULL;
	char *sipuri = NULL;
	Share *share = NULL;
	FetionSip *sip = NULL;
	FxList *fxlist = NULL;
	GThread thread;
	long long filesize;
	char text[1024];
	int response = 0;

	struct sendargs{
		FxMain *fxmain;FxShare *fxshare;
	} *threadargs = (struct sendargs*)malloc(sizeof(struct sendargs));

	filechooser = gtk_file_chooser_dialog_new("请选择要发送的文件"
							   , NULL , GTK_FILE_CHOOSER_ACTION_OPEN
							   , "确定" , 1 , "取消" , 2 , NULL);
	response = gtk_dialog_run(GTK_DIALOG(filechooser));

	if(response == 1){
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser));
	}else{
		return;
	}
	gtk_widget_destroy(filechooser);
	
	filesize = fetion_share_get_filesize(filename);
	if(filesize == -1){
		fx_util_popup_warning(fxmain , "无法获取文件信息");
		return ;
	}
	if(filesize > MAX_FILE_SIZE){
		bzero(text , sizeof(text));
		sprintf(text , "文件过大，飞信限制最大传输文件大小为%d个字节" , MAX_FILE_SIZE);
		fx_util_popup_warning(fxmain , text);
		return;
	}

	gtk_tree_model_get(model , &iter , B_SIPURI_COL , &sipuri , -1);

	share = fetion_share_new_with_path(sipuri , filename);
	fxshare = fx_share_new(fxmain , sipuri);
	free(sipuri);

	fxshare->share = share;
	fx_share_initialize(fxshare);
	fxlist = fx_list_new(fxshare);
	fx_list_append(fxmain->shlist , fxlist);

	threadargs->fxmain = fxmain;
	threadargs->fxshare = fxshare;
	g_thread_create(fx_tree_on_send_thread , threadargs , FALSE , NULL);
	gtk_dialog_run(GTK_DIALOG(fxshare->dialog));

	gtk_widget_destroy(fxshare->dialog);
}

void fx_tree_on_historymenu_clicked(GtkWidget* widget , gpointer data)
{
	Args* args = (Args*)data;
	FxMain* fxmain = args->fxmain;
	char* userid = args->s;
	GtkTreeIter iter = args->iter;
	char* name = NULL;
	FxHistory* fxhistory = NULL;

	GtkWidget* tree = fxmain->mainPanel->treeView;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));

	DEBUG_FOOTPRINT();

	gtk_tree_model_get(model , &iter
					 , B_NAME_COL , &name , -1);
	fxhistory = fx_history_new(fxmain , userid , name);
	fx_history_initialize(fxhistory);
	gtk_dialog_run(GTK_DIALOG(fxhistory->dialog));
	gtk_widget_destroy(fxhistory->dialog);
	free(fxhistory);
	free(args);
}

void fx_tree_on_editmenu_clicked(GtkWidget* widget , gpointer data)
{
	Args* args = (Args*)data;
	FxMain* fxmain = args->fxmain;
	char* userid = args->s;
	int ret;
	FxEdit* fxedit = fx_edit_new(fxmain , args->iter , userid);

	DEBUG_FOOTPRINT();

	fx_edit_initialize(fxedit);
	ret = gtk_dialog_run(GTK_DIALOG(fxedit->dialog));
	gtk_widget_destroy(fxedit->dialog);
	free(fxedit);
	free(args);
}
void* save_contact(void* data)
{
	User* user = (User*)data;

	DEBUG_FOOTPRINT();

	fetion_contact_save(user);
	return NULL;
}

void fx_tree_on_deletemenu_clicked(GtkWidget* widget , gpointer data)
{
	Args* args = (Args*)data;
	FxMain* fxmain = args->fxmain;
	char* userid = args->s;
	GtkTreeIter iter = args->iter;
	GtkWidget *dialog;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(fxmain->mainPanel->treeView));

	DEBUG_FOOTPRINT();

	dialog = gtk_message_dialog_new(GTK_WINDOW(fxmain->window),
									GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_QUESTION,
									GTK_BUTTONS_YES_NO,
									"确定要删除这个好友吗?");
	gtk_window_set_title(GTK_WINDOW(dialog), "删除好友");
	int result = gtk_dialog_run(GTK_DIALOG(dialog));
	if(result == GTK_RESPONSE_YES)
	{
		fetion_contact_delete_buddy(fxmain->user , userid);
		gtk_tree_store_remove(GTK_TREE_STORE(model) , &iter);
		g_thread_create(save_contact , fxmain->user , FALSE , NULL);
	}
	free(args);
	gtk_widget_destroy(dialog);

}

void* fx_tree_reload_thread(void* data)
{
	Args *args = (Args*)data;
	FxMain *fxmain = args->fxmain;
	GtkTreeIter iter = args->iter;
	GtkTreeView *tree = GTK_TREE_VIEW(fxmain->mainPanel->treeView);
	GtkTreeModel *model = gtk_tree_view_get_model(tree);
	char *userid = NULL;
	char *sipuri = NULL;
	char *sid = NULL;
	char *name = NULL;
	char portraitPath[1024];
	GdkPixbuf *pb = NULL;
	Contact *contact = NULL;
	Config *config = fxmain->user->config;

	DEBUG_FOOTPRINT();

	gtk_tree_model_get(model 		, &iter
					 , B_USERID_COL , &userid
					 , B_SIPURI_COL , &sipuri
					 , -1);

	contact = fetion_contact_get_contact_info(fxmain->user , userid);
	
	debug_info("Updating information of user %s" , userid);
	free(userid);

	fetion_user_download_portrait(fxmain->user , sipuri);
	sid = fetion_sip_get_sid_by_sipuri(sipuri);
	free(sipuri);

	bzero(portraitPath , sizeof(portraitPath));
	sprintf(portraitPath , "%s/%s.jpg" , config->iconPath , sid);
	pb = gdk_pixbuf_new_from_file_at_size(portraitPath , 25 , 25 , NULL);
	if(pb == NULL)
		pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.jpg" , 25 , 25 , NULL);

	name = (contact->nickname == NULL || strlen(contact->localname) == 0)
			? contact->nickname : contact->localname;
	gtk_tree_store_set(GTK_TREE_STORE(model) , &iter
					 , B_PIXBUF_COL 	, pb
					 , B_SIPURI_COL 	, contact->sipuri
					 , B_USERID_COL 	, contact->userId
					 , B_NAME_COL 	    , g_markup_escape_text(name , strlen(name)) 
					 , B_IMPRESSION_COL , contact->impression
					 , B_PHONENUM_COL   , contact->mobileno
					 , B_DEVICE_COL     , contact->devicetype
					 , B_CRC_COL        , contact->portraitCrc
					 , B_IDENTITY_COL	, contact->identity
					 , -1);
	g_object_unref(pb);
	return NULL;

}

void fx_tree_on_reload_clicked(GtkWidget* widget , gpointer data)
{
	DEBUG_FOOTPRINT();

	g_thread_create(fx_tree_reload_thread , data , FALSE , NULL);
}

void fx_tree_on_iconchange_clicked(GtkWidget* widget , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	FxTree *fxtree = fxmain->mainPanel;
	Config *config = fxmain->user->config;
	GtkTreeView* tree = GTK_TREE_VIEW(fxtree->treeView);
	GtkTreeModel* model = gtk_tree_view_get_model(tree);
	GtkTreeIter iter;
	GtkTreeIter pointer;
	GdkPixbuf* pb = NULL;
	char *sipuri = NULL;
	char *sid = NULL;
	char path[128];

	DEBUG_FOOTPRINT();

	gtk_tree_model_get_iter_root(model , &iter);
	if(config->iconSize > 30)
	{
		config->iconSize = 25;
		fetion_config_save(fxmain->user);
		debug_info("Changed to use small icon , size : 25px");
	}
	else
	{
		config->iconSize = 50;
		fetion_config_save(fxmain->user);
		debug_info("Changed to use large icon , size : 50px");
	}
	do
	{
		if(gtk_tree_model_iter_children(model , &pointer , &iter))
		{
			do
			{
				if(config->iconSize < 30)
				{
					gtk_tree_model_get(model , &pointer 
									 , B_PIXBUF_COL , &pb
									 , -1);
				}
				else
				{
					gtk_tree_model_get(model		 , &pointer
									 , B_SIPURI_COL  , &sipuri
									 , -1);
					bzero(path , sizeof(path));
					sid = fetion_sip_get_sid_by_sipuri(sipuri);
					sprintf(path , "%s/%s.jpg" , config->iconPath , sid);
					free(sid);
					pb = gdk_pixbuf_new_from_file(path , NULL);
					if(pb == NULL)
						pb = gdk_pixbuf_new_from_file(SKIN_DIR"fetion.jpg" , NULL);
				}
				pb = gdk_pixbuf_scale_simple(pb , config->iconSize , config->iconSize , GDK_INTERP_NEAREST);
				gtk_tree_store_set(GTK_TREE_STORE(model) , &pointer
								 , B_PIXBUF_COL , pb
								 , B_SIZE_COL   , config->iconSize
								 , -1);
				g_object_unref(pb);
			}
			while(gtk_tree_model_iter_next(model , &pointer));
		}
	}
	while(gtk_tree_model_iter_next(model , &iter));
}
void fx_tree_on_gaddmenu_clicked(GtkWidget* widget , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxAddGroup* fxaddgroup = fx_add_group_new(fxmain);

	DEBUG_FOOTPRINT();

	fx_add_group_initialize(fxaddgroup);
	gtk_dialog_run(GTK_DIALOG(fxaddgroup->dialog));
	gtk_widget_destroy(fxaddgroup->dialog);
	fx_add_group_free(fxaddgroup);
}

void fx_tree_on_gdeletemenu_clicked(GtkWidget* widget , gpointer data)
{
	Args* args = (Args*)data;
	FxMain* fxmain = args->fxmain;
	GtkWidget* tree = fxmain->mainPanel->treeView;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
	int groupid;
	GtkWidget *dialog = NULL;

	DEBUG_FOOTPRINT();

	gtk_tree_model_get(model , &(args->iter) , G_ID_COL , &groupid , -1);
	dialog = gtk_message_dialog_new(GTK_WINDOW(fxmain->window),
									GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_QUESTION,
									GTK_BUTTONS_YES_NO,
									"确定要删除这个分组吗?");
	gtk_window_set_title(GTK_WINDOW(dialog), "删除分组");
	int result = gtk_dialog_run(GTK_DIALOG(dialog));
	if(result == GTK_RESPONSE_YES)
	{
		if(fetion_buddylist_delete(fxmain->user , groupid) != -1)
		{
			gtk_tree_store_remove(GTK_TREE_STORE(model) , &args->iter);
			fetion_buddylist_save(fxmain->user);
		}
	}
	free(args);
	gtk_widget_destroy(dialog);
}
void fx_tree_on_geditmenu_clicked(GtkWidget* widget , gpointer data)
{
	Args* args = (Args*)data;
	FxMain* fxmain = args->fxmain;
	GtkWidget* tree = fxmain->mainPanel->treeView;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
	int groupid;
	FxGEdit* fxgedit = NULL;

	DEBUG_FOOTPRINT();

	gtk_tree_model_get(model , &(args->iter) , G_ID_COL , &groupid , -1);

	fxgedit = fx_gedit_new(fxmain , args->iter , groupid);
	fx_gedit_initialize(fxgedit);
	gtk_dialog_run(GTK_DIALOG(fxgedit->dialog));
	gtk_widget_destroy(fxgedit->dialog);
	free(args);
}
void fx_tree_on_movemenu_clicked(GtkWidget* widget , gpointer data)
{
	Args *args = (Args*)data;
	int groupid = args->i;
	char* userid = args->s;
	GtkTreeIter iter = args->iter;
	GtkTreeIter newIter , parentIter , oldParentIter;
	FxMain* fxmain = args->fxmain;
	GtkWidget* tree = fxmain->mainPanel->treeView;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
	char *name , *sipuri , *impression , *mobileno , *device , *crc , *carrier;
	int state , identity , size , imageChanged ;
	int serviceStatus , carrierStatus , relationStatus;
	int gid , oldOnlineCount , oldAllCount , newOnlineCount , newAllCount;
	GdkPixbuf* pb = NULL;

	DEBUG_FOOTPRINT();

	if(fetion_contact_move_to_group(fxmain->user , userid , groupid) > 0)
	{
		gtk_tree_model_get_iter_root(model , &parentIter);
		do
		{
			gtk_tree_model_get(model , &parentIter
							 , G_ID_COL		 		, &gid
							 , G_ALL_COUNT_COL		, &newAllCount
							 , G_ONLINE_COUNT_COL	, &newOnlineCount
							 , -1);
			if(gid == groupid)
				break;
		}
		while(gtk_tree_model_iter_next(model , &parentIter));

		gtk_tree_model_iter_parent(model , &oldParentIter , &iter);
		gtk_tree_model_get(model , &oldParentIter
						 , G_ID_COL		 		, &gid
						 , G_ALL_COUNT_COL		, &oldAllCount
						 , G_ONLINE_COUNT_COL	, &oldOnlineCount
						 , -1);
		gtk_tree_model_get(model , &iter
						 , B_PIXBUF_COL 	, &pb
						 , B_SIPURI_COL 	, &sipuri
						 , B_USERID_COL 	, &userid
						 , B_NAME_COL 	    , &name
						 , B_IMPRESSION_COL , &impression
						 , B_PHONENUM_COL   , &mobileno
						 , B_DEVICE_COL     , &device
						 , B_CARRIER_COL	, &carrier
						 , B_CARRIERSTATUS_COL , &carrierStatus
						 , B_RELATIONSTATUS_COL , &relationStatus
						 , B_SERVICESTATUS_COL , &serviceStatus
						 , B_CRC_COL        , &crc
						 , B_STATE_COL      , &state
						 , B_IDENTITY_COL	, &identity
						 , B_SIZE_COL		, &size
						 , B_IMAGE_CHANGED_COL , &imageChanged
						 , -1);

		gtk_tree_store_remove(GTK_TREE_STORE(model) , &iter);
		oldAllCount --;
		gtk_tree_store_set(GTK_TREE_STORE(model) , &oldParentIter
						 , G_ALL_COUNT_COL	 	 , oldAllCount
						 , -1);
		newAllCount ++;
		gtk_tree_store_set(GTK_TREE_STORE(model) , &parentIter
						 , G_ALL_COUNT_COL		 , newAllCount
						 , -1);
		if(state <= 0)
		{
			gtk_tree_store_append(GTK_TREE_STORE(model) , &newIter , &parentIter);
		}
		else
		{
			gtk_tree_store_prepend(GTK_TREE_STORE(model) , &newIter , &parentIter);
			oldOnlineCount --;
			gtk_tree_store_set(GTK_TREE_STORE(model) , &oldParentIter
							 , G_ONLINE_COUNT_COL	 , oldOnlineCount
							 , -1);
			newOnlineCount ++;
			gtk_tree_store_set(GTK_TREE_STORE(model) , &parentIter
							 , G_ONLINE_COUNT_COL	 , newOnlineCount
							 , -1);
		}

		gtk_tree_store_set(GTK_TREE_STORE(model) , &newIter
						 , B_PIXBUF_COL 	, pb
						 , B_SIPURI_COL 	, sipuri
						 , B_USERID_COL 	, userid
						 , B_NAME_COL 	    , name
						 , B_IMPRESSION_COL , impression
						 , B_PHONENUM_COL   , mobileno
						 , B_CARRIER_COL    , carrier
						 , B_DEVICE_COL     , device
						 , B_CRC_COL        , crc
						 , B_SERVICESTATUS_COL , serviceStatus
						 , B_CARRIERSTATUS_COL , carrierStatus
						 , B_RELATIONSTATUS_COL , relationStatus
						 , B_STATE_COL     , state
						 , B_IDENTITY_COL	, identity
						 , B_IMAGE_CHANGED_COL , imageChanged
						 , B_SIZE_COL		, size
						 , -1);
		free(sipuri);
		free(userid);
		free(name);
		free(impression);
		free(mobileno);
		free(carrier);
		free(device);
		free(crc);
	}
	free(args);
}
gboolean fx_tree_on_show_tooltip(GtkWidget* widget 
								  , int x
								  , int y
								  , gboolean keybord_mode
								  , GtkTooltip* tip
								  , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	Config* config = fxmain->user->config;
	GtkTreePath *path;
	GtkTreeIter iter;
	GtkTreeView *tree;
	GtkTreeModel *model;
	GdkPixbuf *pb;
	char *sipuri , *name , *impression , *sid , *mobileno , *carrier;
	int serviceStatus , carrierStatus , relationStatus;
	char text[1024];
	char phonetext[128];
	char iconpath[100];

	tree = GTK_TREE_VIEW(widget);
	model = gtk_tree_view_get_model(tree);

	if(!gtk_tree_view_get_tooltip_context(tree , &x , &y , keybord_mode 
									, &model , &path , &iter)){
		return FALSE;
	}
	if(gtk_tree_path_get_depth(path) == 1){
		return FALSE;
	}

	gtk_tree_model_get(model          , &iter 
			         , B_SIPURI_COL     , &sipuri
					 , B_NAME_COL       , &name
					 , B_IMPRESSION_COL , &impression
					 , B_PHONENUM_COL	, &mobileno
					 , B_CARRIER_COL , &carrier
					 , B_SERVICESTATUS_COL , &serviceStatus
					 , B_CARRIERSTATUS_COL , &carrierStatus
					 , B_RELATIONSTATUS_COL , &relationStatus
					 , -1);
	sid = fetion_sip_get_sid_by_sipuri(sipuri);
	bzero(phonetext , sizeof(phonetext));
	if(carrierStatus == CARRIER_STATUS_DOWN){
		if(strlen(carrier) == 0){
			sprintf(phonetext , "<span color='#0088bf'>未绑定手机号</span>");
		}else{
			sprintf(phonetext , "<span color='#0088bf'>%s</span>(<b>停机</b>)"
					, strlen(mobileno) == 0 ? "未公开手机号" : mobileno);
		}
	}else if (carrierStatus == CARRIER_STATUS_NORMAL){
		sprintf(phonetext , "<span color='#0088bf'>%s</span>"
				, (carrier == NULL || strlen(carrier) == 0) ? "未绑定手机号"
				: (mobileno == NULL || strlen(mobileno) == 0 ? "未公开手机号" : mobileno));
	}
	bzero(text , sizeof(text));
	sprintf(text , " <span color='#808080'>昵称:</span>  <b>%s</b>\n"
				   " <span color='#808080'>手机:</span>  %s\n"
				   " <span color='#808080'>飞信:</span>  %s\n"
				   " <span color='#808080'>心情:</span>  %s"
		  		  , name == NULL ? "" : g_markup_escape_text(name , strlen(name))
				  ,  phonetext , sid
				  , impression == NULL ? "" : g_markup_escape_text(impression , strlen(impression)));
	free(name);
	free(impression);
	free(mobileno);
	free(sipuri);
	free(carrier);
	bzero(iconpath , sizeof(iconpath));
	sprintf(iconpath , "%s/%s.jpg" , config->iconPath , sid);
	free(sid);
	pb = gdk_pixbuf_new_from_file_at_size(iconpath , 80 , 80 , NULL);
	if(pb == NULL)
		pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.jpg" , 80 , 80 , NULL);
	gtk_tooltip_set_markup(tip , text);
	gtk_tooltip_set_icon(tip , pb);	
	g_object_unref(pb);
	gtk_tree_view_set_tooltip_row(tree , tip , path);
	gtk_tree_path_free(path);
	return TRUE;
}
