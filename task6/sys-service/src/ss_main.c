#include "ss_main.h"

int main(int argc, const char* argv[]) {
  Config Conf = parse_console_args(argc, argv);

  printf("Enter PID, which you want to track: ");
  scanf("%d", &Conf.pid);
  printf("\nPID %d received.\n", Conf.pid);

  configure_service(&Conf);
  
  if (Conf.options == 0) {    // Interactive mode
    if (start_service(Conf.pid) == -1) {
      create_log("Starting service failed.");
      return 1;
    }
  }

  return 0;
}
