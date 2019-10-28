#define _GNU_SOURCE // Get `_sys_nerr` and `_sys_errlist`
                    // declarations from <stdio.h>
#include <stdio.h>
#include <string.h>

#define MAX_ERROR_LEN 256

static char *buff[MAX_ERROR_LEN];

char *strerror(int err)
{
  if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) {
    snprintf(buff, MAX_ERROR_LEN, "Unknown error %d", err);
  }
  else {
    strncpy(buff, _sys_errlist[err], MAX_ERROR_LEN - 1);
    buff[MAX_ERROR_LEN - 1] = '\0';
  }

  return buff;
}
