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

#pragma warning(disable:4201) //avoid warning from mmsystem.h "warning C4201: nonstandard extension used : nameless struct/union"

#include "Fluid.h"
#include "FluidCommon.fxh"
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <Strsafe.h>
#include "Common.h"
using namespace std;

float g_zVelocityScale = 4.0;
float g_xyVelocityScale = 4.0;

#define PI 3.14159265

//---------------------------------------------------------------------------
//Fluid
//---------------------------------------------------------------------------

Fluid::Fluid() : m_pGrid(0), IRenderer::GetRendererInstance()(0), m_pEffect(0),
    m_etAdvect(0), m_etAdvectMACCORMACK(0), m_etVorticity(0), m_etConfinement(0), m_etDiffuse(0),
    m_etGaussian(0), m_etDivergence(0), m_etScalarJacobi(0), m_etProject(0), 
    m_etExtrapolateVelocity(0), m_etRedistance(0),
    m_etDrawTexture(0), m_etStaticObstacleTriangles(0), m_etStaticObstacleLines(0),
    m_etDrawBox(0), m_etCopyTextureDensity(0), m_etAddDensityDerivativeVelocity(0), 
    m_etInitLevelSetToLiquidHeight(0), m_etInjectLiquid(0), m_etLiquidStream_LevelSet(0),
    m_etLiquidStream_Velocity(0), m_etGravity(0), m_etAirPressure(0),
    m_evTextureWidth(0), m_evTextureHeight(0), m_evTextureDepth(0), m_evDrawTexture(0), 
    m_evDecay(0), m_evViscosity(0), m_evRadius(0), m_evCenter(0), m_evColor(0), m_evGravity(0),
    m_evVortConfinementScale(0), m_evTimeStep(0), m_evAdvectAsTemperature(0), m_evTreatAsLiquidVelocity(0),
    m_evFluidType(0), m_evObstBoxLBDcorner(0), m_evObstBoxRTUcorner(0), m_evObstBoxVelocity(0),
    m_evTexture_pressure(0), m_evTexture_velocity(0), m_evTexture_vorticity(0), 
    m_evTexture_divergence(0), m_evTexture_phi(0), m_evTexture_phi_hat(0), m_evTexture_phi_next(0),
    m_evTexture_levelset(0), m_evTexture_obstacles(0), m_evTexture_obstvelocity(0),
    m_currentDstScalar(RENDER_TARGET_SCALAR0), m_currentSrcScalar(RENDER_TARGET_SCALAR1), m_eFluidType(FT_SMOKE),
    m_bMouseDown(false), m_bGravity(false), m_bTreatAsLiquidVelocity(true), m_bUseMACCORMACK(false), 
    m_nJacobiIterations(6), m_nDiffusionIterations(2), m_nVelExtrapolationIterations(6), m_nRedistancingIterations(2),
    m_fDensityDecay(1.0f), m_fDensityViscosity(0.0f), m_fFluidViscosity(0.0f), m_fVorticityConfinementScale(0.0f), m_t(0), m_fSaturation(0.0f), m_fImpulseSize(0.0f),
    m_vImpulsePos(0,0,0,0), m_vImpulseDir(0,0,0,0), m_bLiquidStream(false), m_fStreamSize(0.0f), m_streamCenter(0,0,0,0),
    m_bClosedBoundaries(false), m_bDrawObstacleBox(false), m_vObstBoxPos(0,0,0,0), m_vObstBoxPrevPos(0,0,0,0), m_vObstBoxVelocity(0,0,0,0)

{
    IRenderer::GetRendererInstance() = 0;
    IRenderer::GetRendererInstance() = 0;

    SAFE_ACQUIRE();
    SAFE_ACQUIRE();

#if 1
    m_RenderTargetFormats [RENDER_TARGET_VELOCITY0]   = FORMAT_RGBA16F;
    m_RenderTargetFormats [RENDER_TARGET_VELOCITY1]   = FORMAT_RGBA16F;
    m_RenderTargetFormats [RENDER_TARGET_PRESSURE]    = FORMAT_R16_FLOAT;
    m_RenderTargetFormats [RENDER_TARGET_SCALAR0]     = FORMAT_R16_FLOAT;
    m_RenderTargetFormats [RENDER_TARGET_SCALAR1]     = FORMAT_R16_FLOAT;
    m_RenderTargetFormats [RENDER_TARGET_OBSTACLES]   = FORMAT_R8_UNORM;
    m_RenderTargetFormats [RENDER_TARGET_OBSTVELOCITY]= FORMAT_RGBA16F;
    // RENDER_TARGET_TEMPSCALAR: for AdvectMACCORMACK and for Jacobi (for pressure solver)
    m_RenderTargetFormats [RENDER_TARGET_TEMPSCALAR]  = FORMAT_R16_FLOAT;
    // RENDER_TARGET_TEMPVECTOR: for Advect (when using MACCORMACK), Vorticity and Divergence (for Pressure solver)
    m_RenderTargetFormats [RENDER_TARGET_TEMPVECTOR]  = FORMAT_RGBA16F;
    m_RenderTargetFormats [RENDER_TARGET_TEMPVECTOR1] = FORMAT_RGBA16F;
#else
    m_RenderTargetFormats [RENDER_TARGET_VELOCITY0]   = FORMAT_RGBA32F;
    m_RenderTargetFormats [RENDER_TARGET_VELOCITY1]   = FORMAT_RGBA32F;
    m_RenderTargetFormats [RENDER_TARGET_PRESSURE]    = FORMAT_R32_FLOAT;
    m_RenderTargetFormats [RENDER_TARGET_SCALAR0]     = FORMAT_R32_FLOAT;
    m_RenderTargetFormats [RENDER_TARGET_SCALAR1]     = FORMAT_R32_FLOAT;
    m_RenderTargetFormats [RENDER_TARGET_OBSTACLES]   = FORMAT_R8_UNORM;
    m_RenderTargetFormats [RENDER_TARGET_OBSTVELOCITY]= FORMAT_RGBA32F;
    // RENDER_TARGET_TEMPSCALAR: for AdvectMACCORMACK and for Jacobi (for pressure solver)
    m_RenderTargetFormats [RENDER_TARGET_TEMPSCALAR]  = FORMAT_R32_FLOAT;
    // RENDER_TARGET_TEMPVECTOR: for Advect (when using MACCORMACK), Vorticity and Divergence (for Pressure solver)
    m_RenderTargetFormats [RENDER_TARGET_TEMPVECTOR]  = FORMAT_RGBA32F;
    m_RenderTargetFormats [RENDER_TARGET_TEMPVECTOR1] = FORMAT_RGBA32F;
#endif

    stx_memset(m_p3DTextures, 0, sizeof(m_p3DTextures));
    stx_memset(m_pShaderResourceViews, 0, sizeof(m_pShaderResourceViews));
    stx_memset(m_pRenderTargetViews, 0, sizeof(m_pRenderTargetViews));

    m_t=0;
}


