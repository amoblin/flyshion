#include <fx_server.h>
#include <openfetion.h>

#define NOLEN 128
#define MOBILENO_LEN 11

int cli_to_myself = 0;
int cli_to_other  = 0;
int cli_to_phone  = 0;
int cli_get_info  = 0;
char from_no[NOLEN];
char to_no[NOLEN];
char msg_body[BUFLEN];

static char *generate_body();
static char* convert_xml(xmlChar* in);
static int usage()
{
  fprintf(stderr, "%s		Copyright: levin108\n", FETION_NAME);
  fprintf(stderr, "\nWARNING: THERE IS NO WARRANTY FOR Openfetion. USE AT YOUR OWN RISK!!!\n");
  fprintf(stderr, "\nGeneric Options:\n");
  fprintf(stderr, "-h/-H\t\tShow this message.\n");
  fprintf(stderr, "-f from_number\tspecify which number you will use to send a message.\n");
  fprintf(stderr, "-t to_nomber\tspecify which number you will send a message to.\n");
  fprintf(stderr, "-d msg_body\t\tmessage you will send\n");
  fprintf(stderr, "-m\t\tsend a message to myself\n");
  fprintf(stderr, "-p\t\tsend a message to the phone\n");
  fprintf(stderr, "-g mobile_no\tget information of mobile_no\n");
  fprintf(stderr, "\neg.\n");
  fprintf(stderr, "send \"hello world\" to 18788888888:\n");
  fprintf(stderr, "openfetion -f 15288888888 -t 1878888888 -d \"hello world\"\n\n");
  fprintf(stderr, "send \"hello world\" to yourself:\n");
  fprintf(stderr, "openfetion -m -f 15288888888 -d \"hello world\"\n\n");
  fprintf(stderr, "get information of 18788888888:\n");
  fprintf(stderr, "openfetion -f 15288888888 -g 1878888888\n");
  fprintf(stderr, "\nhome page:\t http://code.google.com/p/ofetion/\n");
  fprintf(stderr, "bug report:\t http://code.google.com/p/ofetion/issues/list\n");
  fprintf(stderr, "author's home page: http://basiccoder.com\n\n");
  return 1;
}

int fx_cli_opt(int argc, char **argv)
{
	char ch;
	memset(from_no, 0, sizeof(from_no));
	memset(to_no, 0, sizeof(to_no));

	while((ch = getopt(argc, argv, "d:f:g:hHmpt:")) != -1) {
		switch(ch) {
			case 'd':
				strcpy(msg_body, optarg);
				break;
			case 'f':
				strcpy(from_no, optarg);
				break;
			case 'g':
				strcpy(to_no, optarg);
				cli_get_info = 1;
				break;
			case 'h':case 'H':
				return usage();
			case 'm':
				cli_to_myself = 1;
				break;
			case 'p':
				cli_to_phone = 1;
				break;
			case 't':
				cli_to_other = 1;
				strcpy(to_no, optarg);
				break;
			default:
				break;
		}
	}
	return 0;
}

