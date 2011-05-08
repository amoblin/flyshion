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

#include "fx_sysmsg.h"

FxSysmsg *fx_sysmsg_new()
{
	FxSysmsg *sysmsg = (FxSysmsg*)malloc(sizeof(FxSysmsg));

	return sysmsg;
}

static void fx_on_more_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	char *url = (char*)data;
	if(fork() == 0)
	{
		execlp("xdg-open" , "xdg-open" , url , NULL);
		free(url);
		return;
	}
}

static void fx_on_close_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	GtkWidget *dialog = (GtkWidget*)data;

	gtk_widget_destroy(dialog);
}

void fx_sysmsg_initialize(FxSysmsg *sysmsg)
{
	GtkWidget *closeBtn;

	GtkBox *vbox , *action_area;
	GdkPixbuf *pb;

	sysmsg->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	vbox = GTK_BOX(gtk_vbox_new (FALSE, 0));
	gtk_container_add (GTK_CONTAINER(sysmsg->dialog), GTK_WIDGET(vbox));

	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"online.svg",
				   22 , 22 ,NULL);
	gtk_window_set_icon(GTK_WINDOW(sysmsg->dialog) , pb);
	g_object_unref(pb);
	gtk_window_set_title(GTK_WINDOW(sysmsg->dialog) , _("System mesage"));
	gtk_window_set_position(GTK_WINDOW(sysmsg->dialog) , GTK_WIN_POS_CENTER);
	gtk_widget_set_usize(GTK_WIDGET(sysmsg->dialog) , 400 , 250);
	gtk_window_set_resizable(GTK_WINDOW(sysmsg->dialog) , FALSE);

	sysmsg->textview = gtk_text_view_new();
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(sysmsg->textview) , FALSE);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(sysmsg->textview) , FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(sysmsg->textview) , GTK_WRAP_CHAR);

	gtk_box_pack_start(vbox , sysmsg->textview, TRUE, TRUE, 0);

	action_area = GTK_BOX(gtk_hbox_new(FALSE , 0));
	gtk_box_pack_start(GTK_BOX(vbox) , GTK_WIDGET(action_area) , FALSE , FALSE , 5);

	closeBtn = gtk_button_new_with_label(_("Close"));
	gtk_widget_set_usize(closeBtn, 100, 30);
	gtk_box_pack_end(action_area , closeBtn, FALSE, FALSE, 2);
	g_signal_connect(closeBtn , "clicked" , G_CALLBACK(fx_on_close_clicked) , sysmsg->dialog);
	sysmsg->moreBtn = gtk_button_new_with_label(_("View details"));
	gtk_widget_set_usize(sysmsg->moreBtn, 100, 30);
	gtk_box_pack_end(action_area , sysmsg->moreBtn, FALSE, FALSE, 2);

	gtk_widget_show_all(sysmsg->dialog);
}

void fx_sysmsg_bind(FxSysmsg *sysmsg
		, const char *msg , const char *url)
{
	GtkTextBuffer *buffer ;
	GtkTextIter iter;
	char *pos;

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sysmsg->textview));
	gtk_text_buffer_get_end_iter(buffer , &iter);
	gtk_text_buffer_insert(buffer , &iter , msg , strlen(msg));

	pos = (char*)malloc(strlen(url) + 1);
	memset(pos, 0, strlen(pos));

	strcpy(pos , url);
	g_signal_connect(sysmsg->moreBtn , "clicked" , G_CALLBACK(fx_on_more_clicked) , pos);

}
