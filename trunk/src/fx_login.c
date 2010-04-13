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

FxLogin* fx_login_new()
{
	FxLogin* fxlogin = (FxLogin*)malloc(sizeof(FxLogin));

	DEBUG_FOOTPRINT();

	memset(fxlogin , 0 , sizeof(FxLogin));
	return fxlogin;
}
void fx_login_free(FxLogin* fxlogin)
{
	DEBUG_FOOTPRINT();

	gtk_widget_destroy(fxlogin->fixed);
	free(fxlogin);
}

gboolean fx_login_proxy_button_func(GtkWidget *widget , GdkEventButton *event , gpointer data)
{
	FxLogin *fxlogin = (FxLogin*)data;
	Proxy *proxy = fxlogin->proxy;
	FxProxy *fxproxy = NULL;
	char text[1024];

	DEBUG_FOOTPRINT();

	bzero(text , sizeof(text));

	switch(event->type)
	{
		case GDK_ENTER_NOTIFY :
			sprintf(text , "<span color='#7ce1a9'><small> 网络代理[%s]</small></span>"
					, (proxy == NULL || !proxy->proxyEnabled) ? "关闭" : "开启");
			gtk_label_set_markup(GTK_LABEL(fxlogin->proxyLabel) , text);
			break;
		case GDK_LEAVE_NOTIFY :
			sprintf(text , "<span color='#0099ff'><small> 网络代理[%s]</small></span>"
					, (proxy == NULL || !proxy->proxyEnabled) ? "关闭" : "开启");
			gtk_label_set_markup(GTK_LABEL(fxlogin->proxyLabel) , text);
			break;
		case GDK_BUTTON_PRESS :
			fxproxy = fx_proxy_new(fxlogin);
			fx_proxy_initialize(fxproxy);
			gtk_dialog_run(GTK_DIALOG(fxproxy->dialog));
			gtk_widget_destroy(fxproxy->dialog);
			break;
	}
	return TRUE;
}

