#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>

void print_instruction(const char* cmd);
void parse_console_args(int argc, const char* argv[]);
