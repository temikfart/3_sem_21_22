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
void Read(int fd, void *buf, size_t nbytes) {
  ssize_t nread = read(fd, buf, nbytes);
  if (nread == -1) {
    perror("read error");
    exit(1);
  }
  if (nread == 0) {
    printf("Network down\n");
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  printf("Client %d is running\n", getpid());


  char fifo_name[100];
  struct simple_message msg;
  msg.sm_clientpid = getpid();
  MakeFifoName(msg.sm_clientpid, fifo_name, sizeof(fifo_name));
  if (mkfifo(fifo_name, 0666) == -1 && errno != EEXIST) {
    perror("mkfifo errro");
    exit(1);
  }

  // Open fd for send msg to server
  int fd_server;
  fd_server = Open(SERVER_FIFO_NAME, O_WRONLY);

  // Sending msg with path to src.txt
  strcpy(msg.sm_data, argv[1]);
  write(fd_server, &msg, sizeof(msg));

  // Opening fifo for receiving the answer
  int fd_client, fd_client_w;
  fd_client = Open(fifo_name, O_RDONLY);
  fd_client_w = Open(fifo_name, O_WRONLY);

  // Receiving answer
  Read(fd_client, &msg, sizeof(msg));
  printf("клиент %d: %s -> %s\n", getpid(), argv[1], msg.sm_data);

  {
    close(fd_server);
    close(fd_client_w);
    close(fd_client);
    unlink(fifo_name);

    printf("Client %d finished work\n", getpid());
  }

  return 0;
}
