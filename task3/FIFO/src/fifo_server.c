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
  ssize_t nread;
  struct simple_message msg;
  char fifo_name[100];

  setlocale(LC_ALL, "");
  while(1) {
    // Receiving msg
    nread = Read(fd_server, &msg, sizeof(msg));
    printf("Msg received. Processing..\n");

    // Processing
    for (int i = 0; msg.sm_data[i] != '\0'; i++) {
      msg.sm_data[i] = (char)toupper(msg.sm_data[i]);
    }

    // Sending results of processing
    MakeFifoName(msg.sm_clientpid, fifo_name, sizeof(fifo_name));

    fd_client = Open(fifo_name, O_WRONLY);
    write(fd_client, &msg, sizeof(msg));
    close(fd_client);
  }

  close(fd_server);

  return 0;
}
