#include <sys/shm.h>
#include <time.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  struct shmid_ds ds;
  int shmid;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s shmid\n", argv[0]);
  }

  shmid = getInt(argv[1], 0, "shmid");

  if (shmctl(shmid, IPC_STAT, &ds) == -1) {
    errExit("shmctl");
  }

  printf("Time of last shmat():                   %s", ctime(&ds.shm_atime));
  printf("Time of last shmdt():                   %s", ctime(&ds.shm_dtime));
  printf("Time of last change:                    %s", ctime(&ds.shm_ctime));
  printf("Size of segment in bytes:               %ld\n", (long)ds.shm_segsz);
  printf("PID of creator:                         %ld\n", (long)ds.shm_cpid);
  printf("PID of last shmat() / shmdt():          %ld\n", (long)ds.shm_lpid);
  printf("Number of currently attached processes: %ld\n", (long)ds.shm_nattch);

  exit(EXIT_SUCCESS);
}
