#include <Framework3/IRenderer.h>
#include "BlendAttributes.h"

#include "../Managers/MediaPathManager.h"

using namespace NatureScene;

BlendAttributes::BlendAttributes(int sourceArg,

                                 int destinationArg)

                : IOXMLObject("BlendAttributes")

{

  destination = destinationArg;

  source      = sourceArg;

}



BlendAttributes::BlendAttributes(const BlendAttributes &copy)

                : IOXMLObject("BlendAttributes")

{

  destination = copy.destination;

  source      = copy.source;

}



BlendAttributes &BlendAttributes::operator = (const BlendAttributes &copy)

{

  destination = copy.destination;

  source      = copy.source;

  return *this;

}



bool  BlendAttributes::loadXMLSettings(XMLElement *element)

{

  if(!isSuitable(element))

    return false;



  XMLElement  *subChild  = 0,

              *child     = 0;

  NSString       path;



  if(child = element->getChildByName("description"))

    return IOXMLObject::loadXMLSettings(child->getValuec());



  for(size_t i = 0; i < element->getChildrenCount(); i++)

  {

    if(!(child = element->getChild(i)))

      continue;



    const NSString &childName = child->getName();



    if(childName == "destination")

      setDestination(getXMLDestinationFactor(child->getValue()));



    if(childName == "source")

      setSource(getXMLSourceFactor(child->getValue()));

  }



  return true;

}



bool  BlendAttributes::exportXMLSettings(ofstream &xmlFile  )

{

  if(!xmlFile.is_open())

    {LOG_PRINT_NONE();return false;}



  if(destination == -1.0f || source == -1.0f)

    {LOG_PRINT_NONE();return false;}


/*
   xmlFile << "  <BlendAttributes  destination = \"" <<((destination ==  D3DBLEND_INVSRCCOLOR) ? "ONE_MINUS_SRC_COLOR\"\n" :

                                                        (destination ==  D3DBLEND_INVSRCALPHA) ? "ONE_MINUS_SRC_ALPHA\"\n" :

                                                        (destination ==  D3DBLEND_INVDESTALPHA) ? "ONE_MINUS_DST_ALPHA\"\n" :

                                                        (destination ==  D3DBLEND_SRCCOLOR)           ? "SRC_COLOR\"\n"           :

                                                        (destination ==  D3DBLEND_SRCALPHA)           ? "SRC_ALPHA\"\n"           :

                                                        (destination ==  D3DBLEND_DESTALPHA)           ? "DST_ALPHA\"\n"           :

                                                        (destination ==  D3DBLEND_ZERO)                ? "ZERO\"\n"                :  "ONE\"\n")<<



              "                    source      = \"" <<((source ==  D3DBLEND_INVDESTCOLOR) ? "ONE_MINUS_DST_COLOR\"/> \n" :

                                                        (source ==  D3DBLEND_INVDESTALPHA) ? "ONE_MINUS_DST_ALPHA\"/> \n" :

                                                        (source ==  D3DBLEND_INVSRCALPHA) ? "ONE_MINUS_SRC_ALPHA\"/> \n" :

                                                        (source ==  D3DBLEND_SRCALPHASAT)  ? "SRC_ALPHA_SATURATE\" />\n"  :

                                                        (source ==  D3DBLEND_DESTALPHA)           ? "DST_ALPHA\" />\n"           :

                                                        (source ==  D3DBLEND_SRCALPHA)           ? "SRC_ALPHA\" />\n"           :

                                                        (source ==  D3DBLEND_DESTCOLOR)           ? "DST_COLOR\" />\n"           :

                                                        (source ==  D3DBLEND_ZERO)                ? "ZERO\" />\n"                :   "ONE\" />\n");
*/
  return true;

}



void BlendAttributes::setAttributes(int source, int destination)

{

  setDestination(destination);

  setSource     (source);

}



void  BlendAttributes::setSource(int sourceArg)

