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


//Notes:
//collision detection for multistrand interpolated hair strands:
//In a prepass we render all the interpolant hair to a texture, all vertices of an interpolated hair strand write to the same pixel. 
//for each vertex we output its vertexID if that vertex collides with a collision implicit. pixels are rendered with min blending. 
//The result is a texture that encodes for each interpolated strand whether any of its vertices intersect with a collision implicit, 
//and if they do, what is the first vertex that does so.
//The use of this is that when we are creating interpolated hair strands using multistrand interpolation, often interpolated strands 
//end up intersecting collision implicits even tho the guide hair did not.
//When such a situation occurs its useful to switch interpolation mode of the strand over to single strand based interpolation. 
//However, it is not enough just to switch interpolation mode of the colliding vertices of the interpolated strand; all vertices after 
//the colliding vertex (as well as some before) have to be switched over to singlestrand based interpolation otherwise the results are not satisfactory

//Mouse movements:
//left click: move the camera
//right click: move the model
//shift + left click: move the light
//shift + right click: move the wind vector
//--------------------------------------------------------------------------------------

#include "Common.h"
#include "Hair.h"
#include "Fluid.h"

#include "resource.h"
#include "DensityGrid.h"
#include "HairShadows.h"
#include <sdkmesh.h>

#include <fstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stddef.h>

#include <Strsafe.h>

using namespace std;


/* uncomment to help track memory leaks
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
*/

#define sqr(a) ( (a)*(a) )

bool g_useSOInterpolatedAttributes = true;
INTERPOLATE_MODEL g_InterpolateModel = HYBRID;

//super sampling
TextureID  g_pSceneColorBuffer2XMS;
TextureID  g_pSceneColorBuffer2X;
TextureID  g_pSceneRTV2X;
TextureID  g_pSceneDepthBuffer2X;
TextureID  g_pSceneDSV2X;
SURFACE_DESC g_pRTVTex2DDesc;

// Window Dimensions
int                         g_Width  = 1920.0f;
int                         g_Height = 1200.0f; 

int g_fluidGridWidth = 32;
int g_fluidGridHeight = 32;
int g_fluidGridDepth = 32;
int g_gridSize = 32; //density grid
static const unsigned NHAIRS_PER_PATCH = 64;
static const unsigned NSEGMENTS_PER_PATCH = 64;

D3DXFROMWINEMATRIX g_currentHairTransform;
D3DXFROMWINEMATRIX g_oldHairTransform;
D3DXFROMWINEMATRIX g_InvInitialTotalTransform;
D3DXFROMWINEMATRIX g_InvInitialRotationScale;
D3DXFROMWINEMATRIX g_InitialTotalTransform;
D3DXFROMWINEMATRIX g_ScalpToMesh;
D3DXFROMWINEMATRIX g_gridWorldInv;
string g_HairBoneName; //this is the name of the bone that the hair is attached to
D3DXFROMWINEMATRIX g_ExternalCameraView;
D3DXFROMWINEVECTOR3 g_ExternalCameraEye;
D3DXFROMWINEVECTOR3 g_ExternalCameaLookAtPt;
unsigned g_MaxSStrandsPerControlHair = 0;
unsigned g_MaxMStrandsPerWisp = 0;
float g_MaxWorldScale = 1;

bool g_useSkinnedCam = false;


//recording mouse movements as transforms that can be replayed later
ofstream g_transformsFileOut;
ifstream g_transformsFileIn;
int g_animationIndex = 0;
bool g_recordTransforms = false;
bool g_playTransforms = false;
bool g_loopTransforms = false;
char* g_tstr_animations[] = 
{ 
	("RecordedTransforms.txt"), 
	("RecordedTransforms1.txt"),
	("RecordedTransforms2.txt"),
	0
};


bool g_RestoreHairStrandsToDefault = false;
bool g_useShortHair = false;

bool g_printHelp = false;

float skinColor[4] = {205/255.0f,172/255.0f,131/255.0f,1};

hairShadingParameters hairShadingParams[3];
int g_currHairShadingParams = 0;
char* g_tstr_shadingOptionLabels[] = 
{ 
	("blond"), 
	("brown"),
	("red"),
	0
};


char g_loadDirectory[MAX_PATH];
char* MayaHairDirectory = "LongHairResources";
char* ProceduralHairDirectory = "ProceduralHairResources";

ofstream g_ForcesFile;

char* g_tstr_blurTypeLabels[] = 
{ 
	("Gaussian"), 
	("Box"),
	0
};
BLURTYPE g_currentBlurChoice = GAUSSIAN_BLUR;

float g_fNumHairsLOD = 1.0f;
float g_fWidthHairsLOD = 1.0f;
float g_LODScalingFactor = 1.2f;

char* g_tstr_interHairForceTypeLabels[] = 
{ 
	("Gradient based"), 
	("From cell center"),
	0
};
int g_currentInterHairForcesChoice = 0; //0 is gradient based


// Any macros to be passed to the shaders used in this sample (for conditional compilation)
D3DXFROMWINEMACRO g_shadersMacros[] = 
{
	0
};
D3DXFROMWINEMACRO *g_pShadersMacros = g_shadersMacros;


HairGrid* g_HairGrid = 0;
Fluid* g_fluid = 0;

#define MAX_INTERPOLATED_HAIR_PER_WISP 200 //note the g_NumMStrandsPerWisp variable cannot/should not be larger than this
int g_NumMStrandsPerWisp = 45;
int g_NumSStrandsPerWisp = 80;
int g_SSFACTOR = 2;
//#define SUPERSAMPLE //undefine this to enable super sampling for the hair, this will reduce the aliasing but will also be costly

bool g_useCS = true;
bool g_hairReset = false;
bool g_bApplyAdditionalTransform = false;
bool g_useSimulationLOD = true;

int g_totalMStrands = 0;
int g_totalSStrands = 0;

float g_maxHairLength = 0;
float g_scalpMaxRadius;
float g_maxHairLengthWorld;	
float g_scalpMaxRadiusWorld;
float g_maxHairLengthHairSpace;
float g_scalpMaxRadiusHairSpace;

bool g_bVisualizeTessellatedCFs = false;
bool g_bVisualizeCFs = false;

bool g_firstFrame = true;
bool g_renderGUI = true;
bool g_firstMovedFrame = true;

float* g_lengths = 0;
D3DXFROMWINEVECTOR2* g_clumpCoordinates = 0;
float2* g_randStrandDeviations = 0;
float* g_masterStrandLengthToRoot = 0;
float* g_masterStrandLengths = 0;
float* g_Strand_Sizes = 0;
D3DXFROMWINEVECTOR2* g_tangentJitter = 0;
D3DXFROMWINEVECTOR2* g_BarycentricCoordinates = 0;
Attributes* g_MasterAttributes = 0;


D3DXFROMWINEMATRIX g_Transform;
D3DXFROMWINEMATRIX g_RootTransform;
D3DXFROMWINEMATRIX g_TotalTransform;
D3DXFROMWINEVECTOR3 g_Center;
bool g_UseTransformations;

//ground plane
#define GROUND_PLANE_RADIUS 30.0f
exBufferID   g_pPlaneVertexBuffer = 0;
exBufferID   g_pPlaneIndexBuffer = 0;
VertexFormatID  g_pPlaneVertexLayout = 0;
stx_Effect::stx_Technique*  g_pTechniqueRenderPlane = 0;

D3DFROMWINEVIEWPORT9 g_superSampleViewport;

//variables for demo---------------------------------------------------------------------------------

bool g_bCurlyHair = false;
bool g_renderDensityOnHair = false;

bool g_applyHForce = true;
bool g_simulate = true;
bool g_reloadEffect = false;
bool g_addGravity = true;
bool g_useLinearStiffness = true;
bool g_useAngularStiffness = false; 
bool g_collisions = true;
bool g_addAngularForces = false; //note this term can lead to some pulsing behaviour in stiff hair when high amounts of force are applied
bool g_RenderCollsionImplicits = false;

float g_timeStep =  0.1f;
float g_WindForce = 0.08f;
float g_windImpulseRandomness = 0.16f;
int g_windImpulseRandomnessSmoothingInterval = 15;
float g_AngularSpringStiffness = 0.025f;
int g_NumTotalConstraintIterations = 20;
int g_numCFIterations = 50;
float g_angularSpringStiffnessMultiplier = 1.0;
float g_GravityStrength = 0.5f; 

//inter hair forces:
int g_BlurRadius = 5;
float g_BlurSigma = 4;
bool g_useBlurredDensityForInterHairForces = true;
float g_densityThreshold = 10.4f;
float g_interHairForces = 0.001f;

//shadows
HairShadows g_HairShadows;
bool g_VisShadowMap = false;
float g_SigmaA = 8.0f;
float g_SoftEdges = 0.0f;
bool g_RenderScene = true;
bool g_RenderScalp = true;
bool g_RenderFloor = true;
bool g_MeshWithShadows = true;
bool g_renderMStrands = true;
bool g_renderSStrands = true;
bool g_useDX11 = true;
bool g_bDynamicLOD = true;
bool g_renderShadows = true;

bool g_takeImage = false;


//shading:
float g_fHairAlpha = 1.0f;


enum UI_OPTION
{
	SHOW_SIMULATION,
	SHOW_HAIR_RENDERING,
	SHOW_SCENE_RENDERING,
	NUM_UI_OPTIONS
};
UI_OPTION g_UIOption = SHOW_HAIR_RENDERING;

char* g_tstr_uiLabels[] = 
{ 
	("Simulation UI"), 
	("Hair Rendering UI"),
	("Scene Rendering UI"),
	0
};

int g_cameraAndWindPreset = 1;
int NumCameraAndWindPresets = 3;



struct HairAdjacencyVertex
{
	int controlVertexIndex;
	float u;
};

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
D3DXFROMWINEMATRIX g_CameraEyePt;
D3DXFROMWINEMATRIX g_CameraProj;
D3DXFROMWINEMATRIX g_CameraView;
D3DXFROMWINEMATRIX g_CameraWorld;
               // A model viewing camera
stx_Effect*            g_Effect;
char                   g_pEffectPath[MAX_PATH];
MeshRenderer2             g_MeshArrow;
MeshRenderer2*             g_Scalp;
MeshRenderer2*             g_FaceMesh;
MeshRenderer2             g_Sphere;

ComputeShaderID  g_pCSSimulateParticles;

int* g_indices = 0;
float* g_coordinateFrames = 0;

bool g_bUseGSTessellation = false;
bool g_bUseGSInterpolation = false;
bool g_bUseInstancedInterpolation = true;

//techniques
stx_Effect::stx_Technique*  g_pTechniqueRenderArrow           = 0;

//render targets
TextureID  g_CollisionsRTV = 0; 

//textures 
TextureID  g_pVelTexture3D = 0; //3D texture of the simulated wind velocity

int g_currentVB = 0;
int g_currentForceVB = 0;
int g_currentVBCoordinateFrame = 0; 

// Vertex buffers
exBufferID  g_MasterStrandVB[2];
exBufferID  g_ForceVB[2];
exBufferID  g_TessellatedMasterStrandVB = 0;
exBufferID  g_TessellatedMasterStrandVBInitial = 0;
exBufferID  g_MasterStrandLinearSpringLengthsVB = 0;
exBufferID  g_MasterStrandAngularSpringConstraintsVB = 0;
exBufferID  g_TessellatedWispVB = 0;
exBufferID  g_MasterStrandVBOldTemp = 0; 
exBufferID  g_MasterStrandVBOld = 0;
exBufferID  g_rootsIndices = 0;
exBufferID  g_rootsIndicesUntessellated = 0;
exBufferID  g_masterLengths = 0;
exBufferID  g_masterLengthsUntessellated = 0;
exBufferID  g_UnTessellatedWispVB = 0;
exBufferID  g_CoordinateFrameVB[2];
exBufferID  g_TessellatedCoordinateFrameVB = 0;
exBufferID  g_TessellatedMasterStrandLengthToRootVB = 0;
exBufferID  g_TessellatedTangentsVB = 0;
exBufferID  g_BarycentricInterpolatedPositionsWidthsVB = 0;
exBufferID  g_BarycentricInterpolatedIdAlphaTexVB = 0;
exBufferID  g_BarycentricInterpolatedTangentVB = 0;
exBufferID  g_ClumpInterpolatedPositionsWidthsVB = 0;
exBufferID  g_ClumpInterpolatedIdAlphaTexVB = 0;
exBufferID  g_ClumpInterpolatedTangentVB = 0;

//data for compute shaders
exBufferID  g_MasterStrandUAB = 0;
exBufferID  g_MasterStrandPrevUAB = 0;
exBufferID  g_CoordinateFrameUAB = 0;
exBufferID  g_pcbCSPerFrame = 0;
exBufferID  g_pcbCSTransforms = 0;
exBufferID  g_pcbCSRootXform = 0;
exBufferID  g_MasterStrandSimulationIndicesBuffer = 0;
TextureID  g_MasterStrandUAB_SRV = 0;
TextureID  g_MasterStrandLinearSpringLengthsSRV = 0;
TextureID  g_MasterStrandSimulationIndicesSRV = 0;
TextureID  g_CoordinateFrameUAB_SRV = 0;
TextureID  g_AngularStiffnessUAB_SRV = 0;
TextureID  g_MasterStrandAngularSpringConstraintsSRV = 0;
ID3D11UnorderedAccessView* g_MasterStrandUAB_UAV = 0;
ID3D11UnorderedAccessView* g_MasterStrandPrevUAB_UAV = 0;
ID3D11UnorderedAccessView* g_CoordinateFrameUAB_UAV = 0;
SamplerStateID *g_pLinearClampSampler = 0;


exBufferID  g_StrandSizes = 0;
exBufferID  g_StrandCoordinates = 0;
exBufferID  g_MasterAttributesBuffer = 0;
exBufferID  g_rootsIndicesMasterStrand = 0;
exBufferID  g_collisionScreenQuadBuffer = 0;
exBufferID  g_ScreenQuadBuffer = 0;
exBufferID  g_SuperSampleScreenQuadBuffer = 0;

// Index buffers
exBufferID  g_MasterStrandSimulation1IB = 0;
exBufferID  g_MasterStrandSimulation2IB = 0;
exBufferID  g_MasterStrandSimulationAngular1IB = 0;
exBufferID  g_MasterStrandSimulationAngular2IB = 0;


// Vertex layouts
VertexFormatID  g_MasterStrandIL = 0;
VertexFormatID  g_MasterStrandILAddForces = 0;
VertexFormatID  g_MasterStrandSpringConstraintsIL = 0;
VertexFormatID  g_MasterStrandAngularSpringConstraintsIL = 0;
VertexFormatID  g_RenderMeshIL = 0;
VertexFormatID  g_MasterStrandTesselatedInputIL = 0;
VertexFormatID  g_pVertexLayoutArrow = 0; // Vertex Layout for arrow
VertexFormatID  g_CoordinateFrameIL = 0;
VertexFormatID  g_screenQuadIL = 0;

// Resource views
TextureID  g_TessellatedMasterStrandRV = 0;
TextureID  g_MasterStrandRV[2];
TextureID  g_ForceRV[2];
TextureID  g_StrandSizesRV = 0;
TextureID  g_StrandCoordinatesRV = 0;
TextureID  g_StrandCircularCoordinatesRV = 0;
TextureID  g_StrandAttributesRV = 0;
TextureID  g_RootsIndicesRV = 0;
TextureID  g_RootsIndicesRVUntessellated = 0;
TextureID  g_MasterLengthsRV = 0;
TextureID  g_MasterLengthsRVUntessellated = 0;
TextureID  g_MasterStrandLengthToRootRV = 0;
TextureID  g_rootsIndicesMasterStrandRV = 0;
TextureID  g_CoordinateFrameRV[2];
TextureID  g_TessellatedMasterStrandRootIndexRV = 0;
TextureID  g_TessellatedCoordinateFrameRV = 0;
TextureID  g_TessellatedMasterStrandLengthToRootRV = 0;
TextureID  g_StrandLengthsRV = 0;
TextureID  g_TessellatedTangentsRV = 0;
TextureID  g_StrandColorsRV = 0;
TextureID  g_StrandDeviationsRV = 0;
TextureID  g_CurlDeviationsRV = 0;
TextureID  g_OriginalMasterStrandRV = 0;
TextureID  g_CollisionsRV = 0;
TextureID  g_FluidVelocityRV = 0;
TextureID  g_originalVectorsRV = 0;
TextureID  g_MasterStrandLocalVertexNumberRV = 0;
TextureID  hairTextureRV = 0;
TextureID  g_TangentJitterRV = 0;
TextureID  g_MasterStrandLengthsRV = 0;
TextureID  g_pSceneColorBufferShaderResourceView = 0;
TextureID  g_BarycentricInterpolatedPositionsWidthsRV = 0;
TextureID  g_BarycentricInterpolatedIdAlphaTexRV = 0;
TextureID  g_BarycentricInterpolatedTangentRV = 0;
TextureID  g_ClumpInterpolatedPositionsWidthsRV = 0;
TextureID  g_ClumpInterpolatedIdAlphaTexRV = 0;
TextureID  g_ClumpInterpolatedTangentRV = 0;
TextureID  g_MstrandsIndexRV = 0;
TextureID  g_SstrandsIndexRV = 0;
TextureID  g_SStrandsPerMasterStrandCumulative = 0;
TextureID  g_MStrandsPerWispCumulative = 0;
TextureID  g_HairBaseRV[3];

// effect variables
stx_Effect::stx_Variable* g_pShadowsShaderVariable = 0;
stx_Effect::stx_Variable* g_pDOMShadowsShaderVariable = 0;
stx_Effect::stx_Variable* g_pApplyHForceShaderVariable = 0;
stx_Effect::stx_Variable* g_pCurlyHairShaderVariable = 0;
stx_Effect::stx_Variable* g_pAddGravityShaderVariable = 0;
stx_Effect::stx_Variable* g_pSimulateShaderVariable = 0;
stx_Effect::stx_Variable* g_InterHairForcesShaderVariable = 0;
stx_Effect::stx_Variable* g_AngularStiffnessMultiplierShaderVariable = 0;
stx_Effect::stx_Variable* g_BlurSigmaShaderVariable = 0;
stx_Effect::stx_Variable* g_BlurRadiusShaderVariable = 0;
stx_Effect::stx_Variable* g_UseBlurTextureForForcesShaderVariable = 0;
stx_Effect::stx_Variable* g_UseGradientBasedForceShaderVariable = 0;
stx_Effect::stx_Variable* g_GravityStrengthShaderVariable = 0;
stx_Effect::stx_Variable* g_UseScalpTextureShaderVariable = 0;
stx_Effect::stx_Variable* g_ArrowColorShaderVariable = 0;
stx_Effect::stx_Variable* g_BaseColorShaderVariable = 0;
stx_Effect::stx_Variable* g_SpecColorShaderVariable = 0;
stx_Effect::stx_Variable* g_hairTextureArrayShaderVariable = 0;
stx_Effect::stx_Variable* g_SceneDepthShaderResourceVariable = 0;
stx_Effect::stx_Variable* g_SceneColor2XShaderResourceVariable = 0;
stx_Effect::stx_Variable* g_collisionSphereTransformationsEV = 0;
stx_Effect::stx_Variable* g_collisionSphereInverseTransformationsEV = 0;
stx_Effect::stx_Variable* g_collisionCylinderTransformationsEV = 0;
stx_Effect::stx_Variable* g_collisionCylinderInverseTransformationsEV = 0;
stx_Effect::stx_Variable* g_SphereNoMoveImplicitInverseTransformEV = 0;
stx_Effect::stx_Variable* g_SphereNoMoveImplicitTransformEV = 0;

int g_numIndicesFirst;
int g_numIndicesSecond;
int g_numIndicesFirstAngular;
int g_numIndicesSecondAngular;
int g_NumWisps;
int g_NumMasterStrands;
unsigned int g_TessellatedMasterStrandLengthMax;
int g_MaxMStrandSegments;
int g_MasterStrandLengthMax;
int g_NumStrands;
int g_NumMasterStrandControlVertices;
HairVertex* g_MasterStrandControlVertices = 0;
Index* g_MasterStrandControlVertexOffsets = 0;
Index* g_TessellatedMasterStrandVertexOffsets = 0;
D3DXFROMWINEVECTOR3* g_masterCVIndices = 0;

// Rendering
int g_NumTessellatedMasterStrandVertices;
int g_NumTessellatedWisps;
int g_NumUntessellatedWisps;

const int g_NumStrandVariables = 1024;//size of Interpolated hair variables. this is used for hair thickness, interpolation coordinates
float g_thinning = 0.5; 


bool g_Reset;

//wind
D3DXFROMWINEVECTOR3                 g_windVector = D3DXFROMWINEVECTOR3(0.75089055f, 0.12556140f, 0.64838076f );

//collisions
int g_numSpheres = 0;
int g_numCylinders = 0;
int g_numSpheresNoMoveConstraint = 0;
vector<collisionObject> collisionObjectTransforms;
vector<int> sphereIsBarycentricObstacle;
vector<int> cylinderIsBarycentricObstacle;
vector<int> sphereNoMoveIsBarycentricObstacle;

float g_blendAxis = 0.1f;

//constants for compute shader
const int g_iCBPSPerFrameBind = 0;
const int g_iCBCSTransformBind = 1;
const int g_iCBCSRootXFormBind = 2;

struct CB_CS_PER_FRAME
{
	D3DXFROMWINEMATRIX additionalTransformation;
	D3DXFROMWINEMATRIX RootTransformation;
    D3DXFROMWINEMATRIX currentHairTransformation;
    D3DXFROMWINEMATRIX WorldToGrid;

    int bApplyHorizontalForce;
    int bAddGravity;
    int numConstraintIterations;
    int numCFIterations;

	float angularStiffness;
    float gravityStrength;
    float TimeStep;
	int integrate;

	int bApplyAdditionalTransform;
	int restoreToDefault;
	float blendAxis;
	int addAngularForces;
};

struct CB_CS_TRANSFORMS
{
    int g_NumSphereImplicits;
    int g_NumCylinderImplicits;
    int g_NumSphereNoMoveImplicits;
    int padding;

    D3DXFROMWINEMATRIX CollisionSphereTransformations[MAX_IMPLICITS];
    D3DXFROMWINEMATRIX CollisionSphereInverseTransformations[MAX_IMPLICITS];
    D3DXFROMWINEMATRIX CollisionCylinderTransformations[MAX_IMPLICITS];
    D3DXFROMWINEMATRIX CollisionCylinderInverseTransformations[MAX_IMPLICITS];
    D3DXFROMWINEMATRIX SphereNoMoveImplicitInverseTransform[MAX_IMPLICITS];
};

CB_CS_TRANSFORMS g_cbTransformsInitialData;

struct SimpleVertex
{
	D3DXFROMWINEVECTOR3 Pos;  
	D3DXFROMWINEVECTOR4 Color; 
};

#define NUM_LINES_MAX 1000


class App: public STXGUI
{
public:
    App(){}
    virtual ~App(){}
    int init(const char* aTitle){STXGUI::init("/GUILayout.xml");}
    void render( ){STXGUI::update();}
//--------------------------------------------------------------------------------------------------------------
//render the hair
//this function renders hair for all the different options that we have (including with and without tessellation
//---------------------------------------------------------------------------------------------------------------

void RenderInterpolatedHair(RENDERTYPE renderType, INTERPOLATE_MODEL interpolateModel, int numMStrands, int numSStrands)
{
	if (g_useDX11 && renderType == SOATTRIBUTES)
	{
		// we want to streamout attributes, but we don't do use streamout for dx11. dx11 computes and renders directly
		return;
	}
    if(interpolateModel==NO_HAIR) return;

	// we dont need a vertex buffer since we are only using vertexID and instanceID
	unsigned int stride = 0;
	unsigned int offset = 0;
	exBufferID  buffer[] = { 0 };
	IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, buffer, &stride, &offset);
	IRenderer::GetRendererInstance()->setVertexFormat(0, 0);

	//get the effect that we are going to be using, which depends on the interpolateModel, the renderType and on the global variables g_useSOInterpolatedAttributes(are we using StreamOut in the intermediate steps of the Dx10 path?) and g_useDX11(are we using DX11 tessellation?)
	stx_Pass* pEffectPass = 0;
	bool bUnsetHSandDS = false;
	bool isValidEffect = getEffectPass(interpolateModel, renderType, &pEffectPass, bUnsetHSandDS);
	if(!isValidEffect) return;

	//set the variables needed
	setVariablesForRendering(interpolateModel, renderType);

    //set the streamout buffers if we are going to be doing streamout for a subsequent pass
	if(interpolateModel == MULTI_HYBRID && renderType == SOATTRIBUTES)
	{
		exBufferID  SO_buffers[] = { g_BarycentricInterpolatedPositionsWidthsVB, 
			g_BarycentricInterpolatedIdAlphaTexVB,
			g_BarycentricInterpolatedTangentVB};
		unsigned int offsets[] = { 0, 0, 0 };
		IRenderer::GetRendererInstance()->SOSetTargets(3, SO_buffers, offsets);
	}
	else if(interpolateModel == SINGLESTRAND && renderType == SOATTRIBUTES)
	{
		exBufferID  SO_buffers[] = { g_ClumpInterpolatedPositionsWidthsVB, 
			g_ClumpInterpolatedIdAlphaTexVB,
			g_ClumpInterpolatedTangentVB};
		unsigned int offsets[] = { 0, 0, 0 };
		IRenderer::GetRendererInstance()->SOSetTargets(3, SO_buffers, offsets);
	}
	else
		IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
	
	pEffectPass->Apply(0);    


	//do the actual primitive rendering
	if((interpolateModel == MULTISTRAND || interpolateModel == MULTI_HYBRID) && renderType == SOATTRIBUTES) renderPointList( g_TessellatedMasterStrandLengthMax*numMStrands);
	else if(interpolateModel == SINGLESTRAND && g_useDX11) renderHairWithTessellation( interpolateModel, pEffectPass);
	else if(interpolateModel == SINGLESTRAND && renderType == SOATTRIBUTES && !g_useDX11) renderPointList( g_TessellatedMasterStrandLengthMax*numSStrands);
	else if(interpolateModel == MULTI_HYBRID && g_useDX11) renderHairWithTessellation( interpolateModel, pEffectPass);
	else if(interpolateModel == MULTI_HYBRID && !g_useDX11) renderLineStrip( g_TessellatedMasterStrandLengthMax * numMStrands);
	else if (interpolateModel == SINGLESTRAND) renderLineStrip( g_TessellatedMasterStrandLengthMax * numSStrands);
	else if(interpolateModel == MULTISTRAND && renderType == INSTANCED_INTERPOLATED_COLLISION && g_useDX11)	renderHairWithTessellation( interpolateModel, pEffectPass);
	else if(interpolateModel == MULTISTRAND && renderType == INSTANCED_INTERPOLATED_COLLISION && !g_useDX11)renderPointListInstanced(g_TessellatedMasterStrandLengthMax, numMStrands);
			

	//unset the variables
	unsetVariablesForRendering(interpolateModel,renderType);
	IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
	pEffectPass->Apply(0);
}


void setVariablesForRendering(INTERPOLATE_MODEL interpolateModel, RENDERTYPE renderType)
{
	if (g_useDX11)
	{
		g_Effect->GetVariableByName("g_iSubHairFirstVert")->AsScalar()->SetInt(0);
		g_Effect->GetVariableByName("g_iFirstPatchHair")->AsScalar()->SetInt(0);
	}

	g_useCS ? g_Effect->GetVariableByName("g_MasterStrandSB")->AsShaderResource()->SetResource(g_MasterStrandUAB_SRV) : g_Effect->GetVariableByName("g_MasterStrand")->AsShaderResource()->SetResource(g_MasterStrandRV[g_currentVB]);
	g_Effect->GetVariableByName("g_TessellatedMasterStrand")->AsShaderResource()->SetResource(g_TessellatedMasterStrandRV);
	g_Effect->GetVariableByName("g_tessellatedCoordinateFrames")->AsShaderResource()->SetResource(g_TessellatedCoordinateFrameRV);
	g_Effect->GetVariableByName("g_TessellatedLengthsToRoots")->AsShaderResource()->SetResource(g_TessellatedMasterStrandLengthToRootRV);
	g_Effect->GetVariableByName("g_TessellatedTangents")->AsShaderResource()->SetResource(g_TessellatedTangentsRV);	
	g_HairGrid->setDemuxTexture();
	g_HairGrid->setBlurTexture();

	if (renderType != INSTANCED_INTERPOLATED_COLLISION)
		g_Effect->GetVariableByName("g_CollisionsTexture")->AsShaderResource()->SetResource(g_CollisionsRV);

	if(!g_useDX11)
	{
		if(g_useSOInterpolatedAttributes && interpolateModel == MULTI_HYBRID && renderType != SOATTRIBUTES)
		{
			g_Effect->GetVariableByName("g_InterpolatedPositionAndWidth")->AsShaderResource()->SetResource(g_BarycentricInterpolatedPositionsWidthsRV);
			g_Effect->GetVariableByName("g_InterpolatedIdAlphaTex")->AsShaderResource()->SetResource(g_BarycentricInterpolatedIdAlphaTexRV);
			g_Effect->GetVariableByName("g_Interpolatedtangent")->AsShaderResource()->SetResource(g_BarycentricInterpolatedTangentRV);
		}
		else if(g_useSOInterpolatedAttributes && interpolateModel == SINGLESTRAND && renderType != SOATTRIBUTES)
		{
			g_Effect->GetVariableByName("g_InterpolatedPositionAndWidthClump")->AsShaderResource()->SetResource(g_ClumpInterpolatedPositionsWidthsRV);
			g_Effect->GetVariableByName("g_InterpolatedIdAlphaTexClump")->AsShaderResource()->SetResource(g_ClumpInterpolatedIdAlphaTexRV);
			g_Effect->GetVariableByName("g_InterpolatedtangentClump")->AsShaderResource()->SetResource(g_ClumpInterpolatedTangentRV);
		}
	}
	else
	{
		if (interpolateModel == SINGLESTRAND)
		{
			g_Effect->GetVariableByName("g_SStrandsPerMasterStrandCumulative")->AsShaderResource()->SetResource(g_SStrandsPerMasterStrandCumulative);
		}
		else
		{
			g_Effect->GetVariableByName("g_MStrandsPerWispCumulative")->AsShaderResource()->SetResource(g_MStrandsPerWispCumulative);
		}
	}

	if(!g_simulate)
	    g_Effect->GetVariableByName("g_bApplyAdditionalRenderingTransform")->AsScalar()->SetBool(true);
	else
	    g_Effect->GetVariableByName("g_bApplyAdditionalRenderingTransform")->AsScalar()->SetBool(false);
	g_Effect->GetVariableByName("additionalTransformation")->AsMatrix()->SetMatrix(g_RootTransform);


	if(renderType == INSTANCED_INTERPOLATED_COLLISION)
		g_HairGrid->setObstacleVoxelizedTexture();

	//bind the shadow texture if we are not actually in the pass that renders the shadows
	if( renderType != INSTANCED_DEPTH && renderType != INSTANCED_DEPTH_DOM)
		g_HairShadows.SetHairShadowTexture();

	if(renderType == INSTANCED_DENSITY)
	{
		g_HairGrid->setDemuxTexture();
		g_HairGrid->setBlurTexture(); 
	}
}

