/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//--------------------------------------------------------------------------------------
// File: HDRCubeMap.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
//include "resource.h"

//--------------------------------------------------------------------------------------
// Defines, and constants
//--------------------------------------------------------------------------------------
#define ENVMAPSIZE 256
#define NUM_LIGHTS 4  // Currently, 4 is the only number of lights supported.
#define LIGHTMESH_RADIUS 0.2f
#define HELPTEXTCOLOR D3DXFROMWINEVECTOR4( 0.0f, 1.0f, 0.3f, 1.0f )


//--------------------------------------------------------------------------------------
FormatDesc g_aVertDecl[] =
{
    0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
    0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
    0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
};

//--------------------------------------------------------------------------------------

const char* g_atszMeshFileName[] =
{
    "/dx9sdk/teapot.x",
    //"misc/skullocc.x",
    //"misc/car.x"
};

#define NUM_MESHES ( sizeof(g_atszMeshFileName) / sizeof(g_atszMeshFileName[0]) )
//--------------------------------------------------------------------------------------
struct ORBITDATA
{
    std::string tszXFile;  // X file
    D3DXFROMWINEVECTOR3 vAxis;     // Axis of rotation
    float fRadius;   // Orbit radius
    float fSpeed;    // Orbit speed in radians per second
};


//--------------------------------------------------------------------------------------
// Mesh file to use for orbiter objects
// These don't have to use the same mesh file.
//--------------------------------------------------------------------------------------
ORBITDATA g_OrbitData[] =
{
    {"/dx9sdk/teapot.x", { -1.0f, 0.0f, 0.0f }, 2.0f, D3DXFROMWINE_PI * 1.0f },
    {"/dx9sdk/teapot.x", { 0.3f,  1.0f, 0.3f }, 2.5f, D3DXFROMWINE_PI / 2.0f }
};

//--------------------------------------------------------------------------------------
// Encapsulate an object in the scene with its world transformation
// matrix.
//--------------------------------------------------------------------------------------
struct CObj
{
    D3DXFROMWINEMATRIX m_mWorld;  // World transformation
    MeshRenderer2 m_Mesh;    // Mesh object

public:
            CObj()
            {
                D3DXFROMWINEMatrixIdentity( &m_mWorld );
            }
            ~CObj()
            {
            }
    int WorldCenterAndScaleToRadius( float fRadius )
    {
        //
        // Compute the world transformation matrix
        // to center the mesh at origin in world space
        // and scale its size to the specified radius.
        //
        int hr;

        float fRadiusBound;
        m_Mesh.GetBoundingSphereRadius(fRadiusBound);
        D3DXFROMWINEVECTOR3 vCtr=m_Mesh.GetMeshBBoxCenter( );

        D3DXFROMWINEMatrixTranslation( &m_mWorld, -vCtr.x, -vCtr.y, -vCtr.z );
        D3DXFROMWINEMATRIX mScale;
        D3DXFROMWINEMatrixScaling( &mScale, fRadius / fRadiusBound,
                           fRadius / fRadiusBound,
                           fRadius / fRadiusBound );
        D3DXFROMWINEMatrixMultiply( &m_mWorld, &m_mWorld, &mScale );

        return hr;
    }  // WorldCenterAndScaleToRadius
};


