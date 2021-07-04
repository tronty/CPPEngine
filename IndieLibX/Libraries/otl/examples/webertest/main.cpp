#include "tester.h"

int main(int argc, const char ** argv)
{
  if (argc!=2)
  {
    printf ("Usage: webertest [xml-file]");
    return 1;
  }

  FILE* file = fopen(argv[1], "rb");
  if (!file)
  {
    printf("File does not exist, program exiting\n");
    return 2;
  }

  Tester test;
  test.Run(argv[1]);
  return 0;
};
