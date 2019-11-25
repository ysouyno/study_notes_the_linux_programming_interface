#include <sys/mman.h>
#include <fcntl.h>
#include "svshm_xfr.h"
#include "semun.h"

#define MMAP_TMP_FILE "/tmp/mmap_tmp_file"

int main(int argc, char *argv[])
{
  int semid, bytes, xfrs;
  struct shmseg shmp;
  union semun dummy;
  int fd;
  char *addr;

  semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
  if (semid == -1) {
    errExit("semget");
  }

  if (initSemAvailable(semid, WRITE_SEM) == -1) {
    errExit("initSemAvailable");
  }

  if (initSemInUse(semid, READ_SEM) == -1) {
    errExit("initSemInUse");
  }

  fd = open(MMAP_TMP_FILE, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    errExit("open");
  }

  if (ftruncate(fd, sizeof(struct shmseg)) == -1) {
    errExit("ftruncate");
  }

  addr = mmap(NULL, sizeof(struct shmseg), PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    errExit("mmap");
  }

  // Transfer blocks of data from stdin to shared memory

  for (xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp.cnt) {
    if (reserveSem(semid, WRITE_SEM) == -1) { // Wait for our turn
      errExit("reserveSem");
    }

    memset(shmp.buf, 0, BUF_SIZE);
    shmp.cnt = read(STDIN_FILENO, shmp.buf, BUF_SIZE);
    if (shmp.cnt == -1) {
      errExit("read");
    }

    memcpy(addr, &shmp, sizeof(struct shmseg));

    if (msync(addr, sizeof(struct shmseg), MS_SYNC) == -1) {
      errExit("msync");
    }

    if (releaseSem(semid, READ_SEM) == -1) { // Give reader a turn
      errExit("releaseSem");
    }

    // Have we reached EOF? We test this after giving the reader
    // a turn so that it can see the 0 value in shmp->cnt.

    if (shmp.cnt == 0) {
      break;
    }
  }

  // Wait until reader has let us have one more turn. We then know
  // reader has finished, and so we can delete the IPC objects.

  if (reserveSem(semid, WRITE_SEM) == -1) {
    errExit("semctl");
  }

  if (semctl(semid, 0, IPC_RMID, dummy) == -1) {
    errExit("semctl");
  }

  fprintf(stderr, "Send %d bytes (%d xfrs)\n", bytes, xfrs);
  exit(EXIT_SUCCESS);
}
