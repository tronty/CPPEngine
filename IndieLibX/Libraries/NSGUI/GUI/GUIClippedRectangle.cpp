#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUIClippedRectangle.h"
#include <math.h>
GUIClippedRectangle::GUIClippedRectangle(int size)
{
  setTextureRectangle(0,0,1,1);
  setBordersColor(0.0f, 0.9f, 0.0f);
  setClipSize(size);
  setBGColor(0.413f, 0.7f, 0.52f, 0.75f);

  drawBackground = false;
  drawBounds     = false;
}

void  GUIClippedRectangle::setClipSize(int clipSizeArg)
{
  clipSize = clampNS(clipSizeArg, 0, 100);
}

int GUIClippedRectangle::getClipSize()
{
  return clipSize;
}

void GUIClippedRectangle::setVisibleBounds(bool visibleArg)
{
  drawBounds = visibleArg;
}

bool  GUIClippedRectangle::boundsVisible()
{
  return drawBounds;
}

bool GUIClippedRectangle::loadXMLClippedRectangleInfo(XMLElement *element)
{
  if(!element)
    return false;

  XMLElement *child    = 0;

  if(child = element->getChildByName("Texture"))
    texture.loadXMLSettings(child);

  if(child = element->getChildByName("TextureRectangle"))
    XMLElement::loadRX_GY_BZ_AWf(*child, textureRectangle);

  if(child = element->getChildByName("BGColor"))
    XMLElement::loadRX_GY_BZ_AWf(*child, bgColor);

  if(child = element->getChildByName("clipSize"))
    clipSize = child->getValuei();

  if(child = element->getChildByName("BordersColor"))
    XMLElement::loadRX_GY_BZf(*child, bordersColor);

  if(child = element->getChildByName("drawBackground"))
    enableBGColor(child->getValue() == "true");

  if(child = element->getChildByName("drawBounds"))
    setVisibleBounds(child->getValue() == "true");

  setTextureRectangle(textureRectangle);
  setBordersColor(bordersColor);
  setClipSize(clipSize);
  setBGColor(bgColor);
  return true;
}

void  GUIClippedRectangle::setBGColor(const D3DXFROMWINEVECTOR4& color)
{
  setBGColor(color.x, color.y, color.z, color.w);
}

void  GUIClippedRectangle::setBGColor(float r, float g, float b, float a)
{
  bgColor=D3DXFROMWINEVECTOR4(clampNS(r, 0.0f, 255.0f),
              clampNS(g, 0.0f, 255.0f),
              clampNS(b, 0.0f, 255.0f),
              clampNS(a, 0.0f, 255.0f));

  bgColor.x /= (bgColor.x > 1.0) ? 255.0f : 1.0f;
  bgColor.y /= (bgColor.y > 1.0) ? 255.0f : 1.0f;
  bgColor.z /= (bgColor.z > 1.0) ? 255.0f : 1.0f;
  bgColor.w /= (bgColor.w > 1.0) ? 255.0f : 1.0f;
}

void  GUIClippedRectangle::setBordersColor(const D3DXFROMWINEVECTOR3& color)
{
  setBordersColor(color.x, color.y, color.z);
}

const D3DXFROMWINEVECTOR3 &GUIClippedRectangle::getBordersColor()
{
  return bordersColor;
}

void  GUIClippedRectangle::setBordersColor(float r, float g, float b)
{
  bordersColor=D3DXFROMWINEVECTOR3(clampNS(r, 0.0f, 255.0f),
                   clampNS(g, 0.0f, 255.0f),
                   clampNS(b, 0.0f, 255.0f));
  bordersColor.x /= (bordersColor.x > 1.0) ? 255.0f : 1.0f;
  bordersColor.y /= (bordersColor.y > 1.0) ? 255.0f : 1.0f;
  bordersColor.z /= (bordersColor.z > 1.0) ? 255.0f : 1.0f;
}

