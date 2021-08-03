/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
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

#pragma warning(disable:4995) // avoid warning for "...was marked as #pragma deprecated"
#pragma warning(disable:4244) // avoid warning for "conversion from 'float' to 'int'"
#pragma warning(disable:4201) //avoid warning from mmsystem.h "warning C4201: nonstandard extension used : nameless struct/union"

#include "Hair.h"
#include "DensityGrid.h"
#include "Common.h"
#include <Strsafe.h>

HairGrid::HairGrid():renderQuadBuffer(0),renderQuadBufferForGrid(0),m_pEffect(0), m_cols(0), m_rows(0),
m_fluidGridWidth(0),m_fluidGridHeight(0),m_fluidGridDepth(0)
{
    IRenderer::GetRendererInstance() = 0;
    IRenderer::GetRendererInstance() = 0;

    SAFE_ACQUIRE();
    SAFE_ACQUIRE();

	//formats
    RenderTargetFormats [RENDER_TARGET_DENSITY]         = FORMAT_RGBA16F;
    RenderTargetFormats [RENDER_TARGET_DENSITY_DEMUX]      = FORMAT_R16_FLOAT;
    RenderTargetFormats [RENDER_TARGET_DENSITY_BLUR_TEMP]  = FORMAT_R16_FLOAT;
    RenderTargetFormats [RENDER_TARGET_DENSITY_BLUR]       = FORMAT_R16_FLOAT;
    RenderTargetFormats [RENDER_TARGET_VOXELIZED_OBSTACLES] = FORMAT_R16_FLOAT;

    //textures
	pDensity2DTextureArray = 0;
    pDensityDemux2DTextureArray = 0;
    pVoxelizedObstacle2DTextureArray = 0;
    pDensityBlurTemp2DTextureArray = 0;
    pDensityBlur2DTextureArray = 0;
	m_pFluidObstacleTexture3D = 0;

	m_pRenderTargetViewFluidObstacles = 0;
    
	//render target views
	stx_memset(pDensityRenderTargetViews, 0, sizeof(pDensityRenderTargetViews));
	stx_memset(pDensityDemuxRenderTargetViews, 0, sizeof(pDensityDemuxRenderTargetViews));
	stx_memset(pVoxelizedObstacleRenderTargetViews, 0, sizeof(pVoxelizedObstacleRenderTargetViews));
	stx_memset(pDensityBlurTempRenderTargetViews, 0, sizeof(pDensityBlurTempRenderTargetViews));
	stx_memset(pDensityBlurRenderTargetViews, 0, sizeof(pDensityBlurRenderTargetViews));
    
	//shader resource views
 	stx_memset(pRenderTargetShaderViews, 0, sizeof(pRenderTargetShaderViews));
}

int HairGrid::ReloadShader(stx_Effect*  pEffect,char pEffectPath[MAX_PATH])
{
	//////////////////SAFE_RELEASE(m_pEffect);
	m_pEffect = 0;
	return LoadShaders(pEffect,pEffectPath);
}

