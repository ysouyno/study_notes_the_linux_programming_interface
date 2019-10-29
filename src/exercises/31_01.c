#include <pthread.h>
#include "tlpi_hdr.h"

struct once_t
{
  pthread_mutex_t mtx;
  int called;
};

#define ONCE_INITIALIZER { PTHREAD_MUTEX_INITIALIZER, 0 }

struct once_t once = ONCE_INITIALIZER;

static int one_time_init(struct once_t *once_control, void (*init)())
{
  int s;

  s = pthread_mutex_lock(&(once_control->mtx));
  if (s != 0) {
    errExitEN(s, "pthread_mutex_lock");
  }

  if (!once_control->called) {
    (*init)();
    once_control->called = 1;
  }

  s = pthread_mutex_unlock(&(once_control->mtx));
  if (s != 0) {
    errExitEN(s, "pthread_mutex_unlock");
  }

  return 0;
}

static void init_func()
{
  printf("init_func() called\n");
}

static void *thread_func(void *arg)
{
  one_time_init(&once, init_func);
  one_time_init(&once, init_func);

  return NULL;
}

int main(int argc, char *argv[])
{
  pthread_t t1, t2;
  int s;

  s = pthread_create(&t1, NULL, thread_func, NULL);
  if (s != 0) {
    errExitEN(s, "pthread_create");
  }

  s = pthread_create(&t2, NULL, thread_func, NULL);
  if (s != 0) {
    errExitEN(s, "pthread_create");
  }

  s = pthread_join(t1, NULL);
  if (s != 0) {
    errExitEN(s, "pthread_join");
  }
  printf("1st thread returned\n");

  s = pthread_join(t2, NULL);
  if (s != 0) {
    errExitEN(s, "pthread_join");
  }
  printf("2nd thread returned\n");

  exit(EXIT_SUCCESS);
}
