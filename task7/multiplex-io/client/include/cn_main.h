#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>

#define MAX_PATH 256
#define S_FIFO_REL "../../server/channels/"
#define S_FIFO_REG "server_reg"     // Registration fifo
#define S_FIFO_RSP "server_rsp"     // Response fifo

char* cn_mkfifo_name(pid_t pid, const char* suf);
int cn_open_s_fifo(const char* fifo_name, int flags);
int cn_close_s_fifo(int fd);
int cn_reg(char* fifo_tx, char* fifo_rx);
