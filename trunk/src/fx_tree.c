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
#include "time.h"
/*private*/

static GtkTreeModel* create_model(User* groupList);
static void fx_tree_create_column(GtkWidget* tree , FxMain* fxmain);
static void pg_text_cell_data_func(GtkTreeViewColumn *UNUSED(col),
	   							 GtkCellRenderer   *renderer,
								 GtkTreeModel      *model,
								 GtkTreeIter       *iter,
								 gpointer           UNUSED(user_data));
static GtkWidget* fx_tree_create_menu(const char* name
		, const char* iconpath , GtkWidget* parent
		, gboolean sensitive
		, void (*func)(GtkWidget* item , gpointer data)
		, gpointer data);
//static void fx_tree_add_new_buddy(FxMain* fxmain , Contact* contact);
/*signal function*/
static gboolean fx_tree_on_rightbutton_click(GtkWidget* UNUSED(tree)
		, GdkEventButton* event , gpointer data);
static gboolean pg_on_rightbutton_click(GtkWidget* UNUSED(tree)
		, GdkEventButton* event , gpointer data);
static void fx_tree_on_double_click(GtkTreeView *treeview
		, GtkTreePath *path , GtkTreeViewColumn  *UNUSED(col)
		, gpointer data);
static void pg_on_double_click(GtkTreeView *treeview
		, GtkTreePath *path , GtkTreeViewColumn  *UNUSED(col)
		, gpointer data);
static void fx_tree_on_chatmenu_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_tree_on_profilemenu_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_tree_on_historymenu_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_tree_on_editmenu_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_tree_on_deletemenu_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_tree_on_reload_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_tree_on_iconchange_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_tree_on_gaddmenu_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_tree_on_gdeletemenu_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_tree_on_geditmenu_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static void fx_tree_on_movemenu_clicked(GtkWidget* UNUSED(widget) , gpointer data);
static gboolean fx_tree_on_show_tooltip(GtkWidget* widget
		, int x , int y , gboolean keybord_mode
		, GtkTooltip* tip , gpointer data);
static gboolean pg_on_show_tooltip(GtkWidget* widget
		, int x , int y , gboolean keybord_mode
		, GtkTooltip* tip , gpointer UNUSED(data));
static void on_send_message_clicked(GtkWidget *UNUSED(widget) , gpointer data);
static void on_send_sms_clicked(GtkWidget *widget , gpointer data);
static void on_view_pgdetail_clicked(GtkWidget *UNUSED(widget) , gpointer data);

int all_light = 0;

Args* fx_args_new(FxMain* fxmain , GtkTreeIter iter , const char* ss , int ii)
{
	Args* args = (Args*)malloc(sizeof(Args));

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

	memset(fxtree , 0 , sizeof(FxTree));
	return fxtree;
}

static void show_search(GtkEntry *entry , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	FxSearch *search = fx_search_new(fxmain);
	GtkTreeView *view = GTK_TREE_VIEW(fxmain->mainPanel->treeView);
	GtkTreeModel *model = gtk_tree_view_get_model(view);
	int x , y , ex , ey , root_x , root_y;
	const char *text;

	text = gtk_entry_get_text(entry);

	if(text == NULL || strlen(text) == 0)
		return;

	if(has_gb(text)){
		fx_util_popup_warning(fxmain , _("Only English words are allowed here!"));
		return;
	}

	gtk_widget_translate_coordinates(GTK_WIDGET(entry) , fxmain->window , 0 , 0 , &ex , &ey );
	gtk_window_get_position(GTK_WINDOW(fxmain->window) , &root_x , &root_y);
	x = root_x + ex + 3;
	y = root_y + ey + 46;

	fx_search_initialize(search , model , text , x , y);
}

static void on_search_button_clicked(GtkEntry *entry , GtkEntryIconPosition *UNUSED(pos)
		, GdkEvent *UNUSED(event) , gpointer data)
{
	show_search(entry , data);
}

static GtkTreeModel *create_pg_model()
{
	GtkTreeStore *store = gtk_tree_store_new(PG_COL_NUM
		 , GDK_TYPE_PIXBUF
		 , G_TYPE_STRING
		 , G_TYPE_STRING
		 , G_TYPE_STRING
		 , G_TYPE_STRING
		 , G_TYPE_INT
		 , G_TYPE_INT
		 , G_TYPE_INT
		 , G_TYPE_INT
		 , G_TYPE_INT);

	return GTK_TREE_MODEL(store);
}

static void create_pg_column(GtkWidget* tree)
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
					  , pg_text_cell_data_func
					  , NULL
					  , NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col0);

}

static gboolean pg_on_show_tooltip(GtkWidget* widget
					  , int x
					  , int y
					  , gboolean keybord_mode
					  , GtkTooltip* tip
					  , gpointer UNUSED(data))
{
	GtkTreeView *tree = GTK_TREE_VIEW(widget);
	GtkTreeModel *model = gtk_tree_view_get_model(tree);
	GdkPixbuf *pixbuf;
	GtkTreePath *path;
	GtkTreeIter iter;
	char text[1024];
	char time[1024];
	char *name;
	char *createTime;
	int identity;
	int currentMemberCount;
	int limitMemberCount;
	int groupRank;
	int maxRank;
	struct tm date;


	if(!gtk_tree_view_get_tooltip_context(tree , &x , &y , keybord_mode
						, &model , &path , &iter)){
		return FALSE;
	}

	pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"online.svg" , 90 , 90 , NULL);
	gtk_tooltip_set_icon(tip , pixbuf);
	g_object_unref(pixbuf);

	gtk_tree_model_get(model , &iter
		, PG_NAME_COL , &name
		, PG_CREATETIME_COL , &createTime
		, PG_IDENTITY_COL , &identity
		, PG_CCOUNT_COL , &currentMemberCount
		, PG_LCOUNT_COL , &limitMemberCount
		, PG_GRANK_COL , &groupRank
		, PG_MRANK_COL , &maxRank
		, -1);

	date = convert_date(createTime);
	strftime(time , sizeof(time) , _("%Y-%m-%d") , &date);
	free(createTime);

	snprintf(text , sizeof(text) - 1
		, _("\n<span color='#808080'>Group Name：</span><b>%s</b>\t\n"
		"<span color='#808080'>Current Member Count:</span> %d\t\n"
		"<span color='#808080'>Limit Member Count:</span> %d\t\n"
		"<span color='#808080'>Group Rank:</span> %d/%d\t\n"
		"<span color='#808080'>Create Time:</span> %s\t\n")
		, g_markup_escape_text(name, -1) , currentMemberCount , limitMemberCount
	       	, groupRank , maxRank , time);

	gtk_tooltip_set_markup(tip , text);
	free(name);
	gtk_tree_view_set_tooltip_row(tree , tip , path);
	gtk_tree_path_free(path);
    	return TRUE;
}

