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

#include "fetion_include.h"

History* fetion_history_message_new(const char* name , const char* userid
		, struct tm time , const char* msg , const int issend)
{
	History* history = (History*)malloc(sizeof(History));
	memset(history , 0 , sizeof(History));
	strcpy(history->name , name);
	strcpy(history->userid , userid);
	memcpy(&(history->sendtime) , &time , sizeof(struct tm));
	strcpy(history->message , msg);
	history->issend = issend;
	return history;
}

void fetion_history_message_free(History* history)
{
	free(history);
}
FetionHistory* fetion_history_new(User* user)
{
	FetionHistory* fhistory;
	Config* config = user->config;
	char filepath[128];
	
	fhistory = (FetionHistory*)malloc(sizeof(FetionHistory));
	memset(fhistory , 0 , sizeof(FetionHistory));
	fhistory->user = user;
	bzero(filepath , sizeof(filepath));
	sprintf(filepath , "%s/history.dat" , config->userPath);
	fhistory->file = fopen(filepath , "a+");
	return fhistory;
}

void fetion_history_free(FetionHistory* fhistory)
{
	if(fhistory != NULL)
	{
		if(fhistory->file != NULL)
			fclose(fhistory->file);
		free(fhistory);
	}
}
void fetion_history_add(FetionHistory* fhistory , History* history)
{
	if(fhistory->file != NULL)
	{
		fwrite(history , sizeof(History) , 1 , fhistory->file);
		fflush(fhistory->file);
	}
}

FxList* fetion_history_get_list(Config* config , const char* userid , int count)
{
	int len = 0;
	History* his;
	FxList* hislist = NULL;
	FxList* pos = NULL;
	FILE* file;
	char path[128];
	int n = 0;
	bzero(path , sizeof(path));
	sprintf(path , "%s/history.dat" , config->userPath);
	file = fopen(path , "r");
	if(file == NULL)
		return NULL;
	while(!feof(file))
	{
		his = (History*)malloc(sizeof(History));
		memset(his , 0 , sizeof(History));
		len = fread(his , sizeof(History) , 1 , file);
		if(len > 0 && strcmp(userid , his->userid) == 0)
		{
			pos = fx_list_new(his);
			fx_list_append(&hislist , pos);
		}
		else
		{
			free(his);
		}
	}
	fclose(file);
	while(pos != NULL)
	{
		if(n ++ == count || pos->pre == NULL)
		{
			hislist = pos;
			break;
		}
		pos = pos->pre;
	}
	return hislist;
}
