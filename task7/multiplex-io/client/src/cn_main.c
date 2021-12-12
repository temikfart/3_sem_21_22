#include "cn_main.h"

static int s_fifo_reg_fd;
static int s_fifo_rsp_fd;

static char* fifo_tx;
static char* fifo_rx;

int cn_reg() {
  printf("Client: Trying to register.\n");

  /* ---- Open server's fifo ---- */
  s_fifo_reg_fd = cn_open_fifo(S_FIFO_REG,
                               O_WRONLY | O_APPEND);
  s_fifo_rsp_fd = cn_open_fifo(S_FIFO_RSP,
                               O_RDONLY);
  printf("Client: REG and RSP fifo opened.\n");
  /* ---- ---- ---- ---- ---- ---- */

  /* ---- Form registration command ---- */
  char cmd[CMD_SZ] = "\0";
  sprintf(cmd, "REGISTER %s %s\n", fifo_tx, fifo_rx);
  write(s_fifo_reg_fd, cmd, strlen(cmd));
  printf("Client: %s", cmd);
  /* ---- ---- ---- ---- ---- ---- ---- */

  /* ---- Receive answer "ASK" ---- */
  char response[CMD_SZ] = "\0";
  read(s_fifo_rsp_fd, response, CMD_SZ);
  printf("Response: %s", response);
  if (strcmp(response, DEFAULT_RSP) != 0) {
    printf("Registration failed. Wrong response.\n");
    exit(1);
  } else {
    printf("Registration was completed successfully.\n");
  }
  /* ---- ---- ---- ---- ---- ---- */

  /* ---- Close server's FIFO ---- */
  cn_close_fifo(s_fifo_reg_fd);
  cn_close_fifo(s_fifo_rsp_fd);
  /* ---- ---- ----- ---- ---- ---- */

  return 0;
}
int cn_work() {
  printf("Good job!\n");
  return 0;
}

int main() {
  /* ---- Registration ---- */
  fifo_tx = cn_mkfifo_path(getpid(), "tx");
  fifo_rx = cn_mkfifo_path(getpid(), "rx");
  printf("Client: tx/rx paths created:\n"
         "\tTX: %s\n\tRX: %s\n", fifo_tx, fifo_rx);

  cn_reg();
  /* ---- ---- ---- ---- */

  /* ---- Start work ---- */
  cn_work();
  /* ---- ---- ---- ---- */

  free(fifo_tx);
  free(fifo_rx);

  return 0;
}

