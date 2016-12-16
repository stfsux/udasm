
#include <ida.hpp>
#include <idp.hpp>
#include <kernwin.hpp>
#include <stdint.h>

/* libmcu */
#include <libmcu.h>
#include "gui.hpp"

#define MCU8051_IREG_HEADER 2
static const char* global_header[] = 
{
  "Name", "Address", NULL
};
static const int global_width[] = 
{
  CHCOL_PLAIN|25, CHCOL_HEX|4
};

/* --------------------------------------------------------------- */
static bool idaapi
 mcu8051_gui_menu_ireg (void* data)
{
  libmcu_mmap_t* map = NULL;

  if (data == NULL) return true;

  map = (libmcu_mmap_t*)data;

  mcu8051_gui_ireg_create (map);
  return true;
}

/* --------------------------------------------------------------- */
static bool idaapi
 mcu8051_gui_menu_ireg_bit (void* data)
{
  libmcu_mmap_t* map = NULL;

  if (data == NULL) return true;

  map = (libmcu_mmap_t*)data;

  mcu8051_gui_ireg_bit_create (map);
  return true;
}

/* --------------------------------------------------------------- */
static bool idaapi
 mcu8051_gui_menu_int (void* data)
{
  libmcu_mmap_t* map = NULL;

  if (data == NULL) return true;

  map = (libmcu_mmap_t*)data;

  mcu8051_gui_int_create (map);
  return true;
}

/* --------------------------------------------------------------- */
void
 mcu8051_gui_menu_create (libmcu_mmap_t* mmap)
{
  add_menu_item("View/Open subviews/Strings",
          "Internal registers (libmcu)", "",
          SETMENU_INS|SETMENU_CTXVIEW,
          mcu8051_gui_menu_ireg, mmap);
  add_menu_item("View/Open subviews/Strings",
          "Internal bit registers (libmcu)", "",
          SETMENU_INS|SETMENU_CTXVIEW,
          mcu8051_gui_menu_ireg_bit, mmap);
  add_menu_item("View/Open subviews/Strings",
          "List of interruptions (libmcu)", "",
          SETMENU_INS|SETMENU_CTXVIEW,
          mcu8051_gui_menu_int, mmap);
}

/* --------------------------------------------------------------- */
void
 mcu8051_gui_menu_destroy (void)
{
  del_menu_item ("View/Open subviews/Internal registers (libmcu)");
  del_menu_item ("View/Open subviews/Internal bit registers"
      "(libmcu)");
  del_menu_item ("View/Open subviews/List of interruptions"
     " (libmcu)");
}

/* --------------------------------------------------------------- */
static uint32_t idaapi
 mcu8051_gui_ireg_num (void* obj)
{
  libmcu_mmap_t* map = NULL;

  if (obj == NULL) return 0;

  map = (libmcu_mmap_t*)obj;

  return map->nidata_registers;
}

/* --------------------------------------------------------------- */
static uint32_t idaapi
 mcu8051_gui_ireg_bit_num (void* obj)
{
  libmcu_mmap_t* map = NULL;

  if (obj == NULL) return 0;

  map = (libmcu_mmap_t*)obj;

  return map->nidata_registers_bit;
}

/* --------------------------------------------------------------- */
static uint32_t idaapi
 mcu8051_gui_int_num (void* obj)
{
  libmcu_mmap_t* map = NULL;

  if (obj == NULL) return 0;

  map = (libmcu_mmap_t*)obj;

  return map->ncode_int;
}

/* --------------------------------------------------------------- */
static void idaapi
 mcu8051_gui_ireg_create_line (void* obj, uint32_t n,
     char* const* arr)
{
  libmcu_mmap_t* map = NULL;
  uint32_t m = 0;
  const char* regname = NULL;
  uint32_t addr = 0;

  if (obj == NULL) return;

  map = (libmcu_mmap_t*)obj;

  if (n == 0)
  {
    for (m = 0; m < MCU8051_IREG_HEADER; m++)
      qstrncpy (arr[m], global_header[m], MAXSTR);
    return;
  }

  regname = libmcu_mmap_idata_reg_name (map, n-1);
  addr = libmcu_mmap_idata_reg_addr (map, n-1);
  qstrncpy (arr[0], regname, MAXSTR);
  qsnprintf (arr[1], MAXSTR, "0x%04X", addr);
}

/* --------------------------------------------------------------- */
static void idaapi
 mcu8051_gui_ireg_bit_create_line (void* obj, uint32_t n,
     char* const* arr)
{
  libmcu_mmap_t* map = NULL;
  uint32_t m = 0;
  const char* regname = NULL;
  uint32_t addr = 0;

  if (obj == NULL) return;

  map = (libmcu_mmap_t*)obj;

  if (n == 0)
  {
    for (m = 0; m < MCU8051_IREG_HEADER; m++)
      qstrncpy (arr[m], global_header[m], MAXSTR);
    return;
  }

  regname = libmcu_mmap_idata_reg_bit_name (map, n-1);
  addr = libmcu_mmap_idata_reg_bit_addr (map, n-1);
  qstrncpy (arr[0], regname, MAXSTR);
  qsnprintf (arr[1], MAXSTR, "0x%04X", addr);
}

/* --------------------------------------------------------------- */
static void idaapi
 mcu8051_gui_int_create_line (void* obj, uint32_t n,
     char* const* arr)
{
  libmcu_mmap_t* map = NULL;
  uint32_t m = 0;
  const char* regname = NULL;
  uint32_t addr = 0;

  if (obj == NULL) return;

  map = (libmcu_mmap_t*)obj;

  if (n == 0)
  {
    for (m = 0; m < MCU8051_IREG_HEADER; m++)
      qstrncpy (arr[m], global_header[m], MAXSTR);
    return;
  }

  regname = libmcu_mmap_code_int_name (map, n-1);
  addr = libmcu_mmap_code_int_addr (map, n-1);
  qstrncpy (arr[0], regname, MAXSTR);
  qsnprintf (arr[1], MAXSTR, "0x%04X", addr);
}

/* --------------------------------------------------------------- */
void
 mcu8051_gui_ireg_create (libmcu_mmap_t* map)
{
  if (map == NULL) return;

  choose2 (false, -1, -1, -1, -1, map,
      MCU8051_IREG_HEADER,
      global_width, mcu8051_gui_ireg_num,
      mcu8051_gui_ireg_create_line,
      "Internal registers", -1, 0, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL
      );
}

/* --------------------------------------------------------------- */
void
 mcu8051_gui_ireg_bit_create (libmcu_mmap_t* map)
{
  if (map == NULL) return;

  choose2 (false, -1, -1, -1, -1, map,
      MCU8051_IREG_HEADER,
      global_width, mcu8051_gui_ireg_bit_num,
      mcu8051_gui_ireg_bit_create_line,
      "Internal bit registers", -1, 0, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL
      );
}

/* --------------------------------------------------------------- */
void
 mcu8051_gui_int_create (libmcu_mmap_t* map)
{
  if (map == NULL) return;

  choose2 (false, -1, -1, -1, -1, map,
      MCU8051_IREG_HEADER,
      global_width, mcu8051_gui_int_num,
      mcu8051_gui_int_create_line,
      "Interruptions", -1, 0, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL
      );
}