void fx_tree_bind_pg_data(FxMain *fxmain)
{
	FxTree *fxtree = fxmain->mainPanel;
	Config *config = fxmain->user->config;
	PGGroup *pggroup = fxmain->user->pggroup;
	PGGroup *pgcur;
	GtkTreeView *tree = GTK_TREE_VIEW(fxtree->pgTreeView);
	GtkTreeModel *model = gtk_tree_view_get_model(tree);
	GtkTreeStore *store = GTK_TREE_STORE(model);
	GtkTreeIter iter;
	GtkTreeIter citer;
	GdkPixbuf *pixbuf;
	char portraitPath[1024];
	char *uri;
	char *pgsid;
	int hasGroup = 0;

	foreach_grouplist(pggroup , pgcur){
		hasGroup = 0;
		if(gtk_tree_model_get_iter_first(model , &citer)){
			do{
				gtk_tree_model_get(model , &citer
				       	, PG_URI_COL , &uri , -1);
				if(strcmp(pgcur->pguri , uri) == 0){
				 	free(uri);
				     	hasGroup = 1;
					break;
				}
			}while(gtk_tree_model_iter_next(model , &citer));
		}
		if(!hasGroup){
			gtk_tree_store_append(store , &iter , NULL);
			pgsid = fetion_sip_get_pgid_by_sipuri(pgcur->pguri);
			snprintf(portraitPath , sizeof(portraitPath) - 1,
						  "%s/PG%s.jpg" , config->iconPath , pgsid);
			free(pgsid);
			pixbuf = gdk_pixbuf_new_from_file_at_size(portraitPath , PG_PORTRAIT_SIZE , PG_PORTRAIT_SIZE , NULL);
			if(pixbuf == NULL){
			    if(pgcur->identity == 1)
				pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"online.svg"
				       	, PG_PORTRAIT_SIZE , PG_PORTRAIT_SIZE , NULL);
			    else
				pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"invisible.svg"
				       	, PG_PORTRAIT_SIZE , PG_PORTRAIT_SIZE , NULL);
			}
			gtk_tree_store_set(store , &iter
				, PG_PIXBUF_COL , pixbuf , -1);
			g_object_unref(pixbuf);
		}
		gtk_tree_store_set(store , (hasGroup == 1 ? &citer : &iter)
		  , PG_URI_COL , pgcur->pguri
		  , PG_NAME_COL , g_markup_escape_text(pgcur->name, -1)
		  , PG_CREATETIME_COL , pgcur->createTime
		  , PG_IDENTITY_COL , pgcur->identity
		  , PG_CCOUNT_COL , pgcur->currentMemberCount
		  , PG_LCOUNT_COL , pgcur->limitMemberCount
		  , PG_GRANK_COL , pgcur->groupRank
		  , PG_MRANK_COL , pgcur->maxRank
		  , -1);
	}
}

void fx_tree_initilize(FxMain* fxmain)
{
	GtkWidget* mainbox = fxmain->mainbox;
	FxTree* fxtree;
	GtkTreeModel* treeModel;
	ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
	args->fxmain = fxmain;
	args->sip = NULL;
	GValue value = { 0, };

	all_light = fxmain->user->config->allHighlight;
	fxtree = fxmain->mainPanel;

	fxtree->searchbox = gtk_entry_new();
	g_value_init(&value, G_TYPE_STRING);
	g_value_set_static_string(&value, "gtk-im-context-simple");
	g_object_set_property(G_OBJECT(fxtree->searchbox),
						"im-module",
						&value);
	g_value_unset (&value);
	gtk_entry_set_icon_from_stock(GTK_ENTRY(fxtree->searchbox)
			, GTK_ENTRY_ICON_SECONDARY , GTK_STOCK_FIND);
	g_signal_connect(fxtree->searchbox , "icon-press" , G_CALLBACK(on_search_button_clicked) , fxmain);
	g_signal_connect(fxtree->searchbox , "activate" , G_CALLBACK(show_search) , fxmain);
	gtk_widget_show_all(fxtree->searchbox);
	gtk_box_pack_start(GTK_BOX(mainbox) , fxtree->searchbox , FALSE , FALSE , 0);

	/* contact tree */
	fxtree->scrollWindow = gtk_scrolled_window_new(NULL , NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fxtree->scrollWindow)
								 , GTK_POLICY_NEVER , GTK_POLICY_AUTOMATIC);

	treeModel = create_model(fxmain->user);
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

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(fxtree->treeView) , FALSE);
 	//gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (fxtree->treeView), TRUE);
	gtk_tree_view_set_level_indentation(GTK_TREE_VIEW(fxtree->treeView) , -30);
	gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(fxtree->treeView) , TRUE);

	gtk_container_set_border_width(GTK_CONTAINER(fxtree->scrollWindow) , 0);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(fxtree->scrollWindow) , fxtree->treeView);
	gtk_box_pack_start(GTK_BOX(mainbox) , fxtree->scrollWindow , TRUE , TRUE , 0);

	/* pg group */
	fxtree->pgScrollWindow = gtk_scrolled_window_new(NULL , NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fxtree->pgScrollWindow)
								 , GTK_POLICY_NEVER , GTK_POLICY_AUTOMATIC);
	treeModel = create_pg_model();
	fxtree->pgTreeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(treeModel));
	gtk_widget_set_usize(fxtree->pgTreeView , 100 , 0);
	g_object_set(fxtree->pgTreeView , "has-tooltip" , TRUE , NULL);
	g_signal_connect(fxtree->pgTreeView , "query-tooltip" , G_CALLBACK(pg_on_show_tooltip) , fxmain);
	create_pg_column(fxtree->pgTreeView);
	g_signal_connect(fxtree->pgTreeView
				   , "button_press_event"
				   , G_CALLBACK(pg_on_rightbutton_click)
				   , fxmain);
	g_signal_connect(fxtree->pgTreeView
				   , "row-activated"
				   , G_CALLBACK(pg_on_double_click)
				   , fxmain);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(fxtree->pgTreeView) , FALSE);
	//gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (fxtree->pgTreeView), TRUE);
	gtk_tree_view_set_level_indentation(GTK_TREE_VIEW(fxtree->pgTreeView) , -30);
	gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(fxtree->pgTreeView) , TRUE);

	gtk_container_set_border_width(GTK_CONTAINER(fxtree->pgScrollWindow) , 0);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(fxtree->pgScrollWindow) , fxtree->pgTreeView);
	gtk_box_pack_start(GTK_BOX(mainbox) , fxtree->pgScrollWindow , TRUE , TRUE , 0);


	/* no group label */
	fxtree->noLabelScrollWindow = gtk_scrolled_window_new(NULL , NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(fxtree->noLabelScrollWindow)
						 , GTK_POLICY_NEVER , GTK_POLICY_AUTOMATIC);
	fxtree->noPgLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(fxtree->noPgLabel) , _("<b>No Group!</b>"));
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(fxtree->noLabelScrollWindow) , fxtree->noPgLabel);
	gtk_box_pack_start(GTK_BOX(mainbox) , fxtree->noLabelScrollWindow , TRUE , TRUE , 0);
	gtk_widget_set_name(fxtree->noLabelScrollWindow , "mainwindow");


	if(fxmain->user->state != P_OFFLINE){
		fetion_contact_subscribe_only(fxmain->user);
		g_thread_create(fx_main_listen_thread_func , args , FALSE , NULL);
	}
}

