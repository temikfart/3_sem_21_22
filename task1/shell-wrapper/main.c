#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 2048*1024

typedef struct Command {
  char ** argv;
  int argc;
}Command;

typedef struct CommandLine {
  Command * cmds;
  int size;
}CommandLine;

char ** scan_CmdLine(int * num) {
  char ** res = NULL;

  // Считывание большой команды и обработка ошибок
  char * cmds = malloc(BUFFER_SIZE);
  if (NULL == fgets(cmds, BUFFER_SIZE, stdin)) {
    char * ans = malloc(50);
    sprintf(ans, "Incorrect command reading");
    perror(ans);
    free(ans);
    exit(1);
  }

  int i = 0;
  int res_sz = 0;
  char delim[] = "|";
  for (char *p = strtok(cmds, delim); p != NULL; p = strtok(NULL, delim)) {
    res_sz += sizeof(char *);
    res = realloc(res, res_sz);
    res[i] = strdup(p);
    i++;
  }
  *num = i;

  free(cmds);
  return res;
}

CommandLine scan_cmds() {
  CommandLine res;

  // Чтение массива команд
  int max_elem;
  char ** buf = scan_CmdLine(&max_elem);

  res.size = max_elem;
  int cmds_sz = (int)(res.size * sizeof(Command));
  res.cmds = malloc(cmds_sz);
  int argv_sz[res.size];

  // Инициализация
  for(int a = 0; a < res.size; a++) {
    argv_sz[a] = 1;
    res.cmds[a].argv = NULL;
  }

  // Разбор аргументов
  int i, j;
  char delim[] = " ";
  for(i = 0; i < res.size; i++) {
    j = 0;
    for (char *p = strtok(buf[i], delim); p != NULL; p = strtok(NULL, delim)) {
      argv_sz[i] += sizeof(char *);
      res.cmds[i].argv = realloc(res.cmds[i].argv, argv_sz[i]);
      res.cmds[i].argv[j] = strdup(p);
      j++;
    }
    // Последний элемент argv должен быть NULL
    argv_sz[i] += sizeof(char *);
    res.cmds[i].argv = realloc(res.cmds[i].argv, argv_sz[i]);
    res.cmds[i].argv[j] = NULL;

    res.cmds[i].argc = j-1;
  }

  // Отладочная печать разобранных команд
  printf("Parsed line:\n");
  for(i = 0; i < res.size; i++) {
    printf("\t(%d): ", i);
    for(j = 0; j < res.cmds[i].argc; j++) {
      printf("%s ", res.cmds[i].argv[j]);
    }
    printf("\n");
  }

  // Освобождение памяти буфера
  for (i = 0; i < max_elem; i++) {
    free(buf[i]);
  }
  free(buf);

  return res;
}

void free_cmds_mem(CommandLine commands) {
  for (int i = 0; i < commands.size; i++) {
    // <= т.к. последний NULL
    for (int j = 0; j <= commands.cmds[i].argc; j++) {
      free(commands.cmds[i].argv[j]);
    }
    free(commands.cmds[i].argv);
  }
  free(commands.cmds);
}

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
