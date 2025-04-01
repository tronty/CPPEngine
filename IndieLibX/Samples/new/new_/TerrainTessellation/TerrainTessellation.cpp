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

/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
#include "Stars.h"
#include "TileRing.h"

const int MAX_RINGS = 10;
TileRing* g_pTileRings[MAX_RINGS];

D3DXFROMWINEVECTOR3 g_MasterEyePt;
D3DXFROMWINEMATRIX g_MasterWorldAhead;
D3DXFROMWINEMATRIX g_MasterProj;
D3DXFROMWINEMATRIX g_MasterView;
D3DXFROMWINEMATRIX g_DebugProj;
D3DXFROMWINEMATRIX g_DebugView;
D3DXFROMWINEMATRIX* g_ControlledProj = &g_MasterProj;
D3DXFROMWINEMATRIX* g_ControlledView = &g_MasterView;
D3DXFROMWINEVECTOR3 g_EyePt;

bool g_syncCameras = true, g_DebugCameraHasBeenUsed = false;

bool g_ShowHelp = false;

stx_SkyBox g_Skybox;
bool g_CheckForCracks = false;		// Set to true for more obvious cracks when debugging.

uint64 g_PrimitivesRendered = 0;
	
stx_Effect* g_pTerrainEffect = 0;
stx_Effect* g_pDeformEffect = 0;
VertexBufferID g_TileTriStripIB = 0;
VertexBufferID g_TileQuadListIB = 0;

// Shader variables and matrices
stx_Effect::stx_Variable* g_pEyePosVar = 0;
stx_Effect::stx_Variable* g_pViewDirVar = 0;
stx_Effect::stx_Variable* g_pFractalOctavesTVar = 0;
stx_Effect::stx_Variable* g_pFractalOctavesDVar = 0;
stx_Effect::stx_Variable* g_pUVOffsetTVar = 0;
stx_Effect::stx_Variable* g_pUVOffsetDVar = 0;
stx_Effect::stx_Variable* g_pTriSizeVar = 0;
stx_Effect::stx_Variable* g_pTileSizeVar = 0;
stx_Effect::stx_Variable* g_DebugShowPatchesVar = 0;
stx_Effect::stx_Variable* g_DebugShowTilesVar = 0;
stx_Effect::stx_Variable* g_pViewportVar = 0;
stx_Effect::stx_Variable* g_WireAlphaVar = 0;
stx_Effect::stx_Variable* g_WireWidthVar = 0;
stx_Effect::stx_Variable* g_DetailNoiseVar = 0;
stx_Effect::stx_Variable* g_DetailUVVar = 0;
stx_Effect::stx_Variable* g_SampleSpacingVar = 0;

stx_Effect::stx_Variable* g_HeightMapVar = 0;
stx_Effect::stx_Variable* g_GradientMapVar = 0;
stx_Effect::stx_Variable* g_InputTexVar = 0;
D3DFROMWINEVIEWPORT9 g_BackBufferVP;

stx_Effect::stx_Technique* g_pTesselationTechnique = 0;
stx_Effect::stx_Technique* g_pInitializationTechnique = 0;
stx_Effect::stx_Technique* g_pGradientTechnique = 0;

ShaderResourceViewID g_pHeightMapSRV = 0;
RenderTargetViewID    g_pHeightMapRTV = 0;
ShaderResourceViewID g_pGradientMapSRV = 0;
RenderTargetViewID    g_pGradientMapRTV = 0;

const int MAX_OCTAVES = 15;
const int g_DefaultRidgeOctaves = 3;			// This many ridge octaves is good for the moon - rather jagged.
const int g_DefaultfBmOctaves = 3;
const int g_DefaultTexTwistOctaves = 1;
const int g_DefaultDetailNoiseScale = 20;
int g_RidgeOctaves     = g_DefaultRidgeOctaves;
int g_fBmOctaves       = g_DefaultfBmOctaves;
int g_TexTwistOctaves  = g_DefaultTexTwistOctaves;
int g_DetailNoiseScale = g_DefaultDetailNoiseScale;
float g_CameraSpeed = 1.0f;// ??? invNonlinearCameraSpeed(100);
int g_PatchCount = 0;
bool g_HwTessellation = true;
int g_tessellatedTriWidth = 6;	// pixels on a triangle edge

