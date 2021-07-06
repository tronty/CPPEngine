/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//======================================================================
//
//      HIGH DYNAMIC RANGE RENDERING DEMONSTRATION
//      Written by Jack Hoxley, November 2005
//
//======================================================================

#include "DXUT.h"
#include "SDKmisc.h"
#include "HDRScene.h"
#include "Luminance.h"
#include "Enumeration.h"

namespace Luminance
{

//--------------------------------------------------------------------------------------
// Data Structure Definitions
//--------------------------------------------------------------------------------------
struct TLVertex
{
    D3DXFROMWINEVECTOR4 p;
    D3DXFROMWINEVECTOR2 t;
};

static const __DWORD__ FVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_TEX1;



//--------------------------------------------------------------------------------------
// Namespace-Level Variables
//--------------------------------------------------------------------------------------
ShaderID g_pLumDispPS = 0;             // PShader used to display the debug panel

ShaderID g_pLum1PS = 0;             // PShader that does a 2x2 downsample and convert to greyscale
LPD3DXCONSTANTTABLE g_pLum1PSConsts = 0;             // Interface to set the sampling points for the above PS

ShaderID g_pLum3x3DSPS = 0;             // The PS that does each 3x3 downsample operation
LPD3DXCONSTANTTABLE g_pLum3x3DSPSConsts = 0;             // Interface for the above PS

static const __DWORD__ g_dwLumTextures = 6;                // How many luminance textures we're creating
// Be careful when changing this value, higher than 6 might
// require you to implement code that creates an additional
// depth-stencil buffer due to the luminance textures dimensions
// being greater than that of the default depth-stencil buffer.
TextureID g_pTexLuminance[ g_dwLumTextures ];         // An array of the downsampled luminance textures
FORMAT g_fmtHDR = FORMAT_NONE;   // Should be either G32R32F or G16R16F depending on the hardware



//--------------------------------------------------------------------------------------
// Private Function Prototypes
//--------------------------------------------------------------------------------------
int RenderToTexture();



//--------------------------------------------------------------------------------------
//  CreateResources( )
//
//      DESC:
//          This function creates the necessary texture chain for downsampling the
//          initial HDR texture to a 1x1 luminance texture.
//
//      PARAMS:
//          IRenderer::GetRendererInstance()      : The current device that resources should be created with/from
//          pDisplayDesc : Describes the back-buffer currently in use, can be useful when
//                         creating GUI based resources.
//
//      NOTES:
//          n/a
//--------------------------------------------------------------------------------------
int CreateResources()
{

    //[ 0 ] DECLARATIONS
    //------------------
    int 0;
    LPD3DXBUFFER pCode;      // Container for the compiled HLSL code



    //[ 1 ] DETERMINE FP TEXTURE SUPPORT
    //----------------------------------
    ( HDREnumeration::FindBestLuminanceFormat( &Luminance::g_fmtHDR ) );
    if( ( hr ) )
    {
        // Bad news!
        LOG_PRINT( "Luminance::CreateResources() - Current hardware does not support HDR rendering!\n" );
        return hr;
    }



    //[ 2 ] CREATE HDR RENDER TARGETS
    //-------------------------------
    int iTextureSize = 1;
    for( int i = 0; i < Luminance::g_dwLumTextures; i++ )
    {

        // Create this element in the array
        ( IRenderer::GetRendererInstance()->addTexture( iTextureSize, iTextureSize, 1,
                                   D3DUSAGE_RENDERTARGET, Luminance::g_fmtHDR,
                                   D3DPOOL_DEFAULT, &Luminance::g_pTexLuminance[i], 0 ) );
        if( ( hr ) )
        {
            // Couldn't create this texture, probably best to inspect the debug runtime
            // for more information
            char str[MAX_PATH];
            swprintf_s( str, MAX_PATH, "Luminance::CreateResources() : Unable to create luminance"
                             " texture of %dx%d (Element %d of %d).\n",
                             iTextureSize,
                             iTextureSize,
                             ( i + 1 ),
                             Luminance::g_dwLumTextures );
            LOG_PRINT( str );
            return hr;
        }

        // Increment for the next texture
        iTextureSize *= 3;

    }



    //[ 3 ] CREATE GUI DISPLAY SHADER
    //-------------------------------
    // Because the intermediary luminance textures are stored as either G32R32F
    // or G16R16F, we need a pixel shader that can convert this to a more meaningful
    // greyscale ARGB value. This shader doesn't actually contribute to the
    // luminance calculations - just the way they are presented to the user via the GUI.
    char str[MAX_PATH];
    ( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, "Shader Code/Luminance.psh" ) );
    ( D3DXFROMWINECompileShaderFromFile(
       str,
       0, 0,
       "LuminanceDisplay",
       "ps_2_0",
       0,
       &pCode,
       0,
       0
       ) );
    if( ( hr ) )
    {

        // Couldn't compile the shader, use the 'compile_shaders.bat' script
        // in the 'Shader Code' folder to get a proper compile breakdown.
        LOG_PRINT(
            "Luminance::CreateResources() - Compiling of 'LuminanceDisplay' from 'Luminance.psh' failed!\n" );
        return hr;

    }

