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

#include "fx_include.h"

struct userlist  *ul;

#define PORTRAIT_SIZE      120
#define LOADING_IMAGE_SIZE 128

static void userlist_remove_clicked(GtkWidget *widget, gpointer data);

FxLogin* fx_login_new()
{
	FxLogin* fxlogin = (FxLogin*)malloc(sizeof(FxLogin));

	memset(fxlogin , 0 , sizeof(FxLogin));
	return fxlogin;
}

void fx_login_hide(FxLogin *fxlogin)
{
	gtk_widget_hide(fxlogin->fixed);
}

void fx_login_free(FxLogin* fxlogin)
{
	gtk_widget_destroy(fxlogin->fixed);
	gtk_widget_destroy(fxlogin->fixed1);
	free(fxlogin);
}

gboolean fx_login_proxy_button_func(GtkWidget *UNUSED(widget)
		, GdkEventButton *event , gpointer data)
{
	FxLogin *fxlogin = (FxLogin*)data;
	Proxy   *proxy = fxlogin->proxy;
	FxProxy *fxproxy = NULL;
	gchar    text[1024];

	memset(text, 0, sizeof(text));

	switch(event->type)
	{
		case GDK_ENTER_NOTIFY :
			snprintf(text, sizeof(text) - 1 , _("<span color='#3465a4'><small> Proxy[%s]</small></span>")
					, (proxy == NULL || !proxy->proxyEnabled) ? _("Off") : _("On"));
			gtk_label_set_markup(GTK_LABEL(fxlogin->proxyLabel) , text);
			break;
		case GDK_LEAVE_NOTIFY :
			snprintf(text, sizeof(text) - 1, _("<span color='#204a87'><small> Proxy[%s]</small></span>")
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
	GtkWidget *label;
	FxLogin *fxlogin;

	mainbox = fxmain->mainbox;
	fxlogin = fxmain->loginPanel;
	fxlogin->fixed1 = gtk_fixed_new();
	gtk_box_pack_start(GTK_BOX(mainbox),
					fxlogin->fixed1, TRUE, TRUE, 0);
	pixbuf = gdk_pixbuf_new_from_file_at_size(
					SKIN_DIR"online.svg",
					LOADING_IMAGE_SIZE,
				   	LOADING_IMAGE_SIZE, NULL);
	fxlogin->image = gtk_image_new_from_file(SKIN_DIR"logining.gif");
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed1),
				   	fxlogin->image, (WINDOW_WIDTH - 128)/2, 70);
	g_object_unref(pixbuf);
	fxlogin->label = gtk_label_new(NULL);
//	gtk_label_set_line_wrap(GTK_LABEL(fxlogin->label), TRUE);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed1),
					fxlogin->label, 0, 280);
	gtk_widget_set_usize(fxlogin->label,
					WINDOW_WIDTH, 0);
	gtk_label_set_justify(GTK_LABEL(fxlogin->label), GTK_JUSTIFY_CENTER);

	label = gtk_label_new(NULL);
	gtk_widget_set_usize(label,
					WINDOW_WIDTH, 0);
	gtk_label_set_justify(GTK_LABEL(label),
					GTK_JUSTIFY_CENTER);
	gtk_label_set_markup(GTK_LABEL(label),
			_("<b>Welcome to OpenFetion</b>"));
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed1),
					label, 0, 350);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label),
			_("<small>OpenFetion a fetion client for linux"
			" based on GTK+2.0, using Fetion Protocol"
			" Version 4.\nOpenFetion is a non-profit software,"
			" aiming at making linux users convenient "
			"to use fetion.</small>"));
	gtk_widget_set_usize(label, WINDOW_WIDTH - 40, 0);
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed1),
					label, 20, 390);

	gtk_widget_show_all(fxlogin->fixed1);
}

