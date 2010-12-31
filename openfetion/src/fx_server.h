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
 *   51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA.            *
 *                                                                         *
 *   OpenSSL linking exception                                             *
 *   --------------------------                                            *
 *   If you modify this Program, or any covered work, by linking or        *
 *   combining it with the OpenSSL project's "OpenSSL" library (or a       *
 *   modified version of that library), containing parts covered by        *
 *   the terms of OpenSSL/SSLeay license, the licensors of this            *
 *   Program grant you additional permission to convey the resulting       *
 *   work. Corresponding Source for a non-source form of such a            *
 *   combination shall include the source code for the parts of the        *
 *   OpenSSL library used as well as that of the covered work.             *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fx_include.h>

#ifndef FX_SERVER_H
#define FX_SERVER_H


struct fifo_mesg {
	unsigned short type;
	unsigned short length;
	unsigned int  pid;
};

struct fifo_resp {
	unsigned short code;
	unsigned short length;
};

#define CLI_SEND_MESSAGE    1
#define CLI_GET_INFORMATION 2

#define OPENFETION_FIFO_FILE "/tmp/openfetion_fifo_%s"
#define OPENFETION_CLIENT_FIFO "/tmp/openfetion_fifo_%d"
#define FIFO_FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define CLI_EXEC_OK   200
#define CLI_EXEC_FAIL 400

#define BUFLEN 4096
#define FIFO_HEAD_SIZE (sizeof(struct fifo_mesg))
#define RESP_HEAD_SIZE (sizeof(struct fifo_resp))

int init_server(FxMain *fxmain);

int start_server(FxMain *fxmain, int fifo);

int fx_cli_opt(int argc, char **argv);

int fx_cli_exec();
#endif
