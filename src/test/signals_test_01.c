#include <signal.h>
#include "tlpi_hdr.h"

static void handler(int sig)
{
  // UNSAFE: This handler uses non-async-signal-safe functions
  // (printf(), exit(); see Section 21.1.2)

  if (sig == SIGABRT) {
    printf("Caught SIGABRT\n");
    return;
  }
}

int main(int argc, char *argv[])
{
  struct sigaction sa;

  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGABRT, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  for (; ; ) {
    pause();
  }
}
