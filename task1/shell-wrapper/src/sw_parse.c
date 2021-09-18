#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sw_parse.h"
#include "sw_main.h"

char ** scan_CmdLine(int * num) {
  char ** res = NULL;

  // Считывание большой команды и обработка ошибок
  char * cmds = malloc(BUFFER_SIZE);
  if (NULL == fgets(cmds, BUFFER_SIZE, stdin)) {
    my_perror("Incorrect command reading");
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
  char delim[] = " \n";
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

    res.cmds[i].argc = j;
  }

  // Отладочная печать разобранных команд
//  printf("Parsed line:\n");
//  for(i = 0; i < res.size; i++) {
//    printf("\t(%d): ", i);
//    for(j = 0; j < res.cmds[i].argc; j++) {
//      printf("%s ", res.cmds[i].argv[j]);
//    }
//    printf("\n");
//  }

  // Освобождение памяти буфера
  for (i = 0; i < max_elem; i++) {
    free(buf[i]);
  }
  free(buf);

  return res;
}

void free_cmds_mem(CommandLine commands) {
  for (int i = 0; i < commands.size; i++) {
    for (int j = 0; j < commands.cmds[i].argc; j++) {
      free(commands.cmds[i].argv[j]);
    }
    free(commands.cmds[i].argv);
  }
  free(commands.cmds);
}
