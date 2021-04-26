#include "tlpi_hdr.h"
#include <shadow.h>

int main(int argc, char *argv[]) {
  struct spwd *shadow = getspnam("ysouyno");
  if (!shadow) {
    errExit("getspnam");
  }

  printf("sp_namp: %s\nsp_pwdp: %s\n", shadow->sp_namp, shadow->sp_pwdp);

  return 0;
}
