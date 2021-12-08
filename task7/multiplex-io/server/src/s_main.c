#include "s_main.h"

int start_server() {
  printf("Server: Preparation started.\n");
  
  // ---- Create server_pub ----
  char* server_fifo_path = calloc(MAX_PATH_LEN, sizeof(char));
  strcat(server_fifo_path, FIFO_STORAGE_REL_PATH);
  strcat(server_fifo_path, PUBLIC_SERVER_FIFO_NAME);
  printf("%s\n", server_fifo_path);
  if (mkfifo(server_fifo_path, 0666) == -1) {
    printf("%s\n", __FILE__);
    perror("Can't create server fifo");
    return -1;
  }
  printf("Server: Server's fifo created in %s\n", server_fifo_path);
  
  // ---- Open fifo ----
  int server_fifo_fd = open(server_fifo_path, O_RDWR | O_CREAT);
  if (server_fifo_fd == -1) {
    perror("Can't open server fifo");
    return -1;
  }
  
  return server_fifo_fd;
}

int main() {
  // ---- START ----
  int s_fifo_fd = start_server();
  if (s_fifo_fd == -1) {
    return 1;
  } else {
    printf("SUCCEED: Server started.\n");
  }
  
  // ---- REGISTRATION ----
  fd_set reg;
  FD_ZERO(&reg);
  FD_SET(s_fifo_fd, &reg);
  
  while (1) {
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
  
    int retval;
    retval = select(1, &reg, NULL, NULL, &tv);
    switch (retval) {
      case -1:
        perror("select error");
        return 1;
      case 0:
        printf("Timeout\n");
        break;
      default:
        if (FD_ISSET(s_fifo_fd, &reg)) {
          char request[3 * MAX_PATH_LEN] = "\0";
          read(s_fifo_fd, request, 3 * MAX_PATH_LEN);
          if (strcmp(strtok(request, " "), "REGISTER") != 0) {
            printf("Server: Someone has registered!\n");
            printf("tx: %s\n", strtok(NULL, " "));
            printf("rx: %s\n", strtok(NULL, " "));
          } else {
            printf("Bad command\n");
          }
        }
    }
  }
  
  return 0;
}