const float CLIP_NEAR = 1, CLIP_FAR = 20000;
D3DXFROMWINEVECTOR2 g_ScreenSize=D3DXFROMWINEVECTOR2(1920,1200);

const int COARSE_HEIGHT_MAP_SIZE = 1024;
const float WORLD_SCALE = 400;
const int VTX_PER_TILE_EDGE = 9;				// overlap => -2
const int TRI_STRIP_INDEX_COUNT = (VTX_PER_TILE_EDGE-1) * (2 * VTX_PER_TILE_EDGE + 2);
const int QUAD_LIST_INDEX_COUNT = (VTX_PER_TILE_EDGE-1) * (VTX_PER_TILE_EDGE-1) * 4;
int g_nRings = 0;
float SNAP_GRID_SIZE = 0;

bool g_ResetTerrain = false;

const char* g_pHelpText = 
	"\n"
	"Move: WASD + mouse\n"
	"0-9: load saved viewpoints.\n"
	"Alt+0-9: save a viewpoint.\n"
	"'h' hide the entire GUI (for screenshots)\n"
	"\n"
	"There are two independent viewpoints for center of projection and LOD.\n"
	"They can be manipulated independently for debug viewing.\n"
	"Saved viewpoint 0 is loaded at start-up.\n"
;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_STATIC                  -1
#define IDC_TOGGLEFULLSCREEN        1
#define IDC_TOGGLEREF               2
#define IDC_CHANGEDEVICE            3
#define IDC_WIREFRAME               4
#define IDC_RIDGE_OCTAVES           6
#define IDC_RIDGE_STATIC            7
#define IDC_CAMERASPEED             8
#define IDC_CAMERASPEED_STATIC      9
#define IDC_HW_TESSELLATION         10
#define IDC_FBM_OCTAVES             12
#define IDC_FBM_STATIC              13
#define IDC_SYNC_CAMERAS            16
#define IDC_CONTROL_BOTH            17
#define IDC_CONTROL_EYE             18
#define IDC_CONTROL_LOD             19
#define IDC_CAMERA_STATIC           20
#define IDC_TRI_SIZE_STATIC         21
#define IDC_TRI_SIZE                22
#define IDC_DEBUG_SHOW_PATCHES      24
#define IDC_WIREFRAME_ALPHA         25
#define IDC_TWIST_OCTAVES           26
#define IDC_TWIST_STATIC            27
#define IDC_DEBUG_SHOW_TILES        28
#define IDC_DETAIL_OCTAVES          29
#define IDC_DETAIL_STATIC           30

