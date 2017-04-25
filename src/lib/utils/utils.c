#include <stdio.h>
#include "utils.h"

char *read_file(const char *path, char **buffer, size_t *size)
{
  FILE *file = fopen(path, "r");
  if (!file)
  {
    return NULL;
  }

  fseek(file, 0, SEEK_END);

  *size = ftell(file);
  if (*size <= 0)
  {
    fclose(file);
    return NULL;
  }

  rewind(file);

  *buffer = (char *)calloc(*size, sizeof(char));
  if (fread(*buffer, sizeof(char), *size, file) <= 0)
  {
    fclose(file);
    free(*buffer);
    return NULL;
  }

  fclose(file);
  return *buffer;
}