void fx_login_initialize(FxMain *fxmain)
{
	FxLogin          *fxlogin = fxmain->loginPanel;
	GtkTreeModel     *stateModel = NULL;
	GtkCellRenderer  *renderer = NULL;
	GtkWidget        *img = NULL;
	GtkWidget        *frame;
	Config           *config = NULL;
	GtkTreeModel     *model = NULL;
	GtkWidget        *proxyHbox = NULL;
	GtkWidget        *rmBtn = NULL;
	GdkPixbuf        *pixbuf;
	Proxy            *proxy = NULL;
	GtkWidget        *image = NULL;
	GtkWidget        *openfetion;
	const gchar      *no;
	const gchar      *psd;
	gchar             text[1024];

	config = fetion_config_new();
	/* load proxy information */
	proxy = fetion_config_load_proxy();
	fxlogin->proxy = proxy;
	
	ul = fetion_user_list_load(config);
	model = fx_login_create_user_model();

	pixbuf = gdk_pixbuf_new_from_file_at_size(
					SKIN_DIR"fetion.svg",
				    PORTRAIT_SIZE,
					PORTRAIT_SIZE, NULL);
	fxlogin->portrait = gtk_image_new_from_pixbuf(pixbuf);
	frame = gtk_frame_new(NULL);
	gtk_widget_set_usize(frame,
				 PORTRAIT_SIZE + 5,
				 PORTRAIT_SIZE + 5);
	gtk_container_add(GTK_CONTAINER(frame), fxlogin->portrait);
	g_object_unref(pixbuf);


	fxlogin->username = gtk_combo_box_entry_new_with_model(model , 0);
	gtk_widget_set_size_request(GTK_WIDGET(fxlogin->username) , 207 , 25);

	g_signal_connect(fxlogin->username, "changed",
				   	G_CALLBACK(fx_login_user_change_func) , fxmain);
	rmBtn = gtk_button_new();
	gtk_widget_set_usize(rmBtn, 30, 25);
	image = gtk_image_new_from_stock(GTK_STOCK_DELETE, 
					GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_button_set_image(GTK_BUTTON(rmBtn), image);
	g_signal_connect(rmBtn, "clicked",
				   	G_CALLBACK(userlist_remove_clicked), fxmain);

	fxlogin->userlabel = gtk_label_new(gettext("Cell number or fetion number:"));
	gtk_label_set_justify(GTK_LABEL(fxlogin->userlabel),
				   	GTK_JUSTIFY_CENTER);

	fxlogin->password = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(fxlogin->password), 16);
	gtk_widget_set_size_request(GTK_WIDGET(fxlogin->password) , 240 , 25);
	gtk_entry_set_visibility(GTK_ENTRY(fxlogin->password) , FALSE);
	g_signal_connect(G_OBJECT(fxlogin->password)
					 , "activate"
					 , G_CALLBACK(fx_login_action_func)
					 , fxmain);

	fxlogin->passlabel = gtk_label_new(_("Please input password:"));
	gtk_label_set_justify(GTK_LABEL(fxlogin->passlabel) , GTK_JUSTIFY_CENTER);

	fxlogin->errlabel = gtk_label_new(NULL);
	gtk_widget_set_usize(GTK_WIDGET(fxlogin->errlabel) , WINDOW_WIDTH - 10 , 0);
	gtk_label_set_justify(GTK_LABEL(fxlogin->errlabel) , GTK_JUSTIFY_CENTER);

	fxlogin->statusLabel = gtk_label_new(_("Status:"));
	fxlogin->loginbutton = gtk_button_new_with_label(_("Login"));

	img = gtk_image_new_from_file(SKIN_DIR"login.png");
	gtk_button_set_image(GTK_BUTTON(fxlogin->loginbutton) , img);

	fxlogin->loginFuncId = g_signal_connect(G_OBJECT(fxlogin->loginbutton)
						   , "clicked"
						   , G_CALLBACK(fx_login_action_func)
						   , fxmain);
	gtk_widget_set_size_request(GTK_WIDGET(fxlogin->loginbutton) , 80 , 25);

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

	gtk_widget_set_usize(GTK_WIDGET(fxlogin->statecombo) , 240 , 28);

	fxlogin->remember = gtk_check_button_new_with_label(_("Remember password"));
	fxlogin->proxyBtn = gtk_event_box_new();

	fxlogin->proxyLabel = gtk_label_new(NULL);
	proxyHbox = gtk_hbox_new(FALSE , FALSE);
	img = gtk_image_new_from_file(SKIN_DIR"proxy.png");
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

	openfetion = gtk_image_new_from_file(SKIN_DIR"openfetion.png");

	fx_login_set_last_login_user(fxlogin);

	fxlogin->fixed = gtk_fixed_new();
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , frame,
				   	(WINDOW_WIDTH - PORTRAIT_SIZE - 10) / 2, 25);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , openfetion,
				   	(WINDOW_WIDTH - 190) / 2, 170);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->userlabel , 20 ,225);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->username , 20, 245);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , rmBtn , 230 , 245);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->passlabel , 20 , 275);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->password , 20, 295);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->statusLabel , 20, 325);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->statecombo , 20, 345);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->remember , 20, 387);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->loginbutton , 180, 385);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->errlabel , 5 , 415);
	gtk_fixed_put(GTK_FIXED(fxlogin->fixed) , fxlogin->proxyBtn , WINDOW_WIDTH - 100 , WINDOW_HEIGHT - 30);
	gtk_box_pack_start(GTK_BOX(fxmain->mainbox) , fxlogin->fixed , TRUE , TRUE , 0);


	GtkWidget *entry = gtk_bin_get_child(GTK_BIN(fxlogin->username));
	GTK_WIDGET_SET_FLAGS(entry, GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS(fxlogin->password, GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS(fxlogin->loginbutton, GTK_CAN_FOCUS);
	/* get login number and password */
	no = gtk_combo_box_get_active_text(
					GTK_COMBO_BOX(fxlogin->username));
	psd = gtk_entry_get_text(
					GTK_ENTRY(fxlogin->password));
	if(!no || *no == '\0')
		gtk_widget_grab_focus(fxlogin->username);
	else if(!psd || *psd == '\0')
		gtk_widget_grab_focus(fxlogin->password);
	else
		gtk_widget_grab_focus(fxlogin->loginbutton);

	gtk_widget_show_all(fxmain->mainbox);

	g_free(config);
}
GtkTreeModel* fx_login_create_state_model()
{
	GtkListStore *store = NULL;
	GtkTreeIter   iter;
	GdkPixbuf    *pb = NULL;
	gint i;

	struct {
		const gchar* name;
		const gchar* icon;
		int type;
	} presence[] = {
		{ N_("Online")	 , SKIN_DIR"online.svg" , P_ONLINE } , 
		{ N_("Leave")	 , SKIN_DIR"away.svg" , P_AWAY } , 
		{ N_("Busy")	 , SKIN_DIR"busy.svg" , P_BUSY } ,
		{ N_("Hide")	 , SKIN_DIR"invisible.svg" , P_HIDDEN } , 
		{ N_("Offline")	 , SKIN_DIR"offline.svg" , P_OFFLINE } , 
		{ N_("Eating out") , SKIN_DIR"away.svg" , P_OUTFORLUNCH } ,
		{ N_("Do Not Disturb") , SKIN_DIR"away.svg" , P_DONOTDISTURB } , 
		{ N_("Back Soon") , SKIN_DIR"away.svg" , P_RIGHTBACK } , 
		{ N_("Meeting")	 , SKIN_DIR"away.svg" , P_MEETING } , 
		{ N_("Calling")	 , SKIN_DIR"away.svg" , P_ONTHEPHONE} ,
		{ NULL		 , NULL 			   , -2}
	};
	enum {
		PIXBUF_COL , 
		TEXT_COL , 
		INT_COL
	};
	store = gtk_list_store_new(3, GDK_TYPE_PIXBUF,
				   	G_TYPE_STRING , G_TYPE_INT);

	for(i = 0 ; presence[i].type != -2 ; i++){
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

void fx_login_show_msg(FxLogin *fxlogin , const char *msg)
{
	char text[1024];
	gdk_threads_enter();
	sprintf(text, "<span font-size='large'><b>%s</b></span>", msg);
	gtk_label_set_markup(GTK_LABEL(fxlogin->label) , text);	
	update();
	gdk_threads_leave();
}

void fx_login_show_err(FxLogin *fxlogin , const char *msg)
{
	gdk_threads_enter();
	gtk_label_set_text(GTK_LABEL(fxlogin->errlabel) , msg);	
	update();
	gdk_threads_leave();
}

static void *fx_offline_login_thread_func(void *data)
{
	fx_conn_offline_login((FxMain*)data);
	return NULL;
}

static void *fx_login_thread_func(void *data)
{
	fx_conn_connect((FxMain*)data);
	return NULL;
}

void fx_login_action_func(GtkWidget* UNUSED(widget) , gpointer data)
{
	FxMain       *fxmain = (FxMain*)data;
	FxLogin      *fxlogin = fxmain->loginPanel;
	GtkTreeIter   stateIter;
	GtkTreeModel *stateModel;
	gint          state;

	/* get login state value */
	gtk_combo_box_get_active_iter(
					GTK_COMBO_BOX(fxlogin->statecombo),
				   	&stateIter);
	stateModel = gtk_combo_box_get_model(
					GTK_COMBO_BOX(fxlogin->statecombo));
	gtk_tree_model_get(stateModel, &stateIter,
				   	2 , &state , -1);
	if(state == P_OFFLINE)
		g_thread_create(fx_offline_login_thread_func,
				fxmain, FALSE, NULL);
	else
		g_thread_create(fx_login_thread_func,
				fxmain , FALSE , NULL);
}

GtkTreeModel* fx_login_create_user_model()
{
	GtkTreeStore* model = gtk_tree_store_new(USER_COLS_NUM
										   , G_TYPE_STRING
										   , G_TYPE_STRING
										   , G_TYPE_INT
										   , G_TYPE_INT);
	struct userlist *ul_cur;
	GtkTreeIter      iter;
	
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
	GtkComboBox   *combo = GTK_COMBO_BOX(fxlogin->username);
	GtkTreeModel  *model = gtk_combo_box_get_model(combo);
	GtkTreeIter    iter;
	gchar         *no;
	gchar         *pwd;
	gint           state;
	gint           last;

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
			if(*pwd != '\0')
				gtk_toggle_button_set_active(
						GTK_TOGGLE_BUTTON(fxlogin->remember) , TRUE);
			g_free(no);
			g_free(pwd);
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
	FxMain        *fxmain = (FxMain*)data;
	FxLogin       *fxlogin = fxmain->loginPanel;
	GtkComboBox   *combo = GTK_COMBO_BOX(widget);
	GtkTreeModel  *model = gtk_combo_box_get_model(combo);
	GdkPixbuf     *pixbuf;
	Config        *config;
	gchar         *pwd;
	gchar         *no;
	gchar          path[1024];
	GtkTreeIter    iter;
	gint           state;
	struct userlist *ul_cur;


	if(!gtk_combo_box_get_active_iter(combo , &iter))
		return;
	gtk_tree_model_get(model, &iter,
				 L_PWD_COL, &pwd,
				 L_NO_COL, &no,
				 L_STATE_COL, &state,  -1);
	ul_cur = fetion_user_list_find_by_no(ul, no);
	config = fetion_config_new();
	fetion_config_initialize(config, ul_cur->userid);
	sprintf(path, "%s/%s.jpg",
			config->iconPath, ul_cur->sid);

	pixbuf = gdk_pixbuf_new_from_file_at_size(path,
					120, 120, NULL);
	if(pixbuf){
		gtk_image_set_from_pixbuf(
				GTK_IMAGE(fxlogin->portrait), pixbuf);
		g_object_unref(pixbuf);
	}
	gtk_entry_set_text(GTK_ENTRY(fxlogin->password) , pwd);
	fx_login_set_last_login_state(fxlogin , state);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxlogin->remember)
							   , *pwd == '\0' ? FALSE : TRUE);

	g_free(pwd);
	g_free(no);
}

static void userlist_remove_clicked(GtkWidget *UNUSED(widget), gpointer data)
{
	FxMain          *fxmain = (FxMain*)data;
	FxLogin         *fxlogin = fxmain->loginPanel;
	GtkComboBox     *combo;
	GtkTreeModel    *model;
	GtkWidget       *entry;
	GdkPixbuf       *pixbuf;
	GtkTreeIter      iter;
	gchar           *no;
	Config          *config;


	combo = GTK_COMBO_BOX(fxlogin->username);
	model = gtk_combo_box_get_model(combo);

	if(!gtk_combo_box_get_active_iter(combo, &iter))
		return;

	config = fetion_config_new();

	gtk_tree_model_get(model, &iter,
					L_NO_COL, &no, -1);

	if(fetion_user_list_remove(config, no) == -1){
		g_free(config);
		g_free(no);
		return;
	}

	gtk_tree_store_remove(GTK_TREE_STORE(model), &iter);
	if(gtk_tree_model_get_iter_root(model, &iter)){
		gtk_combo_box_set_active_iter(combo, &iter);
	}else{
		entry = gtk_bin_get_child(GTK_BIN(fxlogin->username));
		gtk_entry_set_text(GTK_ENTRY(entry), "");
		gtk_entry_set_text(GTK_ENTRY(fxlogin->password), "");
		pixbuf = gdk_pixbuf_new_from_file_at_size(
						SKIN_DIR"fetion.svg",
						PORTRAIT_SIZE,
						PORTRAIT_SIZE, NULL);
		if(pixbuf){
			gtk_image_set_from_pixbuf(
					GTK_IMAGE(fxlogin->portrait), pixbuf);
			g_object_unref(pixbuf);
		}
	}

	g_free(config);
	g_free(no);
}
