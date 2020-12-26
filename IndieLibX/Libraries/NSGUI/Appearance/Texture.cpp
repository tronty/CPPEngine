#include <Framework3/IRenderer.h>
#include "Texture.h"

//include "../Renderer/Renderer.h"



#define USETEXTURES 1//???



NVTexture::NVTexture(TextureType target_) : IOXMLObject("Texture")

{

  currUnit	 = -1;

//  pBuffer	   = 0;

  target	 = target_;

  height7	  = 0;

  width7	  = 0;

  depth 	 = 0;

  aniso 	 = 0;

  id		 = 0;

//    surf	   = 0;



      m_clampS=0;

	m_clampT=0;

    m_magFilter=0;

	m_minFilter=0;

    m_mipmap=0;

}



NVTexture::NVTexture(const NVTexture & texture) : IOXMLObject("Texture")

{

  this->operator=(texture);

}



NVTexture &NVTexture::operator= (const NVTexture & copy)

{

  if(this != &copy)

  {

    setID(copy.id);

//	copy.id->AddRef();



//    textureCoordGenerator = copy.textureCoordGenerator;

    transform	 = copy.transform;

    currUnit	 = copy.currUnit;

//    pBuffer	   = copy.pBuffer;

    target	 = copy.target;

    height7	  = copy.height7;

    width7	  = copy.width7;

    depth	 = copy.depth;

    aniso	 = copy.aniso;



    m_clampS=copy.m_clampS;

	m_clampT=copy.m_clampT;

    m_magFilter=copy.m_magFilter;

	m_minFilter=copy.m_minFilter;

    m_mipmap=copy.m_mipmap;



  }

  return *this;

}



bool NVTexture::checkForRepeat(NSString string)

{

  TextureInfo *textureInfo = 0;



  if(textureInfo = TexturesManager::getTextureInfo(string))

    setID(textureInfo->getMedia());



  return (textureInfo != 0);

}



bool NVTexture::finalizeLoading(NSString astring)

{

  if(!id)

    {LOG_PRINT_NONE("Failed to load texture <%s>\n",astring.data);return false;}

  else

  {

    TextureInfo *textureInfo = new TextureInfo(astring, id);

    TexturesManager::addTextureInfo(textureInfo);

  }

  return true;

}

/*

void NVTexture::doTransform()

{

  textureCoordGenerator.startGeneration();



  glMatrixMode(GL_TEXTURE);

  transform.doTransform();

  glMatrixMode(GL_MODELVIEW);

}



void NVTexture::undoTransform()

{

  textureCoordGenerator.stopGeneration();



  glMatrixMode(GL_TEXTURE);

  transform.undoTransform();

  glMatrixMode(GL_MODELVIEW);

}



void NVTexture::setTextureCoordinatesGenerator(TextureCoordGenerator &texGenerator)

{

  textureCoordGenerator = texGenerator;

}



void NVTexture::associateWithPBuffer(GLPBuffer *buffer)

{

  pBuffer = buffer;

}

*/

const bool NVTexture::activate(int unit)

{

  if(!id)

    return false;

/*

  if(unit > -1)

    glActiveTextureARB(GL_TEXTURE0_ARB + unit);



  glEnable(target);

  glBindTexture(target, id);

 */

    if(unit > -1)

	{

		//LOG_PRINT_NONE("unit=%d\n",unit);

		//LOG_PRINT_NONE("id=%d\n",id);

#if 0//def USETEXTURES//???

		IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader());
		(IRenderer::GetRendererInstance()->setTexture("Base",id));
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());

/*

(3676): unit=0

(3676): id=39420416

Direct3D9: (ERROR) :NVTexture not created with this device. SetTexture failed.



D3D9 Helper: IDirect3DDevice9::SetTexture failed: D3DERR_INVALIDCALL

(3676): NVTexture::activate:119:hr=-2005530516

d:\c++gamesamples\d3dvsogl\d3d\dx9_naturescene\src\appearance\texture.cpp(119): D3DERR_INVALIDCALL

Invalid call

(hr=8876086c)

*/

