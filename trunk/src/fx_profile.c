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

FxProfile* fx_profile_new(FxMain* fxmain , const char* userid)
{
	FxProfile* fxprofile = (FxProfile*)malloc(sizeof(FxProfile));

	DEBUG_FOOTPRINT();

	memset(fxprofile , 0 , sizeof(FxProfile));
	fxprofile->fxmain = fxmain;
	strcpy(fxprofile->userid , userid);
	return fxprofile;
}

void fx_profile_initialize(FxProfile* fxprofile)
{
	GtkWidget* box ;
	GtkWidget* inbox ;
	GdkPixbuf* pb;
	FxMain* fxmain = fxprofile->fxmain;
	
	DEBUG_FOOTPRINT();

	fxprofile->dialog = gtk_dialog_new_with_buttons ("查看好友信息"
									, GTK_WINDOW(fxmain->window)
									, GTK_DIALOG_DESTROY_WITH_PARENT
									, NULL);

	gtk_dialog_set_has_separator(GTK_DIALOG(fxprofile->dialog) , FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(fxprofile->dialog) , 10);

	gtk_widget_set_usize(GTK_WIDGET(fxprofile->dialog) , 500 , 340);

	gtk_window_set_resizable(GTK_WINDOW(fxprofile->dialog) , FALSE);

	box = gtk_table_new(6 , 3 , TRUE);
	gtk_table_set_row_spacings(GTK_TABLE(box) , 2);
	gtk_table_set_col_spacings(GTK_TABLE(box) , 2);

	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxprofile->dialog)->vbox) , box);

	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.png" , 90 , 90 , NULL);
	fxprofile->image = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxprofile->image , 0 , 1 , 0 , 4);

	fxprofile->sid_label = gtk_label_new("飞信号:");
	gtk_misc_set_alignment(GTK_MISC(fxprofile->sid_label) , 0 , 0);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxprofile->sid_label , 1 , 2 , 0 , 1);

	fxprofile->sid_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxprofile->sid_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxprofile->sid_entry , 1 , 2 , 1 , 2);

	fxprofile->gender_label = gtk_label_new("性别:");
	gtk_misc_set_alignment(GTK_MISC(fxprofile->gender_label) , 0 , 0);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxprofile->gender_label , 2 , 3 , 0 , 1);

	fxprofile->gender_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxprofile->gender_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxprofile->gender_entry , 2 , 3 , 1 , 2);

	fxprofile->mno_label = gtk_label_new("手机号:");
	gtk_misc_set_alignment(GTK_MISC(fxprofile->mno_label) , 0 , 0);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxprofile->mno_label , 1 , 2 , 2 , 3);

	fxprofile->mno_entry = gtk_entry_new();
	gtk_widget_set_usize(fxprofile->mno_entry , 150 , 25);
	gtk_entry_set_editable(GTK_ENTRY(fxprofile->mno_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxprofile->mno_entry , 1 , 2 , 3 , 4);

	fxprofile->nick_label = gtk_label_new("昵称:");
	gtk_misc_set_alignment(GTK_MISC(fxprofile->nick_label) , 0 , 0);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxprofile->nick_label , 2 , 3 , 2 , 3);

	fxprofile->nick_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxprofile->nick_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxprofile->nick_entry , 2 , 3 , 3 , 4);

	fxprofile->impre_label = gtk_label_new("心情:");
	gtk_misc_set_alignment(GTK_MISC(fxprofile->impre_label) , 0 , 0);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxprofile->impre_label , 0 , 3 , 4, 5);

	fxprofile->impre_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxprofile->impre_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(box) , fxprofile->impre_entry , 0 , 3 , 5 , 6);

	inbox = gtk_table_new( 4 , 2 , TRUE);
	gtk_table_set_row_spacings(GTK_TABLE(inbox) , 2);
	gtk_table_set_col_spacings(GTK_TABLE(inbox) , 2);
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxprofile->dialog)->vbox) , inbox);

	fxprofile->local_label = gtk_label_new("备注名:");
	gtk_misc_set_alignment(GTK_MISC(fxprofile->local_label) , 0 , 0 );
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxprofile->local_label , 0 , 1 , 0 , 1);

	fxprofile->local_entry = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxprofile->local_entry , 0 , 1 , 1 , 2);
	gtk_entry_set_editable(GTK_ENTRY(fxprofile->local_entry) , FALSE);

	fxprofile->nation_label = gtk_label_new("国家:");
	gtk_misc_set_alignment(GTK_MISC(fxprofile->nation_label) , 0 , 0 );
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxprofile->nation_label , 1 , 2 , 0 , 1);

	fxprofile->nation_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxprofile->nation_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxprofile->nation_entry , 1 , 2 , 1 , 2);

	fxprofile->province_label = gtk_label_new("省份:");
	gtk_misc_set_alignment(GTK_MISC(fxprofile->province_label) , 0 , 0 );
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxprofile->province_label , 0 , 1 , 2 , 3);

	fxprofile->province_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxprofile->province_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxprofile->province_entry , 0 , 1 , 3 , 4);

	fxprofile->city_label = gtk_label_new("城市:");
	gtk_misc_set_alignment(GTK_MISC(fxprofile->city_label) , 0 , 0 );
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxprofile->city_label , 1 , 2 , 2 , 3);

	fxprofile->city_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxprofile->city_entry) , FALSE);
	gtk_table_attach_defaults(GTK_TABLE(inbox) , fxprofile->city_entry , 1 , 2 , 3 , 4);
	
	fxprofile->ok_button = gtk_button_new_with_label("确定");
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxprofile->dialog)->action_area) , fxprofile->ok_button);

	fxprofile->cancel_button = gtk_button_new_with_label("取消");
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxprofile->dialog)->action_area) , fxprofile->cancel_button);

	g_signal_connect(fxprofile->ok_button , "clicked" , G_CALLBACK(fx_profile_on_button_clicked) , fxprofile->dialog);

	g_signal_connect(fxprofile->cancel_button , "clicked" , G_CALLBACK(fx_profile_on_button_clicked) , fxprofile->dialog);

	fx_profile_bind(fxprofile);

	gtk_widget_show_all(fxprofile->dialog);
	gtk_widget_hide(fxprofile->dialog);
}

