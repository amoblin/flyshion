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


FxApp* fx_app_new(FxMain* fxmain , const char* sipuri , const char* userid , const char* desc , int phrase)
{
	FxApp *fxapp = (FxApp*)malloc(sizeof(FxApp));

	DEBUG_FOOTPRINT();

	memset(fxapp , 0 , sizeof(FxApp));
	fxapp->fxmain = fxmain;
	fxapp->phraseid = phrase;
	strcpy(fxapp->sipuri , sipuri);
	strcpy(fxapp->userid , userid);
	strcpy(fxapp->desc   , desc);
	return fxapp;
}

void fx_app_initialize(FxApp* fxapp)
{
	GtkWidget *fixed = NULL;
	GdkPixbuf *pb = NULL;
	GtkWidget *okButton = NULL;
	GtkWidget *cancelButton = NULL;
	GtkWidget *lnLabel = NULL;
	GtkWidget *agLabel = NULL;
	GtkWidget *ifoButton = NULL;
	GtkTreeModel *model = NULL;
	GtkWidget *ifoEntry = NULL;

	FxMain *fxmain = fxapp->fxmain;
	Config *config = fxmain->user->config;
	FxList *phraseList , *pl_cur;
	Phrase *phrase = NULL;

	char text[1024];

	DEBUG_FOOTPRINT();

	fxapp->dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(fxapp->dialog) , "收到一个添加好友请求");
	pb = gdk_pixbuf_new_from_file(SKIN_DIR"user_online.png" , NULL);
	gtk_window_set_modal(GTK_WINDOW(fxapp->dialog) , FALSE);
	gtk_window_set_transient_for(GTK_WINDOW(fxapp->dialog) , GTK_WINDOW(fxmain->window));
	gtk_window_set_icon(GTK_WINDOW(fxapp->dialog) , pb);
	gtk_window_set_resizable(GTK_WINDOW(fxapp->dialog) , FALSE);
	gtk_widget_set_usize(fxapp->dialog , 440 , 280);

	fixed = gtk_fixed_new();

	fxapp->msgLabel = gtk_label_new("");
	phraseList = fetion_config_get_phrase(config);
	foreach_list(phraseList , pl_cur){
		phrase = (Phrase*)pl_cur->data;
		if(fxapp->phraseid == phrase->phraseid){
			bzero(text , sizeof(text));
			sprintf(text , "您好，我是%s，%s" , fxapp->desc , phrase->content);
		}
		fetion_phrase_free(phrase);
	}
	gtk_label_set_markup(GTK_LABEL(fxapp->msgLabel) , text);
	gtk_label_set_line_wrap(GTK_LABEL(fxapp->msgLabel) , TRUE);
	gtk_widget_set_usize(fxapp->msgLabel , 400 , 30);
	gtk_fixed_put(GTK_FIXED(fixed) , fxapp->msgLabel , 20 , 20);

	fxapp->aptButton = gtk_radio_button_new_with_label(NULL , "同意并添加对方为好友");
	fxapp->rdoList = gtk_radio_button_get_group(GTK_RADIO_BUTTON(fxapp->aptButton));
	gtk_fixed_put(GTK_FIXED(fixed) , fxapp->aptButton , 20 , 50);

	lnLabel = gtk_label_new("备注名称:");
	gtk_fixed_put(GTK_FIXED(fixed) , lnLabel , 40 , 85);
	
	fxapp->lnEntry = gtk_entry_new();
	printf("%s , %d\n" , fxapp->desc , strlen(fxapp->desc) );
	gtk_entry_set_text(GTK_ENTRY(fxapp->lnEntry) , fxapp->desc);
	gtk_widget_set_usize(fxapp->lnEntry , 200 , 28);
	gtk_fixed_put(GTK_FIXED(fixed) , fxapp->lnEntry , 120 , 80);

	ifoButton = gtk_button_new_with_label("查看信息");
	g_signal_connect(ifoButton , "clicked" , G_CALLBACK(fx_app_on_check_clicked) , fxapp);
	gtk_widget_set_usize(ifoButton , 80 , 65);
	gtk_fixed_put(GTK_FIXED(fixed) , ifoButton , 340 , 80);

	agLabel = gtk_label_new("添加到组:");
	gtk_fixed_put(GTK_FIXED(fixed) , agLabel , 40 , 120);

	model = fx_app_create_group_model(fxmain);
	fxapp->agCombo = gtk_combo_box_entry_new_with_model(model , APP_G_NAME_COL);
	ifoEntry = gtk_bin_get_child(GTK_BIN(fxapp->agCombo));
	gtk_entry_set_editable(GTK_ENTRY(ifoEntry) , FALSE);
	gtk_combo_box_set_active(GTK_COMBO_BOX(fxapp->agCombo) , 0);
	gtk_widget_set_usize(fxapp->agCombo , 200 , 30);
	gtk_fixed_put(GTK_FIXED(fixed) , fxapp->agCombo , 120 , 115);

	fxapp->rjtButton = gtk_radio_button_new_with_label(fxapp->rdoList , "拒绝对方的添加好友请求");
	gtk_fixed_put(GTK_FIXED(fixed) , fxapp->rjtButton , 20 , 150);

	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxapp->dialog)->vbox) , fixed);

	okButton = gtk_button_new_with_label("确定");
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxapp->dialog)->action_area) , okButton);
	g_signal_connect(okButton , "clicked" , G_CALLBACK(fx_app_on_ok_clicked) , fxapp);
	cancelButton = gtk_button_new_with_label("取消");
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxapp->dialog)->action_area) , cancelButton);
	g_signal_connect(cancelButton , "clicked" , G_CALLBACK(fx_app_on_cancel_clicked) , fxapp);

	gtk_widget_show_all(fxapp->dialog);
	gtk_widget_hide(fxapp->dialog);
}
GtkTreeModel* fx_app_create_group_model(FxMain* fxmain)
{
	GtkTreeStore *store = NULL;
	GtkTreeModel *model = NULL;
	GtkTreeView *tree = NULL;
	GtkTreeIter oIter , nIter;
	char *groupname = NULL;
	int groupid;

	DEBUG_FOOTPRINT();

	tree = GTK_TREE_VIEW(fxmain->mainPanel->treeView);
	model = gtk_tree_view_get_model(tree);

	store = gtk_tree_store_new(APP_G_COL_NUM , G_TYPE_STRING , G_TYPE_INT);

	gtk_tree_model_get_iter_root(model , &oIter);
	do{
		gtk_tree_model_get(model 	   , &oIter
						 , G_NAME_COL , &groupname
						 , G_ID_COL	  , &groupid
						 , -1);
		gtk_tree_store_append(store , &nIter , NULL);
		gtk_tree_store_set(store 		  , &nIter
						 , APP_G_NAME_COL , groupname
						 , APP_G_ID_COL	  , groupid
						 , -1);
	}while(gtk_tree_model_iter_next(model , &oIter));

	return GTK_TREE_MODEL(store);
}
void* fx_app_ok_thread(void* data)
{
	FxApp *fxapp = (FxApp*)data;
	const char *localname = NULL;
	int buddylist;
	int groupid;
	int result;
	int allCount;
	int onlineCount;
	FxMain *fxmain = fxapp->fxmain;
	GtkTreeView *tree = GTK_TREE_VIEW(fxmain->mainPanel->treeView);
	GtkTreeModel *model = gtk_tree_view_get_model(tree);
	GtkTreeModel *gmodel = NULL;
	GtkTreeIter iter , iter1 , piter;
	Contact* contact = NULL;
	GdkPixbuf* pb = NULL;
	Config *config = NULL;
	char portraitPath[128];
	char *sid = NULL;
	
	DEBUG_FOOTPRINT();

	gmodel = gtk_combo_box_get_model(GTK_COMBO_BOX(fxapp->agCombo));
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(fxapp->agCombo) , &iter);
	gtk_tree_model_get(gmodel , &iter , APP_G_ID_COL , &buddylist , -1);




	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxapp->aptButton)))
		result = APP_ACCEPT;
	else
		result = APP_REJECT;

	localname = gtk_entry_get_text(GTK_ENTRY(fxapp->lnEntry));

	localname = "";

	if(result == APP_REJECT){
		contact = fetion_contact_handle_contact_request(fxmain->user
						, fxapp->sipuri , fxapp->userid , localname , buddylist , result);
		return NULL;
	}

	gtk_tree_model_get_iter_root(model , &piter);
	do
	{
		gtk_tree_model_get(model			  , &piter
						 , G_ID_COL 		  , &groupid
						 , G_ALL_COUNT_COL    , &allCount
						 , G_ONLINE_COUNT_COL , &onlineCount 
						 , -1);
		if(groupid == buddylist)
		{
			allCount ++;
			gtk_tree_store_set(GTK_TREE_STORE(model)  , &piter
							 , G_ALL_COUNT_COL		  , allCount
							 , G_ONLINE_COUNT_COL	  , onlineCount
							 , -1);
			break;
		}
	}
	while(gtk_tree_model_iter_next(model , &piter));


	gtk_tree_store_append(GTK_TREE_STORE(model) , &iter1 , &piter);

	fetion_user_download_portrait(fxmain->user , fxapp->sipuri);

	config = fxmain->user->config;

	sid = fetion_sip_get_sid_by_sipuri(fxapp->sipuri);

	bzero(portraitPath , sizeof(portraitPath));
	sprintf(portraitPath , "%s/%s.jpg" , config->iconPath , sid);
	free(sid);

	pb = gdk_pixbuf_new_from_file_at_size(portraitPath
			, config->iconSize , config->iconSize , NULL);
	if(pb == NULL)
		pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"user_online.png"
				, config->iconSize , config->iconSize , NULL);

	gtk_tree_store_set(GTK_TREE_STORE(model) , &iter1
					, B_PIXBUF_COL , pb
					, B_SIPURI_COL , fxapp->sipuri
					, -1);

	contact = fetion_contact_handle_contact_request(fxmain->user
					, fxapp->sipuri , fxapp->userid , localname , buddylist , result);
	printf("CONTACT : %d\n" , contact == NULL);

	if(contact == NULL){
		gtk_tree_store_remove(GTK_TREE_STORE(model) , &iter1);
		return;
	}	

	gtk_tree_store_set(GTK_TREE_STORE(model)		 , &iter1
					, B_USERID_COL			, contact->userId
					, B_NAME_COL			, contact->nickname ? contact->nickname : ""
					, B_SERVICESTATUS_COL	, BASIC_SERVICE_NORMAL
					, B_RELATIONSTATUS_COL	, contact->relationStatus
					, B_CARRIER_COL			, "CMCC"
					, B_CARRIERSTATUS_COL	, CARRIER_STATUS_NORMAL
					, B_DEVICE_COL			, "PC"
					, -1);

	return NULL;
}
void fx_app_on_ok_clicked(GtkWidget* widget , gpointer data)
{
	FxApp *fxapp = (FxApp*)data;
	GThread *thread;

	DEBUG_FOOTPRINT();

	thread = g_thread_create(fx_app_ok_thread , data , TRUE , NULL);

	gtk_dialog_response(GTK_DIALOG(fxapp->dialog) , GTK_RESPONSE_OK);
}