/*

  if(pBuffer)

    wglBindTexImage3ARB(pBuffer->getPBufferHandle(), WGL_FRONT_LEFT_ARB);



  doTransform();

  */

  currUnit = unit;


/*
	(IRenderer::GetRendererInstance()->SetSamplerState(currUnit, D3DSAMP_ADDRESSU, m_clampS ));

	(IRenderer::GetRendererInstance()->SetSamplerState(currUnit, D3DSAMP_ADDRESSV, m_clampT ));

    (IRenderer::GetRendererInstance()->SetSamplerState(currUnit, D3DSAMP_MAGFILTER, m_magFilter ));

	(IRenderer::GetRendererInstance()->SetSamplerState(currUnit, D3DSAMP_MINFILTER, m_minFilter ));
*/
#endif

	}

	return true;

}



void NVTexture::copyCurrentBuffer(TextureID newTarget)

{

  #if 0

  if(!id)

    return;



//???  newTarget = (newTarget == 0) ? target : newTarget;

  activate();

#if 0

  glCopyTexSubImage32D(newTarget, 0, 0, 0, 0, 0, width7, height7);

#else

      D3DSURFACE_DESC desc;

	LPDIRECT3DSURFACE9 pTextureSurface;

    ((TextureID)id)->GetSurfaceLevel( 0, &pTextureSurface );

    pTextureSurface->GetDesc( &desc );

		SAFE_RELEASE(pTextureSurface);



	    (IRenderer::GetRendererInstance()->CreateTexture( desc.Width, desc.Height,1, D3DUSAGE_DYNAMIC, desc.Format, D3DPOOL_DEFAULT, &newTarget, 0));

			D3DLOCKED_RECT pLockedRect1;

			D3DLOCKED_RECT pLockedRect2;

			((IDirect3DTexture9*)newTarget)->LockRect(0,&pLockedRect2,0,0);

			((IDirect3DTexture9*)id)->LockRect(0,&pLockedRect1,0,0);

			//

			for (int x = 0; x < desc.Width; x++)

			{

				for (int y = 0; y < desc.Height; y++)

				{

					stx_memcpy(((BYTE *) pLockedRect2.pBits) + y * pLockedRect2.Pitch,((BYTE *) pLockedRect1.pBits) + y * pLockedRect1.Pitch, sizeof(pLockedRect2.Pitch));//???

				}

			}

			//

			((IDirect3DTexture9*)newTarget)->UnlockRect(0);

			((IDirect3DTexture9*)id)->UnlockRect(0);





#endif

  deactivate();

  #else

  LOG_PRINT_NONE("implement %s:%d\n");

  #endif

}



const bool NVTexture::deactivate()

{

  if(id > 0 )

  {

/*    if(currUnit > -1)

      glActiveTextureARB(GL_TEXTURE0_ARB + currUnit);

*/

#ifdef USETEXTURES//???

    if(currUnit > -1)

    {
/*
	(IRenderer::GetRendererInstance()->SetTexture(currUnit,0));

	(IRenderer::GetRendererInstance()->SetSamplerState(currUnit, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));

	(IRenderer::GetRendererInstance()->SetSamplerState(currUnit, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));

	//(IRenderer::GetRendererInstance()->SetSamplerState(currUnit, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP));

	(IRenderer::GetRendererInstance()->SetSamplerState(currUnit, D3DSAMP_MAGFILTER, D3DTEXF_POINT));

	(IRenderer::GetRendererInstance()->SetSamplerState(currUnit, D3DSAMP_MINFILTER, D3DTEXF_POINT));

	//(IRenderer::GetRendererInstance()->SetSamplerState(currUnit, D3DSAMP_MIPFILTER, D3DTEXF_NONE));
*/
		samplerState=IRenderer::GetRendererInstance()->Getnearest();
	}

#endif

//    undoTransform();

  //  glDisable(target);

    return true;

  }

  return false;

}



void NVTexture::setID(TextureID texID)

