#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <libconfig.h>

/* --------------------------------------------------------------- */
#include <libmcu_arch.h>
#include <libmcu_mmap.h>
#include <libmcu_asmprint.h>

#include "udasm.h"
#include "file.h"

/* --------------------------------------------------------------- */
char* g_udasm_appname = NULL;

/* --------------------------------------------------------------- */
int
 main (int argc, char* const argv[])
{
  struct option udasm_options[] = 
  {
    { "help",          0, 0, 'h' },
    { "list-mcu",      0, 0, 'l' },
    { "version",       0, 0, 'v' },
    { "asm",           1, 0, 'a' },
    { "format",        1, 0, 'f' },
    { "mcu",           1, 0, 'm' },
    { "memory-map",    1, 0, 'e' },
    { "start-address", 1, 0, 's' },
    { NULL,            0, 0,  0  }
  };
  int curr_option = 0;
  int opid = 0;
  uint8_t arch_sel = 0;
  char* arch_name = NULL, *fmt_name = NULL;
  char* memmap_name = NULL;
  uint32_t n = 0, m = 0, i = 0;
  uint8_t arch_found = 0, fmt_found = 0;
  mcu_ctx_t* mcu_ctx = NULL;
  mcu_arch_t* mcu_arch = NULL;
  mcu_asm_fmt_t* mcu_fmt = NULL;
  mcu_mmap_t* mcu_mmap = NULL;
  mcu_dasm_t dasm;
  uint32_t sz = 0;
  uint8_t* code = NULL;
  uint32_t offset = 0;
  uint64_t start_address = 0;
  udasm_file_t* infile = NULL;

  if (argc > 256) return 0;

  while ((curr_option=getopt_long (argc, argv, 
          "hlva:f:m:e:s:", udasm_options, &opid)) != -1)
  {
    switch (curr_option)
    {
      case 'm':
        arch_name = optarg;
        break;

      case 'a':
        fmt_name = optarg;
        break;

      case 'e':
        memmap_name = optarg;
        break;

      case 'l':
        inf ("List of supported architectures.\n");
        inf ("Name                 Assembly syntax\n");
        for (n = 0; n < libmcu_arch_get_num_archs (); n++)
        {
          mcu_arch = libmcu_arch_get (n);
          inf ("%-20s ", mcu_arch->shortname);
          for (m = 0; m < mcu_arch->num_asm_fmts; m++)
          {
            inf ("%s", mcu_arch->asm_fmt[m]->fmt_shortname);
            if ((m+1) != mcu_arch->num_asm_fmts)
              inf (", ");
          }
          inf ("\n");
        }
        return 0;

      case 'h':
        fprintf (stdout, UDASM_USAGE);
        return 0;

      case 'v':
        fprintf (stdout, "%s\n", UDASM_VERSION);
        return 0;

      case 's':
        start_address = strtol (optarg, NULL, 16);
        break;

      default:
        return 0;
    }
  }

  if (strlen(argv[0]) > 2048)
  {
    fprintf (stderr, "what a hell.\n");
    return 0;
  }

  g_udasm_appname = calloc (strlen(argv[0]) + 1, sizeof(char));
  if (g_udasm_appname == NULL)
  {
    fprintf (stderr, "cannot allocate '%zu' byte(s).\n",
        strlen(argv[0]) + 1);
    goto g_exit;
  }
  memcpy (g_udasm_appname, argv[0], strlen(argv[0]));

  if (arch_name == NULL)
    goto g_exit;

  for (n = 0; n < libmcu_arch_get_num_archs (); n++)
  {
    mcu_arch = libmcu_arch_get (n);
    if (strcmp (mcu_arch->shortname, arch_name) == 0)
    {
      arch_sel = n;
      arch_found = 1;
      break;
    }
  }

  if (arch_found == 0)
  {
    err ("cannot find an architecture called '%s'.\n",
        arch_name);
    goto g_exit;
  }

  mcu_ctx = libmcu_arch_create (arch_sel);
  if (mcu_ctx == NULL)
  {
    err ("cannot allocate %zu byte(s).\n",
        sizeof (mcu_ctx_t));
    goto g_exit;
  }

  if (fmt_name == NULL)
  {
    fmt_found = 1;
    mcu_fmt = mcu_ctx->arch->asm_fmt[0];
  }
  else
  {
    for (n = 0; n < mcu_ctx->arch->num_asm_fmts; n++)
    {
      if (!strcmp (mcu_ctx->arch->asm_fmt[n]->fmt_shortname, 
            fmt_name))
      {
        mcu_fmt = mcu_ctx->arch->asm_fmt[n];
        fmt_found = 1;
        break;
      }
    }
    if (fmt_found == 0)
    {
      err ("cannot found asm syntax name `%s'.\n",
          fmt_name);
      goto g_exit;
    }
  }

  if (memmap_name != NULL)
  {
    mcu_mmap = libmcu_mmap_create (memmap_name);
    if (mcu_mmap == NULL)
    {
      err ("cannot read memory map configuration file `%s'.\n",
          memmap_name);
      goto g_exit;
    }
    if (mcu_mmap->error.shithappened)
    {
      err ("%s:%d: %s\n", memmap_name, mcu_mmap->error.line,
          mcu_mmap->error.msg);
      goto g_exit;
    }
    if (libmcu_mmap_check_syntax (mcu_ctx, mcu_mmap) == 0)
    {
      err ("%s:%d: %s\n", memmap_name, mcu_mmap->error.line,
          mcu_mmap->error.msg);
      goto g_exit;
    }
  }

  for (n = optind; n < (uint32_t)argc; n++)
  {
    infile = udasm_file_open (argv[n], 0);
    inf ("begin of disassembly: %s\n", argv[n]);
    if (infile == NULL) continue;
    if (udasm_file_read (infile, start_address) == 0)
    {
      udasm_file_destroy (infile);
      continue;
    }
    for (m = 0; m < infile->nblock; m++)
    {
      offset = 0;
      while (offset < infile->blocks[m]->size)
      {
        /* todo: check int. type. */
        /* disasm. */
        code = calloc (mcu_ctx->arch->opcode_max_size,
            sizeof(uint8_t));
        for (i = 0; i < mcu_ctx->arch->opcode_max_size; i++)
        {
          if (offset + i < infile->blocks[m]->size)
            code[i] = infile->blocks[m]->data[offset+i];
        }

        memset (&dasm, 0, sizeof (dasm));

        sz = libmcu_arch_disasm (mcu_ctx, &dasm, code, 
            mcu_ctx->arch->opcode_max_size,
            infile->blocks[m]->start + offset);

        libmcu_asmprint (mcu_ctx, &dasm, mcu_fmt, mcu_mmap, stdout,
            LIBMCU_ASM_SHOW_OPCODE | LIBMCU_ASM_SHOW_ADDR);

        offset = offset + sz;
        free (code);
        code = NULL;
      }
    }
    inf ("end of disassembly: %s\n", argv[n]);
    udasm_file_destroy (infile);
  }


g_exit:
  libmcu_arch_destroy (mcu_ctx);
  libmcu_mmap_destroy (mcu_mmap);
  if (g_udasm_appname != NULL) free (g_udasm_appname);
  g_udasm_appname = NULL;
  return 0;
}


