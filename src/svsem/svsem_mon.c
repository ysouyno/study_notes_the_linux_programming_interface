#include <sys/types.h>
#include <sys/sem.h>
#include <time.h>
#include "semun.h"
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  struct semid_ds ds;
  union semun arg, dummy; // 4th argument for semctl()
  int semid, j;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s semid\n", argv[0]);
  }

  semid = getInt(argv[1], 0, "semid");

  arg.buf = &ds;

  // About 2th argument `semnum`: For those operations performed on
  // a single semaphore, the semnum argument identifies a particular
  // semaphore within the set. For other operations, this argument
  // is ignored, and we can specify it as 0.
  if (semctl(semid, 0, IPC_STAT, arg) == -1) {
    errExit("semctl");
  }

  printf("Semaphore changed: %s", ctime(&ds.sem_ctime));
  printf("Last semop()     : %s", ctime(&ds.sem_otime));

  // Display per-semaphore information

  arg.array = calloc(ds.sem_nsems, sizeof(arg.array[0]));
  if (arg.array == NULL) {
    errExit("calloc");
  }

  if (semctl(semid, 0, GETALL, arg) == -1) {
    errExit("semctl-GETALL");
  }

  printf("Sem #  Value  SEMPID  SEMNCNT  SEMZCNT\n");

  for (j = 0; j < ds.sem_nsems; ++j) {
    printf("%3d   %5d   %5d  %5d    %5d\n", j, arg.array[j],
           semctl(semid, j, GETPID, dummy),
           semctl(semid, j, GETNCNT, dummy),
           semctl(semid, j, GETZCNT, dummy));
  }

  exit(EXIT_SUCCESS);
}
