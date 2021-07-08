/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include "TerrainUtils.h"
#include "../SceneFrame.h"
extern SceneFrame* g_SceneFrame;

#define TERRAIN_DEPTH_SCALE   20.0f
#define TERRAIN_WIDTH_SCALE   20.0f

#define TERRAIN_Z_OFFSET 128
#define TERRAIN_X_OFFSET 128

#undef USEMESHES

	WaterMesh::WaterMesh()
	{
		shd=-1;
		texID=-1;
		vf=-1;
	}
	WaterMesh::~WaterMesh()
	{
	}
	void WaterMesh::init(const char* aTitle, tShader aShader)
	{
static const float verticesarr[20] =  { -1257.257202f, 0.124995f,  638.826721f, 0.0f, 1.0f, 
					-1257.257935f, 0.124994f, -638.576782f, 0.0f, 0.0f, 
					 1257.007202f, 0.124995f,  638.826660f, 1.0f, 1.0f, 
					 1257.007446f, 0.125005f, -638.576538f, 1.0f, 0.0f };
static const __WORD__ indicesarr[6] =  { 2, 1, 0, 2, 3, 1 };
std::vector<float> verticesvec (verticesarr, verticesarr + sizeof(verticesarr) / sizeof(verticesarr[0]) );
std::vector<__WORD__>  indicesvec  (indicesarr, indicesarr + sizeof(indicesarr) / sizeof(indicesarr[0]) );
vertices=verticesvec;
indices=indicesvec;
	//LOG_FNLN;
	//LOG_PRINT("vertices.size()=%d\n", vertices.size());
	//LOG_PRINT("indices.size()=%d\n", indices.size());
	//shd = IRenderer::GetRendererInstance()->addShaderFromFile("/Framework3/texSHD1.HLSL.shd", "main", "main");
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/NatureScene/WaterShader.shd", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	texID=IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Water/water.png", false, ss);
	}
	unsigned int WaterMesh::render(D3DXFROMWINEMATRIX* mvp)
	{
  	IRenderer::GetRendererInstance()->setShader(shd);
      	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	if(mvp)
	{
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", *mvp);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("TextureMatrix1", *mvp);
	}
	else if(1)
	{
			D3DXFROMWINEMATRIX W, R, T;
			D3DXFROMWINEMatrixRotationYawPitchRoll(&R, D3DXFROMWINEToRadian(180.0f), D3DXFROMWINEToRadian(180.0f), 0.0f);
			D3DXFROMWINEMatrixTranslation(&T, 510.0f, -10.0f, 1500.0f);
			W=R*T;
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("TextureMatrix1", W);
	}
	else
	{
		D3DXFROMWINEMATRIX I;
		D3DXFROMWINEMatrixIdentity(&I);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("TextureMatrix1", I);
	}
  IRenderer::GetRendererInstance()->setTexture("reflection", g_SceneFrame->waterTexture);
  IRenderer::GetRendererInstance()->setTexture("normalmap", g_SceneFrame->normalmap);
  IRenderer::GetRendererInstance()->setTexture("depthmap", g_SceneFrame->waterDepth);

  D3DXFROMWINEMATRIX transform;
  D3DXFROMWINEVECTOR4 translation;
  D3DXFROMWINEMatrixTranslation(&transform, 510.0f, -10.0f, 1500.0f);
  D3DXFROMWINEMatrixTranslation(&transform, 0.0f, 0.0f, 0.0f);
  translation.set(transform._41, transform._42,  transform._43, 0.0f);
  IRenderer::GetRendererInstance()->setShaderConstant4f("terrainInfo", D3DXFROMWINEVECTOR4(					 -TERRAIN_X_OFFSET*TERRAIN_WIDTH_SCALE,
                                                         -TERRAIN_Z_OFFSET*TERRAIN_DEPTH_SCALE,
                                                          TERRAIN_WIDTH_SCALE*255.0f,
                                                          TERRAIN_DEPTH_SCALE*255.0f));
  IRenderer::GetRendererInstance()->setShaderConstant4f("translation", translation);

  D3DXFROMWINEVECTOR4 cameraPosition;
  cameraPosition.set(g_SceneFrame->camera.getViewerPosition().x, g_SceneFrame->camera.getViewerPosition().y, g_SceneFrame->camera.getViewerPosition().z, 0.0f);
  IRenderer::GetRendererInstance()->setShaderConstant4f("camera",      cameraPosition);
  float privateTimeClock = timeGetTime()/1000.0f;
  IRenderer::GetRendererInstance()->setShaderConstant1f("timer",       privateTimeClock);



  	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());

	#if 1
	const uint flags=RESET_ALL;
	#else
	const uint flags=RESET_VB|RESET_IB;
	#endif
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, vertices.size()/5, indices.size()/3, &indices[0], &indices[0], CONSTANT_INDEX2, &vertices[0], &vertices[0], sizeof(float)*5, flags);
	return indices.size()/3;
	}

