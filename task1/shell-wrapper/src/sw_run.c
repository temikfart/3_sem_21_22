#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "sw_parse.h"
#include "sw_run.h"

void run_cmd(CommandLine commands) {
  int fd[2];
  int fd_in = 0;

  for(int i = 0; i < commands.size; i++) {
    if (pipe(fd) < 0) {
      perror("Pipe creation is failed");
      exit(errno);
    }
    const pid_t pid = fork();
    if (pid < 0) {
      perror("fork() unsuccessful");
      exit(errno);
    }

    // Родитель
    if (pid > 0) {
      fd_in = fd[0];
      close(fd[1]);
    } else {
      dup2(fd_in, 0);
      if (i != commands.size - 1) {
        dup2(fd[1], 1);
      }
      close(fd[0]);

      execvp(commands.cmds[i].argv[0], commands.cmds[i].argv);
      perror("exec* failed");
      exit(errno);
    }
  }
  for(int i = 0; i < commands.size; i++) {
    wait(NULL);
  }

  // Освобождение памяти
  free_cmds_mem(commands);
}
