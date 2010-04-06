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

FxEdit* fx_edit_new(FxMain* fxmain , GtkTreeIter iter , const char* userid)
{
	FxEdit* fxedit = (FxEdit*)malloc(sizeof(FxEdit));

	DEBUG_FOOTPRINT();

	memset(fxedit , 0 , sizeof(FxEdit));
	fxedit->fxmain = fxmain;
	fxedit->iter = iter;
	strcpy(fxedit->userid , userid);
	return fxedit;
}

void fx_edit_initialize(FxEdit* fxedit)
{
	GdkPixbuf* pb = gdk_pixbuf_new_from_file(SKIN_DIR"user_online.png" , NULL);
	fxedit->dialog = gtk_dialog_new();

	DEBUG_FOOTPRINT();

	gtk_window_set_icon(GTK_WINDOW(fxedit->dialog) , pb);
	gtk_window_set_title(GTK_WINDOW(fxedit->dialog) , "修改备注名称");
	gtk_window_set_modal(GTK_WINDOW(fxedit->dialog) , TRUE);

	gtk_dialog_set_has_separator(GTK_DIALOG(fxedit->dialog) , FALSE);
	gtk_window_set_resizable(GTK_WINDOW(fxedit->dialog) , FALSE);
	gtk_widget_set_usize(fxedit->dialog , 300 , 150);
	gtk_container_set_border_width(GTK_CONTAINER(fxedit->dialog) , 20);

	fxedit->remark_label = gtk_label_new("请输入备注名称:");
	gtk_misc_set_alignment(GTK_MISC(fxedit->remark_label) , 0 , 0 );
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxedit->dialog)->vbox) , fxedit->remark_label);

	fxedit->remark_entry = gtk_entry_new();
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxedit->dialog)->vbox) , fxedit->remark_entry);
	
	fxedit->ok_button = gtk_button_new_with_label("确定");
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxedit->dialog)->action_area) , fxedit->ok_button);
	g_signal_connect(fxedit->ok_button , "clicked" , G_CALLBACK(fx_edit_on_ok_clicked) , fxedit);

	fxedit->cancel_button = gtk_button_new_with_label("取消");
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxedit->dialog)->action_area) , fxedit->cancel_button);
	g_signal_connect(fxedit->cancel_button , "clicked" , G_CALLBACK(fx_edit_on_cancel_clicked) , fxedit->dialog);
	gtk_widget_show_all(fxedit->dialog);
	gtk_widget_hide(fxedit->dialog);
}

void fx_edit_free(FxEdit* fxedit)
{
	DEBUG_FOOTPRINT();
	free(fxedit);
}
void fx_edit_on_ok_clicked(GtkWidget* widget , gpointer data)
{
	FxEdit* fxedit = (FxEdit*)data;
	FxMain* fxmain = fxedit->fxmain;
	User* user = fxmain->user;
	GtkTreeView* tree = GTK_TREE_VIEW(fxmain->mainPanel->treeView);
	GtkTreeModel* model = gtk_tree_view_get_model(tree);
	GtkTreeIter iter = fxedit->iter;
	const char* name = gtk_entry_get_text(GTK_ENTRY(fxedit->remark_entry));

	DEBUG_FOOTPRINT();

	if(strlen(name) == 0)
		return;
	if(fetion_contact_set_displayname(user , fxedit->userid , name) > 0)
	{
		gtk_tree_store_set(GTK_TREE_STORE(model) , &iter
						 , B_NAME_COL			 , name
						 , -1);
		gtk_dialog_response(GTK_DIALOG(fxedit->dialog) , GTK_RESPONSE_OK);
	}
	else
	{
		gtk_dialog_response(GTK_DIALOG(fxedit->dialog) , GTK_RESPONSE_CANCEL);
	}
}

void fx_edit_on_cancel_clicked(GtkWidget* widget , gpointer data)
{
	GtkWidget* dialog = (GtkWidget*)data;

	DEBUG_FOOTPRINT();

	gtk_dialog_response(GTK_DIALOG(dialog) , GTK_RESPONSE_CANCEL);
}