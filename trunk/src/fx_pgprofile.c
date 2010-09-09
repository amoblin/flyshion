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

static void bind_profile(FxPGProfile *fxpg);
static gboolean focus_in_func(GtkWidget *UNUSED(widget)
		, GdkEventFocus *UNUSED(event) , gpointer data);
static gboolean focus_out_func(GtkWidget *UNUSED(widget)
		, GdkEventFocus *UNUSED(event) , gpointer data);

FxPGProfile *fx_pgprofile_new(FxMain *fxmain , PGGroup *pg)
{
	FxPGProfile *fxpg = (FxPGProfile*)malloc(sizeof(FxPGProfile));
	
	memset(fxpg , 0 , sizeof(FxPGProfile));
	fxpg->fxmain = fxmain;
	fxpg->pggroup = pg;

	return fxpg;
}

void fx_pgprofile_initialize(FxPGProfile *fxpg)
{
	GtkWidget *label;
	GtkWidget *frame;
	GtkWidget *scroll;
	GdkPixbuf *pixbuf;
	GtkWidget *fixed;

	fxpg->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable(GTK_WINDOW(fxpg->window) , FALSE);
	gtk_window_set_position(GTK_WINDOW(fxpg->window) , GTK_WIN_POS_CENTER);
	pixbuf = gdk_pixbuf_new_from_file(SKIN_DIR"pggroup.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxpg->window) , pixbuf);
	g_object_unref(pixbuf);
	gtk_widget_set_name(fxpg->window , "mainwindow");
	gtk_window_set_title(GTK_WINDOW(fxpg->window) , _("Group Details"));
	gtk_widget_set_usize(fxpg->window , 540 , 540);
	g_signal_connect(fxpg->window , "focus-in-event"
			, GTK_SIGNAL_FUNC(focus_in_func) , fxpg );

	g_signal_connect(fxpg->window , "focus-out-event"
			, GTK_SIGNAL_FUNC(focus_out_func) , fxpg );
	fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(fxpg->window) , fixed);

	frame = gtk_frame_new(_("Basic Information"));
	gtk_widget_set_usize(frame , 500 , 200);
	gtk_fixed_put(GTK_FIXED(fixed) , frame , 20 , 10);

	label = gtk_label_new(_("Group Number:"));
	gtk_fixed_put(GTK_FIXED(fixed) , label , 60 , 50);

	label = gtk_label_new(_("Group Name:"));
	gtk_fixed_put(GTK_FIXED(fixed) , label , 60 , 90);

	label = gtk_label_new(_("Administrator:"));
	gtk_fixed_put(GTK_FIXED(fixed) , label , 60 , 130);

	label = gtk_label_new(_("Create Time:"));
	gtk_fixed_put(GTK_FIXED(fixed) , label , 60 , 170);

	fxpg->sidEntry = gtk_entry_new();
	gtk_widget_set_usize(fxpg->sidEntry , 200 , 0);
	gtk_entry_set_editable(GTK_ENTRY(fxpg->sidEntry) , FALSE);
	gtk_fixed_put(GTK_FIXED(fixed) , fxpg->sidEntry , 150 , 45);
	
	fxpg->nameEntry = gtk_entry_new();
	gtk_widget_set_usize(fxpg->nameEntry , 200 , 0);
	gtk_entry_set_editable(GTK_ENTRY(fxpg->nameEntry) , FALSE);
	gtk_fixed_put(GTK_FIXED(fixed) , fxpg->nameEntry , 150 , 85);

	fxpg->leaderEntry = gtk_entry_new();
	gtk_widget_set_usize(fxpg->leaderEntry , 200 , 0);
	gtk_entry_set_editable(GTK_ENTRY(fxpg->leaderEntry) , FALSE);
	gtk_fixed_put(GTK_FIXED(fixed) , fxpg->leaderEntry , 150 , 125);

	fxpg->timeEntry = gtk_entry_new();
	gtk_widget_set_usize(fxpg->timeEntry , 200 , 0);
	gtk_entry_set_editable(GTK_ENTRY(fxpg->timeEntry) , FALSE);
	gtk_fixed_put(GTK_FIXED(fixed) , fxpg->timeEntry , 150 , 165);

	pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.svg" , 120 , 120 , NULL);
	fxpg->image = gtk_image_new_from_pixbuf(pixbuf);
	g_object_unref(pixbuf);

	frame = gtk_frame_new(NULL);
	gtk_container_add(GTK_CONTAINER(frame) , fxpg->image);
	gtk_fixed_put(GTK_FIXED(fixed) , frame , 370 , 50);

	frame = gtk_frame_new(_("Group Bulletin:"));
	gtk_widget_set_usize(frame , 500 , 140);
	gtk_fixed_put(GTK_FIXED(fixed) , frame , 20 , 225);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label) , _("<span color='#808080'>Only visible to group members,less than 256 characters</span>"));
	gtk_fixed_put(GTK_FIXED(fixed) , label , 40 , 250);

	scroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_widget_set_usize(scroll , 460 , 80);
	gtk_fixed_put(GTK_FIXED(fixed) , scroll , 40 , 270);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll)
					, GTK_POLICY_NEVER , GTK_POLICY_AUTOMATIC);

	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll)
					  , GTK_SHADOW_ETCHED_IN);
	fxpg->bulletinView = gtk_text_view_new();
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(fxpg->bulletinView) , FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxpg->bulletinView) , GTK_WRAP_CHAR);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(fxpg->bulletinView) , FALSE);
	gtk_container_add(GTK_CONTAINER(scroll) , fxpg->bulletinView);

	frame = gtk_frame_new(_("Group Summary:"));
	gtk_widget_set_usize(frame , 500 , 140);
	gtk_fixed_put(GTK_FIXED(fixed) , frame , 20 , 370);

	label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(label) , _("<span color='#808080'>Visible to anyone,less than 256 characters</span>"));
	gtk_fixed_put(GTK_FIXED(fixed) , label , 40 , 390);

	scroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_widget_set_usize(scroll , 460 , 80);
	gtk_fixed_put(GTK_FIXED(fixed) , scroll , 40 , 415);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll)
					, GTK_POLICY_NEVER , GTK_POLICY_AUTOMATIC);

	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll)
					  , GTK_SHADOW_ETCHED_IN);
	fxpg->introView = gtk_text_view_new();
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(fxpg->introView) , FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxpg->introView) , GTK_WRAP_CHAR);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(fxpg->introView) , FALSE);
	gtk_container_add(GTK_CONTAINER(scroll) , fxpg->introView);

	bind_profile(fxpg);

	gtk_widget_show_all(fxpg->window);

}

