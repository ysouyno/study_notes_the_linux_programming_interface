#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("program_invocation_name: %s\n", program_invocation_name);
  printf("program_invocation_short_name: %s\n", program_invocation_short_name);
  return 0;
}
