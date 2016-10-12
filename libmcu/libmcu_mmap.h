
#ifndef __MMAP_H__
#define __MMAP_H__

/* --------------------------------------------------------------- */
#define LIBMCU_MMAP_UNMAPPED ((uint32_t)~0) 

#define LIBMCU_MMAP_INT_NONE 0
#define LIBMCU_MMAP_INT_CODE 1
#define LIBMCU_MMAP_INT_DATA 2

/* --------------------------------------------------------------- */
typedef struct mcu_mmap_error
{
  uint8_t shithappened;
  uint32_t line;
  char msg[256];
}mcu_mmap_error_t;

/* --------------------------------------------------------------- */
typedef struct mcu_mmap_t
{
  config_t cfg;
  config_setting_t* idata_registers;
  uint32_t nidata_registers;
  config_setting_t* idata_registers_bit;
  uint32_t nidata_registers_bit;
  config_setting_t* xdata_registers;
  uint32_t nxdata_registers;
  config_setting_t* xdata_registers_bit;
  uint32_t nxdata_registers_bit;
  config_setting_t* registers;
  uint32_t nregisters;
  config_setting_t* registers_bit;
  uint32_t nregisters_bit;
  config_setting_t* code_int;
  uint32_t ncode_int;
  mcu_mmap_error_t error;
}mcu_mmap_t;

/* --------------------------------------------------------------- */
mcu_mmap_t* libmcu_mmap_create (char* filename);
uint8_t libmcu_mmap_check_syntax (mcu_ctx_t* ctx, mcu_mmap_t* map);
uint32_t libmcu_mmap_idata_reg (mcu_mmap_t* map, uint64_t addr);
uint32_t libmcu_mmap_idata_reg_bit (mcu_mmap_t* map, uint64_t addr);
uint32_t libmcu_mmap_code_int (mcu_mmap_t* map, uint64_t addr);
uint8_t libmcu_mmap_code_int_type (mcu_mmap_t* map, uint32_t n);
uint64_t libmcu_mmap_code_int_size (mcu_mmap_t* map, uint32_t n);
const char* libmcu_mmap_idata_reg_name (mcu_mmap_t* map, uint32_t n);
const char* libmcu_mmap_idata_reg_bit_name (mcu_mmap_t* map, uint32_t n);
const char* libmcu_mmap_code_int_name (mcu_mmap_t* map, uint32_t n);
#define libmcu_mmap_destroy(m) libmcu_mmap_destroy_safe (&m)
void libmcu_mmap_destroy_safe (mcu_mmap_t** map);

#endif


