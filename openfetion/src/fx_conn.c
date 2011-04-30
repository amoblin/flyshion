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

#include <fx_include.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fx_server.h>

int old_state;
extern gint presence_count;
extern struct userlist *ul;

#ifdef USE_NETWORKMANAGER
#include <dbus/dbus-glib.h>
#include <NetworkManager.h>

static DBusGConnection *nm_conn = NULL;
static DBusGProxy *nm_proxy = NULL;

static void
nm_state_change_cb(DBusGProxy *UNUSED(proxy), NMState state, gpointer data)
{
	FxMain *fxmain = (FxMain*)data;
	if(!fxmain->user)
		return;
	switch(state)
	{
		case NM_STATE_CONNECTED:
			debug_info("network is connected");
			fx_conn_reconnect(fxmain, old_state);
			break;
		case NM_STATE_ASLEEP:
			debug_info("network is sleeping...");
			fx_conn_offline(fxmain);
			break;
		case NM_STATE_CONNECTING:
			debug_info("network is connecting...");
			break;
		case NM_STATE_DISCONNECTED:
			debug_info("network is disconnected");
			fx_conn_offline(fxmain);
			break;
		case NM_STATE_UNKNOWN:
			debug_info("unknown network state");
		default:
			break;
	}
}

#endif

void fx_conn_init(FxMain *fxmain)
{
#ifdef USE_NETWORKMANAGER
	GError *error = NULL;
#endif
	connlst = (struct conn_list*)malloc(sizeof(struct conn_list));
	connlst->conn = NULL;
	connlst->next = connlst->pre = connlst;


#ifdef USE_NETWORKMANAGER
	nm_conn = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
	if (!nm_conn) {
		debug_error("Error connecting to DBus System service: %s.\n", error->message);
	} else {
		nm_proxy = dbus_g_proxy_new_for_name(nm_conn,
		                                     NM_DBUS_SERVICE,
		                                     NM_DBUS_PATH,
		                                     NM_DBUS_INTERFACE);
		dbus_g_proxy_add_signal(nm_proxy, "StateChange", G_TYPE_UINT, G_TYPE_INVALID);
		dbus_g_proxy_connect_signal(nm_proxy, "StateChange",
		                        G_CALLBACK(nm_state_change_cb), fxmain, NULL);
	}
#endif
}

void fx_conn_append(FetionConnection *conn)
{
	struct conn_list *tmp;
	tmp = (struct conn_list*)malloc(sizeof(struct conn_list));
	tmp->conn = conn;

	connlst->next->pre = tmp;
	tmp->next = connlst->next;
	tmp->pre = connlst;
	connlst->next = tmp;
}

static void fx_conn_remove(struct conn_list *conn)
{
	conn->next->pre = conn->pre;
	conn->pre->next = conn->next;
}

static void popup_startup_msg(FxMain *fxmain)
{
#ifdef USE_LIBNOTIFY
	gchar             notifyText[1024];
	gchar             iconPath[256];
	GdkPixbuf        *pixbuf;
	FxLogin          *fxlogin = fxmain->loginPanel;
	User             *user = fxmain->user;
	Config           *config = user->config;

	sprintf(iconPath, "%s/%s.jpg",
				  config->iconPath, user->sId);
	sprintf(notifyText ,
			_("Public IP: %s\n"
			  "IP of last login: %s\n"
			  "Time of last login: %s\n"),
		   	user->publicIp , user->lastLoginIp,
		   	user->lastLoginTime);
	pixbuf = gdk_pixbuf_new_from_file_at_size(iconPath,
				   	NOTIFY_IMAGE_SIZE, NOTIFY_IMAGE_SIZE, NULL);
	if(!pixbuf){
		fx_login_show_msg(fxlogin, _("Getting portrait..."));
		fetion_user_download_portrait(user , user->sipuri);
		pixbuf = gdk_pixbuf_new_from_file_at_size(iconPath,
					   	NOTIFY_IMAGE_SIZE, NOTIFY_IMAGE_SIZE, NULL);
		if(!pixbuf)
			pixbuf = gdk_pixbuf_new_from_file_at_size(
						SKIN_DIR"fetion.svg",
						NOTIFY_IMAGE_SIZE, NOTIFY_IMAGE_SIZE, NULL);
	}

	gdk_threads_enter();
	notify_notification_update(fxmain->notify,
			_("Login successful")// notifySummary
			, notifyText , NULL);
	notify_notification_set_icon_from_pixbuf(
			fxmain->notify , pixbuf);
	notify_notification_show(fxmain->notify , NULL);
	gdk_threads_leave();
	g_object_unref(pixbuf);
#endif
}

