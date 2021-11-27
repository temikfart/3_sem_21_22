#include "ss_main.h"

static const char* path_prefix =
        "/home/temikfart/Desktop/CT/3_sem_21_22/task6/sys-service/";

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
char* get_time() {
  time_t now = time(NULL);
  char* time_mark = ctime(&now);
  time_mark[strlen(time_mark)-1] = '\0';

  return time_mark;
}
void create_log(int log_fd, const char* format, ...) {
  char* log = (char*)(calloc(LOG_LEN, sizeof(char)));
  log[0] = '\0';
  char* time_mark = get_time();

  fprintf(stdout, "%s\n", time_mark);
  sprintf(log, "%s", time_mark);
  fprintf(stdout, "%s\n", log);
  sprintf(log, "\t");
  fprintf(stdout, "%s\n", log);

  va_list ptr;
  va_start(ptr, format);
  vsprintf(log, format, ptr);
  va_end(ptr);
  fprintf(stdout, "%s\n", log);

  sprintf(log, "\n");
  fprintf(stdout, "%s\n", log);

  write(log_fd, log, strlen(log));
  free(log);
}
int preparing() {     // TODO: in the next features, accept Config arg.
  // Open log file, if -d option
  char* log_path = strdup(path_prefix);
  log_path = strcat(log_path, "log/log.txt");
  int log_fd = open(log_path, O_CREAT | O_WRONLY | O_APPEND);
  if (log_fd == -1) {
    printf("Can't open %s\n", log_path);
    perror("Can't open log.txt");
    return -1;
  }

  // ----- Start preparing -----
  create_log(log_fd, "Start preparing");

  // ----- Closing fds -----
  create_log(log_fd, "Closing file descriptors..");
  if (close(fileno(stdin)) == -1
      || close(fileno(stdout)) == -1
      || close(fileno(stderr)) == -1) {
    // Closing file descriptors fail
    create_log(log_fd, "Closing file descriptors: FAIL");
    return -1;
  } else {
    // Closing file descriptors succeed
    create_log(log_fd, "Closing file descriptors: SUCCEED");
  }

  // ----- Create new session -----
  create_log(log_fd, "Creating new session..");
  pid_t sid = setsid();
  if (sid == -1) {
    // Create new session fail
    create_log(log_fd, "Creating new session: FAIL");
    return -1;
  } else {
    // Create new session succeed
    create_log(log_fd, "SUCCEED: session id is %ld", sid);
  }

  // ----- Set the file creation permissions -----
  create_log(log_fd, "Set the file creation permissions..");
  umask(0);
  create_log(log_fd, "SUCCEED: Perms: %o", MAX_PERMS);

  // ----- Change current directory -----
  create_log(log_fd, "Changing current directory..");
  chdir("/");
  create_log(log_fd, "SUCCEED: current directory is \"/\"");

  // ----- COMPLETED -----
  create_log(log_fd, "Preparing is completed successfully\n");

  close(log_fd);
  return 0;
}
void configure_service(Config* Conf) {
  // TODO: parsing Conf->path file and get config

  // TODO: use daemon mode, if it turned on
  if (preparing() == -1) {
    open(stdin, O_WRONLY);
    printf("Preparing failed\n");
    exit(1);
  }
}

int main(int argc, const char* argv[]) {
  Config Conf = parse_console_args(argc, argv);

  printf("Enter PID, which you want to track: ");
  scanf("%d", &Conf.pid);

  configure_service(&Conf);

  return 0;
}
