#include "tlpi_hdr.h"
#include <errno.h>
#include <fcntl.h>

int fake_dup(int fd) { return fcntl(fd, F_DUPFD, 0); }

int fake_dup2(int oldfd, int newfd) {
  if (oldfd == newfd) {
    // Check if oldfd is valid
    if (-1 == fcntl(oldfd, F_GETFL)) {
      errno = EBADF;
      return -1;
    } else {
      return oldfd;
    }
  } else {
    close(newfd);
    return fcntl(oldfd, F_DUPFD, newfd);
  }
}

int main(int argc, char *argv[]) {
  printf("fake_dup(0): %d\n", fake_dup(0));
  printf("fake_dup2(0, 0): %d\n", fake_dup2(0, 0));
  printf("fake_dup2(0, 1): %d\n", fake_dup2(0, 1));

  if (-1 == fake_dup2(23, 24)) {
    perror("fake_dup2(23, 24)");
  }

  if (-1 == fake_dup2(23, 23)) {
    errExit("fake_dup2(23, 23)");
  }

  exit(EXIT_SUCCESS);
}
