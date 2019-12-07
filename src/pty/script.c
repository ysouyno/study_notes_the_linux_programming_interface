#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <termios.h>
#include <sys/select.h>
#include "pty_fork.h"
#include "tty_functions.h"
#include "tlpi_hdr.h"

#define BUF_SIZE 256
#define MAX_SNAME 1000

struct termios tty_orig;

static void tty_reset()
{
  if (tcsetattr(STDIN_FILENO, TCSANOW, &tty_orig) == -1) {
    errExit("tcsetattr");
  }
}

int main(int argc, char *argv[])
{
  char slave_name[MAX_SNAME];
  char *shell;
  int master_fd, script_fd;
  struct winsize ws;
  fd_set in_fds;
  char buf[BUF_SIZE];
  ssize_t num_read;
  pid_t child_pid;

  if (tcgetattr(STDIN_FILENO, &tty_orig) == -1) {
    errExit("tcgetattr");
  }

  if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0) {
    errExit("ioctl-TIOCGWINSZ");
  }

  child_pid = ptyFork(&master_fd, slave_name, MAX_SNAME, &tty_orig, &ws);
  if (child_pid == -1) {
    errExit("ptyFork");
  }

  if (child_pid == 0) { // Child: execute a shell on pty slave
    shell = getenv("SHELL");
    if (shell == NULL || *shell == '\0') {
      shell = "/bin/sh";
    }

    execlp(shell, shell, (char *)NULL);
    errExit("execlp"); // If we get here, something went wrong
  }

  // Parent: relay data between terminal and pty master

  script_fd = open((argc > 1) ? argv[1] : "typescript",
                   O_WRONLY | O_CREAT | O_TRUNC,
                   S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (script_fd == -1) {
    errExit("open typescript");
  }

  ttySetRaw(STDIN_FILENO, &tty_orig);

  if (atexit(tty_reset) != 0) {
    errExit("atexit");
  }

  for (; ; ) {
    FD_ZERO(&in_fds);
    FD_SET(STDIN_FILENO, &in_fds);
    FD_SET(master_fd, &in_fds);

    if (select(master_fd + 1, &in_fds, NULL, NULL, NULL) == -1) {
      errExit("select");
    }

    if (FD_ISSET(STDIN_FILENO, &in_fds)) { // stdin --> pty
      num_read = read(STDIN_FILENO, buf, BUF_SIZE);
      if (num_read <= 0) {
        exit(EXIT_SUCCESS);
      }

      if (write(master_fd, buf, num_read) != num_read) {
        fatal("partial/failed write (master_fd)");
      }
    }

    if (FD_ISSET(master_fd, &in_fds)) { // pth --> stdout + file
      num_read = read(master_fd, buf, BUF_SIZE);
      if (num_read <= 0) {
        exit(EXIT_SUCCESS);
      }

      if (write(STDOUT_FILENO, buf, num_read) != num_read) {
        fatal("partial/failed write (STDOUT_FILENO)");
      }

      if (write(script_fd, buf, num_read) != num_read) {
        fatal("partial/failed write (script_fd)");
      }
    }
  }
}
