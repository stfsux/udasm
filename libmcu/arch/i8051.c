#include <stdlib.h>
#include <stdint.h>

#include "libmcu.h"
#include "arch/i8051.h"

/* ---------------------------------------------------------------- */
char* i8051_mnemonics[] = 
{
"acall",  "add",  "addc",  "ajmp",   "anl",  "cjne",   "clr",   "cpl",
   "da",  "dec",   "div",  "djnz",   "inc",    "jb",   "jbc",    "jc",
  "jmp",  "jnb",   "jnc",   "jnz",    "jz", "lcall",  "ljmp",   "mov",
 "movc", "movx",   "mul",   "nop",   "orl",   "pop",  "push",   "ret",
 "reti",   "rl",   "rlc",    "rr",   "rrc",  "setb",  "sjmp",  "subb",
 "swap",  "xch",  "xchd",   "xrl",   "???"
};


/* ---------------------------------------------------------------- */
/* These are registers coded in instruction's opcode.               */
/* All of them and more (e.g. DPL, DPH, etc.) are mapped in DATA    */
/* address space. We don't give a fuck here.                        */
char* i8051_regname[] = 
{
  "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "A", "DPTR",
  "PC", "C",  "AB"
};

/* ---------------------------------------------------------------- */
static uint32_t i8051_insn_id[] = 
{
  27,  3, 22, 35, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
  13,  0, 21, 36,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 
  13,  3, 31, 33,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
  17,  0, 32, 34,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
  15,  3, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
  18,  0,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4, 4 ,  4,  4,  
  20,  3, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
  19,  0, 28, 16, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  38,  3,  4, 24, 10, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  23,  0, 23, 24, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,
  28,  3, 23, 12, 26, 44, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
   4,  0,  7,  7,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
  30,  3,  6,  6, 40, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
  29,  0, 37, 37,  8, 11, 42, 42, 11, 11, 11, 11, 11, 11, 11, 11,
  25,  3, 25, 25,  6, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
  25,  0, 25, 25,  7, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
};

