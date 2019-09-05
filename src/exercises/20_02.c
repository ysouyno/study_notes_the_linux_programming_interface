#include <signal.h>
#include "tlpi_hdr.h"

static void handler(int sig)
{
  // UNSAFE: This handler uses non-async-signal-safe functions
  // (printf(), exit(); see Section 21.1.2)

  if (sig == SIGINT) {
    printf("Caught SIGINT\n");
  }
}

int main(int argc, char *argv[])
{
  sigset_t block_set, prev_mask;
  struct sigaction sa;

  // or replace SIG_IGN with handler, the program can
  // see SIGINT after unblock SIGINT
  sa.sa_handler = SIG_IGN;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  // Initialize a signal set to contain SIGINT
  sigemptyset(&block_set);
  sigaddset(&block_set, SIGINT);

  // Block SIGINT, save previous signal mask

  if (sigprocmask(SIG_BLOCK, &block_set, &prev_mask) == -1) {
    errExit("sigprocmask");
  }

  printf("Blocking SIGINT 10 seconds...\n");
  sleep(10);

  if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) == -1) {
    errExit("sigprocmask");
  }

  exit(EXIT_SUCCESS);
}
