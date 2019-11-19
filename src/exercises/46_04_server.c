#include <syslog.h>
#include "svmsg_file.h"
#include "become_daemon.h"

#define SVMSG_SERVER_FILE "/tmp/svmsg_server_file"

const int buff_len = 128;

static int server_id;

static void grim_reaper(int sig)       // SIGCHLD handler
{
  int saved_errno;

  saved_errno = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0) {
    continue;
  }
  errno = saved_errno;
}

static void handler(int sig)
{
  if (sig == SIGINT || sig == SIGTERM) {
    if (msgctl(server_id, IPC_RMID, NULL) == -1) {
      errExit("msgctl");
    }

    unlink(SVMSG_SERVER_FILE);

    signal(sig, SIG_DFL);
    raise(sig);
  }

  if (sig == SIGALRM) {
    openlog("46_04_server", LOG_CONS | LOG_PID, LOG_SYSLOG);
    syslog(LOG_ERR, "PID: %ld, SIGALRM", (long)getpid());
    closelog();
  }
}

static void serve_request(const struct request_msg *req)
{
  struct response_msg resp;
  ssize_t num_read;
  int fd;
  int saved_errno, ret;

  fd = open(req->pathname, O_RDONLY);
  if (fd == -1) {
    resp.mtype = RESP_MT_FAILURE;
    snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
    msgsnd(req->client_id, &resp, strlen(resp.data) + 1, 0);
    exit(EXIT_FAILURE);
  }

  // Transmit file contents in messages with type RESP_MT_DATA. We don't
  // diagnose read() and msgsnd() errors since we can't notify client.

  alarm(5);

  resp.mtype = RESP_MT_DATA;
  while ((num_read = read(fd, resp.data, RESP_MSG_SIZE)) > 0) {
    if ((ret = msgsnd(req->client_id, &resp, num_read, 0)) == -1) {
      break;
    }
  }

  saved_errno = errno;
  alarm(0);
  errno = saved_errno;

  if (ret == -1) {                     // Indispensable
    if (errno == EINTR) {
      openlog("46_04_server", LOG_CONS | LOG_PID, LOG_SYSLOG);
      syslog(LOG_ERR, "msgsnd timed out");

      if (msgctl(req->client_id, IPC_RMID, NULL) == -1) {
        syslog(LOG_ERR, "msgctl error: %d", errno);
      }

      closelog();
    }
  }

  // Send a message of type RESP_MT_END to signify end-of-file

  resp.mtype = RESP_MT_END;
  msgsnd(req->client_id, &resp, 0, 0); // Zero-length mtext
}

int main(int argc, char *argv[])
{
  struct request_msg req;
  pid_t pid;
  ssize_t msg_len;
  struct sigaction sa;
  int fd;
  char buff[buff_len];

  becomeDaemon(0);

  fd = open(SVMSG_SERVER_FILE, O_RDWR | O_CREAT | O_SYNC, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    errExit("open");
  }

  server_id = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL |
                     S_IRUSR | S_IWUSR | S_IWGRP);
  if (server_id == -1) {
    errExit("msgget");
  }

  sprintf(buff, "%d", server_id);
  if (write(fd, buff, strlen(buff)) == -1) {
    close(fd);
    unlink(SVMSG_SERVER_FILE);
    errExit("write");
  }

  close(fd);

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = grim_reaper;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handler;

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  if (sigaction(SIGALRM, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  // Read requests, handle each in a separate child process

  for (; ; ) {
    msg_len = msgrcv(server_id, &req, REQ_MSG_SIZE, 0, 0);
    if (msg_len == -1) {
      if (errno == EINTR) {            // Interrupted by SIGCHLD handler?
        continue;                      // ... then restart msgrcv()
      }

      errMsg("msgrcv");                // Some other error
      break;                           // ... so terminate loop
    }

    pid = fork();
    if (pid == -1) {
      errMsg("fork");
      break;
    }

    if (pid == 0) {                    // Child handles request
      serve_request(&req);
      _exit(EXIT_SUCCESS);
    }

    // Parent loops to receive next client request
  }

  exit(EXIT_SUCCESS);
}
