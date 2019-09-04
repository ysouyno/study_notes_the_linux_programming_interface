#include <sys/wait.h>
#include <time.h>
#include "curr_time.h"         // Declaration of currTime()
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  int num_dead;                // Number of children so far waited for
  pid_t child_pid;             // PID of waited for child
  int j;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s sleep-time...\n", argv[0]);
  }

  setbuf(stdout, NULL);        // Disable buffering of stdout

  for (j = 1; j < argc; ++j) { // Create one child for each argument
    switch (fork()) {
    case -1: {
      errExit("fork");
    }
    case 0: {                  // Child sleeps for a while then exits
      printf("[%s] child %d started with PID %ld, sleep %s seconds\n",
             currTime("%T"), j, (long)getpid(), argv[j]);
      sleep(getInt(argv[j], GN_NONNEG, "sleep-time"));
      _exit(EXIT_SUCCESS);
    }
    default:                   // Parent just continues around loop
      break;
    }
  }

  num_dead = 0;

  for (; ; ) {                 // Parent waits for each child to exit
    child_pid = wait(NULL);
    if (child_pid == -1) {
      if (errno == ECHILD) {
        printf("No more children - bye!\n");
        exit(EXIT_SUCCESS);
      }
      else {                   // Some other (unexpected) error
        errExit("wait");
      }
    }

    num_dead++;
    printf("[%s] wait() returned child PID %ld (num_dead=%d)\n",
           currTime("%T"), (long)child_pid, num_dead);
  }
}
