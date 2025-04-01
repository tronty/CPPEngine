/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

// Copyright (c) 2011 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, NONINFRINGEMENT,IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA 
// OR ITS SUPPLIERS BE  LIABLE  FOR  ANY  DIRECT, SPECIAL,  INCIDENTAL,  INDIRECT,  OR  
// CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS 
// OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY 
// OTHER PECUNIARY LOSS) ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, 
// EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
// Please direct any bugs or questions to SDKFeedback@nvidia.com

#include "DXUT.h"
#include "DXUTCamera.h"
#include "SDKmisc.h"

#include "ocean_simulator.h"

#pragma warning(disable:4995)
#include <vector>
using namespace std;

#define FRESNEL_TEX_SIZE			256
#define PERLIN_TEX_SIZE				64

struct ocean_vertex
{
	float index_x;
	float index_y;
};

// Mesh properties:

// Mesh grid dimension, must be 2^n. 4x4 ~ 256x256
int g_MeshDim = 128;
// Side length of square shaped mesh patch
float g_PatchLength;
// Dimension of displacement map
int g_DisplaceMapDim;
// Subdivision thredshold. Any quad covers more pixels than this value needs to be subdivided.
float g_UpperGridCoverage = 64.0f;
// Draw distance = g_PatchLength * 2^g_FurthestCover
int g_FurthestCover = 8;


// Shading properties:
// Two colors for waterbody and sky color
D3DXFROMWINEVECTOR3 g_SkyColor= D3DXFROMWINEVECTOR3(0.38f, 0.45f, 0.56f);
D3DXFROMWINEVECTOR3 g_WaterbodyColor = D3DXFROMWINEVECTOR3(0.07f, 0.15f, 0.2f);
// Blending term for sky cubemap
float g_SkyBlending = 16.0f;

// Perlin wave parameters
float g_PerlinSize = 1.0f;
float g_PerlinSpeed = 0.06f;
D3DXFROMWINEVECTOR3 g_PerlinAmplitude = D3DXFROMWINEVECTOR3(35, 42, 57);
D3DXFROMWINEVECTOR3 g_PerlinGradient = D3DXFROMWINEVECTOR3(1.4f, 1.6f, 2.2f);
D3DXFROMWINEVECTOR3 g_PerlinOctave = D3DXFROMWINEVECTOR3(1.12f, 0.59f, 0.23f);
D3DXFROMWINEVECTOR2 g_WindDir;

D3DXFROMWINEVECTOR3 g_BendParam = D3DXFROMWINEVECTOR3(0.1f, -0.4f, 0.2f);

// Sunspot parameters
D3DXFROMWINEVECTOR3 g_SunDir = D3DXFROMWINEVECTOR3(0.936016f, -0.343206f, 0.0780013f);
D3DXFROMWINEVECTOR3 g_SunColor = D3DXFROMWINEVECTOR3(1.0f, 1.0f, 0.6f);
float g_Shineness = 400.0f;


// Quadtree structures & routines
struct QuadNode
{
	D3DXFROMWINEVECTOR2 bottom_left;
	float length;
	int lod;

	int sub_node[4];
};

struct QuadRenderParam
{
	unsigned int num_inner_verts;
	unsigned int num_inner_faces;
	unsigned int inner_start_index;

	unsigned int num_boundary_verts;
	unsigned int num_boundary_faces;
	unsigned int boundary_start_index;
};

// Quad-tree LOD, 0 to 9 (1x1 ~ 512x512) 
int g_Lods = 0;
// Pattern lookup array. Filled at init time.
QuadRenderParam g_mesh_patterns[9][3][3][3][3];
// Pick a proper mesh pattern according to the adjacent patches.
QuadRenderParam& selectMeshPattern(const QuadNode& quad_node);

// Rendering list
vector<QuadNode> g_render_list;
int buildNodeList(QuadNode& quad_node, const CBaseCamera& camera);

// D3D11 buffers and layout
VertexBufferID g_pMeshVB = 0;
VertexBufferID g_pMeshIB = 0;
VertexFormatID g_pMeshLayout = 0;

// Color look up 1D texture
TextureID  g_pFresnelMap = 0;
ShaderResourceViewID g_pSRV_Fresnel = 0;

// Distant perlin wave
ShaderResourceViewID g_pSRV_Perlin = 0;

// Environment maps
ShaderResourceViewID g_pSRV_ReflectCube = 0;

// HLSL shaders
ShaderID   g_pOceanSurfVS = 0;
ShaderID   g_pOceanSurfPS = 0;
ShaderID   g_pWireframePS = 0;

// Samplers
SamplerStateID  g_pHeightSampler = 0;
SamplerStateID  g_pGradientSampler = 0;
SamplerStateID  g_pFresnelSampler = 0;
SamplerStateID  g_pPerlinSampler = 0;
SamplerStateID  g_pCubeSampler = 0;

// Constant buffer
struct Const_Per_Call
{
	D3DXFROMWINEMATRIX	g_matLocal;
	D3DXFROMWINEMATRIX	g_matWorldViewProj;
	D3DXFROMWINEVECTOR2 g_UVBase;
	D3DXFROMWINEVECTOR2 g_PerlinMovement;
	D3DXFROMWINEVECTOR3	g_LocalEye;
};

struct Const_Shading
{
	// Water-reflected sky color
	D3DXFROMWINEVECTOR3		g_SkyColor;
	float			unused0;
	// The color of bottomless water body
	D3DXFROMWINEVECTOR3		g_WaterbodyColor;

	// The strength, direction and color of sun streak
	float			g_Shineness;
	D3DXFROMWINEVECTOR3		g_SunDir;
	float			unused1;
	D3DXFROMWINEVECTOR3		g_SunColor;
	float			unused2;
	
	// The parameter is used for fixing an artifact
	D3DXFROMWINEVECTOR3		g_BendParam;

	// Perlin noise for distant wave crest
	float			g_PerlinSize;
	D3DXFROMWINEVECTOR3		g_PerlinAmplitude;
	float			unused3;
	D3DXFROMWINEVECTOR3		g_PerlinOctave;
	float			unused4;
	D3DXFROMWINEVECTOR3		g_PerlinGradient;

	// Constants for calculating texcoord from position
	float			g_TexelLength_x2;
	float			g_UVScale;
	float			g_UVOffset;
};

VertexBufferID g_pPerCallCB = 0;
VertexBufferID g_pPerFrameCB = 0;
VertexBufferID g_pShadingCB = 0;

// State blocks
RasterizerStateID  g_pRSState_Solid = 0;
RasterizerStateID  g_pRSState_Wireframe = 0;
DepthStateID  g_pDSState_Disable = 0;
BlendStateID  g_pBState_Transparent = 0;

