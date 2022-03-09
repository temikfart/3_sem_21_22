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
#include <pthread.h>
#include <sys/epoll.h>

#include "thpool.h"
#include "s_interact.h"

#define MAX_CLIENT 64
#define ACTIVE 1
#define INACTIVE 0

#define CMD_SZ 1024
#define BUF_SZ 1024
#define MAX_PATH 256
#define S_FIFO_REL "../channels/"
#define S_FIFO_REG "server_reg"     // Registration fifo
#define S_FIFO_RSP "server_rsp"     // Response fifo

#define DEFAULT_RSP "ASK\n"

#define THREAD_AMOUNT 20
#define MAX_EVENTS MAX_CLIENT

typedef struct Client {
  char fifo_tx[MAX_PATH];  // Path to transmitter fifo
  char fifo_rx[MAX_PATH];  // Path to receiver fifo
  int is_active;  // 0 -- no active; 1 -- active.
  struct epoll_event event;
} Client;
typedef struct Threads_info {
  int is_busy;
  int tx_fd;
  int rx_fd;

} Thread_Data;
typedef struct Arguments {
  struct epoll_event event;
} Arg;

static Client Clients[MAX_CLIENT];
static int client_max;

static threadpool ThPool;

static int s_fifo_reg_fd_r;
static int s_fifo_rsp_fd_w;

static int s_fifo_reg_fd_w; // Additional
static int s_fifo_rsp_fd_r; // Additional

static int epoll_fd;

int s_prepare();
int s_register_clients();

int s_check_reg_request(char* request);
void s_register(void* A);

void* s_epoll(void* Arguments);
void s_processing_request(void* Arguments);
