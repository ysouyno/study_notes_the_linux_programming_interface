#include "tlpi_hdr.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_SIZE 32

int main(int argc, char *argv[]) {
  int fdsrc, fddst;
  char buf[MAX_SIZE];
  int n;

  if (argc != 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s src-file dst-file\n", argv[0]);
  }

  fdsrc = open(argv[1], O_RDONLY);
  if (-1 == fdsrc) {
    errExit("open src-file");
  }

  fddst = open(argv[2], O_RDWR | O_CREAT | O_TRUNC,
               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (-1 == fddst) {
    errExit("open dst-file");
  }

  while ((n = read(fdsrc, buf, MAX_SIZE))) {
    if (-1 == write(fddst, buf, n)) {
      errExit("write");
    }
  }

  return 0;
}
