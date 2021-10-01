#include <stdlib.h>
#include "dp_pipe.h"
#include "dp_string.h"

void p_clear(Pipe* Pipe) {
  Pipe->buf.clear(&Pipe->buf);
}
size_t p_size(Pipe* Pipe) {
  return Pipe->buf.len(&Pipe->buf);
}

Pipe ctorPipe() {
  Pipe Pipe;
  Pipe.buf = ctorString(BUF_SZ);

  Pipe.clear = p_clear;
  Pipe.size = p_size;

  return Pipe;
}
