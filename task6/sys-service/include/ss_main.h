#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <time.h>

#define LOG_LEN 256
#define MAX_PERMS 0777

static const char* path_prefix;

typedef struct Config {
  u_char options;         // 0000 000D; D -- daemon mode
//  char* path;           // Path to the config file
  pid_t pid;              // Tracking pid
} Config;

void print_instruction(const char* cmd);
Config parse_console_args(int argc, const char* argv[]);
char* get_time();
void create_log(int log_fd, const char* format, ...);
int preparing();
void configure_service(Config* Conf);