/* ---------------------------------------------------------------- */
static uint32_t i8051_insn_size[] =
{
  1, 2, 3, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  3, 2, 3, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  3, 2, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  3, 2, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 1, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 1, 1, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 2, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 1, 1, 3, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

/* ---------------------------------------------------------------- */
static uint8_t i8051_insn_nopds[] =
{
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 1, 2, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 1, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 1, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
};

/* ---------------------------------------------------------------- */
static void
 i8051_set_operands (libmcu_dasm_t* dasm, uint8_t* code)
{
  if (dasm == NULL) return;
  if (code == NULL) return;

  switch (code[0])
  {
      /* nop */
    case 0x00:
      break;

      /* ajmp/acall addr11 */
    case 0x01:
    case 0x11:
    case 0x21:
    case 0x31:
    case 0x41:
    case 0x51:
    case 0x61:
    case 0x71:
    case 0x81:
    case 0x91:
    case 0xA1:
    case 0xB1:
    case 0xC1:
    case 0xD1:
    case 0xE1:
    case 0xF1:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].value = (uint16_t)code[1] |
        ((uint16_t)(code[0]&0xE0)<<3);
      dasm->opd[0].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0x02:
    case 0x12:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].value = (uint16_t)code[2] |
        (((uint16_t)code[1])<<8);
      dasm->opd[0].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0x03:
    case 0x04:
    case 0x13:
    case 0x14:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      break;

    case 0x05:
    case 0x15:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      break;

    case 0x06:
    case 0x16:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[0].value = I8051_REG_R0;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      break;

    case 0x07:
    case 0x17:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[0].value = I8051_REG_R1;
      break;

    case 0x08:
    case 0x18:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R0;
      break;

    case 0x09:
    case 0x19:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R1;
      break;

    case 0x0A:
    case 0x1A:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R2;
      break;

    case 0x0B:
    case 0x1B:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R3;
      break;

    case 0x0C:
    case 0x1C:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R4;
      break;

    case 0x0D:
    case 0x1D:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R5;
      break;

    case 0x0E:
    case 0x1E:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R6;
      break;

    case 0x0F:
    case 0x1F:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R7;
      break;

      /* Bit test with conditional jump */
      /* insn bit, offset */
    case 0x10:
    case 0x20:
    case 0x30:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA_BITS;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].value = (int8_t)code[2] + 3 + dasm->addr;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_CODE;
      break;

      /* ret */
    case 0x22:
      break;

    case 0x23:
    case 0x33:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      break;

    case 0x24:
    case 0x34:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      break;

    case 0x25:
    case 0x35:
    case 0x45:
    case 0x55:
    case 0x65:
    case 0x95:
    case 0xC5:
    case 0xE5:
    case 0xF5:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[1].value = code[1];
      break;

    case 0x26:
    case 0x36:
    case 0x46:
    case 0x56:
    case 0x66:
    case 0xC6:
    case 0xD6:
    case 0xE6:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_R0;
      break;
      
    case 0x27:
    case 0x37:
    case 0x47:
    case 0x57:
    case 0x67:
    case 0xC7:
    case 0xD7:
    case 0xE7:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_R1;
      break;

      
    case 0x28:
    case 0x38:
    case 0x48:
    case 0x58:
    case 0x68:
    case 0xC8:
    case 0xE8:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R0;
      break;
      
    case 0x29:
    case 0x39:
    case 0x49:
    case 0x59:
    case 0x69:
    case 0xC9:
    case 0xE9:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R1;
      break;
      
    case 0x2A:
    case 0x3A:
    case 0x4A:
    case 0x5A:
    case 0x6A:
    case 0xCA:
    case 0xEA:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_R2;
      break;
      
    case 0x2B:
    case 0x3B:
    case 0x4B:
    case 0x5B:
    case 0x6B:
    case 0xCB:
    case 0xEB:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_R3;
      break;
      
    case 0x2C:
    case 0x3C:
    case 0x4C:
    case 0x5C:
    case 0x6C:
    case 0xCC:
    case 0xEC:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_R4;
      break;
      
    case 0x2D:
    case 0x3D:
    case 0x4D:
    case 0x5D:
    case 0x6D:
    case 0xCD:
    case 0xED:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_R5;
      break;
      
    case 0x2E:
    case 0x3E:
    case 0x4E:
    case 0x5E:
    case 0x6E:
    case 0xCE:
    case 0xEE:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_R6;
      break;
      
    case 0x2F:
    case 0x3F:
    case 0x4F:
    case 0x5F:
    case 0x6F:
    case 0xCF:
    case 0xEF:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_R7;
      break;

    case 0x42:
    case 0x52:
    case 0x62:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_A;
      break;

    case 0x43:
    case 0x53:
    case 0x63:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[2];
      break;

    case 0x44:
    case 0x54:
    case 0x64:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      break;

      /* conditional/unconditioanl jump. */
      /* insn offset */
    case 0x40:
    case 0x50:
    case 0x60:
    case 0x70:
    case 0x80:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].value = (int8_t)code[1] + 2 + dasm->addr;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0x72:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_C;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA_BITS;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].value = code[1];
      break;

    case 0x73:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_DISPL;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[0].displ.base_type = LIBMCU_DISPL_TYPE_REG;
      dasm->opd[0].displ.base = I8051_REG_DPTR;
      dasm->opd[0].displ.offset_type = LIBMCU_DISPL_TYPE_REG;
      dasm->opd[0].displ.offset = I8051_REG_A;
      break;

    case 0x74:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      break;

    case 0x75:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[2];
      break;

    case 0x76:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[0].value = I8051_REG_R0;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      break;

    case 0x77:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[0].value = I8051_REG_R1;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      break;

    case 0x78:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R0;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      break;

    case 0x79:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R1;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      break;

    case 0x7A:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R2;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      break;

    case 0x7B:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R3;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      break;

    case 0x7C:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R4;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      break;

    case 0x7D:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R5;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      break;

    case 0x7E:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R6;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      break;

    case 0x7F:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R7;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      break;

    case 0x82:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_C;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA_BITS;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].value = code[1];
      break;

    case 0x83:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_DISPL;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].displ.base_type = LIBMCU_DISPL_TYPE_REG;
      dasm->opd[1].displ.base = I8051_REG_PC;
      dasm->opd[1].displ.offset_type = LIBMCU_DISPL_TYPE_REG;
      dasm->opd[1].displ.offset = I8051_REG_A;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0x84:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].value = I8051_REG_AB;
      break;

    case 0x85:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[2];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[1].value = code[1];
      break;

    case 0x86:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_R0;
      break;

    case 0x87:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_R1;
      break;

    case 0x88:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R0;
      break;

    case 0x89:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R1;
      break;

    case 0x8A:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R2;
      break;

    case 0x8B:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R3;
      break;

    case 0x8C:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R4;
      break;

    case 0x8D:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R5;
      break;
    
    case 0x8E:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R6;
      break;

    case 0x8F:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R7;
      break;

    case 0x90:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].value = I8051_REG_DPTR;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].value = ((uint16_t)code[1]<<8) | code[2];
      break;

    case 0x92:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA_BITS;
      dasm->opd[1].value = I8051_REG_C;
      break;

    case 0x93:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_DISPL;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].displ.base_type = LIBMCU_DISPL_TYPE_REG;
      dasm->opd[1].displ.offset_type = LIBMCU_DISPL_TYPE_REG;
      dasm->opd[1].displ.base = I8051_REG_A;
      dasm->opd[1].displ.offset = I8051_REG_DPTR;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0x94:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      break;

    case 0x96:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_R0;
      break;

    case 0x97:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_R1;
      break;

    case 0x98:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R0;
      break;

    case 0x99:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R1;
      break;

    case 0x9A:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R2;
      break;

    case 0x9B:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R3;
      break;

    case 0x9C:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R4;
      break;

    case 0x9D:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R5;
      break;

    case 0x9E:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R6;
      break;

    case 0x9F:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_R7;
      break;

    case 0xA0:
    case 0xA2:
    case 0xB0:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].value = I8051_REG_C;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA_BITS;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].value = code[1];
      break;

    case 0xA3:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].value = I8051_REG_DPTR;
      break;

    case 0xA4:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].value = I8051_REG_AB;
      break;

    case 0xA6:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[0].value = I8051_REG_R0;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[1].value = code[1];
      break;

    case 0xA7:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[0].value = I8051_REG_R1;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[1].value = code[1];
      break;

    case 0xA8:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R0;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[1].value = code[1];
      break;

    case 0xA9:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R1;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[1].value = code[1];
      break;

    case 0xAA:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R2;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[1].value = code[1];
      break;

    case 0xAB:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R3;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[1].value = code[1];
      break;

    case 0xAC:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R4;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[1].value = code[1];
      break;

    case 0xAD:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R5;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[1].value = code[1];
      break;

    case 0xAE:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R6;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[1].value = code[1];
      break;

    case 0xAF:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R7;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[1].value = code[1];
      break;

    case 0xB2:
    case 0xC2:
    case 0xD2:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA_BITS;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].value = code[1];
      break;

    case 0xB3:
    case 0xC3:
    case 0xD3:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA_BITS;
      dasm->opd[0].value = I8051_REG_C;
      break;

    case 0xB4:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      dasm->opd[2].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[2].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[2].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[2].value = (int8_t)code[2] + 3 + dasm->addr;
      dasm->opd[2].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xB5:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      dasm->opd[2].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[2].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[2].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[2].value = (int8_t)code[2] + 3 + dasm->addr;
      dasm->opd[2].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xB6:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[0].value = I8051_REG_R0;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      dasm->opd[2].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[2].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[2].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[2].value = (int8_t)code[2] + 3 + dasm->addr;
      dasm->opd[2].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xB7:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[0].value = I8051_REG_R1;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      dasm->opd[2].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[2].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[2].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[2].value = (int8_t)code[2] + 3 + dasm->addr;
      dasm->opd[2].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xB8:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R0;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      dasm->opd[2].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[2].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[2].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[2].value = (int8_t)code[2] + 3 + dasm->addr;
      dasm->opd[2].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xB9:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R1;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      dasm->opd[2].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[2].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[2].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[2].value = (int8_t)code[2] + 3 + dasm->addr;
      dasm->opd[2].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xBA:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R2;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      dasm->opd[2].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[2].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[2].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[2].value = (int8_t)code[2] + 3 + dasm->addr;
      dasm->opd[2].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xBB:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R3;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      dasm->opd[2].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[2].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[2].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[2].value = (int8_t)code[2] + 3 + dasm->addr;
      dasm->opd[2].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xBC:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R4;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      dasm->opd[2].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[2].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[2].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[2].value = (int8_t)code[2] + 3 + dasm->addr;
      dasm->opd[2].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xBD:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R5;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      dasm->opd[2].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[2].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[2].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[2].value = (int8_t)code[2] + 3 + dasm->addr;
      dasm->opd[2].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xBE:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R6;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      dasm->opd[2].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[2].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[2].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[2].value = (int8_t)code[2] + 3 + dasm->addr;
      dasm->opd[2].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xBF:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R7;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = code[1];
      dasm->opd[2].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[2].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[2].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[2].value = (int8_t)code[2] + 3 + dasm->addr;
      dasm->opd[2].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xC0:
    case 0xD0:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      break;

    case 0xC4:
    case 0xD4:
    case 0xE4:
    case 0xF4:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      break;

    case 0xD5:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[0].bus = LIBMCU_OPD_BUS_IDATA;
      dasm->opd[0].value = code[1];
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].value = (int8_t)code[1] + 2 + dasm->addr;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xD8:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R0;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].value = (int8_t)code[1] + 2 + dasm->addr;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xD9:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R1;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].value = (int8_t)code[1] + 2 + dasm->addr;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xDA:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R2;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].value = (int8_t)code[1] + 2 + dasm->addr;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xDB:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R3;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].value = (int8_t)code[1] + 2 + dasm->addr;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_CODE;
      break;
      
    case 0xDC:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R4;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].value = (int8_t)code[1] + 2 + dasm->addr;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xDD:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R5;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].value = (int8_t)code[1] + 2 + dasm->addr;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xDE:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R6;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].value = (int8_t)code[1] + 2 + dasm->addr;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xDF:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R7;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_IMM;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_DIRECT;
      dasm->opd[1].value = (int8_t)code[1] + 2 + dasm->addr;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_CODE;
      break;

    case 0xE0:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_DPTR;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_XDATA;
      break;

    case 0xE2:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_R0;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_XDATA;
      break;

    case 0xE3:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_A;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[1].value = I8051_REG_R1;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_XDATA;
      break;

    case 0xF0:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[0].value = I8051_REG_DPTR;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_A;
      break;

    case 0xF2:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[0].value = I8051_REG_R0;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_A;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_XDATA;
      break;

    case 0xF3:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[0].value = I8051_REG_R1;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_A;
      dasm->opd[1].bus = LIBMCU_OPD_BUS_XDATA;
      break;

    case 0xF6:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[0].value = I8051_REG_R0;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_A;
      break;

    case 0xF7:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_INDIRECT;
      dasm->opd[0].value = I8051_REG_R1;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_A;
      break;

    case 0xF8:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R0;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_A;
      break;

    case 0xF9:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R1;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_A;
      break;

    case 0xFA:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R2;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_A;
      break;

    case 0xFB:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R3;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_A;
      break;

    case 0xFC:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R4;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_A;
      break;

    case 0xFD:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R5;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_A;
      break;

    case 0xFE:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R6;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_A;
      break;
      
    case 0xFF:
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = I8051_REG_R7;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = I8051_REG_A;
      break;

  }
}

