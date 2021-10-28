#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include "sm_main.h"

/*
 * 1st byte -- letter count
 * 2nd byte -- number of packages
 * 3rd byte -- number of bytes in the last package
 */
#define PCG_SZ 128
#define SHM_SZ (7 * PCG_SZ + 3)
#define BUF_SZ (SHM_SZ - 3)

void *getaddr(const char *path, size_t shm_sz) {
  key_t key = ftok(path, 1);
  if (key == -1) {
    perror("ftok error for");
    exit(1);
  }

  int shmid = shmget(key, shm_sz, IPC_CREAT);
  if (shmid == -1) {
    printf("%d, %ld", key, shm_sz);
    perror(" shmget error");
    exit(1);
  }

  char *shmptr = (char *)shmat(shmid, NULL, 0);
  if ((size_t)shmptr == -1) {
    perror("shmat error");
    exit(1);
  }

  shmptr[0] = 1;
  shmptr[1] = 1;
  shmptr[2] = 1;


  return shmptr;
}
void sempost(int *sem) {
  int i = 0;
  while (*sem == 0) {
    printf("(%d) Waiting..\n", i);
    i++;
    sleep(1);
  }
  (*sem)--;
  printf("(%d) Closing..\n", i++);
  printf("(%d) Closed!\n", i++);
}
void semwait(int *sem) {
  (*sem)++;
  printf("Opened!\n");
}
void send(char *argv[]) {
  // Get ptr to buffer
  char *shmptr = (char *)getaddr(argv[1], SHM_SZ);

  // Get ptr to two semaphores
  // sem[0] is a semaphore for this process
  // sem[1] is ready-flag (3 if both processes are ready)
  size_t sem_n = 1;
  int *sem = getaddr(argv[2], sem_n);

  // Get file descriptor
  int fin = open(argv[1], O_RDONLY);
  if (fin == -1) {
    perror("open fd error");
    exit(1);
  }

  // Read data from fd and write in shared memory
  size_t read_sz = BUF_SZ;
  char *letter = shmptr;
  *letter = (char)0;
  char *pcg_n = shmptr + 1;
  char *last_pcg_sz = shmptr + 2;
  while (read_sz == BUF_SZ) {
    // -----CRITICAL SECTION----- //
    sempost(sem);

//    if (sem[1] != 3) {
//      sem[1] += 1;
//    }

    // Read data from fd
    read_sz = read(fin, shmptr+3, BUF_SZ);

    // Count size of data and letter number
    if (read_sz == 0) {
      *pcg_n = 0;
      *last_pcg_sz = 0;
    } else {
      (*letter)++; // TODO: may be overflow
      *pcg_n = (char)(read_sz / PCG_SZ);
      *last_pcg_sz = (char)(read_sz % PCG_SZ);
    }

    printf("Sent letter #%d (%d*128+%d): %s\n",
           (*letter), (*pcg_n), (*last_pcg_sz), (shmptr+3));

    semwait(sem);
    // -----------END------------ //
  }

  close(fin);
}
void receive(char *argv[]) {
  // Get ptr to buffer
  char *shmptr = (char *)getaddr(argv[1], SHM_SZ);

  // Get ptr to two semaphores
  // sem[0] is a semaphore for this process
  // sem[1] is ready-flag (3 if both processes are ready)
  size_t sem_n = 3;
  int *sem = getaddr(argv[2], sem_n);
  sem[1] += 1;
  while (sem[1] < 3) {
    printf("%d\t", sem[1]);
  }

  // Get file descriptor
  int fout = open(argv[2], O_WRONLY);
  if (fout == -1) {
    perror("open fd error");
    exit(1);
  }

  // Read data from shared memory and write in fd
  size_t read_sz;
  int wait_letter = 1;
  char *rcv_letter = shmptr;
  char *pcg_n = shmptr + 1;
  char *last_pcg_sz = shmptr + 2;

  // Say to another process, that it ready

  while (1) {
    int size = (int)(*pcg_n) + (int)(*last_pcg_sz);
    printf("(%d) ", size);
    if (size == 0) {
      printf("Nothing to write. End\n");
      break;
    }
    // -----CRITICAL SECTION----- //
    sempost(sem);
    printf("Try to read from shared memory\n");

    // Read data from shared memory
    if (wait_letter == (*rcv_letter)) {
      read_sz = (size_t)((*pcg_n) * PCG_SZ + (*last_pcg_sz));
      printf("(RD_SZ = %ld) ", read_sz);

      printf("Receive letter #%d (%d*128+%d): ",
             (*rcv_letter), (*pcg_n), (*last_pcg_sz));
      write(fileno(stdout), shmptr+3, read_sz);
      printf("\n");

      // Write data from shared memory in file
      {
        size_t written_sz = write(fout, shmptr+3, read_sz);
        if (written_sz != read_sz) {
          printf("Bad write in file\n");
        }
      }

      // Clear protocol data
      if (read_sz != BUF_SZ) {
        (*pcg_n) = 0;
        (*last_pcg_sz) = 0;
      }

      wait_letter++;
    }

    semwait(sem);
    // -----------END------------ //
  }

  close(fout);
};

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
    receive(argv);
  }

  return 0;
}
