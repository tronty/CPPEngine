//--------------------------------------------------------------------------------------
// File: ParticlesGS.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
D3DXFROMWINEMATRIX g_CameraEyePt;
D3DXFROMWINEMATRIX g_CameraProj;
D3DXFROMWINEMATRIX g_CameraView;
D3DXFROMWINEMATRIX g_CameraWorld;
               // A model viewing camera
bool                                g_bFirst = true;
stx_Effect*                        g_pEffect10 = 0;
VertexFormatID                   g_pParticleVertexLayout = 0;

exBufferID                        g_pParticleBuffer = 0;
TextureID            g_pParticleTexRV = 0;

TextureID                     g_pParticleDataTextureTo = 0;
TextureID            g_pParticleDataTexSRVTo = 0;
TextureID              g_pParticleDataTexRTVTo = 0;
TextureID                     g_pParticleDataTextureFrom = 0;
TextureID            g_pParticleDataTexSRVFrom = 0;
TextureID              g_pParticleDataTexRTVFrom = 0;
TextureID                     g_pForceTexture = 0;
TextureID            g_pForceTexSRV = 0;
TextureID              g_pForceTexRTV = 0;

stx_Effect::stx_Technique*              g_pRenderParticles;
stx_Effect::stx_Technique*              g_pAccumulateForces;
stx_Effect::stx_Technique*              g_pAdvanceParticles;

stx_Effect::stx_Variable*         g_pmWorldViewProj;
stx_Effect::stx_Variable*         g_pmInvView;
stx_Effect::stx_Variable*         g_pfGlobalTime;
stx_Effect::stx_Variable*         g_pfElapsedTime;
stx_Effect::stx_Variable*         g_piTexSize;
stx_Effect::stx_Variable*         g_pfParticleRad;
stx_Effect::stx_Variable*         g_pfParticleMass;
stx_Effect::stx_Variable*         g_pfG;

stx_ShaderResourceVariable* g_pDiffuseTex;
stx_ShaderResourceVariable* g_pParticleDataTex;
stx_ShaderResourceVariable* g_pForceTex;

struct PARTICLE_VERTEX
{
    D3DXFROMWINEVECTOR4 Color;
};


#define MAX_PARTICLES 10000
#define MAX_PARTICLES_SIDE 100
#define SIMULATION_SPEED 0.1f

unsigned int                                g_iTexSize;
float                               g_fParticleMass = 10000.0f * 10000.0f;
float                               g_fGConstant = 6.67300e-11f * 10000.0f;
float                               g_fSpread = 400.0f;
float                               g_fParticleRad = 10.0f;

