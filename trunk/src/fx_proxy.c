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

FxProxy *fx_proxy_new(FxLogin *fxlogin)
{
	FxProxy *fxproxy = (FxProxy*)malloc(sizeof(FxProxy));

	DEBUG_FOOTPRINT();

	memset(fxproxy , 0 , sizeof(FxProxy));
	fxproxy->fxlogin = fxlogin;
	return fxproxy;
}

static void fx_proxy_set_sensitive(FxProxy *fxproxy , gboolean enable)
{

	DEBUG_FOOTPRINT();

	gtk_widget_set_sensitive(fxproxy->hostEntry , enable);
	gtk_widget_set_sensitive(fxproxy->portEntry , enable);
	gtk_widget_set_sensitive(fxproxy->userEntry , enable);
	gtk_widget_set_sensitive(fxproxy->passEntry , enable);
}

static void fx_proxy_enable_toggled(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxProxy *fxproxy = (FxProxy*)data;

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxproxy->enableBtn)))
		fx_proxy_set_sensitive(fxproxy , TRUE);
	else
		fx_proxy_set_sensitive(fxproxy , FALSE);
	
	DEBUG_FOOTPRINT();
}

void fx_proxy_initialize(FxProxy *fxproxy)
{
	GtkBox *vbox = NULL;
	GtkBox *action_area = NULL;
	GtkWidget *hostLabel = NULL;
	GtkWidget *portLabel = NULL;
	GtkWidget *userLabel = NULL;
	GtkWidget *passLabel = NULL;
	GtkWidget *fixed = NULL;
	GtkWidget *okBtn = NULL;
	GtkWidget *cancelBtn = NULL;
	GdkPixbuf *pb = NULL;
	char text[128];
	Proxy *proxy = fxproxy->fxlogin->proxy;

	DEBUG_FOOTPRINT();

	fxproxy->dialog = gtk_dialog_new();

	gtk_widget_set_usize(fxproxy->dialog , 350 , 220);
	gtk_window_set_resizable(GTK_WINDOW(fxproxy->dialog) , FALSE);
	gtk_window_set_title(GTK_WINDOW(fxproxy->dialog) , "设置HTTP代理");
	pb = gdk_pixbuf_new_from_file(SKIN_DIR"proxy.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxproxy->dialog) , pb);

	vbox = GTK_BOX(GTK_DIALOG(fxproxy->dialog)->vbox);
	action_area = GTK_BOX(GTK_DIALOG(fxproxy->dialog)->action_area);

	fixed = gtk_fixed_new();

	fxproxy->enableBtn = gtk_check_button_new_with_label("开启HTTP代理");
	g_signal_connect(fxproxy->enableBtn , "toggled"
			, G_CALLBACK(fx_proxy_enable_toggled) , fxproxy);
	gtk_fixed_put(GTK_FIXED(fixed) , fxproxy->enableBtn , 20 , 20);

	fxproxy->errorLabel = gtk_label_new(NULL);
	gtk_fixed_put(GTK_FIXED(fixed) , fxproxy->errorLabel , 180 , 22);
	
	hostLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(hostLabel) , "<b>代理主机</b>");
	gtk_fixed_put(GTK_FIXED(fixed) , hostLabel , 20 , 55);

	portLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(portLabel) , "<b>端口</b>");
	gtk_fixed_put(GTK_FIXED(fixed) , portLabel , 190 , 55);
	
	fxproxy->hostEntry = gtk_entry_new();
	gtk_widget_set_usize(fxproxy->hostEntry , 140 , 25);
	gtk_fixed_put(GTK_FIXED(fixed) , fxproxy->hostEntry , 20 , 75);

	fxproxy->portEntry = gtk_entry_new();
	gtk_widget_set_usize(fxproxy->portEntry , 140 , 25);
	gtk_fixed_put(GTK_FIXED(fixed) , fxproxy->portEntry , 190 , 75);

	userLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(userLabel) , "<b>用户名</b>");
	gtk_fixed_put(GTK_FIXED(fixed) , userLabel , 20 , 110);

	passLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(passLabel) , "<b>密码</b>");
	gtk_fixed_put(GTK_FIXED(fixed) , passLabel , 190 , 110);
	
	fxproxy->userEntry = gtk_entry_new();
	gtk_widget_set_usize(fxproxy->userEntry , 140 , 25);
	gtk_fixed_put(GTK_FIXED(fixed) , fxproxy->userEntry , 20 , 130);

	fxproxy->passEntry = gtk_entry_new();
	gtk_widget_set_usize(fxproxy->passEntry , 140 , 25);
	gtk_entry_set_visibility(GTK_ENTRY(fxproxy->passEntry) , FALSE);
	gtk_fixed_put(GTK_FIXED(fixed) , fxproxy->passEntry , 190 , 130);

	okBtn = gtk_button_new_with_label("确定");
	g_signal_connect(okBtn , "clicked"
			, G_CALLBACK(fx_proxy_on_ok_clicked) , fxproxy);
	cancelBtn = gtk_button_new_with_label("取消");
	g_signal_connect(cancelBtn , "clicked"
			, G_CALLBACK(fx_proxy_on_cancel_clicked) , fxproxy);

	gtk_box_pack_start_defaults(vbox , fixed);
	gtk_box_pack_start_defaults(action_area , okBtn);
	gtk_box_pack_start_defaults(action_area , cancelBtn);

	/* bind data*/
	if(proxy != NULL && proxy->proxyEnabled)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxproxy->enableBtn) , TRUE);
		fx_proxy_set_sensitive(fxproxy , TRUE);
	}
	else
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(fxproxy->enableBtn) , FALSE);
		fx_proxy_set_sensitive(fxproxy , FALSE);
	}
	if(proxy != NULL)
	{
		gtk_entry_set_text(GTK_ENTRY(fxproxy->hostEntry) , proxy->proxyHost);
		bzero(text , sizeof(text));
		sprintf(text , "%d" , proxy->proxyPort);
		gtk_entry_set_text(GTK_ENTRY(fxproxy->portEntry) , text);
		gtk_entry_set_text(GTK_ENTRY(fxproxy->userEntry) , proxy->proxyUser);
		gtk_entry_set_text(GTK_ENTRY(fxproxy->passEntry) , proxy->proxyPass);
	}
	/* show widgets*/
	gtk_widget_show_all(fxproxy->dialog);
	gtk_widget_hide(fxproxy->dialog);

}

