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

CommandLine parse_tokens(char ** tokens, int num) {
  CommandLine res;

  res.size = num;
  int cmds_sz = (int)(res.size * sizeof(Command));
  res.cmds = malloc(cmds_sz);
  int argv_sz[res.size];

  // Инициализация
  for(int a = 0; a < res.size; a++) {
    argv_sz[a] = 0;
    res.cmds[a].argv = NULL;
  }

  // Разбор аргументов
  int i, j;
  char delim[] = " ";
  for(i = 0; i < res.size; i++) {
    j = 0;
    for (char *p = strtok(tokens[i], delim); p != NULL; p = strtok(NULL, delim)) {
      argv_sz[i] += sizeof(char *);
      res.cmds[i].argv = realloc(res.cmds[i].argv, argv_sz[i]);
      res.cmds[i].argv[j] = strdup(p);
      j++;
    }
    res.cmds[i].argc = j;
  }


  return res;
}

char ** scan_cmd(int * num) {
  char ** res = NULL;

  // Считывание большой команды и обработка ошибок
  char * cmds = malloc(BUFFER_SIZE);
  if(NULL == fgets(cmds, BUFFER_SIZE, stdin)) {
    char * ans = malloc(50);
    sprintf(ans, "Incorrect command reading.");
    perror(ans);
    free(ans);
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

int main() {
  // Чтение массива команд
  int max_elem;
  char ** buf = scan_cmd(&max_elem);

  // Парсинг команд
  CommandLine commands = parse_tokens(buf, max_elem);
  // Отладочная печать разобранных команд
  printf("Parsed line:\n");
  for(int i = 0; i < max_elem; i++) {
    printf("\t(%d): ", i);
    for(int j = 0; j < commands.cmds[i].argc; j++) {
      printf("%s ", commands.cmds[i].argv[j]);
    }
    printf("\n");
  }

  // Освобождение памяти
  for (int i = 0; i < commands.size; i++) {
    free(buf[i]);
    for (int j = 0; j < commands.cmds[i].argc; j++) {
      free(commands.cmds[i].argv[j]);
    }
    free(commands.cmds[i].argv);
  }
  free(buf);
  free(commands.cmds);
  return 0;
}