void fx_login_initialize(FxMain* fxmain)
{
	FxLogin* fxlogin = fxmain->loginPanel;
	GtkTreeModel* stateModel = NULL;
	GtkCellRenderer* renderer = NULL;
	GtkWidget* img = NULL;
	GtkWidget* noentry = NULL;
	Config* config = NULL;
	GtkTreeModel* model = NULL;
	GtkWidget *proxyHbox = NULL;
	GtkWidget *label = NULL;
	Proxy *proxy = NULL;
	char text[1024];

	DEBUG_FOOTPRINT();
	
	config = fetion_config_new();
	/**
	 * load proxy information
	 */
	proxy = fetion_config_load_proxy();
	fxlogin->proxy = proxy;
	
	model = fx_login_create_user_model(config);
	fxlogin->username = gtk_combo_box_entry_new_with_model(model , 0);
	noentry = gtk_bin_get_child(GTK_BIN(fxlogin->username));
	gtk_widget_set_size_request(GTK_WIDGET(fxlogin->username) , 200 , 25);

	g_signal_connect(fxlogin->username , "changed" , G_CALLBACK(fx_login_user_change_func) , fxlogin);

	fxlogin->userlabel = gtk_label_new("请输入手机号或飞信号:");
	gtk_label_set_justify(GTK_LABEL(fxlogin->userlabel) , GTK_JUSTIFY_CENTER);

	fxlogin->password = gtk_entry_new();
	gtk_widget_set_size_request(GTK_WIDGET(fxlogin->password) , 200 , 25);
	gtk_entry_set_visibility(GTK_ENTRY(fxlogin->password) , FALSE);

	fxlogin->passlabel = gtk_label_new("请输入密码:");
	gtk_label_set_justify(GTK_LABEL(fxlogin->passlabel) , GTK_JUSTIFY_CENTER);

	fxlogin->label = gtk_label_new("");
	gtk_widget_set_size_request(GTK_WIDGET(fxlogin->label) , WINDOW_WIDTH - 10 , 25);
	gtk_label_set_justify(GTK_LABEL(fxlogin->label) , GTK_JUSTIFY_CENTER);

	fxlogin->loginbutton = gtk_button_new_with_label("登录");

	img = gtk_image_new_from_file(SKIN_DIR"move.gif");
	gtk_button_set_image(GTK_BUTTON(fxlogin->loginbutton) , img);

	fxlogin->loginFuncId = g_signal_connect(G_OBJECT(fxlogin->loginbutton)
								   , "clicked"
								   , G_CALLBACK(fx_login_action_func)
								   , fxmain);
	gtk_widget_set_size_request(GTK_WIDGET(fxlogin->loginbutton) , 80 , 30);

	stateModel = fx_login_create_state_model();	
	fxlogin->statecombo = gtk_combo_box_new_with_model(stateModel);
	g_object_unref(stateModel);
	renderer = gtk_cell_renderer_pixbuf_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (fxlogin->statecombo), renderer, FALSE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (fxlogin->statecombo), renderer,
				    "pixbuf", 0, NULL);
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (fxlogin->statecombo), renderer, FALSE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (fxlogin->statecombo), renderer,
				    "text", 1, NULL);
	gtk_combo_box_set_active (GTK_COMBO_BOX (fxlogin->statecombo), 3);

	gtk_widget_set_usize(GTK_WIDGET(fxlogin->statecombo) , 120 , 32);

	fxlogin->remember = gtk_check_button_new_with_label("记住密码");
	fxlogin->proxyBtn = gtk_event_box_new();

	fxlogin->proxyLabel = gtk_label_new(NULL);
	proxyHbox = gtk_hbox_new(FALSE , FALSE);
	img = gtk_image_new_from_file(SKIN_DIR"proxy.png");
	bzero(text , sizeof(text));
	sprintf(text , "<span color='#0099ff'><small> 网络代理[%s]</small></span>"
			, (fxlogin->proxy == NULL || ! fxlogin->proxy->proxyEnabled) ? "关闭"  : "开启");

	gtk_label_set_markup(GTK_LABEL(fxlogin->proxyLabel) , text);
	gtk_container_add(GTK_CONTAINER(fxlogin->proxyBtn) , proxyHbox);
	gtk_box_pack_start_defaults(GTK_BOX(proxyHbox) , img);
	gtk_box_pack_start_defaults(GTK_BOX(proxyHbox) , fxlogin->proxyLabel);

	g_signal_connect(G_OBJECT(fxlogin->proxyBtn)
				   , "button_press_event"
				   , GTK_SIGNAL_FUNC(fx_login_proxy_button_func)
				   , fxlogin);
				 
	g_signal_connect(G_OBJECT(fxlogin->proxyBtn)
				   , "enter_notify_event"
				   , GTK_SIGNAL_FUNC(fx_login_proxy_button_func)
				   , fxlogin);

	g_signal_connect(G_OBJECT(fxlogin->proxyBtn)
				   , "leave_notify_event"
				   , GTK_SIGNAL_FUNC(fx_login_proxy_button_func)
				   , fxlogin);

	fx_login_set_last_login_user(fxlogin);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label) , "<small>welcome to openfetion</small>");

	fxlogin->fixed = gtk_fixed_new();
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , label , 50 , 20);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->userlabel , 20 ,80);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->username , (WINDOW_WIDTH - 200)/2 , 100 );
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->passlabel , 20 , 130);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->password , (WINDOW_WIDTH - 200)/2 , 150);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->statecombo , (WINDOW_WIDTH - 120)/2 , 180);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->remember , (WINDOW_WIDTH - 80)/2 , 220);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->label , 5 , 240);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->loginbutton , (WINDOW_WIDTH - 80)/2 , 270);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->proxyBtn , (WINDOW_WIDTH - 100) / 2 , 320);
	gtk_box_pack_start(GTK_BOX(fxmain->mainbox) , fxlogin->fixed , TRUE , TRUE , 0);
	gtk_widget_show_all(fxmain->mainbox);
}
GtkTreeModel* fx_login_create_state_model()
{
	GtkListStore* store = NULL;
	GtkTreeIter iter;
	GdkPixbuf* pb = NULL;
	int i;

	DEBUG_FOOTPRINT();

	struct 
	{
		const gchar* name;
		const gchar* icon;
		int type;
	} presence[] = {
		{ "上线"	 , SKIN_DIR"user_online.png" , P_ONLINE } , 
		{ "离开"	 , SKIN_DIR"user_away.png" , P_AWAY } , 
		{ "忙碌"	 , SKIN_DIR"user_busy.png" , P_BUSY } ,
		{ "隐身"	 , SKIN_DIR"user_invisible.png" , P_HIDDEN } , 
		{ "外出就餐" , SKIN_DIR"user_away.png" , P_OUTFORLUNCH } ,
		{ "请勿打扰" , SKIN_DIR"user_away.png" , P_DONOTDISTURB } , 
		{ "马上回来" , SKIN_DIR"user_away.png" , P_RIGHTBACK } , 
		{ "会议中"	 , SKIN_DIR"user_away.png" , P_MEETING } , 
		{ "电话中"	 , SKIN_DIR"user_away.png" , P_ONTHEPHONE} ,
		{ NULL		 , NULL 			   , -1}
	};
	enum
	{
		PIXBUF_COL , 
		TEXT_COL , 
		INT_COL
	};
	store = gtk_list_store_new(3 , GDK_TYPE_PIXBUF , G_TYPE_STRING , G_TYPE_INT);

	for(i = 0 ; presence[i].type != -1 ; i++)
	{
		gtk_list_store_append(store , &iter);
		pb = gdk_pixbuf_new_from_file(presence[i].icon , NULL);
		gtk_list_store_set(store , &iter
				, PIXBUF_COL , pb 
				, TEXT_COL , presence[i].name
				, INT_COL , presence[i].type , -1);
	}
	return GTK_TREE_MODEL(store);

}
void fx_login_show_msg(FxLogin *fxlogin , const char *msg)
{
	gdk_threads_enter();
	gtk_label_set_text(GTK_LABEL(fxlogin->label) , msg);	
	update();
	gdk_threads_leave();
}
void* fx_login_thread_func(void* data)
{
	FxMain* fxmain = (FxMain*)data;
	FxLogin* fxlogin = fxmain->loginPanel;
	FetionConnection* conn = NULL;					 /* connection for sipc 		   */
	const char *no = NULL , *password = NULL;
	char *pos , *nonce , *key , *aeskey , *response; /* string used for authentication */
	Config* config = NULL;							 /* global user config 			   */
	Group* group = NULL;							 /* buddy list		  			   */
	User* user = NULL;								 /* global user information 	   */
	char code[20];									 /* store reply code   			   */
	char statusTooltip[128];
	UserList* ul = NULL;
	UserList* newEntry = NULL;
	UserList* ulPos = NULL;

	GtkTreeIter stateIter;
	GtkTreeModel* stateModel = NULL;
	int state;

	FxCode* fxcode = NULL;
	int ret;

	DEBUG_FOOTPRINT();

	fx_login_show_msg(fxlogin , "正在准备登录");	

	no = gtk_combo_box_get_active_text(GTK_COMBO_BOX(fxlogin->username));
	password = gtk_entry_get_text(GTK_ENTRY(fxlogin->password));
	user = fetion_user_new(no , password);
	fx_main_set_user(fxmain , user);

	config = fetion_config_new();
	if(user == NULL)
	{
		fx_login_show_msg(fxlogin , "登录失败");
		return NULL;
	}

	/* set the proxy structure to config */
	config->proxy = fxlogin->proxy;
	/* set the config structure to user */
	fetion_user_set_config(user , config);
login:
	pos = ssi_auth_action(user);
	if(pos == NULL)
	{
		fx_login_show_msg(fxlogin , "登录失败");
		return NULL;
	}
	parse_ssi_auth_response(pos , user);
	free(pos);
	if(user->loginStatus == 421 || user->loginStatus == 420)
	{
		debug_info(user->verification->text);
		debug_info(user->verification->tips);
		generate_pic_code(user);
		gdk_threads_enter();
		fxcode = fx_code_new(fxmain , user->verification->text , user->verification->tips , CODE_NOT_ERROR);
		fx_code_initialize(fxcode);
		ret = gtk_dialog_run(GTK_DIALOG(fxcode->dialog));
		if(ret == GTK_RESPONSE_OK)
		{
			strcpy(code , gtk_entry_get_text(GTK_ENTRY(fxcode->codeentry)));
			fetion_user_set_verification_code(user , code);
			gtk_widget_destroy(fxcode->dialog);
			gdk_threads_leave();
		}
		else
		{
			gtk_widget_destroy(fxcode->dialog);
			gdk_threads_leave();
			return NULL;
		}
		debug_info("Input verfication code:%s" , code);
		goto login;
	}
	if(user->loginStatus == 401 || user->loginStatus == 400)
	{
		debug_info("Password error!!!");
		fx_login_show_msg(fxlogin , "登录失败，手机号或密码错误");
		return NULL;
	}
	
	fx_login_show_msg(fxlogin , "正在加载本地用户信息");

	fetion_config_initialize(config , user->userId);

	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(fxlogin->statecombo) , &stateIter);
	stateModel = gtk_combo_box_get_model(GTK_COMBO_BOX(fxlogin->statecombo));
	gtk_tree_model_get(stateModel , &stateIter , 2 , &state , -1);

	/**
	 * set user list to be stored in local file
	 */
	ul = fetion_user_list_load(config);
	if(ul == NULL)
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxlogin->remember)))
			newEntry = fetion_user_list_new(no , password , state , 1);
		else
			newEntry = fetion_user_list_new(no , NULL , state , 1);
		ul = newEntry;
	}
	else
	{
		newEntry = fetion_user_list_find_by_no(ul , no);
		bzero(newEntry->password , sizeof(newEntry->password));
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxlogin->remember)))
			strcpy(newEntry->password , password);
		newEntry->laststate = state;
		ulPos = ul;
		while(ulPos != NULL)
		{
			ulPos->islastuser = 0;
			ulPos = ulPos->next;
		}
		newEntry->islastuser = 1;
	}
	fetion_user_list_save(config , ul);

	/**
	 * download xml configuration file from the server
	 */
	fx_login_show_msg(fxlogin , "正在下载配置文件");
	fetion_user_load(user);
	fetion_config_download_configuration(user);
	fetion_config_load_xml(user);
	fetion_config_load_data(user);

	user->state = state;

	/**
	 * start a new tcp connection for registering to sipc server
	 */
	
	conn = tcp_connection_new();
	if(config->proxy->proxyEnabled)
	{
		fx_login_show_msg(fxlogin , "正在连接到代理服务器");
		tcp_connection_connect_with_proxy(conn , config->sipcProxyIP , config->sipcProxyPort , config->proxy);
	}
	else
	{
		fx_login_show_msg(fxlogin , "正在连接到注册服务器");
		tcp_connection_connect(conn , config->sipcProxyIP , config->sipcProxyPort);
	}
	FetionSip* sip = fetion_sip_new(conn , user->sId);
	fetion_user_set_sip(user , sip);

	fx_login_show_msg(fxlogin , "正在注册到SIPC服务器");
	pos = sipc_reg_action(user);
	if(pos == NULL)
	{
		fx_login_show_msg(fxlogin , "登录失败");
		return;
	}
	parse_sipc_reg_response(pos , &nonce , &key);
	free(pos);

	aeskey = generate_aes_key();
	response = generate_response(nonce , user->userId , user->password , key , aeskey);
	free(nonce);
	free(key);
	free(aeskey);

	/**
	 * start sipc authentication using the response created just now
	 */
	fx_login_show_msg(fxlogin , "正在进行SIPC身份验证");
