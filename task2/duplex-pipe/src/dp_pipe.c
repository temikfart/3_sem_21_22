#include <stdlib.h>
#include "dp_pipe.h"
#include "dp_string.h"
#include <stdio.h>
#include <unistd.h>

void p_snd(Pipe* self) {
  close(self->fd[0]);
  scanf("%s", self->buf.data);
  write(self->fd[1],
        self->buf.data,
        self->buf.len(&self->buf));
  printf("(P) Send: %s\n", self->buf.data);
}
void p_rcv(Pipe* self) {
  close(self->fd[1]);
  read(self->fd[0], self->buf.data, BUF_SZ);
  printf("(C) Received: %s\n", self->buf.data);
}
void p_clear(Pipe* self) {
  self->buf.clear(&self->buf);
}
size_t p_size(Pipe* self) {
  return self->buf.len(&self->buf);
}
void p_pipe(Pipe* self) {
  if(pipe(self->fd) < 0) {
    perror("Invalid pipe");
    exit(1);
  }
}

Pipe ctorPipe() {
  Pipe Pipe;
  Pipe.buf = ctorString(BUF_SZ);

  Pipe.send = p_snd;
  Pipe.receive = p_rcv;
  Pipe.clear = p_clear;
  Pipe.size = p_size;
  Pipe.pipe = p_pipe;

  return Pipe;
}
