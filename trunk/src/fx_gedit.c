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


FxGEdit* fx_gedit_new(FxMain* fxmain , GtkTreeIter iter , int groupid)
{
	FxGEdit* fxgedit = (FxGEdit*)malloc(sizeof(FxGEdit));

	memset(fxgedit , 0 , sizeof(FxGEdit));
	fxgedit->fxmain = fxmain;
	fxgedit->iter = iter;
	fxgedit->groupid = groupid;
	return fxgedit;
}

void fx_gedit_initialize(FxGEdit* fxgedit)
{
	fxgedit->dialog = gtk_dialog_new();
	GdkPixbuf* pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"online.svg",
				   22 ,22,NULL);

	gtk_window_set_icon(GTK_WINDOW(fxgedit->dialog) , pb);
	g_object_unref(pb);
	gtk_window_set_title(GTK_WINDOW(fxgedit->dialog) , _("Edit name of a group"));
	gtk_window_set_modal(GTK_WINDOW(fxgedit->dialog) , TRUE);

	gtk_dialog_set_has_separator(GTK_DIALOG(fxgedit->dialog) , FALSE);
	gtk_window_set_resizable(GTK_WINDOW(fxgedit->dialog) , FALSE);
	gtk_widget_set_usize(fxgedit->dialog , 300 , 150);
	gtk_container_set_border_width(GTK_CONTAINER(fxgedit->dialog) , 20);

	fxgedit->remark_label = gtk_label_new(_("Please entry new name of the group:"));
	gtk_misc_set_alignment(GTK_MISC(fxgedit->remark_label) , 0 , 0 );
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxgedit->dialog)->vbox) , fxgedit->remark_label);

	fxgedit->remark_entry = gtk_entry_new();
	g_signal_connect(fxgedit->remark_entry , "activate" , G_CALLBACK(fx_gedit_on_ok_clicked) , fxgedit);
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxgedit->dialog)->vbox) , fxgedit->remark_entry);
	
	fxgedit->ok_button = gtk_button_new_with_label(_("OK"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxgedit->dialog)->action_area) , fxgedit->ok_button);
	g_signal_connect(fxgedit->ok_button , "clicked" , G_CALLBACK(fx_gedit_on_ok_clicked) , fxgedit);

	fxgedit->cancel_button = gtk_button_new_with_label(_("Cancel"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxgedit->dialog)->action_area) , fxgedit->cancel_button);
	g_signal_connect(fxgedit->cancel_button , "clicked" , G_CALLBACK(fx_gedit_on_cancel_clicked) , fxgedit->dialog);
	gtk_widget_show_all(fxgedit->dialog);
}

void fx_gedit_free(FxGEdit* fxgedit)
{
	free(fxgedit);
}
void fx_gedit_on_ok_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxGEdit* fxgedit = (FxGEdit*)data;
	FxMain* fxmain = fxgedit->fxmain;
	User* user = fxmain->user;
	GtkTreeView* tree = GTK_TREE_VIEW(fxmain->mainPanel->treeView);
	GtkTreeModel* model = gtk_tree_view_get_model(tree);
	GtkTreeIter iter = fxgedit->iter;
	int groupid = fxgedit->groupid;
	const char* name = gtk_entry_get_text(GTK_ENTRY(fxgedit->remark_entry));

	if(name == NULL)
		return;
	if(fetion_buddylist_edit(user , groupid , name) > 0)
	{
		gtk_tree_store_set(GTK_TREE_STORE(model) , &iter
						 , G_NAME_COL			 , name
						 , -1);
		gtk_dialog_response(GTK_DIALOG(fxgedit->dialog) , GTK_RESPONSE_OK);
	}
	else
	{
		gtk_dialog_response(GTK_DIALOG(fxgedit->dialog) , GTK_RESPONSE_CANCEL);
	}
}

void fx_gedit_on_cancel_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	GtkWidget* dialog = (GtkWidget*)data;

	gtk_dialog_response(GTK_DIALOG(dialog) , GTK_RESPONSE_CANCEL);
}
