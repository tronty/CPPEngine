#include <stdio.h>

#include "treegen.h"

int main(int argc, const char ** argv)
{
  if (argc != 2)
  {
    LOG_PRINT("Usage: treegen [xml-file]\n");
    return 1;
  }

  FILE* file = stx_fopen(argv[1], "rb");
  if (!file)
  {
    LOG_PRINT("File does not exist, program exiting\n");
    return 7;
  }
  fclose(file);

  TreeGen treegen;
  return treegen.Run(argv[1]);
};
