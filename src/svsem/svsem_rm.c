#include <sys/types.h>
#include <sys/sem.h>
#include "semun.h"
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  union semun dummy;

  if (argc > 1 && strcmp(argv[1], "--help") == 0) {
    usageErr("%s [semid...]\n", argv[0]);
  }

  for (int i = 1; i < argc; ++i) {
    if (semctl(getInt(argv[i], 0, "semid"), 0, IPC_RMID, dummy) == -1) {
      errExit("semctl %s", argv[i]);
    }
  }

  exit(EXIT_SUCCESS);
}
