/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//======================================================================
//
//      HIGH DYNAMIC RANGE RENDERING DEMONSTRATION
//      Written by Jack Hoxley, October 2005
//
//======================================================================

// Project Includes
#include "HDRScene.h"
#include "Enumeration.h"

// As defined in the appropriate header, this translation unit is
// wrapped up in it's own C++ namespace:
namespace HDRScene
{

//--------------------------------------------------------------------------------------
// Data Structure Definitions
//--------------------------------------------------------------------------------------
struct LitVertex
{
    D3DXFROMWINEVECTOR3 p;
    __DWORD__ c;
};

static const __DWORD__ FVF_LITVERTEX = D3DFVF_XYZ | D3DFVF_DIFFUSE;

struct TLVertex
{
    D3DXFROMWINEVECTOR4 p;
    D3DXFROMWINEVECTOR2 t;
};

static const __DWORD__ FVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_TEX1;



//--------------------------------------------------------------------------------------
// Namespace-level variables
//--------------------------------------------------------------------------------------
MeshRenderer2 g_pCubeMesh = 0;             // Mesh representing the HDR source in the middle of the scene
ShaderID g_pCubePS = 0;             // The pixel shader for the cube
LPD3DXCONSTANTTABLE g_pCubePSConsts = 0;             // Interface for setting parameters/constants for the above PS
ShaderID g_pCubeVS = 0;             // The vertex shader for the cube
VerteXFormatID g_pCubeVSDecl = 0;             // The mapping from VB to VS
LPD3DXCONSTANTTABLE g_pCubeVSConsts = 0;             // Interface for setting params for the cube rendering
D3DXFROMWINEMATRIX g_mCubeMatrix;                              // The computed world*view*proj transform for the inner cube

TextureID g_pTexScene = 0;             // The main, floating point, render target
FORMAT g_fmtHDR = FORMAT_NONE;   // Enumerated and either set to 128 or 64 bit

MeshRenderer2 g_pOcclusionMesh = 0;             // The occlusion mesh surrounding the HDR cube
VerteXFormatID g_pOcclusionVSDecl = 0;             // The mapping for the MeshRenderer2
ShaderID g_pOcclusionVS = 0;             // The shader for drawing the occlusion mesh
LPD3DXCONSTANTTABLE g_pOcclusionVSConsts = 0;             // Entry point for configuring above shader
D3DXFROMWINEMATRIX g_mOcclusionMatrix;                         // The world*view*proj transform for transforming the POSITIONS
D3DXFROMWINEMATRIX g_mOcclusionNormals;                        // The transpose(inverse(world)) matrix for transforming NORMALS



//--------------------------------------------------------------------------------------
// Function Prototypes
//--------------------------------------------------------------------------------------
int LoadMesh( char* strFileName, MeshRenderer2 ppMesh );



//--------------------------------------------------------------------------------------
//  CreateResources( )
//
//      DESC:
//          This function creates all the necessary resources to render the HDR scene
//          to a render target for later use. When this function completes successfully
//          rendering can commence. A call to 'DestroyResources()' should be made when
//          the application closes.
//
//      PARAMS:
//          IRenderer::GetRendererInstance()      : The current device that resources should be created with/from
//          pDisplayDesc : Describes the back-buffer currently in use, can be useful when
//                         creating GUI based resources.
//
//      NOTES:
//          n/a
//--------------------------------------------------------------------------------------
int CreateResources(, const D3DSURFACE_DESC* pDisplayDesc )
{
    //[ 0 ] DECLARATIONS
    //------------------
    int 0;
    LPD3DXBUFFER pCode;      // Container for the compiled HLSL code


    //[ 1 ] DETERMINE FP TEXTURE SUPPORT
    //----------------------------------
    ( HDREnumeration::FindBestHDRFormat( &HDRScene::g_fmtHDR ) );
    if( ( hr ) )
    {
        LOG_PRINT( "HDRScene::CreateResources() - Current hardware does not support HDR rendering!\n" );
        return hr;
    }


    //[ 2 ] CREATE HDR RENDER TARGET
    //------------------------------
    ( IRenderer::GetRendererInstance()->addTexture(
       pDisplayDesc->Width, pDisplayDesc->Height,
       1, D3DUSAGE_RENDERTARGET, g_fmtHDR,
       D3DPOOL_DEFAULT, &HDRScene::g_pTexScene, 0
       ) );
    if( ( hr ) )
    {

        // We couldn't create the texture - lots of possible reasons for this. Best
        // check the D3D debug output for more details.
        LOG_PRINT(
            "HDRScene::CreateResources() - Could not create floating point render target. Examine D3D Debug Output for details.\n" );
        return hr;

    }


    //[ 3 ] CREATE HDR CUBE'S GEOMETRY
    //--------------------------------
    ( LoadMesh( "misc/Cube.x", &HDRScene::g_pCubeMesh ) );
    if( ( hr ) )
    {

        // Couldn't load the mesh, could be a file system error...
        LOG_PRINT( "HDRScene::CreateResources() - Could not load 'Cube.x'.\n" );
        return hr;

    }



    //[ 4 ] CREATE HDR CUBE'S PIXEL SHADER
    //------------------------------------
    HDRScene::g_pCubePS="Shader Code/HDRSource.psh";

    // [ 5 ] CREATE THE CUBE'S VERTEX DECL
    //------------------------------------
    FormatDesc cubeVertElems[MAX_FVF_DECL_SIZE];
    HDRScene::g_pCubeMesh->GetDeclaration( cubeVertElems );

    ( IRenderer::GetRendererInstance()->addVertexFormat( cubeVertElems, &HDRScene::g_pCubeVSDecl ) );
    if( ( hr ) )
    {

        // Couldn't create the declaration for the loaded mesh..
        LOG_PRINT(
            "HDRScene::CreateResources() - Couldn't create a vertex declaration for the HDR-Cube mesh.\n" );
        return hr;

    }



    // [ 6 ] CREATE THE CUBE'S VERTEX SHADER
    //--------------------------------------
    g_pCubeVS="Shader Code/HDRSource.vsh";

    //[ 5 ] LOAD THE OCCLUSION MESH
    //-----------------------------
    ( LoadMesh( "misc/OcclusionBox.x", &HDRScene::g_pOcclusionMesh ) );


    //[ 6 ] CREATE THE MESH VERTEX DECLARATION
    //----------------------------------------

    //[ 7 ] CREATE THE OCCLUSION VERTEX SHADER
    //----------------------------------------
    HDRScene::g_pOcclusionVS="Shader Code/OcclusionMesh.vsh";

    //[ 8 ] RETURN SUCCESS IF WE GOT THIS FAR
    //---------------------------------------
    return hr;

}

//--------------------------------------------------------------------------------------
//  CalculateResourceUsage( )
//
//      DESC:
//          Based on the known resources this function attempts to make an accurate
//          measurement of how much VRAM is being used by this part of the application.
//
//      NOTES:
//          Whilst the return value should be pretty accurate, it shouldn't be relied
//          on due to the way drivers/hardware can allocate memory.
//
//          Only the first level of the render target is checked as there should, by
//          definition, be no mip levels.
//
//--------------------------------------------------------------------------------------
__DWORD__ CalculateResourceUsage()
{

    // [ 0 ] DECLARATIONS
    //-------------------
    __DWORD__ usage = 0;

    // [ 1 ] RENDER TARGET SIZE
    //-------------------------
    D3DSURFACE_DESC texDesc;
    HDRScene::g_pTexScene->GetLevelDesc( 0, &texDesc );
    usage += ( ( texDesc.Width * texDesc.Height ) * ( HDRScene::g_fmtHDR == FORMAT_RGBA16f ? 8 : 16 ) );

    // [ 2 ] OCCLUSION MESH SIZE
    //--------------------------
    usage += ( HDRScene::g_pOcclusionMesh->GetNumBytesPerVertex() * HDRScene::g_pOcclusionMesh->GetNumVertices() );
    int index_size = ( ( HDRScene::g_pOcclusionMesh->GetOptions() & D3DXFROMWINEMESH_32BIT ) != 0 ) ? 4 : 2;
    usage += ( index_size * 3 * HDRScene::g_pOcclusionMesh->GetNumFaces() );

    return usage;

}



//--------------------------------------------------------------------------------------
//  RenderScene( )
//
//      DESC:
//          This is the core function for this unit - when it succesfully completes the
//          render target (obtainable via GetOutputTexture) will be a completed scene
//          that, crucially, contains values outside the LDR (0..1) range ready to be
//          fed into the various stages of the HDR post-processing pipeline.
//
//      PARAMS:
//          IRenderer::GetRendererInstance()     : The device that is currently being used for rendering
//
//      NOTES:
//          For future modifications, this is the entry point that should be used if
//          you require a different image/scene to be displayed on the screen.
//
//          This function assumes that the device is already in a ready-to-render
//          state (e.g. BeginScene() has been called).
//
//--------------------------------------------------------------------------------------
int RenderScene()
{


    // [ 0 ] CONFIGURE GEOMETRY INPUTS
    //--------------------------------
    IRenderer::GetRendererInstance()->SetVertexShader( HDRScene::g_pCubeVS );
    IRenderer::GetRendererInstance()->SetVertexDeclaration( HDRScene::g_pCubeVSDecl );
    HDRScene::g_pCubeVSConsts->SetMatrix(, "matWorldViewProj", &HDRScene::g_mCubeMatrix );



    // [ 1 ] PIXEL SHADER ( + PARAMS )
    //--------------------------------
    IRenderer::GetRendererInstance()->SetPixelShader( HDRScene::g_pCubePS );
    HDRScene::g_pCubePSConsts->SetFloat(, "HDRScalar", 5.0f );
    IRenderer::GetRendererInstance()->setTexture( 0, 0 );



    // [ 2 ] GET PREVIOUS RENDER TARGET
    //---------------------------------
    SurfaceID pPrevSurf = 0;
    if( ( IRenderer::GetRendererInstance()->GetRenderTarget( 0, &pPrevSurf ) ) )
    {

        // Couldn't retrieve the current render target (for restoration later on)
        LOG_PRINT( "HDRScene::RenderScene() - Could not retrieve a reference to the previous render target.\n"
                           );
        return 1;

    }



    // [ 3 ] SET NEW RENDER TARGET
    //----------------------------
    SurfaceID pRTSurf = 0;
    if( ( HDRScene::g_pTexScene->GetSurfaceLevel( 0, &pRTSurf ) ) )
    {

        // Bad news! couldn't get a reference to the HDR render target. Most
        // Likely due to a failed/corrupt resource creation stage.
        LOG_PRINT( "HDRScene::RenderScene() - Could not get the top level surface for the HDR render target\n"
                           );
        return 1;

    }

    if( ( IRenderer::GetRendererInstance()->changeRenderTarget( 0, pRTSurf ) ) )
    {

        // For whatever reason we can't set the HDR texture as the
        // the render target...
        LOG_PRINT( "HDRScene::RenderScene() - Could not set the HDR texture as a new render target.\n" );
        return 1;

    }

    // It is worth noting that the colour used to clear the render target will
    // be considered for the luminance measurement stage.
    IRenderer::GetRendererInstance()->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 64, 64, 192 ), 1.0f, 0 );



    // [ 4 ] RENDER THE HDR CUBE
    //--------------------------
    HDRScene::g_pCubeMesh->DrawSubset( 0 );



    // [ 5 ] DRAW THE OCCLUSION CUBE
    //------------------------------
    IRenderer::GetRendererInstance()->SetPixelShader( 0 );
    IRenderer::GetRendererInstance()->SetVertexDeclaration( HDRScene::g_pOcclusionVSDecl );
    IRenderer::GetRendererInstance()->SetVertexShader( HDRScene::g_pOcclusionVS );
    HDRScene::g_pOcclusionVSConsts->SetMatrix(, "matWorldViewProj", &HDRScene::g_mOcclusionMatrix );
    HDRScene::g_pOcclusionVSConsts->SetMatrix(, "matInvTPoseWorld", &HDRScene::g_mOcclusionNormals );

    // Due to the way that the mesh was authored, there is
    // only (and always) going to be 1 subset/group to render.
    HDRScene::g_pOcclusionMesh->DrawSubset( 0 );


    // [ 6 ] RESTORE PREVIOUS RENDER TARGET
    //-------------------------------------
    IRenderer::GetRendererInstance()->changeRenderTarget( 0, pPrevSurf );



    // [ 7 ] //RELEASE TEMPORARY REFERENCES
    //-----------------------------------
    //////////////SAFE_RELEASE( pRTSurf );
    //////////////SAFE_RELEASE( pPrevSurf );



    return 0;

}



