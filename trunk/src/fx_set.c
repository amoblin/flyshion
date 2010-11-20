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

static void fx_set_on_ok_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxSet         *fxset = (FxSet*)data;
	User          *user = fxset->fxmain->user;
	Config        *config = user->config;
	/* system setting varibles */
	GtkTextView   *textview = GTK_TEXT_VIEW(fxset->apEty);
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(textview);
	GtkTextIter    startIter;
   	GtkTextIter    endIter;
	const gchar   *autoReplyMsg = NULL;
	/* personal setting varibles */
	const gchar   *nickname = NULL;
	const gchar   *impression = NULL;
	gchar          nickname_text[1024];
	gint           gender;
	GtkTreeModel  *genderModel = NULL;
	GtkTreeIter    genderIter;

	if(gtk_notebook_get_current_page(GTK_NOTEBOOK(fxset->notebook)) == PAGE_SYSTEM)
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxset->etBtn)))
			config->sendMode = SEND_MODE_ENTER;
		else
			config->sendMode = SEND_MODE_CTRL_ENTER;

		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxset->ppCb)))
			config->autoPopup = AUTO_POPUP_ENABLE;
		else
			config->autoPopup = AUTO_POPUP_DISABLE;

		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxset->smallBtn)))
			config->closeMode = CLOSE_ICON_MODE;
		else
			config->closeMode = CLOSE_DESTROY_MODE;

		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxset->muteBtn)))
			config->isMute = MUTE_ENABLE;
		else
			config->isMute = MUTE_DISABLE;

		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxset->alertBtn)))
			config->msgAlert = MSG_ALERT_DISABLE;
		else
			config->msgAlert = MSG_ALERT_ENABLE;

		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxset->iconBtn))){
			config->canIconify = ICON_CAN;
			config->closeAlert = CLOSE_ALERT_DISABLE;
		}else
			config->canIconify = ICON_CANNOT;

		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxset->autoAwayBtn)))
			config->autoAway = AUTO_AWAY_ENABLE;
		else
			config->autoAway = AUTO_AWAY_DISABLE;

		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxset->onlineNotifyBtn)))
			config->onlineNotify = ONLINE_NOTIFY_ENABLE;
		else
			config->onlineNotify = ONLINE_NOTIFY_DISABLE;

		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxset->closeSysMsgBtn)))
			config->closeSysMsg = CLOSE_SYSMSG_ENABLE;
		else
			config->closeSysMsg = CLOSE_SYSMSG_DISABLE;

		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxset->closeShowBtn)))
			config->closeFetionShow = CLOSE_FETION_SHOW_ENABLE;
		else
			config->closeFetionShow = CLOSE_FETION_SHOW_DISABLE;

		gtk_text_buffer_get_start_iter(buffer , &startIter);
		gtk_text_buffer_get_end_iter(buffer , &endIter);
		autoReplyMsg = gtk_text_buffer_get_text(buffer , &startIter , &endIter , TRUE);

		bzero(config->autoReplyMessage , sizeof(config->autoReplyMessage));

		if(autoReplyMsg != NULL)
			strcpy(config->autoReplyMessage , autoReplyMsg);
		fetion_config_save(user);
	}
	else
	{
		nickname = gtk_entry_get_text(GTK_ENTRY(fxset->nick_entry));
		if(strlen(nickname) == 0)
		{
			fx_util_popup_warning(fxset->fxmain , _("Nick can not be empty!"));
			return;
		}
		impression = gtk_entry_get_text(GTK_ENTRY(fxset->impre_entry));
		bzero(user->nickname , sizeof(user->nickname));
		strcpy(user->nickname , nickname);
		bzero(user->impression , sizeof(user->impression));
		strcpy(user->impression , impression);

		genderModel = gtk_combo_box_get_model(GTK_COMBO_BOX(fxset->gender_combo));
		gtk_combo_box_get_active_iter(GTK_COMBO_BOX(fxset->gender_combo) , &genderIter);

		gtk_tree_model_get(genderModel , &genderIter , COMBO_ID_COL , &gender , -1);
		user->gender = gender;
		if(fetion_user_update_info(user) > 0)
		{
			
			sprintf(nickname_text , "<b>%s</b>"
					, user->nickname == NULL ? user->sId : user->nickname );

			gtk_label_set_markup(GTK_LABEL(fxset->fxmain->headPanel->name_label) , nickname_text );

			gtk_label_set_text(GTK_LABEL(fxset->fxmain->headPanel->impre_label)
					, strlen(user->impression) == 0 ? _("Click here to input signature") : user->impression);

			bzero(fxset->fxmain->headPanel->oldimpression , sizeof(fxset->fxmain->headPanel->oldimpression));
			strcpy(fxset->fxmain->headPanel->oldimpression
				, (strlen(user->impression) == 0 || user->impression == NULL)
				? _("Click here to input signature") : user->impression);
				}
	}
	gtk_dialog_response(GTK_DIALOG(fxset->dialog) , GTK_RESPONSE_CANCEL);
}

