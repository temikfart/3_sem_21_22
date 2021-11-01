#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define POINTS_NUM 10
#define THREADS_NUM 4

typedef struct Point {
  double x;
  double y;
} Point;

typedef struct Interval {
  double a;
  double b;
} Interval;

typedef struct pthread_args {
  Point *Points;
  long N;
  Interval Interval;
} Arg;

Interval ScanInterval();
void *GeneratePoints(void *Args);
//void SendViaShMem();
