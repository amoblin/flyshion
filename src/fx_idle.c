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
#include <fx_include.h>

#ifdef USE_LIBXSS
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>
#include <gdk/gdkx.h>
#endif

#define IDLE_TIMEOUT_SEC 180

extern int old_state;

gint idle_timesec(void)
{

#ifdef USE_LIBXSS
	static XScreenSaverInfo *mit_info = NULL;
	static gint has_extension = -1;
	gint event_base, error_base;

	if (has_extension == -1)
		has_extension = XScreenSaverQueryExtension(
				GDK_DISPLAY(), &event_base, &error_base);

	if(has_extension){
		if (mit_info == NULL)
			mit_info = XScreenSaverAllocInfo();

		XScreenSaverQueryInfo(GDK_DISPLAY(), GDK_ROOT_WINDOW(), mit_info);
		return (mit_info->idle)/1000;
	}
#endif

	return 0;
}

void idle_autoaway(FxMain *fxmain)
{
	typedef struct {
		FxMain* fxmain;
		StateType type;
	} Args1;
	User   *user = fxmain->user;
	Config *config = user->config;
	Args1   *args;
	gint    os;

	if(config->autoAway == AUTO_AWAY_ENABLE){
 		args = (Args1*)malloc(sizeof(Args1));
		args->fxmain = fxmain;

		if(user->state > 0 && user->state != P_AWAY &&
				idle_timesec() > IDLE_TIMEOUT_SEC){
			os = user->state;
			args->type = P_AWAY;
			fx_head_change_state_func(NULL, args);
			old_state = os;
			return;
		}

		if(user->state != old_state &&
				idle_timesec() < IDLE_TIMEOUT_SEC){
			args->type = old_state;
			fx_head_change_state_func(NULL, args);
			return;
		}

		free(args);
	}
}