int HairGrid::Initialize( int width, int height, int depth, stx_Effect*  pEffect,char pEffectPath[MAX_PATH] )
{
    int hr;
 
	m_totalDepth = depth;

    m_Width = m_dim[0] = width;
	m_Height = m_dim[1] = height;
	m_Depth = m_dim[2] = (depth/4.0 + 0.5);

    m_rows =(int)floorf(sqrtf((float)m_totalDepth));
    m_cols = m_rows;
    while( m_rows * m_cols < m_totalDepth ) 
        m_cols++;
    assert( m_rows*m_cols >= m_totalDepth );   
    
    assert( m_totalDepth < MAX_RTS_TOTAL );

    (LoadShaders(pEffect,pEffectPath));


    //Create the textures ------------------------------------------------------------------------------------------------- 

    //create the density texture array
	//each texture has 4 channels so we need m_totalDepth/4 textures to store
	SURFACE_DESC desc;
    desc.BindFlags = BIND_SHADER_RESOURCE | BIND_RENDER_TARGET;
    desc.CPUAccessFlags = 0; 
    desc.MipLevels = 1;
    desc.MiscFlags = 0;
    desc.Usage = USAGE_DEFAULT;
    desc.Width =  m_Width;
    desc.Height = m_Height;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;


	//create the demux and blur textures
	
	FORMAT format = RenderTargetFormats[RENDER_TARGET_DENSITY_DEMUX];
	Image3 img;
	img.create(format, m_Width, m_Height, 1, m_totalDepth);
	pDensityDemux2DTextureArray=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear()); 

	format = RenderTargetFormats[RENDER_TARGET_VOXELIZED_OBSTACLES];
	img.create(format, m_Width, m_Height, 1, m_totalDepth);
	pVoxelizedObstacle2DTextureArray=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear()); 
	
	format = RenderTargetFormats[RENDER_TARGET_DENSITY_BLUR_TEMP];
	img.create(format, m_Width, m_Height, 1, m_totalDepth);
	pDensityBlurTemp2DTextureArray=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear()); 
	
	format = RenderTargetFormats[RENDER_TARGET_DENSITY_BLUR];
	img.create(format, m_Width, m_Height, 1, m_totalDepth);
	pDensityBlur2DTextureArray=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear()); 

	format = RenderTargetFormats[RENDER_TARGET_DENSITY];
	img.create(format, m_Width, m_Height, 1, m_Depth);
	pDensity2DTextureArray=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());

	//create the renderTargetViews -------------------------------------------------------------------------------------------------
    SURFACE_DESC DescRT;
    DescRT.ViewDimension = RTV_DIMENSION_TEXTURE2DARRAY;
	DescRT.Texture2DArray.MipSlice = 0;	
	DescRT.Texture2DArray.ArraySize = 1;

	DescRT.Format = RenderTargetFormats[RENDER_TARGET_DENSITY_DEMUX];
	for(int i=0;i<m_totalDepth;i++)
	{
	    DescRT.Texture2DArray.FirstArraySlice = i;
        ( IRenderer::GetRendererInstance()->CreateRenderTargetView( pDensityDemux2DTextureArray, &DescRT, &pDensityDemuxRenderTargetViews[i]) );
		( IRenderer::GetRendererInstance()->CreateRenderTargetView( pVoxelizedObstacle2DTextureArray, &DescRT, &pVoxelizedObstacleRenderTargetViews[i]) );
        ( IRenderer::GetRendererInstance()->CreateRenderTargetView( pDensityBlurTemp2DTextureArray, &DescRT, &pDensityBlurTempRenderTargetViews[i]) );
        ( IRenderer::GetRendererInstance()->CreateRenderTargetView( pDensityBlur2DTextureArray, &DescRT, &pDensityBlurRenderTargetViews[i]) );
	}

    DescRT.Format = RenderTargetFormats[RENDER_TARGET_DENSITY];
	for(int i=0;i<m_Depth;i++)
	{
	    DescRT.Texture2DArray.FirstArraySlice = i;
        ( IRenderer::GetRendererInstance()->CreateRenderTargetView( pDensity2DTextureArray, &DescRT, &pDensityRenderTargetViews[i]) );
	}




    //-------------------------------------------------------------------------------------------------------------------------------
    // Create the "shader resource view" (SRView) and "shader resource variable" (SRVar) for the given texture 
	
    ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
    SRVDesc.ViewDimension = SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.FirstArraySlice = 0;
	SRVDesc.Texture2DArray.MipLevels = 1;
    SRVDesc.Texture2DArray.MostDetailedMip = 0;

	SRVDesc.Texture2DArray.ArraySize = m_totalDepth;
    SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_DENSITY_DEMUX];
    (CreateRTTextureAsShaderResource(RENDER_TARGET_DENSITY_DEMUX,"Texture_density_Demux",m_pEffect,&SRVDesc));
    
	SRVDesc.Texture2DArray.ArraySize = m_totalDepth;
    SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_VOXELIZED_OBSTACLES];
    (CreateRTTextureAsShaderResource(RENDER_TARGET_VOXELIZED_OBSTACLES,"Texture_Voxelized_Obstacles",m_pEffect,&SRVDesc));

	SRVDesc.Texture2DArray.ArraySize = m_totalDepth;
	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_DENSITY_BLUR_TEMP];
    (CreateRTTextureAsShaderResource(RENDER_TARGET_DENSITY_BLUR_TEMP,"Texture_density_Blur_Temp",m_pEffect,&SRVDesc));
    
	SRVDesc.Texture2DArray.ArraySize = m_totalDepth;
	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_DENSITY_BLUR];
    (CreateRTTextureAsShaderResource(RENDER_TARGET_DENSITY_BLUR,"Texture_density_Blur",m_pEffect,&SRVDesc));
	
	SRVDesc.Texture2DArray.ArraySize = m_Depth;
	SRVDesc.Format = RenderTargetFormats[RENDER_TARGET_DENSITY];
    (CreateRTTextureAsShaderResource(RENDER_TARGET_DENSITY,"Texture_density",m_pEffect,&SRVDesc));


    Reset();

    //make the vertex buffers--------------------------------------------------------------------------------------------------------

	// Create layout
	FormatDesc layoutDesc[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3
	};
	quad_layout = IRenderer::GetRendererInstance()->addVertexFormat(layoutDesc, elementsOf(layoutDesc), -1);

	// Vertex buffer for "m_totalDepth" number quads to draw all the slices of the 3D-texture as a flat 3D-texture to the screen
    // (used to draw all the individual slices at once to the screen buffer)
	GRID_TEXTURE_DISPLAY_STRUCT *renderQuad(0);
    #define VERTICES_PER_SLICE 6
    m_numVerticesRenderQuad = VERTICES_PER_SLICE * m_totalDepth;
    renderQuad = new GRID_TEXTURE_DISPLAY_STRUCT[ m_numVerticesRenderQuad ];
    int index = 0;
    for(int z=0; z<m_totalDepth; z++)
        InitScreenSlice(&renderQuad,z,index);
    (CreateVertexBuffer(sizeof(GRID_TEXTURE_DISPLAY_STRUCT)*m_numVerticesRenderQuad,BIND_VERTEX_BUFFER, &renderQuadBuffer, renderQuad, m_numVerticesRenderQuad));
    delete [] renderQuad;
    renderQuad = 0;

    //Vertex buffer for rendering to a 2D texture array
    m_numVerticesRenderQuadsForGrid = VERTICES_PER_SLICE;
    renderQuad = new GRID_TEXTURE_DISPLAY_STRUCT[ m_numVerticesRenderQuadsForGrid ];
    InitGridSlice(&renderQuad);
    (CreateVertexBuffer(sizeof(GRID_TEXTURE_DISPLAY_STRUCT)*m_numVerticesRenderQuadsForGrid,BIND_VERTEX_BUFFER, &renderQuadBufferForGrid, renderQuad, m_numVerticesRenderQuadsForGrid));
    delete [] renderQuad;
    renderQuad = 0;


    return 0;
}

