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

#ifndef FX_PROFILE_H
#define FX_PROFILE_H

extern FxProfile* fx_profile_new(FxMain* fxmain , const char* userid);

extern void fx_profile_initialize(FxProfile* fxprofile);

extern Contact *fx_profile_fetch(FxProfile *fxprofile);

extern void fx_profile_bind(FxProfile *fxprofile , Contact *contact);

extern void fx_profile_free(FxProfile *fxprofile);

extern void fx_profile_on_button_clicked(GtkWidget* UNUSED(widget) , gpointer data);



#endif
