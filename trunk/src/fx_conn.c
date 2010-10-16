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

extern gint presence_count;

void fx_conn_init(void)
{
	connlst = (struct conn_list*)malloc(sizeof(struct conn_list));
	connlst->conn = NULL;
	connlst->next = connlst->pre = connlst;
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

void fx_conn_offline(FxMain *fxmain)
{
	FxHead           *fxhead;
	FxTree           *fxtree;
	User             *user;
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

	fxhead = fxmain->headPanel;
	user = fxmain->user;

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
		free(sip_cur->data);
		free(sip_cur);
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
		tcp_connection_connect_with_proxy(tcp,
					   	config->sipcProxyIP,
					   	config->sipcProxyPort,
					   	config->proxy);
	else
		tcp_connection_connect(tcp,
					  	config->sipcProxyIP, 
						config->sipcProxyPort);

	/* set the connection to the sip obj already exist */
	fetion_sip_set_conn(sip, tcp);
	
auth:
	/* register to sipc server */
	debug_info("Registering to sipc server");
	res_str = sipc_reg_action(user);
	
	if(!res_str){
		fx_util_popup_warning(fxmain,
				_("Login failed"));
		return -1;
	}

	parse_sipc_reg_response(res_str,
			&nonce, &key);
	g_free(res_str);

	/* generate sipc authencation response string */
	aeskey = generate_aes_key();
	response = generate_response(nonce, user->userId,
				   	user->password, key, aeskey);
	g_free(nonce);
	g_free(key);
	g_free(aeskey);

	/* start sipc authencation */
	debug_info("Starting sipc authencation");
	res_str = sipc_aut_action(user, response);
	
	if(!res_str){
		fx_util_popup_warning(fxmain,
				_("Login failed"));
		return -1;
	}

	if(parse_sipc_auth_response(res_str, user,
			&group_count, &buddy_count) < 0){
		fx_util_popup_warning(fxmain,
				_("Authenticate failed."));
		g_free(res_str);
		return -1;
	}

	g_free(res_str);

	/* error code 401 or 400,means passowd error */
	if(user->loginStatus == 401 ||
			user->loginStatus == 400){
		fx_util_popup_warning(fxmain,
				_("Authenticate failed."));
		return -1;
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
			return -1;
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

	return 1;
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

	fxlogin = fxmain->loginPanel;

	/* get login number and password */
	no = gtk_combo_box_get_active_text(
					GTK_COMBO_BOX(fxlogin->username));
	password = gtk_entry_get_text(
					GTK_ENTRY(fxlogin->password));

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
		return -1;
	}

	/* set the proxy structure to config */
	config->proxy = fxlogin->proxy;
	/* set the config structure to user */
	fetion_user_set_config(user , config);

	fetion_config_initialize(config , user->userId);
	/* initialize history */
	fx_main_history_init(fxmain);

	fetion_config_load(user);
	if(config->sipcProxyPort == 0)
		return -1;

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


	g_thread_exit(0);
failed:
	gdk_threads_enter();
	gtk_widget_destroy(fxlogin->fixed1);
	gtk_widget_show(fxlogin->fixed);
	gtk_widget_grab_focus(fxlogin->loginbutton);
	gdk_threads_leave();
	g_thread_exit(0);
	return 1;

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