HairGrid::~HairGrid( void )
{

    //////////////////SAFE_RELEASE();
	//////////////////SAFE_RELEASE();
    //////////////////SAFE_RELEASE(m_pEffect);

	//////////////////SAFE_RELEASE(pDensity2DTextureArray);
    //////////////////SAFE_RELEASE(pDensityDemux2DTextureArray);
    //////////////////SAFE_RELEASE(pVoxelizedObstacle2DTextureArray);
    //////////////////SAFE_RELEASE(pDensityBlurTemp2DTextureArray);
	//////////////////SAFE_RELEASE(pDensityBlur2DTextureArray);
	
	for(int i=0;i<MAX_RT_GROUPS*8;i++)
       //////////////////SAFE_RELEASE(pDensityRenderTargetViews[i]);
    
	for(int i=0;i<MAX_RTS_TOTAL;i++)
	{
	    //////////////////SAFE_RELEASE(pDensityDemuxRenderTargetViews[i]);
	    //////////////////SAFE_RELEASE(pVoxelizedObstacleRenderTargetViews[i]);		
		//////////////////SAFE_RELEASE(pDensityBlurTempRenderTargetViews[i]);
		//////////////////SAFE_RELEASE(pDensityBlurRenderTargetViews[i]);
	}

    for(int i=0;i<NUM_RENDER_TARGET_TYPES;i++)
		//////////////////SAFE_RELEASE(pRenderTargetShaderViews[i]);

	//////////////////SAFE_RELEASE(renderQuadBuffer);
	//////////////////SAFE_RELEASE(renderQuadBufferForGrid);
    //////////////////SAFE_RELEASE(quad_layout);

	//////////////////SAFE_RELEASE(m_pRenderTargetViewFluidObstacles);
	//////////////////SAFE_RELEASE(m_pFluidObstacleTexture3D);
    m_pFluidObstacleTexture3D = 0;
}


void HairGrid::Reset( void )
{
    float color[4] = {0, 0, 0, 0 };

	for(int i=0;i<m_Depth;i++)
        IRenderer::GetRendererInstance()->ClearRenderTargetView( pDensityRenderTargetViews[i], color );
	
	for(int i=0;i<m_totalDepth;i++)
	{
		IRenderer::GetRendererInstance()->ClearRenderTargetView( pDensityDemuxRenderTargetViews[i], color );
		IRenderer::GetRendererInstance()->ClearRenderTargetView( pVoxelizedObstacleRenderTargetViews[i], color );
		IRenderer::GetRendererInstance()->ClearRenderTargetView( pDensityBlurTempRenderTargetViews[i], color );
		IRenderer::GetRendererInstance()->ClearRenderTargetView( pDensityBlurRenderTargetViews[i], color );
	}
}



