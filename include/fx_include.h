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
#ifndef FX_INCLUDE_H
#define FX_INCLUDE_H

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include <config.h>
#ifdef USE_LIBNOTIFY
 #include <libnotify/notify.h>
#endif
#include <openfetion.h>

#ifdef ENABLE_NLS
	#include "glib/gi18n.h"
#else
	#define  _(String) (String)
	#define N_(String) (String)
#endif
#include "fx_types.h"
#include "fx_util.h"
#include "fx_sound.h"
#include "fx_tree.h"
#include "fx_head.h"
#include "fx_bottom.h"
#include "fx_main.h"
#include "fx_login.h"
#include "fx_proxy.h"
#include "fx_add_group.h"
#include "fx_profile.h"
#include "fx_dsms.h"
#include "fx_edit.h"
#include "fx_gedit.h"
#include "fx_chat.h"
#include "fx_myself.h"
#include "fx_history.h"
#include "fx_many.h"
#include "fx_lookup.h"
#include "fx_lookupres.h"
#include "fx_addbuddy.h"
#include "fx_code.h"
#include "fx_app.h"
#include "fx_set.h"
#include "fx_emotion.h"
#include "fx_sysmsg.h"
#include "fx_close.h"
#include "fx_search.h"
#include "fx_pggroup.h"
#include "fx_pgprofile.h"
#include "fx_smsstat.h"
#include "fx_idle.h"
#include "fx_conn.h"
#include "fx_about.h"
#endif
