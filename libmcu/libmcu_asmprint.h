#ifndef __ASMPRINT_H__
#define __ASMPRINT_H__

#define LIBMCU_ASM_SHOW_ADDR    (1<<0)
#define LIBMCU_ASM_SHOW_OPCODE  (1<<1)

/* --------------------------------------------------------------- */
void libmcu_asmprint (mcu_ctx_t* ctx, mcu_dasm_t* dasm,
    mcu_asm_fmt_t* fmt, mcu_mmap_t* map, FILE* fd, uint32_t flags);
uint32_t libmcu_asmprint_opd (mcu_ctx_t* ctx, mcu_opd_t* opd,
    mcu_asm_fmt_t* fmt, mcu_mmap_t* map, FILE* fd);
void libmcu_asmprint_comm (mcu_asm_fmt_t* fmt, FILE* fd,
    char* comm);
void libmcu_asmprint_end (FILE* fd);

#endif
