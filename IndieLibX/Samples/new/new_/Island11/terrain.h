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

#define terrain_gridpoints					512
#define terrain_numpatches_1d				64
#define terrain_geometry_scale				1.0f
#define terrain_maxheight					30.0f 
#define terrain_minheight					-30.0f 
#define terrain_fractalfactor				0.68f;
#define terrain_fractalinitialvalue			100.0f
#define terrain_smoothfactor1				0.99f
#define terrain_smoothfactor2				0.10f
#define terrain_rockfactor					0.95f
#define terrain_smoothsteps					40

#define terrain_height_underwater_start		-100.0f
#define terrain_height_underwater_end		-8.0f
#define terrain_height_sand_start			-30.0f
#define terrain_height_sand_end				1.7f
#define terrain_height_grass_start			1.7f
#define terrain_height_grass_end			30.0f
#define terrain_height_rocks_start			-2.0f
#define terrain_height_trees_start			4.0f
#define terrain_height_trees_end			30.0f
#define terrain_slope_grass_start			0.96f
#define terrain_slope_rocks_start			0.85f

#define terrain_far_range terrain_gridpoints*terrain_geometry_scale

#define shadowmap_resource_buffer_size_xy				4096
#define water_normalmap_resource_buffer_size_xy			2048
#define terrain_layerdef_map_texture_size				1024
#define terrain_depth_shadow_map_texture_size			512

#define sky_gridpoints						10
#define sky_texture_angle					0.425f

#define main_buffer_size_multiplier			1.1f
#define reflection_buffer_size_multiplier   1.1f
#define refraction_buffer_size_multiplier   1.1f

#define scene_z_near						1.0f
#define scene_z_far							25000.0f
#define camera_fov							110.0f

class CTerrain
{
	public:
		void Initialize(stx_Effect*);
		void DeInitialize();
		void ReCreateBuffers();
		void LoadTextures();
		void CreateTerrain();

		float DynamicTesselationFactor;
		float StaticTesselationFactor;
#if 0
		void SetupNormalView(D3DXFROMWINEMATRIX& g_ProjMatrix, D3DXFROMWINEMATRIX& g_ViewMatrix);
		void SetupReflectionView(D3DXFROMWINEMATRIX& g_ProjMatrix, D3DXFROMWINEMATRIX& g_ViewMatrix);
		void SetupRefractionView(D3DXFROMWINEMATRIX& g_ProjMatrix, D3DXFROMWINEMATRIX& g_ViewMatrix);
		void SetupLightView(D3DXFROMWINEMATRIX& g_ProjMatrix, D3DXFROMWINEMATRIX& g_ViewMatrix);
		//void Render(D3DXFROMWINEMATRIX& g_ProjMatrix, D3DXFROMWINEMATRIX& g_ViewMatrix);
#else
        void SetupNormalView(stx_CFirstPersonCamera *cam);
		void SetupReflectionView(stx_CFirstPersonCamera *cam);
		void SetupRefractionView(stx_CFirstPersonCamera *cam);
		void SetupLightView(stx_CFirstPersonCamera *cam);
        void Render(stx_CFirstPersonCamera* cam);
#endif
		float BackbufferWidth;
		float BackbufferHeight;

		unsigned int MultiSampleCount;
		unsigned int MultiSampleQuality;

		TextureID rock_bump_texture;
		ShaderResourceViewID rock_bump_textureSRV;

		TextureID rock_microbump_texture;
		ShaderResourceViewID rock_microbump_textureSRV;

		TextureID rock_diffuse_texture;
		ShaderResourceViewID rock_diffuse_textureSRV;	

		TextureID sand_bump_texture;
		ShaderResourceViewID sand_bump_textureSRV;

		TextureID sand_microbump_texture;
		ShaderResourceViewID sand_microbump_textureSRV;

		TextureID sand_diffuse_texture;
		ShaderResourceViewID sand_diffuse_textureSRV;	

		TextureID grass_diffuse_texture;
		ShaderResourceViewID grass_diffuse_textureSRV;	

		TextureID slope_diffuse_texture;
		ShaderResourceViewID slope_diffuse_textureSRV;	

		TextureID water_bump_texture;
		ShaderResourceViewID water_bump_textureSRV;	

		TextureID sky_texture;
		ShaderResourceViewID sky_textureSRV;	

		TextureID reflection_color_resource;
		ShaderResourceViewID reflection_color_resourceSRV;
		RenderTargetViewID reflection_color_resourceRTV;
		TextureID refraction_color_resource;
		ShaderResourceViewID refraction_color_resourceSRV;
		RenderTargetViewID refraction_color_resourceRTV;

		TextureID shadowmap_resource;
		ShaderResourceViewID shadowmap_resourceSRV;
		DepthStencilViewID shadowmap_resourceDSV;

		TextureID reflection_depth_resource;
		DepthStencilViewID reflection_depth_resourceDSV;


		TextureID refraction_depth_resource;
		RenderTargetViewID refraction_depth_resourceRTV;
		ShaderResourceViewID refraction_depth_resourceSRV;

		TextureID water_normalmap_resource;
		ShaderResourceViewID water_normalmap_resourceSRV;
		RenderTargetViewID water_normalmap_resourceRTV;

		TextureID main_color_resource;
		ShaderResourceViewID main_color_resourceSRV;
		RenderTargetViewID main_color_resourceRTV;
		TextureID main_depth_resource;
		DepthStencilViewID main_depth_resourceDSV;
		ShaderResourceViewID main_depth_resourceSRV;
		TextureID main_color_resource_resolved;
		ShaderResourceViewID main_color_resource_resolvedSRV;
		stx_Effect* pEffect;

		float				height[terrain_gridpoints+1][terrain_gridpoints+1];
		D3DXFROMWINEVECTOR3			normal[terrain_gridpoints+1][terrain_gridpoints+1];
		D3DXFROMWINEVECTOR3			tangent[terrain_gridpoints+1][terrain_gridpoints+1];
		D3DXFROMWINEVECTOR3			binormal[terrain_gridpoints+1][terrain_gridpoints+1];

		TextureID heightmap_texture;
		ShaderResourceViewID heightmap_textureSRV;

		TextureID layerdef_texture;
		ShaderResourceViewID layerdef_textureSRV;

		TextureID depthmap_texture;
		ShaderResourceViewID depthmap_textureSRV;

		VertexBufferID heightfield_vertexbuffer;
		VertexBufferID sky_vertexbuffer;

		VertexFormatID heightfield_inputlayout;
		VertexFormatID trianglestrip_inputlayout;

};

float bilinear_interpolation(float fx, float fy, float a, float b, float c, float d);

