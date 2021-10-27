#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include "sm_main.h"


#define PCG_SZ 128
#define SHM_SZ (8*PCG_SZ)

char *getaddr(const char *path) {
  key_t key = ftok(path, 1);
  if (key == -1) {
    perror("ftok error for");
    exit(1);
  }

  int shmid = shmget(key, SHM_SZ, IPC_CREAT);
  if (shmid == -1) {
    perror("shmget error");
    exit(1);
  }

  char *shmptr = (char *)shmat(shmid, NULL, 0);
  if ((size_t)shmptr == -1) {
    perror("shmat error");
    exit(1);
  }

  return shmptr;
}
void send(char *argv[]) {
  char *shmptr = getaddr(argv[1]);

  size_t shm_sz = SHM_SZ / sizeof(char);

  int fd_in = open(argv[1], O_RDONLY);
  if (fd_in == -1) {
    perror("open fd_in error");
    exit(1);
  }

  /* SHM_SZ = 1024 = 8 * 128 + 3, пусть PCG_SZ = 128
   * Тогда
   *    Первый байт -- номер пачки пакетов;
   *    Второй байт -- количество пакетов;
   *    Третий байт -- количество байтов
   *                   в последнем пакете. */
  int i = 1;
  size_t r_bytes = shm_sz - 2;
  while(r_bytes == shm_sz - 2) {
    // Открыть семафор + pthread_cond_signal() +++ pthread_cond_wait()
    int sem_id = semget(ftok(argv[1], 1),
                     1,
                     O_CREAT | O_RDWR);

    // Подготовка к подсчету пакетов
    shmptr[0] = (char)(i++);
    shmptr[1] = '0';
    shmptr[2] = '0';
    i %= 128;

    // Считывание
    r_bytes = read(fd_in, shmptr+3, shm_sz - 2);
    printf("(C) r_bytes = %ld; shmptr = %s\n", r_bytes, shmptr+3);

    // Считаем пакеты
    if (r_bytes == 0) {
      // Ничего не считано
      shmptr[0] = '\0';
    } else if (r_bytes < PCG_SZ) {
      // Считано меньше 1 пакета
      shmptr[1] = 1;
      shmptr[2] = (char)r_bytes;
    } else {
      // Считано больше 1 пакета
      shmptr[1] = (char)(r_bytes / PCG_SZ + 1);
      shmptr[2] = (char)(r_bytes % PCG_SZ + 1); // При обратном считывании: -1.
    }
    printf("(C) shmptr[1] = %d; [2] = %d; [3] = %d\n",
           shmptr[0], shmptr[1], shmptr[2]);
    // Закрыть семафор
  }
  shmptr[0] = '\0';

  close(fd_in);
}
void receive(char *shmptr, char *argv[]) {
  int fd_out = open(argv[2], O_WRONLY);
  if (fd_out == -1) {
    perror("open fd_out error");
    exit(1);
  }

  printf("(P) ");
  int j, i = 1;
  int r_bytes;
  int n_pcg;
  while(shmptr[0] > 0) {
    j = (int)shmptr[0];
//      printf("(P) j = %d; ", j);
    if (j == i) {
      i++;
      i %= 128;
      sleep(1);
      n_pcg = shmptr[1];
      if (n_pcg == 1) {
        r_bytes = shmptr[2];
      } else {
        r_bytes = (int) (n_pcg * PCG_SZ + shmptr[2] - 1);
      }
      printf("n_pcg = %d; last = %d\n",
             n_pcg, shmptr[2]);
      printf("(P) shmptr: [0] = %d; [1] = %d; [2] = %d\n",
             shmptr[0], shmptr[1], shmptr[2]);
      write(fd_out, shmptr+3, r_bytes);
      printf("(P) r_bytes = %d; shmptr = %s\n", r_bytes, shmptr+3);
    }
  }

  close(fd_out);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Expected two arguments: src.txt and dst.txt paths.\n");
    exit(1);
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork error");
    exit(1);
  }

  if(pid == 0) {
    // Child
    send(argv);
  } else {
    // Parent
    char *shmptr = getaddr(argv[1]);

    receive(shmptr, argv);
  }

  return 0;
}