struct App
{
    App(){}
    virtual ~App(){}

//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
int OnCreateDevice()
{
    int hr;

    // Read the D3DXFROMWINE effect file
    g_pEffect10="NBodyGravity.fx";

    // Obtain the technique handles
    g_pRenderParticles = g_pEffect10->GetTechniqueByName( "RenderParticles" );
    g_pAccumulateForces = g_pEffect10->GetTechniqueByName( "AccumulateForces" );
    g_pAdvanceParticles = g_pEffect10->GetTechniqueByName( "AdvanceParticles" );

    // Obtain the parameter handles
    g_pmWorldViewProj = g_pEffect10->GetVariableByName( "g_mWorldViewProj" )->AsMatrix();
    g_pmInvView = g_pEffect10->GetVariableByName( "g_mInvView" )->AsMatrix();
    g_pfGlobalTime = g_pEffect10->GetVariableByName( "g_fGlobalTime" )->AsScalar();
    g_pfElapsedTime = g_pEffect10->GetVariableByName( "g_fElapsedTime" )->AsScalar();
    g_piTexSize = g_pEffect10->GetVariableByName( "g_iTexSize" )->AsScalar();
    g_pfParticleRad = g_pEffect10->GetVariableByName( "g_fParticleRad" )->AsScalar();
    g_pfParticleMass = g_pEffect10->GetVariableByName( "g_fParticleMass" )->AsScalar();
    g_pfG = g_pEffect10->GetVariableByName( "g_fG" )->AsScalar();

    g_pDiffuseTex = g_pEffect10->GetVariableByName( "g_txDiffuse" )->AsShaderResource();
    g_pParticleDataTex = g_pEffect10->GetVariableByName( "g_txParticleData" )->AsShaderResource();
    g_pForceTex = g_pEffect10->GetVariableByName( "g_txForce" )->AsShaderResource();

    // Create our vertex input layout
	FormatDesc layout[] = {
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
	};
	g_pParticleVertexLayout = IRenderer::GetRendererInstance()->addVertexFormat(layout, elementsOf(layout), -1);

    // Set effect variables as needed
    D3DXFROMWINECOLOR colorMtrlDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
    D3DXFROMWINECOLOR colorMtrlAmbient( 0.35f, 0.35f, 0.35f, 0 );

    // Create the seeding particle
    ( CreateParticleTextures() );
    ( CreateParticleBuffer() );
    ( CreateForceTexture() );

    // Load the Particle Texture
	g_pParticleTexRV="misc\\Particle.dds";

    // Setup the camera's view parameters
    D3DXFROMWINEVECTOR3 vecEye( -g_fSpread * 2, g_fSpread * 4, -g_fSpread * 3 );
    D3DXFROMWINEVECTOR3 vecAt ( 0.0f, 0.0f, 0.0f );
    g_Camera.SetViewParams( &vecEye, &vecAt );

    return 0;
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
int OnSwapChainResized( )
{
    int hr = 0;

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DXFROMWINE_PI / 4, fAspectRatio, 100.0f, 500000.0f );

    g_bFirst = true;
    return hr;
}


//--------------------------------------------------------------------------------------
bool RenderParticles( D3DXFROMWINEMATRIX& mView, D3DXFROMWINEMATRIX& mProj )
{
    D3DXFROMWINEMATRIX mWorldView;
    D3DXFROMWINEMATRIX mWorldViewProj;

    // Set the Vertex Layout
    IRenderer::GetRendererInstance()->setVertexFormat(0,  g_pParticleVertexLayout );

    // Set IA parameters
    exBufferID  pBuffers[1] = { g_pParticleBuffer };
    unsigned int stride[1] = { sizeof( PARTICLE_VERTEX ) };
    unsigned int offset[1] = { 0 };
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1, pBuffers, stride, offset );
    IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_POINTS );

    // Set Effects Parameters
    D3DXFROMWINEMatrixMultiply( &mWorldViewProj, &mView, &mProj );
    g_pmWorldViewProj->SetMatrix( ( float* )&mWorldViewProj );
    D3DXFROMWINEMATRIX mInvView;
    D3DXFROMWINEMatrixInverse( &mInvView, 0, &mView );
    g_pmInvView->SetMatrix( ( float* )&mInvView );
    g_piTexSize->SetInt( g_iTexSize );
    g_pfParticleRad->SetFloat( g_fParticleRad );

    // Set resources
    g_pParticleDataTex->SetResource( g_pParticleDataTexSRVFrom );
    g_pDiffuseTex->SetResource( g_pParticleTexRV );
    g_pForceTex->SetResource( g_pForceTexSRV );

    // Draw
    
    g_pRenderParticles->GetDesc( &techDesc );

    for( unsigned int p = 0; p < techDesc.Passes; ++p )
    {
        g_pRenderParticles->GetPassByIndex( p )->Apply( 0 );
        IRenderer::GetRendererInstance()->Draw( MAX_PARTICLES, 0 );
    }

    TextureID  pNULLs[4] = {0,0,0,0};
    IRenderer::GetRendererInstance()->VSSetShaderResources( 0, 4, pNULLs );
    IRenderer::GetRendererInstance()->PSSetShaderResources( 0, 4, pNULLs );

    return true;
}


