
#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <stdint.h>

#include <libconfig.h>
#include <libmcu_arch.h>
#include <libmcu_mmap.h>
#include <arch/i8051.h>

#include "ana.hpp"
#include "ins.hpp"
#include "reg.hpp"
#include "out.hpp"
#include "emu.hpp"
#include "mcu8051.hpp"

char* g_libmcu_cfg = NULL;
mcu_mmap_t* g_libmcu_mmap = NULL;
mcu_ctx_t* g_libmcu_ctx = NULL;

static int idaapi
 mcu8051_notify (processor_t::idp_notify msgid, ...)
{
  va_list va;
  int code = 0;
  int retask = 0;
  
  va_start (va, msgid);
  code = invoke_callbacks (HT_IDP, msgid, va);
  if (code) return code;

  switch (msgid)
  {
    /*
    case processor_t::init:
    case processor_t::newfile:
    */
    case processor_t::newbinary:
      ida_libmcu_destroy ();
      g_libmcu_ctx = libmcu_arch_create (LIBMCU_ARCH_I8051);
      retask = askbuttons_c (
          "Load",
          "No",
          NULL,
          ASKBTN_YES,
          "TITLE Load a memory map?\n"
          "ICON QUESTION\n"
          "AUTOHIDE NONE\n"
          "HIDECANCEL\n"
          "Do you want to load a memory map "
          "configuration?\n");
      if (retask == ASKBTN_YES)
      {
        char* tmp = askfile2_c (
            0, NULL, "Memory map config|*.cfg",
            "Load new memory map configuration file");
        if (tmp != NULL)
        {
          g_libmcu_cfg = (char*)calloc (strlen(tmp)+1, sizeof(char));
          if (g_libmcu_cfg != NULL)
          {
            memcpy (g_libmcu_cfg, tmp, strlen(tmp));
            g_libmcu_mmap = libmcu_mmap_create (g_libmcu_cfg);
            if (libmcu_mmap_check_syntax (g_libmcu_ctx,
                  g_libmcu_mmap) == 0)
            {
              warning ("%s:%d %s\n", g_libmcu_cfg, g_libmcu_mmap->error.line,
                  g_libmcu_mmap->error.msg);
            }
            else
              msg ("Memory map %s loaded.\n", g_libmcu_cfg);
          }
        }
      }
      break;

    case processor_t::term:
      ida_libmcu_destroy ();
      break;
  }
  va_end (va);

  return 1;
}

void idaapi
 mcu8051_header (void)
{
}

void idaapi
 mcu8051_footer (void)
{
}

void idaapi
 mcu8051_segstart (ea_t ea)
{
}

void idaapi
 mcu8051_segend (ea_t ea)
{
}

void
 ida_libmcu_destroy (void)
{
  if (g_libmcu_cfg != NULL)
  {
    free (g_libmcu_cfg);
    g_libmcu_cfg = NULL;
  }
  if (g_libmcu_mmap != NULL)
    libmcu_mmap_destroy_safe (&g_libmcu_mmap);
  if (g_libmcu_ctx != NULL)
    libmcu_arch_destroy_safe (&g_libmcu_ctx);
}

static const asm_t mcu8051_temp_asm =
{
  AS_COLON | ASH_HEXF3,
  0,
  "SHIT asm",
  0,
  NULL,
  NULL,
  ".org",
  ".end",

  ";",          // comment string
  '"',          // string delimiter
  '\'',         // char delimiter
  "\\\"'",      // special symbols in char and string constants

  ".db",        // ascii string directive
  ".db",        // byte directive
  ".drw",       // word directive
  NULL,         // dword  (4 bytes)
  NULL,         // qword  (8 bytes)
  NULL,         // oword  (16 bytes)
  NULL,         // float  (4 bytes)
  NULL,         // double (8 bytes)
  NULL,         // tbyte  (10/12 bytes)
  NULL,         // packed decimal real
  NULL,         // arrays (#h,#d,#v,#s(...)
  ".rs %s",     // uninited arrays
  NULL,         // equ
  NULL,         // seg prefix
  NULL,         // checkarg_preline
  NULL,         // checkarg_atomprefix
  NULL,         // checkarg_operations
  NULL,         // XlatAsciiOutput
  NULL,         // curip
  NULL,         // func_header
  NULL,         // func_footer
  NULL,         // public
  NULL,         // weak
  NULL,         // extrn
  NULL,         // comm
  NULL,         // get_type_name
  NULL,         // align
  ' ', ' ',     // lbrace, rbrace
  NULL,    // mod
  NULL,    // and
  NULL,    // or
  NULL,    // xor
  NULL,    // not
  NULL,    // shl
  NULL,    // shr
  NULL,    // sizeof
};


