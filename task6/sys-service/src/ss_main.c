#include "ss_main.h"

//static const char* path_prefix =
//        "/home/temikfart/Desktop/CT/3_sem_21_22/task6/sys-service/";
static int log_fd;

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
void create_log(const char* format, ...) {
  char* log = (char*)(calloc(PATH_LEN, sizeof(char)));
  char* time_mark = get_time();

  strcat(log, time_mark);
  strcat(log, "\t");

  va_list ptr;
  va_start(ptr, format);
  char msg[MSG_LEN];
  vsprintf(msg, format, ptr);
  va_end(ptr);

  strcat(log, msg);
  strcat(log, "\n");

  write(log_fd, log, strlen(log));
  free(log);
}
int preparing() {     // TODO: in the next features, accept Config arg.
  // Open log file, if -d option
//  char* log_path = strdup(path_prefix);
//  log_path = strcat(log_path, "log/log.txt");
  const char log_path[] = "../log/log.txt";
  log_fd = open(log_path,
                O_CREAT | O_RDWR | O_APPEND,
                S_IRUSR | S_IWUSR);
  if (log_fd == -1) {
    printf("Can't open %s\n", log_path);
    perror("Can't open log.txt");
    return -1;
  }

  // ----- Start preparing -----
  create_log("Start preparing");

  // ----- Closing fds -----
  create_log("Closing file descriptors..");
  if (close(fileno(stdin)) == -1
      || close(fileno(stdout)) == -1
      || close(fileno(stderr)) == -1) {
    // Closing file descriptors fail
    create_log("FAIL: %s", strerror(errno));
    return -1;
  } else {
    // Closing file descriptors succeed
    create_log("SUCCEED");
  }

  // ----- Create new session -----
  create_log("Creating new session..");
  pid_t sid = setsid();
  if (sid == -1) {
    // Create new session fail
    create_log("FAIL: %s", strerror(errno));
    return -1;
  } else {
    // Create new session succeed
    create_log("SUCCEED: session id is %ld", sid);
  }

  // ----- Set the file creation permissions -----
  create_log("Set the file creation permissions..");
  umask(0);
  create_log("SUCCEED: Perms: %04o", MAX_PERMS);

  // ----- Change current directory -----
  create_log("Changing current directory..");
  if (chdir("/") == -1) {
    // Changing current directory fail
    create_log("FAIL: %s", strerror(errno));
  }
  create_log("SUCCEED: current directory is \"/\"");

  // ----- COMPLETED -----
  create_log("Preparing is completed successfully\n");

  return 0;
}
void configure_service(Config* Conf) {
  // TODO: parsing Conf->path file and get config

  // TODO: use daemon mode, if it turned on
  if (preparing() == -1) {
    open(stdout, O_WRONLY);
    printf("Preparing failed\n");
    exit(1);
  }
}
void print_parsed_maps_line(MapsLine* PML) {
  create_log("Line: \"%s %s %s %s %s %s\"",
             PML->address, PML->perms, PML->offset,
             PML->device, PML->inode, PML->path);
}
MapsLine parse_maps_line(char* line) {
  MapsLine PML = {NULL, NULL, NULL, NULL, NULL, NULL};
  
  int i = 1;
  char delim[] = " \n";
  for(char* p = strtok(line, delim);
      p != NULL;
      p = strtok(NULL, delim), i++) {
    switch(i) {
      case 1:
        PML.address = strdup(p);
        break;
      case 2:
        PML.perms = strdup(p);
        break;
      case 3:
        PML.offset = strdup(p);
        break;
      case 4:
        PML.device = strdup(p);
        break;
      case 5:
        PML.inode = strdup(p);
        break;
      case 6:
        PML.path = strdup(p);
        break;
      default:    // TODO: return some incorrect PML like -1.
        create_log("Parsing maps line: FAIL");
        break;
    }
  }
  
  return PML;
}
int parse_maps(FILE* maps_file, MapsLine* PML) {
  create_log("Parsing maps file started");
  
  char* buf = NULL;
  size_t len = 0;
  ssize_t read_sz;
  
  size_t PML_sz = 0;
  int count = 0;
  
  while((read_sz = getline(&buf, &len, maps_file)) != -1) {
    create_log("Retrieved line of length %zu", read_sz);
    
    // TODO: Retrieving lines into array
    PML_sz += sizeof(MapsLine);
    PML = realloc(PML, PML_sz);
    PML[count] = parse_maps_line(buf);
    
//    if() {
//      create_log("Parsing of line #%d: FAIL", (count + 1));
//      return -1;
//    }
  
    create_log("Parsing maps line: SUCCEED");
    print_parsed_maps_line(&PML[count]);
    count++;
  }
  
  free(buf);
  return count;
}
int start_service(pid_t tr_pid) {
  // ----- STARTING SERVICE -----
  create_log("Starting service..");
  
  create_log("Tracking pid: %d", tr_pid);
  char maps_path[PATH_LEN] = "/proc/";
  char tr_pid_dir[PATH_LEN];
  
  sprintf(tr_pid_dir, "%d", tr_pid);
  strcat(maps_path, tr_pid_dir);
  strcat(maps_path, "/maps");
  
  create_log("Opening \"%s\"..", maps_path);
  FILE* maps_file = fopen(maps_path, "r");
  if (maps_file == NULL) {
    create_log("FAIL: Can't open %s: %s\n", maps_path, strerror(errno));
    return -1;
  } else {
    create_log("SUCCEED");
  }
  
  create_log("Parsing maps file..");
  // Parse /proc/${PID}/maps for the first time
  MapsLine* ParsedMapsLines = NULL;
  int lines_count = parse_maps(maps_file, ParsedMapsLines);
  if (lines_count == -1) {
    // Parsing maps' lines fail
    create_log("Parsing maps file: FAIL");
    return -1;
  } else {
    create_log("Parsing maps file is completed. "
               "There are %d lines\n\n", lines_count);
  }
  
  
//  fclose(maps_file);
//  create_log("SUCCEED: Service started!");
  return 0;
}

int main(int argc, const char* argv[]) {
  Config Conf = parse_console_args(argc, argv);

  printf("Enter PID, which you want to track: ");
  scanf("%d", &Conf.pid);

  configure_service(&Conf);
  
  if (Conf.options == 0) {    // Interactive mode
    start_service(Conf.pid);
  }

  return 0;
}
