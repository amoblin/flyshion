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

#ifndef FX_TREE_H
#define FX_TREE_H

typedef
enum buddycols
{
	B_PIXBUF_COL = 0 ,
	B_TEXT_COL ,
	B_SIPURI_COL , 
	B_USERID_COL ,
	B_NAME_COL ,
	B_IMPRESSION_COL ,
	B_PHONENUM_COL ,
	B_DEVICE_COL ,
	B_CRC_COL ,
	B_STATE_COL , 
	B_IDENTITY_COL ,
	B_STATUS_COL , 
	B_SIZE_COL ,
	B_IMAGE_CHANGED_COL ,
	COL_NUM
} BuddyCols;

typedef
enum groupcols
{
	G_PIXBUF_COL = 0 ,
	G_TEXT_COL ,
	G_NAME_COL , 
	G_NULL1_COL ,
	G_NULL2_COL , 
	G_NULL3_COL ,
	G_NULL4_COL ,
	G_NULL5_COL ,
	G_NULL6_COL ,
	G_ALL_COUNT_COL , 
	G_ONLINE_COUNT_COL ,
	G_ID_COL 
} GroupCols;

typedef
struct args
{
	FxMain* fxmain;
	GtkTreeIter iter;
	char s[256];
	int i;
} Args;

extern Args* fx_args_new(FxMain* fxmain , GtkTreeIter iter , const char* ss , int ii);

extern FxTree* fx_tree_new();

extern void fx_tree_initilize(FxMain* fxmain);

extern void fx_tree_free(FxTree* fxtree);

extern void fx_tree_move_to_the_first(GtkTreeModel* model , GtkTreeIter* iter);

extern void fx_tree_move_to_the_last(GtkTreeModel* model , GtkTreeIter* iter);

/*private*/

extern GtkTreeModel* fx_tree_create_model(User* groupList);

extern void fx_tree_create_column(GtkWidget* tree , FxMain* fxmain);

extern void fx_tree_get_group_iter_by_id(GtkTreeModel* model
		, int id , GtkTreeIter* iter);

extern int fx_tree_get_buddy_iter_by_userid(GtkTreeModel* model
		, const char* userid , GtkTreeIter* iter);

extern void fx_tree_create_buddy_menu(FxMain* fxmain
		, GtkWidget* tree , GtkTreePath* path
		, GdkEventButton* event , GtkTreeIter iter);

extern void fx_tree_create_group_menu(FxMain* fxmain
		, GtkWidget* tree , GtkTreePath* path
		, GdkEventButton* event , GtkTreeIter iter);

extern GtkWidget* fx_tree_create_menu(const char* name
		, const char* iconpath , GtkWidget* parent
		, gboolean sensitive
		, void (*func)(GtkWidget* item , gpointer data)
		, gpointer data);

extern void fx_tree_add_new_buddy(FxMain* fxmain , Contact* contact);

/*signal function*/

extern void fx_tree_text_cell_data_func(GtkTreeViewColumn *col 
		, GtkCellRenderer *renderer , GtkTreeModel *model
		, GtkTreeIter *iter , gpointer user_data);

extern void fx_tree_pixbuf_cell_data_func(GtkTreeViewColumn *col
		, GtkCellRenderer *renderer , GtkTreeModel *model
		, GtkTreeIter *iter , gpointer user_data);

extern void* fx_tree_update_portrait_thread_func(void* data);

extern gboolean fx_tree_on_rightbutton_click(GtkWidget* tree
		, GdkEventButton* event , gpointer data);

extern void fx_tree_on_double_click(GtkTreeView *treeview
		, GtkTreePath *path , GtkTreeViewColumn  *col
		, gpointer data);

extern void fx_tree_on_chatmenu_clicked(GtkWidget* widget , gpointer data);

extern void fx_tree_on_profilemenu_clicked(GtkWidget* widget , gpointer data);

extern void fx_tree_on_sendfile_clicked(GtkWidget* widget , gpointer data);

extern void fx_tree_on_historymenu_clicked(GtkWidget* widget , gpointer data);

extern void fx_tree_on_editmenu_clicked(GtkWidget* widget , gpointer data);

extern void fx_tree_on_deletemenu_clicked(GtkWidget* widget , gpointer data);

extern void fx_tree_on_reload_clicked(GtkWidget* widget , gpointer data);

extern void fx_tree_on_iconchange_clicked(GtkWidget* widget , gpointer data);

extern void fx_tree_on_gaddmenu_clicked(GtkWidget* widget , gpointer data);

extern void fx_tree_on_gdeletemenu_clicked(GtkWidget* widget , gpointer data);

extern void fx_tree_on_geditmenu_clicked(GtkWidget* widget , gpointer data);

extern void fx_tree_on_movemenu_clicked(GtkWidget* widget , gpointer data);

extern gboolean fx_tree_on_show_tooltip(GtkWidget* widget
		, int x , int y , gboolean keybord_mode
		, GtkTooltip* tip , gpointer data);
/*private*/

#endif
