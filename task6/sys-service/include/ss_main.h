#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>

typedef struct Config {
  u_char options;         // 0000 000D; D -- daemon mode
//  char* path;           // path to the config file
  pid_t pid;
} Config;

void print_instruction(const char* cmd);
Config parse_console_args(int argc, const char* argv[]);
void run(Config* Conf);
