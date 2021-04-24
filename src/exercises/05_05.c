#include "tlpi_hdr.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

off_t get_current_file_off(int fd) { return lseek(fd, 0, SEEK_CUR); }

int main(int argc, char *argv[]) {
  int fd, newfd;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s filename\n", argv[0]);
  }

  fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    errExit("open");
  }

  newfd = dup(fd);
  if (-1 == newfd) {
    errExit("dup");
  }

  printf("current oldfd offset: %lld\n", get_current_file_off(fd));
  printf("current newfd offset: %lld\n", get_current_file_off(newfd));

  if (-1 == lseek(fd, 32, SEEK_SET))
    errExit("lseek");

  printf("current oldfd offset: %lld\n", get_current_file_off(fd));
  printf("current newfd offset: %lld\n", get_current_file_off(newfd));

  exit(EXIT_SUCCESS);
}