int HairGrid::LoadShaders(stx_Effect*  pEffect,char pEffectPath[MAX_PATH])
{
    int hr = 0;
    SAFE_ACQUIRE(m_pEffect,pEffect);

    V_GET_TECHNIQUE_RET( m_pEffect, pEffectPath, TechniqueDrawTexture,"DrawTexture");
    V_GET_TECHNIQUE_RET( m_pEffect, pEffectPath, TechniqueDrawTextureDemuxed,"DrawTextureDemuxed");
    V_GET_TECHNIQUE_RET( m_pEffect, pEffectPath, TechniqueDemux,"TextureDemux");
    V_GET_TECHNIQUE_RET( m_pEffect, pEffectPath, TechniqueVoxelizeObstacles,"VoxelizeObstacles");
    V_GET_TECHNIQUE_RET( m_pEffect, pEffectPath, TechniqueDemuxTo3DFluidObstacles,"DemuxTo3DFluidObstacles");

    ( m_pEffect, pEffectPath, m_TextureWidthVar, "textureWidth", AsScalar );
    ( m_pEffect, pEffectPath, m_TextureHeightVar, "textureHeight", AsScalar );
	( m_pEffect, pEffectPath, m_TextureDepthVar, "textureDepth", AsScalar );
    ( m_pEffect, pEffectPath, m_RowWidth, "rowWidth", AsScalar );
    ( m_pEffect, pEffectPath, m_ColWidth, "colWidth", AsScalar );
    ( m_pEffect, pEffectPath, m_textureIndexShaderVariable, "textureIndex", AsScalar );
    ( m_pEffect, pEffectPath, m_GridZShaderVariable, "gridZIndex", AsScalar );
    ( m_pEffect, pEffectPath, m_BlurDirectionShaderVariable, "blurDirection", AsVector );
    ( m_pEffect, pEffectPath, m_TextureToBlurShaderShaderVariable, "Texure_to_blur", AsShaderResource );

	(m_TextureWidthVar->SetFloat( float(m_Width)));
    (m_TextureHeightVar->SetFloat(float(m_Height)));
    (m_TextureDepthVar->SetFloat(float(m_totalDepth)));
    (m_RowWidth->SetInt( g_Width/m_cols ));
	(m_ColWidth->SetInt( g_Height/m_rows ));

	m_pEffect->GetVariableByName("fluidTextureWidth")->AsScalar()->SetInt(m_fluidGridWidth);
	m_pEffect->GetVariableByName("fluidTextureHeight")->AsScalar()->SetInt(m_fluidGridHeight);
	m_pEffect->GetVariableByName("fluidTextureDepth")->AsScalar()->SetInt(m_fluidGridDepth);

	pShaderResourceVariables[RENDER_TARGET_DENSITY_DEMUX] = m_pEffect->GetVariableByName( "Texture_density_Demux" )->AsShaderResource();
	pShaderResourceVariables[RENDER_TARGET_VOXELIZED_OBSTACLES] = m_pEffect->GetVariableByName( "Texture_Voxelized_Obstacles" )->AsShaderResource();
	pShaderResourceVariables[RENDER_TARGET_DENSITY_BLUR_TEMP] = m_pEffect->GetVariableByName( "Texture_density_Blur_Temp" )->AsShaderResource();
	pShaderResourceVariables[RENDER_TARGET_DENSITY_BLUR] = m_pEffect->GetVariableByName( "Texture_density_Blur" )->AsShaderResource();
	pShaderResourceVariables[RENDER_TARGET_DENSITY] = m_pEffect->GetVariableByName( "Texture_density" )->AsShaderResource();

    for(int rtIndex =0; rtIndex<int(NUM_RENDER_TARGET_TYPES);rtIndex++)
	{
    	assert(pShaderResourceVariables[rtIndex]->IsValid());
        (pShaderResourceVariables[rtIndex]->SetResource( pRenderTargetShaderViews[rtIndex] ));
	}
	return hr;
}


