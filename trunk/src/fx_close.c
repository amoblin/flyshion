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

FxClose *fx_close_new(FxMain *fxmain)
{
	FxClose *fxclose = (FxClose*)malloc(sizeof(FxClose));

	DEBUG_FOOTPRINT();

	memset(fxclose , 0 , sizeof(FxClose));
	fxclose->fxmain = fxmain;

	return fxclose;
}

static void
fx_close_on_ok_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	gtk_dialog_response(GTK_DIALOG(data) , GTK_RESPONSE_OK);
}

static void
fx_close_on_cancel_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	gtk_dialog_response(GTK_DIALOG(data) , GTK_RESPONSE_CANCEL);
}

void fx_close_initialize(FxClose *fxclose)
{
	
	GSList *group;
	GtkWidget *okBtn , *cancelBtn , *label;
	GdkPixbuf *pb;
	GtkBox *vbox , *action_area;
	GtkFixed *fixed;
	
	fxclose->dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(fxclose->dialog) , _("Notification"));
	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"online.svg" , 22 , 22 , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxclose->dialog) , pb);
	g_object_unref(pb);
	gtk_widget_set_usize(fxclose->dialog , 280 , 180);
	gtk_window_set_resizable(GTK_WINDOW(fxclose->dialog) , FALSE);

	vbox = GTK_BOX(GTK_DIALOG(fxclose->dialog)->vbox);
	action_area = GTK_BOX(GTK_DIALOG(fxclose->dialog)->action_area);
	fixed = GTK_FIXED(gtk_fixed_new());
	gtk_box_pack_start_defaults(vbox , GTK_WIDGET(fixed));

	label = gtk_label_new(_("Are you sure you want to quit OpenFetion?"));
	gtk_fixed_put(fixed , label , 20 , 20);

	fxclose->closeBtn = gtk_radio_button_new_with_label(NULL , _("Exit OpenFetion"));
	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(fxclose->closeBtn));
	fxclose->iconBtn = gtk_radio_button_new_with_label(group , _("Minimize to tray"));
	fxclose->notalert = gtk_check_button_new_with_label(_("Don't notify again"));
	gtk_fixed_put(fixed , fxclose->closeBtn , 40 , 50);
	gtk_fixed_put(fixed , fxclose->iconBtn , 40 , 70);
	gtk_fixed_put(fixed , fxclose->notalert , 40 , 100);


	okBtn = gtk_button_new_with_label(_("OK"));
	g_signal_connect(okBtn , "clicked" , G_CALLBACK(fx_close_on_ok_clicked) , fxclose->dialog);
	cancelBtn = gtk_button_new_with_label(_("Cancel"));
	g_signal_connect(cancelBtn , "clicked" , G_CALLBACK(fx_close_on_cancel_clicked) , fxclose->dialog);
	gtk_box_pack_start_defaults(action_area , okBtn);
	gtk_box_pack_start_defaults(action_area , cancelBtn);

	GTK_WIDGET_SET_FLAGS(okBtn, GTK_CAN_FOCUS);
	gtk_widget_grab_focus(okBtn);

	gtk_widget_show_all(fxclose->dialog);
	gtk_widget_hide(fxclose->dialog);
}

int fx_close_get_action(FxClose *fxclose)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxclose->closeBtn))){
		return CLOSE_DESTROY_MODE;
	}else{
		return CLOSE_ICON_MODE;
	}
}

int fx_close_alert(FxClose *fxclose)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxclose->notalert))){
		return CLOSE_ALERT_DISABLE;
	}else{
		return CLOSE_ALERT_ENABLE;
	}
}
