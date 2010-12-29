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

#ifndef FX_TREE_H
#define FX_TREE_H

#define PG_PORTRAIT_SIZE 40

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
	B_CARRIER_COL ,
	B_STATE_COL , 
	B_IDENTITY_COL ,
	B_RELATIONSTATUS_COL ,
	B_CARRIERSTATUS_COL , 
	B_SERVICESTATUS_COL ,
	B_SIZE_COL ,
	B_IMAGE_CHANGED_COL ,
	B_IMAGE_SET_COL ,
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
	G_NULL7_COL ,
	G_NULL8_COL ,
	G_NULL9_COL ,
	G_NULL10_COL ,
	G_ALL_COUNT_COL , 
	G_ONLINE_COUNT_COL ,
	G_ID_COL ,
	G_NULL11_COL
} GroupCols;

typedef enum
{
	PG_PIXBUF_COL = 0 ,
	PG_TEXT_COL ,
	PG_CREATETIME_COL , 
	PG_URI_COL ,
	PG_NAME_COL ,
	PG_IDENTITY_COL ,
	PG_CCOUNT_COL ,
	PG_LCOUNT_COL ,
	PG_GRANK_COL , 
	PG_MRANK_COL ,
	PG_COL_NUM

} PGGroupCols;

typedef struct tree_args {
	FxMain* fxmain;
	GtkTreeIter iter;
	char s[256];
	int i;
} Args;

extern Args* fx_args_new(FxMain* fxmain , GtkTreeIter iter , const char* ss , int ii);

extern FxTree* fx_tree_new();

extern void fx_tree_initilize(FxMain* fxmain);

extern void fx_tree_show(FxMain *fxmain);

extern void fx_tree_free(FxTree* fxtree);

extern void fx_tree_move_to_the_first(GtkTreeModel* model , GtkTreeIter* iter);

extern void fx_tree_move_to_the_last(GtkTreeModel* model , GtkTreeIter* iter);

extern void fx_tree_bind_pg_data(FxMain *fxmain);

extern int fx_tree_get_group_iter_by_id(GtkTreeModel* model
		, int id , GtkTreeIter* iter);

extern int fx_tree_get_buddy_iter_by_userid(GtkTreeModel* model
		, const char* userid , GtkTreeIter* iter);

extern FxPGGroup *pg_create_window(FxMain *fxmain , const char *pguri);

extern void fx_tree_update_portrait(FxMain *fxmain);
/*private*/

#endif
