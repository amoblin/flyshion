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

FxAddbuddy* fx_addbuddy_new(FxMain* fxmain)
{
	FxAddbuddy* fxaddbuddy = (FxAddbuddy*)malloc(sizeof(FxAddbuddy));

	DEBUG_FOOTPRINT();

	memset(fxaddbuddy , 0 , sizeof(FxAddbuddy));
	fxaddbuddy->fxmain = fxmain;
	fxaddbuddy->phraselist = fetion_config_get_phrase(fxmain->user->config);
	return fxaddbuddy;
}

void fx_addbuddy_initialize(FxAddbuddy* fxaddbuddy)
{
	GdkPixbuf *icon;
	GtkWidget *renderer;
	GtkWidget *group_text , *info_frame , *info_box , *ok_button;
	GtkWidget *cancel_button , *name_text , *msg_frame , *msglabel;
	GtkWidget *group_entry , *name_hbox , *name_alignment;
	GtkTreeModel* model;

	DEBUG_FOOTPRINT();

	fxaddbuddy->dialog = gtk_dialog_new();
	icon = gdk_pixbuf_new_from_file(SKIN_DIR"add.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxaddbuddy->dialog) , icon);
	gtk_window_set_title(GTK_WINDOW(fxaddbuddy->dialog) , _("Add Buddy"));
	gtk_dialog_set_has_separator(GTK_DIALOG(fxaddbuddy->dialog) , FALSE);
	gtk_window_set_resizable(GTK_WINDOW(fxaddbuddy->dialog) , FALSE);
	gtk_window_set_modal(GTK_WINDOW(fxaddbuddy->dialog) , TRUE);
	gtk_widget_set_usize(fxaddbuddy->dialog , 350 , 420);
	gtk_container_set_border_width(GTK_CONTAINER(fxaddbuddy->dialog) , 20);

	fxaddbuddy->tablebox = gtk_table_new(2 , 2 , FALSE);

	fxaddbuddy->mobile_button = gtk_radio_button_new_with_label(NULL , _("Cell phone number:"));
	fxaddbuddy->group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(fxaddbuddy->mobile_button));

	fxaddbuddy->fetion_button = gtk_radio_button_new_with_label(fxaddbuddy->group , _("Fetion number:"));
	g_signal_connect(fxaddbuddy->fetion_button , "toggled" , G_CALLBACK(fx_addbuddy_no_type_change) , fxaddbuddy);
	gtk_table_attach_defaults(GTK_TABLE(fxaddbuddy->tablebox) , fxaddbuddy->fetion_button , 0 , 1 , 1 , 2);


	gtk_table_attach_defaults(GTK_TABLE(fxaddbuddy->tablebox) , fxaddbuddy->mobile_button , 0 , 1 , 0 , 1);

	fxaddbuddy->fetion_entry = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(fxaddbuddy->tablebox) , fxaddbuddy->fetion_entry , 1 , 2 , 1 , 2);
	gtk_widget_set_sensitive(fxaddbuddy->fetion_entry , FALSE);

	fxaddbuddy->mobile_entry = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(fxaddbuddy->tablebox) , fxaddbuddy->mobile_entry , 1 , 2 , 0 , 1);

	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxaddbuddy->dialog)->vbox) , fxaddbuddy->tablebox);

	info_frame = gtk_frame_new(_("Contact's Information"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxaddbuddy->dialog)->vbox) , info_frame);
	gtk_container_set_border_width(GTK_CONTAINER(info_frame) , 5);
	
	info_box = gtk_table_new(2 , 3 , FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(info_box) , 7);
	gtk_container_add(GTK_CONTAINER(info_frame) , info_box);

	group_text = gtk_label_new(_("Group: "));
	gtk_table_attach_defaults(GTK_TABLE(info_box) , group_text , 0 , 1 , 0 , 1);

	name_text = gtk_label_new(_("Name: "));
	gtk_table_attach_defaults(GTK_TABLE(info_box) , name_text , 0 , 1 , 1 , 2 );

	model = fx_addbuddy_create_group_model(fxaddbuddy);
	fxaddbuddy->group_combo = gtk_combo_box_new_with_model(model);
    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(fxaddbuddy->group_combo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(fxaddbuddy->group_combo), renderer,
				    "text", 0,
				    NULL);


	group_entry = gtk_bin_get_child(GTK_BIN(fxaddbuddy->group_combo));
	gtk_entry_set_editable(GTK_ENTRY(group_entry) , FALSE);
	
	gtk_table_attach_defaults(GTK_TABLE(info_box) , fxaddbuddy->group_combo , 1 , 2 , 0 , 1 );

	fxaddbuddy->name_entry = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE(info_box) , fxaddbuddy->name_entry , 1 , 2 , 1 , 2);

	msg_frame = gtk_frame_new(_("Send Message:"));
	gtk_container_set_border_width(GTK_CONTAINER(msg_frame) , 5);
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxaddbuddy->dialog)->vbox) , msg_frame);
	fxaddbuddy->msgbox = gtk_vbox_new(FALSE , 3);
	gtk_container_set_border_width(GTK_CONTAINER(fxaddbuddy->msgbox) , 7);
	msglabel = gtk_label_new(_("Hello ,I am "));
	fxaddbuddy->myname_entry = gtk_entry_new();
	gtk_widget_set_usize(fxaddbuddy->myname_entry , 70 , 30);
	name_hbox = gtk_hbox_new(FALSE , 3);
	name_alignment = gtk_alignment_new(0 ,0 , 0 , 0);
	gtk_container_add(GTK_CONTAINER(name_alignment) , name_hbox);
	gtk_box_pack_start_defaults(GTK_BOX(name_hbox) , msglabel);
	gtk_box_pack_start_defaults(GTK_BOX(name_hbox) , fxaddbuddy->myname_entry);
	gtk_box_pack_start_defaults(GTK_BOX(fxaddbuddy->msgbox) , name_alignment);
	gtk_container_add(GTK_CONTAINER(msg_frame) , fxaddbuddy->msgbox);
	
	ok_button = gtk_button_new_with_label(_("OK"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxaddbuddy->dialog)->action_area) , ok_button);
	g_signal_connect(G_OBJECT(ok_button) , "clicked" , G_CALLBACK(fx_addbuddy_on_ok_clicked) , fxaddbuddy);

	cancel_button = gtk_button_new_with_label(_("Cancel"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxaddbuddy->dialog)->action_area) , cancel_button);
	g_signal_connect(G_OBJECT(cancel_button) , "clicked" , G_CALLBACK(fx_addbuddy_on_cancel_clicked) , fxaddbuddy->dialog);

	fx_addbuddy_bind(fxaddbuddy);
	gtk_widget_show_all(fxaddbuddy->dialog);
	gtk_widget_hide(fxaddbuddy->dialog);
}
GtkTreeModel* fx_addbuddy_create_group_model(FxAddbuddy* fxaddbuddy)
{
	FxMain* fxmain = fxaddbuddy->fxmain;
	GtkTreeView* tree = GTK_TREE_VIEW(fxmain->mainPanel->treeView);
	GtkTreeModel* model = gtk_tree_view_get_model(tree);
	GtkTreeStore* store;
	GtkTreeIter iter , cIter;
	char* groupname;
	int groupid;

	DEBUG_FOOTPRINT();

	store = gtk_tree_store_new(GROUP_COL_NUMS , G_TYPE_STRING , G_TYPE_INT);
	gtk_tree_model_get_iter_root(model , &iter);
	do
	{
		gtk_tree_model_get(model , &iter , G_NAME_COL , &groupname , G_ID_COL , &groupid , -1);
		gtk_tree_store_append(store , &cIter , NULL);
		gtk_tree_store_set(store , &cIter
						   , GROUP_NAME_COL , groupname
						   , GROUP_ID_COL , groupid
						   , -1);
	}
	while(gtk_tree_model_iter_next(model , &iter));
	return GTK_TREE_MODEL(store);
}

