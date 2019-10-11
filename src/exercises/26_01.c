#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  switch (fork()) {
  case -1:
    errExit("fork");
  case 0:
    sleep(3);
    printf("Parent PID = %ld\n", (long)getppid());
    _exit(EXIT_SUCCESS);
  default:
    exit(EXIT_SUCCESS);
  }
}
