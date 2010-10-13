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

static void* localization_thread(void *data);
static void fx_login_show_msg(FxLogin *fxlogin , const char *msg);
static void fx_login_show_err(FxLogin *fxlogin , const char *msg);

FxLogin* fx_login_new()
{
	FxLogin* fxlogin = (FxLogin*)malloc(sizeof(FxLogin));

	DEBUG_FOOTPRINT();

	memset(fxlogin , 0 , sizeof(FxLogin));
	return fxlogin;
}
void fx_login_hide(FxLogin *fxlogin)
{
	DEBUG_FOOTPRINT();

	gtk_widget_hide(fxlogin->fixed);
}
void fx_login_free(FxLogin* fxlogin)
{
	DEBUG_FOOTPRINT();

	gtk_widget_destroy(fxlogin->fixed);
	gtk_widget_destroy(fxlogin->fixed1);
	free(fxlogin);
}

gboolean fx_login_proxy_button_func(GtkWidget *UNUSED(widget)
		, GdkEventButton *event , gpointer data)
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
			sprintf(text , _("<span color='#3465a4'><small> Proxy[%s]</small></span>")
					, (proxy == NULL || !proxy->proxyEnabled) ? _("Off") : _("On"));
			gtk_label_set_markup(GTK_LABEL(fxlogin->proxyLabel) , text);
			break;
		case GDK_LEAVE_NOTIFY :
			sprintf(text , _("<span color='#204a87'><small> Proxy[%s]</small></span>")
					, (proxy == NULL || !proxy->proxyEnabled) ? _("Off") : _("On"));
			gtk_label_set_markup(GTK_LABEL(fxlogin->proxyLabel) , text);
			break;
		case GDK_BUTTON_PRESS :
			fxproxy = fx_proxy_new(fxlogin);
			fx_proxy_initialize(fxproxy);
			gtk_dialog_run(GTK_DIALOG(fxproxy->dialog));
			gtk_widget_destroy(fxproxy->dialog);
			break;
		default:
			break;
	}
	return TRUE;
}

void fx_logining_show(FxMain *fxmain)
{
	GdkPixbuf *pixbuf;
	GtkWidget *mainbox;
	GtkWidget *frame;
	GtkWidget *label;
	FxLogin *fxlogin;

	mainbox = fxmain->mainbox;
	fxlogin = fxmain->loginPanel;
	fxlogin->fixed1 = gtk_fixed_new();
	gtk_box_pack_start(GTK_BOX(mainbox),
					fxlogin->fixed1, TRUE, TRUE, 0);
	frame = gtk_frame_new(NULL);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed1),
				   	frame, (WINDOW_WIDTH - 128)/2, 70);
	pixbuf = gdk_pixbuf_new_from_file_at_size(
					SKIN_DIR"online.svg", 128, 128, NULL);
	fxlogin->image = gtk_image_new_from_file(SKIN_DIR"logining.gif");
	g_object_unref(pixbuf);
	gtk_container_add(GTK_CONTAINER(frame), fxlogin->image);
	fxlogin->label = gtk_label_new(NULL);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed1),
					fxlogin->label, 0, 300);
	gtk_widget_set_usize(fxlogin->label,
					WINDOW_WIDTH, 0);
	gtk_label_set_justify(GTK_LABEL(fxlogin->label),
					GTK_JUSTIFY_CENTER);

	label = gtk_label_new(NULL);
	gtk_widget_set_usize(label,
					WINDOW_WIDTH, 0);
	gtk_label_set_justify(GTK_LABEL(label),
					GTK_JUSTIFY_CENTER);
	gtk_label_set_markup(GTK_LABEL(label),
			"<b>Welcome to OpenFetion</b>");
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed1),
					label, 0, 350);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label),
			"<small>OpenFetion a fetion client for linux"
			" based on GTK+2.0, using Fetion Protocol"
			" Version 4.\nOpenFetion is a non-profit software,"
			" aiming at making linux users convenient "
			"to use fetion.</small>");
	gtk_widget_set_usize(label, WINDOW_WIDTH - 40, 0);
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed1),
					label, 20, 390);

	gtk_widget_show_all(fxlogin->fixed1);
}