void fx_addbuddy_bind(FxAddbuddy* fxaddbuddy)
{
	GtkWidget *button;
	GSList *msglist = NULL; 
	FxList *pos;
	Phrase *phrase;
	User *user = fxaddbuddy->fxmain->user;

	DEBUG_FOOTPRINT();


	foreach_list(fxaddbuddy->phraselist , pos){
		phrase = (Phrase*)(pos->data);
		button = gtk_radio_button_new_with_label(msglist , phrase->content);
		msglist = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));
		g_signal_connect(G_OBJECT(button) , "toggled" , G_CALLBACK(fx_addbuddy_on_phrase_change) , fxaddbuddy);
		gtk_box_pack_start(GTK_BOX(fxaddbuddy->msgbox) , button , FALSE , FALSE , 1);
	}
	gtk_combo_box_set_active (GTK_COMBO_BOX(fxaddbuddy->group_combo), 0);

	gtk_entry_set_text(GTK_ENTRY(fxaddbuddy->myname_entry) , user->nickname);
}
void fx_addbuddy_on_ok_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxAddbuddy* fxadd = (FxAddbuddy*)data;
	const char *no = NULL;
	const char *desc = NULL;
	const char *localname = NULL;
	int buddylist;
	GtkTreeIter iter , newIter;
	GtkTreeModel *model = NULL;
	GtkTreeModel *mainModel = NULL;
	Contact* contact = NULL;
	Config* config = NULL;
	GdkPixbuf *pb = NULL;
	FxCode* fxcode = NULL;
	int ret = 0;
	char code[24];
	User* user = fxadd->fxmain->user;

	DEBUG_FOOTPRINT();

	config = user->config;

	if(fxadd->notype == FETION_NO)
	{
		no = gtk_entry_get_text(GTK_ENTRY(fxadd->fetion_entry));
		if(strcmp(no , user->sId) == 0)
		{
			fx_util_popup_warning(fxadd->fxmain , _("You can not add yourself as a friend"));
			gtk_dialog_response(GTK_DIALOG(fxadd->dialog) , GTK_RESPONSE_OK);
			return;
		}
	}
	else
	{
		no = gtk_entry_get_text(GTK_ENTRY(fxadd->mobile_entry));
		if(strcmp(no , user->mobileno) == 0)
		{
			fx_util_popup_warning(fxadd->fxmain , _("You can not add yourself as a friend"));
			gtk_dialog_response(GTK_DIALOG(fxadd->dialog) , GTK_RESPONSE_OK);
			return;
		}
	}
	if(strlen(no) == 0)
		return;
	desc = gtk_entry_get_text(GTK_ENTRY(fxadd->myname_entry));
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(fxadd->group_combo));
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(fxadd->group_combo) , &iter);
	gtk_tree_model_get(model , &iter , GROUP_ID_COL , &buddylist , -1);
	localname = gtk_entry_get_text(GTK_ENTRY(fxadd->name_entry));
