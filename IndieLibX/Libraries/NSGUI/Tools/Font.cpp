#include <Framework3/IRenderer.h>
/* CFont
 *
 * Copyright (C) 2003-2004, Alexander Zaprjagaev <frustum@frustum.org>
 * Modified by Abdul Bezrati <abezrati@hotmail.com>
 */

#include "Font.h"
#include "../Managers/MediaPathManager.h"
#include "../Tools/MathUtils.h"

//include "../Tools/Timer.h"

#define __PRINT__ 1
CFont::CFont()
{
  fontBaseRange = 0;
  fontHeight    = 0;
  stx_memset(spaces, 0, 256);
}

CFont::~CFont()
{
  //if(fontBaseRange)
//    glDeleteLists(fontBaseRange, 256);

  fontBaseRange = 0;
//  fontTexture.destroy();
}

bool CFont::load(NSString fontPath)
{
  NSString fontlocation = //MediaPathManager::lookUpMediaPath(
	  fontPath;
  Image3 image;
  int   width  = 0,
        height = 0;

	
	//LOG_PRINT_NONE("fontPath:%s\n",fontPath);

  if(!image.loadImageLibImage(fontlocation,false))
    return false;//LOG_PRINT_NONE("Can't load font file");

  if(getChannelCount(image.format) != 4)
    return false;//LOG_PRINT_NONE("RGBA image required for font");

  //if(!
	  fontTexture=IRenderer::GetRendererInstance()->addTexture(&image, false,
		  IRenderer::GetRendererInstance()->GetlinearClamp());
		  //GL_CLAMP, GL_CLAMP, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true))
   //return false;//LOG_PRINT_NONE("Can't create font texture");

  unsigned char* data = (unsigned char*)image.getPixels();

  height = image.getHeight();
  width  = image.getWidth();

  int spacey[256];
  stx_memset(spacey, 0, 256);

  int size   = width,
      step   = size / 16;

  int x,y,i,j,k;
  for(y = 0, i = 0; y < 16; y++)
  {
    for(x = 0; x < 16; x++, i++)
    {
      unsigned char* ptr;
      spacey[i] = 0;
      for(j = 0; j < step; j++)
      {
        ptr = data + (size * y * step + x * step + j) * 4;
        for(k = 0; k < step; k++)
        {
          if(*(ptr + 3) != 0) break;
          ptr += size * 4;
        }
        if(k != step) break;
        spacey[i]++;
      }
      spaces[i] = 0;
      if(spacey[i] == step)
        continue;
      for(j = step - 1; j >= 0; j--)
      {
        ptr = data + (size * y * step + x * step + j) * 4;
        for(k = 0; k < step; k++) {
          if(*(ptr + 3) != 0) break;
          ptr += size * 4;
        }
        if(k != step) break;
        spaces[i]++;
      }
      spaces[i] = step - spacey[i] - spaces[i];
    }
  }

  //if(fontBaseRange)
//    glDeleteLists(fontBaseRange, 256);
#if 0
  IRenderer::GetRendererInstance()->InitTex(IRenderer::GetRendererInstance()->GettexShader(),
	  IRenderer::GetRendererInstance()->GettexVF());
  IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader());
  IRenderer::GetRendererInstance()->setTexture("Base",fontTexture);
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1.0f,1.0f,1.0f,1.0f));
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());
  //fontBaseRange = glGenLists(256);
#endif
  spaces[' '] = step/3;

  for(y = 0, i = 0; y < 16; y++)
  {
    for(x = 0; x < 16; x++, i++)
    {
      float s = (float)x / 16.0f + (float)spacey[i] / (float)size;
      float t = (float)y / 16.0f;
      float ds = (float)spaces[i] / (float)size;
      float dt = 1.0f/ 16.0;
  
      //glNewList(fontBaseRange + i,GL_COMPILE);
#if 0
      IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLE_STRIP);
#if 1
      IRenderer::GetRendererInstance()->TexCoord2f(s + ds,t + dt); IRenderer::GetRendererInstance()->Vertex2i(spaces[i],step);
      IRenderer::GetRendererInstance()->TexCoord2f(s + ds,t     ); IRenderer::GetRendererInstance()->Vertex2i(spaces[i],   0);
      IRenderer::GetRendererInstance()->TexCoord2f(s     ,t + dt); IRenderer::GetRendererInstance()->Vertex2i(0        ,step);
      IRenderer::GetRendererInstance()->TexCoord2f(s     ,t     ); IRenderer::GetRendererInstance()->Vertex2i(0        ,   0);
#else
      IRenderer::GetRendererInstance()->TexCoord2f(s + ds,t     ); IRenderer::GetRendererInstance()->Vertex2i(spaces[i],   0);
      IRenderer::GetRendererInstance()->TexCoord2f(s + ds,t + dt); IRenderer::GetRendererInstance()->Vertex2i(spaces[i],step);
      IRenderer::GetRendererInstance()->TexCoord2f(s     ,t     ); IRenderer::GetRendererInstance()->Vertex2i(0        ,   0);
      IRenderer::GetRendererInstance()->TexCoord2f(s     ,t + dt); IRenderer::GetRendererInstance()->Vertex2i(0        ,step);
#endif
      IRenderer::GetRendererInstance()->End();
	  #endif
//???      glTranslatef(float(spaces[i]),0,0);
//      IRenderer::GetRendererInstance()->EndList();
    }
  }
//  fontTexture.deactivate();
  fontHeight = height/16;

  return true;
}

Tuple2i CFont::getStringDimensions(NSString &string)
{
  Tuple2i dimensions(0, fontHeight);

  for(size_t i = 0; i < string.getLength(); i++)
    dimensions.x += spaces[int(string[i])];

  return dimensions;
}