static void *fx_server_func(void *data)
{
	FxMain *fxmain = (FxMain*)data;
	int   fifo;

	if((fifo = init_server(fxmain)) == -1) {
		fprintf(stderr, "init openfetion server failed\n");
		return (void*)0;
	}
	start_server(fxmain, fifo);
	return (void*)0;
}

int fx_conn_connect(FxMain *fxmain)
{
	FxLogin          *fxlogin = fxmain->loginPanel;
	FetionConnection *conn = NULL;					 /* connection for sipc 		   */
	const gchar      *no;
	const gchar      *password;
	gchar            *pos;
	gchar            *nonce;
	gchar            *key;
	gchar            *aeskey;
	gchar            *response;                      /* string used for authentication */
	Config           *config = NULL;				 /* global user config 			   */
	Group            *group = NULL;					 /* buddy list		  			   */
	User             *user = NULL;					 /* global user information 	   */
	gchar             code[20];						 /* store reply code   			   */
	gchar             statusTooltip[128];
	struct userlist  *newul;
	struct userlist  *ul_cur;

	GtkTreeIter       stateIter;
	GtkTreeModel     *stateModel = NULL;
	gint              state;
	gboolean          remember;

	gint              local_buddy_count;
	gint              local_group_count;
	gint              new_buddy_count;
	gint              new_group_count;
	FxCode           *fxcode = NULL;
	gint              ret;

	/* get login state value */
	gtk_combo_box_get_active_iter(
					GTK_COMBO_BOX(fxlogin->statecombo),
				   	&stateIter);
	stateModel = gtk_combo_box_get_model(
					GTK_COMBO_BOX(fxlogin->statecombo));
	gtk_tree_model_get(stateModel, &stateIter,
				   	2 , &state , -1);
	old_state = state;

	/* get login number and password */
	no = gtk_combo_box_get_active_text(
					GTK_COMBO_BOX(fxlogin->username));
	password = gtk_entry_get_text(
					GTK_ENTRY(fxlogin->password));

	/* get whether to remember password */
	remember = gtk_toggle_button_get_active(
					GTK_TOGGLE_BUTTON(fxlogin->remember));

	user = fetion_user_new(no , password);
	fx_main_set_user(fxmain , user);

	gdk_threads_enter();
	fx_login_hide(fxlogin);
	fx_logining_show(fxmain);
	gdk_threads_leave();

	fx_login_show_msg(fxlogin , _("Preparing for login"));	

	config = fetion_config_new();
	if(!user){
		fx_login_show_err(fxlogin , _("Login failed"));
		goto failed;
	}

	/* set the proxy structure to config */
	config->proxy = fxlogin->proxy;
	/* set the config structure to user */
	fetion_user_set_config(user , config);

login:
	pos = ssi_auth_action(user);
	if(!pos){
		fx_login_show_err(fxlogin , _("Login failed"));
		goto failed;
	}
	parse_ssi_auth_response(pos , user);
	g_free(pos);
	if(USER_AUTH_NEED_CONFIRM(user)){
		debug_info(user->verification->text);
		debug_info(user->verification->tips);
		fx_login_show_msg(fxlogin,
				_("Getting code picture，please wait..."));
		generate_pic_code(user);
		gdk_threads_enter();
		fxcode = fx_code_new(fxmain,
					 user->verification->text,
					 user->verification->tips,
					 CODE_NOT_ERROR);
		fx_code_initialize(fxcode);
		ret = gtk_dialog_run(GTK_DIALOG(fxcode->dialog));
		if(ret == GTK_RESPONSE_OK){
			strcpy(code , gtk_entry_get_text(
							GTK_ENTRY(fxcode->codeentry)));
			fetion_user_set_verification_code(user , code);
			gtk_widget_destroy(fxcode->dialog);
			g_free(fxcode);
			gdk_threads_leave();
		}else{
			gtk_widget_destroy(fxcode->dialog);
			g_free(fxcode);
			gdk_threads_leave();
			g_thread_exit(0);
		}
		debug_info("Input verfication code:%s" , code);
		goto login;
	}

	if(USER_AUTH_ERROR(user)){
		debug_error("password ERROR!!!");
		fx_login_show_err(fxlogin,
			_("Login failed. \nIncorrect cell phone number or password"));
		goto failed;
	}
	
	fx_login_show_err(fxlogin,
				 _("Loading local user information"));

	if(fetion_user_init_config(user) == -1) {
		debug_error("initialize config failed");
		fx_login_show_err(fxlogin , _("Login failed"));
		goto failed;
	}

	/* initialize history */
	fx_main_history_init(fxmain);

	/* set user list to be stored in local file	 */
	newul = fetion_user_list_find_by_no(ul , no);
	if(!newul){
		if(remember)
			newul = fetion_user_list_new(no,
						  password , user->userId,
						  user->sId, state , 1);
		else
			newul = fetion_user_list_new(no,
						  NULL, user->userId,
						  user->sId, state , 1);
		foreach_userlist(ul , ul_cur)
			ul_cur->islastuser = 0;
		fetion_user_list_append(ul , newul);
	}else{
		memset(newul->password,
				 0, sizeof(newul->password));
		if(remember)
			strcpy(newul->password , password);
		newul->laststate = state;
		foreach_userlist(ul , ul_cur)
			ul_cur->islastuser = 0;
		newul->islastuser = 1;
	}
	fetion_user_list_save(config , ul);
	fetion_user_list_free(ul);

	/* download xml configuration file from the server */
	fetion_config_load(user);
	if(config->sipcProxyPort == 0)
		fx_login_show_msg(fxlogin,
					_("Detected that this is the first time you login\n"
					"Downloading configuration file..."));
	else
		fx_login_show_msg(fxlogin,
					_("Downloading configuration files"));
	if(fetion_config_download_configuration(user) < 0){
		fx_login_show_err(fxlogin,
					_("Connection has been shutdown by the server"));
		return -1;
	}
	fetion_config_save(user);

	fetion_user_set_st(user , state);

	/*load local data*/
	fetion_user_load(user);
	fetion_contact_load(user, &local_group_count, &local_buddy_count);

	/* start a new tcp connection for registering to sipc server */
	conn = tcp_connection_new();
	if(config->proxy != NULL && config->proxy->proxyEnabled){
		fx_login_show_msg(fxlogin,
					 _("Connecting to proxy server"));
		tcp_connection_connect_with_proxy(conn,
					   	config->sipcProxyIP,
					   	config->sipcProxyPort,
					   	config->proxy);
	}else{
		fx_login_show_msg(fxlogin,
					  _("Connecting to registration server"));
		int ret = tcp_connection_connect(conn,
					  	config->sipcProxyIP, 
						config->sipcProxyPort);
		if(ret == -1) {
			config->sipcProxyPort = 443;
			ret = tcp_connection_connect(conn,
					  	config->sipcProxyIP, 
						config->sipcProxyPort);
			if(ret == -1) {
				fx_login_show_err(fxlogin , _("Login failed"));
				goto failed;
			}
		}
	}

	/* add the connection object into the connection list */
	fx_conn_append(conn);

	/* initialize a sip object */
	FetionSip* sip = fetion_sip_new(conn , user->sId);

	fetion_user_set_sip(user , sip);

	fx_login_show_msg(fxlogin,
				   	_("Registering to SIPC Server"));
	pos = sipc_reg_action(user);

	if(!pos){
		fx_login_show_err(fxlogin , _("Login failed"));
		goto failed;
	}

	parse_sipc_reg_response(pos , &nonce , &key);
	g_free(pos);

	aeskey = generate_aes_key();
	if(aeskey == NULL){
		goto failed;
	}
	response = generate_response(nonce, user->userId,
				   	user->password, key, aeskey);
	g_free(nonce);
	g_free(key);
	g_free(aeskey);

	/* start sipc authentication using the response created just now */
	fx_login_show_msg(fxlogin , _("SIPC Indentify"));
auth:
	pos = sipc_aut_action(user , response);
	if(pos == NULL){
		fx_login_show_err(fxlogin , _("Login failed"));
		goto failed;
	}

	if(parse_sipc_auth_response(pos , user,
			&new_group_count, &new_buddy_count) < 0){
		debug_info("Password error , login failed!!!");
		fx_login_show_err(fxlogin , _("Authenticate failed."));
		goto failed;
	}
	g_free(pos); pos = NULL;

	if(USER_AUTH_ERROR(user)){
		debug_info("Password error , login failed!!!");
		fx_login_show_err(fxlogin , _("Authenticate failed."));
		goto failed;
	}

	if(USER_AUTH_NEED_CONFIRM(user)){
		debug_info(user->verification->text);
		debug_info(user->verification->tips);
		fx_login_show_msg(fxlogin,
				_("Getting code picture，please wait..."));
		generate_pic_code(user);
		gdk_threads_enter();
		fxcode = fx_code_new(fxmain,
					   	user->verification->text,
					  	user->verification->tips,
					   	CODE_NOT_ERROR);
		fx_code_initialize(fxcode);

		ret = gtk_dialog_run(GTK_DIALOG(fxcode->dialog));
		if(ret == GTK_RESPONSE_OK){
			strcpy(code , gtk_entry_get_text(
							GTK_ENTRY(fxcode->codeentry)));
			fetion_user_set_verification_code(user , code);
			gtk_widget_destroy(fxcode->dialog);
			gdk_threads_leave();
			goto auth;
		}else{
			gtk_widget_destroy(fxcode->dialog);
			gdk_threads_leave();
			goto failed;
		}
		debug_info("Input verfication code:%s" , code);
	}

	Contact *c_cur;
	Contact *c_tmp;
	Group   *g_cur;
	Group   *g_tmp;

	/* update buddy list */
	if(user->groupCount == 0)
		user->groupCount = local_group_count;
	else if(user->groupCount != local_group_count){
		for(g_cur = user->groupList->next;
				g_cur != user->groupList;){
			g_tmp = g_cur;
			g_cur = g_cur->next;
			if(!g_tmp->dirty){
				fetion_group_list_remove(g_tmp);
				g_free(g_tmp);
			}
		}
	}

	/* update buddy count */
	if(user->contactCount == 0)
		user->contactCount = local_buddy_count;
	else if(user->contactCount != local_buddy_count){
		/* do some clean on the local buddy data */
		for(c_cur = user->contactList->next;
				c_cur !=  user->contactList;){
			c_tmp = c_cur;
			c_cur = c_cur->next;
			if(!c_tmp->dirty){
				fetion_contact_list_remove(c_tmp);
				g_free(c_tmp);
			}
		}
	}

	fx_login_show_msg(fxlogin , _("Initializing main panel"));
	
	pg_group_get_list(user);

	popup_startup_msg(fxmain);

	fx_login_show_msg(fxlogin , _("Login sucessful"));

	/*if there is not a buddylist name "Ungrouped" or "Strangers", create one */
	if(fetion_group_list_find_by_id(user->groupList,
			BUDDY_LIST_NOT_GROUPED) == NULL &&
		    fetion_contact_has_ungrouped(user->contactList)){
		group = fetion_group_new();
		group->groupid = BUDDY_LIST_NOT_GROUPED;
		strcpy(group->groupname , N_("Ungrouped"));
		fetion_group_list_append(user->groupList , group);
	}
	if(fetion_group_list_find_by_id(user->groupList,
			BUDDY_LIST_STRANGER) == NULL &&
			fetion_contact_has_strangers(user->contactList)){
		group = fetion_group_new();
		group->groupid = BUDDY_LIST_STRANGER;
		strcpy(group->groupname , N_("Strangers"));
		fetion_group_list_prepend(user->groupList , group);
	}
	fx_login_show_msg(fxlogin , _("Initializing main panel"));

	/* initialize head panel */
	gdk_threads_enter();
	fx_head_initialize(fxmain);
	gdk_threads_leave();

	/* initialize main panel which in fact only contains a treeview*/
	gdk_threads_enter();
	fxmain->mainPanel = fx_tree_new();
	fx_tree_initilize(fxmain);
	gdk_threads_leave();

	/* initialize bottom panel */
	gdk_threads_enter();
	fx_bottom_initialize(fxmain);
	gdk_threads_leave();

	gdk_threads_enter();
	gtk_window_set_resizable(GTK_WINDOW(fxmain->window) , TRUE);
	fetion_config_load_size(config);

	if(config->window_width > 0 && config->window_height >0)
		gtk_window_resize(GTK_WINDOW(fxmain->window),
				config->window_width,
				config->window_height);
	gdk_threads_leave();

	/* set tooltip of status icon */
	sprintf(statusTooltip, "%s\n%s",
			 user->nickname , user->mobileno);

	gdk_threads_enter();
	gtk_status_icon_set_tooltip(
			GTK_STATUS_ICON(fxmain->trayIcon),
		   	statusTooltip);
	/* set title of main window*/
	gtk_window_set_title(GTK_WINDOW(fxmain->window),
				   	user->nickname );
	gdk_threads_leave();

	gdk_threads_enter();
	fx_login_free(fxlogin);
	fx_head_show(fxmain);
	fx_tree_show(fxmain);
	fx_bottom_show(fxmain);
	gdk_threads_leave();

	/* start sending keep alive request periodically */
	g_timeout_add_seconds(70 , (GSourceFunc)fx_main_register_func , user);
	g_timeout_add_seconds(5 , (GSourceFunc)fx_main_check_func , fxmain);
	g_thread_create(fx_server_func, fxmain, FALSE, NULL);

	g_thread_exit(0);
failed:
	gdk_threads_enter();
	gtk_widget_destroy(fxlogin->fixed1);
	gtk_widget_show(fxlogin->fixed);
	gtk_widget_grab_focus(fxlogin->loginbutton);
	gdk_threads_leave();
	g_thread_exit(0);
	return -1;
}

