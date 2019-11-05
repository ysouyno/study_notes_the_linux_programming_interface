#include <limits.h>
#include <ctype.h>
#include "print_wait_status.h"
#include "tlpi_hdr.h"

#define POPEN_FMT "/bin/ls -d %s 2> /dev/null"
#define PAT_SIZE 50
#define PCMD_BUF_SIZE (sizeof(POPEN_FMT) + PAT_SIZE)

int main(int argc, char *argv[])
{
  char pat[PAT_SIZE]; // Pattern for globbing
  char popen_cmd[PCMD_BUF_SIZE];
  FILE *fp; // File stream returned by popen()
  Boolean bad_pattern;
  int len, status, file_cnt, j;
  char pathname[PATH_MAX];

  for (; ; ) { // Read pattern, display results of globbing
    printf("pattern: ");
    fflush(stdout);

    if (fgets(pat, PAT_SIZE, stdin) == NULL) {
      break; // EOF
    }

    len = strlen(pat);
    if (len <= 1) { // Empty line
      continue;
    }

    if (pat[len - 1] == '\n') { // Strip trailing newline
      pat[len - 1] = '\0';
    }

    // Ensure that the pattern contains only valid character,
    // i.e., letters, digits, underscore, dot, and the shell
    // globbing characters. (Our definition of valid is more
    // restrictive than the shell, which permits other characters
    // to be included in a filename if they are quoted.)

    for (j = 0, bad_pattern = FALSE; j < len && !bad_pattern; ++j) {
      if (!isalnum((unsigned char)pat[j]) &&
          strchr("_*?[^-].", pat[j]) == NULL) {
        bad_pattern = TRUE;
      }
    }

    if (bad_pattern) {
      printf("Bad pattern character: %c\n", pat[j - 1]);
      continue;
    }

    // Build and execute command to glob `pat`

    snprintf(popen_cmd, PCMD_BUF_SIZE, POPEN_FMT, pat);
    popen_cmd[PCMD_BUF_SIZE - 1] = '\0'; // Ensure string is
                                         // null-terminated
    fp = popen(popen_cmd, "r");
    if (fp == NULL) {
      printf("popen() failed\n");
      continue;
    }

    // Read resulting list of pathnames until EOF

    file_cnt = 0;
    while (fgets(pathname, PATH_MAX, fp) != NULL) {
      printf("%s", pathname);
      file_cnt++;
    }

    // Close pipe, fetch and display termination status

    status = pclose(fp);
    printf("    %d matching file%s\n", file_cnt, (file_cnt != 1) ? "s" : "");
    printf("    pclose() status == %#x\n", (unsigned int)status);
    if (status != -1) {
      printWaitStatus("\t", status);
    }
  }

  exit(EXIT_SUCCESS);
}
