
#ifndef FX_SERVER_H
#define FX_SERVER_H

#include <fx_include.h>

struct fifo_mesg {
	unsigned short type;
	unsigned short length;
	unsigned long  pid;
};

struct fifo_resp {
	unsigned short code;
	unsigned short length;
};

#define CLI_SEND_MESSAGE 1

#define OPENFETION_FIFO_FILE "/tmp/openfetion_fifo_%s"
#define OPENFETION_CLIENT_FIFO "/tmp/openfetion_fifo_%ld"
#define FIFO_FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define BUFLEN 4096
#define FIFO_HEAD_SIZE (sizeof(struct fifo_mesg))
#define RESP_HEAD_SIZE (sizeof(struct fifo_resp))

int init_server(FxMain *fxmain);

int start_server(FxMain *fxmain, int fifo);

#endif