static void fx_set_on_cancel_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	gtk_dialog_response(GTK_DIALOG(data) , GTK_RESPONSE_CANCEL);
}

static void fx_set_on_autoreply_toggled(GtkWidget *widget , gpointer data)
{
	FxSet *fxset = (FxSet*)data;

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		gtk_widget_set_sensitive(fxset->apEty , TRUE);
	else
		gtk_widget_set_sensitive(fxset->apEty , FALSE);
}

FxSet* fx_set_new(FxMain* fxmain)
{
	FxSet *fxset = (FxSet*)malloc(sizeof(FxSet));

	memset(fxset , 0 , sizeof(FxSet));
	fxset->fxmain = fxmain;
	return fxset;
}

void fx_set_initialize(FxSet* fxset)
{
	GtkWidget *psetting_label = NULL;
	GtkWidget *ssetting_label = NULL;

	GtkWidget *ok_button = NULL;
	GtkWidget* cancel_button = NULL;

	GdkPixbuf* pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"online.svg",
				   22, 22, NULL);

	fxset->dialog = gtk_dialog_new();
	gtk_window_set_icon(GTK_WINDOW(fxset->dialog) , pb);
	g_object_unref(pb);
	gtk_dialog_set_has_separator(GTK_DIALOG(fxset->dialog)
							   , FALSE);
	gtk_widget_set_usize(fxset->dialog , 500 , 360);
	gtk_window_set_title(GTK_WINDOW(fxset->dialog) , _("OpenFetion Preference"));

	fxset->notebook = gtk_notebook_new();
	gtk_widget_set_usize(fxset->notebook , 490 , 320);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(fxset->notebook) , FALSE);
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(fxset->notebook) , GTK_POS_TOP);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(fxset->dialog)->vbox)
					 , fxset->notebook , FALSE , FALSE , 0);

	fxset->psetting = gtk_vbox_new(FALSE , FALSE);
	psetting_label = gtk_label_new(_("Personal Settings"));
	gtk_notebook_append_page(GTK_NOTEBOOK(fxset->notebook)
						   , fxset->psetting
						   , psetting_label);

	fxset->ssetting = gtk_vbox_new(FALSE , FALSE);
	ssetting_label = gtk_label_new(_("System Settings"));
	gtk_notebook_append_page(GTK_NOTEBOOK(fxset->notebook)
						   , fxset->ssetting
						   , ssetting_label);


	ok_button = gtk_button_new_with_label(_("OK"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxset->dialog)->action_area) , ok_button);
	g_signal_connect(ok_button , "clicked" , G_CALLBACK(fx_set_on_ok_clicked) , fxset);

	cancel_button = gtk_button_new_with_label(_("Cancel"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxset->dialog)->action_area) , cancel_button);
	g_signal_connect(cancel_button , "clicked" , G_CALLBACK(fx_set_on_cancel_clicked) , fxset->dialog);

	fx_set_initialize_personal(fxset);
	fx_set_initialize_system(fxset);

	gtk_widget_show_all(fxset->dialog);
	gtk_widget_hide(fxset->dialog);
}

