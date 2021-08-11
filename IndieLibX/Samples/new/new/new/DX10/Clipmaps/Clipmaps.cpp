//----------------------------------------------------------------------------------
// File:   Clipmaps.cpp
// Author: Evgeny Makarov
// Email:  sdkfeedback@nvidia.com
// 
// Copyright (c) 2007 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA OR ITS SUPPLIERS
// BE  LIABLE  FOR  ANY  SPECIAL,  INCIDENTAL,  INDIRECT,  OR  CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//
//----------------------------------------------------------------------------------
/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include "Clipmaps.h"
#include "JPEG_Preprocessor.h"
#include <math.h>
#include <vector>
#include <string>

#define SOURCE_FILES_NUM 5

std::string g_SrcMediaPath[SOURCE_FILES_NUM] = {  "Clipmaps//Mars16k.jpg",
                                                      "Clipmaps//Mars8k.jpg",
                                                      "Clipmaps//Mars4k.jpg",
                                                      "Clipmaps//Mars2k.jpg",
                                                      "Clipmaps//Mars1k.jpg" };
                                                      
std::string g_SrcMediaPathHM[SOURCE_FILES_NUM] = {  "Clipmaps//MarsHm16k.jpg",
                                                        "Clipmaps//MarsHm8k.jpg",
                                                        "Clipmaps//MarsHm4k.jpg",
                                                        "Clipmaps//MarsHm2k.jpg",
                                                        "Clipmaps//MarsHm1k.jpg" };
                                                      
std::string g_DstMediaPath[SOURCE_FILES_NUM];
std::string g_DstMediaPathHM[SOURCE_FILES_NUM];

#define SPHERE_MERIDIAN_SLICES_NUM 128
#define SPHERE_PARALLEL_SLICES_NUM 128

#define FILE_BLOCK_SIZE 512

#define CLIPMAP_STACK_SIZE_MAX 4096
#define CLIPMAP_STACK_SIZE_MIN 1024
#define MIPMAP_LEVELS_MAX 7

enum eRenderingTechniques
{
    RENDER_TRILINEAR,
    RENDER_ANISOTROPIC,
    RENDER_COLOR_MIPS,
    TECHNIQUES_NUM,
};

const char* RENDER_TECHNIQUES[] =
{
    "Trilinear",
    "Anisotropic",
    "ColoredMips",
};

const char* RENDER_TECHNIQUESW[] =
{
    "Trilinear",
    "Anisotropic",
    "Colored mips",
};

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
int g_ClipmapStackSize = 4096;
int g_UpdateRegionSize = 64;
int g_RenderingTechnique = 1;

stx_Effect*          g_pEffect = 0;
VertexFormatID     g_pVertexLayout = -1;
VertexBufferID          g_pSphereVertexBuffer = -1;
IndexBufferID          g_pSphereIndexBuffer = -1;
stx_CModelViewerCamera    g_RotateCamera;

bool g_showHelp = false;

int        g_SourceImageWidth;
int        g_SourceImageHeight;
int        g_SourceImageMipsNum;
int**      g_ppSourceImageMipsSize = 0;
int*       g_pMipDataOffset = 0;     // Offset in bytes for each mip level data stored in a temporary file
D3DXFROMWINEVECTOR2 g_StackPosition;            // Virtual clipmap stack position (defined using normalized texture coordinates)
int        g_StackDepth;                // Number of layers in a clipmap stack

std::vector<int>    g_StackSizeList;
std::vector<int>    g_UpdateRegionSizeList;

TextureID              g_pPyramidTexture      = -1; // Texture which is used to store coarse mip levels
TextureID              g_pPyramidTextureHM    = -1; // Height map for pyramid texture
TextureID              g_pStackTexture        = -1; // Clipmap stack texture

D3DXFROMWINEVECTOR3 g_LightPosition;
D3DXFROMWINEVECTOR3 g_EyePosition;
D3DXFROMWINEMATRIX  g_MProjection;
D3DXFROMWINEMATRIX  g_MView;
D3DXFROMWINEMATRIX  g_MWorldViewProjection;

int **g_ppUpdatePositions;                           // Defines positions for each clipmap layer where new data should be placed

bool g_bSampleInitialized    = false;
bool g_bUseParallax          = true;
bool g_bShowStackLevels      = false;

float g_MipmapColors[MIPMAP_LEVELS_MAX][3] =
{
    {0.5f, 1.0f, 0.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 0.0f},
};

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_STATIC             -1
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3
#define IDC_STACK_SIZE          5
#define IDC_UPDATE_SIZE         6
#define IDC_START_SAMPLE        7
#define IDC_RENDER_TECHNIQUES   8
#define IDC_RENDER_PARALLAX     9
#define IDC_SHOW_STACK_LEVELS   10


