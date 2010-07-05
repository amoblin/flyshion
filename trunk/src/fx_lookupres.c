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


FxLookupres* fx_lookupres_new(FxMain* fxmain , Contact* contact)
{
	FxLookupres *fxlookupres = (FxLookupres*)malloc(sizeof(FxLookupres));

	DEBUG_FOOTPRINT();

	memset(fxlookupres , 0 , sizeof(FxLookupres));
	fxlookupres->fxmain = fxmain;
	fxlookupres->contact = contact;
	return fxlookupres;
}
void fx_lookupres_bind(FxLookupres* fxlookupres)
{
	Contact* contact;
	GdkPixbuf* pb = NULL;
	char portrait[100];
	char *sid = NULL;
	char *res = NULL;
	User *user = fxlookupres->fxmain->user;
	Config* config = user->config;

	DEBUG_FOOTPRINT();
		
	contact = fxlookupres->contact;
	if(contact->nickname != NULL )
		gtk_entry_set_text(GTK_ENTRY(fxlookupres->nick_entry) , contact->nickname);

	gtk_entry_set_text(GTK_ENTRY(fxlookupres->gender_entry) ,
						contact->gender == 1 ? "男" : ( contact->gender == 2 ? "女" : "保密"));

	if(contact->mobileno != NULL)
		gtk_entry_set_text(GTK_ENTRY(fxlookupres->mno_entry) , contact->mobileno);

	if(contact->impression != NULL)
		gtk_entry_set_text(GTK_ENTRY(fxlookupres->impre_entry) , contact->impression);

	gtk_entry_set_text(GTK_ENTRY(fxlookupres->sid_entry) , contact->sId);

	if(contact->country != NULL)
		gtk_entry_set_text(GTK_ENTRY(fxlookupres->nation_entry)
					, strcmp(contact->country , "CN") == 0 ? "中国" : contact->country);

	if(contact->province != NULL)
	{
		res = fetion_config_get_province_name(contact->province);
		gtk_entry_set_text(GTK_ENTRY(fxlookupres->province_entry) , res == NULL ? "未知" : res);
		free(res);
	}

	if(contact->city != NULL)
	{
		res = fetion_config_get_city_name(contact->province , contact->city);
		gtk_entry_set_text(GTK_ENTRY(fxlookupres->city_entry) , res == NULL ? "未知" : res);
		free(res);
	}

	bzero(portrait , sizeof(portrait));
	sid = fetion_sip_get_sid_by_sipuri(contact->sipuri);
	sprintf(portrait , "%s/%s.jpg" , config->iconPath , sid);
	free(sid);

	if(contact->birthday != NULL)
		gtk_entry_set_text(GTK_ENTRY(fxlookupres->birth_entry) , contact->birthday);

	if(contact->sipuri != NULL && strlen(contact->sipuri) != 0)
	{
		fetion_user_download_portrait(user , contact->sipuri);
		pb = gdk_pixbuf_new_from_file_at_size(portrait , 90 , 90 , NULL);
	}
	if(pb != NULL)
	{
		gtk_image_set_from_pixbuf(GTK_IMAGE(fxlookupres->image) , pb);
		g_object_unref(pb);
	}
	
}
void fx_lookupres_initialize(FxLookupres* fxlookupres)
{
	GtkWidget* box = NULL ;
	GtkWidget* inbox = NULL ;
	GdkPixbuf* pb = NULL;
	FxMain* fxmain = fxlookupres->fxmain;

	DEBUG_FOOTPRINT();

	fxlookupres->dialog = gtk_dialog_new_with_buttons ("查看用户信息"
									, GTK_WINDOW(fxmain->window)
									, GTK_DIALOG_DESTROY_WITH_PARENT
									, NULL);

	gtk_dialog_set_has_separator(GTK_DIALOG(fxlookupres->dialog) , FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(fxlookupres->dialog) , 10);

	gtk_widget_set_usize(GTK_WIDGET(fxlookupres->dialog) , 500 , 340);

	gtk_window_set_resizable(GTK_WINDOW(fxlookupres->dialog) , FALSE);

	box = gtk_table_new(6 , 3 , TRUE);
	gtk_table_set_row_spacings(GTK_TABLE(box) , 2);
	gtk_table_set_col_spacings(GTK_TABLE(box) , 2);

	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxlookupres->dialog)->vbox) , box);

	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.png" , 90 , 90 , NULL);

	fxlookupres->image = gtk_image_new_from_pixbuf(pb);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxlookupres->image , 0 , 1 , 0 , 4);

	fxlookupres->sid_label = gtk_label_new("飞信号:");
	gtk_misc_set_alignment(GTK_MISC(fxlookupres->sid_label) , 0 , 0);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxlookupres->sid_label , 1 , 2 , 0 , 1);

	fxlookupres->sid_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxlookupres->sid_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxlookupres->sid_entry , 1 , 2 , 1 , 2);

	fxlookupres->gender_label = gtk_label_new("性别:");
	gtk_misc_set_alignment(GTK_MISC(fxlookupres->gender_label) , 0 , 0);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxlookupres->gender_label , 2 , 3 , 0 , 1);

	fxlookupres->gender_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxlookupres->gender_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxlookupres->gender_entry , 2 , 3 , 1 , 2);

	fxlookupres->mno_label = gtk_label_new("手机号:");
	gtk_misc_set_alignment(GTK_MISC(fxlookupres->mno_label) , 0 , 0);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxlookupres->mno_label , 1 , 2 , 2 , 3);

	fxlookupres->mno_entry = gtk_entry_new();
	gtk_widget_set_usize(fxlookupres->mno_entry , 150 , 25);
	gtk_entry_set_editable(GTK_ENTRY(fxlookupres->mno_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxlookupres->mno_entry , 1 , 2 , 3 , 4);

	fxlookupres->nick_label = gtk_label_new("昵称:");
	gtk_misc_set_alignment(GTK_MISC(fxlookupres->nick_label) , 0 , 0);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxlookupres->nick_label , 2 , 3 , 2 , 3);

	fxlookupres->nick_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxlookupres->nick_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxlookupres->nick_entry , 2 , 3 , 3 , 4);

	fxlookupres->impre_label = gtk_label_new("心情:");
	gtk_misc_set_alignment(GTK_MISC(fxlookupres->impre_label) , 0 , 0);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxlookupres->impre_label , 0 , 3 , 4, 5);

	fxlookupres->impre_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxlookupres->impre_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxlookupres->impre_entry , 0 , 3 , 5 , 6);

	inbox = gtk_table_new( 4 , 2 , TRUE);
	gtk_table_set_row_spacings(GTK_TABLE(inbox) , 2);
	gtk_table_set_col_spacings(GTK_TABLE(inbox) , 2);
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxlookupres->dialog)->vbox) , inbox);

	fxlookupres->birth_label = gtk_label_new("生日:");
	gtk_misc_set_alignment(GTK_MISC(fxlookupres->birth_label) , 0 , 0 );
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxlookupres->birth_label , 0 , 1 , 0 , 1);

	fxlookupres->birth_entry = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxlookupres->birth_entry , 0 , 1 , 1 , 2);
	gtk_entry_set_editable(GTK_ENTRY(fxlookupres->birth_entry) , FALSE);

	fxlookupres->nation_label = gtk_label_new("国家:");
	gtk_misc_set_alignment(GTK_MISC(fxlookupres->nation_label) , 0 , 0 );
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxlookupres->nation_label , 1 , 2 , 0 , 1);

	fxlookupres->nation_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxlookupres->nation_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxlookupres->nation_entry , 1 , 2 , 1 , 2);

	fxlookupres->province_label = gtk_label_new("省份:");
	gtk_misc_set_alignment(GTK_MISC(fxlookupres->province_label) , 0 , 0 );
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxlookupres->province_label , 0 , 1 , 2 , 3);

	fxlookupres->province_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxlookupres->province_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxlookupres->province_entry , 0 , 1 , 3 , 4);

	fxlookupres->city_label = gtk_label_new("城市:");
	gtk_misc_set_alignment(GTK_MISC(fxlookupres->city_label) , 0 , 0 );
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxlookupres->city_label , 1 , 2 , 2 , 3);

	fxlookupres->city_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxlookupres->city_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxlookupres->city_entry , 1 , 2 , 3 , 4);
	
	fxlookupres->ok_button = gtk_button_new_with_label("确定");
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxlookupres->dialog)->action_area) , fxlookupres->ok_button);

	fxlookupres->cancel_button = gtk_button_new_with_label("取消");
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxlookupres->dialog)->action_area) , fxlookupres->cancel_button);

	g_signal_connect(fxlookupres->ok_button , "clicked" , G_CALLBACK(fx_lookupres_on_ok_clicked) , fxlookupres->dialog);

	g_signal_connect(fxlookupres->cancel_button , "clicked" , G_CALLBACK(fx_lookupres_on_ok_clicked) , fxlookupres->dialog);

	fx_lookupres_bind(fxlookupres);
	gtk_widget_show_all(fxlookupres->dialog);
	gtk_widget_hide(fxlookupres->dialog);
}
void fx_lookupres_on_ok_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	DEBUG_FOOTPRINT();

	gtk_dialog_response(GTK_DIALOG(data) , GTK_RESPONSE_OK);
}
