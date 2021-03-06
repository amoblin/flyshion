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

static void fx_add_group_on_ok_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxAddGroup* fxaddgroup = (FxAddGroup*)data;
	User* user = fxaddgroup->fxmain->user;
	const char* text = gtk_entry_get_text(GTK_ENTRY(fxaddgroup->add_entry));
	GtkTreeView* tree = GTK_TREE_VIEW(fxaddgroup->fxmain->mainPanel->treeView);
	GtkTreeModel* model = gtk_tree_view_get_model(tree);
	GtkTreeIter iter;
	int groupid;
	
	groupid = fetion_buddylist_create(user , text);
	if(groupid > 0 )
	{
		gtk_tree_store_append(GTK_TREE_STORE(model) , &iter , NULL);
		gtk_tree_store_set(GTK_TREE_STORE(model) , &iter
						 , G_NAME_COL , text 
						 , G_ID_COL , groupid
						 , G_ALL_COUNT_COL , 0
						 , G_ONLINE_COUNT_COL , 0
						 , -1);
		gtk_tree_view_set_model(tree , model);
	}
	gtk_dialog_response(GTK_DIALOG(fxaddgroup->dialog) , GTK_RESPONSE_CANCEL);
}

static void fx_add_group_on_cancel_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	gtk_dialog_response(GTK_DIALOG(data) , GTK_RESPONSE_CANCEL);
}

FxAddGroup* fx_add_group_new(FxMain* fxmain)
{
	FxAddGroup* fxaddgroup = (FxAddGroup*)malloc(sizeof(FxAddGroup));

	memset(fxaddgroup , 0 , sizeof(FxAddGroup));
	fxaddgroup->fxmain = fxmain;
	return fxaddgroup;
}

void fx_add_group_initialize(FxAddGroup* fxaddgroup)
{
	GdkPixbuf* pb;

	fxaddgroup->dialog = gtk_dialog_new();
	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"online.svg" , 22 , 22 , NULL);
	gtk_dialog_set_has_separator(GTK_DIALOG(fxaddgroup->dialog) , FALSE);
	gtk_window_set_resizable(GTK_WINDOW(fxaddgroup->dialog) , FALSE);
	gtk_widget_set_usize(fxaddgroup->dialog , 300 , 150);
	gtk_window_set_icon(GTK_WINDOW(fxaddgroup->dialog) , pb);
	g_object_unref(pb);
	gtk_window_set_title(GTK_WINDOW(fxaddgroup->dialog) , _("Add a buddy group"));
	gtk_container_set_border_width(GTK_CONTAINER(fxaddgroup->dialog) , 20);

	fxaddgroup->add_label = gtk_label_new(_("Input the name of the new group:"));
	gtk_misc_set_alignment(GTK_MISC(fxaddgroup->add_label) , 0 , 0 );
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxaddgroup->dialog)->vbox) , fxaddgroup->add_label);

	fxaddgroup->add_entry = gtk_entry_new();
	g_signal_connect(fxaddgroup->add_entry , "activate" , G_CALLBACK(fx_add_group_on_ok_clicked) , fxaddgroup);
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxaddgroup->dialog)->vbox) , fxaddgroup->add_entry);
	
	fxaddgroup->ok_button = gtk_button_new_with_label(_("OK"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxaddgroup->dialog)->action_area) , fxaddgroup->ok_button);
	g_signal_connect(fxaddgroup->ok_button , "clicked" , G_CALLBACK(fx_add_group_on_ok_clicked) , fxaddgroup);

	fxaddgroup->cancel_button = gtk_button_new_with_label(_("Cancel"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxaddgroup->dialog)->action_area) , fxaddgroup->cancel_button);
	g_signal_connect(fxaddgroup->cancel_button , "clicked" , G_CALLBACK(fx_add_group_on_cancel_clicked) , fxaddgroup->dialog);
	gtk_widget_show_all(fxaddgroup->dialog);
}

void fx_add_group_free(FxAddGroup* fxaddgroup)
{
	free(fxaddgroup);
}
