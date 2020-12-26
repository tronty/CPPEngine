#include "tester.h"

int main(int argc, const char ** argv)
{
  if (argc!=2)
  {
    LOG_PRINT ("Usage: webertest [xml-file]");
    return 1;
  }

  FILE* file = stx_fopen(argv[1], "rb");
  if (!file)
  {
    LOG_PRINT("File does not exist, program exiting\n");
    return 2;
  }

  Tester test;
  test.Run(argv[1]);
  return 0;
};
