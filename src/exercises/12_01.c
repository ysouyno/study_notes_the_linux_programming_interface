#include "tlpi_hdr.h"
#include "ugid_functions.h"
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define BUFMAX 1024

static void list_proc_subdirs(const char *user) {
  DIR *dirp;
  struct dirent *dp;
  char dirpath[] = "/proc";
  pid_t pid;
  int fd;
  char pidpath[128];
  char buf[BUFMAX];
  int num_read;
  char *p, *q;

  if (!user)
    return;

  dirp = opendir(dirpath);
  if (dirp == NULL) {
    errMsg("opendir failed on '%s'", dirpath);
    return;
  }

  for (;;) {
    errno = 0;
    dp = readdir(dirp);
    if (dp == NULL)
      break;

    if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 ||
        dp->d_type != DT_DIR)
      continue;

    pid = atoll(dp->d_name);
    if (0 == pid)
      continue;

    sprintf(pidpath, "%s/%s/status", dirpath, dp->d_name);
    printf("%s\n", pidpath);

    fd = open(pidpath, O_RDONLY);
    if (-1 == fd)
      errExit("open");

    while ((num_read = read(fd, buf, BUFMAX))) {
    }

    close(fd);
  }

  if (errno != 0)
    errExit("readdir");

  if (closedir(dirp) == -1)
    errMsg("closedir");
}

int main(int argc, char *argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    usageErr("%s [user-name]\n", argv[0]);

  list_proc_subdirs(argv[1]);

  exit(EXIT_SUCCESS);
}