void unsetVariablesForRendering(INTERPOLATE_MODEL interpolateModel, RENDERTYPE renderType)
{
	g_useCS ? g_Effect->GetVariableByName("g_MasterStrandSB")->AsShaderResource()->SetResource(0) : g_Effect->GetVariableByName("g_MasterStrand")->AsShaderResource()->SetResource(0);
	g_Effect->GetVariableByName("g_TessellatedMasterStrand")->AsShaderResource()->SetResource(0);
	g_Effect->GetVariableByName("g_tessellatedCoordinateFrames")->AsShaderResource()->SetResource(0);
	g_Effect->GetVariableByName("g_TessellatedLengthsToRoots")->AsShaderResource()->SetResource(0);
	g_Effect->GetVariableByName("g_TessellatedTangents")->AsShaderResource()->SetResource(0);	
	g_HairGrid->unsetDemuxTexture();
	g_HairGrid->unsetBlurTexture();
	if( renderType!= INSTANCED_INTERPOLATED_COLLISION)
		g_Effect->GetVariableByName("g_CollisionsTexture")->AsShaderResource()->SetResource(0);
	
	if(g_useSOInterpolatedAttributes && interpolateModel == MULTI_HYBRID)
	{
		if(renderType == INSTANCED_NORMAL_HAIR)
		{
			g_Effect->GetVariableByName("g_InterpolatedPositionAndWidth")->AsShaderResource()->SetResource(0);
			g_Effect->GetVariableByName("g_InterpolatedIdAlphaTex")->AsShaderResource()->SetResource(0);
			g_Effect->GetVariableByName("g_Interpolatedtangent")->AsShaderResource()->SetResource(0);
		}
		else if(renderType == INSTANCED_DEPTH || renderType == INSTANCED_DEPTH_DOM)
		{
			g_Effect->GetVariableByName("g_InterpolatedPositionAndWidth")->AsShaderResource()->SetResource(0);
			g_Effect->GetVariableByName("g_InterpolatedIdAlphaTex")->AsShaderResource()->SetResource(0);
			g_Effect->GetVariableByName("g_Interpolatedtangent")->AsShaderResource()->SetResource(0);
		}
		else if(renderType == INSTANCED_HAIR_DEPTHPASS)
		{
			g_Effect->GetVariableByName("g_InterpolatedPositionAndWidth")->AsShaderResource()->SetResource(0);
			g_Effect->GetVariableByName("g_InterpolatedIdAlphaTex")->AsShaderResource()->SetResource(0);
			g_Effect->GetVariableByName("g_Interpolatedtangent")->AsShaderResource()->SetResource(0);
		}
	}
	if(g_useSOInterpolatedAttributes && interpolateModel == SINGLESTRAND)
	{
		if(renderType == INSTANCED_NORMAL_HAIR)
		{
			g_Effect->GetVariableByName("g_InterpolatedPositionAndWidthClump")->AsShaderResource()->SetResource(0);
			g_Effect->GetVariableByName("g_InterpolatedIdAlphaTexClump")->AsShaderResource()->SetResource(0);
			g_Effect->GetVariableByName("g_InterpolatedtangentClump")->AsShaderResource()->SetResource(0);
		}
		else if(renderType == INSTANCED_DEPTH || renderType == INSTANCED_DEPTH_DOM)
		{
			g_Effect->GetVariableByName("g_InterpolatedPositionAndWidthClump")->AsShaderResource()->SetResource(0);
			g_Effect->GetVariableByName("g_InterpolatedIdAlphaTexClump")->AsShaderResource()->SetResource(0);
			g_Effect->GetVariableByName("g_InterpolatedtangentClump")->AsShaderResource()->SetResource(0);
		}
		else if(renderType == INSTANCED_HAIR_DEPTHPASS)
		{
			g_Effect->GetVariableByName("g_InterpolatedPositionAndWidthClump")->AsShaderResource()->SetResource(0);
			g_Effect->GetVariableByName("g_InterpolatedIdAlphaTexClump")->AsShaderResource()->SetResource(0);
			g_Effect->GetVariableByName("g_InterpolatedtangentClump")->AsShaderResource()->SetResource(0);
		}
	}
	if(renderType == INSTANCED_INTERPOLATED_COLLISION)
		g_HairGrid->unsetObstacleVoxelizedTexture();

	if(renderType == INSTANCED_DENSITY)
	{
		g_HairGrid->unsetDemuxTexture();
		g_HairGrid->unsetBlurTexture(); 
	}

	g_HairShadows.UnSetHairShadowTexture();
}
			
void renderPointList(int numPoints)
{
    IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_POINTS);
	IRenderer::GetRendererInstance()->Draw(numPoints,0); 
}

void renderPointListInstanced(int vertexCountPerInstance, int instanceCount)
{		
	IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_POINTS);
	IRenderer::GetRendererInstance()->DrawInstanced(vertexCountPerInstance,instanceCount,0,0);
}

void renderLineStrip(int numPoints)
{
    IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_LINE_STRIP);
	IRenderer::GetRendererInstance()->Draw(numPoints,0); 
}

void renderHairWithTessellation(INTERPOLATE_MODEL interpolateModel, stx_Pass* pEffectPass)
{
	if(interpolateModel == MULTISTRAND || interpolateModel == MULTI_HYBRID)
	{		
		IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_1_CONTROL_POINT_PATCHLIST);
		for (unsigned int g_iFirstPatchHair = 0; ; )
		{
			for (int g_iSubHairFirstVert = 0; ; )
			{
				pEffectPass->Apply(0);
				IRenderer::GetRendererInstance()->Draw(g_NumWisps, 0);
				g_iSubHairFirstVert += NSEGMENTS_PER_PATCH;
				if (g_iSubHairFirstVert >= g_MaxMStrandSegments)
					break;
				g_Effect->GetVariableByName("g_iSubHairFirstVert")->AsScalar()->SetInt(g_iSubHairFirstVert);
			}
			g_iFirstPatchHair += NHAIRS_PER_PATCH;
			if (g_iFirstPatchHair >= g_MaxMStrandsPerWisp)
				break;
			g_Effect->GetVariableByName("g_iFirstPatchHair")->AsScalar()->SetInt(g_iFirstPatchHair);
			g_Effect->GetVariableByName("g_iSubHairFirstVert")->AsScalar()->SetInt(0);
		}
	}
	else if(interpolateModel == SINGLESTRAND)
	{
	    IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_1_CONTROL_POINT_PATCHLIST);
		// render first segments of all hairs (up to 64 first segments - HW limitation)
		for (unsigned int iFirstPatchHair = 0; ; )
		{
			IRenderer::GetRendererInstance()->Draw(g_NumMasterStrands, 0);
			iFirstPatchHair += NHAIRS_PER_PATCH;
			if (iFirstPatchHair >= g_MaxSStrandsPerControlHair)
				break;
			g_Effect->GetVariableByName("g_iFirstPatchHair")->AsScalar()->SetInt(iFirstPatchHair);
			pEffectPass->Apply(0);
		}
		// if there are more segments - render the rest (64 in each go)
		unsigned iSubHairFirstVert = NSEGMENTS_PER_PATCH;
		while (iSubHairFirstVert < g_TessellatedMasterStrandLengthMax - 1)
		{
			g_Effect->GetVariableByName("g_iSubHairFirstVert")->AsScalar()->SetInt(iSubHairFirstVert);
			for (unsigned int iFirstPatchHair = 0; ; )
			{
				g_Effect->GetVariableByName("g_iFirstPatchHair")->AsScalar()->SetInt(iFirstPatchHair);
				pEffectPass->Apply(0);
				IRenderer::GetRendererInstance()->Draw(g_NumMasterStrands, 0);
				iFirstPatchHair += NHAIRS_PER_PATCH;
				if (iFirstPatchHair >= g_MaxSStrandsPerControlHair)
					break;
			}
			iSubHairFirstVert += NSEGMENTS_PER_PATCH;
		}
	}

}
			


bool getEffectPass(INTERPOLATE_MODEL interpolateModel, RENDERTYPE renderType, stx_Pass** pEffectPass, bool& bUnsetHSandDS)
{
	//in this interpolation mode we render multistrand interpolated hair, but we detect collisions and switch colliding strand vertices to singlestrand based interpolation
	if(interpolateModel == MULTI_HYBRID)
	{
		if(renderType == INSTANCED_NORMAL_HAIR && g_useSOInterpolatedAttributes && !g_useDX11)
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedVS_LOAD");
		else if(renderType == INSTANCED_NORMAL_HAIR && g_useDX11)
		{
			bUnsetHSandDS = true;
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateAndRenderM_HardwareTess");
		}
		else if(renderType == INSTANCED_NORMAL_HAIR)
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedVS");


		else if(renderType == INSTANCED_DEPTH && g_useDX11)
		{
			bUnsetHSandDS = true;
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateAndRenderDepth_HardwareTess");
		}
		else if(renderType == INSTANCED_DEPTH && g_useSOInterpolatedAttributes)
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedDepthVS_LOAD");
		else if(renderType == INSTANCED_DEPTH)
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedDepthVS");


		else if(renderType == INSTANCED_DEPTH_DOM && g_useDX11)
		{
			bUnsetHSandDS = true;
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateAndRenderDepth_HardwareTess_DOM");
		}
		else if(renderType == INSTANCED_DEPTH_DOM && g_useSOInterpolatedAttributes)
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedDepthVS_LOAD_DOM");
		else if(renderType == INSTANCED_DEPTH_DOM)
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedDepthVS_DOM");




		else if(renderType == INSTANCED_HAIR_DEPTHPASS && g_useSOInterpolatedAttributes)
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedDepthPrepassVS_LOAD");
		else if(renderType == INSTANCED_HAIR_DEPTHPASS)
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedDepthPrepassVS");


		else if(renderType == SOATTRIBUTES)
		{
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("SO_M_Attributes");
		}
		else
			return false;
	}
	else if(interpolateModel == SINGLESTRAND) 	//singlestrand based interpolation
	{
		if(renderType == INSTANCED_NORMAL_HAIR && g_useSOInterpolatedAttributes && !g_useDX11)
		{
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedVSSingleStrand_LOAD");
		}
		else if(renderType == INSTANCED_NORMAL_HAIR && !g_useDX11)
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedVSSingleStrand");
		else if(renderType == INSTANCED_DEPTH && g_useSOInterpolatedAttributes && !g_useDX11)
		{
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedDepthVSSingleStrand_LOAD");	
		}
		else if(renderType == INSTANCED_DEPTH && !g_useDX11)
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedDepthVSSingleStrand");	
		
		else if(renderType == INSTANCED_DEPTH_DOM && g_useSOInterpolatedAttributes && !g_useDX11)
		{
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedDepthVSSingleStrand_LOAD_DOM");	
		}
		else if(renderType == INSTANCED_DEPTH_DOM && !g_useDX11)
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedDepthVSSingleStrand_DOM");	



		else if(renderType == INSTANCED_HAIR_DEPTHPASS && g_useSOInterpolatedAttributes)
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedVSSingleStrandDepthPrepass_LOAD");
		else if(renderType == INSTANCED_HAIR_DEPTHPASS)
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedVSSingleStrandDepthPrepass");


		else if (renderType == SOATTRIBUTES)
		{
			assert(!g_useDX11);
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("SO_S_Attributes");
		}
		else if (g_useDX11) //use tessellation to render singlestrand based interpolated hair
		{
			bUnsetHSandDS = true;
			if (renderType == INSTANCED_DEPTH)
				*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedDepthVSSingleStrand11");
			else if (renderType == INSTANCED_DEPTH_DOM)
				*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedDepthVSSingleStrand11_DOM");
			else if (renderType == INSTANCED_NORMAL_HAIR)
				*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateAndRenderS_HardwareTess");
		}
		else
			return false;
	}
	else if(interpolateModel == MULTISTRAND)//multistrand interpolation without detecting or fixing collisions of interpolated strands with collision geometry
	{
		if(renderType == INSTANCED_INTERPOLATED_COLLISION)
		{
			if(g_useDX11)
			{
				bUnsetHSandDS = true;
				*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateAndRenderCollisions_HardwareTess");
			}
			else
				*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedBaryCentricCollisions");
		}
		else if(renderType == INSTANCED_COLLISION_RESULTS)
			*pEffectPass = g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedVSRenderCollisionsNormal");
		else
			return false;
	}
	else
		return false;

	return true;
}


//------------------------------------------------------------------------------------------
//GPU based simulation of hair. This function calls either compute shader based hair 
//simulation or stream out based simulation, depending on what the current user choice is.
//this function is also responsible for invoking the fluid simulation for the wind
//------------------------------------------------------------------------------------------

int StepHairSimulation()
{
	//keeping track of the frames since simulation started. 
	//This is useful for damping the forces in the beginning as hair resolves constraints and settles down (for example the initial hairstyle might have hair strands colliding with obstacles) 
    static int frame = 0;
	if(g_hairReset)
	{    
		frame = 0;
        g_hairReset = false;
	}
	frame++;


	//add wind velocity to the grid, and advect it forward
	//only do this if we are simulating
	if(g_applyHForce && g_simulate)
		stepFluidSimulation();

    //hair simulation using compute shader
    if(g_useCS)
		return stepHairSimulationDX11( frame);
	
    //hair simulation using Dx10 style GPGPU with Streamout
	return stepHairSimulationDX10( frame);
}

void stepFluidSimulation()
{
	//transform wind to be in the correct coordinate frame
	D3DXFROMWINEVECTOR3 transformedWind;
	D3DXFROMWINEVECTOR3 windNormVector = g_windDirectionWidget.GetLightDirection();
 	D3DXFROMWINEVec3Normalize(&windNormVector,&windNormVector);
	D3DXFROMWINEVec3TransformCoord(&transformedWind,&windNormVector,&g_InvInitialRotationScale);
	D3DXFROMWINEVec3Normalize(&transformedWind,&transformedWind);


	D3DXFROMWINEVECTOR3 gridCenter = D3DXFROMWINEVECTOR3( g_fluidGridWidth/2.0, g_fluidGridHeight/2.0, g_fluidGridDepth/2.0); 
	float center[4] = { gridCenter.x - transformedWind.x*gridCenter.x,
		gridCenter.y - transformedWind.y*gridCenter.y,
		gridCenter.z - transformedWind.z*gridCenter.z, 0 };
	if( center[0] < 0)
		center[0] = 0;
	if( center[0] >= g_fluidGridWidth )
		center[0] = g_fluidGridWidth-1;

	if( center[1] < 0)
		center[1] = 0;
	if( center[1] >= g_fluidGridHeight )
		center[1] = g_fluidGridHeight-1;

	if( center[2] < 0)
		center[2] = 0;
	if( center[2] >= g_fluidGridDepth )
		center[2] = g_fluidGridDepth-1;


	//demux the hair density into the obstacle texture of the fluid
	g_HairGrid->demultiplexTextureTo3DFluidObstacles();

    //compute a linearly smoothed random variation
	static int time = 0;
    time++;
	static float prevRandOffset = (random()-0.5f)*g_windImpulseRandomness;
	static float currRandOffset = (random()-0.5f)*g_windImpulseRandomness;
	if(time%g_windImpulseRandomnessSmoothingInterval==0)
	{
		time = 0;
	    prevRandOffset = currRandOffset;
	    currRandOffset = (random()-0.5f)*g_windImpulseRandomness;
	}
	float t = (float)time/g_windImpulseRandomnessSmoothingInterval;
	float smoothVariation = t*currRandOffset + (1.0f-t)*prevRandOffset;
	float windForce = max(g_WindForce+smoothVariation,0.0f);

	//add wind impulse to the fluid sim and advect forward
	g_fluid->Impulse( center[0], center[1], center[2], transformedWind.x*windForce, transformedWind.y*windForce, transformedWind.z*windForce );

	float lowerWindAmount = 0.05f;
	float higherWindAmount = 0.15f;
	float windScale = (g_WindForce-lowerWindAmount)/(higherWindAmount-lowerWindAmount);

	//determining wind impulse amount
	//the smaller the wind force the smaller the radius of it we want, and thus the larger the impulsesize (this is a weird parameter that is inversely proportional)
	//empirically, these values look good.
	float gridDiagonal = sqrt( float(sqr(g_fluidGridWidth) + sqr(g_fluidGridHeight) + sqr(g_fluidGridDepth)) );
	float lowerImpulseSize = 0.065*166/gridDiagonal;
	float upperImpulseSize = 0.025*166/gridDiagonal;
	if(windScale<0) windScale=0;
	if(windScale>1) windScale=1;
	float impulseSize = windScale*upperImpulseSize + (1-windScale)*lowerImpulseSize;
	float vorticityConfinement = 0;

	g_fluid->Update(2.0f, false, true, vorticityConfinement,0.9f,0,0,impulseSize,0.4f);
}

int stepHairSimulationDX11(int frame)
{
	int hr = 0;

	unsigned int initCounts = 0;
	IRenderer::GetRendererInstance()->CSSetShader( g_pCSSimulateParticles, 0, 0 );

	// Per frame cb update
	

	( IRenderer::GetRendererInstance()->Map( g_pcbCSTransforms, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
	CB_CS_TRANSFORMS* pTransform = ( CB_CS_TRANSFORMS* )MappedResource.pData;
	stx_memcpy(pTransform, &g_cbTransformsInitialData, sizeof(CB_CS_TRANSFORMS));
	IRenderer::GetRendererInstance()->Unmap( g_pcbCSTransforms, 0 );
	IRenderer::GetRendererInstance()->CSSetConstantBuffers( g_iCBCSTransformBind, 1, &g_pcbCSTransforms );

	( IRenderer::GetRendererInstance()->Map( g_pcbCSPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
	CB_CS_PER_FRAME* pPerFrame = ( CB_CS_PER_FRAME* )MappedResource.pData;
	pPerFrame->angularStiffness = g_angularSpringStiffnessMultiplier;
    pPerFrame->numConstraintIterations = g_NumTotalConstraintIterations;
    pPerFrame->numCFIterations = g_numCFIterations;
	pPerFrame->gravityStrength = g_GravityStrength;
	pPerFrame->bApplyHorizontalForce = g_applyHForce;
	pPerFrame->bAddGravity = g_addGravity;
	pPerFrame->TimeStep = g_timeStep;
	pPerFrame->RootTransformation = g_RootTransform;
	pPerFrame->currentHairTransformation = g_currentHairTransform;
	pPerFrame->WorldToGrid = g_gridWorldInv;
	D3DXFROMWINEMATRIX additionalTransformMatrix;
    D3DXFROMWINEMatrixIdentity(&additionalTransformMatrix);
    if(g_bApplyAdditionalTransform) 
		additionalTransformMatrix = g_RootTransform; 
	pPerFrame->additionalTransformation = additionalTransformMatrix;
	pPerFrame->integrate = frame<5?false:true;
	pPerFrame->bApplyAdditionalTransform = g_bApplyAdditionalTransform;
	pPerFrame->restoreToDefault = g_RestoreHairStrandsToDefault;
	pPerFrame->blendAxis = g_blendAxis;
	pPerFrame->addAngularForces = g_addAngularForces;
	IRenderer::GetRendererInstance()->Unmap( g_pcbCSPerFrame, 0 );
	IRenderer::GetRendererInstance()->CSSetConstantBuffers( g_iCBPSPerFrameBind, 1, &g_pcbCSPerFrame );

	//set the shader resources that only need to be read in the CS (vs written to)
	TextureID  ppSRV[7] = { g_MasterStrandSimulationIndicesSRV, 
										   g_MasterStrandLinearSpringLengthsSRV, 
										   g_AngularStiffnessUAB_SRV, 
										   g_OriginalMasterStrandRV,
										   g_FluidVelocityRV,
										   g_MasterStrandAngularSpringConstraintsSRV, 
	                                       g_originalVectorsRV};
	IRenderer::GetRendererInstance()->CSSetShaderResources( 0, 7, ppSRV);

	//set the unordered access views - this is what the CS shader will read from and write to
	//we bind two buffers, one containing the particle positions, and the other containing particle positions from the last frame
	ID3D11UnorderedAccessView* ppUAV[3] = { g_MasterStrandUAB_UAV, g_MasterStrandPrevUAB_UAV, g_CoordinateFrameUAB_UAV };
	IRenderer::GetRendererInstance()->CSSetUnorderedAccessViews( 0, 3, ppUAV, &initCounts );

	//set the sampler
	SamplerStateID *states[1] = { g_pLinearClampSampler };
	IRenderer::GetRendererInstance()->CSSetSamplers( 0, 1, states );

	// Run the CS. we run one CTA for each strand. All the threads in a CTA will work co-operatively on a single strand
	IRenderer::GetRendererInstance()->Dispatch( g_NumMasterStrands, 1, 1 );

	// Unbind resources for CS
	ID3D11UnorderedAccessView* ppUAViewNULL[3] = { 0, 0, 0 };
	IRenderer::GetRendererInstance()->CSSetUnorderedAccessViews( 0, 3, ppUAViewNULL, &initCounts );
	TextureID  ppSRVNULL[7] = { 0, 0, 0, 0, 0, 0, 0 };
	IRenderer::GetRendererInstance()->CSSetShaderResources( 0, 7, ppSRVNULL );

	if(g_RestoreHairStrandsToDefault)
	{
	    g_RestoreHairStrandsToDefault = false;
	    g_hairReset = true;
	}

	return hr;
}

int stepHairSimulationDX10(int frame)
{
	int hr = 0;

	unsigned int stride = sizeof(HairVertex);
	unsigned int offset = 0;

	if(g_RestoreHairStrandsToDefault)
	{
		IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandIL);

		IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &g_MasterStrandVB[g_currentVB], &stride, &offset);
		IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_POINTS);
		g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("restoreToDefaultPositions")->Apply(0);
		IRenderer::GetRendererInstance()->SOSetTargets(1, &g_MasterStrandVB[(g_currentVB+1)%2], &offset);
		IRenderer::GetRendererInstance()->Draw(g_NumMasterStrandControlVertices, 0);
		IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
		g_currentVB = (g_currentVB+1)%2;

		IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &g_MasterStrandVB[g_currentVB], &stride, &offset);
		IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_POINTS);
		g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("restoreToDefaultPositions")->Apply(0);
		IRenderer::GetRendererInstance()->SOSetTargets(1, &g_MasterStrandVB[(g_currentVB+1)%2], &offset);
		IRenderer::GetRendererInstance()->Draw(g_NumMasterStrandControlVertices, 0);
		IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
		g_currentVB = (g_currentVB+1)%2;

		g_RestoreHairStrandsToDefault = false;
		g_hairReset = true;
	}

	if(frame < 5) //this is a way of not appling verlet integration in the beginning, when we are introducing strong distance changes due to collision constraints
		IRenderer::GetRendererInstance()->CopyResource(g_MasterStrandVBOldTemp, g_MasterStrandVB[g_currentVB]);
	else 
		IRenderer::GetRendererInstance()->CopyResource(g_MasterStrandVBOldTemp, g_MasterStrandVBOld);

	IRenderer::GetRendererInstance()->CopyResource(g_MasterStrandVBOld, g_MasterStrandVB[g_currentVB]);    

	exBufferID  buffers[] = { g_MasterStrandVB[g_currentVB], g_MasterStrandVBOldTemp };
	unsigned int strides[] = { sizeof(HairVertex), sizeof(HairVertex) };
	unsigned int offsets[] = { 0, 0 };

	g_Effect->GetVariableByName("g_bApplyAdditionalTransform")->AsScalar()->SetBool(g_bApplyAdditionalTransform);
	g_Effect->GetVariableByName("additionalTransformation")->AsMatrix()->SetMatrix(g_RootTransform);

    RepropagateCoordinateFrames( g_numCFIterations); 

	//add angular
	if(g_addAngularForces)
	{
		g_Effect->GetVariableByName("g_MasterStrand")->AsShaderResource()->SetResource(g_MasterStrandRV[g_currentVB]);
		g_Effect->GetVariableByName("g_coordinateFrames")->AsShaderResource()->SetResource(g_CoordinateFrameRV[g_currentVBCoordinateFrame]);
		g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("addAngularForces")->Apply(0);

		//first batch
		g_Effect->GetVariableByName("g_bClearForces")->AsScalar()->SetBool(true);
		g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("addAngularForces")->Apply(0);
		IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandIL);
		IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &g_ForceVB[g_currentForceVB], &stride, &offset);
		IRenderer::GetRendererInstance()->SOSetTargets(1, &g_ForceVB[(g_currentForceVB+1)%2], &offset);
		IRenderer::GetRendererInstance()->setIndexBuffer(g_MasterStrandSimulation1IB, FORMAT_R16_UINT, 0);
		IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_LINES);
		IRenderer::GetRendererInstance()->DrawIndexed(g_numIndicesFirst*2.0, 0, 0);
		IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
		g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("addAngularForces")->Apply(0);
		g_currentForceVB = (g_currentForceVB+1)%2;

		//second batch
		g_Effect->GetVariableByName("g_bClearForces")->AsScalar()->SetBool(false);
		g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("addAngularForces")->Apply(0);
		IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandIL);
		IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &g_ForceVB[g_currentForceVB], &stride, &offset);
		IRenderer::GetRendererInstance()->SOSetTargets(1, &g_ForceVB[(g_currentForceVB+1)%2], &offset);
		IRenderer::GetRendererInstance()->setIndexBuffer(g_MasterStrandSimulation2IB, FORMAT_R16_UINT, 0);
		IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_LINES);
		IRenderer::GetRendererInstance()->DrawIndexed(g_numIndicesSecond*2.0, 0, 0);
		IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
		g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("addAngularForces")->Apply(0);
		g_currentForceVB = (g_currentForceVB+1)%2;

		//unbind resources
		g_Effect->GetVariableByName("g_MasterStrand")->AsShaderResource()->SetResource(0);
		g_Effect->GetVariableByName("g_coordinateFrames")->AsShaderResource()->SetResource(0);
		g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("addAngularForces")->Apply(0);
	}

	//add per vertex forces and integrate using verlet integration: this is not iterated; we only add forces once
	{
		g_Effect->GetVariableByName("g_FluidVelocityTexture")->AsShaderResource()->SetResource(g_FluidVelocityRV);
		g_Effect->GetVariableByName("g_Forces")->AsShaderResource()->SetResource(g_ForceRV[g_currentForceVB]);
		g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("addForcesAndIntegrate")->Apply(0);

		IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandILAddForces);
		IRenderer::GetRendererInstance()->setVertexBuffer(0, 2, buffers, strides, offsets);
		IRenderer::GetRendererInstance()->SOSetTargets(1, &g_MasterStrandVB[(g_currentVB+1)%2], &offset);
		IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_POINTS);
		IRenderer::GetRendererInstance()->Draw(g_NumMasterStrandControlVertices, 0);
		IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
		g_currentVB = (g_currentVB+1)%2;

		g_Effect->GetVariableByName("g_FluidVelocityTexture")->AsShaderResource()->SetResource(0);
		g_Effect->GetVariableByName("g_Forces")->AsShaderResource()->SetResource(0);
		g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("addForcesAndIntegrate")->Apply(0);
	}

    //iterate over all the constraints multiple times
    {
        for(int totalSimIterations=0;totalSimIterations<g_NumTotalConstraintIterations ;totalSimIterations++)
        {
		    stride = sizeof(HairVertex);

			//satisfy spring constraints
			//linear springs--------------------------------------------------------------------
			if(g_useLinearStiffness)
			{
				strides[0] = sizeof(HairVertex);
				strides[1] = sizeof(float);

				//first batch of linear springs
				buffers[0] = g_MasterStrandVB[g_currentVB];
				buffers[1] = g_MasterStrandLinearSpringLengthsVB;
				IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandSpringConstraintsIL);
				IRenderer::GetRendererInstance()->setVertexBuffer(0, 2, buffers, strides, offsets);
				IRenderer::GetRendererInstance()->SOSetTargets(1, &g_MasterStrandVB[(g_currentVB+1)%2], &offset);
				IRenderer::GetRendererInstance()->setIndexBuffer(g_MasterStrandSimulation1IB, FORMAT_R16_UINT, 0);
				IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_LINES);
				g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("SatisfySpringConstraints")->Apply(0);
				IRenderer::GetRendererInstance()->DrawIndexed(g_numIndicesFirst*2.0, 0, 0);
				IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
				g_currentVB = (g_currentVB+1)%2;

				//second batch of linear springs
				buffers[0] = g_MasterStrandVB[g_currentVB];
				buffers[1] = g_MasterStrandLinearSpringLengthsVB;
				IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandSpringConstraintsIL);
				IRenderer::GetRendererInstance()->setVertexBuffer(0, 2, buffers, strides, offsets);
				IRenderer::GetRendererInstance()->SOSetTargets(1, &g_MasterStrandVB[(g_currentVB+1)%2], &offset);
				IRenderer::GetRendererInstance()->setIndexBuffer(g_MasterStrandSimulation2IB, FORMAT_R16_UINT, 0);
				IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_LINES);
				g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("SatisfySpringConstraints")->Apply(0);
				IRenderer::GetRendererInstance()->DrawIndexed(g_numIndicesSecond*2.0, 0, 0);
				IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
				g_currentVB = (g_currentVB+1)%2;
			}

			//angular springs--------------------------------------------------------------------
			if(g_useAngularStiffness)
			{
				strides[0] = sizeof(HairVertex);
				strides[1] = sizeof(D3DXFROMWINEVECTOR2);

				//first batch of angular-linear springs
				buffers[0] = g_MasterStrandVB[g_currentVB];
				buffers[1] = g_MasterStrandAngularSpringConstraintsVB;
				IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandAngularSpringConstraintsIL);
				IRenderer::GetRendererInstance()->setVertexBuffer(0, 2, buffers, strides, offsets);
				IRenderer::GetRendererInstance()->setIndexBuffer(g_MasterStrandSimulationAngular1IB, FORMAT_R16_UINT, 0);
				IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_LINES /* _ADJ */);
				IRenderer::GetRendererInstance()->SOSetTargets(1, &g_MasterStrandVB[(g_currentVB+1)%2], &offset);
				g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("SatisfyAngularSpringConstraints")->Apply(0);
				IRenderer::GetRendererInstance()->DrawIndexed(g_numIndicesFirstAngular*4.0, 0, 0);
				IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
				g_currentVB = (g_currentVB+1)%2;

				//second batch of angular-linear springs
				buffers[0] = g_MasterStrandVB[g_currentVB];
				buffers[1] = g_MasterStrandAngularSpringConstraintsVB;
				IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandAngularSpringConstraintsIL);
				IRenderer::GetRendererInstance()->setVertexBuffer(0, 2, buffers, strides, offsets);
				IRenderer::GetRendererInstance()->SOSetTargets(1, &g_MasterStrandVB[(g_currentVB+1)%2], &offset);
				IRenderer::GetRendererInstance()->setIndexBuffer(g_MasterStrandSimulationAngular2IB, FORMAT_R16_UINT, 0);
				IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_LINES /* _ADJ */);
				IRenderer::GetRendererInstance()->SOSetTargets(1, &g_MasterStrandVB[(g_currentVB+1)%2], &offset);
				g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("SatisfyAngularSpringConstraints")->Apply(0);
				IRenderer::GetRendererInstance()->DrawIndexed(g_numIndicesSecondAngular*4.0, 0, 0);
				IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
				g_currentVB = (g_currentVB+1)%2;
			}

			if( g_collisions)
			{
				
				//collisions-------------------------------------------------------------------------------------
				//satisfy collision constraints
				IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandIL);
				IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &g_MasterStrandVB[g_currentVB], &stride, &offset);
				IRenderer::GetRendererInstance()->SOSetTargets(1, &g_MasterStrandVB[(g_currentVB+1)%2], &offset);
				IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_POINTS);
				g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("SatisfyCollisionConstraints")->Apply(0);
				IRenderer::GetRendererInstance()->Draw(g_NumMasterStrandControlVertices, 0);
				IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
				g_currentVB = (g_currentVB+1)%2;
				
			}            
		}
	}

    return hr;
}

//--------------------------------------------------------------------------------------
//main loop
//--------------------------------------------------------------------------------------


void OnFrameRender(double fTime, float fElapsedTime)
{
	int hr;

	static bool s_firstFrame=true;

	//get the depth stencil and render target
	TextureID  pRTV = DXUTGetD3D11RenderTargetView();
	TextureID  pDSV = DXUTGetD3D11DepthStencilView();
	IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &pRTV , pDSV ); 
	D3DFROMWINEVIEWPORT9 viewport;
	unsigned int nViewports = 1;
	IRenderer::GetRendererInstance()->RSGetViewports(&nViewports, &viewport);


	if (g_Reset)
		resetScene();

	float clearColor[] = { 0.3176f, 0.3176f, 0.3176f, 0.0f  };
#ifdef SUPERSAMPLE
	IRenderer::GetRendererInstance()->ClearRenderTargetView(g_pSceneRTV2X, clearColor);
	IRenderer::GetRendererInstance()->ClearDepthStencilView(g_pSceneDSV2X, D3D11_CLEAR_DEPTH, 1.0, 0);
#else
	IRenderer::GetRendererInstance()->ClearRenderTargetView(pRTV, clearColor);
	IRenderer::GetRendererInstance()->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

