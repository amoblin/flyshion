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

FxProxy *fx_proxy_new(FxMain *fxmain)
{
	FxProxy *fxproxy = (FxProxy*)malloc(sizeof(FxProxy));

	DEBUG_FOOTPRINT();

	memset(fxproxy , 0 , sizeof(FxProxy));
	fxproxy->fxmain = fxmain;
	return fxproxy;
}

void fx_proxy_initialize(FxProxy *fxproxy)
{
	GtkBox *vbox = NULL;
	GtkBox *action_area = NULL;
	GtkWidget *hostLabel = NULL;
	GtkWidget *portLabel = NULL;
	GtkWidget *userLabel = NULL;
	GtkWidget *passLabel = NULL;

	DEBUG_FOOTPRINT();

	fxproxy->dialog = gtk_dialog_new();

	gtk_widget_set_usize(fxproxy->dialog , 300 , 200);

	vbox = GTK_BOX(GTK_DIALOG(fxproxy->dialog)->vbox);
	action_area = GTK_BOX(GTK_DIALOG(fxproxy->dialog)->action_area);

	fxproxy->enableBtn = gtk_check_button_new_with_label("开启HTTP代理");

	gtk_widget_show_all(fxproxy->dialog);
	gtk_widget_hide(fxproxy->dialog);

}

void fx_proxy_on_ok_clicked(GtkWidget *widget , gpointer data)
{

}

void fx_proxy_on_cancel_clicked(GtkWidget *widget , gpointer data)
{

}
