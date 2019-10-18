#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s pathname\n", argv[0]);
  }

  execl(argv[1], "longest_line.awk", "Makefile", (char *)NULL);
  errExit("execl");
}
