
#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <stdint.h>

#include <libmcu.h>
#include <arch/i8051.h>

#include "out.hpp"
#include "mcu8051.hpp"

bool idaapi
 mcu8051_outop (op_t &x)
{
  uint32_t rid = 0;
  switch (x.type)
  {
    case o_reg:
      if (x.reg < (ph.regsNum-2))
        out_register (ph.regNames[x.reg]);
      break;

    case o_phrase:
      out_symbol ('@');
      if (x.reg < (ph.regsNum-2))
        out_register (ph.regNames[x.reg]);
      break;

    case o_imm:
      out_symbol ('#');
      OutValue (x, OOFS_NOSIGN);
      break;

    case o_mem:
      switch (x.specflag1)
      {
        case MCU_OPD_BUS_CODE:
          out_name_expr (x, x.addr, x.addr);
          break;
          
        case MCU_OPD_BUS_IDATA:
          rid = libmcu_mmap_idata_reg (g_libmcu_mmap, x.addr);
          if (rid != LIBMCU_MMAP_UNMAPPED)
            OutLine (libmcu_mmap_idata_reg_name (g_libmcu_mmap, rid));
          else
            OutValue (x, OOF_ADDR);
          break;

        case MCU_OPD_BUS_IDATA_BITS:
          rid = libmcu_mmap_idata_reg_bit (g_libmcu_mmap, x.addr);
          if (rid != LIBMCU_MMAP_UNMAPPED)
            OutLine (libmcu_mmap_idata_reg_bit_name (g_libmcu_mmap, rid));
          else
            OutValue (x, OOF_ADDR);

          break;
      }
      break;

    case o_displ:
      out_symbol ('@');
      if (x.reg < ((ph.regsNum-2)))
        out_register (ph.regNames[x.reg]);
      out_symbol ('+');
      if (x.value < ((ph.regsNum-2)))
        out_register (ph.regNames[x.value]);
      break;
  }
  return 1;
}

void idaapi
 mcu8051_out (void)
{
  char str[MAXSTR];
  uint32_t n = 0;

  init_output_buffer (str, sizeof(str));

  OutMnem (5);

  for (n = 0; n < UA_MAXOP; n++)
  {
    if (cmd.Operands[n].type != o_void)
    {
      if (n != 0)
      {
        out_symbol (',');
        out_symbol (' ');
      }
      out_one_operand (n);
    }
  }

  term_output_buffer ();
  MakeLine (str);
}


