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

  if ( (pipe(Pipe_PtoC.fd) < 0) || (pipe(Pipe_CtoP.fd) < 0) ) {
    perror("Invalid pipe");
    exit(1);
  }

  switch (fork()) {
    case -1:
      perror("Invalid fork");
      return 1;
    case 0:     /* Child process */
      // The first fd --- read end
      close(Pipe_PtoC.fd[1]);
      read(Pipe_PtoC.fd[0], Pipe_PtoC.buf.data, BUF_SZ);
      printf("(C) Received: %s\n", Pipe_PtoC.buf.data);

      // The second fd --- write end
      close(Pipe_CtoP.fd[0]);
      scanf("%s", Pipe_CtoP.buf.data);
      write(Pipe_CtoP.fd[1],
            Pipe_CtoP.buf.data,
            Pipe_CtoP.buf.len(&Pipe_CtoP.buf));
      printf("(C) Send: %s\n", Pipe_CtoP.buf.data);

      break;
    default:    /* Parent process */
      // The first fd --- write end
      close(Pipe_PtoC.fd[0]);
      scanf("%s", Pipe_PtoC.buf.data);
      write(Pipe_PtoC.fd[1],
            Pipe_PtoC.buf.data,
            Pipe_PtoC.buf.len(&Pipe_PtoC.buf));
      printf("(P) Send: %s\n", Pipe_PtoC.buf.data);

      // The second fd --- read end
      close(Pipe_CtoP.fd[1]);
      read(Pipe_CtoP.fd[0], Pipe_CtoP.buf.data, BUF_SZ);
      printf("(P) Received: %s\n", Pipe_CtoP.buf.data);

      break;
  }

  Pipe_PtoC.clear(&Pipe_PtoC);
  Pipe_CtoP.clear(&Pipe_CtoP);
  return 0;
}
