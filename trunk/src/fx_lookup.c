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

FxLookup* fx_lookup_new(FxMain* fxmain)
{
	FxLookup *fxlookup = (FxLookup*)malloc(sizeof(FxLookup));

	DEBUG_FOOTPRINT();

	memset(fxlookup , 0 , sizeof(fxlookup));
	fxlookup->fxmain = fxmain;
	return fxlookup;
}

void fx_lookup_initialize(FxLookup* fxlookup)
{
	fxlookup->dialog = gtk_dialog_new();
	GdkPixbuf* pb = gdk_pixbuf_new_from_file(SKIN_DIR"find.png" , NULL);

	DEBUG_FOOTPRINT();

	gtk_window_set_icon(GTK_WINDOW(fxlookup->dialog) , pb);
	gtk_window_set_title(GTK_WINDOW(fxlookup->dialog)
			, _("View information of any user (attribution etc)"));
	gtk_window_set_modal(GTK_WINDOW(fxlookup->dialog) , TRUE);

	gtk_dialog_set_has_separator(GTK_DIALOG(fxlookup->dialog) , FALSE);
	gtk_window_set_resizable(GTK_WINDOW(fxlookup->dialog) , FALSE);
	gtk_widget_set_usize(fxlookup->dialog , 300 , 150);
	gtk_container_set_border_width(GTK_CONTAINER(fxlookup->dialog) , 20);

	fxlookup->remark_label = gtk_label_new(_("Please input the user's phone number or fetion number:"));
	gtk_misc_set_alignment(GTK_MISC(fxlookup->remark_label) , 0 , 0 );
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxlookup->dialog)->vbox) , fxlookup->remark_label);

	fxlookup->remark_entry = gtk_entry_new();
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxlookup->dialog)->vbox) , fxlookup->remark_entry);

	fxlookup->ok_button = gtk_button_new_with_label(_("Find"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxlookup->dialog)->action_area) , fxlookup->ok_button);
	g_signal_connect(fxlookup->ok_button , "clicked" , G_CALLBACK(fx_lookup_on_ok_clicked) , fxlookup);

	fxlookup->cancel_button = gtk_button_new_with_label(_("Cancel"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxlookup->dialog)->action_area) , fxlookup->cancel_button);
	g_signal_connect(fxlookup->cancel_button , "clicked" , G_CALLBACK(fx_lookup_on_cancel_clicked) , fxlookup->dialog);
	gtk_widget_show_all(fxlookup->dialog);
	gtk_widget_hide(fxlookup->dialog);
}
void* fx_lookup_ok_thread(void* data)
{
	FxLookup *fxlookup = (FxLookup*)data;
	User* user = fxlookup->fxmain->user;
	const char* mobileno;
	Contact* contact;
	FxLookupres* fxlookupres;
	GtkWidget *dialog;

	DEBUG_FOOTPRINT();

	mobileno = gtk_entry_get_text(GTK_ENTRY(fxlookup->remark_entry));
	if(strlen(mobileno) != 11 && strlen(mobileno) != 9)
	{
		debug_error("Please input a correct mobileno or fetionno");
		return NULL;
	}
	contact = fetion_contact_get_contact_info_by_no(user , mobileno , strlen(mobileno) == 11 ? MOBILE_NO : FETION_NO);
	if(contact == NULL)
	{
		gdk_threads_enter();
		dialog = gtk_message_dialog_new(GTK_WINDOW(fxlookup->fxmain->window),
										GTK_DIALOG_DESTROY_WITH_PARENT,
										GTK_MESSAGE_WARNING,
										GTK_BUTTONS_OK,
										_("Query failed, please input vaild phone number of China Mobile"));
		gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		gdk_threads_leave();
		return NULL;
	}
	gdk_threads_enter();
	gtk_dialog_response(GTK_DIALOG(fxlookup->dialog) , GTK_RESPONSE_OK);
	fxlookupres = fx_lookupres_new(fxlookup->fxmain , contact);	
	fx_lookupres_initialize(fxlookupres);
	gtk_dialog_run(GTK_DIALOG(fxlookupres->dialog));

	free(contact);

	gtk_widget_destroy(fxlookupres->dialog);
	gdk_threads_leave();	
	return NULL;
}
void fx_lookup_on_ok_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	DEBUG_FOOTPRINT();

	g_thread_create(fx_lookup_ok_thread , data , FALSE , NULL);
}
void fx_lookup_on_cancel_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	DEBUG_FOOTPRINT();

	gtk_dialog_response(GTK_DIALOG(data) , GTK_RESPONSE_CANCEL);
}
