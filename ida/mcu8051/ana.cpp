
#include <ida.hpp>
#include <idp.hpp>
#include <stdint.h>

#include <libmcu.h>
#include <arch/i8051.h>

#include "ins.hpp"
#include "mcu8051.hpp"

#define MAXOPCODE __g_libmcu_internal_i8051_arch.opcode_max_size

int idaapi
 mcu8051_ana (void)
{
  uint8_t* code_buffer = NULL;
  uint32_t n = 0;
  uint32_t size = 0;
  libmcu_dasm_t dasm;
  ea_t current_ea = cmd.ea;

  code_buffer = (uint8_t*)calloc (MAXOPCODE, sizeof(uint8_t));
  if (code_buffer == NULL) return 0;

  for (n = 0; n < MAXOPCODE; n++)
    code_buffer[n] = get_byte (current_ea + n);

  memset (&dasm, 0, sizeof(dasm));

  size = libmcu_arch_disasm (g_libmcu_ctx, &dasm, code_buffer, 
      MAXOPCODE, cmd.ea);

  cmd.itype = dasm.mnemonic;

  for (n = 0; n < dasm.nopd; n++)
  {
    switch (dasm.opd[n].type)
    {
      case MCU_OPD_TYPE_REGISTER:
        if (dasm.opd[n].flags & MCU_OPD_FLAGS_INDIRECT)
        {
          cmd.Operands[n].type = o_phrase;
          cmd.Operands[n].reg = dasm.opd[n].value;
        }
        else
        {
          cmd.Operands[n].type = o_reg;
          cmd.Operands[n].reg = dasm.opd[n].value;
        }
        break;

      case MCU_OPD_TYPE_IMM:
        if (dasm.opd[n].flags == 0)
        {
          cmd.Operands[n].type = o_imm;
          cmd.Operands[n].value = dasm.opd[n].value;
        }
        if (dasm.opd[n].flags & MCU_OPD_FLAGS_DIRECT)
        {
          cmd.Operands[n].type = o_mem;
          cmd.Operands[n].addr = dasm.opd[n].value;
        }
        break;

      case MCU_OPD_TYPE_DISPL:
        cmd.Operands[n].type = o_displ;
        cmd.Operands[n].reg = dasm.opd[n].displ.base;
        cmd.Operands[n].value = dasm.opd[n].displ.offset;
        break;
    }
    switch (dasm.opd[n].size)
    {
      case MCU_OPD_SIZE_BYTE:
        cmd.Operands[n].dtyp = dt_byte;
        break;

      case MCU_OPD_SIZE_WORD:
        cmd.Operands[n].dtyp = dt_word;
        break;
    }
    cmd.Operands[n].specflag1 = dasm.opd[n].bus;
  }

  free (code_buffer);

  cmd.size = size;
  return cmd.size;
}

