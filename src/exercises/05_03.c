#include "tlpi_hdr.h"
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  int fd;
  off_t off;

  if (argc < 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s filename num-bytes [x]\n", argv[0]);
  }

  off = atoll(argv[2]);

  fd = open(argv[1], O_RDWR | O_CREAT | (argc > 3 ? 0 : O_APPEND),
            S_IRUSR | S_IWUSR);
  if (fd == -1) {
    errExit("open");
  }

  for (int i = 0; i < off; ++i) {
    if (argc > 3) {
      if (-1 == lseek(fd, 0, SEEK_END))
        errExit("lseek");
    }

    if (write(fd, "x", 1) == -1) {
      errExit("write");
    }
  }

  exit(EXIT_SUCCESS);
}