//--------------------------------------------------------------------------------------
// Encapsulate an orbiter object in the scene with related data
//--------------------------------------------------------------------------------------
class COrbiter : public CObj
{
public:
    D3DXFROMWINEVECTOR3 m_vAxis;       // orbit axis
    float m_fRadius;     // orbit radius
    float m_fSpeed;      // Speed, angle in radian per second

public:
            COrbiter() : m_vAxis( 0.0f, 1.0f, 0.0f ),
                         m_fRadius( 1.0f ),
                         m_fSpeed( D3DXFROMWINE_PI )
            {
            }
    void    SetOrbit( D3DXFROMWINEVECTOR3& vAxis, float fRadius, float fSpeed )
    {
        // Call this after m_mWorld is initialized
        m_vAxis = vAxis; m_fRadius = fRadius; m_fSpeed = fSpeed;
        D3DXFROMWINEVec3Normalize( &m_vAxis, &m_vAxis );

        // Translate by m_fRadius in -Y direction
        D3DXFROMWINEMATRIX m;
        D3DXFROMWINEMatrixTranslation( &m, 0.0f, -m_fRadius, 0.0f );
        D3DXFROMWINEMatrixMultiply( &m_mWorld, &m_mWorld, &m );

        // Apply rotation from X axis to the orbit axis
        D3DXFROMWINEVECTOR3 vX( 1.0f, 0.0f, 0.0f );
        D3DXFROMWINEVECTOR3 vRot;
        D3DXFROMWINEVec3Cross( &vRot, &m_vAxis, &vX );  // Axis of rotation
        // If the cross product is 0, m_vAxis is on the X axis
        // So we either rotate 0 or D3DXFROMWINE_PI.
        if( D3DXFROMWINEVec3LengthSq( &vRot ) == 0 )
        {
            if( m_vAxis.x < 0.0f )
                D3DXFROMWINEMatrixRotationY( &m, D3DXFROMWINE_PI ); // D3DXFROMWINE_PI
            else
                D3DXFROMWINEMatrixIdentity( &m );           // 0
        }
        else
        {
            float fAng = ( float )acos( D3DXFROMWINEVec3Dot( &m_vAxis, &vX ) );  // Angle to rotate
            // Find out direction to rotate
            D3DXFROMWINEVECTOR3 vXxRot;  // X cross vRot
            D3DXFROMWINEVec3Cross( &vXxRot, &vX, &vRot );
            if( D3DXFROMWINEVec3Dot( &vXxRot, &m_vAxis ) >= 0 )
                fAng = -fAng;
            D3DXFROMWINEMatrixRotationAxis( &m, &vRot, fAng );
        }
        D3DXFROMWINEMatrixMultiply( &m_mWorld, &m_mWorld, &m );
    }

    void    Orbit( float fElapsedTime )
    {
        // Compute the orbit transformation and apply to m_mWorld
        D3DXFROMWINEMATRIX m;

        D3DXFROMWINEMatrixRotationAxis( &m, &m_vAxis, m_fSpeed * fElapsedTime );
        D3DXFROMWINEMatrixMultiply( &m_mWorld, &m_mWorld, &m );
    }
};


//--------------------------------------------------------------------------------------
struct CLight
{
    D3DXFROMWINEVECTOR4 vPos;      // Position in world space
    D3DXFROMWINEVECTOR4 vMoveDir;  // Direction in which it moves
    float fMoveDist; // Maximum distance it can move
    D3DXFROMWINEMATRIX mWorld;    // World transform matrix for the light before animation
    D3DXFROMWINEMATRIX mWorking;  // Working matrix (world transform after animation)
};


//--------------------------------------------------------------------------------------
struct CTechniqueGroup
{
    ShaderID RenderSceneSHD;
    ShaderID RenderLightSHD;
    ShaderID RenderEnvMapSHD;
    VertexFormatID RenderSceneVF;
    VertexFormatID RenderLightVF;
    VertexFormatID RenderEnvMapVF;
};


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
#if 1 // ???
    ShaderID RenderSceneSHD;
    ShaderID RenderLightSHD;
    ShaderID RenderEnvMapSHD;
    VertexFormatID RenderSceneVF;
    VertexFormatID RenderLightVF;
    VertexFormatID RenderEnvMapVF;
    ShaderID RenderHDREnvMap2TexSHD;
    VertexFormatID RenderHDREnvMap2TexVF;
    ShaderID RenderHDREnvMapSHD;
    VertexFormatID RenderHDREnvMapVF;
