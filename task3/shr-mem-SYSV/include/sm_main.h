#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include "sm_sem.h"

/*
 * 1st byte -- letter count
 * 2nd byte -- number of packages
 * 3rd byte -- number of bytes in the last package
 */
#define PROTOCOL_SZ 4
#define PCG_SZ 128
#define SHM_SZ (6 * PCG_SZ + PROTOCOL_SZ)
#define BUF_SZ (SHM_SZ - PROTOCOL_SZ)

void *getaddr(const char *path, size_t shm_sz);
void send(char *argv[]);
void receive(char *argv[]);
