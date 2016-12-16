
#ifndef __GUI_HPP__
#define __GUI_HPP__


/* --------------------------------------------------------------- */
static bool idaapi mcu8051_gui_menu_ireg (void* data);
static uint32_t idaapi mcu8051_gui_ireg_num (void* obj);
static void idaapi mcu8051_gui_ireg_create_line (void* obj,
    uint32_t n, char* const* arr);
void mcu8051_gui_ireg_create (libmcu_mmap_t* map);

/* --------------------------------------------------------------- */
static bool idaapi mcu8051_gui_menu_ireg_bit (void* data);
static uint32_t idaapi mcu8051_gui_ireg_bit_num (void* obj);
static void idaapi mcu8051_gui_ireg_bit_create_line (void* obj,
    uint32_t n, char* const* arr);
void mcu8051_gui_ireg_bit_create (libmcu_mmap_t* map);

/* --------------------------------------------------------------- */
static bool idaapi mcu8051_gui_menu_int (void* data);
static uint32_t idaapi mcu8051_gui_int_num (void* obj);
static void idaapi mcu8051_gui_int_create_line (void* obj,
    uint32_t n, char* const* arr);
void mcu8051_gui_int_create (libmcu_mmap_t* map);

/* --------------------------------------------------------------- */
void mcu8051_gui_menu_create (libmcu_mmap_t* map);
void mcu8051_gui_menu_destroy (void);

#endif