#endif

	if (g_reloadEffect)
		reloadEffect();


	// If the settings dialog is being shown, then
	//  render it instead of rendering the app's scene
	if( g_D3DSettingsDlg.IsActive() )
	{
		g_D3DSettingsDlg.OnRender( fElapsedTime );
		return;
	}

	if(g_playTransforms)
		PlayTransforms();

	IRenderer::GetRendererInstance()->OMSetRenderTargets( 0, 0 , 0 ); 

	//animate the mesh------------------------------------------------------------------------

	D3DXFROMWINEMATRIX HairTransform;
	D3DXFROMWINEMatrixIdentity(&HairTransform);

	//mouse transformations for scalp and hair
	if(g_firstFrame)
	{
		D3DXFROMWINEMatrixIdentity(&g_InitialTotalTransform);
		D3DXFROMWINEMatrixIdentity(&g_InvInitialTotalTransform); 
		D3DXFROMWINEMatrixIdentity(&g_InvInitialRotationScale);  
	}
	if(g_UseTransformations)
	{
		g_TotalTransform *= g_Transform;
		g_UseTransformations = false;
	}
	else
		D3DXFROMWINEMatrixIdentity(&g_Transform);

	g_currentHairTransform = g_TotalTransform;	

	//-----------------------------------------------------------------------------------
	// View and projection matrices
	D3DXFROMWINEMATRIX projection;
	D3DXFROMWINEMATRIX view;
	D3DXFROMWINEVECTOR3 eyePosition;
	// Get the projection & view matrix from the camera class
	projection = *g_Camera.GetProjMatrix();
	if(g_useSkinnedCam)
	{
		view = g_ExternalCameraView;
		eyePosition = g_ExternalCameraEye;
	}
	else
	{
		view = *g_Camera.GetViewMatrix();
		eyePosition = *g_Camera.GetEyePt();
	}

	// View projection matrix
	D3DXFROMWINEMATRIX viewProjection;
	viewProjection = view * projection;

	//mesh world matrix; setting to identity currently
	D3DXFROMWINEMATRIX meshWorld;
	D3DXFROMWINEMatrixIdentity(&meshWorld);

	D3DXFROMWINEMATRIX transformViewProjection;


	D3DXFROMWINEVECTOR3 old_center = D3DXFROMWINEVECTOR3(0,0,0);
	D3DXFROMWINEVec3TransformCoord(&g_Center, &old_center, &g_TotalTransform);
	D3DXFROMWINEMATRIX currentHairTransformInverse;
	D3DXFROMWINEMatrixInverse(&currentHairTransformInverse, 0, &g_currentHairTransform);
	(g_Effect->GetVariableByName("currentHairTransformation")->AsMatrix()->SetMatrix(g_currentHairTransform));
	(g_Effect->GetVariableByName("currentHairTransformationInverse")->AsMatrix()->SetMatrix(currentHairTransformInverse));
	D3DXFROMWINEMATRIX WorldViewProjection = HairTransform * viewProjection; 
	transformViewProjection = g_TotalTransform*viewProjection;
	(g_Effect->GetVariableByName("RootTransformation")->AsMatrix()->SetMatrix(g_RootTransform));
	(g_Effect->GetVariableByName("HairToWorldTransform")->AsMatrix()->SetMatrix(g_InitialTotalTransform));
	(g_Effect->GetVariableByName("TotalTransformation")->AsMatrix()->SetMatrix(g_TotalTransform));
	D3DXFROMWINEVECTOR3 transformedEyePosition;//transform the eye position from world space into the hair space
	D3DXFROMWINEVec3TransformCoord(&transformedEyePosition, &eyePosition, &g_InvInitialTotalTransform);

	D3DXFROMWINEVECTOR3 lightVector = g_HairShadows.GetLightWorldDir();

	D3DXFROMWINEVECTOR3 transformedLight;
	D3DXFROMWINEVec3TransformCoord(&transformedLight,&lightVector,&g_InvInitialRotationScale);
	D3DXFROMWINEVec3Normalize(&transformedLight,&transformedLight);
	(g_Effect->GetVariableByName("vLightDirObjectSpace")->AsVector()->SetFloatVector(transformedLight));

	//set all the collision transform matrices
	ExtractAndSetCollisionImplicitMatrices();



	// Effect variables
	(g_Effect->GetVariableByName("ViewProjection")->AsMatrix()->SetMatrix(viewProjection));
	(g_Effect->GetVariableByName("TransformedEyePosition")->AsVector()->SetFloatVector(transformedEyePosition));


	D3DFROMWINEVIEWPORT9 collisionsViewport;
	collisionsViewport.TopLeftX = 0;
	collisionsViewport.TopLeftY = 0;
	collisionsViewport.MinDepth = 0;
	collisionsViewport.MaxDepth = 1;
	collisionsViewport.Width = MAX_INTERPOLATED_HAIR_PER_WISP;  
	collisionsViewport.Height = g_NumWisps; 


	g_Effect->GetVariableByName("TimeStep")->AsScalar()->SetFloat(g_timeStep);


	//------------------------------------------------------------------------------------
	//render the wind directional control, and update the value of the wind vector
	//------------------------------------------------------------------------------------

	(g_Effect->GetVariableByName("ViewProjection")->AsMatrix()->SetMatrix(viewProjection));

#ifdef SUPERSAMPLE
	IRenderer::GetRendererInstance()->RSSetViewports(1,&g_superSampleViewport);
	IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &g_pSceneRTV2X , g_pSceneDSV2X ); 
#else
	IRenderer::GetRendererInstance()->OMSetRenderTargets(  1, &pRTV , pDSV ); 
#endif

	g_windVector = g_windDirectionWidget.GetLightDirection();
	D3DXFROMWINEVec3Normalize(&g_windVector,&g_windVector);
	D3DXFROMWINEVECTOR3 windNormVector = g_windVector;
	g_windVector *= g_WindForce;
	(g_Effect->GetVariableByName("windForce")->AsVector()->SetFloatVector((float*)&g_windVector));

	D3DXFROMWINEVECTOR3 vWindForce = g_windVector * g_windDirectionWidget.GetRadius()*10;
	float greenColor[4] = {0,1,0,1};
	g_ArrowColorShaderVariable->SetFloatVector(greenColor);
	RenderArrow( IRenderer::GetRendererInstance(), vWindForce);

	//render arrow for the light
	float whiteColor[4] = {1,1,1,1};
	g_ArrowColorShaderVariable->SetFloatVector(whiteColor);
	RenderArrow( IRenderer::GetRendererInstance(), g_HairShadows.GetLightWorldDir());

	//-------------------------------------------------------------------------------------------
	// Render the mesh, just for setting depth to enable better zculling for the hair later
	// the actual face mesh is rendered later on because we would possibly like to cast shadows from the simulated hair onto it
	//-------------------------------------------------------------------------------------------

	if(g_RenderScalp && g_RenderScene)
	{ 

		(g_Effect->GetVariableByName("ViewProjection")->AsMatrix()->SetMatrix(transformViewProjection));
		IRenderer::GetRendererInstance()->setVertexFormat(0, g_RenderMeshIL);
		g_UseScalpTextureShaderVariable->SetBool(false);
		g_BaseColorShaderVariable->SetFloatVector(skinColor);
		g_Effect->GetTechniqueByName("RenderMesh")->GetPassByName("RenderMeshDepthPass")->Apply(0);
		g_FaceMesh->Render();
		g_Scalp->Render();
	}


	IRenderer::GetRendererInstance()->OMSetRenderTargets( 0, 0 , 0 );
	(g_Effect->GetVariableByName("ViewProjection")->AsMatrix()->SetMatrix(viewProjection));


	//------------------------------------------------------------------------------------
	//render the collision spheres for debugging
	if(g_RenderCollsionImplicits)
		for(int i=0;i<g_numSpheres;i++)
		{

	#ifdef SUPERSAMPLE
			IRenderer::GetRendererInstance()->RSSetViewports(1,&g_superSampleViewport);
			IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &g_pSceneRTV2X , g_pSceneDSV2X ); 
	#else
			IRenderer::GetRendererInstance()->OMSetRenderTargets(  1, &pRTV , pDSV ); 
	#endif

			//this technique uses the sphere transforms, the hairToWorld matrix, and the viewProjection matrix
			g_Effect->GetVariableByName("currentCollisionSphere")->AsScalar()->SetInt(i); 

			g_Effect->GetTechniqueByName("RenderCollisionObjects")->GetPassByIndex(0)->Apply(0);
			g_Sphere.Render();
		}

	//---------------------------------------------------------------------------------------
	// call simulation code
    //---------------------------------------------------------------------------------------

	//World matrix for grid
	D3DXFROMWINEMATRIX gridScale;
	float diameter = (g_maxHairLengthHairSpace+g_scalpMaxRadiusHairSpace)*2.0;
	D3DXFROMWINEMatrixScaling(&gridScale, diameter, diameter, diameter ); 
	D3DXFROMWINEMATRIX gridWorld = gridScale*g_currentHairTransform;

	//World View projection matrix for grid
	D3DXFROMWINEMATRIX worldViewProjectionGrid;
	worldViewProjectionGrid = gridWorld * viewProjection;

	D3DXFROMWINEMatrixInverse(&g_gridWorldInv, 0, &gridWorld);
	D3DXFROMWINEMATRIX objToVolumeXForm = g_gridWorldInv;

	(g_Effect->GetVariableByName("WorldToGrid")->AsMatrix()->SetMatrix(g_gridWorldInv));
	(g_Effect->GetVariableByName("GridToWorld")->AsMatrix()->SetMatrix(gridWorld));

	unsigned int stride = sizeof(HairVertex);
	unsigned int offset = 0;

	//voxelize the collision obstacles into the grid
	if(g_simulate || g_firstMovedFrame)
	    g_HairGrid->voxelizeObstacles();

    if(g_simulate)
	    StepHairSimulation();

	//------------------------------------------------------------------------------------------------------------------

	//add the hair to the density grid if we need it for display, or for the wind forces in the next time step
	if(g_renderDensityOnHair || g_applyHForce)
	{
		//note: add in the g_MasterStrandSB option here, or get rid of voxelization all together..
		g_Effect->GetVariableByName("g_MasterStrand")->AsShaderResource()->SetResource(g_MasterStrandRV[g_currentVB]);
		g_HairGrid->AddHairDensity(objToVolumeXForm,&renderInstancedInterpolatedHairForGrid);
	}
	if(g_renderDensityOnHair)
	{
		g_HairGrid->demultiplexTexture();
	}


	IRenderer::GetRendererInstance()->RSSetViewports(1,&viewport);
	(g_Effect->GetVariableByName("TransformedEyePosition")->AsVector()->SetFloatVector(transformedEyePosition));

	//Tessellate master strands--------------------------------------------------------------------------------

	if( (g_simulate || g_firstMovedFrame) && g_InterpolateModel != NO_HAIR)
	{
		stride = sizeof(HairAdjacencyVertex);
		if(g_useCS)
		{
		    g_Effect->GetVariableByName("g_MasterStrandSB")->AsShaderResource()->SetResource(g_MasterStrandUAB_SRV);                   
		    g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("TessellateSB")->Apply(0);
		}
		else
		{
			g_Effect->GetVariableByName("g_MasterStrand")->AsShaderResource()->SetResource(g_MasterStrandRV[g_currentVB]);                   
 			g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("Tessellate")->Apply(0);
		}

		IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandTesselatedInputIL);
		IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &g_TessellatedMasterStrandVBInitial, &stride, &offset);
		exBufferID  SO_buffers[] = { g_TessellatedMasterStrandVB, g_TessellatedTangentsVB };
		unsigned int offsets[] = { 0, 0 };
		IRenderer::GetRendererInstance()->SOSetTargets(2, SO_buffers, offsets);
		IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_POINTS);
		IRenderer::GetRendererInstance()->Draw(g_NumTessellatedMasterStrandVertices, 0); 

		IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
		if(g_useCS)
		{
		    g_Effect->GetVariableByName("g_MasterStrandSB")->AsShaderResource()->SetResource(0);                   
		    g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("TessellateSB")->Apply(0);
		}
		else
		{
			g_Effect->GetVariableByName("g_MasterStrand")->AsShaderResource()->SetResource(0);                   
 			g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("Tessellate")->Apply(0);
		}

	}	
	//tessellate the coordinate frames--------------------------------------------------------------------------------
	if( (g_simulate || g_firstMovedFrame) && g_InterpolateModel != NO_HAIR)
	{
		stride = sizeof(HairAdjacencyVertex);
		g_Effect->GetVariableByName("g_TessellatedMasterStrand")->AsShaderResource()->SetResource(g_TessellatedMasterStrandRV);
       
		if(g_useCS)
		{
		    g_Effect->GetVariableByName("g_coordinateFramesSB")->AsShaderResource()->SetResource(g_CoordinateFrameUAB_SRV);
		    g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("TessellateCoordinateFramesSB")->Apply(0);
		}
		else
		{
		    g_Effect->GetVariableByName("g_coordinateFrames")->AsShaderResource()->SetResource(g_CoordinateFrameRV[g_currentVBCoordinateFrame]);
		    g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("TessellateCoordinateFrames")->Apply(0);
		}

		IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandTesselatedInputIL);
		IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &g_TessellatedMasterStrandVBInitial, &stride, &offset);
		IRenderer::GetRendererInstance()->SOSetTargets(1, &g_TessellatedCoordinateFrameVB, &offset);
		IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_POINTS);
		IRenderer::GetRendererInstance()->Draw(g_NumTessellatedMasterStrandVertices, 0);

		IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
		g_Effect->GetVariableByName("g_TessellatedMasterStrand")->AsShaderResource()->SetResource(0);
		if(g_useCS)
		{
		    g_Effect->GetVariableByName("g_coordinateFramesSB")->AsShaderResource()->SetResource(0);
		    g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("TessellateCoordinateFramesSB")->Apply(0);
		}
		else
		{
		    g_Effect->GetVariableByName("g_coordinateFrames")->AsShaderResource()->SetResource(0);
		    g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("TessellateCoordinateFrames")->Apply(0);
		}
	}
	//tesssellate the hair lengths--------------------------------------------------------------------------------------
	if( (g_simulate || g_firstMovedFrame) && g_InterpolateModel != NO_HAIR)
	{
		stride = sizeof(HairAdjacencyVertex);

		IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_POINTS);
		IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandTesselatedInputIL);
		IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &g_TessellatedMasterStrandVBInitial, &stride, &offset);
		IRenderer::GetRendererInstance()->SOSetTargets(1, &g_TessellatedMasterStrandLengthToRootVB, &offset);
		g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("TessellateLengths")->Apply(0);
		IRenderer::GetRendererInstance()->Draw(g_NumTessellatedMasterStrandVertices, 0);
		IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);		
	}

	//-------------------------------------------------------------------------------------------------------------------
	//find which of the interpolated strands in the multistrand interpolation are colliding with obstacles
	//render to a texture. see Notes 1 above for details.

	if(g_firstMovedFrame || (g_simulate && (g_InterpolateModel==MULTI_HYBRID || g_InterpolateModel==HYBRID)))
	{
		//set the viewport and render target
		IRenderer::GetRendererInstance()->RSSetViewports(1,&collisionsViewport);
		float clearValue[4] = {1000, 1000, 1000, 1000 };
		IRenderer::GetRendererInstance()->ClearRenderTargetView( g_CollisionsRTV, clearValue );
		IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &g_CollisionsRTV , 0 ); 

		RenderInterpolatedHair( IRenderer::GetRendererInstance(), INSTANCED_INTERPOLATED_COLLISION, MULTISTRAND, g_totalMStrands, g_totalSStrands);

		//restore the viewport and render targets
		IRenderer::GetRendererInstance()->OMSetRenderTargets( 0, 0, 0 );
		IRenderer::GetRendererInstance()->RSSetViewports(1,&viewport);
	}

	//-------------------------------------------------------------------------------------------------
	//streamout all the calculated attributes to buffers so that we dont have to keep calculating them

	if(g_useSOInterpolatedAttributes && (g_firstMovedFrame || g_simulate))
	{
		IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandIL);
		if(g_InterpolateModel==HYBRID )
		{
			RenderInterpolatedHair( IRenderer::GetRendererInstance(), SOATTRIBUTES, MULTI_HYBRID, g_totalMStrands, g_totalSStrands); 
			RenderInterpolatedHair( IRenderer::GetRendererInstance(), SOATTRIBUTES, SINGLESTRAND, g_totalMStrands, g_totalSStrands); 
		}
		else
		{
			RenderInterpolatedHair( IRenderer::GetRendererInstance(), SOATTRIBUTES,g_InterpolateModel,g_totalMStrands, g_totalSStrands); 
		}
	}

	//render hair to a  shadow map--------------------------------------------------------------------------

	if( g_renderShadows )
	{
		(g_Effect->GetVariableByName("ViewProjection")->AsMatrix()->SetMatrix(g_HairShadows.GetLightWorldViewProj()));
		(g_Effect->GetVariableByName("TransformedEyePosition")->AsVector()->SetFloatVector(g_HairShadows.GetLightCenterWorld()));
		IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandIL);
		g_HairShadows.BeginShadowMapRendering();
		{
			if(g_InterpolateModel==HYBRID)
			{
				RenderInterpolatedHair( IRenderer::GetRendererInstance(), INSTANCED_DEPTH, MULTI_HYBRID, g_totalMStrands, g_totalSStrands); 
				RenderInterpolatedHair( IRenderer::GetRendererInstance(), INSTANCED_DEPTH, SINGLESTRAND, g_totalMStrands, g_totalSStrands); 
			}
			else
				RenderInterpolatedHair( IRenderer::GetRendererInstance(), INSTANCED_DEPTH, g_InterpolateModel,g_totalMStrands, g_totalSStrands); 
		}
		if(g_RenderScalp  && g_RenderScene) 
		{
			// unset hull and domain shaders
			g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("AllNulls")->Apply(0);
			IRenderer::GetRendererInstance()->setVertexFormat(0, g_RenderMeshIL);
			g_Effect->GetTechniqueByName("RenderMeshDepth")->GetPassByIndex(0)->Apply(0);
			g_FaceMesh->Render();
		}
		g_HairShadows.EndShadowMapRendering();

	}//end if(g_renderShadows)

	(g_Effect->GetVariableByName("ViewProjection")->AsMatrix()->SetMatrix(WorldViewProjection));
	(g_Effect->GetVariableByName("TransformedEyePosition")->AsVector()->SetFloatVector(transformedEyePosition));
	IRenderer::GetRendererInstance()->RSSetViewports( 1, &viewport );

#ifdef SUPERSAMPLE

	//set bigger Render Target and Viewport
	IRenderer::GetRendererInstance()->RSSetViewports(1,&g_superSampleViewport);
	IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &g_pSceneRTV2X , g_pSceneDSV2X ); 
#else
	IRenderer::GetRendererInstance()->OMSetRenderTargets(  1, &pRTV , pDSV ); 
#endif

	if (g_VisShadowMap)
	{
		g_HairShadows.VisualizeShadowMap();
		return;
	}

	(g_Effect->GetVariableByName("ViewProjection")->AsMatrix()->SetMatrix(WorldViewProjection));
	(g_Effect->GetVariableByName("TransformedEyePosition")->AsVector()->SetFloatVector(transformedEyePosition));


#ifdef SUPERSAMPLE

	//set bigger Render Target and Viewport
	IRenderer::GetRendererInstance()->RSSetViewports(1,&g_superSampleViewport);
	IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &g_pSceneRTV2X , g_pSceneDSV2X ); 
#else
	IRenderer::GetRendererInstance()->OMSetRenderTargets(  1, &pRTV , pDSV ); 
#endif

	// unset hull and domain shaders
	g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("AllNulls")->Apply(0);
	IRenderer::GetRendererInstance()->setVertexFormat(0, g_RenderMeshIL);
	g_Effect->GetTechniqueByName("RenderMeshDepth")->GetPassByIndex(0)->Apply(0);

	//render the floor--------------------------------------------------------------------------------------

	if(g_RenderFloor && g_RenderScene)
	{
		D3DXFROMWINEVECTOR3 lightVector = g_HairShadows.GetLightWorldDir();
		D3DXFROMWINEVECTOR3 transformedLight;
		D3DXFROMWINEVec3TransformCoord(&transformedLight,&lightVector,&g_InvInitialRotationScale);
		D3DXFROMWINEVec3Normalize(&transformedLight,&transformedLight);
		D3DXFROMWINEVECTOR3 lightPos = g_Center - lightVector;

		(g_Effect->GetVariableByName("ViewProjection")->AsMatrix()->SetMatrix(viewProjection));
		(g_Effect->GetVariableByName("vLightPos")->AsVector()->SetFloatVector(lightPos));
		DrawPlane();
		(g_Effect->GetVariableByName("ViewProjection")->AsMatrix()->SetMatrix(transformViewProjection));
	}

	//render the face/scalp, possibly with shadows-----------------------------------------------------------

	if(g_RenderScalp && g_RenderScene)
	{
		
		(g_Effect->GetVariableByName("ViewProjection")->AsMatrix()->SetMatrix(transformViewProjection));
		IRenderer::GetRendererInstance()->setVertexFormat(0, g_RenderMeshIL);
		g_UseScalpTextureShaderVariable->SetBool(false);
		g_BaseColorShaderVariable->SetFloatVector(skinColor);
		g_HairShadows.SetHairShadowTexture();
		g_Effect->GetTechniqueByName("RenderMesh")->GetPassByName("RenderWithShadows")->Apply(0);
		g_FaceMesh->Render();
		g_HairShadows.UnSetHairShadowTexture();
		g_Effect->GetTechniqueByName("RenderMesh")->GetPassByName("RenderWithShadows")->Apply(0);
		
		(g_Effect->GetVariableByName("ViewProjection")->AsMatrix()->SetMatrix(transformViewProjection));
		IRenderer::GetRendererInstance()->setVertexFormat(0, g_RenderMeshIL);
		g_UseScalpTextureShaderVariable->SetBool(true);
		g_BaseColorShaderVariable->SetFloatVector(hairShadingParams[g_currHairShadingParams].m_baseColor);
		g_HairShadows.SetHairShadowTexture();
		g_Effect->GetTechniqueByName("RenderMesh")->GetPassByName("RenderWithoutShadows")->Apply(0);
		g_Scalp->Render();
		g_HairShadows.UnSetHairShadowTexture();
		g_Effect->GetTechniqueByName("RenderMesh")->GetPassByName("RenderWithoutShadows")->Apply(0);
	}

	(g_Effect->GetVariableByName("ViewProjection")->AsMatrix()->SetMatrix(WorldViewProjection));
	(g_Effect->GetVariableByName("TransformedEyePosition")->AsVector()->SetFloatVector(transformedEyePosition));


	//render the hair in the scene------------------------------------------------------------------------
	IRenderer::GetRendererInstance()->setVertexFormat(0, g_MasterStrandIL);
	g_BaseColorShaderVariable->SetFloatVector(hairShadingParams[g_currHairShadingParams].m_baseColor);
	g_SpecColorShaderVariable->SetFloatVector(hairShadingParams[g_currHairShadingParams].m_specColor);

	if(g_renderDensityOnHair)
	{
		if(g_InterpolateModel==HYBRID)
		{
			RenderInterpolatedHair( IRenderer::GetRendererInstance(), INSTANCED_DENSITY, MULTI_HYBRID, g_totalMStrands, g_totalSStrands); 
			RenderInterpolatedHair( IRenderer::GetRendererInstance(), INSTANCED_DENSITY, SINGLESTRAND, g_totalMStrands, g_totalSStrands); 
		}
		else
			RenderInterpolatedHair( IRenderer::GetRendererInstance(), INSTANCED_DENSITY,g_InterpolateModel,g_totalMStrands, g_totalSStrands); 
	}
	else
	{	
		if(g_InterpolateModel==HYBRID)
		{
			RenderInterpolatedHair( IRenderer::GetRendererInstance(), INSTANCED_NORMAL_HAIR, MULTI_HYBRID,g_totalMStrands, g_totalSStrands); 
			RenderInterpolatedHair( IRenderer::GetRendererInstance(), INSTANCED_NORMAL_HAIR,SINGLESTRAND,g_totalMStrands, g_totalSStrands);
		}
		else
		{
			RenderInterpolatedHair( IRenderer::GetRendererInstance(), INSTANCED_NORMAL_HAIR,g_InterpolateModel,g_totalMStrands, g_totalSStrands); 
		}

	}



	// apply all changes
	g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateAndRenderM_HardwareTess")->Apply(0);
	// unset hull and domain shaders
	g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("AllNulls")->Apply(0);

	// unset shadow map
	g_Effect->GetVariableByName("tShadowMap")->AsShaderResource()->SetResource(0);

	//--------------------------------------------------------------------------------------------------
#ifdef SUPERSAMPLE
	//resolve the MS surface and set it to the fx file-------------------------------------------------------
	IRenderer::GetRendererInstance()->ResolveSubresource(g_pSceneColorBuffer2X, 0, g_pSceneColorBuffer2XMS, 0, FORMAT_RGBA8_UNORM);
	g_SceneColor2XShaderResourceVariable->SetResource(g_pSceneColorBufferShaderResourceView);

	//draw a full screen quad to the backbuffer--------------------------------------------------------------
	IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &pRTV , pDSV ); 
	IRenderer::GetRendererInstance()->RSSetViewports(1,&viewport);

	g_Effect->GetVariableByName("SStextureWidth")->AsScalar()->SetFloat(g_Width*g_SSFACTOR);
	g_Effect->GetVariableByName("SStextureHeight")->AsScalar()->SetFloat(g_Height*g_SSFACTOR);
	g_Effect->GetTechniqueByName("DrawTexture")->GetPassByName( "ResolveSuperSample" )->Apply(0);       
	RenderScreenQuad(g_ScreenQuadBuffer);	   
	g_SceneColor2XShaderResourceVariable->SetResource(0);	   
	g_Effect->GetTechniqueByName("DrawTexture")->GetPassByName( "ResolveSuperSample" )->Apply(0);
#endif

	//--------------------------------------------------------------------------------------------------
	//visualize the coordinate frames
	if(g_bVisualizeCFs)
		VisualizeCoordinateFrames( IRenderer::GetRendererInstance(), g_bVisualizeTessellatedCFs);

	//--------------------------------------------------------------------------------------------------

	IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &pRTV , pDSV ); 
	IRenderer::GetRendererInstance()->RSSetViewports(1,&viewport);


	g_firstFrame = false;
	g_firstMovedFrame = false;
	IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &pRTV , pDSV ); 
	IRenderer::GetRendererInstance()->RSSetViewports(1,&viewport);

    if(g_simulate)
		g_RootTransform = g_Transform;
	else 
		g_RootTransform = g_RootTransform * g_Transform;
    
	g_bApplyAdditionalTransform = false;
}


void reloadEffect()
{
	g_reloadEffect = false;
	//////////////////SAFE_RELEASE(g_Effect);
	InitEffect();
	g_HairGrid->ReloadShader(g_Effect,g_pEffectPath);

	//initialize the fluid again
	SAFE_DELETE(g_fluid);
	//////////////////SAFE_RELEASE(g_pVelTexture3D);
	//////////////////SAFE_RELEASE(g_FluidVelocityRV);
	InitializeFluidState();
}

int resetScene()
{
	int hr = 0;

	g_firstMovedFrame = true;
	g_firstFrame = true;
	g_hairReset = true;
	D3DXFROMWINEMatrixIdentity(&g_Transform);
	D3DXFROMWINEMatrixIdentity(&g_TotalTransform);

	g_playTransforms = false;
    g_SampleUI.GetCheckBox( IDC_PLAY_TRANSFORMS )->SetChecked(g_playTransforms);
	(g_Effect->GetVariableByName("TotalTransformation")->AsMatrix()->SetMatrix(g_TotalTransform));

#ifdef SUPERSAMPLE
    ReinitSuperSampleBuffers();
#endif

	LoadMayaHair(g_loadDirectory, g_useShortHair );
	LoadCollisionObstacles();
	CreateBuffers();
	g_Effect->GetVariableByName("g_TessellatedMasterStrandLengthMax")->AsScalar()->SetInt(g_TessellatedMasterStrandLengthMax);
	g_Effect->GetVariableByName("g_NumTotalWisps")->AsScalar()->SetInt(g_NumWisps);
	g_Reset = false;

	return hr;
}



void PlayTransforms()
{
    if(g_loopTransforms && g_transformsFileIn.eof())
	{
		g_transformsFileIn.close();
		g_transformsFileIn.clear();
		g_transformsFileIn.open (g_tstr_animations[g_animationIndex]);
	}

	if(	g_transformsFileIn && !g_transformsFileIn.eof() )
	{
		//char c;
		//g_transformsFileIn>>c;
		float v00,v01,v02,v03,
			v10,v11,v12,v13,
			v20,v21,v22,v23,
			v30,v31,v32,v33;
		g_transformsFileIn>>v00>>v01>>v02>>v03
			>>v10>>v11>>v12>>v13
			>>v20>>v21>>v22>>v23
			>>v30>>v31>>v32>>v33;
		g_Transform = D3DXFROMWINEMATRIX(v00,v01,v02,v03,
			v10,v11,v12,v13,
			v20,v21,v22,v23,
			v30,v31,v32,v33);
		g_UseTransformations = true;
	}
	else
	{
		g_transformsFileIn.close();
		g_transformsFileIn.clear();
		g_playTransforms = false;
		g_UseTransformations = false;
		g_SampleUI.GetCheckBox( IDC_PLAY_TRANSFORMS )->SetChecked(g_playTransforms);

		D3DXFROMWINEMatrixIdentity(&g_TotalTransform); //clear previous transformations before playing
		g_RestoreHairStrandsToDefault = true;

		g_transformsFileIn.open (g_tstr_animations[g_animationIndex]);
	}
}

//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------

int InitApp()
{ 
	g_transformsFileIn.open (g_tstr_animations[g_animationIndex]);

    if(g_renderMStrands && g_renderSStrands) g_InterpolateModel=HYBRID;
	else if(g_renderMStrands) g_InterpolateModel=MULTI_HYBRID;
	else if(g_renderSStrands) g_InterpolateModel=SINGLESTRAND;
	else g_InterpolateModel=NO_HAIR;

	g_superSampleViewport.TopLeftX = 0;
	g_superSampleViewport.TopLeftY = 0;
	g_superSampleViewport.MinDepth = 0;
	g_superSampleViewport.MaxDepth = 1;
	g_superSampleViewport.Width = g_Width*g_SSFACTOR;  
	g_superSampleViewport.Height = g_Height*g_SSFACTOR; 

	return hr;
}

//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void OnFrameMove(double fTime, float fElapsedTime)
{
	// Update the camera's position based on user input 
	g_Camera.FrameMove(fElapsedTime);
	g_HairShadows.GetCamera().FrameMove(fElapsedTime);
	D3DXFROMWINEVECTOR3 bbExtents = D3DXFROMWINEVECTOR3((g_maxHairLengthWorld+g_scalpMaxRadiusWorld),
		(g_maxHairLengthWorld+g_scalpMaxRadiusWorld),
		(g_maxHairLengthWorld+g_scalpMaxRadiusWorld));
	g_HairShadows.UpdateMatrices(g_Center, bbExtents);


	D3DXFROMWINEVECTOR3 v = *g_Camera.GetEyePt() - g_Center;
	double fDist = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    // LOD has to be updated according to distance from camera

	//rendering / tessellation LOD
	if (g_useDX11 && g_bDynamicLOD) 
	{
		g_fNumHairsLOD = min(1., 20 / fDist); // at distance 20 or closer we will have maximum LOD
        g_fNumHairsLOD = pow(g_fNumHairsLOD,g_LODScalingFactor);
		g_Effect->GetVariableByName("g_fNumHairsLOD")->AsScalar()->SetFloat((float)g_fNumHairsLOD);
		g_SampleUI.GetSlider( IDC_NHAIRSLOD_SCALE )->SetValue(g_fNumHairsLOD * 100);

        float fNumHair = max(0.1f,g_fNumHairsLOD);
		fNumHair = min(1.0f, fNumHair);
		g_fWidthHairsLOD = 1.0f/fNumHair;
		g_Effect->GetVariableByName("g_fWidthHairsLOD")->AsScalar()->SetFloat((float)g_fWidthHairsLOD);
		g_SampleUI.GetSlider( IDC_NHAIRSWIDTHLOD_SCALE )->SetValue(g_fWidthHairsLOD);
	}

    static int frame = 0;

	//simulation LOD
    if(g_useSimulationLOD)
	{
	    int skipFrames = 1 + (int)(15.0f * max(0.0f, 0.4f - min(1.f, 20 / fDist)));
		if(frame%skipFrames == 0) g_simulate = true; else g_simulate = false;
		g_SampleUI.GetCheckBox( IDC_SIMULATE)->SetChecked(g_simulate);
		frame++;
	}
}

//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------


void changeCameraAndWindPresets()
{
    if(g_cameraAndWindPreset == 0)
    {
        g_windVector = D3DXFROMWINEVECTOR3(0.75089055f, 0.12556140f, 0.64838076f );
        g_windDirectionWidget.SetLightDirection( g_windVector );
        g_applyHForce = true;
		g_WindForce = 0.08f;
        
        D3DXFROMWINEVECTOR3 at = D3DXFROMWINEVECTOR3(0, 0, 0);
        D3DXFROMWINEVECTOR3 lightPos = D3DXFROMWINEVECTOR3(4.7908502f,18.887268f,-9.7619190f);
        g_HairShadows.GetCamera().Reset();
        g_HairShadows.GetCamera().SetViewParams(&lightPos, &at);

        D3DXFROMWINEVECTOR3 eyePos = D3DXFROMWINEVECTOR3(0.0f, -1.0f, -15.0f);
        g_Camera.SetViewParams(&eyePos, &at);
    }
    else if(g_cameraAndWindPreset == 1)
    {
        g_windVector = D3DXFROMWINEVECTOR3(0.75089055f, 0.12556140f, 0.64838076f );
        g_windDirectionWidget.SetLightDirection( g_windVector );
        g_applyHForce = true;
		g_WindForce = 0.05f;
        g_windImpulseRandomness = 0.16f;
        g_windImpulseRandomnessSmoothingInterval = 15;
        
        D3DXFROMWINEVECTOR3 at = D3DXFROMWINEVECTOR3(0, 0, 0);
        D3DXFROMWINEVECTOR3 lightPos = D3DXFROMWINEVECTOR3(2.19152f, 15.6187f, -14.4954f);
        g_HairShadows.GetCamera().Reset();
        g_HairShadows.GetCamera().SetViewParams(&lightPos, &at);

        D3DXFROMWINEVECTOR3 eyePos = D3DXFROMWINEVECTOR3(-9.81684f, -0.238484f, -15.0457f);
        g_Camera.SetViewParams(&eyePos, &at);
    }
    else if(g_cameraAndWindPreset == 2)
    {
        g_windVector = D3DXFROMWINEVECTOR3(0.0f,0.0f,1.0f);
        g_windDirectionWidget.SetLightDirection( g_windVector );
        g_applyHForce = false;
		g_WindForce = 0.08f;
        
        D3DXFROMWINEVECTOR3 at = D3DXFROMWINEVECTOR3(0, 0, 0);
        D3DXFROMWINEVECTOR3 lightPos = D3DXFROMWINEVECTOR3(-11.6984f, 16.5866f, 13.4442f);
        g_HairShadows.GetCamera().Reset();
        g_HairShadows.GetCamera().SetViewParams(&lightPos, &at);

        D3DXFROMWINEVECTOR3 eyePos = D3DXFROMWINEVECTOR3(9.0111f, -0.293344f, 14.9507f);
        g_Camera.SetViewParams(&eyePos, &at);
    }

    g_pApplyHForceShaderVariable->SetBool(g_applyHForce);
    g_SampleUI.GetCheckBox( IDC_ADDFORCES )->SetChecked(g_applyHForce); 
	g_SampleUI.GetSlider( IDC_WINDFORCE_SCALE)->SetValue( (int)(g_WindForce*100) );

    if(!g_applyHForce && g_fluid)
        g_fluid->Reset();

}