void GUIClippedRectangle::renderClippedBounds(D3DXFROMWINEVECTOR3* Translation)
{

struct PCVertex {
	D3DXFROMWINEVECTOR2 vertex;
	//D3DXFROMWINEVECTOR4 color;
};

struct PCTVertex {
	D3DXFROMWINEVECTOR2 vertex;

	//D3DXFROMWINEVECTOR4 color;
	D3DXFROMWINEVECTOR2 texCoord;
};
	int nvertices=7;//???
	PCTVertex* PCTvertices=0;
	PCVertex* PCvertices=0;
	if(0)//texture.getID()!=-1 && drawBackground)
	{
		PCTvertices=new PCTVertex[nvertices];
		IRenderer::GetRendererInstance()->InitTex(IRenderer::GetRendererInstance()->GettexShader(), IRenderer::GetRendererInstance()->GettexVF());
		IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader());
		IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GettexVF());
	for(unsigned int i=0;i<nvertices;i++)
	{
		////LOG_PRINT_NONE("vx=%d\n",vertices[i].x);
		////LOG_PRINT_NONE("vy=%d\n",vertices[i].y);

		PCTvertices[i].vertex=D3DXFROMWINEVECTOR2(
			fabs(float(vertices[i].x)),
			fabs(float(vertices[i].y))
			);
		if(Translation)
		{
			PCTvertices[i].vertex.x+=Translation->x;
			PCTvertices[i].vertex.y+=Translation->y;
		}
		PCTvertices[i].texCoord=D3DXFROMWINEVECTOR2(
			texCoords[i].x,
			texCoords[i].y
			);
	}
	}
	else
	{
		PCvertices=new PCVertex[nvertices];
		IRenderer::GetRendererInstance()->InitPlain(IRenderer::GetRendererInstance()->GetplainShader(), IRenderer::GetRendererInstance()->GetplainVF());
		IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GetplainShader());
		IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GetplainVF());
	for(unsigned int i=0;i<nvertices;i++)
	{
		////LOG_PRINT_NONE("vx=%d\n",vertices[i].x);
		////LOG_PRINT_NONE("vy=%d\n",vertices[i].y);

		PCvertices[i].vertex=D3DXFROMWINEVECTOR2(
			fabs(float(vertices[i].x)),
			fabs(float(vertices[i].y))
			);
		if(Translation)
		{
			PCvertices[i].vertex.x+=Translation->x;
			PCvertices[i].vertex.y+=Translation->y;
		}
	}
	}
	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias", IRenderer::GetRendererInstance()->GetscaleBias2D());
  if(drawBackground || drawBounds)
  {
    //IRenderer::GetRendererInstance()->VertexPointer(2, /*GL_INT,*/ 0, &vertices[0]);
    //glEnableClientState(GL_VERTEX_ARRAY);

    if(texture.getID()!=-1 && drawBackground)
    {
#if 0
      //IRenderer::GetRendererInstance()->TexCoordPointer(2, /*GL_FLOAT,*/ 0, &texCoords[0]) ;
      //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        IRenderer::GetRendererInstance()->InitTex(IRenderer::GetRendererInstance()->GettexShader(),IRenderer::GetRendererInstance()->GettexVF());
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GettexVF());
	IRenderer::GetRendererInstance()->setTexture("Base",texture.getID());
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1.0f,1.0f,1.0f,1.0f));
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias", IRenderer::GetRendererInstance()->GetscaleBias2D());
	//LOG_FNLN_NONE;
