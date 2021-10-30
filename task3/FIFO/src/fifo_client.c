#include "fifo_client.h"

int MakeFifoName(pid_t pid, char *name, size_t name_max) {
  snprintf(name, name_max, "fifo%d", pid);
  return 0;
}
int Open(const char *path, int flag) {
  int fd = open(path, flag);
  if (fd == -1) {
    perror("fd wasn't opened");
    exit(1);
  }
  return fd;
}
ssize_t Read(int fd, void *buf, size_t nbytes) {
  ssize_t nread = read(fd, buf, nbytes);
  if (nread == -1) {
    perror("read error");
    exit(1);
  }
  if (nread == 0) {
    printf("Network down\n");
    exit(1);
  }
  return nread;
}

int main() {
  int fd_server, fd_client = -1, fd_client_w = -1, i;
  ssize_t nread;
  struct simple_message msg;
  char fifo_name[100];
  char *work[] = {
          "яблочный соус",
          "тигр",
          "гора",
          NULL
  };

  printf("клиент %d запущен\n", getpid());
  msg.sm_clientpid = getpid();
  MakeFifoName(msg.sm_clientpid, fifo_name, sizeof(fifo_name));
  if (mkfifo(fifo_name, 0666) == -1 && errno != EEXIST) {
    perror("mkfifo errro");
    exit(1);
  }

  fd_server = Open(SERVER_FIFO_NAME, O_WRONLY);

  for(i = 0; work[i] != NULL; i++) {
    strcpy(msg.sm_data, work[i]);
    write(fd_server, &msg, sizeof(msg));
    if (fd_client == -1) {
      fd_client = Open(fifo_name, O_RDONLY);
    }
    if (fd_client_w == -1) {
      fd_client_w = Open(fifo_name, O_WRONLY);
    }
    nread = Read(fd_client, &msg, sizeof(msg));
    printf("клиент %d: %s -> %s\n", getpid(), work[i], msg.sm_data);
  }
  close(fd_server);
  close(fd_client_w);
  close(fd_client);
  unlink(fifo_name);
  printf("Клиент %d закончил работу\n", getpid());

  return 0;
}
