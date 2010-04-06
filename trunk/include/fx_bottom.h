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

#ifndef FX_BOTTOM_H
#define FX_BOTTOM_H

extern FxBottom* fx_bottom_new();

extern void fx_bottom_initialize(FxMain* fxmain);

extern void fx_bottom_free(FxBottom* fxbottom);

extern void fx_bottom_on_setting_clicked(GtkWidget* widget , gpointer data);

extern void fx_bottom_on_sendtome_clicked(GtkWidget* widget , gpointer data);

extern void fx_bottom_on_sendtomany_clicked(GtkWidget* widget , gpointer data);

extern void fx_bottom_on_addfriend_clicked(GtkWidget* widget , gpointer data);

extern void fx_bottom_on_lookup_clicked(GtkWidget* widget , gpointer data);
#endif
