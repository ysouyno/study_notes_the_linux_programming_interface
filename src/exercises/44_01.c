#include <ctype.h> // For toupper()
#include "tlpi_hdr.h"

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
  int pfd1[2]; // parent -> child
  int pfd2[2]; // child -> parent
  char buff[BUFF_SIZE];
  int num_read;

  if (pipe(pfd1) == -1) {
    errExit("pipe");
  }

  if (pipe(pfd2) == -1) {
    errExit("pipe");
  }

  switch (fork()) {
  case -1:
    errExit("fork");
  case 0:
    if (close(pfd1[1]) == -1) {
      errExit("close pfd1[1]");
    }

    if (close(pfd2[0]) == -1) {
      errExit("close pfd2[0]");
    }

    while ((num_read = read(pfd1[0], buff, BUFF_SIZE)) != -1) {
      for (int i = 0; i < num_read; ++i) {
        buff[i] = toupper((unsigned char)buff[i]);
      }

      if (write(pfd2[1], buff, num_read) != num_read) {
        errExit("write");
      }
    }

    _exit(EXIT_SUCCESS);
  default:
    if (close(pfd1[0]) == -1) {
      errExit("close pfd1[0]");
    }

    if (close(pfd2[1]) == -1) {
      errExit("close pfd2[1]");
    }

    while ((num_read = read(STDIN_FILENO, buff, BUFF_SIZE)) != -1) {
      if (write(pfd1[1], buff, num_read) != num_read) {
        errExit("write");
      }

      num_read = read(pfd2[0], buff, BUFF_SIZE);
      if (num_read == -1) {
        errExit("read");
      }

      if (write(STDOUT_FILENO, buff, num_read) != num_read) {
        errExit("write");
      }
    }

    exit(EXIT_SUCCESS);
  }
}
