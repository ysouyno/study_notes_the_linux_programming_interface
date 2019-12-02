#define _BSD_SOURCE

#include <netdb.h>
#include "is_seqnum.h"
#include "read_line_buf.h"

#define BACKLOG 50

int main(int argc, char *argv[])
{
  uint32_t seq_num;
  char req_len_str[INT_LEN];   // Length of requested sequence
  char seq_num_str[INT_LEN];   // Start of granted sequence
  struct sockaddr_storage claddr;
  int lfd, cfd, optval, req_len;
  socklen_t addrlen;
  struct addrinfo hints;
  struct addrinfo *result, *rp;
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
  char addr_str[ADDRSTRLEN];
  char host[NI_MAXHOST];
  char service[NI_MAXSERV];
  struct ReadLineBuf rlb;

  if (argc > 1 && strcmp(argv[1], "--help") == 0) {
    usageErr("%s [init-seq-num]\n", argv[0]);
  }

  seq_num = (argc > 1) ? getInt(argv[1], 0, "init-seq-num") : 0;

  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    errExit("signal");
  }

  // Call getaddrinfo() to obtain a list of addresses that
  // we can try binding to

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC; // Allows IPv4 or IPv6
  hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
                               // Wildcard IP addresses; service name is numeric

  if (getaddrinfo(NULL, PORT_NUM, &hints, &result) != 0) {
    errExit("getaddrinfo");
  }

  // Walk through returned list until we find an address structure
  // that can be used to successfully create and bind a socket

  optval = 1;
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (lfd == -1) {
      continue; // On error, try next address
    }

    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))
        == -1) {
      errExit("setsockopt");
    }

    if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0) {
      break;                   // Success
    }

    // bind() failed: close this socket and try next address

    close(lfd);
  }

  if (rp == NULL) {
    fatal("Could not bind socket to any address");
  }

  if (listen(lfd, BACKLOG) == -1) {
    errExit("listen");
  }

  freeaddrinfo(result);

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

    readLineBufInit(cfd, &rlb);

    if (readLineBuf(&rlb, req_len_str, INT_LEN) <= 0) {
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
