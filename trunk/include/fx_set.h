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

#ifndef FX_SET_H
#define FX_SET_H

enum{
	COMBO_NAME_COL = 0 ,
	COMBO_ID_COL ,
	COMBO_COLS_NUM
};
typedef enum{
	SEND_MODE_ENTER ,
	SEND_MODE_CTRL_ENTER
} SendModeType;

typedef enum{
	AUTO_POPUP_DISABLE ,
	AUTO_POPUP_ENABLE
} AutoPopupType;

typedef enum{
	AUTO_REPLY_DISABLE ,
	AUTO_REPLY_ENABLE
} AutoReplyType;

typedef enum{
	CLOSE_DESTROY_MODE ,
	CLOSE_ICON_MODE 
} CloseType;

typedef enum{
	MUTE_DISABLE ,
	MUTE_ENABLE
} MuteType;

typedef enum{
	PAGE_PERSONAL ,
	PAGE_SYSTEM
} NotebookPageType;

typedef enum{
	ICON_CAN ,
	ICON_CANNOT 
} IconType;

typedef enum {
	AUTO_AWAY_ENABLE,
	AUTO_AWAY_DISABLE
} AutoAway;

typedef enum {
	ONLINE_NOTIFY_ENABLE,
	ONLINE_NOTIFY_DISABLE
} AutoAway;

extern FxSet* fx_set_new(FxMain* fxmain);

extern void fx_set_initialize(FxSet* fxset);

extern void fx_set_bind_system(FxSet* fxset);

extern void fx_set_initialize_personal(FxSet* fxset);

extern void fx_set_initialize_system(FxSet* fxset);

extern GtkTreeModel* fx_set_create_gender_model();

#endif