class App: public STXGUI
{
public:
    App(){}
    virtual ~App(){}
    int init(const char* aTitle){STXGUI::init("/GUILayout.xml");}
    void render( ){STXGUI::update();}
#if 1
//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
int OnCreateDevice()
{
    int hr;
    
    g_bSampleInitialized = false;

    ( LoadEffect( "Clipmaps.fx", &g_pEffect ) );
    
    for( int i = 0; i < SOURCE_FILES_NUM; i++ )
    {
       	g_DstMediaPath[i]=g_SrcMediaPath[i];
        g_DstMediaPathHM[i]=g_SrcMediaPathHM[i];
    }
    
    {Image3 img;
    img.loadJPEG( g_SrcMediaPath[0].c_str() );
    
    g_SourceImageWidth = img.getWidth();
    g_SourceImageHeight = img.getHeight();}
    
    unsigned int baseDimension = CLIPMAP_STACK_SIZE_MAX;

    while( baseDimension >= CLIPMAP_STACK_SIZE_MIN )
    {
        g_StackSizeList.push_back(baseDimension);
        baseDimension >>= 1;
    }
    
    ( CreateIndexBuffer( &g_pSphereIndexBuffer ) );
    
    D3DXFROMWINEVECTOR3 lookAt( 0.0f, 0.0f, 0.0f );
    g_EyePosition = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 3.0f);

    g_RotateCamera.SetViewParams( &g_EyePosition, &lookAt );
    //g_RotateCamera.SetModelCenter( D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f));
    
    return 0;
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
int OnResizedSwapChain()
{
    g_RotateCamera.SetProjParams( D3DXFROMWINE_PI * 0.2f, IRenderer::GetRendererInstance()->GetAspectRatio(), 0.01f, 1000.0f );
    
    g_pEffect->GetVariableByName( "g_ScreenAspectRatio" )-> /* AsScalar()-> */ SetFloat( IRenderer::GetRendererInstance()->GetAspectRatio() );

    return 0;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void OnFrameMove( double fTime, float fElapsedTime)
{
    static float startTime = 0.0f;
    static bool renderedScene = false;
    
    if( g_bSampleInitialized == true && renderedScene == false )
        startTime = (float)fTime;

    renderedScene = g_bSampleInitialized;
    g_RotateCamera.FrameMove(fElapsedTime);
    
    g_LightPosition.x = cosf(((float)fTime - startTime) * 0.05f) * 10.0f;
    g_LightPosition.z = sinf(((float)fTime - startTime) * 0.05f) * 10.0f;
    
    g_pEffect->GetVariableByName( "g_LightPosition" )-> /* AsVector()-> */ SetFloatVectorArray( (float*)&g_LightPosition, 0 , 3 );
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------
void OnFrameRender(double fTime, float fElapsedTime )
{
    static float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    // Clear render target and the depth stencil
    IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));

    if( g_bSampleInitialized )
    {
        g_MProjection = g_RotateCamera.GetProjMatrix();

        float length = 0.0f;

        g_EyePosition = g_RotateCamera.GetEyePt3();

        length = D3DXFROMWINEVec3Length(&g_EyePosition);
        if( length < 1.1f )
        {
            g_EyePosition *= 1.1f / length;

            D3DXFROMWINEVECTOR3 lookAt(0.0f, 0.0f, 0.0f);
            g_RotateCamera.SetViewParams( &g_EyePosition, &lookAt );
        }
            
        g_MView = g_RotateCamera.GetViewMatrix();
        
        g_MWorldViewProjection = g_MView * g_MProjection;

        g_pEffect->GetVariableByName( "g_ModelViewProj" )-> /* AsMatrix()-> */ SetMatrix( g_MWorldViewProjection );
        g_pEffect->GetVariableByName( "g_EyePosition" )-> /* AsVector()-> */ SetFloatVectorArray( (float*)&g_EyePosition, 0 , 3 );
        
        float vectorRight[3] = {g_RotateCamera.GetViewMatrix()._11, g_RotateCamera.GetViewMatrix()._21, g_RotateCamera.GetViewMatrix()._31 };
        float vectorUp[3] = {g_RotateCamera.GetViewMatrix()._12, g_RotateCamera.GetViewMatrix()._22, g_RotateCamera.GetViewMatrix()._32 };
            
        g_pEffect->GetVariableByName( "g_WorldRight" )-> /* AsVector()-> */ SetFloatVectorArray( (float*)vectorRight, 0, 3 );
        g_pEffect->GetVariableByName( "g_WorldUp" )-> /* AsVector()-> */ SetFloatVectorArray( (float*)vectorUp, 0, 3 );
        
        UpdateStackTexture();

        g_pEffect->GetVariableByName( "g_StackCenter" )-> /* AsVector()-> */ SetFloatVectorArray( (float*)g_StackPosition, 0, 2 );
        
        IRenderer::GetRendererInstance()->setVertexFormat(-1);
        IRenderer::GetRendererInstance()->setVertexBuffer(0, -1);
        IRenderer::GetRendererInstance()->setIndexBuffer( g_pSphereIndexBuffer);
        //IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_TRIANGLES );
        
        if( g_bUseParallax && g_RenderingTechnique < 2 )
            g_pEffect->GetTechniqueByName( RENDER_TECHNIQUES[g_RenderingTechnique] )->GetPassByIndex( 1 )->Apply( 0 );
        else
            g_pEffect->GetTechniqueByName( RENDER_TECHNIQUES[g_RenderingTechnique] )->GetPassByIndex( 0 )->Apply( 0 );
        
        IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, SPHERE_MERIDIAN_SLICES_NUM * SPHERE_PARALLEL_SLICES_NUM * 4, SPHERE_MERIDIAN_SLICES_NUM * SPHERE_PARALLEL_SLICES_NUM * 2, g_pSphereIndexBuffer, g_pSphereIndexBuffer, CONSTANT_INDEX2, g_pSphereVertexBuffer, g_pSphereVertexBuffer, sizeof(D3DXFROMWINEVECTOR3));

        if( g_bShowStackLevels )
        {
            //g_pEffect->GetTechniqueByName( "StackDrawPass" )->GetPassByIndex( 0 )->Apply( 0 );
            //IRenderer::GetRendererInstance()->Draw( g_StackDepth, 0 );
	    IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_POINTS, SPHERE_MERIDIAN_SLICES_NUM * SPHERE_PARALLEL_SLICES_NUM * 4, g_pSphereVertexBuffer, g_pSphereVertexBuffer, sizeof(D3DXFROMWINEVECTOR3));
        }
    }
}

