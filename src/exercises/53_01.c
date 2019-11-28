#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 1024

char buf[BUF_SIZE];
static sem_t sem1, sem2;
static int cnt;

static void *thread_wr_proc(void *arg)
{
  int xfrs, bytes;
  int fd;

  fd = *((int *)arg);

  for (xfrs = 0, bytes = 0; ; xfrs++, bytes += cnt) {
    if (sem_wait(&sem1) == -1) {
      errExit("sem_wait wr");
    }

    cnt = read(fd, buf, BUF_SIZE);
    if (cnt == -1) {
      errExit("read");
    }

    if (sem_post(&sem2) == -1) {
      errExit("sem_post wr");
    }

    if (cnt == 0) {
      break;
    }
  }

  printf("Sent     %d bytes (%d xfrs)\n", bytes, xfrs);
  return NULL;
}

static void *thread_rd_proc(void *arg)
{
  int xfrs, bytes;
  int fd;

  fd = *((int *)arg);

  for (xfrs = 0, bytes = 0; ; xfrs++) {
    if (sem_wait(&sem2) == -1) {
      errExit("sem_wait rd");
    }

    if (cnt == 0) {
      break;
    }
    bytes += cnt;

    if (write(fd, buf, cnt) != cnt) {
      fatal("partial/failed write");
    }

    if (sem_post(&sem1) == -1) {
      errExit("sem_post rd");
    }
  }

  printf("Received %d bytes (%d xfrs)\n", bytes, xfrs);
  return NULL;
}

int main(int argc, char *argv[])
{
  int fd_src, fd_dst;
  pthread_t t1, t2;
  int s;

  if (argc != 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s SRCFILE DSTFILE\n", argv[0]);
  }

  fd_src = open(argv[1], O_RDONLY);
  if (fd_src == -1) {
    errExit("open SRCFILE");
  }

  fd_dst = open(argv[2], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd_dst == -1) {
    errExit("open DSTFILE");
  }

  if (sem_init(&sem1, 0, 1) == -1) {
    errExit("sem_init");
  }

  if (sem_init(&sem2, 0, 0) == -1) {
    errExit("sem_init");
  }

  s = pthread_create(&t1, NULL, thread_wr_proc, &fd_src);
  if (s != 0) {
    errExitEN(s, "pthread_create");
  }

  s = pthread_create(&t2, NULL, thread_rd_proc, &fd_dst);
  if (s != 0) {
    errExitEN(s, "pthread_create");
  }

  s = pthread_join(t1, NULL);
  if (s != 0) {
    errExitEN(s, "pthread_join");
  }

  s = pthread_join(t2, NULL);
  if (s != 0) {
    errExitEN(s, "pthread_join");
  }

  exit(EXIT_SUCCESS);
}
