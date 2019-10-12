#include <sys/wait.h>
#include "print_wait_status.h" // Declares printWaitStatus()
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  int status;
  siginfo_t info;
  int ret;

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
      memset(&info, 0, sizeof(info));

      ret = waitid(P_ALL, 0, &info, WEXITED | WSTOPPED
#ifdef WCONTINUED       // Not present on older versions of Linux
                   | WCONTINUED
#endif
                   );
      if (ret == -1) {
        errExit("waitid");
      }

      // Print status in hex, and as separate decimal bytes

      status = info.si_status;

      printf("waitid() returned: PID=%ld; status=0x%04x (%d,%d)\n",
             (long)info.si_pid,
             (unsigned int)status, status >> 8, status & 0xff);
      printWaitStatus(NULL, status);

      if (info.si_code == CLD_EXITED || info.si_code == CLD_KILLED) {
        exit(EXIT_SUCCESS);
      }
    }
  }
}