void fx_profile_bind(FxProfile* fxprofile)
{
	Contact* contact;
	GdkPixbuf* pb;
	char* res;
	User* user = fxprofile->fxmain->user;
	Config* config = user->config;

	DEBUG_FOOTPRINT();
		
	contact = fetion_contact_get_contact_info(fxprofile->fxmain->user , fxprofile->userid);

	if(contact->nickname != NULL)
		gtk_entry_set_text(GTK_ENTRY(fxprofile->nick_entry) , contact->nickname);

	gtk_entry_set_text(GTK_ENTRY(fxprofile->gender_entry) ,
						contact->gender == 1 ? "男" : ( contact->gender == 2 ? "女" : "保密"));

	if(contact->mobileno != NULL)
		gtk_entry_set_text(GTK_ENTRY(fxprofile->mno_entry) , contact->mobileno);

	if(contact->impression != NULL)
		gtk_entry_set_text(GTK_ENTRY(fxprofile->impre_entry) , contact->impression);

	gtk_entry_set_text(GTK_ENTRY(fxprofile->sid_entry) , contact->sId);

	if(contact->country != NULL)
		gtk_entry_set_text(GTK_ENTRY(fxprofile->nation_entry)
					, strcmp(contact->country , "CN") == 0 ? "中国" : contact->country);

	if(contact->province != NULL)
	{
		res = fetion_config_get_province_name(contact->province);
		gtk_entry_set_text(GTK_ENTRY(fxprofile->province_entry) , res == NULL ? "未知" : res);
		free(res);
	}

	if(contact->city != NULL)
	{
		res = fetion_config_get_city_name(contact->province , contact->city);
		gtk_entry_set_text(GTK_ENTRY(fxprofile->city_entry) , res == NULL ? "未知" : res);
		free(res);
	}
	char portrait[100];

	bzero(portrait , sizeof(portrait));
	sprintf(portrait , "%s/%s.jpg" , config->iconPath , contact->sId);

	if(contact->localname != NULL)
		gtk_entry_set_text(GTK_ENTRY(fxprofile->local_entry) , contact->localname);

	pb = gdk_pixbuf_new_from_file_at_size(portrait , 90 , 90 , NULL);

	if(pb != NULL)
	{
		gtk_image_set_from_pixbuf(GTK_IMAGE(fxprofile->image) , pb);
		g_object_unref(pb);
	}
}
void fx_profile_free(FxProfile* fxprofile)
{
	DEBUG_FOOTPRINT();

	free(fxprofile);
}
void fx_profile_on_button_clicked(GtkWidget* UNUSED(widget) , gpointer data)
{
	DEBUG_FOOTPRINT();

	gtk_dialog_response(GTK_DIALOG(data) , GTK_RESPONSE_OK);
}
