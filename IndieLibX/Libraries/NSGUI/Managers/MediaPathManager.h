#ifndef MEDIAPATHMANAGER_H

#define MEDIAPATHMANAGER_H




#include <GUI/GUIUtils.h>

#include "../Parsers/XMLParser.h"
#include <vector>



using namespace std;



class MediaPathManager

{

  public:

    static int          getPathCount();

    static bool         registerPath(XMLElement *MediaPathNode);

    static bool         registerPath(NSString path);

    static void         printAllPaths();

    static NSString  lookUpMediaPath(NSString path);

  public:

    static vector<NSString> pathStack;

};



#endif
