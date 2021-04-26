#include "tlpi_hdr.h"
#include <pwd.h>

int main(int argc, char *argv[]) {
  struct passwd *pw = getpwnam("ysouyno");
  if (!pw) {
    errExit("getpwname");
  }

  printf("pw_name: %s\npw_passwd: %s\npw_uid: %d\npw_gid: %d\npw_gecos: "
         "%s\npw_dir: %s\npw_shell: %s\n",
         pw->pw_name, pw->pw_passwd, pw->pw_uid, pw->pw_gid, pw->pw_gecos,
         pw->pw_dir, pw->pw_shell);

  exit(EXIT_SUCCESS);
}
