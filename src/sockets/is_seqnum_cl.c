#include <netdb.h>
#include "is_seqnum.h"

int main(int argc, char *argv[])
{
  char *req_len_str;
  char seq_num_str[INT_LEN];
  int cfd;
  ssize_t num_read;
  struct addrinfo hints;
  struct addrinfo *result, *rp;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s server-host [sequence-len]\n", argv[0]);
  }

  // Call getaddrinfo() to obtain a list of address that
  // we can try connecting to

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_NUMERICSERV;

  if (getaddrinfo(argv[1], PORT_NUM, &hints, &result) != 0) {
    errExit("getaddrinfo");
  }

  // Walk through returned list until we find an address structure
  // that can be used to successfully connect a socket

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (cfd == -1) {
      continue;
    }

    if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1) {
      break;
    }

    // Connect failed: close this socket and try next address

    close(cfd);
  }

  if (rp == NULL) {
    fatal("Could not connect socket to any address");
  }

  freeaddrinfo(result);

  // Send requested sequence length, with terminating newline

  req_len_str = (argc > 2) ? argv[2] : "1";

  if (write(cfd, req_len_str, strlen(req_len_str)) != strlen(req_len_str)) {
    fatal("Partial/failed write (req_len_str)");
  }

  if (write(cfd, "\n", 1) != 1) {
    fatal("Partial/failed write (newline)");
  }

  // Read and display sequence number returned by server

  num_read = readLine(cfd, seq_num_str, INT_LEN);
  if (num_read == -1) {
    errExit("readLine");
  }

  if (num_read == 0) {
    fatal("Unexpected EOF from server");
  }

  printf("Sequence number: %s", seq_num_str); // Includes '\n'

  exit(EXIT_SUCCESS);
}