//--------------------------------------------------------------------------------------
// move the model
//--------------------------------------------------------------------------------------

void WorldToScreen(const D3DXFROMWINEVECTOR3& position, float& x, float& y)
{
	D3DXFROMWINEVECTOR3 screenPosition;
	D3DXFROMWINEMATRIX projection;
	D3DXFROMWINEMATRIX view;
	projection = *g_Camera.GetProjMatrix();
	view = *g_Camera.GetViewMatrix();
	D3DXFROMWINEMATRIX viewProjection;
	viewProjection = view * projection;

	D3DXFROMWINEVec3TransformCoord(&screenPosition, &position, &viewProjection);
	x = ( screenPosition.x + 1) * g_Width  / 2;
	y = (-screenPosition.y + 1) * g_Height / 2;
}

D3DXFROMWINEVECTOR3 ScreenToTrackballPoint(float x, float y)
{
	float scale = 4;
	x = - x / (scale * g_Width / 2);
	y =   y / (scale * g_Height / 2);
	float z = 0;
	float mag = x * x + y * y;
	if (mag > 1) {
		float scale = 1.0f / sqrtf(mag);
		x *= scale;
		y *= scale;
	}
	else
		z = sqrtf(1.0f - mag);
	return D3DXFROMWINEVECTOR3(x, y, z);
}

int ExtractAndSetCollisionImplicitMatrices()
{
	int hr = 0;

	vector<D3DXFROMWINEMATRIX> sphereTransformationMatrices;
	sphereTransformationMatrices.reserve(g_numSpheres);
	vector<D3DXFROMWINEMATRIX> sphereInverseTransformationMatrices;
	sphereInverseTransformationMatrices.reserve(g_numSpheres);

	vector<D3DXFROMWINEMATRIX> cylinderTransformationMatrices;
	cylinderTransformationMatrices.reserve(g_numCylinders);
	vector<D3DXFROMWINEMATRIX> cylinderInverseTransformationMatrices;
	cylinderInverseTransformationMatrices.reserve(g_numCylinders);

	vector<D3DXFROMWINEMATRIX> sphereNoMoveConstraintTransformationMatrices;
	sphereNoMoveConstraintTransformationMatrices.reserve(g_numSpheresNoMoveConstraint);
	vector<D3DXFROMWINEMATRIX> sphereNoMoveConstraintInverseTransformationMatrices;
	sphereNoMoveConstraintInverseTransformationMatrices.reserve(g_numSpheresNoMoveConstraint);


	for(int i=0;i<int(collisionObjectTransforms.size());i++)
	{
		D3DXFROMWINEMATRIX initialTransformation = collisionObjectTransforms.at(i).InitialTransform;
		//if this is a static object all the collision objects have the same transform
		D3DXFROMWINEMATRIX currentTransformation = g_TotalTransform;
		D3DXFROMWINEMATRIX totalTransformation; 
		totalTransformation = initialTransformation * currentTransformation;
		D3DXFROMWINEMATRIX inverseTotalTransformation;
		D3DXFROMWINEMatrixInverse(&inverseTotalTransformation, 0, &totalTransformation);
		if(collisionObjectTransforms.at(i).implicitType==SPHERE)
		{
			sphereTransformationMatrices.push_back(totalTransformation); 
			sphereInverseTransformationMatrices.push_back(inverseTotalTransformation);
		}
		else if(collisionObjectTransforms.at(i).implicitType==CYLINDER)
		{
			cylinderTransformationMatrices.push_back(totalTransformation); 
			cylinderInverseTransformationMatrices.push_back(inverseTotalTransformation);
		}
		else if(collisionObjectTransforms.at(i).implicitType==SPHERE_NO_MOVE_CONSTRAINT)
		{
			sphereNoMoveConstraintTransformationMatrices.push_back(totalTransformation);	
			sphereNoMoveConstraintInverseTransformationMatrices.push_back(inverseTotalTransformation);		
		}
	}

	//spheres
	assert(sphereTransformationMatrices.size()==(unsigned int)g_numSpheres);
	assert(sphereTransformationMatrices.size()<MAX_IMPLICITS); //make sure the number of matrices is less than that declared in fx file
	if(g_numSpheres>0)
		(g_collisionSphereTransformationsEV->SetMatrixArray((float*)&sphereTransformationMatrices[0], 0, static_cast<unsigned int>(sphereTransformationMatrices.size())));
	assert(sphereInverseTransformationMatrices.size()==(unsigned int)g_numSpheres);
	assert(sphereInverseTransformationMatrices.size()<MAX_IMPLICITS); //make sure the number of matrices is less than that declared in fx file
	if(g_numSpheres>0)
		(g_collisionSphereInverseTransformationsEV->SetMatrixArray((float*)&sphereInverseTransformationMatrices[0], 0, static_cast<unsigned int>(sphereInverseTransformationMatrices.size())));

	//cylinders
	assert(cylinderTransformationMatrices.size()==(unsigned int)g_numCylinders);
	assert(cylinderTransformationMatrices.size()<MAX_IMPLICITS); //make sure the number of matrices is less than that declared in fx file
	if(g_numCylinders>0)
		(g_collisionCylinderTransformationsEV->SetMatrixArray((float*)&cylinderTransformationMatrices[0], 0, static_cast<unsigned int>(cylinderTransformationMatrices.size())));
	assert(cylinderInverseTransformationMatrices.size()==(unsigned int)g_numCylinders);
	assert(cylinderInverseTransformationMatrices.size()<MAX_IMPLICITS); //make sure the number of matrices is less than that declared in fx file
	if(g_numCylinders>0)
		(g_collisionCylinderInverseTransformationsEV->SetMatrixArray((float*)&cylinderInverseTransformationMatrices[0], 0, static_cast<unsigned int>(cylinderInverseTransformationMatrices.size())));

	//sphere no move constraint
	assert(sphereNoMoveConstraintTransformationMatrices.size()==(unsigned int)g_numSpheresNoMoveConstraint);
	assert(sphereNoMoveConstraintTransformationMatrices.size()<MAX_IMPLICITS); //make sure the number of matrices is less than that declared in fx file
	if(g_numSpheresNoMoveConstraint>0)
		(g_SphereNoMoveImplicitTransformEV->SetMatrixArray((float*)&sphereNoMoveConstraintTransformationMatrices[0], 0, static_cast<unsigned int>(sphereNoMoveConstraintTransformationMatrices.size())));
	assert(sphereNoMoveConstraintInverseTransformationMatrices.size()==(unsigned int)g_numSpheresNoMoveConstraint);
	assert(sphereNoMoveConstraintInverseTransformationMatrices.size()<MAX_IMPLICITS); //make sure the number of matrices is less than that declared in fx file
	if(g_numSpheresNoMoveConstraint>0)
		(g_SphereNoMoveImplicitInverseTransformEV->SetMatrixArray((float*)&sphereNoMoveConstraintInverseTransformationMatrices[0], 0, static_cast<unsigned int>(sphereNoMoveConstraintInverseTransformationMatrices.size())));


    //also put these transforms in a buffer for the compute shader
    g_cbTransformsInitialData.g_NumSphereImplicits = g_numSpheres;
    g_cbTransformsInitialData.g_NumCylinderImplicits = g_numCylinders;
    g_cbTransformsInitialData.g_NumSphereNoMoveImplicits = g_numSpheresNoMoveConstraint;
    g_cbTransformsInitialData.padding = 0;

    for(unsigned int i=0;i<sphereTransformationMatrices.size();i++)
        g_cbTransformsInitialData.CollisionSphereTransformations[i] = sphereTransformationMatrices[i];
    for(unsigned int i=0;i<sphereInverseTransformationMatrices.size();i++)
         g_cbTransformsInitialData.CollisionSphereInverseTransformations[i] = sphereInverseTransformationMatrices[i];
    for(unsigned int i=0;i<cylinderTransformationMatrices.size();i++)
         g_cbTransformsInitialData.CollisionCylinderTransformations[i] = cylinderTransformationMatrices[i];
    for(unsigned int i=0;i<cylinderInverseTransformationMatrices.size();i++)
        g_cbTransformsInitialData.CollisionCylinderInverseTransformations[i] = cylinderInverseTransformationMatrices[i];
    for(unsigned int i=0;i<sphereNoMoveConstraintInverseTransformationMatrices.size();i++)
        g_cbTransformsInitialData.SphereNoMoveImplicitInverseTransform[i] = sphereNoMoveConstraintInverseTransformationMatrices[i];

	return hr;

}

int InitEffect()
{
	int hr;

	// Read the D3DXFROMWINE effect file

	//Try to read the pre-compiled file since its much faster, if that does not work read and compile the original file
	hr = DXUTFindDXSDKMediaFileCch(g_pEffectPath, MAX_PATH, "Hair.fxo");
    if (FAILED(hr))
    {
		g_Effect="Hair.fx";
    }
    else
    {
	    g_Effect=g_pEffectPath;
    }

	g_pTechniqueRenderArrow  = g_Effect->GetTechniqueByName("RenderArrow");

	( LoadComputeShader( "HairSimulateCS.hlsl", "UpdateParticlesSimulate", "cs_4_0", &g_pCSSimulateParticles) );

	//get shader variables
	g_pShadowsShaderVariable = g_Effect->GetVariableByName( "useShadows")->AsScalar();
	g_pShadowsShaderVariable->SetBool(g_renderShadows);
	g_pApplyHForceShaderVariable = g_Effect->GetVariableByName( "g_bApplyHorizontalForce")->AsScalar();
	g_pApplyHForceShaderVariable->SetBool(g_applyHForce);
	g_Effect->GetVariableByName( "g_densityThreshold")->AsScalar()->SetFloat(g_densityThreshold);
	g_pSimulateShaderVariable = g_Effect->GetVariableByName( "g_bSimulate")->AsScalar();
	g_pSimulateShaderVariable->SetBool(g_simulate);
	g_pAddGravityShaderVariable = g_Effect->GetVariableByName( "g_bAddGravity")->AsScalar();
	g_pAddGravityShaderVariable->SetBool(g_addGravity);
	g_pCurlyHairShaderVariable = g_Effect->GetVariableByName("doCurlyHair")->AsScalar();
	g_pCurlyHairShaderVariable->SetBool(g_bCurlyHair); 
	g_InterHairForcesShaderVariable = g_Effect->GetVariableByName( "g_interHairForces")->AsScalar();
	g_InterHairForcesShaderVariable->SetFloat(g_interHairForces);
	g_AngularStiffnessMultiplierShaderVariable = g_Effect->GetVariableByName("g_angularStiffness")->AsScalar();
	g_AngularStiffnessMultiplierShaderVariable->SetFloat(g_angularSpringStiffnessMultiplier);
	g_GravityStrengthShaderVariable = g_Effect->GetVariableByName("g_gravityStrength")->AsScalar();
	g_GravityStrengthShaderVariable->SetFloat(g_GravityStrength);
	g_BlurRadiusShaderVariable = g_Effect->GetVariableByName("blurRadius")->AsScalar();
	g_BlurRadiusShaderVariable->SetInt(g_BlurRadius);
	g_BlurSigmaShaderVariable = g_Effect->GetVariableByName("blurSigma")->AsScalar();
	g_BlurSigmaShaderVariable->SetFloat(g_BlurSigma);
	( g_Effect, g_pEffectPath, g_hairTextureArrayShaderVariable, "hairTextureArray", AsShaderResource );
	g_hairTextureArrayShaderVariable->SetResource( hairTextureRV );
	g_UseBlurTextureForForcesShaderVariable = g_Effect->GetVariableByName("useBlurTexture")->AsScalar(); 
	g_UseBlurTextureForForcesShaderVariable->SetBool(g_useBlurredDensityForInterHairForces); 
	g_UseGradientBasedForceShaderVariable = g_Effect->GetVariableByName("useGradientBasedForce")->AsScalar(); 
	g_currentInterHairForcesChoice==0 ? g_UseGradientBasedForceShaderVariable->SetBool(true):g_UseGradientBasedForceShaderVariable->SetBool(false);
	g_ArrowColorShaderVariable = g_Effect->GetVariableByName("arrowColor")->AsVector();
	g_BaseColorShaderVariable = g_Effect->GetVariableByName("g_baseColor")->AsVector();
	g_SpecColorShaderVariable = g_Effect->GetVariableByName("g_specColor")->AsVector();
	g_UseScalpTextureShaderVariable = g_Effect->GetVariableByName("g_useScalpTexture")->AsScalar();
	(g_Effect, g_pEffectPath, g_collisionSphereTransformationsEV, "CollisionSphereTransformations", AsMatrix);
	(g_Effect, g_pEffectPath, g_collisionSphereInverseTransformationsEV, "CollisionSphereInverseTransformations", AsMatrix);
	(g_Effect, g_pEffectPath, g_collisionCylinderTransformationsEV, "CollisionCylinderTransformations", AsMatrix);
	(g_Effect, g_pEffectPath, g_collisionCylinderInverseTransformationsEV, "CollisionCylinderInverseTransformations", AsMatrix);
	(g_Effect, g_pEffectPath, g_SphereNoMoveImplicitInverseTransformEV, "SphereNoMoveImplicitInverseTransform", AsMatrix);
	(g_Effect, g_pEffectPath, g_SphereNoMoveImplicitTransformEV, "SphereNoMoveImplicitTransform", AsMatrix);
	(g_Effect, g_pEffectPath, g_SceneColor2XShaderResourceVariable, "g_SupersampledSceneColor", AsShaderResource);
	(g_Effect, g_pEffectPath, g_SceneDepthShaderResourceVariable, "g_sceneDepth", AsShaderResource);


	g_Effect->GetVariableByName("g_SoftEdges")->AsScalar()->SetFloat(g_SoftEdges);
	g_Effect->GetVariableByName("g_SigmaA")->AsScalar()->SetFloat(-g_SigmaA/g_MaxWorldScale);
	g_Effect->GetVariableByName("g_TessellatedMasterStrandLengthMax")->AsScalar()->SetInt(g_TessellatedMasterStrandLengthMax);
	g_Effect->GetVariableByName("g_thinning")->AsScalar()->SetFloat(g_thinning);

	g_Effect->GetVariableByName("g_NumTotalWisps")->AsScalar()->SetInt(g_NumWisps);
	g_Effect->GetVariableByName("g_NumMaxStrandsPerWisp")->AsScalar()->SetInt(MAX_INTERPOLATED_HAIR_PER_WISP);
	g_Effect->GetVariableByName("g_maxLengthToRoot")->AsScalar()->SetFloat(g_maxHairLength);



	g_Effect->GetVariableByName("g_StrandCircularCoordinates")->AsShaderResource()->SetResource(g_StrandCircularCoordinatesRV);
	g_Effect->GetVariableByName("g_StrandCoordinates")->AsShaderResource()->SetResource(g_StrandCoordinatesRV);
	g_Effect->GetVariableByName("g_StrandSizes")->AsShaderResource()->SetResource(g_StrandSizesRV);
	g_Effect->GetVariableByName("g_MasterStrandRootIndices")->AsShaderResource()->SetResource(g_RootsIndicesRV);
	g_Effect->GetVariableByName("g_OriginalMasterStrandRootIndices")->AsShaderResource()->SetResource(g_rootsIndicesMasterStrandRV);
	g_Effect->GetVariableByName("g_MasterStrandLengths")->AsShaderResource()->SetResource(g_MasterLengthsRV);
	g_Effect->GetVariableByName("g_MasterStrandLengthsUntessellated")->AsShaderResource()->SetResource(g_MasterLengthsRVUntessellated);
	g_Effect->GetVariableByName("g_MasterStrandRootIndicesUntessellated")->AsShaderResource()->SetResource(g_RootsIndicesRVUntessellated); 
	g_Effect->GetVariableByName("g_Attributes")->AsShaderResource()->SetResource(g_StrandAttributesRV);
	g_Effect->GetVariableByName("g_LengthsToRoots")->AsShaderResource()->SetResource(g_MasterStrandLengthToRootRV);		
	g_Effect->GetVariableByName("g_tessellatedMasterStrandRootIndex")->AsShaderResource()->SetResource(g_TessellatedMasterStrandRootIndexRV);
	g_Effect->GetVariableByName("g_tessellatedCoordinateFrames")->AsShaderResource()->SetResource(g_TessellatedCoordinateFrameRV);	
	g_Effect->GetVariableByName("g_StrandLengths")->AsShaderResource()->SetResource(g_StrandLengthsRV);
	g_Effect->GetVariableByName("g_StrandColors")->AsShaderResource()->SetResource(g_StrandColorsRV);
	g_Effect->GetVariableByName("g_strandDeviations")->AsShaderResource()->SetResource(g_StrandDeviationsRV);
	g_Effect->GetVariableByName("g_curlDeviations")->AsShaderResource()->SetResource(g_CurlDeviationsRV);
	g_Effect->GetVariableByName("g_OriginalMasterStrand")->AsShaderResource()->SetResource(g_OriginalMasterStrandRV);
	g_Effect->GetVariableByName("g_OriginalVectors")->AsShaderResource()->SetResource(g_originalVectorsRV);
	g_Effect->GetVariableByName("g_VertexNumber")->AsShaderResource()->SetResource(g_MasterStrandLocalVertexNumberRV);
	g_Effect->GetVariableByName("g_TangentJitter")->AsShaderResource()->SetResource(g_TangentJitterRV);
	g_Effect->GetVariableByName("g_Lengths")->AsShaderResource()->SetResource(g_MasterStrandLengthsRV);


	//collisions
	g_Effect->GetVariableByName("g_NumSphereImplicits")->AsScalar()->SetInt(g_numSpheres);
	g_Effect->GetVariableByName("g_NumCylinderImplicits")->AsScalar()->SetInt(g_numCylinders);
	g_Effect->GetVariableByName("g_NumSphereNoMoveImplicits")->AsScalar()->SetInt(g_numSpheresNoMoveConstraint);

	if(g_numSpheres>0)
		g_Effect->GetVariableByName("g_UseSphereForBarycentricInterpolant")->AsScalar()->SetIntArray(&sphereIsBarycentricObstacle[0],0,g_numSpheres);
	if(g_numCylinders>0)
		g_Effect->GetVariableByName("g_UseCylinderForBarycentricInterpolant")->AsScalar()->SetIntArray(&cylinderIsBarycentricObstacle[0],0,g_numCylinders);
	if(g_numSpheresNoMoveConstraint>0)
		g_Effect->GetVariableByName("g_UseSphereNoMoveForBarycentricInterpolant")->AsScalar()->SetIntArray(&sphereNoMoveIsBarycentricObstacle[0],0,g_numSpheresNoMoveConstraint);

	ExtractAndSetCollisionImplicitMatrices();

	(g_Effect->GetVariableByName("TotalTransformation")->AsMatrix()->SetMatrix(g_TotalTransform));

	char fullPath[MAX_PATH];

	//textures
	specularNoise=IRenderer::GetRendererInstance()->addImageLibTexture("HairTextures\\specNoise.dds", false, IRenderer::GetRendererInstance()->Getlinear());

	TextureID  hairTexture=IRenderer::GetRendererInstance()->addImageLibTexture("HairTextures/HairSpecMap", false, IRenderer::GetRendererInstance()->Getlinear());

	g_HairBaseRV[0]=IRenderer::GetRendererInstance()->addImageLibTexture("hairbase.dds", false, IRenderer::GetRendererInstance()->Getlinear());

	g_HairBaseRV[1]=IRenderer::GetRendererInstance()->addImageLibTexture("hairbaseBrown.dds", false, IRenderer::GetRendererInstance()->Getlinear());

	g_HairBaseRV[2]=IRenderer::GetRendererInstance()->addImageLibTexture("hairbaseRed.dds", false, IRenderer::GetRendererInstance()->Getlinear());

	"meshAOMap"=IRenderer::GetRendererInstance()->addImageLibTexture("meshAOMap.dds", false, IRenderer::GetRendererInstance()->Getlinear());

	"densityThicknessMapBarycentric"=IRenderer::GetRendererInstance()->addImageLibTexture("densityThicknessMapBarycentric.dds", false, IRenderer::GetRendererInstance()->Getlinear());

	"densityThicknessMapClump"=IRenderer::GetRendererInstance()->addImageLibTexture("densityThicknessMapClump.dds", false, IRenderer::GetRendererInstance()->Getlinear());

	changeCameraAndWindPresets();

	return hr;
}

//--------------------------------------------------------------------------------------
// Initialize the super sampling buffers
//--------------------------------------------------------------------------------------
int ReinitSuperSampleBuffers()
{
	int hr(0);
	TextureID pRTV = DXUTGetD3D11RenderTargetView();
	stx_Resource* pRTVResource;
	pRTV->GetResource(&pRTVResource);
	TextureID pRTVTex2D = static_cast<TextureID >(pRTVResource);
	assert(pRTVTex2D);
	SURFACE_DESC pRTVTex2DDesc;
	pRTVTex2D->GetDesc(&pRTVTex2DDesc);
	pRTVResource->Release();    
	g_pRTVTex2DDesc = pRTVTex2DDesc;

	//supersampling buffer and RTV along with an additional resource to resolve to and read from 
	//////////////////SAFE_RELEASE(g_pSceneColorBuffer2XMS);
	//////////////////SAFE_RELEASE(g_pSceneColorBuffer2X);
	//////////////////SAFE_RELEASE(g_pSceneRTV2X);

	//supersampling depth buffer
	//////////////////SAFE_RELEASE(g_pSceneDepthBuffer2X);
	//////////////////SAFE_RELEASE(g_pSceneDSV2X);


	CreateScreenQuadVB();
	CreateSuperSampleScreenQuadVB();


	SURFACE_DESC descTex;
	SURFACE_DESC descRTV;


	//create the color super sample buffer----------------------------------------------------------
	//textures
	{Image3 img;
	img.create(FORMAT_RGBA8, g_Width*g_SSFACTOR, g_Height*g_SSFACTOR, 1, 1);
	g_pSceneColorBuffer2XMS=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());}

	//also create a non-MS texture to resolve to
	{Image3 img;
	img.create(FORMAT_RGBA8, g_Width*g_SSFACTOR, g_Height*g_SSFACTOR, 1, 1);
	g_pSceneColorBuffer2X=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());}

	//create the depth super sample buffer----------------------------------------------------------
	{
		Image3 img;
		img.create(FORMAT_D24F, g_Width*g_SSFACTOR, g_Height*g_SSFACTOR, 1, 1);
		g_pSceneDepthBuffer2X=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear()); 
	}

	
	descDSV.Format = FORMAT_D24_UNORM_S8_UINT; 
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	descDSV.Flags = 0;
	( IRenderer::GetRendererInstance()->CreateDepthStencilView( g_pSceneDepthBuffer2X, &descDSV, &g_pSceneDSV2X ) );


	return hr;
}



//--------------------------------------------------------------------------------------
// Initialize fluid and voxelizer states
//--------------------------------------------------------------------------------------
int InitializeFluidState()
{
	int hr(0);

	SAFE_DELETE(g_fluid);

	// Initialize fluid state
	g_fluid = new Fluid();
	if( !g_fluid ) 
		return E_OUTOFMEMORY;

	(g_fluid->Initialize(g_fluidGridWidth, g_fluidGridHeight, g_fluidGridDepth, Fluid::FT_SMOKE));

	g_fluid->SetNumJacobiIterations(2);
	g_fluid->SetUseMACCORMACK(false);
	g_fluid->SetMouseDown(true);

	// Create the shader resource view for velocity
	SAFE_ACQUIRE(g_pVelTexture3D, g_fluid->Get3DTexture(Fluid::RENDER_TARGET_VELOCITY0));
	
	ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
	SRVDesc.Format = g_fluid->GetFormat(Fluid::RENDER_TARGET_VELOCITY0);
	SRVDesc.ViewDimension = SRV_DIMENSION_TEXTURE3D;
	SRVDesc.Texture3D.MipLevels = 1;
	SRVDesc.Texture3D.MostDetailedMip = 0;
	(IRenderer::GetRendererInstance()->CreateShaderResourceView( g_pVelTexture3D, &SRVDesc, &g_FluidVelocityRV));
	

	//pass off the obstacles texture to the Hair grid, which is going to write obstacles into this
	g_HairGrid->setFluidObstacleTexture(g_fluid->Get3DTexture(Fluid::RENDER_TARGET_OBSTACLES),
		g_fluid->GetFormat(Fluid::RENDER_TARGET_OBSTACLES),
		g_fluidGridWidth,g_fluidGridHeight,g_fluidGridDepth);

	return hr;
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
int OnCreateDevice()
{
	int hr;

    g_MasterStrandVB[0] = 0;
    g_MasterStrandVB[1] = 0;
    g_ForceVB[0] = 0;
    g_ForceVB[1] = 0;
    g_CoordinateFrameVB[0] = 0;
    g_CoordinateFrameVB[1] = 0;
    g_MasterStrandRV[0] = 0;
    g_MasterStrandRV[1] = 0;
    g_ForceRV[0] = 0;
    g_ForceRV[1] = 0;
    g_CoordinateFrameRV[0] = 0;
    g_CoordinateFrameRV[1] = 0;
    g_HairBaseRV[0] = 0;
    g_HairBaseRV[1] = 0;
    g_HairBaseRV[2] = 0;

	g_firstFrame = true;
	g_firstMovedFrame = true;

	g_Center = D3DXFROMWINEVECTOR3(0, 0, 0);
	D3DXFROMWINEMatrixIdentity(&g_Transform);
	D3DXFROMWINEMatrixIdentity(&g_TotalTransform);
    D3DXFROMWINEMatrixIdentity(&g_RootTransform);

	g_UseTransformations = false;

	sprintf_s(g_loadDirectory,MAX_PATH,"%s",MayaHairDirectory);

	InitEffect();

	// Setup the camera's view parameters
	D3DXFROMWINEVECTOR3 eye, at;
	eye = D3DXFROMWINEVECTOR3(0.0f, -1.0f, -15.0f); 
	at = D3DXFROMWINEVECTOR3(0, 0, 0);

	g_Camera.SetViewParams(&eye, &at);
	g_Camera.SetModelCenter( at );
	g_Camera.SetEnablePositionMovement(true);
	g_Camera.SetScalers(0.001f,20.f);

	D3DXFROMWINEVECTOR3 lightPos = D3DXFROMWINEVECTOR3(4.7908502f,18.887268f,-9.7619190f);//D3DXFROMWINEVECTOR3(-5,10.0f,10.0f);

	g_HairShadows.GetCamera().SetViewParams(&lightPos, &at);
	g_HairShadows.GetCamera().SetRadius(30);

	g_ExternalCameraView = *g_Camera.GetViewMatrix();


	(LoadScalp());
	LoadMayaHair(g_loadDirectory, g_useShortHair);
	LoadCollisionObstacles();

	changeCameraAndWindPresets();


	(CreateResources());

	g_Effect->GetVariableByName("g_TessellatedMasterStrandLengthMax")->AsScalar()->SetInt(g_TessellatedMasterStrandLengthMax);
	g_Effect->GetVariableByName("g_NumTotalWisps")->AsScalar()->SetInt(g_NumWisps);
	g_Effect->GetVariableByName("g_maxLengthToRoot")->AsScalar()->SetFloat(g_maxHairLength);


	//initialize grid
	SAFE_DELETE(g_HairGrid);
	g_HairGrid = new HairGrid();
	if( !g_HairGrid ) 
		return E_OUTOFMEMORY;
	(g_HairGrid->Initialize(g_gridSize ,g_gridSize ,g_gridSize ,g_Effect, g_pEffectPath));


	//Initialize fluid state
	InitializeFluidState();

	//load the mesh for the arrow---------------------------------------------------------------------

	// Create the input layout
	FormatDesc layoutArrow[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	g_pVertexLayoutArrow = IRenderer::GetRendererInstance()->addVertexFormat(layoutArrow, elementsOf(layoutArrow), -1);

	g_MeshArrow.Create("arrow.sdkmesh");

	//-------------------------------------------------------------------------------------------------
	(CreatePlane(GROUND_PLANE_RADIUS,-10.0f));

	g_HairShadows.OnCreateDevice();

	//initialize the hair shading presets
	{
    //blond hair
	float baseColor[4] = {111/255.0f,77/255.0f,42/255.0f,1};
	float specColor[4] = {111/255.0f,77/255.0f,42/255.0f,1};
	float ksP = 0.4f;
	float ksS = 0.3f;
	float kd = 1.0f;
	float specPowerPrimary = 6.0f;
	float specPowerSecondary = 6.0f;    
	float ksP_sparkles = 0.2f;
	float specPowerPrimarySparkles = 600;
	float ka = 0.3f;
    hairShadingParams[0].assignValues(baseColor,specColor,ksP,ksS,kd,specPowerPrimary,specPowerSecondary,ksP_sparkles,specPowerPrimarySparkles,ka);
	}
	{
    //brown hair
	float baseColor[4] = {72.0f/255.0f,51.0f/255.0f,43.0f/255.0f,1.0f};
	float specColor[4] = {132.0f/255.0f,106.0f/255.0f,79.0f/255.0f,1};
    float ksP = 0.3f;
    float ksS = 0.4f;
    float kd = 0.6f;
    float specPowerPrimary = 30.0f;
    float specPowerSecondary = 15.0f;    
    float ksP_sparkles = 0.2f;
    float specPowerPrimarySparkles = 600;
    float ka = 0.2f;
	hairShadingParams[1].assignValues(baseColor,specColor,ksP,ksS,kd,specPowerPrimary,specPowerSecondary,ksP_sparkles,specPowerPrimarySparkles,ka);
	}
	{
    //red hair
	float baseColor[4] = {81/255.0f,41/255.0f,13/255.0f,1};
	float specColor[4] = {255/255.0f,75/255.0f,4/255.0f,1};
    float ksP = 0.4f;
    float ksS = 0.4f;
    float kd = 0.8f;
    float specPowerPrimary = 60.0f;
    float specPowerSecondary = 15.0f;    
    float ksP_sparkles = 0.1f;
    float specPowerPrimarySparkles = 600;
    float ka = 0.2f;
    hairShadingParams[2].assignValues(baseColor,specColor,ksP,ksS,kd,specPowerPrimary,specPowerSecondary,ksP_sparkles,specPowerPrimarySparkles,ka);
	}
	hairShadingParams[g_currHairShadingParams].setShaderVariables(g_Effect);
    g_Effect->GetVariableByName("hairTexture")->AsShaderResource()->SetResource( g_HairBaseRV[g_currHairShadingParams] );

	D3D11_SAMPLER_DESC desc2[1] = { 
		D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, 
		D3D11_TEXTURE_ADDRESS_CLAMP, 
		D3D11_TEXTURE_ADDRESS_CLAMP, 
		D3D11_TEXTURE_ADDRESS_CLAMP, 
		0.0, 0, D3D11_COMPARISON_NEVER, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f,
	};
	IRenderer::GetRendererInstance()->CreateSamplerState(desc2, &g_pLinearClampSampler);
	

	//saveHair();

	//delete all the data we were carying around
	delete[] g_clumpCoordinates;
	g_clumpCoordinates = 0;
	delete[] g_randStrandDeviations;
	g_randStrandDeviations = 0;
	delete[] g_masterStrandLengthToRoot;
	g_masterStrandLengthToRoot = 0;
	delete[] g_masterStrandLengths;
	g_masterStrandLengths = 0;
	delete[] g_Strand_Sizes;  
	g_Strand_Sizes = 0;
	delete[] g_tangentJitter;   
	g_tangentJitter = 0;
	delete[] g_BarycentricCoordinates;  
	g_BarycentricCoordinates = 0;

	return 0;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
int OnSwapChainResized()
{
	int hr = 0;

	g_Width = pBackBufferSurfaceDesc->Width;
	g_Height = pBackBufferSurfaceDesc->Height;

	g_superSampleViewport.TopLeftX = 0;
	g_superSampleViewport.TopLeftY = 0;
	g_superSampleViewport.MinDepth = 0;
	g_superSampleViewport.MaxDepth = 1;
	g_superSampleViewport.Width = g_Width*g_SSFACTOR;  
	g_superSampleViewport.Height = g_Height*g_SSFACTOR; 

	(g_Effect->GetVariableByName("g_ScreenWidth")->AsScalar()->SetFloat(g_Width));
	(g_Effect->GetVariableByName("g_ScreenHeight")->AsScalar()->SetFloat(g_Height));

	// Setup the camera's projection parameters
	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams(D3DXFROMWINE_PI/3, fAspectRatio, 0.1f, 5000.0f);

#ifdef SUPERSAMPLE
	(ReinitSuperSampleBuffers());
#endif

	return hr;
}



void RenderArrow(D3DXFROMWINEVECTOR3 pos)
{
	int hr;
	D3DXFROMWINEVECTOR3 g_modelCentroid = g_Center;

	//calculate and set the world view projection matrix for transforming the arrow
	float arrowScale = 0.03f;
	D3DXFROMWINEMATRIX mScale;
	D3DXFROMWINEMatrixScaling( &mScale, arrowScale, arrowScale, arrowScale);
	D3DXFROMWINEMATRIX mLookAtMatrix;
	D3DXFROMWINEVECTOR3 mArrowPos =  g_modelCentroid - pos;
	D3DXFROMWINEVECTOR3 mOrigin = g_Center;
	D3DXFROMWINEVECTOR3 mUp = D3DXFROMWINEVECTOR3(0, 1, 0);
	D3DXFROMWINEMatrixLookAtLH(&mLookAtMatrix, &mArrowPos, &mOrigin, &mUp);
	D3DXFROMWINEMATRIX mLookAtInv;
	D3DXFROMWINEMatrixInverse(&mLookAtInv, 0, &mLookAtMatrix);
	D3DXFROMWINEMATRIX mWorldView,mWorldViewProj;
	D3DXFROMWINEMATRIX mWorld = mScale * mLookAtInv ;
	if(g_useSkinnedCam)
		D3DXFROMWINEMatrixMultiply( &mWorldView, &mWorld, &g_ExternalCameraView);
	else
		D3DXFROMWINEMatrixMultiply( &mWorldView, &mWorld, g_Camera.GetViewMatrix());
	D3DXFROMWINEMatrixMultiply( &mWorldViewProj, &mWorldView, g_Camera.GetProjMatrix() );
	(g_Effect->GetVariableByName("ViewProjection")->AsMatrix()->SetMatrix(mWorldViewProj));

	//render the arrow
	IRenderer::GetRendererInstance()->setVertexFormat(0, g_pVertexLayoutArrow);
	g_pTechniqueRenderArrow->GetPassByIndex(0)->Apply(0);
	g_MeshArrow.Render();
}

int RepropagateCoordinateFrames(int iterations)
{ 
	int hr= 0;

	g_Effect->GetVariableByName("g_blendAxis")->AsScalar()->SetFloat(g_blendAxis);
	RecreateCoordinateFrames();

	unsigned int stride = 3*4*sizeof(float);     
	unsigned int offset = 0;
	IRenderer::GetRendererInstance()->setVertexFormat(0, g_CoordinateFrameIL);
	(g_Effect->GetVariableByName("g_MasterStrand")->AsShaderResource()->SetResource(g_MasterStrandRV[g_currentVB]));
	(g_Effect->GetVariableByName("g_VertexNumber")->AsShaderResource()->SetResource(g_MasterStrandLocalVertexNumberRV));

	for(int i=0;i<iterations;i++)
	{
		g_Effect->GetVariableByName("g_coordinateFrames")->AsShaderResource()->SetResource(g_CoordinateFrameRV[g_currentVBCoordinateFrame]);
		g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("PropagateCoordinateFrame")->Apply(0);
		IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &g_CoordinateFrameVB[g_currentVBCoordinateFrame], &stride, &offset);
		IRenderer::GetRendererInstance()->SOSetTargets(1, &g_CoordinateFrameVB[(g_currentVBCoordinateFrame+1)%2], &offset);
		IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_POINTS);
		IRenderer::GetRendererInstance()->Draw(g_NumMasterStrandControlVertices, 0);
		IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
		g_Effect->GetVariableByName("g_coordinateFrames")->AsShaderResource()->SetResource(0);
		g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("PropagateCoordinateFrame")->Apply(0);
	}

	IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
	g_Effect->GetVariableByName("g_MasterStrand")->AsShaderResource()->SetResource(0);
	g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("PropagateCoordinateFrame")->Apply(0);
	g_currentVBCoordinateFrame = (g_currentVBCoordinateFrame+1)%2;

	return hr;

}


void RecreateCoordinateFrames()
{
	unsigned int stride = 3*4*sizeof(float);     
	unsigned int offset = 0;
	IRenderer::GetRendererInstance()->setVertexFormat(0, g_CoordinateFrameIL);

	IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
	g_Effect->GetVariableByName("g_MasterStrand")->AsShaderResource()->SetResource(g_MasterStrandRV[g_currentVB]);
	//g_Effect->GetVariableByName("g_coordinateFrames")->AsShaderResource()->SetResource(0);
	g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("UpdateCoordinateFrame")->Apply(0);

	IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &g_CoordinateFrameVB[g_currentVBCoordinateFrame], &stride, &offset);
	//g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("UpdateCoordinateFrame")->Apply(0);
	IRenderer::GetRendererInstance()->SOSetTargets(1, &g_CoordinateFrameVB[(g_currentVBCoordinateFrame+1)%2], &offset);
	IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_POINTS);
	IRenderer::GetRendererInstance()->Draw(g_NumMasterStrandControlVertices, 0);

	IRenderer::GetRendererInstance()->SOSetTargets(0, 0, 0);
	g_Effect->GetVariableByName("g_MasterStrand")->AsShaderResource()->SetResource(0);
	g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("UpdateCoordinateFrame")->Apply(0);
	g_currentVBCoordinateFrame = (g_currentVBCoordinateFrame+1)%2;

}

