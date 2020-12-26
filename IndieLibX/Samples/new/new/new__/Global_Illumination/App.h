//-----------------------------------------------------------------------------
// File: D3D10\Global Illumination\App.h
// Copyright (c) 2007 Advanced Micro Devices Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _APP_H_
#define _APP_H_

#define FRAMEWORK_VERSION 1
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
#if 0
#include <Framework/Util/Model.h>
#else
class Model
{
public:
	Model(){}
	virtual ~Model(){}
};
#endif

// Pass indices
#define SHADOW_PASS 0
#define PROBE_PASS  2
#define FINAL_PASS  7

#define LIGHTING_TECH 12

// Size of the probe grid
#define SIZE_X 11
#define SIZE_Y 4
#define SIZE_Z 7

// Scale of the grid in each direction
#define SCALE_X 1.7f
#define SCALE_Y 2.0f
#define SCALE_Z 1.85f

#define SCALE (1.0 / D3DXFROMWINEVECTOR3(SCALE_X * SIZE_X, SCALE_Y * SIZE_Y, SCALE_Z * SIZE_Z))

#define MAX_PROBE_COUNT (SIZE_X * SIZE_Y * SIZE_Z)
#define PROBE_SLICES (PROBE_COUNT * 6)
#define PROBE_CUBES_PER_PASS 5
#define PROBE_SLICES_PER_PASS (PROBE_CUBES_PER_PASS * 6)

#define PROBE_PASSES_PER_FRAME 8
#define PROBE_CUBES_PER_FRAME (PROBE_PASSES_PER_FRAME * PROBE_CUBES_PER_PASS)

#define PROBE_SIZE 8

#define SHADOWMAP_SIZE 768

// Number of SH bands to use. If this is changed you'll
// have to reconfigure the texture locations in the shader.
#define SH_BANDS 4
#define SH_COEFFS (SH_BANDS * SH_BANDS)
#define SH_COEFF_VECTORS (SH_COEFFS / 4)

#if ((SH_BANDS & 0x1) != 0)
#error "SH_BANDS must be even so that SH_COEFF_VECTORS is an integer"
#endif

// Size of the transfer function SH coefficient cubemap
#define SH_TF_CUBE_SIZE 32

// Number of wall materials
#define WALL_MATERIAL_COUNT 4

// Constant buffers divided by frequency of update. They match the cbuffer declaration in Lighting.fx
struct PerFrame
{
	D3DXFROMWINEMATRIX mvp;
	D3DXFROMWINEVECTOR3 lightPos;
	float intensity;
	D3DXFROMWINEVECTOR3 camPos;
	float exposure;
	D3DXFROMWINEVECTOR4 lmModulate[2];
};

struct PerObject
{
	D3DXFROMWINEVECTOR3 diffuse; float dummy;
	D3DXFROMWINEVECTOR4 specular;
};

// Vertex_ layout for the geometry in this sample
struct Vertex_
{
	D3DXFROMWINEVECTOR3 pos;
	D3DXFROMWINEVECTOR3 normal;
	uint texCoord; // Packed texture coordinate
};

// Vertex_ layout for the probe SH coefficient computation pass
struct ShVertex
{
	D3DXFROMWINEVECTOR2 xy;
	int z;
};

// Extend the model class to load resources and render
class MyModel : public Model
{
public:
	MyModel();
	virtual ~MyModel();

	void Setup();
	void SetupTexture( const int index);
	void SetupMaterial( const int index);

	void RenderMaterial( const int material, const int count = 1);
	void RenderAll();

	virtual bool Load( const char *name, const char *texturePath);

private:
	void AddToMaterialRange(uint32 *indices, int &index, const int mat, const uint startMesh, const uint meshCount);

	VertexBufferID m_vertexBuffer;
	VertexBufferID m_indexBuffer;

	TextureID m_textures[7];
	TextureID m_textureSRVs[7];

	VertexBufferID m_materials[10];

	int m_materialRange[14];
};

// Main application class
class App
{
public:
	const char *GetHomeDirectory(){ return ("."); }

	void ResetCamera();
	bool OnKeyPress(const unsigned int key, const bool pressed);

	bool Create();
	void Destroy();

	bool CreateSHTextures();

	bool Load();
	void Unload();

	void OnRender();

protected:
	float EvaluateTransferFunction(const D3DXFROMWINEVECTOR3 &normal, const D3DXFROMWINEVECTOR3 *samples, const int sampleCount, const float *shArray, const int l, const int m) const;

	void SetupScene(const int passIndex);
	void RenderScene(const int passIndex, const D3DXFROMWINEMATRIX *mvp, const D3DXFROMWINEMATRIX *mvpInv, const int matrixCount);

	// Main effect and some extracted shaders
	stx_Effect*m_lighting, *m_shEffect;
	ShaderID m_lightingPS[LIGHTING_TECH];
	ShaderID m_shadowGS[2];
	VertexFormatID m_lightingIL;

	RasterizerStateID m_cullBack, *m_cullNone;

	SamplerStateID m_baseSS;
	SamplerStateID m_lightMapSS;

	TextureID m_shadowMap;
	TextureID m_shadowMapRTV;
	TextureID m_shadowMapSRV;
	SamplerStateID m_shadowMapSS;

	TextureID m_shadowMapDepth;
	DepthStateID m_shadowMapDepthDSV;

	D3DXFROMWINEVECTOR3 m_probePos[MAX_PROBE_COUNT];

	int m_probeCount;
	int m_currProbe;
	int m_waitFrames;
	int m_currWallMaterial;

	float m_lmValCeiling1;
	float m_lmValCeiling2;
	float m_lmValWall;
	float m_lightIntensity;
	float m_exposure;


	// Render target for the light probes
	TextureID m_probes;
	TextureID m_probesRTV;
	TextureID m_probesChunkRTVs[PROBE_PASSES_PER_FRAME];
	TextureID m_probesSRV;

	// Depth surfaces for the light probes
	TextureID m_probesDepth;
	DepthStateID m_probesDepthDSV;
	DepthStateID m_probesDepthChunkDSVs[PROBE_PASSES_PER_FRAME];



	VertexBufferID m_perFrameCB;
	VertexBufferID m_gsMvpCB;
	VertexBufferID m_gsMvpInvCB;

	TextureID m_shTable[SH_COEFF_VECTORS];
	TextureID m_shTableSRV[SH_COEFF_VECTORS];

	TextureID m_shCube[SH_COEFF_VECTORS];
	TextureID m_shCubeSRV[SH_COEFF_VECTORS];


	// 3D textures containing the resulting SH coefficients
	TextureID m_shCoeffs[3][SH_COEFF_VECTORS];
	TextureID m_shCoeffsRTV[3][SH_COEFF_VECTORS];
	TextureID m_shCoeffsSRV[3][SH_COEFF_VECTORS];
	SamplerStateID m_shCoeffsSS;

	VertexBufferID m_shCoeffVB;
	VertexFormatID m_shCoeffIL;

	TextureID m_texLightGrad;
	TextureID m_texLightGradSRV;

	D3DXFROMWINEMATRIX m_lightView;
	D3DXFROMWINEMATRIX m_lightProj;
	MyModel *m_model;

	bool m_animateLight;
	bool m_showProbes;
	bool m_showHelp;
};

#endif
