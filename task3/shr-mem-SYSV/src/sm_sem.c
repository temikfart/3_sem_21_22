#include "sm_sem.h"

static struct sembuf Init = {0, 0, 0};
static struct sembuf P = {0, -1, 0};
static struct sembuf V = {0, 1, 0};

int *MySemOpen(const char *path) {
  int *sem_id = NULL;
  key_t key;
  union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
  } Arg;

  (void)close(open(path, O_WRONLY | O_CREAT, 0));
  key = ftok(path, 1);
  if (key == -1) {
    perror("ftok error");
    exit(1);
  }
  sem_id = malloc(sizeof(int));

  *sem_id = semget(key, 1, IPC_CREAT | 0666);
  if (*sem_id != -1) {
    Arg.val = 0;
    semctl(*sem_id, 0, SETVAL, Arg);
    semop(*sem_id, &Init, 1);
  } else {
    if (errno == EEXIST) {
      while (1) {
        if ((*sem_id = semget(key, 1, IPC_CREAT | 0666)) == -1) {
          if (errno == ENOENT) {
            sleep(1);
            continue;
          } else {
            exit(1);
          }
        } else {
          break;
        }
      }
      while (1) {
        struct semid_ds buf;
        Arg.buf = &buf;
        semctl(*sem_id, 0, IPC_CREAT, Arg);
        if (buf.sem_otime == 0) {
          sleep(1);
          continue;
        } else {
          break;
        }
      }
    } else {
      exit(1);
    }
  }
  return sem_id;
}
int MySemClose(int *sem_id) {
  free(sem_id);
  return 0;
}
int MySemRemove(const char *path) {
  key_t key;
  int sem_id;

  if((key = ftok(path, 1)) == -1) {
    if(errno != ENOENT) {
      exit(1);
    }
  } else {
    if ((sem_id = semget(key, 1, IPC_CREAT | 0666)) == -1) {
      if (errno != ENOENT) {
        exit(1);
      } else {
        semctl(sem_id, 0, IPC_RMID);
      }
    }
  }
  return 0;
}
int MySemPost(const int *sem_id) {
  return semop(*sem_id, &P, 1);
}
int MySemWait(const int *sem_id) {
  return semop(*sem_id, &V, 1);
}