int Fluid::Initialize( int width, int height, int depth, FLUID_TYPE fluidType )
{
    int hr(0);

    m_eFluidType = fluidType;

    (LoadShaders());

    for(int rtIndex=0; rtIndex<NUM_RENDER_TARGETS; rtIndex++)
    {
       desc.Format = m_RenderTargetFormats[rtIndex];
       (CreateTextureAndViews( rtIndex, width, height, depth ));
    }

    (m_evTextureWidth->SetFloat( float(width)));
    (m_evTextureHeight->SetFloat(float(height)));
    (m_evTextureDepth->SetFloat(float(depth)));

    try
    {
        m_pGrid = new Grid();
    }
    catch(...)
    {
        SAFE_DELETE(m_pGrid);
        return E_OUTOFMEMORY;
    }


    (m_pGrid->Initialize( width, height, depth, m_etAdvect ));

    Reset();

    // Simulation variables
    m_bMouseDown = false;
    m_bGravity = true;
    m_bTreatAsLiquidVelocity = true;
    m_bUseMACCORMACK = true;
    m_nJacobiIterations = 6;
    m_nDiffusionIterations = 2;
    m_nVelExtrapolationIterations = 10;
    m_nRedistancingIterations = 11;
    m_fDensityDecay = 1.0f;
    m_fDensityViscosity = 0.0f;
    m_fFluidViscosity = 0.0f;
    m_fVorticityConfinementScale = 0.0f;
    m_t = 0;

    m_fSaturation =  0.78f;
    m_fImpulseSize = 0.0f;
    m_vImpulsePos.x = (float)(m_pGrid->GetDimX() / 2);
    m_vImpulsePos.y = 0;
    m_vImpulsePos.z = (float)(m_pGrid->GetDimZ() / 2);
    m_vImpulsePos.w = 1;
    m_vImpulseDir.x = 0;
    m_vImpulseDir.y = 1;
    m_vImpulseDir.z = 0;
    m_vImpulseDir.w = 1;

    m_bLiquidStream = true;

    m_bClosedBoundaries = false;

    m_bDrawObstacleBox = false;
    m_vObstBoxPos = D3DXFROMWINEVECTOR4(0, 0, 0, 0);
    m_vObstBoxPrevPos = D3DXFROMWINEVECTOR4(0, 0, 0, 0);
    m_vObstBoxVelocity = D3DXFROMWINEVECTOR4(0, 0, 0, 0);


    if( m_eFluidType == FT_LIQUID )
    {
        m_fImpulseSize = 0.20f;

        m_fStreamSize = 3.5;
        m_streamCenter = D3DXFROMWINEVECTOR4(float(m_pGrid->GetDimX()/2 - 3)  , float(m_pGrid->GetDimY()/2 - 20), float(m_pGrid->GetDimZ()/2 + 12), 1.0f);

        // Used closed boundaries for water
        m_bClosedBoundaries = true;
#if 0
        // FOR TESTING PURPOSES
        // disable velocity advection only within the levelset (advect everywhere)
        m_bTreatAsLiquidVelocity = false;
        // disable velicity extrapolation out of the level set
        m_nVelExtrapolationIterations = 2;
#endif
    }
    else
    {
        m_fImpulseSize = 0.15f;

        m_bGravity = false;

        m_bLiquidStream = false;
    }


    return 0;
}

Fluid::~Fluid( void )
{
    SAFE_DELETE(m_pGrid);
    //////////////////SAFE_RELEASE();
    //////////////////SAFE_RELEASE();
    //////////////////SAFE_RELEASE(m_pEffect);

    for(int i=0;i<NUM_RENDER_TARGETS;i++)
    {
        //////////////////SAFE_RELEASE(m_p3DTextures[i]);
        //////////////////SAFE_RELEASE(m_pShaderResourceViews[i]);
        //////////////////SAFE_RELEASE(m_pRenderTargetViews[i]);
    }
}

void Fluid::SetObstaclePositionInNormalizedGrid( float x, float y, float z )
{
    m_vObstBoxPos = D3DXFROMWINEVECTOR4(x, y, z, 1);
    D3DXFROMWINEVECTOR3 pos = D3DXFROMWINEVECTOR3(x, y, z);

    D3DXFROMWINEVECTOR3 boxHDims(0.2f * 0.5f, 0.3f * 0.5f, 0.3f * 0.5f);
    D3DXFROMWINEVECTOR3 boxLBDcorner = (pos - boxHDims);
    boxLBDcorner.x *= m_pGrid->GetDimX(); boxLBDcorner.y *= m_pGrid->GetDimY(); boxLBDcorner.z *= m_pGrid->GetDimZ();
    D3DXFROMWINEVECTOR3 boxRTUcorner = (pos + boxHDims);
    boxRTUcorner.x *= m_pGrid->GetDimX(); boxRTUcorner.y *= m_pGrid->GetDimY(); boxRTUcorner.z *= m_pGrid->GetDimZ();

    m_evObstBoxLBDcorner->SetFloatVector(boxLBDcorner);
    m_evObstBoxRTUcorner->SetFloatVector(boxRTUcorner);

    m_bDrawObstacleBox = true;
}