void fx_set_bind_system(FxSet* fxset)
{
	FxMain *fxmain = fxset->fxmain;  
	Config *config = fxmain->user->config;
	GtkTextBuffer *buffer = NULL;
	GtkTextIter startIter , endIter;
	char *autoReplyMsg = NULL;

	if(config->sendMode == SEND_MODE_ENTER)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->etBtn) , TRUE);	
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->ctBtn) , TRUE);	

	if(config->autoPopup == AUTO_POPUP_ENABLE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->ppCb) , TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->ppCb) , FALSE);

	if(config->closeMode == CLOSE_ICON_MODE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->smallBtn) , TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->smallBtn) , FALSE);

	if(config->isMute == MUTE_ENABLE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->muteBtn) , TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->muteBtn) , FALSE);

	if(config->msgAlert == MSG_ALERT_ENABLE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->alertBtn) , FALSE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->alertBtn) , TRUE);

	if(config->canIconify == ICON_CAN)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->iconBtn) , TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->iconBtn) , FALSE);

	if(config->autoAway == AUTO_AWAY_ENABLE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->autoAwayBtn) , TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->autoAwayBtn) , FALSE);

	if(config->onlineNotify == ONLINE_NOTIFY_ENABLE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->onlineNotifyBtn), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->onlineNotifyBtn), FALSE);

	if(config->closeSysMsg == CLOSE_SYSMSG_ENABLE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->closeSysMsgBtn), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->closeSysMsgBtn), FALSE);

	if(config->closeFetionShow == CLOSE_FETION_SHOW_ENABLE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->closeShowBtn), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxset->closeShowBtn), FALSE);

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxset->apEty));
	gtk_text_buffer_get_start_iter(buffer , &startIter);
	gtk_text_buffer_get_end_iter(buffer , &endIter);
	gtk_text_buffer_delete(buffer , &startIter , &endIter);

	if(strlen(config->autoReplyMessage) == 0)
	{
		autoReplyMsg = _("Sorry, I am not in now, and will reply to you soon");
		gtk_text_buffer_insert(buffer , &startIter , autoReplyMsg , strlen(autoReplyMsg));
	}
	else
	{
		gtk_text_buffer_insert(buffer , &startIter , config->autoReplyMessage , strlen(config->autoReplyMessage));
	}
}

