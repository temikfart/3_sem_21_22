#include "fifo_main.h"
#include <string.h>

int make_fifo_name(pid_t pid, char *name, size_t name_max) {
  snprintf(name, name_max, "fifo%d", pid);
  return 0;
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
  make_fifo_name(msg.sm_clientpid, fifo_name, sizeof(fifo_name));
  if (mkfifo(fifo_name, 0666) == -1 && errno != EEXIST) {
    perror("mkfifo errro");
    exit(1);
  }
  fd_server = open(SERVER_FIFO_NAME, O_WRONLY);

  for(i = 0; work[i] != NULL; i++) {
    strcpy(msg.sm_data, work[i]);
    write(fd_server, &msg, sizeof(msg));
    if (fd_client == -1) {
      fd_client = open(fifo_name, O_RDONLY);
    }
    if (fd_client_w == -1) {
      fd_client = open(fifo_name, O_WRONLY);
    }
    nread = read(fd_client, &msg, sizeof(msg));
    if (nread == 0) {
      printf("Network down\n");
      exit(1);
    }
    printf("клиент %d: %s -> %s\n", getpid(), work[i], msg.sm_data);
  }
  close(fd_server);
  close(fd_client_w);
  close(fd_client);
  unlink(fifo_name);
  printf("Клиент %d закончил работу\n", getpid());

  return 0;
}
