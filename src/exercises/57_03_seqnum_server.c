#include <sys/socket.h>
#include <sys/un.h>
#include "fifo_seqnum.h"

#define SV_SOCK_PATH "/tmp/us_xfr"
#define BACKLOG 5

int main(int argc, char *argv[])
{
  struct request req;
  struct response resp;
  int seq_num = 0;          // This is our "service"
  struct sockaddr_un addr;
  int sfd, cfd;

  // Create well-known FIFO, and open it for reading

  umask(0);                 // So we get the permissions we want

  sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd == -1) {
    errExit("socket");
  }

  if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
    errExit("remove-%s", SV_SOCK_PATH);
  }

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
    errExit("bind");
  }

  if (listen(sfd, BACKLOG) == -1) {
    errExit("listen");
  }

  for (; ; ) {              // Read requests and send responses
    cfd = accept(sfd, NULL, NULL);
    if (cfd == -1) {
      errExit("accept");
    }

    if (read(cfd, &req, sizeof(struct request)) != sizeof(struct request)) {
      fprintf(stderr, "Error reading request; discarding\n");
      continue;
    }

    printf("read ok\n");

    resp.seq_num = seq_num;
    if (write(cfd, &resp, sizeof(struct response)) != sizeof(struct response)) {
      fprintf(stderr, "Error writing to client\n");
    }

    if (close(cfd) == -1) {
      errMsg("close");
    }

    seq_num += req.seq_len; // Update our sequence number
  }
}
