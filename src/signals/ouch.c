#include <signal.h>
#include "tlpi_hdr.h"

static void sig_handler(int sig)
{
  printf("Ouch!\n"); // UNSAFE (see Section 21.1.2)
}

int main(int argc, char *argv[])
{
  int j;

  if (signal(SIGINT, sig_handler) == SIG_ERR) {
    errExit("signal");
  }

  for (j = 0; ; ++j) {
    printf("%d\n", j);
    sleep(3);
  }
}
