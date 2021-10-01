#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "dp_main.h"

size_t f_length(String *self) {
  return strlen(self->data);
}
void f_delete(String *self) {
  free(self->data);
  free(self);
}
String *ctorString(int data_sz) {
  String *str = malloc(sizeof(String));
  str->data = calloc(data_sz, sizeof(char));

  // Methods
  str->len = f_length;
  str->delete = f_delete;

  return str;
}

int main() {
  String* str1 = ctorString(BUF_SZ);
  String* str2 = ctorString(BUF_SZ);
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
      // The first fd --- read end
      close(fd1[1]);
      read(fd1[0], str1->data, BUF_SZ);
      printf("(C) Received: %s\n", str1->data);

      // The second fd --- write end
      close(fd2[0]);
      strcat(str2->data, "Hi there!");
      write(fd2[1], str2->data, str2->len(str2));
      printf("(C) Send: %s\n", str2->data);

      break;
    default:    /* Parent process */
      // The first fd --- write end
      close(fd1[0]);
      strcat(str1->data, "Hello");
      write(fd1[1], str1->data, str1->len(str1));
      printf("(P) Send: %s\n", str1->data);

      // The second fd --- read end
      close(fd2[1]);
      read(fd2[0], str2->data, BUF_SZ);
      printf("(P) Received: %s\n", str2->data);

      break;
  }

  str1->delete(str1);
  str2->delete(str2);
  return 0;
}
