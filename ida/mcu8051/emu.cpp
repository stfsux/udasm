
#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <stdint.h>

#include <libconfig.h>
#include <libmcu_arch.h>
#include <libmcu_mmap.h>
#include <arch/i8051.h>

#define MCU8051_NCOND_INSN 9
static uint32_t mcu8051_cond_jump[MCU8051_NCOND_INSN] =
{
  I8051_INSN_CJNE,
  I8051_INSN_DJNZ,
  I8051_INSN_JB,
  I8051_INSN_JBC,
  I8051_INSN_JC,
  I8051_INSN_JNB,
  I8051_INSN_JNC,
  I8051_INSN_JNZ,
  I8051_INSN_JZ
};

static uint8_t
 mcu8051_is_cond_jump (int itype)
{
  uint32_t n = 0;
  for (n = 0; n < MCU8051_NCOND_INSN; n++)
  {
    if (itype == mcu8051_cond_jump[n])
      return 1;
  }
  return 0;
}

int idaapi
 mcu8051_emu (void)
{
  ulong cflags = 0;
  uint32_t n = 0;

  cflags = cmd.get_canon_feature ();

  if (mcu8051_is_cond_jump (cmd.itype))
  {
    for (n = 0; n < UA_MAXOP; n++)
    {
      if (cmd.Operands[n].type == o_mem &&
          cmd.Operands[n].specflag1 == MCU_OPD_BUS_CODE)
        ua_add_cref (0, cmd.Operands[n].addr, fl_JF);
    }
  }

  if (cflags&CF_STOP)
  {
    for (n = 0; n < UA_MAXOP; n++)
    {
      if (cmd.Operands[n].type == o_mem &&
          cmd.Operands[n].specflag1 == MCU_OPD_BUS_CODE)
        ua_add_cref (0, cmd.Operands[n].addr, fl_JF);
    }
  }
  else if (cflags&CF_CALL)
  {
    ua_add_cref (0, cmd.Op1.addr, fl_CF);
    ua_add_cref (0, cmd.ea + cmd.size, fl_F);
  }
  else
    ua_add_cref (0, cmd.ea + cmd.size, fl_F);

  return cmd.size;
}