void fx_tree_show(FxMain *fxmain)
{
	FxTree *fxtree = fxmain->mainPanel;
	gtk_widget_show_all(fxtree->scrollWindow);
	gtk_widget_show_all(fxtree->pgScrollWindow);
	gtk_widget_show_all(fxtree->noLabelScrollWindow);
	gtk_widget_hide(fxtree->pgScrollWindow);
	gtk_widget_hide(fxtree->noLabelScrollWindow);
}

void fx_tree_free(FxTree* fxtree)
{
	free(fxtree);
}
void fx_tree_move_to_the_first(GtkTreeModel* model , GtkTreeIter* iter)
{
	gtk_tree_store_move_after(GTK_TREE_STORE(model) , iter , NULL);
}
void fx_tree_move_to_the_last(GtkTreeModel* model , GtkTreeIter* iter)
{
	gtk_tree_store_move_before(GTK_TREE_STORE(model) , iter , NULL);
}
static GtkTreeModel* create_model(User* user)
{
	Group         *group = NULL;
	Contact       *contact;
	Config        *config = NULL;
	GdkPixbuf     *pb = NULL;
	GtkTreeStore  *store = NULL;
	GtkTreeIter    iter;
	GtkTreeIter    iter1;
	gchar         *sid = NULL;
	gchar         *name;
	gchar          path[1024];
	gint           count;
	gint           count1;
	gint           imageset = 0;

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
							 , G_TYPE_INT
							 , G_TYPE_INT);

	foreach_grouplist(user->groupList , group){
		gtk_tree_store_append(store , &iter , NULL);
		gtk_tree_store_set(store , &iter
						 , G_NAME_COL 		  , _(group->groupname)
						 , G_ID_COL           , group->groupid
						 , G_ALL_COUNT_COL    , 0
						 , G_ONLINE_COUNT_COL , 0 , -1);
	}
	foreach_contactlist(user->contactList , contact){
		if(strlen(contact->sId) == 0)
			sid = fetion_sip_get_sid_by_sipuri(contact->sipuri);
		sprintf(path, "%s/%s.jpg", config->iconPath,
					 strlen(contact->sId) == 0 ? sid : contact->sId);
		if(sid){
			g_free(sid);
			sid = NULL;
		}
		pb = gdk_pixbuf_new_from_file_at_size(path
				, config->iconSize , config->iconSize , NULL);
		if(!pb){
			pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.svg"
					, config->iconSize , config->iconSize , NULL);
			imageset = 0;
		}else
			imageset = 1;

		if(fx_tree_get_group_iter_by_id(GTK_TREE_MODEL(store),
					contact->groupid , &iter ) < 0){
			//debug_info("Error when a group iter not found , groupId :%d\n" , contact->groupid);
			continue;
		}
		if(contact->state > 0)
			gtk_tree_store_prepend(store , &iter1 , &iter);
		else
			gtk_tree_store_append(store , &iter1 , &iter);
		name = (contact->localname == NULL || strlen(contact->localname) == 0) ?
					contact->nickname : contact->localname;

		gtk_tree_store_set(store , &iter1
						 , B_PIXBUF_COL 	, pb
						 , B_SIPURI_COL 	, contact->sipuri ? contact->sipuri : ""
						 , B_USERID_COL 	, contact->userId ? contact->userId : ""
						 , B_NAME_COL 	    , name ? g_markup_escape_text(name, -1) : ""
						 , B_IMPRESSION_COL , g_markup_escape_text(contact->impression, -1)
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
						 , B_IMAGE_SET_COL  , imageset
						 , B_IMAGE_CHANGED_COL, contact->imageChanged
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
int fx_tree_get_group_iter_by_id(GtkTreeModel* model , int id , GtkTreeIter* iter)
{
	int idt;

	if(gtk_tree_model_get_iter_root(model , iter)){
		do{
			gtk_tree_model_get(model , iter , G_ID_COL , &idt , -1);
			if(idt == id)
				return 1;
		}while(gtk_tree_model_iter_next(model , iter));
	}

	return -1;
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
static void fx_tree_on_hightlight_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	User *user = fxmain->user;
	Config *config = user->config;

	if(all_light){
		all_light = 0;
		config->allHighlight = 0;
	}else{
		all_light = 1;
		config->allHighlight = 1;
	}

	fetion_config_save(user);
}
static void fx_tree_create_buddy_menu(FxMain* fxmain , GtkWidget* UNUSED(tree)
		, GtkTreePath* path , GdkEventButton* event , GtkTreeIter iter)
{
	gchar             *sipuri;
	gchar             *groupname;
	gchar             *userid;
	gchar             *mobileno;
	gchar             *carrier;
	GtkWidget         *menu = NULL;
	GtkWidget         *groupSubmenu = NULL;
	GtkWidget         *moveItem = NULL;
	GtkTreeSelection  *selection = NULL;
	GtkTreeModel      *model = NULL;
	GtkTreeIter        groupiter;
	Args              *profileargs;
	Args              *moveargs;
	Args              *chatargs;
	gint               groupid;
	gint               iconsize;
	gint               serviceStatus;
	gint               relationStatus;
	gint               carrierStatus;
	FxTree            *fxtree = fxmain->mainPanel;
	User              *user = fxmain->user;
	Config            *config = user->config;


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
		fx_tree_create_menu(_("Send IM mesages") , SKIN_DIR"myselfsms.png" , menu
						  , ((serviceStatus == BASIC_SERVICE_ABNORMAL
								  && (carrierStatus == CARRIER_STATUS_CLOSED || (strlen(carrier)!= 0 && strlen(mobileno) == 0)))
						  || relationStatus == RELATION_STATUS_UNAUTHENTICATED || user->state == P_OFFLINE)
						  ? FALSE : TRUE
						  , fx_tree_on_chatmenu_clicked , chatargs);

		fx_tree_create_menu(_("view contact's information") , SKIN_DIR"profile.png"
						, menu , (serviceStatus == BASIC_SERVICE_ABNORMAL
						  && (carrierStatus == CARRIER_STATUS_CLOSED ||
							  (strlen(carrier)!= 0 && strlen(mobileno) == 0))) ?
						FALSE : TRUE , fx_tree_on_profilemenu_clicked , profileargs);
#if 0
		fx_tree_create_menu("FILE" , SKIN_DIR"sendfile.png"
						, menu , TRUE , fx_tree_on_sendfile_clicked , profileargs);
#endif
		fx_tree_create_menu(_("View chat logs") , SKIN_DIR"history.png"
						, menu , TRUE ,  fx_tree_on_historymenu_clicked , profileargs);

		if(user->state != P_OFFLINE)
			fx_tree_create_menu(_("Refresh information") , SKIN_DIR"refresh.png"
						, menu , TRUE ,  fx_tree_on_reload_clicked , profileargs);

		if(user->state != P_OFFLINE)
			fx_tree_create_menu(_("Edit note name") , SKIN_DIR"edit.png"
						, menu , TRUE , fx_tree_on_editmenu_clicked , profileargs);

		fx_tree_create_menu( config->allHighlight ? _("Hightlight online contacts") : _("Hightlight all contacts") , SKIN_DIR"hilight.png"
						, menu , TRUE , fx_tree_on_hightlight_clicked , fxmain);

		fx_tree_create_menu(iconsize > 30 ? _("Use small icon") : _("Use big icon")
						, SKIN_DIR"bigimage.png" , menu , TRUE , fx_tree_on_iconchange_clicked , fxmain);

		if(user->state != P_OFFLINE)
			fx_tree_create_menu(_("Remove this contact") , SKIN_DIR"delete.png"
						, menu , TRUE , fx_tree_on_deletemenu_clicked , profileargs);

		gtk_tree_model_iter_parent(model , &groupiter , &iter);
		gtk_tree_model_get(model , &groupiter , G_ID_COL , &groupid , -1);

		if(groupid != BUDDY_LIST_STRANGER && user->state != P_OFFLINE)
		{
			moveItem = fx_tree_create_menu(_("Move this contact to") , SKIN_DIR"move.png"
							, menu , TRUE , NULL , NULL);
			/*add group child menu*/
			gtk_tree_model_get_iter_root(GTK_TREE_MODEL(model) , &groupiter);
			do
			{
				gtk_tree_model_get(GTK_TREE_MODEL(model) , &groupiter
								 , G_NAME_COL 			 , &groupname
								 , G_ID_COL 			 , &groupid , -1);
				moveargs = fx_args_new(fxmain , iter , userid , groupid);
				fx_tree_create_menu(groupname 	  , SKIN_DIR"online.svg"
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

static void fx_tree_create_group_menu(FxMain* fxmain , GtkWidget* tree
		, GdkEventButton* event , GtkTreeIter iter)
{
	GtkWidget* menu = NULL;
	Args* args = fx_args_new(fxmain , iter ,  NULL , 0);
	int count , groupid;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));

	menu = gtk_menu_new();
	gtk_tree_model_get(model ,  &iter
					 , G_ALL_COUNT_COL , &count
					 , G_ID_COL		   , &groupid , -1);
	fx_tree_create_menu(_("Add a new group") , SKIN_DIR"myselfsms.png"
					, menu , TRUE , fx_tree_on_gaddmenu_clicked , fxmain);
	fx_tree_create_menu(_("Edit name of a group") , SKIN_DIR"edit.png"
					, menu , (groupid == BUDDY_LIST_NOT_GROUPED || groupid == BUDDY_LIST_STRANGER) ? FALSE : TRUE
					, fx_tree_on_geditmenu_clicked , args);
	fx_tree_create_menu(_("Remove this group") , SKIN_DIR"delete.png"
					, menu , (count == 0 && groupid != BUDDY_LIST_NOT_GROUPED && groupid != BUDDY_LIST_STRANGER) ? TRUE : FALSE
					, fx_tree_on_gdeletemenu_clicked , args);

	gtk_widget_show_all(menu);
	gtk_menu_popup(GTK_MENU(menu) , NULL , NULL , NULL , NULL
			, (event != NULL) ? event->button : 0 , gdk_event_get_time((GdkEvent*)event));
}
static GtkWidget* fx_tree_create_menu(const char* name
							 , const char* iconpath
							 , GtkWidget* parent
							 , gboolean sensitive
							 , void (*func)(GtkWidget* item , gpointer data)
							 , gpointer data)
{
	GtkWidget *item = gtk_image_menu_item_new_with_label(name);
	GdkPixbuf *pb = gdk_pixbuf_new_from_file_at_size(iconpath , 16 , 16 , NULL);
	GtkWidget *img = gtk_image_new_from_pixbuf(pb);

	g_object_unref(pb);

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

#if 0
static void fx_tree_add_new_buddy(FxMain* fxmain , Contact* contact)
{
	GtkTreeView *tree = GTK_TREE_VIEW(fxmain->mainPanel->treeView);
	GtkTreeModel *model = gtk_tree_view_get_model(tree);
	GtkTreeIter oIter , nIter;
	GdkPixbuf *pb = NULL;
	Config *config = fxmain->user->config;
	int groupid;

	gtk_tree_model_get_iter_root(model , &oIter);
	do
	{
		gtk_tree_model_get(model		, &oIter
						 , G_ID_COL		, &groupid
						 , -1);
		if(groupid == contact->groupid)
		{
			pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.svg" , config->iconSize , config->iconSize , NULL);
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
#endif

static void fx_tree_pixbuf_cell_data_func(GtkTreeViewColumn *UNUSED(col)
								 , GtkCellRenderer *renderer
								 , GtkTreeModel *model
								 , GtkTreeIter *iter
								 , gpointer UNUSED(user_data))
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
static void fx_tree_text_cell_data_func(GtkTreeViewColumn *UNUSED(col),
	   							 GtkCellRenderer   *renderer,
								 GtkTreeModel      *model,
								 GtkTreeIter       *iter,
								 gpointer           UNUSED(user_data))
{
	GtkTreePath* path = gtk_tree_model_get_path(model , iter);
	char text[2048];
	/*contact data*/
	char *name , *impression , *sipuri , *sid , *stateStr , *mobileno , *device , *carrier;
	char stateStr1[1024];
	char statusStr[1024];
	int presence, size;
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
		memset(statusStr, 0, sizeof(statusStr));
		if(relationStatus == RELATION_STATUS_UNAUTHENTICATED){
			snprintf(statusStr , sizeof(statusStr) - 1
						   	, _("<span color='#d4b4b4'>[Unverified]</span>"));
		}else if(serviceStatus == BASIC_SERVICE_ABNORMAL){
			if(carrierStatus == CARRIER_STATUS_CLOSED){
				snprintf(statusStr , sizeof(statusStr) - 1,
							 _("<span color='#d4b4b4'>[Has shut fetion service]</span>"));
			}else{
				if(carrier != NULL || strlen(carrier) != 0){
					snprintf(statusStr , sizeof(statusStr) - 1,
								 _("<span color='#d4b4b4'>[Online with SMS]</span>"));
					if(mobileno == NULL || strlen(mobileno) == 0){
						snprintf(statusStr , sizeof(statusStr) - 1,
									 _("<span color='#d4b4b4'>[Has shut fetion service]</span>"));
					}
				}else{
					snprintf(statusStr , sizeof(statusStr) - 1,
								 	_("<span color='#d4b4b4'>[Has shut fetion service]</span>"));
				}
			}
		}else if(carrierStatus == CARRIER_STATUS_DOWN){
			if(strlen(carrier) != 0){
				snprintf(statusStr , sizeof(statusStr) - 1,
							   	_("<span color='#d4b4b4'>[Out of service]</span>"));
			}
		}
		if(sipuri == NULL){
			printf("NULL\n");
			return;
		}
		sid = fetion_sip_get_sid_by_sipuri(sipuri);
		snprintf(stateStr1 , sizeof(stateStr1) - 1,
					   	"<span color='#0099FF'>%s</span>" , stateStr);
		escape_impression(impression);

        char newline[3] = "";
		if(size >= 30)
			sprintf(newline, "\n");
        snprintf(text , sizeof(text) - 1 , "<b>%s</b>%s%s"
                       "(%s) %s <span color='#838383'>%s</span>"
                       , name == NULL ? "" : name
                       , (strlen(statusStr) == 0 ? (presence == 0 ? "" : stateStr1) : statusStr)
                       , (device != NULL && strcmp(device , "PC") != 0) ? _("[Login with cell phone]") : "", sid, newline
                       , impression == NULL ? "" : impression);
		g_object_set(renderer, "markup", text, NULL);

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
		snprintf(text , sizeof(text) - 1 , "%s [%d/%d]" , buddylistName , onlineCount , allCount );
		g_object_set(renderer , "text" , text , NULL);
		free(buddylistName);
	}
	gtk_tree_path_free(path);
}
static void pg_text_cell_data_func(GtkTreeViewColumn *UNUSED(col),
	   							 GtkCellRenderer   *renderer,
								 GtkTreeModel      *model,
								 GtkTreeIter       *iter,
								 gpointer           UNUSED(user_data))
{
 	char *name;
	int identity;
	int currentMemberCount;
	int limitMemberCount;
	char *createTime;
	struct tm date;
	char time[1024];
	char text[1024];

	gtk_tree_model_get(model , iter
	  , PG_NAME_COL , &name
	  , PG_IDENTITY_COL , &identity
	  , PG_CCOUNT_COL , &currentMemberCount
	  , PG_LCOUNT_COL , &limitMemberCount
	  , PG_CREATETIME_COL , &createTime
	  , -1);
	if(createTime != NULL && strlen(createTime) != 0){
	    date = convert_date(createTime);
	    strftime(time , sizeof(time) , _("%Y-%m-%d") , &date);
	    snprintf(text , sizeof(text) - 1,
					   	_("%s\n<span color='#808080'>Create Time: %s</span>")
		    , g_markup_escape_text(name , strlen(name)) , time);
	    g_object_set(renderer , "markup" , text , NULL);
	    free(createTime);
	}
	free(name);

}

static void fx_tree_create_column(GtkWidget* tree , FxMain* fxmain)
{
	GtkCellRenderer* renderer;
	GtkTreeViewColumn *col , *col0;

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
void fx_tree_update_portrait(FxMain *fxmain)
{
	GtkTreeModel  *model;
	GtkTreeIter    iter;
	GtkTreeIter    pos;
	GdkPixbuf     *pb;
	User          *user = fxmain->user;
	Config        *config = user->config;
	gchar         *sipuri;
	gchar         *crc;
	gchar         *sid;
	gchar          portraitPath[1024];
	gint           size;
	gint           imageChanged;
	gint           imageset;
	gint           nr = 0;

	debug_info("updating portrait...");

	model = gtk_tree_view_get_model(
			GTK_TREE_VIEW(fxmain->mainPanel->treeView));
	gtk_tree_model_get_iter_root(model , &iter);

	do{
		if(gtk_tree_model_iter_children(model , &pos , &iter)){
			do{
				gtk_tree_model_get(model , &pos,
								 B_SIPURI_COL, &sipuri,
								 B_CRC_COL, &crc,
								 B_SIZE_COL, &size,
								 B_IMAGE_CHANGED_COL, &imageChanged,
								 B_IMAGE_SET_COL, &imageset,
								 -1);

				/* if image has been set and no changes,then do nothing */
				if(imageset && !imageChanged){
					g_free(sipuri);
					g_free(crc);
					continue;
				}

				/* if image has not been set,but this buddy has no imaeg,
				 * then also do nothing */
				if(!imageset && strcmp(crc, "0") == 0){
					g_free(sipuri);
					g_free(crc);
					continue;
				}

				sid = fetion_sip_get_sid_by_sipuri(sipuri);
				snprintf(portraitPath, sizeof(portraitPath) - 1,
						"%s/%s.jpg" , config->iconPath , sid);
				/* download portrait */
				fetion_user_download_portrait(user , sipuri);
				pb = gdk_pixbuf_new_from_file_at_size(portraitPath,
						size , size , NULL);

				if(pb){
					nr ++;
					gdk_threads_enter();
					gtk_tree_store_set(GTK_TREE_STORE(model),
							&pos , B_PIXBUF_COL , pb , -1);
					g_object_unref(pb);
					gdk_threads_leave();
				}

				gdk_threads_enter();
				gtk_tree_store_set(GTK_TREE_STORE(model), &pos,
						B_IMAGE_CHANGED_COL , IMAGE_NOT_CHANGED,
						B_IMAGE_SET_COL, 1,-1);
				gdk_threads_leave();
				g_free(sipuri);
				g_free(sid);
			}
			while(gtk_tree_model_iter_next(model , &pos));
		}
	}
	while(gtk_tree_model_iter_next(model , &iter));

	/* update pggroup portrait */
	gchar    pgPortraitServer[1024];
	gchar    pgPortraitPath[1024];
	gchar   *strcur;
	gchar   *pgsid;
	PGGroup *pgcur;
	gint     n;

	model = gtk_tree_view_get_model(
			GTK_TREE_VIEW(fxmain->mainPanel->pgTreeView));
	if(gtk_tree_model_get_iter_root(model , &iter)){
		do{
			gtk_tree_model_get(model , &iter
				, PG_URI_COL , &sipuri , -1);

			pgsid = fetion_sip_get_pgid_by_sipuri(sipuri);
			snprintf(portraitPath, sizeof(portraitPath) - 1,
					"%s/PG%s.jpg", config->iconPath , pgsid);

			pb = gdk_pixbuf_new_from_file_at_size(portraitPath,
					PG_PORTRAIT_SIZE , PG_PORTRAIT_SIZE , NULL);

			if(pb == NULL){
				foreach_pg_group(user->pggroup , pgcur){
					if(strcmp(pgcur->pguri , sipuri) == 0)
					    break;
				}
				strcur = pgcur->getProtraitUri;
				if(strstr(strcur , "http://") != NULL)
				    strcur = strstr(strcur , "http://") + 7;
				n = strlen(strcur) - strlen(strstr(strcur , "/"));
				if(n == 0)
				    continue;
				strncpy(pgPortraitServer , strcur , n);
				strcur = strstr(strcur , "/");
				strcpy(pgPortraitPath , strcur);
				/* download portrait */
				fetion_user_download_portrait_with_uri(user,
						sipuri , pgPortraitServer , pgPortraitPath);

				pb = gdk_pixbuf_new_from_file_at_size(portraitPath
				       	, PG_PORTRAIT_SIZE , PG_PORTRAIT_SIZE , NULL);
				if(pb != NULL){
					gdk_threads_enter();
					gtk_tree_store_set(GTK_TREE_STORE(model) , &iter
						, PG_PIXBUF_COL , pb , -1);
					g_object_unref(pb);
					gdk_threads_leave();
				}
			}else{
				g_object_unref(pb);
			}

			g_free(sipuri);
			g_free(pgsid);
		}while(gtk_tree_model_iter_next(model , &iter));
	}

	debug_info("%d portrait was successfully updated", nr);
}

static void fx_tree_on_double_click(GtkTreeView *treeview
		, GtkTreePath *path , GtkTreeViewColumn  *UNUSED(col) , gpointer data)
{
	FxMain        *fxmain = (FxMain*)data;
	GtkTreeModel  *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	GtkTreeIter    iter;
	gtk_tree_model_get_iter(model , &iter , path);
	gint depth = gtk_tree_path_get_depth(path);
	gchar *sipuri , *mobileno , *carrier;
	gint serviceStatus , relationStatus , carrierStatus;

	if(depth > 1){
		if(!fx_conn_check_action(fxmain))
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
			fx_util_popup_warning(fxmain , _("You can not send a message to anyone who has not been in your contact list"));
			return;
		}
		if(serviceStatus == BASIC_SERVICE_ABNORMAL &&
			(carrierStatus == CARRIER_STATUS_CLOSED ||
			 (strlen(carrier) != 0 && strlen(mobileno) == 0)))
		{
			fx_util_popup_warning(fxmain , _("This user have shut fetion service,so you cannot send mesage to him/her"));
			return;
		}
		fx_main_create_chat_window(fxmain , sipuri);
		g_free(sipuri);
		g_free(mobileno);
		g_free(carrier);
	}
}
static gboolean fx_tree_on_rightbutton_click(GtkWidget* UNUSED(tree)
		, GdkEventButton* event , gpointer data)
{
	GtkTreeIter   iter;
	GtkTreePath  *path = NULL;
	GtkTreeModel *model = NULL;
	FxMain       *fxmain = NULL;
	FxTree       *fxtree = NULL;
	int depth;

	if(event->type == GDK_BUTTON_PRESS)
	{
		fxmain = (FxMain*)data;
		fxtree = fxmain->mainPanel;
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(fxtree->treeView));
		gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(fxtree->treeView)
				, (gint)event->x , (gint)event->y , &path , NULL , NULL , NULL);
		if(path == NULL)
			return FALSE;

		if(!gtk_tree_model_get_iter(GTK_TREE_MODEL(model) , &iter , path))
			return FALSE;

		depth = gtk_tree_path_get_depth(path);

		if(event->button == 3){
			if(depth == 2)
				fx_tree_create_buddy_menu(fxmain , fxtree->treeView , path , event , iter);
			else
				fx_tree_create_group_menu(fxmain , fxtree->treeView , event , iter);
			
			gtk_tree_path_free(path);
			return TRUE;
		}else if(event->button == 1){
			if(depth == 2)
				return FALSE;
			if(gtk_tree_view_row_expanded(GTK_TREE_VIEW(fxtree->treeView) , path))
				gtk_tree_view_collapse_row(GTK_TREE_VIEW(fxtree->treeView) , path);
			else
				gtk_tree_view_expand_row(GTK_TREE_VIEW(fxtree->treeView) , path , TRUE);
			return TRUE;
		}
	}
	return FALSE;
}
static void fx_tree_on_chatmenu_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	Args    *args = (Args*)data;
	FxMain  *fxmain = args->fxmain;
	gchar   *sipuri = args->s;

	fx_main_create_chat_window(fxmain , sipuri);
	free(args);
}

static void* update_contact_info(void *data)
{
	FxProfile *fxprofile = (FxProfile*)data;
	gchar *res;
	Contact *contact = fetion_contact_get_contact_info(
			fxprofile->fxmain->user,
			fxprofile->userid);

	gdk_threads_enter();
	res = fetion_config_get_province_name(contact->province);
	gtk_entry_set_text(GTK_ENTRY(fxprofile->province_entry) , res == NULL ? _("Unknown") : res);
	update();
	free(res);
	res = fetion_config_get_city_name(contact->province , contact->city);
	gtk_entry_set_text(GTK_ENTRY(fxprofile->city_entry) , res == NULL ? _("Unknown") : res);
	update();
	free(res);
	gdk_threads_leave();

	return NULL;

}

static void fx_tree_on_profilemenu_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	Args        *args = (Args*)data;
	FxMain      *fxmain = args->fxmain;
	Contact     *contact;
	gchar       *userid = args->s;
	FxProfile   *fxprofile;

	fxprofile = fx_profile_new(fxmain , userid);

	fx_profile_initialize(fxprofile);

	contact = fx_profile_fetch(fxprofile);

	if(contact){
		fx_profile_bind(fxprofile , contact);
		if(strlen(contact->province) == 0 && fxmain->user->state != P_OFFLINE)
			g_thread_create(update_contact_info, fxprofile, FALSE, NULL);
	}

	gtk_dialog_run(GTK_DIALOG(fxprofile->dialog));
	gtk_widget_destroy(fxprofile->dialog);

	g_free(fxprofile);
	g_free(args);
}

static void fx_tree_on_historymenu_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	Args         *args = (Args*)data;
	FxMain      *fxmain = args->fxmain;
	GtkTreeIter  iter = args->iter;
	gchar        *name = NULL;
	gchar        *userid = args->s;
	FxHistory    *fxhistory = NULL;

	GtkWidget* tree = fxmain->mainPanel->treeView;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));

	gtk_tree_model_get(model , &iter
					 , B_NAME_COL , &name , -1);
	fxhistory = fx_history_new(fxmain , userid , name);
	fx_history_initialize(fxhistory);
	gtk_dialog_run(GTK_DIALOG(fxhistory->dialog));
	gtk_widget_destroy(fxhistory->dialog);
	g_free(fxhistory);
	g_free(args);
}

