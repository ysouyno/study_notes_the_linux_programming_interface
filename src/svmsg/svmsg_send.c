#include <sys/types.h>
#include <sys/msg.h>
#include "tlpi_hdr.h"

#define MAX_MTEXT 1024

struct mbuf
{
  long mtype;
  char mtext[MAX_MTEXT];
};

static void usage_error(const char *prog_name, const char *msg)
{
  if (msg != NULL) {
    fprintf(stderr, "%s", msg);
  }

  fprintf(stderr, "Usage: %s [-n] msqid msg-type [msg-text]\n", prog_name);
  fprintf(stderr, "    -n          Use IPC_NOWAIT flag\n");

  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
  int msqid, flags, msg_len;
  struct mbuf msg;
  int opt; // Option character from getopt()

  flags = 0;

  while ((opt = getopt(argc, argv, "n")) != -1) {
    if (opt == 'n') {
      flags |= IPC_NOWAIT;
    }
    else {
      usage_error(argv[0], NULL);
    }
  }

  if (argc < optind + 2 || argc > optind + 3) {
    usage_error(argv[0], "Wrong number of argument\n");
  }

  msqid = getInt(argv[optind], 0, "msqid");
  msg.mtype = getInt(argv[optind + 1], 0, "msg-type");

  if (argc > optind + 2) { // `msg-text` was supplied
    msg_len = strlen(argv[optind + 2]) + 1;
    if (msg_len > MAX_MTEXT) {
      cmdLineErr("msg-text too long (max: %d characters)\n",
                 MAX_MTEXT);
    }

    memcpy(msg.mtext, argv[optind + 2], msg_len);
  }
  else { // No `msg-text` => zero-length msg
    msg_len = 0;
  }

  // Send message

  if (msgsnd(msqid, &msg, msg_len, flags) == -1) {
    errExit("msgsnd");
  }

  exit(EXIT_SUCCESS);
}
