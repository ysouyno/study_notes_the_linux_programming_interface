#define _GNU_SOURCE
#include <sys/shm.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  int maxind, ind, shmid;
  struct shmid_ds ds;
  struct shminfo shminfo;

  maxind = shmctl(0, SHM_INFO, &shminfo);
  if (maxind == -1) {
    errExit("shmctl-SHM_INFO");
  }

  printf("maxind: %d\n\n", maxind);
  printf("index     id       key      shared_mems\n");

  for (ind = 0; ind <= maxind; ++ind) {
    shmid = shmctl(ind, SHM_STAT, &ds);
    if (shmid == -1) {
      if (errno != EINVAL && errno != EACCES) {
        errMsg("shmctl-SHM_STAT");
      }

      continue;
    }

    printf("%4d %8d  0x%08lx %7ld\n", ind, shmid,
           (unsigned long)ds.shm_perm.__key, (long)ds.shm_nattch);
  }

  exit(EXIT_SUCCESS);
}