void VisualizeCoordinateFrames(bool drawTessellatedCFs)
{
	unsigned int stride =  3*4*sizeof(float);        
	unsigned int offset = 0;
	IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_POINTS);
	IRenderer::GetRendererInstance()->setVertexFormat(0, g_CoordinateFrameIL);	
	if(drawTessellatedCFs)
	{   
		//set variables
		g_Effect->GetVariableByName("g_TessellatedMasterStrand")->AsShaderResource()->SetResource(g_TessellatedMasterStrandRV);
		g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("RenderCoordinateFrameTessellated")->Apply(0);

		IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, &g_TessellatedCoordinateFrameVB, &stride, &offset);
		IRenderer::GetRendererInstance()->Draw(g_NumTessellatedMasterStrandVertices, 0); 

		//unset variables
		g_Effect->GetVariableByName("g_TessellatedMasterStrand")->AsShaderResource()->SetResource(0);
		g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("RenderCoordinateFrameTessellated")->Apply(0);

	}
	else    
	{
        if(g_useCS)
		{
		    g_Effect->GetVariableByName("g_MasterStrandSB")->AsShaderResource()->SetResource(g_MasterStrandUAB_SRV);
			g_Effect->GetVariableByName("g_coordinateFramesSB")->AsShaderResource()->SetResource(g_CoordinateFrameUAB_SRV);
		    g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("RenderCoordinateFrameUnTessellatedSB")->Apply(0);
		}
		else
		{
		    g_Effect->GetVariableByName("g_MasterStrand")->AsShaderResource()->SetResource(g_MasterStrandRV[g_currentVB]);
		    g_Effect->GetVariableByName("g_coordinateFrames")->AsShaderResource()->SetResource(g_CoordinateFrameRV[g_currentVBCoordinateFrame]);
		    g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("RenderCoordinateFrameUnTessellated")->Apply(0);
    	}
	
		unsigned int stride = 0;
	    unsigned int offset = 0;
	    exBufferID  buffer[] = { 0 };
    	IRenderer::GetRendererInstance()->setVertexBuffer(0, 1, buffer, &stride, &offset);
	    IRenderer::GetRendererInstance()->setVertexFormat(0, 0);

		IRenderer::GetRendererInstance()->Draw(g_NumMasterStrandControlVertices, 0); 

        if(g_useCS)
		{
		    g_Effect->GetVariableByName("g_MasterStrandSB")->AsShaderResource()->SetResource(0);
			g_Effect->GetVariableByName("g_coordinateFramesSB")->AsShaderResource()->SetResource(0);
		    g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("RenderCoordinateFrameUnTessellatedSB")->Apply(0);
		}
		else
		{
		    g_Effect->GetVariableByName("g_MasterStrand")->AsShaderResource()->SetResource(0);
		    g_Effect->GetVariableByName("g_coordinateFrames")->AsShaderResource()->SetResource(0);
		    g_Effect->GetTechniqueByName("SimulateHair")->GetPassByName("RenderCoordinateFrameUnTessellated")->Apply(0);
    	}
	}

}



int renderInstancedInterpolatedHairForGrid(D3DXFROMWINEMATRIX& worldViewProjection , float ZMin, float ZStep, int numRTs )
{
	int hr = 0;

	g_Effect->GetVariableByName("WorldViewProjection")->AsMatrix()->SetMatrix(worldViewProjection);
	g_Effect->GetVariableByName("g_StrandWidthMultiplier")->AsScalar()->SetFloat(25); 
	D3DXFROMWINEVECTOR3 camPosition(1,0,0);
	g_Effect->GetVariableByName("EyePosition")->AsVector()->SetFloatVector(camPosition);
	g_Effect->GetVariableByName("g_gridZMin")->AsScalar()->SetFloat(ZMin);
	g_Effect->GetVariableByName("g_gridZStep")->AsScalar()->SetFloat(ZStep);
	g_Effect->GetVariableByName("g_gridNumRTs")->AsScalar()->SetInt(numRTs);

	unsigned int stride = 0;
	unsigned int offset = 0;
	exBufferID  buffer[] = { 0 };
	IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1, buffer, &stride, &offset );
	IRenderer::GetRendererInstance()->setVertexFormat(0, 0);

	IRenderer::GetRendererInstance()->IASetPrimitiveTopology(PRIM_LINE_STRIP);
	g_Effect->GetVariableByName("g_MasterStrandLengthsUntessellated")->AsShaderResource()->SetResource(g_MasterLengthsRVUntessellated);
	g_Effect->GetVariableByName("g_MasterStrandRootIndicesUntessellated")->AsShaderResource()->SetResource(g_RootsIndicesRVUntessellated); 
	g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedVSForGrid3DTextureMRT")->Apply(0);

	IRenderer::GetRendererInstance()->DrawInstanced(g_MasterStrandLengthMax,g_NumWisps,0,0); 

	g_Effect->GetVariableByName("g_MasterStrandLengthsUntessellated")->AsShaderResource()->SetResource(0);
	g_Effect->GetVariableByName("g_MasterStrandRootIndicesUntessellated")->AsShaderResource()->SetResource(0); 
	g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("InterpolateInstancedVSForGrid3DTextureMRT")->Apply(0);

	return hr;
}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnCreateDevice 
//--------------------------------------------------------------------------------------
void OnDestroyDevice( )
{
	ReleaseResources();

	//gui stuff
	//////////////////SAFE_RELEASE(g_Effect);

	SAFE_DELETE(g_fluid);
	//////////////////SAFE_RELEASE(g_pVelTexture3D);
	//////////////////SAFE_RELEASE(g_FluidVelocityRV);
	SAFE_DELETE(g_HairGrid);

	g_Sphere.Destroy();

	//////////////////SAFE_RELEASE(g_pLinearClampSampler);

	g_MeshArrow.Destroy();
	//////////////////SAFE_RELEASE( g_pVertexLayoutArrow );
	g_HairShadows.OnDestroyDevice();

	//mesh
	if(g_indices!=0)
		delete[] g_indices;
	g_indices = 0;

	SAFE_DELETE(g_Scalp);
	g_Scalp = 0;

	SAFE_DELETE(g_FaceMesh);
    g_FaceMesh = 0;

	//////////////////SAFE_RELEASE(g_RenderMeshIL);

    //the ground plane
    //////////////////SAFE_RELEASE(g_pPlaneVertexBuffer);
    //////////////////SAFE_RELEASE(g_pPlaneIndexBuffer);
    //////////////////SAFE_RELEASE(g_pPlaneVertexLayout);

	g_hairReset = true;
}


//--------------------------------------------------------------------------------------
// Load scalp 
//--------------------------------------------------------------------------------------
int LoadScalp()
{
	int hr;
	FormatDesc elements[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
		0, TYPE_TANGENT,   FORMAT_FLOAT, 3
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(elements, elementsOf(elements), -1);

	char xModelFile[MAX_PATH];
	char filename[MAX_PATH];
	sprintf_s(filename,100,"%s\\%s",g_loadDirectory,"scalp_mesh.sdkmesh");
	mbstowcs_s(0, xModelFile, MAX_PATH, filename, MAX_PATH);
	g_Scalp = new MeshRenderer2();
	(g_Scalp->Create( xModelFile));

    //load the unit sphere to display the collision objects with
	(g_Sphere.Create( "unitSphere.sdkmesh"));

	sprintf_s(filename,100,"%s\\%s",g_loadDirectory,"Face.sdkmesh");
	mbstowcs_s(0, xModelFile, MAX_PATH, filename, MAX_PATH);
	g_FaceMesh = new MeshRenderer2();
	(g_FaceMesh->Create( xModelFile));
	

	return 0;
}


//--------------------------------------------------------------------------------------
// Load the Collision Obstacles
// this file also contains the transforms from hair base pose to mesh initial pose, 
// and from hair base pose to mesh bind pose
//--------------------------------------------------------------------------------------

int LoadCollisionObstacles()
{
	int hr = 0;

	collisionObjectTransforms.clear();
	collisionObjectTransforms.reserve(MAX_IMPLICITS*2);

	sphereIsBarycentricObstacle.clear();
	sphereIsBarycentricObstacle.reserve(MAX_IMPLICITS*2);
	cylinderIsBarycentricObstacle.clear();
	cylinderIsBarycentricObstacle.reserve(MAX_IMPLICITS*2);
	sphereNoMoveIsBarycentricObstacle.clear();
	sphereNoMoveIsBarycentricObstacle.reserve(MAX_IMPLICITS*2);


	//read the collisions file ------------------------------------------------------------------------------
	ifstream infileCollisions;
	infileCollisions.open("collision_objects.txt");
	IMPLICIT_TYPE type;

	g_numSpheres = 0;
	g_numCylinders = 0;
	g_numSpheresNoMoveConstraint = 0;

	char c[MAX_PATH];


	if (infileCollisions) 
	{
		while(!infileCollisions.eof())
		{      
			type = NOT_AN_IMPLICIT;
			infileCollisions.getline(c,MAX_PATH);
			string s(c);
			string::size_type locComment = s.find("//",0);
			string::size_type locHash = s.find("#",0);

			if( locComment != string::npos && locHash != 0 )//if line is not part of the header, and it is the beginning of an implicit
			{
				//read the type of implicit
				if( s.find("'s'",0) != string::npos && s.find("'NoMoveConstraint'",0) != string::npos )
					type = SPHERE_NO_MOVE_CONSTRAINT;	
				else if( s.find("'s'",0) != string::npos )
					type = SPHERE;
				else if( s.find("'c'",0) != string::npos )
					type = CYLINDER;        
				else if( ( s.find("'scalpToMesh'",0) != string::npos ))
				{
					//read the scalpToMesh matrix
					float scalpToMeshMatrix[16];
					infileCollisions.getline(c,MAX_PATH);
					string stringvalues(c);
					istringstream valuesString(stringvalues);
					for(int i=0;i<16;i++)
						valuesString>>scalpToMeshMatrix[i];

					g_ScalpToMesh = D3DXFROMWINEMATRIX(scalpToMeshMatrix);
				}
				else if( ( s.find("'InitialTotalTransform'",0) != string::npos )) 
				{
					//read the g_InitialTotalTransform matrix
					float initialTransform[16];
					infileCollisions.getline(c,MAX_PATH);
					string stringvalues(c);
					istringstream valuesString(stringvalues);
					for(int i=0;i<16;i++)
						valuesString>>initialTransform[i];
					g_InitialTotalTransform = D3DXFROMWINEMATRIX(initialTransform);
				}

				if(type == SPHERE || type == CYLINDER || type == SPHERE_NO_MOVE_CONSTRAINT)
				{
					//read the values
					infileCollisions.getline(c,MAX_PATH);
					string valuesString(c);

					//see if this is a barycentric obstacle
					bool isBarycentricObstacle = false;
					if(valuesString.find("'barycentricObstacle'",0) != string::npos)
					{
						isBarycentricObstacle = true;
						infileCollisions.getline(c,MAX_PATH);
						valuesString = string(c);
					}

					//see if there is a bone
					string boneName;
					unsigned int posBone = (unsigned int) valuesString.find("'bone'",0);
					if( posBone != string::npos)
					{
						//read the name of the bone
						posBone += 7; //number of letters in 'bone' followed by space  
						boneName = valuesString.substr(posBone, valuesString.length()-posBone);

						//read the next line 
						infileCollisions.getline(c,MAX_PATH);
						valuesString = string(c);
					}

					//see if there is a basePoseToMeshBind transform
					D3DXFROMWINEMATRIX basePoseToMeshBind;
					D3DXFROMWINEMatrixIdentity(&basePoseToMeshBind);
					if( valuesString.find("'basePoseToMeshBind'",0) != string::npos )
					{
						infileCollisions.getline(c,MAX_PATH);
						valuesString = string(c);
						istringstream valuesIString(valuesString);
						float transform[16];
						for(int i=0;i<16;i++)
							valuesIString>>transform[i];
						basePoseToMeshBind = D3DXFROMWINEMATRIX(transform);	

						//read the next line 
						infileCollisions.getline(c,MAX_PATH);
						valuesString = string(c);
					}


					//scale
					collisionImplicit imp;
					istringstream valuesIString(valuesString);
					valuesIString>>imp.scale.x>>imp.scale.y>>imp.scale.z;
					//rotation
					infileCollisions.getline(c,MAX_PATH);
					valuesString = string(c);
					valuesIString.clear();
					valuesIString.str(valuesString);
					valuesIString>>imp.rotation.x>>imp.rotation.y>>imp.rotation.z; 
					//translation
					infileCollisions.getline(c,MAX_PATH);
					valuesString = string(c);
					valuesIString.clear();
					valuesIString.str(valuesString);
					valuesIString>>imp.center.x>>imp.center.y>>imp.center.z; 


					// maya applies scale,then rotation and then translation
					// for rotation assuming maya does x then y then z
					D3DXFROMWINEMATRIX initialScale, initialRotationX, initialRotationY, initialRotationZ, initialTranslation, initalTransformation;

					D3DXFROMWINEMatrixScaling( &initialScale, imp.scale.x, imp.scale.y, imp.scale.z);
					D3DXFROMWINEMatrixRotationX(&initialRotationX, imp.rotation.x);
					D3DXFROMWINEMatrixRotationY(&initialRotationY, imp.rotation.y);
					D3DXFROMWINEMatrixRotationZ(&initialRotationZ, imp.rotation.z);
					D3DXFROMWINEMatrixTranslation( &initialTranslation, imp.center.x, imp.center.y, imp.center.z);

					D3DXFROMWINEMatrixMultiply( &initalTransformation, &initialScale, &initialRotationX );
					D3DXFROMWINEMatrixMultiply( &initalTransformation, &initalTransformation, &initialRotationY );
					D3DXFROMWINEMatrixMultiply( &initalTransformation, &initalTransformation, &initialRotationZ );
					D3DXFROMWINEMatrixMultiply( &initalTransformation, &initalTransformation, &initialTranslation );

					collisionObjectTransforms.push_back(collisionObject());

					if(type == SPHERE)
					{
						collisionObjectTransforms.back().implicitType = SPHERE;
						if(isBarycentricObstacle)
							sphereIsBarycentricObstacle.push_back(1);
						else
							sphereIsBarycentricObstacle.push_back(0);
						g_numSpheres++;						
					}
					else if(type == CYLINDER)
					{
						collisionObjectTransforms.back().implicitType = CYLINDER;  
						if(isBarycentricObstacle)
							cylinderIsBarycentricObstacle.push_back(1);
						else
							cylinderIsBarycentricObstacle.push_back(0);
						g_numCylinders++;
					}
					else if(type == SPHERE_NO_MOVE_CONSTRAINT)
					{
						collisionObjectTransforms.back().implicitType = SPHERE_NO_MOVE_CONSTRAINT;
						if(isBarycentricObstacle)
							sphereNoMoveIsBarycentricObstacle.push_back(1);
						else
							sphereNoMoveIsBarycentricObstacle.push_back(0);
						g_numSpheresNoMoveConstraint++;					
					}

					collisionObjectTransforms.back().InitialTransform = initalTransformation;
					collisionObjectTransforms.back().boneName = boneName;
					collisionObjectTransforms.back().objToMesh = basePoseToMeshBind;
					collisionObjectTransforms.back().isHead = false;
					D3DXFROMWINEMATRIX CurrrentTransform;
					D3DXFROMWINEMatrixIdentity(&CurrrentTransform);
					collisionObjectTransforms.back().currentTransform = CurrrentTransform;

					//find out if this is the head
					if( s.find("'head'",0) != string::npos )
					{
						collisionObjectTransforms.back().isHead = true;
						g_scalpMaxRadius = max(imp.scale.x,max(imp.scale.y,imp.scale.z));
						g_HairBoneName = boneName;
					}
				}
			}
		}
		infileCollisions.close();
	} 

	D3DXFROMWINEMatrixInverse(&g_InvInitialTotalTransform,0,&g_InitialTotalTransform);
	g_InvInitialRotationScale = g_InvInitialTotalTransform;
	g_InvInitialRotationScale._41 = 0;
	g_InvInitialRotationScale._42 = 0;
	g_InvInitialRotationScale._43 = 0;
	g_InvInitialRotationScale._44 = 1;   

	//transform the implicit by the inverse initial total transform, to get it into hair space
	for(int i=0;i<int(collisionObjectTransforms.size());i++)
	{

		D3DXFROMWINEMATRIX initalTransformation = collisionObjectTransforms.at(i).InitialTransform;
		D3DXFROMWINEMatrixMultiply( &initalTransformation, &initalTransformation, &g_InvInitialTotalTransform);
		collisionObjectTransforms.at(i).InitialTransform = initalTransformation;
	}

	//numbers
	g_Effect->GetVariableByName("g_NumSphereImplicits")->AsScalar()->SetInt(g_numSpheres);
	g_Effect->GetVariableByName("g_NumCylinderImplicits")->AsScalar()->SetInt(g_numCylinders);
	g_Effect->GetVariableByName("g_NumSphereNoMoveImplicits")->AsScalar()->SetInt(g_numSpheresNoMoveConstraint);

	g_Effect->GetVariableByName("g_UseSphereForBarycentricInterpolant")->AsScalar()->SetIntArray(&sphereIsBarycentricObstacle[0],0,g_numSpheres);
	if(g_numCylinders)
	g_Effect->GetVariableByName("g_UseCylinderForBarycentricInterpolant")->AsScalar()->SetIntArray(&cylinderIsBarycentricObstacle[0],0,g_numCylinders);
	if (g_numSpheresNoMoveConstraint > 0) g_Effect->GetVariableByName("g_UseSphereNoMoveForBarycentricInterpolant")->AsScalar()->SetIntArray(&sphereNoMoveIsBarycentricObstacle[0],0,g_numSpheresNoMoveConstraint);

	//set the transforms
	ExtractAndSetCollisionImplicitMatrices();

	g_MaxWorldScale = max(g_InitialTotalTransform._11,max(g_InitialTotalTransform._22,g_InitialTotalTransform._33));

	g_Effect->GetVariableByName("g_SigmaA")->AsScalar()->SetFloat(-g_SigmaA/g_MaxWorldScale);

	//transform from hair space to world
	g_maxHairLengthWorld = g_maxHairLength*g_MaxWorldScale;
	g_scalpMaxRadiusWorld = g_scalpMaxRadius;
	g_maxHairLengthHairSpace = g_maxHairLength;
	//transform from world to hair space
	g_scalpMaxRadiusHairSpace = g_scalpMaxRadius*max(g_InvInitialTotalTransform._11,max(g_InvInitialTotalTransform._22,g_InvInitialTotalTransform._33)); 

	return hr;   

}


//--------------------------------------------------------------------------------------
// Create resources 
//--------------------------------------------------------------------------------------
int CreateResources()
{
	int hr;
	(CreateBuffers());
	(CreateInputLayouts());
	return 0;
}

//--------------------------------------------------------------------------------------
// Create buffers 
//--------------------------------------------------------------------------------------
int CreateBuffers()
{

	int hr = 0;
	(CreateMasterStrandSimulationIBs());
	(CreateMasterStrandVB());
	(CreateForceVB());
	(CreateTessellatedMasterStrandVB());
	(CreateTessellatedWispVB());
	(CreateMasterStrandTesselatedInputVB());
	(CreateStrandColors());
	(CreateStrandLengths());
	(CreateDeviantStrandsAndStrandSizes());
	(CreateCurlDeviations());
	(CreateStrandBarycentricCoordinates());
	(CreateRandomCircularCoordinates());
	(CreateStrandAttributes());
	(CreateMasterStrandSpringLengthVBs());
	(CreateCoordinateFrameVB());
	(CreateTessellatedCoordinateFrameVB());
	(CreateCollisionDetectionResources());
	(CreateCollisionScreenQuadVB());
	(CreateTangentJitter());
	(CreateStrandToBarycentricIndexMap());
	(CreateStrandToClumpIndexMap());
	(CreateInterpolatedValuesVBs());
    (CreateComputeShaderConstantBuffers());

	return 0;
}

int CreateMasterStrandSimulationIBs()
{
	

	//linear springs --------------------------------------------------------------------------------------
	//indicesFirst :  (0,1) (2,3) (4,5) (6,6)
	//indicesSecond : (0,0) (1,2) (3,4) (5,6)

	//overestimate, since exact estimate would require going over all of g_MasterStrandControlVertexOffsets
	g_numIndicesFirst =  (g_NumMasterStrandControlVertices/2) + g_NumMasterStrands;
	g_numIndicesSecond = (g_NumMasterStrandControlVertices/2) + g_NumMasterStrands*2;

	//make two index buffers that batch up the control vertices into two groups
	short (*indicesFirst)[2] = new short[g_numIndicesFirst][2];
	short (*indicesSecond)[2] = new short[g_numIndicesSecond][2];

	int ind = 0;
	short v = 0;
	for (int s = 0; s < g_NumMasterStrands; ++s) 
	{
		for (; v < g_MasterStrandControlVertexOffsets[s]; ++ind, v += 2) 
		{
			indicesFirst[ind][0] = v;
			if(v+1 < g_MasterStrandControlVertexOffsets[s])
				indicesFirst[ind][1] = v + 1;
			else
				indicesFirst[ind][1] = v;            
		}
		v = g_MasterStrandControlVertexOffsets[s];
	}
	//chop off the part of this buffer that wasnt used
	g_numIndicesFirst = ind;


	ind = 0;
	v = 0;
	for (int s = 0; s < g_NumMasterStrands; ++s) 
	{   
		indicesSecond[ind][0] = v;
		indicesSecond[ind][1] = v;
		v++;
		ind++;
		for (; v < g_MasterStrandControlVertexOffsets[s]; ++ind, v += 2) 
		{
			indicesSecond[ind][0] = v;
			if(v+1 < g_MasterStrandControlVertexOffsets[s])
				indicesSecond[ind][1] = v + 1;
			else
				indicesSecond[ind][1] = v;
		}
		v = g_MasterStrandControlVertexOffsets[s];
	}
	g_numIndicesSecond = ind;


	//angular linear springs --------------------------------------------------------------------------------------
	//indicesFirstAngular :  (0,1,2,3) (4,5,6,6)
	//indicesSecondAngular : (0,1,1,1) (2,3,4,5) (6,6,6,6)

	//overestimate, since exact estimate would require going over all of g_MasterStrandControlVertexOffsets
	g_numIndicesFirstAngular =  (g_NumMasterStrandControlVertices/4) + g_NumMasterStrands;
	g_numIndicesSecondAngular = (g_NumMasterStrandControlVertices/4) + g_NumMasterStrands*2;

	//make two index buffers that batch up the control vertices into two groups
	short (*indicesFirstAngular)[4] = new short[g_numIndicesFirstAngular][4];
	short (*indicesSecondAngular)[4] = new short[g_numIndicesSecondAngular][4];

	ind = 0;
	v = 0;
	short index;
	for (int s = 0; s < g_NumMasterStrands; ++s) 
	{
		for (; v < g_MasterStrandControlVertexOffsets[s]; ++ind, v += 4) 
		{   
			index = v-1;
			for(int i=0;i<4;i++)
			{    
				if( v+i < g_MasterStrandControlVertexOffsets[s])
					index++; 
				indicesFirstAngular[ind][i] = index;

			}
		}
		v = g_MasterStrandControlVertexOffsets[s];
	}
	//chop off the part of this buffer that wasnt used
	g_numIndicesFirstAngular = ind;


	ind = 0;
	v = 0;
	for (int s = 0; s < g_NumMasterStrands; ++s) 
	{
		indicesSecondAngular[ind][0] = v;
		indicesSecondAngular[ind][1] = v+1;
		indicesSecondAngular[ind][2] = v+1;
		indicesSecondAngular[ind][3] = v+1;
		v+=2;
		ind++;

		for (; v < g_MasterStrandControlVertexOffsets[s]; ++ind, v += 4) 
		{   
			index = v-1;
			for(int i=0;i<4;i++)
			{    
				if( v+i < g_MasterStrandControlVertexOffsets[s])
					index++; 
				indicesSecondAngular[ind][i] = index;

			}
		}
		v = g_MasterStrandControlVertexOffsets[s];
	}
	//chop off the part of this buffer that wasnt used
	g_numIndicesSecondAngular = ind;



	//create all 4 Vertex Buffers from the data calculated---------------------------------------------

	initialData.pSysMem = indicesFirst;
	int hr;
	
	bufferDesc.ByteWidth = g_numIndicesFirst*2*sizeof(short); 
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	//////////////////SAFE_RELEASE(g_MasterStrandSimulation1IB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_MasterStrandSimulation1IB));

	initialData.pSysMem = indicesSecond;
	bufferDesc.ByteWidth = g_numIndicesSecond*2*sizeof(short);
	//////////////////SAFE_RELEASE(g_MasterStrandSimulation2IB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_MasterStrandSimulation2IB));

	initialData.pSysMem = indicesFirstAngular;
	bufferDesc.ByteWidth = g_numIndicesFirstAngular*4*sizeof(short);
	//////////////////SAFE_RELEASE(g_MasterStrandSimulationAngular1IB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_MasterStrandSimulationAngular1IB));

	initialData.pSysMem = indicesSecondAngular;
	bufferDesc.ByteWidth = g_numIndicesSecondAngular*4*sizeof(short);
	//////////////////SAFE_RELEASE(g_MasterStrandSimulationAngular2IB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_MasterStrandSimulationAngular2IB));

	delete[] indicesFirst;
	delete[] indicesSecond;
	delete[] indicesFirstAngular;
	delete[] indicesSecondAngular;

	//also create a buffer and SRV to read and index from in the compute shader
	initialData.pSysMem = g_MasterStrandControlVertexOffsets;
	bufferDesc.ByteWidth = g_NumMasterStrands*sizeof(int);
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	//////////////////SAFE_RELEASE(g_MasterStrandSimulationIndicesBuffer);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_MasterStrandSimulationIndicesBuffer));
    
	SRVDesc.Format = FORMAT_R32_SINT;
	SRVDesc.ViewDimension = SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.ElementOffset = 0;
	SRVDesc.Buffer.ElementWidth = g_NumMasterStrands;
	//////////////////SAFE_RELEASE(g_MasterStrandSimulationIndicesSRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_MasterStrandSimulationIndicesBuffer, &SRVDesc, &g_MasterStrandSimulationIndicesSRV));

	return 0;
}

float vec3Length(D3DXFROMWINEVECTOR4 vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

//create linear springs, angular springs, and original vectors (for real angular forces)
int CreateMasterStrandSpringLengthVBs()
{
	float *masterStrandLinearLengths = new float[g_NumMasterStrandControlVertices];
	D3DXFROMWINEVECTOR2 *masterStrandAngularLengthsStiffness = new D3DXFROMWINEVECTOR2[g_NumMasterStrandControlVertices];

	for(int i=0;i<g_NumMasterStrandControlVertices-1;i++)
	{
		masterStrandLinearLengths[i] =  vec3Length(g_MasterStrandControlVertices[i].Position - g_MasterStrandControlVertices[i+1].Position); 
		masterStrandAngularLengthsStiffness[i] =  D3DXFROMWINEVECTOR2(vec3Length(g_MasterStrandControlVertices[i].Position - g_MasterStrandControlVertices[i+2].Position),0); 
	}

	//linear springs
	int hr;
	
	bufferDesc.ByteWidth = g_NumMasterStrandControlVertices * sizeof(float);
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags =  BIND_VERTEX_BUFFER | BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	
	initialData.pSysMem = masterStrandLinearLengths;
	//////////////////SAFE_RELEASE(g_MasterStrandLinearSpringLengthsVB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_MasterStrandLinearSpringLengthsVB));
	delete[] masterStrandLinearLengths;

	//create a shader resource view to read this in the compute shader
	
	desc.Format = FORMAT_R32_FLOAT;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumMasterStrandControlVertices;
	//////////////////SAFE_RELEASE(g_MasterStrandLinearSpringLengthsSRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_MasterStrandLinearSpringLengthsVB, &desc, &g_MasterStrandLinearSpringLengthsSRV));


	int ind = 0;
	int v = 0;
	float lt = 0.25f;
	float ht = 0.4f;
	int lastEndVertex = 0;

	for (int s = 0; s < g_NumMasterStrands; ++s) 
	{   
		for (; v < g_MasterStrandControlVertexOffsets[s]; ++ind, v ++) 
		{	
			float ratio = float(v-lastEndVertex)/(g_MasterStrandControlVertexOffsets[s]-1 - lastEndVertex);

			if( (g_MasterStrandControlVertexOffsets[s]-1 - lastEndVertex) < 5 )
				masterStrandAngularLengthsStiffness[ind].y = 0.0;
			else if( ratio < lt ) 
				masterStrandAngularLengthsStiffness[ind].y = g_AngularSpringStiffness;
			else if(ratio > ht)
				masterStrandAngularLengthsStiffness[ind].y = 0.0;
			else
				masterStrandAngularLengthsStiffness[ind].y = g_AngularSpringStiffness*(1.0 - ((ratio - lt)/(ht - lt)));

			//maximum stiffness for long demo hair
			if(!g_useShortHair)
			    masterStrandAngularLengthsStiffness[ind].y = 1.0;   
		}
		v = g_MasterStrandControlVertexOffsets[s];
		lastEndVertex = g_MasterStrandControlVertexOffsets[s];
	}

	bufferDesc.ByteWidth = g_NumMasterStrandControlVertices * sizeof(D3DXFROMWINEVECTOR2);
	
	initialDataStiffness.pSysMem = masterStrandAngularLengthsStiffness;
	//////////////////SAFE_RELEASE(g_MasterStrandAngularSpringConstraintsVB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialDataStiffness, &g_MasterStrandAngularSpringConstraintsVB));

	//create a shader resource view to read this in the compute shader
	
	descSRV.Format = FORMAT_R32G32_FLOAT;
	descSRV.ViewDimension = SRV_DIMENSION_BUFFER;
	descSRV.Buffer.ElementOffset = 0;
	descSRV.Buffer.ElementWidth = g_NumMasterStrandControlVertices;
	//////////////////SAFE_RELEASE(g_MasterStrandAngularSpringConstraintsSRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_MasterStrandAngularSpringConstraintsVB, &descSRV, &g_MasterStrandAngularSpringConstraintsSRV));

	delete[] masterStrandAngularLengthsStiffness; 


	//original hair vectors-----------------------------------------------------------------------------------------------------
	float3* original_vectorsInLocalCF = new float3[g_NumMasterStrandControlVertices];
	v=0;
	for (int s = 0; s < g_NumMasterStrands; ++s) 
	{   
		original_vectorsInLocalCF[v] = float3( g_MasterStrandControlVertices[v+1].Position.x-g_MasterStrandControlVertices[v].Position.x,
			g_MasterStrandControlVertices[v+1].Position.y-g_MasterStrandControlVertices[v].Position.y,
			g_MasterStrandControlVertices[v+1].Position.z-g_MasterStrandControlVertices[v].Position.z );
		v++;
		for (; v < g_MasterStrandControlVertexOffsets[s]-1; v++) 
		{	
			original_vectorsInLocalCF[v] = TrasformFromWorldToLocalCF( v-1, 
				float3( g_MasterStrandControlVertices[v+1].Position.x-g_MasterStrandControlVertices[v].Position.x,
				g_MasterStrandControlVertices[v+1].Position.y-g_MasterStrandControlVertices[v].Position.y,
				g_MasterStrandControlVertices[v+1].Position.z-g_MasterStrandControlVertices[v].Position.z  )  
				);

		}

		//the last vertex dosnt need a vector since it does not make sense
		original_vectorsInLocalCF[v] = float3( 0, 0, 0 );
		v++;
	}


	exBufferID  originalVectorsVB;
	
	initialData2.pSysMem = original_vectorsInLocalCF;
	initialData2.SysMemPitch = 0;
	initialData2.SysMemSlicePitch = 0;
	
	bufferDesc2.ByteWidth = g_NumMasterStrandControlVertices * sizeof(D3DXFROMWINEVECTOR3);
	bufferDesc2.Usage = USAGE_IMMUTABLE;
	bufferDesc2.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc2.CPUAccessFlags = 0;
	bufferDesc2.MiscFlags = 0;
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc2, &initialData2, &originalVectorsVB));
	
	ZeroMemory( &desc2, sizeof(desc2) );
	desc2.Format = FORMAT_R32G32B32_FLOAT;
	desc2.ViewDimension = SRV_DIMENSION_BUFFER;
	desc2.Buffer.ElementOffset = 0;
	desc2.Buffer.ElementWidth = g_NumMasterStrandControlVertices;
	//////////////////SAFE_RELEASE(g_originalVectorsRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(originalVectorsVB, &desc2, &g_originalVectorsRV));
	g_Effect->GetVariableByName("g_OriginalVectors")->AsShaderResource()->SetResource(g_originalVectorsRV);
	//////////////////SAFE_RELEASE(originalVectorsVB);
	delete[] original_vectorsInLocalCF; 


	//hair lengths: ---------------------------------------------------------------------------------------------------------
	//keep them as a fraction of the total hair length
	if(g_masterStrandLengthToRoot) delete [] g_masterStrandLengthToRoot;
	if(g_masterStrandLengths) delete [] g_masterStrandLengths;
	g_masterStrandLengthToRoot = new float[g_NumMasterStrandControlVertices];
	g_masterStrandLengths = new float[g_NumMasterStrands];
	int strand = 0;
	ind = 0;
	v = 0;
	float hairLength = 0;
	for (int s = 0; s < g_NumMasterStrands; ++s) 
	{   
		hairLength = 0;
		int ind_original = ind;

		g_masterStrandLengthToRoot[ind++] = 0;
		v++;

		//get the actual hair lengths 
		for (; v < g_MasterStrandControlVertexOffsets[s]; ++ind, v ++) 
		{
			hairLength += vec3Length(g_MasterStrandControlVertices[v].Position - g_MasterStrandControlVertices[v-1].Position); 
			g_masterStrandLengthToRoot[ind] = hairLength;
		}

		//create the fractional hair lengths
		for (; ind_original < ind; ++ind_original) 
			g_masterStrandLengthToRoot[ind_original] /= hairLength;

		g_masterStrandLengths[strand++] = hairLength;
		v = g_MasterStrandControlVertexOffsets[s];
	}

	//original hair lengths
	exBufferID  MasterStrandLengthToRootVB;
	bufferDesc.BindFlags =  BIND_SHADER_RESOURCE;
	initialData.pSysMem = g_masterStrandLengthToRoot;
	bufferDesc.ByteWidth = g_NumMasterStrandControlVertices * sizeof(float);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &MasterStrandLengthToRootVB));
	desc.Format = FORMAT_R32_FLOAT;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumMasterStrandControlVertices;
	//////////////////SAFE_RELEASE(g_MasterStrandLengthToRootRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(MasterStrandLengthToRootVB, &desc, &g_MasterStrandLengthToRootRV));
	g_Effect->GetVariableByName("g_LengthsToRoots")->AsShaderResource()->SetResource(g_MasterStrandLengthToRootRV);
	//////////////////SAFE_RELEASE(MasterStrandLengthToRootVB);

	exBufferID  MasterStrandLengthsVB;
	initialData.pSysMem = g_masterStrandLengths;
	bufferDesc.ByteWidth = g_NumMasterStrands * sizeof(float);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &MasterStrandLengthsVB));
	desc.Buffer.ElementWidth = g_NumMasterStrands;
	//////////////////SAFE_RELEASE(g_MasterStrandLengthsRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(MasterStrandLengthsVB, &desc, &g_MasterStrandLengthsRV));
	g_Effect->GetVariableByName("g_Lengths")->AsShaderResource()->SetResource(g_MasterStrandLengthsRV);
	//////////////////SAFE_RELEASE(MasterStrandLengthsVB);

	//tessellated hair lengths
	bufferDesc.Usage = USAGE_DEFAULT;
	bufferDesc.BindFlags =  BIND_SHADER_RESOURCE | D3D11_BIND_STREAM_OUTPUT;
	bufferDesc.ByteWidth = g_NumTessellatedMasterStrandVertices * sizeof(float);
	//////////////////SAFE_RELEASE(g_TessellatedMasterStrandLengthToRootVB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, 0, &g_TessellatedMasterStrandLengthToRootVB));
	desc.Format = FORMAT_R32_FLOAT;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumTessellatedMasterStrandVertices;
	//////////////////SAFE_RELEASE(g_TessellatedMasterStrandLengthToRootRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_TessellatedMasterStrandLengthToRootVB, &desc, &g_TessellatedMasterStrandLengthToRootRV));


	//vertex number along hair----------------------------------------------------------------------------------------------------------
	//resource to convert vertexID of the one long VB we have to a vertex number from the root
	int *vertexNumber = new int[g_NumMasterStrandControlVertices];
	v = 0;
	int rootV = 0;
	for (int s = 0; s < g_NumMasterStrands; ++s) 
	{	for (; v < g_MasterStrandControlVertexOffsets[s]; v++) 
	{
		vertexNumber[v] = v-rootV;
	}
	rootV = v;
	}
	//original hair lengths
	exBufferID  vertexNumberVB;
	bufferDesc.BindFlags =  BIND_SHADER_RESOURCE;
	initialData.pSysMem = vertexNumber;
	bufferDesc.ByteWidth = g_NumMasterStrandControlVertices * sizeof(int);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &vertexNumberVB));
	desc.Format = FORMAT_R32_SINT;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumMasterStrandControlVertices;
	//////////////////SAFE_RELEASE(g_MasterStrandLocalVertexNumberRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(vertexNumberVB, &desc, &g_MasterStrandLocalVertexNumberRV));
	g_Effect->GetVariableByName("g_VertexNumber")->AsShaderResource()->SetResource(g_MasterStrandLocalVertexNumberRV);
	//////////////////SAFE_RELEASE(vertexNumberVB);
	delete[] vertexNumber;

	return 0; 
}

