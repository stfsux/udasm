#ifndef __ASMPRINT_H__
#define __ASMPRINT_H__

#define LIBMCU_ASM_SHOW_ADDR    (1<<0)
#define LIBMCU_ASM_SHOW_OPCODE  (1<<1)

/* --------------------------------------------------------------- */
LIBMCU_CDECL void libmcu_asmprint (libmcu_ctx_t* ctx,
    libmcu_dasm_t* dasm, libmcu_asm_fmt_t* fmt, libmcu_mmap_t* map,
    FILE* fd, uint32_t flags);
LIBMCU_CDECL uint32_t libmcu_asmprint_opd (libmcu_ctx_t* ctx,
    libmcu_opd_t* opd, libmcu_asm_fmt_t* fmt, libmcu_mmap_t* map,
    FILE* fd);
LIBMCU_CDECL void libmcu_asmprint_comm (libmcu_asm_fmt_t* fmt,
    FILE* fd, char* comm);
LIBMCU_CDECL void libmcu_asmprint_end (FILE* fd);

#endif
