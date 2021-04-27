#include "tlpi_hdr.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int id;
  if (argc > 1)
    id = atoll(argv[1]);
  else
    id = getuid();

  // For unprivileged process, only the effective user ID is changed, and it can
  // be changed only to the same value as either the read user ID or saved
  // set-user-ID.
  if (-1 == setuid(id)) {
    errExit("setuid");
  } else {
    printf("setuid(getuid()) ok\n");
  }

  printf("getuid() : %d\n", getuid());
  printf("geteuid(): %d\n", geteuid());

  return 0;
}
