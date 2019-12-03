#include <time.h>
#include <poll.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  int num_pipes, j, ready, rand_pipe, num_writes;
  int (*pfds)[2]; // File descriptors for all pipes
  struct pollfd *poll_fd;

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

  poll_fd = calloc(num_pipes, sizeof(struct pollfd));
  if (poll_fd == NULL) {
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

  for (j = 0; j < num_pipes; ++j) {
    poll_fd[j].fd = pfds[j][0];
    poll_fd[j].events = POLLIN;
  }

  ready = poll(poll_fd, num_pipes, 0);
  if (ready == -1) {
    errExit("poll");
  }

  printf("poll() returned: %d\n", ready);

  // Check which pipes have available for reading

  for (j = 0; j < num_pipes; ++j) {
    if (poll_fd[j].revents & POLLIN) {
      printf("Readable: %3d\n", poll_fd[j].fd);
    }
  }

  exit(EXIT_SUCCESS);
}
