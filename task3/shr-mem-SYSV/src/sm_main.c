#include "sm_main.h"

SharedMemory *SharedMemoryInit(const char *path) {
  SharedMemory *Shmem = malloc(sizeof(SharedMemory));
  Shmem->ptr = (char *)getaddr(path, SHM_SZ);
  Shmem->s_ind = (typeof(Shmem->s_ind))(Shmem->ptr);
  *(Shmem->s_ind) = 0;
  Shmem->r_ind = (typeof(Shmem->s_ind))
          (Shmem->ptr + sizeof(typeof(Shmem->r_ind)));
  *(Shmem->r_ind) = 0;
  Shmem->buf = Shmem->ptr + INDEX_NUM * sizeof(typeof(Shmem->s_ind));

  return Shmem;
}
Semid *SemaphoresInit(char *argv[]) {
  Semid *sem = malloc(sizeof(Semid));
  sem->empty = MySemOpen(argv[0], BUF_SZ);
  sem->mutex = MySemOpen(argv[1], 1);
  sem->full = MySemOpen(argv[2], 1);

  printf("empty=%d mutex=%d full=%d\n",
         *(sem->empty), *(sem->mutex), *(sem->full));

  if (sem->empty == NULL
      || sem->mutex == NULL
      || sem->full == NULL) {
    perror("MySemOpen error");
    exit(1);
  }

  return sem;
}
void SemaphoreRemove(Semid *sem, char *argv[]) {
  MySemClose(sem->empty);
  MySemClose(sem->mutex);
  MySemClose(sem->full);
  MySemRemove(argv[0]);
  MySemRemove(argv[1]);
  MySemRemove(argv[2]);
  free(sem);
}
void SharedMemoryRemove(SharedMemory *Shmem) {
  free(Shmem->ptr);
  free(Shmem);
}

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

  return shmptr;
}

void send(char *argv[], SharedMemory *Shmem, Semid *sem) {
  // Get file descriptor
  int fin = open(argv[1], O_RDONLY);
  if (fin == -1) {
    perror("open fd error");
    exit(1);
  }

  size_t *snd_pos = Shmem->s_ind;
  size_t *rcv_pos = Shmem->r_ind;
  size_t read_sz = BUF_SZ;
  while (read_sz != 0) {
    // -----CRITICAL SECTION----- //
    MySemWait(sem->empty);
    printf("Sending.\n");
    MySemWait(sem->mutex);
    printf("Sending..\n");

    if ((*snd_pos) != (*rcv_pos) - 1) {
      // Read data from fd and write in shared memory
      read_sz = read(fin, (Shmem->buf) + (*snd_pos), 1);
      if (read_sz == 0) {
        printf("EOF!\n");
      }
      // Count index
      *snd_pos = ((*snd_pos) + read_sz) % BUF_SZ;
    }

    MySemPost(sem->mutex);
    MySemPost(sem->full);
    // -----------END------------ //
  }

  close(fin);
}
void receive(char *argv[], SharedMemory *Shmem, Semid *sem) {
  // Get file descriptor
  int fout = open(argv[2], O_WRONLY);
  if (fout == -1) {
    perror("open fd error");
    exit(1);
  }

  // Read data from shared memory and write in file
  size_t *snd_pos = Shmem->s_ind;
  size_t *rcv_pos = Shmem->r_ind;
  size_t read_sz = 1;
  size_t written_sz = BUF_SZ;
  int flag = 1;
  while (flag) {
    // -----CRITICAL SECTION----- //
    MySemWait(sem->full);
    printf("Receiving.\n");
    MySemWait(sem->mutex);
    printf("Receiving..\n");

    if ((*rcv_pos) != (*snd_pos) - 1) {
      // Read data from shared memory and write in file
      written_sz = write(fout, Shmem->buf, read_sz);
      if (written_sz == 0) {
        printf("Bad writing in file\n");
        exit(1);
      }
      // Count index
      *rcv_pos = ((*rcv_pos) + read_sz) % BUF_SZ;
    } else {
      flag = 0;
    }

    MySemPost(sem->mutex);
    MySemPost(sem->full);
    // -----------END------------ //
  }

  close(fout);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Expected two arguments: src.txt and dst.txt paths.\n");
    exit(1);
  }

  SharedMemory *Shmem = SharedMemoryInit(argv[1]);
  Semid *sem = SemaphoresInit(argv);

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork error");
    exit(1);
  }

  if(pid == 0) {
    // Child
    send(argv, Shmem, sem);
  } else {
    // Parent
    receive(argv, Shmem, sem);
  }

  SemaphoreRemove(sem, argv);
  SharedMemoryRemove(Shmem);

  return 0;
}
