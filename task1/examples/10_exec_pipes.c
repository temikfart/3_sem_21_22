// note: code is not optimal!

#include <stdlib.h>
#include <unistd.h>

void seq_pipe(char ***cmd)
{
  int   p[2];
  pid_t pid;
  int   fd_in = 0;
  int   i = 0;

  while (cmd[i] != NULL) {
    pipe(p);
    if ((pid = fork()) == -1) {
          exit(1);
    } else if (pid == 0) {
        dup2(fd_in, 0); //stdin -> read from pipe
        if (cmd[i+1] != NULL)
          dup2(p[1], 1); //stdout -> write to pipe
        close(p[0]);
        execvp((cmd)[i][0], cmd[i]);
        exit(2);
    } else {
      wait(NULL);
      close(p[1]);
      fd_in = p[0];
        i++;
    }
}
  return;
}

int main()
{
  char *ls[] = {"ls", NULL};
  char *wc1[] = {"wc", NULL};
  char *wc2[] = {"wc", NULL};
  char **cmd[] = {ls, wc1, wc2, NULL};
  seq_pipe(cmd);
}
