
#ifndef __FILE_H__
#define __FILE_H__

typedef struct udasm_file_block
{
  uint64_t start;
  uint64_t size;
  uint8_t* data;
}udasm_file_block_t;

typedef struct udasm_file
{
  FILE* fd;
  uint32_t size;
  udasm_file_block_t** blocks;
  uint32_t nblock;
  uint8_t type;
#define UDASM_FILE_UNKNOWN 0
#define UDASM_FILE_BINARY  1
#define UDASM_FILE_IHEX    2
#define UDASM_FILE_SREC    3
}udasm_file_t;

udasm_file_t* udasm_file_open (const char* filename, uint8_t type);
uint8_t udasm_file_read_bin (udasm_file_t* file, uint64_t start);
uint8_t udasm_file_read (udasm_file_t* file, uint64_t start);
#define udasm_file_destroy(f) udasm_file_destroy_safe (&f)
void udasm_file_destroy_safe (udasm_file_t** file);


#endif

