#define _GNU_SOURCE // Get `_sys_nerr` and `_sys_errlist`
                    // declarations from <stdio.h>
#include <stdio.h>
#include <string.h>

#define MAX_ERROR_LEN 256

#ifdef ENABLE_SYS_NERR

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

#else

// https://sourceware.org/glibc/wiki/Release/2.32#Deprectation_sys_errlist.2C__sys_errlist.2C_sys_nerr.2C_and__sys_nerr
char *strerror(int err)
{
  const char *r = strerrordesc_np(err);
  return (char *)(NULL == r ? "Unknown error" : r);
}

#endif
