#include <signal.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  sigset_t block_set, prev_mask;

  // Initialize a signal set to contain SIGINT
  sigemptyset(&block_set);
  sigaddset(&block_set, SIGINT);

  // Block SIGINT, save previous signal mask
  if (sigprocmask(SIG_BLOCK, &block_set, &prev_mask) == -1) {
    errExit("sigprocmask1");
  }

  // Code that should not be interrupted by SIGINT
  printf("Blocking SIGINT...\n");
  sleep(10);

  // Restore previous signal mask, unblocking SIGINT
  if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) == -1) {
    errExit("sigprocmask2");
  }

  printf("Unblocking SIGINT...\n");
  sleep(5);

  exit(EXIT_SUCCESS);
}
