#include <stdlib.h>
#include <stdint.h>

#include "libmcu.h"
#include "arch/i8051.h"
#include "arch/avr8.h"

/* ---------------------------------------------------------------- */
#define LIBMCU_NUM_ARCHS 2

const libmcu_arch_t* g_libmcu_archs[LIBMCU_NUM_ARCHS] =
{
  &g_libmcu_internal_i8051_arch,
  &g_libmcu_internal_avr8_arch,
};

/* ---------------------------------------------------------------- */
libmcu_ctx_t*
 libmcu_arch_create (uint8_t arch)
{
  libmcu_ctx_t* ctx = NULL;

  ctx = calloc (1, sizeof(libmcu_ctx_t));
  if (ctx == NULL) return NULL;

  switch (arch)
  {
    case LIBMCU_ARCH_I8051:
      ctx->arch = g_libmcu_archs[LIBMCU_ARCH_I8051];
      break;

    case LIBMCU_ARCH_AVR8:
      ctx->arch = g_libmcu_archs[LIBMCU_ARCH_AVR8];
      break;

    default:
      free (ctx);
      return NULL;
  }

  return ctx;
}

/* ---------------------------------------------------------------- */
uint8_t
 libmcu_arch_get_num_archs (void)
{
  return LIBMCU_NUM_ARCHS;
}

/* ---------------------------------------------------------------- */
libmcu_arch_t*
 libmcu_arch_get (uint8_t num)
{
  if (num < LIBMCU_NUM_ARCHS)
    return g_libmcu_archs[num];
  return NULL;
}

/* ---------------------------------------------------------------- */
void
 libmcu_arch_set_predisasm (libmcu_ctx_t* ctx,
     uint32_t (*predisasm)(libmcu_dasm_t* dasm,uint8_t* code,
       uint32_t size, void* data))
{
  if (ctx == NULL) return;
  if (predisasm == NULL) return;
  if (ctx->arch == NULL) return;

  ctx->arch->predisasm = predisasm;
}

/* ---------------------------------------------------------------- */
void
 libmcu_arch_set_postdisasm (libmcu_ctx_t* ctx,
     uint32_t (*postdisasm)(libmcu_dasm_t* dasm,uint8_t* code,
       uint32_t size, void* data))
{
  if (ctx == NULL) return;
  if (postdisasm == NULL) return;
  if (ctx->arch == NULL) return;

  ctx->arch->postdisasm = postdisasm;
}

/* ---------------------------------------------------------------- */
uint32_t
 libmcu_arch_disasm (libmcu_ctx_t* ctx, libmcu_dasm_t* dasm,
     uint8_t* code, uint32_t size, uint64_t addr)
{
  uint32_t ret = 0;

  if (dasm == NULL) return 0;
  if (code == NULL) return 0;
  if (size == 0) return 0;
  if (ctx == NULL) return 0;
  if (ctx->arch == NULL) return 0;

  if (ctx->arch->predisasm != NULL)
    ctx->arch->predisasm (dasm, code,
        size, NULL);

  if (ctx->arch->disasm == NULL)
    return 0;

  ret = ctx->arch->disasm (dasm, code, size, addr);

  if (ctx->arch->postdisasm != NULL)
    ctx->arch->postdisasm (dasm, code, size, NULL);

  return ret;
}

/* ---------------------------------------------------------------- */
uint32_t
 libmcu_arch_disasm_ex (libmcu_ctx_t* ctx, libmcu_dasm_t* dasm,
     uint8_t* code, uint32_t size, uint64_t addr, void* data)
{
  uint32_t ret = 0;

  if (dasm == NULL) return 0;
  if (code == NULL) return 0;
  if (size == 0) return 0;
  if (ctx == NULL) return 0;
  if (ctx->arch == NULL) return 0;

  if (ctx->arch->predisasm != NULL)
    ctx->arch->predisasm (dasm, code, size, data);

  if (ctx->arch->disasm == NULL)
    return 0;

  ret = ctx->arch->disasm (dasm, code, size, addr);

  if (ctx->arch->postdisasm != NULL)
    ctx->arch->postdisasm (dasm, code, size, data);

  return ret;
}

/* ---------------------------------------------------------------- */
void 
 libmcu_arch_destroy_safe (libmcu_ctx_t** ctx)
{
  if (ctx == NULL) return;
  if (*ctx == NULL) return;

  free (*ctx);
  *ctx = NULL;
}



