#include <Framework3/IRenderer.h>
#include "TexturesManager.h"



vector<TextureInfo*> TexturesManager::textureCollection;



bool TexturesManager::addTextureInfo(TextureInfo *textureInfo)

{

  flushUnusedTextures();



  if(!textureInfo)

    return false;

  textureCollection.push_back(textureInfo);

  return true;

}



TextureInfo *TexturesManager::getTextureInfo(NSString texturePath)

{

  flushUnusedTextures();

  if(!texturePath || !textureCollection.size())

    return 0;



  for(size_t i = 0; i < textureCollection.size(); i++)

    if(textureCollection[i]->getMediaPath() == texturePath)

      return textureCollection[i];



  return 0;

}



TextureInfo *TexturesManager::getTextureInfo(TextureID textureID)

{

  if(!textureID || !textureCollection.size())

    return 0;



  for(size_t i = 0; i < textureCollection.size(); i++)

    if(textureCollection[i]->getMedia() == textureID)

      return textureCollection[i];



  return 0;

}



void TexturesManager::flushUnusedTextures()

{

  vector<TextureInfo*> validTextures;

  size_t i         = 0,

         count     = 0;

  TextureID textureID = 0;



  for(i = 0; i < textureCollection.size(); i++)

    if(textureCollection[i]->getUserCount() > 0)

    {

      validTextures.push_back(textureCollection[i]);

    }

    else

    {

      count++;

      textureID = (TextureID/*???*/)textureCollection[i]->getMedia();

//      SAFE_RELEASE(textureID);

      deleteObject(textureCollection[i]);

    }



  textureCollection.clear();



  for(i = 0; i < validTextures.size(); i++)

      textureCollection.push_back(validTextures[i]);



  if(count)

    LOG_PRINT_NONE("\n");

}



void TexturesManager::flushAllTextures(){

  TextureID textureID = 0;



  size_t i = 0;

  for(i = 0; i < textureCollection.size(); i++)

  {

    textureID = (TextureID/*???*/)textureCollection[i]->getMedia();

//    SAFE_RELEASE(textureID);

    deleteObject(textureCollection[i]);

  }



  textureCollection.clear();

  if(i) LOG_PRINT_NONE("\n");

}



void TexturesManager::printTexturesInfo()

{

  if(!textureCollection.size())

    cout << "This Manager contains no Textures as of yet." << endl;

  else{

    cout << "Textures list: " << endl;

    for(size_t i = 0; i < textureCollection.size(); i++)

      cout <<  "<users = \"" << textureCollection[i]->getUserCount() << "\" "

           <<  " path  = \"" << textureCollection[i]->getMediaPath() << "\" "

           <<  " id    = \"" << textureCollection[i]->getMedia()     << "\" />" << endl;

  }

}

