#include "sm_main.h"

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

  shmptr[0] = 0;
  shmptr[3] = 0;

  return shmptr;
}
void send(char *argv[]) {
  // Get ptr to buffer
  char *shmptr = (char *)getaddr(argv[1], SHM_SZ);

  int *semid_empty = MySemOpen(argv[0]);
  int *semid_mutex = MySemOpen(argv[1]);
  int *semid_full = MySemOpen(argv[2]);
  if (semid_empty == NULL || semid_mutex == NULL || semid_full == NULL) {
    perror("MySemOpen error");
    exit(1);
  }
//  printf("(C): semid = %d\n", *semid);

  // Get file descriptor
  int fin = open(argv[1], O_RDONLY);
  if (fin == -1) {
    perror("open fd error");
    exit(1);
  }

  // Read data from fd and write in shared memory
  size_t read_sz = BUF_SZ;
  char *letter = shmptr;
  char *pcg_n = shmptr + 1;
  char *last_pcg_sz = shmptr + 2;
  char *snt_letter = shmptr + 3;
  while (read_sz == BUF_SZ) {
    // -----CRITICAL SECTION----- //
    MySemWait(semid_empty);
    MySemWait(semid_mutex);

    if (*snt_letter == *letter) {
      // Read data from fd
      read_sz = read(fin, shmptr+PROTOCOL_SZ, BUF_SZ);

      // Count size of data and letter number
      (*letter)++;
      (*letter) = (char)((int)(*letter) % 128);
      if (read_sz == 0) {
        *pcg_n = 0;
        *last_pcg_sz = 0;
      } else {
        *pcg_n = (char)(read_sz / PCG_SZ);
        *last_pcg_sz = (char)(read_sz % PCG_SZ);
      }

//      printf("Sent letter #%d (%d*128+%d): ",
//             (*letter), (*pcg_n), (*last_pcg_sz));
//      write(fileno(stdout), shmptr+PROTOCOL_SZ, read_sz);
//      printf("\n");
        printf("Sent %ld\n", read_sz);
    }

    MySemPost(semid_mutex);
    MySemPost(semid_full);
    // -----------END------------ //
  }

  close(fin);
  MySemClose(semid_empty);
  MySemClose(semid_mutex);
  MySemClose(semid_full);
  MySemRemove(argv[0]);
  MySemRemove(argv[1]);
  MySemRemove(argv[2]);
  shmdt(shmptr);
}
void receive(char *argv[]) {
  // Get ptr to buffer
  char *shmptr = (char *)getaddr(argv[1], SHM_SZ);

  int *semid_empty = MySemOpen(argv[0]);
  int *semid_mutex = MySemOpen(argv[1]);
  int *semid_full = MySemOpen(argv[2]);
  if (semid_empty == NULL || semid_mutex == NULL || semid_full == NULL) {
    perror("MySemOpen error");
    exit(1);
  }
//  printf("(P): semid = %d\n", *semid);

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
  char *rd_letter = shmptr + 3;

  // Say to another process, that it ready

  while (1) {
    // -----CRITICAL SECTION----- //
    MySemWait(semid_full);
    MySemWait(semid_mutex);

    int size = (int)(*pcg_n) * PCG_SZ + (int)(*last_pcg_sz);
//    printf("(%d) ", size);
    if (size == 0 && *rcv_letter != 0) {
      printf("Nothing to write. End\n");
      break;
    }
//    printf("Try to read from shared memory\n");

    // Read data from shared memory
    if (wait_letter == (*rcv_letter)) {
      read_sz = (size_t)((*pcg_n) * PCG_SZ + (*last_pcg_sz));
//      printf("(RD_SZ = %ld) ", read_sz);

//      printf("Receive letter #%d (%d*128+%d): ",
//             (*rcv_letter), (*pcg_n), (*last_pcg_sz));
//      write(fileno(stdout), shmptr+PROTOCOL_SZ, read_sz);
//      printf("\n");

      // Write data from shared memory in file
      {
        size_t written_sz = write(fout, shmptr+PROTOCOL_SZ, read_sz);
        if (written_sz != read_sz) {
          printf("Bad write in file\n");
        }
      }

      // Clear protocol data
      if (read_sz != BUF_SZ) {
        *rcv_letter = 1;
        *pcg_n = 0;
        *last_pcg_sz = 0;
      }

      wait_letter++;
      wait_letter %= 128;
      (*rd_letter)++;
      (*rd_letter) = (char)((int)(*rd_letter) % 128);
      printf("Received %ld\n", read_sz);
    }

    MySemPost(semid_mutex);
    MySemPost(semid_empty);
    // -----------END------------ //
  }

  close(fout);
  MySemClose(semid_empty);
  MySemClose(semid_mutex);
  MySemClose(semid_full);
  MySemRemove(argv[0]);
  MySemRemove(argv[1]);
  MySemRemove(argv[2]);
  shmdt(shmptr);
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
    receive(argv);
  }

  return 0;
}
