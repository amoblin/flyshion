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