#endif
    }
  }

  if(drawBackground)
  {
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", bgColor);
    //glDrawArrays(GL_TRIANGLE_FAN, 0, 7);

	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());

		if(PCTvertices){
		unsigned int ret=IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN,
		//IRenderer::GetRendererInstance()->getPrimitiveCountR(PRIM_TRIANGLE_FAN,nvertices),
		nvertices-2,
		PCTvertices, PCTvertices, sizeof(PCTVertex));
		if(ret){LOG_PRINT_NONE("%s:%s:%d:ret=%x\n",__FILE__,__FUNCTION__,__LINE__,ret);}
		}else{
		unsigned int ret=IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN,
		//IRenderer::GetRendererInstance()->getPrimitiveCountR(PRIM_TRIANGLE_FAN,nvertices),
		nvertices-2,
		PCvertices, PCvertices, sizeof(PCVertex));
		if(ret)
		{
			LOG_PRINT_NONE("%s:%s:%d:ret=%x\n",__FILE__,__FUNCTION__,__LINE__,ret);
			LOG_PRINT_NONE("nvertices=%x\n",nvertices);
			LOG_PRINT_NONE("PCvertices=%x\n",PCvertices);
		}}


	//glDisable(GL_BLEND);
/*
    if(texture!=-1)
    {
      texture.deactivate();
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }*/
  }

  if(drawBounds)
  {
#if 0
	D3DXFROMWINEVECTOR4 col(1.0f, 0.0f, 0.0f, 1.0f);
#else
	D3DXFROMWINEVECTOR4 col(bordersColor.x,bordersColor.y,bordersColor.z,1);
#endif

    IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", col);
    //glDrawArrays(GL_LINE_STRIP, 0, 7);

	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());

#if 0//???
		if(PCTvertices)
		{
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_LINE_STRIP,
		nvertices-1,
		PCTvertices, PCTvertices, sizeof(PCTVertex));
		}
		else
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_LINE_STRIP,
		nvertices-1,
		PCvertices, PCvertices, sizeof(PCVertex));
#endif

  }

	delete[] PCTvertices;
	delete[] PCvertices;
	/*
	int i1=getPrimitiveCount(g_prim,nvertices);
	int i2=getPrimitiveCountR(g_prim,nvertices);
	//LOG_PRINT_NONE("PC=%d\n",getPrimitiveCount(g_prim,nvertices));
	//LOG_PRINT_NONE("PCR=%d\n",getPrimitiveCountR(g_prim,nvertices));
*/

  //IRenderer::GetRendererInstance()->Color3f(1,1,1);
  //glDisableClientState(GL_VERTEX_ARRAY);
}

