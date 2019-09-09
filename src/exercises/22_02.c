#define _GNU_SOURCE
#include <signal.h>
#include "tlpi_hdr.h"

static volatile all_done = 1;

static void siginfo_handler(int sig, siginfo_t *si, void *ucontext)
{
  // UNSAFE: This handler uses non-async-signal-safe functions
  // (printf(); see Section 21.1.2)

  if (sig == SIGINT || sig == SIGTERM) {
    all_done = 1;
    return;
  }

  printf("caught signal %d\n", sig);
  sleep(1);
}

int main(int argc, char *argv[])
{
  struct sigaction sa;
  int sig;
  sigset_t block_set, prev_mask;

  printf("%s: PID is %ld\n", argv[0], (long)getpid());

  sa.sa_sigaction = siginfo_handler;
  sigfillset(&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO;

  for (sig = 1; sig < NSIG; ++sig) {
    if (sig != SIGTSTP && sig != SIGQUIT) {
      sigaction(sig, &sa, NULL);
    }
  }

  sigfillset(&block_set);
  sigdelset(&block_set, SIGINT);
  sigdelset(&block_set, SIGTERM);

  if (sigprocmask(SIG_SETMASK, &block_set, &prev_mask) == -1) {
    errExit("sigprocmask");
  }

  printf("%s: signals blocked - sleeping 60 seconds\n", argv[0]);
  sleep(60);
  printf("%s: sleep complete\n", argv[0]);

  if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) == -1) {
    errExit("sigprocmask");
  }

  while (!all_done) {
    pause();
  }
}
