
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "libmcu.h"

/* --------------------------------------------------------------- */
libmcu_mmap_t*
 libmcu_mmap_create (char* filename)
{
  libmcu_mmap_t* map = NULL;

  if (filename == NULL) return NULL;

  map = calloc (1, sizeof(libmcu_mmap_t));
  if (map == NULL) return NULL;

  memset (&map->error, 0, sizeof(map->error));

  config_init (&map->cfg);

  if (!config_read_file (&map->cfg, filename))
  {
    map->error.shithappened = 1;
    map->error.line = config_error_line (&map->cfg);
    snprintf (map->error.msg, sizeof(map->error.msg) - 1,
        "%s", config_error_text (&map->cfg));
  }

  return map;
}

/* --------------------------------------------------------------- */
uint8_t
 libmcu_mmap_check_syntax (libmcu_ctx_t* ctx, libmcu_mmap_t* map)
{
  uint32_t n = 0;
  config_setting_t* s = NULL;
  const char* str = NULL;

  if (ctx == NULL) return 0;
  if (map == NULL) return 0;
  
  map->idata_registers = config_lookup (&map->cfg,
      "IDATA.registers");
  map->idata_registers_bit = config_lookup (&map->cfg,
      "IDATA.registers_bit");
  map->xdata_registers = config_lookup (&map->cfg,
      "XDATA.registers");
  map->xdata_registers_bit = config_lookup (&map->cfg,
      "XDATA.registers_bit");
  map->registers = config_lookup (&map->cfg,
      "DATA.registers");
  map->registers_bit = config_lookup (&map->cfg,
      "DATA.registers");
  map->code_int = config_lookup (&map->cfg,
      "CODE.interrupts");

  if (map->idata_registers != NULL)
    map->nidata_registers = config_setting_length (
        map->idata_registers
        );

  if (map->idata_registers_bit != NULL)
    map->nidata_registers_bit = config_setting_length (
        map->idata_registers_bit
        );

  if (map->xdata_registers != NULL)
    map->nxdata_registers = config_setting_length (
        map->xdata_registers
        );

  if (map->xdata_registers_bit != NULL)
    map->nxdata_registers_bit = config_setting_length (
        map->xdata_registers_bit
        );

  if (map->registers != NULL)
    map->nregisters = config_setting_length (
        map->registers
        );

  if (map->registers_bit != NULL)
    map->nregisters_bit = config_setting_length (
        map->registers_bit
        );

  if (map->code_int != NULL)
    map->ncode_int = config_setting_length (
        map->code_int
        );

  for (n = 0; n < map->nidata_registers; n++)
  {
    s = config_setting_get_elem (map->idata_registers,
        n);
    if (config_setting_get_string_elem (s, 0) == NULL)
    {
      map->error.shithappened = 1;
      map->error.line = config_setting_source_line (s);
      snprintf (map->error.msg, sizeof(map->error.msg) - 1,
          "First item is not a valid string");
      return 0;
    }
  }

  for (n = 0; n < map->nidata_registers_bit; n++)
  {
    s = config_setting_get_elem (map->idata_registers_bit, n);
    if (config_setting_get_string_elem (s, 0) == NULL)
    {
      map->error.shithappened = 1;
      map->error.line = config_setting_source_line (s);
      snprintf (map->error.msg, sizeof(map->error.msg) - 1,
          "First item is not a valid string");
      return 0;
    }
  }

  for (n = 0; n < map->nxdata_registers; n++)
  {
    s = config_setting_get_elem (map->xdata_registers, n);
    if (config_setting_get_string_elem (s, 0) == NULL)
    {
      map->error.shithappened = 1;
      map->error.line = config_setting_source_line (s);
      snprintf (map->error.msg, sizeof(map->error.msg) - 1,
          "First item is not a valid string");
      return 0;
    }
  }

  for (n = 0; n < map->nxdata_registers_bit; n++)
  {
    s = config_setting_get_elem (map->xdata_registers_bit, n);
    if (config_setting_get_string_elem (s, 0) == NULL)
    {
      map->error.shithappened = 1;
      map->error.line = config_setting_source_line (s);
      snprintf (map->error.msg, sizeof(map->error.msg) - 1,
          "First item is not a valid string");
      return 0;
    }
  }

  for (n = 0; n < map->nregisters; n++)
  {
    s = config_setting_get_elem (map->registers, n);
    if (config_setting_get_string_elem (s, 0) == NULL)
    {
      map->error.shithappened = 1;
      map->error.line = config_setting_source_line (s);
      snprintf (map->error.msg, sizeof(map->error.msg) - 1,
          "First item is not a valid string");
      return 0;
    }
  }

  for (n = 0; n < map->nregisters_bit; n++)
  {
    s = config_setting_get_elem (map->registers_bit, n);
    if (config_setting_get_string_elem (s, 0) == NULL)
    {
      map->error.shithappened = 1;
      map->error.line = config_setting_source_line (s);
      snprintf (map->error.msg, sizeof(map->error.msg) - 1,
          "First item is not a valid string");
      return 0;
    }
  }

  for (n = 0; n < map->ncode_int; n++)
  {
    s = config_setting_get_elem (map->code_int, n);
    if (config_setting_length (s) != 4)
    {
      map->error.shithappened = 1;
      map->error.line = config_setting_source_line (s);
      snprintf (map->error.msg, sizeof(map->error.msg) - 1,
          "Interrupts memory map configuration has wrong "
          "number of elements");
      return 0;
    }
    if (config_setting_get_string_elem (s, 0) == NULL)
    {
      map->error.shithappened = 1;
      map->error.line = config_setting_source_line (s);
      snprintf (map->error.msg, sizeof(map->error.msg) - 1,
          "First item is not a valid string");
      return 0;
    }
    if (config_setting_get_string_elem (s, 3) == NULL)
    {
      map->error.shithappened = 1;
      map->error.line = config_setting_source_line (s);
      snprintf (map->error.msg, sizeof(map->error.msg) - 1,
          "Interrupt type is not a valid string");
      return 0;
    }
    str = config_setting_get_string_elem (s, 3);
    if (strcmp (str, "code") != 0 &&
        strcmp (str, "data") != 0)
    {
      map->error.shithappened = 1;
      map->error.line = config_setting_source_line (s);
      snprintf (map->error.msg, sizeof(map->error.msg) - 1,
          "'%s' is not a valid type of interrupt",
          str);
      return 0;
    }
    if ((uint32_t)config_setting_get_int_elem (s, 2) >=
        (uint32_t)ctx->arch->opcode_max_size)
    {
      map->error.shithappened = 1;
      map->error.line = config_setting_source_line (s);
      snprintf (map->error.msg, sizeof(map->error.msg) - 1,
          "Size of interrupt must be less than "
          "opcode_max_size field");
      return 0;
    }
  }

  return 1;
}

