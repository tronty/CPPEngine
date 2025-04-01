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

#ifndef FLUID3D_FLUID_H
#define FLUID3D_FLUID_H

#pragma warning(disable:4995)
#include <fstream>
#include "Grid.h"

#include <d3dx11effect.h>

class Fluid
{
public:
    enum RENDER_TARGET
    {
        RENDER_TARGET_VELOCITY0,
        RENDER_TARGET_VELOCITY1,
        RENDER_TARGET_PRESSURE,
        RENDER_TARGET_SCALAR0,
        RENDER_TARGET_SCALAR1,
        RENDER_TARGET_OBSTACLES,
        RENDER_TARGET_OBSTVELOCITY,
        RENDER_TARGET_TEMPSCALAR,
        RENDER_TARGET_TEMPVECTOR,
        RENDER_TARGET_TEMPVECTOR1,
        NUM_RENDER_TARGETS
    };

    enum FLUID_TYPE
    {
        FT_SMOKE =0,
        FT_FIRE  =1,
        FT_LIQUID=2
    };

    Fluid                   ();
    virtual ~Fluid          ( void );

    int Initialize      ( int width, int height, int depth, FLUID_TYPE fluidType );

    void Update             ( float timestep, bool bUseMACCORMACKVelocity=true, bool bUseSuppliedParameters = false, float fInputVorticityConfinementScale=0, float fInputDensityDecay=0.9999f, float fInputDensityViscosity=0, float fInputFluidViscosity=0,float fInputImpulseSize=0, float randCenterScale = 1.0);



    void Render3D           ( void );
    int Draw            ( int field );
    void Impulse            ( int x, int y, int z, float dX, float dY, float dZ );
    void Reset              ( void );
    int GetTextureWidth     ( void );
    int GetTextureHeight    ( void );
    int GetGridCols         ( void );
    int GetGridRows         ( void );
    TextureID  Get3DTexture(RENDER_TARGET rt) {return m_p3DTextures[rt];};
    void SetObstaclePositionInNormalizedGrid( float x, float y, float z );

    void SetUseMACCORMACK       (bool b)    { m_bUseMACCORMACK = b; }
    void SetNumJacobiIterations (int i)     { m_nJacobiIterations = i;    }
    void SetMouseDown           (bool b)    { m_bMouseDown = b;     }
    void SetEnableGravity       (bool b)    { m_bGravity = b;       }
    void SetEnableLiquidStream  (bool b)    { m_bLiquidStream = b;   }

    bool GetUseMACCORMACK       ( void )    { return m_bUseMACCORMACK;  }
    int  GetNumJacobiIterations ( void )    { return m_nJacobiIterations;     }
    bool GetMouseDown           ( void )    { return m_bMouseDown;      }
    bool GetEnableGravity       ( void )    { return m_bGravity;        }
    bool GetEnableLiquidStream  ( void )    { return m_bLiquidStream;    }
	int GetDimX                 ( void )    { if(!m_pGrid) return 0; return m_pGrid->GetDimX(); };
	int GetDimY                 ( void )    { if(!m_pGrid) return 0; return m_pGrid->GetDimY(); };
	int GetDimZ                 ( void )    { if(!m_pGrid) return 0; return m_pGrid->GetDimZ(); };

	FORMAT GetFormat       ( RENDER_TARGET rt ) {return m_RenderTargetFormats[rt];};  
	TextureID  getCurrentShaderResourceView();
	void drawGridSlices         ( void )    {m_pGrid->DrawSlices(); };


protected:
    Fluid                         ( void ) {};

    int LoadShaders           ( void );

