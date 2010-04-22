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
			strcpy(name , "[在线]");
			break;
		case P_BUSY:
			strcpy(name , "[忙碌]");
			break;
		case P_AWAY:
			strcpy(name , "[离开]");
			break;
		case P_HIDDEN:
			strcpy(name , "[隐身]");
			break;
		case P_OUTFORLUNCH:
			strcpy(name , "[外出就餐]");
			break;
		case P_DONOTDISTURB:
			strcpy(name , "[请勿打扰]");
			break;
		case P_RIGHTBACK:
			strcpy(name , "[马上回来]");
			break;
		case P_MEETING:
			strcpy(name , "[会议中]");
			break;
		case P_ONTHEPHONE:
			strcpy(name , "[电话中]");
			break;
		default:
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

    if ((g_strstr_len(str, -1, ":\"&gt;")) != NULL) {
        str = fx_util_str_replace(str, ":\"&gt;", "#10#");
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
        str = fx_util_str_replace(str, "(})", "#28#");
    }

    if ((g_strstr_len(str, -1, "({)")) != NULL) {
        str = fx_util_str_replace(str, "({)", "#29#");
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

    if ((g_strstr_len(str, -1, "-8")) != NULL) {
        str = fx_util_str_replace(str, "-8", "#35#");
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

    if ((g_strstr_len(str, -1, ":-s")) != NULL) {
        str = fx_util_str_replace(str, ":-s", "#44#");
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
