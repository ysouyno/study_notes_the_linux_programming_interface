#include <mqueue.h>
#include <fcntl.h>
#include "fifo_seqnum.h"

#define CLIENT_MQ_TEMPLATE "/seqnum_cl.%ld"
#define CLIENT_MQ_NAME_LEN (sizeof(CLIENT_MQ_TEMPLATE) + 20)
#define SERVER_MQ_NAME "/mq_seqnum"

int main(int argc, char *argv[])
{
  char client_mq[CLIENT_MQ_NAME_LEN];
  struct request req;
  struct response resp;
  int seq_num = 0;          // This is our "service"
  mqd_t mqd_client, mqd_server;
  struct mq_attr attr;
  int num_read;
  unsigned int prio;

  // Create well-known MQ, and open it for reading

  attr.mq_msgsize = sizeof(struct request);
  attr.mq_maxmsg = 10;

  mqd_server = mq_open(SERVER_MQ_NAME, O_CREAT, S_IRUSR | S_IWUSR, &attr);
  if (mqd_server == (mqd_t)-1) {
    errExit("mq_open server");
  }

  for (; ; ) {              // Read requests and send responses
    num_read = mq_receive(mqd_server, &req, sizeof(struct request), &prio);
    if (num_read == -1) {
      fprintf(stderr, "Error reading request; discarding\n");
      continue;
    }

    // Open client MQ (previously created by client)

    snprintf(client_mq, CLIENT_MQ_NAME_LEN, CLIENT_MQ_TEMPLATE,
             (long)req.pid);

    mqd_client = mq_open(client_mq, O_WRONLY);
    if (mqd_client == (mqd_t)-1) {
      errMsg("mq_open %s", client_mq);
      continue;
    }

    // Send responses and close MQ

    resp.seq_num = seq_num;

    if (mq_send(mqd_client, &resp, sizeof(struct response), 0) == -1) {
      fprintf(stderr, "Error writing to MQ %s\n", client_mq);
    }

    if (mq_close(mqd_client) == -1) {
      errMsg("mq_close");
    }

    seq_num += req.seq_len; // Update our sequence number
  }
}
