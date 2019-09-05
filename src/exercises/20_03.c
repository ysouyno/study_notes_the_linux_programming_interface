#include <signal.h>
#include "tlpi_hdr.h"

static void handler(int sig)
{
  sigset_t prev_mask;

  // UNSAFE: This handler uses non-async-signal-safe functions
  // (printf(), exit(); see Section 21.1.2)

  if (sig == SIGINT) {
    printf("Caught SIGINT\n");
    return;
  }

  if (sig == SIGQUIT) {
    printf("Caught SIGQUIT\n");
    exit(EXIT_SUCCESS);
  }

  if (sig == SIGUSR1) {
    printf("Caught SIGUSR1\n");

    if (sigprocmask(SIG_SETMASK, NULL, &prev_mask)) {
      errExit("sigprocmask");
    }

    if (sigismember(&prev_mask, sig)) {
      printf("SIGUSR1 in signal mask\n");
    }
    else {
      printf("SIGUSR1 NOT in signal mask\n");
    }
  }
}

int main(int argc, char *argv[])
{
  struct sigaction sa;

  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESETHAND;

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  sa.sa_flags = 0;
  if (sigaction(SIGQUIT, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  sa.sa_flags = SA_NODEFER;
  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  for (; ; ) {
    pause();
  }
}
