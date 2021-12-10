#include "cn_main.h"

char* cn_mkfifo_name(pid_t pid, const char* suf) {
  char* name = calloc(MAX_PATH, sizeof(char));
  sprintf(name, "%sfifo%d_%s",
          S_FIFO_REL, pid, suf);
  return name;
}
int cn_open_s_fifo(const char* fifo_name, int flags) {
  /* ---- Make FIFO name ---- */
  char* s_fifo_path = calloc(MAX_PATH, sizeof(char));
  strcat(s_fifo_path, S_FIFO_REL);
  strcat(s_fifo_path, fifo_name);

  /* ---- Open FIFO ---- */
  int fd = open(s_fifo_path, flags);
  if (fd == -1) {
    perror("Can't open server fifo");
    return -1;
  }

  return fd;
}
int cn_close_s_fifo(int fd) {
  if (close(fd) == -1) {
    return -1;
  }
  return 0;
}
int cn_reg(char* fifo_tx, char* fifo_rx) {
  /* ---- Open server's FIFO ---- */
  int s_fifo_reg_fd = cn_open_s_fifo(S_FIFO_REG, O_WRONLY);
  int s_fifo_rsp_fd = cn_open_s_fifo(S_FIFO_RSP, O_RDONLY);
  if (s_fifo_reg_fd == -1 || s_fifo_rsp_fd == -1) {
    printf("Client %d: Can't open fifo.\n", getpid());
    return 1;
  }

  /* ---- Form registration command ---- */
  char cmd[MAX_PATH] = "REGISTER ";
  strcat(cmd, fifo_tx);
  strcat(cmd, " ");
  strcat(cmd, fifo_rx);
  printf("Command: %s\n", cmd);
  write(s_fifo_reg_fd, cmd, strlen(cmd));

  /* ---- Receive answer "ASK" ---- */
  char response[MAX_PATH] = "\0";
  read(s_fifo_rsp_fd, response, MAX_PATH);
  printf("Response: %s\n", response);
  if (strcmp(response, "ASK") != 0) {
    printf("Registration failed.\n");
    return -1;
  } else {
    printf("Registration was completed successfully.\n");
  }

  cn_close_s_fifo(s_fifo_reg_fd);
  cn_close_s_fifo(s_fifo_rsp_fd);

  return 0;
}

int main() {
  char* fifo_tx = cn_mkfifo_name(getpid(), "tx");
  char* fifo_rx = cn_mkfifo_name(getpid(), "rx");
  printf("tx: %s\nrx: %s\n", fifo_tx, fifo_rx);
  
  if (cn_reg(fifo_tx, fifo_rx) == -1) {
    printf("Registration failed\n");
    return 1;
  }

  free(fifo_tx);
  free(fifo_rx);

  return 0;
}

