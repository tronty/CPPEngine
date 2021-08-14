#ifndef XML_IO_H
#define XML_IO_H

#include "XMLParser.h"

class IOXMLObject

{

  private:
  public:

    IOXMLObject(const char *aioname=""){if(aioname) ioname=aioname;}



    virtual bool loadXMLSettings(const char *path  )
    {
    	return true;
    }

    virtual bool loadXMLSettings(XMLElement *element)   = 0;


/*
    virtual bool exportXMLSettings(ofstream &xmlFile  ) = 0;

    virtual bool exportXMLSettings(const char* xmlPath);
*/


    bool    isSuitable(XMLElement *element){return true;}

  protected:

    NSString ioname;

};

#endif

