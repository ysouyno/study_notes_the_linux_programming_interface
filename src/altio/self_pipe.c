#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
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
  fd_set readfds;
  int ready, nfds, flags;
  struct timeval timeout;
  struct timeval *pto;
  struct sigaction sa;
  char ch;
  int fd, j;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s {timeout|-} fd...\n"
             "\t\t(`-` means infinite timeout)\n", argv[0]);
  }

  // Initialize `timeout`, `readfds`, and `nfds` for select()

  if (strcmp(argv[1], "-") == 0) {
    pto = NULL; // Infinite timeout
  }
  else {
    pto = &timeout;
    timeout.tv_sec = getLong(argv[1], 0, "timeout");
    timeout.tv_usec = 0;
  }

  nfds = 0;

  // Build the `readfds` from the fd numbers given in command line

  FD_ZERO(&readfds);
  for (j = 2; j < argc; ++j) {
    fd = getInt(argv[j], 0, "fd");
    if (fd >= FD_SETSIZE) {
      cmdLineErr("file descriptor exceeds limit (%d)\n", FD_SETSIZE);
    }

    if (fd >= nfds) {
      nfds = fd + 1;
    }

    FD_SET(fd, &readfds);
  }

  if (pipe(pfd) == -1) {
    errExit("pipe");
  }

  FD_SET(pfd[0], &readfds);
  nfds = max(nfds, pfd[0] + 1);

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

  while ((ready = select(nfds, &readfds, NULL, NULL, pto)) == -1 &&
         errno == EINTR) {
    continue; // Restart if interrupted by signal
  }

  if (ready == -1) {
    errExit("select");
  }

  if (FD_ISSET(pfd[0], &readfds)) { // Handler was called
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

  // Examine file descriptor sets returned by select() to see
  // which other file descriptors are ready

  printf("ready = %d\n", ready);

  for (j = 2; j < argc; ++j) {
    fd = getInt(argv[j], 0, "fd");
    printf("%d: %s\n", fd, FD_ISSET(fd, &readfds) ? "r" : "");
  }

  // And check if read end of pipe is ready

  printf("%d: %s   (read end of pipe)\n", pfd[0],
         FD_ISSET(pfd[0], &readfds) ? "r" : "");

  if (pto != NULL) {
    printf("timeout after select(): %ld.%03ld\n", (long)timeout.tv_sec,
           (long)timeout.tv_usec / 1000);
  }

  exit(EXIT_SUCCESS);
}
