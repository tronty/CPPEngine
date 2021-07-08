#ifndef XML_Parser_H
#define XML_Parser_H

#if 0
#define LOG_PRINT
#define LOG_PRINT_NONE
#define LOG_FNLN
#else
#define LOG_PRINT printf
#define LOG_PRINT_NONE printf
#define LOG_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__)
#endif

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
		name=aName;
		value=aValue;
		//CheckValue();
    }
    XMLAttribute(TiXmlAttribute* pAttribute=0) : NamedObject()
    {
	if(pAttribute)
	{
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
	//CheckValue();
    }

   ~XMLAttribute(){}

    XMLAttribute(const XMLAttribute &copy){value=copy.value;name=copy.name;}

    XMLAttribute &operator =(const XMLAttribute &copy){value=copy.value;name=copy.name;}

	void setName( const std::string& _name ){name=_name;}
	void setValue( const std::string& _value ){value=_value;}
	
	void CheckValue()
	{
	#if 0
		char buffer [33];
		int i=atoi(value.c_str());
		stx_snprintf(buffer, 33, "%d", i);
		std::string v=std::string(buffer);
		if(v==value)
			raise(SIGABRT);		
		float f=atof(value.c_str());
		stx_snprintf(buffer, 33, "%f", f);
		v=std::string(buffer);
		if(v==value)
			raise(SIGABRT);
	#endif
	}

    std::string  &getValue (){return value;}

    const char*    getValuec(){return value.c_str();}

    const double   getValued(){return getValuei();}

    const float    getValuef()
    {
    	float f=atof(value.c_str());
    	#if 1
    	LOG_FNLN;
    	printf("%s, %f\n", name.c_str(), f);
    	#endif
    	return f;
    }

    const int      getValuei()
    {
    	int i=atoi(value.c_str());
    	#if 1
    	LOG_FNLN;
    	printf("%s, %d\n", name.c_str(), i);
    	#endif
    	return i;
    }
    
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
	if(pElement)
	{
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
    {
    	#if 1
    	if(element.getAttributeByName("x")) container.x=atof(element.getAttributeByName("x")->getValue().c_str());
    	if(element.getAttributeByName("y")) container.y=atof(element.getAttributeByName("y")->getValue().c_str());
    	if(element.getAttributeByName("z")) container.z=atof(element.getAttributeByName("z")->getValue().c_str());
    	if(element.getAttributeByName("w")) container.w=atof(element.getAttributeByName("w")->getValue().c_str());
    	#endif
    }
    static void loadRX_GY_BZ_AWf(XMLElement &element, D3DXFROMWINEVECTOR4 &container)
    {
    	#if 1
    	if(element.getAttributeByName("x")) container.x=atof(element.getAttributeByName("x")->getValue().c_str());
    	if(element.getAttributeByName("y")) container.y=atof(element.getAttributeByName("y")->getValue().c_str());
    	if(element.getAttributeByName("z")) container.z=atof(element.getAttributeByName("z")->getValue().c_str());
    	if(element.getAttributeByName("w")) container.w=atof(element.getAttributeByName("w")->getValue().c_str());
    	#endif
    }
    static void loadRX_GY_BZi(XMLElement &element, Tuple3i& container)
    {
    	#if 1
    	if(element.getAttributeByName("x")) container.x=atof(element.getAttributeByName("x")->getValue().c_str());
    	if(element.getAttributeByName("y")) container.y=atof(element.getAttributeByName("y")->getValue().c_str());
    	if(element.getAttributeByName("z")) container.z=atof(element.getAttributeByName("z")->getValue().c_str());
    	#endif
    }
    static void loadRX_GY_BZf(XMLElement &element, D3DXFROMWINEVECTOR3 &container)
    {
    	#if 1
    	if(element.getAttributeByName("x")) container.x=atof(element.getAttributeByName("x")->getValue().c_str());
    	if(element.getAttributeByName("y")) container.y=atof(element.getAttributeByName("y")->getValue().c_str());
    	if(element.getAttributeByName("z")) container.z=atof(element.getAttributeByName("z")->getValue().c_str());
    	#endif
    }

    static void loadRX_GYi(XMLElement &element, Tuple2i& container)
    {
    	#if 1
    	if(element.getAttributeByName("x")) container.x=atof(element.getAttributeByName("x")->getValue().c_str());
    	if(element.getAttributeByName("y")) container.y=atof(element.getAttributeByName("y")->getValue().c_str());
    	#endif
    }

    static void loadRX_GYf(XMLElement &element, D3DXFROMWINEVECTOR2 &container)
    {
    	#if 1
    	if(element.getAttributeByName("x")) container.x=atof(element.getAttributeByName("x")->getValue().c_str());
    	if(element.getAttributeByName("y")) container.y=atof(element.getAttributeByName("y")->getValue().c_str());
    	#endif
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

