#include <ctype.h>
#include <stdbool.h>
#include <fcntl.h>
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
  char buf[BUF_SIZE];
  ssize_t num_read;
  bool append;
  int open_flags;
  mode_t file_perms;
  int nfiles;
  int *files_fd;

  opt = 0;
  append = false;
  open_flags = O_CREAT | O_WRONLY;
  nfiles = 0;
  files_fd = NULL;

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

  if (append) {
    open_flags |= O_APPEND;
  }
  else {
    open_flags |= O_TRUNC;
  }

  file_perms =
    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-

  printf("argc - optind = %d\n", argc - optind);

  nfiles = argc - optind;
  files_fd = malloc((nfiles + 1) * sizeof(int));
  files_fd[0] = STDOUT_FILENO;

  for (int i = 1; i <= nfiles; ++i) {
    files_fd[i] = open(argv[optind + i - 1], open_flags, file_perms);
    printf("filename: %s, fd: %d\n", argv[optind + i - 1], files_fd[i]);
  }

  while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
    for (int i = 0; i <= nfiles; ++i) {
      if (write(files_fd[i], buf, num_read) != num_read) {
        free(files_fd);
        errExit("write");
      }
    }
  }

  for (int i = 1; i <= nfiles; ++i) {
    if (close(files_fd[i]) == -1) {
      free(files_fd);
      errExit("close");
    }
  }

  free(files_fd);
  return EXIT_SUCCESS;
}
