#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>

#define MAX_PATH_LEN 256
#define FIFO_STORAGE_REL_PATH "../../server/channels/"
#define PUBLIC_SERVER_FIFO_NAME "server_pub"
