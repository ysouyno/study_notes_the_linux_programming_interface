#include "tlpi_hdr.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("sysconf(_SC_CLK_TCK): %ld\n", sysconf(_SC_CLK_TCK));
  return 0;
}
