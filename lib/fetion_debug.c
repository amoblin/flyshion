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

struct tm* get_currenttime()
{
	time_t t;
	time(&t);
	return localtime(&t);
}
void debug_info(const char* format , ...)
{
	char t_str[32] = { 0 };
	char fmt[4096] = { 0 };
	va_list ap;
	struct tm* t = get_currenttime();
	strftime(t_str , sizeof(t_str) , "%T" , t );
	sprintf(fmt , "[\e[32m\e[1m%s\e[0m]  %s\n" , t_str , format);
	va_start(ap, format);
	vfprintf(stdout , fmt , ap);
	va_end(ap);
}
void debug_error(const char* format , ...)
{
	char t_str[32] = { 0 };
	char fmt[4096] = { 0 };
	va_list ap;
	struct tm* t = get_currenttime();
	strftime(t_str , sizeof(t_str) , "%T" , t );
	sprintf(fmt , "[%s] ***ERROR*** %s\n" , t_str , format);
	va_start(ap, format);
	vfprintf(stdout , fmt , ap);
	va_end(ap);
}