{
/*
  switch(sourceArg)

  {

    case D3DBLEND_ZERO:

    case D3DBLEND_ONE:

    case D3DBLEND_DESTCOLOR:

    case D3DBLEND_INVDESTCOLOR:

    case D3DBLEND_SRCALPHA:

    case D3DBLEND_INVSRCALPHA:

    case D3DBLEND_DESTALPHA:

    case D3DBLEND_INVDESTALPHA:

    case D3DBLEND_SRCALPHASAT:

      source = sourceArg;

    return;

  }
*/


  LOG_PRINT_NONE();

  source = -1;

}



int   BlendAttributes::getSource()

{

  return source;

}



void  BlendAttributes::setDestination(int destinationArg)

{
/*
  switch(destinationArg)

  {

    case D3DBLEND_ZERO:

    case D3DBLEND_ONE:

    case D3DBLEND_SRCCOLOR:

    case D3DBLEND_INVSRCCOLOR:

    case D3DBLEND_SRCALPHA:

    case D3DBLEND_INVSRCALPHA:

    case D3DBLEND_DESTALPHA:

    case D3DBLEND_INVDESTALPHA:

      destination = destinationArg;

    return;

  }
*/


  LOG_PRINT_NONE();

  destination = -1;

}



int BlendAttributes::getDestination()

{

  return destination;

}



void  BlendAttributes::apply()

{

  if(source != -1.0 && destination != -1)

  {
#if 0
       (g_pApp->m_pRenderer->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));

       //(g_pApp->m_pRenderer->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1));

       //(g_pApp->m_pRenderer->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE));

       (g_pApp->m_pRenderer->SetRenderState( D3DRS_SRCBLEND, source ));

       (g_pApp->m_pRenderer->SetRenderState( D3DRS_DESTBLEND, destination ));
#elif 0
	if(1)
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	else
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());
	IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetnoDepthTest());
#endif
  }

}



void  BlendAttributes::stop()

{
/*
  if(source != -1.0 && destination != -1)

  {

    //glDisable(GL_BLEND);

	       (g_pApp->m_pRenderer->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE));

  }
*/
}



int   BlendAttributes::getXMLDestinationFactor(const NSString &value)

{
/*
  if(value.getLength())

  {

    if(value == "ONE_MINUS_SRC_COLOR") return D3DBLEND_INVSRCCOLOR;

    if(value == "ONE_MINUS_SRC_ALPHA") return D3DBLEND_INVSRCALPHA;

    if(value == "ONE_MINUS_DST_ALPHA") return D3DBLEND_INVDESTALPHA;

    if(value == "SRC_COLOR")           return D3DBLEND_SRCCOLOR;

    if(value == "SRC_ALPHA")           return D3DBLEND_SRCALPHA;

    if(value == "DST_ALPHA")           return D3DBLEND_DESTALPHA;

    if(value == "ZERO")                return D3DBLEND_ZERO;

    if(value == "ONE")                 return D3DBLEND_ONE;

  }
*/
  return -1;

}



int   BlendAttributes::getXMLSourceFactor(const NSString &value)

{
/*
  if(value.getLength())

  {

    if(value == "ONE_MINUS_DST_COLOR") return D3DBLEND_INVDESTCOLOR;

    if(value == "ONE_MINUS_DST_ALPHA") return D3DBLEND_INVDESTALPHA;

    if(value == "ONE_MINUS_SRC_ALPHA") return D3DBLEND_INVSRCALPHA;

    if(value == "SRC_ALPHA_SATURATE")  return D3DBLEND_SRCALPHASAT;

    if(value == "DST_ALPHA")           return D3DBLEND_DESTALPHA;

    if(value == "SRC_ALPHA")           return D3DBLEND_SRCALPHA;

    if(value == "DST_COLOR")           return D3DBLEND_DESTCOLOR;

    if(value == "ZERO")                return D3DBLEND_ZERO;

    if(value == "ONE")                 return D3DBLEND_ONE;

  }
*/
  return -1;

}

