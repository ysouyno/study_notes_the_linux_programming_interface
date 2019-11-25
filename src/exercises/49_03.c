#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  char *addr;
  int fd;
  struct stat sb;
  int offset;

  if (argc != 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s file offset\n", argv[0]);
  }

  fd = open(argv[1], O_RDWR);
  if (fd == -1) {
    errExit("open");
  }

  if (fstat(fd, &sb) == -1) {
    errExit("fstat");
  }

  printf("file.st_size: %ld\n", (long)sb.st_size);

  addr = mmap(NULL, 8192, PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    errExit("mmap");
  }
  printf("mmap size: 8192 ok\n");

  offset = getInt(argv[2], GN_NONNEG, "offset");
  printf("addr[%d]: 0x%08x\n", offset, addr[offset]);

  // Only write to addr[offset] will occur SIGSEGV
  addr[offset] = 1;

  exit(EXIT_SUCCESS);
}
