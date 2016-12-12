#include <stdlib.h>
#include <stdint.h>

#include "libmcu.h"
#include "avr8.h"

/* ---------------------------------------------------------------- */
/* TODO: setup operand(s) bus type. */

/* ---------------------------------------------------------------- */
/* There are too many fking pseudo-instruction...                   */
/* Need to clean up dis shit.                                       */
/* cbr: same shit as andi rd, ~K                                    */
/* clr: same shit as eor rd, rd                                     */
/* bclr s: same shit as clc, clh, cli, cln, cls. etc.               */
/* bset s: same shit as set, sev, sez, sen, sei, etc.               */
/* lsl rd: same shit as add rd, rd                                  */
/* rol rd: same shit as adc rd, rd                                  */
/* sbr rd, k: same shit as ori rd, k                                */
/* ser rd: same shit as ldi rd, 0xFF                                */
/* tst rd: same shit as and rd, rd                                  */
/* brbc s,k: same shit as brne, etc.                                */
/* brbs s,k: same shit as breq, etc.                                */
char* avr8_mnemonics[] = 
{
   "add",    "adc",   "adiw",    "sub",   "subi",    "sbc",
  "sbci",   "sbiw",    "and",   "andi",     "or",    "ori",
   "eor",    "com",    "neg",    "sbr",    "cbr",    "inc",
   "dec",    "tst",    "clr",    "ser",    "mul",   "muls",
 "mulsu",   "fmul",  "fmuls", "fmulsu",    "des",   "rjmp",
  "ijmp",  "eijmp",    "jmp",  "rcall",  "icall", "eicall",
  "call",    "ret",   "reti",   "cpse",     "cp",    "cpc",
   "cpi",   "sbrc",   "sbrs",   "sbic",   "sbis",   "brbs",
  "brbc",   "breq",   "brne",   "brcs",   "brcc",   "brsh",
  "brlo",   "brmi",   "brpl",   "brge",   "brlt",   "brhs",
  "brhc",   "brts",   "brtc",   "brvs",   "brvc",   "brie",
  "brid",    "mov",   "movw",    "ldi",    "lds",    "ldd", 
   "sts",     "st",    "std",    "lpm",   "elpm",    "spm",
    "in",    "out",   "push",    "pop",    "xch",    "las", 
   "lac",    "lat",    "lsl",    "lsr",    "rol",    "ror", 
   "asr",   "swap",   "bset",   "bclr",    "sbi",    "cbi",
   "bst",    "bld",    "sec",    "clc",    "sen",    "cln",
   "sez",    "clz",    "sei",    "cli",    "ses",    "cls",
   "sev",    "clv",    "set",    "clt",    "seh",    "clh", 
 "break",    "nop",  "sleep",    "wdr",     "ld",    "???"
};

