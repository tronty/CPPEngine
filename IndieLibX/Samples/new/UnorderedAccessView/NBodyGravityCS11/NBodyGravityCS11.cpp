//--------------------------------------------------------------------------------------
// File: NBodyGravityCS11.cpp
//
// Demonstrates how to use Compute Shader to do n-body gravity computation
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
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

VertexShaderID                 g_pRenderParticlesVS = 0;
GeometryShaderID               g_pRenderParticlesGS = 0;
PixelShaderID                  g_pRenderParticlesPS = 0;
SamplerStateID*                 g_pSampleStateLinear = 0;
BlendStateID                   g_pBlendingStateParticle = 0;
DepthStateID            g_pDepthStencilState = 0;

ComputeShaderID                 g_pCalcCS = 0;
exBufferID                        g_pcbCS = 0;

exBufferID                        g_pParticlePosVelo0 = 0;
exBufferID                        g_pParticlePosVelo1 = 0;
TextureID            g_pParticlePosVeloRV0 = 0;
TextureID            g_pParticlePosVeloRV1 = 0;
ID3D11UnorderedAccessView*          g_pParticlePosVeloUAV0 = 0;
ID3D11UnorderedAccessView*          g_pParticlePosVeloUAV1 = 0;
exBufferID                        g_pParticleBuffer = 0;
VertexFormatID                   g_pParticleVertexLayout = 0;

exBufferID                        g_pcbGS = 0;

TextureID            g_pParticleTexRV = 0;

float                               g_fSpread = 400.0f;

struct PARTICLE_VERTEX
{
    D3DXFROMWINEVECTOR4 Color;
};
#define MAX_PARTICLES 10000         // the number of particles in the n-body simulation

struct CB_GS
{
    D3DXFROMWINEMATRIX m_WorldViewProj;
    D3DXFROMWINEMATRIX m_InvView;
};

struct CB_CS
{
    unsigned int param[4];
    float paramf[4];
};

struct PARTICLE
{
    D3DXFROMWINEVECTOR4 pos;
    D3DXFROMWINEVECTOR4 velo;
};

class App: public STXGUI
{
public:
    App(){}
    virtual ~App(){}
    int init(const char* aTitle){STXGUI::init("/GUILayout.xml");}
    void render( ){STXGUI::update();}

int CreateParticleBuffer()
{
    int hr = 0;
#if 0
    D3D11_BUFFER_DESC vbdesc =
    {
        MAX_PARTICLES * sizeof( PARTICLE_VERTEX ),
        USAGE_DEFAULT,
        BIND_VERTEX_BUFFER,
        0,
        0
    };
#endif

    PARTICLE_VERTEX* pVertices = new PARTICLE_VERTEX[ MAX_PARTICLES ];
    if( !pVertices )
        return E_OUTOFMEMORY;

    for( unsigned int i = 0; i < MAX_PARTICLES; i++ )
    {
        pVertices[i].Color = D3DXFROMWINEVECTOR4( 1, 1, 0.2f, 1 );
    }
    g_pParticleBuffer=IRenderer::GetRendererInstance()->addVertexBuffer(MAX_PARTICLES*sizeof(PARTICLE_VERTEX), STATIC, (const void *)pVertices);
    SAFE_DELETE_ARRAY( pVertices );

    return hr;
}

//--------------------------------------------------------------------------------------
float RPercent()
{
    float ret = ( float )( ( rand() % 10000 ) - 5000 );
    return ret / 5000.0f;
}

float
normalize(D3DXFROMWINEVECTOR3& vector)
{
    float dist = sqrtf(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z);
    if (dist > 1e-6)
    {
        vector.x /= dist;
        vector.y /= dist;
        vector.z /= dist;
    }
    return dist;
}

//--------------------------------------------------------------------------------------
// This helper function loads a group of particles
//--------------------------------------------------------------------------------------
void LoadParticles( PARTICLE* pParticles,
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

        pParticles[i].pos.x = Center.x + Delta.x;
        pParticles[i].pos.y = Center.y + Delta.y;
        pParticles[i].pos.z = Center.z + Delta.z;
        pParticles[i].pos.w = 10000.0f * 10000.0f;

        pParticles[i].velo = Velocity;
    }    
}

