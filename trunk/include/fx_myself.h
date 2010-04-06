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

#ifndef FX_MYSELF_H
#define FX_MYSELF_H

extern FxMyself* fx_myself_new(FxMain* fxmain);

extern void fx_myself_bind(FxMyself* fxmyself);

extern void fx_myself_add_message(FxMyself* fxmyself , const char* message);

extern void fx_myself_initialize(FxMyself* fxmyself);

extern gboolean fx_myself_on_enter_pressed(GtkWidget* widget , GdkEventKey* event , gpointer data);

extern void fx_myself_on_close_clicked(GtkWidget* widget , gpointer data);

extern void fx_myself_on_send_clicked(GtkWidget* widget , gpointer data);

#endif