int CreateMasterStrandTesselatedInputVB()
{   
	
	HairAdjacencyVertex* vertices = new HairAdjacencyVertex[g_NumTessellatedMasterStrandVertices];

	int ind = 0;
	short v = 0;

	//this is for b splines:
	for (int s = 0; s < g_NumMasterStrands; ++s)
	{
		bool firstSegment = true;
		for (v = v-1 ; v < g_MasterStrandControlVertexOffsets[s] - 2; v += 1) //starting one vertex early, and ending one vertex late, to get extra beginning and end segments 
		{   
			for (int i = 0; i < NUM_TESSELLATION_SEGMENTS; ++i, ++ind) 
			{
				HairAdjacencyVertex tempVertex;
				tempVertex.controlVertexIndex = v;
				tempVertex.u = i / (float)(NUM_TESSELLATION_SEGMENTS);

				if(firstSegment) //first segment
				{   
					tempVertex.controlVertexIndex = v+1;
					tempVertex.u -= 2;
				}
				else if( v == g_MasterStrandControlVertexOffsets[s] - 3 ) //last segment
				{
					tempVertex.controlVertexIndex = v-1;
					tempVertex.u += 2;
				}

				vertices[ind] = tempVertex;
			}
			firstSegment = false;
		}

		//extra end vertex
		HairAdjacencyVertex tempVertex;
		tempVertex.controlVertexIndex = v-2; 
		tempVertex.u = 3.0; 
		vertices[ind++] = tempVertex; 

		v+=2;
	}
	/*
	// this is for bezier curves: 
	for (int s = 0; s < g_NumMasterStrands; ++s) 
	{
	for (; v < g_MasterStrandControlVertexOffsets[s] - 1; v += 3) 
	{   
	for (int i = 0; i < NUM_TESSELLATION_SEGMENTS; ++i, ++ind) 
	{
	HairAdjacencyVertex tempVertex;
	tempVertex.controlVertexIndex = v;
	tempVertex.u = i / (float)(NUM_TESSELLATION_SEGMENTS);
	vertices[ind] = tempVertex;
	}
	}
	HairAdjacencyVertex tempVertex;
	tempVertex.controlVertexIndex = v; 
	tempVertex.u = 0.0;
	vertices[ind] = tempVertex;
	++v;
	++ind;       

	assert(v == g_MasterStrandControlVertexOffsets[s]);
	}*/

	initialData.pSysMem = vertices;
	int hr;
	
	bufferDesc.ByteWidth = g_NumTessellatedMasterStrandVertices * sizeof(HairAdjacencyVertex);
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
    //////////////////SAFE_RELEASE(g_TessellatedMasterStrandVBInitial);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_TessellatedMasterStrandVBInitial));
	delete [] vertices;
	return 0;
}



int CreateInterpolatedValuesVBs()
{
	int hr;

	int numBVertices = g_TessellatedMasterStrandLengthMax*g_totalMStrands;
	int numCVertices = g_TessellatedMasterStrandLengthMax*g_totalSStrands;


	//create the vertex buffers
	
	bufferDesc.ByteWidth = numBVertices*4*sizeof(float);
	bufferDesc.Usage = USAGE_DEFAULT;
	bufferDesc.BindFlags =  BIND_SHADER_RESOURCE |D3D11_BIND_STREAM_OUTPUT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	//////////////////SAFE_RELEASE(g_BarycentricInterpolatedPositionsWidthsVB);
	//////////////////SAFE_RELEASE(g_BarycentricInterpolatedIdAlphaTexVB);
	//////////////////SAFE_RELEASE(g_BarycentricInterpolatedTangentVB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, 0, &g_BarycentricInterpolatedPositionsWidthsVB));//position and width
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, 0, &g_BarycentricInterpolatedIdAlphaTexVB)); //ID, alpha and texture
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, 0, &g_BarycentricInterpolatedTangentVB)); //tangent  

	bufferDesc.ByteWidth = numCVertices*4*sizeof(float);
    //////////////////SAFE_RELEASE(g_ClumpInterpolatedPositionsWidthsVB);
    //////////////////SAFE_RELEASE(g_ClumpInterpolatedIdAlphaTexVB);
    //////////////////SAFE_RELEASE(g_ClumpInterpolatedTangentVB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, 0, &g_ClumpInterpolatedPositionsWidthsVB));//position and width
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, 0, &g_ClumpInterpolatedIdAlphaTexVB)); //ID, alpha and texture
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, 0, &g_ClumpInterpolatedTangentVB)); //tangent  

	//create the shader resource views
	
	desc.Format = FORMAT_RGBA32F;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = numBVertices;
    //////////////////SAFE_RELEASE(g_BarycentricInterpolatedPositionsWidthsRV);
    //////////////////SAFE_RELEASE(g_BarycentricInterpolatedIdAlphaTexRV);
    //////////////////SAFE_RELEASE(g_BarycentricInterpolatedTangentRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_BarycentricInterpolatedPositionsWidthsVB, &desc, &g_BarycentricInterpolatedPositionsWidthsRV));
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_BarycentricInterpolatedIdAlphaTexVB, &desc, &g_BarycentricInterpolatedIdAlphaTexRV));
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_BarycentricInterpolatedTangentVB, &desc, &g_BarycentricInterpolatedTangentRV));

	desc.Buffer.ElementWidth = numCVertices;
    //////////////////SAFE_RELEASE(g_ClumpInterpolatedPositionsWidthsRV);
    //////////////////SAFE_RELEASE(g_ClumpInterpolatedIdAlphaTexRV);
    //////////////////SAFE_RELEASE(g_ClumpInterpolatedTangentRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_ClumpInterpolatedPositionsWidthsVB, &desc, &g_ClumpInterpolatedPositionsWidthsRV));
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_ClumpInterpolatedIdAlphaTexVB, &desc, &g_ClumpInterpolatedIdAlphaTexRV));
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_ClumpInterpolatedTangentVB, &desc, &g_ClumpInterpolatedTangentRV));

	return hr;
}


int CreateMasterStrandVB()
{
	int hr;
	
	bufferDesc.ByteWidth = g_NumMasterStrandControlVertices * sizeof(HairVertex);
	bufferDesc.Usage = USAGE_DEFAULT;
	bufferDesc.BindFlags =  BIND_VERTEX_BUFFER | BIND_SHADER_RESOURCE |D3D11_BIND_STREAM_OUTPUT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	
	initialData.pSysMem = g_MasterStrandControlVertices;
	//////////////////SAFE_RELEASE(g_MasterStrandVB[0]);
	//////////////////SAFE_RELEASE(g_MasterStrandVB[1]);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_MasterStrandVB[0]));
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_MasterStrandVB[1])); 
	bufferDesc.BindFlags = BIND_VERTEX_BUFFER;
	//////////////////SAFE_RELEASE(g_MasterStrandVBOldTemp);
	//////////////////SAFE_RELEASE(g_MasterStrandVBOld);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_MasterStrandVBOldTemp));
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_MasterStrandVBOld)); 

	//create shader resource views for the master strand; this is for non-dynamic tesselation
	
	desc.Format = FORMAT_RGBA32F;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumMasterStrandControlVertices;
	//////////////////SAFE_RELEASE(g_MasterStrandRV[0]);
	//////////////////SAFE_RELEASE(g_MasterStrandRV[1]);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_MasterStrandVB[0], &desc, &g_MasterStrandRV[0]));
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_MasterStrandVB[1], &desc, &g_MasterStrandRV[1]));

	exBufferID  originalMasterStrand = 0;
	initialData.pSysMem = g_MasterStrandControlVertices;
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	//////////////////SAFE_RELEASE(originalMasterStrand);
	//////////////////SAFE_RELEASE(g_OriginalMasterStrandRV);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &originalMasterStrand));
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(originalMasterStrand, &desc, &g_OriginalMasterStrandRV));
	g_Effect->GetVariableByName("g_OriginalMasterStrand")->AsShaderResource()->SetResource(g_OriginalMasterStrandRV);
	//////////////////SAFE_RELEASE(originalMasterStrand);

	return 0;
}


int CreateForceVB()
{
	//initialize forces to be (0,0,0,k)
	//k is the angular stiffness of the hair
	//k is 10 for the first 15 vertices and 3 for the rest of the vertices in a strand with some blend region
	//make shorter (<15 vertices) hair stiff all over, unless texture specifies otherwise

	int hr;

	//load the stiffness texture
	stx_Resource* pRes = "LongHairResources/stiffnessMap.dds";


	TextureID  pTemp;
	pRes->QueryInterface( __uuidof( TextureID ), (LPVOID*)&pTemp );
	SURFACE_DESC texDesc;
	pTemp->GetDesc( &texDesc );

	
	IRenderer::GetRendererInstance()->Map(pTemp, 0, D3D11_MAP_READ, 0, &mappedTex2D);
	FLOAT* pTexels = (FLOAT*)mappedTex2D.pData;


	int shortStiffHairLength = 15;
	float flex = 0.0;
	float stiff = 1.0; 
	int blendRegion = 8;
	int stiffRegion = 9;
	float hairStiffnessMultiplier = 1.0f;
    if(g_useShortHair)
	{
	    shortStiffHairLength = 10;
		stiffRegion = 5;
        hairStiffnessMultiplier = 0.15f; 
	}

	int blendBegin = stiffRegion-blendRegion/2.0;
	int blendEnd = stiffRegion+blendRegion/2.0;

	float4* forces = new float4[g_NumMasterStrandControlVertices];
    float* stiffness = new float[g_NumMasterStrandControlVertices];
	int v=0;
	int rootV = 0;


	for (int s = 0; s < g_NumMasterStrands; ++s) 
	{   
		float hairTexStiffness;

		unsigned int x = g_MasterAttributes[s].texcoord.x*texDesc.Width;
		if(x>=texDesc.Width)
			x = texDesc.Width-1;

		unsigned int y = g_MasterAttributes[s].texcoord.y*texDesc.Height;
		if(y>=texDesc.Height)
			y = texDesc.Height-1;


		int location =   x*4 
			+ y*texDesc.Width*4;

		hairTexStiffness = pTexels[location]*hairStiffnessMultiplier; 


		int strandLength = g_MasterStrandControlVertexOffsets[s] - rootV;
		for (; v < g_MasterStrandControlVertexOffsets[s]; v++) 
		{	
			if(strandLength < shortStiffHairLength && hairTexStiffness>0.1)
			{
				forces[v] = float4(0,0,0,1);
				stiffness[v] = 1.0f;
			}
			else
			{   
				float localStiff = hairTexStiffness*stiff + (1-hairTexStiffness)*0.1;

				if(v-rootV < blendBegin) 
				{
					forces[v] = float4(0,0,0,localStiff);
					stiffness[v] = localStiff;
				}
				else if(v-rootV >= blendEnd)
				{
					forces[v] = float4(0,0,0,flex);
					stiffness[v] = flex;
				}
				else
				{    
					float blend = float(v-rootV-blendBegin)/blendRegion; 
					float stif = blend*flex + (1-blend)*localStiff;
					forces[v] = float4(0,0,0,stif);
					stiffness[v] = stif;
				}
			}
		}
		rootV = v;
	}

	IRenderer::GetRendererInstance()->Unmap(pTemp, 0);
	//////////////////SAFE_RELEASE( pTemp );
	//////////////////SAFE_RELEASE( pRes );


	
	bufferDesc.ByteWidth = g_NumMasterStrandControlVertices * sizeof(HairVertex);
	bufferDesc.Usage = USAGE_DEFAULT;
	bufferDesc.BindFlags =  BIND_VERTEX_BUFFER | BIND_SHADER_RESOURCE |D3D11_BIND_STREAM_OUTPUT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	
	initialData.pSysMem = forces;
	//////////////////SAFE_RELEASE(g_ForceVB[0]);
	//////////////////SAFE_RELEASE(g_ForceVB[1]);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_ForceVB[0]));
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_ForceVB[1])); 
	delete[] forces;

	//create shader resource views; this is for reading in the final integrate pass
	
	desc.Format = FORMAT_RGBA32F;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumMasterStrandControlVertices;
	//////////////////SAFE_RELEASE(g_ForceRV[0]);
	//////////////////SAFE_RELEASE(g_ForceRV[1]);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_ForceVB[0], &desc, &g_ForceRV[0]));
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_ForceVB[1], &desc, &g_ForceRV[1]));

    //create a buffer and shader resource for the compute shader (angular forces are directly computed and used in compute shader, so we just need angular stiffness)
	//the structured buffers
	exBufferID  stiffnessUAB;
    bufferDesc.ByteWidth = g_NumMasterStrandControlVertices * sizeof(float);
	bufferDesc.Usage = USAGE_DEFAULT;
	bufferDesc.BindFlags =  BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride  = sizeof(float);
	initialData.pSysMem = stiffness;
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &stiffnessUAB));	

	// create the Shader Resource View (SRV) for the structured buffers
	// this will be used by the shaders trying to read data output by the compute shader pass
	
	sbSRVDesc.Buffer.ElementOffset          = 0;
	sbSRVDesc.Buffer.ElementWidth           = sizeof(float); 
	sbSRVDesc.Buffer.FirstElement           = 0;
	sbSRVDesc.Buffer.NumElements            = g_NumMasterStrandControlVertices;
	sbSRVDesc.Format                        = FORMAT_R32_FLOAT; 
	sbSRVDesc.ViewDimension                 = SRV_DIMENSION_BUFFER;
	//////////////////SAFE_RELEASE(g_AngularStiffnessUAB_SRV);
	( IRenderer::GetRendererInstance()->CreateShaderResourceView(stiffnessUAB, &sbSRVDesc, &g_AngularStiffnessUAB_SRV) );

	delete[] stiffness;
	//////////////////SAFE_RELEASE(stiffnessUAB);

	return 0;
}

int CreateCoordinateFrameVB()
{
	int hr;
	
	bufferDesc.ByteWidth = g_NumMasterStrandControlVertices * 3 * 4 * sizeof(float); 
	bufferDesc.Usage = USAGE_DEFAULT;
	bufferDesc.BindFlags =  BIND_VERTEX_BUFFER | BIND_SHADER_RESOURCE |D3D11_BIND_STREAM_OUTPUT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	
	initialData.pSysMem = g_coordinateFrames;
	//////////////////SAFE_RELEASE(g_CoordinateFrameVB[0]);
	//////////////////SAFE_RELEASE(g_CoordinateFrameVB[1]);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_CoordinateFrameVB[0]));
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_CoordinateFrameVB[1]));

	
	desc.Format = FORMAT_RGBA32F;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumMasterStrandControlVertices * 3;
	//////////////////SAFE_RELEASE(g_CoordinateFrameRV[0]);
	//////////////////SAFE_RELEASE(g_CoordinateFrameRV[1]);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_CoordinateFrameVB[0], &desc, &g_CoordinateFrameRV[0]));
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_CoordinateFrameVB[1], &desc, &g_CoordinateFrameRV[1]));


	//buffers for the compute shader-----------------------------------------------------------

	bufferDesc.BindFlags =  BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride  = 4 * sizeof(float);
	//bufferDesc.ByteWidth = g_NumMasterStrandControlVertices * 3 * 4 * sizeof(float);
	//////////////////SAFE_RELEASE(g_CoordinateFrameUAB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_CoordinateFrameUAB));

	return 0; 
}

int CreateTessellatedCoordinateFrameVB()
{
	int hr;
	
	bufferDesc.ByteWidth = g_NumTessellatedMasterStrandVertices * 3 * 4 * sizeof(float); 
	bufferDesc.Usage = USAGE_DEFAULT;
	bufferDesc.BindFlags =   BIND_VERTEX_BUFFER | BIND_SHADER_RESOURCE |D3D11_BIND_STREAM_OUTPUT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	//////////////////SAFE_RELEASE(g_TessellatedCoordinateFrameVB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, 0, &g_TessellatedCoordinateFrameVB));

	
	desc.Format = FORMAT_RGBA32F;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumTessellatedMasterStrandVertices * 3;
	//////////////////SAFE_RELEASE(g_TessellatedCoordinateFrameRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_TessellatedCoordinateFrameVB, &desc, &g_TessellatedCoordinateFrameRV));
	g_Effect->GetVariableByName("g_tessellatedCoordinateFrames")->AsShaderResource()->SetResource(g_TessellatedCoordinateFrameRV);

	return 0;	
}

int CreateCollisionDetectionResources()
{
	int hr = 0;

	// Create the texture
	Image3 img;
	img.create(FORMAT_R32f, MAX_INTERPOLATED_HAIR_PER_WISP, g_NumWisps, 1, 1);
	texture=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());

	//////////////////SAFE_RELEASE( texture );
	return hr;
}

int CreateTessellatedMasterStrandVB()
{
	int hr;

	//create the positions ---------------------------------------------------
	
	bufferDesc.ByteWidth = g_NumTessellatedMasterStrandVertices * sizeof(HairVertex);
	bufferDesc.Usage = USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT | BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	//////////////////SAFE_RELEASE(g_TessellatedMasterStrandVB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, 0, &g_TessellatedMasterStrandVB));

	
	desc.Format = FORMAT_RGBA32F;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumTessellatedMasterStrandVertices;
	//////////////////SAFE_RELEASE(g_TessellatedMasterStrandRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_TessellatedMasterStrandVB, &desc, &g_TessellatedMasterStrandRV));


	//create the tangents -----------------------------------------------------------
	bufferDesc.ByteWidth = g_NumTessellatedMasterStrandVertices * sizeof(HairVertex);
	bufferDesc.Usage = USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT | BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
    //////////////////SAFE_RELEASE(g_TessellatedTangentsVB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, 0, &g_TessellatedTangentsVB));
	desc.Format = FORMAT_RGBA32F;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumTessellatedMasterStrandVertices;
	//////////////////SAFE_RELEASE(g_TessellatedTangentsRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_TessellatedTangentsVB, &desc, &g_TessellatedTangentsRV));

	return 0;
}

int CreateTessellatedWispVB()
{
	int hr;

	Wisp* wisps = new Wisp[g_NumWisps * g_TessellatedMasterStrandLengthMax];
	Wisp* untessellatedWisps = new Wisp[g_NumWisps * g_MasterStrandLengthMax];
	g_NumTessellatedWisps = 0;
	g_NumUntessellatedWisps = 0;

	D3DXFROMWINEVECTOR3* rootsS = new D3DXFROMWINEVECTOR3[g_NumWisps];
	g_masterCVIndices = new D3DXFROMWINEVECTOR3[g_NumWisps];
	D3DXFROMWINEVECTOR4*  masterLengths = new D3DXFROMWINEVECTOR4[g_NumWisps];

	D3DXFROMWINEVECTOR3* rootsSUntessellated = new D3DXFROMWINEVECTOR3[g_NumWisps];
	D3DXFROMWINEVECTOR4* masterLengthsUntessellated = new D3DXFROMWINEVECTOR4[g_NumWisps];

	int* indices = g_indices;
	g_MaxMStrandSegments = 0;

	for (Index w = 0; w < g_NumWisps; w++, indices += 3) 
	{
		int offset[3];
		Index num[3];
		int offsetUnTessellated[3];
		int numUnTessellated[3];

		for (int i = 0; i < 3; ++i) 
		{
			int index = indices[i];
			offset[i] = index == 0 ? 0 : g_TessellatedMasterStrandVertexOffsets[index - 1];
			num[i] = g_TessellatedMasterStrandVertexOffsets[index] - offset[i];

			offsetUnTessellated[i] = index == 0 ? 0 : g_MasterStrandControlVertexOffsets[index - 1];
			numUnTessellated[i] = g_MasterStrandControlVertexOffsets[index] - offsetUnTessellated[i];
		}

		//tessellated
		rootsS[w] = D3DXFROMWINEVECTOR3(offset[0], offset[1], offset[2]);
		g_masterCVIndices[w] = D3DXFROMWINEVECTOR3(indices[0], indices[1], indices[2]); //the positions of the 3 vertices of the triangle in the original VB of CVs
		Index n = (num[0] + num[1] + num[2]) / 3;
		masterLengths[w] = D3DXFROMWINEVECTOR4(num[0]-1,num[1]-1,num[2]-1,min(min(num[0]-1,num[1]-1),num[2]-1));
		g_MaxMStrandSegments = max((int)masterLengths[w].w - 1, g_MaxMStrandSegments);

		//Un - tessellated
		rootsSUntessellated[w] = D3DXFROMWINEVECTOR3(offsetUnTessellated[0], offsetUnTessellated[1], offsetUnTessellated[2]);
		Index nUntessellated = (numUnTessellated[0] + numUnTessellated[1] + numUnTessellated[2]) / 3;
		masterLengthsUntessellated[w] = D3DXFROMWINEVECTOR4(numUnTessellated[0]-1,numUnTessellated[1]-1,numUnTessellated[2]-1,n);

		//tessellated VB
		for (Index v = 0; v < n; ++v, ++g_NumTessellatedWisps) 
		{
			for (int i = 0; i < 3; ++i)
			{
				wisps[g_NumTessellatedWisps][i] = offset[i] + (v >= num[i] ? num[i] - 1 : v);
			}
			wisps[g_NumTessellatedWisps][3] = w + 1;
		}
		wisps[g_NumTessellatedWisps - 1][3] *= - 1;


		//untessellated VB
		for (Index v = 0; v < nUntessellated; ++v, ++g_NumUntessellatedWisps) 
		{
			for (int i = 0; i < 3; ++i)
			{
				untessellatedWisps[g_NumUntessellatedWisps][i] = offsetUnTessellated[i] + (v >= numUnTessellated[i] ? numUnTessellated[i] - 1 : v);
			}
			untessellatedWisps[g_NumUntessellatedWisps][3] = w + 1;
		}
		untessellatedWisps[g_NumUntessellatedWisps - 1][3] *= - 1;


	}

	//create a constant buffer for the root indices
	
	initialData2.pSysMem = rootsS;
	initialData2.SysMemPitch = 0;
	initialData2.SysMemSlicePitch = 0;
	
	bufferDesc2.ByteWidth = g_NumWisps * sizeof(D3DXFROMWINEVECTOR3);
	bufferDesc2.Usage = USAGE_IMMUTABLE;
	bufferDesc2.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc2.CPUAccessFlags = 0;
	bufferDesc2.MiscFlags = 0;
	//////////////////SAFE_RELEASE(g_rootsIndices);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc2, &initialData2, &g_rootsIndices));
	
	ZeroMemory( &desc, sizeof(desc) );
	desc.Format = FORMAT_R32G32B32_FLOAT;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumWisps;
	//////////////////SAFE_RELEASE(g_RootsIndicesRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_rootsIndices, &desc, &g_RootsIndicesRV));
	g_Effect->GetVariableByName("g_MasterStrandRootIndices")->AsShaderResource()->SetResource(g_RootsIndicesRV);
	delete [] rootsS;
	//untessellated
	initialData2.pSysMem = rootsSUntessellated;
	//////////////////SAFE_RELEASE(g_rootsIndicesUntessellated);
	//////////////////SAFE_RELEASE(g_RootsIndicesRVUntessellated);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc2, &initialData2, &g_rootsIndicesUntessellated));
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_rootsIndicesUntessellated, &desc, &g_RootsIndicesRVUntessellated));
	g_Effect->GetVariableByName("g_MasterStrandRootIndicesUntessellated")->AsShaderResource()->SetResource(g_RootsIndicesRVUntessellated); 
	delete[] rootsSUntessellated;
	//single strand
	exBufferID  rootsIndexVB;
	initialData2.pSysMem = g_TessellatedMasterStrandVertexOffsets;
	initialData2.SysMemPitch = 0;
	initialData2.SysMemSlicePitch = 0;
	bufferDesc2.ByteWidth = g_NumMasterStrands * sizeof(Index); 
	bufferDesc2.Usage = USAGE_IMMUTABLE;
	bufferDesc2.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc2.CPUAccessFlags = 0;
	bufferDesc2.MiscFlags = 0;
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc2, &initialData2, &rootsIndexVB));
	ZeroMemory( &desc, sizeof(desc) );
	desc.Format = FORMAT_R32_SINT; 
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumMasterStrands;
	//////////////////SAFE_RELEASE(g_TessellatedMasterStrandRootIndexRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(rootsIndexVB, &desc, &g_TessellatedMasterStrandRootIndexRV));
	g_Effect->GetVariableByName("g_tessellatedMasterStrandRootIndex")->AsShaderResource()->SetResource(g_TessellatedMasterStrandRootIndexRV);
	//////////////////SAFE_RELEASE(rootsIndexVB);


	initialData2.pSysMem = g_masterCVIndices;
	initialData2.SysMemPitch = 0;
	initialData2.SysMemSlicePitch = 0;
	
	bufferDesc3.ByteWidth = g_NumWisps * sizeof(D3DXFROMWINEVECTOR3);
	bufferDesc3.Usage = USAGE_IMMUTABLE;
	bufferDesc3.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc3.CPUAccessFlags = 0;
	bufferDesc3.MiscFlags = 0;
	
	ZeroMemory( &desc3, sizeof(desc3) );
	desc3.Format = FORMAT_R32G32B32_FLOAT; 
	desc3.ViewDimension = SRV_DIMENSION_BUFFER;
	desc3.Buffer.ElementOffset = 0;
	desc3.Buffer.ElementWidth = g_NumWisps;
	//////////////////SAFE_RELEASE(g_rootsIndicesMasterStrand);
	//////////////////SAFE_RELEASE(g_rootsIndicesMasterStrandRV);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc3, &initialData2, &g_rootsIndicesMasterStrand));
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_rootsIndicesMasterStrand, &desc3, &g_rootsIndicesMasterStrandRV));
	g_Effect->GetVariableByName("g_OriginalMasterStrandRootIndices")->AsShaderResource()->SetResource(g_rootsIndicesMasterStrandRV);

	initialData2.pSysMem = masterLengths;
	initialData2.SysMemPitch = 0;
	initialData2.SysMemSlicePitch = 0;
	bufferDesc2.ByteWidth = g_NumWisps * sizeof(D3DXFROMWINEVECTOR4);
	bufferDesc2.Usage = USAGE_IMMUTABLE;
	bufferDesc2.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc2.CPUAccessFlags = 0;
	bufferDesc2.MiscFlags = 0;
	//////////////////SAFE_RELEASE(g_masterLengths);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc2, &initialData2, &g_masterLengths));
	ZeroMemory( &desc, sizeof(desc) );
	desc.Format = FORMAT_RGBA32F;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumWisps;
	//////////////////SAFE_RELEASE(g_MasterLengthsRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_masterLengths, &desc, &g_MasterLengthsRV));
	g_Effect->GetVariableByName("g_MasterStrandLengths")->AsShaderResource()->SetResource(g_MasterLengthsRV);
	delete[] masterLengths;
	//untessellated
	initialData2.pSysMem = masterLengthsUntessellated;
	//////////////////SAFE_RELEASE(g_masterLengthsUntessellated);
	//////////////////SAFE_RELEASE(g_MasterLengthsRVUntessellated);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc2, &initialData2, &g_masterLengthsUntessellated));
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_masterLengthsUntessellated, &desc, &g_MasterLengthsRVUntessellated));
	g_Effect->GetVariableByName("g_MasterStrandLengthsUntessellated")->AsShaderResource()->SetResource(g_MasterLengthsRVUntessellated);
	delete[] masterLengthsUntessellated;


	//The VBs
	
	initialData.pSysMem = wisps;
	
	bufferDesc.ByteWidth = g_NumTessellatedWisps * sizeof(Wisp);
	bufferDesc.Usage = USAGE_DEFAULT;
	bufferDesc.BindFlags = BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	//////////////////SAFE_RELEASE(g_TessellatedWispVB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_TessellatedWispVB));
	//untessellated
	initialData.pSysMem = untessellatedWisps;
	bufferDesc.ByteWidth = g_NumUntessellatedWisps * sizeof(Wisp);
	//////////////////SAFE_RELEASE(g_UnTessellatedWispVB);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_UnTessellatedWispVB));

	delete [] wisps;
	delete [] untessellatedWisps;

	return 0;
}

