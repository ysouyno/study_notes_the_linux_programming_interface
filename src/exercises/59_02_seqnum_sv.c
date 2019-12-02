#define _BSD_SOURCE

#include <netdb.h>
#include "is_seqnum.h"
#include "inet_sockets.h"

#define BACKLOG 50

int main(int argc, char *argv[])
{
  uint32_t seq_num;
  char req_len_str[INT_LEN];   // Length of requested sequence
  char seq_num_str[INT_LEN];   // Start of granted sequence
  struct sockaddr_storage claddr;
  int lfd, cfd, req_len;
  socklen_t addrlen;
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
  char addr_str[ADDRSTRLEN];
  char host[NI_MAXHOST];
  char service[NI_MAXSERV];

  if (argc > 1 && strcmp(argv[1], "--help") == 0) {
    usageErr("%s [init-seq-num]\n", argv[0]);
  }

  seq_num = (argc > 1) ? getInt(argv[1], 0, "init-seq-num") : 0;

  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    errExit("signal");
  }

  lfd = inetListen(PORT_NUM, BACKLOG, &addrlen);
  if (lfd == -1) {
    errExit("inetListen");
  }

  for (; ; ) {
    // Accept a client connection, obtaining client's address
    addrlen = sizeof(struct sockaddr_storage);
    cfd = accept(lfd, (struct sockaddr *)&claddr, &addrlen);
    if (cfd == -1) {
      errMsg("accept");
      continue;
    }

    if (getnameinfo((struct sockaddr *)&claddr, addrlen, host, NI_MAXHOST,
                    service, NI_MAXSERV, 0) == 0) {
      snprintf(addr_str, ADDRSTRLEN, "(%s, %s)", host, service);
    }
    else {
      snprintf(addr_str, ADDRSTRLEN, "(?UNKNOWN?)");
    }
    printf("Connection from %s\n", addr_str);

    // Read client request, send sequence number back

    if (readLine(cfd, req_len_str, INT_LEN) <= 0) {
      close(cfd);
      continue;                // Failed read; skip request
    }

    req_len = atoi(req_len_str);
    if (req_len <= 0) {        // Watch for misbehaving clients
      close(cfd);
      continue;                // Bad request; skip it
    }

    snprintf(seq_num_str, INT_LEN, "%d\n", seq_num);
    if (write(cfd, seq_num_str, strlen(seq_num_str)) != strlen(seq_num_str)) {
      fprintf(stderr, "Error on write");
    }

    seq_num += req_len;        // Update sequence number

    if (close(cfd) == -1) {
      errMsg("close");
    }
  }
}
