#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "sw_parse.h"
#include "sw_run.h"
#include "sw_main.h"

int main() {
  while(1) {
    printf(">>> ");
    // Получение и парсинг команд
    CommandLine commands = scan_cmds();

    run_cmd(commands);
  }

  return 0;
}