void fx_conn_offline(FxMain *fxmain)
{
	FxTree           *fxtree;
	GtkTreeView      *treeview;
	GtkTreeModel     *model;
	GtkTreeIter       rootiter;
	GtkTreeIter       childiter;
	struct conn_list *cur;
	struct conn_list *tmp;
	FxList           *cw_cur;
	FxList           *sip_cur;
	FxList           *tmp_cur;
	FxChat           *fxchat;

	/* set state image to be offline */
	fx_head_set_state_image(fxmain, P_OFFLINE);
	
	/* set user state to be offline */
	fetion_user_set_st(fxmain->user, P_OFFLINE);

	/* shutdown all the connections */
	for(cur = connlst->next;cur != connlst;){
		tmp = cur;
		cur = cur->next;
		tcp_connection_close(tmp->conn);
		fx_conn_remove(tmp);
		g_free(tmp);
	}

	/* clear the state of all contacts */
	fxtree = fxmain->mainPanel;
	treeview = GTK_TREE_VIEW(fxtree->treeView);
	model = gtk_tree_view_get_model(treeview);

	if(gtk_tree_model_get_iter_root(model, &rootiter)){
		do{
			/* clear group online numbers */
			gtk_tree_store_set(GTK_TREE_STORE(model),
					&rootiter, G_ONLINE_COUNT_COL, 0, -1);

			if(gtk_tree_model_iter_children(model,
						&childiter, &rootiter))
				do{
					/* clear contacts' online state */
					gtk_tree_store_set(GTK_TREE_STORE(model),
							&childiter, B_STATE_COL, P_HIDDEN, -1);
				}while(gtk_tree_model_iter_next(model, &childiter));

		}while(gtk_tree_model_iter_next(model, &rootiter));
	}

	/* notify all the chat window */
	foreach_list(fxmain->clist, cw_cur){
		fxchat = (FxChat*)cw_cur->data;
		fx_chat_add_information(fxchat,
			_("\nConnection has been shutdown\n"));
	}

	/* clear the sip list */
	for(sip_cur = fxmain->slist->next;
			sip_cur != fxmain->slist;){
		tmp_cur = sip_cur;
		sip_cur = sip_cur->next;
		fx_list_remove(tmp_cur);
		free(tmp_cur);
	}
		
}

