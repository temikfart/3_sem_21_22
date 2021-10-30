#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define SERVER_FIFO_NAME "../channels/fifo_server"

struct simple_message {
  pid_t sm_clientpid;
  char sm_data[200];
};
