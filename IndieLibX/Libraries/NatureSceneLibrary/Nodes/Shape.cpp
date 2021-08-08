#include <STX/STX.h>
#include <STX/STXCPP.h>
#include <STX/STXinterfaces.h>
#include <STX/CInput.h>
#include <STX/Noise.h>
#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>

#include "Shape.h"

#include "TransformGroup.h"



//include "../Renderer/Renderer.h"

#include <Managers/GeometryManager.h>

#ifdef _INCLUDE_SCENEFRAME_
#include <SceneFrame.h>
#endif


int pickingIndicesProgress = 1;



Shape::Shape(Geometry *geom, Appearance *app)

{

  initialize();

  setAppearance(app);

  setGeometry(geom);

  visible	 = true;

}



Shape::Shape(const Shape &shape)

{

  if(this != &shape)

    initialize();



  this->operator=(shape);

}



Shape &Shape::operator = (const Shape &shape)

{

  if(this != &shape)

  {

    if(shape.appearance)

      setAppearance(shape.appearance);



    if(shape.geometry)

      setGeometry(shape.geometry);



    pickingIndex = pickingIndicesProgress++;

    visible	 = shape.visible;

  }

  return *this;

}



void Shape::initialize()

{

  pickingIndex = pickingIndicesProgress++;

  appearance   = 0;

  geometry     = 0;

  visible      = true;

  parent       = 0;

}



void Shape::setParent(TransformGroup *parentArg)

{

  if(!parentArg) return;

  parent = parentArg;

}



TransformGroup *Shape::getParent()

{

  return parent;

}



int Shape::draw(int rendering, const uint flags)

{

  if(!appearance || !geometry || !visible)

    return 0;



  //int  format = geometry->getVertexFormat();



//  if(rendering & MATERIAL && (appearance->getMaterial()))

//    appearance->getMaterial()->bindMaterial();


#if 0//???
  if(rendering & BLEND_ATTR)
  {
    BlendStateID a=appearance->getBlendAttributes();
    IRenderer::GetRendererInstance()->setBlendState(a);
  }


  if(rendering & ALPHA_ATTR)
  {
    BlendStateID a=appearance->getAlphaAttributes();
    IRenderer::GetRendererInstance()->setBlendState(a);
  }
#endif

//  if(appearance->colorOn())

//    IRenderer::GetRendererInstance()->Color4fv(appearance->getColor());



  //appearance->

  //this->

  ShaderID sha=0;

  if(rendering	& SHADERS)

    if(appearance->shadersOn())

  {

      sha=appearance->getShaders();//.deactivate();

	}

    if(appearance->shadersOn()&&(sha!=-1))

  {
/*
    D3DXFROMWINEMATRIX m_matProj=IRenderer::GetRendererInstance()->p;//SceneFrame::camera.GetProjMatrix();
    D3DXFROMWINEMATRIX m_matView=IRenderer::GetRendererInstance()->mv;//SceneFrame::camera.GetViewMatrix();
	D3DXFROMWINEMATRIX worldViewProjection = identity4();//m_matView*m_matProj;
	multMatrix(worldViewProjection,m_matProj, m_matView);
*/

	//IRenderer::GetRendererInstance()->setShaderConstant4x4f( "worldViewProj", g_worldViewProjection );
	}

  if(geometry->RESIDE_ON_GFX_DEVICE())

    drawVBOs(rendering,flags);

  else

  {

    enableTextureUnits(rendering, sha);

    drawRegular(rendering,flags);

  }



  disableTexureUnits(rendering);











  return geometry->getTriangleCount();

}

void PrintPrimitive(Primitives p)