//--------------------------------------------------------------------------------------
bool AccumulateForces()
{
    // Clear the force texture
    IRenderer::GetRendererInstance()->Clear(true, false, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));

    // Store the old render targets
    TextureID  pOldRTV;
    TextureID  pOldDSV;
    IRenderer::GetRendererInstance()->OMGetRenderTargets( 1, &pOldRTV, &pOldDSV );

    // Store the old viewport
    unsigned int iNumVP = 1;
    D3DFROMWINEVIEWPORT9 OldVP;
    IRenderer::GetRendererInstance()->RSGetViewports( &iNumVP, &OldVP );

    // Set the new render targets
    IRenderer::GetRendererInstance->changeRenderTarget(&g_pForceTexRTV, 0 );


    IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_TRIANGLES );

    // Set the new viewport
    D3DFROMWINEVIEWPORT9 NewVP;
    NewVP.Width = g_iTexSize;
    NewVP.Height = g_iTexSize;
    NewVP.TopLeftX = 0;
    NewVP.TopLeftY = 0;
    NewVP.MinDepth = 0.0f;
    NewVP.MaxDepth = 1.0f;
    IRenderer::GetRendererInstance()->RSSetViewports( 1, &NewVP );

    // Set the Vertex Layout
    IRenderer::GetRendererInstance()->setVertexFormat(0,  0 );


    // Set Effects Parameters
    g_piTexSize->SetInt( g_iTexSize );
    g_pfParticleMass->SetFloat( g_fParticleMass );
    g_pfG->SetFloat( g_fGConstant );
    g_pfParticleRad->SetFloat( g_fParticleRad );

    // Set resources
    g_pParticleDataTex->SetResource( g_pParticleDataTexSRVFrom );

    // Draw
    
    g_pAccumulateForces->GetDesc( &techDesc );

    for( unsigned int p = 0; p < techDesc.Passes; ++p )
    {
        g_pAccumulateForces->GetPassByIndex( p )->Apply( 0 );
        IRenderer::GetRendererInstance()->Draw( MAX_PARTICLES_SIDE * 6, 0 );
    }

    // Restore the old vp
    IRenderer::GetRendererInstance()->RSSetViewports( 1, &OldVP );

    // Restore the old render targets
    IRenderer::GetRendererInstance->changeRenderTarget(&pOldRTV, pOldDSV );
    //////////////////SAFE_RELEASE( pOldRTV );
    //////////////////SAFE_RELEASE( pOldDSV );

    TextureID  pNULLs[4] = {0,0,0,0};
    IRenderer::GetRendererInstance()->VSSetShaderResources( 0, 4, pNULLs );
    IRenderer::GetRendererInstance()->PSSetShaderResources( 0, 4, pNULLs );

    return true;
}


//--------------------------------------------------------------------------------------
bool AdvanceParticles( float fElapsedTime )
{
    // Store the old render targets
    TextureID  pOldRTV;
    TextureID  pOldDSV;
    IRenderer::GetRendererInstance()->OMGetRenderTargets( 1, &pOldRTV, &pOldDSV );

    // Store the old viewport
    unsigned int iNumVP = 1;
    D3DFROMWINEVIEWPORT9 OldVP;
    IRenderer::GetRendererInstance()->RSGetViewports( &iNumVP, &OldVP );

    // Set the new render targets
    IRenderer::GetRendererInstance->changeRenderTarget(&g_pParticleDataTexRTVTo, 0 );

    // Set the new viewport
    D3DFROMWINEVIEWPORT9 NewVP;
    NewVP.Width = g_iTexSize;
    NewVP.Height = g_iTexSize;
    NewVP.TopLeftX = 0;
    NewVP.TopLeftY = 0;
    NewVP.MinDepth = 0.0f;
    NewVP.MaxDepth = 1.0f;
    IRenderer::GetRendererInstance()->RSSetViewports( 1, &NewVP );

    // Set the Vertex Layout
    IRenderer::GetRendererInstance()->setVertexFormat(0,  g_pParticleVertexLayout );

    // Set IA parameters
    exBufferID  pBuffers[1] = { g_pParticleBuffer };
    unsigned int stride[1] = { sizeof( PARTICLE_VERTEX ) };
    unsigned int offset[1] = { 0 };
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1, pBuffers, stride, offset );
    IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_POINTS );

    // Set Effects Parameters
    g_piTexSize->SetInt( g_iTexSize );
    g_pfParticleMass->SetFloat( g_fParticleMass );
    g_pfElapsedTime->SetFloat( fElapsedTime );

    // Set resources
    g_pParticleDataTex->SetResource( g_pParticleDataTexSRVFrom );
    g_pForceTex->SetResource( g_pForceTexSRV );

    // Draw
    
    g_pAdvanceParticles->GetDesc( &techDesc );

    for( unsigned int p = 0; p < techDesc.Passes; ++p )
    {
        g_pAdvanceParticles->GetPassByIndex( p )->Apply( 0 );
        IRenderer::GetRendererInstance()->Draw( 1, 0 );
    }

    // Restore the old vp
    IRenderer::GetRendererInstance()->RSSetViewports( 1, &OldVP );

    // Restore the old render targets
    IRenderer::GetRendererInstance->changeRenderTarget(&pOldRTV, pOldDSV );
    //////////////////SAFE_RELEASE( pOldRTV );
    //////////////////SAFE_RELEASE( pOldDSV );

    TextureID  pNULLs[4] = {0,0,0,0};
    IRenderer::GetRendererInstance()->VSSetShaderResources( 0, 4, pNULLs );
    IRenderer::GetRendererInstance()->PSSetShaderResources( 0, 4, pNULLs );

    return true;
}