/* ---------------------------------------------------------------- */
uint32_t
 i8051_disasm (libmcu_dasm_t* dasm, uint8_t* code, uint32_t size,
     uint64_t addr)
{
  uint8_t opcode = 0;
  uint32_t insn_size = 0;

  if (dasm == NULL) return 0;
  if (code == NULL) return 0;
  if (size == 0)    return 0;

  opcode = code[0];

  dasm->mnemonic = i8051_insn_id[opcode];
  insn_size = i8051_insn_size[opcode];
  dasm->size = insn_size;
  dasm->code = code;
  dasm->addr = addr;

  if (insn_size > size) return 0;

  dasm->nopd = i8051_insn_nopds[opcode];

  i8051_set_operands (dasm, code);

  return insn_size;
}

/* ---------------------------------------------------------------- */
/* Thanksfully visual studio's compiler of shit the C99 structure   */
/* declaration with a dot is NOT FKING SUPPORTED.                   */
/* Just go fking die microsoft. Die you piece of shit cumbag mofo.  */
libmcu_asm_fmt_t i8051_default_fmt = 
{
/*  .fmt_name          =*/ "A51 assembler format",
/*  .fmt_shortname     =*/ "a51",
/*  .opcode_padding    =*/ 3,
/*  .comm_padding      =*/ 40,
/*  .dst_first         =*/ LIBMCU_ASM_DST_FIRST,
/*  .def_fmt           =*/ LIBMCU_ASM_DEF_FORMAT_HEX,
/*  .insn_prefix       =*/ NULL,
/*  .insn_suffix       =*/ NULL,
/*  .register_prefix   =*/ NULL,
/*  .register_suffix   =*/ NULL,
/*  .indirect_prefix   =*/ "@",
/*  .indirect_suffix   =*/ NULL,
/*  .displ_prefix      =*/ NULL, 
/*  .displ_suffix      =*/ NULL,
/*  .immediat_prefix   =*/ "#",
/*  .immediat_suffix   =*/ NULL,
/*  .direct_prefix     =*/ NULL,
/*  .direct_suffix     =*/ NULL,
/*  .indexed_prefix    =*/ NULL,
/*  .indexed_suffix    =*/ NULL,
/*  .hex_prefix        =*/ "0x",
/*  .hex_suffix        =*/ NULL,
/*  .bin_prefix        =*/ NULL,
/*  .bin_suffix        =*/ "b",
/*  .dec_prefix        =*/ NULL,
/*  .dec_suffix        =*/ "d",
/*  .oct_prefix        =*/ NULL,
/*  .oct_suffix        =*/ "q",
/*  .displ_add         =*/ "+",
/*  .displ_sub         =*/ NULL,
/*  .displ_mul         =*/ NULL,
/*  .size_byte_prefix  =*/ NULL,
/*  .size_byte_suffix  =*/ NULL,
/*  .size_word_prefix  =*/ NULL,
/*  .size_word_suffix  =*/ NULL,
/*  .size_dword_prefix =*/ NULL,
/*  .size_dword_suffix =*/ NULL,
/*  .size_qword_prefix =*/ NULL,
/*  .size_qword_suffix =*/ NULL,
/*  .data_byte         =*/ "BYTE",
/*  .data_word         =*/ "WORD",
/*  .data_dword        =*/ "DWORD",
/*  .data_qword        =*/ NULL,
/*  .comm_prefix       =*/ "; ",
/*  .comm_suffix       =*/ NULL,
/*  .multi_reg         =*/ NULL,
};