{

	switch(p)

	{

		case PRIM_POINTS:

			//LOG_PRINT("PRIM_POINTS\n");

			break;

		case PRIM_LINES:

			//LOG_PRINT("PRIM_LINES\n");

			break;

		case PRIM_LINE_STRIP:

			//LOG_PRINT("PRIM_LINE_STRIP\n");

			break;

		case PRIM_TRIANGLES:

			//LOG_PRINT("PRIM_TRIANGLES\n");

			break;

		case PRIM_TRIANGLE_STRIP:

			//LOG_PRINT("PRIM_TRIANGLE_STRIP\n");

			break;

		case PRIM_TRIANGLE_FAN:

			//LOG_PRINT("PRIM_TRIANGLE_FAN\n");

			break;

		case PRIM_QUADS:

			//LOG_PRINT("PRIM_QUADS\n");

			break;

		case PRIM_LINE_LOOP:

			//LOG_PRINT("PRIM_LINE_LOOP\n");

			break;

	}

}

void Shape::drawVBOs(int rendering, const uint flags)

{

  //glBindBufferARB(GL_ARRAY_BUFFER_ARB, geometry->getElementsVBOID());

  //enableTextureUnits(rendering);



  {

	  VertexFormatID FVF=geometry->getVertexFormat();

	  int FVF2=geometry->FVF;

	  int VertexCount=geometry->verticesCount;

	  int TriangleCount=geometry->indicesCount/3;

	  int VertexSize=geometry->strideSize;




/*
	  if((VertexSize==56))//&&(FVF==23))

	  {

 FVF=VERTICES|NORMALS|TEXTURE0
|TEXTURE1
|TEXTURE2;
	}

	  else

	  {

		DBG_HALT;

	  }

*/



	(IRenderer::GetRendererInstance()->setVertexFormat(FVF));

	(IRenderer::GetRendererInstance()->setVertexBuffer(0,geometry->elementsVBOID));

#ifdef _INCLUDE_SCENEFRAME_
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	if (g_SceneFrame->GetTerrain()->getWireFrameFlag() || g_SceneFrame->getSkyWireFrameFlag())
		IRenderer::GetRendererInstance()->setRasterizerState(
			IRenderer::GetRendererInstance()->GetfillWireFrame());
	else
		IRenderer::GetRendererInstance()->setRasterizerState(
			IRenderer::GetRendererInstance()->GetfillSolid());
#endif

	if(geometry->getIndicesVBOID()!=-1)

	{

		(IRenderer::GetRendererInstance()->setIndexBuffer(geometry->getIndicesVBOID()));

		//(IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, VertexCount, 0, TriangleCount, RESET_VB|RESET_IB));

		
		//LOG_FNLN;
		//LOG_PRINT("DrawIndexedPrimitive\n");
		(IRenderer::GetRendererInstance()->DrawIndexedPrimitive(geometry->getType(), 0, 0, VertexCount, 0, TriangleCount, RESET_VB|RESET_IB));

	}

	else

	{

		//PrintPrimitive(geometry->getType());

		//LOG_PRINT("TriangleCount=%d\n",TriangleCount);

		
		//LOG_FNLN;
		//LOG_PRINT("DrawPrimitive\n");
		(IRenderer::GetRendererInstance()->DrawPrimitive(geometry->getType(), 0, TriangleCount, RESET_VB|RESET_IB));

	}


  }



}



void Shape::drawRegular(int rendering, const uint flags)