int CreateParticlePosVeloBuffers()
{
    int hr = 0;

    
    ZeroMemory( &desc, sizeof(desc) );
    desc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
    desc.ByteWidth = MAX_PARTICLES * sizeof(PARTICLE);
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.StructureByteStride = sizeof(PARTICLE);
    desc.Usage = USAGE_DEFAULT;

    // Initialize the data in the buffers
    PARTICLE* pData1 = new PARTICLE[ MAX_PARTICLES ];
    if( !pData1 )
        return E_OUTOFMEMORY;    

    srand( GetTickCount() );   

#if 1
    // Disk Galaxy Formation
    float fCenterSpread = g_fSpread * 0.50f;
    LoadParticles( pData1,
        D3DXFROMWINEVECTOR3( fCenterSpread, 0, 0 ), D3DXFROMWINEVECTOR4( 0, 0, -20, 1/10000.0f / 10000.0f ),
        g_fSpread, MAX_PARTICLES / 2 );
    LoadParticles( &pData1[MAX_PARTICLES / 2],
        D3DXFROMWINEVECTOR3( -fCenterSpread, 0, 0 ), D3DXFROMWINEVECTOR4( 0, 0, 20, 1/10000.0f / 10000.0f ),
        g_fSpread, MAX_PARTICLES / 2 );    
#else
    // Disk Galaxy Formation with impacting third cluster
    LoadParticles( pData1, 
        D3DXFROMWINEVECTOR3(g_fSpread,0,0), D3DXFROMWINEVECTOR4(0,0,-8,1/10000.0f / 10000.0f),
        g_fSpread, MAX_PARTICLES/3 );
    LoadParticles( &pData1[MAX_PARTICLES/3],
        D3DXFROMWINEVECTOR3(-g_fSpread,0,0), D3DXFROMWINEVECTOR4(0,0,8,1/10000.0f / 10000.0f),
        g_fSpread, MAX_PARTICLES/2 );
    LoadParticles( &pData1[2*(MAX_PARTICLES/3)],
        D3DXFROMWINEVECTOR3(0,0,g_fSpread*15.0f), D3DXFROMWINEVECTOR4(0,0,-60,1/10000.0f / 10000.0f),
        g_fSpread, MAX_PARTICLES - 2*(MAX_PARTICLES/3) );
#endif
    g_pParticlePosVelo0=IRenderer::GetRendererInstance()->addVertexBuffer( MAX_PARTICLES*sizeof(PARTICLE), STATIC, (const void *)pData1);
    g_pParticlePosVelo1=IRenderer::GetRendererInstance()->addVertexBuffer( MAX_PARTICLES*sizeof(PARTICLE), STATIC, (const void *)pData1);
    SAFE_DELETE_ARRAY( pData1 );

    return hr;
}

template <class T>
void SWAP( T* &x, T* &y )
{
    T* temp = x;
    x = y;
    y = temp;
}

