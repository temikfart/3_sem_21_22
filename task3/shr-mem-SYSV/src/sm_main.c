#include "sm_main.h"

Semid *SemaphoresInit(char *argv[]) {
  Semid *sem = malloc(sizeof(Semid));
  sem->empty = MySemOpen(argv[0], BUF_SZ);
  sem->mutex = MySemOpen(argv[1], 1);
  sem->full = MySemOpen(argv[2], 0);

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
SharedMemory *SharedMemoryInit(const char *path) {
  SharedMemory *Shmem = malloc(sizeof(SharedMemory));
  Shmem->ptr = (char *)getaddr(path, SHM_SZ);

  Shmem->status = (typeof(Shmem->status))(Shmem->ptr);
  *(Shmem->status) = 1;

  Shmem->s_ind = (typeof(Shmem->s_ind))
          (Shmem->status + sizeof(typeof(Shmem->s_ind)));
  *(Shmem->s_ind) = 0;

  Shmem->r_ind = (typeof(Shmem->s_ind))
          (Shmem->s_ind + sizeof(typeof(Shmem->r_ind)));
  *(Shmem->r_ind) = 0;

  Shmem->buf = Shmem->ptr + INDEX_NUM * sizeof(typeof(Shmem->status));

  return Shmem;
}
void SharedMemoryRemove(SharedMemory *Shmem) {
  shmdt(Shmem->ptr);
  free(Shmem);
}

void send(char *argv[], SharedMemory *Shmem, Semid *sem) {
  // Get file descriptor
  int fin = open(argv[1], O_RDONLY);
  if (fin == -1) {
    perror("open fd error");
    exit(1);
  }

  size_t *status = Shmem->status;
  size_t *snd_pos = Shmem->s_ind;
  size_t read_sz;
  while (*status) {
    // -----CRITICAL SECTION----- //
    MySemWait(sem->empty);
    MySemWait(sem->mutex);

    // Read data from fd and write in shared memory
    read_sz = read(fin, (Shmem->buf) + (*snd_pos), 1);

    if (read_sz == 0) {
      *status = 0;
      printf("Sender: EOF!\n");
    } else {
      // Count index
      *snd_pos = ((*snd_pos) + read_sz) % (size_t)(BUF_SZ);
    }

    MySemPost(sem->mutex);
    MySemPost(sem->full);
    // -----------END------------ //
  }
  while (*status == 0);

  close(fin);
}
void receive(char *argv[], SharedMemory *Shmem, Semid *sem) {
  // Get file descriptor
  int fout = fileno(fopen(argv[2], "w"));
  if (fout == -1) {
    perror("open fd error");
    exit(1);
  }

  // Read data from shared memory and write in file
  size_t *status = Shmem->status;
  size_t *rcv_pos = Shmem->r_ind;
  size_t written_sz;
  while (1) {
    // -----CRITICAL SECTION----- //
    MySemWait(sem->full);
    MySemWait(sem->mutex);

    if ((((*status) == 1)
         + (semctl(*(sem->full), 0, GETVAL)) != 0) <= 0) {
      MySemPost(sem->mutex);
      MySemPost(sem->empty);
      break;
    }

    // Read data from shared memory and write in file
    written_sz = write(fout, Shmem->buf+(*rcv_pos), 1);
    if (written_sz == 0) {
      printf("Bad writing in file\n");
      exit(1);
    }
    // Count index
    *rcv_pos = ((*rcv_pos) + 1) % BUF_SZ;

    MySemPost(sem->mutex);
    MySemPost(sem->empty);
    // -----------END------------ //
  }
  *status = 1;
  printf("Receiver: EOF!\n");

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
    receive(argv, Shmem, sem);
  } else {
    // Parent
    send(argv, Shmem, sem);
    wait(NULL);
  }

  SemaphoreRemove(sem, argv);
  SharedMemoryRemove(Shmem);

  return 0;
}
