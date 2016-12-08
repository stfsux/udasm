
#include <ida.hpp>
#include <idp.hpp>
#include <stdint.h>

/* libmcu */
#include <libconfig.h>
#include <libmcu_arch.h>
#include <libmcu_mmap.h>
#include <arch/i8051.h>

#include "ins.hpp"

instruc_t mcu8051_instructions[] = 
{
  { INSN(I8051_INSN_ACALL), CF_CALL },
  { INSN(I8051_INSN_ADD), 0 },
  { INSN(I8051_INSN_ADDC), 0 },
  { INSN(I8051_INSN_AJMP), CF_STOP },
  { INSN(I8051_INSN_ANL), 0 },
  { INSN(I8051_INSN_CJNE), 0 },
  { INSN(I8051_INSN_CLR), 0 },
  { INSN(I8051_INSN_CPL), 0 },
  { INSN(I8051_INSN_DA), 0 },
  { INSN(I8051_INSN_DEC), 0 },
  { INSN(I8051_INSN_DIV), 0 },
  { INSN(I8051_INSN_DJNZ), 0 },
  { INSN(I8051_INSN_INC), 0 },
  { INSN(I8051_INSN_JB), 0 },
  { INSN(I8051_INSN_JBC), 0 },
  { INSN(I8051_INSN_JC), 0 },
  { INSN(I8051_INSN_JMP), CF_STOP },
  { INSN(I8051_INSN_JNB), 0 },
  { INSN(I8051_INSN_JNC), 0 },
  { INSN(I8051_INSN_JNZ), 0 },
  { INSN(I8051_INSN_JZ), 0 },
  { INSN(I8051_INSN_LCALL), CF_CALL },
  { INSN(I8051_INSN_LJMP), CF_STOP },
  { INSN(I8051_INSN_MOV), 0 },
  { INSN(I8051_INSN_MOVC), 0 },
  { INSN(I8051_INSN_MOVX), 0 },
  { INSN(I8051_INSN_MUL), 0 },
  { INSN(I8051_INSN_NOP), 0 },
  { INSN(I8051_INSN_ORL), 0 },
  { INSN(I8051_INSN_POP), 0 },
  { INSN(I8051_INSN_PUSH), 0 },
  { INSN(I8051_INSN_RET), CF_STOP },
  { INSN(I8051_INSN_RETI), CF_STOP },
  { INSN(I8051_INSN_RL), 0 },
  { INSN(I8051_INSN_RLC), 0 },
  { INSN(I8051_INSN_RR), 0 },
  { INSN(I8051_INSN_RRC), 0 },
  { INSN(I8051_INSN_SETB), 0 },
  { INSN(I8051_INSN_SJMP), CF_STOP },
  { INSN(I8051_INSN_SUBB), 0 },
  { INSN(I8051_INSN_SWAP), 0 },
  { INSN(I8051_INSN_XCH), 0 },
  { INSN(I8051_INSN_XCHD), 0 },
  { INSN(I8051_INSN_XRL), 0 },
  { INSN(I8051_INSN_UNKN), 0 },
};

