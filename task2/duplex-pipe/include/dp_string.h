#pragma once

#define BUF_SZ 1024*1024

typedef struct pString String;
typedef struct pString {
  char *data;

  // Methods:
  size_t (*len)(String *self);
  void (*clear)(String *self);
} String;

// String methods
size_t str_length(String *self);
void str_clear(String *self);

String ctorString(int data_sz);
