#include <stdlib.h>
#include <string.h>
#include "dp_string.h"

size_t str_length(String *self) {
  return strlen(self->data);
}
void str_clear(String *self) {
  free(self->data);
}

String ctorString(int data_sz) {
  String Str;
  Str.data = calloc(data_sz, sizeof(char));

  // Methods
  Str.len = str_length;
  Str.clear = str_clear;

  return Str;
}