//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnCreateDevice 
//--------------------------------------------------------------------------------------
void OnDestroyDevice( )
{
    //SAFE_RELEASE( g_pFont );
    //SAFE_RELEASE( g_pSprite );
    //SAFE_RELEASE( g_pEffect );
    //SAFE_RELEASE( g_pPyramidTexture );
    //SAFE_RELEASE( g_pPyramidTextureHM );
    //SAFE_RELEASE( g_pStackTexture );
    //SAFE_RELEASE( g_pVertexLayout );
    //SAFE_RELEASE( g_pSphereIndexBuffer );

    g_JPEG_Manager.Release();
    
    if( g_ppSourceImageMipsSize )
    {
        for( int i = 0; i < g_SourceImageMipsNum; i++ )
        {
            delete [] g_ppSourceImageMipsSize[i];
        }
        
        delete [] g_ppSourceImageMipsSize;
        g_ppSourceImageMipsSize = 0;
    }
    
    if( g_ppUpdatePositions )
    {
        for( int i = 0; i < g_StackDepth; i++ )
        {
            delete [] g_ppUpdatePositions[i];
        }
        
        delete [] g_ppUpdatePositions;
        g_ppUpdatePositions = 0;
    }
    
    if( g_pMipDataOffset )
    {
        delete [] g_pMipDataOffset;
        g_pMipDataOffset = 0;     
    }
}

int CreateClipmapTextures()
{
    int hr;
    
    g_pPyramidTexture=IRenderer::GetRendererInstance()->addImageLibTexture(g_DstMediaPath[g_StackDepth].c_str(), false, IRenderer::GetRendererInstance()->Getlinear());

    g_pPyramidTextureHM=IRenderer::GetRendererInstance()->addImageLibTexture(g_DstMediaPathHM[g_StackDepth].c_str(), false, IRenderer::GetRendererInstance()->Getlinear());

    g_pEffect->GetVariableByName( "PyramidTexture" )->SetTexture( g_pPyramidTexture );
    g_pEffect->GetVariableByName( "PyramidTextureHM" )->SetTexture( g_pPyramidTextureHM );
    g_pEffect->GetVariableByName( "StackTexture" )->SetTexture( g_pStackTexture );

    return 0;
}


//--------------------------------------------------------------------------------------
// Create an index buffer to be used for procedural sphere mesh rendering
//--------------------------------------------------------------------------------------
int CreateIndexBuffer(IndexBufferID  ppIndexBuffer )
{
    int hr;
    int IndexNumber = SPHERE_MERIDIAN_SLICES_NUM * SPHERE_PARALLEL_SLICES_NUM * 6;

    unsigned *pIndices = new unsigned[IndexNumber];
    unsigned indexCount = 0;

    for( int i = 0; i < SPHERE_PARALLEL_SLICES_NUM; i++ )
    {
        for( int j = 0; j < SPHERE_MERIDIAN_SLICES_NUM; j++ )
        {
            pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
            indexCount++;

            pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
            indexCount++;
            
            pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
            indexCount++;
            
            pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
            indexCount++;
            
            pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
            indexCount++;
            
            pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
            indexCount++;
        }
    }

    g_pSphereIndexBuffer=IRenderer::GetRendererInstance()->addIndexBuffer(indexCount, 2, STATIC, (const void *)pIndices);

    if( pIndices )
    {
        delete [] pIndices;
        pIndices = 0;
    }
    
    return 0;
}


