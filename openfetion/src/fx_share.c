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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netdb.h>

FxShare *fx_share_new(FxMain *fxmain , const char *sipuri)
{
	FxShare *fxshare = (FxShare*)malloc(sizeof(FxShare));
	User *user = fxmain->user;

// 	DEBUG_FOOTPRINT();

	memset(fxshare , 0 , sizeof(FxShare));

	fxshare->fxmain = fxmain;
	fxshare->contact = fetion_contact_list_find_by_sipuri(user->contactList , sipuri);
	if(fxshare->contact == NULL){
		return NULL;
	}

	return fxshare;
}

void fx_share_initialize(FxShare *fxshare)
{
	GtkBox *vbox;
	GtkWidget *fixed;
	GtkWidget *pLabel;
	GtkWidget *okButton;
	GtkWidget *cancelButton;
	GdkPixbuf *pb;
	char text[1024];

// 	DEBUG_FOOTPRINT();

	fxshare->dialog = gtk_dialog_new();

	gtk_window_set_default_size(GTK_WINDOW(fxshare->dialog) , 400 , 180);
	pb = gdk_pixbuf_new_from_file(SKIN_DIR"sendfile.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxshare->dialog) , pb);
	gtk_window_set_title(GTK_WINDOW(fxshare->dialog) , _("Transport files by Fetion"));

	vbox = GTK_BOX(GTK_DIALOG(fxshare->dialog)->vbox);

	fixed = gtk_fixed_new();
	gtk_box_pack_start_defaults(GTK_BOX(vbox) , fixed);

	fxshare->uLabel = gtk_label_new(NULL);
	memset(text, 0, sizeof(text));

	sprintf(text , _("Transporting files <b>OS Revolution.mkv </b> to <b>%s</b>...")
			, fxshare->contact->nickname);

	gtk_label_set_markup(GTK_LABEL(fxshare->uLabel) , text);
	gtk_fixed_put(GTK_FIXED(fixed) , fxshare->uLabel , 20 , 20);

	pLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(pLabel) , _("<b>Progress: </b>"));
	gtk_fixed_put(GTK_FIXED(fixed) , pLabel , 10 , 65);

	fxshare->progress = gtk_progress_bar_new();
	gtk_widget_set_usize(fxshare->progress , 300 , 30);
	gtk_fixed_put(GTK_FIXED(fixed) , fxshare->progress , 60 , 60);
	gtk_progress_set_percentage(GTK_PROGRESS(fxshare->progress) , 0.8);

	fxshare->iLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(fxshare->iLabel)
			, _("<span color='#838383'>Establishing Connection...</span>"));
	gtk_fixed_put(GTK_FIXED(fixed) , fxshare->iLabel , 60 , 100);

	okButton = gtk_button_new_with_label(_("OK"));
	gtk_widget_set_sensitive(okButton , FALSE);
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxshare->dialog)->action_area) , okButton);

	cancelButton = gtk_button_new_with_label(_("Cancel"));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxshare->dialog)->action_area) , cancelButton);
	

	gtk_widget_show_all(fxshare->dialog);
}

FxShare *fx_share_find_by_sipuri(FxList *fxlist , const char *sipuri)
{
	FxShare *fxshare;
	FxList *pos = fxlist;

// 	DEBUG_FOOTPRINT();

	while(pos != NULL){
		fxshare = (FxShare*)(pos->data);
		if(fxshare->contact == NULL){
			pos = pos->next;
			continue;
		}
		if(strcmp(fxshare->contact->sipuri , sipuri) == 0){
			return fxshare;
		}
		pos = pos->next;
	}
	return NULL;
}

void fx_share_start_transfer(FxShare *fxshare)
{
	Share *share = fxshare->share;
	FetionConnection *tcp ;
	char *innerIp;
	int innerUdpPort;
	FILE *file;
	unsigned char buf[1024];
	int i;

// 	DEBUG_FOOTPRINT();

	gtk_label_set_markup(GTK_LABEL(fxshare->iLabel)
			, _("<span color='#838383'>Contact acccept your request of transporting files. Transporting...</span>"));

	tcp = tcp_connection_new_with_port(1435);
	tcp_connection_getname(tcp , &innerIp , &innerUdpPort);
	printf("%s:%d\n" , innerIp , innerUdpPort);
	
	share->innerUdpPort = 1435;
	printf("%s:%d\n" , share->outerIp , share->outerTcpPort);
	int ret;
	ret = tcp_connection_connect(tcp , share->outerIp , share->outerTcpPort);
	printf("%d\n" , ret);

	file = fopen(share->absolutePath , "r");


	while(1){
		
		i = fread(buf , 1 , sizeof(buf) , file);
		if(i == 0){
			break;
		}
		printf("%d\n" , i);
		ret = tcp_connection_send(tcp , buf , i);
		printf("SEND : %d\n" , ret);
	}
	fclose(file);
	tcp_connection_free(tcp);
}
