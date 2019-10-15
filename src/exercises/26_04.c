#include <signal.h>
#include <libgen.h>     // For basename() declaration
#include "tlpi_hdr.h"

#define CMD_SIZE 200
#define SYNC_SIG SIGUSR1

static void on_exit_handler(int exit_status, void *arg)
{
  printf("on_exit_handler: status = %d, arg = %ld\n",
         exit_status, (long)arg);

  if (kill((pid_t)arg, SYNC_SIG) == -1) {
    errExit("kill");
  }
}

static void handler(int sig)
{
}

int main(int argc, char *argv[])
{
  char cmd[CMD_SIZE];
  pid_t child_pid;
  sigset_t block_mask, orig_mask, empty_mask;
  struct sigaction sa;

  setbuf(stdout, NULL); // Disable buffering of stdout

  printf("Parent PID = %ld\n", (long)getpid());

  sigemptyset(&block_mask);
  sigaddset(&block_mask, SYNC_SIG);
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
  case -1:
    errExit("fork");
  case 0:               // Child: immediately exits to become zombie
    if (on_exit(on_exit_handler, (void *)getppid()) != 0) {
      fatal("on_exit");
    }

    printf("Child (PID = %ld) exiting\n", (long)getpid());

    // _exit(EXIT_SUCCESS);
    break;
  default:              // Parent
    sigemptyset(&empty_mask);
    if (sigsuspend(&empty_mask) == -1 && errno != EINTR) {
      errExit("sigsuspend");
    }

    printf("Parent got signal from child's exit handler\n");

    snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
    cmd[CMD_SIZE - 1] = '\0';
    system(cmd);        // View zombie child

    // Now send the "sure kill" signal to the zombie

    if (kill(child_pid, SIGKILL) == -1) {
      errMsg("kill");
    }

    sleep(3);           // Give child a chance to react to signal

    printf("After sending SIGKILL to zombie (PID = %ld):\n", (long)child_pid);
    system(cmd);        // View zombie child again

    exit(EXIT_SUCCESS);
  }
}
