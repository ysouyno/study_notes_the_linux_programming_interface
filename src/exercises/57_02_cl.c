#include <sys/un.h>
#include <sys/socket.h>
#include "tlpi_hdr.h"

#define SV_SOCK_PATH "/tmp/us_xfr"
#define BUF_SIZE 100

int main(int argc, char *argv[])
{
  struct sockaddr_un addr;
  int sfd;
  ssize_t num_read;
  char buf[BUF_SIZE];

  sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd == -1) {
    errExit("socket");
  }

  // Construct server address, and make the connection

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(&addr.sun_path[1], SV_SOCK_PATH, sizeof(addr.sun_path) - 2);

  if (connect(sfd, (struct sockaddr *)&addr,
              sizeof(struct sockaddr_un)) == -1) {
    errExit("connect");
  }

  // Copy stdin to socket

  while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
    if (write(sfd, buf, num_read) != num_read) {
      fatal("partial/failed write");
    }
  }

  if (num_read == -1) {
    errExit("read");
  }

  exit(EXIT_SUCCESS);
}