/* ---------------------------------------------------------------- */
libmcu_asm_fmt_t i8051_fmt_colors = 
{
/*  .fmt_name          =*/ "A51 assembler format with colors",
/*  .fmt_shortname     =*/ "a51_colors",
/*  .opcode_padding    =*/ 3,
/*  .comm_padding      =*/ 40,
/*  .dst_first         =*/ LIBMCU_ASM_DST_FIRST,
/*  .def_fmt           =*/ LIBMCU_ASM_DEF_FORMAT_HEX,
/*  .insn_prefix       =*/ NULL,
/*  .insn_suffix       =*/ NULL,
/*  .register_prefix   =*/ "\x1b[0;31m",
/*  .register_suffix   =*/ "\x1b[0m",
/*  .indirect_prefix   =*/ "\x1b[0;33m@",
/*  .indirect_suffix   =*/ "\x1b[0m",
/*  .displ_prefix      =*/ NULL,
/*  .displ_suffix      =*/ NULL,
/*  .immediat_prefix   =*/ "\x1b[0;34m#",
/*  .immediat_suffix   =*/ "\x1b[0m",
/*  .direct_prefix     =*/ "\x1b[0;32m",
/*  .direct_suffix     =*/ "\x1b[0m",
/*  .indexed_prefix    =*/ NULL,
/*  .indexed_suffix    =*/ NULL,
/*  .hex_prefix        =*/ "0x",
/*  .hex_suffix        =*/ NULL,
/*  .bin_prefix        =*/ NULL,
/*  .bin_suffix        =*/ "b",
/*  .dec_prefix        =*/ NULL,
/*  .dec_suffix        =*/ "d",
/*  .oct_prefix        =*/ NULL,
/*  .oct_suffix        =*/ "q",
/*  .displ_add         =*/ "+",
/*  .displ_sub         =*/ NULL,
/*  .displ_mul         =*/ NULL,
/*  .size_byte_prefix  =*/ NULL,
/*  .size_byte_suffix  =*/ NULL,
/*  .size_word_prefix  =*/ NULL,
/*  .size_word_suffix  =*/ NULL,
/*  .size_dword_prefix =*/ NULL,
/*  .size_dword_suffix =*/ NULL,
/*  .size_qword_prefix =*/ NULL,
/*  .size_qword_suffix =*/ NULL,
/*  .data_byte         =*/ "BYTE",
/*  .data_word         =*/ "WORD",
/*  .data_dword        =*/ "DWORD",
/*  .data_qword        =*/ NULL,
/*  .comm_prefix       =*/ "; ",
/*  .comm_suffix       =*/ NULL,
/*  .multi_reg         =*/ NULL,
};