void Fluid::Update( float timestep, bool bUseMACCORMACKVelocity, bool bUseSuppliedParameters, float fInputVorticityConfinementScale, float fInputDensityDecay, float fInputDensityViscosity, float fInputFluidViscosity, float fInputImpulseSize, float InputRandCenterScale)
{
    // All drawing will take place to a viewport with the dimensions of a 3D texture slice
    D3DFROMWINEVIEWPORT9 rtViewport;
    rtViewport.TopLeftX = 0;
    rtViewport.TopLeftY = 0;
    rtViewport.MinDepth = 0;
    rtViewport.MaxDepth = 1;
    rtViewport.Width =  (float)GetTextureWidth();  
    rtViewport.Height = (float)GetTextureHeight(); 
    IRenderer::GetRendererInstance()->RSSetViewports(1,&rtViewport);

    
    // If the mouse is not being used, set fixed emitter position and direction
    if( !m_bMouseDown )
    {
        // If there is no user interaction with the mouse
        //  emit with some initial velocity from a fixed location in the volume
        // the emitter location
        m_vImpulsePos.x = float(m_pGrid->GetDimX()) * 0.1f;
        m_vImpulsePos.y = float(m_pGrid->GetDimY()) * 0.9f;
        m_vImpulsePos.z = float(m_pGrid->GetDimZ()) * 0.25f;
        // the emitter direction and initial velocity
        float impulseStrength = 0.8f;
        m_vImpulseDir.x = 0.5f * impulseStrength;
        m_vImpulseDir.y = -0.5f * impulseStrength;
        m_vImpulseDir.z = 0.8f * impulseStrength;
    }

    
    // Hard-coded obstacle box used for testing purposes:
    {
        // Update the obstacle box velocity based on its movement
        {
            m_vObstBoxVelocity = (m_vObstBoxPos - m_vObstBoxPrevPos) / timestep;
            // Exagerate the velocity a bit to give more momentum to the fluid
            m_vObstBoxVelocity *= 1.5f;
            // Scale m_vObstBoxVelocity to voxel space
            m_vObstBoxVelocity.x *= m_pGrid->GetDimX(); m_vObstBoxVelocity.y *= m_pGrid->GetDimY(); m_vObstBoxVelocity.z *= m_pGrid->GetDimZ();
            m_evObstBoxVelocity->SetFloatVector(m_vObstBoxVelocity);
            m_vObstBoxPrevPos = m_vObstBoxPos;
        }
    
        if( m_bDrawObstacleBox )
        {
            DrawObstacleTestBox( RENDER_TARGET_OBSTACLES, RENDER_TARGET_OBSTVELOCITY );
        }
    }

    if( m_bClosedBoundaries )
    {
        DrawObstacleBoundaryBox( RENDER_TARGET_OBSTACLES, RENDER_TARGET_OBSTVELOCITY );
    }

    
    
    // Set vorticity confinment and decay parameters
    // (m_bUserMACCORMACK may change dynamically based on user input, so we set these variables at every frame)
    if( bUseSuppliedParameters)
	{
	   m_fVorticityConfinementScale = fInputVorticityConfinementScale;
       m_fDensityDecay = fInputDensityDecay;
       m_fDensityViscosity = fInputDensityViscosity;
       m_fFluidViscosity =  fInputFluidViscosity;
	}
	else if( m_bUseMACCORMACK )
    {
        if(m_eFluidType == FT_FIRE)
        {
            m_fVorticityConfinementScale = 0.03f;
            m_fDensityDecay = 0.9995f;
        }
        else
        {
            //m_fVorticityConfinementScale = 0.02f;
            //m_fDensityDecay = 0.994f;
            m_fVorticityConfinementScale = 0.0085f;
            m_fDensityDecay = 1.0f;
        }

        // use Diffusion
        m_fDensityViscosity = 0.0003f;
        m_fFluidViscosity =  0.000025f;
    }
    else
    {
       m_fVorticityConfinementScale = 0.12f;
       m_fDensityDecay = 0.9999f;
       // no Diffusion
       m_fDensityViscosity = 0.0f;
       m_fFluidViscosity =  0.0f;
    }

    if( m_eFluidType == FT_LIQUID )
    {
        // Use no decay for Level Set advection (liquids)
        m_fDensityDecay = 1.0f;
        // no diffusion applied to level set advection
        m_fDensityViscosity = 0.0f;
    }

    // we want the rate of decay to be the same with any timestep (since we tweaked with timestep = 2.0 we use 2.0 here)
    double dDensityDecay = pow((double)m_fDensityDecay, (double)(timestep/2.0));
    float fDensityDecay = (float) dDensityDecay;


    m_evFluidType->SetInt( m_eFluidType );

    static RENDER_TARGET currentSrcVelocity = RENDER_TARGET_VELOCITY0;
    static RENDER_TARGET currentDstVelocity = RENDER_TARGET_VELOCITY1;

    // Advection of Density or Level set
    bool bAdvectAsTemperature = (m_eFluidType == FT_FIRE);
    if( m_bUseMACCORMACK )
    {
        // Advect forward to get \phi^(n+1)
        Advect(  timestep, 1.0f, false, bAdvectAsTemperature, RENDER_TARGET_TEMPVECTOR, currentSrcVelocity, m_currentSrcScalar, RENDER_TARGET_OBSTACLES, m_currentSrcScalar );
        // Advect back to get \bar{\phi}
        Advect( -timestep, 1.0f, false, bAdvectAsTemperature, RENDER_TARGET_TEMPSCALAR, currentSrcVelocity, RENDER_TARGET_TEMPVECTOR, RENDER_TARGET_OBSTACLES, m_currentSrcScalar );
        // Advect forward but use the MACCORMACK advection shader which uses both \phi and \bar{\phi} 
        //  as source quantity  (specifically, (3/2)\phi^n - (1/2)\bar{\phi})
        AdvectMACCORMACK( timestep, fDensityDecay, false, bAdvectAsTemperature, m_currentDstScalar, currentSrcVelocity, m_currentSrcScalar, RENDER_TARGET_TEMPSCALAR, RENDER_TARGET_OBSTACLES, m_currentSrcScalar );
    }
    else
    {
        Advect( timestep, fDensityDecay, false, bAdvectAsTemperature, m_currentDstScalar, currentSrcVelocity, m_currentSrcScalar, RENDER_TARGET_OBSTACLES, m_currentSrcScalar );
    }

    // Advection of Velocity 
    bool bAdvectAsLiquidVelocity = (m_eFluidType == FT_LIQUID) && m_bTreatAsLiquidVelocity;


    if( m_bUseMACCORMACK && bUseMACCORMACKVelocity)
    {
        // Advect forward to get \phi^(n+1)
        Advect(  timestep, 1.0f, bAdvectAsLiquidVelocity, false, RENDER_TARGET_TEMPVECTOR, currentSrcVelocity, currentSrcVelocity, RENDER_TARGET_OBSTACLES, m_currentDstScalar );

        // Advect back to get \bar{\phi}
        Advect( -timestep, 1.0f, bAdvectAsLiquidVelocity, false, RENDER_TARGET_TEMPVECTOR1, currentSrcVelocity, RENDER_TARGET_TEMPVECTOR, RENDER_TARGET_OBSTACLES, m_currentDstScalar );
        
        // Advect forward but use the MACCORMACK advection shader which uses both \phi and \bar{\phi} 
        //  as source quantity  (specifically, (3/2)\phi^n - (1/2)\bar{\phi})
        AdvectMACCORMACK( timestep, 1.0f, bAdvectAsLiquidVelocity, false, currentDstVelocity, currentSrcVelocity, currentSrcVelocity, RENDER_TARGET_TEMPVECTOR1, RENDER_TARGET_OBSTACLES, m_currentDstScalar );
    }
    else
    {
        Advect( timestep, 1.0f, bAdvectAsLiquidVelocity, false, currentDstVelocity, currentSrcVelocity, currentSrcVelocity, RENDER_TARGET_OBSTACLES, m_currentDstScalar );
    }

    // Diffusion of Density for smoke
    if( (m_fDensityViscosity > 0.0f) && (m_eFluidType == FT_SMOKE) )
    {
        swap(m_currentDstScalar, m_currentSrcScalar);
        Diffuse( timestep, m_fDensityViscosity, m_currentDstScalar, m_currentSrcScalar, RENDER_TARGET_TEMPSCALAR, RENDER_TARGET_OBSTACLES );
    }
    // Diffusion of Velocity field
    if( m_fFluidViscosity > 0.0f )
    {
        swap(currentDstVelocity, currentSrcVelocity);
        Diffuse( timestep, m_fFluidViscosity, currentDstVelocity, currentSrcVelocity, RENDER_TARGET_TEMPVECTOR, RENDER_TARGET_OBSTACLES );
    }
    

    // Vorticity confinement
    if( m_fVorticityConfinementScale > 0.0f )
    {
        ComputeVorticity( RENDER_TARGET_TEMPVECTOR, currentDstVelocity);
        ApplyVorticityConfinement( timestep, currentDstVelocity, RENDER_TARGET_TEMPVECTOR, m_currentDstScalar, RENDER_TARGET_OBSTACLES);
    }

    // TODO: this is not really working very well, some flickering occurrs for timesteps less than 2.0
    // Addition of external density/liquid and external forces
    static float elapsedTimeSinceLastUpdate = 2.0f;
    if( elapsedTimeSinceLastUpdate >= 2.0f )
    {
        AddNewMatter( timestep, m_currentDstScalar, RENDER_TARGET_OBSTACLES );   
        if(bUseSuppliedParameters)
			AddExternalForces( timestep, currentDstVelocity, RENDER_TARGET_OBSTACLES, m_currentDstScalar,fInputImpulseSize, InputRandCenterScale );
		else
			AddExternalForces( timestep, currentDstVelocity, RENDER_TARGET_OBSTACLES, m_currentDstScalar,m_fImpulseSize, 1 );

        elapsedTimeSinceLastUpdate = 0.0f;
    }
    elapsedTimeSinceLastUpdate += timestep;

    // Pressure projection
    ComputeVelocityDivergence( RENDER_TARGET_TEMPVECTOR, currentDstVelocity, RENDER_TARGET_OBSTACLES, RENDER_TARGET_OBSTVELOCITY);
    ComputePressure( RENDER_TARGET_PRESSURE, RENDER_TARGET_TEMPSCALAR, RENDER_TARGET_TEMPVECTOR, RENDER_TARGET_OBSTACLES, m_currentDstScalar);
    ProjectVelocity( currentSrcVelocity, RENDER_TARGET_PRESSURE, currentDstVelocity, RENDER_TARGET_OBSTACLES, RENDER_TARGET_OBSTVELOCITY, m_currentDstScalar);

#if 1
    if( m_eFluidType == FT_LIQUID )
    {
        // Redistancing of level set (only for liquids)
        RedistanceLevelSet(m_currentSrcScalar, m_currentDstScalar, RENDER_TARGET_TEMPSCALAR);
        // Velocity extrapolation: from the surface along the level set gradient 
        ExtrapolateVelocity(currentSrcVelocity, RENDER_TARGET_TEMPVECTOR, RENDER_TARGET_OBSTACLES, m_currentSrcScalar);
    }
    else
#endif
    {
        // Swap scalar textures (we ping-pong between them for advection purposes)
        swap(m_currentDstScalar, m_currentSrcScalar);
    }

}