// init & cleanup
void initRenderResource(const OceanParameter& ocean_param);
void cleanupRenderResource();
// create a triangle strip mesh for ocean surface.
void createSurfaceMesh();
// create color/fresnel lookup table.
void createFresnelMap();
// create perlin noise texture for far-sight rendering
void loadTextures();
// Rendering routines
void renderShaded(const CBaseCamera& camera, ShaderResourceViewID displacemnet_map, ShaderResourceViewID gradient_map);
void renderWireframe(const CBaseCamera& camera, ShaderResourceViewID displacemnet_map);
// Shader compilation
int CompileShaderFromFile(char* szFileName, std::string szEntryPoint, std::string szShaderModel, ID3DBlob** ppBlobOut);


void initRenderResource(const OceanParameter& ocean_param
	g_PatchLength = ocean_param.patch_length;
	g_DisplaceMapDim = ocean_param.dmap_dim;
	g_WindDir = ocean_param.wind_dir;

	// D3D buffers
	createSurfaceMesh();
	createFresnelMap();
	loadTextures();

	// HLSL
	// Vertex_ & pixel shaders
	ID3DBlob* pBlobOceanSurfVS = 0;
	ID3DBlob* pBlobOceanSurfPS = 0;
	ID3DBlob* pBlobWireframePS = 0;

	CompileShaderFromFile("ocean_shading.hlsl", "OceanSurfVS", "vs_4_0", &pBlobOceanSurfVS);
	CompileShaderFromFile("ocean_shading.hlsl", "OceanSurfPS", "ps_4_0", &pBlobOceanSurfPS);
	CompileShaderFromFile("ocean_shading.hlsl", "WireframePS", "ps_4_0", &pBlobWireframePS);
	assert(pBlobOceanSurfVS);
	assert(pBlobOceanSurfPS);
	assert(pBlobWireframePS);

	->CreateVertexShader(pBlobOceanSurfVS->GetBufferPointer(), pBlobOceanSurfVS->GetBufferSize(), 0, &g_pOceanSurfVS);
	->CreatePixelShader(pBlobOceanSurfPS->GetBufferPointer(), pBlobOceanSurfPS->GetBufferSize(), 0, &g_pOceanSurfPS);
	->CreatePixelShader(pBlobWireframePS->GetBufferPointer(), pBlobWireframePS->GetBufferSize(), 0, &g_pWireframePS);
	assert(g_pOceanSurfVS);
	assert(g_pOceanSurfPS);
	assert(g_pWireframePS);
	//////////////////SAFE_RELEASE(pBlobOceanSurfPS);
	//////////////////SAFE_RELEASE(pBlobWireframePS);

	// Input layout
	FormatDesc mesh_layout_desc[] = {
		0, TYPE_VERTEX, FORMAT_FLOAT, 2,
	};
	g_pMeshLayout = IRenderer::GetRendererInstance()->addVertexFormat(mesh_layout_desc, elementsOf(mesh_layout_desc), -1);
	assert(g_pMeshLayout);

	//////////////////SAFE_RELEASE(pBlobOceanSurfVS);

	// Constants
	
	cb_desc.Usage = D3D11_USAGE_DYNAMIC;
	cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb_desc.MiscFlags = 0;    
	cb_desc.ByteWidth = PAD16(sizeof(Const_Per_Call));
	cb_desc.StructureByteStride = 0;
	->CreateBuffer(&cb_desc, 0, &g_pPerCallCB);
	assert(g_pPerCallCB);

	Const_Shading shading_data;
	// Grid side length * 2
	shading_data.g_TexelLength_x2 = g_PatchLength / g_DisplaceMapDim * 2;;
	// Color
	shading_data.g_SkyColor = g_SkyColor;
	shading_data.g_WaterbodyColor = g_WaterbodyColor;
	// Texcoord
	shading_data.g_UVScale = 1.0f / g_PatchLength;
	shading_data.g_UVOffset = 0.5f / g_DisplaceMapDim;
	// Perlin
	shading_data.g_PerlinSize = g_PerlinSize;
	shading_data.g_PerlinAmplitude = g_PerlinAmplitude;
	shading_data.g_PerlinGradient = g_PerlinGradient;
	shading_data.g_PerlinOctave = g_PerlinOctave;
	// Multiple reflection workaround
	shading_data.g_BendParam = g_BendParam;
	// Sun streaks
	shading_data.g_SunColor = g_SunColor;
	shading_data.g_SunDir = g_SunDir;
	shading_data.g_Shineness = g_Shineness;

	
	cb_init_data.pSysMem = &shading_data;
	cb_init_data.SysMemPitch = 0;
	cb_init_data.SysMemSlicePitch = 0;

	cb_desc.Usage = USAGE_IMMUTABLE;
	cb_desc.CPUAccessFlags = 0;
	cb_desc.ByteWidth = PAD16(sizeof(Const_Shading));
	cb_desc.StructureByteStride = 0;
	->CreateBuffer(&cb_desc, &cb_init_data, &g_pShadingCB);
	assert(g_pShadingCB);

	// Samplers
	
	sam_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sam_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sam_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sam_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sam_desc.MipLODBias = 0;
	sam_desc.MaxAnisotropy = 1;
	sam_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sam_desc.BorderColor[0] = 1.0f;
	sam_desc.BorderColor[1] = 1.0f;
	sam_desc.BorderColor[2] = 1.0f;
	sam_desc.BorderColor[3] = 1.0f;
	sam_desc.MinLOD = 0;
	sam_desc.MaxLOD = FLT_MAX;
	->CreateSamplerState(&sam_desc, &g_pHeightSampler);
	assert(g_pHeightSampler);

	sam_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	->CreateSamplerState(&sam_desc, &g_pCubeSampler);
	assert(g_pCubeSampler);

	sam_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	sam_desc.MaxAnisotropy = 8;
	->CreateSamplerState(&sam_desc, &g_pGradientSampler);
	assert(g_pGradientSampler);

	sam_desc.MaxLOD = FLT_MAX;
	sam_desc.MaxAnisotropy = 4;
	->CreateSamplerState(&sam_desc, &g_pPerlinSampler);
	assert(g_pPerlinSampler);

	sam_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sam_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sam_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sam_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	->CreateSamplerState(&sam_desc, &g_pFresnelSampler);
	assert(g_pFresnelSampler);

	// State blocks
	
	ras_desc.FillMode = D3D11_FILL_SOLID; 
	ras_desc.CullMode = D3D11_CULL_NONE; 
	ras_desc.FrontCounterClockwise = FALSE; 
	ras_desc.DepthBias = 0;
	ras_desc.SlopeScaledDepthBias = 0.0f;
	ras_desc.DepthBiasClamp = 0.0f;
	ras_desc.DepthClipEnable= TRUE;
	ras_desc.ScissorEnable = FALSE;
	ras_desc.MultisampleEnable = TRUE;
	ras_desc.AntialiasedLineEnable = FALSE;

	->CreateRasterizerState(&ras_desc, &g_pRSState_Solid);
	assert(g_pRSState_Solid);

	ras_desc.FillMode = D3D11_FILL_WIREFRAME; 

	->CreateRasterizerState(&ras_desc, &g_pRSState_Wireframe);
	assert(g_pRSState_Wireframe);

	
	stx_memset(&depth_desc, 0, sizeof(
	depth_desc.DepthEnable = FALSE;
	depth_desc.StencilEnable = FALSE;
	->CreateDepthStencilState(&depth_desc, &g_pDSState_Disable);
	assert(g_pDSState_Disable);

	
	stx_memset(&blend_desc, 0, sizeof(
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	->CreateBlendState(&blend_desc, &g_pBState_Transparent);
	assert(g_pBState_Transparent);
}

void cleanupRenderResource()
{
	//////////////////SAFE_RELEASE(g_pMeshIB);
	//////////////////SAFE_RELEASE(g_pMeshVB);
	//////////////////SAFE_RELEASE(g_pMeshLayout);

	//////////////////SAFE_RELEASE(g_pOceanSurfVS);
	//////////////////SAFE_RELEASE(g_pOceanSurfPS);
	//////////////////SAFE_RELEASE(g_pWireframePS);

	//////////////////SAFE_RELEASE(g_pFresnelMap);
	//////////////////SAFE_RELEASE(g_pSRV_Fresnel);
	//////////////////SAFE_RELEASE(g_pSRV_Perlin);
	//////////////////SAFE_RELEASE(g_pSRV_ReflectCube);

	//////////////////SAFE_RELEASE(g_pHeightSampler);
	//////////////////SAFE_RELEASE(g_pGradientSampler);
	//////////////////SAFE_RELEASE(g_pFresnelSampler);
	//////////////////SAFE_RELEASE(g_pPerlinSampler);
	//////////////////SAFE_RELEASE(g_pCubeSampler);

	//////////////////SAFE_RELEASE(g_pPerCallCB);
	//////////////////SAFE_RELEASE(g_pPerFrameCB);
	//////////////////SAFE_RELEASE(g_pShadingCB);

	//////////////////SAFE_RELEASE(g_pRSState_Solid);
	//////////////////SAFE_RELEASE(g_pRSState_Wireframe);
	//////////////////SAFE_RELEASE(g_pDSState_Disable);
	//////////////////SAFE_RELEASE(g_pBState_Transparent);

	g_render_list.clear();
}

#define MESH_INDEX_2D(x, y)	(((y) + vert_rect.bottom) * (g_MeshDim + 1) + (x) + vert_rect.left)

// Generate boundary mesh for a patch. Return the number of generated indices
int generateBoundaryMesh(int left_degree, int right_degree, int bottom_degree, int top_degree,
						 RECT vert_rect, __DWORD__* output)
{
	// Triangle list for bottom boundary
	int i, j;
	int counter = 0;
	int width = vert_rect.right - vert_rect.left;

	if (bottom_degree > 0)
	{
		int b_step = width / bottom_degree;

		for (i = 0; i < width; i += b_step)
		{
			output[counter++] = MESH_INDEX_2D(i, 0);
			output[counter++] = MESH_INDEX_2D(i + b_step / 2, 1);
			output[counter++] = MESH_INDEX_2D(i + b_step, 0);

			for (j = 0; j < b_step / 2; j ++)
			{
				if (i == 0 && j == 0 && left_degree > 0)
					continue;

				output[counter++] = MESH_INDEX_2D(i, 0);
				output[counter++] = MESH_INDEX_2D(i + j, 1);
				output[counter++] = MESH_INDEX_2D(i + j + 1, 1);
			}

			for (j = b_step / 2; j < b_step; j ++)
			{
				if (i == width - b_step && j == b_step - 1 && right_degree > 0)
					continue;

				output[counter++] = MESH_INDEX_2D(i + b_step, 0);
				output[counter++] = MESH_INDEX_2D(i + j, 1);
				output[counter++] = MESH_INDEX_2D(i + j + 1, 1);
			}
		}
	}

	// Right boundary
	int height = vert_rect.top - vert_rect.bottom;

	if (right_degree > 0)
	{
		int r_step = height / right_degree;

		for (i = 0; i < height; i += r_step)
		{
			output[counter++] = MESH_INDEX_2D(width, i);
			output[counter++] = MESH_INDEX_2D(width - 1, i + r_step / 2);
			output[counter++] = MESH_INDEX_2D(width, i + r_step);

			for (j = 0; j < r_step / 2; j ++)
			{
				if (i == 0 && j == 0 && bottom_degree > 0)
					continue;

				output[counter++] = MESH_INDEX_2D(width, i);
				output[counter++] = MESH_INDEX_2D(width - 1, i + j);
				output[counter++] = MESH_INDEX_2D(width - 1, i + j + 1);
			}

			for (j = r_step / 2; j < r_step; j ++)
			{
				if (i == height - r_step && j == r_step - 1 && top_degree > 0)
					continue;

				output[counter++] = MESH_INDEX_2D(width, i + r_step);
				output[counter++] = MESH_INDEX_2D(width - 1, i + j);
				output[counter++] = MESH_INDEX_2D(width - 1, i + j + 1);
			}
		}
	}

	// Top boundary
	if (top_degree > 0)
	{
		int t_step = width / top_degree;

		for (i = 0; i < width; i += t_step)
		{
			output[counter++] = MESH_INDEX_2D(i, height);
			output[counter++] = MESH_INDEX_2D(i + t_step / 2, height - 1);
			output[counter++] = MESH_INDEX_2D(i + t_step, height);

			for (j = 0; j < t_step / 2; j ++)
			{
				if (i == 0 && j == 0 && left_degree > 0)
					continue;

				output[counter++] = MESH_INDEX_2D(i, height);
				output[counter++] = MESH_INDEX_2D(i + j, height - 1);
				output[counter++] = MESH_INDEX_2D(i + j + 1, height - 1);
			}

			for (j = t_step / 2; j < t_step; j ++)
			{
				if (i == width - t_step && j == t_step - 1 && right_degree > 0)
					continue;

				output[counter++] = MESH_INDEX_2D(i + t_step, height);
				output[counter++] = MESH_INDEX_2D(i + j, height - 1);
				output[counter++] = MESH_INDEX_2D(i + j + 1, height - 1);
			}
		}
	}

	// Left boundary
	if (left_degree > 0)
	{
		int l_step = height / left_degree;

		for (i = 0; i < height; i += l_step)
		{
			output[counter++] = MESH_INDEX_2D(0, i);
			output[counter++] = MESH_INDEX_2D(1, i + l_step / 2);
			output[counter++] = MESH_INDEX_2D(0, i + l_step);

			for (j = 0; j < l_step / 2; j ++)
			{
				if (i == 0 && j == 0 && bottom_degree > 0)
					continue;

				output[counter++] = MESH_INDEX_2D(0, i);
				output[counter++] = MESH_INDEX_2D(1, i + j);
				output[counter++] = MESH_INDEX_2D(1, i + j + 1);
			}

			for (j = l_step / 2; j < l_step; j ++)
			{
				if (i == height - l_step && j == l_step - 1 && top_degree > 0)
					continue;

				output[counter++] = MESH_INDEX_2D(0, i + l_step);
				output[counter++] = MESH_INDEX_2D(1, i + j);
				output[counter++] = MESH_INDEX_2D(1, i + j + 1);
			}
		}
	}

	return counter;
}

// Generate boundary mesh for a patch. Return the number of generated indices
int generateInnerMesh(RECT vert_rect, __DWORD__* output)
{
	int i, j;
	int counter = 0;
	int width = vert_rect.right - vert_rect.left;
	int height = vert_rect.top - vert_rect.bottom;

	bool reverse = false;
	for (i = 0; i < height; i++)
	{
		if (reverse == false)
		{
			output[counter++] = MESH_INDEX_2D(0, i);
			output[counter++] = MESH_INDEX_2D(0, i + 1);
			for (j = 0; j < width; j++)
			{
				output[counter++] = MESH_INDEX_2D(j + 1, i);
				output[counter++] = MESH_INDEX_2D(j + 1, i + 1);
			}
		}
		else
		{
			output[counter++] = MESH_INDEX_2D(width, i);
			output[counter++] = MESH_INDEX_2D(width, i + 1);
			for (j = width - 1; j >= 0; j--)
			{
				output[counter++] = MESH_INDEX_2D(j, i);
				output[counter++] = MESH_INDEX_2D(j, i + 1);
			}
		}

		reverse = !reverse;
	}

	return counter;
}

void createSurfaceMesh()
{
	// --------------------------------- Vertex_ Buffer -------------------------------
	int num_verts = (g_MeshDim + 1) * (g_MeshDim + 1);
	ocean_vertex* pV = new ocean_vertex[num_verts];
	assert(pV);

	int i, j;
	for (i = 0; i <= g_MeshDim; i++)
	{
		for (j = 0; j <= g_MeshDim; j++)
		{
			pV[i * (g_MeshDim + 1) + j].index_x = (float)j;
			pV[i * (g_MeshDim + 1) + j].index_y = (float)i;
		}
	}
	
	//////////////////SAFE_RELEASE(g_pMeshVB);
	g_pMeshVB=IRenderer::GetRendererInstance()->addVertexBuffer(num_verts * sizeof(ocean_vertex), STATIC, (const void *)pV);
	assert(g_pMeshVB);

	SAFE_DELETE_ARRAY(pV);


	// --------------------------------- Index Buffer -------------------------------
	// The index numbers for all mesh LODs (up to 256x256)
	const int index_size_lookup[] = {0, 0, 4284, 18828, 69444, 254412, 956916, 3689820, 14464836};

	stx_memset(&g_mesh_patterns[0][0][0][0][0], 0, sizeof(g_mesh_patterns));

	g_Lods = 0;
	for (i = g_MeshDim; i > 1; i >>= 1)
		g_Lods ++;

	// Generate patch meshes. Each patch contains two parts: the inner mesh which is a regular
	// grids in a triangle strip. The boundary mesh is constructed w.r.t. the edge degrees to
	// meet water-tight requirement.
	__DWORD__* index_array = new __DWORD__[index_size_lookup[g_Lods]];
	assert(index_array);

	int offset = 0;
	int level_size = g_MeshDim;

	// Enumerate patterns
	for (int level = 0; level <= g_Lods - 2; level ++)
	{
		int left_degree = level_size;

		for (int left_type = 0; left_type < 3; left_type ++)
		{
			int right_degree = level_size;

			for (int right_type = 0; right_type < 3; right_type ++)
			{
				int bottom_degree = level_size;

				for (int bottom_type = 0; bottom_type < 3; bottom_type ++)
				{
					int top_degree = level_size;

					for (int top_type = 0; top_type < 3; top_type ++)
					{
						QuadRenderParam* pattern = &g_mesh_patterns[level][left_type][right_type][bottom_type][top_type];

						// Inner mesh (triangle strip)
						RECT inner_rect;
						inner_rect.left   = (left_degree   == level_size) ? 0 : 1;
						inner_rect.right  = (right_degree  == level_size) ? level_size : level_size - 1;
						inner_rect.bottom = (bottom_degree == level_size) ? 0 : 1;
						inner_rect.top    = (top_degree    == level_size) ? level_size : level_size - 1;

						int num_new_indices = generateInnerMesh(inner_rect, index_array + offset);

						pattern->inner_start_index = offset;
						pattern->num_inner_verts = (level_size + 1) * (level_size + 1);
						pattern->num_inner_faces = num_new_indices - 2;
						offset += num_new_indices;

						// Boundary mesh (triangle list)
						int l_degree = (left_degree   == level_size) ? 0 : left_degree;
						int r_degree = (right_degree  == level_size) ? 0 : right_degree;
						int b_degree = (bottom_degree == level_size) ? 0 : bottom_degree;
						int t_degree = (top_degree    == level_size) ? 0 : top_degree;

						RECT outer_rect = {0, level_size, level_size, 0};
						num_new_indices = generateBoundaryMesh(l_degree, r_degree, b_degree, t_degree, outer_rect, index_array + offset);

						pattern->boundary_start_index = offset;
						pattern->num_boundary_verts = (level_size + 1) * (level_size + 1);
						pattern->num_boundary_faces = num_new_indices / 3;
						offset += num_new_indices;

						top_degree /= 2;
					}
					bottom_degree /= 2;
				}
				right_degree /= 2;
			}
			left_degree /= 2;
		}
		level_size /= 2;
	}

	assert(offset == index_size_lookup[g_Lods]);

	//////////////////SAFE_RELEASE(g_pMeshIB);
	g_pMeshIB=IRenderer::GetRendererInstance()->addIndexBuffer(index_size_lookup[g_Lods], 4, STATIC, (const void *)index_array);
	assert(g_pMeshIB);

	SAFE_DELETE_ARRAY(index_array);
}

void createFresnelMap()
{
	__DWORD__* buffer = new __DWORD__[FRESNEL_TEX_SIZE];
	for (int i = 0; i < FRESNEL_TEX_SIZE; i++)
	{
		float cos_a = i / (FLOAT)FRESNEL_TEX_SIZE;
		// Using water's refraction index 1.33
		__DWORD__ fresnel = (__DWORD__)(D3DXFROMWINEFresnelTerm(cos_a, 1.33f) * 255);

		__DWORD__ sky_blend = (__DWORD__)(powf(1 / (1 + cos_a), g_SkyBlending) * 255);

		buffer[i] = (sky_blend << 8) | fresnel;
	}

	Image3 img;
	img.create(FORMAT_RGBA8, FRESNEL_TEX_SIZE, 1, 1, 1);
	img.setPixels((ubyte*)&buffer[0]);
	g_pFresnelMap=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());
	assert(g_pFresnelMap);

	SAFE_DELETE_ARRAY(buffer);
}

void loadTextures()
{
    g_pSRV_Perlin=IRenderer::GetRendererInstance()->addImageLibTexture("Media/OceanCS/perlin_noise.dds", false, IRenderer::GetRendererInstance()->Getlinear());

    g_pSRV_ReflectCube=IRenderer::GetRendererInstance()->addImageLibTexture("Media/OceanCS/reflect_cube.dds", false, IRenderer::GetRendererInstance()->Getlinear());
}

bool checkNodeVisibility(const QuadNode& quad_node, const CBaseCamera& camera)
{
	// Plane equation setup

	D3DXFROMWINEMATRIX matProj = *camera.GetProjMatrix();

	// Left plane
	float fov_x = atan(1.0f / matProj(0, 0));
	D3DXFROMWINEVECTOR4 plane_left(cos(fov_x), 0, sin(fov_x), 0);
	// Right plane
	D3DXFROMWINEVECTOR4 plane_right(-cos(fov_x), 0, sin(fov_x), 0);

	// Bottom plane
	float fov_y = atan(1.0f / matProj(1, 1));
	D3DXFROMWINEVECTOR4 plane_bottom(0, cos(fov_y), sin(fov_y), 0);
	// Top plane
	D3DXFROMWINEVECTOR4 plane_top(0, -cos(fov_y), sin(fov_y), 0);

	// Test quad corners against view frustum in view space
	D3DXFROMWINEVECTOR4 corner_verts[4];
	corner_verts[0] = D3DXFROMWINEVECTOR4(quad_node.bottom_left.x, quad_node.bottom_left.y, 0, 1);
	corner_verts[1] = corner_verts[0] + D3DXFROMWINEVECTOR4(quad_node.length, 0, 0, 0);
	corner_verts[2] = corner_verts[0] + D3DXFROMWINEVECTOR4(quad_node.length, quad_node.length, 0, 0);
	corner_verts[3] = corner_verts[0] + D3DXFROMWINEVECTOR4(0, quad_node.length, 0, 0);

	D3DXFROMWINEMATRIX matView = D3DXFROMWINEMATRIX(1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1) * *camera.GetViewMatrix();
	D3DXFROMWINEVec4Transform(&corner_verts[0], &corner_verts[0], &matView);
	D3DXFROMWINEVec4Transform(&corner_verts[1], &corner_verts[1], &matView);
	D3DXFROMWINEVec4Transform(&corner_verts[2], &corner_verts[2], &matView);
	D3DXFROMWINEVec4Transform(&corner_verts[3], &corner_verts[3], &matView);

	// Test against eye plane
	if (corner_verts[0].z < 0 && corner_verts[1].z < 0 && corner_verts[2].z < 0 && corner_verts[3].z < 0)
		return false;

	// Test against left plane
	float dist_0 = D3DXFROMWINEVec4Dot(&corner_verts[0], &plane_left);
	float dist_1 = D3DXFROMWINEVec4Dot(&corner_verts[1], &plane_left);
	float dist_2 = D3DXFROMWINEVec4Dot(&corner_verts[2], &plane_left);
	float dist_3 = D3DXFROMWINEVec4Dot(&corner_verts[3], &plane_left);
	if (dist_0 < 0 && dist_1 < 0 && dist_2 < 0 && dist_3 < 0)
		return false;

	// Test against right plane
	dist_0 = D3DXFROMWINEVec4Dot(&corner_verts[0], &plane_right);
	dist_1 = D3DXFROMWINEVec4Dot(&corner_verts[1], &plane_right);
	dist_2 = D3DXFROMWINEVec4Dot(&corner_verts[2], &plane_right);
	dist_3 = D3DXFROMWINEVec4Dot(&corner_verts[3], &plane_right);
	if (dist_0 < 0 && dist_1 < 0 && dist_2 < 0 && dist_3 < 0)
		return false;

	// Test against bottom plane
	dist_0 = D3DXFROMWINEVec4Dot(&corner_verts[0], &plane_bottom);
	dist_1 = D3DXFROMWINEVec4Dot(&corner_verts[1], &plane_bottom);
	dist_2 = D3DXFROMWINEVec4Dot(&corner_verts[2], &plane_bottom);
	dist_3 = D3DXFROMWINEVec4Dot(&corner_verts[3], &plane_bottom);
	if (dist_0 < 0 && dist_1 < 0 && dist_2 < 0 && dist_3 < 0)
		return false;

	// Test against top plane
	dist_0 = D3DXFROMWINEVec4Dot(&corner_verts[0], &plane_top);
	dist_1 = D3DXFROMWINEVec4Dot(&corner_verts[1], &plane_top);
	dist_2 = D3DXFROMWINEVec4Dot(&corner_verts[2], &plane_top);
	dist_3 = D3DXFROMWINEVec4Dot(&corner_verts[3], &plane_top);
	if (dist_0 < 0 && dist_1 < 0 && dist_2 < 0 && dist_3 < 0)
		return false;

	return true;
}

float estimateGridCoverage(const QuadNode& quad_node, const CBaseCamera& camera, float screen_area)
{
	// Estimate projected area

	// Test 16 points on the quad and find out the biggest one.
	const static float sample_pos[16][2] =
	{
		{0, 0},
		{0, 1},
		{1, 0},
		{1, 1},
		{0.5f, 0.333f},
		{0.25f, 0.667f},
		{0.75f, 0.111f},
		{0.125f, 0.444f},
		{0.625f, 0.778f},
		{0.375f, 0.222f},
		{0.875f, 0.556f},
		{0.0625f, 0.889f},
		{0.5625f, 0.037f},
		{0.3125f, 0.37f},
		{0.8125f, 0.704f},
		{0.1875f, 0.148f},
	};

	D3DXFROMWINEMATRIX matProj = *camera.GetProjMatrix();
	D3DXFROMWINEVECTOR3 eye_point = *camera.GetEyePt();
	eye_point = D3DXFROMWINEVECTOR3(eye_point.x, eye_point.z, eye_point.y);
	float grid_len_world = quad_node.length / g_MeshDim;

	float max_area_proj = 0;
	for (int i = 0; i < 16; i++)
	{
		D3DXFROMWINEVECTOR3 test_point(quad_node.bottom_left.x + quad_node.length * sample_pos[i][0], quad_node.bottom_left.y + quad_node.length * sample_pos[i][1], 0);
		D3DXFROMWINEVECTOR3 eye_vec = test_point - eye_point;
		float dist = D3DXFROMWINEVec3Length(&eye_vec);

		float area_world = grid_len_world * grid_len_world;// * abs(eye_point.z) / sqrt(nearest_sqr_dist);
		float area_proj = area_world * matProj(0, 0) * matProj(1, 1) / (dist * dist);

		if (max_area_proj < area_proj)
			max_area_proj = area_proj;
	}

	float pixel_coverage = max_area_proj * screen_area * 0.25f;

	return pixel_coverage;
}

bool isLeaf(const QuadNode& quad_node)
{
	return (quad_node.sub_node[0] == -1 && quad_node.sub_node[1] == -1 && quad_node.sub_node[2] == -1 && quad_node.sub_node[3] == -1);
}

int searchLeaf(const vector<QuadNode>& node_list, const D3DXFROMWINEVECTOR2& point)
{
	int index = -1;
	
	int size = (int)node_list.size();
	QuadNode node = node_list[size - 1];

	while (!isLeaf(node))
	{
		bool found = false;

		for (int i = 0; i < 4; i++)
		{
			index = node.sub_node[i];
			if (index == -1)
				continue;

			QuadNode sub_node = node_list[index];
			if (point.x >= sub_node.bottom_left.x && point.x <= sub_node.bottom_left.x + sub_node.length &&
				point.y >= sub_node.bottom_left.y && point.y <= sub_node.bottom_left.y + sub_node.length)
			{
				node = sub_node;
				found = true;
				break;
			}
		}

		if (!found)
			return -1;
	}

	return index;
}

QuadRenderParam& selectMeshPattern(const QuadNode& quad_node)
{
	// Check 4 adjacent quad.
	D3DXFROMWINEVECTOR2 point_left = quad_node.bottom_left + D3DXFROMWINEVECTOR2(-g_PatchLength * 0.5f, quad_node.length * 0.5f);
	int left_adj_index = searchLeaf(g_render_list, point_left);

	D3DXFROMWINEVECTOR2 point_right = quad_node.bottom_left + D3DXFROMWINEVECTOR2(quad_node.length + g_PatchLength * 0.5f, quad_node.length * 0.5f);
	int right_adj_index = searchLeaf(g_render_list, point_right);

	D3DXFROMWINEVECTOR2 point_bottom = quad_node.bottom_left + D3DXFROMWINEVECTOR2(quad_node.length * 0.5f, -g_PatchLength * 0.5f);
	int bottom_adj_index = searchLeaf(g_render_list, point_bottom);

	D3DXFROMWINEVECTOR2 point_top = quad_node.bottom_left + D3DXFROMWINEVECTOR2(quad_node.length * 0.5f, quad_node.length + g_PatchLength * 0.5f);
	int top_adj_index = searchLeaf(g_render_list, point_top);

	int left_type = 0;
	if (left_adj_index != -1 && g_render_list[left_adj_index].length > quad_node.length * 0.999f)
	{
		QuadNode adj_node = g_render_list[left_adj_index];
		float scale = adj_node.length / quad_node.length * (g_MeshDim >> quad_node.lod) / (g_MeshDim >> adj_node.lod);
		if (scale > 3.999f)
			left_type = 2;
		else if (scale > 1.999f)
			left_type = 1;
	}

	int right_type = 0;
	if (right_adj_index != -1 && g_render_list[right_adj_index].length > quad_node.length * 0.999f)
	{
		QuadNode adj_node = g_render_list[right_adj_index];
		float scale = adj_node.length / quad_node.length * (g_MeshDim >> quad_node.lod) / (g_MeshDim >> adj_node.lod);
		if (scale > 3.999f)
			right_type = 2;
		else if (scale > 1.999f)
			right_type = 1;
	}

	int bottom_type = 0;
	if (bottom_adj_index != -1 && g_render_list[bottom_adj_index].length > quad_node.length * 0.999f)
	{
		QuadNode adj_node = g_render_list[bottom_adj_index];
		float scale = adj_node.length / quad_node.length * (g_MeshDim >> quad_node.lod) / (g_MeshDim >> adj_node.lod);
		if (scale > 3.999f)
			bottom_type = 2;
		else if (scale > 1.999f)
			bottom_type = 1;
	}

	int top_type = 0;
	if (top_adj_index != -1 && g_render_list[top_adj_index].length > quad_node.length * 0.999f)
	{
		QuadNode adj_node = g_render_list[top_adj_index];
		float scale = adj_node.length / quad_node.length * (g_MeshDim >> quad_node.lod) / (g_MeshDim >> adj_node.lod);
		if (scale > 3.999f)
			top_type = 2;
		else if (scale > 1.999f)
			top_type = 1;
	}

	// Check lookup table, [L][R][B][T]
	return g_mesh_patterns[quad_node.lod][left_type][right_type][bottom_type][top_type];
}

// Return value: if successful pushed into the list, return the position. If failed, return -1.
int buildNodeList(QuadNode& quad_node, const CBaseCamera& camera)
{
	// Check against view frustum
	if (!checkNodeVisibility(quad_node, camera))
		return -1;

	// Estimate the min grid coverage
	unsigned int num_vps = 1;
	D3DFROMWINEVIEWPORT9 vp;
	DXUTGetD3D11DeviceContext()->RSGetViewports(&num_vps, &vp);
	float min_coverage = estimateGridCoverage(quad_node, camera, (float)vp.Width * vp.Height);

	// Recursively attatch sub-nodes.
	bool visible = true;
	if (min_coverage > g_UpperGridCoverage && quad_node.length > g_PatchLength)
	{
		// Recursive rendering for sub-quads.
		QuadNode sub_node_0 = {quad_node.bottom_left, quad_node.length / 2, 0, {-1, -1, -1, -1}};
		quad_node.sub_node[0] = buildNodeList(sub_node_0, camera);

		QuadNode sub_node_1 = {quad_node.bottom_left + D3DXFROMWINEVECTOR2(quad_node.length/2, 0), quad_node.length / 2, 0, {-1, -1, -1, -1}};
		quad_node.sub_node[1] = buildNodeList(sub_node_1, camera);

		QuadNode sub_node_2 = {quad_node.bottom_left + D3DXFROMWINEVECTOR2(quad_node.length/2, quad_node.length/2), quad_node.length / 2, 0, {-1, -1, -1, -1}};
		quad_node.sub_node[2] = buildNodeList(sub_node_2, camera);

		QuadNode sub_node_3 = {quad_node.bottom_left + D3DXFROMWINEVECTOR2(0, quad_node.length/2), quad_node.length / 2, 0, {-1, -1, -1, -1}};
		quad_node.sub_node[3] = buildNodeList(sub_node_3, camera);

		visible = !isLeaf(quad_node);
	}

	if (visible)
	{
		// Estimate mesh LOD
		int lod = 0;
		for (lod = 0; lod < g_Lods - 1; lod++)
		{
			if (min_coverage > g_UpperGridCoverage)
				break;
			min_coverage *= 4;
		}

		// We don't use 1x1 and 2x2 patch. So the highest level is g_Lods - 2.
		quad_node.lod = min(lod, g_Lods - 2);
	}
	else
		return -1;

	// Insert into the list
	int position = (int)g_render_list.size();
	g_render_list.push_back(quad_node);

	return position;
}

void renderShaded(const stx_CFirstPersonCamera& camera, ShaderResourceViewID displacemnet_map, ShaderResourceViewID gradient_map, float time)
{
	#if 0 // ???
	// Build rendering list
	g_render_list.clear();
	float ocean_extent = g_PatchLength * (1 << g_FurthestCover);
	QuadNode root_node = {D3DXFROMWINEVECTOR2(-ocean_extent * 0.5f, -ocean_extent * 0.5f), ocean_extent, 0, {-1,-1,-1,-1}};
	buildNodeList(root_node, camera);

	// Matrices
	D3DXFROMWINEMATRIX matView = D3DXFROMWINEMATRIX(1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1) * *camera.GetViewMatrix();
	D3DXFROMWINEMATRIX matProj = *camera.GetProjMatrix();

	// VS & PS
	IRenderer::GetRendererInstance()->VSSetShader(g_pOceanSurfVS, 0, 0);
	IRenderer::GetRendererInstance()->PSSetShader(g_pOceanSurfPS, 0, 0);

	// Textures
	ShaderResourceViewID vs_srvs[2] = {displacemnet_map, g_pSRV_Perlin};
	IRenderer::GetRendererInstance()->VSSetShaderResources(0, 2, &vs_srvs[0]);

	ShaderResourceViewID ps_srvs[4] = {g_pSRV_Perlin, gradient_map, g_pSRV_Fresnel, g_pSRV_ReflectCube};
    IRenderer::GetRendererInstance()->PSSetShaderResources(1, 4, &ps_srvs[0]);

	// Samplers
	SamplerStateID  vs_samplers[2] = {g_pHeightSampler, g_pPerlinSampler};
	IRenderer::GetRendererInstance()->VSSetSamplers(0, 2, &vs_samplers[0]);

	SamplerStateID  ps_samplers[4] = {g_pPerlinSampler, g_pGradientSampler, g_pFresnelSampler, g_pCubeSampler};
	IRenderer::GetRendererInstance()->PSSetSamplers(1, 4, &ps_samplers[0]);

	// IA setup
	IRenderer::GetRendererInstance()->setIndexBuffer(g_pMeshIB, FORMAT_R32_UINT, 0);

	VertexBufferID vbs[1] = {g_pMeshVB};
	unsigned int strides[1] = {sizeof(ocean_vertex)};
	unsigned int offsets[1] = {0};
	IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &vbs[0], &strides[0], &offsets[0]);

	IRenderer::GetRendererInstance()->setVertexFormat(g_pMeshLayout);

	// State blocks
	IRenderer::GetRendererInstance()->RSSetState(g_pRSState_Solid);

	// Constants
	VertexBufferID cbs[1] = {g_pShadingCB};
	IRenderer::GetRendererInstance()->VSSetConstantBuffers(2, 1, cbs);
	IRenderer::GetRendererInstance()->PSSetConstantBuffers(2, 1, cbs);

	// We assume the center of the ocean surface at (0, 0, 0).
	for (int i = 0; i < (int)g_render_list.size(); i++)
	{
		QuadNode& node = g_render_list[i];
		
		if (!isLeaf(node))
			continue;

		// Check adjacent patches and select mesh pattern
		QuadRenderParam& render_param = selectMeshPattern(node);

		// Find the right LOD to render
		int level_size = g_MeshDim;
		for (int lod = 0; lod < node.lod; lod++)
			level_size >>= 1;

		// Matrices and constants
		Const_Per_Call call_consts;

		// Expand of the local coordinate to world space patch size
		D3DXFROMWINEMATRIX matScale;
		D3DXFROMWINEMatrixScaling(&matScale, node.length / level_size, node.length / level_size, 0);
		D3DXFROMWINEMatrixTranspose(&call_consts.g_matLocal, &matScale);

		// WVP matrix
		D3DXFROMWINEMATRIX matWorld;
		D3DXFROMWINEMatrixTranslation(&matWorld, node.bottom_left.x, node.bottom_left.y, 0);
		D3DXFROMWINEMATRIX matWVP = matWorld * matView * matProj;
		D3DXFROMWINEMatrixTranspose(&call_consts.g_matWorldViewProj, &matWVP);

		// Texcoord for perlin noise
		D3DXFROMWINEVECTOR2 uv_base = node.bottom_left / g_PatchLength * g_PerlinSize;
		call_consts.g_UVBase = uv_base;

		// Constant g_PerlinSpeed need to be adjusted mannually
		D3DXFROMWINEVECTOR2 perlin_move = -g_WindDir * time * g_PerlinSpeed;
		call_consts.g_PerlinMovement = perlin_move;

		// Eye point
		D3DXFROMWINEMATRIX matInvWV = matWorld * matView;
		D3DXFROMWINEMatrixInverse(&matInvWV, 0, &matInvWV);
		D3DXFROMWINEVECTOR3 vLocalEye(0, 0, 0);
		D3DXFROMWINEVec3TransformCoord(&vLocalEye, &vLocalEye, &matInvWV);
		call_consts.g_LocalEye = vLocalEye;

		// Update constant buffer
		            
		IRenderer::GetRendererInstance()->Map(g_pPerCallCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res);
		assert(mapped_res.pData);
		*(Const_Per_Call*)mapped_res.pData = call_consts;
		IRenderer::GetRendererInstance()->Unmap(g_pPerCallCB, 0);

		cbs[0] = g_pPerCallCB;
		IRenderer::GetRendererInstance()->VSSetConstantBuffers(4, 1, cbs);
		IRenderer::GetRendererInstance()->PSSetConstantBuffers(4, 1, cbs);

		// Perform draw call
		if (render_param.num_inner_faces > 0)
		{
			// Inner mesh of the patch
			IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP);
			IRenderer::GetRendererInstance()->DrawIndexed(render_param.num_inner_faces + 2, render_param.inner_start_index, 0);
		}

		if (render_param.num_boundary_faces > 0)
		{
			// Boundary mesh of the patch
			IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLES);
			IRenderer::GetRendererInstance()->DrawIndexed(render_param.num_boundary_faces * 3, render_param.boundary_start_index, 0);
		}
	}

	// Unbind
	vs_srvs[0] = 0;
	vs_srvs[1] = 0;
	IRenderer::GetRendererInstance()->VSSetShaderResources(0, 2, &vs_srvs[0]);

	ps_srvs[0] = 0;
	ps_srvs[1] = 0;
	ps_srvs[2] = 0;
	ps_srvs[3] = 0;
    IRenderer::GetRendererInstance()->PSSetShaderResources(1, 4, &ps_srvs[0]);
	#endif
}

