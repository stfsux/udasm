
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

/* Ignore escape sequences. */
uint32_t
 libmcu_str_length (char* str)
{
  char* ptr = str;
  uint32_t sz = 0;
  uint8_t csi_seq = 0;

  if (str == NULL) return 0;

  while (*ptr)
  {
    switch (*ptr)
    {
      case 'm':
        if (csi_seq == 1) csi_seq = 0;
        else sz = sz + 1;
        break;

      case 0x1B:
        csi_seq = 1;
        break;

      default:
        if (csi_seq == 0) sz = sz + 1;
        break;
    }
    ptr++;
  }
  return sz;
}


