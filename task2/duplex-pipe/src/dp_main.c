#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "dp_main.h"

int main() {
  char str1[100] = "\0";
  char str2[100] = "\0";
  int fd1[2], fd2[2];
  if ( (pipe(fd1) < 0) || (pipe(fd2) < 0) ) {
    perror("Invalid pipe");
    return 1;
  }

  switch (fork()) {
    case -1:
      perror("Invalid fork");
      return 1;
    case 0:     /* Child process */
      // The first fd:
      close(fd1[1]);
      read(fd1[0], str1, sizeof(str1));
      printf("(C) Received: %s\n", str1);
      // The second fd:
      close(fd2[0]);
      strcat(str2, "Hi there!");
      write(fd2[1], str2, sizeof(str2));
      printf("(C) Send: %s\n", str2);
      break;
    default:    /* Parent process */
      // The first fd:
      close(fd1[0]);
      strcat(str1, "Hello");
      write(fd1[1], str1, sizeof(str1));
      printf("(P) Send: %s\n", str1);
      // The second fd:
      close(fd2[1]);
      read(fd2[0], str2, sizeof(str2));
      printf("(P) Received: %s\n", str2);
      break;
  }

  return 0;
}
