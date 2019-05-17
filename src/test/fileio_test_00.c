#include <fcntl.h>
#include <tlpi_hdr.h>

int main(int argc, char *argv[])
{
  int fd;

  if (close(STDIN_FILENO) == -1) {
    errExit("close");
  }

  fd = open("/home/ysouyno/temp/a.txt", O_RDONLY);
  if (fd == -1) {
    errExit("open");
  }

  printf("fd: %d\n", fd);

  if (close(fd) == -1) {
    errExit("close");
  }

  return 0;
}
