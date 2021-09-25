#pragma once

#define BUFFER_SIZE 2048*1024

typedef struct Command {
  char ** argv;
  int argc;
}Command;

typedef struct CommandLine {
  Command * cmds;
  int size;
}CommandLine;

char ** scan_CmdLine(int * num);
CommandLine scan_cmds();
void free_cmds_mem(CommandLine commands);