int HairGrid::CreateRTTextureAsShaderResource(RENDER_TARGET rtIndex, char shaderTextureName,stx_Effect*  pEffect,D3D11_SHADER_RESOURCE_VIEW_DESC *SRVDesc )
{
    int hr;
 
	// Create the "shader resource view" (SRView) and "shader resource variable" (SRVar) for the given texture 
    //////////////////SAFE_RELEASE(pRenderTargetShaderViews[rtIndex]);
	if( rtIndex == RENDER_TARGET_DENSITY )
	{	 (IRenderer::GetRendererInstance()->CreateShaderResourceView( pDensity2DTextureArray,SRVDesc, &pRenderTargetShaderViews[rtIndex])); }
	else if( rtIndex == RENDER_TARGET_DENSITY_DEMUX) 
	{    (IRenderer::GetRendererInstance()->CreateShaderResourceView( pDensityDemux2DTextureArray,SRVDesc, &pRenderTargetShaderViews[rtIndex])); }
	else if( rtIndex == RENDER_TARGET_VOXELIZED_OBSTACLES) 
	{    (IRenderer::GetRendererInstance()->CreateShaderResourceView( pVoxelizedObstacle2DTextureArray,SRVDesc, &pRenderTargetShaderViews[rtIndex])); }
	else if(rtIndex == RENDER_TARGET_DENSITY_BLUR_TEMP)
	{    (IRenderer::GetRendererInstance()->CreateShaderResourceView( pDensityBlurTemp2DTextureArray,SRVDesc, &pRenderTargetShaderViews[rtIndex])); }
	else if(rtIndex == RENDER_TARGET_DENSITY_BLUR)
	{    (IRenderer::GetRendererInstance()->CreateShaderResourceView( pDensityBlur2DTextureArray,SRVDesc, &pRenderTargetShaderViews[rtIndex])); }
	else
		return 1;
	
	pShaderResourceVariables[rtIndex] = pEffect->GetVariableByName( shaderTextureName )->AsShaderResource();
    assert(pShaderResourceVariables[rtIndex]->IsValid());
    
    return 0;
}



//assuming that we are going to be rendering parallel to the z direction 
int HairGrid::AddHairDensity(D3DXFROMWINEMATRIX& objToVolumeXForm, int (*DrawHair)(D3DXFROMWINEMATRIX&,float,float,int))
{
    int hr = 0;

    float color[4] = {0, 0, 0, 0 };

    D3DXFROMWINEMATRIX proj;
    D3DXFROMWINEMATRIX worldViewProj;

	//render to 8 render targets at once (use 8 MRT)

    //draw all the hair
	for(int z=0;z<m_Depth;z+=8) 
	{
		//create, clear and set the viewport
		for(int i=z;i<z+8;i++)
		{
		    if(i>=m_Depth)
                break;
			
       	    IRenderer::GetRendererInstance()->ClearRenderTargetView( pDensityRenderTargetViews[i], color );
    	}

		int numSlices;
		if(z<m_Depth-8)
			numSlices = 8;
		else
			numSlices = m_Depth-z;
          

		TextureID  pCnDRTs = new TextureID [numSlices];
		for(int i=0;i<numSlices;i++)
            pCnDRTs[i] = pDensityRenderTargetViews[i+z];   
	
        IRenderer::GetRendererInstance()->OMSetRenderTargets( numSlices, pCnDRTs, 0 ); 

		//set the viewport
        D3DFROMWINEVIEWPORT9 viewport = { 0, 0, m_Width, m_Height, 0.0f, 1.0f };
        IRenderer::GetRendererInstance()->RSSetViewports(1, &viewport);

	    //set the projection matrix with the correct near, far plane
        D3DXFROMWINEMatrixOrthoOffCenterLH(&proj, -0.5, 0.5, -0.5, 0.5, ((float)z)/m_Depth - 0.5f, (float)(z+8)/m_Depth - 0.5f );
        D3DXFROMWINEMatrixMultiply(&worldViewProj, &objToVolumeXForm, &proj);

		DrawHair(worldViewProj, IRenderer::GetRendererInstance(), 0.0f, 1.0/32.0, numSlices); //32 = 8 RTs * 4 slices per RT 

		delete[] pCnDRTs;

	}

    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);

	return hr;
}


