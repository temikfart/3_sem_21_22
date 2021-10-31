#include "fifo_server.h"

char fifo_name[100] = "\0";

int MakeFifoName(pid_t pid, char *name, size_t name_max) {
  snprintf(name, name_max, "%sfifo%d", CHANNELS_REL_PATH, pid);
  return 0;
}
int Open(const char *path, int flag) {
  int fd = open(path, flag);
  if (fd == -1) {
    perror("fd wasn't opened");
    exit(1);
  }
  return fd;
}
void Read(int fd, void *buf, size_t nbytes) {
  ssize_t nread = read(fd, buf, nbytes);
  if (nread == -1) {
    perror("read error");
    exit(1);
  }
  if (nread == 0) {
    printf("Network down\n");
    exit(1);
  }
}
int GetStarted(int fd_server, struct simple_message *msg) {
  // Receiving and processing msgs
  int fd_client;
  // Receiving msg
  Read(fd_server, msg, sizeof(*msg));
  printf("Msg received: %s\nProcessing..\n", msg->sm_data);

  // Sending results of processing
  MakeFifoName(msg->sm_clientpid, fifo_name, sizeof(fifo_name));
  printf("fifo_name: %s\n", fifo_name);

  struct simple_message ready_msg;
  fd_client = Open(fifo_name, O_WRONLY);
  strcpy(ready_msg.sm_data, READY_MSG);
  write(fd_client, &ready_msg, sizeof(ready_msg));

  return fd_client;
}

int main() {
  printf("Server is running\n");
  if (mkfifo(SERVER_FIFO_NAME, 0666) == -1 && errno != EEXIST) {
    perror("mkfifo error");
    exit(1);
  }

  // Open file descriptors
  int fd_server, fd_server_w;
  fd_server = Open(SERVER_FIFO_NAME, O_RDONLY);
  // Additional fd: to be running always
  fd_server_w = Open(SERVER_FIFO_NAME, O_WRONLY);

  // Make connection
  struct simple_message msg;
  int fd_client = GetStarted(fd_server, &msg);

  // Start upload file
//  int fin = Open(msg.sm_data, O_RDONLY);
  printf("Try to open source file..\n");
  int fin = Open(msg.sm_data, O_RDONLY);
  printf("Server is starting to send data from the file\n");

  size_t read_sz;
  int i = 1;
  while (1) {
    // Read data from file and write in msg
    read_sz = read(fin, msg.sm_data+PROT_SZ, MAX_READ_SZ);

    if (read_sz != MAX_READ_SZ) {
      if (read_sz != 0) {
        // Send last msg
        msg.sm_data[0] = (char)read_sz;
        write(fd_client, msg.sm_data, read_sz + PROT_SZ);
//        printf("Server sent %d msg with %d(%ld) symbols: %s\n",
//               i++, msg.sm_data[0], read_sz, msg.sm_data+PROT_SZ);
      }
      // End of sending
      printf("Server: EOF!\n");

      break;
    } else {
      // Msg sending
      msg.sm_data[0] = (char)read_sz;
      write(fd_client, msg.sm_data, read_sz + PROT_SZ);
//      printf("Server sent %d msg with %d(%ld) symbols: %s\n",
//             i++, msg.sm_data[0], read_sz, msg.sm_data+PROT_SZ);
    }
  }

  close(fin);

  close(fd_client);
  close(fd_server);

  printf("Server finished work.\n");

  return 0;
}