    ( IRenderer::GetRendererInstance()->CreatePixelShader( reinterpret_cast< __DWORD__* >( pCode->GetBufferPointer() ),
                                   &Luminance::g_pLumDispPS ) );
    if( ( hr ) )
    {

        // Couldn't turn the compiled shader into an actual, usable, pixel shader!
        LOG_PRINT(
            "Luminance::CreateResources() - Could not create a pixel shader object for 'LuminanceDisplay'.\n" );
        pCode->Release();
        return hr;

    }

    pCode->Release();



    //[ 4 ] CREATE FIRST-PASS DOWNSAMPLE SHADER
    //-----------------------------------------
    // The first pass of down-sampling has to convert the RGB data to
    // a single luminance value before averaging over the kernel. This
    // is slightly different to the subsequent down-sampling shader.
    ( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, "Shader Code/Luminance.psh" ) );
    ( D3DXFROMWINECompileShaderFromFile(
       str,
       0, 0,
       "GreyScaleDownSample",
       "ps_2_0",
       0,
       &pCode,
       0,
       &Luminance::g_pLum1PSConsts
       ) );
    if( ( hr ) )
    {

        // Couldn't compile the shader, use the 'compile_shaders.bat' script
        // in the 'Shader Code' folder to get a proper compile breakdown.
        LOG_PRINT(
            "Luminance::CreateResources() - Compiling of 'GreyScaleDownSample' from 'Luminance.psh' failed!\n" );
        return hr;

    }

    ( IRenderer::GetRendererInstance()->CreatePixelShader( reinterpret_cast< __DWORD__* >( pCode->GetBufferPointer() ), &Luminance::g_pLum1PS ) );
    if( ( hr ) )
    {

        // Couldn't turn the compiled shader into an actual, usable, pixel shader!
        LOG_PRINT(
            "Luminance::CreateResources() - Could not create a pixel shader object for 'GreyScaleDownSample'.\n" );
        pCode->Release();
        return hr;

    }

    pCode->Release();



    //[ 5 ] CREATE DOWNSAMPLING PIXEL SHADER
    //--------------------------------------
    // This down-sampling shader assumes that the incoming pixels are
    // already in G16R16F or G32R32F format, and of a paired luminance/maximum value.
    ( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, "Shader Code/Luminance.psh" ) );
    ( D3DXFROMWINECompileShaderFromFile(
       str,
       0, 0,
       "DownSample",
       "ps_2_0",
       0,
       &pCode,
       0,
       &Luminance::g_pLum3x3DSPSConsts
       ) );
    if( ( hr ) )
    {

        // Couldn't compile the shader, use the 'compile_shaders.bat' script
        // in the 'Shader Code' folder to get a proper compile breakdown.
        LOG_PRINT( "Luminance::CreateResources() - Compiling of 'DownSample' from 'Luminance.psh' failed!\n"
                           );
        return hr;

    }

    ( IRenderer::GetRendererInstance()->CreatePixelShader( reinterpret_cast< __DWORD__* >( pCode->GetBufferPointer() ),
                                   &Luminance::g_pLum3x3DSPS ) );
    if( ( hr ) )
    {

        // Couldn't turn the compiled shader into an actual, usable, pixel shader!
        LOG_PRINT( "Luminance::CreateResources() : Could not create a pixel shader object for 'DownSample'.\n"
                           );
        pCode->Release();
        return hr;

    }

    pCode->Release();


    return hr;

}