static void fx_tree_on_editmenu_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	Args    *args = (Args*)data;
	FxMain  *fxmain = args->fxmain;
	gchar   *userid = args->s;
	gint     ret;
	FxEdit  *fxedit;

	if(!fx_conn_check_action(fxmain))
		return;

	fxedit = fx_edit_new(fxmain , args->iter , userid);

	fx_edit_initialize(fxedit);
	ret = gtk_dialog_run(GTK_DIALOG(fxedit->dialog));
	gtk_widget_destroy(fxedit->dialog);
	g_free(fxedit);
	g_free(args);
}

static void fx_tree_on_deletemenu_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	Args         *args = (Args*)data;
	FxMain       *fxmain = args->fxmain;
	gchar        *userid = args->s;
	GtkTreeIter   iter = args->iter;
	GtkWidget    *dialog;
	GtkTreeModel *model;

	if(!fx_conn_check_action(fxmain))
		return;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(fxmain->mainPanel->treeView));

	dialog = gtk_message_dialog_new(GTK_WINDOW(fxmain->window),
									GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_QUESTION,
									GTK_BUTTONS_YES_NO,
									_("Remove this contact?"));
	gtk_window_set_title(GTK_WINDOW(dialog), _("Remove contact"));
	gint result = gtk_dialog_run(GTK_DIALOG(dialog));

	if(result == GTK_RESPONSE_YES){
		fetion_contact_delete_buddy(fxmain->user , userid);
		gtk_tree_store_remove(GTK_TREE_STORE(model) , &iter);
	}

	g_free(args);
	gtk_widget_destroy(dialog);

}

