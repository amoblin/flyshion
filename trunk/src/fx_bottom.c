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


FxBottom* fx_bottom_new()
{
	FxBottom* fxbottom = (FxBottom*)malloc(sizeof(FxBottom));

	DEBUG_FOOTPRINT();

	memset(fxbottom , 0 , sizeof(FxBottom));
	return fxbottom;
}

static void 
fx_bottom_on_directsms_clicked(GtkWidget *widget , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	FxDSMS *fxdsms = fx_dsms_new(fxmain);

	DEBUG_FOOTPRINT();

	if(fxmain->user->carrierStatus == CARRIER_STATUS_DOWN){
		fx_util_popup_warning(fxmain , SERVICE_DOWN_MESSAGE);
		return;
	}
	fx_dsms_initialize(fxdsms);
}

static void
fx_bottom_on_help_clicked(GtkWidget* widget , gpointer data)
{
	if(fork() == 0)
	{
		execlp("firefox" , "firefox" , "http://basiccoder.com/openfetion" , (char**)NULL);
	}
}
void fx_bottom_initialize(FxMain* fxmain)
{
	FxBottom* fxbottom = fx_bottom_new();
	GtkWidget* icon;
	fxmain->bottomPanel = fxbottom;
	User *user = fxmain->user;
	GtkWidget* mainbox = fxmain->mainbox;

	DEBUG_FOOTPRINT();

	fxbottom->toolbar = gtk_toolbar_new();
	gtk_box_pack_start(GTK_BOX(mainbox) , fxbottom->toolbar , FALSE , FALSE , 0);
	icon = gtk_image_new_from_file(SKIN_DIR"options.png");
	gtk_toolbar_append_item(GTK_TOOLBAR(fxbottom->toolbar)
						  , NULL , "设置" , NULL , icon
						  , G_CALLBACK(fx_bottom_on_setting_clicked)
						  , fxmain );

	gtk_toolbar_set_style(GTK_TOOLBAR(fxbottom->toolbar) , GTK_TOOLBAR_ICONS);

	icon = gtk_image_new_from_file(SKIN_DIR"addbuddy.png");
	gtk_toolbar_append_item(GTK_TOOLBAR(fxbottom->toolbar)
						  , "添加" , "添加好友" , NULL , icon
						  , G_CALLBACK(fx_bottom_on_addfriend_clicked)
						  , fxmain);
	if(user->boundToMobile == BOUND_MOBILE_ENABLE){
		icon = gtk_image_new_from_file(SKIN_DIR"myselfsms.png");
		gtk_toolbar_append_item(GTK_TOOLBAR(fxbottom->toolbar)
							  , "自己" , "给自己发短信" , NULL , icon
							  , G_CALLBACK(fx_bottom_on_sendtome_clicked)
							  , fxmain);
		icon = gtk_image_new_from_file(SKIN_DIR"groupsend.png");
		gtk_toolbar_append_item(GTK_TOOLBAR(fxbottom->toolbar)
							  , "群发" , "群发短信" , NULL , icon
							  , G_CALLBACK(fx_bottom_on_sendtomany_clicked)
							  , fxmain);
		icon = gtk_image_new_from_file(SKIN_DIR"directsms.png");
		gtk_toolbar_append_item(GTK_TOOLBAR(fxbottom->toolbar)
							  , "短信" , "发送直接短信" , NULL , icon
							  , G_CALLBACK(fx_bottom_on_directsms_clicked)
							  , fxmain);
	}
	icon = gtk_image_new_from_file(SKIN_DIR"find.png");
	gtk_toolbar_append_item(GTK_TOOLBAR(fxbottom->toolbar)
						  , "查询" , "查询任意移动用户信息(归属地等)" , NULL , icon
						  , G_CALLBACK(fx_bottom_on_lookup_clicked)
						  , fxmain);
	icon = gtk_image_new_from_file(SKIN_DIR"home.png");
	gtk_toolbar_append_item(GTK_TOOLBAR(fxbottom->toolbar)
						  , "帮助" , "到Openfetion页面获取帮助或外馈意见"
						  , NULL , icon , G_CALLBACK(fx_bottom_on_help_clicked) , NULL);
	gtk_widget_show_all(fxbottom->toolbar);
}

void fx_bottom_free(FxBottom* fxbottom)
{
	DEBUG_FOOTPRINT();
	free(fxbottom);
}
void fx_bottom_on_setting_clicked(GtkWidget* widget , gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	FxSet *fxset = fx_set_new(fxmain);
	fx_set_initialize(fxset);

	DEBUG_FOOTPRINT();

	gtk_dialog_run(GTK_DIALOG(fxset->dialog));
	gtk_widget_destroy(fxset->dialog);
}

void fx_bottom_on_sendtome_clicked(GtkWidget* widget , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxMyself* fxmyself = fx_myself_new(fxmain);

	DEBUG_FOOTPRINT();

	if(fxmain->user->carrierStatus == CARRIER_STATUS_DOWN){
		fx_util_popup_warning(fxmain , SERVICE_DOWN_MESSAGE);
		return;
	}

	fx_myself_initialize(fxmyself);
	gtk_dialog_run(GTK_DIALOG(fxmyself->dialog));
	gtk_widget_destroy(fxmyself->dialog);
}

void fx_bottom_on_sendtomany_clicked(GtkWidget* widget , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxMany* fxmany = fx_many_new(fxmain);

	DEBUG_FOOTPRINT();

	if(fxmain->user->carrierStatus == CARRIER_STATUS_DOWN){
		fx_util_popup_warning(fxmain , SERVICE_DOWN_MESSAGE);
		return;
	}

	fx_many_initialize(fxmany);
	gtk_dialog_run(GTK_DIALOG(fxmany->dialog));
	gtk_widget_destroy(fxmany->dialog);
	free(fxmany);
}

void fx_bottom_on_addfriend_clicked(GtkWidget* widget , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxAddbuddy *fxaddbuddy = fx_addbuddy_new(fxmain);

	DEBUG_FOOTPRINT();

	fx_addbuddy_initialize(fxaddbuddy);
	gtk_dialog_run(GTK_DIALOG(fxaddbuddy->dialog));
	gtk_widget_destroy(fxaddbuddy->dialog);
	free(fxaddbuddy);
}
void fx_bottom_on_lookup_clicked(GtkWidget* widget , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	FxLookup* fxlookup = fx_lookup_new(fxmain);

	DEBUG_FOOTPRINT();

	fx_lookup_initialize(fxlookup);
	gtk_dialog_run(GTK_DIALOG(fxlookup->dialog));
	gtk_widget_destroy(fxlookup->dialog);
	free(fxlookup);
}