void fx_login_initialize(FxMain *fxmain)
{
	FxLogin* fxlogin = fxmain->loginPanel;
	GtkTreeModel* stateModel = NULL;
	GtkCellRenderer* renderer = NULL;
	GtkWidget* img = NULL;
	GtkWidget* noentry = NULL;
	Config* config = NULL;
	GtkTreeModel* model = NULL;
	GtkWidget *proxyHbox = NULL;
	Proxy *proxy = NULL;
	char text[1024];

	DEBUG_FOOTPRINT();

	config = fetion_config_new();
	/* load proxy information */
	proxy = fetion_config_load_proxy();
	fxlogin->proxy = proxy;
	
	model = fx_login_create_user_model(config);
	fxlogin->username = gtk_combo_box_entry_new_with_model(model , 0);
	noentry = gtk_bin_get_child(GTK_BIN(fxlogin->username));
	gtk_widget_set_size_request(GTK_WIDGET(fxlogin->username) , 200 , 25);

	g_signal_connect(fxlogin->username, "changed",
				   	G_CALLBACK(fx_login_user_change_func) , fxlogin);

	fxlogin->userlabel = gtk_label_new(gettext("Cell number or fetion number:"));
	gtk_label_set_justify(GTK_LABEL(fxlogin->userlabel),
				   	GTK_JUSTIFY_CENTER);

	fxlogin->password = gtk_entry_new();
	gtk_widget_set_size_request(GTK_WIDGET(fxlogin->password) , 200 , 25);
	gtk_entry_set_visibility(GTK_ENTRY(fxlogin->password) , FALSE);
	g_signal_connect(G_OBJECT(fxlogin->password)
					 , "activate"
					 , G_CALLBACK(fx_login_action_func)
					 , fxmain);

	fxlogin->passlabel = gtk_label_new(_("Please input password:"));
	gtk_label_set_justify(GTK_LABEL(fxlogin->passlabel) , GTK_JUSTIFY_CENTER);

	fxlogin->errlabel = gtk_label_new(NULL);
	gtk_widget_set_size_request(GTK_WIDGET(fxlogin->errlabel) , WINDOW_WIDTH - 10 , 25);
	gtk_label_set_justify(GTK_LABEL(fxlogin->errlabel) , GTK_JUSTIFY_CENTER);

	fxlogin->loginbutton = gtk_button_new_with_label(_("Login"));

	img = gtk_image_new_from_file(SKIN_DIR"login.png");
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

	fxlogin->remember = gtk_check_button_new_with_label(_("Remember password"));
	fxlogin->proxyBtn = gtk_event_box_new();

	fxlogin->proxyLabel = gtk_label_new(NULL);
	proxyHbox = gtk_hbox_new(FALSE , FALSE);
	img = gtk_image_new_from_file(SKIN_DIR"proxy.png");
	bzero(text , sizeof(text));
	sprintf(text , _("<span color='#204a87'><small> Proxy[%s]</small></span>")
			, (fxlogin->proxy == NULL || ! fxlogin->proxy->proxyEnabled) ? _("Off")  : _("On"));

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

	img = gtk_image_new_from_pixbuf(
		gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.svg" , 128 , 128 , NULL));

	fxlogin->fixed = gtk_fixed_new();
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , img , 70, 25);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->userlabel , 20 ,165);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->username , (WINDOW_WIDTH - 200)/2 , 185);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->passlabel , 20 , 215);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->password , (WINDOW_WIDTH - 200)/2 , 235);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->statecombo , (WINDOW_WIDTH - 120)/2 , 265);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->remember , (WINDOW_WIDTH - 80)/2 , 305);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->errlabel , 5 , 325);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->loginbutton , (WINDOW_WIDTH - 80)/2 , 355);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->proxyBtn , (WINDOW_WIDTH - 100) / 2 , 405);
	gtk_box_pack_start(GTK_BOX(fxmain->mainbox) , fxlogin->fixed , TRUE , TRUE , 0);

	GTK_WIDGET_SET_FLAGS(fxlogin->loginbutton, GTK_CAN_FOCUS);
	gtk_widget_grab_focus(fxlogin->loginbutton);

	gtk_widget_show_all(fxmain->mainbox);

}
GtkTreeModel* fx_login_create_state_model()
{
	GtkListStore* store = NULL;
	GtkTreeIter iter;
	GdkPixbuf* pb = NULL;
	int i;

	DEBUG_FOOTPRINT();

	struct {
		const gchar* name;
		const gchar* icon;
		int type;
	} presence[] = {
		{ N_("Online")	 , SKIN_DIR"online.svg" , P_ONLINE } , 
		{ N_("Leave")	 , SKIN_DIR"away.svg" , P_AWAY } , 
		{ N_("Busy")	 , SKIN_DIR"busy.svg" , P_BUSY } ,
		{ N_("Hide")	 , SKIN_DIR"invisible.svg" , P_HIDDEN } , 
		{ N_("Eating out") , SKIN_DIR"away.svg" , P_OUTFORLUNCH } ,
		{ N_("Do Not Disturb") , SKIN_DIR"away.svg" , P_DONOTDISTURB } , 
		{ N_("Back Soon") , SKIN_DIR"away.svg" , P_RIGHTBACK } , 
		{ N_("Meeting")	 , SKIN_DIR"away.svg" , P_MEETING } , 
		{ N_("Calling")	 , SKIN_DIR"away.svg" , P_ONTHEPHONE} ,
		{ NULL		 , NULL 			   , -1}
	};
	enum {
		PIXBUF_COL , 
		TEXT_COL , 
		INT_COL
	};
	store = gtk_list_store_new(3, GDK_TYPE_PIXBUF,
				   	G_TYPE_STRING , G_TYPE_INT);

	for(i = 0 ; presence[i].type != -1 ; i++){
		gtk_list_store_append(store , &iter);
		pb = gdk_pixbuf_new_from_file_at_size(presence[i].icon,
				20, 20, NULL);
		gtk_list_store_set(store , &iter
				, PIXBUF_COL , pb 
				, TEXT_COL , _(presence[i].name)
				, INT_COL , presence[i].type , -1);
		g_object_unref(pb);
	}
	return GTK_TREE_MODEL(store);

}

