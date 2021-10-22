#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "sm_main.h"


#define SHM_SZ 1024

int *getaddr(const char *path) {
  key_t key = ftok(path, 1);
  if (key == -1) {
    perror("ftok error");
    exit(1);
  }

  int shmid = shmget(key, SHM_SZ, IPC_CREAT);
  if (shmid == -1) {
    perror("shmget error");
    exit(1);
  }

  int *shmptr = (int *)shmat(shmid, NULL, 0);
  if ((size_t)shmptr == -1) {
    perror("shmat error");
    exit(1);
  }

  return shmptr;
}

int main(int argc, char *argv[]) {
  if (argc == 1 || argc > 2) {
    printf("Expected one argument with the path to file.\n");
    exit(1);
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork error");
    exit(1);
  }

  if(pid == 0) {
    // Child
    int *shmptr = getaddr(argv[1]);

    int a = 0;
    while (*shmptr != 99) {
      if (a != *shmptr) {
        printf("Потомок обнаружил число %d\n", *shmptr);
        a = *shmptr;
      }
    }
    printf("Потомок завершил работу\n");
  } else {
    // Parent
    int *shmptr = getaddr(argv[1]);

    for (*shmptr = 1; *shmptr < 4; (*shmptr)++) {
      sleep(1);
    }
    *shmptr = 99;
  }

  return 0;
}
