#include <ctype.h>
#include <stdbool.h>
#include "tlpi_hdr.h"

#define printable(ch) (isprint((unsigned char)ch) ? ch : '#')

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

static void usageError(char *prog_name, char *msg, int opt)
{
  if (msg != NULL && opt != 0) {
    fprintf(stderr, "%s (-%c)\n", msg, printable(opt));
  }
  fprintf(stderr, "Usage :%s [-a]\n", prog_name);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
  int opt;
  char *pfile;
  char buf[BUF_SIZE];
  ssize_t num_read;
  bool append;

  opt = 0;
  pfile = NULL;
  append = false;

  while ((opt = getopt(argc, argv, ":a")) != -1) {
    printf("opt = %3d (%c); optind = %d", opt, printable(opt), optind);
    if (opt == '?' || opt == ':') {
      printf("; optopt = %3d (%c)", optopt, printable(optopt));
    }
    printf("\n");

    switch (opt) {
    case 'a':
      append = true;
      break;
    case ':': usageError(argv[0], "Missing argument", optopt);
    case '?': usageError(argv[0], "Unrecognized argument", optopt);
    default: fatal("Unexpected case in switch()");
    }
  }

  printf("argc - optind = %d\n", argc - optind);

  /*
  while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
    if (write(STDOUT_FILENO, buf, num_read) != num_read) {
      errExit("write");
    }
  }
  */

  return 0;
}
