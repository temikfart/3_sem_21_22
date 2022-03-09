#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>

#define MAX_PATH 256
#define CN_FIFO_REL "../../server/channels/"

#define MAX_REQUEST (2 * MAX_PATH)

char* cn_mkfifo_path(pid_t pid, const char* suf);
int cn_open_fifo(const char* fifo_name, int flags);
int cn_close_fifo(int fd);

char* cn_scan_request();