static void* fx_tree_reload_thread(void* data)
{
	Args         *args = (Args*)data;
	FxMain       *fxmain = args->fxmain;
	GtkTreeIter   iter = args->iter;
	GtkTreeView  *tree = GTK_TREE_VIEW(fxmain->mainPanel->treeView);
	GtkTreeModel *model = gtk_tree_view_get_model(tree);
	gchar        *userid = NULL;
	gchar        *sipuri = NULL;
	gchar        *sid = NULL;
	gchar        *name = NULL;
	gchar         portraitPath[1024];
	GdkPixbuf    *pb = NULL;
	Contact      *contact = NULL;
	Config       *config = fxmain->user->config;

	gtk_tree_model_get(model 		, &iter
					 , B_USERID_COL , &userid
					 , B_SIPURI_COL , &sipuri
					 , -1);

	contact = fetion_contact_get_contact_info(fxmain->user , userid);

	debug_info("Updating information of user %s" , userid);
	g_free(userid);

	fetion_user_download_portrait(fxmain->user , sipuri);
	sid = fetion_sip_get_sid_by_sipuri(sipuri);
	g_free(sipuri);

	snprintf(portraitPath , sizeof(portraitPath) - 1,
				   	"%s/%s.jpg" , config->iconPath , sid);
	pb = gdk_pixbuf_new_from_file_at_size(portraitPath , 25 , 25 , NULL);
	if(pb == NULL)
		pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.svg" , 25 , 25 , NULL);

	name = (contact->localname == NULL || strlen(contact->localname) == 0)
			? contact->nickname : contact->localname;
	gdk_threads_enter();
	gtk_tree_store_set(GTK_TREE_STORE(model) , &iter
					 , B_PIXBUF_COL 	, pb
					 , B_SIPURI_COL 	, contact->sipuri
					 , B_USERID_COL 	, contact->userId
					 , B_NAME_COL 	    , g_markup_escape_text(name, -1)
					 , B_IMPRESSION_COL , g_markup_escape_text(contact->impression, -1)
					 , B_PHONENUM_COL   , contact->mobileno
					 , B_DEVICE_COL     , contact->devicetype
					 , B_CRC_COL        , contact->portraitCrc
					 , B_IDENTITY_COL	, contact->identity
					 , -1);
	gdk_threads_leave();
	g_object_unref(pb);
	return NULL;

}

