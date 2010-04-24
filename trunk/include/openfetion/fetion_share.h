#ifndef FETION_SHARE_H
#define FETION_SHARE_H

#define MAX_FILE_SIZE 2097151

typedef struct{

	char guid[64];
	char sessionid[64];
	char filename[64];
	char sipuri[64];
	char md5[64];
	long long filesize;
	char localip[24];
	int localport;
	char remoteip[24];
	int remoteport;
} Share;

extern Share *fetion_share_new(const char *sipuri
		, const char *absolutePath);

extern void fetion_share_request(FetionSip *sip , Share *share);

extern char* fetion_share_compute_md5(const char *absolutePath);

extern long long fetion_share_get_filesize(const char *absolutePath);
#endif
