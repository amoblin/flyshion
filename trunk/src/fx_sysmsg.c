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
#include "fx_sysmsg.h"

FxSysmsg *fx_sysmsg_new()
{
	FxSysmsg *sysmsg = (FxSysmsg*)malloc(sizeof(FxSysmsg));

	DEBUG_FOOTPRINT();

	return sysmsg;
}

static void fx_on_more_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	char *url = (char*)data;
	if(fork() == 0)
	{
		execlp("firefox" , "firefox" , url , NULL);
		free(url);
		return;
	}
}

static void fx_on_close_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	GtkWidget *dialog = (GtkWidget*)data;

	DEBUG_FOOTPRINT();

	gtk_dialog_response(GTK_DIALOG(dialog) , GTK_RESPONSE_CANCEL);
}

void fx_sysmsg_initialize(FxSysmsg *sysmsg)
{
	GtkWidget *closeBtn;

	GtkBox *vbox , *action_area;
	GdkPixbuf *pb;

	DEBUG_FOOTPRINT();

	sysmsg->dialog = gtk_dialog_new();
	vbox = GTK_BOX(GTK_DIALOG(sysmsg->dialog)->vbox);
	action_area = GTK_BOX(GTK_DIALOG(sysmsg->dialog)->action_area);

	pb = gdk_pixbuf_new_from_file(SKIN_DIR"user_online.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(sysmsg->dialog) , pb);
	gtk_window_set_title(GTK_WINDOW(sysmsg->dialog) , "系统消息");
	gtk_widget_set_usize(GTK_WIDGET(sysmsg->dialog) , 400 , 250);
	gtk_window_set_resizable(GTK_WINDOW(sysmsg->dialog) , FALSE);

	sysmsg->textview = gtk_text_view_new();
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(sysmsg->textview) , FALSE);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(sysmsg->textview) , FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(sysmsg->textview) , GTK_WRAP_CHAR);

	gtk_box_pack_start_defaults(vbox , sysmsg->textview);

	sysmsg->moreBtn = gtk_button_new_with_label("查看详情");
	gtk_box_pack_start_defaults(action_area , sysmsg->moreBtn);
	closeBtn = gtk_button_new_with_label("关闭");
	gtk_box_pack_start_defaults(action_area , closeBtn);
	g_signal_connect(closeBtn , "clicked" , G_CALLBACK(fx_on_close_clicked) , sysmsg->dialog);

	gtk_widget_show_all(sysmsg->dialog);
	gtk_widget_hide(sysmsg->dialog);
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
	bzero(pos , strlen(pos));

	strcpy(pos , url);
	g_signal_connect(sysmsg->moreBtn , "clicked" , G_CALLBACK(fx_on_more_clicked) , pos);

}
