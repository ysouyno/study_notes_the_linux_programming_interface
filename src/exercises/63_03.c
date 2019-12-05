#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/msg.h>
#include <stddef.h>
#include <signal.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 1024

#define MAX_MTEXT 400

struct pbuf
{
  int msqid;
  int len;
  long mtype;
  char mtext[MAX_MTEXT];
};

void handle_svmsg(int msqid, int fd)
{
  struct pbuf pmsg;
  ssize_t msg_len;
  size_t wlen;

  for (; ; ) {
    msg_len = msgrcv(msqid, &pmsg.mtype, MAX_MTEXT, 0, 0);
    if (msg_len == -1) {
      errExit("msgrcv");
    }

    pmsg.msqid = msqid;
    pmsg.len = msg_len;

    wlen = offsetof(struct pbuf, mtext) + msg_len;

    if (write(fd, &pmsg, wlen) != wlen) {
      fatal("partial/failed write to pipe");
    }
  }

}

int main(int argc, char *argv[])
{
  fd_set readfds;
  int ready, nfds, j;
  char buf[BUF_SIZE];
  int pfd[2];
  ssize_t num_read;
  struct pbuf pmsg;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s msqid...\n", argv[0]);
  }

  if (pipe(pfd) == -1) {
    errExit("pipe");
  }

  for (j = 1; j < argc; ++j) {
    switch (fork()) {
    case -1:
      errMsg("fork");
      killpg(0, SIGTERM);
      _exit(EXIT_FAILURE);
    case 0:
      handle_svmsg(getInt(argv[1], 0, "msqid"), pfd[1]);
      _exit(EXIT_FAILURE);
    default:
      break;
    }
  }

  for (; ; ) {
    nfds = 0;
    FD_ZERO(&readfds);

    FD_SET(STDIN_FILENO, &readfds);
    nfds = max(nfds, STDIN_FILENO + 1);

    FD_SET(pfd[0], &readfds);
    nfds = max(nfds, pfd[0] + 1);

    // We've build all of the arguments; now call select()

    ready = select(nfds, &readfds, NULL, NULL, NULL);

    if (ready == -1) {
      errExit("select");
    }

    if (FD_ISSET(STDIN_FILENO, &readfds)) {
      num_read = read(STDIN_FILENO, buf, BUF_SIZE);
      if (num_read == -1) {
        errExit("read stdin");
      }

      printf("%ld bytes from stdin\n", num_read - 1);
    }

    if (FD_ISSET(pfd[0], &readfds)) {
      num_read = read(pfd[0], &pmsg, offsetof(struct pbuf, mtext));
      if (num_read == -1) {
        errExit("read pipe");
      }

      if (num_read == 0) {
        fatal("EOF on pipe");
      }

      num_read = read(pfd[0], &pmsg.mtext, pmsg.len);
      if (num_read == -1) {
        errExit("read pipe");
      }

      if (num_read == 0) {
        fatal("EOF on pipe");
      }

      printf("%ld bytes from pipe\n", num_read - 1);
    }
  }

  exit(EXIT_SUCCESS);
}
