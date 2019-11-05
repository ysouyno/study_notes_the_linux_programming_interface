// Reference:
// https://www.retro11.de/ouxr/211bsd/usr/src/lib/libc/gen/popen.c.html

#include <sys/wait.h>
#include <signal.h>
#include "tlpi_hdr.h"

extern char **environ;
static int *pids;

FILE *popen(const char *command, const char *type)
{
  FILE *iop;
  int pfd[2];
  char *argp[] = {"sh", "-c", NULL, NULL};
  pid_t pid;
  int fds;

  printf("fake_popen()\n");

  if ((*type != 'r' && *type != 'w') || type[1] != '\0') {
    errno = EINVAL;
    return NULL;
  }

  if (pids == NULL) {
    if ((fds = getdtablesize()) <= 0) {
      return NULL;
    }

    if ((pids = malloc(fds * sizeof(int))) == NULL) {
      return NULL;
    }

    bzero(pids, fds * sizeof(int));
  }

  if (pipe(pfd) == -1) {
    perror("pipe");
    return NULL;
  }

  switch (pid = fork()) {
  case -1:
    close(pfd[0]);
    close(pfd[1]);
    return NULL;
  case 0:
    if (*type == 'r') {
      // `r` means parent reads from child, so child is writing
      // process, that means need to close `pfd[0]`
      if (close(pfd[0]) == -1) {
        perror("close");
        return NULL;
      }

      if (pfd[1] != STDOUT_FILENO) {
        dup2(pfd[1], STDOUT_FILENO);
        close(pfd[1]);
      }
    }
    else {
      if (close(pfd[1]) == -1) {
        perror("close");
        return NULL;
      }

      if (pfd[0] != STDIN_FILENO) {
        dup2(pfd[0], STDIN_FILENO);
        close(pfd[0]);
      }
    }

    argp[2] = (char *)command;
    execve("/bin/sh", argp, environ);
    _exit(127);
  }

  // Parent, assume fdopen can't fail
  if (*type == 'r') {
    iop = fdopen(pfd[0], type);
    close(pfd[1]);
  }
  else {
    iop = fdopen(pfd[1], type);
    close(pfd[0]);
  }

  pids[fileno(iop)] = pid;
  return iop;
}

int pclose(FILE *iop)
{
  int fdes;
  sigset_t omask, nmask;
  int status;
  pid_t pid;

  printf("fake_pclose()\n");

  // pclose return -1 if stream is not associated with a
  // `popened` command, if already `pclosed`, or waitpid
  // return an error.
  if (pids == NULL || pids[fdes = fileno(iop)] == 0) {
    return -1;
  }

  fclose(iop);

  sigemptyset(&nmask);
  sigaddset(&nmask, SIGINT);
  sigaddset(&nmask, SIGQUIT);
  sigaddset(&nmask, SIGHUP);

  sigprocmask(SIG_BLOCK, &nmask, &omask);
  do
    {
      pid = waitpid(pids[fdes], &status, 0);
    } while (pid == -1 && errno == EINTR);
  sigprocmask(SIG_SETMASK, &omask, NULL);

  pids[fdes] = 0;

  return (pid == -1 ? -1 : status);
}
