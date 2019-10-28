#include <pthread.h>
#include "tlpi_hdr.h"

static pthread_cond_t thread_died = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;

// Total number of threads created
static int tot_threads = 0;

// Total number of threads still alive or terminated but not yet joined
static int num_live = 0;

// Number of terminated threads that have not yet been joined
static int num_unjoined = 0;

enum tstate {
             TS_ALIVE,
             TS_TERMINATED,
             TS_JOINED
};

static struct
{
  pthread_t tid;
  enum tstate state;
  int sleep_times;
} *thread;

static void *thread_func(void *arg)
{
  int idx = (int)arg;
  int s;

  sleep(thread[idx].sleep_times);
  printf("Thread %d terminating\n", idx);

  s = pthread_mutex_lock(&thread_mutex);
  if (s != 0) {
    errExitEN(s, "pthread_mutex_lock");
  }

  num_unjoined++;
  thread[idx].state = TS_TERMINATED;

  s = pthread_mutex_unlock(&thread_mutex);
  if (s != 0) {
    errExitEN(s, "pthread_mutex_unlock");
  }

  s = pthread_cond_signal(&thread_died);
  if (s != 0) {
    errExitEN(s, "pthread_cond_signal");
  }

  return NULL;
}

int main(int argc, char *argv[])
{
  int s, idx;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s num-secs...\n", argv[0]);
  }

  thread = calloc(argc - 1, sizeof(*thread));
  if (thread == NULL) {
    errExit("calloc");
  }

  // Create all threads

  for (idx = 0; idx < argc - 1; ++idx) {
    thread[idx].sleep_times = getInt(argv[idx + 1], GN_NONNEG, NULL);
    thread[idx].state = TS_ALIVE;
    s = pthread_create(&thread[idx].tid, NULL, thread_func, (void *)idx);
    if (s != 0) {
      errExitEN(s, "pthread_create");
    }
  }

  tot_threads = argc - 1;
  num_live = tot_threads;

  // Join with terminated threads

  while (num_live > 0) {
    s = pthread_mutex_lock(&thread_mutex);
    if (s != 0) {
      errExitEN(s, "pthread_mutex_lock");
    }

    while (num_unjoined == 0) {
      s = pthread_cond_wait(&thread_died, &thread_mutex);
      if (s != 0) {
        errExitEN(s, "pthread_cond_wait");
      }
    }

    for (idx = 0; idx < tot_threads; ++idx) {
      if (thread[idx].state == TS_TERMINATED) {
        s = pthread_join(thread[idx].tid, NULL);
        if (s != 0) {
          errExitEN(s, "pthread_join");
        }

        thread[idx].state = TS_JOINED;
        num_live--;
        num_unjoined--;

        printf("Reaped thread %d (num_live = %d)\n", idx, num_live);
      }
    }

    s = pthread_mutex_unlock(&thread_mutex);
    if (s != 0) {
      errExitEN(s, "pthread_mutex_unlock");
    }
  }

  exit(EXIT_SUCCESS);
}
