#pragma once



#include <iostream>

#include <fstream>

#include <vector>



using namespace std;

//include <Framework3/IRenderer.h>


#include "../Tools/MathUtils.h"


#include "../Tools/NamedObject.h"

//include "../Tools/NSString.h"

#define deleteObject(A){ if(A){ delete	 A; A = 0; } }

#define deleteArray(A) { if(A){ delete[] A; A = 0; } }



#define MAX_TEX_UNITS 8



#define XML_FILE_NOT_FOUND  0x0001

#define XML_SUCCESS         0x0002

#define XML_FAILED          0x0004

#define XML_READY           0x0008

#define XML_MATCH           0x0010

#define XML_BUFFER_OVERFLOW 0x0020



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

    NSString  value;

  public:

    XMLElement();

   ~XMLElement();



    XMLElement(const XMLElement &copy);

    XMLElement &operator =(const XMLElement &copy);



    static void loadRX_GY_BZ_AWi(XMLElement &element, Tuple4i& container);

    static void loadRX_GY_BZ_AWf(XMLElement &element, D3DXFROMWINEVECTOR4 &container);



    static void loadRX_GY_BZi(XMLElement &element, Tuple3i& container);

    static void loadRX_GY_BZf(XMLElement &element, D3DXFROMWINEVECTOR3 &container);



    static void loadRX_GYi(XMLElement &element, Tuple2i& container);

    static void loadRX_GYf(XMLElement &element, D3DXFROMWINEVECTOR2 &container);



    void setValue(NSString &val);

    void print();

    void flush();



    NSString  &getValue ();

    const double   getValued();

    const float    getValuef();

    const char*    getValuec();

    const int      getValuei();



    RawData rawData;

};



class XMLStack : public XMLTree

{

  public:

    XMLStack(char *xmlFilePath = 0, bool loggerOn = true);

    XMLStack(const XMLStack &copy);

   ~XMLStack();

    XMLStack &operator =(const XMLStack &copy);



    void   print();

    void   flush();

    int    loadXMLFile(const char *xmlFilePath);



  private:

    void   writeFatalLogInfoList(const char* format,...);

    void   writeFatalLogInfo(const char* string);



    void   getStreamedValue(char **stream, NSString&);

    void   getStringValue  (char **stream, NSString&);

    void   getIdentifier   (char **stream, NSString&);



    bool   moveAndCheckProgress(int jump = 1);

    bool   consumeWhiteSpaces(char **stream);

    bool   consumeXMLHeader(char **stream);

    bool   consumeComment(char **stream);

    bool   fillRawData(char **stream, RawData *dataStruct, int count = -1);

    char*  parseXMLStream(char *stream, XMLElement *parent);

    int    getRemainingBytes();



    NSString encoding,

           logFilePath;

    float  XMLVersion;

    bool   loggerOn;

    int    bufferProgress,

           bufferSize,

           state;

};