void fx_set_initialize_personal(FxSet* fxset)
{
	FxMain *fxmain = fxset->fxmain;
	User *user = fxmain->user;
	Config *config = user->config;
	char filepath[128];
	GdkPixbuf *pb = NULL;

	GtkWidget *box = NULL;
	GtkTreeModel *gmodel = NULL;
	GtkWidget *gender_entry = NULL;
	GtkTreeIter iter;
	int id;
	char *cityName = NULL;
	char *provinceName = NULL;

	box = gtk_fixed_new();
	gtk_box_pack_start_defaults(GTK_BOX(fxset->psetting) , box);

	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.svg" , 90 , 90 , NULL);
	fxset->image = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_fixed_put(GTK_FIXED(box) , fxset->image , 10 , 15 );

	fxset->sid_label = gtk_label_new(_("Fetion Number:"));
	gtk_misc_set_alignment(GTK_MISC(fxset->sid_label) , 0 , 0);
	gtk_fixed_put(GTK_FIXED(box) , fxset->sid_label , 120 , 10 );

	fxset->sid_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxset->sid_entry) , FALSE);
	gtk_fixed_put(GTK_FIXED(box) , fxset->sid_entry , 120 , 30 );

	fxset->gender_label = gtk_label_new(_("Sex:"));
	gtk_misc_set_alignment(GTK_MISC(fxset->gender_label) , 0 , 0);
	gtk_fixed_put(GTK_FIXED(box) , fxset->gender_label , 310 , 10 );

	gmodel = fx_set_create_gender_model();
	fxset->gender_combo = gtk_combo_box_entry_new_with_model(gmodel , COMBO_NAME_COL);
	gender_entry = gtk_bin_get_child(GTK_BIN(fxset->gender_combo));
	gtk_entry_set_editable(GTK_ENTRY(gender_entry) , FALSE);
	gtk_combo_box_set_active(GTK_COMBO_BOX(fxset->gender_combo) , 1);
	gtk_widget_set_usize(fxset->gender_combo , 150 , 25);
	gtk_fixed_put(GTK_FIXED(box) , fxset->gender_combo , 310 , 30 );

	fxset->mno_label = gtk_label_new(_("Cell Phone Number:"));
	gtk_misc_set_alignment(GTK_MISC(fxset->mno_label) , 0 , 0);
	gtk_fixed_put(GTK_FIXED(box) , fxset->mno_label , 120 , 60 );

	fxset->mno_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxset->mno_entry) , FALSE);
	gtk_fixed_put(GTK_FIXED(box) , fxset->mno_entry , 120 , 80 );

	fxset->nick_label = gtk_label_new(_("Nickname:"));
	gtk_misc_set_alignment(GTK_MISC(fxset->nick_label) , 0 , 0);
	gtk_fixed_put(GTK_FIXED(box) , fxset->nick_label , 310 , 60 );

	fxset->nick_entry = gtk_entry_new();
	gtk_fixed_put(GTK_FIXED(box) , fxset->nick_entry , 310 , 80 );

	fxset->impre_label = gtk_label_new(_("Personal Signature:"));
	gtk_misc_set_alignment(GTK_MISC(fxset->impre_label) , 0 , 0);
	gtk_fixed_put(GTK_FIXED(box) , fxset->impre_label , 10 , 115 );

	fxset->impre_entry = gtk_entry_new();
	gtk_widget_set_usize(fxset->impre_entry , 460 , 25);
	gtk_fixed_put(GTK_FIXED(box) , fxset->impre_entry , 10 , 135 );
	
	fxset->province_label = gtk_label_new(_("Province:"));
	gtk_misc_set_alignment(GTK_MISC(fxset->province_label) , 0 , 0 );
	gtk_fixed_put(GTK_FIXED(box) , fxset->province_label , 10 , 165 );

	fxset->province_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxset->province_entry) , FALSE);
	gtk_widget_set_usize(fxset->province_entry , 220 , 25);
	gtk_fixed_put(GTK_FIXED(box) , fxset->province_entry , 10 , 185 );

	fxset->city_label = gtk_label_new(_("City:"));
	gtk_misc_set_alignment(GTK_MISC(fxset->city_label) , 0 , 0 );
	gtk_fixed_put(GTK_FIXED(box) , fxset->city_label , 250 , 165 );

	fxset->city_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxset->city_entry) , FALSE);
	gtk_widget_set_usize(fxset->city_entry , 220 , 25);
	gtk_fixed_put(GTK_FIXED(box) , fxset->city_entry , 250 , 185 );
	
	fxset->job_label = gtk_label_new(_("Job occupation:"));
	gtk_misc_set_alignment(GTK_MISC(fxset->job_label) , 0 , 0 );
	gtk_fixed_put(GTK_FIXED(box) , fxset->job_label , 10 , 215 );

	fxset->job_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxset->job_entry) , FALSE);
	gtk_widget_set_usize(fxset->job_entry , 220 , 25);
	gtk_fixed_put(GTK_FIXED(box) , fxset->job_entry , 10 , 235 );

	fxset->email_label = gtk_label_new(_("Email:"));
	gtk_misc_set_alignment(GTK_MISC(fxset->email_label) , 0 , 0 );
	gtk_fixed_put(GTK_FIXED(box) , fxset->email_label , 250 , 215 );

	fxset->email_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(fxset->email_entry) , FALSE);
	gtk_widget_set_usize(fxset->email_entry , 220 , 25);
	gtk_fixed_put(GTK_FIXED(box) , fxset->email_entry , 250 , 235 );


	gtk_entry_set_text(GTK_ENTRY(fxset->sid_entry) , user->sId);
	if(user->mobileno)
		gtk_entry_set_text(GTK_ENTRY(fxset->mno_entry) , user->mobileno);

	if(user->nickname)
		gtk_entry_set_text(GTK_ENTRY(fxset->nick_entry) , user->nickname);

	if(user->impression)
		gtk_entry_set_text(GTK_ENTRY(fxset->impre_entry) , user->impression);

	provinceName = fetion_config_get_province_name(user->province);

	if(provinceName)
		gtk_entry_set_text(GTK_ENTRY(fxset->province_entry) , provinceName);
	free(provinceName);

	cityName = fetion_config_get_city_name(user->province , user->city);

	if(cityName)
		gtk_entry_set_text(GTK_ENTRY(fxset->city_entry) , cityName);
	free(cityName);

	gtk_entry_set_text(GTK_ENTRY(fxset->job_entry) , _("Secrecy"));
	gtk_entry_set_text(GTK_ENTRY(fxset->email_entry) , _("Secrecy"));

	gtk_tree_model_get_iter_root(gmodel , &iter);
	do
	{
		gtk_tree_model_get(gmodel , &iter , COMBO_ID_COL , &id , -1);
		if(id == user->gender)
		{
			gtk_combo_box_set_active_iter(GTK_COMBO_BOX(fxset->gender_combo), &iter);
			break;
		}
	}
	while(gtk_tree_model_iter_next(gmodel , &iter));

	sprintf(filepath , "%s/%s.jpg" , config->iconPath , user->sId);
	pb = gdk_pixbuf_new_from_file_at_size(filepath , 90 , 90 , NULL);
	if(pb != NULL){
		gtk_image_set_from_pixbuf(GTK_IMAGE(fxset->image) , pb);
		g_object_unref(pb);
	}
}

