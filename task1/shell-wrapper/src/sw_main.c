#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "sw_parse.h"

int main() {
  // Получение и парсинг команд
  CommandLine commands = scan_cmds();

  Command child_cmd;
  for(int i = 0; i < commands.size; i++) {
    child_cmd = commands.cmds[i];
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
      int status;
      waitpid(pid, &status, 0);
      printf("(p_pid:%d) Ret code: %d\n", pid, WEXITSTATUS(status));
    } else {
      // Ребёнок
      printf("(c_pid:%d) I'm a child, my cmd is %s!\n", pid, child_cmd.argv[0]);
      // Исполнение
      printf("Executing %s:\n", child_cmd.argv[0]);
      execvp(child_cmd.argv[0], child_cmd.argv);
      // Обработка ошибок exec*
      printf("exec* failed\n");
      exit(42);
    }
  }

  // Освобождение памяти
  free_cmds_mem(commands);

  return 0;
}