#endif
stx_Effect*                    g_pEffect = 0;          // D3DXFROMWINE effect interface
//stx_CModelViewerCamera              g_Camera;                  // A model viewing camera
D3DXFROMWINEMATRIX g_CameraViewMatrix;
D3DXFROMWINEMATRIX g_CameraProjMatrix;
D3DXFROMWINEMATRIX g_CameraWorldMatrix;
bool                            g_bShowHelp = true;        // If true, it renders the UI control text
CLight g_aLights[NUM_LIGHTS];     // Parameters of light objects
D3DXFROMWINEVECTOR4                     g_vLightIntensity;         // Light intensity
CObj g_EnvMesh[NUM_MESHES];     // Mesh to receive environment mapping
int                             g_nCurrMesh = 0;           // Index of the element in m_EnvMesh we are currently displaying
MeshRenderer2                  g_Room;                    // Mesh representing room (wall, floor, ceiling)
COrbiter g_Orbiter[sizeof( g_OrbitData ) / sizeof( g_OrbitData[0] )]; // Orbiter meshes
MeshRenderer2                  g_LightMesh;               // Mesh for the light object
VertexFormatID    g_pVertDecl = -1;        // Vertex_ decl for the sample
TextureID          g_apCubeMapFp[2];          // Floating point format cube map
TextureID          g_pCubeMap32 = -1;       // 32-bit cube map (for fallback)
int                             g_nNumFpCubeMap = 0;       // Number of cube maps required for using floating point
#if 1
//SurfaceID               g_pDepthCube = -1;       // Depth-stencil buffer for rendering to cube texture
CTechniqueGroup g_aTechGroupFp[2];         // Group of techniques to use with floating pointcubemaps (2 cubes max)
CTechniqueGroup                 g_TechGroup32;             // Group of techniques to use with integer cubemaps
stx_Effect::stx_Variable* g_hWorldView = 0;       // Handle for world+view matrix in effect
stx_Effect::stx_Variable* g_hProj = 0;            // Handle for projection matrix in effect
std::string g_ahtxCubeMap[2];          // Handle for the cube texture in effect
stx_Effect::stx_Variable* g_htxScene = 0;         // Handle for the scene texture in effect
stx_Effect::stx_Variable* g_hLightIntensity = 0;  // Handle for the light intensity in effect
stx_Effect::stx_Variable* g_hLightPosView = 0;    // Handle for view space light positions in effect
stx_Effect::stx_Variable* g_hReflectivity = 0;    // Handle for reflectivity in effect
#endif

int                             g_nNumCubes;               // Number of cube maps used based on current cubemap format
TextureID         g_apCubeMap = g_apCubeMapFp;// Cube map(s) to use based on current cubemap format
CTechniqueGroup*                g_pTech = g_aTechGroupFp; // Techniques to be used based on cubemap format

float                           g_fWorldRotX = 0.0f;       // World rotation state X-axis
float                           g_fWorldRotY = 0.0f;       // World rotation state Y-axis
bool                            g_bUseFloatCubeMap;        // Whether we use floating point format cube map
float                           g_fReflectivity;           // Reflectivity value. Ranges from 0 to 1.

struct App
{
	App(){}
	virtual ~App(){}
//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    ResetParameters();

    // Set the light positions
    g_aLights[0].vPos = D3DXFROMWINEVECTOR4( -3.5f, 2.3f, -4.0f, 1.0f );
    g_aLights[0].vMoveDir = D3DXFROMWINEVECTOR4( 0.0f, 0.0f, 1.0f, 0.0f );
    g_aLights[0].fMoveDist = 8.0f;
#if NUM_LIGHTS > 1
    g_aLights[1].vPos = D3DXFROMWINEVECTOR4( 3.5f, 2.3f, 4.0f, 1.0f );
    g_aLights[1].vMoveDir = D3DXFROMWINEVECTOR4( 0.0f, 0.0f, -1.0f, 0.0f );
    g_aLights[1].fMoveDist = 8.0f;
#endif
#if NUM_LIGHTS > 2
    g_aLights[2].vPos = D3DXFROMWINEVECTOR4( -3.5f, 2.3f, 4.0f, 1.0f );
    g_aLights[2].vMoveDir = D3DXFROMWINEVECTOR4( 1.0f, 0.0f, 0.0f, 0.0f );
    g_aLights[2].fMoveDist = 7.0f;
#endif
#if NUM_LIGHTS > 3
    g_aLights[3].vPos = D3DXFROMWINEVECTOR4( 3.5f, 2.3f, -4.0f, 1.0f );
    g_aLights[3].vMoveDir = D3DXFROMWINEVECTOR4( -1.0f, 0.0f, 0.0f, 0.0f );
    g_aLights[3].fMoveDist = 7.0f;
#endif
}

