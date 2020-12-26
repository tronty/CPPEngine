#include "treegen.h"

#include <opentree/weber/weber.h>
#include <helper/ottreesaver.h>

#include <helper/xmlparser.h>
#include <helper/xmlparserarbaro.h>

int TreeGen::parse(const char* format, const char* file)
{
  if (!strcmp(format, "otl-xml"))
  {
    opentree::xmlParser parser;

    if (!parser.Load(params, file))
    {
      LOG_PRINT("Parsing of xml data failed!\n");
      return 2;
    }
  }
  else if (!strcmp(format, "arbaro-xml"))
  {
    opentree::xmlParserArbaro parser;

    if (!parser.Load(params, file))
    {
      LOG_PRINT("Parsing of xml data failed!\n");
      return 2;
    }
  }
  else
  {
    LOG_PRINT("Unknown xml format '%s'\n", format);
    return 3;
  }
  return 0;
}

int TreeGen::generate(const char* algo)
{
  opentree::iWeber* gen = 0;
  if (!strcmp(algo, "otl-weber"))
  {
    gen = opentree::newWeber();
  }
  else if (!strcmp(algo, "arbaro-weber"))
  {
    gen = opentree::newWeberArbaro();
  }
  else
  {
    LOG_PRINT("Unknown weber algo '%s'\n", algo);
    return 4;
  }

  gen->setParams(params);
  tree = gen->generate();
  delete gen;

  return 0;
}

int TreeGen::save(const char* filename)
{
  opentree::otTreeSaver saver;

  char* data = 0;
  unsigned int size = saver.save(tree, data);

  FILE* file = stx_fopen(filename,"wb");
  if (!file)
  {
    LOG_PRINT("Unable to write file '%s'\n", filename);
    return 5;
  }
  fwrite(data,1,size,file);
  fclose(file);

  delete [] data;

  return 0;
}

int TreeGen::Run(const char* file)
{
  int retval=0;

  TesterData data;

  TestXmlParser parser;

  if (!parser.Load(file))
  {
    LOG_PRINT("Error parsing the xml configuration file\n");
    retval=6;
    return retval;
  }

  while (parser.getNext(data))
  {
    int rv = 0;
    LOG_PRINT("Generation: %s with %s..... ", data.name, data.algo);

    opentree::setRandomSeed(4);
    rv = parse(data.format, data.inputfile);
    if (!rv) rv = generate(data.algo);
    if (!rv) rv = save(data.outputfile);

    if (rv)
      LOG_PRINT("[failed]\n");
    else
      LOG_PRINT("[succeeded]\n");
  }
  
  return retval;
}
