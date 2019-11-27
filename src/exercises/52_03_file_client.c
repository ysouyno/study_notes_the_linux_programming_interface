#include "52_03.h"

int main(int argc, char *argv[])
{
  struct resp_msg resp;
  struct requ_msg requ;
  int num_msgs;
  ssize_t msg_len, tot_bytes;
  mqd_t mqd_server, mqd_client;
  struct mq_attr attr;
  unsigned int prio;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s pathname\n", argv[0]);
  }

  if (strlen(argv[1]) > sizeof(requ.pathname) - 1) {
    cmdLineErr("pathname too long (max: %ld bytes)\n",
               (long)sizeof(requ.pathname) - 1);
  }

  mqd_server = mq_open(SERVER_MQ_NAME, O_WRONLY);
  if (mqd_server == (mqd_t)-1) {
    errExit("mq_open server");
  }

  // Send message asking for file named in argv[1]

  snprintf(requ.client_mq_name, CLIENT_MQ_NAME_LEN, CLIENT_MQ_NAME,
           (long)getpid());
  strncpy(requ.pathname, argv[1], sizeof(requ.pathname) - 1);
  requ.pathname[sizeof(requ.pathname) - 1] = '\0';

  attr.mq_msgsize = RESP_SIZE;
  attr.mq_maxmsg = 10;
  mqd_client = mq_open(requ.client_mq_name, O_CREAT | O_RDWR,
                       S_IRUSR | S_IWUSR, &attr);
  if (mqd_client == (mqd_t)-1) {
    errExit("mq_open client");
  }

  if (mq_send(mqd_server, &requ, REQU_SIZE, 0) == -1) {
    errExit("mq_send");
  }

  // Get first response, which may be failure notification

  msg_len = mq_receive(mqd_client, &resp, RESP_SIZE, &prio);
  if (msg_len == -1) {
    errExit("mq_receive");
  }

  if (resp.type == RESP_MT_FAILURE) {
    printf("%s\n", resp.data);                   // Display msg from server
    exit(EXIT_SUCCESS);
  }

  // File was opened successfully by server; process message
  // (including the one already received) containing file data

  tot_bytes = 0;
  for (num_msgs = 1; resp.type == RESP_MT_DATA; ++num_msgs) {
    msg_len = mq_receive(mqd_client, &resp, RESP_SIZE, &prio);
    if (msg_len == -1) {
      errExit("mq_receive");
    }

    tot_bytes += msg_len;
    tot_bytes -= sizeof(resp.type);
  }

  printf("Received %ld bytes (%d messages)\n", (long)tot_bytes, num_msgs);

  if (mq_unlink(requ.client_mq_name) == -1) {
    errExit("mq_unlink");
  }

  exit(EXIT_SUCCESS);
}
