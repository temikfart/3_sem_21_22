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

#define MAX_CLIENT 64
#define ACTIVE 1
#define INACTIVE 0

#define MAX_PATH 256
#define S_FIFO_REL "../channels/"
#define S_FIFO_REG "server_reg"     // Registration fifo
#define S_FIFO_RSP "server_rsp"     // Response fifo

typedef struct Client {
  char fifo_tx[MAX_PATH];  // Path to transmitter fifo
  char fifo_rx[MAX_PATH];  // Path to receiver fifo
  int is_active;  // 0 -- no active; 1 -- active.
} Client;

static Client Clients[MAX_CLIENT];
static int client_max;
static int s_fifo_reg_fd;
static int s_fifo_rsp_fd;

int s_mkfifo(const char* fifo_name);
int s_prepare();
int s_reg_clients();
int s_check_reg();
void s_draft(void* A);
