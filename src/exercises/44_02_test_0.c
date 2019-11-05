#include "tlpi_hdr.h"

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
  FILE *fp;
  int status;
  char buff[BUFF_SIZE];

  fp = popen("pwd", "r");
  if (fp == NULL) {
    errExit("popen");
  }

  while (fgets(buff, BUFF_SIZE, fp) != NULL) {
    printf("%s", buff);
  }

  status = pclose(fp);
  if (status == -1) {
    errExit("plose");
  }
  else {
    printf("status: %d\n", status);
  }
}
