#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  char ch = 'f';

  switch (vfork()) {
  case -1:
    errExit("vfork");
  case 0:
    write(STDOUT_FILENO, "Child executing\n", 16);

    close(STDIN_FILENO);
    if (read(STDIN_FILENO, &ch, sizeof(ch)) < 0) {
      perror("read");
    }
    printf("ch: %c\n", ch);

    _exit(EXIT_SUCCESS);
  default:
    write(STDOUT_FILENO, "Parent executing\n", 17);

    if (read(STDIN_FILENO, &ch, sizeof(ch)) < 0) {
      perror("read");
    }
    printf("ch: %c\n", ch);

    exit(EXIT_SUCCESS);
  }
}