int CreateComputeShaderConstantBuffers()
{
	int hr = 0;

    
    Desc.Usage = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc.MiscFlags = 0;
    Desc.ByteWidth = sizeof( CB_CS_PER_FRAME );
	//////////////////SAFE_RELEASE(g_pcbCSPerFrame);
    ( IRenderer::GetRendererInstance()->CreateBuffer( &Desc, 0, &g_pcbCSPerFrame ) );

    Desc.ByteWidth = sizeof( CB_CS_TRANSFORMS );
	//////////////////SAFE_RELEASE(g_pcbCSTransforms);
    ( IRenderer::GetRendererInstance()->CreateBuffer( &Desc, 0, &g_pcbCSTransforms ) );

    Desc.ByteWidth = sizeof( D3DXFROMWINEMATRIX );	
	//////////////////SAFE_RELEASE(g_pcbCSRootXform);
    ( IRenderer::GetRendererInstance()->CreateBuffer( &Desc, 0, &g_pcbCSRootXform ) );

    return 0;
}

//-----------------------------------------------------------------------------------------

int saveHair()
{
	int hr = 0;

	ofstream outfile;
	outfile.open ("hairData.txt",ios::trunc|ios::out);

	//outfile<<fixed;
	outfile<<showpoint;

	//write out the vertex count
	outfile<<g_NumMasterStrandControlVertices<<endl;

	//write out the master strand control vertices
	for(int i=0;i<g_NumMasterStrandControlVertices;i++)
		outfile<<'('<<g_MasterStrandControlVertices[i].Position.x<<
		','<<g_MasterStrandControlVertices[i].Position.y<<
		','<<g_MasterStrandControlVertices[i].Position.z<<')';
	outfile<<endl;

	//write out the number of master strands
	outfile<<g_NumMasterStrands<<endl;

	//write out the control vertex offset array
	for(int i=0;i<g_NumMasterStrands;i++)
		outfile<<g_MasterStrandControlVertexOffsets[i]<<' '; 
	outfile<<endl;

	//write out the texcoords
	for(int i=0;i<g_NumMasterStrands;i++)
		outfile<<'('<<g_MasterAttributes[i].texcoord.x<<','<<g_MasterAttributes[i].texcoord.y<<')'; 
	outfile<<endl;

	//write out the coordinate frames of the master strands
	for(int i=0;i<g_NumMasterStrandControlVertices;i++)
	{
		outfile<<'('<<g_coordinateFrames[i*12+0]<<','
			<<g_coordinateFrames[i*12+1]<<','
			<<g_coordinateFrames[i*12+2]<<','
			<<g_coordinateFrames[i*12+4]<<','
			<<g_coordinateFrames[i*12+5]<<','
			<<g_coordinateFrames[i*12+6]<<','
			<<g_coordinateFrames[i*12+8]<<','
			<<g_coordinateFrames[i*12+9]<<','
			<<g_coordinateFrames[i*12+10]<<')';
	}
	outfile<<endl;

	//write out the fractional length to root for each vertex
	for(int i=0;i<g_NumMasterStrandControlVertices;i++)
		outfile<<g_masterStrandLengthToRoot[i]<<' ';
	outfile<<endl;

	//write out the total real length for each strand
	for(int i=0;i<g_NumMasterStrands;i++)
		outfile<<g_masterStrandLengths[i]<<' ';
	outfile<<endl;

	//write out the number of strand variables
	outfile<<64<<endl;

	//write out the clump offsets
	for(int i=0;i<64;i++)
		outfile<<'('<<g_clumpCoordinates[i].x<<','<<g_clumpCoordinates[i].y<<')';
	outfile<<endl;

	//write out the strand widths
	for(int i=0;i<64;i++)
		outfile<<g_Strand_Sizes[i]<<' ';
	outfile<<endl;

	//write out the strand lengths
	for(int i=0;i<64;i++)
		outfile<<g_lengths[i]<<' ';
	outfile<<endl;

	//write out the tangent jitter
	for(int i=0;i<64;i++)
		outfile<<'('<<g_tangentJitter[i].x<<','<<g_tangentJitter[i].y<<')';
	outfile<<endl;

	//barycentric hair--------------------------------

	//write out the face count
	int faceCount = g_NumWisps;
	outfile<<faceCount<<endl; 

	//write out the index buffer
	for(int i=0;i<faceCount*3;i++)
		outfile<<g_indices[i]<<" ";
	outfile<<endl;

	//write out the random barycentric coordinates
	for(int i=0;i<64;i++)
		outfile<<'('<<g_BarycentricCoordinates[i].x<<','<<g_BarycentricCoordinates[i].y<<')';
	outfile<<endl;

	outfile.close();
	return hr;
}


//random numbers; replace with a better quality generator for better particle distribution
//(0,1]
float random()
{
	return (float(   (double)stx_rand() / ((double)(RAND_MAX)+(double)(1)) ));
}



void BoxMullerTransform(float& var1, float& var2)
{
	float unifVar1 = random();
	float unifVar2 = random();
	float temp = sqrt(-2*log(unifVar1));
	var1 = temp*cos(2*D3DXFROMWINE_PI*unifVar2);
	var2 = temp*sin(2*D3DXFROMWINE_PI*unifVar2);
}



void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
{
	float minVal, maxVal, delta;

	minVal = min( min(r, g), b );
	maxVal = max( max(r, g), b );
	*v = maxVal;				// v

	delta = maxVal - minVal;

	if( maxVal != 0 )
		*s = delta / maxVal;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}

	if( r == maxVal )
		*h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == maxVal )
		*h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		*h = 4 + ( r - g ) / delta;	// between magenta & cyan

	*h *= 60;				// degrees
	if( *h < 0 )
		*h += 360;

}

void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;

	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}

	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i ) {
case 0:
	*r = v;
	*g = t;
	*b = p;
	break;
case 1:
	*r = q;
	*g = v;
	*b = p;
	break;
case 2:
	*r = p;
	*g = v;
	*b = t;
	break;
case 3:
	*r = p;
	*g = q;
	*b = v;
	break;
case 4:
	*r = t;
	*g = p;
	*b = v;
	break;
default:		// case 5:
	*r = v;
	*g = p;
	*b = q;
	break;
	}

}

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)

//create random strand colors
int CreateStrandColors()
{
	
	float3* colors = new float3[g_NumStrandVariables];


	float3 baseColorRGB = float3(117/255.0f,76/255.0f,36/255.0f);
	float3 baseColorHSV; //this is the mean
	RGBtoHSV(baseColorRGB.x,baseColorRGB.y,baseColorRGB.z,&baseColorHSV.x,&baseColorHSV.y,&baseColorHSV.z);
	float3 HSVSDs = float3(1,0.05f,0.05f);

	for (int i = 0; i < g_NumStrandVariables; ++i) 
	{
		float h,s,v,t,r,g,b;
		BoxMullerTransform(h,s);
		BoxMullerTransform(v,t); 
		h = HSVSDs.x*h + baseColorHSV.x;
		s = HSVSDs.y*s + baseColorHSV.y;
		v = HSVSDs.z*v + baseColorHSV.z;
		HSVtoRGB(&r,&g,&b,h,s,v);
		colors[i] = float3(r,g,b);

	}

	exBufferID  StrandColors;
	initialData.pSysMem = colors;
	int hr;
	
	bufferDesc.ByteWidth = g_NumStrandVariables * sizeof(float3);
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &StrandColors));
	
	desc.Format = FORMAT_R32G32B32_FLOAT;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumStrandVariables;
	//////////////////SAFE_RELEASE(g_StrandColorsRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(StrandColors, &desc, &g_StrandColorsRV));
	g_Effect->GetVariableByName("g_StrandColors")->AsShaderResource()->SetResource(g_StrandColorsRV);
	delete [] colors;
	//////////////////SAFE_RELEASE(StrandColors);
	return 0;
}


//create strand deviations and strand sizes
int CreateDeviantStrandsAndStrandSizes()
{
	
	float2* randStrandDeviations = new float2[g_NumStrandVariables*g_TessellatedMasterStrandLengthMax];


	//deviant strands-------------------------------------------------------------------------
	//using bsplines:
	int numCVs = 7;
	int numSegments = numCVs - 3;
	int numVerticesPerSegment = ceil(g_TessellatedMasterStrandLengthMax/float(numSegments));
	float uStep = 1.0/numVerticesPerSegment;
	D3DXFROMWINEVECTOR2* CVs = new D3DXFROMWINEVECTOR2[numCVs];
	CVs[0] = D3DXFROMWINEVECTOR2(0,0);
	CVs[1] = D3DXFROMWINEVECTOR2(0,0);
	float x,y;
	int index = 0;
	int lastIndex = 0;

	D3DXFROMWINEMATRIX basisMatrix = D3DXFROMWINEMATRIX
		(
		-1/6.0f,  3/6.0f, -3/6.0f,  1/6.0f,
		3/6.0f, -6/6.0f,      0,  4/6.0f,
		-3/6.0f,  3/6.0f,  3/6.0f,  1/6.0f,
		1/6.0f,      0,       0,      0
		);


	//strand sizes---------------------------------------------------------------------------
	//make the deviant strands thinner
	g_Strand_Sizes = new float[g_NumStrandVariables];
	float size;

	float minSize;
	float threshold;
	float variance;

	minSize = 0.06f;
	threshold = 0.75f;
	variance = 0.0208f;

	//-------------------------------------------------------------------------------------

	float sdScale = 1.0f;
	if(g_useShortHair) sdScale = 0.6f;

	for (int i = 0; i < g_NumStrandVariables; i++) 
	{
		float randomChoice = random();
		if(randomChoice>0.6)
		{
			float maxLength = (1.0-g_thinning) + g_thinning*g_lengths[i%g_NumStrandVariables];//between 0 and 1
			int maxCV = floor(maxLength*numSegments)+ 2;//can do 1 here with sd=1.2;

			//create the random CVs
			for(int j=2;j<numCVs;j++)
			{   
				float sd;

				if(randomChoice > 0.95)//make some very stray hair
					sd = 1.2f;
				else if(randomChoice > 0.8)
					sd = 0.8f;
				else //make some lesser stray hair that are more deviant near the ends
				{
					if(maxLength>((numCVs-1)/numCVs) && j==numCVs-1)    
						sd = 100.0f;
					else if(j>=maxCV)
						sd = 4.0f;
					else 
						sd = 0.12f;
				}			 

				BoxMullerTransform(x,y);
				x *= sd * sdScale;
				y *= sd * sdScale; 
				CVs[j] = D3DXFROMWINEVECTOR2(x,y);
			}

			//create the points
			for(int s=0;s<numSegments;s++)
			{
				for(float u=0;u<1.0;u+=uStep)
				{  
					float4 basis;
					vectorMatrixMultiply(&basis,basisMatrix,float4(u * u * u, u * u, u, 1));  
					float2 position = float2(0,0);
					for (int c = 0; c < 4; ++c) 
						position += basis[c] * CVs[s+c];
					if( index-lastIndex < (int)g_TessellatedMasterStrandLengthMax )
						randStrandDeviations[index++] = position;
				}
			}
		}
		else
		{
			for(unsigned int j=0;j<g_TessellatedMasterStrandLengthMax;j++)
				randStrandDeviations[index++] = float2(0,0);
		}


		//sizes
		size =  minSize;
		if( randomChoice > 0.9)
			size -= variance;
		else if( randomChoice > threshold)
			size += (random()-0.5)*variance;
		g_Strand_Sizes[i] = size;



		lastIndex = index;
	}

	//random deviations---------------------------------------------------------------------------
	exBufferID  randNumbersVB;
	initialData.pSysMem = randStrandDeviations;
	int hr;
	
	bufferDesc.ByteWidth = g_NumStrandVariables*g_TessellatedMasterStrandLengthMax * sizeof(float2);
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &randNumbersVB));
	
	desc.Format = FORMAT_R32G32_FLOAT;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumStrandVariables*g_TessellatedMasterStrandLengthMax;
	//////////////////SAFE_RELEASE(g_StrandDeviationsRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(randNumbersVB, &desc, &g_StrandDeviationsRV));
	g_Effect->GetVariableByName("g_strandDeviations")->AsShaderResource()->SetResource(g_StrandDeviationsRV);
	//////////////////SAFE_RELEASE(randNumbersVB);
	delete[] randStrandDeviations;

	//sizes-----------------------------------------------------------------------------------------

	initialData.pSysMem = g_Strand_Sizes;
	bufferDesc.ByteWidth = g_NumStrandVariables * sizeof(float);
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	//////////////////SAFE_RELEASE(g_StrandSizes);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_StrandSizes));
	desc.Format = FORMAT_R32_FLOAT;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumStrandVariables;
	//////////////////SAFE_RELEASE(g_StrandSizesRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_StrandSizes, &desc, &g_StrandSizesRV));
	g_Effect->GetVariableByName("g_StrandSizes")->AsShaderResource()->SetResource(g_StrandSizesRV);
	//delete [] sizes;

	delete[] CVs;

	return 0;
}



int CreateCurlDeviations()
{
	//curls---------------------------------------------------------------------------------------
	int hr;

	int numCVs = 13;
	float sd = 1.5;
    if(g_useShortHair)
	{
		numCVs = 10;
		sd = 1.2f;
	}
	int numSegments = numCVs - 3;
	int numVerticesPerSegment = ceil(g_TessellatedMasterStrandLengthMax/float(numSegments));
	float uStep = 1.0/numVerticesPerSegment;
	D3DXFROMWINEVECTOR2* CVs = new D3DXFROMWINEVECTOR2[numCVs];
	CVs[0] = D3DXFROMWINEVECTOR2(0,0);
	CVs[1] = D3DXFROMWINEVECTOR2(0,0);
	CVs[2] = D3DXFROMWINEVECTOR2(0,0);
	CVs[3] = D3DXFROMWINEVECTOR2(0,0);
	float x,y;
	int index = 0;
	int lastIndex = 0;

	D3DXFROMWINEMATRIX basisMatrix = D3DXFROMWINEMATRIX
		(
		-1/6.0f,  3/6.0f, -3/6.0f,  1/6.0f,
		3/6.0f, -6/6.0f,      0,  4/6.0f,
		-3/6.0f,  3/6.0f,  3/6.0f,  1/6.0f,
		1/6.0f,      0,       0,      0
		);


	float2* curlDeviations = new float2[g_NumMasterStrands*g_TessellatedMasterStrandLengthMax];
	for (int i = 0; i < g_NumMasterStrands; i++) 
	{
		//create the random CVs
		for(int j=4;j<numCVs;j++)
		{   
			BoxMullerTransform(x,y);
			x *= sd;
			y *= sd; 
			CVs[j] = D3DXFROMWINEVECTOR2(x,y);
		}

		//create the points
		for(int s=0;s<numSegments;s++)
		{
			for(float u=0;u<1.0;u+=uStep)
			{  
				float4 basis;
				vectorMatrixMultiply(&basis,basisMatrix,float4(u * u * u, u * u, u, 1));  
				float2 position = float2(0,0);
				for (int c = 0; c < 4; ++c) 
					position += basis[c] * CVs[s+c];
				if( index-lastIndex < (int)g_TessellatedMasterStrandLengthMax )
					curlDeviations[index++] = position;
			}
		}

		lastIndex = index;
	}

	exBufferID  curlsVB;
	
	initialData.pSysMem = curlDeviations;
	
	bufferDesc.ByteWidth = g_NumMasterStrands*g_TessellatedMasterStrandLengthMax * sizeof(float2);
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &curlsVB));
	
	desc.Format = FORMAT_R32G32_FLOAT;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumMasterStrands*g_TessellatedMasterStrandLengthMax;
	//////////////////SAFE_RELEASE(g_CurlDeviationsRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(curlsVB, &desc, &g_CurlDeviationsRV));
	g_Effect->GetVariableByName("g_curlDeviations")->AsShaderResource()->SetResource(g_CurlDeviationsRV);
	//////////////////SAFE_RELEASE(curlsVB);
	delete[] curlDeviations;
	delete[] CVs;

	return 0;
}

int CreateStrandLengths()
{
	
	g_lengths = new float[g_NumStrandVariables];

	float minLength = 0.2f;

	for (int i = 0; i < g_NumStrandVariables; ++i) 
		g_lengths[i] = random()*(1-minLength) + minLength;

	exBufferID  StrandLengths;
	initialData.pSysMem = g_lengths;
	int hr;
	
	bufferDesc.ByteWidth = g_NumStrandVariables * sizeof(float);
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &StrandLengths));
	
	desc.Format = FORMAT_R32_FLOAT;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumStrandVariables;
	//////////////////SAFE_RELEASE(g_StrandLengthsRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(StrandLengths, &desc, &g_StrandLengthsRV));
	g_Effect->GetVariableByName("g_StrandLengths")->AsShaderResource()->SetResource(g_StrandLengthsRV);
    //////////////////SAFE_RELEASE(StrandLengths);

	return 0;
}

int CreateStrandBarycentricCoordinates()
{
	
	if(g_BarycentricCoordinates)
		delete[] g_BarycentricCoordinates;
	g_BarycentricCoordinates = new D3DXFROMWINEVECTOR2[g_NumStrandVariables];
	float coord1 = 1;
	float coord2 = 0;
	for (int c = 0; c < g_NumStrandVariables; ++c) 
	{
		coord1 = random();
		coord2 = random();
		if(coord1 + coord2 > 1)
		{
			coord1 = 1.0 - coord1;
			coord2 = 1.0 - coord2;
		}


		g_BarycentricCoordinates[c] = D3DXFROMWINEVECTOR2(coord1, coord2);
	}
	initialData.pSysMem = g_BarycentricCoordinates;
	int hr;
	
	bufferDesc.ByteWidth = g_NumStrandVariables * sizeof(D3DXFROMWINEVECTOR2);
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	//////////////////SAFE_RELEASE(g_StrandCoordinates);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_StrandCoordinates));
	
	desc.Format = FORMAT_R32G32_FLOAT;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumStrandVariables;
	//////////////////SAFE_RELEASE(g_StrandCoordinatesRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_StrandCoordinates, &desc, &g_StrandCoordinatesRV));
	g_Effect->GetVariableByName("g_StrandCoordinates")->AsShaderResource()->SetResource(g_StrandCoordinatesRV);
	return 0;
}



int CreateTangentJitter()
{
	
	if(g_tangentJitter) delete[] g_tangentJitter;
	g_tangentJitter = new D3DXFROMWINEVECTOR2[g_NumStrandVariables];
	float val1 = 0;
	float val2 = 0;
	for (int c = 0; c < g_NumStrandVariables; ++c) 
	{
		val1 = random()-0.5;
		val2 = random()-0.5;
		g_tangentJitter[c] = D3DXFROMWINEVECTOR2(val1, val2);
	}
	exBufferID  TangentJitterVB;
	initialData.pSysMem = g_tangentJitter;
	int hr;
	
	bufferDesc.ByteWidth = g_NumStrandVariables * sizeof(D3DXFROMWINEVECTOR2);
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &TangentJitterVB));
	
	desc.Format = FORMAT_R32G32_FLOAT;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumStrandVariables;
	//////////////////SAFE_RELEASE(g_TangentJitterRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(TangentJitterVB, &desc, &g_TangentJitterRV));
	g_Effect->GetVariableByName("g_TangentJitter")->AsShaderResource()->SetResource(g_TangentJitterRV);
	//delete [] jitter;
	//////////////////SAFE_RELEASE(TangentJitterVB);
	return 0;
}


int CreateStrandToClumpIndexMap()
{
	int hr = 0;

	//map the texture for reading
	char filename[MAX_PATH];
	char wfilename[MAX_PATH];
	char fullPath[MAX_PATH];

	FILE * inTexture;
	errno_t error;
	if((error = fopen_s(&inTexture,"clumpMapBarycentric.raw","r")) != 0)
	{
        LOG_PRINT("Could not find file clumpMapBarycentric.raw","Error",MB_OK);
		return 1;
	}

#define textureSize 256
	float texture[textureSize][textureSize];
	unsigned char * buffer;
	buffer = (unsigned char*) malloc (sizeof(unsigned char)*textureSize*textureSize);
	fread (buffer,sizeof(unsigned char),textureSize*textureSize,inTexture);

	int ind=0;

	//read the texture
	for(int y=0;y<textureSize;y++)
		for(int x=0;x<textureSize;x++)
			texture[y][x] = float(buffer[ind++])/255.0;

	fclose(inTexture);

	//for each masterStrand sample the texture, and decide on how many strands go in the clump, save this info
	int* numStrandsCumulativeTri = new int[g_NumMasterStrands];
	g_totalSStrands = 0;
	int minSStrands = 1 << 30;
	int maxSStrands = 0;
	int avgSStrands = 0;
	for(int i=0;i<g_NumMasterStrands;i++)
	{
		D3DXFROMWINEVECTOR2 Texcoord = g_MasterAttributes[i].texcoord;
		int num = texture[int(Texcoord.y*256)][int(Texcoord.x*256)]*g_NumSStrandsPerWisp;
		numStrandsCumulativeTri[i] = num;
		minSStrands = min(num, minSStrands);
		maxSStrands = max(num, maxSStrands);
		avgSStrands += num;
		if(i>0)
			numStrandsCumulativeTri[i] += numStrandsCumulativeTri[i-1];		
	}
	avgSStrands /= g_NumMasterStrands;
	g_MaxSStrandsPerControlHair = maxSStrands;
	g_totalSStrands = numStrandsCumulativeTri[g_NumMasterStrands-1];

	//upload map to buffer
	{
	exBufferID  SStrandsPerMasterStrandCumulative;
	
	initialData.pSysMem = numStrandsCumulativeTri;
	initialData.SysMemPitch = 0;
	initialData.SysMemSlicePitch = 0;
	
	bufferDesc.ByteWidth = g_NumMasterStrands * sizeof(unsigned); 
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &SStrandsPerMasterStrandCumulative));
	
	ZeroMemory( &desc, sizeof(desc) );
	desc.Format = FORMAT_R32_SINT; 
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumMasterStrands;
	//////////////////SAFE_RELEASE(g_SStrandsPerMasterStrandCumulative);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(SStrandsPerMasterStrandCumulative, &desc, &g_SStrandsPerMasterStrandCumulative));
	g_Effect->GetVariableByName("g_SStrandsPerMasterStrandCumulative")->AsShaderResource()->SetResource(g_SStrandsPerMasterStrandCumulative);
	//////////////////SAFE_RELEASE(SStrandsPerMasterStrandCumulative);
	}

	//make a map where each strand is mapped to its masterStrand number
	float* Map=new float[g_totalSStrands];
	int currentTriIndex=0;
	int previosCNumber = 0;
	for(int i=0;i<g_totalSStrands;i++)
	{
		while(i>=numStrandsCumulativeTri[currentTriIndex])
		{
			previosCNumber = numStrandsCumulativeTri[currentTriIndex];
			currentTriIndex++;
		}
		Map[i] = currentTriIndex;

		Map[i] += (i-previosCNumber)/1000.0;
	}


	//upload map to buffer
	exBufferID  SstrandsIndexVB;
	
	initialData.pSysMem = Map;
	initialData.SysMemPitch = 0;
	initialData.SysMemSlicePitch = 0;
	
	bufferDesc.ByteWidth = g_totalSStrands * sizeof(float); 
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &SstrandsIndexVB));
	
	ZeroMemory( &desc, sizeof(desc) );
	desc.Format = FORMAT_R32_FLOAT; 
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_totalSStrands;
	//////////////////SAFE_RELEASE(g_SstrandsIndexRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(SstrandsIndexVB, &desc, &g_SstrandsIndexRV));
	g_Effect->GetVariableByName("g_sStrandIndices")->AsShaderResource()->SetResource(g_SstrandsIndexRV);
	//////////////////SAFE_RELEASE(SstrandsIndexVB);

	free (buffer);
	delete[] Map;
	delete[] numStrandsCumulativeTri;

	return hr;
}

int CreateStrandToBarycentricIndexMap()
{
	int hr = 0;

	//map the texture for reading
	char filename[MAX_PATH];
	char wfilename[MAX_PATH];
	char fullPath[MAX_PATH];

	FILE * inTexture;
	errno_t error;
	if((error = fopen_s(&inTexture,"densityMapBarycentric.raw","r")) != 0)
	{
        LOG_PRINT("Could not find file densityMapBarycentric.raw","Error",MB_OK);
		return 1;
	}

#define textureSize 256
	float texture[textureSize][textureSize];
	unsigned char * buffer;
	buffer = (unsigned char*) malloc (sizeof(unsigned char)*textureSize*textureSize);
	fread (buffer,sizeof(unsigned char),textureSize*textureSize,inTexture);

	int ind=0;

	//read the texture
	for(int y=0;y<textureSize;y++)
		for(int x=0;x<textureSize;x++)
			texture[y][x] = float(buffer[ind++])/255.0;

	fclose(inTexture);

	//for each triangle average its 3 texcoords, sample the texture, and decide on how many strands go in the triangle, save this info
	int* numStrandsCumulativeTri = new int[g_NumWisps];
	g_totalMStrands = 0;
	g_MaxMStrandsPerWisp = 0;
	for(int i=0;i<g_NumWisps;i++)
	{
		D3DXFROMWINEVECTOR3 indices = g_masterCVIndices[i];
		D3DXFROMWINEVECTOR2 averageTexcoord= D3DXFROMWINEVECTOR2(0,0);
		averageTexcoord += 0.3333f*g_MasterAttributes[(int)indices.x].texcoord;
		averageTexcoord += 0.3333f*g_MasterAttributes[(int)indices.y].texcoord;
		averageTexcoord += 0.3333f*g_MasterAttributes[(int)indices.z].texcoord;

		int num = texture[int(averageTexcoord.y*256)][int(averageTexcoord.x*256)]*g_NumMStrandsPerWisp;
		numStrandsCumulativeTri[i] = num;
		g_MaxMStrandsPerWisp = max((unsigned)num, g_MaxMStrandsPerWisp);
		if(i>0)
			numStrandsCumulativeTri[i] += numStrandsCumulativeTri[i-1];		
	}
	g_totalMStrands = numStrandsCumulativeTri[g_NumWisps-1];

	//upload map to buffer
	{
	exBufferID  MStrandsPerWispCumulative;
	
	initialData.pSysMem = numStrandsCumulativeTri;
	initialData.SysMemPitch = 0;
	initialData.SysMemSlicePitch = 0;
	
	bufferDesc.ByteWidth = g_NumWisps * sizeof(unsigned); 
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &MStrandsPerWispCumulative));
	
	ZeroMemory( &desc, sizeof(desc) );
	desc.Format = FORMAT_R32_SINT; 
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumWisps;
	//////////////////SAFE_RELEASE(g_MStrandsPerWispCumulative);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(MStrandsPerWispCumulative, &desc, &g_MStrandsPerWispCumulative));
	g_Effect->GetVariableByName("g_MStrandsPerWispCumulative")->AsShaderResource()->SetResource(g_MStrandsPerWispCumulative);
	//////////////////SAFE_RELEASE(MStrandsPerWispCumulative);
	}

	//make a map where each strand is mapped to its triangle number
	float* Map=new float[g_totalMStrands];
	int currentTriIndex=0;
	int previosCNumber = 0;
	for(int i=0;i<g_totalMStrands;i++)
	{
		while(i>=numStrandsCumulativeTri[currentTriIndex])
		{
			previosCNumber = numStrandsCumulativeTri[currentTriIndex];
			currentTriIndex++;
		}
		Map[i] = currentTriIndex;

		Map[i] += (i-previosCNumber)/1000.0;
	}


	//upload map to buffer
	exBufferID  MstrandsIndexVB;
	
	initialData.pSysMem = Map;
	initialData.SysMemPitch = 0;
	initialData.SysMemSlicePitch = 0;
	
	bufferDesc.ByteWidth = g_totalMStrands * sizeof(float); 
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &MstrandsIndexVB));
	
	ZeroMemory( &desc, sizeof(desc) );
	desc.Format = FORMAT_R32_FLOAT; 
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_totalMStrands;
	//////////////////SAFE_RELEASE(g_MstrandsIndexRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(MstrandsIndexVB, &desc, &g_MstrandsIndexRV));
	g_Effect->GetVariableByName("g_mStrandIndices")->AsShaderResource()->SetResource(g_MstrandsIndexRV);
	//////////////////SAFE_RELEASE(MstrandsIndexVB);

	free (buffer);
	delete[] Map;
	delete[] numStrandsCumulativeTri;

	return hr;
}

