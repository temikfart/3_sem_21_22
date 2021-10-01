#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "dp_main.h"
#include "dp_string.h"
#include "dp_pipe.h"

int main() {
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
