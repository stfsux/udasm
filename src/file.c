
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "udasm.h"
#include "file.h"

/* ---------------------------------------------------------------- */
udasm_file_t*
 udasm_file_open (const char* filename, uint8_t type)
{
  udasm_file_t* file = NULL;

  file = calloc (1, sizeof(udasm_file_t));
  if (file == NULL)
  {
    err ("cannot allocate %zu byte(s).\n",
        sizeof(udasm_file_t));
    return NULL;
  }

  file->fd = fopen (filename, "rb");
  if (file->fd == NULL)
  {
    err ("cannot open file `%s' for reading.\n",
        filename);
    free (file);
    return NULL;
  }

  if (strstr (filename, ".bin") != NULL ||
      strstr (filename, ".BIN") != NULL)
    file->type = UDASM_FILE_BINARY;
  else if (strstr (filename, ".ihex") != NULL ||
      strstr (filename, ".IHEX") != NULL ||
      strstr (filename, ".ihx") != NULL ||
      strstr (filename, ".IHX") != NULL ||
      strstr (filename, ".hex") != NULL ||
      strstr (filename, ".HEX") != NULL)
    file->type = UDASM_FILE_IHEX;
  else if (strstr (filename, ".srec") != NULL ||
      strstr (filename, ".SREC") != NULL)
    file->type = UDASM_FILE_SREC;
  else
    file->type = UDASM_FILE_UNKNOWN;

  if (file->type == UDASM_FILE_UNKNOWN &&
      type == UDASM_FILE_UNKNOWN)
  {
    err ("input file `%s' type is unknown.\n",
        filename);
    fclose (file->fd);
    free (file);
    return NULL;
  }
  else if (file->type == UDASM_FILE_UNKNOWN &&
      type != UDASM_FILE_UNKNOWN)
  {
    file->type = type;
  }
  return file;
}

/* ---------------------------------------------------------------- */
uint8_t
 udasm_file_read_bin (udasm_file_t* file, uint64_t start)
{
  if (file == NULL) return 0;
  file->nblock = 1;

  file->blocks = calloc (file->nblock,
      sizeof (udasm_file_block_t*));
  if (file->blocks == NULL)
  {
    err ("cannot allocate %zu byte(s).\n",
        sizeof (udasm_file_block_t*)*file->nblock);
    return 0;
  }
  file->blocks[0] = calloc (1, sizeof(udasm_file_block_t));
  if (file->blocks[0] == NULL)
  {
    err ("cannot allocate %zu byte(s).\n",
        sizeof (udasm_file_block_t));
    return 0;
  }

  fseek (file->fd, 0, SEEK_END);
  file->size = ftell (file->fd);
  file->blocks[0]->size = file->size;
  fseek (file->fd, 0, SEEK_SET);

  file->blocks[0]->data = calloc (file->blocks[0]->size,
      sizeof (uint8_t));
  if (file->blocks[0]->data == NULL)
  {
    err ("cannot allocate %zu byte(s).\n",
        file->blocks[0]->size*sizeof(uint8_t));
    return 0;
  }

  file->blocks[0]->start = start;
  fread (file->blocks[0]->data, sizeof(uint8_t),
      file->blocks[0]->size, file->fd);
  return 1;
}
 
/* ---------------------------------------------------------------- */
uint8_t
 udasm_file_read (udasm_file_t* file, uint64_t start)
{
  if (file == NULL) return 0;

  switch (file->type)
  {
    case UDASM_FILE_BINARY:
      return udasm_file_read_bin (file, start);
  }

  return 0;
}

/* ---------------------------------------------------------------- */
void
 udasm_file_destroy_safe (udasm_file_t** file)
{
  uint32_t n = 0;

  if (file == NULL) return;
  if (*file == NULL) return;

  for (n = 0; n < (*file)->nblock; n++)
  {
    if ((*file)->blocks[n] != NULL)
    {
      if ((*file)->blocks[n]->data != NULL)
        free ((*file)->blocks[n]->data);
      free ((*file)->blocks[n]);
    }
  }

  if ((*file)->blocks != NULL)
    free ((*file)->blocks);

  if ((*file)->fd != NULL) fclose ((*file)->fd);

  free (*file);
  *file = NULL;
}


