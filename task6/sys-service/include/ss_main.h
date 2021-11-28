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

#define PATH_LEN 256
#define TIME_MARK_LEN 24
#define MSG_LEN (PATH_LEN - TIME_MARK_LEN)
#define MAX_PERMS 0777

//static const char* path_prefix;

typedef struct Config {
  u_char options;         // 0000 000D; D -- daemon mode
//  char* path;           // Path to the config file
  pid_t pid;              // Tracking pid
} Config;
typedef struct MapsLine {
  char* address;
  char* perms;
  char* offset;
  char* device;
  char* inode;
  char* path;
} MapsLine;

void print_instruction(const char* cmd);
Config parse_console_args(int argc, const char* argv[]);
char* get_time();
void create_log(const char* format, ...);
int preparing();
void configure_service(Config* Conf);
MapsLine parse_maps_line(char* line);
int parse_maps(FILE* maps_file, MapsLine* PML);
int start_service(pid_t tr_pid);
