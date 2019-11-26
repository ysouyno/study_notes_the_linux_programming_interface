#include <mqueue.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

static void usage_error(const char *prog_name)
{
  fprintf(stderr, "Usage: %s [-n] mq-name\n", prog_name);
  fprintf(stderr, "    -n Use O_NONBLOCK flag\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
  int flags, opt;
  mqd_t mqd;
  unsigned int prio;
  void *buffer;
  struct mq_attr attr;
  ssize_t num_read;

  flags = O_RDONLY;
  while ((opt = getopt(argc, argv, "n")) != -1) {
    switch (opt) {
    case 'n':
      flags |= O_NONBLOCK;
      break;
    default:
      usage_error(argv[0]);
    }
  }

  if (optind >= argc) {
    usage_error(argv[0]);
  }

  mqd = mq_open(argv[optind], flags);
  if (mqd == (mqd_t)-1) {
    errExit("mq_open");
  }

  if (mq_getattr(mqd, &attr) == -1) {
    errExit("mq_getattr");
  }

  buffer = malloc(attr.mq_msgsize);
  if (buffer == NULL) {
    errExit("malloc");
  }

  num_read = mq_receive(mqd, buffer, attr.mq_msgsize, &prio);
  if (num_read == -1) {
    errExit("mq_receive");
  }

  printf("Read %ld bytes: priority = %u\n", (long)num_read, prio);

  if (write(STDOUT_FILENO, buffer, num_read) == -1) {
    errExit("write");
  }
  write(STDOUT_FILENO, "\n", 1);

  exit(EXIT_SUCCESS);
}
