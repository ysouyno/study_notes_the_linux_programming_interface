#define _GNU_SOURCE
#include <string.h>
#include <signal.h>
#include "tlpi_hdr.h"

static volatile int handler_sleep_time;
static volatile int sig_cnt = 0; // Number of signals received
static volatile int all_done = 0;

// Handler for signals established using SA_SIGINFO
static void siginfo_handler(int sig, siginfo_t *si, void *ucontext)
{
  // UNSAFE: This handler uses non-async-signal-safe functions
  // (printf(); see Section 21.1.2)

  // SIGINT or SIGTERM can be used to terminate program

  if (sig == SIGINT || sig == SIGTERM) {
    all_done = 1;
    return;
  }

  sig_cnt++;
  printf("caught signal %d\n", sig);

  printf("    sig_signo=%d, si_code=%d (%s), ", si->si_signo, si->si_code,
         (si->si_code == SI_USER) ? "SI_USER" :
         (si->si_code == SI_QUEUE) ? "SI_QUEUE" : "other");
  printf("si_value=%d\n", si->si_value.sival_int);
  printf("    si_pid=%ld, si_uid=%ld\n", (long)si->si_pid, (long)si->si_uid);

  sleep(handler_sleep_time);
}

int main(int argc, char *argv[])
{
  struct sigaction sa;
  int sig;
  sigset_t prev_mask, block_mask;

  if (argc > 1 && strcmp(argv[1], "--help") == 0) {
    usageErr("%s [block-time [handler-sleep-time]]\n", argv[0]);
  }

  printf("%s: PID is %ld\n", argv[0], (long)getpid());

  handler_sleep_time = (argc > 2) ?
    getInt(argv[2], GN_NONNEG, "handler-sleep-time") : 1;

  // Establish handler for most signals, During execution of the handler,
  // mask all other signals to prevent handlers recursively interrupting
  // each other (which would make the output hard to read).

  sa.sa_sigaction = siginfo_handler;
  sa.sa_flags = SA_SIGINFO;
  sigfillset(&sa.sa_mask);

  for (sig = 1; sig < NSIG; ++sig) {
    if (sig != SIGTSTP && sig != SIGQUIT) {
      sigaction(sig, &sa, NULL);
    }
  }

  // Optionally block signals and sleep, allowing signals to be
  // sent to us before they are unblocked and handled

  if (argc > 1) {
    sigfillset(&block_mask);
    sigdelset(&block_mask, SIGINT);
    sigdelset(&block_mask, SIGTERM);

    if (sigprocmask(SIG_SETMASK, &block_mask, &prev_mask) == -1) {
      errExit("sigprocmask");
    }

    printf("%s: signals blocked - sleeping %s seconds\n", argv[0], argv[1]);
    sleep(getInt(argv[1], GN_GT_0, "block-time"));
    printf("%s: sleep complete\n", argv[0]);

    if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) == -1) {
      errExit("sigprocmask");
    }
  }

  while (!all_done) { // Wait for incoming signals
    pause();
  }
}