void renderWireframe(const stx_CFirstPersonCamera& camera, ShaderResourceViewID displacemnet_map, float time)
{
	#if 0 // ???
	// Build rendering list
	g_render_list.clear();
	float ocean_extent = g_PatchLength * (1 << g_FurthestCover);
	QuadNode root_node = {D3DXFROMWINEVECTOR2(-ocean_extent * 0.5f, -ocean_extent * 0.5f), ocean_extent, 0, {-1,-1,-1,-1}};
	buildNodeList(root_node, camera);

	// Matrices
	D3DXFROMWINEMATRIX matView = D3DXFROMWINEMATRIX(1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1) * *camera.GetViewMatrix();
	D3DXFROMWINEMATRIX matProj = *camera.GetProjMatrix();

	// VS & PS
	IRenderer::GetRendererInstance()->VSSetShader(g_pOceanSurfVS, 0, 0);
	IRenderer::GetRendererInstance()->PSSetShader(g_pWireframePS, 0, 0);

	// Textures
	ShaderResourceViewID vs_srvs[2] = {displacemnet_map, g_pSRV_Perlin};
	IRenderer::GetRendererInstance()->VSSetShaderResources(0, 2, &vs_srvs[0]);

	// Samplers
	SamplerStateID  vs_samplers[2] = {g_pHeightSampler, g_pPerlinSampler};
	IRenderer::GetRendererInstance()->VSSetSamplers(0, 2, &vs_samplers[0]);

	SamplerStateID  ps_samplers[4] = {0, 0, 0, 0};
	IRenderer::GetRendererInstance()->PSSetSamplers(1, 4, &ps_samplers[0]);

	// IA setup
	IRenderer::GetRendererInstance()->setIndexBuffer(g_pMeshIB, FORMAT_R32_UINT, 0);

	VertexBufferID vbs[1] = {g_pMeshVB};
	unsigned int strides[1] = {sizeof(ocean_vertex)};
	unsigned int offsets[1] = {0};
	IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &vbs[0], &strides[0], &offsets[0]);

	IRenderer::GetRendererInstance()->setVertexFormat(g_pMeshLayout);

	// State blocks
	IRenderer::GetRendererInstance()->RSSetState(g_pRSState_Wireframe);

	// Constants
	VertexBufferID cbs[1] = {g_pShadingCB};
	IRenderer::GetRendererInstance()->VSSetConstantBuffers(2, 1, cbs);
	IRenderer::GetRendererInstance()->PSSetConstantBuffers(2, 1, cbs);

	// We assume the center of the ocean surface is at (0, 0, 0).
	for (int i = 0; i < (int)g_render_list.size(); i++)
	{
		QuadNode& node = g_render_list[i];
		
		if (!isLeaf(node))
			continue;

		// Check adjacent patches and select mesh pattern
		QuadRenderParam& render_param = selectMeshPattern(node);

		// Find the right LOD to render
		int level_size = g_MeshDim;
		for (int lod = 0; lod < node.lod; lod++)
			level_size >>= 1;

		// Matrices and constants
		Const_Per_Call call_consts;

		// Expand of the local coordinate to world space patch size
		D3DXFROMWINEMATRIX matScale;
		D3DXFROMWINEMatrixScaling(&matScale, node.length / level_size, node.length / level_size, 0);
		D3DXFROMWINEMatrixTranspose(&call_consts.g_matLocal, &matScale);

		// WVP matrix
		D3DXFROMWINEMATRIX matWorld;
		D3DXFROMWINEMatrixTranslation(&matWorld, node.bottom_left.x, node.bottom_left.y, 0);
		D3DXFROMWINEMATRIX matWVP = matWorld * matView * matProj;
		D3DXFROMWINEMatrixTranspose(&call_consts.g_matWorldViewProj, &matWVP);

		// Texcoord for perlin noise
		D3DXFROMWINEVECTOR2 uv_base = node.bottom_left / g_PatchLength * g_PerlinSize;
		call_consts.g_UVBase = uv_base;

		// Constant g_PerlinSpeed need to be adjusted mannually
		D3DXFROMWINEVECTOR2 perlin_move = -g_WindDir * time * g_PerlinSpeed;
		call_consts.g_PerlinMovement = perlin_move;

		// Eye point
		D3DXFROMWINEMATRIX matInvWV = matWorld * matView;
		D3DXFROMWINEMatrixInverse(&matInvWV, 0, &matInvWV);
		D3DXFROMWINEVECTOR3 vLocalEye(0, 0, 0);
		D3DXFROMWINEVec3TransformCoord(&vLocalEye, &vLocalEye, &matInvWV);
		call_consts.g_LocalEye = vLocalEye;

		// Update constant buffer
		            
		IRenderer::GetRendererInstance()->Map(g_pPerCallCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res);
		assert(mapped_res.pData);
		*(Const_Per_Call*)mapped_res.pData = call_consts;
		IRenderer::GetRendererInstance()->Unmap(g_pPerCallCB, 0);

		cbs[0] = g_pPerCallCB;
		IRenderer::GetRendererInstance()->VSSetConstantBuffers(4, 1, cbs);
		IRenderer::GetRendererInstance()->PSSetConstantBuffers(4, 1, cbs);

		// Perform draw call
		if (render_param.num_inner_faces > 0)
		{
			// Inner mesh of the patch
			IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP);
			IRenderer::GetRendererInstance()->DrawIndexed(render_param.num_inner_faces + 2, render_param.inner_start_index, 0);
		}

		if (render_param.num_boundary_faces > 0)
		{
			// Boundary mesh of the patch
			IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLES);
			IRenderer::GetRendererInstance()->DrawIndexed(render_param.num_boundary_faces * 3, render_param.boundary_start_index, 0);
		}
	}

	// Unbind
	vs_srvs[0] = 0;
	vs_srvs[1] = 0;
	IRenderer::GetRendererInstance()->VSSetShaderResources(0, 2, &vs_srvs[0]);

	// Restore states
	IRenderer::GetRendererInstance()->RSSetState(g_pRSState_Solid);
	#endif
}