int Fluid::Draw( int field )
{
    int hr(0);
    D3DFROMWINEVIEWPORT9 rtViewport;
    rtViewport.TopLeftX = 0;
    rtViewport.TopLeftY = 0;
    rtViewport.MinDepth = 0;
    rtViewport.MaxDepth = 1;
    rtViewport.Width = (float)g_Width;
    rtViewport.Height = (float)g_Height;
    IRenderer::GetRendererInstance()->RSSetViewports(1,&rtViewport);

    TextureID  pRTV = DXUTGetD3D11RenderTargetView();
    IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &pRTV , 0 ); 

    m_evDrawTexture->SetInt( field );

    // Set resources and apply technique
    m_evTexture_phi->SetResource(m_pShaderResourceViews[m_currentDstScalar]);
    m_evTexture_velocity->SetResource(m_pShaderResourceViews[RENDER_TARGET_VELOCITY0]);
    m_evTexture_obstacles->SetResource(m_pShaderResourceViews[RENDER_TARGET_OBSTACLES]);
    m_evTexture_obstvelocity->SetResource(m_pShaderResourceViews[RENDER_TARGET_OBSTVELOCITY]);
    m_evTexture_pressure->SetResource(m_pShaderResourceViews[RENDER_TARGET_PRESSURE]);
    (m_etDrawTexture->GetPassByIndex(0)->Apply(0));

    m_pGrid->DrawSlicesToScreen();

    // Unset resources and apply technique (so that the resource is actually unbound)
    m_evTexture_phi->SetResource(0);
    m_evTexture_velocity->SetResource(0);
    m_evTexture_obstacles->SetResource(0);
    m_evTexture_obstvelocity->SetResource(0);
    m_evTexture_pressure->SetResource(0);
    (m_etDrawTexture->GetPassByIndex(0)->Apply(0));
 
    return hr; 
}

TextureID  Fluid::getCurrentShaderResourceView()
{
    return m_pShaderResourceViews[m_currentSrcScalar]; 
}

void Fluid::Impulse( int x, int y, int z, float dX, float dY, float dZ )
{
    m_vImpulsePos.x  = (float)x;
    m_vImpulsePos.y  = (float)y;
    m_vImpulsePos.z  = (float)z;
    m_vImpulseDir.x = (float)dX;
    m_vImpulseDir.y = (float)dY;
    m_vImpulseDir.z = (float)dZ;

}

void Fluid::Reset( void )
{
    float zero[4] = {0, 0, 0, 0 };
    float obstacle_exterior[4] = { OBSTACLE_EXTERIOR, OBSTACLE_EXTERIOR, OBSTACLE_EXTERIOR, OBSTACLE_EXTERIOR };
    m_currentDstScalar = RENDER_TARGET_SCALAR0;
    m_currentSrcScalar = RENDER_TARGET_SCALAR1;
    m_t=0;
    IRenderer::GetRendererInstance()->ClearRenderTargetView( m_pRenderTargetViews[RENDER_TARGET_VELOCITY0], zero );
    IRenderer::GetRendererInstance()->ClearRenderTargetView( m_pRenderTargetViews[RENDER_TARGET_VELOCITY1], zero );
    IRenderer::GetRendererInstance()->ClearRenderTargetView( m_pRenderTargetViews[RENDER_TARGET_PRESSURE], zero );
    IRenderer::GetRendererInstance()->ClearRenderTargetView( m_pRenderTargetViews[RENDER_TARGET_SCALAR0], zero );
    IRenderer::GetRendererInstance()->ClearRenderTargetView( m_pRenderTargetViews[RENDER_TARGET_SCALAR1], zero );
    IRenderer::GetRendererInstance()->ClearRenderTargetView( m_pRenderTargetViews[RENDER_TARGET_OBSTACLES], obstacle_exterior );
    IRenderer::GetRendererInstance()->ClearRenderTargetView( m_pRenderTargetViews[RENDER_TARGET_OBSTVELOCITY], zero );
    IRenderer::GetRendererInstance()->ClearRenderTargetView( m_pRenderTargetViews[RENDER_TARGET_TEMPSCALAR], zero );
    IRenderer::GetRendererInstance()->ClearRenderTargetView( m_pRenderTargetViews[RENDER_TARGET_TEMPVECTOR], zero );


    if( m_eFluidType == FT_LIQUID )
    {
        // All drawing will take place to a viewport with the dimensions of a 3D texture slice
        D3DFROMWINEVIEWPORT9 rtViewport;
        rtViewport.TopLeftX = 0;
        rtViewport.TopLeftY = 0;
        rtViewport.MinDepth = 0;
        rtViewport.MaxDepth = 1;
        rtViewport.Width =  (float)GetTextureWidth();  
        rtViewport.Height = (float)GetTextureHeight(); 
        IRenderer::GetRendererInstance()->RSSetViewports(1,&rtViewport);


        SetRenderTarget( RENDER_TARGET_SCALAR0 );
        m_etInitLevelSetToLiquidHeight->GetPassByIndex(0)->Apply(0);
        m_pGrid->DrawSlices();
        IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
        
        SetRenderTarget( RENDER_TARGET_SCALAR1 );
        m_etInitLevelSetToLiquidHeight->GetPassByIndex(0)->Apply(0);
        m_pGrid->DrawSlices();
        IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
    }

}

int Fluid::GetTextureWidth( void )
{
    return m_pGrid->GetDimX();
}

int Fluid::GetTextureHeight( void )
{
    return m_pGrid->GetDimY();
}

void Fluid::DrawObstacleTestBox( RENDER_TARGET dstObst, RENDER_TARGET dstObstVel )
{
    // Draw a box into the obstacles 3D textures.
    // In RENDER_TARGET_OBSTACLES, cells inside the obstacle will be grey 
    //   (0.5 at the boundary and 1.0 if within boundary inside), and cells outside will be black
    // In RENDER_TARGET_OBSTVELOCITY, cell at the boundary will have a defined velocity,
    //  while cells inside or outside will have undefined velocity (likely to be set to 0)

    TextureID pObstRenderTargets[2] = {
        m_pRenderTargetViews[dstObst],
        m_pRenderTargetViews[dstObstVel]
    };
    IRenderer::GetRendererInstance()->OMSetRenderTargets( 2, pObstRenderTargets, 0 ); 

    m_etDrawBox->GetPassByIndex(0)->Apply(0);
    m_pGrid->DrawSlices();

    IRenderer::GetRendererInstance()->OMSetRenderTargets( 0, 0, 0 );
}

void Fluid::DrawObstacleBoundaryBox( RENDER_TARGET dstObst, RENDER_TARGET dstObstVel )
{
    // Draw the boundary walls of the box into the obstacles 3D textures.
    // In RENDER_TARGET_OBSTACLES, cells inside the obstacle will be grey 
    //   (0.5 at the boundary and 1.0 if within boundary inside), and cells outside will be black
    // In RENDER_TARGET_OBSTVELOCITY, cell at the boundary will have a defined velocity,
    //  while cells inside or outside will have undefined velocity (likely to be set to 0)

    TextureID pObstRenderTargets[2] = {
        m_pRenderTargetViews[dstObst],
        m_pRenderTargetViews[dstObstVel]
    };
    IRenderer::GetRendererInstance()->OMSetRenderTargets( 2, pObstRenderTargets, 0 ); 
    
    m_etStaticObstacleTriangles->GetPassByIndex(0)->Apply(0);
    m_pGrid->DrawBoundaryQuads();
    
    m_etStaticObstacleLines->GetPassByIndex(0)->Apply(0);
    m_pGrid->DrawBoundaryLines();

    IRenderer::GetRendererInstance()->OMSetRenderTargets( 0, 0, 0 );
}