{



  {

	  int FVF=geometry->getVertexFormat();

	  int VertexCount=geometry->verticesCount;

	  int TriangleCount=geometry->indicesCount/3;

	  int VertexSize=geometry->strideSize;


/*
	  if((TriangleCount==2)&&(VertexCount==4))

	  {

	  //LOG_PRINT("drawRegular:\nFVF=%d\n", FVF);

//		DecodeFVF(FVF);

	//LOG_PRINT("VB=%d\n", geometry->elementsVBOID);

	//LOG_PRINT("SV=%d\n", VertexSize);

	//LOG_PRINT("IB=%d\n", geometry->getIndicesVBOID());

	//LOG_PRINT("NV=%d\n", VertexCount);

	//LOG_PRINT("NT=%d\n", TriangleCount);

	  }
*/


	(IRenderer::GetRendererInstance()->setVertexFormat(FVF));

	(IRenderer::GetRendererInstance()->setVertexBuffer(0,geometry->elementsVBOID));

#ifdef _INCLUDE_SCENEFRAME_
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	if (g_SceneFrame->GetTerrain()->getWireFrameFlag())
		IRenderer::GetRendererInstance()->setRasterizerState(
			IRenderer::GetRendererInstance()->GetfillWireFrame());
	else
		IRenderer::GetRendererInstance()->setRasterizerState(
			IRenderer::GetRendererInstance()->GetfillSolid());
#endif

	if(geometry->getIndicesVBOID()!=-1)

	{

		(IRenderer::GetRendererInstance()->setIndexBuffer(geometry->getIndicesVBOID()));

		//(IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, VertexCount, 0, TriangleCount, RESET_VB|RESET_IB));

		
		//LOG_FNLN;
		//LOG_PRINT("DrawIndexedPrimitive\n");
		(IRenderer::GetRendererInstance()->DrawIndexedPrimitive(geometry->getType(), 0, 0, VertexCount, 0, TriangleCount, RESET_VB|RESET_IB));

	}

	else

	{

		//(IRenderer::GetRendererInstance()->DrawPrimitive( PRIM_TRIANGLE_STRIP, 0, VertexCount-1, RESET_VB|RESET_IB));

		
		//LOG_FNLN;
		//LOG_PRINT("DrawPrimitive\n");
		(IRenderer::GetRendererInstance()->DrawPrimitive(geometry->getType(), 0, TriangleCount, RESET_VB|RESET_IB));

	}


  }



}



bool Shape::isTextureActive(int format, int index)

{

  bool activeTexture = false;



  switch(index)

  {

    case 7: activeTexture = ((format & TEXTURE7) != 0); break;

    case 6: activeTexture = ((format & TEXTURE6) != 0); break;

    case 5: activeTexture = ((format & TEXTURE5) != 0); break;

    case 4: activeTexture = ((format & TEXTURE4) != 0); break;

    case 3: activeTexture = ((format & TEXTURE3) != 0); break;

    case 2: activeTexture = ((format & TEXTURE2) != 0); break;

    case 1: activeTexture = ((format & TEXTURE1) != 0); break;

    case 0: activeTexture = ((format & TEXTURE0) != 0); break;

    default: activeTexture = false;

  }

  return activeTexture;

}



void Shape::enableTextureUnits(int rendering, ShaderID sha)

{

//  TextureID texturePointer;

 // bool	     activeTexture = false;

 // int      format	   = geometry->getVertexFormat();


/*
  for(int i =0; i < MAX_TEX_UNITS; i++)

  {

    if(!isTextureActive(rendering, i))

      continue;

    activeTexture = false;

*/
/*
    texturePointer = appearance->getTexture(0);//???i);

    activeTexture  = texturePointer != -1;

    if(activeTexture)
*/
//      texturePointer->activate(i);
	//LOG_FNLN;
	//LOG_PRINT("sha = %x\n", sha);	
	IRenderer::GetRendererInstance()->setTexture("shape", sha);//???texturePointer);
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());

  //}

}



void Shape::disableTexureUnits(int format)

{

  TextureID texturePointer;

  bool	   activeTexture = false;



  for(int i = MAX_TEX_UNITS - 1; i >-1; i--)

  {

    if(!isTextureActive(format, i))  continue;

/*

	if(geometry->getTextureElementsCount(i) > 0){

      glClientActiveTextureARB(GL_TEXTURE0_ARB + i);

      //glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    }

*/

    texturePointer = appearance->getTexture(i);

    activeTexture  = (texturePointer!=-1);//texturePointer && texturePointer;



//    if(activeTexture)

  //    texturePointer->deactivate();

  }

}



void Shape::compile(bool shadows)

{





  if(!geometry)

    return;





	if(!(geometry->RESIDE_ON_GFX_DEVICE()))

  {





    geometry->compile();





    if(parent && !parent->isOccluder())

      geometry->cleanUpAfterCompilation();





  }





}



void Shape::renderSilhouette(D3DXFROMWINEVECTOR4 &viewPosition)

