#include <sys/stat.h>
#include <sys/sem.h>
#include "curr_time.h"  // Declaration of currTime()
#include "semun.h"
#include "tlpi_hdr.h"

static int semid;

int main(int argc, char *argv[])
{
  struct sembuf sop;
  union semun arg;

  setbuf(stdout, NULL); // Disable buffering of stdout

  semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  if (semid == -1) {
    errExit("semget");
  }

  arg.val = 1;
  if (semctl(semid, 0, SETVAL, arg) == -1) {
    errExit("semctl");
  }

  switch (fork()) {
  case -1: {
    errExit("fork");
  }
  case 0: {             // Child
    // Child does some required action here...
    printf("[%s %ld] Child started - doing some work\n",
           currTime("%T"), (long)getpid());
    sleep(2);           // Simulate time spent doing some work

    printf("[%s %ld] Child about to semaphore parent\n",
           currTime("%T"), (long)getpid());
    arg.val = 0;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
      errExit("semctl");
    }

    // Now child can do other things...

    _exit(EXIT_SUCCESS);
  }
  default:              // Parent
    // Parent may do some work here, and then waits for child to
    // complete the required action

    printf("[%s %ld] Parent about to wait for semaphore\n",
           currTime("%T"), (long)getpid());

    sop.sem_num = 0;
    sop.sem_op = 0;     // Wait for 0
    sop.sem_flg = 0;
    if (semop(semid, &sop, 1) == -1) {
      errExit("semop");
    }

    printf("[%s %ld] Parent got semaphore\n", currTime("%T"), (long)getpid());

    // Parent carries on to do other things

    exit(EXIT_SUCCESS);
  }
}
