#include "mk_main.h"

Interval ScanInterval() {
  Interval s;
  s.a = 0;
  s.b = 1;

  return s;
}
void GeneratePoints(Point *Points, Interval Interval) {
  // Set seed to random generator
  u_int now = time(NULL);
  srand(now);

  // Generate points into the array Points
  double range = Interval.b - Interval.a;
  for (int i = 0; i < POINTS_NUM; i++) {
    // Generating points into the [a,b]^2
    Points[i].x = (double)rand() / (RAND_MAX)
            * range + Interval.a;               // x in [a,b]
    Points[i].y = (double)rand() / (RAND_MAX)
            * Interval.b;                       // y in [0,b]
  }
}

int main() {
  Interval Interval = ScanInterval();

  // Generate POINTS_NUM points in array
  Point *Points = (Point *)malloc(POINTS_NUM * sizeof(Point));
  GeneratePoints(Points, Interval);

  for (int i = 0; i < POINTS_NUM; i++) {
    printf("(%2d): (%.2f;%.2f)\n", (i+1), Points[i].x, Points[i].y);
  }

  free(Points);

  return 0;
}
