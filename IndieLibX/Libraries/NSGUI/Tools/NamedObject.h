#ifndef NAMEDOBJECT_H

#define NAMEDOBJECT_H



#include "NSString.h"



class NamedObject

{



  protected:

    NSString name;



  public:

    NamedObject(const char*  argName = 0);

    NamedObject(const NSString &argName);

    NamedObject(const NamedObject &copy);

   ~NamedObject();



    NamedObject &operator=(const NamedObject &copy);

    void   setName(const char* nameArg);

    void   setName(const NSString &name);



    NSString /*&*/getName()     ;

};



#endif
