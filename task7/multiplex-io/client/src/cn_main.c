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
  /* ---- Open fd ---- */
  int fd_tx = cn_open_fifo(fifo_tx, O_WRONLY);
  int fd_rx = cn_open_fifo(fifo_rx, O_RDONLY);
  printf("tx/rx fds opened.\n");
  /* ---- ---- ---- ---- */

  /* ---- Epoll tx fd ---- */
  int event_count;
  struct epoll_event event, events[5];
  event.data.fd = fd_tx;
  event.events = EPOLLIN;

  int epoll_fd = epoll_create1(0);
  if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event)) {
    perror("Failed to add file descriptor to epoll");
    close(epoll_fd);
    exit(1);
  }

  while(1) {
    /* ---- Send request ----*/
    char* request = NULL;
    request = cn_scan_request();
    printf("Request: %s", request);
    write(fd_tx, request, strlen(request));
    /* ---- ---- ---- ---- */

    event_count = epoll_wait(epoll_fd,
                             events,
                             5,
                             10000);
    for (int i = 0; i < event_count; i++) {
      /* ---- Receive response ----*/
      ssize_t read_sz = 0;
      char response[MAX_PATH];
      read_sz = read(events[i].data.fd,
                     response,
                     MAX_PATH);
      if (read_sz == -1) {
        perror("Read from rx failed");
        close(fd_tx);
        close(fd_rx);
        exit(1);
      }
      printf("Response: %s\n", response);
      /* ---- ---- ---- ---- ---- */
    }
    break;
  }
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

