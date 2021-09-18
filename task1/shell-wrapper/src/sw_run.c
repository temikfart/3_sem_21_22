#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "sw_parse.h"
#include "sw_run.h"
#include "sw_main.h"

void run_cmd(CommandLine commands) {
  int fd[2];
  int fd_in = 0;

  for(int i = 0; i < commands.size; i++) {
    if (pipe(fd) < 0) {
      my_perror("Pipe creation is failed");
    }
    const pid_t pid = fork();
    if (pid < 0) {
      my_perror("fork() unsuccessful");
    }

    // Родитель
    if (pid > 0) {
      fd_in = fd[0];
      close(fd[1]);

      if(strcmp(commands.cmds[i].argv[0], "exit") == 0) {
        printf("Goodbye!\nExiting...\n");
        exit(0);
      }
    } else {
      dup2(fd_in, 0);
      if (i != commands.size - 1) {
        dup2(fd[1], 1);
      }
      close(fd[0]);

      execvp(commands.cmds[i].argv[0], commands.cmds[i].argv);
      my_perror("exec* failed");
    }
  }
  for(int i = 0; i < commands.size; i++) {
    wait(NULL);
  }

  // Освобождение памяти
  free_cmds_mem(commands);
}