//-----------------------------------------------------------------------------
// Reset light and material parameters to default values
//-----------------------------------------------------------------------------
void ResetParameters()
{
    int hr;

    g_bUseFloatCubeMap = true;
    g_fReflectivity = 0.4f;
    g_vLightIntensity = D3DXFROMWINEVECTOR4( 24.0f, 24.0f, 24.0f, 24.0f );

    if( g_pEffect )
    {
        IRenderer::GetRendererInstance()->setShaderConstant1f("g_fReflectivity", g_fReflectivity);
	IRenderer::GetRendererInstance()->setShaderConstant4f("g_vLightIntensity", g_vLightIntensity);
    }

    //UpdateUiWithChanges();
}

//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
int OnCreateDevice()
{
    RenderSceneSHD = IRenderer::GetRendererInstance()->addShaderFromFile("/HDRCubeMap.fx", "HDRVertScene", "HDRPixScene");
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
		0, TYPE_NORMAL, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	RenderSceneVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), RenderSceneSHD);}

	RenderLightSHD = IRenderer::GetRendererInstance()->addShaderFromFile("/HDRCubeMap.fx", "HDRVertLight", "HDRPixLight");
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX, FORMAT_FLOAT, 4
	};
	RenderLightVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), RenderLightSHD);}

	RenderHDREnvMapSHD = IRenderer::GetRendererInstance()->addShaderFromFile("/HDRCubeMap.fx", "HDRVertEnvMap", "HDRPixEnvMap");
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
		0, TYPE_NORMAL, FORMAT_FLOAT, 3
	};
	RenderHDREnvMapVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), RenderHDREnvMapSHD);}

	RenderHDREnvMap2TexSHD = IRenderer::GetRendererInstance()->addShaderFromFile("/HDRCubeMap.fx", "HDRVertEnvMap", "HDRPixEnvMap2Tex");
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
		0, TYPE_NORMAL, FORMAT_FLOAT, 3
	};
	RenderHDREnvMap2TexVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), RenderHDREnvMap2TexSHD);}

    g_hWorldView = g_pEffect->GetVariableByName("g_mWorldView" );
    g_hProj = g_pEffect->GetVariableByName("g_mProj" );
    g_ahtxCubeMap[0] = g_pEffect->GetVariableByName("g_txCubeMap" );
    g_ahtxCubeMap[1] = g_pEffect->GetVariableByName("g_txCubeMap2" );
    g_htxScene = g_pEffect->GetVariableByName("g_txScene" );
    g_hLightIntensity = g_pEffect->GetVariableByName("g_vLightIntensity" );
    g_hLightPosView = g_pEffect->GetVariableByName("g_vLightPosView" );
    g_hReflectivity = g_pEffect->GetVariableByName("g_fReflectivity" );

    // Determine the technique to render with

    // Initialize reflectivity   
    IRenderer::GetRendererInstance()->setShaderConstant1f("g_fReflectivity", g_fReflectivity);

    // Initialize light intensity 
    IRenderer::GetRendererInstance()->setShaderConstant4f("g_vLightIntensity", g_vLightIntensity);

    // Create vertex declaration
    ( IRenderer::GetRendererInstance()->addVertexFormat( g_aVertDecl, &g_pVertDecl ) );

    // Load the mesh
    int i;
    for( i = 0; i < NUM_MESHES; ++i )
    {
        g_EnvMesh[i].m_Mesh.Load(g_atszMeshFileName[i] eSimpleShader, true, true);
        g_EnvMesh[i].WorldCenterAndScaleToRadius( 1.0f );  // Scale to radius of 1
    }


    g_Room.Load("room.x", eSimpleShader, true, true);
    g_LightMesh.Load("/dx9sdk/teapot.x", eSimpleShader, true, true);

    // Initialize the world matrix for the lights
    float fRadius;
    g_LightMesh.GetBoundingSphereRadius(fRadius);
    D3DXFROMWINEVECTOR3 vCtr=g_LightMesh.GetMeshBBoxCenter( unsigned int );

    D3DXFROMWINEMATRIX mWorld, m;
    D3DXFROMWINEMatrixTranslation( &mWorld, -vCtr.x, -vCtr.y, -vCtr.z );
    D3DXFROMWINEMatrixScaling( &m, LIGHTMESH_RADIUS / fRadius,
                       LIGHTMESH_RADIUS / fRadius,
                       LIGHTMESH_RADIUS / fRadius );
    D3DXFROMWINEMatrixMultiply( &mWorld, &mWorld, &m );
    for( i = 0; i < NUM_LIGHTS; ++i )
    {
        D3DXFROMWINEMatrixTranslation( &m, g_aLights[i].vPos.x,
                               g_aLights[i].vPos.y,
                               g_aLights[i].vPos.z );
        D3DXFROMWINEMatrixMultiply( &g_aLights[i].mWorld, &mWorld, &m );
    }

    // Orbiter
    for( i = 0; i < sizeof( g_Orbiter ) / sizeof( g_Orbiter[0] ); ++i )
    {
        g_Orbiter[i].m_Mesh.Load(g_OrbitData[i].tszXFile, eSimpleShader, true, true);

        g_Orbiter[i].WorldCenterAndScaleToRadius( 0.7f );
        D3DXFROMWINEVECTOR3 vAxis( g_OrbitData[i].vAxis );
        g_Orbiter[i].SetOrbit( vAxis, g_OrbitData[i].fRadius, g_OrbitData[i].fSpeed );
    }

    // World transform to identity
    D3DXFROMWINEMATRIX mIdent;
    D3DXFROMWINEMatrixIdentity( &mIdent );
    ( IRenderer::GetRendererInstance()->SetTransform( D3DTS_WORLD, &mIdent ) );

    // Setup the camera's view parameters
    D3DXFROMWINEVECTOR3 vFromPt( 0.0f, 0.0f, -2.5f );
    D3DXFROMWINEVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXFROMWINEMatrixLookAtLH(&g_View, &vFromPt, &vLookatPt );

    return 0;
}

