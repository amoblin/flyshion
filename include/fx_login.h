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

#ifndef FX_LOGIN_H
#define FX_LOGIN_H
enum
{
	L_NO_COL = 0 ,
	L_PWD_COL ,
	L_STATE_COL ,
	L_LAST_COL ,
	USER_COLS_NUM
};
extern FxLogin* fx_login_new();

extern void fx_login_free(FxLogin* fxlogin);

extern void fx_login_initialize(FxMain* fxmain);

extern GtkTreeModel* fx_login_create_state_model();

extern GtkTreeModel* fx_login_create_user_model();

extern void fx_login_set_last_login_user(FxLogin* fxlogin);

extern void fx_login_set_last_login_state(FxLogin* fxlogin , StateType state);

extern void fx_login_show_msg(FxLogin *fxlogin , const char *msg);

extern void fx_login_show_err(FxLogin *fxlogin , const char *msg);

extern void fx_login_hide(FxLogin *fxlogin);

extern void fx_login_free(FxLogin* fxlogin);

extern void fx_logining_show(FxMain *fxmain);

/* signal function */

extern void fx_login_action_func(GtkWidget* UNUSED(widget) , gpointer data);

extern void fx_login_user_change_func(GtkWidget* widget , gpointer data);

extern gboolean fx_login_proxy_button_func(GtkWidget *UNUSED(widget)
		, GdkEventButton *event , gpointer data);

#endif
