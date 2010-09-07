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

#include "fx_include.h"

char* fx_util_get_state_name(StateType type)
{
	char *name = (char*)malloc(48);
	memset(name , 0 , 48);
	switch(type)
	{
		case P_ONLINE:
			strcpy(name , _("[Online]"));
			break;
		case P_BUSY:
			strcpy(name , _("[Busy]"));
			break;
		case P_AWAY:
			strcpy(name , _("[Leave]"));
			break;
		case P_HIDDEN:
			strcpy(name , _("[Hide]"));
			break;
		case P_OUTFORLUNCH:
			strcpy(name , _("[Eating out]"));
			break;
		case P_DONOTDISTURB:
			strcpy(name , _("[Do Not Disturb]"));
			break;
		case P_RIGHTBACK:
			strcpy(name , _("[Be Back Soon]"));
			break;
		case P_MEETING:
			strcpy(name , _("[Meeting]"));
			break;
		case P_ONTHEPHONE:
			strcpy(name , _("[Calling]"));
			break;
		default:
			if(type > 400 && type < 500)
				strcpy(name , _("[Online]"));
			else
				strcpy(name , _("[Unknown Statement]"));
			break;
	}
	return name;
}
void fx_util_popup_warning(FxMain* fxmain , const char* message)
{
	GtkWidget *dialog = NULL;
	dialog = gtk_message_dialog_new(GTK_WINDOW(fxmain->window),
									GTK_DIALOG_DESTROY_WITH_PARENT,
									GTK_MESSAGE_WARNING,
									GTK_BUTTONS_OK,
									message);
	gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

char *fx_util_str_replace(char *str, const char *oldstr, const char *newstr)
{
    gchar *out = NULL;
    gchar **tokens = NULL;

    tokens = g_strsplit(str , oldstr , -1);
    out = g_strjoinv(newstr , tokens);

    g_strfreev(tokens);
    return out;
}

char *fx_util_replace_emotion(char *str)
{

    if ((g_strstr_len(str, -1, "&gt;:)")) != NULL) {
        str = fx_util_str_replace(str, "&gt;:)", "#50#");
    }

    if ((g_strstr_len(str, -1, "*-:)")) != NULL) {
        str = fx_util_str_replace(str, "*-:)", "#37#");
    }

	if ((g_strstr_len(str, -1, ":)")) != NULL) {
        str = fx_util_str_replace(str, ":)", "#1#");
    }

    if ((g_strstr_len(str, -1, ":-d")) != NULL) {
        str = fx_util_str_replace(str, ":-d", "#2#");
    }

    if ((g_strstr_len(str, -1, ";)")) != NULL) {
        str = fx_util_str_replace(str, ";)", "#3#");
    }

    if ((g_strstr_len(str, -1, ":-o")) != NULL) {
        str = fx_util_str_replace(str, ":-o", "#4#");
    }

    if ((g_strstr_len(str, -1, ":-p")) != NULL) {
        str = fx_util_str_replace(str, ":-p", "#5#");
    }

    if ((g_strstr_len(str, -1, "(h)")) != NULL) {
        str = fx_util_str_replace(str, "(h)", "#6#");
    }

    if ((g_strstr_len(str, -1, ":-@")) != NULL) {
        str = fx_util_str_replace(str, ":-@", "#7#");
    }

    if ((g_strstr_len(str, -1, ":(")) != NULL) {
        str = fx_util_str_replace(str, ":(", "#8#");
    }

    if ((g_strstr_len(str, -1, ":'(")) != NULL) {
        str = fx_util_str_replace(str, ":'(", "#9#");
    }

    if ((g_strstr_len(str, -1, ":\">")) != NULL) {
        str = fx_util_str_replace(str, ":\">", "#10#");
    }

    if ((g_strstr_len(str, -1, "^o)")) != NULL) {
        str = fx_util_str_replace(str, "^o)", "#11#");
    }

    if ((g_strstr_len(str, -1, ":&")) != NULL) {
        str = fx_util_str_replace(str, ":&", "#12#");
    }

    if ((g_strstr_len(str, -1, "8o|")) != NULL) {
        str = fx_util_str_replace(str, "8o|", "#13#");
    }

    if ((g_strstr_len(str, -1, "|-)")) != NULL) {
        str = fx_util_str_replace(str, "|-)", "#14#");
    }

    if ((g_strstr_len(str, -1, ":-#")) != NULL) {
        str = fx_util_str_replace(str, ":-#", "#15#");
    }

    if ((g_strstr_len(str, -1, "8-)")) != NULL) {
        str = fx_util_str_replace(str, "8-)", "#16#");
    }

    if ((g_strstr_len(str, -1, "(s)")) != NULL) {
        str = fx_util_str_replace(str, "(s)", "#17#");
    }

    if ((g_strstr_len(str, -1, "(st)")) != NULL) {
        str = fx_util_str_replace(str, "(st)", "#18#");
    }

    if ((g_strstr_len(str, -1, "(o)")) != NULL) {
        str = fx_util_str_replace(str, "(o)", "#19#");
    }

    if ((g_strstr_len(str, -1, "(l)")) != NULL) {
        str = fx_util_str_replace(str, "(l)", "#20#");
    }

    if ((g_strstr_len(str, -1, "(u)")) != NULL) {
        str = fx_util_str_replace(str, "(u)", "#21#");
    }

    if ((g_strstr_len(str, -1, "(@)")) != NULL) {
        str = fx_util_str_replace(str, "(@)", "#22#");
    }

    if ((g_strstr_len(str, -1, "(&)")) != NULL) {
        str = fx_util_str_replace(str, "(&)", "#23#");
    }

    if ((g_strstr_len(str, -1, "(sn)")) != NULL) {
        str = fx_util_str_replace(str, "(sn)", "#24#");
    }

    if ((g_strstr_len(str, -1, "(*)")) != NULL) {
        str = fx_util_str_replace(str, "(*)", "#25#");
    }

    if ((g_strstr_len(str, -1, "(#)")) != NULL) {
        str = fx_util_str_replace(str, "(#)", "#26#");
    }

    if ((g_strstr_len(str, -1, "(r)")) != NULL) {
        str = fx_util_str_replace(str, "(r)", "#27#");
    }

    if ((g_strstr_len(str, -1, "(})")) != NULL) {
        str = fx_util_str_replace(str, "(})", "#29#");
    }

    if ((g_strstr_len(str, -1, "({)")) != NULL) {
        str = fx_util_str_replace(str, "({)", "#28#");
    }

    if ((g_strstr_len(str, -1, "(k)")) != NULL) {
        str = fx_util_str_replace(str, "(k)", "#30#");
    }

    if ((g_strstr_len(str, -1, "(f)")) != NULL) {
        str = fx_util_str_replace(str, "(f)", "#31#");
    }

    if ((g_strstr_len(str, -1, "(w)")) != NULL) {
        str = fx_util_str_replace(str, "(w)", "#32#");
    }

    if ((g_strstr_len(str, -1, "(g)")) != NULL) {
        str = fx_util_str_replace(str, "(g)", "#33#");
    }

    if ((g_strstr_len(str, -1, "(^)")) != NULL) {
        str = fx_util_str_replace(str, "(^)", "#34#");
    }

    if ((g_strstr_len(str, -1, "(8)")) != NULL) {
        str = fx_util_str_replace(str, "(8)", "#35#");
    }

    if ((g_strstr_len(str, -1, "(i)")) != NULL) {
        str = fx_util_str_replace(str, "(i)", "#36#");
    }

    if ((g_strstr_len(str, -1, "(c)")) != NULL) {
        str = fx_util_str_replace(str, "(c)", "#38#");
    }

    if ((g_strstr_len(str, -1, "(um)")) != NULL) {
        str = fx_util_str_replace(str, "(um)", "#39#");
    }

    if ((g_strstr_len(str, -1, "(mp)")) != NULL) {
        str = fx_util_str_replace(str, "(mp)", "#40#");
    }

    if ((g_strstr_len(str, -1, "(co)")) != NULL) {
        str = fx_util_str_replace(str, "(co)", "#41#");
    }

    if ((g_strstr_len(str, -1, ":-|")) != NULL) {
        str = fx_util_str_replace(str, ":-|", "#42#");
    }

    if ((g_strstr_len(str, -1, ":-/")) != NULL) {
        str = fx_util_str_replace(str, ":-/", "#43#");
    }

    if ((g_strstr_len(str, -1, ":s")) != NULL) {
        str = fx_util_str_replace(str, ":s", "#44#");
    }

    if ((g_strstr_len(str, -1, ")-|")) != NULL) {
        str = fx_util_str_replace(str, ")-|", "#45#");
    }

    if ((g_strstr_len(str, -1, "(d)")) != NULL) {
        str = fx_util_str_replace(str, "(d)", "#46#");
    }

    if ((g_strstr_len(str, -1, "O:)")) != NULL) {
        str = fx_util_str_replace(str, "O:)", "#47#");
    }

    if ((g_strstr_len(str, -1, ":-?")) != NULL) {
        str = fx_util_str_replace(str, ":-?", "#48#");
    }

    if ((g_strstr_len(str, -1, "(y)")) != NULL) {
        str = fx_util_str_replace(str, "(y)", "#49#");
    }

    if ((g_strstr_len(str, -1, ":-b")) != NULL) {
        str = fx_util_str_replace(str, ":-b", "#51#");
    }

    if ((g_strstr_len(str, -1, "b)")) != NULL) {
        str = fx_util_str_replace(str, "b)", "#52#");
    }
	return str;
}

void escape_impression(char *impre)
{
	int i;
	for(i = 0 ; i < strlen(impre); i++){
		if(impre[i] == 10 || impre[i] == 13)
			impre[i] = 32;
	}
}

static gchar getpychar(guchar uword0 , guchar uword1)
{
	gchar pychar;

	int i1 = (short)(uword0 - '\0');
	int i2 = (short)(uword1 - '\0');

	int tmp = i1 * 256 + i2;

	if(tmp >= 45217 && tmp <= 45252) pychar = 'A'; 
	else if(tmp >= 45253 && tmp <= 45760) pychar = 'B'; 
	else if(tmp >= 45761 && tmp <= 46317) pychar = 'C'; 
	else if(tmp >= 46318 && tmp <= 46825) pychar = 'D'; 
	else if(tmp >= 46826 && tmp <= 47009) pychar = 'E'; 
	else if(tmp >= 47010 && tmp <= 47296) pychar = 'F'; 
	else if(tmp >= 47297 && tmp <= 47613) pychar = 'G'; 
	else if(tmp >= 47614 && tmp <= 48118) pychar = 'H'; 
	else if(tmp >= 48119 && tmp <= 49061) pychar = 'J'; 
	else if(tmp >= 49062 && tmp <= 49323) pychar = 'K'; 
	else if(tmp >= 49324 && tmp <= 49895) pychar = 'L'; 
	else if(tmp >= 49896 && tmp <= 50370) pychar = 'M'; 
	else if(tmp >= 50371 && tmp <= 50613) pychar = 'N'; 
	else if(tmp >= 50614 && tmp <= 50621) pychar = 'O'; 
	else if(tmp >= 50622 && tmp <= 50905) pychar = 'P'; 
	else if(tmp >= 50906 && tmp <= 51386) pychar = 'Q'; 
	else if(tmp >= 51387 && tmp <= 51445) pychar = 'R'; 
	else if(tmp >= 51446 && tmp <= 52217) pychar = 'S'; 
	else if(tmp >= 52218 && tmp <= 52697) pychar = 'T'; 
	else if(tmp >= 52698 && tmp <= 52979) pychar = 'W'; 
	else if(tmp >= 52980 && tmp <= 53640) pychar = 'X'; 
	else if(tmp >= 53689 && tmp <= 54480) pychar = 'Y'; 
	else if(tmp >= 54481 && tmp <= 55289) pychar = 'Z'; 
	else pychar = ' ';

	return pychar;
}

gchar *get_pystring(const gchar *in)
{
	gsize inlen , olen , i , j = 0;
	gchar *gword = g_convert(in , strlen(in)
			, "gb18030" , "utf8" , &inlen , &olen , NULL);

	guchar *uword = (guchar*)gword;
	gchar *out = (gchar*)malloc(olen);
	
	memset(out , 0 , olen);

	for(i = 0 ; i < olen ; i++){
		if(uword[i] >= 0xa1){
			if(uword[i] != 0xa3){
				out[j++] = getpychar(uword[i] , uword[i + 1]);
				i ++;
			}
		}else{
			out[j++] = toupper((gchar)uword[i]);
		}
	}

	return out;

}

gboolean has_gb(const gchar *in)
{
	gsize inlen , olen , i;
	gchar *gword = g_convert(in , strlen(in)
			, "gb18030" , "utf8" , &inlen , &olen , NULL);

	guchar *uword = (guchar*)gword;

	for(i = 0 ; i < olen ; i++)
		if(uword[i] >= 0xa1)
			return TRUE;

	return FALSE;
}