/* ---------------------------------------------------------------- */
char* avr8_regname[] = 
{
   "r0",  "r1",  "r2",  "r3",  "r4",  "r5",
   "r6",  "r7",  "r8",  "r9", "r10", "r11",
  "r12", "r13", "r14", "r15", "r16", "r17",
  "r18", "r19", "r20", "r21", "r22", "r23",
  "r24", "r25", "r26", "r27", "r28", "r29",
  "r30", "r31",   "X",   "Y",   "Z",   "C",
    "z",   "N",   "V",   "S",   "H",   "T",
    "I"
};

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_rd_5bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t rd = 0;

  rd = (opcode&0x01F0)>>4;

  opd->type = LIBMCU_OPD_TYPE_REGISTER;
  opd->size = LIBMCU_OPD_SIZE_BYTE;
  opd->value = AVR8_REG_GPO(rd);
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_rr_5bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t rr = 0;

  rr = (opcode&0x000F) | ((opcode&0x0200)>>5);

  opd->type = LIBMCU_OPD_TYPE_REGISTER;
  opd->size = LIBMCU_OPD_SIZE_BYTE;
  opd->value = AVR8_REG_GPO(rr);
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_rd_double_2bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t rd = 0;

  rd = ((opcode&0x0030)>>4) + AVR8_REG_R24;

  opd->type = LIBMCU_OPD_TYPE_DOUBLE_REG;
  opd->size = LIBMCU_OPD_SIZE_WORD;
  opd->value = (AVR8_REG_GPO((rd+1))) |
    (AVR8_REG_GPO(((uint16_t)rd<<8)));
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_rd_double_4bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t rd = 0;

  rd = (opcode&0x00F0)>>3;

  opd->type = LIBMCU_OPD_TYPE_DOUBLE_REG;
  opd->size = LIBMCU_OPD_SIZE_WORD;
  opd->value = (AVR8_REG_GPO((rd+1))) |
    (AVR8_REG_GPO(((uint16_t)rd<<8)));
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_rr_double_4bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t rr = 0;

  rr = (opcode&0x000F);
  rr = rr << 1;

  opd->type = LIBMCU_OPD_TYPE_DOUBLE_REG;
  opd->size = LIBMCU_OPD_SIZE_WORD;
  opd->value = (AVR8_REG_GPO((rr+1))) |
    (AVR8_REG_GPO(((uint16_t)rr<<8)));
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_k_6bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t k = 0;

  k = ((opcode&0x00C0)>>2) | (opcode&0x000F);

  opd->type = LIBMCU_OPD_TYPE_IMM;
  opd->size = LIBMCU_OPD_SIZE_BYTE;
  opd->value = k;
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_rd_4bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t rd = 0;

  rd = (opcode&0x00F0)>>4;

  opd->type = LIBMCU_OPD_TYPE_REGISTER;
  opd->size = LIBMCU_OPD_SIZE_BYTE;
  opd->value = AVR8_REG_GPO(rd+AVR8_REG_R16);
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_rr_4bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t rr = 0;

  rr = (opcode&0x000F);

  opd->type = LIBMCU_OPD_TYPE_REGISTER;
  opd->size = LIBMCU_OPD_SIZE_BYTE;
  opd->value = AVR8_REG_GPO(rr);
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_k_8bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t k = 0;

  k = ((opcode&0x0F00)>>4) | (opcode&0x000F);

  opd->type = LIBMCU_OPD_TYPE_IMM;
  opd->size = LIBMCU_OPD_SIZE_BYTE;
  opd->value = k;
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_b_3bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t b = 0;

  b = opcode&0x0007;

  opd->type = LIBMCU_OPD_TYPE_IMM;
  opd->size = LIBMCU_OPD_SIZE_BYTE;
  opd->value = b;
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_k_7bit_rel (libmcu_dasm_t* dasm, 
     libmcu_opd_t* opd, uint16_t opcode)
{
  uint64_t addr = 0;
  uint8_t addr7bit = 0;
  int64_t rel = 0;

  addr7bit = ((opcode&0x03F8)>>3);

  if (addr7bit&0x40)
    rel = ~(0x7F);

  rel = rel | addr7bit;
  addr = rel*2 + dasm->addr + 2;

  opd->type = LIBMCU_OPD_TYPE_IMM;
  opd->size = LIBMCU_OPD_SIZE_WORD;
  opd->flags = LIBMCU_OPD_FLAGS_DIRECT;
  opd->value = addr;
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_k_22bit (libmcu_opd_t* opd, uint32_t opcode)
{
  uint64_t addr = 0;

  addr = ((opcode&0x01F00000)>>3) |
    (opcode&0x0001FFFF);
  addr = addr << 1;

  opd->type = LIBMCU_OPD_TYPE_IMM;
  opd->size = LIBMCU_OPD_SIZE_WORD;
  opd->flags = LIBMCU_OPD_FLAGS_DIRECT;
  opd->value = addr;
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_k_4bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t k = 0;

  k = ((opcode&0x00F0)>>4);

  opd->type = LIBMCU_OPD_TYPE_IMM;
  opd->size = LIBMCU_OPD_SIZE_BYTE;
  opd->value = k;
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_a_5bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t addr = 0;

  addr = (opcode&0x00F8)>>3;

  opd->type = LIBMCU_OPD_TYPE_IMM;
  opd->size = LIBMCU_OPD_SIZE_BYTE;
  opd->flags = LIBMCU_OPD_FLAGS_DIRECT;
  opd->value = addr;
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_rd_3bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t rd = 0;

  rd = ((opcode&0x0070)>>4);

  opd->type = LIBMCU_OPD_TYPE_REGISTER;
  opd->size = LIBMCU_OPD_SIZE_BYTE;
  opd->value = AVR8_REG_GPO(rd);
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_rr_3bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t rr = 0;

  rr = (opcode&0x0007);

  opd->type = LIBMCU_OPD_TYPE_REGISTER;
  opd->size = LIBMCU_OPD_SIZE_BYTE;
  opd->value = AVR8_REG_GPO(rr);
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_a_6bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint8_t a = 0;

  a = ((opcode&0x0600)>>5) | (opcode&0x000F);

  opd->type = LIBMCU_OPD_TYPE_IMM;
  opd->size = LIBMCU_OPD_SIZE_BYTE;
  opd->flags = LIBMCU_OPD_FLAGS_DIRECT;
  opd->bus = LIBMCU_OPD_BUS_IDATA;
  opd->value = a;
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_q_6bit (libmcu_opd_t* opd, uint64_t reg,
    uint16_t opcode)
{
  uint8_t q = 0;

  q = ((opcode&0x2000)>>8) | ((opcode&0x0C00)>>7) |
    (opcode&0x0007);

  opd->type = LIBMCU_OPD_TYPE_DISPL;
  opd->size = LIBMCU_OPD_SIZE_BYTE;
  opd->displ.base_type = LIBMCU_DISPL_TYPE_REG;
  opd->displ.base = reg;
  opd->displ.offset_type = LIBMCU_DISPL_TYPE_IMM;
  opd->displ.offset = q;
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_k_16bit (libmcu_opd_t* opd, uint32_t opcode)
{
  uint16_t k = 0;

  k = opcode&0x0000FFFF;

  opd->type = LIBMCU_OPD_TYPE_IMM;
  opd->size = LIBMCU_OPD_SIZE_WORD;
  opd->flags = LIBMCU_OPD_FLAGS_DIRECT;
  opd->bus = LIBMCU_OPD_BUS_IDATA;
  opd->value = k;
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_k_7bit (libmcu_opd_t* opd, uint16_t opcode)
{
  uint16_t k = 0;

  k = ((opcode&0x0700)>>4) | (opcode&0x000F);
  k = k << 1;

  opd->type = LIBMCU_OPD_TYPE_IMM;
  opd->size = LIBMCU_OPD_SIZE_WORD;
  opd->flags = LIBMCU_OPD_FLAGS_DIRECT;
  opd->value = k;
}

/* ---------------------------------------------------------------- */
static void
 avr8_set_operand_k_12bit_rel (libmcu_opd_t* opd, libmcu_dasm_t* dasm,
     uint16_t opcode)
{
  uint64_t addr = 0;
  int64_t rel = 0;
  uint16_t addr12bit = 0;

  addr12bit = (opcode&0x0FFF);

  if (addr12bit&0x800)
    rel = ~(0x00000000000007FFull);

  rel = rel | addr12bit;
  addr = (int64_t)((int64_t)rel*2 + dasm->addr + 2);

  opd->type = LIBMCU_OPD_TYPE_IMM;
  opd->size = LIBMCU_OPD_SIZE_WORD;
  opd->flags = LIBMCU_OPD_FLAGS_DIRECT;
  opd->value = addr;
}

/* ---------------------------------------------------------------- */
/* CBR is missing. Prolly a pseudo-instruction? */
uint32_t
 avr8_disasm (libmcu_dasm_t* dasm, uint8_t* code, uint32_t size,
     uint64_t addr)
{
  uint16_t opcode = 0;
  uint32_t large_opcode = 0;
  uint32_t insn_size = 2;

  if (dasm == NULL) return 0;
  if (code == NULL) return 0;

  if (size < 2) return 0;

  dasm->mnemonic = AVR8_INSN_UNKNOWN;
  dasm->code = code;
  dasm->addr = addr;

  opcode = ((uint16_t)code[1]<<8) | code[0];

  /* 16-bit instruction opcode. */
  switch (opcode)
  {
      /* break */
    case 0x9598:
      dasm->mnemonic = AVR8_INSN_BREAK;
      break;

      /* clc */
    case 0x9488:
      dasm->mnemonic = AVR8_INSN_CLC;
      break;

      /* clh */
    case 0x94D8:
      dasm->mnemonic = AVR8_INSN_CLH;
      break;

      /* cli */
    case 0x94F8:
      dasm->mnemonic = AVR8_INSN_CLI;
      break;

      /* cln */
    case 0x94A8:
      dasm->mnemonic = AVR8_INSN_CLN;
      break;

      /* cls */
    case 0x94C8:
      dasm->mnemonic = AVR8_INSN_CLS;
      break;

      /* clt */
    case 0x94E8:
      dasm->mnemonic = AVR8_INSN_CLT;
      break;

      /* clv */
    case 0x94B8:
      dasm->mnemonic = AVR8_INSN_CLV;
      break;

      /* clz */
    case 0x9498:
      dasm->mnemonic = AVR8_INSN_CLZ;
      break;

      /* eicall */
    case 0x9519:
      dasm->mnemonic = AVR8_INSN_EICALL;
      break;

      /* eijmp */
    case 0x9419:
      dasm->mnemonic = AVR8_INSN_EIJMP;
      break;

      /* elpm r0, z */
    case 0x95D8:
      dasm->mnemonic = AVR8_INSN_ELPM;
      dasm->nopd = 2;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = AVR8_REG_R0;
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].value = AVR8_REG_Z;
      break;

      /* icall */
    case 0x9509:
      dasm->mnemonic = AVR8_INSN_ICALL;
      break;

      /* ijmp */
    case 0x9409:
      dasm->mnemonic = AVR8_INSN_IJMP;
      break;

      /* lpm */
    case 0x95C8:
      dasm->mnemonic = AVR8_INSN_LPM;
      break;

      /* nop */
    case 0x0000:
      dasm->mnemonic = AVR8_INSN_NOP;
      break;

      /* ret */
    case 0x9508:
      dasm->mnemonic = AVR8_INSN_RET;
      break;

      /* reti */
    case 0x9518:
      dasm->mnemonic = AVR8_INSN_RETI;
      break;

      /* sec */
    case 0x9408:
      dasm->mnemonic = AVR8_INSN_SEC;
      break;

      /* seh */
    case 0x9458:
      dasm->mnemonic = AVR8_INSN_SEH;
      break;
      
      /* sei */
    case 0x9478:
      dasm->mnemonic = AVR8_INSN_SEI;
      break;

      /* sen */
    case 0x9428:
      dasm->mnemonic = AVR8_INSN_SEN;
      break;

      /* ses */
    case 0x9448:
      dasm->mnemonic = AVR8_INSN_SES;
      break;

      /* set */
    case 0x9468:
      dasm->mnemonic = AVR8_INSN_SET;
      break;

      /* sev */
    case 0x9438:
      dasm->mnemonic = AVR8_INSN_SEV;
      break;

      /* sez */
    case 0x9418:
      dasm->mnemonic = AVR8_INSN_SEZ;
      break;

      /* sleep */
    case 0x9588:
      dasm->mnemonic = AVR8_INSN_SLEEP;
      break;

      /* spm */
    case 0x95E8:
      dasm->mnemonic = AVR8_INSN_SPM;
      dasm->nopd = 1;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].value = AVR8_REG_Z;
      break;

      /* spm z+ */
    case 0x95F8:
      dasm->mnemonic = AVR8_INSN_SPM;
      dasm->nopd = 1;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_POSTINC;
      dasm->opd[0].value = AVR8_REG_Z;
      break;

      /* wdr */
    case 0x9558:
      dasm->mnemonic = AVR8_INSN_WDR;
      break;
  }

  /* 14-bit instruction opcode. */
  /* 0bIIII IIII xxxx IIII */
  switch (opcode&0xFF0F)
  {
      /* des k */
    case 0x940B:
      dasm->mnemonic = AVR8_INSN_DES;
      dasm->nopd = 1;
      avr8_set_operand_k_4bit (&dasm->opd[0], opcode);
      break;
  }

  /* 11-bit instruction opcode. */
  /* 0bIIII IIIx xxxx IIII */
  switch (opcode&0xFE0F)
  {
      /* asr rd */
    case 0x9405:
      dasm->mnemonic = AVR8_INSN_ASR;
      dasm->nopd = 1;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      break;

      /* com rd */
    case 0x9400:
      dasm->mnemonic = AVR8_INSN_COM;
      dasm->nopd = 1;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      break;

      /* dec rd */
    case 0x940A:
      dasm->mnemonic = AVR8_INSN_DEC;
      dasm->nopd = 1;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      break;

      /* elpm rd, rampz:z */
    case 0x9006:
      dasm->mnemonic = AVR8_INSN_ELPM;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].value = AVR8_REG_Z;
      break;

      /* elpm rd, rampz:z+ */
    case 0x9007:
      dasm->mnemonic = AVR8_INSN_ELPM;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_POSTINC;
      dasm->opd[1].value = AVR8_REG_Z;
      break;

      /* inc rd */
    case 0x9403:
      dasm->mnemonic = AVR8_INSN_INC;
      dasm->nopd = 1;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      break;

      /* lac z, rd */
    case 0x9206:
      dasm->mnemonic = AVR8_INSN_LAC;
      dasm->nopd = 2;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = AVR8_REG_Z;
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;

      /* las z, rd */
    case 0x9205:
      dasm->mnemonic = AVR8_INSN_LAS;
      dasm->nopd = 2;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = AVR8_REG_Z;
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;

      /* lat z, rd */
    case 0x9207:
      dasm->mnemonic = AVR8_INSN_LAT;
      dasm->nopd = 2;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[0].value = AVR8_REG_Z;
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;

      /* ld rd, X */
    case 0x900C:
      dasm->mnemonic = AVR8_INSN_LD;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = AVR8_REG_X;
      break;

      /* ld rd, X+ */
    case 0x900D:
      dasm->mnemonic = AVR8_INSN_LD;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_POSTINC;
      dasm->opd[1].value = AVR8_REG_X;
      break;

      /* ld rd, -X */
    case 0x900E:
      dasm->mnemonic = AVR8_INSN_LD;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_PREDEC;
      dasm->opd[1].value = AVR8_REG_X;
      break;

      /* ld rd, y */
    case 0x8008:
      dasm->mnemonic = AVR8_INSN_LD;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = AVR8_REG_Y;
      break;

      /* ld rd, y+ */
    case 0x9009:
      dasm->mnemonic = AVR8_INSN_LD;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_POSTINC;
      dasm->opd[1].value = AVR8_REG_Y;
      break;

      /* ld rd, -y */
    case 0x900A:
      dasm->mnemonic = AVR8_INSN_LD;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_PREDEC;
      dasm->opd[1].value = AVR8_REG_Y;
      break;

      /* ld rd, z */
    case 0x8000:
      dasm->mnemonic = AVR8_INSN_LD;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].value = AVR8_REG_Z;
      break;

      /* ld rd, z+ */
    case 0x9001:
      dasm->mnemonic = AVR8_INSN_LD;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_POSTINC;
      dasm->opd[1].value = AVR8_REG_Z;
      break;

      /* ld rd, -z */
    case 0x9002:
      dasm->mnemonic = AVR8_INSN_LD;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_BYTE;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_PREDEC;
      dasm->opd[1].value = AVR8_REG_Z;
      break;

      /* lds rd, k */
    case 0x9000:
      dasm->mnemonic = AVR8_INSN_LDS;
      if (size < 4) return 0;
      large_opcode = ((uint32_t)opcode<<16) | ((uint32_t)code[2]<<8) |
        code[3];
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], large_opcode);
      avr8_set_operand_k_16bit (&dasm->opd[1], large_opcode);
      insn_size = 4;
      break;

      /* lpm rd, z */
    case 0x9004:
      dasm->mnemonic = AVR8_INSN_LPM;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].value = AVR8_REG_Z;
      break;

      /* lpm rd, z+ */
    case 0x9005:
      dasm->mnemonic = AVR8_INSN_LPM;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      dasm->opd[1].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[1].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[1].flags = LIBMCU_OPD_FLAGS_POSTINC;
      dasm->opd[1].value = AVR8_REG_Z;
      break;

      /* lsr rd */
    case 0x9406:
      dasm->mnemonic = AVR8_INSN_LSR;
      dasm->nopd = 1;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      break;

      /* neg rd */
    case 0x9401:
      dasm->mnemonic = AVR8_INSN_NEG;
      dasm->nopd = 1;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      break;

      /* pop rd */
    case 0x900F:
      dasm->mnemonic = AVR8_INSN_POP;
      dasm->nopd = 1;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      break;

      /* push rd */
    case 0x920F:
      dasm->mnemonic = AVR8_INSN_PUSH;
      dasm->nopd = 1;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      break;

      /* ror rd */
    case 0x9407:
      dasm->mnemonic = AVR8_INSN_ROR;
      dasm->nopd = 1;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      break;

      /* st x, rr */
    case 0x920C:
      dasm->mnemonic = AVR8_INSN_ST;
      dasm->nopd = 2;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].value = AVR8_REG_X;
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;

      /* st x+, rr */
    case 0x920D:
      dasm->mnemonic = AVR8_INSN_ST;
      dasm->nopd = 2;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_POSTINC;
      dasm->opd[0].value = AVR8_REG_X;
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;

      /* st -x, rr */
    case 0x920E:
      dasm->mnemonic = AVR8_INSN_ST;
      dasm->nopd = 2;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_PREDEC;
      dasm->opd[0].value = AVR8_REG_X;
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;

      /* st y, rr */
    case 0x8208:
      dasm->mnemonic = AVR8_INSN_ST;
      dasm->nopd = 2;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].value = AVR8_REG_Y;
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;

      /* st y+, rr */
    case 0x9209:
      dasm->mnemonic = AVR8_INSN_ST;
      dasm->nopd = 2;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_POSTINC;
      dasm->opd[0].value = AVR8_REG_Y;
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;

      /* st -y, rr */
    case 0x920A:
      dasm->mnemonic = AVR8_INSN_ST;
      dasm->nopd = 2;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_PREDEC;
      dasm->opd[0].value = AVR8_REG_Y;
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;

      /* st z, rr */
    case 0x8200:
      dasm->mnemonic = AVR8_INSN_ST;
      dasm->nopd = 2;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].value = AVR8_REG_Z;
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;

      /* st z+, rr */
    case 0x9201:
      dasm->mnemonic = AVR8_INSN_ST;
      dasm->nopd = 2;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_POSTINC;
      dasm->opd[0].value = AVR8_REG_Z;
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;

      /* st -z, rr */
    case 0x9202:
      dasm->mnemonic = AVR8_INSN_ST;
      dasm->nopd = 2;
      dasm->opd[0].type = LIBMCU_OPD_TYPE_REGISTER;
      dasm->opd[0].size = LIBMCU_OPD_SIZE_WORD;
      dasm->opd[0].flags = LIBMCU_OPD_FLAGS_PREDEC;
      dasm->opd[0].value = AVR8_REG_Z;
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;

      /* sts k, rr */
    case 0x9200:
      dasm->mnemonic = AVR8_INSN_STS;
      if (size < 4) return 0;
      large_opcode = ((uint32_t)opcode<<16) | ((uint32_t)code[3]<<8) |
        code[2];
      dasm->nopd = 2;
      avr8_set_operand_k_16bit (&dasm->opd[0], large_opcode);
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      insn_size = 4;
      break;

      /* swap rd */
    case 0x9402:
      dasm->mnemonic = AVR8_INSN_SWAP;
      dasm->nopd = 1;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      break;

      /* xch rd */
    case 0x9204:
      dasm->mnemonic = AVR8_INSN_XCH;
      dasm->nopd = 1;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      break;
  }

  /* 10-bit instruction opcode. */
  /* 0bIIII IIII Ixxx Ixxx */
  switch (opcode&0xFF88)
  {
      /* fmul rd, rr */
    case 0x0308:
      dasm->mnemonic = AVR8_INSN_FMUL;
      dasm->nopd = 2;
      avr8_set_operand_rd_3bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_3bit (&dasm->opd[1], opcode);
      break;

      /* fmuls rd, rr */
    case 0x0380:
      dasm->mnemonic = AVR8_INSN_FMULS;
      dasm->nopd = 2;
      avr8_set_operand_rd_3bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_3bit (&dasm->opd[1], opcode);
      break;

      /* fmulsu rd, rr */
    case 0x0388:
      dasm->mnemonic = AVR8_INSN_FMULSU;
      dasm->nopd = 2;
      avr8_set_operand_rd_3bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_3bit (&dasm->opd[1], opcode);
      break;

      /* mulsu rd, rr */
    case 0x0300:
      dasm->mnemonic = AVR8_INSN_MULSU;
      dasm->nopd = 2;
      avr8_set_operand_rd_3bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_3bit (&dasm->opd[1], opcode);
      break;
  }

  /* 9-bit instruction opcode. */
  /* 0bIIII IIIx xxxx IIIx */
  switch (opcode&0xFE0E)
  {
      /* call k */
    case 0x940E:
      dasm->mnemonic = AVR8_INSN_CALL;
      dasm->nopd = 1;
      if (size < 4) return 0;
      large_opcode = ((uint32_t)opcode<<16) | ((uint32_t)code[3]<<8) |
        code[2];
      avr8_set_operand_k_22bit (&dasm->opd[0], large_opcode);
      insn_size = 4;
      break;

      /* jmp k */
    case 0x940C:
      dasm->mnemonic = AVR8_INSN_JMP;
      dasm->nopd = 1;
      if (size < 4) return 0;
      large_opcode = ((uint32_t)opcode<<16) | ((uint32_t)code[3]<<8) |
        code[2];
      avr8_set_operand_k_22bit (&dasm->opd[0], large_opcode);
      insn_size = 4;
      break;
  }

  /* 9-bit instruction opcode. */
  /* 0bIIII IIxx xxxx xIII */
  switch (opcode&0xFC07)
  {
      /* brcc k */
      /* brsh k */
    case 0xF400:
      dasm->mnemonic = AVR8_INSN_BRCC;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;

      /* brlo k */
      /* brcs k */
    case 0xF000:
      dasm->mnemonic = AVR8_INSN_BRCS;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;

      /* breq k */
    case 0xF001:
      dasm->mnemonic = AVR8_INSN_BREQ;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;

      /* brlt k */
      /* brge k */
    case 0xF404:
      dasm->mnemonic = AVR8_INSN_BRGE;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;

      /* brhc k */
    case 0xF405:
      dasm->mnemonic = AVR8_INSN_BRHC;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;

      /* brhs k */
    case 0xF005:
      dasm->mnemonic = AVR8_INSN_BRHS;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;

      /* brid k */
    case 0xF407:
      dasm->mnemonic = AVR8_INSN_BRID;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;

      /* brie k */
    case 0xF007:
      dasm->mnemonic = AVR8_INSN_BRIE;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;

      /* brmi k */
    case 0xF002:
      dasm->mnemonic = AVR8_INSN_BRMI;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;

      /* brne k */
    case 0xF401:
      dasm->mnemonic = AVR8_INSN_BRNE;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;

      /* brpl k */
    case 0xF402:
      dasm->mnemonic = AVR8_INSN_BRPL;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;

      /* brtc k */
    case 0xF406:
      dasm->mnemonic = AVR8_INSN_BRTC;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;

      /* brts k */
    case 0xF006:
      dasm->mnemonic = AVR8_INSN_BRTS;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;

      /* brvc k */
    case 0xF403:
      dasm->mnemonic = AVR8_INSN_BRVC;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;

      /* brvs k */
    case 0xF003:
      dasm->mnemonic = AVR8_INSN_BRVS;
      dasm->nopd = 1;
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[0],
         opcode);
      break;
  }

  /* 8-bit instruction opcode. */
  /* 0bIIII IIII xxxx xxxx */
  switch (opcode&0xFF00)
  {
      /* adiw rd:rd+1, k */
    case 0x9600:
      dasm->mnemonic = AVR8_INSN_ADIW;
      dasm->nopd = 2;
      avr8_set_operand_rd_double_2bit (&dasm->opd[0], opcode);
      avr8_set_operand_k_6bit (&dasm->opd[1], opcode);
      break;

      /* cbi A,b */
    case 0x9800:
      dasm->mnemonic = AVR8_INSN_CBI;
      dasm->nopd = 2;
      avr8_set_operand_a_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_b_3bit (&dasm->opd[1], opcode);
      break;

      /* movw rd+1:rd, rr+1:rr */
    case 0x0100:
      dasm->mnemonic = AVR8_INSN_MOVW;
      dasm->nopd = 2;
      avr8_set_operand_rd_double_4bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_double_4bit (&dasm->opd[1], opcode);
      break;

      /* muls rd, rr */
    case 0x0200:
      dasm->mnemonic = AVR8_INSN_MULS;
      dasm->nopd = 2;
      avr8_set_operand_rd_4bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_4bit (&dasm->opd[1], opcode);
      break;

      /* sbi a, b */
    case 0x9A00:
      dasm->mnemonic = AVR8_INSN_SBI;
      dasm->nopd = 2;
      avr8_set_operand_a_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_b_3bit (&dasm->opd[1], opcode);
      break;

      /* sbic a, b */
    case 0x9900:
      dasm->mnemonic = AVR8_INSN_SBIC;
      dasm->nopd = 2;
      avr8_set_operand_a_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_b_3bit (&dasm->opd[1], opcode);
      break;

      /* sbis a, b */
    case 0x9B00:
      dasm->mnemonic = AVR8_INSN_SBIS;
      dasm->nopd = 2;
      avr8_set_operand_a_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_b_3bit (&dasm->opd[1], opcode);
      break;

      /* sbiw rd+1:rd, k */
    case 0x9700:
      dasm->mnemonic = AVR8_INSN_SBIW;
      dasm->nopd = 2;
      avr8_set_operand_rd_double_2bit (&dasm->opd[0], opcode);
      avr8_set_operand_k_6bit (&dasm->opd[1], opcode);
      break;

  }

  /* 8-bit instruction opcode. */
  /* 0bIIII IIIx xxxx Ixxx */
  switch (opcode&0xFE08)
  {
      /* bld rd, b */
    case 0xF800:
      dasm->mnemonic = AVR8_INSN_BLD;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_b_3bit (&dasm->opd[1], opcode);
      break;

      /* bst rd, b */
    case 0xFA00:
      dasm->mnemonic = AVR8_INSN_BST;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_b_3bit (&dasm->opd[1], opcode);
      break;

      /* sbrc rd, b */
    case 0xFC00:
      dasm->mnemonic = AVR8_INSN_SBRC;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_b_3bit (&dasm->opd[1], opcode);
      break;

      /* sbrs rd, b */
    case 0xFE00:
      dasm->mnemonic = AVR8_INSN_SBRS;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_b_3bit (&dasm->opd[1], opcode);
      break;
  }

  /* 6-bit instruction opcode. */
  /* 0bIIII IIxx xxxx xxxx */
  switch (opcode&0xFC00)
  {
      /* adc rd, rr */
    case 0x1C00:
      dasm->mnemonic = AVR8_INSN_ADC;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_5bit (&dasm->opd[1], opcode);
      break;

      /* add rd, rr */
    case 0x0C00:
      dasm->mnemonic = AVR8_INSN_ADD;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_5bit (&dasm->opd[1], opcode);
      break;

      /* and rd, rr */
    case 0x2000:
      dasm->mnemonic = AVR8_INSN_AND;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_5bit (&dasm->opd[1], opcode);
      break;

      /* brbc s,k */
      /*
    case 0xF400:
      dasm->mnemonic = AVR8_INSN_BRBC;
      dasm->nopd = 2;
      avr8_set_operand_b_3bit (&dasm->opd[0], opcode);
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[1],
          opcode);
      break;
      */

      /* brbs s,k */
      /*
    case 0xF000:
      dasm->mnemonic = AVR8_INSN_BRBS;
      avr8_set_operand_b_3bit (&dasm->opd[0], opcode);
      avr8_set_operand_k_7bit_rel (dasm, &dasm->opd[1],
          opcode);
      break;
      */

      /* eor rd, rr */
    case 0x2400:
      dasm->mnemonic = AVR8_INSN_EOR;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_5bit (&dasm->opd[1], opcode);
      break;

      /* cp rd, rr */
    case 0x1400:
      dasm->mnemonic = AVR8_INSN_CP;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_5bit (&dasm->opd[1], opcode);
      break;

      /* cpc rd, rr */
    case 0x0400:
      dasm->mnemonic = AVR8_INSN_CPC;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_5bit (&dasm->opd[1], opcode);
      break;

      /* cpse rd, rr */
    case 0x1000:
      dasm->mnemonic = AVR8_INSN_CPSE;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_5bit (&dasm->opd[1], opcode);
      break;

      /* mov rd, rr */
    case 0x2C00:
      dasm->mnemonic = AVR8_INSN_MOV;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_5bit (&dasm->opd[1], opcode);
      break;

      /* mul rd, rr */
    case 0x9C00:
      dasm->mnemonic = AVR8_INSN_MUL;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_5bit (&dasm->opd[1], opcode);
      break;

      /* or rd, rr */
    case 0x2800:
      dasm->mnemonic = AVR8_INSN_OR;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_5bit (&dasm->opd[1], opcode);
      break;

      /* sbc rd, rr */
    case 0x0800:
      dasm->mnemonic = AVR8_INSN_SBC;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_5bit (&dasm->opd[1], opcode);
      break;

      /* sub rd, rr */
    case 0x1800:
      dasm->mnemonic = AVR8_INSN_SUB;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_rr_5bit (&dasm->opd[1], opcode);
      break;

  }

  /* 5-bit instruction opcode. */
  /* 0bIIII Ixxx xxxx xxxx */
  switch (opcode&0xF800)
  {
      /* in rd, a */
    case 0xB000:
      dasm->mnemonic = AVR8_INSN_IN;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_a_6bit (&dasm->opd[1], opcode);
      break;

      /* lds rd, k */
    case 0xA000:
      dasm->mnemonic = AVR8_INSN_LDS;
      dasm->nopd = 2;
      avr8_set_operand_rd_4bit (&dasm->opd[0], opcode);
      avr8_set_operand_k_7bit (&dasm->opd[1], opcode);
      break;

      /* out a, rr */
    case 0xB800:
      dasm->mnemonic = AVR8_INSN_OUT;
      dasm->nopd = 2;
      avr8_set_operand_a_6bit (&dasm->opd[0], opcode);
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;

      /* sts k, rr */
    case 0xA800:
      dasm->mnemonic = AVR8_INSN_STS;
      dasm->nopd = 2;
      avr8_set_operand_k_7bit (&dasm->opd[0], opcode);
      avr8_set_operand_rd_4bit (&dasm->opd[1], opcode);
      break;
  }

  /* 5-bit instruction opcode. */
  /* 0bIIxI xxIx xxxx Ixxx */
  switch (opcode&0xD208)
  {
      /* ldd rd, y+q */
    case 0x8008:
      dasm->mnemonic = AVR8_INSN_LDD;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_q_6bit (&dasm->opd[1], AVR8_REG_Y, opcode);
      break;

      /* ldd rd, z+q */
    case 0x8000:
      dasm->mnemonic = AVR8_INSN_LDD;
      dasm->nopd = 2;
      avr8_set_operand_rd_5bit (&dasm->opd[0], opcode);
      avr8_set_operand_q_6bit (&dasm->opd[1], AVR8_REG_Z, opcode);
      break;

      /* std y+q, rr */
    case 0x8208:
      dasm->mnemonic = AVR8_INSN_STD;
      dasm->nopd = 2;
      avr8_set_operand_q_6bit (&dasm->opd[0], AVR8_REG_Y, opcode);
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;

      /* std z+q, rr */
    case 0x8200:
      dasm->mnemonic = AVR8_INSN_STD;
      dasm->nopd = 2;
      avr8_set_operand_q_6bit (&dasm->opd[0], AVR8_REG_Z, opcode);
      avr8_set_operand_rd_5bit (&dasm->opd[1], opcode);
      break;
  }

  /* 4-bit instruction opcode. */
  /* 0bIIII xxxx xxxx xxxx */
  switch (opcode&0xF000)
  {
      /* andi rd, k */
    case 0x7000:
      dasm->mnemonic = AVR8_INSN_ANDI;
      dasm->nopd = 2;
      avr8_set_operand_rd_4bit (&dasm->opd[0], opcode);
      avr8_set_operand_k_8bit (&dasm->opd[1], opcode);
      break;

      /* cpi */
    case 0x3000:
      dasm->mnemonic = AVR8_INSN_CPI;
      dasm->nopd = 2;
      avr8_set_operand_rd_4bit (&dasm->opd[0], opcode);
      avr8_set_operand_k_8bit (&dasm->opd[1], opcode);
      break;

      /* ldi rd, k */
    case 0xE000:
      dasm->mnemonic = AVR8_INSN_LDI;
      dasm->nopd = 2;
      avr8_set_operand_rd_4bit (&dasm->opd[0], opcode);
      avr8_set_operand_k_8bit (&dasm->opd[1], opcode);
      break;

      /* ori rd, k */
    case 0x6000:
      dasm->mnemonic = AVR8_INSN_ORI;
      dasm->nopd = 2;
      avr8_set_operand_rd_4bit (&dasm->opd[0], opcode);
      avr8_set_operand_k_8bit (&dasm->opd[1], opcode);
      break;

      /* rcall k */
    case 0xD000:
      dasm->mnemonic = AVR8_INSN_RCALL;
      dasm->nopd = 1;
      avr8_set_operand_k_12bit_rel (&dasm->opd[0], dasm, opcode);
      break;

      /* rjmp k */
    case 0xC000:
      dasm->mnemonic = AVR8_INSN_RJMP;
      dasm->nopd = 1;
      avr8_set_operand_k_12bit_rel (&dasm->opd[0], dasm, opcode);
      break;

      /* sbci rd, k */
    case 0x4000:
      dasm->mnemonic = AVR8_INSN_SBCI;
      dasm->nopd = 2;
      avr8_set_operand_rd_4bit (&dasm->opd[0], opcode);
      avr8_set_operand_k_8bit (&dasm->opd[1], opcode);
      break;

      /* subi rd, k */
    case 0x5000:
      dasm->mnemonic = AVR8_INSN_SUBI;
      dasm->nopd = 2;
      avr8_set_operand_rd_4bit (&dasm->opd[0], opcode);
      avr8_set_operand_k_8bit (&dasm->opd[1], opcode);
      break;
  }
  
  dasm->size = insn_size;

  return insn_size;
}