static void fx_tree_on_reload_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxMain *fxmain = ((Args*)data)->fxmain;
	if(!fx_conn_check_action(fxmain))
		return;

	g_thread_create(fx_tree_reload_thread , data , FALSE , NULL);
}

static void fx_tree_on_iconchange_clicked(GtkWidget* UNUSED(widget) , gpointer data)
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

	gtk_tree_model_get_iter_root(model , &iter);
	if(config->iconSize > 30)
	{
		config->iconSize = 25;
		fetion_config_save(fxmain->user);
		debug_info("Changed to use small icon , size : 25px");
	}
	else
	{
		config->iconSize = 40;
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
					sid = fetion_sip_get_sid_by_sipuri(sipuri);
					snprintf(path , sizeof(path) - 1,
								   	"%s/%s.jpg" , config->iconPath , sid);
					free(sid);
					pb = gdk_pixbuf_new_from_file(path , NULL);
					if(pb == NULL)
						pb = gdk_pixbuf_new_from_file(SKIN_DIR"fetion.svg" , NULL);
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
static void fx_tree_on_gaddmenu_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxAddGroup* fxaddgroup = fx_add_group_new(fxmain);

	fx_add_group_initialize(fxaddgroup);
	gtk_dialog_run(GTK_DIALOG(fxaddgroup->dialog));
	gtk_widget_destroy(fxaddgroup->dialog);
	fx_add_group_free(fxaddgroup);
}