void InitStackTexture()
{
    int mipCornerLU[2];
    int mipCornerRD[2];
    int tileBlockSize = 0;
    int blockIndex[2];
    int subBlockIndex[2];
    int blockCorner[2];
    
    D3DFROMWINE_BOX SubResourceBox;
    SubResourceBox.front = 0;
    SubResourceBox.back = 1;
    
    int srcBlock[2];
    int dstBlock[2];
    
    for( int i = 0; i < g_StackDepth; ++i )
    {
        mipCornerLU[0] = int(g_StackPosition.x * g_ppSourceImageMipsSize[i][0] - g_ClipmapStackSize* 0.5f);
        mipCornerLU[1] = int(g_StackPosition.y * g_ppSourceImageMipsSize[i][1] - g_ClipmapStackSize* 0.5f);

        mipCornerRD[0] = int(g_StackPosition.x * g_ppSourceImageMipsSize[i][0] + g_ClipmapStackSize* 0.5f);    
        mipCornerRD[1] = int(g_StackPosition.y * g_ppSourceImageMipsSize[i][1] + g_ClipmapStackSize* 0.5f);

        tileBlockSize = int(g_UpdateRegionSize / pow(2.0, i));
        
        SubResourceBox.top = 0;
        SubResourceBox.bottom = SubResourceBox.top + tileBlockSize;

        if( g_ppSourceImageMipsSize[i][1] == g_ClipmapStackSize )
        {
            g_ppUpdatePositions[i][1] = g_ClipmapStackSize / 2;
            
            for( int j = mipCornerLU[1] + g_ClipmapStackSize / 2; j < mipCornerRD[1]; j += tileBlockSize )
            {
                SubResourceBox.left = 0;
                SubResourceBox.right = SubResourceBox.left + tileBlockSize;

                blockIndex[1] = j / FILE_BLOCK_SIZE;
                blockCorner[1] = blockIndex[1] * FILE_BLOCK_SIZE;
                subBlockIndex[1] = ( j - blockCorner[1] ) / tileBlockSize;
            
                for( int k = mipCornerLU[0]; k < mipCornerRD[0]; k += tileBlockSize )
                {
                    int tempBoundary = k;

                    if( tempBoundary < 0 )
                        tempBoundary += g_ppSourceImageMipsSize[i][0];
                
                    blockIndex[0] = tempBoundary / FILE_BLOCK_SIZE;
                                
                    blockCorner[0] = blockIndex[0] * FILE_BLOCK_SIZE;
                    subBlockIndex[0] = ( tempBoundary - blockCorner[0] ) / tileBlockSize; 

                    srcBlock[0] = tempBoundary;
                    srcBlock[1] = j;
                
                    dstBlock[0] = SubResourceBox.left;
                    dstBlock[1] = SubResourceBox.top;
                
                    g_JPEG_Manager.AddBlock( i, srcBlock, dstBlock );
                    
                    SubResourceBox.left += tileBlockSize;
                    SubResourceBox.right += tileBlockSize;
                }
                
                g_JPEG_Manager.Update( i );

                SubResourceBox.top += tileBlockSize;
                SubResourceBox.bottom += tileBlockSize;
            }

            SubResourceBox.top = g_ClipmapStackSize/ 2;
            SubResourceBox.bottom = SubResourceBox.top + tileBlockSize;

            for( int j = mipCornerLU[1]; j < mipCornerRD[1] - g_ClipmapStackSize* 0.5; j += tileBlockSize )
            {
                SubResourceBox.left = 0;
                SubResourceBox.right = SubResourceBox.left + tileBlockSize;

                blockIndex[1] = j / FILE_BLOCK_SIZE;
                blockCorner[1] = blockIndex[1] * FILE_BLOCK_SIZE;
                subBlockIndex[1] = ( j - blockCorner[1] ) / tileBlockSize;
            
                for( int k = mipCornerLU[0]; k < mipCornerRD[0]; k += tileBlockSize )
                {
                    int tempBoundary = k;

                    if( tempBoundary < 0 )
                        tempBoundary += g_ppSourceImageMipsSize[i][0];
                
                    blockIndex[0] = tempBoundary / FILE_BLOCK_SIZE;

                    blockCorner[0] = blockIndex[0] * FILE_BLOCK_SIZE;
                    subBlockIndex[0] = ( tempBoundary - blockCorner[0] ) / tileBlockSize; 

                    srcBlock[0] = tempBoundary;
                    srcBlock[1] = j;
                
                    dstBlock[0] = SubResourceBox.left;
                    dstBlock[1] = SubResourceBox.top;
                
                    g_JPEG_Manager.AddBlock( i, srcBlock, dstBlock );
                    
                    SubResourceBox.left += tileBlockSize;
                    SubResourceBox.right += tileBlockSize;
                }
                
                g_JPEG_Manager.Update( i );

                SubResourceBox.top += tileBlockSize;
                SubResourceBox.bottom += tileBlockSize;
            }
        }
        else
        {
            for( int j = mipCornerLU[1]; j < mipCornerRD[1]; j += tileBlockSize )
            {
                SubResourceBox.left = 0;
                SubResourceBox.right = SubResourceBox.left + tileBlockSize;

                blockIndex[1] = j / FILE_BLOCK_SIZE;
                blockCorner[1] = blockIndex[1] * FILE_BLOCK_SIZE;
                subBlockIndex[1] = ( j - blockCorner[1] ) / tileBlockSize;
            
                for( int k = mipCornerLU[0]; k < mipCornerRD[0]; k += tileBlockSize )
                {
                    int tempBoundary = k;

                    if( tempBoundary < 0 )
                        tempBoundary += g_ppSourceImageMipsSize[i][0];
                
                    blockIndex[0] = tempBoundary / FILE_BLOCK_SIZE;

                    blockCorner[0] = blockIndex[0] * FILE_BLOCK_SIZE;
                    subBlockIndex[0] = ( tempBoundary - blockCorner[0] ) / tileBlockSize; 

                    srcBlock[0] = tempBoundary;
                    srcBlock[1] = j;
                
                    dstBlock[0] = SubResourceBox.left;
                    dstBlock[1] = SubResourceBox.top;
                
                    g_JPEG_Manager.AddBlock( i, srcBlock, dstBlock );
                   
                    SubResourceBox.left += tileBlockSize;
                    SubResourceBox.right += tileBlockSize;
                }
                
                g_JPEG_Manager.Update( i );

                SubResourceBox.top += tileBlockSize;
                SubResourceBox.bottom += tileBlockSize;
            }
        }
    }
}
#endif

