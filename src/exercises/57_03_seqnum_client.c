#include <sys/socket.h>
#include <sys/un.h>
#include "fifo_seqnum.h"

#define SV_SOCK_PATH "/tmp/us_xfr"

int main(int argc, char *argv[])
{
  struct request req;
  struct response resp;
  int sfd;
  struct sockaddr_un addr;

  if (argc > 1 && strcmp(argv[1], "--help") == 0) {
    usageErr("%s [seq-len...]\n", argv[0]);
  }

  umask(0);               // So we get the permission we want

  sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd == -1) {
    errExit("socket");
  }

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

  if (connect(sfd, (struct sockaddr *)&addr,
              sizeof(struct sockaddr_un)) == -1) {
    errExit("connect");
  }

  // Construct request message, open server FIFO, and send request

  req.pid = getpid();
  req.seq_len = (argc > 1) ? getInt(argv[1], GN_GT_0, "seq-len") : 1;

  if (write(sfd, &req, sizeof(struct request)) != sizeof(struct request)) {
    fatal("Can't write to server");
  }

  if (read(sfd, &resp, sizeof(struct response)) != sizeof(struct response)) {
    fatal("Can't read response from server");
  }

  printf("%d\n", resp.seq_num);
  exit(EXIT_SUCCESS);
}
