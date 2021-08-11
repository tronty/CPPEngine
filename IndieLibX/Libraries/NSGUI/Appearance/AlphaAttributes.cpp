#include <Framework3/IRenderer.h>
#include "AlphaAttributes.h"

#include "../Managers/MediaPathManager.h"


using namespace NatureScene;

AlphaAttributes::AlphaAttributes(int   functionArg,

                                 float referenceArg)

                : IOXMLObject("AlphaAttributes")

{

  reference = referenceArg;

  function  = functionArg;

}



AlphaAttributes::AlphaAttributes(const AlphaAttributes &copy)

                : IOXMLObject("AlphaAttributes")

{

  reference = copy.reference;

  function  = copy.function;

}



AlphaAttributes &AlphaAttributes::operator = (const AlphaAttributes &copy)

{

  reference = copy.reference;

  function  = copy.function;

  return *this;

}



bool  AlphaAttributes::loadXMLSettings(XMLElement *element)

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



    if(childName == "function")

      setFunction(getXMLAlphaFunc(child->getValue()));



    if(childName == "reference")

      setReference(child->getValuef());

  }



  return true;

}



bool  AlphaAttributes::exportXMLSettings(ofstream &xmlFile  )

{

  if(!xmlFile.is_open())

    {LOG_PRINT_NONE();return false;}



  if(function == -1.0f || reference == -1.0f)

    {LOG_PRINT_NONE();return false;}


/*
   xmlFile << "  <AlphaAttributes  reference = \"" << reference << "\n"

              "                    function  = \"" <<((function ==  D3DCMP_NEVER   ) ? "NEVER\"/> \n"    :

                                                      (function ==  D3DCMP_LESS    ) ? "LESS\"/> \n"     :

                                                      (function ==  D3DCMP_EQUAL   ) ? "EQUAL\"/> \n"    :

                                                      (function ==  D3DCMP_NOTEQUAL) ? "NOTEQUAL\" />\n" :

                                                      (function ==  D3DCMP_GREATER ) ? "GREATER\" />\n"  :

                                                      (function ==  D3DCMP_LESSEQUAL  ) ? "LEQUAL\" />\n"   :

                                                      (function ==  D3DCMP_GREATEREQUAL  ) ? "GEQUAL\" />\n"   :  "ALWAYS\" />\n");
*/
  return true;

}



void AlphaAttributes::setAttributes(int function, float reference)

{

  setReference(reference);

  setFunction (function);

}



void  AlphaAttributes::setFunction(int functionArg)

{
/*
  switch(functionArg)

  {

    case D3DCMP_NEVER:

    case D3DCMP_LESS:

    case D3DCMP_EQUAL:

    case D3DCMP_LESSEQUAL:

    case D3DCMP_GREATER:

    case D3DCMP_NOTEQUAL:

    case D3DCMP_GREATEREQUAL:

    case D3DCMP_ALWAYS:

     function = functionArg;

     return;

  }

*/

   LOG_PRINT_NONE();

  function = -1;

}



int   AlphaAttributes::getFunction()

{

  return function;

}



void  AlphaAttributes::setReference(float referenceArg)

{

  reference = clampNS(referenceArg, 0.0f, 1.0f);

  if(reference != referenceArg)

    LOG_PRINT_NONE();

}



float AlphaAttributes::getReference()

{

  return reference;

}



void  AlphaAttributes::apply()

{

  if(reference != -1.0 && function != -1)

  {
#if 0
    (g_pApp->m_pRenderer->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE));

    (g_pApp->m_pRenderer->SetRenderState(D3DRS_ALPHAREF, 0x08));

    (g_pApp->m_pRenderer->SetRenderState((D3DRENDERSTATETYPE)function, reference));//D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL));
#elif 0
	if(1)
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	else
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());
	IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetnoDepthTest());
#endif
  }

}



void  AlphaAttributes::stop()

{
/*???
  if(reference != -1.0 && function != -1)

  {

        (g_pApp->m_pRenderer->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE));

  }
*/
}



int   AlphaAttributes::getXMLAlphaFunc(const NSString &value)

{
/*
  if(value.getLength())

  {

    if(value == "NOTEQUAL") return D3DCMP_NOTEQUAL;

    if(value == "GREATER")  return D3DCMP_GREATER;

    if(value == "GEQUAL")   return D3DCMP_GREATEREQUAL;

    if(value == "LEQUAL")   return D3DCMP_LESSEQUAL;

    if(value == "ALWAYS")   return D3DCMP_ALWAYS;

    if(value == "NEVER")    return D3DCMP_NEVER;

    if(value == "EQUAL")    return D3DCMP_EQUAL;

    if(value == "LESS")     return D3DCMP_LESS;

  }
*/
  return -1;

}

