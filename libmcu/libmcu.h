
#ifndef __LIBMCU_H__
#define __LIBMCU_H__

#ifdef __cplusplus
#define LIBMCU_CDECL extern "C"
#else
#define LIBMCU_CDECL extern
#endif

#include <stdint.h>
#include <libconfig.h>
#include <libmcu_arch.h>
#include <libmcu_str.h>
#include <libmcu_mmap.h>
#include <libmcu_asmprint.h>

#endif

