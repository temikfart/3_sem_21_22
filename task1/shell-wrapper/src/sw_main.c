#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "sw_parse.h"

int main() {
  // Получение и парсинг команд
  CommandLine commands = scan_cmds();
  int fd[2];
  int fd_in = 0;
  int status;

  for(int i = 0; i < commands.size; i++) {
    // Pipe
    if (pipe(fd) < 0) {
      char *ans = malloc(50);
      sprintf(ans, "Pipe creation is failed");
      perror(ans);
      free(ans);
      exit(1);
    }

    // fork
    const pid_t pid = fork();
    // Обработка ошибок вызова fork()
    if (pid < 0) {
      char *ans = malloc(50);
      sprintf(ans, "fork() unsuccessful");
      perror(ans);
      free(ans);
      exit(1);
    }

    // Родитель
    if (pid > 0) {
      // Check child's status
      waitpid(pid, &status, 0);
//      printf("(p_pid:%d) Ret code: %d\n",
//             pid, WEXITSTATUS(status));                       // Отладка

      // Работа с дескрипторами
      close(fd[1]);
      fd_in = fd[0];
    } else {
      // Ребёнок
//      printf("\n\n(c_pid:%d) I'm a child, my cmd is %s!\n",
//             pid, commands.cmds[i].argv[0]);                // Отладка

      // Работа с дескрипторами
      dup2(fd_in, 0);
      if (i != commands.size - 1) {
        dup2(fd[1], 1);
      }
      close(fd[0]);

      // Исполнение
//      printf("Executing %s:\n", commands.cmds[i].argv[0]);  // Отладка
      execvp(commands.cmds[i].argv[0], commands.cmds[i].argv);
      // Обработка ошибок exec*
      printf("exec* failed\n");
      exit(42);
    }
  }

  // Освобождение памяти
  free_cmds_mem(commands);

  return 0;
}
