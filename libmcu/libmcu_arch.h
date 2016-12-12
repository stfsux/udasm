
#ifndef __ARCH_H__
#define __ARCH_H__

/* TODO: sign in disp struct. */

typedef struct libmcu_opd_disp
{
  uint8_t base_type;
#define LIBMCU_DISPL_TYPE_NONE 0
#define LIBMCU_DISPL_TYPE_REG  1
#define LIBMCU_DISPL_TYPE_IMM  2
  uint64_t base;

  uint8_t mul_type;
  uint64_t mul;

  uint8_t offset_type;
  uint64_t offset;
}libmcu_opd_disp_t;

typedef struct libmcu_opd
{
  uint8_t type;
#define LIBMCU_OPD_TYPE_UNKNOWN    0
#define LIBMCU_OPD_TYPE_REGISTER   1
#define LIBMCU_OPD_TYPE_IMM        2
#define LIBMCU_OPD_TYPE_DISPL      3
#define LIBMCU_OPD_TYPE_DOUBLE_REG 4
  uint8_t size;
#define LIBMCU_OPD_SIZE_UNKNOWN  0
#define LIBMCU_OPD_SIZE_BYTE     1
#define LIBMCU_OPD_SIZE_WORD     2
#define LIBMCU_OPD_SIZE_DWORD    3
#define LIBMCU_OPD_SIZE_QWORD    4
#define LIBMCU_OPD_SIZE_BIT      5
  uint32_t flags;
#define LIBMCU_OPD_FLAGS_DIRECT    ((uint32_t)1<<0)
#define LIBMCU_OPD_FLAGS_INDIRECT  ((uint32_t)1<<1)
#define LIBMCU_OPD_FLAGS_POSTINC   ((uint32_t)1<<2)
#define LIBMCU_OPD_FLAGS_PREINC    ((uint32_t)1<<3)
#define LIBMCU_OPD_FLAGS_POSTDEC   ((uint32_t)1<<4)
#define LIBMCU_OPD_FLAGS_PREDEC    ((uint32_t)1<<5)
  uint8_t bus;
  /* Von Neumann arch. */
#define LIBMCU_OPD_BUS_WATEVER     0
  /* Harvard / Harvard-mod arch. */
#define LIBMCU_OPD_BUS_CODE        1
#define LIBMCU_OPD_BUS_IDATA       2
#define LIBMCU_OPD_BUS_XDATA       3
#define LIBMCU_OPD_BUS_IDATA_BITS  4
  uint64_t value;
  libmcu_opd_disp_t displ;
}libmcu_opd_t;

typedef struct libmcu_asm_fmt
{
  char* fmt_name;
  char* fmt_shortname;
  uint8_t opcode_padding;
  uint8_t comm_padding;
  uint8_t dst_first;
#define LIBMCU_ASM_DST_UNKN  0
#define LIBMCU_ASM_DST_FIRST 1
#define LIBMCU_ASM_DST_LAST  2
  uint8_t def_fmt;
#define LIBMCU_ASM_DEF_FORMAT_UNK  0
#define LIBMCU_ASM_DEF_FORMAT_HEX  1
#define LIBMCU_ASM_DEF_FORMAT_DEC  2
#define LIBMCU_ASM_DEF_FORMAT_BIN  3
#define LIBMCU_ASM_DEF_FORMAT_OCT  4
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
  char* multi_reg;
}libmcu_asm_fmt_t;

typedef struct libmcu_dasm
{
  uint32_t mnemonic;
  uint8_t nopd;
#define LIBMCU_DASM_MAX_OPND 16
  libmcu_opd_t opd[LIBMCU_DASM_MAX_OPND];
#define LIBMCU_DASM_OPND_DST_LAST  1
#define LIBMCU_DASM_OPND_DST_FIRST 2
  uint8_t flags;
  uint32_t size;
  uint64_t addr;
  uint8_t* code;
}libmcu_dasm_t;

typedef struct libmcu_arch
{
  /* arch plugin informations */
  char* name;
  char* shortname;
  char* author;
  char* licname;
  char* licfull;
  char* description;

  /* disassembler data */
  char** mnemonics;
  uint32_t ninsn;
  char** regname;
  uint32_t nregs;

  uint8_t bus_type;
#define LIBMCU_ARCH_BUS_UNKNOWN      0
  /* Single bus for both code and memory. */
#define LIBMCU_ARCH_BUS_VONNEUMANN   1
  /* One bus for code and one for memory. */
  /* Instruction set cannot access to code address space. */
#define LIBMCU_ARCH_BUS_HARVARD      2
  /* Instruction set is able to access two bus (code and memory). */
#define LIBMCU_ARCH_BUS_HARVARD_MOD  3
  uint8_t bus_size;
#define LIBMCU_ARCH_BUS_SIZE_UNKN    0
#define LIBMCU_ARCH_BUS_SIZE_8BIT    1
#define LIBMCU_ARCH_BUS_SIZE_16BIT   2
#define LIBMCU_ARCH_BUS_SIZE_32BIT   3
#define LIBMCU_ARCH_BUS_SIZE_64BIT   4

  /* disassembler functions */
  uint32_t (*predisasm)(libmcu_dasm_t* dasm, uint8_t* code,
      uint32_t size, void* data);
  /* disasm: */
  /* must returns size of the instruction. */
  /* must fill at least : */
  /*  - a copy of code pointer into dasm->code. */
  /*  - size of the instruction into dasm->size. */
  /* if you don't fill these fields it will explode. */
  /* So do it, you mofo. */
  uint32_t (*disasm)(libmcu_dasm_t* dasm, uint8_t* code, uint32_t size,
      uint64_t addr);
  uint32_t (*postdisasm)(libmcu_dasm_t* dasm, uint8_t* code,
      uint32_t size, void* data);

  libmcu_asm_fmt_t** asm_fmt;
  uint8_t num_asm_fmts;
  uint32_t opcode_max_size;
}libmcu_arch_t;

typedef struct libmcu_ctx
{
  libmcu_arch_t* arch;
}libmcu_ctx_t;

/* ---------------------------------------------------------------- */
#define LIBMCU_ARCH_I8051 0
#define LIBMCU_ARCH_AVR8  1

/* ---------------------------------------------------------------- */
LIBMCU_CDECL libmcu_ctx_t* libmcu_arch_create (uint8_t arch);
LIBMCU_CDECL uint8_t libmcu_arch_get_num_archs (void);
LIBMCU_CDECL libmcu_arch_t* libmcu_arch_get (uint8_t num);
LIBMCU_CDECL uint32_t libmcu_arch_disasm (libmcu_ctx_t* ctx,
    libmcu_dasm_t* dasm, uint8_t* code, uint32_t size, uint64_t addr);
LIBMCU_CDECL uint32_t libmcu_arch_disasm_ex (libmcu_ctx_t* ctx,
    libmcu_dasm_t* dasm, uint8_t* code, uint32_t size,
    uint64_t addr, void* data);
#define libmcu_arch_destroy(a) libmcu_arch_destroy_safe (&a)
LIBMCU_CDECL void libmcu_arch_destroy_safe (libmcu_ctx_t** ctx);

#endif


