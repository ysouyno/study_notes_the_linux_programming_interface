#include <sys/wait.h>
#include "print_wait_status.h" // Declares printWaitStatus()
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  int status;
  pid_t child_pid;

  if (argc > 1 && strcmp(argv[1], "--help") == 0) {
    usageErr("%s [exit-status]\n", argv[0]);
  }

  switch (fork()) {
  case -1: {
    errExit("fork");
  }
  case 0: { // Child: either exits immediately with given
            // status or loops waiting for signals
    printf("Child started with PID = %ld\n", (long)getpid());
    if (argc > 1) {     // Status supplied on command line?
      exit(getInt(argv[1], 0, "exit-status"));
    }
    else {              // Otherwise, wait for signals
      for (; ; ) {
        pause();
      }
    }
    exit(EXIT_SUCCESS); // Not reached, but good practice
  }
  default:  // Parent: repeatedly wait on child until it
            // either exits or is terminated by a signal
    for (; ; ) {
      child_pid = waitpid(-1, &status, WUNTRACED
#ifdef WCONTINUED       // Not present on older versions of Linux
                          | WCONTINUED
#endif
                          );
      if (child_pid == -1) {
        errExit("waitpid");
      }

      // Print status in hex, and as separate decimal bytes

      printf("waitpid() returned: PID=%ld; status=0x%04x (%d,%d)\n",
             (long)child_pid,
             (unsigned int)status, status >> 8, status & 0xff);
      printWaitStatus(NULL, status);

      if (WIFEXITED(status) || WIFSIGNALED(status)) {
        exit(EXIT_SUCCESS);
      }
    }
  }
}
