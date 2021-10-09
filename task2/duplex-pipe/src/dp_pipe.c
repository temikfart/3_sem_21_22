#include <stdlib.h>
#include "dp_pipe.h"
#include "dp_string.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void p_snd(Pipe *self) {
  close(self->fd[0]);
  FILE *fin = fopen(self->fp_r, "r");

  int i = 1;
  size_t last_sym = BUF_SZ - 3;
  int num_pcg = 0;
  while(last_sym == BUF_SZ - 3) {
    self->buf.data[0] = (char)(i++);
    i %= 128;
    self->buf.data[1] = '0';
    self->buf.data[2] = '0';
    last_sym = fread(self->buf.data+3, sizeof(char), BUF_SZ - 3, fin);
    /* Max data size is 254 packages of 256 bytes.
     * the first symbol in the data is the number of package (0-255),
     * the second symbol is the number of packages (0-255),
     * then the number of symbols in the last package. */
    num_pcg = (int)(last_sym / PCG_SZ + 1);
    self->buf.data[1] = (char)num_pcg;
    self->buf.data[2] = 1;
    if (last_sym % PCG_SZ != 0) {
      self->buf.data[2] = (char)(last_sym % PCG_SZ + 1);
    }
    write(self->fd[1], self->buf.data, last_sym+3);
  }

  fclose(fin);
  close(self->fd[1]);
}
void p_rcv(Pipe *self) {
  close(self->fd[1]);
  FILE *fout = fopen(self->fp_w, "w");

  int j, i = 1;
  size_t length;
  u_char num_pcg, num_l_pcg;
  while(read(self->fd[0], self->buf.data, BUF_SZ) > 0) {
    j = (int)self->buf.data[0];
    if (j == i) {
      i++;
      i %= 128;
      num_pcg = (u_char)(self->buf.data[1] - 1);
      num_l_pcg = (u_char)(self->buf.data[2] - 1);
      length = num_pcg * PCG_SZ + num_l_pcg;
      fwrite(self->buf.data+3, sizeof(char), length, fout);
    }
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
