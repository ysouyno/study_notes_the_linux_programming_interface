#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  int fd;
  char template[] = "/tmp/sometringXXXXXX";

  fd = mkstemp(template);
  if (fd == -1) {
    errExit("mkstemp");
  }

  printf("Generated filename was: %s\n", template);

  unlink(template); /* Name disappears immediately, but the file
                       is removed only close */

  if (close(fd) == -1) {
    errExit("close");
  }

  exit(EXIT_SUCCESS);
}