static void fx_login_show_msg(FxLogin *fxlogin , const char *msg)
{
	gdk_threads_enter();
	gtk_label_set_text(GTK_LABEL(fxlogin->label) , msg);	
	update();
	gdk_threads_leave();
}

static void fx_login_show_err(FxLogin *fxlogin , const char *msg)
{
	gdk_threads_enter();
	gtk_label_set_text(GTK_LABEL(fxlogin->errlabel) , msg);	
	update();
	gdk_threads_leave();
}
void* fx_login_thread_func(void* data)
{
	FxMain* fxmain = (FxMain*)data;
	FxLogin* fxlogin = fxmain->loginPanel;
	FetionConnection* conn = NULL;					 /* connection for sipc 		   */
	const char *no , *password;
	char *pos , *nonce , *key , *aeskey , *response; /* string used for authentication */
	Config* config = NULL;							 /* global user config 			   */
	Group* group = NULL;							 /* buddy list		  			   */
	User* user = NULL;								 /* global user information 	   */
	char code[20];									 /* store reply code   			   */
	char statusTooltip[128];
	struct userlist *ul , *newul , *ul_cur;

	GtkTreeIter stateIter;
	GtkTreeModel* stateModel = NULL;
	int state;
	gboolean remember;

	FxCode* fxcode = NULL;
	int ret;

	/* get login state value */
	gtk_combo_box_get_active_iter(
					GTK_COMBO_BOX(fxlogin->statecombo),
				   	&stateIter);
	stateModel = gtk_combo_box_get_model(
					GTK_COMBO_BOX(fxlogin->statecombo));
	gtk_tree_model_get(stateModel, &stateIter,
				   	2 , &state , -1);

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
	free(pos);
	if(user->loginStatus == 421 || user->loginStatus == 420){
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
			free(fxcode);
			gdk_threads_leave();
		}else{
			gtk_widget_destroy(fxcode->dialog);
			free(fxcode);
			gdk_threads_leave();
			g_thread_exit(0);
		}
		debug_info("Input verfication code:%s" , code);
		goto login;
	}
	if(user->loginStatus == 401 ||
	  	user->loginStatus == 400 ||
	   	user->loginStatus == 404){
		debug_info("password ERROR!!!");
		fx_login_show_err(fxlogin,
			_("Login failed. Incorrect cell phone number or password"));
		goto failed;
	}
	
	fx_login_show_err(fxlogin,
				 _("Loading local user information"));

	fetion_config_initialize(config , user->userId);
	/* initialize history */
	fx_main_history_init(fxmain);

	/* set user list to be stored in local file	 */
	ul = fetion_user_list_load(config);
	newul = fetion_user_list_find_by_no(ul , no);
	if(!newul){
		if(remember)
			newul = fetion_user_list_new(no,
						  password , state , 1);
		else
			newul = fetion_user_list_new(no,
						  NULL , state , 1);
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
					_("It detected that this`s"
					" this first time you login\n"
					"downloading configuration file..."));
	else
		fx_login_show_msg(fxlogin,
					_("Downloading configuration files"));
	fetion_config_download_configuration(user);
	fetion_config_save(user);

	fetion_user_set_st(user , state);

	/*load local data*/
	fetion_user_load(user);
	fetion_contact_load(user);

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
		tcp_connection_connect(conn,
					  	config->sipcProxyIP, 
						config->sipcProxyPort);
	}

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
	free(pos);

	aeskey = generate_aes_key();
	response = generate_response(nonce, user->userId,
				   	user->password, key, aeskey);
	free(nonce);
	free(key);
	free(aeskey);

	/* start sipc authentication using the response created just now */
	fx_login_show_msg(fxlogin , _("SIPC Indentify"));