int fx_conn_reconnect(FxMain *fxmain, int state)
{
	User             *user;
	Config           *config;
	FetionSip        *sip;
	FetionConnection *tcp;
	ThreadArgs       *args;
	FxCode           *fxcode;
	FxHead           *fxhead;
	GdkPixbuf        *pixbuf;
	gchar            *res_str;
	gchar            *nonce;
	gchar            *key;
	gchar            *response;
	gchar            *aeskey;
	gchar             code[16];
	gchar             path[1024];
	gint              ret;
	gint              group_count;
	gint              buddy_count;

	user   = fxmain->user;
	sip    = user->sip;
	config = user->config;
	fxhead = fxmain->headPanel;

	/* set user state to be not offline */
	fetion_user_set_st(user , state);

	/* create a new connection to the sip server */
	tcp = tcp_connection_new();

	/* add connection to connlist */
	fx_conn_append(tcp);

	debug_info("Connecting to sipc server");
	if(config->proxy && config->proxy->proxyEnabled)
		ret = tcp_connection_connect_with_proxy(tcp,
					   	config->sipcProxyIP,
					   	config->sipcProxyPort,
					   	config->proxy);
	else {
		ret = tcp_connection_connect(tcp,
					  	config->sipcProxyIP, 
						config->sipcProxyPort);
		if(ret == -1 && config->sipcProxyPort != 443) {
			config->sipcProxyPort = 443;
			ret = tcp_connection_connect(tcp,
							config->sipcProxyIP, 
							config->sipcProxyPort);
		}
	}

	if(ret == -1){
		gdk_threads_enter();
		fx_util_popup_warning(fxmain,
				_("Login failed"));
		gdk_threads_leave();
		goto failed2;
	}

	/* set the connection to the sip obj already exist */
	fetion_sip_set_conn(sip, tcp);
	
	/* register to sipc server */
	debug_info("Registering to sipc server");
	res_str = sipc_reg_action(user);
	
	if(!res_str){
		gdk_threads_enter();
		fx_util_popup_warning(fxmain,
				_("Login failed"));
		gdk_threads_leave();
		goto failed2;
	}

	parse_sipc_reg_response(res_str,
			&nonce, &key);
	g_free(res_str);

	/* generate sipc authencation response string */
	aeskey = generate_aes_key();
	if(aeskey == NULL){
		goto failed2;
	}
	response = generate_response(nonce, user->userId,
				   	user->password, key, aeskey);
	g_free(nonce);
	g_free(key);
	g_free(aeskey);

auth:
	/* start sipc authencation */
	debug_info("Starting sipc authencation");
	res_str = sipc_aut_action(user, response);
	
	if(!res_str){
		gdk_threads_enter();
		fx_util_popup_warning(fxmain,
				_("Login failed"));
		gdk_threads_leave();
		goto failed2;
	}

	if(parse_sipc_auth_response(res_str, user,
			&group_count, &buddy_count) < 0){
		gdk_threads_enter();
		fx_util_popup_warning(fxmain,
				_("Authenticate failed."));
		gdk_threads_leave();
		g_free(res_str);
		goto failed2;
	}

	g_free(res_str);

	/* error code 401 or 400,means passowd error */
	if(user->loginStatus == 401 ||
			user->loginStatus == 400){
		gdk_threads_enter();
		fx_util_popup_warning(fxmain,
				_("Authenticate failed."));
		gdk_threads_leave();
		goto failed2;
	}

	/* need verification */
	if(user->loginStatus == 421 ||
			user->loginStatus == 420){
		debug_info(user->verification->text);
		debug_info(user->verification->tips);
		debug_info("Getting code picture，please wait...");
		generate_pic_code(user);
		gdk_threads_enter();
		fxcode = fx_code_new(fxmain,
					   	user->verification->text,
					  	user->verification->tips,
					   	CODE_NOT_ERROR);
		fx_code_initialize(fxcode);

		ret = gtk_dialog_run(GTK_DIALOG(fxcode->dialog));
		if(ret == GTK_RESPONSE_OK){
			strcpy(code , gtk_entry_get_text(
						GTK_ENTRY(fxcode->codeentry)));
			fetion_user_set_verification_code(user , code);
			gtk_widget_destroy(fxcode->dialog);
			gdk_threads_leave();
			goto auth;
		}else{
			gtk_widget_destroy(fxcode->dialog);
			gdk_threads_leave();
			goto failed2;
		}
		debug_info("Input verfication code:%s" , code);
	}

	args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
	args->fxmain = fxmain;
	args->sip = NULL;

	/* start listening thread */
	fetion_contact_subscribe_only(user);
	presence_count = 0;
	g_thread_create(fx_main_listen_thread_func,
			args, FALSE, NULL);

	/* recover the portrait */
	sprintf(path, "%s/%s.jpg",
			config->iconPath, user->sId);
	pixbuf = gdk_pixbuf_new_from_file_at_size(path,
					USER_PORTRAIT_SIZE, 
					USER_PORTRAIT_SIZE, NULL);
	if(!pixbuf)
		pixbuf = gdk_pixbuf_new_from_file_at_size(
					SKIN_DIR"fetion.svg",
					USER_PORTRAIT_SIZE, 
					USER_PORTRAIT_SIZE, NULL);

	gdk_threads_enter();
	gtk_image_set_from_pixbuf(
			GTK_IMAGE(fxhead->portrait), pixbuf);
	gtk_widget_set_sensitive(fxhead->portrait, TRUE);
	gdk_threads_leave();
	g_object_unref(pixbuf);

	/* set state image */
	gdk_threads_enter();
	fx_head_set_state_image(fxmain, state);
	gdk_threads_leave();

	/* start sending keep alive request periodically */
	g_timeout_add_seconds(60 , (GSourceFunc)fx_main_register_func , user);
	g_timeout_add_seconds(3 , (GSourceFunc)fx_main_check_func , fxmain);
	return 1;

failed2:
	fetion_user_set_st(user, P_OFFLINE);
	sprintf(path, "%s/%s.jpg",
			config->iconPath, user->sId);
	pixbuf = gdk_pixbuf_new_from_file_at_size(path,
					USER_PORTRAIT_SIZE, 
					USER_PORTRAIT_SIZE, NULL);
	if(!pixbuf)
		pixbuf = gdk_pixbuf_new_from_file_at_size(
					SKIN_DIR"fetion.svg",
					USER_PORTRAIT_SIZE, 
					USER_PORTRAIT_SIZE, NULL);

	gdk_threads_enter();
	gtk_image_set_from_pixbuf(
			GTK_IMAGE(fxhead->portrait), pixbuf);
	gdk_threads_leave();
	g_object_unref(pixbuf);
	return -1;
}

