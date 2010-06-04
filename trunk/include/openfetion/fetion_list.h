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

#ifndef FETION_LIST_H
#define FETION_LIST_H

#define foreach_list(head , item) \
	for(item = head; (item = item->next) != head; )

#define foreach_list_back(head , item) \
	for(item = head; (item = item->pre) != head; )

#define list_empty(list) (list->next == list)

extern FxList* fx_list_new(void *data);

extern void fx_list_free();

extern void fx_list_append(FxList *fxlist , FxList *fxitem);

extern void fx_list_prepend(FxList *fxlist , FxList *fxitem);

extern void fx_list_remove(FxList *fxitem);

#endif
