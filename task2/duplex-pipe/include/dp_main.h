#pragma once

#define BUF_SZ 1024*1024

typedef struct pString String;
typedef struct pString {
  char* data;
  // Methods:
  size_t (*len)(String *self);
  void (*delete)(String *self);
} String;

// String methods
size_t f_length(String *self);
void f_delete(String *self);

String *ctorString(int data_sz);

