#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

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
        if (i > 0)
          dup2(fd_in, 0); //stdin <- read from fd_in (dup и dup2(int oldfd, int newfd) создают копию файлового дескриптора oldfd)
        if (cmd[i+1] != NULL)
          dup2(p[1], 1); //stdout -> write to pipe
        close(p[0]);
        execvp((cmd)[i][0], cmd[i]);
        exit(2);
    } else {
      wait(NULL);
      close(p[1]);
      fd_in = p[0]; //fd_in <--read from pipe
        i++;
    }
}
  return;
}

int main()
{
  char *ls[] = {"/bin/ls","-ltr",".", NULL};
  char *grep1[] = {"grep","rw",NULL};
  char *grep2[] = {"grep","1", NULL};
  char **cmd[] = {ls, grep1, grep2, NULL};
  seq_pipe(cmd);
}
