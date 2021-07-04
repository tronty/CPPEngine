#ifndef NAMEDOBJECT_H
#define NAMEDOBJECT_H

#include <string>

class NamedObject

{



  protected:

    std::string name;



  public:

    NamedObject(const char*  argName = "");

    NamedObject(const std::string &argName);

    NamedObject(const NamedObject &copy)
    {
    	name=copy.name;
    }

   ~NamedObject();



    NamedObject &operator=(const NamedObject &copy)
    {
    	name=copy.name;
    }

    void   setName(const char* nameArg="");

    void   setName(const std::string &aname);



    std::string /*&*/getName()     ;

};
#endif

