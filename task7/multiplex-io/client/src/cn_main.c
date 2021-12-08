#include "cn_main.h"

char* make_fifo_name(pid_t pid, const char* suf) {
  char* name = calloc(MAX_PATH_LEN, sizeof(char));
  sprintf(name, "%sfifo%d_%s",
          FIFO_STORAGE_REL_PATH, pid, suf);
  return name;
}
int register_me(char* fifo_tx, char* fifo_rx) {
  char* s_fifo_path = calloc(MAX_PATH_LEN, sizeof(char));
  strcat(s_fifo_path, FIFO_STORAGE_REL_PATH);
  strcat(s_fifo_path, PUBLIC_SERVER_FIFO_NAME);
  printf("%s\n", s_fifo_path);
  
  int s_fifo_fd = open(s_fifo_path, O_RDWR);
  if (s_fifo_fd == -1) {
    perror("Can't open server fifo");
    return -1;
  }
  
  char cmd[MAX_PATH_LEN] = "REGISTER ";
  strcat(cmd, fifo_tx);
  strcat(cmd, " ");
  strcat(cmd, fifo_rx);
  printf("Command: %s\n", cmd);
  
  write(s_fifo_fd, cmd, strlen(cmd));
  
  char response[MAX_PATH_LEN] = "\0";
  read(s_fifo_fd, response, MAX_PATH_LEN);
  printf("Response: %s\n", response);
  if (strcmp(response, "ASK") != 0) {
    return -1;
  }
  
  return s_fifo_fd;
}

int main() {
  char* fifo_tx = make_fifo_name(getpid(), "tx");
  char* fifo_rx = make_fifo_name(getpid(), "rx");
  printf("tx: %s\nrx: %s\n", fifo_tx, fifo_rx);
  
  int s_fifo_fd = register_me(fifo_tx, fifo_rx);
  if (s_fifo_fd == -1) {
    printf("Registration failed\n");
    return 1;
  }
  
	return 0;
}