    void DrawObstacleTestBox      ( RENDER_TARGET dstObst, RENDER_TARGET dstObstVel );
    void DrawObstacleBoundaryBox( RENDER_TARGET dstObst, RENDER_TARGET dstObstVel );
    void AdvectMACCORMACK         ( float timestep, float decay, bool bAsLiquidVelocity, bool bAsTemperature, RENDER_TARGET dstPhi, RENDER_TARGET srcVel, RENDER_TARGET srcPhi, RENDER_TARGET srcPhi_hat, RENDER_TARGET srcObst, RENDER_TARGET srcLevelSet );
    void Advect                   ( float timestep, float decay, bool bAsLiquidVelocity, bool bAsTemperature, RENDER_TARGET dstPhi, RENDER_TARGET srcVel, RENDER_TARGET srcPhi, RENDER_TARGET srcObst, RENDER_TARGET srcLevelSet );
    void Diffuse                  ( float timestep, float viscosity, RENDER_TARGET dstPhi, RENDER_TARGET srcPhi, RENDER_TARGET tmpPhi, RENDER_TARGET srcObst );
    void ComputeVorticity         ( RENDER_TARGET dstVorticity, RENDER_TARGET srcVel );
    void ApplyVorticityConfinement( float timestep, RENDER_TARGET dstVel, RENDER_TARGET srcVorticity, RENDER_TARGET srcObst, RENDER_TARGET srcLevelSet );
    void AddNewMatter             ( float timestep, RENDER_TARGET dstPhi, RENDER_TARGET srcObst );
    void AddExternalForces        ( float timestep, RENDER_TARGET dstVel, RENDER_TARGET srcObst, RENDER_TARGET srcLevelSet, float impulseSize, float randCenterScale );
    void ComputeVelocityDivergence( RENDER_TARGET dstDivergence, RENDER_TARGET srcVel, RENDER_TARGET srcObst, RENDER_TARGET srcObstVel );
    void ComputePressure          ( RENDER_TARGET dstAndSrcPressure, RENDER_TARGET tmpPressure, RENDER_TARGET srcDivergence, RENDER_TARGET srcObst, RENDER_TARGET srcLevelSet );
    void ProjectVelocity          ( RENDER_TARGET dstVel, RENDER_TARGET srcPressure, RENDER_TARGET srcVel, RENDER_TARGET srcObst, RENDER_TARGET srcObstVel, RENDER_TARGET srcLevelSet );
    void RedistanceLevelSet       ( RENDER_TARGET dstLevelSet, RENDER_TARGET srcLevelSet, RENDER_TARGET tmpLevelSet );
    void ExtrapolateVelocity      ( RENDER_TARGET dstAndSrcVel, RENDER_TARGET tmpVel, RENDER_TARGET srcObst, RENDER_TARGET srcLeveSet );