char* mcu8051_psnames[] = 
{
  "mcu8051",
  NULL
};

char* mcu8051_plnames[] = 
{
  "Intel i8051 microcontroller (libmcu)",
  NULL
};

const asm_t* const mcu8051_asm_fmt[] = 
{
  &mcu8051_temp_asm,
  NULL
};

static uint8_t retcode_1[] = { 0x22 };
static uint8_t retcode_2[] = { 0x32 };

static bytes_t mcu8051_retcodes[] =
{
  { sizeof(retcode_1), retcode_1 },
  { sizeof(retcode_2), retcode_2 },
  { 0, NULL }
};

extern "C" __declspec(dllexport) processor_t
 LPH =
{
  IDP_INTERFACE_VERSION, /* version */
  0x66600000,            /* id      */
  PRN_HEX | PR_BINMEM,   /* flags   */
                         /*  - default hex      */
                         /*  - no ROM/RAM setup */
  8,                     /* cnbits */
  8,                     /* dnbits */
  mcu8051_psnames,       /* psnames */
  mcu8051_plnames,       /* plnames */
  mcu8051_asm_fmt,       /* assemblers */
  mcu8051_notify,        /* notify() */
  mcu8051_header,        /* header() */
  mcu8051_footer,        /* footer() */
  mcu8051_segstart,      /* segstart() */
  mcu8051_segend,        /* segend() */
  NULL,                  /* assumes() */
  mcu8051_ana,           /* u_ana() */
  mcu8051_emu,           /* u_emu() */
  mcu8051_out,           /* u_out() */
  mcu8051_outop,         /* u_outop() */
  intel_data,            /* d_out() */
  NULL,                  /* cmp_opnd() */
  NULL,                  /* can_have_type() */
  i8051_arch.nregs+2,    /* regNum */
  mcu8051_regname,       /* regNames */
  NULL,                  /* getreg() */
  0,                     /* rFiles */
  NULL,                  /* rFnames */
  NULL,                  /* rFdescs */
  NULL,                  /* CPUregs */
  I8051_REG_VCS,         /* regFirstSreg */
  I8051_REG_VDS,         /* regLastSreg */
  0,                     /* segreg_size */
  I8051_REG_VCS,         /* regCodeSreg */
  I8051_REG_VDS,         /* regDataSreg */
  NULL,                  /* codestart */
  mcu8051_retcodes,      /* retcodes */
  I8051_INSN_ACALL,      /* instruc_start */
  I8051_INSN_UNKN,       /* instruc_end   */
  mcu8051_instructions,  /* instruc */
  NULL,                  /* is_far_jump() */
  NULL,                  /* translate() */
  0,                     /* tbyte_size */
  NULL,                  /* realcvt() */
  { 0, 0, 0, 0 },        /* real_width[4] */
  NULL,                  /* is_switch() */
  NULL,                  /* gen_map_file() */
  NULL,                  /* extract_address() */
  NULL,                  /* is_sp_based() */
  NULL,                  /* create_func_frame() */
  NULL,                  /* get_frame_retsize() */
  NULL,                  /* gen_stkvar_def() */
  gen_spcdef,            /* u_outspec() */
  I8051_INSN_RET,        /* icode_return */
  NULL,                  /* set_idp_options */
  NULL,                  /* is_align_insn() */
  NULL,                  /* mvm */
  0                      /* high_fixup_bits */
};

