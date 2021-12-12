#include "s_interact.h"

int s_mkfifo(const char* fifo_name) {
  char* fifo_path = calloc(MAX_PATH, sizeof(char));
  sprintf(fifo_path, "%s%s", S_FIFO_REL, fifo_name);

  if (mkfifo(fifo_path, 0666) == -1 && errno != EEXIST) {
    perror("Can't create fifo");
    exit(1);
  } else {
    printf("Created FIFO at %s\n", fifo_path);
  }

  free(fifo_path);
  return 0;
}
int s_open_fifo(const char* fifo_name, int flags) {
  char* fifo_path = calloc(MAX_PATH, sizeof(char));
  sprintf(fifo_path, "%s%s", S_FIFO_REL, fifo_name);

  int fifo_fd = open(fifo_path, flags);
  if (fifo_fd == -1 && errno != EEXIST) {
    perror("Can't open fifo");
    exit(1);
  }

  free(fifo_path);
  return fifo_fd;
}
int s_unlink_fifo(const char* fifo_name) {
  char* fifo_path = calloc(MAX_PATH, sizeof(char));
  sprintf(fifo_path, "%s%s", S_FIFO_REL, fifo_name);

  if (unlink(fifo_path) == -1) {
    perror("Can't unlink fifo");
    exit(1);
  }

  free(fifo_path);
  return 0;
}