/* ---------------------------------------------------------------- */
/* Thanksfully visual studio's compiler of shit the C99 structure   */
/* declaration with a dot is NOT FKING SUPPORTED.                   */
/* Just go fking die microsoft. Die you piece of shit cumbag mofo.  */
libmcu_asm_fmt_t avr8_default_fmt = 
{
/*  .fmt_name          =*/ "avrasm2 assembler format",
/*  .fmt_shortname     =*/ "avrasm2",
/*  .opcode_padding    =*/ 4,
/*  .comm_padding      =*/ 40,
/*  .dst_first         =*/ LIBMCU_ASM_DST_FIRST,
/*  .def_fmt           =*/ LIBMCU_ASM_DEF_FORMAT_HEX,
/*  .insn_prefix       =*/ NULL,
/*  .insn_suffix       =*/ NULL,
/*  .register_prefix   =*/ NULL,
/*  .register_suffix   =*/ NULL,
/*  .indirect_prefix   =*/ NULL,
/*  .indirect_suffix   =*/ NULL,
/*  .displ_prefix      =*/ NULL, 
/*  .displ_suffix      =*/ NULL,
/*  .immediat_prefix   =*/ NULL,
/*  .immediat_suffix   =*/ NULL,
/*  .direct_prefix     =*/ NULL,
/*  .direct_suffix     =*/ NULL,
/*  .indexed_prefix    =*/ NULL,
/*  .indexed_suffix    =*/ NULL,
/*  .hex_prefix        =*/ "0x",
/*  .hex_suffix        =*/ NULL,
/*  .bin_prefix        =*/ "0b",
/*  .bin_suffix        =*/ NULL,
/*  .dec_prefix        =*/ NULL,
/*  .dec_suffix        =*/ NULL,
/*  .oct_prefix        =*/ "0",
/*  .oct_suffix        =*/ NULL,
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
/*  .multi_reg         =*/ ":",
};

