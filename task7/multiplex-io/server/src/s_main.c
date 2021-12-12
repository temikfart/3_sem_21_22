#include "s_main.h"

static Client Clients[MAX_CLIENT];
static int client_max = 0;

static int s_fifo_reg_fd_r;
static int s_fifo_rsp_fd_w;

static int s_fifo_reg_fd_w;
static int s_fifo_rsp_fd_r;

int s_prepare() {
  printf("Server: Preparation started.\n");
  
  /* ---- Make and open fifo fds ---- */
  s_mkfifo(S_FIFO_REG);
  s_mkfifo(S_FIFO_RSP);

  s_fifo_reg_fd_r = s_open_fifo(S_FIFO_REG,
                                O_RDONLY
                                | O_NONBLOCK);
  s_fifo_rsp_fd_r = s_open_fifo(S_FIFO_RSP,
                                O_RDONLY
                                | O_NONBLOCK);

  s_fifo_reg_fd_w = s_open_fifo(S_FIFO_REG,
                                O_WRONLY);
  s_fifo_rsp_fd_w = s_open_fifo(S_FIFO_RSP,
                                O_WRONLY
                                | O_APPEND);
  /* ---- ---- ---- ---- ---- ---- */

  /* ---- Initialize mini-Database ---- */
  for (int i = 0; i < MAX_CLIENT; i++) {
    Clients[i].fifo_tx[0] = '\0';
    Clients[i].fifo_rx[0] = '\0';
    Clients[i].is_active = INACTIVE;
  }
  /* ---- ---- ---- ---- ---- ---- ---- */

  return 0;
}
int s_reg_clients() {
  printf("Server: Waiting for clients.\n");
  char request[BUF_SZ] = "\0";
  while (1) {
    /* ---- Receive the request ---- */
    read(s_fifo_reg_fd_r, request, BUF_SZ);
    /* ---- ---- ---- ---- ---- ---- */

    /* ---- Check the request and register new client ---- */
    if (strcmp(request, "") != 0) {
      request[strlen(request) - 1] = '\0';
      printf("\tRequest: %s\n", request);

      if (s_check_reg(request) == -1) {
        printf("Server: Bad registration. Wrong command:\n\t%s\n", request);
      } else {
        printf("Server: New client has been registered.\n");

        Arg Arguments;
        Arguments.client_num = client_max-1;
        s_draft(&Arguments);
      }
    }
    /* ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- */

    memset(request, 0, strlen(request));
  }
  return 0;
}

int s_check_reg(char* request) {
  /* ---- Receive request ---- */
  char cmd[CMD_SZ] = "\0";
  strcat(cmd, request);
//  printf("Server: cmd = %s\n", cmd);
  /*  ---- ---- ---- ---- ---- */

  /* ---- Parse request ---- */
  int i = 0;
  for (char* p = strtok(cmd, " ");
       p != NULL;
       p = strtok(NULL, " "), i++) {
    switch (i) {
      case 0:
        if (strcmp(p, "REGISTER") != 0) {
          printf("Parser: Wrong command: <%s>", p);
          return -1;
        }
        break;
      case 1:
        sprintf(Clients[client_max].fifo_tx, "%s",
                (strrchr(p, '/') + 1));
//        printf("Parser: tx = %s\n", Clients[client_max].fifo_tx);
        break;
      case 2:
        sprintf(Clients[client_max].fifo_rx, "%s",
                (strrchr(p, '/') + 1));
//        printf("Parser: rx = %s\n", Clients[client_max].fifo_rx);
        break;
      default:
        return -1;
    }
  }
  Clients[client_max].is_active = 1;
  client_max++;
  /* ---- ---- ---- ---- ---- */

  return 0;
}
void s_draft(void* Arguments) {
  Arg *A = (Arg*)Arguments;
  Client* Cn = &Clients[A->client_num];

  printf("Server: Start working with new client in the new thread.\n");
  printf("\tClient #%d is %s:\n\t\t%s\n\t\t%s\n", A->client_num,
         Cn->is_active ? "active" : "inactive",
         Cn->fifo_tx,
         Cn->fifo_rx);

  /* ---- Create tx/rx fifo ---- */
  int cn_fifo_tx_fd_r, cn_fifo_rx_fd_w;
  int cn_fifo_tx_fd_w, cn_fifo_rx_fd_r; // Additional

  cn_fifo_tx_fd_r = s_mkfifo(Cn->fifo_tx);
  cn_fifo_rx_fd_w = s_mkfifo(Cn->fifo_rx);
  /* ---- ---- ---- ---- ---- ---- */

  /* ---- Open main fifo for the client ---- */
  cn_fifo_tx_fd_r = s_open_fifo(Cn->fifo_tx,
                                O_RDONLY
                                | O_NONBLOCK);
  cn_fifo_rx_fd_r = s_open_fifo(Cn->fifo_rx,
                                O_RDONLY
                                | O_NONBLOCK);

  cn_fifo_tx_fd_w = s_open_fifo(Cn->fifo_tx,
                                O_WRONLY);
  cn_fifo_rx_fd_w = s_open_fifo(Cn->fifo_rx,
                                O_WRONLY
                                | O_APPEND);
  /* ---- ---- ---- ---- ---- */

  /* ---- Write response to the client ---- */
  if (write(s_fifo_rsp_fd_w,
            DEFAULT_RSP,
            strlen(DEFAULT_RSP)) == -1) {
    perror("Can't write response");
    exit(1);
  }
  printf("Server: tx/rx fifo configured.\n");
  /* ---- ---- ---- ---- ---- ---- ---- ---- */

  s_unlink_fifo(Cn->fifo_tx);
  s_unlink_fifo(Cn->fifo_rx);

  printf("Server: Work with Client#%d ended", A->client_num);
}

int main() {
  /* ---- START ---- */
  s_prepare();
  printf("Server: started.\n");

  /* ---- REGISTRATION ---- */
  s_reg_clients();

  return 0;
}

