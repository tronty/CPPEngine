
#include <STX/STX.h>
#include <STX/STXCPP.h>
#include <STX/STXinterfaces.h>
#include <STX/CInput.h>
#include <STX/Noise.h>
#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>
#include "IOXMLObject.h"

#include "../Managers/MediaPathManager.h"



IOXMLObject::IOXMLObject(const char *ionameArg)

{

  ioname = ionameArg;

}



bool IOXMLObject::loadXMLSettings(const char* apath)

{

	LOG_FNLN_NONE;
	LOG_PRINT_NONE("apath:%s\n",apath);
	std::string f1=apath;
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("f1:%s\n",f1.c_str());
	std::string f2=stx_convertpath(f1);
	LOG_PRINT_NONE("f2:%s\n",f2.c_str());
	const char* path=f2.c_str();
	LOG_PRINT_NONE("path:%s\n",path);

  if(!ioname.getLength())

  {

    //LOG_PRINT_NONE();

    return false;

  }

  //LOG_PRINT_NONE("path    =%s\n",path);
  NSString verifiedNS = MediaPathManager::lookUpMediaPath(path);

  const char *verified=verifiedNS.data;

  //LOG_PRINT_NONE("verified:%s\n",verified);

	char path3[1024];
	stx_strlcpy(path3,path,1024);

		int slash1 = int(stx_strrchr(path3, stx_getOsSeparator())-path3);
		int slash2 = int(stx_strrchr(path3, stx_getOsSeparator())-path3);
		int slash=max(slash1,slash2);
		path3[slash+1]='\0';

  MediaPathManager::registerPath(path3);



  if(!verified)

  {

	LOG_PRINT_NONE("Couldn't locate the <%s> XML file at \"%s\" even with a look up", ioname.c_str(), path);

    return false;

  }

  XMLElement  *child  = 0;

  XMLStack     xmlStack;



  LOG_FNLN_NONE;
	LOG_PRINT_NONE("verified:%s\n",verified);
  if(!xmlStack.loadXMLFile(verified))

  {

	LOG_PRINT_NONE("Invalid XML file -> %s\n", verified);

    return false;

  }


 //LOG_PRINT_NONE("ioname=%s\n",ioname.data);
  if(!(child = xmlStack.getChildByName(ioname)))

  {

	LOG_PRINT_NONE("Need a <%s> tag in the XML file\n", ioname.c_str());

    return false;

  }



  return loadXMLSettings(child);

}



bool IOXMLObject::exportXMLSettings(const char* xmlPath)

{

  ofstream xmlFile(xmlPath, ios::app | ios::binary);

  bool result = exportXMLSettings(xmlFile);

  xmlFile.close();

  return result;

}



bool IOXMLObject::isSuitable(XMLElement *element)

{

  if(!element)

  {

	LOG_PRINT_NONE("0 <%s> node\n", ioname.c_str());

    return false;

  }



  if(element->getName() != ioname)

  {

	LOG_PRINT_NONE("Need a <%s> tag in the XML Node\n", ioname.c_str());

    return false;

  }



  return true;

}