{

  static int unknown = 1;



  if(!texID)

  {

    LOG_PRINT_NONE("invalid texID, NVTexture::setID(unsigned int texID)\n");

    return;

  }



  TextureInfo *newTextureInfo = TexturesManager::getTextureInfo(texID),

	      *oldTextureInfo = TexturesManager::getTextureInfo(id);



  if(newTextureInfo)

    newTextureInfo->increaseUserCount();

  else

  {

    newTextureInfo = new TextureInfo(NSString("Unknown texture #") + unknown, texID);

    unknown++;

  }



  if(oldTextureInfo)

    oldTextureInfo->decreaseUserCount();



  id = texID;



 // int info;



  activate();

  //glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH , &info);

  //width7  = info;

  //glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &info);

  //height7 = info;


/*
    D3DSURFACE_DESC desc;

	//LPDIRECT3DSURFACE9 pTextureSurface;

    ((TextureID)id)->GetSurfaceLevel( 0, &surf );

    surf->GetDesc( &desc );
*/
	width7 = IRenderer::GetRendererInstance()->GetTextureWidth(id);

	height7 = IRenderer::GetRendererInstance()->GetTextureHeight(id);

		//SAFE_RELEASE(pTextureSurface);



  deactivate();

}



const TextureID NVTexture::getID() const

{

	//LOG_PRINT_NONE("tex id=%d\n",id);



//	if(id)id->AddRef();

  return id;

}
/*
const LPDIRECT3DSURFACE9 NVTexture::getSurf() const

{

	if(surf)surf->AddRef();

  return surf;

}
*/
/*

TextureCoordGenerator &NVTexture::getTextureCoordinatesGenerator()

{

  return textureCoordGenerator;

}

*/

void NVTexture::setTarget(TextureType target_)

{

  target = target_;

}



const unsigned int  NVTexture::getTarget() const { return target; }



void NVTexture::setTransform(const Transform3D &transform_)

{

  transform = transform_;

}



Transform3D &NVTexture::getTransform()

{

  return transform;

}



const unsigned int NVTexture::getHeight() const { return height7;  }

const unsigned int NVTexture::getWidth()	const { return width7;	 }

const unsigned int NVTexture::getDepth()	const { return depth;	}

bool NVTexture::loadXMLSettings(XMLElement *element)

{

  if(!isSuitable(element))

   return false;



  XMLElement  *child	 = 0,

	      *subChild  = 0;



  NSString       path;

  int	       aniso	 =	  0,

	       clampS	 = 0,//D3DTADDRESS_CLAMP,

	       clampT	 = 0,//D3DTADDRESS_CLAMP,

	       clampR	 = 0,//D3DTADDRESS_CLAMP,

	       magFilter = 0,//D3DTEXF_LINEAR,

	       minFilter = 0;//D3DTEXF_LINEAR;

  bool	       mipmap	 = false,

	       result	 = false;



  if(child = element->getChildByName("description"))

    return IOXMLObject::loadXMLSettings(child->getValuec());



  for(size_t i = 0; i < element->getChildrenCount(); i++)

  {

    if(!(child = element->getChild(i)))

      continue;



    const NSString &childName = child->getName();



    if(childName == "path")

    {

      path = child->getValuec();

      continue;

    }



    if(childName == "type")

    {

      target =	(TextureType)getXMLType(child->getValue());

      continue;

    }



    if(childName == "mipmap")

    {

      mipmap = (child->getValue() == "true");

      continue;

    }



    if(childName == "Wrap")

    {

      if((subChild = child->getChildByName("s")) ||

	 (subChild = child->getChildByName("u")))

	clampS	= getXMLWrapMode(subChild->getValue());



      if((subChild = child->getChildByName("t")) ||

	 (subChild = child->getChildByName("v")))

	clampT	= getXMLWrapMode(subChild->getValue());



      if((subChild = child->getChildByName("r")) ||

	 (subChild = child->getChildByName("w")))

	clampR	= getXMLWrapMode(subChild->getValue());

      continue;

    }



   if(childName == "Filter")

    {

      if(subChild = child->getChildByName("mag"))

	magFilter  = getXMLMagFilter(subChild->getValue());



      if(subChild = child->getChildByName("min"))

	minFilter  = getXMLMinFilter(subChild->getValue());



      if(subChild = child->getChildByName("aniso"))

	aniso  =  subChild->getValuei();

      continue;

    }

/*

    if(childName == "TextureCoordGenerator")

    {

      textureCoordGenerator.loadXMLSettings(child);

      continue;

    }*/

  }



  result = true;


/*
if(target == TEXTURE2D	    )

  //load2D(
  //RendererHLSLCg::addTexture(path,false,IRenderer->Get
  //, clampS, clampT, magFilter, minFilter, mipmap);

else

{

  LOG_PRINT_NONE("Unsupported texture type\n");

}
*/


 // if(result)

   // setAnisoLevel(aniso);

  return result;

}



