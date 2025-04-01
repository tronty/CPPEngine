/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
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

#pragma warning(disable:4995)
#include "hair.h"

#define MAX_RTS_TOTAL 256 
#define MAX_RT_GROUPS 8  // 256/32 = 8 -- we get 32 because in one render call we can render to 8 RTs and each RT can have 4 channels, leading to 8*4=32

struct GRID_TEXTURE_DISPLAY_STRUCT;

class HairGrid
{

public:

     enum RENDER_TARGET
    {
        //RENDER_TARGET_VELOCITY,
		RENDER_TARGET_DENSITY,
		RENDER_TARGET_DENSITY_DEMUX,
        RENDER_TARGET_DENSITY_BLUR_TEMP,
		RENDER_TARGET_DENSITY_BLUR,
		RENDER_TARGET_VOXELIZED_OBSTACLES,
        NUM_RENDER_TARGET_TYPES
    };

    HairGrid ();
	virtual ~HairGrid ();
    int Initialize      ( int width, int height, int depth, stx_Effect*  pEffect , char pEffectPath[MAX_PATH]);
	void Reset( void );
    int AddHairDensity(D3DXFROMWINEMATRIX& objToVolumeXForm, int (*DrawHair)(D3DXFROMWINEMATRIX&,float,float,int));
	void AddDensityBasedForces();
    int DisplayTexture(RENDER_TARGET rtIndex);
	int demultiplexTexture();
	int setFluidObstacleTexture( TextureID  texture, FORMAT format, int texWidth, int texHeight, int texDepth );
    int demultiplexTextureTo3DFluidObstacles();
	int ReloadShader( stx_Effect*  pEffect, char pEffectPath[MAX_PATH] );
	int voxelizeObstacles();

    int setObstacleVoxelizedTexture();
	int unsetObstacleVoxelizedTexture();
    int setDemuxTexture();
	int setBlurTexture();
	int unsetDemuxTexture();
	int unsetBlurTexture();

protected:
    HairGrid                   ( void ) {};
	int LoadShaders        ( stx_Effect*  pEffect,char pEffectPath[MAX_PATH]  );
    void InitScreenSlice(GRID_TEXTURE_DISPLAY_STRUCT** vertices, int z, int& index );
    void InitGridSlice(GRID_TEXTURE_DISPLAY_STRUCT** vertices );
	int CreateVertexBuffer( int ByteWidth, unsigned int bindFlags, exBufferID   vertexBuffer,GRID_TEXTURE_DISPLAY_STRUCT* vertices,int numVertices);  
    void DrawSlicesToScreen( void );
    int CreateRTTextureAsShaderResource(RENDER_TARGET rtIndex, char shaderTextureName,stx_Effect*  pEffect,


    int m_dim[3];
    int m_Width;
	int m_Height;
	int m_Depth;
	int m_numVerticesRenderQuad;
	int m_numVerticesRenderQuadsForGrid;
	int m_cols;
	int m_rows;
	int m_numRTVs;
	int m_totalDepth;

	int m_fluidGridWidth;
    int m_fluidGridHeight;
	int m_fluidGridDepth;

    //density
	TextureID                      pDensity2DTextureArray;
	TextureID               pDensityRenderTargetViews    [ MAX_RT_GROUPS*8 ]; 
    //demux
	TextureID                      pDensityDemux2DTextureArray;
	TextureID               pDensityDemuxRenderTargetViews    [ MAX_RTS_TOTAL ];
	//blurx
	TextureID                      pDensityBlurTemp2DTextureArray;
	TextureID               pDensityBlurTempRenderTargetViews    [ MAX_RTS_TOTAL ];
	//blury
	TextureID                      pDensityBlur2DTextureArray;
	TextureID               pDensityBlurRenderTargetViews    [ MAX_RTS_TOTAL ];
	//obstacles
	TextureID                      pVoxelizedObstacle2DTextureArray;
	TextureID               pVoxelizedObstacleRenderTargetViews    [ MAX_RTS_TOTAL ];


     //fluid obstacle texture
	TextureID                      m_pFluidObstacleTexture3D;
    TextureID               m_pRenderTargetViewFluidObstacles;


    TextureID             pRenderTargetShaderViews     [ NUM_RENDER_TARGET_TYPES ];
    stx_Effect::stx_Variable*  pShaderResourceVariables     [ NUM_RENDER_TARGET_TYPES ];
    FORMAT                          RenderTargetFormats          [ NUM_RENDER_TARGET_TYPES ]; 

	
	
	stx_Effect*              m_pEffect;
	
	stx_Effect::stx_Technique*     TechniqueDrawTexture;
	stx_Effect::stx_Technique*     TechniqueDrawTextureDemuxed;
	stx_Effect::stx_Technique*     TechniqueDemux;
	stx_Effect::stx_Technique*     TechniqueVoxelizeObstacles;
	stx_Effect::stx_Technique*     TechniqueDemuxTo3DFluidObstacles;

    exBufferID               renderQuadBuffer;
    exBufferID               renderQuadBufferForGrid;
    VertexFormatID          quad_layout;

    stx_Effect::stx_Variable*          m_TextureWidthVar;
    stx_Effect::stx_Variable*          m_TextureHeightVar;
    stx_Effect::stx_Variable*          m_TextureDepthVar;
	stx_Effect::stx_Variable*          m_RowWidth;
    stx_Effect::stx_Variable*          m_ColWidth;  
	stx_Effect::stx_Variable*          m_textureIndexShaderVariable; //which render target texture to render from
	stx_Effect::stx_Variable*          m_GridZShaderVariable; //which slice of texture array to read from
    stx_Effect::stx_Variable*          m_BlurDirectionShaderVariable;   
	stx_Effect::stx_Variable*  m_TextureToBlurShaderShaderVariable;
};
