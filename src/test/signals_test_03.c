#include <signal.h>
#include <setjmp.h>
#include "tlpi_hdr.h"

static volatile sig_atomic_t can_jump = 0;
static sigjmp_buf senv;

static void handler(int sig)
{
  // UNSAFE: This handler uses non-async-signal-safe functions
  // (printf(), exit(); see Section 21.1.2)

  if (sig == SIGABRT) {
    printf("Caught SIGABRT\n");
    if (!can_jump) {
      return;
    }

    siglongjmp(senv, 1);
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

  if (sigsetjmp(senv, 1) == 0) {
    can_jump = 1;
  }
  else {
    printf("abort() cancelled\n");
    sleep(1);
  }

  abort();
}
