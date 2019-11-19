#include "svmsg_file.h"

#define SVMSG_SERVER_FILE "/tmp/svmsg_server_file"

const int buff_len = 128;

static int client_id;

static void remove_queue()
{
  if (msgctl(client_id, IPC_RMID, NULL) == -1) {
    errExit("msgctl");
  }
}

static void handler(int sig)
{
  if (sig == SIGALRM) {
    printf("Caught SIGALRM\n"); // UNSAFE (see Section 21.1.2)
  }
}

int main(int argc, char *argv[])
{
  struct response_msg resp;
  struct request_msg req;
  int server_id, num_msgs;
  ssize_t msg_len, tot_bytes;
  int fd;
  char buff[buff_len];
  struct sigaction sa;
  int saved_errno, ret;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s pathname\n", argv[0]);
  }

  if (strlen(argv[1]) > sizeof(req.pathname) - 1) {
    cmdLineErr("pathname too long (max: %ld bytes)\n",
               (long)sizeof(req.pathname) - 1);
  }

  // Get server's queue identifier; crate queue for response

  fd = open(SVMSG_SERVER_FILE, O_RDWR | O_CREAT | O_SYNC, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    errExit("open");
  }

  if (read(fd, buff, buff_len) == -1) {
    close(fd);
    errExit("read");
  }

  server_id = atoi(buff);

  client_id = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IWGRP);
  if (client_id == -1) {
    errExit("msgget - client message queue");
  }

  if (atexit(remove_queue) != 0) {
    errExit("atexit");
  }

  sigemptyset(&sa.sa_mask);
  sa.sa_handler = handler;
  if (sigaction(SIGALRM, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  // Send message asking for file named in argv[1]

  req.mtype = 1;                // Any type will do
  req.client_id = client_id;
  strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);
  req.pathname[sizeof(req.pathname) - 1] = '\0'; // Ensure string is terminated

  alarm(5);

  ret = msgsnd(server_id, &req, REQ_MSG_SIZE, 0);

  saved_errno = errno;
  alarm(0);
  errno = saved_errno;

  if (ret == -1) {
    if (errno == EINTR) {
      printf("msgsnd timed out\n");
      exit(EXIT_FAILURE);
    }
    else {
      errExit("msgsnd");
    }
  }

  // Get first response, which may be failure notification

  getchar();

  alarm(5);

  msg_len = msgrcv(client_id, &resp, RESP_MSG_SIZE, 0, 0);

  saved_errno = errno;
  alarm(0);
  errno = saved_errno;

  if (msg_len == -1) {
    if (errno == EINTR) {
      printf("msgrcv timed out\n");
      exit(EXIT_FAILURE);
    }
    else {
      errExit("msgrcv");
    }
  }

  if (resp.mtype == RESP_MT_FAILURE) {
    printf("%s\n", resp.data);  // Display msg from server
    exit(EXIT_SUCCESS);
  }

  // File was opened successfully by server; process message
  // (including the one already received) containing file data

  tot_bytes = msg_len;
  for (num_msgs = 1; resp.mtype == RESP_MT_DATA; ++num_msgs) {
    msg_len = msgrcv(client_id, &resp, RESP_MSG_SIZE, 0, 0);
    if (msg_len == -1) {
      errExit("msgrcv");
    }

    tot_bytes += msg_len;
  }

  printf("Received %ld bytes (%d messages)\n", (long)tot_bytes, num_msgs);

  exit(EXIT_SUCCESS);
}
