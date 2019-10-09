#include <sys/wait.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  int child_pid;

  switch (fork()) {
  case -1:
    errExit("fork");
  case 0:
    printf("Core Dump\n");
    abort();
  default:
    break;
  }

  for (; ; ) {
    child_pid = wait(NULL);
    if (child_pid == -1) {
      if (errno == ECHILD) {
        printf("No more children - bye!\n");
        exit(EXIT_SUCCESS);
      }
      else {
        errExit("wait");
      }
    }
  }
}