void fx_app_on_cancel_clicked(GtkWidget* widget , gpointer data)
{
	FxApp *fxapp = (FxApp*)data;

	DEBUG_FOOTPRINT();

	gtk_dialog_response(GTK_DIALOG(fxapp->dialog) , GTK_RESPONSE_CANCEL);
}

void* fx_app_check_thread(void* data)
{
	FxApp *fxapp = (FxApp*)data;
	User* user = fxapp->fxmain->user;
	char* sid = NULL;
	Contact* contact = NULL;
	FxLookupres* fxlookupres = NULL;

	DEBUG_FOOTPRINT();

	sid = fetion_sip_get_sid_by_sipuri(fxapp->sipuri);
	contact = fetion_contact_get_contact_info_by_no(user , sid , FETION_NO);
	free(sid);
	if(contact == NULL)
	{
		gdk_threads_enter();
		fx_util_popup_warning(fxapp->fxmain , "查询失败，对方身份不明,原因未知...");
		gdk_threads_leave();
		return NULL;
	}
	gdk_threads_enter();
	fxlookupres = fx_lookupres_new(fxapp->fxmain , contact);	
	fx_lookupres_initialize(fxlookupres);
	gtk_dialog_run(GTK_DIALOG(fxlookupres->dialog));

	free(contact);

	gtk_widget_destroy(fxlookupres->dialog);
	gdk_threads_leave();	
	return NULL;
}
void fx_app_on_check_clicked(GtkWidget* widget , gpointer data)
{
	g_thread_create(fx_app_check_thread , data , FALSE , NULL);
}