int  CFont::getMaxFittingLength(const NSString &string, int bounds)
{
  int index         = 0,
      currentLength = 0;

  if(!bounds || !string.getLength() || -1!=fontTexture)
    return 0;

  for(size_t i = 0; i < string.getLength(); i++)
    if(currentLength < bounds)
    {
      currentLength += spaces[int(string[i])];
      index++;
    }
    else 
     break;

  return (currentLength < bounds) ? index : index - 1;
}


void CFont::printProportional(float widthRatio, float heightRatio,
                             float width,      float height,
                             float r, float g, float b,
                             NSString &string)
{
#if 0
  Tuple4i viewport;

  if(!string.getLength())
    return;

  glGetIntegerv(GL_VIEWPORT, viewport);

  Tuple2i dimensions = getStringDimensions(string);

  float xPosition = (viewport.z - dimensions.x*width)*widthRatio,
        yPosition = (viewport.w - dimensions.y*height)*heightRatio;

  print(xPosition, yPosition, width, height, r, g, b, string);
#else
  Tuple2i dimensions = getStringDimensions(string);

	D3DFROMWINEVIEWPORT9 viewport=IRenderer::GetRendererInstance()->GetViewport();
  float xPosition = (viewport.Width - dimensions.x*width)*widthRatio,
        yPosition = (viewport.Height - dimensions.y*height)*heightRatio;
/*
	char stringdata[1024];
	stx_memcpy(stringdata,string.data,string.getLength());
	stringdata[string.getLength()]='\0';
	*/
  #ifdef __PRINT__
	IRenderer::GetRendererInstance()->drawText(string.c_str(), xPosition, yPosition, 
		//15, 18,
		fontHeight*0.83f,fontHeight,
		IRenderer::GetRendererInstance()->GetdefaultFont(), 
		IRenderer::GetRendererInstance()->GetlinearClamp(), 
		IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
		IRenderer::GetRendererInstance()->GetnoDepthTest());
#endif
#endif
}

void CFont::print(float xPosition,  float yPosition,
                 float width,      float height,
                 float r, float g, float b,
                 NSString &string)
{
#if 0
  int stringLength =  string.getLength();
  if(!stringLength || -1!=fontTexture)
    return;

  IRenderer::GetRendererInstance()->Color3f(r,g,b);
  IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader());
  IRenderer::GetRendererInstance()->setTexture("Base",fontTexture);
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1.0f,1.0f,1.0f,1.0f));
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());
//  glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#if 0
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(xPosition, yPosition, 0);

  glListBase(fontBaseRange);
  glScalef(width, height,1.0f);
  glCallLists(stringLength,GL_UNSIGNED_BYTE, string);
  glPopMatrix();
//  glDisable(GL_BLEND);
#endif
  IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader());
  IRenderer::GetRendererInstance()->setTexture("Base",fontTexture);
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1.0f,1.0f,1.0f,1.0f));
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());
  IRenderer::GetRendererInstance()->Color3f(1,1,1);
#else
	/*
	char stringdata[1024];
	stx_memcpy(stringdata,string.data,string.getLength());
	stringdata[string.getLength()]='\0';*/
	#ifdef __PRINT__
	IRenderer::GetRendererInstance()->drawText(string.c_str(), xPosition, yPosition, 
		//15, 18,
		fontHeight*0.83f,fontHeight,
		IRenderer::GetRendererInstance()->GetdefaultFont(), 
		IRenderer::GetRendererInstance()->GetlinearClamp(), 
		IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
		IRenderer::GetRendererInstance()->GetnoDepthTest());
#endif
#endif
}

int  CFont::getHeight()
{
  return fontHeight;
}

void CFont::printSubString(float xPosition,  float yPosition,
                          float width,      float height,
                          float r, float g, float b,
                          int   start, int end,
                          NSString &string)
{
#if 0
  if(start >= end)
    return;

  int stringLength =  string.getLength();
  if(stringLength  < end)
    return;

  IRenderer::GetRendererInstance()->Color3f(r,g,b);
  IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader());
  IRenderer::GetRendererInstance()->setTexture("Base",fontTexture);
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1.0f,1.0f,1.0f,1.0f));
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());
//  glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#if 0
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(xPosition, yPosition, 0);

  glListBase(fontBaseRange);
  glScalef(width, height,1.0f);
  glCallLists(end - start,GL_UNSIGNED_BYTE, string + start);
  glPopMatrix();
//  glDisable(GL_BLEND);
#endif
//  fontTexture.deactivate();
  IRenderer::GetRendererInstance()->Color3f(1,1,1);
#else
	/*
	//LOG_PRINT_NONE("l=%d\n",string.getLength());
	//LOG_PRINT_NONE("s=%s\n",string.data);
	char stringdata[1024];
	stx_memcpy(stringdata,string.data,string.getLength());
	stringdata[string.getLength()]='\0';*/
#ifdef __PRINT__
#if 0
	LOG_PRINT_NONE("string=%s\n", string.c_str());
	LOG_PRINT_NONE("xPosition=%d\n", xPosition);
	LOG_PRINT_NONE("yPosition=%d\n", yPosition); 
	LOG_PRINT_NONE("fontHeight*0.83f=%f\n", fontHeight*0.83f);
	LOG_PRINT_NONE("fontHeight=%f\n", fontHeight);
#endif
	IRenderer::GetRendererInstance()->drawText(string.c_str(), xPosition, yPosition, 
		//15, 18,
		fontHeight*0.83f,fontHeight,
		IRenderer::GetRendererInstance()->GetdefaultFont(), 
		IRenderer::GetRendererInstance()->GetlinearClamp(), 
		IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
		IRenderer::GetRendererInstance()->GetnoDepthTest());
#endif
#endif
}

const   int *CFont::getCharHorizontalGlyphs() const{ return spaces; }

