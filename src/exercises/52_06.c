#include <signal.h>
#include <mqueue.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#define NOTIFY_SIG SIGUSR1

int main(int argc, char *argv[])
{
  struct sigevent sev;
  mqd_t mqd;
  struct mq_attr attr;
  void *buffer;
  ssize_t num_read;
  sigset_t all_sigs;
  int sig;
  siginfo_t si;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s mq-name\n", argv[0]);
  }

  mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
  if (mqd == (mqd_t)-1) {
    errExit("mq_open");
  }

  if (mq_getattr(mqd, &attr) == -1) {
    errExit("mq_getattr");
  }

  buffer = malloc(attr.mq_msgsize);
  if (buffer == NULL) {
    errExit("malloc");
  }

  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = NOTIFY_SIG;
  sev.sigev_value.sival_ptr = &mqd;
  if (mq_notify(mqd, &sev) == -1) {
    errExit("mq_notify");
  }

  // Block all signals (except SIGKILL and SIGSTOP)

  sigfillset(&all_sigs);
  if (sigprocmask(SIG_SETMASK, &all_sigs, NULL) == -1) {
    errExit("sigprocmask");
  }

  for (; ; ) {
    sig = sigwaitinfo(&all_sigs, &si);
    if (sig == -1) {
      errExit("sigwaitinfo");
    }

    if (sig == SIGINT || sig == SIGTERM) {
      exit(EXIT_SUCCESS);
    }

    if (sig != NOTIFY_SIG) {
      continue;
    }

    printf("si_code:   %s\n", (si.si_code == SI_MESGQ) ? "SI_MESGQ" : "???");
    printf("si_signo:  %d\n", si.si_signo);
    printf("si_pid:    %d\n", si.si_pid);
    printf("si_uid:    %d\n", si.si_uid);
    printf("sival_ptr: %p\n", si.si_value.sival_ptr);

    if (mq_notify(mqd, &sev) == -1) {
      errExit("mq_notify");
    }

    while ((num_read = mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) >= 0) {
      printf("Read %ld bytes\n", (long)num_read);
    }

    if (errno != EAGAIN) { // Unexpected error
      errExit("mq_receive");
    }
  }
}