int HairGrid::demultiplexTexture()
{
    int hr = 0;

	unsigned int stride[1] = { sizeof(GRID_TEXTURE_DISPLAY_STRUCT) };
    unsigned int offset[1] = { 0 };

	pShaderResourceVariables[RENDER_TARGET_DENSITY]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_DENSITY] );
	pShaderResourceVariables[RENDER_TARGET_VOXELIZED_OBSTACLES]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_VOXELIZED_OBSTACLES] );

    //set the viewport
    D3DFROMWINEVIEWPORT9 viewport = { 0, 0, m_Width, m_Height, 0.0f, 1.0f };
    IRenderer::GetRendererInstance()->RSSetViewports(1, &viewport);

    float color[4] = {0, 0, 0, 0 };

    for(int i=0;i<m_totalDepth;i++)
	{   
   	    IRenderer::GetRendererInstance()->ClearRenderTargetView( pDensityDemuxRenderTargetViews[i], color );
		IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &(pDensityDemuxRenderTargetViews[i]), 0 ); 
    	m_GridZShaderVariable->SetInt(i);
        (TechniqueDemux->GetPassByIndex(0)->Apply(0));

		//render the quad
		IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_TRIANGLES );
        IRenderer::GetRendererInstance()->setVertexFormat(0,  quad_layout );
        IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1,  &renderQuadBufferForGrid, stride, offset );
        IRenderer::GetRendererInstance()->Draw( m_numVerticesRenderQuadsForGrid, 0 ); 
		
	}
	
	pShaderResourceVariables[RENDER_TARGET_VOXELIZED_OBSTACLES]->SetResource( 0 );
	pShaderResourceVariables[RENDER_TARGET_DENSITY]->SetResource( 0 );
	IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
    TechniqueDemux->GetPassByIndex(0)->Apply(0);

	return hr;
}

int HairGrid::voxelizeObstacles()
{
    int hr = 0;

	unsigned int stride[1] = { sizeof(GRID_TEXTURE_DISPLAY_STRUCT) };
    unsigned int offset[1] = { 0 };

    //set the viewport
    D3DFROMWINEVIEWPORT9 viewport = { 0, 0, m_Width, m_Height, 0.0f, 1.0f };
    IRenderer::GetRendererInstance()->RSSetViewports(1, &viewport);

    float color[4] = {0, 0, 0, 0 };

    for(int i=0;i<m_totalDepth;i++)
	{   
   	    IRenderer::GetRendererInstance()->ClearRenderTargetView( pVoxelizedObstacleRenderTargetViews[i], color );
		IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &(pVoxelizedObstacleRenderTargetViews[i]), 0 ); 
    	m_GridZShaderVariable->SetInt(i);
        (TechniqueVoxelizeObstacles->GetPassByIndex(0)->Apply(0));

		//render the quad
		IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_TRIANGLES );
        IRenderer::GetRendererInstance()->setVertexFormat(0,  quad_layout );
        IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1,  &renderQuadBufferForGrid, stride, offset );
        IRenderer::GetRendererInstance()->Draw( m_numVerticesRenderQuadsForGrid, 0 ); 
		
	}
	
	IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
    TechniqueVoxelizeObstacles->GetPassByIndex(0)->Apply(0);

	return hr;
}


int HairGrid::setDemuxTexture()
{
    int hr = 0;
	pShaderResourceVariables[RENDER_TARGET_DENSITY_DEMUX]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_DENSITY_DEMUX] );
	return hr;
}
int HairGrid::setObstacleVoxelizedTexture()
{
    int hr = 0;
	pShaderResourceVariables[RENDER_TARGET_VOXELIZED_OBSTACLES]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_VOXELIZED_OBSTACLES] );
	return hr;
}
int HairGrid::setBlurTexture()
{
    int hr = 0;
	pShaderResourceVariables[RENDER_TARGET_DENSITY_BLUR]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_DENSITY_BLUR] );
	return hr;
}
int HairGrid::unsetDemuxTexture()
{
    int hr = 0;
	pShaderResourceVariables[RENDER_TARGET_DENSITY_DEMUX]->SetResource( 0 );
	return hr;
}
int HairGrid::unsetObstacleVoxelizedTexture()
{
    int hr = 0;
	pShaderResourceVariables[RENDER_TARGET_VOXELIZED_OBSTACLES]->SetResource( 0 );
	return hr;
}
int HairGrid::unsetBlurTexture()
{
    int hr = 0;
	pShaderResourceVariables[RENDER_TARGET_DENSITY_BLUR]->SetResource( 0 );
	return hr;
}