//--------------------------------------------------------------------------------------
//  UpdateScene( )
//
//      DESC:
//          An entry point for updating various parameters and internal data on a
//          per-frame basis.
//
//      PARAMS:
//          IRenderer::GetRendererInstance()     : The currently active device
//          fTime       : The number of milliseconds elapsed since the start of execution
//          pCamera     : The arcball based camera that the end-user controls
//
//      NOTES:
//          n/a
//
//--------------------------------------------------------------------------------------
int UpdateScene(, float fTime, stx_CModelViewerCamera* pCamera )
{

    // The HDR cube in the middle of the scene never changes position in world
    // space, but must respond to view changes.
    HDRScene::g_mCubeMatrix = ( *pCamera->GetViewMatrix() ) * ( *pCamera->GetProjMatrix() );

    D3DXFROMWINEMATRIX matWorld;
    D3DXFROMWINEMATRIX matTemp;

    // The occlusion cube must be slightly larger than the inner HDR cube, so
    // a scaling constant is applied to the world matrix.
    D3DXFROMWINEMatrixIdentity( &matTemp );
    D3DXFROMWINEMatrixScaling( &matTemp, 2.5f, 2.5f, 2.5f );
    D3DXFROMWINEMatrixMultiply( &matWorld, &matTemp, pCamera->GetWorldMatrix() ); //&matWorld );

    // The occlusion cube contains lighting normals, so for the shader to operate
    // on them correctly, the inverse transpose of the world matrix is needed.
    D3DXFROMWINEMatrixIdentity( &matTemp );
    D3DXFROMWINEMatrixInverse( &matTemp, 0, &matWorld );
    D3DXFROMWINEMatrixTranspose( &HDRScene::g_mOcclusionNormals, &matTemp );

    HDRScene::g_mOcclusionMatrix = matWorld * ( *pCamera->GetViewMatrix() ) * ( *pCamera->GetProjMatrix() );

    return 0;

}