//--------------------------------------------------------------------------------------
void SwapParticleTextures()
{
    TextureID  Temp1 = g_pParticleDataTextureTo;
    TextureID  Temp2 = g_pParticleDataTexSRVTo;
    TextureID  Temp3 = g_pParticleDataTexRTVTo;

    g_pParticleDataTextureTo = g_pParticleDataTextureFrom;
    g_pParticleDataTexSRVTo = g_pParticleDataTexSRVFrom;
    g_pParticleDataTexRTVTo = g_pParticleDataTexRTVFrom;

    g_pParticleDataTextureFrom = Temp1;
    g_pParticleDataTexSRVFrom = Temp2;
    g_pParticleDataTexRTVFrom = Temp3;
}

//--------------------------------------------------------------------------------------
void OnFrameRender( double fTime, float fElapsedTime )
{
    IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));

    // Accumulate forces
    AccumulateForces();

    // Apply forces to particles
    AdvanceParticles( SIMULATION_SPEED );

    // Swap the ping-pong buffers
    SwapParticleTextures();

    D3DXFROMWINEMATRIX mView;
    D3DXFROMWINEMATRIX mProj;

    // Get the projection & view matrix from the camera class
    mProj = *g_Camera.GetProjMatrix();
    mView = *g_Camera.GetViewMatrix();

    // Render the particles
    RenderParticles( mView, mProj );
}

//--------------------------------------------------------------------------------------
float RPercent()
{
    float ret = ( float )( ( stx_rand() % 10000 ) - 5000 );
    return ret / 5000.0f;
}


//--------------------------------------------------------------------------------------
// This helper function loads a group of particles
//--------------------------------------------------------------------------------------
void LoadParticles( D3DXFROMWINEVECTOR4* pPositions, D3DXFROMWINEVECTOR4* pVelocities,
                    D3DXFROMWINEVECTOR3 Center, D3DXFROMWINEVECTOR4 Velocity, float Spread, unsigned int NumParticles )
{
    for( unsigned int i = 0; i < NumParticles; i++ )
    {
        D3DXFROMWINEVECTOR3 Delta( Spread, Spread, Spread );

        while( D3DXFROMWINEVec3LengthSq( &Delta ) > Spread * Spread )
        {
            Delta.x = RPercent() * Spread;
            Delta.y = RPercent() * Spread;
            Delta.z = RPercent() * Spread;
        }

        pPositions[i].x = Center.x + Delta.x;
        pPositions[i].y = Center.y + Delta.y;
        pPositions[i].z = Center.z + Delta.z;
        pPositions[i].w = 1.0f;

        pVelocities[i] = Velocity;
    }
}

