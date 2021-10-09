#include <stdio.h>
#include <unistd.h>
#include "dp_pipe.h"
#include "dp_main.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Bad arguments. Try again.\n");
    return 1;
  }

  // Пусть ./duplex-pipe src dst
  Pipe Pipe_PtoC = ctorPipe(argv[1], argv[2]);
  Pipe Pipe_CtoP = ctorPipe(argv[2], argv[1]);

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
