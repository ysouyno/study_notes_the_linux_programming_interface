#include <sys/stat.h>
#include <sys/ipc.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  struct stat sb;
  key_t key;

  if (argc != 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s file-name keychar", argv[0]);
  }

  printf("Size of key_t = %ld bytes\n", (long)sizeof(key_t));

  if (stat(argv[1], &sb) == -1) {
    errExit("stat");
  }

  key = ftok(argv[1], argv[2][0]);
  if (key == -1) {
    errExit("ftok");
  }

  printf("Key = %lx i-node = %lx st_dev = %lx proj = %x\n",
         (unsigned long)key, (unsigned long)sb.st_ino,
         (unsigned long)sb.st_dev, (unsigned int)argv[2][0]);
  if (key == -1) {
    printf("File does not exist\n");
  }

  exit(EXIT_SUCCESS);
}