int NVTexture::getXMLMagFilter(const NSString &value)

{

  if(value.getLength())

  {

    if(value == "NEAREST") return 0;//D3DTEXF_POINT;

  }

  return 0;//D3DTEXF_LINEAR;

}



int NVTexture::getXMLMinFilter(const NSString &value)

{

  if(value.getLength())

  {

    if(value == "NEAREST_MIPMAP_NEAREST") return 0;//D3DTEXF_POINT;//GL_NEAREST_MIPMAP_NEAREST;

    if(value == "LINEAR_MIPMAP_NEAREST")  return 0;//D3DTEXF_LINEAR;//GL_LINEAR_MIPMAP_NEAREST;

    if(value == "NEAREST_MIPMAP_LINEAR")  return 0;//D3DTEXF_POINT;//GL_NEAREST_MIPMAP_LINEAR;

    if(value == "LINEAR_MIPMAP_LINEAR")   return 0;//D3DTEXF_LINEAR;

    if(value == "NEAREST")		  return 0;//D3DTEXF_POINT;//GL_NEAREST;

  }

  return 0;//D3DTEXF_LINEAR;

}



int NVTexture::getXMLWrapMode(const NSString &value)

{

  if(value.getLength())

  {

    if(value == "CLAMP_TO_BORDER") return 0;//D3DTADDRESS_BORDER;

    if(value == "CLAMP_TO_EDGE")   return 0;//D3DTADDRESS_CLAMP;

    if(value == "REPEAT")	   return 0;//D3DTADDRESS_WRAP;

  }

  return 0;//D3DTADDRESS_CLAMP;

}



int NVTexture::getXMLType(const NSString &value)

{

  if(value.getLength())

  {

    if(value == "TEXTURE_1D")	    return TEXTURE1D;

    if(value == "TEXTURE_3D")	    return TEXTURE3D;

    if(value == "TEXTURE_CUBE_MAP") return TEXTURECUBE;

  }

  return TEXTURE2D;

}

/*

typedef enum _D3DTEXTUREADDRESS {

    D3DTADDRESS_WRAP		= 1,

    D3DTADDRESS_MIRROR		= 2,

    D3DTADDRESS_CLAMP		= 3,

    D3DTADDRESS_BORDER		= 4,

    D3DTADDRESS_MIRRORONCE	= 5,

    D3DTADDRESS_FORCE_DWORD	= 0x7fffffff, // force 32-bit size enum

} D3DTEXTUREADDRESS;

typedef enum _D3DTEXTUREFILTERTYPE

{

    D3DTEXF_NONE	    = 0,    // filtering disabled (valid for mip filter only)

    D3DTEXF_POINT	    = 1,    // nearest

    D3DTEXF_LINEAR	    = 2,    // linear interpolation

    D3DTEXF_ANISOTROPIC     = 3,    // anisotropic

    D3DTEXF_PYRAMIDALQUAD   = 6,    // 4-sample tent

    D3DTEXF_GAUSSIANQUAD    = 7,    // 4-sample gaussian

    D3DTEXF_FORCE_DWORD     = 0x7fffffff,   // force 32-bit size enum

} D3DTEXTUREFILTERTYPE;

*/



int NVTexture::getValidWrapMode(int clampNS)