/* ---------------------------------------------------------------- */
libmcu_asm_fmt_t avr8_fmt_colors = 
{
/*  .fmt_name          =*/ "avrasm2 assembler format with colors",
/*  .fmt_shortname     =*/ "avrasm2_colors",
/*  .opcode_padding    =*/ 4,
/*  .comm_padding      =*/ 40,
/*  .dst_first         =*/ LIBMCU_ASM_DST_FIRST,
/*  .def_fmt           =*/ LIBMCU_ASM_DEF_FORMAT_HEX,
/*  .insn_prefix       =*/ NULL,
/*  .insn_suffix       =*/ NULL,
/*  .register_prefix   =*/ "\x1b[0;31m",
/*  .register_suffix   =*/ "\x1b[0m",
/*  .indirect_prefix   =*/ "\x1b[0;33m",
/*  .indirect_suffix   =*/ "\x1b[0m",
/*  .displ_prefix      =*/ NULL,
/*  .displ_suffix      =*/ NULL,
/*  .immediat_prefix   =*/ "\x1b[0;34m",
/*  .immediat_suffix   =*/ "\x1b[0m",
/*  .direct_prefix     =*/ "\x1b[0;32m",
/*  .direct_suffix     =*/ "\x1b[0m",
/*  .indexed_prefix    =*/ NULL,
/*  .indexed_suffix    =*/ NULL,
/*  .hex_prefix        =*/ "0x",
/*  .hex_suffix        =*/ NULL,
/*  .bin_prefix        =*/ "0b",
/*  .bin_suffix        =*/ NULL,
/*  .dec_prefix        =*/ NULL,
/*  .dec_suffix        =*/ NULL,
/*  .oct_prefix        =*/ "0",
/*  .oct_suffix        =*/ NULL,
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
/*  .multi_reg         =*/ ":",
};

