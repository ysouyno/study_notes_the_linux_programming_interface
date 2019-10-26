#include <pthread.h>
#include <time.h>
#include "tlpi_hdr.h"

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static int avail = 0;

static void *thread_func(void *arg)
{
  int cnt = atoi((char *)arg);
  int s, j;

  for (j = 0; j < cnt; ++j) {
    sleep(1);

    // Code to produce a unit omitted

    s = pthread_mutex_lock(&mtx);
    if (s != 0) {
      errExitEN(s, "pthread_mutex_lock");
    }

    avail++;

    s = pthread_mutex_unlock(&mtx);
    if (s != 0) {
      errExitEN(s, "pthread_mutex_unlock");
    }
  }

  return NULL;
}

int main(int argc, char *argv[])
{
  pthread_t tid;
  int s, j;
  int tot_required; // Total number of units that all threads will produce
  int num_consumed; // Total units so far consumed
  Boolean done;
  time_t t;

  t = time(NULL);

  // Create all threads

  tot_required = 0;

  for (j = 1; j < argc; ++j) {
    tot_required += atoi(argv[j]);

    s = pthread_create(&tid, NULL, thread_func, argv[j]);
    if (s != 0) {
      errExitEN(s, "pthread_create");
    }
  }

  // Use a polling loop to check for available units

  num_consumed = 0;
  done = FALSE;

  for (; ; ) {
    s = pthread_mutex_lock(&mtx);
    if (s != 0) {
      errExitEN(s, "pthread_mutex_lock");
    }

    while (avail > 0) {
      // Do something with produced units

      num_consumed++;
      avail--;
      printf("T = %ld: num_consumed = %d\n",
             (long)(time(NULL) - t), num_consumed);

      done = num_consumed >= tot_required;
    }

    s = pthread_mutex_unlock(&mtx);
    if (s != 0) {
      errExitEN(s, "pthread_mutex_unlock");
    }

    if (done) {
      break;
    }

    // Perhaps do other work here that does not require mutex lock
  }

  exit(EXIT_SUCCESS);
}