auth:
	pos = sipc_aut_action(user , response);
	if(pos == NULL)
	{
		fx_login_show_msg(fxlogin , "登录失败");
		return;
	}
	parse_sipc_auth_response(pos , user);
	free(pos); pos = NULL;
	if(user->loginStatus == 401 || user->loginStatus == 400)
	{
		debug_info("Password error , login failed!!!");
		fx_login_show_msg(fxlogin , "身份验证失败，手机号或密码错误");
		return NULL;
	}
	if(user->loginStatus == 421 || user->loginStatus == 420)
	{
		debug_info(user->verification->text);
		debug_info(user->verification->tips);
		generate_pic_code(user);
		gdk_threads_enter();
		fxcode = fx_code_new(fxmain , user->verification->text , user->verification->tips , CODE_NOT_ERROR);
		fx_code_initialize(fxcode);
		ret = gtk_dialog_run(GTK_DIALOG(fxcode->dialog));
		if(ret == GTK_RESPONSE_OK)
		{
			strcpy(code , gtk_entry_get_text(GTK_ENTRY(fxcode->codeentry)));
			fetion_user_set_verification_code(user , code);
			gtk_widget_destroy(fxcode->dialog);
			gdk_threads_leave();
			goto auth;
		}
		else
		{
			gtk_widget_destroy(fxcode->dialog);
			gdk_threads_leave();
			return NULL;
		}
		debug_info("Input verfication code:%s" , code);
	}
	
	fx_login_show_msg(fxlogin , "登录成功");
	gdk_threads_enter();
	gtk_window_set_resizable(GTK_WINDOW(fxmain->window) , TRUE);
	gdk_threads_leave();

	/**
	 *  if there is not a buddylist name "未分组" or "陌生人", create one
	 */
	if(fetion_group_list_find_by_id(user->groupList , BUDDY_LIST_NOT_GROUPED) == NULL)
	{
		group = fetion_group_new();
		group->groupid = BUDDY_LIST_NOT_GROUPED;
		strcpy(group->groupname , "未分组");
		fetion_group_list_append(user->groupList , group);
	}
	if(fetion_group_list_find_by_id(user->groupList , BUDDY_LIST_STRANGER) == NULL)
	{
		group = fetion_group_new();
		group->groupid = BUDDY_LIST_STRANGER;
		strcpy(group->groupname , "陌生人");
		fetion_group_list_append(user->groupList , group);
	}
	gdk_threads_enter();

	fx_login_free(fxlogin);
	fxmain->loginPanel = NULL;
	/**
	 * initialize main panel which in fact only contains a treeview
	 */
	fx_head_initialize(fxmain);
	fxmain->mainPanel = fx_tree_new();
	fx_tree_initilize(fxmain);
	update();
	fx_bottom_initialize(fxmain);
	update();
	/**
	 * set tooltip of status icon
	 */
	bzero(statusTooltip , sizeof(statusTooltip));
	sprintf(statusTooltip , "%s\n%s" , user->nickname , user->mobileno);
	gtk_status_icon_set_tooltip(GTK_STATUS_ICON(fxmain->trayIcon) , statusTooltip);
	/**
	 * set title of main window
	 */
	gtk_window_set_title(GTK_WINDOW(fxmain->window) , user->nickname );
	gdk_threads_leave();
	/**
	 * start sending keep alive request periodically
	 */
	g_timeout_add_seconds(180 , (GSourceFunc)fx_main_register_func , user);

	g_thread_exit(0);
	return NULL;
}
void fx_login_action_func(GtkWidget* widget , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	fxmain->loginPanel->loginThread = g_thread_create(fx_login_thread_func , fxmain , FALSE , NULL);
}
GtkTreeModel* fx_login_create_user_model(Config* config)
{
	GtkTreeStore* model = gtk_tree_store_new(USER_COLS_NUM
										   , G_TYPE_STRING
										   , G_TYPE_STRING
										   , G_TYPE_INT
										   , G_TYPE_INT);
	UserList* list = fetion_user_list_load(config);
	UserList* pos = list;

	DEBUG_FOOTPRINT();

	while(pos != NULL)
	{
		GtkTreeIter iter;
		gtk_tree_store_append(model , &iter , NULL);
		gtk_tree_store_set(model    , &iter 
						 , L_NO_COL   , pos->no
						 , L_PWD_COL  , pos->password
						 , L_STATE_COL, pos->laststate
						 , L_LAST_COL , pos->islastuser
						 , -1);
		pos = pos->next;
	}
	return GTK_TREE_MODEL(model);
}
void fx_login_set_last_login_user(FxLogin* fxlogin)
{
	GtkComboBox* combo = GTK_COMBO_BOX(fxlogin->username);
	GtkTreeModel* model = gtk_combo_box_get_model(combo);
	GtkTreeIter iter;
	char *no , *pwd;
	int state , last;

	DEBUG_FOOTPRINT();

	if(!gtk_tree_model_get_iter_root(model , &iter))
		return;
	do
	{
		gtk_tree_model_get(model    , &iter
						 , L_NO_COL   , &no
						 , L_PWD_COL  , &pwd
						 , L_STATE_COL, &state
						 , L_LAST_COL , &last
						 , -1);
		if(last == 1)
		{
			gtk_combo_box_set_active_iter(combo , &iter);
			gtk_entry_set_text(GTK_ENTRY(fxlogin->password) , pwd);
			fx_login_set_last_login_state(fxlogin , state);	
			if(strlen(pwd) != 0)
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxlogin->remember) , TRUE);
			break;
		}
	}
	while(gtk_tree_model_iter_next(model , &iter));
}
void fx_login_set_last_login_state(FxLogin* fxlogin , StateType state)
{
	GtkTreeModel* model = gtk_combo_box_get_model(GTK_COMBO_BOX(fxlogin->statecombo));
	GtkTreeIter iter;
	int s;

	DEBUG_FOOTPRINT();

	gtk_tree_model_get_iter_root(model , &iter);
	do
	{
		gtk_tree_model_get(model , &iter , 2 , &s , -1);
		if(s == state)
			gtk_combo_box_set_active_iter(GTK_COMBO_BOX(fxlogin->statecombo) , &iter);
	}
	while(gtk_tree_model_iter_next(model , &iter));

}
void fx_login_user_change_func(GtkWidget* widget , gpointer data)
{
	FxLogin* fxlogin = (FxLogin*)data;
	char* pwd = NULL;
	int state;
	GtkComboBox* combo = GTK_COMBO_BOX(widget);
	GtkTreeModel* model = gtk_combo_box_get_model(combo);
	GtkTreeIter iter;

	DEBUG_FOOTPRINT();

	if(!gtk_combo_box_get_active_iter(combo , &iter))
		return;
	gtk_tree_model_get(model , &iter , L_PWD_COL , &pwd , L_STATE_COL , &state ,  -1);
	gtk_entry_set_text(GTK_ENTRY(fxlogin->password) , pwd);
	fx_login_set_last_login_state(fxlogin , state);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxlogin->remember)
							   , strlen(pwd) == 0 ? FALSE : TRUE);
}
