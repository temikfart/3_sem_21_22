#include "mk_main.h"

Interval ScanInterval() {
  Interval s;
  s.a = 0;
  s.b = 1;

  return s;
}
void *GeneratePoints(void *Args) {
  // Initialize values
  Arg *A = (Arg *)Args;

  // Set seed to random generator
  u_int now = time(NULL) + (u_int)rand();
  srand(now);

  // Generate points into the array Points
  double range = A->Interval.b - A->Interval.a;
  for (int i = 0; i < A->N; i++) {
    // Generating points into the [a,b]^2
    A->Points[i].x = (double)rand() / (RAND_MAX)
            * range + A->Interval.a;               // x in [a,b]
    A->Points[i].y = (double)rand() / (RAND_MAX)
            * A->Interval.b;                       // y in [0,b]
  }
}

int main() {
  Interval Interval = ScanInterval();

  // Generate POINTS_NUM points in array via THREADS_NUM threads
  Point *Points = (Point *)malloc(POINTS_NUM * sizeof(Point));
  pthread_t threads[THREADS_NUM];
  Arg Args[THREADS_NUM];
  // Calc count of needed points from one thread
  long K = (long)(POINTS_NUM / THREADS_NUM);
  for (int i = 0; i < THREADS_NUM; i++) {
    long N;
    if ((i + 1) == THREADS_NUM) {
      N = POINTS_NUM - (i * K);
    } else {
      N = K;
    }

    Args[i].Points = Points + (i * K);
    Args[i].N = N;
    Args[i].Interval = Interval;

    pthread_create(threads + i, NULL,
                   GeneratePoints, (Args + i));
    pthread_join(*(threads + i), NULL);
  }

  for (int i = 0; i < POINTS_NUM; i++) {
    printf("(%2d): (%.2f;%.2f)\n", (i+1), Points[i].x, Points[i].y);
  }

  free(Points);

  return 0;
}
