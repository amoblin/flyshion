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

char* fx_util_get_state_name(StateType type)
{
	char *name = (char*)malloc(48);
	memset(name , 0 , 48);
	switch(type)
	{
		case P_ONLINE:
			strcpy(name , "[在线]");
			break;
		case P_BUSY:
			strcpy(name , "[忙碌]");
			break;
		case P_AWAY:
			strcpy(name , "[离开]");
			break;
		case P_HIDDEN:
			strcpy(name , "[隐身]");
			break;
		case P_OUTFORLUNCH:
			strcpy(name , "[外出就餐]");
			break;
		case P_DONOTDISTURB:
			strcpy(name , "[请勿打扰]");
			break;
		case P_RIGHTBACK:
			strcpy(name , "[马上回来]");
			break;
		case P_MEETING:
			strcpy(name , "[会议中]");
			break;
		case P_ONTHEPHONE:
			strcpy(name , "[电话中]");
			break;
		default:
			break;
	}
	return name;
}
void fx_util_popup_warning(FxMain* fxmain , const char* message)
{
	GtkWidget *dialog = NULL;
	dialog = gtk_message_dialog_new(GTK_WINDOW(fxmain->window),
									GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_WARNING,
									GTK_BUTTONS_OK,
									message);
	gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}
