#ifndef __SNPRINTF__H__
#define __SNPRINTF__H__

#if defined(_MSC_VER)
#if _MSC_VER < 1900
#define WORST_FKING_COMPILER_EVER
#endif
#endif

#if defined(WORST_FKING_COMPILER_EVER)
/* --------------------------------------------------------------- */
int vsnprintf (char* buffer, size_t size, const char*
  format, va_list ap);
int snprintf (char* buffer, size_t size, const char*
    format, ...);
#endif


#endif

