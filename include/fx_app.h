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

#ifndef FX_APP_H
#define FX_APP_H

enum
{
	APP_G_NAME_COL , 
	APP_G_ID_COL ,
	APP_G_COL_NUM
};

enum
{
	APP_ACCEPT = 1,
	APP_REJECT = 0
};

extern FxApp* fx_app_new(FxMain* fxmain , const char* sipuri , const char* userid , const char* desc , int phrase);

extern void fx_app_initialize(FxApp* fxapp);

extern GtkTreeModel* fx_app_create_group_model(FxMain* fxmain);

extern void fx_app_on_ok_clicked(GtkWidget* widget , gpointer data);

extern void fx_app_on_cancel_clicked(GtkWidget* widget , gpointer data);

extern void fx_app_on_check_clicked(GtkWidget* widget , gpointer data);

#endif
