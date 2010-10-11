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

History* fetion_history_message_new(const char* name,
		const char* userid, struct tm time,
		const char* msg , const int issend)
{
	History* history = (History*)malloc(sizeof(History));
	memset(history , 0 , sizeof(History));

	strcpy(history->name , name);
	strcpy(history->userid , userid);
	strftime(history->sendtime,
			sizeof(history->sendtime),
			"%Y-%m-%d %H:%M:%S" , &time);
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
	sprintf(filepath, "%s/data.db",
			config->userPath);
	sqlite3_open(filepath, &(fhistory->db));
	return fhistory;
}

void fetion_history_free(FetionHistory* fhistory)
{
	if(fhistory){
		if(fhistory->db)
			sqlite3_close(fhistory->db);
		free(fhistory);
	}
}
void fetion_history_add(FetionHistory* fhistory , History* history)
{
	sqlite3 *db;
	char sql[4096];
	char sql1[4096];
	char *errMsg;
	db = fhistory->db;

	if(!db){
		debug_error("db is closed,write history FAILED");
		return;
	}

	escape_sql(history->message);

	sprintf(sql, "insert into history values"
			" ('%s','%s','%s',datetime('%s'),%d)",
			history->name, history->userid,
			history->message, history->sendtime,
			history->issend);

	if(sqlite3_exec(db, sql, 0, 0, &errMsg)){
		sprintf(sql1, "create table history ("
				"name,userid,message,updatetime,issend);");
		if(sqlite3_exec(db, sql1, 0, 0, &errMsg))
			debug_error("create table history:%s",
					errMsg);
		if(sqlite3_exec(db, sql, 0, 0, &errMsg))
			debug_error("%s\n%s",errMsg, sql);
	}

}

FxList* fetion_history_get_list(Config* config,
		const char* userid , int count)
{
	sqlite3 *db;
	char sql[4096];
	char path[256];
	char *errMsg;
	char **res;
	int nrows, ncols, start, i;
	FxList *hislist, *pos;
	History *his;

	sprintf(path, "%s/data.db",
				   	config->userPath);

	hislist = fx_list_new(NULL);

	debug_info("Load chat history with %s",
			userid);
	if(sqlite3_open(path, &db)){
		debug_error("open data.db:%s",
					sqlite3_errmsg(db));
		return hislist;
	}

	sprintf(sql, "select * from history"
			" where userid='%s' order"
			" by updatetime desc limit %d;",
			userid, count);

	if(sqlite3_get_table(db, sql, &res,
				&nrows, &ncols, &errMsg)){
		sqlite3_close(db);
		return hislist;
	}

	for(i = 0; i < nrows; i ++){
		start = ncols + i * ncols;
		his = (History*)malloc(sizeof(History));
		memset(his , 0 , sizeof(History));
		strcpy(his->name, res[start]);
		strcpy(his->userid, res[start+1]);
		strcpy(his->message, res[start+2]);
		if(res[start+3])
			strcpy(his->sendtime, res[start+3]);
		his->issend = atoi(res[start+4]);
		unescape_sql(his->message);
		pos = fx_list_new(his);
		fx_list_prepend(hislist , pos);
	}
	return hislist;
}

FxList* fetion_history_get_e_list(Config *config,
		const char *userid , int type)
{
	sqlite3 *db;
	char sql[4096];
	char path[256];
	char condition[256];
	char *errMsg;
	char **res;
	int nrows, ncols, start, i;
	FxList *hislist, *pos;
	History *his;

	sprintf(path, "%s/data.db",
				   	config->userPath);

	hislist = fx_list_new(NULL);

	debug_info("Load chat history with %s",
			userid);
	if(sqlite3_open(path, &db)){
		debug_error("open data.db:%s",
					sqlite3_errmsg(db));
		return hislist;
	}

	switch(type){
		case HISTORY_TODAY:
			sprintf(condition,
				"strftime('%%Y',updatetime) == strftime('%%Y','now') and "
				"strftime('%%m',updatetime) == strftime('%%m','now') and "
				"strftime('%%d',updatetime) == strftime('%%d','now') ");
			break;
		case HISTORY_WEEK:
			sprintf(condition,
				"strftime('%%Y',updatetime) == strftime('%%Y','now') and "
				"strftime('%%W',updatetime) == strftime('%%W','now') ");
			break;
		case HISTORY_MONTH:
			sprintf(condition,
				"strftime('%%Y',updatetime) == strftime('%%Y','now') and "
				"strftime('%%m',updatetime) == strftime('%%m','now') ");
			break;
		case HISTORY_ALL:
			sprintf(condition, "1==1");
			break;
		default:
			break;
	};

	sprintf(sql, "select * from history"
			" where userid='%s' and %s order"
			" by updatetime desc;",
			userid, condition);

	if(sqlite3_get_table(db, sql, &res,
				&nrows, &ncols, &errMsg)){
		sqlite3_close(db);
		return hislist;
	}

	for(i = 0; i < nrows; i ++){
		start = ncols + i * ncols;
		his = (History*)malloc(sizeof(History));
		memset(his , 0 , sizeof(History));
		strcpy(his->name, res[start]);
		strcpy(his->userid, res[start+1]);
		strcpy(his->message, res[start+2]);
		if(res[start+3])
			strcpy(his->sendtime, res[start+3]);
		his->issend = atoi(res[start+4]);
		unescape_sql(his->message);
		pos = fx_list_new(his);
		fx_list_prepend(hislist , pos);
	}
	return hislist;
}

int fetion_history_export(Config *config , const char *myid
		, const char *userid , const char *filename)
{
	sqlite3 *db;
	char sql[4096];
	char text[2048];
	char path[256];
	char *errMsg;
	char **res;
	int nrows, ncols, start, i;
	FILE *f;

	if(!(f = fopen(filename, "w+"))){
		debug_error("export chat history FAILED");
		return -1;
	}

	sprintf(path, "%s/data.db",
				   	config->userPath);

	debug_info("Export chat history with %s",
			userid);
	if(sqlite3_open(path, &db)){
		debug_error("open data.db:%s",
					sqlite3_errmsg(db));
		return -1;
	}

	sprintf(sql, "select * from history"
			" where userid='%s' order"
			" by updatetime desc;",
			userid);

	if(sqlite3_get_table(db, sql, &res,
				&nrows, &ncols, &errMsg)){
		sqlite3_close(db);
		return -1;
	}

	for(i = 0; i < nrows; i ++){
		start = ncols * (i + 1);
		sprintf(text, "%s(%s) %s\n",
				res[start], 
				atoi(res[start+4]) ? myid : res[start+1],
				res[start+3]);
		strcat(text , res[start+2]);
		strcat(text , "\n");
		fwrite(text , strlen(text) , 1 , f);
		fflush(f);
	}
	
	sqlite3_close(db);
	fclose(f);
	return 1;
}

int fetion_history_delete(Config *config,
		const char *userid)
{
	sqlite3 *db;
	char sql[4096];
	char path[256];
	char *errMsg;

	sprintf(path, "%s/data.db",
				   	config->userPath);

	debug_info("Delete chat history with %s",
			userid);
	if(sqlite3_open(path, &db)){
		debug_error("open data.db:%s",
					sqlite3_errmsg(db));
		return -1;
	}
	sprintf(sql, "delete from history where "
			"userid = '%s'", userid);
	if(sqlite3_exec(db, sql, 0, 0, &errMsg)){
		debug_error("delete history with %s failed:%s",
				userid, errMsg);
		sqlite3_close(db);
		return -1;
	}
	sqlite3_close(db);
	return 1;
}