//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
int OnResetDevice()
{
    int hr;

    // Setup the camera's projection parameters
    float fAspectRatio = IRenderer::GetRendererInstance()->GetAspectRatio();
    D3DXFROMWINEMatrixPerspectiveFovLH(&g_Proj, D3DXFROMWINE_PI / 4, fAspectRatio, 0.1f, 1000.0f );

    // Restore meshes
    for( int i = 0; i < NUM_MESHES; ++i )
        g_EnvMesh[i].m_Mesh.RestoreDeviceObjects();
    g_Room.RestoreDeviceObjects();
    g_LightMesh.RestoreDeviceObjects();
    for( int i = 0; i < sizeof( g_Orbiter ) / sizeof( g_Orbiter[0] ); ++i )
        g_Orbiter[i].m_Mesh.RestoreDeviceObjects();

    // Create the cube textures
    ZeroMemory( g_apCubeMapFp, sizeof( g_apCubeMapFp ) );
	g_apCubeMapFp[0] = IRenderer::GetRendererInstance()->addCubeRenderTarget(ENVMAPSIZE, ENVMAPSIZE, ENVMAPSIZE, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());

	Image3 img;
	img.create(FORMAT_RGBA8, ENVMAPSIZE, ENVMAPSIZE, ENVMAPSIZE, 1);
	g_pCubeMap32 = IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());
