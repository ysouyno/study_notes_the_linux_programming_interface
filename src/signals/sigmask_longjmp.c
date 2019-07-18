#define _GNU_SOURCE           // Get strsignal() declaration from <string.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include "signal_functions.h" // Declaration of printSigMask()
#include "tlpi_hdr.h"

// Set to 1 once "env" buffer has been initialized by [sig]setjmp()
static volatile sig_atomic_t can_jump = 0;

#ifdef USE_SIGSETJMP
static sigjmp_buf senv;
#else
static jmp_buf env;
#endif // USE_SIGSETJMP

static void handler(int sig)
{
  // UNSAFE: This handler uses non-async-signal-safe functions
  // (printf(), strsignal(), printSigMask(); see Section 21.1.2)

  printf("Received signal %d (%s), signal mask is:\n", sig, strsignal(sig));
  printSigMask(stdout, NULL);

  if (!can_jump) {
    printf("'env' buffer not yet set, doing a simple return\n");
    return;
  }

#ifdef USE_SIGSETJMP
  siglongjmp(senv, 1);
#else
  longjmp(env, 1);
#endif // USE_SIGSETJMP
}

int main(int argc, char *argv[])
{
  struct sigaction sa;

  printSigMask(stdout, "Signal mask at startup:\n");

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handler;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    errExit("sigaction");
  }

#ifdef USE_SIGSETJMP
  printf("Calling sigsetjmp()\n");
  if (sigsetjmp(senv, 1) == 0)
#else
  printf("Calling setjmp()\n");
  if (setjmp(env) == 0)
#endif // USE_SIGSETJMP
    can_jump = 1;             // Executed after [sig]setjmp()
  else                        // Executed after [sig]longjmp()
    printSigMask(stdout, "After jump handler, signal mask is:\n");

  for (; ; ) {                // Wait for signals until killed
    pause();
  }
}
