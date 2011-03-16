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

FxEdit* fx_edit_new(FxMain* fxmain , GtkTreeIter iter , const char* userid)
{
	FxEdit* fxedit = (FxEdit*)malloc(sizeof(FxEdit));

	memset(fxedit , 0 , sizeof(FxEdit));
	fxedit->fxmain = fxmain;
	fxedit->iter = iter;
	strcpy(fxedit->userid , userid);
	return fxedit;
}

void fx_edit_initialize(FxEdit* fxedit)
{
	GdkPixbuf* pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"online.svg",
				   22 , 22,	NULL);
	fxedit->dialog = gtk_dialog_new();

	gtk_window_set_icon(GTK_WINDOW(fxedit->dialog) , pb);
	gtk_window_set_title(GTK_WINDOW(fxedit->dialog) , _("Edit note name"));
	gtk_window_set_modal(GTK_WINDOW(fxedit->dialog) , TRUE);

	gtk_dialog_set_has_separator(GTK_DIALOG(fxedit->dialog) , FALSE);
	gtk_window_set_resizable(GTK_WINDOW(fxedit->dialog) , FALSE);
	gtk_widget_set_usize(fxedit->dialog , 300 , 150);
	gtk_container_set_border_width(GTK_CONTAINER(fxedit->dialog) , 20);

	fxedit->remark_label = gtk_label_new(_("Please input note name:"));
	gtk_misc_set_alignment(GTK_MISC(fxedit->remark_label) , 0 , 0 );
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxedit->dialog)->vbox) , fxedit->remark_label);

	fxedit->remark_entry = gtk_entry_new();
	g_signal_connect(fxedit->remark_entry , "activate" , G_CALLBACK(fx_edit_on_ok_clicked) , fxedit);
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxedit->dialog)->vbox) , fxedit->remark_entry);
	
	fxedit->ok_button = gtk_button_new_with_label(_("OK"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxedit->dialog)->action_area) , fxedit->ok_button);
	g_signal_connect(fxedit->ok_button , "clicked" , G_CALLBACK(fx_edit_on_ok_clicked) , fxedit);

	fxedit->cancel_button = gtk_button_new_with_label(_("Cancel"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxedit->dialog)->action_area) , fxedit->cancel_button);
	g_signal_connect(fxedit->cancel_button , "clicked" , G_CALLBACK(fx_edit_on_cancel_clicked) , fxedit->dialog);
	gtk_widget_show_all(fxedit->dialog);
}

void fx_edit_free(FxEdit* fxedit)
{
	free(fxedit);
}

void fx_edit_on_ok_clicked(GtkWidget* UNUSED(widget), gpointer data)
{
	FxEdit* fxedit = (FxEdit*)data;
	FxMain* fxmain = fxedit->fxmain;
	User* user = fxmain->user;
	GtkTreeView* tree = GTK_TREE_VIEW(fxmain->mainPanel->treeView);
	GtkTreeModel* model = gtk_tree_view_get_model(tree);
	GtkTreeIter iter;
	Contact *contact;
	const char* name = gtk_entry_get_text(GTK_ENTRY(fxedit->remark_entry));
	if(*name == '\0')	return;

	if(fetion_contact_set_displayname(user , fxedit->userid , name) == 0) {
		contact = fetion_contact_list_find_by_userid(user->contactList, fxedit->userid);
		foreach_groupids(contact->groupids) {
			if(fx_tree_get_buddy_iter(model, group_id, fxedit->userid, &iter) == -1) continue;
			gtk_tree_store_set(GTK_TREE_STORE(model) , &iter
							 , B_NAME_COL			 , name
							 , -1);
		} end_groupids(contact->groupids)
		gtk_dialog_response(GTK_DIALOG(fxedit->dialog) , GTK_RESPONSE_OK);
	} else {
		gtk_dialog_response(GTK_DIALOG(fxedit->dialog) , GTK_RESPONSE_CANCEL);
	}
}

void fx_edit_on_cancel_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	GtkWidget* dialog = (GtkWidget*)data;

	gtk_dialog_response(GTK_DIALOG(dialog) , GTK_RESPONSE_CANCEL);
}
