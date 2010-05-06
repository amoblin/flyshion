#ifndef FETION_SHARE_H
#define FETION_SHARE_H

#define MAX_FILE_SIZE 2097151

extern Share *fetion_share_new_with_path(const char *sipuri
		, const char *absolutePath);

extern Share *fetion_share_new(const char *sipuri);

extern void fetion_share_request(FetionSip *sip , Share *share);

extern char* fetion_share_compute_md5(const char *absolutePath);

extern long long fetion_share_get_filesize(const char *absolutePath);

extern void fetion_share_response_accept(FetionSip *sip , Share *share);

#endif
