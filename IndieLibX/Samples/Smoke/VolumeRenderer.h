#ifndef FLUID3D_VOLUMERENDERER_H
#define FLUID3D_VOLUMERENDERER_H
/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//----------------------------------------------------------------------------------
// File:   VolumeRenderer.h
// Author: Ignacio Llamas and Chris (Wei-Tae) Kim and Sarah Tariq 
// Email:  sdkfeedback@nvidia.com
// 
// Copyright (c) 2007 NVIDIA_Corporation. All rights reserved.
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

#include "nvutPCH.h"
#include "Smoke.h"
#include <windows.h>


extern bool                                g_bRenderGlow;
extern float                               g_zNear;
extern float                               g_zFar;
extern float                               g_glowContribution;
extern float                               g_finalIntensityScale; 
extern float                               g_finalAlphaScale;
extern float                               g_smokeColorMultiplier;   
extern float                               g_smokeAlphaMultiplier;
extern int                                 g_RednessFactor; 
extern float                               g_xyVelocityScale;
extern float                               g_zVelocityScale;
extern float                               g_fModelRotation;
extern float                               g_fRotSpeed;
extern D3DXFROMWINEMATRIX                          g_View;
extern D3DXFROMWINEMATRIX                          g_Projection;
extern float                               g_Fovy;

extern TextureID           *g_pSceneDepthSRV;
extern TextureID           *g_pEnvMapSRV;
extern D3DXFROMWINEMATRIX                          g_gridWorld;


//does a volume rendering of a box using specified voxel data
class VolumeRenderer
{
public:

    enum RENDER_MODE
    {
        RM_SMOKE    = 0,
        RM_FIRE     = 1,
        RM_LEVELSET = 2
    };

    VolumeRenderer;
    virtual ~VolumeRenderer(void);
    
    int Initialize(int gridWidth, int gridHeight, int gridDepth, RENDER_MODE renderMode);
    int SetScreenSize(int width, int height);
    void Cleanup(void);
    void Draw(TextureID * pSourceTexSRV);
    const float GetMaxDim(){return m_maxDim;};

protected:

    int initShaders(void);
    int createGridBox(void);
    int createScreenQuad(void);
    void calculateRenderTextureSize(int screenWidth, int screenHeight);
    int createRayDataResources(int width, int height);
    int createJitterTexture();

    void computeRayData(void);
    void computeEdgeTexture(void);
    void drawBox(void);
    void drawScreenQuad(void);

    

    float                       m_gridDim[3];
    float                       m_maxDim;
    D3DXFROMWINEMATRIX                  m_gridMatrix;

    char                       m_effectPath[MAX_PATH];
    EffectID                *m_pEffect;
    stx_Technique       *m_pTechnique;

    stx_Pass* *m_epQuadRaycastLevelSet;
    stx_Pass* *m_epQuadRaycastFire;
    stx_Pass* *m_epQuadRaycastSmoke; 
    stx_Pass* *m_epGlowHorizontal;
    stx_Pass* *m_epGlowVertical;
    stx_Pass* *m_epQuadRaycastUpsampleLevelSet;
    stx_Pass* *m_epQuadRaycastUpsampleFire;
    stx_Pass* *m_epQuadRaycastUpsampleSmoke;
    stx_Pass* *m_epCompRayData_Back;
    stx_Pass* *m_epCompRayData_Front;
    stx_Pass* *m_epCompRayData_FrontNOBLEND;
    stx_Pass* *m_epQuadDownSampleRayDataTexture;
    stx_Pass* *m_epQuadEdgeDetect;


    stx_ShaderResourceVariableID  *m_evTexture_rayData;
    stx_ShaderResourceVariableID  *m_evTexture_rayDataSmall;
    stx_ShaderResourceVariableID  *m_evTexture_volume;
    stx_ShaderResourceVariableID  *m_evTexture_rayCast;
    stx_ShaderResourceVariableID  *m_evTexture_edge;
    stx_ShaderResourceVariableID  *m_evTexture_glow;
    stx_ShaderResourceVariableID  *m_evTexture_sceneDepthTex;

    stx_Variable  *m_evRTWidth;
    stx_Variable  *m_evRTHeight;
    stx_Variable  *m_evUseGlow;
    stx_Variable  *m_evWorldViewProjectionMatrix;
    stx_Variable  *m_evInvWorldViewProjectionMatrix;
    stx_Variable  *m_evGrid2WorldMatrix;
    stx_Variable  *m_evZNear;
    stx_Variable  *m_evZFar;
    stx_Variable  *m_evGridScaleFactor;
    stx_Variable  *m_evEyeOnGrid;

    stx_Variable  *m_evGlowContribution;
    stx_Variable  *m_evFinalIntensityScale;
    stx_Variable  *m_evFinalAlphaScale;
    stx_Variable  *m_evSmokeColorMultiplier;
    stx_Variable  *m_evsmokeAlphaMultiplier;
    stx_Variable  *m_evRednessFactor;

    stx_Variable  *m_evWorldViewMatrix;
    stx_Variable  *m_evTan_FovYhalf;
    stx_Variable  *m_evTan_FovXhalf;
    stx_Variable  *m_evRaycastBisection;
    stx_Variable  *m_evRaycastFilterTricubic;
    stx_Variable  *m_evRaycastShaderAsWater;


    VertexFormatID           *m_pGridBoxLayout;
    exBufferID                *m_pGridBoxVertexBuffer;
    exBufferID                *m_pGridBoxIndexBuffer;

    VertexFormatID           *m_pQuadLayout;
    exBufferID                *m_pQuadVertexBuffer;


    TextureID             *pRayDataTex2D[2];
    TextureID      *m_pRayDataRTV[2];
    TextureID    *m_pRayDataSRV[2];

    TextureID             *m_pRayDataSmallTex2D;
    TextureID      *m_pRayDataSmallRTV;
    TextureID    *m_pRayDataSmallSRV;
    
    TextureID             *m_pRayCastTex2D;
    TextureID      *m_pRayCastRTV;
    TextureID    *m_pRayCastSRV;
    
    TextureID             *m_pEdgeTex2D;
    TextureID      *m_pEdgeRTV;
    TextureID    *m_pEdgeSRV;
    
    TextureID             *m_pGlowTex2D;
    TextureID      *m_pGlowRTV;
    TextureID    *m_pGlowSRV;

    TextureID             *m_pGlowTempTex2D;
    TextureID      *m_pGlowTempRTV;
    TextureID    *m_pGlowTempSRV;


    bool                        m_bRaycastBisection;
    bool                        m_bRaycastFilterTricubic;
    bool                        m_bRaycastShadeAsWater;

    RENDER_MODE                 m_eRenderMode;

    int                         m_renderTextureWidth;
    int                         m_renderTextureHeight;

    bool                        m_useFP32Blending;
};


#endif

