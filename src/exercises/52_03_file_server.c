#include "52_03.h"

static void grim_reaper(int sig)
{
  int saved_errno;

  saved_errno = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0) {
    continue;
  }
  errno = saved_errno;
}

static void serve_request(const struct requ_msg *requ)
{
  struct resp_msg resp;
  ssize_t num_read;
  int fd;
  mqd_t mqd;
  int tot_bytes;

  mqd = mq_open(requ->client_mq_name, O_WRONLY);
  if (mqd == (mqd_t)-1) {
    errExit("mq_open client");
  }

  fd = open(requ->pathname, O_RDONLY);
  if (fd == -1) {
    resp.type = RESP_MT_FAILURE;
    snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
    mq_send(mqd, &resp, RESP_SIZE, 0);
    exit(EXIT_FAILURE);
  }

  // Transmit file contents in messages with type RESP_MT_DATA. We don't
  // diagnose read() and msgsnd() errors since we can't notify client.

  tot_bytes = 0;
  resp.type = RESP_MT_DATA;
  while ((num_read = read(fd, resp.data, DATA_SIZE)) > 0) {
    if (mq_send(mqd, &resp, num_read + sizeof(resp.type), 0) == -1) {
      errMsg("mq_send");
      break;
    }

    tot_bytes += num_read;
  }
  printf("Sent %ld bytes\n", (long)tot_bytes);

  // Send a message of type RESP_MT_END to signify end-of-file

  resp.type = RESP_MT_END;
  mq_send(mqd, &resp, RESP_SIZE, 0);
}

int main(int argc, char *argv[])
{
  struct requ_msg req;
  pid_t pid;
  ssize_t msg_len;
  struct sigaction sa;
  mqd_t mqd;
  struct mq_attr attr;
  unsigned int prio;

  attr.mq_msgsize = REQU_SIZE;
  attr.mq_maxmsg = 10;
  mqd = mq_open(SERVER_MQ_NAME, O_CREAT, S_IRUSR | S_IWUSR, &attr);
  if (mqd == (mqd_t)-1) {
    errMsg("mq_open server");
  }

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = grim_reaper;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  // Read requests, handle each in a separate child process

  for (; ; ) {
    msg_len = mq_receive(mqd, &req, REQU_SIZE, &prio);
    if (msg_len == -1) {
      if (errno == EINTR) {
        continue;
      }

      errMsg("mq_receive");
      break;
    }

    pid = fork();
    if (pid == -1) {
      errMsg("fork");
      break;
    }

    if (pid == 0) {
      serve_request(&req);
      _exit(EXIT_SUCCESS);
    }

    // Parent loops to receive next client request
  }

  if (mq_unlink(SERVER_MQ_NAME) == -1) {
    errExit("mq_unlink");
  }

  exit(EXIT_SUCCESS);
}
