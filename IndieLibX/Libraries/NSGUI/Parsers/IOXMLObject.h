#ifndef XML_IO_H

#define XML_IO_H

#include "XMLParser.h"



class IOXMLObject

{

  public:

    IOXMLObject(const char *ioname);



    virtual bool loadXMLSettings(const char *path  );

    virtual bool loadXMLSettings(XMLElement *element)   = 0;



    virtual bool exportXMLSettings(ofstream &xmlFile  ) = 0;

    virtual bool exportXMLSettings(const char* xmlPath);



    bool    isSuitable(XMLElement *element);

  protected:

    NSString ioname;

};



#endif

