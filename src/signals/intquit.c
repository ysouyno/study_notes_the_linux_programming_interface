#include <signal.h>
#include "tlpi_hdr.h"

static void sig_handler(int sig)
{
  static int count = 0;

  // UNSAFE: This handler uses non-async-signal-safe functions
  // (printf(), exit(); see Section 21.1.2)

  if (sig == SIGINT) {
    count++;
    printf("Caught SIGINT (%d)\n", count);
    return; // Resume execution at point of interruption
  }

  // Must be SIGQUIT - print a message and terminate the process

  printf("Caught SIGQUIT - that's all folks!\n");
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
  // Establish same handler for SIGINT and SIGQUIT

  if (signal(SIGINT, sig_handler) == SIG_ERR) {
    errExit("signal");
  }

  if (signal(SIGQUIT, sig_handler) == SIG_ERR) {
    errExit("signal");
  }

  for (; ; ) { // Loop forever, waiting for signals
    pause();   // Block until a signal is caught
  }
}
