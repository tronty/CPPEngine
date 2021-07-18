#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUIMaterialSurface.h"

GUIMaterialSurface::GUIMaterialSurface(NSString callback) :
                                       GUIAlphaElement(callback), GUIClippedRectangle()
{
  setBordersColor(0.0f, 0.75f, 0.0f);
  setDimensions(128, 128);
  widgetType = MATERIAL_SURFACE;
}

bool GUIMaterialSurface::loadXMLSettings(XMLElement *element)
{
  if(!element || element->getName() != "MaterialSurface")
    return false;//LOG_PRINT_NONE("Need a MaterialSurface node in the xml file");

  XMLElement *node     = 0,
             *child    = 0,
             *subChild = 0;

  int         textureIndex = 0;

  for(size_t i = 0; i < element->getChildrenCount(); i++)
  {
    node = element->getChild(i);
    if(!node)
      continue;

    const NSString &name = node->getName();

    if(name == "Texture")
    {
      NVTexture texture;
      if(texture.loadXMLSettings(node))
        appearance.setTexture(textureIndex++, texture.getID());
      continue;
    }
  }
  return GUIAlphaElement::loadXMLSettings(element) &&
         GUIClippedRectangle::loadXMLClippedRectangleInfo(element);
}

Appearance *GUIMaterialSurface::getAppearance()
{
  return &appearance;
}

void  GUIMaterialSurface::bindAppearance(bool enable)
{
// Material *materialPointer = appearance.getMaterial();
  TextureID  texturePointer;
  bool     activeTexture = false;

  if(enable)
  {
    //if(materialPointer)
  //    materialPointer->bindMaterial();

    IRenderer::GetRendererInstance()->Color4fv(appearance.getColor());
    if(appearance.shadersOn())
      IRenderer::GetRendererInstance()->setShader(appearance.getShaders());
	//IRenderer::GetRendererInstance()->setShaderConstant4f("translation",
	//	IRenderer::GetRendererInstance()->GetTranslation());

    for(int i =0; i < 8; i++)
    {
      texturePointer = appearance.getTexture(i);
      if(texturePointer==-1)
        continue;

     // glClientActiveTextureARB(GL_TEXTURE0_ARB + i);
      //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      IRenderer::GetRendererInstance()->TexCoordPointer(2, /*GL_FLOAT,*/ 0, texCoords[0]) ;
#if 0
	  DBG_HALT;
      	IRenderer::GetRendererInstance()->InitTex(IRenderer::GetRendererInstance()->GettexShader(),IRenderer::GetRendererInstance()->GettexVF());
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GettexVF());
	IRenderer::GetRendererInstance()->setTexture("Base",texturePointer);
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1.0f,1.0f,1.0f,1.0f));
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias", IRenderer::GetRendererInstance()->GetscaleBias2D());
	LOG_FNLN_NONE;
        //->activate(i);
#endif
    }
  }/*
  else
  {
    for(int i = 7; i >-1; i--)
    {
      texturePointer = appearance.getTexture(i);
      if(texturePointer==-1)
        continue;

     // glClientActiveTextureARB(GL_TEXTURE0_ARB + i);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);

      texturePointer->deactivate();
    }
 //   if(appearance.shadersOn())
   //   appearance.getShaders().deactivate();
  }*/
}

void GUIMaterialSurface::render(float clockTick)
{
  if(!parent || !visible)
    return;

 // if(update)
  {
    computeClippedBounds(getWindowBounds());
  }

  bindAppearance(true);
  IRenderer::GetRendererInstance()->VertexPointer(2, /*GL_INT,*/ 0, &vertices[0]);
//  glEnableClientState(GL_VERTEX_ARRAY);
  //???glDrawArrays(GL_TRIANGLE_FAN, 0, 9);
  //glDisableClientState(GL_VERTEX_ARRAY);
  bindAppearance(false);

  if(drawBounds)
  {
    IRenderer::GetRendererInstance()->Color3fv(bordersColor);
    IRenderer::GetRendererInstance()->VertexPointer(2, /*GL_INT,*/ 0, &vertices[0]);
    //glEnableClientState(GL_VERTEX_ARRAY);
//???    glDrawArrays(GL_LINE_STRIP, 0, 7);
    //glDisableClientState(GL_VERTEX_ARRAY);
  }
  IRenderer::GetRendererInstance()->Color3f(1,1,1);
}
