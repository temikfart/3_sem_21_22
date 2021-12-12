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

#include "cn_interact.h"

#define CMD_SZ 1024

#define MAX_PATH 256
#define S_FIFO_REG "server_reg"     // Registration fifo
#define S_FIFO_RSP "server_rsp"     // Response fifo

#define DEFAULT_RSP "ASK\n"

static int s_fifo_reg_fd;
static int s_fifo_rsp_fd;

int cn_reg();
int cn_work();
