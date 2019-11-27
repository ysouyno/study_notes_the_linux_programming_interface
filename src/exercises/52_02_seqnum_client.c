#include <mqueue.h>
#include <fcntl.h>
#include "fifo_seqnum.h"

#define CLIENT_MQ_TEMPLATE "/seqnum_cl.%ld"
#define CLIENT_MQ_NAME_LEN (sizeof(CLIENT_MQ_TEMPLATE) + 20)
#define SERVER_MQ_NAME "/mq_seqnum"

static char client_mq[CLIENT_MQ_NAME_LEN];

static void remove_mq() // Invoked on exit to delete client MQ
{
  mq_unlink(client_mq);
}

int main(int argc, char *argv[])
{
  struct request req;
  struct response resp;
  mqd_t mqd_client, mqd_server;
  int num_read;
  unsigned int prio;
  struct mq_attr attr;

  if (argc > 1 && strcmp(argv[1], "--help") == 0) {
    usageErr("%s [seq-len...]\n", argv[0]);
  }

  // Create our MQ (before sending request, to avoid a race)

  snprintf(client_mq, CLIENT_MQ_NAME_LEN, CLIENT_MQ_TEMPLATE,
           (long)getpid());

  attr.mq_msgsize = sizeof(struct response);
  attr.mq_maxmsg = 10;

  mqd_client = mq_open(client_mq, O_CREAT, S_IRUSR | S_IWUSR, &attr);
  if (mqd_client == (mqd_t)-1) {
    errExit("mq_open client");
  }

  if (atexit(remove_mq) != 0) {
    errExit("atexit");
  }

  // Construct request message, open server MQ, and send request

  req.pid = getpid();
  req.seq_len = (argc > 1) ? getInt(argv[1], GN_GT_0, "seq-len") : 1;

  mqd_server = mq_open(SERVER_MQ_NAME, O_WRONLY);
  if (mqd_server == (mqd_t)-1) {
    errExit("mq_open server");
  }

  if (mq_send(mqd_server, &req, sizeof(struct request), 0) == -1) {
    errExit("mq_send");
  }

  num_read = mq_receive(mqd_client, &resp, sizeof(struct response), &prio);
  if (num_read == -1) {
    errExit("mq_receive");
  }

  printf("%d\n", resp.seq_num);
  exit(EXIT_SUCCESS);
}
