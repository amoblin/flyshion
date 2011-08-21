#include "fetion_log.h"

int fetion_log_init(char *file_path)
{
    int logfd = open(file_path, O_RDWR | O_CREAT | O_APPEND, 0644);
    if (logfd == -1) {
        return -1;
    }
    close(STDERR_FILENO);
    dup2(logfd, STDERR_FILENO);
    close(logfd);
    openlog(NULL, LOG_PERROR, LOG_DAEMON);
    return 0;
}
