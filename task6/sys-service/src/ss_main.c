#include "ss_main.h"

void print_instruction(const char* cmd) {
  printf("Usage: %s [PID] [OPTIONS]\n", cmd);
  printf("PID:\n");
  printf("%20s -- pid of tracking process\n", "pid");
  printf("OPTIONS:\n");
  printf("%20s -- start the system service as a daemon\n", "-d, --daemon");
}

void parse_console_args(int argc, const char* argv[]) {
  int flag = 0;
  opterr = 0;

  // If without arguments
  if (argc == 1) {
    print_instruction(argv[0]);
    exit(0);
  }

  const char optstring[] = "d";
  const struct option longopts[] =
          {{"daemon", 0, NULL, 'd'}};
  while ((flag = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
    switch (flag) {
      case 'd':
        printf("flag: %s\n", "-d");
        break;
      default:
        printf("In parse_console_args(): "
               "default condition was activated.\n");
        exit(1);
    }
  }

  // TODO: need to accept file with config.
}

int main(int argc, const char* argv[]) {
  parse_console_args(argc, argv);

  return 0;
}
