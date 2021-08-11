//include "../Tools/Perlin.h"

#include "Texture.h"

//include <gl/glu.h>

using namespace NatureScene;

#include <Framework3/IRenderer.h>
bool NVTexture::create2DShell(const char* name,

			    unsigned int width7    , unsigned int height7,

			    FORMAT inFormat , FORMAT format,

			    unsigned int clampS   , unsigned int clampT,

			    unsigned int magFilter, unsigned int minFilter)

{

  if(!name)

    {LOG_PRINT("2D NVTexture Shell must have a valid name"));return false;}



  if(checkForRepeat(name))

    return true;



  destroy();

  target = TEXTURE2D;



  //LOG_PRINT("Loading new 2D Shell: width = ") + int(width7)

//		       + NSString(", height = ") + int(height7));

/*

  glGenTextures(1, &id);

  glBindTexture(GL_TEXTURE_2D, id);



  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getValidMagFilter(magFilter));

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getValidMinFilter(minFilter));

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,	getValidWrapMode(clampS));

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,	getValidWrapMode(clampT));

  glTexImage32D(GL_TEXTURE_2D, 0, inFormat, width7, height7, 0, format, GL_UNSIGNED_BYTE, 0);

 */


#if 0//???
	(g_pApp->m_pRenderer->CreateTexture( width7, height7, 1, D3DUSAGE_RENDERTARGET/*_D3DUSAGE_DYNAMIC*/,

		inFormat,

		D3DPOOL_DEFAULT, (IDirect3DTexture9 **)&id, 0));



  (g_pApp->m_pRenderer->SetTexture(0, id));

  ((LPDIRECT3DTEXTURE9)id)->GetSurfaceLevel(0,&this->surf);

#endif

  this->height7 = height7;

  this->width7	= width7;

  depth        = 1;



	  m_clampS=clampS;

	m_clampT=clampT;

	m_magFilter=magFilter;

	m_minFilter=minFilter;

	m_mipmap=1;



	//<+>Loading new 2D Shell: width = 512, height = 512

	LOG_PRINT("Loading new 2D Shell: width = ")+int(width7)+NSString(", height = ")+int(height7));



	LOG_PRINT("NVTexture::create2DShell tex %s %d OK\n",name,id);



  return finalizeLoading(name);

}



bool NVTexture::load2D(const char* name,

		     unsigned int clampS   , unsigned int clampT,

		     unsigned int magFilter, unsigned int minFilter,

		     bool  mipmap)

{

	LOG_PRINT("NVTexture::load2D tex %s\n",name);



	if(strcmp("water.png",name)==0)

	{

id=IRenderer::GetRendererInstance()->addImageLibTexture(
		"/NatureScene/Water/water.png"
		,false,
		IRenderer::GetRendererInstance()->Getlinear());

	LOG_PRINT("Loaded Texture2D file at -> /NatureScene/Water/water.png"));
  	LOG_PRINT("addImageLibTexture:")+"/NatureScene/Water/water.png:"+id);

	}

	else if(strcmp("clouds.png",name)==0)

	{

id=IRenderer::GetRendererInstance()->addImageLibTexture(
		(std::string("/NatureScene/Sky/clouds.png")).c_str()
		,false,
		IRenderer::GetRendererInstance()->Getlinear());

	LOG_PRINT("Loaded Texture2D file at -> /NatureScene/Sky/clouds.png"));
  	LOG_PRINT("addImageLibTexture:")+"/NatureScene/Sky/clouds.png:"+id);

	}

	else

	{

id=IRenderer::GetRendererInstance()->addImageLibTexture(
		name
		,false,
		IRenderer::GetRendererInstance()->Getlinear());
	LOG_PRINT("Loaded Texture2D file at -> ")+name);
  	LOG_PRINT("addImageLibTexture:")+name+":"+id);
	}



	m_clampS=clampS;

	m_clampT=clampT;

	m_magFilter=magFilter;

	m_minFilter=minFilter;

	m_mipmap=mipmap;



	LOG_PRINT("NVTexture::load2D tex %s %d OK\n",name,id);

	return true;

}

bool NVTexture::load2DImage3(const Image3& image,

			  unsigned int clampS	, unsigned int clampT,

			  unsigned int magFilter, unsigned int minFilter,

			  bool	mipmap)

{
#if 0
	LoadTexture(image,(TextureID *)&id);
#else
	//???id=image;
#endif

	m_clampS=clampS;

	m_clampT=clampT;

	m_magFilter=magFilter;

	m_minFilter=minFilter;

	m_mipmap=mipmap;



	LOG_PRINT("NVTexture::load2DImage3 %d OK\n",id);



	return true;

}

