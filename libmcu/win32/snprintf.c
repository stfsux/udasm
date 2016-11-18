#include <stdio.h>
#include <stdarg.h>
#include "snprintf.h"

#if defined(WORST_FKING_COMPILER_EVER)

/* --------------------------------------------------------------- */
int vsnprintf (char* buffer, size_t size, const char*
  format, va_list ap)
{
  int count = -1;

  if (size != 0)
    count = _vsnprintf_s (buffer, size, _TRUNCATE, format, ap);
  if (count == -1)
    count = _vscprintf (format, ap);

  return count;
}

/* --------------------------------------------------------------- */
int snprintf (char* buffer, size_t size, const char*
    format, ...)
{
  int count;
  va_list ap;

  va_start (ap, format);
  count = vsnprintf (buffer, size, format, ap);
  va_end (ap);

  return count;
}


#endif