/* --------------------------------------------------------------- */
uint32_t
 libmcu_mmap_idata_reg (libmcu_mmap_t* map, uint64_t addr)
{
  uint32_t n = 0;
  config_setting_t* s = NULL;

  if (map == NULL) return LIBMCU_MMAP_UNMAPPED;

  for (n = 0; n < map->nidata_registers; n++)
  {
    s = config_setting_get_elem (map->idata_registers, n);
    if (addr == (uint64_t)config_setting_get_int_elem (s, 1))
      return n;
  }

  return LIBMCU_MMAP_UNMAPPED;
}

/* --------------------------------------------------------------- */
uint32_t
 libmcu_mmap_idata_reg_bit (libmcu_mmap_t* map, uint64_t addr)
{
  uint32_t n = 0;
  config_setting_t* s = NULL;

  if (map == NULL) return LIBMCU_MMAP_UNMAPPED;

  for (n = 0; n < map->nidata_registers_bit; n++)
  {
    s = config_setting_get_elem (map->idata_registers_bit, n);
    if (addr == (uint64_t)config_setting_get_int_elem (s, 1))
      return n;
  }

  return LIBMCU_MMAP_UNMAPPED;
}

/* --------------------------------------------------------------- */
uint32_t
 libmcu_mmap_code_int (libmcu_mmap_t* map, uint64_t addr)
{
  uint32_t n = 0;
  config_setting_t* s = NULL;

  if (map == NULL) return LIBMCU_MMAP_UNMAPPED;

  for (n = 0; n < map->ncode_int; n++)
  {
    s = config_setting_get_elem (map->code_int, n);
    if (addr == (uint64_t)config_setting_get_int_elem (s, 1))
      return n;
  }
  return LIBMCU_MMAP_UNMAPPED;
}

/* --------------------------------------------------------------- */
uint64_t
 libmcu_mmap_code_int_size (libmcu_mmap_t* map, uint32_t n)
{
  (void)map;
  (void)n;
  return 0;
}


/* --------------------------------------------------------------- */
uint8_t
 libmcu_mmap_code_int_type (libmcu_mmap_t* map, uint32_t n)
{
  config_setting_t* s = NULL;
  const char* str = NULL;

  if (map == NULL) return LIBMCU_MMAP_INT_NONE;

  s = config_setting_get_elem (map->code_int, n);
  str = config_setting_get_string_elem (s, 3);

  if (strcmp (str, "code") == 0)
    return LIBMCU_MMAP_INT_CODE;
  else if (strcmp (str, "data") == 0)
    return LIBMCU_MMAP_INT_DATA;

  return LIBMCU_MMAP_INT_NONE;
}

/* --------------------------------------------------------------- */
const char*
 libmcu_mmap_idata_reg_name (libmcu_mmap_t* map, uint32_t n)
{
  config_setting_t* s = NULL;

  s = config_setting_get_elem (map->idata_registers, n);
  return config_setting_get_string_elem (s, 0);
}

/* --------------------------------------------------------------- */
const char*
 libmcu_mmap_idata_reg_bit_name (libmcu_mmap_t* map, uint32_t n)
{
  config_setting_t* s = NULL;

  s = config_setting_get_elem (map->idata_registers_bit, n);
  return config_setting_get_string_elem (s, 0);
}

/* --------------------------------------------------------------- */
const char*
 libmcu_mmap_code_int_name (libmcu_mmap_t* map, uint32_t n)
{
  config_setting_t* s = NULL;

  if (map == NULL) return NULL;
  if (n == LIBMCU_MMAP_UNMAPPED) return NULL;

  s = config_setting_get_elem (map->code_int, n);
  return config_setting_get_string_elem (s, 0);
}

/* --------------------------------------------------------------- */
void
 libmcu_mmap_destroy_safe (libmcu_mmap_t** map)
{
  if (map == NULL) return;
  if (*map == NULL) return;

  config_destroy (&(*map)->cfg);
  free (*map);
  *map = NULL;
}