//--------------------------------------------------------------------------------------
//  MeasureLuminance( )
//
//      DESC:
//          This is the core function for this particular part of the application, it's
//          job is to take the previously rendered (in the 'HDRScene' namespace) HDR
//          image and compute the overall luminance for the scene. This is done by
//          repeatedly downsampling the image until it is only 1x1 in size. Doing it
//          this way (pixel shaders and render targets) keeps as much of the work on
//          the GPU as possible, consequently avoiding any resource transfers, locking
//          and modification.
//
//      PARAMS:
//          IRenderer::GetRendererInstance() : The currently active device that will be used for rendering.
//
//      NOTES:
//          The results from this function will eventually be used to compose the final
//          image. See OnFrameRender() in 'HDRDemo.cpp'.
//
//--------------------------------------------------------------------------------------
int MeasureLuminance()
{

    //[ 0 ] DECLARE VARIABLES AND ALIASES
    //-----------------------------------
    TextureID pSourceTex = 0;     // We use this texture as the input
    TextureID pDestTex = 0;     // We render to this texture...
    SurfaceID pDestSurf = 0;     // ... Using this ptr to it's top-level surface


    //[ 1 ] SET THE DEVICE TO RENDER TO THE HIGHEST
    //      RESOLUTION LUMINANCE MAP.
    //---------------------------------------------
    HDRScene::GetOutputTexture( &pSourceTex );
    pDestTex = Luminance::g_pTexLuminance[ Luminance::g_dwLumTextures - 1 ];
    if( ( pDestTex->GetSurfaceLevel( 0, &pDestSurf ) ) )
    {

        // Couldn't acquire this surface level. Odd!
        LOG_PRINT(
            "Luminance::MeasureLuminance( ) : Couldn't acquire surface level for hi-res luminance map!\n" );
        return 1;

    }

    IRenderer::GetRendererInstance()->changeRenderTarget( 0, pDestSurf );
    IRenderer::GetRendererInstance()->setTexture( 0, pSourceTex );

    //IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    //IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );


    //[ 2 ] RENDER AND DOWNSAMPLE THE HDR TEXTURE
    //      TO THE LUMINANCE MAP.
    //-------------------------------------------

    // Set which shader we're going to use. g_pLum1PS corresponds
    // to the 'GreyScaleDownSample' entry point in 'Luminance.psh'.
    IRenderer::GetRendererInstance()->SetPixelShader( Luminance::g_pLum1PS );

    // We need to compute the sampling offsets used for this pass.
    // A 2x2 sampling pattern is used, so we need to generate 4 offsets.
    //
    // NOTE: It is worth noting that some information will likely be lost
    //       due to the luminance map being less than 1/2 the size of the
    //       original render-target. This mis-match does not have a particularly
    //       big impact on the final luminance measurement. If necessary,
    //       the same process could be used - but with many more samples, so as
    //       to correctly map from HDR->Luminance without losing information.
    D3DXFROMWINEVECTOR4 offsets[4];

    // Find the dimensions for the source data
    D3DSURFACE_DESC srcDesc;
    pSourceTex->GetLevelDesc( 0, &srcDesc );

    // Because the source and destination are NOT the same sizes, we
    // need to provide offsets to correctly map between them.
    float sU = ( 1.0f / static_cast< float >( srcDesc.Width ) );
    float sV = ( 1.0f / static_cast< float >( srcDesc.Height ) );

    // The last two components (z,w) are unused. This makes for simpler code, but if
    // constant-storage is limited then it is possible to pack 4 offsets into 2 D3DXFROMWINEVECTOR4's
    offsets[0] = D3DXFROMWINEVECTOR4( -0.5f * sU, 0.5f * sV, 0.0f, 0.0f );
    offsets[1] = D3DXFROMWINEVECTOR4( 0.5f * sU, 0.5f * sV, 0.0f, 0.0f );
    offsets[2] = D3DXFROMWINEVECTOR4( -0.5f * sU, -0.5f * sV, 0.0f, 0.0f );
    offsets[3] = D3DXFROMWINEVECTOR4( 0.5f * sU, -0.5f * sV, 0.0f, 0.0f );

    // Set the offsets to the constant table
    Luminance::g_pLum1PSConsts->SetVectorArray(, "tcLumOffsets", offsets, 4 );

    // With everything configured we can now render the first, initial, pass
    // to the luminance textures.
    RenderToTexture();

    // Make sure we clean up the remaining reference
    //////////////SAFE_RELEASE( pDestSurf );
    //////////////SAFE_RELEASE( pSourceTex );


    //[ 3 ] SCALE EACH RENDER TARGET DOWN
    //      The results ("dest") of each pass feeds into the next ("src")
    //-------------------------------------------------------------------
    for( int i = ( Luminance::g_dwLumTextures - 1 ); i > 0; i-- )
    {

        // Configure the render targets for this iteration
        pSourceTex = Luminance::g_pTexLuminance[ i ];
        pDestTex = Luminance::g_pTexLuminance[ i - 1 ];
        if( ( pDestTex->GetSurfaceLevel( 0, &pDestSurf ) ) )
        {

            // Couldn't acquire this surface level. Odd!
            LOG_PRINT( "Luminance::MeasureLuminance( ) : Couldn't acquire surface level for luminance map!\n"
                               );
            return 1;

        }

        IRenderer::GetRendererInstance()->changeRenderTarget( 0, pDestSurf );
        IRenderer::GetRendererInstance()->setTexture( 0, pSourceTex );

        // We don't want any filtering for this pass
        //IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        //IRenderer::GetRendererInstance()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

        // Because each of these textures is a factor of 3
        // different in dimension, we use a 3x3 set of sampling
        // points to downscale.
        D3DSURFACE_DESC srcTexDesc;
        pSourceTex->GetLevelDesc( 0, &srcTexDesc );

        // Create the 3x3 grid of offsets
        D3DXFROMWINEVECTOR4 DSoffsets[9];
        int idx = 0;
        for( int x = -1; x < 2; x++ )
        {
            for( int y = -1; y < 2; y++ )
            {
                DSoffsets[idx++] = D3DXFROMWINEVECTOR4(
                    static_cast< float >( x ) / static_cast< float >( srcTexDesc.Width ),
                    static_cast< float >( y ) / static_cast< float >( srcTexDesc.Height ),
                    0.0f,   //unused
                    0.0f    //unused
                    );
            }
        }

        // Set them to the current pixel shader
        IRenderer::GetRendererInstance()->SetPixelShader( Luminance::g_pLum3x3DSPS );
        Luminance::g_pLum3x3DSPSConsts->SetVectorArray(, "tcDSOffsets", DSoffsets, 9 );

        // Render the display to this texture
        RenderToTexture();

        // Clean-up by releasing the level-0 surface
        //////////////SAFE_RELEASE( pDestSurf );

    }


    // =============================================================
    //    At this point, the g_pTexLuminance[0] texture will contain
    //    a 1x1 texture that has the downsampled luminance for the
    //    scene as it has currently been rendered.
    // =============================================================

    return 0;

}



