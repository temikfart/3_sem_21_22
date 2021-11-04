#include "mk_main.h"

static pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;

double function(double x) {
  return sqrt(x);
}
void *GeneratePoints(void *Args) {
  /* Declare values */
  Arg *A = (Arg *)Args;
  double range = A->Interval.b - A->Interval.a;
  Point New_point;
  int s;

  /* Set seed to random generator */
//  u_int now = time(NULL) + (u_int)rand_r();
  u_int id = pthread_self();
  srand(id);

  /* Generate points into the array Points */
  for (int i = 0; i < A->N; i++) {
    /* Generating point into the [a,b]^2 */
//    New_point.x = i;
//    New_point.y = i;
    New_point.x = (double)rand_r(&id) / (RAND_MAX)
                  * range + A->Interval.a;               // x in [a,b]
    New_point.y = (double)rand_r(&id) / (RAND_MAX)
                  * A->Interval.b;                       // y in [0,b]

    /* Does the point (x;y) belong to the area
     * under the graph of the function? */
    if (New_point.y - function(New_point.x) < 0) {
      s++;
    }
  }

  /* =============== CRITICAL SECTION =============== */
  pthread_mutex_lock(&pmutex);

  /* Sending number of points, which placed
   * under the graph via Shared Memory */
  A->shm[0] += 1;
  A->shm[1] += s;

  pthread_mutex_unlock(&pmutex);
  /* ================================================ */

  pthread_exit(NULL);
}
void *getaddr(const char *path, size_t shm_sz) {
  key_t key = ftok(path, 1);
  if (key == -1) {
    perror("ftok error for");
    exit(1);
  }

  int shmid = shmget(key, shm_sz, IPC_CREAT);
  if (shmid == -1) {
    printf("%s %d, %ld", path, key, shm_sz);
    perror(" shmget error");
    exit(1);
  }

  char *shmptr = (char *)shmat(shmid, NULL, 0);
  if ((size_t)shmptr == -1) {
    perror("shmat error");
    exit(1);
  }

  return shmptr;
}
void Send(int *shm, Interval Interval) {
  /* Generate POINTS_NUM points in array via THREADS_NUM threads */
  pthread_t threads[THREADS_NUM];
  Arg Args[THREADS_NUM];

  /* Calc count of needed points for one thread */
  long K = (long)(POINTS_NUM / THREADS_NUM);
  for (int i = 0; i < THREADS_NUM; i++) {
    long N;
    if ((i + 1) == THREADS_NUM) {
      N = POINTS_NUM - (i * K);
    } else {
      N = K;
    }

    Args[i].shm = shm;
    Args[i].N = N;
    Args[i].Interval = Interval;

    pthread_create(threads + i, NULL,
                   GeneratePoints, (Args + i));
  }
  for (int i = 0; i < THREADS_NUM; i++) {
    pthread_join(*(threads + i), NULL);
  }
}
void Receive(int *shm, Interval Interval) {
  /* Declare values */
  double result;

  /* Processing result */
  while(shm[0] != THREADS_NUM); // Wait info about points

  double range = Interval.b - Interval.a;
  result = ((double)shm[1] / POINTS_NUM) * range * range;

  printf("I = %.5f\n", result);
}

int main(int argc, char *argv[]) {
  /* Get interval for integral */
  Interval Interval;
  Interval.a = 0;
  Interval.b = 1; // Wait I = 0,(6) as result

  /* Creating Sheared Memory */
  int *shmptr = (int *)getaddr(argv[0], SHMEM_SZ * sizeof(int));
  shmptr[0] = 0;  // Status
  shmptr[1] = 0;  // Number of points, which
                  // placed under the graph

  /* Making two processes for sending points and processing of results */
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork error");
    exit(1);
  }

  if (pid == 0) {
    Send(shmptr, Interval);
  } else {
    Receive(shmptr, Interval);
  }

  return 0;
}