static void fx_tree_on_gdeletemenu_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	Args* args = (Args*)data;
	FxMain* fxmain = args->fxmain;
	GtkWidget* tree = fxmain->mainPanel->treeView;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
	int groupid;
	GtkWidget *dialog = NULL;

	gtk_tree_model_get(model , &(args->iter) , G_ID_COL , &groupid , -1);
	dialog = gtk_message_dialog_new(GTK_WINDOW(fxmain->window),
									GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_QUESTION,
									GTK_BUTTONS_YES_NO,
									_("Remove this group?"));
	gtk_window_set_title(GTK_WINDOW(dialog), _("Remove group"));
	int result = gtk_dialog_run(GTK_DIALOG(dialog));
	if(result == GTK_RESPONSE_YES)
	{
		if(fetion_buddylist_delete(fxmain->user , groupid) != -1)
		{
			gtk_tree_store_remove(GTK_TREE_STORE(model) , &args->iter);
		}
	}
	free(args);
	gtk_widget_destroy(dialog);
}
static void fx_tree_on_geditmenu_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	Args* args = (Args*)data;
	FxMain* fxmain = args->fxmain;
	GtkWidget* tree = fxmain->mainPanel->treeView;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
	int groupid;
	FxGEdit* fxgedit = NULL;

	gtk_tree_model_get(model , &(args->iter) , G_ID_COL , &groupid , -1);

	fxgedit = fx_gedit_new(fxmain , args->iter , groupid);
	fx_gedit_initialize(fxgedit);
	gtk_dialog_run(GTK_DIALOG(fxgedit->dialog));
	gtk_widget_destroy(fxgedit->dialog);
	free(args);
}
static void fx_tree_on_movemenu_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	Args          *args = (Args*)data;
	int            groupid = args->i;
	char          *userid = args->s;
	GtkTreeIter    iter = args->iter;
	GtkTreeIter    newIter;
	GtkTreeIter    parentIter;
	GtkTreeIter    oldParentIter;
	FxMain        *fxmain = args->fxmain;
	GtkWidget     *tree = fxmain->mainPanel->treeView;
	GtkTreeModel  *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
	gchar         *name , *sipuri , *impression , *mobileno , *device , *crc , *carrier;
	gint           state , identity , size , imageChanged ;
	gint           serviceStatus , carrierStatus , relationStatus;
	gint           gid , oldOnlineCount , oldAllCount , newOnlineCount , newAllCount;
	GdkPixbuf     *pb = NULL;

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
static gboolean fx_tree_on_show_tooltip(GtkWidget* widget
				  , int x
				  , int y
				  , gboolean keybord_mode
				  , GtkTooltip* tip
				, gpointer data)
{
	FxMain       *fxmain = (FxMain*)data;
	Config       *config = fxmain->user->config;
	GtkTreePath  *path;
	GtkTreeIter   iter;
	GtkTreeView  *tree;
	GtkTreeModel *model;
	GdkPixbuf    *pb;
	gchar        *sipuri;
	gchar        *name;
	gchar        *impression;
	gchar        *sid;
	gchar        *mobileno;
	gchar        *carrier;
	gchar         text[2048];
	gchar         phonetext[1024];
	gchar         iconpath[1024];
	gint          serviceStatus;
	gint          carrierStatus;
	gint          relationStatus;

	tree = GTK_TREE_VIEW(widget);
	model = gtk_tree_view_get_model(tree);

	if(!gtk_tree_view_get_tooltip_context(tree , &x , &y , keybord_mode
						, &model , &path , &iter))
		return FALSE;

	if(gtk_tree_path_get_depth(path) == 1)
		return FALSE;


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

		if(strlen(carrier) == 0)
			snprintf(phonetext, sizeof(phonetext) - 1,
					_("<span color='#0088bf'>Not bind to a phone number.</span>"));
		else
			snprintf(phonetext, sizeof(phonetext) - 1,
					_("<span color='#0088bf'>%s</span>(<b>Out of service</b>)")
					, strlen(mobileno) == 0 ? _("Phone number not be published.") : mobileno);

	}else if (carrierStatus == CARRIER_STATUS_NORMAL){
		snprintf(phonetext, sizeof(phonetext) - 1,
				"<span color='#0088bf'>%s</span>"
				, (carrier == NULL || strlen(carrier) == 0) ? _("Not bind to a phone number.")
				: (mobileno == NULL || strlen(mobileno) == 0 ? _("Phone number not be published.") : mobileno));
	}
	snprintf(text , sizeof(text) - 1 ,
				   	_(" <span color='#808080'>Nickname:</span>  <b>%s</b>\n"
				   " <span color='#808080'>Phone Number:</span>  %s\n"
				   " <span color='#808080'>Fetion Number:</span>  %s\n"
				   " <span color='#808080'>Signature:</span>  %s")
		  		  , name == NULL ? "" : name
				  , phonetext, sid
				  , impression == NULL ? "" : impression);
	g_free(name);
	g_free(impression);
	g_free(mobileno);
	g_free(sipuri);
	g_free(carrier);

	snprintf(iconpath , sizeof(iconpath) - 1,
				   	"%s/%s.jpg" , config->iconPath , sid);
	g_free(sid);
	pb = gdk_pixbuf_new_from_file_at_size(iconpath , 80 , 80 , NULL);
	if(pb == NULL)
		pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"portrait.png" , 80 , 80 , NULL);
	gtk_tooltip_set_markup(tip , text);
	gtk_tooltip_set_icon(tip , pb);
	g_object_unref(pb);
	gtk_tree_view_set_tooltip_row(tree , tip , path);
	gtk_tree_path_free(path);
	return TRUE;
}

