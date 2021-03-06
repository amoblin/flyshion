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


FxBottom* fx_bottom_new()
{
	FxBottom* fxbottom = (FxBottom*)malloc(sizeof(FxBottom));

	memset(fxbottom , 0 , sizeof(FxBottom));
	return fxbottom;
}

static void 
fx_bottom_on_directsms_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	FxDSMS *fxdsms = fx_dsms_new(fxmain);

	if(fxmain->user->carrierStatus == CARRIER_STATUS_DOWN){
		fx_util_popup_warning(fxmain , SERVICE_DOWN_MESSAGE);
		return;
	}

	if(fx_conn_check_action(fxmain))
		fx_dsms_initialize(fxdsms);
}

static void
fx_bottom_on_pggroup_clicked(GtkWidget *widget , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	PGGroup *pg = fxmain->user->pggroup;
	FxTree *fxtree = fxmain->mainPanel;

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))){
		if(pg)
			gtk_widget_show(fxtree->pgScrollWindow);
		else
			gtk_widget_show(fxtree->noLabelScrollWindow);
		gtk_widget_hide(fxtree->scrollWindow);
	}else{
		gtk_widget_hide(fxtree->pgScrollWindow);
		gtk_widget_hide(fxtree->noLabelScrollWindow);
		gtk_widget_show(fxtree->scrollWindow);
	}
}

static void
fx_bottom_on_help_clicked(GtkWidget* UNUSED(widget) , gpointer UNUSED(data))
{
	show_about();
}
void fx_bottom_initialize(FxMain* fxmain)
{
	FxBottom *fxbottom = fx_bottom_new();
	GtkWidget *icon;
	GdkPixbuf *pb;
	fxmain->bottomPanel = fxbottom;
	User *user = fxmain->user;
	GtkWidget *mainbox = fxmain->mainbox;

	fxbottom->toolbar = gtk_toolbar_new();
	gtk_box_pack_start(GTK_BOX(mainbox) , fxbottom->toolbar , FALSE , FALSE , 0);
	icon = gtk_image_new_from_file(SKIN_DIR"options.png");
	gtk_toolbar_append_item(GTK_TOOLBAR(fxbottom->toolbar)
						  , NULL , _("Personal Settings") , NULL , icon
						  , G_CALLBACK(fx_bottom_on_setting_clicked)
						  , fxmain );

	gtk_toolbar_set_style(GTK_TOOLBAR(fxbottom->toolbar) , GTK_TOOLBAR_ICONS);

	icon = gtk_image_new_from_file(SKIN_DIR"add.png");
	gtk_toolbar_append_item(GTK_TOOLBAR(fxbottom->toolbar)
						  , NULL , _("Add Buddy") , NULL , icon
						  , G_CALLBACK(fx_bottom_on_addfriend_clicked)
						  , fxmain);
	if(user->boundToMobile == BOUND_MOBILE_ENABLE){
		pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"myselfsms.png" , 16 , 16 , NULL);
		icon = gtk_image_new_from_pixbuf(pb);
		g_object_unref(pb);
		gtk_toolbar_append_item(GTK_TOOLBAR(fxbottom->toolbar)
							  , NULL , _("Send a message to myself") , NULL , icon
							  , G_CALLBACK(fx_bottom_on_sendtome_clicked)
							  , fxmain);
		pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"groupsend.png" , 16 , 16 , NULL);
		icon = gtk_image_new_from_pixbuf(pb);
		g_object_unref(pb);
		gtk_toolbar_append_item(GTK_TOOLBAR(fxbottom->toolbar)
							  , NULL , _("SMS to many") , NULL , icon
							  , G_CALLBACK(fx_bottom_on_sendtomany_clicked)
							  , fxmain);
		pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"directsms.png" , 16 , 16 , NULL);
		icon = gtk_image_new_from_pixbuf(pb);
		g_object_unref(pb);
		gtk_toolbar_append_item(GTK_TOOLBAR(fxbottom->toolbar)
							  , NULL , _("SMS directly") , NULL , icon
							  , G_CALLBACK(fx_bottom_on_directsms_clicked)
							  , fxmain);
	}

	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"pggroup.png" , 16 , 16 , NULL);
	icon = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_toolbar_append_element(GTK_TOOLBAR(fxbottom->toolbar) , GTK_TOOLBAR_CHILD_TOGGLEBUTTON , NULL
						  , NULL , _("Fetion Group")
						  , NULL , icon , G_CALLBACK(fx_bottom_on_pggroup_clicked) , fxmain);

	icon = gtk_image_new_from_file(SKIN_DIR"find.png");
	gtk_toolbar_append_item(GTK_TOOLBAR(fxbottom->toolbar)
						  , NULL , _("View information of any user (attribution etc)") , NULL , icon
						  , G_CALLBACK(fx_bottom_on_lookup_clicked)
						  , fxmain);
	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"home.png" , 16 , 16 , NULL);
	icon = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_toolbar_append_item(GTK_TOOLBAR(fxbottom->toolbar)
						  , NULL , _("Show OpenFetion About dialog")
						  , NULL , icon , G_CALLBACK(fx_bottom_on_help_clicked) , NULL);
}

void fx_bottom_show(FxMain *fxmain)
{
	FxBottom *fxbottom = fxmain->bottomPanel;
	gtk_widget_show_all(fxbottom->toolbar);
}

void fx_bottom_free(FxBottom* fxbottom)
{
	free(fxbottom);
}
void fx_bottom_on_setting_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	FxSet *fxset = fx_set_new(fxmain);
	fx_set_initialize(fxset);
}

void fx_bottom_on_sendtome_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxMyself* fxmyself = fx_myself_new(fxmain);

	if(fxmain->user->carrierStatus == CARRIER_STATUS_DOWN){
		fx_util_popup_warning(fxmain , SERVICE_DOWN_MESSAGE);
		return;
	}

	if(fx_conn_check_action(fxmain)){
		fx_myself_initialize(fxmyself);
	}
}

void fx_bottom_on_sendtomany_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxMany* fxmany = fx_many_new(fxmain);

	if(fxmain->user->carrierStatus == CARRIER_STATUS_DOWN){
		fx_util_popup_warning(fxmain , SERVICE_DOWN_MESSAGE);
		return;
	}

	if(fx_conn_check_action(fxmain)){
		fx_many_initialize(fxmany);
	}
}

void fx_bottom_on_addfriend_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxAddbuddy *fxaddbuddy = fx_addbuddy_new(fxmain);

	if(fx_conn_check_action(fxmain)){
		fx_addbuddy_initialize(fxaddbuddy);
		gtk_dialog_run(GTK_DIALOG(fxaddbuddy->dialog));
		gtk_widget_destroy(fxaddbuddy->dialog);
		free(fxaddbuddy);
	}
}
void fx_bottom_on_lookup_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxLookup* fxlookup = fx_lookup_new(fxmain);

	if(fx_conn_check_action(fxmain)){
		fx_lookup_initialize(fxlookup);
		gtk_dialog_run(GTK_DIALOG(fxlookup->dialog));
		gtk_widget_destroy(fxlookup->dialog);
		free(fxlookup);
	}
}
