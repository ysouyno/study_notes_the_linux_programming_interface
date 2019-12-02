#include <netdb.h>
#include "is_seqnum.h"
#include "inet_sockets.h"

int main(int argc, char *argv[])
{
  char *req_len_str;
  char seq_num_str[INT_LEN];
  int cfd;
  ssize_t num_read;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s server-host [sequence-len]\n", argv[0]);
  }

  cfd = inetConnect(argv[1], PORT_NUM, SOCK_STREAM);
  if (cfd == -1) {
    errExit("inetConnect");
  }

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
