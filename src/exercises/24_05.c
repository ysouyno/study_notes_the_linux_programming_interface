#include <signal.h>
#include "curr_time.h" // Declaration of currTime()
#include "tlpi_hdr.h"

#define SYNC_SIG SIGUSR1 // Synchronization signal

static void handler(int sig) // Signal handler - does nothing but return
{
}

int main(int argc, char *argv[])
{
  pid_t child_pid;
  sigset_t block_mask, orig_mask, empty_mask;
  struct sigaction sa;

  setbuf(stdout, NULL); // Disable buffering of stdout

  sigemptyset(&block_mask);
  sigaddset(&block_mask, SYNC_SIG); // Block signal
  if (sigprocmask(SIG_BLOCK, &block_mask, &orig_mask) == -1) {
    errExit("sigprocmask");
  }

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = handler;
  if (sigaction(SYNC_SIG, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  switch (child_pid = fork()) {
  case -1: {
    errExit("fork");
  }
  case 0: { // Child
    // Child does some required action here...
    printf("[%s %ld] Child started - doing some work\n",
           currTime("%T"), (long)getpid());
    sleep(2); // Simulate time spent doing some work

    // And then signals parent that it's done

    printf("[%s %ld] Child about to signal parent\n",
           currTime("%T"), (long)getpid());
    if (kill(getppid(), SYNC_SIG) == -1) {
      errExit("kill");
    }

    // Now child can do other things...

    printf("[%s %ld] Child about to wait for signal\n",
           currTime("%T"), (long)getpid());
    sigemptyset(&empty_mask);
    if (sigsuspend(&empty_mask) == -1 && errno != EINTR) {
      errExit("sigsuspend");
    }
    printf("[%s %ld] Child got signal\n", currTime("%T"), (long)getpid());

    _exit(EXIT_SUCCESS);
  }
  default: // Parent
    // Parent may do some work here, and then waits for child to
    // complete the required action

    printf("[%s %ld] Parent about to wait for signal\n",
           currTime("%T"), (long)getpid());
    sigemptyset(&empty_mask);
    if (sigsuspend(&empty_mask) == -1 && errno != EINTR) {
      errExit("sigsuspend");
    }
    printf("[%s %ld] Parent got signal\n", currTime("%T"), (long)getpid());

    // Signals parent that it's done

    printf("[%s %ld] Parent about to signal child\n",
           currTime("%T"), (long)getpid());
    if (kill(child_pid, SYNC_SIG) == -1) {
      errExit("kill");
    }

    // If required, return signal mask to its original state

    if (sigprocmask(SIG_SETMASK, &orig_mask, NULL) == -1) {
      errExit("sigprocmask");
    }

    // Parent carries on to do other things

    exit(EXIT_SUCCESS);
  }
}
