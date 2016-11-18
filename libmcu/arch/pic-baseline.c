#include <stdlib.h>
#include <stdint.h>
#include "libmcu_arch.h"

/*

PIC architecture is pointless here since you have
direct/indirect access to memory though special instruction
and internal register. So we need to make at least a static
analysis for playing with memory map configuration.

*/

/* 
char* pic12_mnemonics[] = 
{
  "nop",    "option", "sleep", "clrwdt", "tris", "movlb",
  "return", "retfie", "movwf", "clr",    "subwf", "decf",
  "iorwf",  "andwf",  "xorwf", "addwf",  "movf", "comf",
  "incf",   "decfsz", "rrf",   "rlf",    "swapf", "incfsz",
  "bcf",    "bsf",    "btfsc", "btfss",  "retlw", "call",
  "goto",   "movlw",  "iorlw", "xorlw"
};

#define PIC12_INSN_NOP      0
#define PIC12_INSN_OPTION   1
#define PIC12_INSN_SLEEP    2
#define PIC12_INSNS_CLRWDT  3
#define PIC12_INSN_TRIS     4
#define PIC12_INSN_MOVLB    5
#define PIC12_INSN_RETURN   6
#define PIC12_INSN_RETFIE   7
#define PIC12_INSN_MOVWF    8

static void
 pic12_setup_insn (mcu_dasm_t* dasm, uint8_t* code, uint32_t size)
{
  uint16_t opcode = 0;

  if (dasm == NULL) return;
  if (code == NULL) return;
  if (size < 2) return;

  opcode = code[0] | ((uint16_t)code[1] << 8);

  switch (opcode & 0xFFF)
  {
    case 0x00:
      dasm->mnemonic = PIC12_INSN_NOP;
      break;

    case 0x02:
      dasm->mnnemonic = PIC12_INSN_OPTION;
      break;

    case 0x03:
      dasm->mnemonic = PIC12_INSN_SLEEP;
      break;

    case 0x04:
      dasm->mnemonic = PIC12_INSN_CLRWDT;
      break;

    case 0x0D:
      dasm->mnemonic = PIC12_INSN_RETURN;
      break;

    case 0x0F:
      dasm->mnemonic = PIC12_INSN_RETFIE;
      break;
  }

  switch ((opcode & 0xFD0)>>4)
  {
    case 0x01:
      break;
  }
}

uint32_t 
 pic12_disasm (mcu_dasm_t* dasm, uint8_t* code, uint32_t size,
     uint64_t addr)
{
  return 2;
}

mcu_arch_t pic12_arch =
{
  .name            = "Microchip PIC Baseline"
                     " (12-bit instruction set)",
  .shortname       = "pic12",
  .author          = "stfsux",
  .licname         = "WTFPL",
  .licfull         = "bitebitebitbiebtiei",

  .mnemonics       = pic12_mnemonics,
  .ninsn           = 0x00,

  .regname         = NULL,
  .nregs           = 0x00,

  .bus_type        = MCU_ARCH_BUS_HARVARD_MOD,
  .bus_size        = MCU_ARCH_BUS_SIZE_16BIT,
  .predisasm       = NULL,
  .disasm          = NULL,
  .postdisasm      = NULL,

  .asm_fmt         = NULL, 
  .num_asm_fmts    = 0,

  .opcode_max_size = 256,
};

*/

