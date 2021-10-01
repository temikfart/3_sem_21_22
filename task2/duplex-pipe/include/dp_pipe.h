#pragma once
#include "dp_string.h"

typedef struct pPipe Pipe;
typedef struct pPipe {
  int fd[2];
  String buf;

  // Methods:
//  size_t (*snd)(Pipe *self);
//  size_t (*rcv)(Pipe *self);
  void (*clear)(Pipe* Pipe);
  size_t (*size)(Pipe* Pipe);
} Pipe;

// Pipe methods
//size_t p_snd(Pipe *self);
//size_t p_rcv(Pipe *self);
void p_clear(Pipe* Pipe);
size_t p_size(Pipe* Pipe);

Pipe ctorPipe();
