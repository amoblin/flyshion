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

#ifndef FETION_HISTORY_H
#define FETION_HISTORY_H

#define HISTORY_TODAY 1
#define HISTORY_YEST  2
#define HISTORY_MONTH 3
#define HISTORY_ALL   4

extern History* fetion_history_message_new(const char* name
		, const char* userid , struct tm time , const char* msg , const int issend);

extern void fetion_history_message_free(History* history);

extern FetionHistory* fetion_history_new(User* user);

extern void fetion_history_free(FetionHistory* fhistory);

extern void fetion_history_add(FetionHistory* fhistory , History* history);

extern FxList* fetion_history_get_list(Config* config , const char* sid , int count);

extern FxList* fetion_history_get_e_list(Config *config , const char *userid , int type);

extern int fetion_history_export(Config *config , const char *myid
		, const char *userid , const char *filename);

#endif
