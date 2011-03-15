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

int global_days;

static void on_ok_clicked(GtkWidget *widget , gpointer data);
static void on_cancel_clicked(GtkWidget *widget , gpointer data);
static void on_days_toggled(GtkWidget *widget , gpointer data);

FxSmsstat *fx_smsstat_new(FxMain *fxmain)
{
	FxSmsstat *fxsmsst = (FxSmsstat*)malloc(sizeof(FxSmsstat));

	global_days = 1;

	memset(fxsmsst , 0 , sizeof(FxSmsstat));
	fxsmsst->fxmain = fxmain;

	return fxsmsst;
}

void fx_smsstat_initialize(FxSmsstat *fxst)
{
	GtkWidget *fixed;
	GtkWidget *rbtn;
	GtkWidget *label;
	GdkPixbuf *pixbuf;
	GtkWidget *action_btn;
	GSList *gslist = NULL;
	int *arg;

	fxst->dialog = gtk_dialog_new();
	gtk_widget_set_usize(fxst->dialog , 350 , 280);
	gtk_window_set_title(GTK_WINDOW(fxst->dialog) , _("Settings Of Receving Message"));
	pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.svg",
				   20 , 20 ,NULL);
	gtk_window_set_icon(GTK_WINDOW(fxst->dialog) , pixbuf);
	g_object_unref(pixbuf);
	gtk_window_set_resizable(GTK_WINDOW(fxst->dialog) , FALSE);

	fixed = gtk_fixed_new();
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxst->dialog)->vbox) , fixed);

	label = gtk_label_new(_("You can set not to receive sms here , even\n"
				"for a period,in whitch you won`t receive the \n"
				"sms sent from fetion,but you can still receive \n"
				"the message from fetion client, the period will be:"));
	gtk_widget_set_usize(label , 320 , 0);
	gtk_fixed_put(GTK_FIXED(fixed) , label , 20 , 20);


	rbtn = gtk_radio_button_new_with_label(gslist , _("24 hours"));
	arg = (int*)malloc(sizeof(int));
	(*arg) = 1;
	g_signal_connect(rbtn , "toggled" , G_CALLBACK(on_days_toggled) , arg);
	gtk_fixed_put(GTK_FIXED(fixed) , rbtn , 35 , 110);

	gslist = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtn));
	rbtn = gtk_radio_button_new_with_label(gslist , _("3 days"));
	arg = (int*)malloc(sizeof(int));
	(*arg) = 3;
	g_signal_connect(rbtn , "toggled" , G_CALLBACK(on_days_toggled) , arg);
	gtk_fixed_put(GTK_FIXED(fixed) , rbtn , 35 , 135);

	gslist = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtn));
	rbtn = gtk_radio_button_new_with_label(gslist , _("1 week"));
	arg = (int*)malloc(sizeof(int));
	(*arg) = 7;
	g_signal_connect(rbtn , "toggled" , G_CALLBACK(on_days_toggled) , arg);
	gtk_fixed_put(GTK_FIXED(fixed) , rbtn , 35 , 160);

	gslist = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtn));
	rbtn = gtk_radio_button_new_with_label(gslist , _("forever"));
	arg = (int*)malloc(sizeof(int));
	(*arg) = 36500; /* 100 years , fuck it... */
	g_signal_connect(rbtn , "toggled" , G_CALLBACK(on_days_toggled) , arg);
	gtk_fixed_put(GTK_FIXED(fixed) , rbtn , 35 , 185);

	action_btn = gtk_button_new_with_label(_("OK"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxst->dialog)->action_area) , action_btn);
	g_signal_connect(action_btn , "clicked" , G_CALLBACK(on_ok_clicked) , fxst);

	action_btn = gtk_button_new_with_label(_("Cancel"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxst->dialog)->action_area) , action_btn);
	g_signal_connect(action_btn , "clicked" , G_CALLBACK(on_cancel_clicked) , fxst);

	gtk_widget_show_all(fxst->dialog);

}

static void on_ok_clicked(GtkWidget *UNUSED(widget), gpointer data)
{
	FxSmsstat *fxst = (FxSmsstat*)data;
	FxMain *fxmain = fxst->fxmain;
	User *user = fxmain->user;

	fetion_user_set_sms_status(user , global_days);
	
	gtk_dialog_response(GTK_DIALOG(fxst->dialog) , GTK_RESPONSE_OK);
}

static void on_cancel_clicked(GtkWidget *UNUSED(widget), gpointer data)
{
	FxSmsstat *fxst = (FxSmsstat*)data;
	
	gtk_dialog_response(GTK_DIALOG(fxst->dialog) , GTK_RESPONSE_CANCEL);
}

static void on_days_toggled(GtkWidget *UNUSED(widget), gpointer data)
{
	int *d = (int*)data;
	global_days = (*d);
}
