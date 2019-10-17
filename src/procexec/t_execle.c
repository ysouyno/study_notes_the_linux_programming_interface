#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  char *env_vec[] = { "GREET=salut", "BYE=adieu", NULL };
  char *filename;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s pathname\n", argv[0]);
  }

  filename = strrchr(argv[1], '/'); // Get basename from argv[1]
  if (filename != NULL) {
    filename++;
  }
  else {
    filename = argv[1];
  }

  execle(argv[1], filename, "hello world", (char *)NULL, env_vec);
  errExit("execle"); // If we get here, something went wrong
}