void fx_set_initialize_system(FxSet* fxset)
{
	
	GtkWidget *fixed = NULL;
	GtkWidget *label1 = NULL;
	GtkWidget *label2 = NULL;
	GtkWidget *label3 = NULL;
	GtkWidget *apScr = NULL;
	GSList	  *gl = NULL;
	
	fixed = gtk_fixed_new();

	label1 = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(label1) , _("<b>Appearance</b>"));
	gtk_fixed_put(GTK_FIXED(fixed) , label1 , 20 , 20);
	
	fxset->iconBtn = gtk_check_button_new_with_label(_("Minimize to Tray"));
	gtk_fixed_put(GTK_FIXED(fixed) , fxset->iconBtn , 40 , 42);

	fxset->smallBtn = gtk_check_button_new_with_label(_("Close to Tray"));
	gtk_fixed_put(GTK_FIXED(fixed) , fxset->smallBtn , 230 , 42);
	
	fxset->ppCb = gtk_check_button_new_with_label(_("Auto popup message"));
	gtk_fixed_put(GTK_FIXED(fixed) , fxset->ppCb , 40 , 67);

	fxset->alertBtn = gtk_check_button_new_with_label(_("Disable Message Notification"));
	gtk_fixed_put(GTK_FIXED(fixed) , fxset->alertBtn , 230 , 67);

	fxset->muteBtn = gtk_check_button_new_with_label(_("Mute"));
	gtk_fixed_put(GTK_FIXED(fixed) , fxset->muteBtn , 40 , 92);

	fxset->autoAwayBtn = gtk_check_button_new_with_label(_("Auto away when idle"));
	gtk_fixed_put(GTK_FIXED(fixed) , fxset->autoAwayBtn , 230 , 92);

	fxset->onlineNotifyBtn = gtk_check_button_new_with_label(_("Online notification enabled"));
	gtk_fixed_put(GTK_FIXED(fixed) , fxset->onlineNotifyBtn , 40 , 117);

	fxset->closeSysMsgBtn = gtk_check_button_new_with_label(_("Disable System Message"));
	gtk_fixed_put(GTK_FIXED(fixed), fxset->closeSysMsgBtn, 230, 117);

	fxset->closeShowBtn = gtk_check_button_new_with_label(_("Disable Fetion Show"));
	gtk_fixed_put(GTK_FIXED(fixed), fxset->closeShowBtn, 40, 139);

	label2 = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(label2) , _("<b>Auto Reply</b>"));
	gtk_fixed_put(GTK_FIXED(fixed) , label2 , 20 , 167);

	fxset->apBtn = gtk_check_button_new_with_label(_("Enable"));
	g_signal_connect(fxset->apBtn , "toggled" , G_CALLBACK(fx_set_on_autoreply_toggled) , fxset);
	gtk_fixed_put(GTK_FIXED(fixed) , fxset->apBtn , 140 , 167);

	fxset->apEty = gtk_text_view_new();
	gtk_widget_set_sensitive(fxset->apEty , FALSE);
	gtk_widget_set_usize(fxset->apEty , 380 , 20);
	apScr = gtk_scrolled_window_new(NULL , NULL);
	gtk_container_add(GTK_CONTAINER(apScr) , fxset->apEty);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(apScr)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_NEVER);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(apScr)
									  , GTK_SHADOW_ETCHED_IN);
	gtk_fixed_put(GTK_FIXED(fixed) , apScr , 40 , 192);

	label3 = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(label3) , _("<b>Send Message</b>"));
	gtk_fixed_put(GTK_FIXED(fixed) , label3 , 20 , 222);

	fxset->etBtn = gtk_radio_button_new_with_label(NULL , _("Press Enter to Send"));
	gtk_fixed_put(GTK_FIXED(fixed) , fxset->etBtn , 40 , 240);
	gl = gtk_radio_button_get_group(GTK_RADIO_BUTTON(fxset->etBtn));
	fxset->ctBtn = gtk_radio_button_new_with_label(gl , _("Press CTRL + Enter to Send"));
	gtk_fixed_put(GTK_FIXED(fixed) , fxset->ctBtn , 40 , 260);

	gtk_box_pack_start_defaults(GTK_BOX(fxset->ssetting) , fixed);

	fx_set_bind_system(fxset);
}

GtkTreeModel* fx_set_create_gender_model()
{
	struct
	{
		const char* name;
		int id;
	} genders[] = 
	{
		{ N_("Male") , 1 } ,
		{ N_("Female") , 2 } ,
		{ N_("Secrecy") , 0} ,
		{ NULL , 1}
	};
	GtkTreeStore* model = gtk_tree_store_new(COMBO_COLS_NUM
										, G_TYPE_STRING
										, G_TYPE_INT);
	GtkTreeIter iter;
	int i;

	for(i = 0; genders[i].name != NULL ; i++)
	{
		gtk_tree_store_append(model , &iter , NULL);
		gtk_tree_store_set(model , &iter
						 , COMBO_NAME_COL , _(genders[i].name)
						 , COMBO_ID_COL , genders[i].id
						 , -1);
	}
	return GTK_TREE_MODEL(model);
}