//--------------------------------------------------------------------------------------
// This helper function creates the texture array that will store all of the particle
// data.  Position, Velocity.
//--------------------------------------------------------------------------------------
int CreateParticleTextures()
{
    int hr = 0;

    unsigned int SideSize = ( unsigned int )sqrtf( MAX_PARTICLES ) + 1;
    g_iTexSize = SideSize;
    unsigned int ArraySize = 2;

    // Create the texture
    SURFACE_DESC dstex;
    dstex.Width = SideSize;
    dstex.Height = SideSize;
    dstex.MipLevels = 1;
    dstex.ArraySize = ArraySize;
    dstex.Format = FORMAT_RGBA32F;
    dstex.Usage = USAGE_DEFAULT;
    dstex.BindFlags = BIND_SHADER_RESOURCE | BIND_RENDER_TARGET;
    dstex.CPUAccessFlags = 0;
    dstex.MiscFlags = 0;
    dstex.SampleDesc.Count = 1;
    dstex.SampleDesc.Quality = 0;

    // Initialize the data in the buffers
    unsigned int MaxTextureParticles = SideSize * SideSize;
    D3DXFROMWINEVECTOR4* pData1 = new D3DXFROMWINEVECTOR4[ MaxTextureParticles ];
    if( !pData1 )
        return E_OUTOFMEMORY;
    D3DXFROMWINEVECTOR4* pData2 = new D3DXFROMWINEVECTOR4[ MaxTextureParticles ];
    if( !pData2 )
        return E_OUTOFMEMORY;

    stx_srand( timeGetTime() );

#if 1
    // Disk Galaxy Formation
    float fCenterSpread = g_fSpread * 0.50f;
    LoadParticles( pData1, pData2,
                   D3DXFROMWINEVECTOR3( fCenterSpread, 0, 0 ), D3DXFROMWINEVECTOR4( 0, 0, -20, 0 ),
                   g_fSpread, MaxTextureParticles / 2 );
    LoadParticles( &pData1[MaxTextureParticles / 2], &pData2[MaxTextureParticles / 2],
                   D3DXFROMWINEVECTOR3( -fCenterSpread, 0, 0 ), D3DXFROMWINEVECTOR4( 0, 0, 20, 0 ),
                   g_fSpread, MaxTextureParticles / 2 );
#else
    // Disk Galaxy Formation with impacting third cluster
    LoadParticles( pData1, pData2,
                   D3DXFROMWINEVECTOR3(g_fSpread,0,0), D3DXFROMWINEVECTOR4(0,0,-8,0),
                   g_fSpread, MaxTextureParticles/3 );
    LoadParticles( &pData1[MaxTextureParticles/3], &pData2[MaxTextureParticles/3],
                   D3DXFROMWINEVECTOR3(-g_fSpread,0,0), D3DXFROMWINEVECTOR4(0,0,8,0),
                   g_fSpread, MaxTextureParticles/2 );
    LoadParticles( &pData1[2*(MaxTextureParticles/3)], &pData2[2*(MaxTextureParticles/3)],
                   D3DXFROMWINEVECTOR3(0,0,g_fSpread*15.0f), D3DXFROMWINEVECTOR4(0,0,-60,0),
                   g_fSpread, MaxTextureParticles - 2*(MaxTextureParticles/3) );
#endif
	{Image3 img;
	img.create(FORMAT_RGBA32f, SideSize, SideSize, 1, ArraySize);
	img.setPixels(pData1);
	g_pParticleDataTextureTo=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());}
	{Image3 img;
	img.create(FORMAT_RGBA32f, SideSize, SideSize, 1, ArraySize);
	img.setPixels(pData2);
	g_pParticleDataTextureFrom=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());}

    SAFE_DELETE_ARRAY( pData1 );
    SAFE_DELETE_ARRAY( pData2 );

    return hr;
}