void  GUIClippedRectangle::computeClippedBounds(const Tuple4i &windowBounds)
{
  float yTexOffset    = float(clipSize)/(windowBounds.w - windowBounds.y),
        xTexOffset    = float(clipSize)/(windowBounds.z - windowBounds.x);

  xTexOffset *= textureRectangle.z - textureRectangle.x;
  yTexOffset *= textureRectangle.w - textureRectangle.y;
/*
		   	   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
*/
/*
		  	   x	  y     tx    ty
	float v[16] =  { -1.0f, -1.0f, 0.0f, 1.0f,  
			  1.0f, -1.0f, 1.0f, 1.0f,   
			  1.0f,  1.0f, 1.0f, 0.0f,  
			 -1.0f,  1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, &v[0], &v[0], 4*sizeof(float));
*/

  vertices[0]=Tuple2i(windowBounds.x, windowBounds.y + clipSize);
  vertices[1]=Tuple2i(windowBounds.x, windowBounds.w);
  vertices[2]=Tuple2i(windowBounds.z - clipSize, windowBounds.w);
  vertices[3]=Tuple2i(windowBounds.z, windowBounds.w - clipSize);
  vertices[4]=Tuple2i(windowBounds.z, windowBounds.y);
  vertices[5]=Tuple2i(windowBounds.x + clipSize, windowBounds.y);
  vertices[6]=Tuple2i(windowBounds.x, windowBounds.y + clipSize);

  texCoords[0]=D3DXFROMWINEVECTOR2(textureRectangle.x             , textureRectangle.w - yTexOffset);
  texCoords[1]=D3DXFROMWINEVECTOR2(textureRectangle.x             , textureRectangle.y);
  texCoords[2]=D3DXFROMWINEVECTOR2(textureRectangle.z - xTexOffset, textureRectangle.y             );
  texCoords[3]=D3DXFROMWINEVECTOR2(textureRectangle.z             , textureRectangle.y + yTexOffset);
  texCoords[4]=D3DXFROMWINEVECTOR2(textureRectangle.z             , textureRectangle.w);
  texCoords[5]=D3DXFROMWINEVECTOR2(textureRectangle.x + xTexOffset, textureRectangle.w             );
  texCoords[6]=D3DXFROMWINEVECTOR2(textureRectangle.x             , textureRectangle.w - yTexOffset);
#if 0
/*
GL_TRIANGLE_STRIP:

Indices:     0 1 2 3 4 5 ...
Triangles:  {0 1 2}
              {1 2 3}  drawing order is (2 1 3) to maintain proper winding
                {2 3 4}
                  {3 4 5}  drawing order is (4 3 5) to maintain proper winding

GL_TRIANGLE_FAN:

Indices:     0 1 2 3 4 5 ...
Triangles:  {0 1 2}
            {0} {2 3}
            {0}   {3 4}
            {0}     {4 5}
*/
  tmpvertices=vertices[5];
  vertices[2]=tmpvertices;
  vertices[3]=tmpvertices;
  tmpvertices=vertices[2];
  vertices[5]=tmpvertices;
  tmpvertices=vertices[1];
  vertices[4]=tmpvertices;

  tmptexCoords=texCoords[5];
  texCoords[2]=tmptexCoords;
  texCoords[3]=tmptexCoords;
  tmptexCoords=texCoords[2];
  texCoords[5]=tmptexCoords;
  tmptexCoords=texCoords[1];
  texCoords[4]=tmptexCoords;
#endif
}

void  GUIClippedRectangle::enableBGColor(bool enable)
{
  drawBackground = enable;
}

void GUIClippedRectangle::setTextureRectangle(const D3DXFROMWINEVECTOR4 &tr)
{
  setTextureRectangle(tr.x, tr.y, tr.z, tr.w);
}

void GUIClippedRectangle::setTextureRectangle(float x, float y, float z, float w)
{
  if(x > 1.0f || y > 1.0f || z > 1.0f || w > 1.0f)
  if(texture.getID()!=-1)
  {
    x = clampNS(x, 0.0f, float(texture.getWidth() ));
    y = clampNS(y, 0.0f, float(texture.getHeight()));
    z = clampNS(z, 0.0f, float(texture.getWidth()));
    w = clampNS(w, 0.0f, float(texture.getHeight()));

    x /= texture.getWidth();
    z /= texture.getWidth();

    w /= texture.getHeight();
    y /= texture.getHeight();

  }

  textureRectangle=D3DXFROMWINEVECTOR4(clampNS(x, 0.0f, 1.0f),
                       clampNS(y, 0.0f, 1.0f),
                       clampNS(z, 0.0f, 1.0f),
                       clampNS(w, 0.0f, 1.0f));
}

const  D3DXFROMWINEVECTOR4 &GUIClippedRectangle::getTextureRectangle()
{
  return textureRectangle;
}

NVTexture &GUIClippedRectangle::getTexture()
{
  return texture;
}

void  GUIClippedRectangle::setTexture(const NVTexture & textureArg)
{
  texture = textureArg;
}

bool  GUIClippedRectangle::isBGColorOn()
{
  return drawBackground;
}

const D3DXFROMWINEVECTOR4 &GUIClippedRectangle::getBGColor()
{
  return bgColor;
}

const Tuple2i *GUIClippedRectangle::getVertices() const
{
  return vertices;
}

const D3DXFROMWINEVECTOR2 *GUIClippedRectangle::getTexCoords() const
{
  return texCoords;
}
