#include <signal.h>
#include <stdlib.h>
#include "fifo_seqnum.h"

#define SEQNUM_NO_FILE "/tmp/seqnum_no"

const int buff_len = 128;

static int get_seq_num();
void set_seq_num(int seq_num);

static int get_seq_num()
{
  char buff[buff_len];
  int fd, num_read, seq_num = 0;

  fd = open(SEQNUM_NO_FILE, O_RDWR | O_CREAT | O_SYNC, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    errExit("open");
  }

  num_read = read(fd, buff, buff_len);
  if (num_read == -1) {
    close(fd);
    errExit("read");
  }

  if (num_read == 0) {
    if (write(fd, &seq_num, sizeof(int)) != sizeof(int)) {
      close(fd);
      errExit("write");
    }
  }
  else {
    seq_num = atoi(buff);
  }

  close(fd);
  return seq_num;
}

void set_seq_num(int seq_num)
{
  int fd, num_written;
  char buff[buff_len];

  fd = open(SEQNUM_NO_FILE, O_RDWR | O_CREAT | O_SYNC, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    errExit("open");
  }

  sprintf(buff, "%d", seq_num);

  num_written = write(fd, buff, strlen(buff));
  if (num_written == -1) {
    close(fd);
    errExit("write");
  }

  close(fd);
}

int main(int argc, char *argv[])
{
  int server_fd, dummy_fd, client_fd;
  char client_fifo[CLIENT_FIFO_NAME_LEN];
  struct request req;
  struct response resp;
  int seq_num = 0;          // This is our "service"

  // Create well-known FIFO, and open it for reading

  umask(0);                 // So we get the permissions we want

  if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 &&
      errno != EEXIST) {
    errExit("mkfifo", SERVER_FIFO);
  }

  // The server's open() blocks until the first client opens
  // the other end of the server FIFO for writing

  server_fd = open(SERVER_FIFO, O_RDONLY);
  if (server_fd == -1) {
    errExit("open %s", SERVER_FIFO);
  }

  // Open an extra write descriptor, so that we never see EOF

  dummy_fd = open(SERVER_FIFO, O_WRONLY);
  if (dummy_fd == -1) {
    errExit("open %s", SERVER_FIFO);
  }

  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    errExit("signal");
  }

  for (; ; ) {              // Read requests and send responses
    seq_num = get_seq_num();
    if (read(server_fd, &req, sizeof(struct request)) !=
        sizeof(struct request)) {
      fprintf(stderr, "Error reading request; discarding\n");
      continue;             // Either partial read or error
    }

    // Open client FIFO (previously created by client)

    snprintf(client_fifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
             (long)req.pid);
    client_fd = open(client_fifo, O_WRONLY);
    if (client_fd == -1) {  // Open failed, give up on client
      errMsg("open %s", client_fifo);
      continue;
    }

    // Send responses and close FIFO

    resp.seq_num = seq_num;
    if (write(client_fd, &resp, sizeof(struct response)) !=
        sizeof(struct response)) {
      fprintf(stderr, "Error writing to FIFO %s\n", client_fifo);
    }

    if (close(client_fd) == -1) {
      errMsg("close");
    }

    seq_num += req.seq_len; // Update our sequence number
    set_seq_num(seq_num);
  }
}