{

  return 0;//(clampNS == 0,//D3DTADDRESS_CLAMP	      ) ? D3DTADDRESS_CLAMP	     :

	 //(clampNS == 0,//D3DTADDRESS_WRAP	    ) ? D3DTADDRESS_WRAP	  :

	 //(clampNS == 0,//D3DTADDRESS_CLAMP	) ? D3DTADDRESS_CLAMP	:

	// (clampNS == 0,//D3DTADDRESS_BORDER) ? D3DTADDRESS_BORDER : D3DTADDRESS_WRAP;

}



int NVTexture::getValidMagFilter(int filter)

{

  return 0;//(filter == 0,//D3DTEXF_POINT ) ? D3DTEXF_POINT  : D3DTEXF_LINEAR;

}



int NVTexture::getValidMinFilter(int filter)

{

  return 0;/*(filter == 0,//D3DTEXF_POINT		 ) ? D3DTEXF_POINT		  :

	 (filter == 0,//D3DTEXF_LINEAR   ) ? D3DTEXF_LINEAR;	 :

	 (filter == GL_NEAREST_MIPMAP_LINEAR  ) ? GL_NEAREST_MIPMAP_LINEAR  :

	 (filter == GL_LINEAR_MIPMAP_NEAREST  ) ? GL_LINEAR_MIPMAP_NEAREST  :

	 (filter == GL_NEAREST_MIPMAP_NEAREST ) ? GL_NEAREST_MIPMAP_NEAREST : D3DTEXF_LINEAR;*/

}



bool NVTexture::exportXMLSettings(ofstream &xmlFile  )

{

  if(!xmlFile.is_open())

    {LOG_PRINT_NONE("Cannot export NVTexture to XML: file not ready.\n");return false;}



  if(!id)

    {LOG_PRINT_NONE("Cannot export NVTexture to XML: invalid texture handle.\n");return false;}



  TextureInfo *textureInfo  = TexturesManager::getTextureInfo(id);



  if(!textureInfo)

    {LOG_PRINT_NONE(
"Cannot export NVTexture to XML: cannot locate a valid MediaInfo.\n");return false;}



  xmlFile << "	<NVTexture  type	      = \""

	  << ((target == TEXTURE2D	) ? "TEXTURE_2D\"\n" :

	      (target == TEXTURE1D	) ? "TEXTURE_1D\"\n" :

	      (target == TEXTURE3D	) ? "TEXTURE_3D\"\n" : "TEXTURE_CUBE_MAP\"\n")

	  << "		  path	      = \"" << textureInfo->getMediaPath() << "\"\n"

	  << "		  mipmap      = \"true\" > \n"

	  << "	  <Wrap   s	      = \"REPEAT\" \n"

	  << ((target == TEXTURE3D)  ? "	  r	      = \"REPEAT\" \n" : "")

	  << "		  t	      = \"REPEAT\" />\n"

	  << "	  <Filter mag	      = \"LINEAR\" \n"

	  << "		  min	      = \"LINEAR_MIPMAP_LINEAR\"\n"

	  << "		  aniso       =  " << aniso << "\" /> \n";



//  textureCoordGenerator.exportXMLSettings(xmlFile);



  xmlFile << "	</NVTexture>\n";

  return true;

}

/*

void NVTexture::setAnisoLevel(int level)

{

  if(!id || !Renderer::getMaxAnisotropicFilter() || !level)

    return;



  aniso = clampNS(stx_getClosestPowerOfTwo(level), 0, Renderer::getMaxAnisotropicFilter());

  activate();

	glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);

  deactivate();

}



unsigned int NVTexture::getAnisoLevel()

{

  return aniso;

}



*/

void NVTexture::destroy()

{

  TextureInfo *textureInfo = TexturesManager::getTextureInfo(id);



  if(textureInfo)

    textureInfo->decreaseUserCount();
/*
  else

  {

    SAFE_RELEASE(surf);

    SAFE_RELEASE(id);

  }*/



  TexturesManager::flushUnusedTextures();



  height7	  = 0;

  width7	  = 0;

  depth 	 = 0;

  id		 = 0;

//  surf		 = 0;

}



NVTexture::~NVTexture()

{

  destroy();

}