addbuddy:
	contact = fetion_contact_add_buddy(user , no , fxadd->notype , buddylist , localname , desc , fxadd->phraseid , &ret);
	switch(ret)
	{
		case BUDDY_USER_EXIST :
			fx_util_popup_warning(fxadd->fxmain , _("The contact you want to add has already been in your contact list, please don't add him(her) repeatedly!"));
			gtk_dialog_response(GTK_DIALOG(fxadd->dialog) , GTK_RESPONSE_OK);
			return;
		case BUDDY_SAME_USER_DAILY_LIMIT :
			fx_util_popup_warning(fxadd->fxmain , _("You have reached the daily limit of adding buddies, please retry later"));
			gtk_dialog_response(GTK_DIALOG(fxadd->dialog) , GTK_RESPONSE_OK);
			return;
		case BUDDY_BAD_REQUEST :
			fx_util_popup_warning(fxadd->fxmain , _("Add buddy failed.Unknown reason!"));
			gtk_dialog_response(GTK_DIALOG(fxadd->dialog) , GTK_RESPONSE_OK);
			return;
		default:
			break;
	}
	if(contact != NULL)
	{
		mainModel = gtk_tree_view_get_model(GTK_TREE_VIEW(fxadd->fxmain->mainPanel->treeView));
		gtk_tree_model_get_iter_root(mainModel , &iter);
		do
		{
			gtk_tree_model_get(mainModel , &iter , G_ID_COL , &buddylist , -1);		
			if(buddylist == contact->groupid)
			{
				pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.svg" , 25 , 25 , NULL);
				gtk_tree_store_append(GTK_TREE_STORE(mainModel) , &newIter , &iter);
				gtk_tree_store_set(GTK_TREE_STORE(mainModel) , &newIter
								 , B_PIXBUF_COL 	, pb
								 , B_SIPURI_COL 	, contact->sipuri
								 , B_USERID_COL 	, contact->userId
								 , B_NAME_COL 	    , contact->localname
								 , B_CARRIER_COL	, "CMCC"
								 , B_STATE_COL     ,    0
								 , B_SERVICESTATUS_COL	, contact->serviceStatus
								 , B_CARRIERSTATUS_COL ,  CARRIER_STATUS_NORMAL
								 , B_RELATIONSTATUS_COL , RELATION_STATUS_UNAUTHENTICATED
								 , B_IMAGE_CHANGED_COL , -1
								 , B_SIZE_COL		, config->iconSize
								 , -1);
				break;
			}
		}
		while(gtk_tree_model_iter_next(mainModel , &iter));
	}
	else
	{
		if(user->verification != NULL)
		{
			generate_pic_code(user);
			fxcode = fx_code_new(fxadd->fxmain , user->verification->text , user->verification->tips , CODE_NOT_ERROR);
			fx_code_initialize(fxcode);
			ret = gtk_dialog_run(GTK_DIALOG(fxcode->dialog));
			if(ret == GTK_RESPONSE_OK)
			{
				bzero(code , sizeof(code));
				strcpy(code , gtk_entry_get_text(GTK_ENTRY(fxcode->codeentry)));
				fetion_user_set_verification_code(user , code);
				gtk_widget_destroy(fxcode->dialog);
				goto addbuddy;
				gdk_threads_leave();
			}
			else
			{
				gtk_widget_destroy(fxcode->dialog);
				gdk_threads_leave();
			}

		}
	}
	gtk_dialog_response(GTK_DIALOG(fxadd->dialog) , GTK_RESPONSE_OK);
}

