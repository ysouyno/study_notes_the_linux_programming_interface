#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  char *addr_src, *addr_dst;
  int fd_src, fd_dst;
  struct stat sb;

  if (argc != 3 || strcmp(argv[1], "--hep") == -1) {
    usageErr("%s SRC DST\n", argv[0]);
  }

  fd_src = open(argv[1], O_RDONLY);
  if (fd_src == -1) {
    errExit("open");
  }

  if (fstat(fd_src, &sb) == -1) {
    errExit("fstat");
  }

  addr_src = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd_src, 0);
  if (addr_src == MAP_FAILED) {
    errExit("mmap");
  }

  if (close(fd_src) == -1) {
    errExit("close");
  }

  printf("addr_src: %p\n", addr_src);

  fd_dst = open(argv[2], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd_dst == -1) {
    errExit("open");
  }

  if (ftruncate(fd_dst, sb.st_size) == -1) {
    errExit("ftruncate");
  }

  addr_dst = mmap(NULL, sb.st_size, PROT_WRITE, MAP_SHARED, fd_dst, 0);
  if (addr_dst == MAP_FAILED) {
    errExit("mmap");
  }

  if (close(fd_dst) == -1) {
    errExit("close");
  }

  printf("addr_dst: %p\n", addr_dst);

  memcpy(addr_dst, addr_src, sb.st_size);

  if (msync(addr_dst, sb.st_size, MS_SYNC) == -1) {
    errExit("msync");
  }

  exit(EXIT_SUCCESS);
}
