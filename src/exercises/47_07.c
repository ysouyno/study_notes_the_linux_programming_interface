#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/sem.h>
#include "semun.h"
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  int maxtind, ind, semid;
  struct semid_ds ds;
  struct seminfo seminfo;

  // Obtain size of kernel `entries` array

  maxtind = semctl(0, 0, SEM_INFO, (struct semid_ds *)&seminfo);
  if (maxtind == -1) {
    errExit("semctl-SEM_INFO");
  }

  printf("maxtind: %d\n\n", maxtind);
  printf("index     id       key      semaphores\n");

  // Retrieve and display information from each element of `entries` array

  for (ind = 0; ind <= maxtind; ++ind) {
    semid = semctl(ind, 0, SEM_STAT, &ds);
    if (semid == -1) {
      if (errno != EINVAL && errno != EACCES) {
        errMsg("semctl-SEM_STAT"); // Unexpected error
      }

      continue;                    // Ignore this item
    }

    printf("%4d %8d  0x%08lx %7ld\n", ind, semid,
           (unsigned long)ds.sem_perm.__key, (long)ds.sem_nsems);
  }

  exit(EXIT_SUCCESS);
}
