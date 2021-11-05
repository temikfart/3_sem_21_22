/*
This is one of tunable parameters of malloc; it sets the threshold, from which mmap will be called inside malloc.
See more parameters on https://www.gnu.org/software/libc/manual/html_node/Malloc-Tunable-Parameters.html
MMAP_THRESHOLD is 128 kB by default (it was in 2017 at least, as I know)
*/

#include <malloc.h>
#include <stdio.h>

int main() {
   if (!mallopt(M_MMAP_THRESHOLD, 1024*1024)) // returns 0 if fail, else returns 1 https://man7.org/linux/man-pages/man3/mallopt.3.html
      printf("Mallopt error\n");
   return 0;
}
