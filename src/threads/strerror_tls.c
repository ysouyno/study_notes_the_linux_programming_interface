#define _GNU_SOURCE // Get `_sys_nerr` and `_sys_errlist`
                    // declarations from <stdio.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MAX_ERROR_LEN 256

#ifdef ENABLE_SYS_NERR

static __thread char buf[MAX_ERROR_LEN]; // Thread-local return buffer

char *strerror(int err)
{
  if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) {
    snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
  }
  else {
    strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
    buf[MAX_ERROR_LEN - 1] = '\0';
  }

  return buf;
}

#else

// https://sourceware.org/glibc/wiki/Release/2.32#Deprectation_sys_errlist.2C__sys_errlist.2C_sys_nerr.2C_and__sys_nerr
char *strerror(int err)
{
  const char *r = strerrordesc_np(err);
  return (char *)(NULL == r ? "Unknown error" : r);
}

#endif
