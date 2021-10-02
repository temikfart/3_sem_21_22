#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "dp_pipe.h"
#include "dp_main.h"

void load_file(const char *filename) {
  FILE *fin = fopen(filename, "r");
  if (fin == NULL) {
    perror("Can't open file");
    fclose(fin);
    exit(1);
  }
  printf("load_file: success.\n"); // Отладка

  fclose(fin);
  exit(0);
}

int main(int argc, char* argv[]) {
  if (argc == 1) {
    printf("Please, enter the filepath: ");
    char fp[FP_SIZE];
    scanf("%s", fp);
    load_file(fp);
  } else {
    load_file(argv[1]);
  }

  Pipe Pipe_PtoC = ctorPipe();
  Pipe Pipe_CtoP = ctorPipe();

  Pipe_PtoC.pipe(&Pipe_PtoC);
  Pipe_CtoP.pipe(&Pipe_CtoP);

  switch (fork()) {
    case -1:
      perror("Invalid fork");
      return 1;
    case 0:     /* Child process */
      Pipe_PtoC.receive(&Pipe_PtoC);
      Pipe_CtoP.send(&Pipe_CtoP);
      break;
    default:    /* Parent process */
      Pipe_PtoC.send(&Pipe_PtoC);
      Pipe_CtoP.receive(&Pipe_CtoP);
      break;
  }

  Pipe_PtoC.clear(&Pipe_PtoC);
  Pipe_CtoP.clear(&Pipe_CtoP);
  return 0;
}
