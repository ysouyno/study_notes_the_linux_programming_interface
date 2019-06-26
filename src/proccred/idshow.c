#define _GNU_SOURCE
#include <unistd.h>
#include <sys/fsuid.h>
#include <limits.h>
#include "ugid_functions.h" // userNameFromId() & groupNameFromId()
#include "tlpi_hdr.h"

#define SG_SIZE (NGROUPS_MAX + 1)

int main(int argc, char *argv[])
{
  uid_t ruid, euid, suid, fsuid;
  gid_t rgid, egid, sgid, fsgid;
  gid_t supp_groups[SG_SIZE];
  int num_groups, j;
  char *p;

  if (getresuid(&ruid, &euid, &suid) == -1) {
    errExit("getresuid");
  }

  if (getresgid(&rgid, &egid, &sgid) == -1) {
    errExit("getresgid");
  }

  // Attempts to change the file-system IDs are always ignored
  // for unprivileged processes, but event so, the following
  // calls return the current file-system IDs

  fsuid = setfsuid(0);
  fsgid = setfsgid(0);

  printf("UID: ");
  p = userNameFromId(ruid);
  printf("real=%s (%ld); ", (p == NULL) ? "???" : p, (long)ruid);
  p = userNameFromId(euid);
  printf("eff=%s (%ld); ", (p == NULL) ? "???" : p, (long)euid);
  p = userNameFromId(suid);
  printf("saved=%s (%ld) ;", (p == NULL) ? "???" : p, (long)suid);
  p = userNameFromId(fsuid);
  printf("fs=%s (%ld) ;", (p == NULL) ? "???" : p, (long)fsuid);
  printf("\n");

  printf("GID: ");
  p = userNameFromId(rgid);
  printf("real=%s (%ld); ", (p == NULL) ? "???" : p, (long)rgid);
  p = userNameFromId(egid);
  printf("eff=%s (%ld); ", (p == NULL) ? "???" : p, (long)egid);
  p = userNameFromId(sgid);
  printf("saved=%s (%ld) ;", (p == NULL) ? "???" : p, (long)sgid);
  p = userNameFromId(fsgid);
  printf("fs=%s (%ld) ;", (p == NULL) ? "???" : p, (long)fsgid);
  printf("\n");

  num_groups = getgroups(SG_SIZE, supp_groups);
  if (num_groups == -1) {
    errExit("getgroups");
  }

  printf("Supplementary group (%d): ", num_groups);

  for (j = 0; j < num_groups; ++j) {
    p = groupNameFromId(supp_groups[j]);
    printf("%s (%ld) ", (p == NULL) ? "???" : p, (long)supp_groups[j]);
  }
  printf("\n");

  exit(EXIT_SUCCESS);
}