int fx_conn_offline_login(FxMain *fxmain)
{
	FxLogin          *fxlogin;
	FetionConnection *conn;
	FetionSip        *sip;
	User             *user;
	Config           *config;
	const gchar      *no;
	const gchar      *password;
	gint              local_group_count;
	gint              local_buddy_count;
	struct userlist  *ul_cur;

	fxlogin = fxmain->loginPanel;

	gdk_threads_enter();
	fx_login_hide(fxlogin);
	fx_logining_show(fxmain);
	gdk_threads_leave();

	/* get login number and password */
	no = gtk_combo_box_get_active_text(
					GTK_COMBO_BOX(fxlogin->username));
	password = gtk_entry_get_text(
					GTK_ENTRY(fxlogin->password));

	ul_cur = fetion_user_list_find_by_no(ul, no);
	if(!ul_cur){
		fx_login_show_err(fxlogin,
			_("No local information stored"));
		goto failed1;
	}

	if(strcmp(ul_cur->password, password) != 0){
		fx_login_show_err(fxlogin,
			_("Authenticate failed."));
		goto failed1;
	}

	user = fetion_user_new(no , password);
	fetion_user_set_userid(user, ul_cur->userid);
	fx_main_set_user(fxmain , user);

	fx_login_show_msg(fxlogin , _("Preparing for login"));	

	config = fetion_config_new();
	if(!user){
		fx_login_show_err(fxlogin , _("Login failed"));
		goto failed1;
	}

	/* set the proxy structure to config */
	config->proxy = fxlogin->proxy;
	/* set the config structure to user */
	fetion_user_set_config(user , config);

	if(fetion_user_init_config(user)) {
		fx_login_show_err(fxlogin , _("Login failed"));
		debug_error("initialize config failed");
		goto failed1;
	}
	/* initialize history */
	fx_main_history_init(fxmain);

	fetion_config_load(user);
	if(config->sipcProxyPort == 0)
		goto failed1;

	fetion_user_set_st(user , P_OFFLINE);

	/*load local data*/
	fetion_user_load(user);
	fetion_contact_load(user, &local_group_count, &local_buddy_count);

	/* add the connection object into the connection list */
	conn = tcp_connection_new();
	fx_conn_append(conn);

	/* initialize a sip object */
	sip = fetion_sip_new(conn , user->sId);
	fetion_user_set_sip(user , sip);

	/* initialize head panel */
	gdk_threads_enter();
	fx_head_initialize(fxmain);
	gdk_threads_leave();

	/* initialize main panel which in fact only contains a treeview*/
	gdk_threads_enter();
	fxmain->mainPanel = fx_tree_new();
	fx_tree_initilize(fxmain);
	gdk_threads_leave();

	/* initialize bottom panel */
	gdk_threads_enter();
	fx_bottom_initialize(fxmain);
	gdk_threads_leave();

	gdk_threads_enter();
	/* set title of main window*/
	gtk_window_set_title(GTK_WINDOW(fxmain->window),
				   	user->nickname );
	gdk_threads_leave();

	gdk_threads_enter();
	fx_login_free(fxlogin);
	fx_head_show(fxmain);
	fx_tree_show(fxmain);
	fx_bottom_show(fxmain);
	gdk_threads_leave();


	return 1;
failed1:
	gdk_threads_enter();
	gtk_widget_destroy(fxlogin->fixed1);
	gtk_widget_show(fxlogin->fixed);
	gtk_widget_grab_focus(fxlogin->loginbutton);
	gdk_threads_leave();
	g_thread_exit(0);
	return -1;

}

int fx_conn_check_action(FxMain *fxmain)
{
	if(fxmain->user->state == P_OFFLINE){
		fx_util_popup_warning(fxmain,
				_("The action can't be performed in offline state"));
		return 0;
	}
	return 1;
}
