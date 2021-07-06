#include "NamedObject.h"



NamedObject::NamedObject(const char*  argName)

{
	if(argName)
		name = argName;
	else
		name = "";

}



NamedObject::NamedObject(const std::string &argName)

{

  name = argName;

}

void NamedObject::setName(const char* nameArg)

{

	if(nameArg)
  name = nameArg;

}



void NamedObject::setName(const std::string &nameArg)

{

  name = nameArg;

}



std::string /*&*/NamedObject::getName() 

{

  return name;

}



NamedObject::~NamedObject()

{

  name="";

}

