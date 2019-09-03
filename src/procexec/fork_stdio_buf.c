#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  printf("Hello world\n");
  write(STDOUT_FILENO, "Cao\n", 4);

  if (fork() == -1) {
    errExit("fork");
  }

  // Both child and parent continue execution here

  exit(EXIT_SUCCESS);
}