/* ---------------------------------------------------------------- */
libmcu_asm_fmt_t* i8051_asm_fmts[] = 
{
  &i8051_default_fmt,
  &i8051_fmt_colors,
  NULL
};

/* ---------------------------------------------------------------- */
const libmcu_arch_t g_libmcu_internal_i8051_arch =
{
/*  .name            =*/ "Intel 8051 generic",
/*  .shortname       =*/ "i8051",
/*  .author          =*/ "stfsux",
/*  .licname         =*/ "WTFPL",
/*  .licfull         =*/ "bitebitebitbiebtiei",
/*  .description     =*/ "Generic 8-bit Intel 8051 microcontroller "
                         "disassembler.",

/*  .mnemonics       =*/ i8051_mnemonics,
/*  .ninsn           =*/ 45,

/*  .regname         =*/ i8051_regname,
/*  .nregs           =*/ 13,

/*  .bus_type        =*/ LIBMCU_ARCH_BUS_HARVARD_MOD,
/*  .bus_size        =*/ LIBMCU_ARCH_BUS_SIZE_16BIT,
/*  .predisasm       =*/ NULL,
/*  .disasm          =*/ i8051_disasm,
/*  .postdisasm      =*/ NULL,

/*  .asm_fmt         =*/ i8051_asm_fmts, 
/*  .num_asm_fmts    =*/ 2,

/*  .opcode_max_size =*/ 256,
};



