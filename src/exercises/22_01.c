#include <signal.h>
#include "tlpi_hdr.h"

static void handler(int sig)
{
  // UNSAFE: This handler uses non-async-signal-safe functions
  // (printf(), exit(); see Section 21.1.2)

  if (sig == SIGCONT) {
    printf("Caught SIGCONT\n");
  }
}

int main(int argc, char *argv[])
{
  struct sigaction sa;
  sigset_t block_set, prev_mask;

  sigemptyset(&block_set);
  sigaddset(&block_set, SIGCONT);

  if (sigprocmask(SIG_BLOCK, &block_set, &prev_mask) == -1) {
    errExit("sigprocmask");
  }

  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGCONT, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  printf("getchar() to unblock SIGCONT\n");
  getchar();

  if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) == -1) {
    errExit("sigprocmask");
  }

  printf("%s will quit after 3 seconds\n", argv[0]);
  sleep(3);

  exit(EXIT_SUCCESS);
}