//--------------------------------------------------------------------------------------
//  DisplayLuminance( )
//
//      DESC:
//          This function is for presentation purposes only - and isn't a *required*
//          part of the HDR rendering pipeline. It draws the 6 stages of the luminance
//          calculation to the appropriate part of the screen.
//
//      PARAMS:
//          IRenderer::GetRendererInstance()     : The device to be rendered to.
//          pFont       : The font to use when adding the annotations
//          pTextSprite : Used to improve performance of the text rendering
//          pArrowTex   : Stores the 4 (up/down/left/right) icons used in the GUI
//
//      NOTES:
//          This code uses several hard-coded ratios to position the elements correctly
//          - as such, changing the underlying diagram may well break this code.
//
//--------------------------------------------------------------------------------------
int DisplayLuminance( TextureID pArrowTex )
{

    // [ 0 ] COMMON INITIALIZATION
    //----------------------------

    SurfaceID pSurf = 0;
    D3DSURFACE_DESC d;

    if( ( IRenderer::GetRendererInstance()->GetRenderTarget( 0, &pSurf ) ) )
    {

        // Couldn't get the current render target!
        LOG_PRINT(
            "Luminance::DisplayLuminance() - Could not get current render target to extract dimensions.\n" );
        return 1;

    }

    pSurf->GetDesc( &d );
    //////////////SAFE_RELEASE( pSurf );

    // Cache the dimensions as floats for later use
    float fW = static_cast< float >( d.Width );
    float fH = static_cast< float >( d.Height );
    float fCellH = ( fH - 36.0f ) / 4.0f;
    float fCellW = ( fW - 48.0f ) / 4.0f;
    float fLumCellSize = ( ( fH - ( ( 2.0f * fCellH ) + 32.0f ) ) - 32.0f ) / 3.0f;
    float fLumStartX = ( fCellW + 16.0f ) - ( ( 2.0f * fLumCellSize ) + 32.0f );

    // Fill out the basic TLQuad information - this
    // stuff doesn't change for each stage
    Luminance::TLVertex v[4];
    v[0].t = D3DXFROMWINEVECTOR2( 0.0f, 0.0f );
    v[1].t = D3DXFROMWINEVECTOR2( 1.0f, 0.0f );
    v[2].t = D3DXFROMWINEVECTOR2( 0.0f, 1.0f );
    v[3].t = D3DXFROMWINEVECTOR2( 1.0f, 1.0f );

    // Configure the device for it's basic states
    IRenderer::GetRendererInstance()->SetVertexShader( 0 );
    IRenderer::GetRendererInstance()->SetFVF( FVF_TLVERTEX );
    IRenderer::GetRendererInstance()->SetPixelShader( g_pLumDispPS );

    CDXUTTextHelper txtHelper( pFont, pTextSprite, 12 );
    txtHelper.SetForegroundColor( D3DXFROMWINEVECTOR4( 1.0f, 0.5f, 0.0f, 1.0f ) );

    // [ 1 ] RENDER FIRST LEVEL
    //-------------------------
    v[0].p = D3DXFROMWINEVECTOR4( fLumStartX, ( 2.0f * fCellH ) + 32.0f, 0.0f, 1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize, ( 2.0f * fCellH ) + 32.0f, 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( fLumStartX, ( 2.0f * fCellH ) + 32.0f + fLumCellSize, 0.0f, 1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize, ( 2.0f * fCellH ) + 32.0f + fLumCellSize, 0.0f, 1.0f );

    IRenderer::GetRendererInstance()->setTexture( 0, Luminance::g_pTexLuminance[ 5 ] );
    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( Luminance::TLVertex ) );


    // [ 2 ] RENDER SECOND LEVEL
    //--------------------------
    v[0].p = D3DXFROMWINEVECTOR4( fLumStartX, ( 2.0f * fCellH ) + 48.0f + fLumCellSize, 0.0f, 1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize, ( 2.0f * fCellH ) + 48.0f + fLumCellSize, 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( fLumStartX, ( 2.0f * fCellH ) + 48.0f + ( 2.0f * fLumCellSize ), 0.0f, 1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize, ( 2.0f * fCellH ) + 48.0f + ( 2.0f * fLumCellSize ), 0.0f, 1.0f );

    IRenderer::GetRendererInstance()->setTexture( 0, Luminance::g_pTexLuminance[ 4 ] );
    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( Luminance::TLVertex ) );

    // [ 3 ] RENDER THIRD LEVEL
    //-------------------------
    v[0].p = D3DXFROMWINEVECTOR4( fLumStartX, ( 2.0f * fCellH ) + 64.0f + ( 2.0f * fLumCellSize ), 0.0f, 1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize, ( 2.0f * fCellH ) + 64.0f + ( 2.0f * fLumCellSize ), 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( fLumStartX, ( 2.0f * fCellH ) + 64.0f + ( 3.0f * fLumCellSize ), 0.0f, 1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize, ( 2.0f * fCellH ) + 64.0f + ( 3.0f * fLumCellSize ), 0.0f, 1.0f );

    IRenderer::GetRendererInstance()->setTexture( 0, Luminance::g_pTexLuminance[ 3 ] );
    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( Luminance::TLVertex ) );

    // [ 4 ] RENDER FOURTH LEVEL
    //--------------------------
    v[0].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize + 16.0f, ( 2.0f * fCellH ) + 64.0f + ( 2.0f * fLumCellSize ), 0.0f,
                          1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 2.0f * fLumCellSize ) + 16.0f, ( 2.0f * fCellH ) + 64.0f +
                          ( 2.0f * fLumCellSize ), 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize + 16.0f, ( 2.0f * fCellH ) + 64.0f + ( 3.0f * fLumCellSize ), 0.0f,
                          1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 2.0f * fLumCellSize ) + 16.0f, ( 2.0f * fCellH ) + 64.0f +
                          ( 3.0f * fLumCellSize ), 0.0f, 1.0f );

    IRenderer::GetRendererInstance()->setTexture( 0, Luminance::g_pTexLuminance[ 2 ] );
    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( Luminance::TLVertex ) );

    // [ 5 ] RENDER FIFTH LEVEL
    //--------------------------
    v[0].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize + 16.0f, ( 2.0f * fCellH ) + 48.0f + ( 1.0f * fLumCellSize ), 0.0f,
                          1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 2.0f * fLumCellSize ) + 16.0f, ( 2.0f * fCellH ) + 48.0f +
                          ( 1.0f * fLumCellSize ), 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize + 16.0f, ( 2.0f * fCellH ) + 48.0f + ( 2.0f * fLumCellSize ), 0.0f,
                          1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 2.0f * fLumCellSize ) + 16.0f, ( 2.0f * fCellH ) + 48.0f +
                          ( 2.0f * fLumCellSize ), 0.0f, 1.0f );

    IRenderer::GetRendererInstance()->setTexture( 0, Luminance::g_pTexLuminance[ 1 ] );
    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( Luminance::TLVertex ) );

    // [ 6 ] RENDER SIXTH LEVEL
    //--------------------------
    v[0].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize + 16.0f, ( 2.0f * fCellH ) + 32.0f + ( 0.0f * fLumCellSize ), 0.0f,
                          1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 2.0f * fLumCellSize ) + 16.0f, ( 2.0f * fCellH ) + 32.0f +
                          ( 0.0f * fLumCellSize ), 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize + 16.0f, ( 2.0f * fCellH ) + 32.0f + ( 1.0f * fLumCellSize ), 0.0f,
                          1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 2.0f * fLumCellSize ) + 16.0f, ( 2.0f * fCellH ) + 32.0f +
                          ( 1.0f * fLumCellSize ), 0.0f, 1.0f );

    IRenderer::GetRendererInstance()->setTexture( 0, Luminance::g_pTexLuminance[ 0 ] );
    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( Luminance::TLVertex ) );

    // [ 7 ] RENDER ARROWS
    //--------------------
    IRenderer::GetRendererInstance()->SetPixelShader( 0 );
    IRenderer::GetRendererInstance()->setTexture( 0, pArrowTex );

    // Select the "down" arrow

    v[0].t = D3DXFROMWINEVECTOR2( 0.50f, 0.0f );
    v[1].t = D3DXFROMWINEVECTOR2( 0.75f, 0.0f );
    v[2].t = D3DXFROMWINEVECTOR2( 0.50f, 1.0f );
    v[3].t = D3DXFROMWINEVECTOR2( 0.75f, 1.0f );

    // From 1st down to 2nd

    v[0].p = D3DXFROMWINEVECTOR4( fLumStartX + ( fLumCellSize / 2.0f ) - 8.0f, ( 2.0f * fCellH ) + 32.0f +
                          ( 1.0f * fLumCellSize ), 0.0f, 1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fLumStartX + ( fLumCellSize / 2.0f ) + 8.0f, ( 2.0f * fCellH ) + 32.0f +
                          ( 1.0f * fLumCellSize ), 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( fLumStartX + ( fLumCellSize / 2.0f ) - 8.0f, ( 2.0f * fCellH ) + 48.0f +
                          ( 1.0f * fLumCellSize ), 0.0f, 1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fLumStartX + ( fLumCellSize / 2.0f ) + 8.0f, ( 2.0f * fCellH ) + 48.0f +
                          ( 1.0f * fLumCellSize ), 0.0f, 1.0f );

    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( Luminance::TLVertex ) );

    // From 2nd down to 3rd

    v[0].p = D3DXFROMWINEVECTOR4( fLumStartX + ( fLumCellSize / 2.0f ) - 8.0f, ( 2.0f * fCellH ) + 48.0f +
                          ( 2.0f * fLumCellSize ), 0.0f, 1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fLumStartX + ( fLumCellSize / 2.0f ) + 8.0f, ( 2.0f * fCellH ) + 48.0f +
                          ( 2.0f * fLumCellSize ), 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( fLumStartX + ( fLumCellSize / 2.0f ) - 8.0f, ( 2.0f * fCellH ) + 64.0f +
                          ( 2.0f * fLumCellSize ), 0.0f, 1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fLumStartX + ( fLumCellSize / 2.0f ) + 8.0f, ( 2.0f * fCellH ) + 64.0f +
                          ( 2.0f * fLumCellSize ), 0.0f, 1.0f );

    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( Luminance::TLVertex ) );

    // Select "right" arrow

    v[0].t = D3DXFROMWINEVECTOR2( 0.25f, 0.0f );
    v[1].t = D3DXFROMWINEVECTOR2( 0.50f, 0.0f );
    v[2].t = D3DXFROMWINEVECTOR2( 0.25f, 1.0f );
    v[3].t = D3DXFROMWINEVECTOR2( 0.50f, 1.0f );

    // Across from 3rd to 4th

    v[0].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize, ( 2.0f * fCellH ) + 56.0f + ( 2.5f * fLumCellSize ), 0.0f, 1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize + 16.0f, ( 2.0f * fCellH ) + 56.0f + ( 2.5f * fLumCellSize ), 0.0f,
                          1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize, ( 2.0f * fCellH ) + 72.0f + ( 2.5f * fLumCellSize ), 0.0f, 1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fLumStartX + fLumCellSize + 16.0f, ( 2.0f * fCellH ) + 72.0f + ( 2.5f * fLumCellSize ), 0.0f,
                          1.0f );

    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( Luminance::TLVertex ) );

    // Select "up" arrow

    v[0].t = D3DXFROMWINEVECTOR2( 0.00f, 0.0f );
    v[1].t = D3DXFROMWINEVECTOR2( 0.25f, 0.0f );
    v[2].t = D3DXFROMWINEVECTOR2( 0.00f, 1.0f );
    v[3].t = D3DXFROMWINEVECTOR2( 0.25f, 1.0f );

    // Up from 4th to 5th

    v[0].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 1.5f * fLumCellSize ) + 8.0f, ( 2.0f * fCellH ) + 48.0f +
                          ( 2.0f * fLumCellSize ), 0.0f, 1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 1.5f * fLumCellSize ) + 24.0f, ( 2.0f * fCellH ) + 48.0f +
                          ( 2.0f * fLumCellSize ), 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 1.5f * fLumCellSize ) + 8.0f, ( 2.0f * fCellH ) + 64.0f +
                          ( 2.0f * fLumCellSize ), 0.0f, 1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 1.5f * fLumCellSize ) + 24.0f, ( 2.0f * fCellH ) + 64.0f +
                          ( 2.0f * fLumCellSize ), 0.0f, 1.0f );

    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( Luminance::TLVertex ) );

    // Up from 5th to 6th

    v[0].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 1.5f * fLumCellSize ) + 8.0f, ( 2.0f * fCellH ) + 32.0f +
                          ( 1.0f * fLumCellSize ), 0.0f, 1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 1.5f * fLumCellSize ) + 24.0f, ( 2.0f * fCellH ) + 32.0f +
                          ( 1.0f * fLumCellSize ), 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 1.5f * fLumCellSize ) + 8.0f, ( 2.0f * fCellH ) + 48.0f +
                          ( 1.0f * fLumCellSize ), 0.0f, 1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 1.5f * fLumCellSize ) + 24.0f, ( 2.0f * fCellH ) + 48.0f +
                          ( 1.0f * fLumCellSize ), 0.0f, 1.0f );

    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( Luminance::TLVertex ) );

    // Select "right" arrow

    v[0].t = D3DXFROMWINEVECTOR2( 0.25f, 0.0f );
    v[1].t = D3DXFROMWINEVECTOR2( 0.50f, 0.0f );
    v[2].t = D3DXFROMWINEVECTOR2( 0.25f, 1.0f );
    v[3].t = D3DXFROMWINEVECTOR2( 0.50f, 1.0f );

    // From 6th to final image composition

    v[0].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 2.0f * fLumCellSize ) + 16.0f, ( 2.0f * fCellH ) + 24.0f +
                          ( 0.5f * fLumCellSize ), 0.0f, 1.0f );
    v[1].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 2.0f * fLumCellSize ) + 32.0f, ( 2.0f * fCellH ) + 24.0f +
                          ( 0.5f * fLumCellSize ), 0.0f, 1.0f );
    v[2].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 2.0f * fLumCellSize ) + 16.0f, ( 2.0f * fCellH ) + 40.0f +
                          ( 0.5f * fLumCellSize ), 0.0f, 1.0f );
    v[3].p = D3DXFROMWINEVECTOR4( fLumStartX + ( 2.0f * fLumCellSize ) + 32.0f, ( 2.0f * fCellH ) + 40.0f +
                          ( 0.5f * fLumCellSize ), 0.0f, 1.0f );

    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( Luminance::TLVertex ) );


    return 0;

}