void fx_addbuddy_on_cancel_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	DEBUG_FOOTPRINT();

	gtk_dialog_response(GTK_DIALOG(data) , GTK_RESPONSE_CANCEL);
}
void fx_addbuddy_on_phrase_change(GtkWidget* widget , gpointer data)
{
	FxAddbuddy *fxaddbuddy = (FxAddbuddy*)data;
	const char *label;
	FxList *pos;
	Phrase *phrase;

	DEBUG_FOOTPRINT();

	if(!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		return;
	label = gtk_button_get_label(GTK_BUTTON(widget));
	pos = fxaddbuddy->phraselist;
	foreach_list(fxaddbuddy->phraselist , pos){
		phrase = (Phrase*)(pos->data);
		if(strcmp(phrase->content , label) == 0){
			fxaddbuddy->phraseid = phrase->phraseid;
			break;
		}
	}
}
void fx_addbuddy_no_type_change(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxAddbuddy* fxaddbuddy = (FxAddbuddy*)data;

	DEBUG_FOOTPRINT();

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxaddbuddy->fetion_button)))
	{
		fxaddbuddy->notype = FETION_NO;
		gtk_widget_set_sensitive(fxaddbuddy->fetion_entry , TRUE);
		gtk_widget_set_sensitive(fxaddbuddy->mobile_entry , FALSE);
	}
	else
	{
		fxaddbuddy->notype = MOBILE_NO;
		gtk_widget_set_sensitive(fxaddbuddy->fetion_entry , FALSE);
		gtk_widget_set_sensitive(fxaddbuddy->mobile_entry , TRUE);
	}
}	
