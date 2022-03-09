#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define MAX_PATH 256
#define S_FIFO_REL "../channels/"

#define MAX_REQUEST (2 * MAX_PATH)

int s_mkfifo(const char* fifo_name);
int s_open_fifo(const char* fifo_name, int flags);
int s_unlink_fifo(const char* fifo_name);

char* s_scan_request(int fd);