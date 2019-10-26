#include <pthread.h>
#include "tlpi_hdr.h"

pthread_t tid;

static void *thread_func(void *arg)
{
  int s;

  if (!pthread_equal(tid, pthread_self())) {
    s = pthread_join(pthread_self(), NULL);
    if (s != 0) {
      errExitEN(s, "pthread_join");
    }
  }

  printf("pthread_join() execed in thread_func\n");
  return NULL;
}

int main(int argc, char *argv[])
{
  int s;

  s = pthread_create(&tid, NULL, thread_func, NULL);
  if (s != 0) {
    errExitEN(s, "pthread_create");
  }

  getchar();
  exit(EXIT_SUCCESS);
}
