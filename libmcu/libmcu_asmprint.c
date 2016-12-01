#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <libconfig.h>

#include "libmcu_arch.h"
#include "libmcu_mmap.h"
#include "libmcu_asmprint.h"
#include "libmcu_str.h"
#if defined(_WIN32)
#include "snprintf.h"
#endif

/* --------------------------------------------------------------- */
void
 libmcu_asmprint (mcu_ctx_t* ctx, mcu_dasm_t* dasm,
     mcu_asm_fmt_t* fmt, mcu_mmap_t* map, FILE* fd, uint32_t flags)
{
  uint32_t n = 0;
  uint32_t intcode = 0;
  uint32_t nchars = 0;

  if (ctx == NULL) return;
  if (dasm == NULL) return;

  intcode = libmcu_mmap_code_int (map, dasm->addr);
  if (intcode != LIBMCU_MMAP_UNMAPPED)
  {
    if (libmcu_mmap_code_int_type (map, intcode) == LIBMCU_MMAP_INT_DATA)
    {
      switch (libmcu_mmap_code_int_size (map, intcode))
      {
        case 1:
          if (fmt->data_byte != NULL)
            fprintf (fd, "%s ", fmt->data_byte);
          fprintf (fd, "0x%02X", dasm->code[0]);
          break;

        case 2:
          if (fmt->data_word != NULL)
            fprintf (fd, "%s ", fmt->data_word);
          /* TODO: endianess :( */
          for (n = 0; n < 2; n++)
            fprintf (fd, "%02X", dasm->code[n]);
          break;

        case 4:
          if (fmt->data_dword != NULL)
            fprintf (fd, "%s ", fmt->data_dword);
          for (n = 0; n < 4; n++)
            fprintf (fd, "%02X", dasm->code[n]);
          break;

        case 8:
          if (fmt->data_qword != NULL)
            fprintf (fd, "%s ", fmt->data_qword);
          for (n = 0; n < 8; n++)
            fprintf (fd, "%02X", dasm->code[n]);
          break;
      }
      return;
    }
  }

  if (flags & LIBMCU_ASM_SHOW_ADDR)
  {
    switch (ctx->arch->bus_size)
    {
      case MCU_ARCH_BUS_SIZE_8BIT:
        fprintf (fd, "0x%02X", (uint8_t)dasm->addr);
        nchars = nchars + 4;
        break;

      case MCU_ARCH_BUS_SIZE_16BIT:
        fprintf (fd, "0x%04X", (uint16_t)dasm->addr);
        nchars = nchars + 6;
        break;

      case MCU_ARCH_BUS_SIZE_32BIT:
        fprintf (fd, "0x%08X", (uint32_t)dasm->addr);
        nchars = nchars + 10;
        break;

      case MCU_ARCH_BUS_SIZE_64BIT:
        fprintf (fd, "0x%016lX", dasm->addr);
        nchars = nchars + 18;
        break;
    }
    fprintf (fd, "  ");
    nchars = nchars + 2;
  }

  if (flags & LIBMCU_ASM_SHOW_OPCODE)
  {
    for (n = 0; n < dasm->size; n++)
    {
      fprintf (fd, "%02X", dasm->code[n]);
      nchars = nchars + 2;
    }
    for (n = 0; n < (uint32_t)fmt->opcode_padding - dasm->size; n++)
    {
      fprintf (fd, "  ");
      nchars = nchars + 2;
    }
    fprintf (fd, "  ");
    nchars = nchars + 2;
  }

  if (fmt->insn_prefix != NULL)
  {
    fprintf (fd, "%s", fmt->insn_prefix);
    nchars = nchars + libmcu_str_length (fmt->insn_prefix);
  }

  fprintf (fd, "%s", ctx->arch->mnemonics[dasm->mnemonic]);
  nchars = nchars + libmcu_str_length (
      ctx->arch->mnemonics[dasm->mnemonic]
      );

  if (fmt->insn_suffix != NULL)
  {
    fprintf (fd, "%s", fmt->insn_suffix);
    nchars = nchars + libmcu_str_length (fmt->insn_suffix);
  }

  if (dasm->nopd != 0)
  {
    fprintf (fd, " ");
    nchars = nchars + 1;
  }

  for (n = 0; n < dasm->nopd; n++)
  {
    nchars = nchars + libmcu_asmprint_opd (ctx, &dasm->opd[n], fmt, map, fd);
    if ((n+1) < dasm->nopd)
    {
      fprintf (fd, ", ");
      nchars = nchars + 2;
    }
  }

  if (intcode != LIBMCU_MMAP_UNMAPPED)
  {
    if (libmcu_mmap_code_int_type (map, intcode) == LIBMCU_MMAP_INT_CODE)
    {
      if (nchars < fmt->comm_padding)
      {
        for (n = 0; n < fmt->comm_padding-nchars; n++)
          fprintf (fd, " ");
      }
      if (libmcu_mmap_code_int_type (map, intcode) == LIBMCU_MMAP_INT_CODE)
      {
        if (fmt->comm_prefix != NULL)
        {
          fprintf (fd, "%s", fmt->comm_prefix);
          nchars = nchars + libmcu_str_length (fmt->comm_prefix);
        }
        fprintf (fd, "Interruption: ");
        fprintf (fd, libmcu_mmap_code_int_name (map, intcode));
        if (fmt->comm_suffix != NULL)
        {
          fprintf (fd, "%s", fmt->comm_suffix);
          nchars = nchars + libmcu_str_length (fmt->comm_suffix);
        }
      }
    }
  }

  libmcu_asmprint_end (fd);
}