void fx_proxy_on_ok_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxProxy *fxproxy = (FxProxy*)data;
	FxLogin *fxlogin = fxproxy->fxlogin;
	const char *text = NULL;
	Proxy *proxy = NULL;

	DEBUG_FOOTPRINT();

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxproxy->enableBtn)))
	{
		proxy = (Proxy*)malloc(sizeof(Proxy));
		memset(proxy , 0 , sizeof(proxy));
		proxy->proxyEnabled = TRUE;
		text = gtk_entry_get_text(GTK_ENTRY(fxproxy->hostEntry));
		if(strlen(text) == 0)
		{
			gtk_label_set_markup(GTK_LABEL(fxproxy->errorLabel)
					, "<span color='red'>请输入主机名</span>");
			return;
		}
		strcpy(proxy->proxyHost , text);
		text = gtk_entry_get_text(GTK_ENTRY(fxproxy->portEntry));
		proxy->proxyPort = atoi(text);
		if(strlen(text) == 0)
		{
			gtk_label_set_markup(GTK_LABEL(fxproxy->errorLabel)
					, "<span color='red'>请输入端口号</span>");
			return;
		}
		text = gtk_entry_get_text(GTK_ENTRY(fxproxy->userEntry));
		strcpy(proxy->proxyUser , text);
		text = gtk_entry_get_text(GTK_ENTRY(fxproxy->passEntry));
		strcpy(proxy->proxyPass , text);
		fetion_config_save_proxy(proxy);
		free(fxlogin->proxy);
		fxlogin->proxy = proxy;
		gtk_label_set_markup(GTK_LABEL(fxlogin->proxyLabel) ,
				"<span color='#0099ff'><small> 网络代理[开启]</small></span>");
	}
	else
	{
		if(fxlogin->proxy != NULL)
		{
			fxlogin->proxy->proxyEnabled = FALSE;			
			fetion_config_save_proxy(fxlogin->proxy);
		}

		gtk_label_set_markup(GTK_LABEL(fxlogin->proxyLabel) ,
				"<span color='#0099ff'><small> 网络代理[关闭]</small></span>");
	}

	gtk_dialog_response(GTK_DIALOG(fxproxy->dialog) , GTK_RESPONSE_OK);
}

void fx_proxy_on_cancel_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxProxy *fxproxy = (FxProxy*)data;

	DEBUG_FOOTPRINT();

	gtk_dialog_response(GTK_DIALOG(fxproxy->dialog) , GTK_RESPONSE_CANCEL);
}
