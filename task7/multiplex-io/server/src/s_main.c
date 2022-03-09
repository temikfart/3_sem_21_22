#include "s_main.h"

static Client Clients[MAX_CLIENT];
static int client_max = 0;

static threadpool ThPool;

static int s_fifo_reg_fd_r;
static int s_fifo_rsp_fd_w;

static int s_fifo_reg_fd_w;
static int s_fifo_rsp_fd_r;

static int epoll_fd;

pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;

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
    Clients[i].event.events = EPOLLIN;
    Clients[i].event.data.fd = -1;    // tx fd
    Clients[i].event.data.u32 = -1;   // rx fd
  }

  ThPool = thpool_init(THREAD_AMOUNT);
  /* ---- ---- ---- ---- ---- ---- ---- */

  /* ---- Create epoll ---- */
  pthread_t epoll_pthread;
  Arg Epoll_Args;
  pthread_create(&epoll_pthread,
                 NULL,
                 s_epoll,
                 &Epoll_Args);
  printf("Server: Created pthread(id=%ld) with epoll",
         epoll_pthread);
  /* ---- ---- ---- ---- ---- */

  return 0;
}
int s_register_clients() {
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

      if (s_check_reg_request(request) == -1) {
        printf("Server: Bad registration. Wrong command:\n\t%s\n", request);
      } else {
        int client_num = client_max-1;
        s_register(&client_num);

        printf("Server: New client has been registered.\n");
      }
    }
    /* ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- */

    memset(request, 0, strlen(request));
  }
  return 0;
}

int s_check_reg_request(char* request) {
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
void s_register(void* Arguments) {
  int *client_num = (int*)Arguments;
  Client* Cn = &Clients[*client_num];

  printf("Server: Start working with new client in the new thread.\n");
  printf("\tClient #%d is %s:\n\t\t%s\n\t\t%s\n", *client_num,
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
  /* ---- ---- ---- ---- ---- ---- ---- ---- */

  /* ---- Write response to the client ---- */
  if (write(s_fifo_rsp_fd_w,
            DEFAULT_RSP,
            strlen(DEFAULT_RSP)) == -1) {
    perror("Can't write response");
    exit(1);
  }
  printf("Server: tx/rx fifo configured.\n");
  /* ---- ---- ---- ---- ---- ---- ---- ---- */

  /* ---- Add fd to epoll ---- */
  Cn->event.data.fd = cn_fifo_tx_fd_r;
  Cn->event.data.u32 = cn_fifo_rx_fd_w;

  /* ---- CRITICAL SECTION ---- */
  pthread_mutex_lock(&pmutex);

  if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &(Cn->event))) {
    perror("Failed to add file descriptor to epoll");
    close(epoll_fd);
    exit(1);
  }

  pthread_mutex_unlock(&pmutex);
  /* ---- ---- ---- ---- ---- */

  printf("Client #%d was added to epoll\n", *client_num);
  /* ---- ---- ---- ---- ---- */
}

void* s_epoll(void* Arguments) {
  /* ---- Create epoll ---- */
  printf("Server: Creating epoll..\n");
  epoll_fd = epoll_create1(0);
  if(epoll_fd == -1) {
    perror("Failed to create epoll file descriptor");
    exit(1);
  }
  /* ---- ---- ---- ---- ---- */

  /* ---- Declare variables for epoll ---- */
  int event_count;
  struct epoll_event events[MAX_EVENTS];
  int running = 1;
  /* ---- ---- ---- ---- ---- ---- ---- */

  while(running) {
    printf("\nPolling for input...\n");
    event_count = epoll_wait(epoll_fd, events,
                             MAX_EVENTS,
                             30000);
    printf("%d ready events\n", event_count);

    /* ---- CRITICAL SECTION ---- */
    pthread_mutex_lock(&pmutex);

    for(int i = 0; i < event_count; i++) {
      printf("Server: processing request #%d", i);

      struct epoll_event* req_event = malloc(sizeof(events[i]));
      thpool_add_work(ThPool,
                      s_processing_request,
                      (void*)req_event);
    }

    pthread_mutex_unlock(&pmutex);
    /* ---- ---- ---- ---- ---- */
  }
}

void s_processing_request(void* Arguments) {
  struct epoll_event* event = (struct epoll_event*)Arguments;

  while(1) {
    /* ---- Scan request ---- */
    char *request = s_scan_request((int) event->data.fd);
    if (request == NULL) {
      printf("Wrong request.\n");
      return;
    }
    /* ---- ---- ---- ---- ---- */

    /* ---- Parse request ---- */
    char *cmd = strdup(strtok(request, " "));
    char *path = strdup(strtok(NULL, " "));

    if (strcmp(cmd, "GET") != 0) {
      printf("Wrong command %s.\n", cmd);
      free(request);
      return;
    }

    path[strlen(path)] = '\0';
    printf("Got path: %s\n", path);
    /* ---- ---- ---- ---- ---- */

    /* ---- Processing ---- */
    printf("Write response into rx(%d)\n", event->data.u32);
    write((int) event->data.u32, "Hello!", 6);
    printf("Response was sent.\n");
    /* ---- ---- ---- ---- */

    break;
  }
  printf("Good job!\n");
}

int main() {
  /* ---- START ---- */
  s_prepare();
  printf("Server: started.\n");

  /* ---- REGISTRATION ---- */
  s_register_clients();

  return 0;
}

