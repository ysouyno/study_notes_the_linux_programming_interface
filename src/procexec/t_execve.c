#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  char *arg_vec[10]; // Larger than required
  char *env_vec[] = { "GREET=salut", "BYE=adieu", NULL };

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s pathname\n", argv[0]);
  }

  arg_vec[0] = strrchr(argv[1], '/'); // Get basename from argv[1]
  if (arg_vec[0] != NULL) {
    arg_vec[0]++;
  }
  else {
    arg_vec[0] = argv[1];
  }

  arg_vec[1] = "hello world";
  arg_vec[2] = "goodbye";
  arg_vec[3] = NULL; // List must be NULL-terminated

  execve(argv[1], arg_vec, env_vec);
  errExit("execve"); // If we get here, something went wrong
}