int CreateRandomCircularCoordinates()
{
	if(g_clumpCoordinates)
		delete[] g_clumpCoordinates;
	g_clumpCoordinates = new D3DXFROMWINEVECTOR2[g_NumStrandVariables];
	//create two coordinates that lie inside the unit circle
	int c = 0;
	float coord1 = 0;
	float coord2 = 0;
	g_clumpCoordinates[c++] = D3DXFROMWINEVECTOR2(coord1, coord2);
	while(c<g_NumStrandVariables)
	{
		//distributed with a gaussian distribution
		BoxMullerTransform(coord1,coord2);
		float sd = 0.25;
		coord1 *= sd;
		coord2 *= sd;


		if( sqrt(sqr(coord1) + sqr(coord2)) < 1 )
			g_clumpCoordinates[c++] = D3DXFROMWINEVECTOR2(coord1, coord2);	
	}

	
	initialData.pSysMem = g_clumpCoordinates;
	int hr;
	
	bufferDesc.ByteWidth = g_NumStrandVariables * sizeof(D3DXFROMWINEVECTOR2);
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	exBufferID  strandCircularCoordinates;
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &strandCircularCoordinates));
	
	desc.Format = FORMAT_R32G32_FLOAT;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumStrandVariables;
	//////////////////SAFE_RELEASE(g_StrandCircularCoordinatesRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(strandCircularCoordinates, &desc, &g_StrandCircularCoordinatesRV));
	g_Effect->GetVariableByName("g_StrandCircularCoordinates")->AsShaderResource()->SetResource(g_StrandCircularCoordinatesRV);
	//////////////////SAFE_RELEASE(strandCircularCoordinates);
	return 0;
}


int CreateStrandAttributes()
{
	
	initialData.pSysMem = g_MasterAttributes;
	int hr;
	
	bufferDesc.ByteWidth = g_NumMasterStrands * sizeof(Attributes);
	bufferDesc.Usage = USAGE_IMMUTABLE;
	bufferDesc.BindFlags = BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	//////////////////SAFE_RELEASE(g_MasterAttributesBuffer);
	(IRenderer::GetRendererInstance()->CreateBuffer(&bufferDesc, &initialData, &g_MasterAttributesBuffer));
	
	desc.Format = FORMAT_R32G32_FLOAT;
	desc.ViewDimension = SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = g_NumMasterStrands;
	//////////////////SAFE_RELEASE(g_StrandAttributesRV);
	(IRenderer::GetRendererInstance()->CreateShaderResourceView(g_MasterAttributesBuffer, &desc, &g_StrandAttributesRV));
	g_Effect->GetVariableByName("g_Attributes")->AsShaderResource()->SetResource(g_StrandAttributesRV);
	return 0;
}

//--------------------------------------------------------------------------------------
// Create input layouts 
//--------------------------------------------------------------------------------------
int CreateInputLayouts()
{
	int hr;
	(CreateMasterStrandIL());
	(CreateMasterStrandTesselatedInputIL());
	return 0;
}

int CreateMasterStrandIL()
{
	int hr;

	FormatDesc elements2[] =
	{
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
		1, TYPE_VERTEX, FORMAT_FLOAT, 4
	};
	g_MasterStrandILAddForces = IRenderer::GetRendererInstance()->addVertexFormat(elements2, elementsOf(elements2), -1);

	FormatDesc elements[] =
	{
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
	};
	g_MasterStrandIL = IRenderer::GetRendererInstance()->addVertexFormat(elements, elementsOf(elements), -1);

	FormatDesc elements3[] =
	{
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 1
	};
	g_MasterStrandSpringConstraintsIL = IRenderer::GetRendererInstance()->addVertexFormat(elements3, elementsOf(elements3), -1);

	FormatDesc elements4[] =
	{
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	g_MasterStrandAngularSpringConstraintsIL = IRenderer::GetRendererInstance()->addVertexFormat(elements4, elementsOf(elements4), -1);

	FormatDesc elements6[] =
	{
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 4
	};
	g_CoordinateFrameIL = IRenderer::GetRendererInstance()->addVertexFormat(elements6, elementsOf(elements6), -1);

	FormatDesc elements7[] =
	{
		0, TYPE_VERTEX, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3
	};
	g_screenQuadIL = IRenderer::GetRendererInstance()->addVertexFormat(elements7, elementsOf(elements7), -1);

	return 0;
}

int CreateMasterStrandTesselatedInputIL()
{
	int hr;

	FormatDesc elements[] =
	{
		0, TYPE_VERTEX, FORMAT_FLOAT, 1,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 1
	};
	g_MasterStrandTesselatedInputIL = IRenderer::GetRendererInstance()->addVertexFormat(elements, elementsOf(elements), -1);

	return 0;
}
//--------------------------------------------------------------------------------------
// Release resources 
//--------------------------------------------------------------------------------------
void ReleaseResources()
{
	if(g_TessellatedMasterStrandVertexOffsets)
	{
		delete[] g_TessellatedMasterStrandVertexOffsets;
		g_TessellatedMasterStrandVertexOffsets = 0;
	}
	if(g_MasterStrandControlVertices)
	{
		delete[] g_MasterStrandControlVertices;
		g_MasterStrandControlVertices = 0;
	}
	if(g_MasterStrandControlVertexOffsets)
	{
		delete[] g_MasterStrandControlVertexOffsets;
		g_MasterStrandControlVertexOffsets = 0;
	}
	if(g_MasterStrandControlVertices)
	{
		delete[] g_MasterStrandControlVertices;
		g_MasterStrandControlVertices = 0;
	}
	if(g_coordinateFrames)
	{
		delete[] g_coordinateFrames;
		g_coordinateFrames = 0;
	}
	if(g_masterCVIndices)
	{
    	delete[] g_masterCVIndices;
		g_masterCVIndices = 0;
	}
	if(g_clumpCoordinates)
	{
		delete[] g_clumpCoordinates;
		g_clumpCoordinates = 0;
	}
	if(g_masterStrandLengthToRoot)
	{
		delete [] g_masterStrandLengthToRoot;
		g_masterStrandLengthToRoot = 0;
	}
	if(g_masterStrandLengths)
	{
		delete [] g_masterStrandLengths;
		g_masterStrandLengths = 0;
	}
	if(g_Strand_Sizes)
	{
		delete [] g_Strand_Sizes;
		g_Strand_Sizes = 0;
	}
	if(g_tangentJitter)
	{
		delete [] g_tangentJitter;
		g_tangentJitter = 0; 
	}
	if(g_BarycentricCoordinates)
	{
		delete [] g_BarycentricCoordinates;
		g_BarycentricCoordinates = 0; 
	}
	delete [] g_lengths;
	delete [] g_MasterAttributes;

	//input layouts
	//////////////////SAFE_RELEASE(g_MasterStrandIL);
	//////////////////SAFE_RELEASE(g_CoordinateFrameIL);
	//////////////////SAFE_RELEASE(g_MasterStrandILAddForces);
	//////////////////SAFE_RELEASE(g_MasterStrandSpringConstraintsIL);
	//////////////////SAFE_RELEASE(g_MasterStrandAngularSpringConstraintsIL);
	//////////////////SAFE_RELEASE(g_MasterStrandTesselatedInputIL);
	//////////////////SAFE_RELEASE(g_screenQuadIL);

	//index buffers
	//////////////////SAFE_RELEASE(g_MasterStrandSimulation1IB);
	//////////////////SAFE_RELEASE(g_MasterStrandSimulation2IB);
	//////////////////SAFE_RELEASE(g_MasterStrandSimulationAngular1IB);
	//////////////////SAFE_RELEASE(g_MasterStrandSimulationAngular2IB);

	//vertex buffers
	//////////////////SAFE_RELEASE(g_MasterStrandVB[0]);
	//////////////////SAFE_RELEASE(g_MasterStrandVB[1]);
	//////////////////SAFE_RELEASE(g_ForceVB[0]);
	//////////////////SAFE_RELEASE(g_ForceVB[1]);
	//////////////////SAFE_RELEASE(g_TessellatedMasterStrandVB);
	//////////////////SAFE_RELEASE(g_TessellatedMasterStrandVBInitial);
	//////////////////SAFE_RELEASE(g_MasterStrandLinearSpringLengthsVB);
	//////////////////SAFE_RELEASE(g_MasterStrandAngularSpringConstraintsVB);
	//////////////////SAFE_RELEASE(g_TessellatedWispVB);
	//////////////////SAFE_RELEASE(g_MasterStrandVBOldTemp);
	//////////////////SAFE_RELEASE(g_MasterStrandVBOld);

    //compute shader variables
	//////////////////SAFE_RELEASE(g_MasterStrandUAB);
	//////////////////SAFE_RELEASE(g_MasterStrandPrevUAB);
    //////////////////SAFE_RELEASE(g_MasterStrandUAB_SRV);
    //////////////////SAFE_RELEASE(g_MasterStrandUAB_UAV);
    //////////////////SAFE_RELEASE(g_MasterStrandPrevUAB_UAV);
	//////////////////SAFE_RELEASE(g_pCSSimulateParticles);
	//////////////////SAFE_RELEASE(g_MasterStrandLinearSpringLengthsSRV);
    //////////////////SAFE_RELEASE(g_MasterStrandSimulationIndicesBuffer);
    //////////////////SAFE_RELEASE(g_MasterStrandSimulationIndicesSRV);
	//////////////////SAFE_RELEASE(g_pcbCSPerFrame);
	//////////////////SAFE_RELEASE(g_pcbCSTransforms);
    //////////////////SAFE_RELEASE(g_pcbCSRootXform);
    //////////////////SAFE_RELEASE(g_CoordinateFrameUAB_UAV);
    //////////////////SAFE_RELEASE(g_CoordinateFrameUAB_SRV);
    //////////////////SAFE_RELEASE(g_CoordinateFrameUAB);
    //////////////////SAFE_RELEASE(g_AngularStiffnessUAB_SRV);
	//////////////////SAFE_RELEASE(g_MasterStrandAngularSpringConstraintsSRV);

	//////////////////SAFE_RELEASE(g_StrandSizes);
	//////////////////SAFE_RELEASE(g_StrandCoordinates);
	//////////////////SAFE_RELEASE(g_MasterAttributesBuffer);
	//////////////////SAFE_RELEASE(g_masterLengths);
	//////////////////SAFE_RELEASE(g_masterLengthsUntessellated);
	//////////////////SAFE_RELEASE(g_rootsIndices);
	//////////////////SAFE_RELEASE(g_rootsIndicesUntessellated);
	//////////////////SAFE_RELEASE(g_rootsIndicesMasterStrand);
	//////////////////SAFE_RELEASE(g_UnTessellatedWispVB);
	//////////////////SAFE_RELEASE(g_CoordinateFrameVB[0]);
	//////////////////SAFE_RELEASE(g_CoordinateFrameVB[1]);
	//////////////////SAFE_RELEASE(g_TessellatedCoordinateFrameVB);
	//////////////////SAFE_RELEASE(g_TessellatedMasterStrandLengthToRootVB);
	//////////////////SAFE_RELEASE(g_TessellatedTangentsVB);
	//////////////////SAFE_RELEASE(g_collisionScreenQuadBuffer);
	//////////////////SAFE_RELEASE(g_ScreenQuadBuffer);
	//////////////////SAFE_RELEASE(g_SuperSampleScreenQuadBuffer);
	//////////////////SAFE_RELEASE(g_BarycentricInterpolatedPositionsWidthsVB);
	//////////////////SAFE_RELEASE(g_BarycentricInterpolatedIdAlphaTexVB);
	//////////////////SAFE_RELEASE(g_BarycentricInterpolatedTangentVB);
	//////////////////SAFE_RELEASE(g_ClumpInterpolatedPositionsWidthsVB);
	//////////////////SAFE_RELEASE(g_ClumpInterpolatedIdAlphaTexVB);
	//////////////////SAFE_RELEASE(g_ClumpInterpolatedTangentVB);

	//shaders resource views
	//////////////////SAFE_RELEASE(g_TessellatedMasterStrandRV);
	//////////////////SAFE_RELEASE(g_MasterStrandRV[0]);
	//////////////////SAFE_RELEASE(g_MasterStrandRV[1]);
	//////////////////SAFE_RELEASE(g_ForceRV[0]);
	//////////////////SAFE_RELEASE(g_ForceRV[1]);
	//////////////////SAFE_RELEASE(g_CoordinateFrameRV[0]);
	//////////////////SAFE_RELEASE(g_CoordinateFrameRV[1]);
	//////////////////SAFE_RELEASE(g_StrandSizesRV);
	//////////////////SAFE_RELEASE(g_StrandCoordinatesRV);
	//////////////////SAFE_RELEASE(g_StrandCircularCoordinatesRV);
	//////////////////SAFE_RELEASE(g_StrandAttributesRV);
	//////////////////SAFE_RELEASE(g_RootsIndicesRV);
	//////////////////SAFE_RELEASE(g_RootsIndicesRVUntessellated);
	//////////////////SAFE_RELEASE(g_MasterLengthsRV);
	//////////////////SAFE_RELEASE(g_rootsIndicesMasterStrandRV);
	//////////////////SAFE_RELEASE(g_MasterLengthsRVUntessellated);
	//////////////////SAFE_RELEASE(g_TessellatedMasterStrandRootIndexRV);
	//////////////////SAFE_RELEASE(g_TessellatedCoordinateFrameRV);
	//////////////////SAFE_RELEASE(g_TessellatedMasterStrandLengthToRootRV);
	//////////////////SAFE_RELEASE(g_StrandLengthsRV);
	//////////////////SAFE_RELEASE(g_TessellatedTangentsRV);
	//////////////////SAFE_RELEASE(g_StrandColorsRV);
	//////////////////SAFE_RELEASE(g_StrandDeviationsRV);
	//////////////////SAFE_RELEASE(g_CurlDeviationsRV);
	//////////////////SAFE_RELEASE(g_OriginalMasterStrandRV);
	//////////////////SAFE_RELEASE(g_CollisionsRV );
	//////////////////SAFE_RELEASE(g_originalVectorsRV);
	//////////////////SAFE_RELEASE(g_MasterStrandLocalVertexNumberRV);
	//////////////////SAFE_RELEASE(hairTextureRV);
	//////////////////SAFE_RELEASE(g_TangentJitterRV);
	//////////////////SAFE_RELEASE(g_MasterStrandLengthsRV);
	//////////////////SAFE_RELEASE(g_MasterStrandLengthToRootRV);
	//////////////////SAFE_RELEASE(g_pSceneColorBufferShaderResourceView);
	//////////////////SAFE_RELEASE(g_BarycentricInterpolatedPositionsWidthsRV);
	//////////////////SAFE_RELEASE(g_BarycentricInterpolatedIdAlphaTexRV);
	//////////////////SAFE_RELEASE(g_BarycentricInterpolatedTangentRV);
	//////////////////SAFE_RELEASE(g_ClumpInterpolatedPositionsWidthsRV);
	//////////////////SAFE_RELEASE(g_ClumpInterpolatedIdAlphaTexRV);
	//////////////////SAFE_RELEASE(g_ClumpInterpolatedTangentRV);
	//////////////////SAFE_RELEASE(g_MstrandsIndexRV);
	//////////////////SAFE_RELEASE(g_SstrandsIndexRV);
	//////////////////SAFE_RELEASE(g_SStrandsPerMasterStrandCumulative);
	//////////////////SAFE_RELEASE(g_MStrandsPerWispCumulative);
	//////////////////SAFE_RELEASE(g_HairBaseRV[0]);
	//////////////////SAFE_RELEASE(g_HairBaseRV[1]);
	//////////////////SAFE_RELEASE(g_HairBaseRV[2]);

	//render target views
	//////////////////SAFE_RELEASE(g_CollisionsRTV );
	//////////////////SAFE_RELEASE(g_pSceneRTV2X);
	//////////////////SAFE_RELEASE(g_pSceneDSV2X);

	//textures
	//////////////////SAFE_RELEASE(g_pSceneDepthBuffer2X);
	//////////////////SAFE_RELEASE(g_pSceneColorBuffer2X);
	//////////////////SAFE_RELEASE(g_pSceneColorBuffer2XMS);

}



//--------------------------------------------------------------------------------------
// Helper functions
//--------------------------------------------------------------------------------------

//load a single texture and associated view
int loadTextureFromFile(char file,char shaderTextureName)
{

	int hr;

	D3DXFROMWINE11_IMAGE_LOAD_INFO texLoadInfo;
	texLoadInfo.MipLevels = 8;
	texLoadInfo.MipFilter = D3DXFROMWINE10_FILTER_TRIANGLE;
	texLoadInfo.Filter = D3DXFROMWINE10_FILTER_TRIANGLE;
	stx_Resource* pRes = file;

	//////////////////SAFE_RELEASE( pRes );

	return 0;
}

//load a single texture and associated view
int loadTextureFromFile(char file,TextureID   textureRview)
{

	int hr;

	D3DXFROMWINE11_IMAGE_LOAD_INFO texLoadInfo;
	texLoadInfo.MipLevels = 8;
	texLoadInfo.MipFilter = D3DXFROMWINE10_FILTER_TRIANGLE;
	texLoadInfo.Filter = D3DXFROMWINE10_FILTER_TRIANGLE;
	stx_Resource* pRes = file;

	return 0;
}


float vecLength(HairVertex v1,HairVertex v2)
{
	return sqrt( sqr(v1.Position.x-v2.Position.x) + sqr(v1.Position.y-v2.Position.y) + sqr(v1.Position.z-v2.Position.z) );
}


void vectorMatrixMultiply(D3DXFROMWINEVECTOR3* vecOut,const D3DXFROMWINEMATRIX matrix,const D3DXFROMWINEVECTOR3 vecIn)
{
	vecOut->x = vecIn.x*matrix(0,0) + vecIn.y*matrix(1,0) + vecIn.z*matrix(2,0) + matrix(3,0);
	vecOut->y  = vecIn.x*matrix(0,1)+ vecIn.y*matrix(1,1) + vecIn.z*matrix(2,1) + matrix(3,1);
	vecOut->z = vecIn.x*matrix(0,2) + vecIn.y*matrix(1,2) + vecIn.z*matrix(2,2) + matrix(3,2);
}
void vectorMatrixMultiply(D3DXFROMWINEVECTOR4* vecOut,const D3DXFROMWINEMATRIX matrix,const D3DXFROMWINEVECTOR4 vecIn)
{
	vecOut->x = vecIn.x*matrix(0,0) + vecIn.y*matrix(0,1) + vecIn.z*matrix(0,2) + vecIn.w*matrix(0,3);
	vecOut->y = vecIn.x*matrix(1,0) + vecIn.y*matrix(1,1) + vecIn.z*matrix(1,2) + vecIn.w*matrix(1,3);
	vecOut->z = vecIn.x*matrix(2,0) + vecIn.y*matrix(2,1) + vecIn.z*matrix(2,2) + vecIn.w*matrix(2,3);
	vecOut->w = vecIn.x*matrix(3,0) + vecIn.y*matrix(3,1) + vecIn.z*matrix(3,2) + vecIn.w*matrix(3,3);

}


//note: this method is only valid for when the axis is perpendicular to the prevVec
void rotateVector(const float3& rotationAxis,float theta,const float3& prevVec, float3& newVec)
{
	float3 axisDifference;
	D3DXFROMWINEVec3Subtract(&axisDifference,&rotationAxis,&prevVec);
	if( D3DXFROMWINEVec3Length(&axisDifference)<SMALL_NUMBER || theta < SMALL_NUMBER )
	{
		newVec = prevVec;
		return;
	}

	float c = cos(theta);
	float s = sin(theta);
	float t = 1 - c;
	float x = rotationAxis.x;
	float y = rotationAxis.y;
	float z = rotationAxis.z;

	D3DXFROMWINEMATRIX rotationMatrix( t*x*x + c,   t*x*y - s*z, t*x*z + s*y, 0,
		t*x*y + s*z, t*y*y + c,   t*y*z - s*x, 0,
		t*x*z - s*y, t*y*z + s*x, t*z*z + c  , 0,
		0          , 0          , 0          , 1);

	vectorMatrixMultiply(&newVec,rotationMatrix,prevVec);

}


int CreateCollisionScreenQuadVB()
{	
	int hr;

	GRID_TEXTURE_DISPLAY_STRUCT *renderQuad(0);
	renderQuad = new GRID_TEXTURE_DISPLAY_STRUCT[ 6 ];

	//fill render quad
	GRID_TEXTURE_DISPLAY_STRUCT tempVertex1;
	GRID_TEXTURE_DISPLAY_STRUCT tempVertex2;
	GRID_TEXTURE_DISPLAY_STRUCT tempVertex3;
	GRID_TEXTURE_DISPLAY_STRUCT tempVertex4;

	float w = float(MAX_INTERPOLATED_HAIR_PER_WISP);
	float h = float(g_NumWisps);

	float left   = -1.0f + 2.0f/w;
	float right  =  1.0f - 2.0f/w;
	float top    =  1.0f - 2.0f/h;
	float bottom = -1.0f + 2.0f/h;

	tempVertex1.Pos = D3DXFROMWINEVECTOR3( left     , top       , 0.0f      );
	tempVertex1.Tex = D3DXFROMWINEVECTOR3( 0.0f     , 0.0f      , 0.0f      );

	tempVertex2.Pos = D3DXFROMWINEVECTOR3( right    , top       , 0.0f      );
	tempVertex2.Tex = D3DXFROMWINEVECTOR3( 1.0f     , 0.0f      , 0.0f      );

	tempVertex3.Pos = D3DXFROMWINEVECTOR3( right    , bottom    , 0.0f      );
	tempVertex3.Tex = D3DXFROMWINEVECTOR3( 1.0f     , 1.0f      , 0.0f      );

	tempVertex4.Pos = D3DXFROMWINEVECTOR3( left     , bottom    , 0.0f      );
	tempVertex4.Tex = D3DXFROMWINEVECTOR3( 0.0f     , 1.0       , 0.0f      );

	int index = 0;
	renderQuad[index++] = tempVertex1;
	renderQuad[index++] = tempVertex2;
	renderQuad[index++] = tempVertex3;
	renderQuad[index++] = tempVertex1;
	renderQuad[index++] = tempVertex3;
	renderQuad[index++] = tempVertex4;

	
	bd.ByteWidth = sizeof(GRID_TEXTURE_DISPLAY_STRUCT)*6;
	bd.Usage = USAGE_DEFAULT;
	bd.BindFlags = BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	
	ZeroMemory( &InitData, sizeof(
	InitData.pSysMem = renderQuad;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	//////////////////SAFE_RELEASE(g_collisionScreenQuadBuffer);
	( IRenderer::GetRendererInstance()->CreateBuffer( &bd, &InitData,&g_collisionScreenQuadBuffer  ) ); 	

	delete [] renderQuad;
	renderQuad = 0;
	return 0;
}

//this is the size of the back buffer
int CreateScreenQuadVB()
{	
	int hr;

	//////////////////SAFE_RELEASE(g_ScreenQuadBuffer);

	GRID_TEXTURE_DISPLAY_STRUCT *renderQuad(0);
	renderQuad = new GRID_TEXTURE_DISPLAY_STRUCT[ 6 ];

	//fill render quad
	GRID_TEXTURE_DISPLAY_STRUCT tempVertex1;
	GRID_TEXTURE_DISPLAY_STRUCT tempVertex2;
	GRID_TEXTURE_DISPLAY_STRUCT tempVertex3;
	GRID_TEXTURE_DISPLAY_STRUCT tempVertex4;

	float left   = -1.0f;
	float right  =  1.0f;
	float top    =  1.0f;
	float bottom = -1.0f;

	tempVertex1.Pos = D3DXFROMWINEVECTOR3( left     , top       , 0.0f      );
	tempVertex1.Tex = D3DXFROMWINEVECTOR3( 0.0f     , 0.0f      , 0.0f      );

	tempVertex2.Pos = D3DXFROMWINEVECTOR3( right    , top       , 0.0f      );
	tempVertex2.Tex = D3DXFROMWINEVECTOR3( 1.0f     , 0.0f      , 0.0f      );

	tempVertex3.Pos = D3DXFROMWINEVECTOR3( right    , bottom    , 0.0f      );
	tempVertex3.Tex = D3DXFROMWINEVECTOR3( 1.0f     , 1.0f      , 0.0f      );

	tempVertex4.Pos = D3DXFROMWINEVECTOR3( left     , bottom    , 0.0f      );
	tempVertex4.Tex = D3DXFROMWINEVECTOR3( 0.0f     , 1.0       , 0.0f      );

	int index = 0;
	renderQuad[index++] = tempVertex1;
	renderQuad[index++] = tempVertex2;
	renderQuad[index++] = tempVertex3;
	renderQuad[index++] = tempVertex1;
	renderQuad[index++] = tempVertex3;
	renderQuad[index++] = tempVertex4;

	
	bd.ByteWidth = sizeof(GRID_TEXTURE_DISPLAY_STRUCT)*6;
	bd.Usage = USAGE_DEFAULT;
	bd.BindFlags = BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	
	ZeroMemory( &InitData, sizeof(
	InitData.pSysMem = renderQuad;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	( IRenderer::GetRendererInstance()->CreateBuffer( &bd, &InitData,&g_ScreenQuadBuffer  ) ); 	

	delete [] renderQuad;
	renderQuad = 0;
	return 0;
}


//this is the size of the super sample buffer
int CreateSuperSampleScreenQuadVB()
{	
	int hr;

	//////////////////SAFE_RELEASE(g_SuperSampleScreenQuadBuffer);

	GRID_TEXTURE_DISPLAY_STRUCT *renderQuad(0);
	renderQuad = new GRID_TEXTURE_DISPLAY_STRUCT[ 6 ];

	//fill render quad
	GRID_TEXTURE_DISPLAY_STRUCT tempVertex1;
	GRID_TEXTURE_DISPLAY_STRUCT tempVertex2;
	GRID_TEXTURE_DISPLAY_STRUCT tempVertex3;
	GRID_TEXTURE_DISPLAY_STRUCT tempVertex4;

	float left   = -1.0f;
	float right  =  1.0f;
	float top    =  1.0f;
	float bottom = -1.0f;

	tempVertex1.Pos = D3DXFROMWINEVECTOR3( left     , top       , 0.0f      );
	tempVertex1.Tex = D3DXFROMWINEVECTOR3( 0.0f     , 0.0f      , 0.0f      );

	tempVertex2.Pos = D3DXFROMWINEVECTOR3( right    , top       , 0.0f      );
	tempVertex2.Tex = D3DXFROMWINEVECTOR3( 1.0f     , 0.0f      , 0.0f      );

	tempVertex3.Pos = D3DXFROMWINEVECTOR3( right    , bottom    , 0.0f      );
	tempVertex3.Tex = D3DXFROMWINEVECTOR3( 1.0f     , 1.0f      , 0.0f      );

	tempVertex4.Pos = D3DXFROMWINEVECTOR3( left     , bottom    , 0.0f      );
	tempVertex4.Tex = D3DXFROMWINEVECTOR3( 0.0f     , 1.0       , 0.0f      );

	int index = 0;
	renderQuad[index++] = tempVertex1;
	renderQuad[index++] = tempVertex2;
	renderQuad[index++] = tempVertex3;
	renderQuad[index++] = tempVertex1;
	renderQuad[index++] = tempVertex3;
	renderQuad[index++] = tempVertex4;

	
	bd.ByteWidth = sizeof(GRID_TEXTURE_DISPLAY_STRUCT)*6;
	bd.Usage = USAGE_DEFAULT;
	bd.BindFlags = BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	
	ZeroMemory( &InitData, sizeof(
	InitData.pSysMem = renderQuad;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	( IRenderer::GetRendererInstance()->CreateBuffer( &bd, &InitData,&g_SuperSampleScreenQuadBuffer  ) ); 	

	delete [] renderQuad;
	renderQuad = 0;
	return 0;
}



void RenderScreenQuad(exBufferID  buffer)
{
	unsigned int stride[1] = { sizeof(GRID_TEXTURE_DISPLAY_STRUCT) };
	unsigned int offset[1] = { 0 };

	IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_TRIANGLES );
	IRenderer::GetRendererInstance()->setVertexFormat(0,  g_screenQuadIL );
	IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1,  &buffer, stride, offset );
	IRenderer::GetRendererInstance()->Draw( 6, 0 ); 
}



float3 TrasformFromWorldToLocalCF(int index,float3 oldVector)
{
	float3 vX = float3( g_coordinateFrames[ index*12 + 0] , g_coordinateFrames[ index*12 + 1] , g_coordinateFrames[ index*12 + 2] );
	float3 vY = float3( g_coordinateFrames[ index*12 + 4] , g_coordinateFrames[ index*12 + 5] , g_coordinateFrames[ index*12 + 6] );
	float3 vZ = float3( g_coordinateFrames[ index*12 + 8] , g_coordinateFrames[ index*12 + 9] , g_coordinateFrames[ index*12 + 10] );

	float3 worldX = float3(1,0,0);
	float3 worldY = float3(0,1,0);
	float3 worldZ = float3(0,0,1);


	D3DXFROMWINEMATRIX WorldToV = D3DXFROMWINEMATRIX
		(
		D3DXFROMWINEVec3Dot(&vX,&worldX), D3DXFROMWINEVec3Dot(&vY,&worldX), D3DXFROMWINEVec3Dot(&vZ,&worldX), 0,
		D3DXFROMWINEVec3Dot(&vX,&worldY), D3DXFROMWINEVec3Dot(&vY,&worldY), D3DXFROMWINEVec3Dot(&vZ,&worldY), 0, 
		D3DXFROMWINEVec3Dot(&vX,&worldZ), D3DXFROMWINEVec3Dot(&vY,&worldZ), D3DXFROMWINEVec3Dot(&vZ,&worldZ), 0,
		0,                        0,                        0,                        1
		);

	//transform from old to new coordinate frame
	float3 retVector;
	D3DXFROMWINEVec3TransformCoord(&retVector,&oldVector,&WorldToV);
	return retVector;

}


//--------------------------------------------------------------------------------------
// LoadTextureArray loads a texture array and associated view from a series
// of textures on disk.
//--------------------------------------------------------------------------------------
int LoadTextureArray( char* sTexturePrefix, int iNumTextures, TextureID   ppTex2D, TextureID   ppSRV,int offset)
{
	int hr = 0;
	SURFACE_DESC desc;
	ZeroMemory( &desc, sizeof(SURFACE_DESC) );

	char szTextureName[MAX_PATH];
	for(int i=0; i<iNumTextures; i++)
	{
		stx_snprintf(szTextureName, "%S%.2d.dds", sTexturePrefix, i+offset); 
		stx_Resource* pRes = szTextureName;
	}

	return hr;
}

//--------------------------------------------------------------------------------------

//functions for creating and rendering the plane

struct PlaneVertex
{
    D3DXFROMWINEVECTOR3 Pos;
    D3DXFROMWINEVECTOR3 Normal; 
};


int CreatePlane( float radius, float height )
{
    int hr;
    PlaneVertex vertices[] =
    {
        { D3DXFROMWINEVECTOR3( -radius, height,  radius ), D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3(  radius, height,  radius ), D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3(  radius, height, -radius ), D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3( -radius, height, -radius ), D3DXFROMWINEVECTOR3( 0.0f, 1.0f, 0.0f ) },
    };
    
    bd.Usage = USAGE_DEFAULT;
    bd.ByteWidth = sizeof( PlaneVertex ) * 4;
    bd.BindFlags = BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    
    InitData.pSysMem = vertices;
    hr = IRenderer::GetRendererInstance()->CreateBuffer( &bd, &InitData, &g_pPlaneVertexBuffer );
    if( FAILED(hr) )
        return hr;

    DWORD indices[] =
    {
        0,1,2,
        0,2,3,
    };

    bd.Usage = USAGE_DEFAULT;
    bd.ByteWidth = sizeof( DWORD ) * 6;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    InitData.pSysMem = indices;
    hr = IRenderer::GetRendererInstance()->CreateBuffer( &bd, &InitData, &g_pPlaneIndexBuffer );
    if( FAILED(hr) )
        return hr;

    g_pTechniqueRenderPlane = g_Effect->GetTechniqueByName("RenderPlane");

    // Create the input layout
	FormatDesc layoutPlane[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL, FORMAT_FLOAT, 3
	};
	g_pPlaneVertexLayout = IRenderer::GetRendererInstance()->addVertexFormat(layoutPlane, elementsOf(layoutPlane), -1);

    return 0;
}

void DrawPlane()
{
	g_HairShadows.SetHairShadowTexture();

    if(g_MeshWithShadows && g_renderShadows)
        g_pTechniqueRenderPlane->GetPassByName("RenderPlaneWithShadows")->Apply(0);
    else
        g_pTechniqueRenderPlane->GetPassByName("RenderPlaneWithoutShadows")->Apply(0);

    unsigned int stride = sizeof( PlaneVertex );
    unsigned int offset = 0;
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1, &g_pPlaneVertexBuffer, &stride, &offset );
    IRenderer::GetRendererInstance()->setIndexBuffer( g_pPlaneIndexBuffer, FORMAT_R32_UINT, 0 );
    IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_TRIANGLES );
    IRenderer::GetRendererInstance()->setVertexFormat(0,  g_pPlaneVertexLayout );
    IRenderer::GetRendererInstance()->DrawIndexed( 6, 0, 0 );

	g_HairShadows.UnSetHairShadowTexture();

    if(g_MeshWithShadows && g_renderShadows)
        g_pTechniqueRenderPlane->GetPassByName("RenderPlaneWithShadows")->Apply(0);
    else
        g_pTechniqueRenderPlane->GetPassByName("RenderPlaneWithoutShadows")->Apply(0);
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
	IRenderer* r=IRenderer::GetRendererInstance("Hair");
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
    