void Fluid::Advect(  float timestep, float decay, bool bAsLiquidVelocity, bool bAsTemperature, RENDER_TARGET dstPhi, RENDER_TARGET srcVel, RENDER_TARGET srcPhi, RENDER_TARGET srcObst, RENDER_TARGET srcLevelSet)
{
    m_evAdvectAsTemperature->SetBool(bAsTemperature);
    m_evTimeStep->SetFloat(timestep);
    m_evDecay->SetFloat(decay);
    m_evTexture_velocity->SetResource( m_pShaderResourceViews[srcVel] );
    m_evTexture_phi->SetResource( m_pShaderResourceViews[srcPhi] );
    m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
    m_evTreatAsLiquidVelocity->SetBool( bAsLiquidVelocity );
    m_evTexture_levelset->SetResource( m_pShaderResourceViews[srcLevelSet] );
    m_etAdvect->GetPassByIndex(0)->Apply(0);
    SetRenderTarget( dstPhi );

    m_pGrid->DrawSlices();

    m_evTexture_velocity->SetResource( 0 );
    m_evTexture_phi->SetResource( 0 );
    m_evTexture_obstacles->SetResource( 0 );
    m_evTreatAsLiquidVelocity->SetBool( false );
    m_evTexture_levelset->SetResource( 0 );
    m_etAdvect->GetPassByIndex(0)->Apply(0);
    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
}

void Fluid::AdvectMACCORMACK( float timestep, float decay, bool bAsLiquidVelocity, bool bAsTemperature, RENDER_TARGET dstPhi, RENDER_TARGET srcVel, RENDER_TARGET srcPhi, RENDER_TARGET srcPhi_hat, RENDER_TARGET srcObst, RENDER_TARGET srcLevelSet )
{
    // Advect forward but use the MACCORMACK advection shader which uses both \phi and \bar{\phi} 
    //  as source quantity  (specifically, (3/2)\phi^n - (1/2)\bar{\phi})
    m_evAdvectAsTemperature->SetBool(bAsTemperature);
    m_evTimeStep->SetFloat(timestep);
    m_evDecay->SetFloat(decay);
    m_evTexture_velocity->SetResource( m_pShaderResourceViews[srcVel] );
    m_evTexture_phi->SetResource( m_pShaderResourceViews[srcPhi] );
    m_evTexture_phi_hat->SetResource( m_pShaderResourceViews[srcPhi_hat] );
    m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
    m_evTreatAsLiquidVelocity->SetBool( bAsLiquidVelocity );
    m_evTexture_levelset->SetResource( m_pShaderResourceViews[srcLevelSet] );
    m_etAdvectMACCORMACK->GetPassByIndex(0)->Apply(0);
    SetRenderTarget( dstPhi );

    m_pGrid->DrawSlices();

    m_evTexture_velocity->SetResource( 0 );
    m_evTexture_phi->SetResource( 0 );
    m_evTexture_phi_hat->SetResource( 0 );
    m_evTexture_obstacles->SetResource( 0 );
    m_evTreatAsLiquidVelocity->SetBool( false );
    m_evTexture_levelset->SetResource( 0 );
    m_etAdvectMACCORMACK->GetPassByIndex(0)->Apply(0);
    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
}

void Fluid::Diffuse( float timestep, float viscosity, RENDER_TARGET dstPhi, RENDER_TARGET srcPhi, RENDER_TARGET tmpPhi, RENDER_TARGET srcObst )
{
    // These are bound first, as they stay the same through all iterations
    m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
    m_evTexture_phi->SetResource( m_pShaderResourceViews[srcPhi] );
    m_evViscosity->SetFloat(viscosity);
    m_evTimeStep->SetFloat(timestep);

    // If # of iterations is odd we write first to the desired destination, otherwise to the tmp buffer. The source is the opposite
    RENDER_TARGET dst = (m_nDiffusionIterations & 1) ? dstPhi : tmpPhi;
    RENDER_TARGET src = (m_nDiffusionIterations & 1) ? tmpPhi : dstPhi;
    
    // First pass is a bit different, as we need the srcPhi also in phi_next
    m_evTexture_phi_next->SetResource( m_pShaderResourceViews[srcPhi] );
    m_etDiffuse->GetPassByIndex(0)->Apply(0);
    SetRenderTarget( dst );
    m_pGrid->DrawSlices();
    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);

    // Do more passes if needed
    for( int iteration = 1; iteration < m_nDiffusionIterations; iteration++ )
    {
        swap(dst, src);
        
        m_evTexture_phi_next->SetResource( m_pShaderResourceViews[src] );
        m_etDiffuse->GetPassByIndex(0)->Apply(0);
        SetRenderTarget( dst );

        m_pGrid->DrawSlices();

        m_evTexture_phi_next->SetResource(0);
        m_etDiffuse->GetPassByIndex(0)->Apply(0);
        IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
    }

    m_evTexture_obstacles->SetResource(0);
    m_evTexture_phi->SetResource(0);
    m_evTexture_phi_next->SetResource( 0 );
    m_etDiffuse->GetPassByIndex(0)->Apply(0);
}

void Fluid::RedistanceLevelSet( RENDER_TARGET dstLevelSet, RENDER_TARGET srcLevelSet, RENDER_TARGET tmpLevelSet )
{
    m_evTexture_phi->SetResource( m_pShaderResourceViews[srcLevelSet] );

    RENDER_TARGET src = tmpLevelSet;
    RENDER_TARGET dst = dstLevelSet;

    // To ensure the end result is in dstLevelSet we use an odd number of iterations (override it if it's not odd)
    m_nRedistancingIterations |= 0x1;

    for( int iteration = 0; iteration < m_nRedistancingIterations; iteration++ )
    {
        if( iteration == 0 )
            m_evTexture_phi_next->SetResource( m_pShaderResourceViews[srcLevelSet] );
        else
            m_evTexture_phi_next->SetResource( m_pShaderResourceViews[src] );

        m_etRedistance->GetPassByIndex(0)->Apply(0);
        SetRenderTarget( dst );

        m_pGrid->DrawSlices();

        m_evTexture_phi_next->SetResource(0);
        m_etRedistance->GetPassByIndex(0)->Apply(0);
        IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);

        swap(src, dst);
    }

    m_evTexture_phi->SetResource(0);
    m_evTexture_phi_next->SetResource(0);
    m_etRedistance->GetPassByIndex(0)->Apply(0);
    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
}

void Fluid::ExtrapolateVelocity( RENDER_TARGET dstAndSrcVel, RENDER_TARGET tmpVel, RENDER_TARGET srcObst, RENDER_TARGET srcLeveSet )
{
    m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
    m_evTexture_levelset->SetResource( m_pShaderResourceViews[srcLeveSet] );
    m_evTreatAsLiquidVelocity->SetBool(m_bTreatAsLiquidVelocity);

    RENDER_TARGET src = dstAndSrcVel;
    RENDER_TARGET dst = tmpVel;

    // To ensure the end result is in the original texture
    //  override the number of iterations if it's not even (add one more in that case)
    if( m_nVelExtrapolationIterations & 0x1 )
        m_nVelExtrapolationIterations++;

    for( int iteration = 0; iteration < m_nVelExtrapolationIterations; iteration++ )
    {
        m_evTexture_velocity->SetResource( m_pShaderResourceViews[src] );
        m_etExtrapolateVelocity->GetPassByIndex(0)->Apply(0);
        SetRenderTarget( dst );

        m_pGrid->DrawSlices();

        m_evTexture_velocity->SetResource( 0 );
        m_etExtrapolateVelocity->GetPassByIndex(0)->Apply(0);
        IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);

        swap(src, dst);
    }

    m_evTexture_obstacles->SetResource(0);
    m_evTexture_levelset->SetResource(0);
    m_evTexture_velocity->SetResource(0);
    m_evTreatAsLiquidVelocity->SetBool(false);
    m_etScalarJacobi->GetPassByIndex(0)->Apply(0);
    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
}