int HairGrid::demultiplexTextureTo3DFluidObstacles()
{
    int hr = 0;

    // All drawing will take place to a viewport with the dimensions of a 3D texture slice
    D3DFROMWINEVIEWPORT9 rtViewport;
    rtViewport.TopLeftX = 0;
    rtViewport.TopLeftY = 0;
    rtViewport.MinDepth = 0;
    rtViewport.MaxDepth = 1;
    rtViewport.Width =  m_fluidGridWidth;  
    rtViewport.Height = m_fluidGridHeight; 
    IRenderer::GetRendererInstance()->RSSetViewports(1,&rtViewport);

    float clearValue[4] = {1, 1, 1, 1 };
    IRenderer::GetRendererInstance()->ClearRenderTargetView( m_pRenderTargetViewFluidObstacles, clearValue );


    pShaderResourceVariables[RENDER_TARGET_DENSITY]->SetResource( pRenderTargetShaderViews[RENDER_TARGET_DENSITY] );
    (TechniqueDemuxTo3DFluidObstacles->GetPassByIndex(0)->Apply(0));
    IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &m_pRenderTargetViewFluidObstacles , 0 ); 
    g_fluid->drawGridSlices();

	
	IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);

	return hr;
}


int HairGrid::DisplayTexture(RENDER_TARGET rtIndex)
{
    int hr = 0;
    D3DFROMWINEVIEWPORT9 rtViewport;
    rtViewport.TopLeftX = 0;
    rtViewport.TopLeftY = 0;
    rtViewport.MinDepth = 0;
    rtViewport.MaxDepth = 1;
    rtViewport.Width = g_Width;
    rtViewport.Height = g_Height;
    IRenderer::GetRendererInstance()->RSSetViewports(1,&rtViewport);

    TextureID  pRTV = DXUTGetD3D11RenderTargetView();
    IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &pRTV , 0 ); 

    //bind texture as shader resource
    (pShaderResourceVariables[rtIndex]->SetResource( pRenderTargetShaderViews[rtIndex] ));

	//set the correct texture index for the shader
	m_textureIndexShaderVariable->SetInt(rtIndex);

	//set the correct technique
	if(rtIndex==RENDER_TARGET_DENSITY)
	{     (TechniqueDrawTexture->GetPassByIndex(0)->Apply(0));}
	else
	{	 (TechniqueDrawTextureDemuxed->GetPassByIndex(0)->Apply(0));}

	DrawSlicesToScreen();

    // Unset resources and apply technique (so that the resource is actually unbound)
    pShaderResourceVariables[rtIndex]->SetResource(0);
	if(rtIndex==RENDER_TARGET_DENSITY)
	{     (TechniqueDrawTexture->GetPassByIndex(0)->Apply(0));}
	else
	{	 (TechniqueDrawTextureDemuxed->GetPassByIndex(0)->Apply(0));}
 
    return hr; 
}

void HairGrid::DrawSlicesToScreen( void )
{
    unsigned int stride[1] = { sizeof(GRID_TEXTURE_DISPLAY_STRUCT) };
    unsigned int offset[1] = { 0 };

    IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_TRIANGLES );
    IRenderer::GetRendererInstance()->setVertexFormat(0,  quad_layout );
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1,  &renderQuadBuffer, stride, offset );
    IRenderer::GetRendererInstance()->Draw( m_numVerticesRenderQuad, 0 ); 
}

void HairGrid::InitScreenSlice(GRID_TEXTURE_DISPLAY_STRUCT** vertices, int z, int& index )
{
    GRID_TEXTURE_DISPLAY_STRUCT tempVertex1;
    GRID_TEXTURE_DISPLAY_STRUCT tempVertex2;
    GRID_TEXTURE_DISPLAY_STRUCT tempVertex3;
    GRID_TEXTURE_DISPLAY_STRUCT tempVertex4;

    // compute the offset (px, py) in the "flat 3D-texture" space for the slice with given 'z' coordinate
    int column      = z % m_cols;
    int row         = (int)floorf((float)(z/m_cols));
    int px = column * m_dim[0];
    int py = row    * m_dim[1];

    float w = float(m_dim[0]);
    float h = float(m_dim[1]);

    float Width  = float(m_cols * m_dim[0]);
    float Height = float(m_rows * m_dim[1]);

    float left   = -1.0f + (px*2.0f/Width);
    float right  = -1.0f + ((px+w)*2.0f/Width);
    float top    =  1.0f - ((py+h)*2.0f/Height);
    float bottom =  1.0f - py*2.0f/Height;

    tempVertex1.Pos   = D3DXFROMWINEVECTOR3( left   , bottom    , 0.0f      );
    tempVertex1.Tex   = D3DXFROMWINEVECTOR3( 0      , 0         , float(z)  );

    tempVertex2.Pos   = D3DXFROMWINEVECTOR3( right  , bottom    , 0.0f      );
    tempVertex2.Tex   = D3DXFROMWINEVECTOR3( w      ,  0        , float(z)  );

    tempVertex3.Pos   = D3DXFROMWINEVECTOR3( right  , top       , 0.0f      );
    tempVertex3.Tex   = D3DXFROMWINEVECTOR3( w      ,  h        , float(z)  );

    tempVertex4.Pos   = D3DXFROMWINEVECTOR3( left   , top       , 0.0f      );
    tempVertex4.Tex   = D3DXFROMWINEVECTOR3( 0      ,  h        , float(z)  );

    (*vertices)[index++] = tempVertex1;
    (*vertices)[index++] = tempVertex2;
    (*vertices)[index++] = tempVertex3;
    (*vertices)[index++] = tempVertex1;
    (*vertices)[index++] = tempVertex3;
    (*vertices)[index++] = tempVertex4;
}


