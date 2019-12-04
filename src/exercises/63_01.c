#include <sys/select.h>
#include <time.h>
#include <poll.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  int num_pipes, j, ready, rand_pipe, num_writes;
  int (*pfds)[2]; // File descriptors for all pipes
  fd_set readfds;
  int nfds, fd;
  struct timeval timeout;
  struct timeval *pto;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s num-pipes [num-writes]\n", argv[0]);
  }

  // Allocate the arrays that we use. The arrays are sized according
  // to the number of pipes specified on command line

  num_pipes = getInt(argv[1], GN_GT_0, "num-pipes");

  pfds = calloc(num_pipes, sizeof(int[2]));
  if (pfds == NULL) {
    errExit("calloc");
  }

  // Create the number of pipes specified on command line

  for (j = 0; j < num_pipes; ++j) {
    if (pipe(pfds[j]) == -1) {
      errExit("pipe %d", j);
    }
  }

  // Perform specified number of writes to random pipes

  num_writes = (argc > 2) ? getInt(argv[2], GN_GT_0, "num-writes") : 1;

  srandom((int)time(NULL));
  for (j = 0; j < num_writes; ++j) {
    rand_pipe = random() % num_pipes;
    printf("Writing to fd: %3d (read : %3d)\n",
           pfds[rand_pipe][1], pfds[rand_pipe][0]);

    if (write(pfds[rand_pipe][1], "a", 1) == -1) {
      errExit("write %d", pfds[rand_pipe][1]);
    }
  }

  // Build the file descriptor list to be supplied to poll(). This list
  // is set to contain the file descriptors for the read ends of all of
  // the pipes.

  nfds = 0;
  FD_ZERO(&readfds);

  pto = &timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

  for (j = 0; j < num_pipes; ++j) {
    if (pfds[j][0] >= nfds) {
      nfds = pfds[j][0] + 1;
    }

    FD_SET(pfds[j][0], &readfds);
  }

  ready = select(nfds, &readfds, NULL, NULL, pto);
  if (ready == -1) {
    errExit("select");
  }

  printf("select() returned: %d\n", ready);

  // Check which pipes have available for reading

  for (fd = 0; fd < nfds; fd++) {
    if (FD_ISSET(fd, &readfds)) {
      printf("Readable: %3d\n", fd);
    }
  }

  exit(EXIT_SUCCESS);
}