void Fluid::ComputeVorticity( RENDER_TARGET dstVorticity, RENDER_TARGET srcVel )
{
    m_evTexture_velocity->SetResource( m_pShaderResourceViews[srcVel] );
    m_etVorticity->GetPassByIndex(0)->Apply(0);
    SetRenderTarget( dstVorticity );

    m_pGrid->DrawSlices(); 

    m_evTexture_velocity->SetResource( 0 );
    m_etVorticity->GetPassByIndex(0)->Apply(0);
    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
}

void Fluid::ApplyVorticityConfinement( float timestep, RENDER_TARGET dstVel, RENDER_TARGET srcVorticity, RENDER_TARGET srcObst, RENDER_TARGET srcLevelSet )
{
    // Compute and apply vorticity confinement force (adding to the current velocity with additive blending)

    m_evVortConfinementScale->SetFloat(m_fVorticityConfinementScale);
    m_evTimeStep->SetFloat(timestep);
    m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
    m_evTexture_vorticity->SetResource( m_pShaderResourceViews[srcVorticity] );
    m_evTexture_levelset->SetResource( m_pShaderResourceViews[srcLevelSet] );
    if( (m_eFluidType == FT_LIQUID) && m_bTreatAsLiquidVelocity)
        m_evTreatAsLiquidVelocity->SetBool(true);
    m_etConfinement->GetPassByIndex(0)->Apply(0);
    SetRenderTarget( dstVel );

    m_pGrid->DrawSlices();

    m_evTexture_obstacles->SetResource( 0);
    m_evTexture_vorticity->SetResource( 0 );
    m_evTexture_levelset->SetResource( 0 );
    m_evTreatAsLiquidVelocity->SetBool( false );
    m_etConfinement->GetPassByIndex(0)->Apply(0);
    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
}


static float lilrand()
{
    return (stx_rand()/float(RAND_MAX) - 0.5f)*5.0f;
}

static float random()
{
    return (float(   (double)stx_rand() / ((double)(RAND_MAX)+(double)(1)) ));
}

void Fluid::AddNewMatter( float timestep, RENDER_TARGET dstPhi, RENDER_TARGET srcObst )
{
    SetRenderTarget( dstPhi );

    //if( !m_bMouseDown )


    switch(m_eFluidType)
    {
        // Add Fire Density
        case FT_FIRE:
        {
            // density of the fire is computed using a sinusoidal function of 'm_t' to make it more interesting.
            FLOAT fireDensity = 3.0f/**timestep*/ * ((((sin(m_t)*0.5f + 0.5f)) * random()*2 )*(1.0f-m_fSaturation) + m_fSaturation);
            D3DXFROMWINEVECTOR4 vDensity(fireDensity, fireDensity, fireDensity, 1.0f);

            m_evColor->SetFloatVector((float*)&vDensity );        
            m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
            m_etCopyTextureDensity->GetPassByIndex(0)->Apply(0);            

            m_pGrid->DrawSlice(3);

            m_evTexture_obstacles->SetResource( 0 );
            m_etCopyTextureDensity->GetPassByIndex(0)->Apply(0);
        }   
        break;

        // Add Smoke Density (rendering guassian balls of smoke)
        case FT_SMOKE:
        {
            // the density of the smoke is computed using a sinusoidal function of 'm_t' to make it more interesting.
            FLOAT density = 1.5f/*timestep*/ * (((sin( m_t + 2.0f*float(PI)/3.0f )*0.5f + 0.5f))*m_fSaturation + (1.0f-m_fSaturation));
            D3DXFROMWINEVECTOR4 smokeDensity(density, density, density, 1.0f);

            m_evRadius->SetFloat(m_fImpulseSize);
            m_evCenter->SetFloatVector((float*)&m_vImpulsePos );        
            m_evColor->SetFloatVector((float*)&smokeDensity );
            m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
            m_etGaussian->GetPassByIndex(0)->Apply(0);

            m_pGrid->DrawSlices();

            m_evTexture_obstacles->SetResource( 0 );
            m_etGaussian->GetPassByIndex(0)->Apply(0);
        }
        break;

        case FT_LIQUID:
        {
            // Hack to counter the lack of convergence in the pressure projection step with Jacobi iterations:
            //  blend between a "known" initial balanced state and the current state
#if 0
            m_etInjectLiquid->GetPassByIndex(0)->Apply(0);
            m_pGrid->DrawSlices();
#endif
            
            // Add a ball of liquid (should appear as a stream)
            if(m_bLiquidStream) {
                m_evRadius->SetFloat(m_fStreamSize);
                m_evCenter->SetFloatVector((float*)&m_streamCenter );
                m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
                m_etLiquidStream_LevelSet->GetPassByIndex(0)->Apply(0);

                m_pGrid->DrawSlices();

                m_evTexture_obstacles->SetResource( 0 );
                m_etLiquidStream_LevelSet->GetPassByIndex(0)->Apply(0);
            }            
        }
        break;

        default:
        assert(0);
        break;
    }

    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
}

