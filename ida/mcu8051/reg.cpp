
#include <ida.hpp>
#include <idp.hpp>
#include <stdint.h>

#include <libconfig.h>
#include <libmcu_arch.h>
#include <libmcu_mmap.h>
#include <arch/i8051.h>

#include "reg.hpp"

const char *const mcu8051_regname[] = 
{
  REG(I8051_REG_R0),
  REG(I8051_REG_R1),
  REG(I8051_REG_R2),
  REG(I8051_REG_R3),
  REG(I8051_REG_R4),
  REG(I8051_REG_R5),
  REG(I8051_REG_R6),
  REG(I8051_REG_R7),
  REG(I8051_REG_A),
  REG(I8051_REG_DPTR),
  REG(I8051_REG_PC),
  REG(I8051_REG_C),
  REG(I8051_REG_AB),
  "cs",
  "ds"
};

