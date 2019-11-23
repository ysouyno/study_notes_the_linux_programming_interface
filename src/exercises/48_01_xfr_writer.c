#include "svshm_xfr.h"
#include "semun.h"
#include "event_flags.h"

int main(int argc, char *argv[])
{
  int semid, shmid, bytes, xfrs;
  struct shmseg *shmp;
  union semun dummy;

  semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
  if (semid == -1) {
    errExit("semget");
  }

  if (setEventFlag(semid, WRITE_SEM) == -1) {
    errExit("setEventFlag");
  }

  if (clearEventFlag(semid, READ_SEM) == -1) {
    errExit("clearEventFlag");
  }

  shmid = shmget(SHM_KEY, sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS);
  if (shmid == -1) {
    errExit("shmget");
  }

  shmp = shmat(shmid, NULL, 0);
  if (shmp == (void *)-1) {
    errExit("shmat");
  }

  // Transfer blocks of data from stdin to shared memory

  for (xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp->cnt) {
    if (waitForEventFlag(semid, WRITE_SEM) == -1) { // Wait for our turn
      errExit("waitForEventFlag");
    }

    if (clearEventFlag(semid, WRITE_SEM) == -1) {
      errExit("clearEventFlag");
    }

    shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
    if (shmp->cnt == -1) {
      errExit("read");
    }

    if (setEventFlag(semid, READ_SEM) == -1) {
      errExit("setEventFlag");
    }

    // Have we reached EOF? We test this after giving the reader
    // a turn so that it can see the 0 value in shmp->cnt.

    if (shmp->cnt == 0) {
      break;
    }
  }

  // Wait until reader has let us have one more turn. We then know
  // reader has finished, and so we can delete the IPC objects.

  if (waitForEventFlag(semid, WRITE_SEM) == -1) {
    errExit("waitForEventFlag");
  }

  if (semctl(semid, 0, IPC_RMID, dummy) == -1) {
    errExit("semctl");
  }

  if (shmdt(shmp) == -1) {
    errExit("shmdt");
  }

  if (shmctl(shmid, IPC_RMID, 0) == -1) {
    errExit("shmctl");
  }

  fprintf(stderr, "Send %d bytes (%d xfrs)\n", bytes, xfrs);
  exit(EXIT_SUCCESS);
}