/* ---------------------------------------------------------------- */
libmcu_asm_fmt_t* avr8_asm_fmts[] = 
{
  &avr8_default_fmt,
  &avr8_fmt_colors,
  NULL
};

/* ---------------------------------------------------------------- */
libmcu_arch_t __g_libmcu_internal_avr8_arch =
{
/*  .name            =*/ "ATMEL AVR 8(16)-bit",
/*  .shortname       =*/ "avr8",
/*  .author          =*/ "stfsux",
/*  .licname         =*/ "WTFPL",
/*  .licfull         =*/ "bitebitebitbiebtiei",
/*  .description     =*/ "Generic 8(16)-bit ATMEL AVR microcontroller "
                         "disassembler.",

/*  .mnemonics       =*/ avr8_mnemonics,
/*  .ninsn           =*/ 0x00,

/*  .regname         =*/ avr8_regname,
/*  .nregs           =*/ 0x00,

/*  .bus_type        =*/ LIBMCU_ARCH_BUS_HARVARD_MOD,
/*  .bus_size        =*/ LIBMCU_ARCH_BUS_SIZE_16BIT,
/*  .predisasm       =*/ NULL,
/*  .disasm          =*/ avr8_disasm,
/*  .postdisasm      =*/ NULL,

/*  .asm_fmt         =*/ avr8_asm_fmts, 
/*  .num_asm_fmts    =*/ 2,

/*  .opcode_max_size =*/ 256,
};

