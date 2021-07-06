#ifndef XML_Parser_H
#define XML_Parser_H

// /Libraries/tinyxml/include/tinyxml/tinyxml.h
#include "../../tinyxml/include/tinyxml/tinyxml.h"

#include <iostream>

#include <fstream>

#include <vector>



using namespace std;

//include <Framework3/IRenderer.h>


#include "../Tools/MathUtils.h"


#include "../Tools/NamedObject.h"

//#include <string>

#define deleteObject(A){ if(A){ delete	 A; A = 0; } }

#define deleteArray(A) { if(A){ delete[] A; A = 0; } }



#define MAX_TEX_UNITS 8



#define XML_FILE_NOT_FOUND  0x0001

#define XML_SUCCESS         0x0002

#define XML_FAILED          0x0004

#define XML_READY           0x0008

#define XML_MATCH           0x0010

#define XML_BUFFER_OVERFLOW 0x0020


#if 1
class RawData

{

  public:

    bool  skipDelete;

    char *data;

    int   byteCount;



    RawData(){}

    RawData(const RawData &copy);

    RawData &operator=(const RawData &copy);

   ~RawData(){}

    void destroy();

};
#endif

class XMLElement;
class XMLAttribute;

class XMLTree : public NamedObject
{
  protected:
    //std::string name;
    std::string value;
    vector<XMLElement*> children;

  public:
    XMLTree(TiXmlElement* pElement=0);

   ~XMLTree(){children.clear();}

    virtual XMLElement* getChildByName(const char* name="");

    virtual XMLElement* getChild(size_t index);

    void        addChild(XMLElement *child){children.push_back(child);}

    size_t      getChildrenCount(){return children.size();}
    
    void        flush(){}

    void        print(){}
    
    void setValue(const std::string& a){value=a;}
    
    const std::string& getValue(){return value;}
    
	void Dump();
};


class XMLAttribute : public NamedObject
{
  protected:
    //std::string name;
    std::string value;
  public:
    XMLAttribute(std::string aName, std::string aValue) : NamedObject() // : NamedObject(aName)
    {
    		LOG_FNLN;
		name=aName;
		value=aValue;
    }
    XMLAttribute(TiXmlAttribute* pAttribute=0) : NamedObject()
    {
	if(pAttribute)
	{
		LOG_FNLN;
		LOG_PRINT("pAttribute->Name()=%s\n", pAttribute->Name());
		LOG_PRINT("pAttribute->Value()=%s\n", pAttribute->Value());
		if(pAttribute->Name())
			name=pAttribute->Name();
		else
			name="";
		if(pAttribute->Value())
			value=pAttribute->Value();
		else
			value="";
	}
	else
	{
		name="";
		value="";
	}
    }

   ~XMLAttribute(){}

    XMLAttribute(const XMLAttribute &copy){value=copy.value;name=copy.name;}

    XMLAttribute &operator =(const XMLAttribute &copy){value=copy.value;name=copy.name;}

	void setName( const std::string& _name ){name=_name;}
	void setValue( const std::string& _value ){value=_value;}

    std::string  &getValue (){return value;}

    const double   getValued(){ return atoi(value.c_str());}

    const float    getValuef(){ return atof(value.c_str());}

    const char*    getValuec(){return value.c_str();}

    const int      getValuei(){ return atoi(value.c_str());}
    
	void Dump(){LOG_PRINT("%s=%s ", name.c_str(), value.c_str());}
};

class XMLElement : public XMLTree
{
  public:
    vector<XMLAttribute*> attributes;

    virtual XMLAttribute *getAttributeByName(const char *name="");

    virtual XMLAttribute *getAttribute(size_t index);

    void        addAttribute(std::string aName, std::string aValue){attributes.push_back(new XMLAttribute(aName, aValue));}

    void        addAttribute(XMLAttribute *child){attributes.push_back(child);}

    size_t      getAttributeCount(){return attributes.size();}

	void setName( const std::string& _name ){name=_name;}
	void setValue( const std::string& _value ){value=_value;}
	
    XMLElement(TiXmlElement* pElement=0) : XMLTree(pElement)
    {
    		LOG_FNLN;
	if(pElement)
	{
		LOG_FNLN;
		LOG_PRINT("pElement->Value()=%s\n", pElement->Value());
		if(pElement->Value())
			name=pElement->Value();
		else
			name="";
		value="";
	}
	else
	{
		name="";
		value="";
	}
    }

   ~XMLElement(){attributes.clear();}

    XMLElement(const XMLElement &copy){value=copy.value;name=copy.name;}

