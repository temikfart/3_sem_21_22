#include "ss_main.h"

void print_instruction(const char* cmd) {
  printf("Usage: %s [PID] [OPTIONS]\n", cmd);
  printf("PID:\n");
  printf("%20s -- pid of tracking process\n", "pid");
  printf("OPTIONS:\n");
  printf("%20s -- start the system service as a daemon\n", "-d, --daemon");
  printf("%20s -- call this instruction\n", "-h, --help");
}

Config parse_console_args(int argc, const char* argv[]) {
  Config Conf;
  Conf.options = 0;

  int flag = 0;
  opterr = 0;

  // if without arguments
  if (argc == 1) {
    return Conf;
  }

  const char optstring[] = "dh";
  const struct option longopts[] =
          {{"daemon", 0, NULL, 'd'},
          {"help", 0, NULL, 'h'}};
  while ((flag = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
    switch (flag) {
      case 'd':
        printf("flag: %s\n", "-d");
        Conf.options |= 1;
        break;
      case 'h':
        print_instruction(argv[0]);
        exit(0);
      default:
        printf("In parse_console_args(): "
               "default condition was activated.\n");
        exit(1);
    }
  }

  // TODO: need to accept file with config.
  // Conf.path = ...

  return Conf;
}
void run(Config* Conf) {
  return;
}

int main(int argc, const char* argv[]) {
  Config Conf = parse_console_args(argc, argv);

  printf("Enter PID, which you want to track: ");
  scanf("%d", &Conf.pid);

  run(&Conf);

  return 0;
}
