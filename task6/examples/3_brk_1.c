/* This is the simplest malloc emplementation ever; 
   try to allocate more than XX (configurable heuristic) Mb by brk -- you will probably get segfault */
#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>

int main() {
    void *b;
    char *p, *end;
    unsigned int limit = 0x1000000;
    b = sbrk(0);
    p = (char *)b;
    end = p + limit;
    brk(end);
    while (p < end) {
        *(p++) = 0;
    }
    brk(b);
    return 0;
}