//--------------------------------------------------------------------------------------
//  GetLuminanceTexture( )
//
//      DESC:
//          The final 1x1 luminance texture created by the MeasureLuminance() function
//          is required as an input into the final image composition. Because of this
//          it is necessary for other parts of the application to have access to this
//          particular texture.
//
//      PARAMS:
//          pTexture    : Should be 0 on entry, will be a valid reference on exit
//
//      NOTES:
//          The code that requests the reference is responsible for releasing their
//          copy of the texture as soon as they are finished using it.
//
//--------------------------------------------------------------------------------------
int GetLuminanceTexture( TextureID pTex )
{

    // [ 0 ] ERASE ANY DATA IN THE INPUT
    //----------------------------------
    //////////////SAFE_RELEASE( *pTex );

    // [ 1 ] COPY THE PRIVATE REFERENCE
    //---------------------------------
    *pTex = Luminance::g_pTexLuminance[ 0 ];

    // [ 2 ] INCREMENT THE REFERENCE COUNT..
    //--------------------------------------
    ( *pTex )->AddRef();

    return 0;

}



//--------------------------------------------------------------------------------------
//  ComputeResourceUsage( )
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
__DWORD__ ComputeResourceUsage()
{

    __DWORD__ usage = 0;

    for( __DWORD__ i = 0; i < Luminance::g_dwLumTextures; i++ )
    {

        D3DSURFACE_DESC d;
        Luminance::g_pTexLuminance[ i ]->GetLevelDesc( 0, &d );

        usage += ( ( d.Width * d.Height ) * ( Luminance::g_fmtHDR == FORMAT_RG32f ? 8 : 4 ) );

    }

    return usage;

}