void Fluid::AddExternalForces( float timestep, RENDER_TARGET dstVel, RENDER_TARGET srcObst, RENDER_TARGET srcLevelSet, float impulseSize, float randCenterScale)
{
    SetRenderTarget( dstVel );

    switch(m_eFluidType)
    {    
        case FT_FIRE:
        {
            // Add Fire Velocity
            m_t += 0.015f * timestep;

            float var = 0.25f;
            FLOAT fireDensity = 1.5f * ((((sin(m_t)*0.5f + 0.5f)) * random()*2 )*(1.0f-m_fSaturation) + m_fSaturation);
            D3DXFROMWINEVECTOR4 fireVelocity(g_xyVelocityScale*(0.5f + var) + fireDensity,
                              g_xyVelocityScale*(0.5f + var) + fireDensity,
                              g_zVelocityScale *(0.5f + var) + fireDensity, 1.0f);

            m_evColor->SetFloatVector((float*)&fireVelocity);
            m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
            m_etAddDensityDerivativeVelocity->GetPassByIndex(0)->Apply(0);

            m_pGrid->DrawSlice(3);

            m_evTexture_obstacles->SetResource( 0 );
            m_etAddDensityDerivativeVelocity->GetPassByIndex(0)->Apply(0);
        }
        break;
    
        case FT_SMOKE:
        {
            // Add gaussian ball of Velocity
            m_t += 0.025f * timestep;
        
			D3DXFROMWINEVECTOR4 center(m_vImpulsePos.x+lilrand()*randCenterScale, m_vImpulsePos.y+lilrand()*randCenterScale, m_vImpulsePos.z+lilrand()*randCenterScale, 1);

            // m_evColor in this case is the initial velocity given to the emitted smoke
            m_evRadius->SetFloat(impulseSize);
            m_evCenter->SetFloatVector((float*)&center);
            m_evColor->SetFloatVector((float*)&m_vImpulseDir);
            m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
            m_etGaussian->GetPassByIndex(0)->Apply(0);

            m_pGrid->DrawSlices();

            m_evTexture_obstacles->SetResource( 0 );
            m_etGaussian->GetPassByIndex(0)->Apply(0);
        }
        break;

        case FT_LIQUID:
        {
#if 0
            // for boat
            D3DXFROMWINEVECTOR4 posBoat(- 10.0f, m_pGrid->GetDimY()/2.0f + 4.0f, 38.0f, 1.0f );
            D3DXFROMWINEMATRIX rotBoat;

            D3DXFROMWINEMatrixRotationY(&rotBoat, g_fModelRotation);
            D3DXFROMWINEVec3Transform((D3DXFROMWINEVECTOR4*)&posBoat, (D3DXFROMWINEVECTOR3*)&posBoat, &rotBoat);
         
            D3DXFROMWINEVECTOR4 dirBoat(-0.4f * g_fRotSpeed /10.0f - 0.3f, -0.3f * g_fRotSpeed/10.0f, 0.0f, 1.0f);
            D3DXFROMWINEVec3Transform((D3DXFROMWINEVECTOR4*)&dirBoat, (D3DXFROMWINEVECTOR3*)&dirBoat, &rotBoat);

            // for water ski
            D3DXFROMWINEVECTOR4 posWaterSki(-8.0f, m_pGrid->GetDimY()/2.0f + 4.0f, -41.0f, 1.0f);
            D3DXFROMWINEMATRIX rotWaterSki;

            D3DXFROMWINEMatrixRotationY(&rotWaterSki, g_fModelRotation);
            D3DXFROMWINEVec3Transform((D3DXFROMWINEVECTOR4*)&posWaterSki, (D3DXFROMWINEVECTOR3*)&posWaterSki, &rotWaterSki);

            D3DXFROMWINEVECTOR4 dirWaterSki(-0.4f * g_fRotSpeed /10.0f, -0.2f * g_fRotSpeed/10.0f, -0.2f * g_fRotSpeed/10.0f, 1.0f);
            D3DXFROMWINEVec3Transform((D3DXFROMWINEVECTOR4*)&dirWaterSki, (D3DXFROMWINEVECTOR3*)&dirWaterSki, &rotWaterSki);


            // velocity impulse for boat
            D3DXFROMWINEVECTOR3 centerBoat(  posBoat.x+lilrand() + m_pGrid->GetDimX()/2, posBoat.y+lilrand(), posBoat.z+lilrand() + m_pGrid->GetDimZ()/2);

            m_evRadius->SetFloat(m_fImpulseSize);
            m_evCenter->SetFloatVector((float*)&centerBoat);
            m_evColor->SetFloatVector((float*)&dirBoat);
            m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
            m_etGaussian->GetPassByIndex(0)->Apply(0);
            
            m_pGrid->DrawSlices();

            
            // velocity impulse for water ski
            D3DXFROMWINEVECTOR3 centerWaterSki(  posWaterSki.x+lilrand() + m_pGrid->GetDimX()/2, posWaterSki.y+lilrand(), posWaterSki.z+lilrand() + m_pGrid->GetDimZ()/2);

            m_evRadius->SetFloat(m_fImpulseSize*1.2f);
            m_evCenter->SetFloatVector((float*)&centerWaterSki);
            m_evColor->SetFloatVector((float*)&dirWaterSki);
            m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
            m_etGaussian->GetPassByIndex(0)->Apply(0);
            
            m_pGrid->DrawSlices();

            m_evTexture_obstacles->SetResource( 0 );
            m_etGaussian->GetPassByIndex(0)->Apply(0);
#endif
            
            // liquid stream
#if 1
            if(m_bLiquidStream) {
                D3DXFROMWINEVECTOR4 streamVelocity(-0.7f, 1.0f, -2.0f, 1.0f);

                m_evRadius->SetFloat(m_fStreamSize);
                m_evCenter->SetFloatVector((float*)&m_streamCenter);
                m_evColor->SetFloatVector((float*)&streamVelocity);
                m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
                m_etLiquidStream_Velocity->GetPassByIndex(0)->Apply(0);

                m_pGrid->DrawSlices();

                m_evTexture_obstacles->SetResource( 0 );
                m_etLiquidStream_Velocity->GetPassByIndex(0)->Apply(0);
            }
#endif

        }
        break;
        default:
        assert(0);
        break;
    }

    // Apply gravity
    if( m_bGravity )
    {
        D3DXFROMWINEVECTOR3 gravity(0.0f, 0.0f, -0.04f);

        m_evGravity->SetFloatVector((float*)&gravity);
        if( (m_eFluidType == FT_LIQUID) && m_bTreatAsLiquidVelocity)
            m_evTreatAsLiquidVelocity->SetBool( true );
        m_evTexture_levelset->SetResource( m_pShaderResourceViews[srcLevelSet] );
        m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
        m_etGravity->GetPassByIndex(0)->Apply(0);
        
        m_pGrid->DrawSlices();

        m_evTreatAsLiquidVelocity->SetBool( false );
        m_evTexture_levelset->SetResource( 0 );
        m_evTexture_obstacles->SetResource( 0 );
        m_etGravity->GetPassByIndex(0)->Apply(0);
    }

    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
}

void Fluid::ComputeVelocityDivergence( RENDER_TARGET dstDivergence, RENDER_TARGET srcVel, RENDER_TARGET srcObst, RENDER_TARGET srcObstVel )
{
    m_evTexture_velocity->SetResource( m_pShaderResourceViews[srcVel] );
    m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
    m_evTexture_obstvelocity->SetResource( m_pShaderResourceViews[srcObstVel] );    
    m_etDivergence->GetPassByIndex(0)->Apply(0);
    SetRenderTarget( dstDivergence );

    m_pGrid->DrawSlices();

    m_evTexture_velocity->SetResource( 0 );
    m_evTexture_obstacles->SetResource( 0 );
    m_evTexture_obstvelocity->SetResource( 0 );
    m_etDivergence->GetPassByIndex(0)->Apply(0);
    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
}


void Fluid::ComputePressure( RENDER_TARGET dstAndSrcPressure, RENDER_TARGET tmpPressure, RENDER_TARGET srcDivergence, RENDER_TARGET srcObst, RENDER_TARGET srcLevelSet)
{
    m_evTexture_divergence->SetResource( m_pShaderResourceViews[srcDivergence] );
    m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );

    for( int iteration = 0; iteration < m_nJacobiIterations/2.0; iteration++ )
    {
        m_evTexture_pressure->SetResource( m_pShaderResourceViews[dstAndSrcPressure] );
        m_etScalarJacobi->GetPassByIndex(0)->Apply(0);
        SetRenderTarget( tmpPressure );

        m_pGrid->DrawSlices();

        IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);


        m_evTexture_pressure->SetResource( m_pShaderResourceViews[tmpPressure] );
        m_etScalarJacobi->GetPassByIndex(0)->Apply(0);
        SetRenderTarget( dstAndSrcPressure );

        m_pGrid->DrawSlices();
#if 1
        if( (m_eFluidType == FT_LIQUID) && m_bTreatAsLiquidVelocity )
        {
            m_evTreatAsLiquidVelocity->SetBool( true );
            m_evTexture_levelset->SetResource( m_pShaderResourceViews[srcLevelSet] );

            m_etAirPressure->GetPassByIndex(0)->Apply(0);

            m_pGrid->DrawSlices();

            m_evTreatAsLiquidVelocity->SetBool( false );
            m_evTexture_levelset->SetResource( 0 );
            m_evTexture_phi->SetResource( 0 );
            m_etAirPressure->GetPassByIndex(0)->Apply(0);
        }
