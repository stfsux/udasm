
#ifndef __ARCH_H__
#define __ARCH_H__

/* TODO: sign in disp struct. */

typedef struct mcu_opd_disp
{
  uint8_t base_type;
#define MCU_DISPL_TYPE_NONE 0
#define MCU_DISPL_TYPE_REG  1
#define MCU_DISPL_TYPE_IMM  2
  uint64_t base;

  uint8_t mul_type;
  uint64_t mul;

  uint8_t offset_type;
  uint64_t offset;
}mcu_opd_disp_t;

typedef struct mcu_opd
{
  uint8_t type;
#define MCU_OPD_TYPE_UNKNOWN  0
#define MCU_OPD_TYPE_REGISTER 1
#define MCU_OPD_TYPE_IMM      2
#define MCU_OPD_TYPE_DISPL    3
  uint8_t size;
#define MCU_OPD_SIZE_UNKNOWN  0
#define MCU_OPD_SIZE_BYTE     1
#define MCU_OPD_SIZE_WORD     2
#define MCU_OPD_SIZE_DWORD    3
#define MCU_OPD_SIZE_QWORD    4
#define MCU_OPD_SIZE_BIT      5
  uint32_t flags;
#define MCU_OPD_FLAGS_DIRECT    ((uint32_t)1<<0)
#define MCU_OPD_FLAGS_INDIRECT  ((uint32_t)1<<1)
#define MCU_OPD_FLAGS_POSTINC   ((uint32_t)1<<2)
#define MCU_OPD_FLAGS_PREINC    ((uint32_t)1<<3)
#define MCU_OPD_FLAGS_POSTDEC   ((uint32_t)1<<4)
#define MCU_OPD_FLAGS_PREDEC    ((uint32_t)1<<5)
  uint8_t bus;
  /* Von Neumann arch. */
#define MCU_OPD_BUS_WATEVER 0
  /* Harvard / Harvard-mod arch. */
#define MCU_OPD_BUS_CODE    1
#define MCU_OPD_BUS_IDATA   2
#define MCU_OPD_BUS_XDATA   3
  uint64_t value;
  mcu_opd_disp_t displ;
}mcu_opd_t;

typedef struct mcu_asm_fmt
{
  char* fmt_name;
  char* fmt_shortname;
  uint8_t opcode_padding;
  uint8_t comm_padding;
  uint8_t dst_first;
#define MCU_ASM_DST_UNKN  0
#define MCU_ASM_DST_FIRST 1
#define MCU_ASM_DST_LAST  2
  uint8_t def_fmt;
#define MCU_ASM_DEF_FORMAT_UNK  0
#define MCU_ASM_DEF_FORMAT_HEX  1
#define MCU_ASM_DEF_FORMAT_DEC  2
#define MCU_ASM_DEF_FORMAT_BIN  3
#define MCU_ASM_DEF_FORMAT_OCT  4
  char* insn_prefix;
  char* insn_suffix;
  char* register_prefix;
  char* register_suffix;
  char* indirect_prefix;
  char* indirect_suffix;
  char* displ_prefix;
  char* displ_suffix;
  char* immediat_prefix;
  char* immediat_suffix;
  char* direct_prefix;
  char* direct_suffix;
  char* indexed_prefix;
  char* indexed_suffix;
  char* hex_prefix;
  char* hex_suffix;
  char* bin_prefix;
  char* bin_suffix;
  char* dec_prefix;
  char* dec_suffix;
  char* oct_prefix;
  char* oct_suffix;
  char* displ_add;
  char* displ_sub;
  char* displ_mul;
  char* size_byte_prefix;
  char* size_byte_suffix;
  char* size_word_prefix;
  char* size_word_suffix;
  char* size_dword_prefix;
  char* size_dword_suffix;
  char* size_qword_prefix;
  char* size_qword_suffix;
  char* data_byte;
  char* data_word;
  char* data_dword;
  char* data_qword;
  char* comm_prefix;
  char* comm_suffix;
}mcu_asm_fmt_t;

typedef struct mcu_dasm
{
  uint32_t mnemonic;
  uint8_t nopd;
#define MCU_DASM_MAX_OPND 16
  mcu_opd_t opd[MCU_DASM_MAX_OPND];
#define MCU_DASM_OPND_DST_LAST  1
#define MCU_DASM_OPND_DST_FIRST 2
  uint8_t flags;
  uint32_t size;
  uint64_t addr;
  uint8_t* code;
}mcu_dasm_t;

typedef struct mcu_arch
{
  /* arch plugin informations */
  char* name;
  char* shortname;
  char* author;
  char* licname;
  char* licfull;

  /* disassembler data */
  char** mnemonics;
  uint32_t ninsn;
  char** regname;
  uint32_t nregs;

  uint8_t bus_type;
#define MCU_ARCH_BUS_UNKNOWN      0
  /* Single bus for both code and memory. */
#define MCU_ARCH_BUS_VONNEUMANN   1
  /* One bus for code and one for memory. */
  /* Instruction set cannot access to code address space. */
#define MCU_ARCH_BUS_HARVARD      2
  /* Instruction set is able to access two bus (code and memory). */
#define MCU_ARCH_BUS_HARVARD_MOD  3
  uint8_t bus_size;
#define MCU_ARCH_BUS_SIZE_UNKN    0
#define MCU_ARCH_BUS_SIZE_8BIT    1
#define MCU_ARCH_BUS_SIZE_16BIT   2
#define MCU_ARCH_BUS_SIZE_32BIT   3
#define MCU_ARCH_BUS_SIZE_64BIT   4

  /* disassembler functions */
  uint32_t (*predisasm)(mcu_dasm_t* dasm, uint8_t* code,
      uint32_t size, void* data);
  /* disasm: */
  /* must returns size of the instruction. */
  /* must fill at least : */
  /*  - a copy of code pointer into dasm->code. */
  /*  - size of the instruction into dasm->size. */
  /* if you don't fill these fields it will explode. */
  /* So do it, you mofo. */
  uint32_t (*disasm)(mcu_dasm_t* dasm, uint8_t* code, uint32_t size,
      uint64_t addr);
  uint32_t (*postdisasm)(mcu_dasm_t* dasm, uint8_t* code,
      uint32_t size, void* data);

  mcu_asm_fmt_t** asm_fmt;
  uint8_t num_asm_fmts;
  uint32_t opcode_max_size;
}mcu_arch_t;

typedef struct mcu_ctx
{
  mcu_arch_t* arch;
}mcu_ctx_t;

/* ---------------------------------------------------------------- */
#define LIBMCU_ARCH_I8051 0

/* ---------------------------------------------------------------- */
extern mcu_arch_t i8051_arch;

/* ---------------------------------------------------------------- */
mcu_ctx_t* libmcu_arch_create (uint8_t arch);
uint8_t libmcu_arch_get_num_archs (void);
mcu_arch_t* libmcu_arch_get (uint8_t num);
uint32_t libmcu_arch_disasm (mcu_ctx_t* ctx, mcu_dasm_t* dasm,
    uint8_t* code, uint32_t size, uint64_t addr);
uint32_t libmcu_arch_disasm_ex (mcu_ctx_t* ctx, mcu_dasm_t* dasm,
    uint8_t* code, uint32_t size, uint64_t addr, void* data);
#define libmcu_arch_destroy(a) libmcu_arch_destroy_safe (&a)
void libmcu_arch_destroy_safe (mcu_ctx_t** ctx);

#endif