/* --------------------------------------------------------------- */
uint32_t
 libmcu_asmprint_opd (mcu_ctx_t* ctx, mcu_opd_t* opd,
     mcu_asm_fmt_t* fmt, mcu_mmap_t* map, FILE* fd)
{
  uint32_t idataregid = LIBMCU_MMAP_UNMAPPED;
  uint32_t idataregbitid = LIBMCU_MMAP_UNMAPPED;
  uint32_t nchars = 0;

  if (ctx == NULL) return 0;
  if (fmt == NULL) return 0;
  if (fd == NULL) return 0;

  /* print size prefix. */
  switch (opd->size)
  {
    case MCU_OPD_SIZE_BYTE:
      if (fmt->size_byte_prefix != NULL)
      {
        fprintf (fd, "%s", fmt->size_byte_prefix);
        nchars = nchars + libmcu_str_length (fmt->size_byte_prefix);
      }
      break;

    case MCU_OPD_SIZE_WORD:
      if (fmt->size_word_prefix != NULL)
      {
        fprintf (fd, "%s", fmt->size_word_prefix);
        nchars = nchars + libmcu_str_length (fmt->size_word_prefix);
      }
      break;

    case MCU_OPD_SIZE_DWORD:
      if (fmt->size_dword_prefix != NULL)
      {
        fprintf (fd, "%s", fmt->size_dword_prefix);
        nchars = nchars + libmcu_str_length (fmt->size_dword_prefix);
      }
      break;

    case MCU_OPD_SIZE_QWORD:
      if (fmt->size_qword_prefix != NULL)
      {
        fprintf (fd, "%s", fmt->size_qword_prefix);
        nchars = nchars + libmcu_str_length (fmt->size_qword_prefix);
      }
      break;
  }

  /* print special prefix. */
  if (opd->flags & MCU_OPD_FLAGS_DIRECT)
  {
    if (fmt->direct_prefix != NULL)
    {
      fprintf (fd, "%s", fmt->direct_prefix);
      nchars = nchars + libmcu_str_length (fmt->direct_prefix);
    }
  }
  if (opd->flags & MCU_OPD_FLAGS_INDIRECT)
  {
    if (fmt->indirect_prefix != NULL)
    {
      fprintf (fd, "%s", fmt->indirect_prefix);
      nchars = nchars + libmcu_str_length (fmt->indirect_prefix);
    }
  }
  switch (opd->type)
  {
    case MCU_OPD_TYPE_REGISTER:
      if (fmt->register_prefix != NULL && opd->flags == 0)
      {
        fprintf (fd, "%s", fmt->register_prefix);
        nchars = nchars + libmcu_str_length (fmt->register_prefix);
      }
      break;

    case MCU_OPD_TYPE_IMM:
      if (!(opd->flags & MCU_OPD_FLAGS_DIRECT))
      {
        if (fmt->immediat_prefix != NULL)
        {
          fprintf (fd, "%s", fmt->immediat_prefix);
          nchars = nchars + libmcu_str_length (fmt->immediat_prefix);
        }
      }
      break;

    case MCU_OPD_TYPE_DISPL:
      if (fmt->displ_prefix != NULL)
      {
        fprintf (fd, "%s", fmt->displ_prefix);
        nchars = nchars + libmcu_str_length (fmt->displ_prefix);
      }
      break;
  }

  /* print value. */
  switch (opd->type)
  {
    case MCU_OPD_TYPE_REGISTER:
      fprintf (fd, "%s", ctx->arch->regname[opd->value]);
      nchars = nchars + libmcu_str_length (ctx->arch->regname[opd->value]);
      break;

    case MCU_OPD_TYPE_DOUBLE_REG:
      fprintf (fd, "%s", ctx->arch->regname[opd->value&0xFF]);
      nchars = nchars + libmcu_str_length (ctx->arch->regname[opd->value&0xFF]);
      if (fmt->multi_reg != NULL)
        fprintf (fd, "%s", fmt->multi_reg);
      fprintf (fd, "%s", ctx->arch->regname[(opd->value&0xFF00)>>8]);
      break;

    case MCU_OPD_TYPE_IMM:
      if (opd->size == MCU_OPD_SIZE_BYTE)
        idataregid = libmcu_mmap_idata_reg (map, opd->value);
      else if (opd->size == MCU_OPD_SIZE_BIT)
        idataregbitid = libmcu_mmap_idata_reg_bit (map, opd->value);

      if (opd->flags & MCU_OPD_FLAGS_DIRECT &&
          opd->bus == MCU_OPD_BUS_IDATA &&
          idataregid != LIBMCU_MMAP_UNMAPPED)
      {
        fprintf (fd, "%s",
            libmcu_mmap_idata_reg_name (map, idataregid));
        nchars = nchars + libmcu_str_length (
            (char*)libmcu_mmap_idata_reg_name (map, idataregid)
            );
      }
      else if (opd->flags & MCU_OPD_FLAGS_DIRECT &&
          opd->bus == MCU_OPD_BUS_IDATA &&
          idataregbitid != LIBMCU_MMAP_UNMAPPED)
      {
        fprintf (fd, "%s",
            libmcu_mmap_idata_reg_bit_name (map, idataregbitid));
        nchars = nchars + libmcu_str_length (
            (char*)libmcu_mmap_idata_reg_bit_name (map, idataregbitid)
            );
      }
      else
      {
        if (fmt->hex_prefix != NULL)
        {
          fprintf (fd, "%s", fmt->hex_prefix);
          nchars = nchars + libmcu_str_length (fmt->hex_prefix);
        }
        switch (opd->size)
        {
          case MCU_OPD_SIZE_BYTE:
            fprintf (fd, "%02X", (uint8_t)opd->value);
            nchars = nchars + 2;
            break;

          case MCU_OPD_SIZE_WORD:
            fprintf (fd, "%04X", (uint16_t)opd->value);
            nchars = nchars + 4;
            break;

          case MCU_OPD_SIZE_DWORD:
            fprintf (fd, "%08X", (uint32_t)opd->value);
            nchars = nchars + 8;
            break;

          case MCU_OPD_SIZE_QWORD:
            fprintf (fd, "%016lX", opd->value);
            nchars = nchars + 16;
            break;
        }
        if (fmt->hex_suffix != NULL)
        {
          fprintf (fd, "%s", fmt->hex_suffix);
          nchars = nchars + libmcu_str_length (fmt->hex_suffix);
        }
      }
      break;

    case MCU_OPD_TYPE_DISPL:
      /* base. */
      switch (opd->displ.base_type)
      {
        case MCU_DISPL_TYPE_REG:
          fprintf (fd, "%s", ctx->arch->regname[opd->displ.base]);
          nchars = nchars + libmcu_str_length (
              ctx->arch->regname[opd->displ.base]
              );
          break;

        case MCU_DISPL_TYPE_IMM:
          fprintf (fd, "%08X", (uint32_t)opd->displ.base);
          nchars = nchars + 8;
          break;
      }

      /* mul. */
      if (fmt->displ_mul != NULL)
      {
        fprintf (fd, "%s", fmt->displ_mul);
        nchars = nchars + libmcu_str_length (fmt->displ_mul);
      }

      switch (opd->displ.mul_type)
      {
        case MCU_DISPL_TYPE_REG:
          fprintf (fd, "%s", ctx->arch->regname[opd->displ.mul]);
          nchars = nchars + libmcu_str_length (
              ctx->arch->regname[opd->displ.mul]
              );
          break;

        case MCU_DISPL_TYPE_IMM:
          fprintf (fd, "%08X", (uint32_t)opd->displ.mul);
          nchars = nchars + 8;
          break;
      }

      /* should check the sign. */
      /* offset. */
      if (fmt->displ_add != NULL)
      {
        fprintf (fd, "%s", fmt->displ_add);
        nchars = nchars + libmcu_str_length (fmt->displ_add);
      }

      switch (opd->displ.offset_type)
      {
        case MCU_DISPL_TYPE_REG:
          fprintf (fd, "%s", ctx->arch->regname[opd->displ.offset]);
          nchars = nchars + libmcu_str_length (
              ctx->arch->regname[opd->displ.mul]
              );
          break;

        case MCU_DISPL_TYPE_IMM:
          switch (ctx->arch->bus_size)
          {
            case MCU_ARCH_BUS_SIZE_16BIT:
              fprintf (fd, "0x%04X", (uint32_t)opd->displ.offset);
              nchars = nchars + 4 + 2;
              break;

            case MCU_ARCH_BUS_SIZE_32BIT:
              fprintf (fd, "0x%08X", (uint32_t)opd->displ.offset);
              nchars = nchars + 8 + 2;
              break;
          }
          break;
      }
      break;
  }

  /* print special suffix. */
  if (opd->flags & MCU_OPD_FLAGS_DIRECT)
  {
    if (fmt->direct_suffix != NULL)
    {
      fprintf (fd, "%s", fmt->direct_suffix);
      nchars = nchars + libmcu_str_length (fmt->direct_suffix);
    }
  }
  if (opd->flags & MCU_OPD_FLAGS_INDIRECT)
  {
    if (fmt->indirect_suffix != NULL)
    {
      fprintf (fd, "%s", fmt->indirect_suffix);
      nchars = nchars + libmcu_str_length (fmt->indirect_suffix);
    }
  }
  switch (opd->type)
  {
    case MCU_OPD_TYPE_REGISTER:
      if (fmt->register_suffix != NULL)
      {
        fprintf (fd, "%s", fmt->register_suffix);
        nchars = nchars + libmcu_str_length (fmt->register_suffix);
      }
      break;

    case MCU_OPD_TYPE_IMM:
      if (!(opd->flags & MCU_OPD_FLAGS_DIRECT))
      {
        if (fmt->immediat_suffix != NULL)
        {
          fprintf (fd, "%s", fmt->immediat_suffix);
          nchars = nchars + libmcu_str_length (fmt->immediat_suffix);
        }
      }
      break;

    case MCU_OPD_TYPE_DISPL:
      if (fmt->displ_suffix != NULL)
      {
        fprintf (fd, "%s", fmt->displ_suffix);
        nchars = nchars + libmcu_str_length (fmt->displ_suffix);
      }
      break;
  }

  /* print size suffix. */
  switch (opd->size)
  {
    case MCU_OPD_SIZE_BYTE:
      if (fmt->size_byte_suffix != NULL)
      {
        fprintf (fd, "%s", fmt->size_byte_suffix);
        nchars = nchars + libmcu_str_length (fmt->size_byte_suffix);
      }
      break;

    case MCU_OPD_SIZE_WORD:
      if (fmt->size_word_suffix != NULL)
      {
        fprintf (fd, "%s", fmt->size_word_suffix);
        nchars = nchars + libmcu_str_length (fmt->size_word_suffix);
      }
      break;

    case MCU_OPD_SIZE_DWORD:
      if (fmt->size_dword_suffix != NULL)
      {
        fprintf (fd, "%s", fmt->size_dword_suffix);
        nchars = nchars + libmcu_str_length (fmt->size_dword_suffix);
      }
      break;

    case MCU_OPD_SIZE_QWORD:
      if (fmt->size_qword_suffix != NULL)
      {
        fprintf (fd, "%s", fmt->size_qword_suffix);
        nchars = nchars + libmcu_str_length (fmt->size_qword_suffix);
      }
      break;
  }
  return nchars;
}

