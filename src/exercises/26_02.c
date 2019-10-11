#include <sys/wait.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  int parent_pid, grandchild_pid;
  int status;

  switch (parent_pid = fork()) {
  case -1:
    perror("fork");
    break;
  case 0: {     // parent: grandparent's child
    switch (grandchild_pid = fork()) {
    case -1:
      perror("fork");
      break;
    case 0:     // grandchild: parent's child
      sleep(2); // wait until parent become zombie
      printf("after parent quit before grandparent wait: parent id: %ld\n",
             (long)getppid());
      sleep(2); // wait grandparent call wait()
      printf("after grandparent wait: parent id: %ld\n", (long)getppid());
      printf("grandchild quit\n");
      _exit(EXIT_SUCCESS);
    default:    // parent
      printf("parent quit\n");
      exit(EXIT_SUCCESS);
    }
  }
  default:      // grandparent
    sleep(3);   // let parent become a zombie process
    waitpid(parent_pid, &status, WNOHANG);
    printf("waitpid execed\n");
    exit(EXIT_SUCCESS);
  }
}