FxPGGroup *pg_create_window(FxMain *fxmain , const char *pguri)
{
	FxPGGroup *fxpg;
	PGGroup *pgcur;
	FxList *pglist;

	foreach_list(fxmain->pglist , pglist){
		fxpg = (FxPGGroup*)(pglist->data);
		if(strcmp(fxpg->pggroup->pguri , pguri) == 0)
		    return fxpg;
	}

	foreach_pg_group(fxmain->user->pggroup , pgcur){
		if(strcmp(pgcur->pguri , pguri) == 0)
		    break;
	}
	fxpg = fx_pggroup_new(fxmain , pgcur);
	fx_pggroup_initialize(fxpg);

	pglist = fx_list_new(fxpg);
	fx_list_append(fxmain->pglist , pglist);
	return fxpg;
}

static void pg_on_double_click(GtkTreeView *treeview
		, GtkTreePath *path , GtkTreeViewColumn  *UNUSED(col) , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
    	FxPGGroup *fxpg;
	GtkTreeIter iter;
	char *pguri;
	GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	gtk_tree_model_get_iter(model , &iter , path);
	gtk_tree_model_get(model , &iter , PG_URI_COL , &pguri , -1);
	fxpg = pg_create_window(fxmain , pguri);
	free(pguri);
}

static gboolean pg_on_rightbutton_click(GtkWidget* UNUSED(tree)
		, GdkEventButton* event , gpointer data)
{
   	FxMain       *fxmain = (FxMain*)data;
	FxTree       *fxtree = fxmain->mainPanel;
    GtkTreeModel *model;
	GtkTreePath  *path;
	GtkWidget    *menu;
	GtkWidget    *seperator;
	GtkTreeIter   iter;
	gchar        *sipuri;
	gint         identity;
	Args         *arg;

	if(event->type == GDK_BUTTON_PRESS && event->button == 3)
	{
	    menu = gtk_menu_new();
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(fxtree->pgTreeView));
		gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(fxtree->pgTreeView)
				, (gint)event->x , (gint)event->y , &path , NULL , NULL , NULL);
		if(!path)
			return FALSE;

		gtk_tree_model_get_iter(GTK_TREE_MODEL(model) , &iter , path);
		gtk_tree_model_get(model , &iter
		       	, PG_URI_COL , &sipuri
		       	, PG_IDENTITY_COL , &identity , -1);

		arg = fx_args_new(fxmain , iter , sipuri , 0);
		g_free(sipuri);
		fx_tree_create_menu(_("Send group message") , SKIN_DIR"groupsend.png"
						, menu , TRUE , on_send_message_clicked , arg);
		fx_tree_create_menu(_("Send group sms") , SKIN_DIR"myselfsms.png"
						, menu , (identity == 3 ? FALSE : TRUE) , on_send_sms_clicked , arg);

		seperator = gtk_separator_menu_item_new();
		gtk_menu_shell_append(GTK_MENU_SHELL(menu) , seperator);

		fx_tree_create_menu(_("View group details") , SKIN_DIR"groupsend.png"
						, menu , TRUE , on_view_pgdetail_clicked , arg);

		gtk_widget_show_all(menu);
		gtk_menu_popup(GTK_MENU(menu) , NULL , NULL , NULL , NULL
			, (event != NULL) ? event->button : 0 , gdk_event_get_time((GdkEvent*)event));
		gtk_tree_path_free(path);
		return TRUE;
	}
	return FALSE;
}

static void on_send_message_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	Args *arg = (Args*)data;
	FxMain *fxmain = arg->fxmain;
	char *pguri = arg->s;

	pg_create_window(fxmain , pguri);
	free(arg);
}

static void on_send_sms_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	Args *arg = (Args*)data;
	FxPGGroup *fxpg;
	FxMain *fxmain = arg->fxmain;
	char *pguri = arg->s;

	fxpg = pg_create_window(fxmain , pguri);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxpg->phoneButton) , TRUE);

	free(arg);
}

static void on_view_pgdetail_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	Args *arg = (Args*)data;
	PGGroup *pgcur;
	FxMain *fxmain = arg->fxmain;
	FxPGProfile *fxpg;
	char *pguri = arg->s;

	foreach_pg_group(fxmain->user->pggroup , pgcur){
		if(strcmp(pgcur->pguri , pguri) == 0)
		    break;
	}

	fxpg = fx_pgprofile_new(fxmain , pgcur);
	fx_pgprofile_initialize(fxpg);

	free(arg);
}
