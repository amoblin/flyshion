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

#include <openfetion.h>

FxList* fx_list_new(void* data)
{
	FxList* fxlist = (FxList*)malloc(sizeof(FxList));
	memset(fxlist , 0 , sizeof(FxList));
	fxlist->pre = fxlist;
	fxlist->data = data;
	fxlist->next = fxlist;
	return fxlist;
}

void fx_list_free(FxList *fxlist)
{
	if(fxlist != NULL)
		free(fxlist);
}

void fx_list_append(FxList *fxlist , FxList *fxitem)
{
	fxlist->next->pre = fxitem;
	fxitem->next = fxlist->next;
	fxitem->pre = fxlist;
	fxlist->next = fxitem;
}

void fx_list_prepend(FxList *fxlist , FxList *fxitem)
{
	fxlist->pre->next = fxitem;
	fxitem->pre = fxlist->pre;
	fxitem->next = fxlist;
	fxlist->pre = fxitem;
}

void fx_list_remove(FxList *fxitem)
{
	fxitem->next->pre = fxitem->pre;
	fxitem->pre->next = fxitem->next;
}
