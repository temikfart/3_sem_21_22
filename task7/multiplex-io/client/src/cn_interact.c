#include "cn_interact.h"

char* cn_mkfifo_path(pid_t pid, const char* suf) {
  char* name = calloc(MAX_PATH, sizeof(char));
  sprintf(name, "%sfifo%d_%s",
          CN_FIFO_REL, pid, suf);

  return name;
}
int cn_open_fifo(const char* fifo_name, int flags) {
  char *fifo_path = calloc(MAX_PATH, sizeof(char));
  sprintf(fifo_path, "%s%s", CN_FIFO_REL, fifo_name);

  int fifo_fd = open(fifo_path, flags);
  if (fifo_fd == -1 && errno != EEXIST) {
    perror("Can't open fifo");
    exit(1);
  }

  free(fifo_path);
  return fifo_fd;
}
int cn_close_fifo(int fd) {
  if (close(fd) == -1) {
    perror("Can't close fifo");
    exit(1);
  }
  return 0;
}
