#include "fifo_server.h"

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

int main() {
  printf("Server is running\n");
  if (mkfifo(SERVER_FIFO_NAME, 0666) == -1 && errno != EEXIST) {
    perror("mkfifo error");
    exit(1);
  }

  // Open file descriptors
  int fd_server, fd_server_w;
  fd_server = Open(SERVER_FIFO_NAME, O_RDONLY);
  // Additional fd: to be running always
  fd_server_w = Open(SERVER_FIFO_NAME, O_WRONLY);

  // Receiving and processing msgs
  int fd_client;
  struct simple_message msg;
  char fifo_name[100];
  // Receiving msg
  Read(fd_server, &msg, sizeof(msg));
  printf("Msg received. Processing..\n");

  // Processing
  int fin = Open(msg.sm_data, O_WRONLY);
  printf("Successful. Server is starting to send data from the file\n");
  close(fin);

  // Sending results of processing
  MakeFifoName(msg.sm_clientpid, fifo_name, sizeof(fifo_name));

  fd_client = Open(fifo_name, O_WRONLY);
  write(fd_client, &msg, sizeof(msg));
  close(fd_client);

  close(fd_server);

  return 0;
}
