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

#include "fetion.h"
#include "fx_code.h"

#include <gdk/gdkkeysyms.h>

static void fx_code_on_ok_clicked(GtkWidget *UNUSED(widget), gpointer data);
static void fx_code_on_cancel_clicked(GtkWidget *UNUSED(widget), gpointer data);
static gboolean fx_code_on_key_pressed(GtkWidget *UNUSED(widget), GdkEventKey *event, gpointer data);

FxCode* fx_code_new(const gchar *reason, const gchar *tips)
{
	FxCode* fxcode = (FxCode*)malloc(sizeof(FxCode));

	memset(fxcode , 0 , sizeof(FxCode));
	if(reason) strncpy(fxcode->reason, reason, sizeof(fxcode->reason));
	if(tips)   strncpy(fxcode->tip, tips, sizeof(fxcode->tip));
	return fxcode;
}

void fx_code_initialize(FxCode *fxcode, const gchar *pic_path)
{
	GtkWidget *fixed;
	GtkWidget *vbox , *action_area;
	GtkWidget *ok_button , *cancel_button;
	GdkPixbuf *pb;

	fxcode->dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(fxcode->dialog) , _("Please entry your verification code"));
	vbox = GTK_DIALOG(fxcode->dialog)->vbox;
	action_area = GTK_DIALOG(fxcode->dialog)->action_area;
	fixed = gtk_fixed_new();
	gtk_widget_set_usize(fxcode->dialog , 460 , 250);
	//gtk_fixed_put(GTK_FIXED(fixed) , warningpic , 20 , 20 );
	
	fxcode->reasonlabel = gtk_label_new(fxcode->reason);
	gtk_widget_set_usize(fxcode->reasonlabel , 300 , 40);
	gtk_label_set_line_wrap(GTK_LABEL(fxcode->reasonlabel) , TRUE );
	gtk_label_set_justify(GTK_LABEL(fxcode->reasonlabel), GTK_JUSTIFY_FILL);
	gtk_fixed_put(GTK_FIXED(fixed) , fxcode->reasonlabel , 150 , 20);

	fxcode->codeentry = gtk_entry_new();
	gtk_widget_set_usize(fxcode->codeentry , 300 , 30);
	gtk_fixed_put(GTK_FIXED(fixed)  , fxcode->codeentry , 150 , 70);
	g_signal_connect(fxcode->codeentry, "key_press_event" , G_CALLBACK(fx_code_on_key_pressed), fxcode);

	pb = gdk_pixbuf_new_from_file_at_size(pic_path, 130, 36, NULL);
	fxcode->codepic = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_widget_set_tooltip_markup(fxcode->codepic , _("Click here getting new verification code"));
	gtk_fixed_put(GTK_FIXED(fixed) , fxcode->codepic , 150 , 120);


	fxcode->tiplabel = gtk_label_new(fxcode->tip);
	gtk_widget_set_usize(fxcode->tiplabel , 450 , 40);
	gtk_label_set_line_wrap(GTK_LABEL(fxcode->tiplabel) , TRUE );
	gtk_label_set_justify(GTK_LABEL(fxcode->tiplabel), GTK_JUSTIFY_FILL);
	gtk_fixed_put(GTK_FIXED(fixed) , fxcode->tiplabel , 20 , 160);

	ok_button = gtk_button_new_with_label(_("OK"));
	GTK_WIDGET_SET_FLAGS(ok_button , GTK_CAN_FOCUS);
	gtk_widget_grab_focus(ok_button);
	g_signal_connect(ok_button , "clicked" , G_CALLBACK(fx_code_on_ok_clicked) , fxcode->dialog);
	gtk_box_pack_start_defaults(GTK_BOX(action_area) , ok_button);

	cancel_button = gtk_button_new_with_label(_("Cancel"));
	g_signal_connect(cancel_button , "clicked" , G_CALLBACK(fx_code_on_cancel_clicked) , fxcode->dialog);
	gtk_box_pack_start_defaults(GTK_BOX(action_area) , cancel_button);

	gtk_box_pack_start_defaults(GTK_BOX(vbox) , fixed);
	gtk_widget_show_all(fxcode->dialog);
	gtk_widget_hide(fxcode->dialog);
}

const gchar *fx_code_get_code(FxCode *fxcode)
{
	return gtk_entry_get_text(GTK_ENTRY(fxcode->codeentry));
}

static void fx_code_on_ok_clicked(GtkWidget *UNUSED(widget), gpointer data)
{
	gtk_dialog_response(GTK_DIALOG(data), GTK_RESPONSE_OK);
}

static void fx_code_on_cancel_clicked(GtkWidget *UNUSED(widget), gpointer data)
{
	gtk_dialog_response(GTK_DIALOG(data), GTK_RESPONSE_CANCEL);
}

static gboolean fx_code_on_key_pressed(GtkWidget *UNUSED(widget), GdkEventKey *event, gpointer data)
{
	FxCode *fxcode = (FxCode*)data;
	if(event->keyval == GDK_Return || event->keyval == GDK_ISO_Enter || event->keyval == GDK_KP_Enter) {
		fx_code_on_ok_clicked(NULL, fxcode->dialog);
		return TRUE;
	}
	return FALSE;
}