void bind_profile(FxPGProfile *fxpg)
{
	PGGroup *pggroup = fxpg->pggroup;
	Config *config = fxpg->fxmain->user->config;
	PGGroupMember *memcur;
	GdkPixbuf *pixbuf;
	GtkTextIter iter;
	struct tm date;
	char time[1024];
	char *sid;
	char path[1024];
	GtkTextView *text;
	GtkTextBuffer *buffer;

	sid = fetion_sip_get_pgid_by_sipuri(pggroup->pguri);
	if(sid == NULL){
	    fprintf(stderr , "ERROR : sid == NULL");
	    return;
	}

	gtk_entry_set_text(GTK_ENTRY(fxpg->sidEntry) , sid);
	gtk_entry_set_text(GTK_ENTRY(fxpg->nameEntry) , pggroup->name);
	foreach_pg_member(pggroup->member , memcur){
		if(memcur->identity == 1){
			gtk_entry_set_text(GTK_ENTRY(fxpg->leaderEntry) , memcur->nickname);
			break;
		}
	}
	date = convert_date(pggroup->createTime);
	strftime(time , sizeof(time) , _("%Y-%m-%d") , &date);
	gtk_entry_set_text(GTK_ENTRY(fxpg->timeEntry) , time);

	snprintf(path , 1023 , "%s/PG%s.jpg" , config->iconPath , sid);	
	pixbuf = gdk_pixbuf_new_from_file_at_size(path , 120 , 120 , NULL);
	if(pixbuf != NULL){
		gtk_image_set_from_pixbuf(GTK_IMAGE(fxpg->image) , pixbuf);
		g_object_unref(pixbuf);	
	}
	text = GTK_TEXT_VIEW(fxpg->bulletinView);
	buffer = gtk_text_view_get_buffer(text);
	gtk_text_buffer_get_end_iter(buffer , &iter);
	gtk_text_buffer_insert(buffer , &iter , pggroup->bulletin , strlen(pggroup->bulletin));

	text = GTK_TEXT_VIEW(fxpg->introView);
	buffer = gtk_text_view_get_buffer(text);
	gtk_text_buffer_get_end_iter(buffer , &iter);
	gtk_text_buffer_insert(buffer , &iter , pggroup->summary , strlen(pggroup->summary));

	free(sid);
}

static gboolean focus_in_func(GtkWidget *UNUSED(widget)
		, GdkEventFocus *UNUSED(event) , gpointer data) 
{
	FxPGGroup *fxpg = (FxPGGroup*)data;
	fxpg->hasFocus = CHAT_DIALOG_FOCUSED;
	return FALSE;
}

static gboolean focus_out_func(GtkWidget *UNUSED(widget)
		, GdkEventFocus *UNUSED(event) , gpointer data) 
{
	FxPGGroup *fxpg = (FxPGGroup*)data;
	fxpg->hasFocus = CHAT_DIALOG_NOT_FOCUSED;
	return FALSE;
}
