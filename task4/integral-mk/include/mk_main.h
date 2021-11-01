#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define POINTS_NUM 10

typedef struct Point {
  double x;
  double y;
} Point;

typedef struct Interval {
  double a;
  double b;
} Interval;

Interval ScanInterval();
void GeneratePoints(Point *Points, Interval Interval);
//void SendViaShMem();