int fx_cli_exec()
{
	int               serv_fifo;
	int               cli_fifo;
	int               n;
	char             *xml_body;
	struct fifo_resp  resp;
	char              client_fifo_file[BUFLEN];
	char              server_fifo_file[BUFLEN];
	char              buf[BUFLEN];
	struct fifo_mesg *mesg;

	if(strlen(from_no) != 11)
		return usage();

	if(cli_to_myself && *msg_body == '\0')
		return usage();

	if(cli_to_other && *msg_body == '\0')
		return usage();

	if(!cli_to_other && !cli_to_myself)
		return usage();

	if((cli_to_myself || cli_to_phone) && cli_get_info) {
		debug_error("couldn't send a message when getting information");
		return 1;
	}

	if(to_no[0] != '\0' && strlen(to_no) != 11) {
		debug_error("mobile number error");
		return 1;
	}

	snprintf(client_fifo_file, sizeof(client_fifo_file) - 1, OPENFETION_CLIENT_FIFO, getpid());
	snprintf(server_fifo_file, sizeof(server_fifo_file) - 1, OPENFETION_FIFO_FILE, from_no);

	if(mkfifo(client_fifo_file, FIFO_FILE_MODE) == -1) {
		debug_error("create fifo %s:%s", client_fifo_file, strerror(errno));
		return 1;
	}

	if((serv_fifo = open(server_fifo_file, O_WRONLY, 0)) == -1) {
		debug_error("open fifo %s:%s", client_fifo_file, server_fifo_file);
		unlink(client_fifo_file);
		return 1;
	}
	
	mesg = (struct fifo_mesg*)buf;
	xml_body = generate_body();
	if(cli_get_info)
		mesg->type = CLI_GET_INFORMATION;
	else
		mesg->type = CLI_SEND_MESSAGE;
	mesg->length = strlen(xml_body);
	mesg->pid = getpid();
	memcpy(buf + sizeof(struct fifo_mesg), xml_body, strlen(xml_body));
	free(xml_body);

	n = write(serv_fifo, buf, mesg->length + FIFO_HEAD_SIZE);
	if(n != mesg->length + FIFO_HEAD_SIZE) {
		debug_error("write %d bytes,but %d bytes expected", n, mesg->length + FIFO_HEAD_SIZE);
		unlink(client_fifo_file);
		close(serv_fifo);
		return 1;
	}

	if((cli_fifo = open(client_fifo_file, O_RDONLY, 0)) == -1) {
		debug_error("open fifo %s:%s\n", client_fifo_file, strerror(errno));
		unlink(client_fifo_file);
		close(serv_fifo);
		return 1;
	}

	n = read(cli_fifo, &resp, RESP_HEAD_SIZE);
	if(n != RESP_HEAD_SIZE) {
		debug_error("read %d bytes,but %d bytes expected", n, RESP_HEAD_SIZE);
		unlink(client_fifo_file);
		close(serv_fifo);
		close(cli_fifo);
		return 1;
	}

	memset(buf, 0, sizeof(buf));
	n = read(cli_fifo, buf, resp.length);
	if(n != resp.length) {
		unlink(client_fifo_file);
		debug_error("read %d bytes,but %d bytes expected", n, resp.length);
		close(serv_fifo);
		close(cli_fifo);
		return 1;
	}

	unlink(client_fifo_file);
	close(serv_fifo);
	close(cli_fifo);

	if(resp.code == CLI_EXEC_OK){
		printf("[\e[32m\e[1mOK\e[0m] %s\n", buf);
		return 0;
	}else{
		printf("[\e[31m\e[1mFAIL\e[0m] %s\n", buf);
		return 1;
	}


	return 0;
}

static char *generate_body()
{
	const char *xml = "<r></r>";
	xmlChar    *res;
	xmlDocPtr   doc;
	xmlNodePtr  node;

	doc = xmlParseMemory(xml, strlen(xml));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node, NULL, BAD_CAST "m", NULL);
	xmlNewProp(node, BAD_CAST "bd", BAD_CAST msg_body);
	if(cli_to_myself) 
		xmlNewProp(node, BAD_CAST "no", BAD_CAST from_no);
	else
		xmlNewProp(node, BAD_CAST "no", BAD_CAST to_no);

	if(cli_to_phone)
		xmlNewProp(node, BAD_CAST "p", BAD_CAST "1");
	else
		xmlNewProp(node, BAD_CAST "p", BAD_CAST "0");
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return convert_xml(res);
}	

static char* convert_xml(xmlChar* in)
{
	char *res , *pos ;
	pos = strstr((char*)in , "?>") + 2;
	res = (char*)malloc(strlen(pos) + 1);
	memset(res , 0 , strlen(pos) + 1);
	memcpy(res , pos , strlen(pos));
	xmlFree(in);
	return res;
}