void HairGrid::InitGridSlice(GRID_TEXTURE_DISPLAY_STRUCT** vertices )
{
    GRID_TEXTURE_DISPLAY_STRUCT tempVertex1;
    GRID_TEXTURE_DISPLAY_STRUCT tempVertex2;
    GRID_TEXTURE_DISPLAY_STRUCT tempVertex3;
    GRID_TEXTURE_DISPLAY_STRUCT tempVertex4;

    float w = float(m_dim[0]);
    float h = float(m_dim[1]);

    float left   = -1.0f;
    float right  =  1.0f;
    float top    = -1.0f;
    float bottom =  1.0f;

    tempVertex1.Pos   = D3DXFROMWINEVECTOR3( left   , bottom    , 0.0f      );
    tempVertex1.Tex   = D3DXFROMWINEVECTOR3( 0      , 0         , 0         );

    tempVertex2.Pos   = D3DXFROMWINEVECTOR3( right  , bottom    , 0.0f      );
    tempVertex2.Tex   = D3DXFROMWINEVECTOR3( w      ,  0        , 0         );

    tempVertex3.Pos   = D3DXFROMWINEVECTOR3( right  , top       , 0.0f      );
    tempVertex3.Tex   = D3DXFROMWINEVECTOR3( w      ,  h        , 0         );

    tempVertex4.Pos   = D3DXFROMWINEVECTOR3( left   , top       , 0.0f      );
    tempVertex4.Tex   = D3DXFROMWINEVECTOR3( 0      ,  h        , 0         );

	 int index = 0;
    (*vertices)[index++] = tempVertex1;
    (*vertices)[index++] = tempVertex2;
    (*vertices)[index++] = tempVertex3;
    (*vertices)[index++] = tempVertex1;
    (*vertices)[index++] = tempVertex3;
    (*vertices)[index++] = tempVertex4;
}


int HairGrid::CreateVertexBuffer( int ByteWidth, unsigned int bindFlags, VertexBufferID   vertexBuffer,
                                 GRID_TEXTURE_DISPLAY_STRUCT* vertices,int numVertices)
{
    int hr;

	vertexBuffer=IRenderer::GetRendererInstance()->addVertexBuffer(ByteWidth*numVertices, STATIC, (const void *)vertices);

    return 0;
}


int HairGrid::setFluidObstacleTexture( TextureID  texture, FORMAT format, int texWidth, int texHeight, int texDepth )
{
    int hr = 0;

	//////////////////SAFE_RELEASE(m_pFluidObstacleTexture3D);
	m_pFluidObstacleTexture3D=0;

	//acquire the texture
    SAFE_ACQUIRE(m_pFluidObstacleTexture3D, texture);
    
	//create the render target view
    SURFACE_DESC DescRT;
    ZeroMemory( &DescRT, sizeof(DescRT) );
    DescRT.Format = format;
    DescRT.ViewDimension =  RTV_DIMENSION_TEXTURE3D;
    DescRT.Texture3D.FirstWSlice = 0;
    DescRT.Texture3D.MipSlice = 0;
    DescRT.Texture3D.WSize = texDepth;
    ( IRenderer::GetRendererInstance()->CreateRenderTargetView( m_pFluidObstacleTexture3D, &DescRT, &m_pRenderTargetViewFluidObstacles) );

    m_fluidGridWidth = texWidth;
    m_fluidGridHeight = texHeight;
	m_fluidGridDepth = texDepth;

	m_pEffect->GetVariableByName("fluidTextureWidth")->AsScalar()->SetInt(texWidth);
	m_pEffect->GetVariableByName("fluidTextureHeight")->AsScalar()->SetInt(texHeight);
	m_pEffect->GetVariableByName("fluidTextureDepth")->AsScalar()->SetInt(texDepth);

    return hr;
}
