#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 2048*1024

char ** scan_cmd(int * num) {
  char ** res = malloc(BUFFER_SIZE);

  // Разбор аргументов
  char delim[] = "|";
  char * cmds = malloc(BUFFER_SIZE);

  // Считывание большой команды
  fgets(cmds, BUFFER_SIZE, stdin);

  // Обработка ошибки чтения
  if(ferror(stdin) != 0) {
    printf("Error: incorrect command reading.\n");
    exit(1);
  }

//  printf("\nWhole string: %s", cmds);

  int i = 0;
  for (char *p = strtok(cmds, delim); p != NULL; p = strtok(NULL, delim)) {
    res[i] = p;
//    printf("str%d: %s\n", i, p);
//    printf("str%d: %s\n", i, res[i]);
    i++;
  }
  printf("i = %d\n", i);
  *num = i;
//  printf("num: %d\n", *num);

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

  free(buf);
  return 0;
}