#endif
        IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
    }

    m_evTexture_pressure->SetResource(0);
    m_evTexture_divergence->SetResource(0);
    m_evTexture_obstacles->SetResource( 0 );
    m_etScalarJacobi->GetPassByIndex(0)->Apply(0);
    IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
}

void Fluid::ProjectVelocity( RENDER_TARGET dstVel, RENDER_TARGET srcPressure, RENDER_TARGET srcVel, RENDER_TARGET srcObst, RENDER_TARGET srcObstVel, RENDER_TARGET srcLevelSet )
{
    m_evTexture_pressure->SetResource( m_pShaderResourceViews[srcPressure] );
    m_evTexture_velocity->SetResource( m_pShaderResourceViews[srcVel] );
    m_evTexture_obstacles->SetResource( m_pShaderResourceViews[srcObst] );
    m_evTexture_obstvelocity->SetResource( m_pShaderResourceViews[srcObstVel] );
    if( (m_eFluidType == FT_LIQUID) && m_bTreatAsLiquidVelocity )
        m_evTreatAsLiquidVelocity->SetBool( true );
    m_evTexture_levelset->SetResource( m_pShaderResourceViews[srcLevelSet] );
    m_etProject->GetPassByIndex(0)->Apply(0);
    SetRenderTarget( dstVel );

    m_pGrid->DrawSlices();

    m_evTexture_pressure->SetResource( 0 );
    m_evTexture_velocity->SetResource( 0 );
    m_evTexture_obstacles->SetResource( 0 );
    m_evTexture_obstvelocity->SetResource( 0 );
    m_evTreatAsLiquidVelocity->SetBool( false );
    m_evTexture_levelset->SetResource( 0 );
    m_etProject->GetPassByIndex(0)->Apply(0);
    IRenderer::GetRendererInstance()->OMSetRenderTargets( 0, 0, 0 );
}

int Fluid::LoadShaders( )
{
    int hr(0);m_pEffect="FluidSim.fx";

    // Get handles to all the techiques
    //=================================

    // core fluid simulation 
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etAdvect, "Advect" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etAdvectMACCORMACK, "AdvectMACCORMACK" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etVorticity, "Vorticity" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etConfinement, "Confinement" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etDiffuse, "Diffuse" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etDivergence, "Divergence" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etScalarJacobi, "ScalarJacobi" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etProject, "Project" );
    
    // additional techniques for liquid simulation
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etInitLevelSetToLiquidHeight, "InitLevelSetToLiquidHeight" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etInjectLiquid, "InjectLiquid" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etAirPressure, "AirPressure" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etRedistance, "Redistance" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etExtrapolateVelocity, "ExtrapolateVelocity" );
    
    //  external forces and matter for liquids
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etLiquidStream_LevelSet, "LiquidStream_LevelSet" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etLiquidStream_Velocity, "LiquidStream_Velocity" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etGravity, "Gravity" );

    // external forces and matter for smoke or fire
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etGaussian, "Gaussian" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etCopyTextureDensity, "CopyTexture" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etAddDensityDerivativeVelocity, "AddDerivativeVelocity" );

    // for procedural obstacle texture modification (to simulate closed boundaries and test boundary conditions)
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etStaticObstacleTriangles, "StaticObstacleTriangles" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etStaticObstacleLines, "StaticObstacleLines" );
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etDrawBox, "DynamicObstacleBox" );

    // to display all the slices spread on the screen
    V_GET_TECHNIQUE_RET( m_pEffect, m_effectPath, m_etDrawTexture, "DrawTexture" );
    
    // Get handle to variables
    //========================
    
    // Grid (3D texture) dimensions
    ( m_pEffect, m_effectPath, m_evTextureWidth, "textureWidth", AsScalar );
    ( m_pEffect, m_effectPath, m_evTextureHeight, "textureHeight", AsScalar );
    ( m_pEffect, m_effectPath, m_evTextureDepth, "textureDepth", AsScalar );

    // For Draw call (to choose which texture is displayed on the screen)
    ( m_pEffect, m_effectPath, m_evDrawTexture, "drawTextureNumber", AsScalar );

    ( m_pEffect, m_effectPath, m_evAdvectAsTemperature, "advectAsTemperature", AsScalar );
    ( m_pEffect, m_effectPath, m_evTreatAsLiquidVelocity, "treatAsLiquidVelocity", AsScalar );
    
    // Simulation parameters: for project, advect, or confinement
    ( m_pEffect, m_effectPath, m_evTimeStep, "timestep", AsScalar );
    ( m_pEffect, m_effectPath, m_evDecay, "decay", AsScalar );
    ( m_pEffect, m_effectPath, m_evViscosity, "viscosity", AsScalar );
    ( m_pEffect, m_effectPath, m_evVortConfinementScale, "vortConfinementScale", AsScalar );
    ( m_pEffect, m_effectPath, m_evGravity, "gravity", AsVector );
    
    // For gaussian or stream (to add density or velocity)
    ( m_pEffect, m_effectPath, m_evRadius, "radius", AsScalar );
    ( m_pEffect, m_effectPath, m_evCenter, "center", AsVector );
    ( m_pEffect, m_effectPath, m_evColor, "color", AsVector );
    ( m_pEffect, m_effectPath, m_evFluidType, "fluidType", AsScalar );

    // For obstacle box
    ( m_pEffect, m_effectPath, m_evObstBoxLBDcorner, "obstBoxLBDcorner", AsVector );
    ( m_pEffect, m_effectPath, m_evObstBoxRTUcorner, "obstBoxRTUcorner", AsVector );
    ( m_pEffect, m_effectPath, m_evObstBoxVelocity, "obstBoxVelocity", AsVector );
    
    // Shader Resource variables
    ( m_pEffect, m_effectPath, m_evTexture_pressure, "Texture_pressure", AsShaderResource );
    ( m_pEffect, m_effectPath, m_evTexture_velocity, "Texture_velocity", AsShaderResource );
    ( m_pEffect, m_effectPath, m_evTexture_vorticity, "Texture_vorticity", AsShaderResource );
    ( m_pEffect, m_effectPath, m_evTexture_divergence, "Texture_divergence", AsShaderResource );
    ( m_pEffect, m_effectPath, m_evTexture_phi, "Texture_phi", AsShaderResource );
    ( m_pEffect, m_effectPath, m_evTexture_phi_hat, "Texture_phi_hat", AsShaderResource );
    ( m_pEffect, m_effectPath, m_evTexture_phi_next, "Texture_phi_next", AsShaderResource );
    ( m_pEffect, m_effectPath, m_evTexture_levelset, "Texture_levelset", AsShaderResource );
    ( m_pEffect, m_effectPath, m_evTexture_obstacles, "Texture_obstacles", AsShaderResource );
    ( m_pEffect, m_effectPath, m_evTexture_obstvelocity, "Texture_obstvelocity", AsShaderResource );


    return 0;
}

int Fluid::GetGridCols()
{
    return m_pGrid->GetCols();
}

int Fluid ::GetGridRows()
{
    return m_pGrid->GetRows();
}

void Fluid::SetRenderTarget(RENDER_TARGET rtIndex, TextureID  optionalDSV )
{
    IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &m_pRenderTargetViews[rtIndex] , optionalDSV ); 
}


int Fluid::CreateTextureAndViews(int rtIndex, int width, int height, int depth)
{

    int hr(0);

    // Release resources in case they exist
    //////////////////SAFE_RELEASE( m_p3DTextures[rtIndex] );
    //////////////////SAFE_RELEASE( m_pRenderTargetViews[rtIndex] );
    //////////////////SAFE_RELEASE( m_pShaderResourceViews[rtIndex] );

    // Create the texture

	Image3 img;
	img.create(FORMAT_RGBA32f, width, height, depth, 1);
	m_p3DTextures[rtIndex]=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());

    return 0;
}

