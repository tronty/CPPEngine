#pragma once

#include "../Tools/NamedObject.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define XML_FILE_NOT_FOUND  0x0001
#define XML_SUCCESS         0x0002   
#define XML_FAILED          0x0004
#define XML_READY           0x0008
#define XML_MATCH           0x0010
#define XML_BUFFER_OVERFLOW 0x0020

#include "../Math/MathUtils.h"

class RawData
{
  public:
    bool  skipDelete;
    char *data;
    int   byteCount;

    RawData();
    RawData(const RawData &copy);
    RawData &operator=(const RawData &copy);
   ~RawData();
    void destroy();
};

class XMLElement;

class XMLTree
{
  protected:
    vector<XMLElement*> children;

  public:
   ~XMLTree();

    void        addChild(XMLElement *child);
    XMLElement *getChildByName(const char *name);
    XMLElement *getChild(size_t index);
    size_t      getChildrenCount();
    void        flush();
    void        print();
};

class XMLElement : public XMLTree, public NamedObject
{
  private:
    String  value;
  public:
    XMLElement();
   ~XMLElement();

    XMLElement(const XMLElement &copy);
    XMLElement &operator =(const XMLElement &copy);

    static void loadRX_GY_BZ_AWi(XMLElement &element, Tuple4i &container);
    static void loadRX_GY_BZ_AWf(XMLElement &element, Tuple4f &container);

    static void loadRX_GY_BZi(XMLElement &element, Tuple3i &container);
    static void loadRX_GY_BZf(XMLElement &element, Tuple3f &container);

    static void loadRX_GYi(XMLElement &element, Tuple2i &container);
    static void loadRX_GYf(XMLElement &element, Tuple2f &container);

    void setValue(String &val);
    void print();
    void flush(); 
    
    const String  &getValue ();
    const double   getValued();
    const float    getValuef();
    const char*    getValuec();
    const int      getValuei();

    RawData rawData;
};

class XMLStack : public XMLTree
{
  public:
    XMLStack(char *xmlFilePath = NULL, bool loggerOn = true);    
    XMLStack(const XMLStack &copy);
   ~XMLStack();
    XMLStack &operator =(const XMLStack &copy);

    void   print();
    void   flush();
    int    loadXMLFile(const char *xmlFilePath);

  private:
    void   writeFatalLogInfoList(const char* format,...);
    void   writeFatalLogInfo(const char* string);

    void   getStreamedValue(char **stream, String&);
    void   getStringValue  (char **stream, String&);
    void   getIdentifier   (char **stream, String&);

    bool   moveAndCheckProgress(int jump = 1);
    bool   consumeWhiteSpaces(char **stream);
    bool   consumeXMLHeader(char **stream);
    bool   consumeComment(char **stream);
    bool   fillRawData(char **stream, RawData *dataStruct, int count = -1);
    char*  parseXMLStream(char *stream, XMLElement *parent);
    int    getRemainingBytes();

    String encoding,
           logFilePath;
    float  XMLVersion;
    bool   loggerOn;
    int    bufferProgress,
           bufferSize,
           state;
};