    XMLElement &operator =(const XMLElement &copy){value=copy.value;name=copy.name;}    
    
    static void loadRX_GY_BZ_AWi(XMLElement &element, Tuple4i& container)
    { /*
    	container.x=atof(element.element->Attribute("x"));
    	container.y=atof(element.element->Attribute("y"));
    	container.z=atof(element.element->Attribute("z"));
    	container.w=atof(element.element->Attribute("w")); */
    }
    static void loadRX_GY_BZ_AWf(XMLElement &element, D3DXFROMWINEVECTOR4 &container)
    { /*
    	container.x=atof(element.element->Attribute("x"));
    	container.y=atof(element.element->Attribute("y"));
    	container.z=atof(element.element->Attribute("z"));
    	container.w=atof(element.element->Attribute("w")); */
    }
    static void loadRX_GY_BZi(XMLElement &element, Tuple3i& container)
    { /*
    	container.x=atof(element.element->Attribute("x"));
    	container.y=atof(element.element->Attribute("y"));
    	container.z=atof(element.element->Attribute("z")); */
    }
    static void loadRX_GY_BZf(XMLElement &element, D3DXFROMWINEVECTOR3 &container)
    { /*
    	container.x=atof(element.element->Attribute("x"));
    	container.y=atof(element.element->Attribute("y"));
    	container.z=atof(element.element->Attribute("z")); */
    }

    static void loadRX_GYi(XMLElement &element, Tuple2i& container)
    { /*
    	container.x=atof(element.element->Attribute("x"));
    	container.y=atof(element.element->Attribute("y")); */
    }

    static void loadRX_GYf(XMLElement &element, D3DXFROMWINEVECTOR2 &container)
    { /*
    	container.x=atof(element.element->Attribute("x"));
    	container.y=atof(element.element->Attribute("y")); */
    }

    void setValue(std::string &val){value=val;}

    void print(){}

    void flush(){}

    std::string  &getValue (){return value;}

    const double   getValued(){ return atoi(value.c_str());}

    const float    getValuef(){ return atof(value.c_str());}

    const char*    getValuec(){return value.c_str();}

    const int      getValuei(){ return atoi(value.c_str());}

    RawData rawData;
    
	void Dump()
	{
		LOG_PRINT("< %s ", name.c_str());
    		for(unsigned int i=0;i<attributes.size();i++)
    			attributes[i]->Dump();
    		LOG_PRINT(">\n");	
	}
};



class XMLStack : public XMLTree
{
  protected:
  	TiXmlElement* rootPanelElement;
  	XMLElement* rootElement;
  public:

    XMLStack(char *xmlFilePath = "", bool loggerOn = true){rootPanelElement=0;rootElement=0;}

    XMLStack(const XMLStack &copy)
    {
    	rootPanelElement=copy.rootPanelElement;
    	encoding=copy.encoding;
        logFilePath=copy.logFilePath;
           XMLVersion=copy.XMLVersion;
           loggerOn=copy.loggerOn;
	bufferProgress=copy.bufferProgress;
           bufferSize=copy.bufferSize;
           state=copy.state;
    }

   ~XMLStack(){}

    XMLStack &operator =(const XMLStack &copy)
    {
    	rootPanelElement=copy.rootPanelElement;
    	encoding=copy.encoding;
        logFilePath=copy.logFilePath;
           XMLVersion=copy.XMLVersion;
           loggerOn=copy.loggerOn;
	bufferProgress=copy.bufferProgress;
           bufferSize=copy.bufferSize;
           state=copy.state;
    }

    void   print(){}

    void   flush(){}

    int    loadXMLFile(const char *xmlFilePath="");

  private:
#if 0
    void   writeFatalLogInfoList(const char* format,...);

    void   writeFatalLogInfo(const char* string);



    void   getStreamedValue(char **stream, std::string&);

    void   getStringValue  (char **stream, std::string&);

    void   getIdentifier   (char **stream, std::string&);



    bool   moveAndCheckProgress(int jump = 1);

    bool   consumeWhiteSpaces(char **stream);

    bool   consumeXMLHeader(char **stream);

    bool   consumeComment(char **stream);

    //bool   fillRawData(char **stream, RawData *dataStruct, int count = -1);

    char*  parseXMLStream(char *stream, XMLElement *parent);

    int    getRemainingBytes();
#endif

    std::string encoding,

           logFilePath;

    float  XMLVersion;

    bool   loggerOn;

    int    bufferProgress,

           bufferSize,

           state;
};
#endif