//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//--------------------------------------------------------------------------------------
void OnFrameMove( double fTime, float fElapsedTime)
{
    int hr;

    int dimx = int(ceil(MAX_PARTICLES/128.0f));
    
    {
        IRenderer::GetRendererInstance()->CSSetShader( g_pCalcCS, 0, 0 );

        // For CS input            
        TextureID  aRViews[ 1 ] = { g_pParticlePosVeloRV0 };
        IRenderer::GetRendererInstance()->CSSetShaderResources( 0, 1, aRViews );

        // For CS output
        ID3D11UnorderedAccessView* aUAViews[ 1 ] = { g_pParticlePosVeloUAV1 };
        IRenderer::GetRendererInstance()->CSSetUnorderedAccessViews( 0, 1, aUAViews, (unsigned int*)(&aUAViews) );

        // For CS constant buffer
        
        ( IRenderer::GetRendererInstance()->Map( g_pcbCS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
        CB_CS* pcbCS = ( CB_CS* )MappedResource.pData;
        pcbCS->param[0] = MAX_PARTICLES;
        pcbCS->param[1] = dimx;      
        pcbCS->paramf[0] = 0.1f;
        pcbCS->paramf[1] = 1;
        IRenderer::GetRendererInstance()->Unmap( g_pcbCS, 0 );
        exBufferID  ppCB[1] = { g_pcbCS };
        IRenderer::GetRendererInstance()->CSSetConstantBuffers( 0, 1, ppCB );

        // Run the CS
        IRenderer::GetRendererInstance()->Dispatch( dimx, 1, 1 );

        // Unbind resources for CS

        //IRenderer::GetRendererInstance()->CSSetShader( 0, 0, 0 );

        SWAP( g_pParticlePosVelo0, g_pParticlePosVelo1 );
        SWAP( g_pParticlePosVeloRV0, g_pParticlePosVeloRV1 );
        SWAP( g_pParticlePosVeloUAV0, g_pParticlePosVeloUAV1 );
    }

    // Update the camera's position based on user input 
    g_Camera.FrameMove( fElapsedTime );
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
int OnCreateDevice()
{
    int hr;

    // Create the shaders
    ( CompileShaderFromFile( "ParticleDraw.hlsl", "VSParticleDraw", "vs_4_0", &pBlobRenderParticlesVS ) );
    ( CompileShaderFromFile( "ParticleDraw.hlsl", "GSParticleDraw", "gs_4_0", &pBlobRenderParticlesGS ) );
    ( CompileShaderFromFile( "ParticleDraw.hlsl", "PSParticleDraw", "ps_4_0", &pBlobRenderParticlesPS ) );
    ( CompileShaderFromFile( "NBodyGravityCS11.hlsl", "CSMain", "cs_4_0", &pBlobCalcCS ) );

    // Create our vertex input layout
	FormatDesc layout[] = {
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
	};
	g_pParticleVertexLayout = IRenderer::GetRendererInstance()->addVertexFormat(layout, elementsOf(layout), -1);

    ( CreateParticleBuffer() );
    ( CreateParticlePosVeloBuffers() );

    // Setup constant buffer
    g_pcbGS=IRenderer::GetRendererInstance()->addComputeBuffer(sizeof( CB_GS ), DYNAMIC);
    g_pcbCS=IRenderer::GetRendererInstance()->addComputeBuffer(sizeof( CB_CS ), DYNAMIC);

    // Load the Particle Texture
    g_pParticleTexRV="misc\\Particle.dds";

    
    ZeroMemory( &SamplerDesc, sizeof(SamplerDesc) );
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    ( IRenderer::GetRendererInstance()->CreateSamplerState( &SamplerDesc, &g_pSampleStateLinear ) );
    DXUT_SetDebugName( g_pSampleStateLinear, "Linear" );

    
    ZeroMemory( &BlendStateDesc, sizeof(BlendStateDesc) );
    BlendStateDesc.RenderTarget[0].BlendEnable = TRUE;
    BlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    BlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    BlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    BlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    BlendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
    ( IRenderer::GetRendererInstance()->CreateBlendState( &BlendStateDesc, &g_pBlendingStateParticle ) );
    DXUT_SetDebugName( g_pBlendingStateParticle, "Blending" );

    
    ZeroMemory( &DepthStencilDesc, sizeof(DepthStencilDesc) );
    DepthStencilDesc.DepthEnable = FALSE;
    DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    IRenderer::GetRendererInstance()->CreateDepthStencilState( &DepthStencilDesc, &g_pDepthStencilState );
    DXUT_SetDebugName( g_pDepthStencilState, "DepthOff" );

    // Setup the camera's view parameters
    D3DXFROMWINEVECTOR3 vecEye( -g_fSpread * 2, g_fSpread * 4, -g_fSpread * 3 );
    D3DXFROMWINEVECTOR3 vecAt ( 0.0f, 0.0f, 0.0f );
    g_Camera.SetViewParams( &vecEye, &vecAt );

    CompilingShadersDlg.DestroyDialog();

    return 0;
}

int OnResizedSwapChain()
{
    int hr;

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DXFROMWINE_PI / 4, fAspectRatio, 10.0f, 500000.0f );
    g_Camera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );

    return hr;
}

//--------------------------------------------------------------------------------------
bool RenderParticles( D3DXFROMWINEMATRIX& mView, D3DXFROMWINEMATRIX& mProj )
{
    D3DXFROMWINEMATRIX mWorldViewProj;

    BlendStateIDpBlendState0 = 0;
    DepthStateIDpDepthStencilState0 = 0;
    unsigned int SampleMask0, StencilRef0;
    D3DXFROMWINEVECTOR4 BlendFactor0;
    IRenderer::GetRendererInstance()->OMGetBlendState( &pBlendState0, &BlendFactor0.x, &SampleMask0 );
    IRenderer::GetRendererInstance()->OMGetDepthStencilState( &pDepthStencilState0, &StencilRef0 );

    IRenderer::GetRendererInstance()->VSSetShader( g_pRenderParticlesVS, 0, 0 );
    IRenderer::GetRendererInstance()->GSSetShader( g_pRenderParticlesGS, 0, 0 );
    IRenderer::GetRendererInstance()->PSSetShader( g_pRenderParticlesPS, 0, 0 );
    
    IRenderer::GetRendererInstance()->setVertexFormat(0,  g_pParticleVertexLayout );

    // Set IA parameters
    exBufferID  pBuffers[1] = { g_pParticleBuffer };
    unsigned int stride[1] = { sizeof( PARTICLE_VERTEX ) };
    unsigned int offset[1] = { 0 };
    IRenderer::GetRendererInstance()->setVertexBuffer( 0, 1, pBuffers, stride, offset );
    IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_POINTS );

    TextureID  aRViews[ 1 ] = { g_pParticlePosVeloRV0 };
    IRenderer::GetRendererInstance()->VSSetShaderResources( 0, 1, aRViews );

    
    IRenderer::GetRendererInstance()->Map( g_pcbGS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
    CB_GS* pCBGS = ( CB_GS* )MappedResource.pData; 
    D3DXFROMWINEMatrixMultiply( &pCBGS->m_WorldViewProj, &mView, &mProj );   
    D3DXFROMWINEMatrixInverse( &pCBGS->m_InvView, 0, &mView );
    IRenderer::GetRendererInstance()->Unmap( g_pcbGS, 0 );
    IRenderer::GetRendererInstance()->GSSetConstantBuffers( 0, 1, &g_pcbGS );

    IRenderer::GetRendererInstance()->PSSetShaderResources( 0, 1, &g_pParticleTexRV );
    IRenderer::GetRendererInstance()->PSSetSamplers( 0, 1, &g_pSampleStateLinear );

    IRenderer::GetRendererInstance()->OMSetBlendState( g_pBlendingStateParticle, D3DXFROMWINEVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF  );
    IRenderer::GetRendererInstance()->OMSetDepthStencilState( g_pDepthStencilState, 0 );

    IRenderer::GetRendererInstance()->Draw( MAX_PARTICLES, 0 );

    TextureID  ppSRVNULL[1] = { 0 };
    IRenderer::GetRendererInstance()->VSSetShaderResources( 0, 1, ppSRVNULL );
    IRenderer::GetRendererInstance()->PSSetShaderResources( 0, 1, ppSRVNULL );

    /*exBufferID  ppBufNULL[1] = { 0 };
    IRenderer::GetRendererInstance()->GSSetConstantBuffers( 0, 1, ppBufNULL );*/

    IRenderer::GetRendererInstance()->GSSetShader( 0, 0, 0 );
    IRenderer::GetRendererInstance()->OMSetBlendState( pBlendState0, &BlendFactor0.x, SampleMask0 ); //////////////////////SAFE_RELEASE(pBlendState0);
    IRenderer::GetRendererInstance()->OMSetDepthStencilState( pDepthStencilState0, StencilRef0 ); //////////////////////SAFE_RELEASE(pDepthStencilState0);

    return true;
}

void OnFrameRender( double fTime,
                                 float fElapsedTime )
{
    IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));

    D3DXFROMWINEMATRIX mView;
    D3DXFROMWINEMATRIX mProj;

    // Get the projection & view matrix from the camera class
    mProj = *g_Camera.GetProjMatrix();
    mView = *g_Camera.GetViewMatrix();

    // Render the particles
    RenderParticles( mView, mProj );

    // The following could be used to output fps stats into debug output window,
    // which is useful because you can then turn off all UI rendering as they cloud performance
    /*static DWORD dwTimefirst = GetTickCount();
    if ( GetTickCount() - dwTimefirst > 5000 )
    {    
        OutputDebugString( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
        OutputDebugString( "\n" );
        dwTimefirst = GetTickCount();
    }*/
}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnCreateDevice 
//--------------------------------------------------------------------------------------
void OnDestroyDevice( )
{

    //////////////////////SAFE_RELEASE( g_pParticleBuffer ); 
    //////////////////////SAFE_RELEASE( g_pParticleVertexLayout );

    //////////////////////SAFE_RELEASE( g_pParticlePosVelo0 );
    //////////////////////SAFE_RELEASE( g_pParticlePosVelo1 );
    //////////////////////SAFE_RELEASE( g_pParticlePosVeloRV0 );
    //////////////////////SAFE_RELEASE( g_pParticlePosVeloRV1 );
    //////////////////////SAFE_RELEASE( g_pParticlePosVeloUAV0 );
    //////////////////////SAFE_RELEASE( g_pParticlePosVeloUAV1 );

    //////////////////////SAFE_RELEASE( g_pcbGS );
    //////////////////////SAFE_RELEASE( g_pcbCS );

    //////////////////////SAFE_RELEASE( g_pParticleTexRV );

    //////////////////////SAFE_RELEASE( g_pRenderParticlesVS );
    //////////////////////SAFE_RELEASE( g_pRenderParticlesGS );
    //////////////////////SAFE_RELEASE( g_pRenderParticlesPS );
    //////////////////////SAFE_RELEASE( g_pCalcCS );
    //////////////////////SAFE_RELEASE( g_pSampleStateLinear );
    //////////////////////SAFE_RELEASE( g_pBlendingStateParticle );
    //////////////////////SAFE_RELEASE( g_pDepthStencilState );
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
	IRenderer* r=IRenderer::GetRendererInstance("NBodyGravityCS11");
	//if(std::string(r->getString())!="D3D11") stx_exit(11);
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

