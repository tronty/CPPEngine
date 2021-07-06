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

    static bool         registerPath(std::string path);

    static void         printAllPaths();

    static std::string  lookUpMediaPath(std::string path);

  public:

    static vector<std::string> pathStack;

};



#endif
