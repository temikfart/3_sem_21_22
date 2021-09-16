#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 2048*1024

char ** scan_cmd(int * num) {
  char ** res = NULL;

  // Разбор аргументов
  char delim[] = "|";
  char * cmds = malloc(BUFFER_SIZE);

  // Считывание большой команды и обработка ошибок
  if(NULL == fgets(cmds, BUFFER_SIZE, stdin)) {
    char * ans = malloc(50);
    sprintf(ans, "Incorrect command reading.");
    perror(ans);
    free(ans);
  }

  int i = 0;
  int res_sz = 0;
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

  // Печать массива отдельных команд
  for(int k = 0; k < max_elem; k++) {
    printf("buf[%d]: %s\n", k, buf[k]);
  }

//  const pid_t pid = fork();
//
//  // Обработка ошибок
//  if (pid < 0) {
//    printf("fork() error\n");
//    return -1;
//  }
//
//  if (pid > 0) {
//    int status;
//    waitpid(pid, &status, 0);
//    printf("Ret code: %d\n", WEXITSTATUS(status));
//  } else {
////    execl("/bin/pwd", "pwd", NULL);
//    for (char c = 'n'; tolower(c) != 'y'; scanf("%c", &c)) {
//      printf("Kill me?(y/n):\n");
//    }
//    exit(42);
//  }

  for(int i = 0; i < max_elem; i++) {
    free(buf[i]);
  }
  free(buf);
  return 0;
}
