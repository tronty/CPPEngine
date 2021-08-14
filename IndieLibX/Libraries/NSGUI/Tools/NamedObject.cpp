#include "NamedObject.h"



NamedObject::NamedObject(const char*  argName)

{

  name = argName;

}



NamedObject::NamedObject(const NSString &argName)

{

  name = argName;

}



NamedObject::NamedObject(const NamedObject &copy)

{

  if(this != &copy)

    name = copy.name;

}



NamedObject &NamedObject::operator=(const NamedObject &copy)

{

  if(this != &copy)

    name = copy.name;

  return *this;

}



void NamedObject::setName(const char* nameArg)

{

  name = nameArg;

}



void NamedObject::setName(const NSString &nameArg)

{

  name = nameArg;

}



NSString /*&*/NamedObject::getName() 

{

  return name;

}



NamedObject::~NamedObject()

{

  name.clear();

}