auth:
	pos = sipc_aut_action(user , response);
	if(pos == NULL){
		fx_login_show_err(fxlogin , _("Login failed"));
		goto failed;
	}

	if(parse_sipc_auth_response(pos , user) < 0){
		debug_info("Password error , login failed!!!");
		fx_login_show_err(fxlogin , _("Authenticate failed."));
		goto failed;
	}
	free(pos); pos = NULL;

	if(user->loginStatus == 401 || user->loginStatus == 400){
		debug_info("Password error , login failed!!!");
		fx_login_show_err(fxlogin , _("Authenticate failed."));
		goto failed;
	}

	if(user->loginStatus == 421 || user->loginStatus == 420){
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
	fx_login_show_msg(fxlogin , _("Initializing main panel"));
	
	pg_group_get_list(user);

#ifdef USE_LIBNOTIFY
	char notifyText[1024];
	char iconPath[256];
	GdkPixbuf *pb;
	sprintf(iconPath, "%s/%s.jpg",
				  config->iconPath, user->sId);
	sprintf(notifyText ,
			_("Public IP: %s\n"
			"IP of last login: %s\n"
			"Time of last login: %s\n"),
		   	user->publicIp , user->lastLoginIp,
		   	user->lastLoginTime);
	pb = gdk_pixbuf_new_from_file_at_size(iconPath,
				   	48 , 48 , NULL);
	if(!pb){
		fx_login_show_msg(fxlogin, _("Getting portrait..."));
		fetion_user_download_portrait(user , user->sipuri);
		pb = gdk_pixbuf_new_from_file_at_size(iconPath,
					   	48 , 48 , NULL);
		if(!pb)
			pb = gdk_pixbuf_new_from_file_at_size(
						SKIN_DIR"fetion.svg",
						48, 48, NULL);
	}
	gdk_threads_enter();
	notify_notification_update(fxmain->notify,
				   	_("Login successful")// notifySummary
			, notifyText , NULL);
	notify_notification_set_icon_from_pixbuf(fxmain->notify , pb);
	notify_notification_show(fxmain->notify , NULL);
	gdk_threads_leave();
	g_object_unref(pb);
#endif
	fx_login_show_msg(fxlogin , _("Login sucessful"));

	gdk_threads_enter();
	gtk_window_set_resizable(GTK_WINDOW(fxmain->window) , TRUE);
	gdk_threads_leave();

	/**
	 *  if there is not a buddylist name "Ungrouped" or "Strangers", create one
	 */
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

	/* set tooltip of status icon */
	sprintf(statusTooltip, "%s\n%s",
			 user->nickname , user->mobileno);

	gdk_threads_enter();
	gtk_status_icon_set_tooltip(GTK_STATUS_ICON(fxmain->trayIcon),
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
	g_timeout_add_seconds(180 , (GSourceFunc)fx_main_register_func , user);
	g_timeout_add_seconds(10 , (GSourceFunc)fx_main_check_func , fxmain);

	/*localization*/
	g_thread_create(localization_thread, user, FALSE, NULL);

	g_thread_exit(0);
failed:
	gdk_threads_enter();
	gtk_widget_destroy(fxlogin->fixed1);
	gtk_widget_show(fxlogin->fixed);
	gtk_widget_grab_focus(fxlogin->loginbutton);
	gdk_threads_leave();
	g_thread_exit(0);
	return NULL;
}
void fx_login_action_func(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxMain* fxmain = (FxMain*)data;
	g_thread_create(fx_login_thread_func,
			fxmain , FALSE , NULL);
}
GtkTreeModel* fx_login_create_user_model(Config* config)
{
	GtkTreeStore* model = gtk_tree_store_new(USER_COLS_NUM
										   , G_TYPE_STRING
										   , G_TYPE_STRING
										   , G_TYPE_INT
										   , G_TYPE_INT);
	struct userlist *ul , *ul_cur;
	GtkTreeIter iter;
	
	DEBUG_FOOTPRINT();
	
	ul = fetion_user_list_load(config);

	foreach_userlist(ul , ul_cur){
		gtk_tree_store_append(model , &iter , NULL);
		gtk_tree_store_set(model    , &iter 
						 , L_NO_COL   , ul_cur->no
						 , L_PWD_COL  , ul_cur->password
						 , L_STATE_COL, ul_cur->laststate
						 , L_LAST_COL , ul_cur->islastuser
						 , -1);
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
	do{
		gtk_tree_model_get(model    , &iter
						 , L_NO_COL   , &no
						 , L_PWD_COL  , &pwd
						 , L_STATE_COL, &state
						 , L_LAST_COL , &last
						 , -1);
		if(last == 1){
			gtk_combo_box_set_active_iter(combo , &iter);
			gtk_entry_set_text(GTK_ENTRY(fxlogin->password) , pwd);
			fx_login_set_last_login_state(fxlogin , state);	
			if(strlen(pwd) != 0)
				gtk_toggle_button_set_active(
						GTK_TOGGLE_BUTTON(fxlogin->remember) , TRUE);
			free(no);
			free(pwd);
			break;
		}
	}
	while(gtk_tree_model_iter_next(model , &iter));
}
void fx_login_set_last_login_state(FxLogin* fxlogin , StateType state)
{
	GtkTreeModel* model = gtk_combo_box_get_model(
			GTK_COMBO_BOX(fxlogin->statecombo));
	GtkTreeIter iter;
	int s;

	DEBUG_FOOTPRINT();

	gtk_tree_model_get_iter_root(model , &iter);
	do{
		gtk_tree_model_get(model , &iter , 2 , &s , -1);
		if(s == state)
			gtk_combo_box_set_active_iter(
					GTK_COMBO_BOX(fxlogin->statecombo) , &iter);
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

	free(pwd);
}

static void* localization_thread(void *data)
{
	fetion_user_save((User*)data);
	fetion_contact_save((User*)data);
	return NULL;
}