/* --------------------------------------------------------------- */
uint32_t
 libmcu_asmprint_data (FILE* fd, mcu_asm_fmt_t* fmt, uint8_t* data,
     uint32_t size)
{
  uint32_t nchars = 0;
  uint8_t byte = 0;
  uint16_t word = 0;
  uint32_t dword = 0;
  uint64_t qword = 0;

  if (fd == NULL) return 0;
  if (data == NULL) return 0;
  if (fmt == NULL) return 0;

  switch (size)
  {
    case 0x01:
      byte = *data;
      if (fmt->data_byte != NULL)
      {
        fprintf (fd, "%s ", fmt->data_byte);
        nchars = nchars + libmcu_str_length (fmt->data_byte);
      }
      if (fmt->hex_prefix != NULL)
      {
        fprintf (fd, "%s", fmt->hex_prefix);
        nchars = nchars + libmcu_str_length (fmt->hex_prefix);
      }
      fprintf (fd, "%02X", byte);
      nchars = nchars + 2;
      if (fmt->hex_suffix != NULL)
      {
        fprintf (fd, "%s", fmt->hex_suffix);
        nchars = nchars + libmcu_str_length (fmt->hex_suffix);
      }
      break;

    case 0x02:
      word = data[0] | ((uint16_t)data[1]<<8);
      if (fmt->data_word != NULL)
      {
        fprintf (fd, "%s ", fmt->data_word);
        nchars = nchars + libmcu_str_length (fmt->data_word);
      }
      if (fmt->hex_prefix != NULL)
      {
        fprintf (fd, "%s", fmt->hex_prefix);
        nchars = nchars + libmcu_str_length (fmt->hex_prefix);
      }
      fprintf (fd, "%04X", word);
      nchars = nchars + 4;
      if (fmt->hex_suffix != NULL)
      {
        fprintf (fd, "%s", fmt->hex_suffix);
        nchars = nchars + libmcu_str_length (fmt->hex_suffix);
      }
      break;

    case 0x04:
      dword = data[0] | ((uint32_t)data[1]<<8) |
        ((uint32_t)data[2]<<16) | ((uint32_t)data[3]<<24);
      if (fmt->data_dword != NULL)
      {
        fprintf (fd, "%s ", fmt->data_dword);
        nchars = nchars + libmcu_str_length (fmt->data_dword);
      }
      if (fmt->hex_prefix != NULL)
      {
        fprintf (fd, "%s", fmt->hex_prefix);
        nchars = nchars + libmcu_str_length (fmt->hex_prefix);
      }
      fprintf (fd, "%08X", dword);
      nchars = nchars + 8;
      if (fmt->hex_suffix != NULL)
      {
        fprintf (fd, "%s", fmt->hex_suffix);
        nchars = nchars + libmcu_str_length (fmt->hex_suffix);
      }
      break;

    case 0x08:
      qword = data[0] | ((uint64_t)data[1]<<8) |
        ((uint64_t)data[2]<<16) | ((uint64_t)data[3]<<24) |
        ((uint64_t)data[4]<<32) | ((uint64_t)data[5]<<40) |
        ((uint64_t)data[6]<<48) | ((uint64_t)data[8]<<56);
      if (fmt->data_qword != NULL)
      {
        fprintf (fd, "%s ", fmt->data_qword);
        nchars = nchars + libmcu_str_length (fmt->data_qword);
      }
      if (fmt->hex_prefix != NULL)
      {
        fprintf (fd, "%s", fmt->hex_prefix);
        nchars = nchars + libmcu_str_length (fmt->hex_prefix);
      }
      fprintf (fd, "%016lX", qword);
      nchars = nchars + 16;
      if (fmt->hex_suffix != NULL)
      {
        fprintf (fd, "%s", fmt->hex_suffix);
        nchars = nchars + libmcu_str_length (fmt->hex_suffix);
      }
      break;
  }
  return nchars;
}

/* --------------------------------------------------------------- */
void
 libmcu_asmprint_end (FILE* fd)
{
  fprintf (fd, "\n");
}

