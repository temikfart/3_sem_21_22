#include "fifo_main.h"

int make_fifo_name(pid_t pid, char *name, size_t name_max) {
  snprintf(name, name_max, "fifo%d", pid);
  return 0;
}

int main() {
  int fd_server, fd_client, i;
  ssize_t nread;
  struct simple_message msg;
  char fifo_name[100];

  printf("Server is running\n");
  if (mkfifo(SERVER_FIFO_NAME, 0666) == -1 && errno != EEXIST) {
    perror("mkfifo error");
    exit(1);
  }

  fd_server = open(SERVER_FIFO_NAME, O_RDONLY);
  if (fd_server == -1) {
    perror("fd_server wasn't opened");
    exit(1);
  }

  // to be running always
  int fd_server_w = open(SERVER_FIFO_NAME, O_WRONLY);
  if (fd_server_w == -1) {
    perror("fd_server wasn't opened");
    exit(1);
  }

  while(1) {
    nread = read(fd_server, &msg, sizeof(msg));
    if (nread == 0) {
      printf("Network down\n");
      exit(1);
    }

    for (i = 0; msg.sm_data[i] != '\0'; i++) {
      msg.sm_data[i] = (char)toupper(msg.sm_data[i]);
    }

    make_fifo_name(msg.sm_clientpid, fifo_name, sizeof(fifo_name));
    fd_client = open(fifo_name, O_WRONLY);
    if (fd_client == -1) {
      perror("fd_client wasn't opened");
      exit(1);
    }
    write(fd_client, &msg, sizeof(msg));
    close(fd_client);
  }

  close(fd_server);

  return 0;
}
