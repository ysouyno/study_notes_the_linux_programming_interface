#include <sys/mman.h>
#include <fcntl.h>
#include "svshm_xfr.h"

#define MMAP_TMP_FILE "/tmp/mmap_tmp_file"

int main(int argc, char *argv[])
{
  int semid, xfrs, bytes;
  struct shmseg *shmp;
  int fd;
  char *addr;

  // Get IDs for semaphore set and shared memory created by writer

  semid = semget(SEM_KEY, 0, 0);
  if (semid == -1) {
    errExit("semget");
  }

  fd = open(MMAP_TMP_FILE, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    errExit("open");
  }

  addr = mmap(NULL, sizeof(struct shmseg), PROT_READ, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    errExit("mmap");
  }

  // Transfer blocks of data from shared memory to stdout

  for (xfrs = 0, bytes = 0; ; xfrs++) {
    if (reserveSem(semid, READ_SEM) == -1) { // Wait for out turn
      errExit("reserveSem");
    }

    shmp = (struct shmseg *)addr;

    if (shmp->cnt == 0) { // Writer encountered EOF
      break;
    }

    bytes += shmp->cnt;

    if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt) {
      fatal("partial/failed write");
    }

    if (releaseSem(semid, WRITE_SEM) == -1) { // Give write a turn
      errExit("releaseSem");
    }
  }

  // Give writer one more turn, so it can clean up

  if (releaseSem(semid, WRITE_SEM) == -1) {
    errExit("releaseSem");
  }

  fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
  exit(EXIT_SUCCESS);
}
