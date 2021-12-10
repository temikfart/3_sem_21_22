#include "s_main.h"

static Client Clients[MAX_CLIENT];
static int client_max = 0;
static int s_fifo_reg_fd;
static int s_fifo_rsp_fd;

int s_mkfifo(const char* fifo_name) {
  /* ---- Make FIFO ---- */
  char* s_fifo_path = calloc(MAX_PATH, sizeof(char));
  strcat(s_fifo_path, S_FIFO_REL);
  strcat(s_fifo_path, fifo_name);
  if (mkfifo(s_fifo_path, 0666) == -1 && errno != EEXIST) {
    perror("Can't create server fifo");
    return -1;
  }
  printf("Server: created FIFO %s\n", s_fifo_path);

  /* ---- Open fifo ---- */
  int s_fifo_fd = open(s_fifo_path, O_RDWR | O_CREAT);
  if (s_fifo_fd == -1) {
    perror("Can't open server fifo");
    return -1;
  }

  return s_fifo_fd;
}
int s_prepare() {
  printf("Server: Preparation started.\n");
  
  /* ---- Make and open fifo fds ---- */
  s_fifo_reg_fd = s_mkfifo(S_FIFO_REG);
  s_fifo_rsp_fd = s_mkfifo(S_FIFO_RSP);

  if (s_fifo_reg_fd == -1 || s_fifo_rsp_fd == -1) {
    return -1;
  }

  /* ---- Initialize mini-Database ---- */
  for (int i = 0; i < MAX_CLIENT; i++) {
//    Clients[i].fifo_tx = "\0";
//    Clients[i].fifo_rx = "\0";
    Clients[i].is_active = INACTIVE;
  }

  return 0;
}
int s_reg_clients() {
  /* ---- Create set of fds ---- */
  fd_set reg;
  FD_ZERO(&reg);
  FD_SET(s_fifo_reg_fd, &reg);

  /* ---- Additional variables ---- */
  struct timeval tv;
  int retval;

  /* ---- Registration ---- */
  while (1) {
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    /* ---- Check on changes in fd set ---- */
    retval = select(1, &reg, NULL, NULL, &tv);
    /* ---- Check what's happened ---- */
    switch (retval) {
      /* ---- Something went wrong with select() ---- */
      case -1:
        perror("select error");
        return 1;
      /* ---- Timeout ---- */
      case 0:
        printf("Waiting\n");
        break;
      /* ---- Someone has registered ---- */
      default:
        if (FD_ISSET(s_fifo_reg_fd, &reg)) {
          /* ---- Accept the request ---- */
          if (s_check_reg() == -1) {
            printf("Server: Bad registration. Wrong command.\n");
          } else {
            printf("Server: New client has been registered.\n");
            s_draft(&retval);
          }
        } else {
          /* ---- Miss of checked fd from set ---- */
          printf("Server: Bad registration. Miss.\n");
        }
    }
  }
}
int s_check_reg() {
  /* ---- Receive request ---- */
  char request[3 * MAX_PATH] = "\0";
  read(s_fifo_reg_fd, request, 3 * MAX_PATH);

  /* ---- Parse request ---- */
  int i = 0;
  for (char* p = strtok(request, " ");
       p != NULL;
       p = strtok(NULL, " "), i++) {
    switch (i) {
      case 0:
        if (strcmp(p, "REGISTER") != 0) {
          return -1;
        }
        break;
      case 1:
        strcat(Clients[client_max].fifo_tx, p);
        break;
      case 2:
        strcat(Clients[client_max].fifo_rx, p);
        break;
      default:
        return -1;
    }
  }
  Clients[client_max].is_active = 1;
  client_max++;

  return 0;
}
void s_draft(void* A) {
  printf("Server: Start working with new client in new thread.\n");
}

int main() {
  /* ---- START ---- */
  if (s_prepare() == -1) {
    printf("Server: start failed.\n");
    return 1;
  } else {
    printf("Server: started.\n");
  }

  /* ---- REGISTRATION ---- */
  if(s_reg_clients() == -1) {
    printf("Something went wrong: "
           "Registration process failed.\n");
    return 1;
  }

  return 0;
}

