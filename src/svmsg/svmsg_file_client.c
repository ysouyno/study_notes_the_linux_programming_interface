#include "svmsg_file.h"

static int client_id;

static void remove_queue()
{
  if (msgctl(client_id, IPC_RMID, NULL) == -1) {
    errExit("msgctl");
  }
}

int main(int argc, char *argv[])
{
  struct response_msg resp;
  struct request_msg req;
  int server_id, num_msgs;
  ssize_t msg_len, tot_bytes;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s pathname\n", argv[0]);
  }

  if (strlen(argv[1]) > sizeof(req.pathname) - 1) {
    cmdLineErr("pathname too long (max: %ld bytes)\n",
               (long)sizeof(req.pathname) - 1);
  }

  // Get server's queue identifier; crate queue for response

  server_id = msgget(SERVER_KEY, S_IWUSR);
  if (server_id == -1) {
    errExit("msgget - server message queue");
  }

  client_id = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IWGRP);
  if (client_id == -1) {
    errExit("msgget - client message queue");
  }

  if (atexit(remove_queue) != 0) {
    errExit("atexit");
  }

  // Send message asking for file named in argv[1]

  req.mtype = 1;                                 // Any type will do
  req.client_id = client_id;
  strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);
  req.pathname[sizeof(req.pathname) - 1] = '\0'; // Ensure string is terminated

  if (msgsnd(server_id, &req, REQ_MSG_SIZE, 0) == -1) {
    errExit("msgsnd");
  }

  // Get first response, which may be failure notification

  msg_len = msgrcv(client_id, &resp, RESP_MSG_SIZE, 0, 0);
  if (msg_len == -1) {
    errExit("msgrcv");
  }

  if (resp.mtype == RESP_MT_FAILURE) {
    printf("%s\n", resp.data);                   // Display msg from server
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