{

#ifdef INCLUDESHADOW

  ShadowInfo *shadowInfo = geometry->getInfoShadow();

  if(!shadowInfo)

    return;



  shadowInfo->computeSilhouette(viewPosition);

  shadowInfo->drawSilhouette();

#endif

}



void Shape::doShadowPass(D3DXFROMWINEVECTOR4 &lightPos, bool drawCaps, int renderingPath)

{

#ifdef INCLUDESHADOW

  ShadowInfo *shadowInfo = geometry->getInfoShadow();



  if(!shadowInfo)

    return;



  shadowInfo->drawDegeneratedQuads(lightPos, drawCaps, renderingPath);

#endif

}



void Shape::setGeometry(Geometry *geom)

{

  GeometryInfo *geomtryInfo = 0;



  if(!geom)

    return;



  if(geometry)

  if(geomtryInfo = GeometryManager::getGeometryInfo(geometry->getName()))

    geomtryInfo->decreaseUserCount();



  if(geomtryInfo = GeometryManager::getGeometryInfo(geom->getName()))

    geomtryInfo->increaseUserCount();

  else

  {

    geomtryInfo = new GeometryInfo(geom->getName(), geom);

    GeometryManager::addGeometryInfo(geomtryInfo);

  }

  geometry = geom;

}



Geometry   *Shape::getGeometry()  { return geometry;   }



void	    Shape::setAppearance(Appearance *app){ appearance =  app; }

Appearance *Shape::getAppearance()		 { return appearance; }



int  Shape::getPickingIndex() { return pickingIndex; }



void Shape::setVisible(bool v){ visible = v;	}

bool Shape::isVisible()       { return visible; }



bool Shape::loadXMLSettings(XMLElement *element, const char *shapePath)

{

  if(!element || !shapePath)

    {LOG_PRINT("Invalid XMLElement or shapePath -> Shape::loadXMLSettings ");return false;}



  XMLElement *geometryNode   = element->getChildByName("Geometry"),

	     *appearanceNode = element->getChildByName("Appearance");



  if(!geometryNode || !appearanceNode)

    {LOG_PRINT("Can't locate <Appearance> or <Geometry> in <Shape> nodel -> %s\n", shapePath);return false;}



  if(!appearance)

    appearance = new Appearance();

  else

    appearance->destroy();



  if(geometry)

    geometry->destroy();



  GeometryInfo *geomtryInfo = GeometryManager::getGeometryInfo(shapePath);



  if(!geomtryInfo)

  {

    geometry = new Geometry(shapePath);



    if(geometry->loadXMLSettings(geometryNode))

    {

      geomtryInfo = new GeometryInfo(geometry->getName(), geometry);

      GeometryManager::addGeometryInfo(geomtryInfo);

    }

    else

    {

      deleteObject(geometry);

      {LOG_PRINT("Failed to load XML geometry -> %s\n", shapePath);return false;}

    }

  }

  else

    setGeometry(geomtryInfo->getMedia());



  if(!appearance->loadXMLSettings(appearanceNode))

    LOG_PRINT("Failed to load XML appearance -> %s\n", shapePath);

  return true;

}



bool Shape::exportXMLSettings(ofstream &xmlFile  )

{

  if(!xmlFile.is_open())

    {LOG_PRINT("Cannot export Shape to XML: file not ready.");return false;}



  if(!geometry)

    {LOG_PRINT("Cannot export Shape to XML: 0 geometry.");return false;}



  if(!appearance)

    {LOG_PRINT("Cannot export Shape to XML: 0 appearance.");return false;}



  int success = 1;



  xmlFile << "<Shape>\n";

  success *= appearance->exportXMLSettings(xmlFile);

  success *= geometry  ->exportXMLSettings(xmlFile);

  xmlFile << "</Shape>\n";

  return (success != 0);

}



bool Shape::exportXMLSettings(const char* xmlPath)

{

  ofstream xmlFile(xmlPath, ios::app | ios::binary);

  bool result = exportXMLSettings(xmlFile);

  xmlFile.close();

  return result;

}



Shape::~Shape()

{

  if(geometry)

    geometry->destroy();

  deleteObject(appearance);

  parent = 0;

}

