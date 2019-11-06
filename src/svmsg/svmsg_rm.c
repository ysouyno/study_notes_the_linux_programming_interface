#include <sys/types.h>
#include <sys/msg.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  if (argc > 1 && strcmp(argv[1], "--help") == 0) {
    usageErr("%s [msqid...]\n", argv[0]);
  }

  for (int i = 1; i < argc ; ++i) {
    if (msgctl(getInt(argv[1], 0, "msqid"), IPC_RMID, NULL) == -1) {
      errExit("msgctl");
    }
  }

  exit(EXIT_SUCCESS);
}
