#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define POINTS_NUM 100000000
#define THREADS_NUM 100
#define SHMEM_SZ 2

static pthread_mutex_t pmutex;

typedef struct Point {
  double x;
  double y;
} Point;

typedef struct Interval {
  double a;
  double b;
} Interval;

typedef struct pthread_args {
  int *shm;
  long N;
  Interval Interval;
} Arg;

double function(double x);
void *GeneratePoints(void *Args);
void *getaddr(const char *path, size_t shm_sz);
void Send(int *shm, Interval Interval);
void Receive(int *shm, Interval Interval);