//--------------------------------------------------------------------------------------
//  GetOutputTexture( )
//
//      DESC:
//          The results of this modules rendering are used as inputs to several
//          other parts of the rendering pipeline. As such it is necessary to obtain
//          a reference to the internally managed texture.
//
//      PARAMS:
//          pTexture    : Should be 0 on entry, will be a valid reference on exit
//
//      NOTES:
//          The code that requests the reference is responsible for releasing their
//          copy of the texture as soon as they are finished using it.
//
//--------------------------------------------------------------------------------------
int GetOutputTexture( TextureID pTexture )
{

    // [ 0 ] ERASE ANY DATA IN THE INPUT
    //----------------------------------
    //////////////SAFE_RELEASE( *pTexture );

    // [ 1 ] COPY THE PRIVATE REFERENCE
    //---------------------------------
    *pTexture = HDRScene::g_pTexScene;

    // [ 2 ] INCREMENT THE REFERENCE COUNT..
    //--------------------------------------
    ( *pTexture )->AddRef();

    return 0;

}



//--------------------------------------------------------------------------------------
//  DrawToScreen( )
//
//      DESC:
//          Part of the GUI in this application displays the "raw" HDR data as part
//          of the process. This function places the texture, created by this
//          module, in the correct place on the screen.
//
//      PARAMS:
//          IRenderer::GetRendererInstance()     : The device to be drawn to.
//          pFont       : The font to use when annotating the display
//          pTextSprite : Used with the font for more efficient rendering
//          pArrowTex   : Stores the 4 (up/down/left/right) icons used in the GUI
//
//      NOTES:
//          n/a
//
//--------------------------------------------------------------------------------------
int DrawToScreen(, ID3DXFont* pFont, ID3DXSprite* pTextSprite,
                      TextureID pArrowTex )
{

    // [ 0 ] GATHER NECESSARY INFORMATION
    //-----------------------------------
    SurfaceID pSurf = 0;
    D3DSURFACE_DESC d;
    if( ( IRenderer::GetRendererInstance()->GetRenderTarget( 0, &pSurf ) ) )
    {

        // Couldn't get the current render target!
        LOG_PRINT( "HDRScene::DrawToScreen() - Could not get current render target to extract dimensions.\n"
                           );
        return 1;

    }

    pSurf->GetDesc( &d );
    //////////////SAFE_RELEASE( pSurf );

    // Cache the dimensions as floats for later use
    float fCellWidth = ( static_cast< float >( d.Width ) - 48.0f ) / 4.0f;
    float fCellHeight = ( static_cast< float >( d.Height ) - 36.0f ) / 4.0f;

    // [ 1 ] CREATE TILE GEOMETRY
    //---------------------------
    HDRScene::TLVertex v[4];

    v[0].p = D3DXFROMWINEVECTOR4( 0.0f, fCellHeight + 16.0f, 0.0f, 1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fCellWidth, fCellHeight + 16.0f, 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( 0.0f, ( 2.0f * fCellHeight ) + 16.0f, 0.0f, 1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fCellWidth, ( 2.0f * fCellHeight ) + 16.0f, 0.0f, 1.0f );

    v[0].t = D3DXFROMWINEVECTOR2( 0.0f, 0.0f );
    v[1].t = D3DXFROMWINEVECTOR2( 1.0f, 0.0f );
    v[2].t = D3DXFROMWINEVECTOR2( 0.0f, 1.0f );
    v[3].t = D3DXFROMWINEVECTOR2( 1.0f, 1.0f );

    // [ 2 ] DISPLAY TILE ON SCREEN
    //-----------------------------
    IRenderer::GetRendererInstance()->SetVertexShader( 0 );
    IRenderer::GetRendererInstance()->SetFVF( HDRScene::FVF_TLVERTEX );
    IRenderer::GetRendererInstance()->setTexture( 0, HDRScene::g_pTexScene );
    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( HDRScene::TLVertex ) );

    // [ 3 ] RENDER CONNECTING ARROWS
    //-------------------------------
    IRenderer::GetRendererInstance()->setTexture( 0, pArrowTex );

    v[0].p = D3DXFROMWINEVECTOR4( ( fCellWidth / 2.0f ) - 8.0f, fCellHeight, 0.0f, 1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( ( fCellWidth / 2.0f ) + 8.0f, fCellHeight, 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( ( fCellWidth / 2.0f ) - 8.0f, fCellHeight + 16.0f, 0.0f, 1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( ( fCellWidth / 2.0f ) + 8.0f, fCellHeight + 16.0f, 0.0f, 1.0f );

    v[0].t = D3DXFROMWINEVECTOR2( 0.0f, 0.0f );
    v[1].t = D3DXFROMWINEVECTOR2( 0.25f, 0.0f );
    v[2].t = D3DXFROMWINEVECTOR2( 0.0f, 1.0f );
    v[3].t = D3DXFROMWINEVECTOR2( 0.25f, 1.0f );

    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( HDRScene::TLVertex ) );

    v[0].p = D3DXFROMWINEVECTOR4( fCellWidth, fCellHeight + 8.0f + ( fCellHeight / 2.0f ), 0.0f, 1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fCellWidth + 16.0f, fCellHeight + 8.0f + ( fCellHeight / 2.0f ), 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( fCellWidth, fCellHeight + 24.0f + ( fCellHeight / 2.0f ), 0.0f, 1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fCellWidth + 16.0f, fCellHeight + 24.0f + ( fCellHeight / 2.0f ), 0.0f, 1.0f );

    v[0].t = D3DXFROMWINEVECTOR2( 0.25f, 0.0f );
    v[1].t = D3DXFROMWINEVECTOR2( 0.50f, 0.0f );
    v[2].t = D3DXFROMWINEVECTOR2( 0.25f, 1.0f );
    v[3].t = D3DXFROMWINEVECTOR2( 0.50f, 1.0f );

    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( HDRScene::TLVertex ) );

    float fLumCellSize = ( ( static_cast< float >( d.Height ) - ( ( 2.0f * fCellHeight ) + 32.0f ) ) - 32.0f ) / 3.0f;
    float fLumStartX = ( fCellWidth + 16.0f ) - ( ( 2.0f * fLumCellSize ) + 32.0f );

    v[0].p = D3DXFROMWINEVECTOR4( fLumStartX + ( fLumCellSize / 2.0f ) - 8.0f, ( 2.0f * fCellHeight ) + 16.0f, 0.0f, 1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fLumStartX + ( fLumCellSize / 2.0f ) + 8.0f, ( 2.0f * fCellHeight ) + 16.0f, 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( fLumStartX + ( fLumCellSize / 2.0f ) - 8.0f, ( 2.0f * fCellHeight ) + 32.0f, 0.0f, 1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fLumStartX + ( fLumCellSize / 2.0f ) + 8.0f, ( 2.0f * fCellHeight ) + 32.0f, 0.0f, 1.0f );

    v[0].t = D3DXFROMWINEVECTOR2( 0.50f, 0.0f );
    v[1].t = D3DXFROMWINEVECTOR2( 0.75f, 0.0f );
    v[2].t = D3DXFROMWINEVECTOR2( 0.50f, 1.0f );
    v[3].t = D3DXFROMWINEVECTOR2( 0.75f, 1.0f );

    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( HDRScene::TLVertex ) );

    return 0;

}



//--------------------------------------------------------------------------------------
//  LoadMesh( )
//
//      DESC:
//          A utility method borrowed from the DXSDK samples. Loads a .X mesh into
//          an MeshRenderer2 object for rendering.
//
//--------------------------------------------------------------------------------------
int LoadMesh( char* strFileName, MeshRenderer2 ppMesh )
{

    MeshRenderer2 pMesh = strFileName;

    *ppMesh = pMesh;

    return hr;

}

}
;

