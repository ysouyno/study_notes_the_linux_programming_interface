#include "svmsg_file.h"

int main(int argc, char *argv[])
{
  struct response_msg resp;
  struct request_msg req;
  int server_id;
  ssize_t msg_len;
  pid_t pid;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s [seq-len...]\n", argv[0]);
  }

  // Get server's queue identifier; crate queue for response

  server_id = msgget(SERVER_KEY, S_IWUSR);
  if (server_id == -1) {
    errExit("msgget - server message queue");
  }

  pid = getpid();

  req.mtype = 1;
  req.client_id = pid;
  strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);
  req.pathname[sizeof(req.pathname) - 1] = '\0'; // Ensure string is terminated

  if (msgsnd(server_id, &req, REQ_MSG_SIZE, 0) == -1) {
    errExit("msgsnd");
  }

  // Get first response, which may be failure notification

  msg_len = msgrcv(server_id, &resp, RESP_MSG_SIZE, getpid(), 0);
  if (msg_len == -1) {
    errExit("msgrcv");
  }

  printf("Client get response from server: %d\n", atoi(resp.data));

  exit(EXIT_SUCCESS);
}
