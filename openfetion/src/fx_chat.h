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

#ifndef FX_CHAT_H
#define FX_CHAT_H

extern FxChat* fx_chat_new(FxMain *fxmain , Conversation *conv);

extern void fx_chat_add_message(FxChat *fxchat , const char *msg
		, const struct tm *datetime , int issendmsg,
		int issysback);

extern void fx_chat_add_information(FxChat *fxchat , const char *msg);

extern void fx_chat_initialize(FxChat *fxchat);

extern void fx_chat_free(FxChat *fxchat);

extern void fx_chat_destroy(GtkWidget *UNUSED(widget) , gpointer data);

extern void fx_chat_send_message(FxChat *fxchat);

extern void fx_chat_nudge(FxChat *fxchat);

extern void fx_chat_nudge_in_thread(FxChat *fxchat);

extern void fx_chat_update_window(FxChat *fxchat);

extern void fx_chat_set_input(FxChat *fxchat);

extern void fx_chat_clear_input(FxChat *fxchat);

#endif
