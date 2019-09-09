#include <signal.h>
#include "tlpi_hdr.h"

#define TESTSIG SIGUSR1

static void handler(int sig)
{
}

int main(int argc, char *argv[])
{
  int num_sigs, scnt;
  pid_t child_pid;
  sigset_t wait_set;
  struct sigaction sa;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s num-sigs\n", argv[0]);
  }

  num_sigs = getInt(argv[1], GN_GT_0, "num-sigs");

  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(TESTSIG, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  // Block the signal before fork(), so that the child doesn't manage
  // to send it to the parent before the parent is ready to catch it.

  // We can fetch a pending signal with sigwaitinfo() even while that
  // signal is blocked.

  sigemptyset(&wait_set);
  sigaddset(&wait_set, TESTSIG);
  if (sigprocmask(SIG_SETMASK, &wait_set, NULL) == -1) {
    errExit("sigprocmask");
  }

  switch (child_pid = fork()) {
  case -1:
    errExit("fork");
  case 0:
    for (scnt = 0; scnt < num_sigs; scnt++) {
      if (kill(getppid(), TESTSIG) == -1) {
        errExit("kill");
      }

      if (sigwaitinfo(&wait_set, NULL) == -1) {
        errExit("sigwaitinfo");
      }
    }

    exit(EXIT_SUCCESS);
  default:
    for (scnt = 0; scnt < num_sigs; scnt++) {
      if (sigwaitinfo(&wait_set, NULL) == -1) {
        errExit("sigwaitinfo");
      }

      if (kill(child_pid, TESTSIG) == -1) {
        errExit("kill");
      }
    }

    exit(EXIT_SUCCESS);
  }
}
