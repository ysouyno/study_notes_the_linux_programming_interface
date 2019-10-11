#include <signal.h>
#include <sys/wait.h>
#include "print_wait_status.h"
#include "curr_time.h"
#include "tlpi_hdr.h"

// Number of children started but not yet waited on
static volatile int num_live_children = 0;

static void sigchld_handler(int sig)
{
  int status, saved_errno;
  pid_t child_pid;

  // UNSAFE: This handler uses non-async-signal-safe functions
  // (printf(), printWaitStatus(), currTime(); see Section 21.1.2)

  saved_errno = errno; // In case we modify 'errno'

  printf("%s handler: Caught SIGCHLD\n", currTime("%T"));

  while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
    printf("%s handler: Reaped child %ld - ", currTime("%T"), (long)child_pid);
    printWaitStatus(NULL, status);
    num_live_children--;
  }

  if (child_pid == -1 && errno != ECHILD) {
    errMsg("waitpid");
  }

  sleep(5); // Artificially lengthen execution of handler
  printf("%s handler: returning\n", currTime("%T"));

  errno = saved_errno;
}

int main(int argc, char *argv[])
{
  int j, sig_cnt;
  sigset_t block_mask, empty_mask;
  struct sigaction sa;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s child-sleep-time...\n", argv[0]);
  }

  setbuf(stdout, NULL); // Disable buffering of stdout

  sig_cnt = 0;
  num_live_children = argc - 1;

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = sigchld_handler;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  // Block SIGCHLD to prevent its delivery if a child terminates
  // before the parent commences the sigsuspend() loop below

  sigemptyset(&block_mask);
  sigaddset(&block_mask, SIGCHLD);
  if (sigprocmask(SIG_SETMASK, &block_mask, NULL) == -1) {
    errExit("sigprocmask");
  }

  for (j = 1; j < argc; ++j) {
    switch (fork()) {
    case -1:
      errExit("fork");
    case 0: // Child - sleep and then exits
      sleep(getInt(argv[j], GN_NONNEG, "child-sleep-time"));
      printf("%s Child %d (PID = %ld) exiting\n", currTime("%T"),
             j, (long)getpid());
      _exit(EXIT_SUCCESS);
    default: // Parent - loops to create next child
      break;
    }
  }

  // Parent comes here: wait for SIGCHLD until all children are dead

  sigemptyset(&empty_mask);
  while (num_live_children > 0) {
    if (sigsuspend(&empty_mask) == -1 && errno != EINTR) {
      errExit("sigsuspend");
    }

    sig_cnt++;
  }

  printf("%s All %d children have terminated; SIGCHLD was caught "
         "%d times\n", currTime("%T"), argc - 1, sig_cnt);

  exit(EXIT_SUCCESS);
}
