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
#include <errno.h>

History* fetion_history_message_new(const char* name , const char* userid
		, struct tm time , const char* msg , const int issend)
{
	History* history = (History*)malloc(sizeof(History));
	memset(history , 0 , sizeof(History));

	strcpy(history->name , name);
	strcpy(history->userid , userid);
	memcpy(&(history->sendtime) , &time , sizeof(struct tm));
	snprintf(history->message , 549 , "%s" , msg);
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
	if(fhistory != NULL){
		if(fhistory->file != NULL)
			fclose(fhistory->file);
		free(fhistory);
	}
}
void fetion_history_add(FetionHistory* fhistory , History* history)
{
	if(fhistory->file != NULL){
		fwrite(history , sizeof(History) , 1 , fhistory->file);
		fflush(fhistory->file);
	}
}

FxList* fetion_history_get_list(Config* config , const char* userid , int count)
{
	int len = 0;
	History* his;
	FxList* hislist;
	FxList* pos = NULL;
	FILE* file;
	char path[128];
	int n = 0;

	bzero(path , sizeof(path));
	sprintf(path , "%s/history.dat" , config->userPath);
	file = fopen(path , "r");
	if(file == NULL)
		return NULL;
	hislist = fx_list_new(NULL);
	while(!feof(file))
	{
		his = (History*)malloc(sizeof(History));
		memset(his , 0 , sizeof(History));
		len = fread(his , sizeof(History) , 1 , file);
		if(len > 0 && strcmp(userid , his->userid) == 0){
			pos = fx_list_new(his);
			fx_list_prepend(hislist , pos);
		}else{
			free(his);
		}
	}
	fclose(file);
	foreach_list_back(hislist , pos){
		if(n ++ == count){
			hislist->next = pos->next;
			break;
		}
	}
	return hislist;
}

FxList* fetion_history_get_e_list(Config *config , const char *userid , int type)
{
	
	int len = 0;
	History* his;
	FxList* hislist;
	FxList* pos = NULL;
	FILE* file;
	char path[128];
	time_t now;
	struct tm *tm;

	bzero(path , sizeof(path));
	sprintf(path , "%s/history.dat" , config->userPath);
	file = fopen(path , "r");
	if(file == NULL)
		return NULL;

	time(&now);
	tm = gmtime(&now);

	hislist = fx_list_new(NULL);
	while(!feof(file))
	{
		his = (History*)malloc(sizeof(History));
		memset(his , 0 , sizeof(History));
		len = fread(his , sizeof(History) , 1 , file);
		if(len > 0 && strcmp(userid , his->userid) == 0){
			  if(type == HISTORY_TODAY){
				  if(his->sendtime.tm_year == tm->tm_year
					  && his->sendtime.tm_yday == tm->tm_yday){
					pos = fx_list_new(his);
					fx_list_prepend(hislist , pos);
				  }
			  }else if(type == HISTORY_YEST){
				  if(his->sendtime.tm_year == tm->tm_year
					  && his->sendtime.tm_yday == tm->tm_yday - 1){
					pos = fx_list_new(his);
					fx_list_prepend(hislist , pos);
				  }
			  }else if(type == HISTORY_MONTH){
				  if(his->sendtime.tm_year == tm->tm_year
					  && his->sendtime.tm_mon == tm->tm_mon){
					pos = fx_list_new(his);
					fx_list_prepend(hislist , pos);
				  }
			  }else if(type == HISTORY_ALL){
					pos = fx_list_new(his);
					fx_list_prepend(hislist , pos);
			  }else{
			  		free(his);
			  }

		}else{
			free(his);
		}
	}
	fclose(file);
	return hislist;
}

int fetion_history_export(Config *config , const char *myid
		, const char *userid , const char *filename)
{
	FILE *f;
        FILE *f1;
	History his;
	char path[1024];
	char text[2048];
	char time[64];
	int n;

	memset(path , 0 , sizeof(path));
	sprintf(path , "%s/history.dat" , config->userPath);

	f1 = fopen(path , "r");
	if(f1 == NULL)
		return -1;

	f = fopen(filename , "w+");
	if(f == NULL)
		return -1;

	while(!feof(f1)){
		
		n = fread(&his , 1 , sizeof(History) , f1);
		if(n != sizeof(History))
			break;

		strftime(time , sizeof(time) , "%m月%d日 %H:%M:%S" , &(his.sendtime));
		memset(text , 0 , sizeof(text));
		sprintf(text , "%s(%s) %s\n" , his.name
				, his.issend ? myid : userid , time );
		strcat(text , his.message);
		strcat(text , "\n");
		fwrite(text , strlen(text) , 1 , f);
		fflush(f);

	}
	
	fclose(f);
	fclose(f1);

	return 1;
}