//--------------------------------------------------------------------------------------
int CreateForceTexture()
{
    int hr = 0;

    unsigned int SideSize = ( unsigned int )sqrtf( MAX_PARTICLES ) + 1;
    g_iTexSize = SideSize;
    unsigned int ArraySize = 1;

    // Create the texture
    SURFACE_DESC dstex;
    dstex.Width = SideSize;
    dstex.Height = SideSize;
    dstex.MipLevels = 1;
    dstex.ArraySize = ArraySize;
    dstex.Format = FORMAT_RGBA32F;
    dstex.Usage = USAGE_DEFAULT;
    dstex.BindFlags = BIND_SHADER_RESOURCE | BIND_RENDER_TARGET;
    dstex.CPUAccessFlags = 0;
    dstex.MiscFlags = 0;
    dstex.SampleDesc.Count = 1;
    dstex.SampleDesc.Quality = 0;

    // Initialize the data in the buffers
    unsigned int MaxTextureParticles = SideSize * SideSize;
    D3DXFROMWINEVECTOR4* pData = new D3DXFROMWINEVECTOR4[ MaxTextureParticles ];
    if( !pData )
        return E_OUTOFMEMORY;

    for( unsigned int i = 0; i < MaxTextureParticles; i++ )
    {
        pData[i] = D3DXFROMWINEVECTOR4( 0, 0, 0, 1 );
    }
	Image3 img;
	img.create(FORMAT_RGBA32f, MaxTextureParticles, 1, 1, 1);
	img.setPixels(pData);
	g_pForceTexture=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());
    SAFE_DELETE_ARRAY( pData );

    return hr;
}

//--------------------------------------------------------------------------------------
// This helper function creates 3 vertex buffers.  The first is used to seed the
// particle system.  The second two are used as output and intput buffers alternatively
// for the GS particle system.  Since a buffer cannot be both an input to the GS and an
// output of the GS, we must ping-pong between the two.
//--------------------------------------------------------------------------------------
int CreateParticleBuffer()
{
    int hr = 0;

    D3D10_BUFFER_DESC vbdesc =
    {
        MAX_PARTICLES * sizeof( PARTICLE_VERTEX ),
        USAGE_DEFAULT,
        BIND_VERTEX_BUFFER,
        0,
        0
    };
    
    ZeroMemory( &vbInitData, sizeof( 

    PARTICLE_VERTEX* pVertices = new PARTICLE_VERTEX[ MAX_PARTICLES ];
    if( !pVertices )
        return E_OUTOFMEMORY;

    for( unsigned int i = 0; i < MAX_PARTICLES; i++ )
    {
        pVertices[i].Color = D3DXFROMWINEVECTOR4( 1, 1, 0.2f, 1 );
    }

    g_pParticleBuffer=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof(PARTICLE_VERTEX)*MAX_PARTICLES, STATIC, (const void *)pVertices);
    SAFE_DELETE_ARRAY( pVertices );

    return hr;
}



//--------------------------------------------------------------------------------------
// This helper function creates a 1D texture full of random vectors.  The shader uses
// the current time value to index into this texture to get a random vector.
//--------------------------------------------------------------------------------------
/*int CreateRandomTexture()
   {
   int hr = 0;
   
   int iNumRandValues = 1024;
   stx_srand( timeGetTime() );
   //create the data
   
   InitData.pSysMem = new float[iNumRandValues*4];
   if(!InitData.pSysMem)
   return E_OUTOFMEMORY;
   InitData.SysMemPitch = iNumRandValues*4*sizeof(float);
   InitData.SysMemSlicePitch = iNumRandValues*4*sizeof(float);
   for(int i=0; i<iNumRandValues*4; i++)
   {
   ((float*)InitData.pSysMem)[i] = float( (stx_rand()%10000) - 5000 );
   }
   
   // Create the texture
   
   dstex.Width = iNumRandValues;
   dstex.MipLevels = 1;
   dstex.Format = FORMAT_RGBA32F;
   dstex.Usage = USAGE_DEFAULT;
   dstex.BindFlags = BIND_SHADER_RESOURCE;
   dstex.CPUAccessFlags = 0;
   dstex.MiscFlags = 0;
   dstex.ArraySize = 1;
   (IRenderer::GetRendererInstance()->CreateTexture1D( &dstex, &InitData, &g_pRandomTexture ));
   SAFE_DELETE_ARRAY(InitData.pSysMem);
   
   return hr;
   }*/
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
	IRenderer* r=IRenderer::GetRendererInstance("NBodyGravity");
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
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