//--------------------------------------------------------------------------------------
// Calculate base parameters using defined sizes
//--------------------------------------------------------------------------------------
void CalculateClipmapParameters()
{
    g_StackDepth = 0;
    int dimensionMax = g_SourceImageWidth;
    
    if( g_SourceImageHeight > g_SourceImageWidth )
        dimensionMax = g_SourceImageHeight;
    
    g_SourceImageMipsNum = int( log( (double)dimensionMax + 1.0 ) / log( 2.0 ) ) + 1;
    
    for( int i = 0; i < dimensionMax; ++i )
    {
        if( dimensionMax / pow(2.0f, g_StackDepth) > g_ClipmapStackSize)
        {
            g_StackDepth++;
        }
        else
        {
            break;
        }
    }

    g_ppSourceImageMipsSize = new int*[g_SourceImageMipsNum];

    for( int i = 0; i < g_SourceImageMipsNum; ++i )
    {
        g_ppSourceImageMipsSize[i] = new int[2];
        
        g_ppSourceImageMipsSize[i][0] = g_SourceImageWidth / (unsigned)pow( 2.0f, i );
        g_ppSourceImageMipsSize[i][1] = g_SourceImageHeight / (unsigned)pow( 2.0f, i );
    }
    
    g_StackPosition.x = 0.0f;
    g_StackPosition.y = 0.5f;

    assert(g_StackDepth);
    
    g_pMipDataOffset = new int[g_StackDepth];
    g_ppUpdatePositions = new int*[g_StackDepth];

    for( int i = 0; i < g_StackDepth; ++i )
    {
        g_ppUpdatePositions[i] = new int[2];
        
        g_ppUpdatePositions[i][0] = 0;
        g_ppUpdatePositions[i][1] = 0;
    }
    
    g_pEffect->GetVariableByName( "g_StackDepth" )-> /* AsScalar()-> */ SetInt( g_StackDepth );
    
    D3DXFROMWINEVECTOR2 scaleFactor;
    scaleFactor.x = (float)g_SourceImageWidth / g_ClipmapStackSize;
    scaleFactor.y = (float)g_SourceImageHeight / g_ClipmapStackSize;
    g_pEffect->GetVariableByName( "g_ScaleFactor" )-> /* AsVector()-> */ SetFloatVectorArray( scaleFactor, 0, 2 );
    
    int textureSize[2];
    textureSize[0] = g_SourceImageWidth;
    textureSize[1] = g_SourceImageHeight;

    g_pEffect->GetVariableByName( "g_TextureSize" )-> /* AsVector()-> */ SetIntVectorArray( (int*)textureSize, 0, 2 );
    g_pEffect->GetVariableByName( "g_MipColors" )-> /* AsVector()-> */ SetFloatVectorArray( (float*)g_MipmapColors, 0, MIPMAP_LEVELS_MAX * 3 );

    g_pEffect->GetVariableByName( "g_SphereMeridianSlices" )-> /* AsScalar()-> */ SetInt( SPHERE_MERIDIAN_SLICES_NUM );
    g_pEffect->GetVariableByName( "g_SphereParallelSlices" )-> /* AsScalar()-> */ SetInt( SPHERE_PARALLEL_SLICES_NUM );
}