    //D3D11 helper functions
    int CreateTextureAndViews(int rtIndex, 
    void SetRenderTarget(RENDER_TARGET rtIndex, TextureID  optionalDSV = 0 );


    // Internal State
    //===============
   
    // Grid is used to draw quads for all the slices in the 3D simulation grid
    Grid                                *m_pGrid;
    

    // D3D11 device
    
    

    // Textures, rendertarget views and shader resource views
    TextureID m_p3DTextures                [ NUM_RENDER_TARGETS ];
    TextureID m_pShaderResourceViews       [ NUM_RENDER_TARGETS ];
    TextureID m_pRenderTargetViews         [ NUM_RENDER_TARGETS ];
    FORMAT                         m_RenderTargetFormats         [ NUM_RENDER_TARGETS ];

    // Effect (simulation shaders)
    char                               m_effectPath[MAX_PATH];
    stx_Effect* m_pEffect;

    // Effect Techniques
    stx_Effect::stx_Technique               *m_etAdvect;
    stx_Effect::stx_Technique               *m_etAdvectMACCORMACK;
    stx_Effect::stx_Technique               *m_etVorticity;
    stx_Effect::stx_Technique               *m_etConfinement;
    stx_Effect::stx_Technique               *m_etDiffuse;
    stx_Effect::stx_Technique               *m_etDivergence;
    stx_Effect::stx_Technique               *m_etScalarJacobi;
    stx_Effect::stx_Technique               *m_etProject;

    stx_Effect::stx_Technique               *m_etInitLevelSetToLiquidHeight;
    stx_Effect::stx_Technique               *m_etInjectLiquid;
    stx_Effect::stx_Technique               *m_etAirPressure;
    stx_Effect::stx_Technique               *m_etRedistance;
    stx_Effect::stx_Technique               *m_etExtrapolateVelocity;

    stx_Effect::stx_Technique               *m_etLiquidStream_LevelSet;
    stx_Effect::stx_Technique               *m_etLiquidStream_Velocity;
    stx_Effect::stx_Technique               *m_etGravity;

    stx_Effect::stx_Technique               *m_etGaussian;
    stx_Effect::stx_Technique               *m_etCopyTextureDensity;
    stx_Effect::stx_Technique               *m_etAddDensityDerivativeVelocity;

    stx_Effect::stx_Technique               *m_etStaticObstacleTriangles;
    stx_Effect::stx_Technique               *m_etStaticObstacleLines;
    stx_Effect::stx_Technique               *m_etDrawBox;

    stx_Effect::stx_Technique               *m_etDrawTexture;

    // Shader variables
    stx_Effect::stx_Variable          *m_evTextureWidth;
    stx_Effect::stx_Variable          *m_evTextureHeight;
    stx_Effect::stx_Variable          *m_evTextureDepth;
    stx_Effect::stx_Variable          *m_evDrawTexture;
    stx_Effect::stx_Variable          *m_evDecay;
    stx_Effect::stx_Variable          *m_evViscosity;
    stx_Effect::stx_Variable          *m_evRadius;
    stx_Effect::stx_Variable          *m_evCenter;
    stx_Effect::stx_Variable          *m_evColor;
    stx_Effect::stx_Variable          *m_evGravity;
    stx_Effect::stx_Variable          *m_evVortConfinementScale;
    stx_Effect::stx_Variable          *m_evTimeStep;
    stx_Effect::stx_Variable          *m_evAdvectAsTemperature;
    stx_Effect::stx_Variable          *m_evTreatAsLiquidVelocity;
    stx_Effect::stx_Variable          *m_evFluidType;
    stx_Effect::stx_Variable          *m_evObstBoxLBDcorner;
    stx_Effect::stx_Variable          *m_evObstBoxRTUcorner;
    stx_Effect::stx_Variable          *m_evObstBoxVelocity;

    stx_Effect::stx_Variable  *m_evTexture_pressure;
    stx_Effect::stx_Variable  *m_evTexture_velocity;
    stx_Effect::stx_Variable  *m_evTexture_vorticity;
    stx_Effect::stx_Variable  *m_evTexture_divergence;
    stx_Effect::stx_Variable  *m_evTexture_phi;
    stx_Effect::stx_Variable  *m_evTexture_phi_hat;
    stx_Effect::stx_Variable  *m_evTexture_phi_next;
    stx_Effect::stx_Variable  *m_evTexture_levelset;
    stx_Effect::stx_Variable  *m_evTexture_obstacles;
    stx_Effect::stx_Variable  *m_evTexture_obstvelocity;

    // Use this to ping-pong between two render-targets used to store the density or level set
    RENDER_TARGET                       m_currentDstScalar;
    RENDER_TARGET                       m_currentSrcScalar;

    // Simulation Parameters
    //======================
    FLUID_TYPE  m_eFluidType;
    bool        m_bMouseDown;
    bool        m_bTreatAsLiquidVelocity;
    bool        m_bGravity;
    bool        m_bUseMACCORMACK;
    int         m_nJacobiIterations;
    int         m_nDiffusionIterations;
    int         m_nVelExtrapolationIterations;
    int         m_nRedistancingIterations;
    float       m_fDensityDecay;        // smoke/fire decay rate (1.0 - dissipation_rate)
    float       m_fDensityViscosity;    // smoke/fire viscosity
    float       m_fFluidViscosity;      // viscosity of the fluid (for Diffusion applied to velocity field)
    float       m_fVorticityConfinementScale;    // vorticity confinement scale
    float       m_t;                    // local 'time' for procedural variation

    // Emitter parameters
    //=========================
    float       m_fSaturation;
    float       m_fImpulseSize;
    D3DXFROMWINEVECTOR4 m_vImpulsePos;
    D3DXFROMWINEVECTOR4 m_vImpulseDir;

    // Liquid stream
    //==============
    bool        m_bLiquidStream;
    float       m_fStreamSize;
    D3DXFROMWINEVECTOR4 m_streamCenter;

    // Obstacles
    //===================================
    // Simulation Domain Boundary
    bool        m_bClosedBoundaries;    // can be open or closed
    // ObstacleBox (for testing purposes)
    bool        m_bDrawObstacleBox;
    D3DXFROMWINEVECTOR4 m_vObstBoxPos;
    D3DXFROMWINEVECTOR4 m_vObstBoxPrevPos;
    D3DXFROMWINEVECTOR4 m_vObstBoxVelocity;
};

#endif

