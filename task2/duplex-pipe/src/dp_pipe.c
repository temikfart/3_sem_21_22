#include <stdlib.h>
#include "dp_pipe.h"
#include "dp_string.h"
#include <stdio.h>
#include <unistd.h>

void p_snd(Pipe *self) {
  close(self->fd[0]);
  FILE *fin = fopen(self->fp_r, "r");

  size_t last_sym = BUF_SZ;
  while(last_sym == BUF_SZ) {
    last_sym = fread(self->buf.data, sizeof(char), BUF_SZ, fin);
    write(self->fd[1], self->buf.data, self->buf.len(&self->buf));
  }

  fclose(fin);
  close(self->fd[1]);
}
void p_rcv(Pipe *self) {
  close(self->fd[1]);
  FILE *fout = fopen(self->fp_w, "w");

  while(read(self->fd[0], self->buf.data, BUF_SZ) > 0) {
    fwrite(self->buf.data, sizeof(char), self->buf.len(&self->buf), fout);
  }

  fclose(fout);
  close(self->fd[0]);
}
void p_clear(Pipe *self) {
  self->buf.clear(&self->buf);
}
size_t p_size(Pipe *self) {
  return self->buf.len(&self->buf);
}
void p_pipe(Pipe *self) {
  if(pipe(self->fd) < 0) {
    perror("Invalid pipe");
    exit(1);
  }
}

Pipe ctorPipe(const char *fp_r, const char *fp_w) {
  Pipe Pipe;
  Pipe.buf = ctorString(BUF_SZ);
  Pipe.fp_r = fp_r;
  Pipe.fp_w = fp_w;

  Pipe.send = p_snd;
  Pipe.receive = p_rcv;
  Pipe.clear = p_clear;
  Pipe.size = p_size;
  Pipe.pipe = p_pipe;

  return Pipe;
}
