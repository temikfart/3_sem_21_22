#include "mq_main.h"

int main(int argc, char *argv[]) {
  key_t key = ftok(argv[0], 1);
  if (key == -1) {
    perror("ftok error");
    exit(1);
  }

  int msgid = msgget(key, IPC_CREAT | 0666);
  if(msgid == -1) {
    perror("msgget error");
    exit(1);
  }

  pid_t pid = fork();
  if(pid < 0) {
    perror("fork error");
    exit(1);
  }

  if (pid == 0) {
    // Initialization
    Message s_msg;
    s_msg.mtype = STD_MSG_TYPE;
    strcpy(s_msg.mtext, "Hello, world!");

    // Sending msg
    size_t len = strlen(s_msg.mtext);
    msgsnd(msgid, &s_msg, len, 0);
    printf("Sender: msg \"%s\" was sent.\n", s_msg.mtext);

    // Send last msg
    s_msg.mtype = LAST_MSG_TYPE;
    msgsnd(msgid, &s_msg, 0, 0);
  } else {
    Message r_msg;
    while (1) {
      // Receiving msg
      int len = (int)msgrcv(msgid, &r_msg, MSG_TEXT_SZ, 0, 0);
      if (len < 0) {
        printf("len < 0\n");
        exit(1);
      }

      // Check on last msg
      if (r_msg.mtype == LAST_MSG_TYPE) {
        printf("Receiver: last msg was received\n");
        msgctl(msgid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(0);
      } else {
        printf("Receiver: msg \"%s\" was received.\n", r_msg.mtext);
      }
    }
  }

  return 0;
}
