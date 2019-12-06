#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#include "tlpi_hdr.h"

static int pfd[2];

static void handler(int sig)
{
  int saved_errno;

  saved_errno = errno;
  if (write(pfd[1], "x", 1) == -1 && errno != EAGAIN)
    errExit("write");
  errno = saved_errno;
}

int main(int argc, char *argv[])
{
  int ready, nfds, flags;
  struct timeval timeout;
  struct timeval *pto;
  struct sigaction sa;
  char ch;
  int j;
  struct pollfd *poll_fd;

  if (argc < 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s {timeout|-} fd...\n"
             "\t\t(`-` means infinite timeout)\n", argv[0]);
  }

  if (strcmp(argv[1], "-") == 0) {
    pto = NULL; // Infinite timeout
  }
  else {
    pto = &timeout;
    timeout.tv_sec = getLong(argv[1], 0, "timeout");
    timeout.tv_usec = 0;
  }

  nfds = argc - 2 + 1; // Plus 1 for the read end of pipe

  if (pipe(pfd) == -1) {
    errExit("pipe");
  }

  flags = fcntl(pfd[0], F_GETFL);
  if (flags == -1) {
    errExit("fcntl-F_GETFL");
  }
  flags |= O_NONBLOCK;
  if (fcntl(pfd[0], F_SETFL, flags) == -1) {
    errExit("fcntl-F_SETFL");
  }

  flags = fcntl(pfd[1], F_GETFL);
  if (flags == -1) {
    errExit("fcntl-F_GETFL");
  }
  flags |= O_NONBLOCK;
  if (fcntl(pfd[1], F_SETFL, flags) == -1) {
    errExit("fcntl-F_SETFL");
  }

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = handler;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  poll_fd = calloc(nfds, sizeof(struct pollfd));
  if (poll_fd == NULL) {
    errExit("calloc");
  }

  for (j = 2; j < argc; ++j) {
    poll_fd[j - 2].fd = getInt(argv[j], GN_NONNEG, "fd");
    poll_fd[j - 2].events = POLLIN;
  }

  // Add the read end of pipe to `poll_fd`
  poll_fd[nfds - 1].fd = pfd[0];
  poll_fd[nfds - 1].events = POLLIN;

  while ((ready = poll(poll_fd, nfds, pto->tv_sec * 1000)) == -1 &&
         errno == EINTR) {
    continue;
  }

  if (ready == -1) {
    errExit("poll");
  }

  // Examine file descriptor sets returned by poll() to see
  // which other file descriptors are ready

  printf("ready = %d\n", ready);

  for (j = 0; j < nfds; ++j) {
    if ((poll_fd[j].revents & POLLIN) &&
        (poll_fd[j].fd == pfd[0])) {
      printf("A signal was caught\n");

      for (; ; ) { // Consume bytes from pipe
        if (read(pfd[0], &ch, 1) == -1) {
          if (errno == EAGAIN) {
            break;
          }
          else {
            errExit("read");
          }
        }

        // Perform any actions that should be taken in response to signal
      }
    }
  }

  for (j = 0; j < nfds; ++j) {
    if (poll_fd[j].fd == pfd[0]) {
      // Check if read end of pipe is ready
      printf("%d: %s   (read end of pipe)\n", poll_fd[j].fd,
             (poll_fd[j].revents & POLLIN) ? "r" : "");
    }
    else {
      printf("%d: %s\n", poll_fd[j].fd,
             (poll_fd[j].revents & POLLIN) ? "r" : "");
    }
  }

  exit(EXIT_SUCCESS);
}
