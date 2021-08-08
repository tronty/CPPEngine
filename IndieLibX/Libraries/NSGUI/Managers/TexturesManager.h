#ifndef TEXTURESMANAGER_H
#define TEXTURESMANAGER_H

#include "../Tools/MediaInfo.h"

#ifdef USE_TINYXML
#include "../Parsers_tinyXML/XMLParser.h"
#else
#include "../Parsers/XMLParser.h"
#endif

typedef MediaInfo<TextureID> TextureInfo;



class TexturesManager

{

  public:

    static bool        addTextureInfo(TextureInfo *textureInfo);

    static TextureInfo *getTextureInfo(NSString texturePath);

    static TextureInfo *getTextureInfo(TextureID textureID);

    static void        flushUnusedTextures();



    static void printTexturesInfo();

    static void flushAllTextures();

  private:

    static vector<TextureInfo*> textureCollection;



};



#endif
