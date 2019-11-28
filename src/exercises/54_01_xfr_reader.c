#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "svshm_xfr.h"

#define SHM_TEST_NAME "/shm_demo"

int main(int argc, char *argv[])
{
  int semid, xfrs, bytes;
  struct shmseg *shmp;
  int fd;
  char *addr;
  struct stat sb;

  // Get IDs for semaphore set and shared memory created by writer

  semid = semget(SEM_KEY, 0, 0);
  if (semid == -1) {
    errExit("semget");
  }

  fd = shm_open(SHM_TEST_NAME, O_RDWR, 0); // Open existing object
  if (fd == -1) {
    errExit("shm_open");
  }

  if (fstat(fd, &sb) == -1) {
    errExit("fstat");
  }

  addr = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    errExit("mmap");
  }

  shmp = (struct shmseg *)addr;

  // Transfer blocks of data from shared memory to stdout

  for (xfrs = 0, bytes = 0; ; xfrs++) {
    if (reserveSem(semid, READ_SEM) == -1) { // Wait for out turn
      errExit("reserveSem");
    }

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
