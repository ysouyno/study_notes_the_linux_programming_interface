#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("getuid() : %d\n", getuid());
  printf("getgid() : %d\n", getgid());
  printf("geteuid(): %d\n", geteuid());
  printf("getegid(): %d\n", getegid());
  return 0;
}
