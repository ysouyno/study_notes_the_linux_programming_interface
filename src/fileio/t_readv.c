#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <tlpi_hdr.h>

int main(int argc, char *argv[])
{
  int fd;
  struct iovec iov[3];
  struct stat my_struct; // First buffer
  int x;                 // Second buffer
#define STR_SIZE 100
  char str[STR_SIZE];    // Third buffer
  ssize_t num_read, tot_required;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s file\n", argv[0]);
  }

  fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    errExit("open");
  }

  tot_required = 0;

  iov[0].iov_base = &my_struct;
  iov[0].iov_len = sizeof(struct stat);
  tot_required += iov[0].iov_len;

  iov[1].iov_base = &x;
  iov[1].iov_len = sizeof(x);
  tot_required += iov[1].iov_len;

  iov[2].iov_base = str;
  iov[2].iov_len = STR_SIZE;
  tot_required += iov[2].iov_len;

  num_read = readv(fd, iov, 3);
  if (num_read == -1) {
    errExit("readv");
  }

  if (num_read < tot_required) {
    printf("Read fewer bytes than requested\n");
  }

  printf("Total bytes requested: %ld; bytes read: %ld\n",
         (long)tot_required, (long)num_read);

  exit(EXIT_SUCCESS);
}
