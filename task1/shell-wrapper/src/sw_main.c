#include <stdio.h>
#include "sw_parse.h"
#include "sw_run.h"

int main() {
  while(1) {
    printf(">>> ");
    // Получение и парсинг команд
    CommandLine commands = scan_cmds();

    run_cmd(commands);
  }

  return 0;
}