struct App
{
	App(){}
	virtual ~App(){}
int loadTextureFromFile(std::string file, std::string shaderTextureName, stx_Effect* pEffect)
{
    // ??? =IRenderer::GetRendererInstance()->addImageLibTexture(file.c_str(), true, IRenderer::GetRendererInstance()->Getlinear());
    stx_Effect::stx_Variable* textureRVar = pEffect->GetVariableByName(shaderTextureName.c_str());
    return 0;
}

float nonlinearCameraSpeed(float f)
{
	// Make the slider control logarithmic.
	return pow(10.0f, f/100.0f);	// f [1,400]
}

float invNonlinearCameraSpeed(float f)
{
	// Make the slider control logarithmic.
	return 100.0f * (logf(f) / logf(10.0f));
}

void InitCameras()
{
	D3DXFROMWINEVECTOR3 vEye(786.1f,  -86.5f,  62.2f);
	D3DXFROMWINEVECTOR3  vAt(786.3f, -130.0f, 244.1f);
	D3DXFROMWINEVECTOR3  up(0.0f, 1.0f, 0.0f);

	// Default to the above hardcoded vector only in case loading from file fails.
    	D3DXFROMWINEMatrixLookAtLH(&g_MasterView, &vEye, &vAt, &up);
	g_EyePt=vEye;
	g_MasterEyePt=vEye;

	// The two cameras start out the same.
	g_DebugProj = g_MasterProj;
	g_DebugView = g_MasterView;
}

void InitApp()
{
	InitCameras();
	ReadStars();
}

void CreateTileTriangleIB()
{   
    int index = 0;
    unsigned long* pIndices = new unsigned long[TRI_STRIP_INDEX_COUNT];

	// Using the same patch-corner vertices as for h/w tessellaiton, tessellate them into 2 tris each.
	// Create the usual zig-zag pattern of stripped triangles for a regular gridded terrain tile.
    for (int y = 0; y < VTX_PER_TILE_EDGE-1; ++y)
    {
		const int rowStart = y * VTX_PER_TILE_EDGE;

        for (int x = 0; x < VTX_PER_TILE_EDGE; ++x)
        {
            pIndices[index++] = rowStart + x;
            pIndices[index++] = rowStart + x + VTX_PER_TILE_EDGE;
        }

		// Repeat the last one on this row and the first on the next to join strips with degenerates.
        pIndices[index] = pIndices[index-1];
		++index;
        pIndices[index++] = rowStart + VTX_PER_TILE_EDGE;
    }
	assert(index == TRI_STRIP_INDEX_COUNT);
	g_TileTriStripIB=IRenderer::GetRendererInstance()->addIndexBuffer(TRI_STRIP_INDEX_COUNT, 4, STATIC, pIndices);
    delete[] pIndices;
}

void CreateTileQuadListIB()
{   
    int index = 0;
    unsigned long* pIndices = new unsigned long[QUAD_LIST_INDEX_COUNT];

	// The IB describes one tile of NxN patches.
	// Four vertices per quad, with VTX_PER_TILE_EDGE-1 quads per tile edge.
    for (int y = 0; y < VTX_PER_TILE_EDGE-1; ++y)
    {
		const int rowStart = y * VTX_PER_TILE_EDGE;

        for (int x = 0; x < VTX_PER_TILE_EDGE-1; ++x)
        {
            pIndices[index++] = rowStart + x;
            pIndices[index++] = rowStart + x + VTX_PER_TILE_EDGE;
            pIndices[index++] = rowStart + x + VTX_PER_TILE_EDGE + 1;
            pIndices[index++] = rowStart + x + 1;
        }
    }
	assert(index == QUAD_LIST_INDEX_COUNT);
    g_TileQuadListIB=IRenderer::GetRendererInstance()->addIndexBuffer(QUAD_LIST_INDEX_COUNT, 4, STATIC, pIndices);
    delete[] pIndices;
}

int CreateTerrainEffect()
{
    int hr;

	#if 0
    g_pTerrainEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/new/TerrainTessellation/TerrainTessellation.fx");
    #else
    g_pTerrainEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/SimpleTexture/SimpleTexture.fx");
    #endif

    // Obtain techniques
    g_pTesselationTechnique = g_pTerrainEffect->GetTechniqueByName("TesselationTechnique");
    
    // Obtain miscellaneous variables
	g_HeightMapVar = g_pTerrainEffect->GetVariableByName("g_CoarseHeightMap");
	g_GradientMapVar = g_pTerrainEffect->GetVariableByName("g_CoarseGradientMap");

    g_pEyePosVar  = g_pTerrainEffect->GetVariableByName("g_EyePos");
    g_pViewDirVar = g_pTerrainEffect->GetVariableByName("g_ViewDir");
	g_pFractalOctavesTVar = g_pTerrainEffect->GetVariableByName("g_FractalOctaves");
	g_pUVOffsetTVar = g_pTerrainEffect->GetVariableByName("g_TextureWorldOffset");
    g_pViewportVar = g_pTerrainEffect->GetVariableByName( "Viewport" );
	g_pTriSizeVar = g_pTerrainEffect->GetVariableByName("g_tessellatedTriWidth");
	g_pTileSizeVar = g_pTerrainEffect->GetVariableByName("g_tileSize");
	g_DebugShowPatchesVar = g_pTerrainEffect->GetVariableByName("g_DebugShowPatches");
	g_DebugShowTilesVar = g_pTerrainEffect->GetVariableByName("g_DebugShowTiles");
	g_WireAlphaVar = g_pTerrainEffect->GetVariableByName("g_WireAlpha");
	g_WireWidthVar = g_pTerrainEffect->GetVariableByName("g_WireWidth");
	g_DetailNoiseVar = g_pTerrainEffect->GetVariableByName("g_DetailNoiseScale");
	g_DetailUVVar = g_pTerrainEffect->GetVariableByName("g_DetailUVScale");
	g_SampleSpacingVar = g_pTerrainEffect->GetVariableByName("g_CoarseSampleSpacing");
    
	(loadTextureFromFile("TerrainTessellation/LunarSurface1.dds",     "g_TerrainColourTexture1", g_pTerrainEffect));
	(loadTextureFromFile("TerrainTessellation/LunarMicroDetail1.dds", "g_TerrainColourTexture2", g_pTerrainEffect));
	(loadTextureFromFile("GaussianNoise256.jpg", "g_NoiseTexture",   g_pTerrainEffect));
	(loadTextureFromFile("fBm5Octaves.dds",      "g_DetailNoiseTexture",     g_pTerrainEffect));
	(loadTextureFromFile("fBm5OctavesGrad.dds",  "g_DetailNoiseGradTexture", g_pTerrainEffect));
    
    return 0;
}

int CreateAmplifiedHeights()
{
	// No initial data here - it's initialized by deformation.
    g_pHeightMapRTV = IRenderer::GetRendererInstance()->addRenderTarget(COARSE_HEIGHT_MAP_SIZE, COARSE_HEIGHT_MAP_SIZE,
			FORMAT_R32F, 
			IRenderer::GetRendererInstance()->GetnearestClamp());

	// No initial data here - it's initialized by deformation.
    g_pGradientMapRTV = IRenderer::GetRendererInstance()->addRenderTarget(COARSE_HEIGHT_MAP_SIZE, COARSE_HEIGHT_MAP_SIZE,
			FORMAT_RG16F, 
			IRenderer::GetRendererInstance()->GetnearestClamp());

	return 0;
}

int CreateDeformEffect()
{
	#if 0
    g_pDeformEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/new/TerrainTessellation/Deformation.fx");
    #else
    g_pDeformEffect=IRenderer::GetRendererInstance()->addEffectFromFile("/SimpleTexture/SimpleTexture.fx");
    #endif

    // Obtain techniques
	g_pInitializationTechnique = g_pDeformEffect->GetTechniqueByName("InitializationTechnique");
	g_pGradientTechnique       = g_pDeformEffect->GetTechniqueByName("GradientTechnique");
    
    // Obtain miscellaneous variables
	g_pFractalOctavesDVar = g_pDeformEffect->GetVariableByName("g_FractalOctaves");
	g_pUVOffsetDVar = g_pDeformEffect->GetVariableByName("g_TextureWorldOffset");
	g_InputTexVar = g_pDeformEffect->GetVariableByName("g_InputTexture");

	(loadTextureFromFile("GaussianNoise256.jpg", "g_NoiseTexture", g_pDeformEffect));
    
    return 0;
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
int  OnCreateDevice()
{
    int hr;

	CreateAmplifiedHeights();

	CreateTerrainEffect();
	CreateDeformEffect();

	// This array defines the outer width of each successive ring.
	int widths[] = { 0, 16, 16, 16, 16 };
	g_nRings = sizeof(widths) / sizeof(widths[0]) - 1;		// widths[0] doesn't define a ring hence -1
	assert(g_nRings <= MAX_RINGS);

	float tileWidth = 0.125f;
	for (int i=0; i!=g_nRings && i!=MAX_RINGS; ++i)
	{
		g_pTileRings[i] = new TileRing(widths[i]/2, widths[i+1], tileWidth);
		tileWidth *= 2.0f;
	}

	// This is a whole fraction of the max tessellation, i.e., 64/N.  The intent is that 
	// the height field scrolls through the terrain mesh in multiples of the polygon spacing.
	// So polygon vertices mostly stay fixed relative to the displacement map and this reduces
	// scintillation.  Without snapping, it scintillates badly.  Additionally, we make the
	// snap size equal to one patch width, purely to stop the patches dancing around like crazy.
	// The non-debug rendering works fine either way, but crazy flickering of the debug patches 
	// makes understanding much harder.
	const int PATCHES_PER_TILE_EDGE = VTX_PER_TILE_EDGE-1;
	SNAP_GRID_SIZE = WORLD_SCALE * g_pTileRings[g_nRings-1]->tileSize() / PATCHES_PER_TILE_EDGE;

	TileRing::CreateInputLayout(g_pTesselationTechnique);
	CreateTileTriangleIB();
	CreateTileQuadListIB();

	g_Skybox.init("/new/TerrainTessellation/MilkyWayCube.dds");
	ReleaseStars();
	CreateStars();
    
    return 0;
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
int  OnResizedSwapChain()
{
    int hr;

	// Setup the camera's projection parameters
    float aspectRatio = IRenderer::GetRendererInstance()->GetAspectRatio();
	D3DXFROMWINEMatrixPerspectiveFovLH(&g_DebugProj, D3DXFROMWINE_PI / 3, aspectRatio, CLIP_NEAR, CLIP_FAR);
    D3DXFROMWINEMatrixPerspectiveFovLH(&g_MasterProj, D3DXFROMWINE_PI / 3, aspectRatio, CLIP_NEAR, CLIP_FAR);

	g_ResetTerrain = true;		// Reset once to init.

    return 0;
}

//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void  OnFrameMove(double fTime, float fElapsedTime)
{
	const D3DXFROMWINEVECTOR3 beforePos = g_EyePt;

	const bool moved = (beforePos != g_EyePt) != 0;
	g_ResetTerrain = g_ResetTerrain || moved;

	// The debug camera tracks the master until the debug camera is first used separately.
	if (!g_DebugCameraHasBeenUsed)
	{
		g_DebugProj = g_MasterProj;
		g_DebugView = g_MasterView;
	}
}

void FullScreenPass( stx_Effect::stx_Variable* minVar, stx_Effect::stx_Variable* maxVar, stx_Effect::stx_Technique* pTech)
{
	// All of clip space:
	D3DXFROMWINEVECTOR3 areaMin(-1, -1, 0), areaMax(1, 1, 0);

	minVar->SetFloatVector(areaMin);
	maxVar->SetFloatVector(areaMax);
	pTech->GetPassByIndex(0)->Apply(0);
	IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLES, 0, 4);
}

void InitializeHeights()
{
	stx_Effect::stx_Variable* deformMinVar = g_pDeformEffect->GetVariableByName("g_DeformMin");
	stx_Effect::stx_Variable* deformMaxVar = g_pDeformEffect->GetVariableByName("g_DeformMax");

	// This viewport is the wrong size for the texture.  But I've tweaked the noise frequencies etc to match.
	// So keep it like this for now.  TBD: tidy up.
	IRenderer::GetRendererInstance()->setViewport(0,0, COARSE_HEIGHT_MAP_SIZE, COARSE_HEIGHT_MAP_SIZE);
	IRenderer::GetRendererInstance()->setVertexFormat(0);
	IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 4 /* ??? */ );
	IRenderer::GetRendererInstance()->changeRenderTarget(g_pHeightMapRTV);

	FullScreenPass( deformMinVar, deformMaxVar, g_pInitializationTechnique);

	IRenderer::GetRendererInstance()->setViewport(0,0, COARSE_HEIGHT_MAP_SIZE, COARSE_HEIGHT_MAP_SIZE);
	g_InputTexVar->SetResource(g_pHeightMapSRV);
	IRenderer::GetRendererInstance()->changeRenderTarget(g_pGradientMapRTV);
	FullScreenPass( deformMinVar, deformMaxVar, g_pGradientTechnique);

	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
}

void SetUVOffset(stx_Effect::stx_Variable* pVar)
{
	D3DXFROMWINEVECTOR3 eye = g_MasterEyePt;
	eye.y = 0;
	if (SNAP_GRID_SIZE > 0)
	{
		eye.x = floorf(eye.x / SNAP_GRID_SIZE) * SNAP_GRID_SIZE;
		eye.z = floorf(eye.z / SNAP_GRID_SIZE) * SNAP_GRID_SIZE;
	}
	eye /= WORLD_SCALE;
	eye.z *= -1;
	pVar->SetFloatVector(eye);
}

void DeformInitTerrain()
{
	// Reset this so that it's not simultaneously resource and RT.
	g_HeightMapVar->SetResource(0);
	g_GradientMapVar->SetResource(0);
	g_pTesselationTechnique->GetPassByName("ShadedTriStrip")->Apply(0);

	int octaves[3] = { g_RidgeOctaves, g_fBmOctaves, g_TexTwistOctaves };
	g_pFractalOctavesDVar->SetIntVector(octaves);

	SetUVOffset(g_pUVOffsetDVar);
	InitializeHeights();
}

void setViewport( D3DFROMWINEVIEWPORT9& vp)
{
	// Solid wireframe needs the vp in a GS constant.  Set both.
    IRenderer::GetRendererInstance()->setViewport(&vp);
    
    float viewportf[4];
    viewportf[0] = (float) vp.Width;
    viewportf[1] = (float) vp.Height;
	viewportf[2] = (float) vp.X;
	viewportf[3] = (float) vp.Y;
    g_pViewportVar->SetFloatVector(viewportf);
}

void SetMatrices(const D3DXFROMWINEMATRIX& mProj_, const D3DXFROMWINEMATRIX& mView_, const D3DXFROMWINEMATRIX& mProj)
{
	const D3DXFROMWINEMATRIX mView = mView_;

	// Set matrices
	D3DXFROMWINEMATRIX mWorld, mScale, mTrans;
	D3DXFROMWINEMatrixScaling(&mScale, WORLD_SCALE, WORLD_SCALE, WORLD_SCALE);

	// We keep the eye centered in the middle of the tile rings.  The height map scrolls in the horizontal 
	// plane instead.
	const D3DXFROMWINEVECTOR3 eye = g_MasterEyePt;
	float snappedX = eye.x, snappedZ = eye.z;
	if (SNAP_GRID_SIZE > 0)
	{
		snappedX = floorf(snappedX / SNAP_GRID_SIZE) * SNAP_GRID_SIZE;
		snappedZ = floorf(snappedZ / SNAP_GRID_SIZE) * SNAP_GRID_SIZE;
	}
	const float dx = eye.x - snappedX;
	const float dz = eye.z - snappedZ;
	snappedX = eye.x - 2*dx;				// Why the 2x?  I'm confused.  But it works.
	snappedZ = eye.z - 2*dz;
	D3DXFROMWINEMatrixTranslation(&mTrans, snappedX, 0, snappedZ);
	D3DXFROMWINEMatrixMultiply(&mWorld, &mScale, &mTrans);
    
	stx_Effect::stx_Variable* pmWorldViewProj    = g_pTerrainEffect->GetVariableByName("g_WorldViewProj");
    stx_Effect::stx_Variable* pmProj             = g_pTerrainEffect->GetVariableByName("g_Proj");
    stx_Effect::stx_Variable* pmWorldViewProjLOD = g_pTerrainEffect->GetVariableByName("g_WorldViewProjLOD");
    stx_Effect::stx_Variable* pmWorldViewLOD     = g_pTerrainEffect->GetVariableByName("g_WorldViewLOD");

    D3DXFROMWINEMATRIX mWorldView = mWorld * mView;
    D3DXFROMWINEMATRIX mWorldViewProj = mWorldView * mProj;
    pmWorldViewProj->SetMatrix((float*) &mWorldViewProj);
    pmProj->SetMatrix((float*) &mProj);

	// For LOD calculations, we always use the master camera's view matrix.
    D3DXFROMWINEMATRIX mWorldViewLOD = mWorld * *g_MasterView;
    D3DXFROMWINEMATRIX mWorldViewProjLOD = mWorldViewLOD * mProj;
    pmWorldViewProjLOD->SetMatrix((float*) &mWorldViewProjLOD);
	pmWorldViewLOD->SetMatrix((float*) &mWorldViewLOD);

	// Due to the snapping tricks, the centre of projection moves by a small amount in the range ([0,2*dx],[0,2*dz])
	// relative to the terrain.  For frustum culling, we need this eye position.
	D3DXFROMWINEVECTOR3 cullingEye = eye;
	cullingEye.x -= snappedX;
	cullingEye.z -= snappedZ;
    g_pEyePosVar->SetFloatVector(cullingEye);
	g_pViewDirVar->SetFloatVector((float*) g_MasterWorldAhead);
}

void RenderTerrain( const D3DXFROMWINEMATRIX& mProj, D3DFROMWINEVIEWPORT9& vp, const char* passOverride=0)
{
	D3DXFROMWINEMATRIX mProj_ = *g_ControlledProj;
    	D3DXFROMWINEMATRIX mView = *g_ControlledView;
	if(!g_syncCameras)
	{	
    		mProj_ = g_DebugProj;
    		mView = g_DebugView;
	}
	SetMatrices(mProj_ , mView, mProj);

	if (g_HwTessellation)
	{
		// ??? IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_4_CONTROL_POINT_PATCHLIST);
		IRenderer::GetRendererInstance()->setIndexBuffer(g_TileQuadListIB);
	}
	else
	{
		// ??? IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_STRIP);
		IRenderer::GetRendererInstance()->setIndexBuffer(g_TileTriStripIB);
	}

	const char* passName = "ShadedTriStrip";

	const bool wire = false;
	if (wire)
		passName = "Wireframe";
	if (g_HwTessellation)
	{
		if (wire)
			passName = "HwTessellatedWireframe";
		else
			passName = "HwTessellated";
	}
	if (passOverride != 0)
		passName = passOverride;

	stx_Effect::stx_Pass* pPass = g_pTesselationTechnique->GetPassByName(passName);
	if (!pPass)
		return;		// Shouldn't happen unless the FX file is broken (like wrong pass name).

	setViewport( vp);

	for (int i=0; i!=g_nRings; ++i)
	{
		const TileRing* pRing = g_pTileRings[i];
		pRing->SetRenderingState();

		g_HeightMapVar->SetResource(g_pHeightMapSRV);
		g_GradientMapVar->SetResource(g_pGradientMapSRV);
		g_pTileSizeVar->SetFloat(pRing->tileSize());
	
		// Need to apply the pass after setting its vars.
		pPass->Apply(0);

		// Instancing is used: one tiles is one instance and the index buffer describes all the 
		// NxN patches within one tile.
		const int nIndices = (g_HwTessellation)? QUAD_LIST_INDEX_COUNT: TRI_STRIP_INDEX_COUNT;
		IRenderer::GetRendererInstance()->DrawIndexedInstanced(nIndices, pRing->nTiles(), 0, 0, 0);
	}
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void RenderToBackBuffer( double fTime, float fElapsedTime)
{
    // Clear render target and the depth stencil 
    D3DXFROMWINEVECTOR4 ClearColor(1.0f, 0.0f, 1.0f, 1.0f);					// Purple to better spot terrain cracks (disable sky cube).
    //D3DXFROMWINEVECTOR4 ClearColor(0.465f, 0.725f, 0.0f, 1.0f);				// NV green for colour-consistent illustrations.
	// ??? IRenderer::GetRendererInstance()->changeRenderTarget(BackBuffer);
	IRenderer::GetRendererInstance()->Clear(true, true, ClearColor);

	// Something's wrong in the shader and the tri size is out by a factor of 2.  Why?!?
	int i_=2 * g_tessellatedTriWidth;
	g_pTriSizeVar->SetInt(&i_);

	const bool debugDrawPatches = false;
	g_DebugShowPatchesVar->SetBool(debugDrawPatches);

	const bool debugDrawTiles = false;
	g_DebugShowTilesVar->SetBool(debugDrawTiles);

	const float wireAlpha = 1.0f; // ??? 0.01f * (float) g_SampleUI.GetSlider(IDC_WIREFRAME_ALPHA)->GetValue();

	// Below 1.0, we fade the lines out with blending; above 1, we increase line thickness.
	if (wireAlpha < 1)
	{
		g_WireAlphaVar->SetFloat(wireAlpha);
		g_WireWidthVar->SetFloat(1);
	}
	else
	{
		g_WireAlphaVar->SetFloat(1);
		g_WireWidthVar->SetFloat(wireAlpha);
	}

	g_DetailNoiseVar->SetFloat(0.001f * (float)g_DetailNoiseScale);
	g_SampleSpacingVar->SetFloat(WORLD_SCALE * g_pTileRings[g_nRings-1]->outerWidth() / (float) COARSE_HEIGHT_MAP_SIZE);
    
	D3DXFROMWINEMATRIX mProj = *g_ControlledProj;
    	D3DXFROMWINEMATRIX mView = *g_ControlledView;
	if(!g_syncCameras)
	{	
    		mProj = g_DebugProj;
    		mView = g_DebugView;
	}

	setViewport( g_BackBufferVP);

	D3DXFROMWINEMATRIX mViewCopy = mView;
	mViewCopy._41 = mViewCopy._42 = mViewCopy._43 = 0;
	D3DXFROMWINEMATRIX mWVP = StarWorldMatrix() * mViewCopy * mProj;
	if (!g_CheckForCracks)
		g_Skybox.render();//(&mWVP);

	RenderStars( mViewCopy, mProj, g_ScreenSize);

	int vec[3] = { g_RidgeOctaves, g_fBmOctaves, g_TexTwistOctaves };
	g_pFractalOctavesTVar->SetIntVector(vec);

	// I'm still trying to figure out if the detail scale can be derived from any combo of ridge + twist.
	// I don't think this works well (nor does ridge+twist+fBm).  By contrast the relationship with fBm is
	// straightforward.  The -4 is a fudge factor that accounts for the frequency of the coarsest ocatve
	// in the pre-rendered detail map.
	const float DETAIL_UV_SCALE = powf(2.0f, std::max(g_RidgeOctaves, g_TexTwistOctaves) + g_fBmOctaves - 4.0f);
	g_DetailUVVar->SetFloatVector(D3DXFROMWINEVECTOR2(DETAIL_UV_SCALE, 1.0f/DETAIL_UV_SCALE));

	SetUVOffset(g_pUVOffsetTVar);

	RenderTerrain( mProj, g_BackBufferVP);
}

void  OnFrameRender(double fTime, float fElapsedTime)
{
	if (g_ResetTerrain)
	{
		g_ResetTerrain = false;
		DeformInitTerrain();
	}

	RenderToBackBuffer( fTime, fElapsedTime);
}
};
App app;
int init(const char* aTitle)
{
    	app.OnCreateDevice();	
	return 0;
}

void render()
{
   double fTime=0;
   float fElapsedTime=0.0f;
   app.OnFrameRender(fTime, fElapsedTime);
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("TerrainTessellation");
	//if(std::string(r->getString())!="D3D11") stx_exit(11);
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	STX_PRINT("Function init finished!\n");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		IRenderer::GetRendererInstance()->BeginScene();
		render();
		IRenderer::GetRendererInstance()->EndScene();
		IRenderer::GetRendererInstance()->Present( );
	}
	return 0;
}