#if 0
    // Create the stencil buffer to be used with the cube textures
    // Create the depth-stencil buffer to be used with the shadow map
    // We do this to ensure that the depth-stencil buffer is large
    // enough and has correct multisample type/quality when rendering
    // the shadow map.  The default depth-stencil buffer created during
    // device creation will not be large enough if the user resizes the
    // window to a very small size.  Furthermore, if the device is created
    // with multisampling, the default depth-stencil buffer will not
    // work with the shadow map texture because texture render targets
    // do not support multisample.
    ( IRenderer::GetRendererInstance()->CreateDepthStencilSurface( ENVMAPSIZE,
                                                     ENVMAPSIZE,
                                                     d3dSettings.d3d9.pp.AutoDepthStencilFormat,
                                                     D3DMULTISAMPLE_NONE,
                                                     0,
                                                     TRUE,
                                                     &g_pDepthCube,
                                                     0 ) );
#endif
    return 0;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void OnFrameMove( double fTime, float fElapsedTime)
{
    // Update the camera's position based on user input 
    //g_Camera.FrameMove( fElapsedTime );

    for( int i = 0; i < sizeof( g_Orbiter ) / sizeof( g_Orbiter[0] ); ++i )
        g_Orbiter[i].Orbit( fElapsedTime );
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void OnFrameRender( double fTime, float fElapsedTime)
{
    int hr;

    RenderSceneIntoCubeMap( fTime );

	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
    
    RenderScene( g_CameraViewMatrix, g_CameraProjMatrix, g_pTech, true, fTime );

    IRenderer::GetRendererInstance()->EndScene();
}

//--------------------------------------------------------------------------------------
// Renders the scene with a specific view and projection matrix.
//--------------------------------------------------------------------------------------
void RenderScene( const D3DXFROMWINEMATRIX* pmView, const D3DXFROMWINEMATRIX* pmProj,
                  CTechniqueGroup* pTechGroup, bool bRenderEnvMappedMesh, double fTime )
{
    int hr;
    unsigned int p, cPass;
    D3DXFROMWINEMATRIX mWorldView;

    ( IRenderer::GetRendererInstance()->setShaderConstant4x4f( g_hProj, pmProj ) );

    // Write camera-space light positions to effect
    D3DXFROMWINEVECTOR4 avLightPosView[NUM_LIGHTS];
    for( int i = 0; i < NUM_LIGHTS; ++i )
    {
        // Animate the lights
        float fDisp = ( 1.0f + cosf( fmodf( ( float )fTime, D3DXFROMWINE_PI ) ) ) * 0.5f * g_aLights[i].fMoveDist; // Distance to move
        D3DXFROMWINEVECTOR4 vMove = g_aLights[i].vMoveDir * fDisp;  // In vector form
        D3DXFROMWINEMatrixTranslation( &g_aLights[i].mWorking, vMove.x, vMove.y, vMove.z ); // Matrix form
        D3DXFROMWINEMatrixMultiply( &g_aLights[i].mWorking, &g_aLights[i].mWorld, &g_aLights[i].mWorking );
        vMove += g_aLights[i].vPos;  // Animated world coordinates
        D3DXFROMWINEVec4Transform( &avLightPosView[i], &vMove, pmView );
    }
    IRenderer::GetRendererInstance()->setShaderConstantArray4f("g_vLightPosView", avLightPosView, NUM_LIGHTS);

    //
    // Render the environment-mapped mesh if specified
    //

    if( bRenderEnvMappedMesh )
    {
    	IRenderer::GetRendererInstance()->setShader(pTechGroup->RenderEnvMapSHD);
    	IRenderer::GetRendererInstance()->setVertexFormat(pTechGroup->RenderEnvMapVF);
    	cPass=1;
        // Combine the offset and scaling transformation with
        // rotation from the camera to form the final
        // world transformation matrix.
        D3DXFROMWINEMatrixMultiply( &mWorldView, &g_EnvMesh[g_nCurrMesh].m_mWorld, g_CameraWorldMatrix );
        D3DXFROMWINEMatrixMultiply( &mWorldView, &mWorldView, pmView );
        ( IRenderer::GetRendererInstance()->setShaderConstant4x4f( g_hWorldView, &mWorldView ) );

        for( int i = 0; i < g_nNumCubes; ++i )
            ( IRenderer::GetRendererInstance()->setTexture( g_ahtxCubeMap[i], g_apCubeMap[i] ) );

        for( p = 0; p < cPass; ++p )
        {
            
            MeshRenderer2 pMesh = g_EnvMesh[g_nCurrMesh].m_Mesh.GetMesh();
            for( __DWORD__ i = 0; i < g_EnvMesh[g_nCurrMesh].m_Mesh.m_dwNumMaterials; ++i )
                ( pMesh->DrawSubset( i ) );
    	}
    }

    //
    // Render light spheres
    //
    IRenderer::GetRendererInstance()->setShader(pTechGroup->RenderLightSHD);
    IRenderer::GetRendererInstance()->setVertexFormat(pTechGroup->RenderLighteVF);
    cPass=1;
    for( p = 0; p < cPass; ++p )
    {
        

        for( int i = 0; i < NUM_LIGHTS; ++i )
        {
            D3DXFROMWINEMatrixMultiply( &mWorldView, &g_aLights[i].mWorking, pmView );
            ( IRenderer::GetRendererInstance()->setShaderConstant4x4f( g_hWorldView, &mWorldView ) );
            ( g_pEffect->CommitChanges() );
            ( g_LightMesh.Render() );
        }
    }

    //
    // Render the rest of the scene
    //

    IRenderer::GetRendererInstance()->setShader(pTechGroup->RenderSceneSHD);
    IRenderer::GetRendererInstance()->setVertexFormat(pTechGroup->RenderSceneVF);
    cPass=1;
    for( p = 0; p < cPass; ++p )
    {
        MeshRenderer2 pMeshObj;

        //
        // Orbiters
        //
        for( int i = 0; i < sizeof( g_Orbiter ) / sizeof( g_Orbiter[0] ); ++i )
        {
            D3DXFROMWINEMatrixMultiply( &mWorldView, &g_Orbiter[i].m_mWorld, pmView );
            ( IRenderer::GetRendererInstance()->setShaderConstant4x4f( g_hWorldView, &mWorldView ) );
            // Obtain the D3DXFROMWINE mesh object
            pMeshObj = g_Orbiter[i].m_Mesh.GetMesh();
            // Iterate through each subset and render with its texture
            for( __DWORD__ m = 0; m < g_Orbiter[i].m_Mesh.m_dwNumMaterials; ++m )
            {
                ( IRenderer::GetRendererInstance()->setTexture( g_htxScene, g_Orbiter[i].m_Mesh.m_pTextures[m] ) );
                ( g_pEffect->CommitChanges() );
                ( pMeshObj->DrawSubset( m ) );
            }
        }

        //
        // The room object (walls, floor, ceiling)
        //

        ( IRenderer::GetRendererInstance()->setShaderConstant4x4f( g_hWorldView, pmView ) );
        pMeshObj = g_Room.GetMesh();
        // Iterate through each subset and render with its texture
        for( __DWORD__ m = 0; m < g_Room.m_dwNumMaterials; ++m )
        {
            ( IRenderer::GetRendererInstance()->setTexture( g_htxScene, g_Room.m_pTextures[m] ) );
            ( g_pEffect->CommitChanges() );
            ( pMeshObj->DrawSubset( m ) );
        }
    }
}
//--------------------------------------------------------------------------------------
// Set up the cube map by rendering the scene into it.
//--------------------------------------------------------------------------------------
void RenderSceneIntoCubeMap( double fTime )
{
    int hr;

#if 0 // ???
    // The projection matrix has a FOV of 90 degrees and asp ratio of 1
    D3DXFROMWINEMATRIX mProj;
    D3DXFROMWINEMatrixPerspectiveFovLH( &mProj, D3DXFROMWINE_PI * 0.5f, 1.0f, 0.01f, 100.0f );

    D3DXFROMWINEMATRIX mViewDir=g_CameraViewMatrix;
    mViewDir._41 = mViewDir._42 = mViewDir._43 = 0.0f;

    SurfaceID pRTOld = 0;
    ( IRenderer::GetRendererInstance()->GetRenderTarget( 0, &pRTOld ) );
    SurfaceID pDSOld = 0;
    if( ( IRenderer::GetRendererInstance()->GetDepthStencilSurface( &pDSOld ) ) )
    {
        // If the device has a depth-stencil buffer, use
        // the depth stencil buffer created for the cube textures.
        ( //IRenderer::GetRendererInstance()->SetDepthStencilSurface( g_pDepthCube ) );
    }
    for( int nCube = 0; nCube < g_nNumCubes; ++nCube )
        for( int nFace = 0; nFace < 6; ++nFace )
        {
            SurfaceID pSurf;

            ( g_apCubeMap[nCube]->GetCubeMapSurface( ( D3DCUBEMAP_FACES )nFace, 0, &pSurf ) );
            ( IRenderer::GetRendererInstance()->changeRenderTarget( 0, pSurf ) );
            //////////////SAFE_RELEASE( pSurf );

            D3DXFROMWINEMATRIX mView = DXUTGetCubeMapViewMatrix( nFace );
            D3DXFROMWINEMatrixMultiply( &mView, &mViewDir, &mView );

	        float f=128.0f/256.0f;
	        IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));

            // Begin the scene
	        IRenderer::GetRendererInstance()->BeginScene();
            RenderScene( &mView, &mProj, &g_pTech[nCube], false, fTime );

            // End the scene.
            IRenderer::GetRendererInstance()->EndScene();
        }

    // Restore depth-stencil buffer and render target
    if( pDSOld )
    {
        ( //IRenderer::GetRendererInstance()->SetDepthStencilSurface( pDSOld ) );
        //////////////SAFE_RELEASE( pDSOld );
    }
    ( IRenderer::GetRendererInstance()->changeRenderTarget( 0, pRTOld ) );
    //////////////SAFE_RELEASE( pRTOld );
#endif
}
int init(const char* aTitle)
{
	OnCreateDevice();
	InitApp();
	return 0;
}

void render()
{
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mWorldView", );
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mProj", );
	D3DXFROMWINEVECTOR4 v4(15.0f, 15.0f, 15.0f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("g_vLightIntensity", v4);
	IRenderer::GetRendererInstance()->setShaderConstantArray4f("g_vLightPosView", , MAX_NUM_LIGHTS);  // Light positions in view space

	IRenderer::GetRendererInstance()->setShaderConstant1f("g_fReflectivity", 0.4f);                  // Reflectivity
 	IRenderer::GetRendererInstance()->setTexture("g_samCubeMap", );
	IRenderer::GetRendererInstance()->setTexture("g_samCubeMap2", );
	IRenderer::GetRendererInstance()->setTexture("g_samScene", );


    double fTime;
    float fElapsedTime;
    OnFrameMove( fTime, fElapsedTime);
	OnFrameRender( fTime, fElapsedTime);
}
};
App app;
int init(const char* aTitle)
{
    app.init(aTitle);	
	return 0;
}

void render()
{
   app.render(); 
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("HDRCubeMap");
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
		IRenderer::GetRendererInstance()->Present( );
	}
	return 0;
}