//--------------------------------------------------------------------------------------
//  RenderToTexture( )
//
//      DESC:
//          A simple utility function that draws, as a TL Quad, one texture to another
//          such that a pixel shader (configured before this function is called) can
//          operate on the texture. Used by MeasureLuminance() to perform the
//          downsampling and filtering.
//
//      PARAMS:
//          IRenderer::GetRendererInstance() : The currently active device
//
//      NOTES:
//          n/a
//
//--------------------------------------------------------------------------------------
int RenderToTexture()
{

    D3DSURFACE_DESC desc;
    SurfaceID pSurfRT;

    IRenderer::GetRendererInstance()->GetRenderTarget( 0, &pSurfRT );
    pSurfRT->GetDesc( &desc );
    pSurfRT->Release();

    // To correctly map from texels->pixels we offset the coordinates
    // by -0.5f:
    float fWidth = static_cast< float >( desc.Width ) - 0.5f;
    float fHeight = static_cast< float >( desc.Height ) - 0.5f;

    // Now we can actually assemble the screen-space geometry
    Luminance::TLVertex v[4];

    v[0].p = D3DXFROMWINEVECTOR4( -0.5f, -0.5f, 0.0f, 1.0f );
    v[0].t = D3DXFROMWINEVECTOR2( 0.0f, 0.0f );

    v[1].p = D3DXFROMWINEVECTOR4( fWidth, -0.5f, 0.0f, 1.0f );
    v[1].t = D3DXFROMWINEVECTOR2( 1.0f, 0.0f );

    v[2].p = D3DXFROMWINEVECTOR4( -0.5f, fHeight, 0.0f, 1.0f );
    v[2].t = D3DXFROMWINEVECTOR2( 0.0f, 1.0f );

    v[3].p = D3DXFROMWINEVECTOR4( fWidth, fHeight, 0.0f, 1.0f );
    v[3].t = D3DXFROMWINEVECTOR2( 1.0f, 1.0f );

    // Configure the device and render..
    IRenderer::GetRendererInstance()->SetVertexShader( 0 );
    IRenderer::GetRendererInstance()->SetFVF( Luminance::FVF_TLVERTEX );
    IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, v, sizeof( Luminance::TLVertex ) );

    return 0;

}

}
;