//--------------------------------------------------------------------------------------
// Perform stack texture updates based on a new viewer's position
//--------------------------------------------------------------------------------------
void UpdateStackTexture()
{
    D3DXFROMWINEVECTOR2 updateBorder;
    float length = sqrtf( g_EyePosition.x * g_EyePosition.x + g_EyePosition.z * g_EyePosition.z );
    float posHorizontal;
    float posVertical;

    // Positions are calculated in order to map current viewer's position
    // to a rectangular region of a source image mapped on a spherical mesh.
    // Positions are represented by values clamped to [0 - 1] range in each dimension
    if( g_EyePosition.z >= 0 )
    {
        if( g_EyePosition.x <= 0 )
            posHorizontal = atanf( -( g_EyePosition.x / g_EyePosition.z ) ) / (D3DXFROMWINE_PI * 2);
        else
            posHorizontal = 1.0f - atanf( g_EyePosition.x / g_EyePosition.z ) / (D3DXFROMWINE_PI * 2);
    }
    else
    {
        posHorizontal = 0.5f - atanf((g_EyePosition.x / g_EyePosition.z)) / (D3DXFROMWINE_PI * 2);
    }
        
    posVertical = 0.5f - atanf(g_EyePosition.y / length) / D3DXFROMWINE_PI;
    
    int srcBlock[2];
    int dstBlock[2];
    
    // Calculate border sizes to be updated
    updateBorder.x = posHorizontal - g_StackPosition.x;
    
    if( updateBorder.x > 0 )
    {
        if( updateBorder.x > 0.5f )
            updateBorder.x = updateBorder.x - 1.0f;
    }
    else
    {
        if( updateBorder.x < -0.5f )
            updateBorder.x += 1.0f; 
    }

    updateBorder.y = posVertical - g_StackPosition.y;

    int updateBorderSize[2];
    updateBorderSize[0] = int( updateBorder.x * g_SourceImageWidth );
    updateBorderSize[1] = int( updateBorder.y * g_SourceImageHeight );

    int tileBlockSize = 0;
    int mipCornerLU[2];
    int mipCornerRD[2];

    D3DFROMWINE_BOX SubResourceBox;
    SubResourceBox.front = 0;
    SubResourceBox.back = 1;

    unsigned correction = 0;

    // Update square region when we move "Image space Right"
    if( updateBorderSize[0] > g_UpdateRegionSize )
    {
        // Update all clipmap stack layers one by one
        for( int i = 0; i < g_StackDepth; ++i )
        {            
            tileBlockSize = g_UpdateRegionSize / (unsigned)pow(2.0, i);

            // Calculate regions of source image that should be updated in the current stack layer
            mipCornerLU[0] = int(g_StackPosition.x * g_ppSourceImageMipsSize[i][0]) + g_ClipmapStackSize / 2;
            if( mipCornerLU[0] > int(g_ppSourceImageMipsSize[i][0] - tileBlockSize) )
                mipCornerLU[0] -= g_ppSourceImageMipsSize[i][0];
            mipCornerRD[0] = mipCornerLU[0] + tileBlockSize;
            
            mipCornerLU[1] = int(g_StackPosition.y * g_ppSourceImageMipsSize[i][1]) - g_ClipmapStackSize / 2;
            mipCornerRD[1] = int(g_StackPosition.y * g_ppSourceImageMipsSize[i][1]) + g_ClipmapStackSize / 2;

            if( mipCornerLU[1] < 0 )
            {
                correction = -mipCornerLU[1];
                mipCornerLU[1] = 0;
            }
            else if( mipCornerLU[1] >= g_ppSourceImageMipsSize[i][1] )
            {
                UpdateMipPosition(g_ppUpdatePositions[i][0], tileBlockSize);
                continue;
            }

            if( mipCornerRD[1] <= 0 )
            {
                UpdateMipPosition(g_ppUpdatePositions[i][0], tileBlockSize);
                continue;
            }
            else if( mipCornerRD[1] > g_ppSourceImageMipsSize[i][1] )
                mipCornerRD[1] = g_ppSourceImageMipsSize[i][1];

            SubResourceBox.left = g_ppUpdatePositions[i][0];
            SubResourceBox.right = SubResourceBox.left + tileBlockSize;
            SubResourceBox.top = g_ppUpdatePositions[i][1] + correction;

            for( int j = mipCornerLU[1]; j < mipCornerRD[1]; j += tileBlockSize )
            {
                SubResourceBox.bottom = SubResourceBox.top + tileBlockSize;

                srcBlock[0] = mipCornerLU[0];
                srcBlock[1] = j;
                
                dstBlock[0] = SubResourceBox.left;
                dstBlock[1] = SubResourceBox.top;
                
                g_JPEG_Manager.AddBlock( i, srcBlock, dstBlock );

                SubResourceBox.top += tileBlockSize;

                if( SubResourceBox.top > UINT(g_ClipmapStackSize - tileBlockSize) )
                    SubResourceBox.top = 0;
            }
        
            g_JPEG_Manager.Update( i );
            UpdateMipPosition( g_ppUpdatePositions[i][0], tileBlockSize );
        }

        g_StackPosition.x += (float)g_UpdateRegionSize / g_SourceImageWidth;
    }

    // Update square region when we move "Image space Left"
    if( updateBorderSize[0] < -g_UpdateRegionSize )
    {
        for( int i = 0; i < g_StackDepth; ++i )
        {
            tileBlockSize = g_UpdateRegionSize / (unsigned)pow(2.0, i);

            UpdateMipPosition(g_ppUpdatePositions[i][0], -tileBlockSize);
            
            mipCornerLU[0] = int(g_StackPosition.x * g_ppSourceImageMipsSize[i][0]) - g_ClipmapStackSize / 2 - tileBlockSize;
            if( mipCornerLU[0] < 0 )
                mipCornerLU[0] += g_ppSourceImageMipsSize[i][0];
            mipCornerRD[0] = mipCornerLU[0] + tileBlockSize;
            
            mipCornerLU[1] = int(g_StackPosition.y * g_ppSourceImageMipsSize[i][1]) - g_ClipmapStackSize / 2;
            mipCornerRD[1] = int(g_StackPosition.y * g_ppSourceImageMipsSize[i][1]) + g_ClipmapStackSize / 2;

            if( mipCornerLU[1] < 0 )
            {
                correction = -mipCornerLU[1];
                mipCornerLU[1] = 0;
            }
            else if( mipCornerLU[1] >= g_ppSourceImageMipsSize[i][1] )
                continue;

            if( mipCornerRD[1] <= 0 )
                continue;
            else if( mipCornerRD[1] > g_ppSourceImageMipsSize[i][1] )
                mipCornerRD[1] = g_ppSourceImageMipsSize[i][1];

            SubResourceBox.left = g_ppUpdatePositions[i][0];
            SubResourceBox.right = SubResourceBox.left + tileBlockSize;
            SubResourceBox.top = g_ppUpdatePositions[i][1] + correction;

            for( int j = mipCornerLU[1]; j < mipCornerRD[1]; j += tileBlockSize )
            {
                SubResourceBox.bottom = SubResourceBox.top + tileBlockSize;

                srcBlock[0] = mipCornerLU[0];
                srcBlock[1] = j;
                
                dstBlock[0] = SubResourceBox.left;
                dstBlock[1] = SubResourceBox.top;
                
                g_JPEG_Manager.AddBlock( i, srcBlock, dstBlock );

                SubResourceBox.top += tileBlockSize;

                if( SubResourceBox.top > UINT(g_ClipmapStackSize - tileBlockSize) )
                    SubResourceBox.top = 0;
            }
            
            g_JPEG_Manager.Update( i );
        }
        
        g_StackPosition.x -= (float)g_UpdateRegionSize / g_SourceImageWidth;
    }

    if( g_StackPosition.x < 0.0f )
        g_StackPosition.x = 1.0f + g_StackPosition.x;
    else if( g_StackPosition.x > 1.0f )
        g_StackPosition.x -= 1.0f;
    
    // Update square region when we move "Image space Down"
    if( updateBorderSize[1] > g_UpdateRegionSize )
    {
        for( int i = 0; i < g_StackDepth; ++i )
        {
            correction = 0;
            
            tileBlockSize = g_UpdateRegionSize / (unsigned)pow(2.0, i);

            mipCornerLU[0] = int(g_StackPosition.x * g_ppSourceImageMipsSize[i][0]) - g_ClipmapStackSize / 2;
            if( mipCornerLU[0] < 0 )
                mipCornerLU[0] += g_ppSourceImageMipsSize[i][0];
                            
            int stepsNum = g_ClipmapStackSize / tileBlockSize;

            mipCornerLU[1] = int(g_StackPosition.y * g_ppSourceImageMipsSize[i][1]) + g_ClipmapStackSize / 2;
            mipCornerRD[1] = mipCornerLU[1] + tileBlockSize;

            if( mipCornerLU[1] < 0 )
                mipCornerLU[1] = 0;
            else if( mipCornerLU[1] >= g_ppSourceImageMipsSize[i][1] )
            {
                UpdateMipPosition(g_ppUpdatePositions[i][1], tileBlockSize);
                continue;
            }

            if( mipCornerRD[1] <= 0 )
                continue;
            else if( mipCornerRD[1] > g_ppSourceImageMipsSize[i][1] )
                mipCornerRD[1] = g_ppSourceImageMipsSize[i][1];

            SubResourceBox.top = g_ppUpdatePositions[i][1];
            SubResourceBox.bottom = SubResourceBox.top + tileBlockSize;
            SubResourceBox.left = g_ppUpdatePositions[i][0];

            while( SubResourceBox.left > UINT(g_ClipmapStackSize - tileBlockSize) )
                SubResourceBox.left -= g_ClipmapStackSize;

            for( int step = 0, j = mipCornerLU[0]; step < stepsNum; step++, j += tileBlockSize )
            {
                if( j == g_ppSourceImageMipsSize[i][0] )
                    j = 0;

                SubResourceBox.right = SubResourceBox.left + tileBlockSize;
                
                srcBlock[0] = j;
                srcBlock[1] = mipCornerLU[1];
                
                dstBlock[0] = SubResourceBox.left;
                dstBlock[1] = SubResourceBox.top;
                
                g_JPEG_Manager.AddBlock( i, srcBlock, dstBlock );

                SubResourceBox.left += tileBlockSize;

                if( SubResourceBox.left > UINT( g_ClipmapStackSize - tileBlockSize ) )
                    SubResourceBox.left = 0;
            }

            g_JPEG_Manager.Update( i );
            UpdateMipPosition( g_ppUpdatePositions[i][1], tileBlockSize );
        }

        g_StackPosition.y += (float)g_UpdateRegionSize / g_SourceImageHeight;
    }
    
    // Update square region when we move "Image space Up"
    if( updateBorderSize[1] < -g_UpdateRegionSize )
    {
        for( int i = 0; i < g_StackDepth; ++i )
        {
            correction = 0;
            
            tileBlockSize = g_UpdateRegionSize / (unsigned)pow(2.0, i);

            mipCornerLU[0] = int(g_StackPosition.x * g_ppSourceImageMipsSize[i][0]) - g_ClipmapStackSize / 2;
            if( mipCornerLU[0] < 0 )
                mipCornerLU[0] += g_ppSourceImageMipsSize[i][0];
            
            int stepsNum = g_ClipmapStackSize / tileBlockSize;
            
            UpdateMipPosition(g_ppUpdatePositions[i][1], -tileBlockSize);
                        
            mipCornerLU[1] = int(g_StackPosition.y * g_ppSourceImageMipsSize[i][1]) - g_ClipmapStackSize / 2 - tileBlockSize;
            if( mipCornerLU[1] < 0 )
            {
                mipCornerLU[1] += g_ppSourceImageMipsSize[i][1];
            }

            mipCornerRD[1] = mipCornerLU[1] + tileBlockSize;

            if( mipCornerRD[1] <= 0 )
                continue;
            else if( mipCornerRD[1] > g_ppSourceImageMipsSize[i][1] )
                mipCornerRD[1] = g_ppSourceImageMipsSize[i][1];

            SubResourceBox.top = g_ppUpdatePositions[i][1];
            SubResourceBox.bottom = SubResourceBox.top + tileBlockSize;
            SubResourceBox.left = g_ppUpdatePositions[i][0];

            while( SubResourceBox.left > UINT(g_ClipmapStackSize - tileBlockSize) )
                SubResourceBox.left -= g_ClipmapStackSize;

            for( int step = 0, j = mipCornerLU[0]; step < stepsNum; step++, j += tileBlockSize )
            {
                if( j == g_ppSourceImageMipsSize[i][0] )
                    j = 0;
            
                SubResourceBox.right = SubResourceBox.left + tileBlockSize;

                srcBlock[0] = j;
                srcBlock[1] = mipCornerLU[1];
                
                dstBlock[0] = SubResourceBox.left;
                dstBlock[1] = SubResourceBox.top;
                
                g_JPEG_Manager.AddBlock( i, srcBlock, dstBlock );

                SubResourceBox.left += tileBlockSize;

                if( SubResourceBox.left > UINT(g_ClipmapStackSize - tileBlockSize) )
                    SubResourceBox.left = 0;
            }
            
            g_JPEG_Manager.Update( i );
        }

        g_StackPosition.y -= (float)g_UpdateRegionSize / g_SourceImageHeight;
    }

    if( g_StackPosition.y < 0.0f )
        g_StackPosition.y = 1.0f + g_StackPosition.y;
    else if( g_StackPosition.y > 1.0f )
        g_StackPosition.y -= 1.0f;
}

//--------------------------------------------------------------------------------------
// Calculate new positions of updatable regions in each clipmap stack level.
// Positions define the upper left corner of updatable square region
//--------------------------------------------------------------------------------------
void UpdateMipPosition( int &position, int offset )
{
    position += offset;

    if( offset > 0 )
    {
        if( position > g_ClipmapStackSize - offset)
            position = 0;
    }
    else
    {
        if( position < 0 )
            position = g_ClipmapStackSize + offset;
    }    
}

virtual void actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
    GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
    if(radioButton->isPressed() && radioButton->isChecked())
    {
		if(callbackString == "")
      	{
		}
	}
	}
}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Clipmaps");
	//if(std::string(r->getString())!="D3D11") stx_exit(11);
	IInput*    i=STX_Service::GetInputInstance();
	App app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		IRenderer::GetRendererInstance()->BeginScene();
		app.render();
		IRenderer::GetRendererInstance()->EndScene();
		IRenderer::GetRendererInstance()->Present( );
	}
	return 0;
}

