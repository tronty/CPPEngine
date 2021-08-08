/*
 * TAR::Engine - Free C++ 3D engine ;
 * Copyright (C) 2007 Tommi Roenty ;
 * This program is free software;
 * you can redistribute it and/or ;
 * modify it under the terms of the GNU General Public License ;
 * as published by the Free Software Foundation;
 * either version 2 ;
 * of the License, or (at your option) any later version. ;
 * This program is distributed in the hope that it will be useful, ;
 * but WITHOUT ANY WARRANTY;
 * without even the implied warranty of ;
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the ;
 * GNU General Public License for more details. ;
 * You should have received a copy of the GNU General Public License ;
 * along with this program;
 * if not, write to the Free Software ;
 * Foundation, Inc., ;
 * 59 Temple Place - Suite 330, ;
 * Boston, MA 02111-1307, USA. ;
 * E-mail : tommi.ronty@iki.fi ;
 */
#include <vector>
#include <CommonD3DGL/CommonD3D9.h>
#include <CommonD3DGL/Image.h>
#include <CommonD3DGL/Camera.h>
#include <sdl_image.h>

#ifdef _MSC_VER
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "sdl.lib")
#pragma comment(lib, "SDL_image.lib")
#pragma comment(lib, "opengl32.lib")

#if __D3DGL__
#define AUTODEPTHSTENCILFORMAT D3DFMT_D32
#pragma comment(lib, "d3d9gl.lib")
#pragma comment(lib, "d3dx9gl.lib")
D3DXMACRO defines[5] =
    {
	{ "__HLSL__", "" },
	{ "__D3DGL__", "1" },
	{ "__D3DDX__", "0" },
	{ "__D3D__", "0" },
	{ NULL, NULL }
    };
#endif
#if __D3DDX__
#define AUTODEPTHSTENCILFORMAT D3DFMT_D16
#pragma comment(lib, "d3d9dx.lib")
#pragma comment(lib, "d3dx9dx.lib")
D3DXMACRO defines[5] =
    {
	{ "__HLSL__", "" },
	{ "__D3DGL__", "0" },
	{ "__D3DDX__", "1" },
	{ "__D3D__", "0" },
	{ NULL, NULL }
    };
#endif
#if __D3D__
#define AUTODEPTHSTENCILFORMAT D3DFMT_D16
#ifdef _DEBUG
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9d.lib")
#if(DIRECT3D_VERSION > 0x0900)
   #if (D3DX_SDK_VERSION >= 42) //aug 2009 sdk and up there is no dxerr9 anymore
      //include <Dxerr.h>
      #pragma comment (lib,"DxErr.lib")
   #else
      //include <dxerr9.h>
      //define DXGetErrorString(hr) DXGetErrorString9(hr)
      //define DXGetErrorDescription(hr) DXGetErrorDescription9(hr)
      #pragma comment (lib,"Dxerr9.lib")
   #endif
#endif
#else
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#if(DIRECT3D_VERSION > 0x0900)
   #if (D3DX_SDK_VERSION >= 42) //aug 2009 sdk and up there is no dxerr9 anymore
      //include <Dxerr.h>
      #pragma comment (lib,"DxErr.lib")
   #else
      //include <dxerr9.h>
      //define DXGetErrorString(hr) DXGetErrorString9(hr)
      //define DXGetErrorDescription(hr) DXGetErrorDescription9(hr)
      #pragma comment (lib,"Dxerr9.lib")
   #endif
#endif
#endif
D3DXMACRO defines[5] =
    {
	{ "__HLSL__", "" },
	{ "__D3DGL__", "0" },
	{ "__D3DDX__", "0" },
	{ "__D3D__", "1" },
	{ NULL, NULL }
    };
#endif
#endif
HWND				g_hWnd		= NULL;
LPDIRECT3D9			g_pD3D		= NULL;
LPDIRECT3DDEVICE9	g_pd3dDevice	= NULL;
LPDIRECT3DSURFACE9 g_fbColor=NULL;
LPDIRECT3DSURFACE9 g_fbDepth=NULL;
LPD3DXEFFECT g_pQuadEffect=NULL;
D3DCAPS9 g_caps;
D3DSURFACE_DESC g_pBackBufferSurfaceDesc;
float g_fDistance = 4.5f;
D3DFORMAT g_fmt;
D3DFORMAT g_depthStencilFmt=AUTODEPTHSTENCILFORMAT;
LPD3DXFONT	  g_pd3dxFont=0;
LPD3DXFONT	  g_pd3dxSmallFont=0;
LPD3DXMESH g_pMesh=NULL;
#ifdef INCLUDECAMERA
Camera* g_pCamera=0;
#endif
D3DPRESENT_PARAMETERS g_d3dpp;
extern char title[256];

	// Time keeping stuff
	//DWORD g_dwStartTime=0;
	DWORD g_dwTime=0;
	// Performance stats
	DWORD g_dwFPS=0;
	DWORD g_dwLoopFrames=0;
	DWORD g_dwLoopCount=0;
	DWORD g_dwLastPerfTime=0;
	CHAR g_strStats[128];

static D3DFORMAT texFormats[] = {
	D3DFMT_UNKNOWN,
	D3DFMT_L8,
	D3DFMT_A8L8,
	D3DFMT_X8R8G8B8,
	D3DFMT_A8R8G8B8,

	D3DFMT_L16,
	D3DFMT_G16R16,
	D3DFMT_A16B16G16R16, // RGB16 will be expanded to RGBA16
	D3DFMT_A16B16G16R16,

	D3DFMT_R16F,
	D3DFMT_G16R16F,
	D3DFMT_A16B16G16R16F, // RGB16F will be expanded to RGBA16F
	D3DFMT_A16B16G16R16F,

	D3DFMT_R32F,
	D3DFMT_G32R32F,
	D3DFMT_A32B32G32R32F, // RGB32F will be expanded to RGBA32F
	D3DFMT_A32B32G32R32F,

	D3DFMT_R16F, // I and IA formats uploaded as R and RG
	D3DFMT_G16R16F,
	D3DFMT_R32F,
	D3DFMT_G32R32F,

	D3DFMT_V8U8,
	D3DFMT_Q8W8V8U8,
	D3DFMT_V16U16,
	D3DFMT_Q16W16V16U16,

	(D3DFORMAT) MAKEFOURCC('D', 'F', '1', '6'), // Depth16
	(D3DFORMAT) MAKEFOURCC('D', 'F', '2', '4'), // Depth24

	D3DFMT_R3G3B2,
	D3DFMT_A2B10G10R10,
	D3DFMT_L6V5U5,
	D3DFMT_A2W10V10U10,

	D3DFMT_DXT1,
	D3DFMT_DXT3,
	D3DFMT_DXT5,
	(D3DFORMAT) MAKEFOURCC('A', 'T', 'I', '1'), // ATI1N
	(D3DFORMAT) MAKEFOURCC('A', 'T', 'I', '2'), // ATI2N
};

enum TextureFilter {
	// Single image filters
	FILTER_POINT,
	FILTER_LINEAR,

	// Mipmapped image filters
	FILTER_BILINEAR,
	FILTER_TRILINEAR,

	// Mipmapped image filters with anisotropic
	FILTER_BILINEAR_ANISO,
	FILTER_TRILINEAR_ANISO,
};

std::vector<TextureFilterParams> g_SamplerStates;

struct TextureD3DGL {
	LPDIRECT3DBASETEXTURE9 texture;
	int filter;
	LPDIRECT3DSURFACE9 *rtSurfaces;
	int nRTSurfaces;
	int width, height;
	D3DFORMAT format;
	DWORD usage;
	D3DPOOL pool;
	unsigned int flags;
	TextureD3DGL(LPDIRECT3DBASETEXTURE9 atexture=0,
	int afilter=-1,
	LPDIRECT3DSURFACE9 *artSurfaces=0,
	int anRTSurfaces=0,
	int awidth=0, int aheight=0,
	D3DFORMAT aformat=D3DFMT_X8R8G8B8,
	DWORD ausage=0,
	D3DPOOL apool=D3DPOOL_DEFAULT,
	unsigned int aflags=0):texture(atexture),filter(afilter),rtSurfaces(artSurfaces),
	nRTSurfaces(anRTSurfaces),
	width(awidth),height(aheight),format(aformat),usage(ausage),
	pool(apool),flags(aflags){}
	/*
	TextureD3DGL():
		  texture(0),
		  filter(-1),
		  rtSurfaces(0),
		  nRTSurfaces(0),
		  width(0), height(0),
		  format(D3DFMT_X8R8G8B8),
		  usage(0),
		  pool(D3DPOOL_DEFAULT),
		  flags(0){}*/
	virtual ~TextureD3DGL(){}
};

std::vector<TextureD3DGL> g_Textures;

int inscene=0;
int g_nrt=1;
void ResetRTD(LPDIRECT3DSURFACE9 d=0, int nrt=0, LPDIRECT3DSURFACE9* rt=0, D3DVIEWPORT9* vp=0)
{
    if(vp)
    {
      V(g_pd3dDevice->SetViewport( vp ));
    }
    if(nrt>0)
    {
	for(unsigned int i=0;i<nrt;i++)
	{
	  V(g_pd3dDevice->SetRenderTarget(i,rt[i]));
	}
	if(g_nrt>nrt)
	 for(unsigned int i=nrt;i<g_nrt;i++)
	{
	  V(g_pd3dDevice->SetRenderTarget(i,NULL));
	}
	g_nrt=nrt;
    }
    if(1)//(d)
    {
	V(g_pd3dDevice->SetDepthStencilSurface(d));
    }
}
void ChangeScene(LPDIRECT3DSURFACE9 d, int nrt, LPDIRECT3DSURFACE9* rt, D3DVIEWPORT9* vp)
{
  if(inscene==0)
  {
  ResetRTD(d, nrt, rt, vp);
  V(g_pd3dDevice->BeginScene());
  inscene=1;
  }
  else if(inscene==1)
  {
  V(g_pd3dDevice->EndScene());
  ResetRTD(d, nrt, rt, vp);
  V(g_pd3dDevice->BeginScene());
  }

}
void ChangeScene()
{
  if(inscene==0)
  {
	LPDIRECT3DSURFACE9 rt[1];
    rt[0]=g_fbColor;
	D3DVIEWPORT9 vp;
    vp.Width = width;
    vp.Height = height;
    vp.MinZ = 0;
    vp.MaxZ = 1;
    vp.X = 0;
    vp.Y = 0;
	ResetRTD(g_fbDepth,1, rt, &vp);
	V(g_pd3dDevice->BeginScene());
	inscene=1;
  }
  else if(inscene==1)
  {
  V(g_pd3dDevice->EndScene());
	LPDIRECT3DSURFACE9 rt[1];
    rt[0]=g_fbColor;
	D3DVIEWPORT9 vp;
    vp.Width = width;
    vp.Height = height;
    vp.MinZ = 0;
    vp.MaxZ = 1;
    vp.X = 0;
    vp.Y = 0;
	ResetRTD(g_fbDepth,1, rt, &vp);
  V(g_pd3dDevice->BeginScene());
  }

}
LPD3DXEFFECT g_pCurrentEffect=0;
INT g_pInPass=0;
void ChangeShader(LPD3DXEFFECT pEffect, WORD pass)
{
	static WORD currentPass=0;

	if((g_pCurrentEffect==pEffect)&&(currentPass!=pass))
	{
		V(g_pCurrentEffect->EndPass());
		V(g_pCurrentEffect->BeginPass(pass));
		currentPass=pass;
		g_pInPass=1;
		return;
	} else

	if(g_pCurrentEffect!=pEffect)
	{
		currentPass=pass;
	if(g_pCurrentEffect!=0)
	{
		V(g_pCurrentEffect->EndPass());
	//    }
		V(g_pCurrentEffect->End());
		g_pInPass=0;
	}
	g_pCurrentEffect=pEffect;

	if(g_pInPass==0)
	{
	UINT uPasses;
	V(g_pCurrentEffect->SetTechnique("Technique0"));
    V(g_pCurrentEffect->Begin( &uPasses, 0 ));
	//LOG_PRINT("uPasses=%d\n",uPasses);
	UINT uPass = currentPass;
//    for( uPass = 0; uPass < uPasses; ++uPass )
//    {
	V(g_pCurrentEffect->BeginPass( uPass ));
	}
	}
}


void Present()
{
  if(inscene==0)
  {
   DBG_HALT;
  }
  if(g_pInPass!=0)
  {
		V(g_pCurrentEffect->EndPass());
		V(g_pCurrentEffect->End());
		g_pInPass=0;
		g_pCurrentEffect=0;
  }
  else if(inscene==1)
  {
	DrawFPS();
//	DrawYawPitch();
  V(g_pd3dDevice->EndScene());
	V(g_pd3dDevice->Present( NULL, NULL, NULL, NULL ));
	SaveScreenShot();
//	DBG_HALT;
  //ResetRTD(nrt, rt, d, vp);
  //V(g_pd3dDevice->BeginScene());
  inscene=0;
  }
}




void InitD3D9(void)
{
  if((g_pD3D!=NULL)&&(g_pd3dDevice!=NULL))return;
  CHECK_HEAP0;
  bool doNotInit=((g_fbColor!=0)||
  (g_fbDepth!=0)||
  (g_pd3dxFont!=0)||
	  (g_pd3dxSmallFont!=0)||
	(g_pd3dDevice!=0)||
	(g_pD3D!=0));

  if(doNotInit)return;

  SAFE_RELEASE(g_fbColor);
  SAFE_RELEASE(g_fbDepth);
  SAFE_RELEASE(g_pd3dxFont);
	  SAFE_RELEASE(g_pd3dxSmallFont);
	SAFE_RELEASE(g_pd3dDevice);
	SAFE_RELEASE(g_pD3D);

    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWMInfo(&info);
	 SDL_WM_SetIcon(IMG_Load("mainicon.bmp"), NULL);
	std::string filename_icon = "mainicon.bmp";

	     if( 1)//File_Exists( filename_icon ) )

	     {

		   SDL_Surface *icon = IMG_Load( filename_icon.c_str() );

		   SDL_WM_SetIcon( icon, NULL );

		   SDL_FreeSurface( icon );

	     }

	     else

	     {

		   LOG_PRINT( "Warning : Window icon %s does not exist\n", filename_icon.c_str() );

	     }
#if defined(_MSC_VER) || defined(__MINGW32__)
	g_hWnd = info.window;
#else
	g_hWnd = 0;
#endif
/*else
    dpy=info.info.x11.display;
    win=info.info.x11.window;
#endif*/
	//d3dInit();
	LOG_FNLN;
    g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

//	LOG("%s:%d,g_pD3D=%d,%x",__FUNCTION__,__LINE__, g_pD3D, g_pD3D);


	LOG_FNLN;

	V(g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &g_caps));
    D3DDISPLAYMODE d3ddm;

    V(g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ));

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );

	//d3dpp.hDeviceWindow=g_hWnd;
    d3dpp.Windowed		 = TRUE;
    d3dpp.SwapEffect		 = D3DSWAPEFFECT_COPY;
    d3dpp.BackBufferFormat	 = d3ddm.Format;
	g_fmt= d3ddm.Format;
	//d3dpp.BackBufferWidth    = 640;
	//d3dpp.BackBufferHeight   = 480;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = AUTODEPTHSTENCILFORMAT;
    d3dpp.PresentationInterval	 = D3DPRESENT_INTERVAL_IMMEDIATE;

	DWORD behaviorFlags=0;
if( g_caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
   behaviorFlags |= /*D3DCREATE_PUREDEVICE |*/ D3DCREATE_HARDWARE_VERTEXPROCESSING;
else
   behaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	g_d3dpp=d3dpp;
    V(g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
			  behaviorFlags,//D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			  &d3dpp, &g_pd3dDevice ));
/*
			HRESULT hr=D3DXCreateBox(g_pd3dDevice
				, -1.0f,
				 -1.0f,
				 -1.0f,
				0,0);
		//LOG_PRINT("D3DXCreateBox=%d\n",hr);
		//LOG_PRINT("D3D_OK=%d\n",D3D_OK);
*/
//	LOG("g_pd3dDevice=%d\n",g_pd3dDevice);
		LOG_PRINT("DirectX Versions:\n");
  /*
  LOG_PRINT("VERSION=%d\n",D3D11_MAJOR_VERSION);
  LOG_PRINT("VERSION=%d\n",D3D11_MINOR_VERSION);
  LOG_PRINT("VERSION=%d\n",D3D11_SHADER_MAJOR_VERSION);
  LOG_PRINT("VERSION=%d\n",D3D11_SHADER_MINOR_VERSION);
  LOG_PRINT("VERSION=%d\n",D3D11_SPEC_VERSION);
  LOG_PRINT("VERSION=%d\n",D3D11_SDK_VERSION);

  LOG_PRINT("D3D10_SHADER_MAJOR_VERSION=%d\n",D3D10_SHADER_MAJOR_VERSION);
  LOG_PRINT("D3D10_SHADER_MINOR_VERSION=%d\n",D3D10_SHADER_MINOR_VERSION);
  LOG_PRINT("D3D_MAJOR_VERSION=%d\n",D3D_MAJOR_VERSION);
  LOG_PRINT("D3D_MINOR_VERSION=%d\n",D3D_MINOR_VERSION);
  LOG_PRINT("D3D_SPEC_VERSION=%d\n",D3D_SPEC_VERSION);
  LOG_PRINT("D3D10_SDK_VERSION=%d\n",D3D10_SDK_VERSION);
    */
  LOG_PRINT("DIRECT3D_VERSION=%x\n",DIRECT3D_VERSION);
#ifdef D3D_DEBUG_INFO
  LOG_PRINT("D3D_SDK_VERSION=%x\n",D3D_SDK_VERSION);
  LOG_PRINT("D3D9b_SDK_VERSION=%x\n",D3D9b_SDK_VERSION);
#else
  LOG_PRINT("D3D_SDK_VERSION=%d\n",D3D_SDK_VERSION);
  LOG_PRINT("D3D9b_SDK_VERSION=%d\n",D3D9b_SDK_VERSION);
#endif
//  LOG_PRINT("D3DENUM_NO_DRIVERVERSION=%x\n",D3DENUM_NO_DRIVERVERSION);

  V(g_pd3dDevice->GetRenderTarget(0, &g_fbColor));
  V(g_pd3dDevice->GetDepthStencilSurface(&g_fbDepth));

		HDC hDC;
	int nHeight;
	int nPointSize = 18;

	hDC = GetDC( NULL );

	nHeight = -( MulDiv( nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72 ) );

	ReleaseDC( NULL, hDC );
		// Create a font for statistics and help output
	//HRESULT hr =
			V(D3DXCreateFont( g_pd3dDevice, nHeight, 0, FW_BOLD, 0, FALSE,
				 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
				 DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"),
				 &g_pd3dxFont ));
			V(D3DXCreateFont( g_pd3dDevice, nHeight/2, 0, FW_BOLD, 0, FALSE,
				 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
				 DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"),
				 &g_pd3dxSmallFont ));

LPDIRECT3DSURFACE9 pBackBuffer = NULL;
    V(g_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ));
    pBackBuffer->GetDesc( &g_pBackBufferSurfaceDesc );
    pBackBuffer->Release();
    CHECK_HEAP0;

	D3DVIEWPORT9 vp;
    vp.Width = width;
    vp.Height = height;
    vp.MinZ = 0;
    vp.MaxZ = 1;
    vp.X = 0;
    vp.Y = 0;
    V(g_pd3dDevice->SetViewport(& vp ));
    LOG_PRINT("VP %dx%d\n",vp.Width,vp.Height);
}

void writeData(LPDIRECT3DVERTEXBUFFER9 mVertexBuffer,
	       unsigned int z,
	       unsigned int sib,
	       void* mVertices,
	       bool t)
{
 FillBuffer(mVertexBuffer,


	       mVertices
	       );
}
void FillBuffer(LPDIRECT3DVERTEXBUFFER9 mVertexBuffer,
	       void* mVertices)
{
    //sib=???mVertexBuffer->getSizeInBytes();

  D3DVERTEXBUFFER_DESC vbdesc;
  mVertexBuffer->GetDesc (&vbdesc);
  unsigned int sib2=vbdesc.Size;

    void *pVertices = NULL;
    mVertexBuffer->Lock( 0, sib2, (void**)&pVertices, 0 );
    memcpy( pVertices, mVertices, sib2 );
    mVertexBuffer->Unlock();
}
void writeData(LPDIRECT3DINDEXBUFFER9 mIndexBuffer,
	       unsigned int z,
	       unsigned int sib,
	       void* indexbuffer,
	       bool t)
{
	       FillBuffer(mIndexBuffer,


	       indexbuffer
	       );
}
void FillBuffer(LPDIRECT3DINDEXBUFFER9 mIndexBuffer,
	       void* indexbuffer)
{
    //sib=???mIndexBuffer->getSizeInBytes();

  D3DINDEXBUFFER_DESC ibdesc;
  mIndexBuffer->GetDesc (&ibdesc);
  unsigned int sib2=ibdesc.Size;

    void *pIndices = NULL;
	mIndexBuffer->Lock( 0, sib2, (void**)&pIndices, 0 );
    memcpy( pIndices, indexbuffer, sib2 );
    mIndexBuffer->Unlock();
}

void CreateBuffer(UINT N, UINT S,
	LPDIRECT3DVERTEXBUFFER9* ppVertexBuffer
		,DWORD U
	,D3DPOOL P
	,void* mVertices
 )
{
	g_pd3dDevice->CreateVertexBuffer(
		N*S,
		U,
		0,
		P,
		ppVertexBuffer,
		NULL );
       if(mVertices)
       FillBuffer(*ppVertexBuffer,
	       mVertices);
}
void CreateBuffer(UINT N, UINT S,
	LPDIRECT3DINDEXBUFFER9* ppIndexBuffer
		,DWORD U
	,D3DPOOL P
	,void* indexbuffer
)
{
  D3DFORMAT F=D3DFMT_INDEX32;
  if(S=2)
	 F=D3DFMT_INDEX16;
  else if(S=4)
	 F=D3DFMT_INDEX32;
  else DBG_HALT;

    g_pd3dDevice->CreateIndexBuffer(
  N*S
		, U,
		F,
		P,
		ppIndexBuffer,
		NULL );
  if(indexbuffer)
       FillBuffer(*ppIndexBuffer,
	       indexbuffer);
}

LPD3DXEFFECT g_pEffect=0;
D3DXHANDLE timeNVIDIA,TIMENVIDIA,WorldViewProjectionNVIDIA,WorldNVIDIA,ViewNVIDIA,ViewInverseNVIDIA,VIEWPORTPIXELSIZENVIDIA;
void LoadEffectParamsNVIDIA()
{
	timeNVIDIA = g_pEffect->GetParameterBySemantic( NULL, "Time" );
	TIMENVIDIA = g_pEffect->GetParameterBySemantic( NULL, "TIME" );
	WorldViewProjectionNVIDIA = g_pEffect->GetParameterBySemantic( NULL, "WorldViewProjection" );
	WorldNVIDIA = g_pEffect->GetParameterBySemantic( NULL, "World" );
	ViewNVIDIA = g_pEffect->GetParameterBySemantic( NULL, "View" );
	ViewInverseNVIDIA = g_pEffect->GetParameterBySemantic( NULL, "ViewInverse" );
	VIEWPORTPIXELSIZENVIDIA = g_pEffect->GetParameterBySemantic( NULL, "VIEWPORTPIXELSIZE" );
	/*
	LPDIRECT3DVOLUMETEXTURE9	g_pNoiseTexture = NULL;
	LPDIRECT3DTEXTURE9	  g_pFlameTexture = NULL;
	V(D3DXCreateVolumeTextureFromFile(g_pd3dDevice,"noiseL8_32x32x32.dds",&g_pNoiseTexture));
	V(D3DXCreateTextureFromFile(g_pd3dDevice,"flame.png",&g_pFlameTexture));
	V( g_pEffect->SetTexture("noiseTexture",g_pNoiseTexture));
	V( g_pEffect->SetTexture("flameTexture",g_pFlameTexture));
	*/
	// Apply the technique contained in the effect
	D3DXHANDLE hTech;
	g_pEffect->FindNextValidTechnique( NULL, &hTech );
	g_pEffect->SetTechnique( hTech );
}
void SetEffectParamsNVIDIA()
{
	float tim = (float)GetTickCount()/1000.0f;
	g_pEffect->SetFloat( timeNVIDIA, tim );
	g_pEffect->SetFloat( TIMENVIDIA, tim );
	D3DXMATRIX wor,vie,pro,vieI;
	V(g_pd3dDevice->GetTransform( D3DTS_WORLD, &wor ));
	V(g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &pro ));
	V(g_pd3dDevice->GetTransform( D3DTS_VIEW, &vie ));
	D3DXMatrixInverse( &vieI, NULL, &vie );
	g_pEffect->SetMatrix( WorldViewProjectionNVIDIA, &(wor*vie*pro) );
	g_pEffect->SetMatrix( WorldNVIDIA, &wor );
	g_pEffect->SetMatrix( ViewNVIDIA, &vie );
	g_pEffect->SetMatrix( ViewInverseNVIDIA, &vieI );
	D3DXVECTOR2 vp(width, height);//0,0);//1,1);//???
	g_pEffect->SetValue( VIEWPORTPIXELSIZENVIDIA, vp, sizeof(D3DXVECTOR2));
}
D3DXHANDLE timeATI,WorldViewProjectionATI,ViewProjectionATI,ViewPositionATI,ViewATI;
void LoadEffectParamsATI()
{
	timeATI = g_pEffect->GetParameterBySemantic( NULL, "Time0_X" );
	WorldViewProjectionATI = g_pEffect->GetParameterBySemantic( NULL, "WorldViewProjection" );
	ViewProjectionATI = g_pEffect->GetParameterBySemantic( NULL, "ViewProjection" );
	ViewPositionATI = g_pEffect->GetParameterBySemantic( NULL, "ViewPosition" );
	ViewATI = g_pEffect->GetParameterBySemantic( NULL, "View" );
/*
	LPDIRECT3DVOLUMETEXTURE9	g_pNoiseTexture = NULL;
	LPDIRECT3DTEXTURE9	  g_pFlameTexture = NULL;
	V(D3DXCreateVolumeTextureFromFile(g_pd3dDevice,"noiseL8_32x32x32.dds",&g_pNoiseTexture));
	V(D3DXCreateTextureFromFile(g_pd3dDevice,"flame.png",&g_pFlameTexture));
	V( g_pEffect->SetTexture("noiseTexture",g_pNoiseTexture));
	V( g_pEffect->SetTexture("flameTexture",g_pFlameTexture));
*/
	// Apply the technique contained in the effect
	D3DXHANDLE hTech;
	g_pEffect->FindNextValidTechnique( NULL, &hTech );
	g_pEffect->SetTechnique( hTech );
}
void SetEffectParamsATI()
{
	float tim = (float)GetTickCount()/1000.0f;
	g_pEffect->SetFloat( timeATI, tim );
	D3DXMATRIX wor,vie,pro;
	V(g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &pro ));
	V(g_pd3dDevice->GetTransform( D3DTS_VIEW, &vie ));
	V(g_pd3dDevice->GetTransform( D3DTS_WORLD, &wor ));
	g_pEffect->SetMatrix( ViewProjectionATI, &(vie*pro) );
	g_pEffect->SetMatrix( WorldViewProjectionATI, &(wor*vie*pro) );
	g_pEffect->SetMatrix( ViewATI, &vie );
#ifdef INCLUDECAMERA
	g_pEffect->SetVector( ViewPositionATI, &D3DXVECTOR4(g_Camera._pos.x,g_Camera._pos.y,g_Camera._pos.z,1) );
#endif
}
void CreateMesh(int aQuad,D3DVERTEXELEMENT9* g_pMeshDecl)
{

	if(g_pMesh) g_pMesh->Release();
	ID3DXMesh * tmp = NULL;
	if(aQuad==1)
	{
		struct QuadVertex
		{
		    float x, y, z;
		};
		QuadVertex g_quadVertices[] =
		{
			{-1.0f, 1.0f, 0.0f},
			{ 1.0f, 1.0f, 0.0f},
			{-1.0f,-1.0f, 0.0f},
			{ 1.0f,-1.0f, 0.0f}
		};
		//HRESULT hr=
			V(D3DXCreateMeshFVF(2,4, D3DXMESH_SYSTEMMEM | D3DXMESH_32BIT, D3DFVF_XYZ, g_pd3dDevice, &tmp ));
		QuadVertex* VertexPtr=0;
		tmp->LockVertexBuffer( 0, (void**)&VertexPtr );
		VertexPtr[0]=g_quadVertices[0];
		VertexPtr[1]=g_quadVertices[1];
		VertexPtr[2]=g_quadVertices[2];
		VertexPtr[3]=g_quadVertices[3];
		tmp->UnlockVertexBuffer();
		DWORD* IndexPtr=NULL;
		tmp->LockIndexBuffer( 0, (void**)&IndexPtr );
		IndexPtr[0]=2;
		IndexPtr[1]=3;
		IndexPtr[2]=1;
		IndexPtr[3]=2;
		IndexPtr[4]=0;
		IndexPtr[5]=1;
	tmp->UnlockIndexBuffer();
	}
	else if(aQuad==2)
	{
		V(D3DXCreateBox(g_pd3dDevice, 1.0f, 1.0f, 1.0f, &tmp, /*LPV(D3DXBUFFER* ppAdjacency*/0));
	}
	else if(aQuad==3)
	{
		V(D3DXCreateSphere(g_pd3dDevice,1.0f,5,5,&tmp,0));
	}
	else
	{
		V(D3DXCreateBox(g_pd3dDevice, 1.0f, 1.0f, 1.0f, &tmp, /*LPV(D3DXBUFFER* ppAdjacency*/0));
	}

	tmp->CloneMesh(0, g_pMeshDecl, g_pd3dDevice, &g_pMesh);

	tmp->Release();
}
void AddShaders(
    const char* nameVS,
    const char* namePS,
	LPDIRECT3DVERTEXSHADER9      VS,
	LPD3DXCONSTANTTABLE	     CTVS,
	LPDIRECT3DPIXELSHADER9		 PS,
	LPD3DXCONSTANTTABLE		 CTPS,
	D3DXMACRO* defines
	)
	{
	HRESULT hr;
    LPD3DXBUFFER pCode;
    DWORD dwShaderFlags = 0;
	LPD3DXBUFFER pBufferErrors = NULL;
     #if defined( DEBUG ) || defined( _DEBUG )
    // Set the V(D3DXSHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows
    // the shaders to be optimized and to run exactly the way they will run in
    // the release configuration of this program.
    dwShaderFlags |= D3DXSHADER_DEBUG;
    #endif
    #ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_SKIPOPTIMIZATION|D3DXSHADER_DEBUG;
    #endif
    #ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_SKIPOPTIMIZATION|D3DXSHADER_DEBUG;
    #endif
/*
D3DXMACRO defines[2] =
    {
	{ "__HLSL__", "" },
	{ NULL, NULL }
    };
*/
    hr=D3DXCompileShaderFromFile( nameVS, defines,NULL,  "main",
				    "vs_2_0", dwShaderFlags, &pCode,
				    &pBufferErrors, &CTVS );
    if( FAILED(hr) )
	{
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		//LOG_PRINT( "Vertex ShaderD3DGL Compile Error\n");
		 //LOG_PRINT( (const char*)pCompilErrors);
	}
    V(g_pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(),
				      &VS ));
    pCode->Release();
    hr = D3DXCompileShaderFromFile( namePS, defines,NULL,  "main",
				    "ps_2_0", dwShaderFlags, &pCode,
				    &pBufferErrors, &CTPS );
    if( FAILED(hr) )
	{
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		//LOG_PRINT("Pixel ShaderD3DGL Compile Error\n");
		//LOG_PRINT( (const char*)pCompilErrors);
	}
    V(g_pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(),
				      &PS ));
    pCode->Release();
}
HRESULT LoadEffectFromString(const char* txtFX, LPD3DXEFFECT* pEffect)
{
	//LOG_PRINT("Loading Effect %s:\n",nameFX,*pEffect);
	HRESULT hr;
	LPD3DXBUFFER pBufferErrors = NULL;
/*
V(D3DXMACRO defines[2] =
    {
	{ "__HLSL__", "" },
	{ NULL, NULL }
    };
*/

	hr = D3DXCreateEffect( g_pd3dDevice,
					   txtFX,
					   strlen(txtFX),
					   defines,
					   NULL,
					   0,
					   NULL,
					   pEffect,
					   &pBufferErrors );

	if( FAILED(hr) )
	{
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		LOG_PRINT( "Fx Compile Error\n");
		LOG_PRINT( (const char*)pCompilErrors);
		DBG_HALT(-1);
	}
	else
	{
		//LOG_PRINT("Effect %s\n %d OK\n",txtFX,*pEffect);
	}
	return hr;
}
HRESULT LoadEffect(const char* nameFX, LPD3DXEFFECT* pEffect)
{
	//LOG_PRINT("Loading Effect %s:\n",nameFX,*pEffect);
	HRESULT hr;
	LPD3DXBUFFER pBufferErrors = NULL;
/*
V(D3DXMACRO defines[2] =
    {
	{ "__HLSL__", "" },
	{ NULL, NULL }
    };
*/

	hr = D3DXCreateEffectFromFile( g_pd3dDevice,
					   nameFX,
					   defines,
					   NULL,
					   0,
					   NULL,
					   pEffect,
					   &pBufferErrors );

	if( FAILED(hr) )
	{
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		LOG_PRINT( "Fx Compile Error\n");
		LOG_PRINT( (const char*)pCompilErrors);
		DBG_HALT(-1);
	}
	else
	{
		LOG_PRINT("Effect %s %d OK\n",nameFX,*pEffect);
	}
	return hr;
}
void RGBToRGBA(BYTE** v, DWORD n, D3DCOLOR cc)
{
			DWORD cr=D3DCOLOR_R(cc);
			DWORD cg=D3DCOLOR_G(cc);
			DWORD cb=D3DCOLOR_B(cc);
  BYTE* vnew=new BYTE[n*4];
  for(DWORD i=0;i<n;i++)
  {
    BYTE a=0;
    if(((*v)[i*3+0]!=cr)||((*v)[i*3+1]!=cg)||((*v)[i*3+2]!=cb))
    {
    vnew[i*4+0]=255;
    vnew[i*4+1]=(*v)[i*3+0];
    vnew[i*4+2]=(*v)[i*3+1];
    vnew[i*4+3]=(*v)[i*3+2];
    }
    else
    {
    vnew[i*4+0]=0;
    vnew[i*4+1]=0;
    vnew[i*4+2]=0;
    vnew[i*4+3]=0;
    }
  }
  delete[] *v;
  *v=vnew;
}
void RGBAToRGB(BYTE** v, DWORD n, D3DCOLOR cc)
{
			DWORD cr=D3DCOLOR_R(cc);
			DWORD cg=D3DCOLOR_G(cc);
			DWORD cb=D3DCOLOR_B(cc);
  BYTE* vnew=new BYTE[n*3];
  for(DWORD i=0;i<n;i++)
  {
    BYTE a=0;
    if(((*v)[i*3+1]!=cr)||((*v)[i*3+2]!=cg)||((*v)[i*3+3]!=cb))
    {
    vnew[i*3+0]=(*v)[i*4+1];
    vnew[i*3+1]=(*v)[i*4+2];
    vnew[i*3+2]=(*v)[i*4+3];
    }
    else
    {
    vnew[i*3+0]=cr;
    vnew[i*3+1]=cg;
    vnew[i*3+2]=cb;
    }
  }
  delete[] *v;
  *v=vnew;
}

#ifdef INCLUDEIMAGE
void ImageToRGBA8(ImageD3DGL& img)
{
	img.decompress();
	FORMAT format = img.getFormat();
	if(format!=FORMAT_RGB8)
	   img.convert(FORMAT_RGB8);
	format = img.getFormat();
	if (isPlainFormat(format) && getChannelCount(format) == 3)
	{
		img.addChannel(1);
	}
}
void ImageToI8(ImageD3DGL& img)
{
	img.decompress();
	FORMAT format = img.getFormat();
	if(format!=FORMAT_I8)
	   img.convert(FORMAT_I8);
}

void LoadTexture(const ImageD3DGL &img0 ,LPDIRECT3DBASETEXTURE9* ret, bool atlas, FORMAT f, unsigned int w, unsigned int h, DWORD u)
{

	CHECK_HEAP0;
	DWORD ccc=D3DCOLOR_RGBA(0,0,0,0);
	if(atlas)
	{
	  ccc=D3DCOLOR_RGBA(0,0,0,255);
	  f=FORMAT_RGBA8;
	}
	ImageD3DGL img(img0);
	if((w!=0)||(h!=0)) img.resize(w,h,1);

	//CHECK_HEAP
	//ImageD3DGL img2=img0;*/
	//CHECK_HEAP
		//LOG_PRINT("110 pixels=%d\n",img0.getPixels());
		//LOG_PRINT("111 pixels=%d\n",img.getPixels());
		//LOG_PRINT("112 pixels=%d\n",img2.getPixels());

	    char buf[256];
	TextureD3DGL tex;
	memset(&tex, 0, sizeof(tex));


	// Turn a 2D texture formatted as a cross into a proper cubemap if requested
//	  if (img.is2D() && (flags & CUBEMAP)) img.crossToCube();


	// Generate mipmaps if they are missing, unless explicitly requested otherwise
	//if (img.getMipMapCount() <= 1 && hasMipMaps(filter)) img.generateMipMaps();
 /*

	// Turn a 2D texture into a normal map if requested
	if (img.is2D() && (flags & NORMALMAP)){
		FORMAT destFormat = GETFMT(flags);
		if (destFormat == FORMAT_NONE) destFormat = FORMAT_RGBA8;

		if (!img.toNormalMap(destFormat)){
			//LOG_PRINT("Couldn't generate normalmap");
			return 0;
		}
	}
   */

	// Not clamping to edge with cubemaps makes little sense
//	  if (img.isCube()) flags |= (CLAMP_S | CLAMP_T);


	FORMAT format = img.getFormat();
	/*if(format != f)
	{
	     img.decompress();
	      img.convert(f);
	      format = img.getFormat();
	}else*/
	if (isPlainFormat(format) && getChannelCount(format) == 3){
		img.addChannel(1);
		format = img.getFormat();
	}
	else if((!isPlainFormat(format)) && getChannelCount(format) == 3)
	{
	  DBG_HALT;
	}

	tex.format = texFormats[format];

		sprintf(buf,"tex.format=%d\n",tex.format);
		OutputDebugString(buf);


	tex.width  = img.getWidth();
	tex.height = img.getHeight();
	tex.pool   = D3DPOOL_MANAGED;

	// Create actual textures
	LPDIRECT3DTEXTURE9 texture;
	LPDIRECT3DCUBETEXTURE9 cubeMap;
	LPDIRECT3DVOLUMETEXTURE9 volumeMap;


	if (img.isCube()){


		OutputDebugString("CreateCubeTexture\n");

		V(g_pd3dDevice->CreateCubeTexture(tex.width, 1/*img.getMipMapCount()*/, u, tex.format, D3DPOOL_MANAGED, &cubeMap, NULL));

		*ret = cubeMap;


	} else if (img.is3D()){


		OutputDebugString("CreateVolumeTexture\n");

		V(g_pd3dDevice->CreateVolumeTexture(tex.width, tex.height, img.getDepth(), 1/*img.getMipMapCount()*/, u, tex.format, D3DPOOL_MANAGED, &volumeMap, NULL));

		*ret = volumeMap;


	} else {


		OutputDebugString("CreateTexture\n");

		V(g_pd3dDevice->CreateTexture(tex.width, tex.height, 1/*img.getMipMapCount()*/, u, tex.format, D3DPOOL_MANAGED, &texture, NULL));

//???		*ret = texture;

		//ret=(LPDIRECT3DBASETEXTURE9*)&texture;
		(*ret)=(LPDIRECT3DBASETEXTURE9)texture;

	}

	// D3D uses BGR byte order for 8bit formats
	if (format == FORMAT_RGBA8) img.swapChannels(0, 2);

	// Upload the texture image
	ubyte *src;
	int mipMapLevel = 0;
	while ((src = img.getPixels(mipMapLevel)) != NULL){
		int w = img.getWidth(mipMapLevel);
		int h = img.getHeight(mipMapLevel);
		int rows = img.getHeightInBlocks(mipMapLevel);
		int rowSize   = getImageSize(format, w, 1, 1, 1);
		int sliceSize = getImageSize(format, w, h, 1, 1);


		if (img.is3D()){


			int d = img.getDepth(mipMapLevel);

			D3DLOCKED_BOX box;
			if (volumeMap->LockBox(mipMapLevel, &box, NULL, 0) == D3D_OK){
				if (format == FORMAT_ATI1N || format == FORMAT_ATI2N){
					// There's an issue with the D3D runtime causing Pitch to be invalid
					// for ATI1N and ATI2N, so we'll just use plain memory copy in this case.
					memcpy(box.pBits, src, d * sliceSize);
				} else {
					for (int z = 0; z < d; z++){
						for (int y = 0; y < rows; y++){
							memcpy(((ubyte *) box.pBits) + z * box.SlicePitch + y * box.RowPitch, src + z * sliceSize + y * rowSize, rowSize);
						}
					}
				}
				volumeMap->UnlockBox(mipMapLevel);
			}


		} else if (img.isCube()){


			D3DLOCKED_RECT rect;
			for (int i = 0; i < 6; i++){
				if (cubeMap->LockRect((D3DCUBEMAP_FACES) i, mipMapLevel, &rect, NULL, 0) == D3D_OK){
					if (format == FORMAT_ATI1N || format == FORMAT_ATI2N){
						// There's an issue with the D3D runtime causing Pitch to be invalid
						// for ATI1N and ATI2N, so we'll just use plain memory copy in this case.
						memcpy(rect.pBits, src, sliceSize);
					} else {
						for (int y = 0; y < rows; y++){
							memcpy(((ubyte *) rect.pBits) + y * rect.Pitch, src + y * rowSize, rowSize);
						}
					}
					cubeMap->UnlockRect((D3DCUBEMAP_FACES) i, mipMapLevel);
				}
				src += sliceSize;
			}


		} else {


			D3DLOCKED_RECT rect;
			if (texture->LockRect(mipMapLevel, &rect, NULL, 0) == D3D_OK){
				if (format == FORMAT_ATI1N || format == FORMAT_ATI2N){
					// There's an issue with the D3D runtime causing Pitch to be invalid
					// for ATI1N and ATI2N, so we'll just use plain memory copy in this case.
					memcpy(rect.pBits, src, sliceSize);
				} else {
					for (int y = 0; y < rows; y++){
						memcpy(((ubyte *) rect.pBits) + y * rect.Pitch, src + y * rowSize, rowSize);
					}
				}
				texture->UnlockRect(mipMapLevel);
			}


		}
		mipMapLevel++;
	}
/*

	setupTextureFilter(&tex, filter, flags);


	TextureID tmp=insertTexture(tex);
	   */
	CHECK_HEAP0;

}
void LoadTexture(const char * file, LPDIRECT3DBASETEXTURE9* ret, bool atlas, D3DFORMAT f, unsigned int w, unsigned int h, DWORD u)
{
	//LOG_PRINT("Tex file=%s\n",file);
	CHECK_HEAP0;
	DWORD ccc=D3DCOLOR_RGBA(0,0,0,0);
	if(atlas)
	{
	  ccc=D3DCOLOR_RGBA(0,0,0,255);
	  f=D3DFMT_A8R8G8B8;
	}

	ImageD3DGL img;
	CHECK_HEAP
	if (!img.loadFromFile(file))
		LOG_PRINT("Couldn't open \"", file, "\"");

	if((w!=0)||(h!=0)) img.resize(w,h,1);

CHECK_HEAP
/*
		DDSURFACEDESC2 img;
	char filecode[4];
	FILE *fp;
	fp = fopen(file, "rb");
	if (fp == NULL)
		return -1;
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0)
	{
		fclose(fp);
		return -1;
	}
	fread(&img, sizeof(img), 1, fp);
		fclose(fp);
*/

	LPDIRECT3DTEXTURE9 texture;
	LPDIRECT3DCUBETEXTURE9 cubeMap;
	LPDIRECT3DVOLUMETEXTURE9 volumeMap;
	if (img.depth == 0)//if (img.dwDepth == 0)
	{
		OutputDebugString("D3DXCreateCubeTextureFromFile(\"");
		V(
			//D3DXCreateCubeTextureFromFile(g_pd3dDevice,file,&cubeMap)
			D3DXCreateCubeTextureFromFileEx(
					g_pd3dDevice,
					file,
					D3DX_DEFAULT,
					//D3DX_DEFAULT,
					0,
					u,
					f,//D3DFMT_UNKNOWN,
					D3DPOOL_MANAGED,
					D3DX_FILTER_LINEAR,
					D3DX_FILTER_LINEAR,
					ccc,
					NULL,
					NULL,
					&cubeMap
					)

			);

		*ret = cubeMap;
	} else if (img.depth >	1)//} else if (img.dwDepth >  1)
	{
		OutputDebugString("D3DXCreateVolumeTextureFromFile(\"");
		V(
			//D3DXCreateVolumeTextureFromFile(g_pd3dDevice,file,&volumeMap)
			D3DXCreateVolumeTextureFromFileEx(
					g_pd3dDevice,
					file,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					0,
					u,
					f,//D3DFMT_UNKNOWN,
					D3DPOOL_MANAGED,
					D3DX_FILTER_LINEAR,
					D3DX_FILTER_LINEAR,
					ccc,
					NULL,
					NULL,
					&volumeMap
					)
			);
		*ret = volumeMap;
	} else {
		OutputDebugString("D3DXCreateTextureFromFile(\"");
		CHECK_HEAP
		V(
		//D3DXCreateTextureFromFile(g_pd3dDevice, file, &texture)
 /*
      V(D3DXCreateTextureFromFileExA(g_pd3dDevice,
				fn,
				D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
				0, D3DFMT_A8R8G8B8,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT, D3DX_DEFAULT,
				D3DCOLOR_RGBA(0,0,0,255), NULL, NULL,
				(LPDIRECT3DTEXTURE9 *)&g_pTexture6));
 */
	    D3DXCreateTextureFromFileEx(
					g_pd3dDevice,
					file,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					u,
					f,//D3DFMT_UNKNOWN,
					D3DPOOL_MANAGED,
					D3DX_FILTER_LINEAR,
					D3DX_FILTER_LINEAR,
					ccc,
					NULL,
					NULL,
					&texture
					)
		);
		CHECK_HEAP
	//LOG_PRINT("ret=%d\n",*ret);
CHECK_HEAP
		//ret=(LPDIRECT3DBASETEXTURE9*)&texture;
		(*ret)=(LPDIRECT3DBASETEXTURE9)texture;

		//LOG_PRINT("ret=%d\n",*ret);
CHECK_HEAP
	}
	OutputDebugString(file);
	OutputDebugString("\")\n");

CHECK_HEAP0;
}
const int iatlassize=1024;
const int isubtexturesize=256;
int AddTextureToAtlas(ImageD3DGL& atlas,ImageD3DGL& img,bool start)
{
	CHECK_HEAP0;
	static int atlasTextureCount=0;
	if(start) atlasTextureCount=0;
	img.decompress();
	FORMAT format = img.getFormat();
	if(format!=FORMAT_RGB8)
	   img.convert(FORMAT_RGB8);
	format = img.getFormat();
//if(img.format==FORMAT_RGB8) img.addChannel();

	if (isPlainFormat(format) && getChannelCount(format) == 3)
	{
		//LOG_PRINT("Add Channel\n");
		img.addChannel(1);
	}
atlas.decompress();
if((atlas.width!=iatlassize)||(atlas.height!=iatlassize))
{
  //LOG_PRINT("Atlas size should be (%d x %d)\n",iatlassize,iatlassize);
	  return atlasTextureCount;
}
if((img.width!=isubtexturesize)||(img.height!=isubtexturesize))
	  img.resize(isubtexturesize, isubtexturesize, 1);

//applyGamma( img, gamma );

ubyte *fDest = atlas.getPixels();
ubyte *fSrc = img.getPixels();
int ix = atlasTextureCount / (iatlassize/isubtexturesize);
int iy = atlasTextureCount-ix*(iatlassize/isubtexturesize);
for (int y = 0; y < isubtexturesize; y++){
	//if(y > (isubtexturesize-1)) return ++atlasTextureCount;
DWORD src1 = iatlassize * 4 * y + ix*isubtexturesize * 4+iy*iatlassize * 4 *isubtexturesize;
memcpy(fDest + src1, fSrc  + isubtexturesize * 4 * y, isubtexturesize * 4);

}
CHECK_HEAP0;
return ++atlasTextureCount;
}
/*
 ImageD3DGL atlas0,grass1,grass2;
 ubyte *fDest = atlas0.create(FORMAT_RGBA8, iatlassize, iatlassize, 1, 1);
 if (!grass1.loadDDS("Media\\Textures\\Particles\\Particle_grass001.dds")) //LOG_PRINT("tex failed %s:%d\n",__FUNCTION__,__LINE__);
 if (!grass2.loadDDS("Media\\Textures\\Particles\\Particle_grass002.dds")) //LOG_PRINT("tex failed %s:%d\n",__FUNCTION__,__LINE__);
 AddTextureToAtlas(atlas0,grass1,true);
 AddTextureToAtlas(atlas0,grass2);
*/
void LoadRGB(const char* fn, LPDIRECT3DTEXTURE9* pTexture, D3DCOLOR cc)
{
  ImageD3DGL img;
  img.loadFromFile(fn);
  LoadRGB(img, pTexture,cc);
}
void LoadRGB(ImageD3DGL& img, LPDIRECT3DTEXTURE9* pTexture, D3DCOLOR cc)
{
  img.decompress();
	    char buf[256];
	TextureD3DGL tex;
	LPDIRECT3DTEXTURE9 texture=NULL;
	memset(&tex, 0, sizeof(tex));
	FORMAT format = img.getFormat();
	if(format != FORMAT_RGB8)
	{
	     //img.decompress();
		if(format == FORMAT_RGBA8)
		{
#if 0
			img.removeChannel(3);

				//img.swapChannels(0, 2);
#elif 1
			BYTE* p=img.getPixels();
			DWORD cr=D3DCOLOR_R(cc);
			DWORD cg=D3DCOLOR_G(cc);
			DWORD cb=D3DCOLOR_B(cc);
#if 1
	if (format >= FORMAT_RGB8 && format <= FORMAT_RGBA8)
	{
	  int nMipMaps=img.getMipMapCount();
	int nChannels = (format == FORMAT_RGB8)? 3 : 4;
	int nPixels = img.getPixelCount(0, nMipMaps);
	ubyte *src = img.getPixels();
	//ubyte *newPixels = new ubyte[nPixels];
	//ubyte *dest = newPixels;
	do {
		if((*(src+2)!=cg)||(*(src+1)!=cr)||(*(src+3)!=cb))
			*(src+3)=BYTE(255.0f);
		else
			*(src+3)=BYTE(0);

		//*dest++ = (ubyte) (src[0] * rf + src[1] * gf + src[2] * bf);
		src += nChannels;
	} while (--nPixels);
	//format = FORMAT_I8;
	//setPixels(newPixels);
	}
#else
	for (int y = 0; y < img.height; y++){
	for (int x = 0; x < img.width; x++){
		int idx=y*img.width+x;
		if((p[idx*4+2]!=cg)||(p[idx*4+1]!=cr)||(p[idx*4+3]!=cb))
		{/*
			BYTE tmp=p[idx*4+3];
			p[idx*4+3]=p[idx*4+2];
			p[idx*4+2]=tmp;*/
			p[idx*4+0]=BYTE(255.0f);
		}
		else
		{/*
			p[idx*4+3]=BYTE(0);
			p[idx*4+1]=BYTE(0);
			p[idx*4+2]=BYTE(0);*/
			p[idx*4+0]=BYTE(0);
		}
	}}
#endif
#endif
		}
		img.convert(FORMAT_RGB8);
	      format = img.getFormat();
	}else
	if (isPlainFormat(format) && getChannelCount(format) == 3){
		img.addChannel(1);
		format = img.getFormat();
	}
	tex.format = texFormats[format];
	tex.width  = img.getWidth();
	tex.height = img.getHeight();
	tex.pool   = D3DPOOL_MANAGED;
	if (img.isCube()){
		DBG_HALT;
	} else if (img.is3D()){
		DBG_HALT;
	} else {
		OutputDebugString("CreateTexture\n");
#if 0
		V(g_pd3dDevice->CreateTexture(tex.width, tex.height, 1/*img.getMipMapCount()*/, 0, tex.format, D3DPOOL_MANAGED, &texture, NULL));
#else
		/*
     V(D3DXCreateTextureEx(g_pd3dDevice,tex.width, tex.height, 1,
				0, D3DFMT_A8R8G8B8,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT, D3DX_DEFAULT,
				D3DCOLOR_RGBA(0,0,0,255), NULL, NULL,
				*/
	V(D3DXCreateTexture(
  g_pd3dDevice,
  tex.width, tex.height, img.getMipMapCount(),
  0,
  D3DFMT_R8G8B8,
  D3DPOOL_MANAGED,
  (LPDIRECT3DTEXTURE9 *)pTexture));
	texture=*pTexture;
#endif
	}
	// D3D uses BGR byte order for 8bit formats
	//if (format == FORMAT_RGBA8) img.swapChannels(0, 2);
	// Upload the texture image
	ubyte *src;
	int mipMapLevel = 0;
	while ((src = img.getPixels(mipMapLevel)) != NULL){
		int w = img.getWidth(mipMapLevel);
		int h = img.getHeight(mipMapLevel);
		int rows = img.getHeightInBlocks(mipMapLevel);
		int rowSize   = getImageSize(format, w, 1, 1, 1);
		int sliceSize = getImageSize(format, w, h, 1, 1);
		if (img.is3D()){
		  DBG_HALT;
		} else if (img.isCube()){
		  DBG_HALT;
		} else {
			D3DLOCKED_RECT rect;
			if (texture->LockRect(mipMapLevel, &rect, NULL, 0) == D3D_OK){
				if (format == FORMAT_ATI1N || format == FORMAT_ATI2N){
					// There's an issue with the D3D runtime causing Pitch to be invalid
					// for ATI1N and ATI2N, so we'll just use plain memory copy in this case.
					memcpy(rect.pBits, src, sliceSize);
				} else {
					for (int y = 0; y < rows; y++){
						memcpy(((ubyte *) rect.pBits) + y * rect.Pitch, src + y * rowSize, rowSize);
					}
				}
				texture->UnlockRect(mipMapLevel);
			}
		}
		mipMapLevel++;
	}
	CHECK_HEAP
}
void LoadAtlas(ImageD3DGL& atlas, LPDIRECT3DTEXTURE9* pAtlasTexture, D3DCOLOR cc)
{
	  CHECK_HEAP
	DWORD cr=D3DCOLOR_R(cc);
	DWORD cg=D3DCOLOR_G(cc);
	DWORD cb=D3DCOLOR_B(cc);

  atlas.decompress();
  //atlas.convert(FORMAT_RGB8);
  //atlas.convert(FORMAT_RGBA8);
	ImageD3DGL img(atlas);
	    char buf[256];
	TextureD3DGL tex;
	LPDIRECT3DTEXTURE9 texture=NULL;
	memset(&tex, 0, sizeof(tex));
	FORMAT format = img.getFormat();
	if(format != FORMAT_RGBA8)
	{
	     //img.decompress();
	    if(format != FORMAT_RGB8)img.convert(FORMAT_RGB8);
		img.convert(FORMAT_RGBA8);
	      format = img.getFormat();
	}else
	if (isPlainFormat(format) && getChannelCount(format) == 3){
		img.addChannel(1);
		format = img.getFormat();
	}
#if 1
	if (format >= FORMAT_RGB8 && format <= FORMAT_RGBA8)
	{
	  int nMipMaps=img.getMipMapCount();
	int nChannels = (format == FORMAT_RGB8)? 3 : 4;
	int nPixels = img.getPixelCount(0, nMipMaps);
	ubyte *src = img.getPixels();
	//ubyte *newPixels = new ubyte[nPixels];
	//ubyte *dest = newPixels;
	do {
		if((*(src+2)!=cb)||(*(src+1)!=cg)||(*(src)!=cr))
			*(src+3)=BYTE(255.0f);
		else
			*(src+3)=BYTE(0);

		//*dest++ = (ubyte) (src[0] * rf + src[1] * gf + src[2] * bf);
		src += nChannels;
	} while (--nPixels);
	//format = FORMAT_I8;
	//setPixels(newPixels);
	}
#else
		BYTE* p=img.getPixels();

	for (int y = 0; y < img.height; y++){
	for (int x = 0; x < img.width; x++){
		int idx=y*img.width+x;
		if((p[idx*4+2]!=cb)||(p[idx*4+1]!=cg)||(p[idx*4]!=cr))
			p[idx*4+3]=BYTE(255.0f);
		else
			p[idx*4+3]=BYTE(0);
	}}
#endif
	tex.format = texFormats[format];
	tex.width  = img.getWidth();
	tex.height = img.getHeight();
	tex.pool   = D3DPOOL_MANAGED;
	if (img.isCube()){
		DBG_HALT;
	} else if (img.is3D()){
		DBG_HALT;
	} else {
		OutputDebugString("CreateTexture\n");
#if 0
		V(g_pd3dDevice->CreateTexture(tex.width, tex.height, 1/*img.getMipMapCount()*/, 0, tex.format, D3DPOOL_MANAGED, &texture, NULL));
#else
		/*
     V(D3DXCreateTextureEx(g_pd3dDevice,tex.width, tex.height, 1,
				0, D3DFMT_A8R8G8B8,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT, D3DX_DEFAULT,
				D3DCOLOR_RGBA(0,0,0,255), NULL, NULL,
				*/
	V(D3DXCreateTexture(
  g_pd3dDevice,
  tex.width, tex.height, img.getMipMapCount(),
  0,
  D3DFMT_A8R8G8B8,
  D3DPOOL_MANAGED,
  (LPDIRECT3DTEXTURE9 *)pAtlasTexture));
	texture=*pAtlasTexture;
#endif
	}
	// D3D uses BGR byte order for 8bit formats
	if (format == FORMAT_RGBA8) img.swapChannels(0, 2);
	// Upload the texture image
	ubyte *src;
	int mipMapLevel = 0;
	while ((src = img.getPixels(mipMapLevel)) != NULL){
		int w = img.getWidth(mipMapLevel);
		int h = img.getHeight(mipMapLevel);
		int rows = img.getHeightInBlocks(mipMapLevel);
		int rowSize   = getImageSize(format, w, 1, 1, 1);
		int sliceSize = getImageSize(format, w, h, 1, 1);
		if (img.is3D()){
		  DBG_HALT;
		} else if (img.isCube()){
		  DBG_HALT;
		} else {
			D3DLOCKED_RECT rect;
			if (texture->LockRect(mipMapLevel, &rect, NULL, 0) == D3D_OK){
				if (format == FORMAT_ATI1N || format == FORMAT_ATI2N){
					// There's an issue with the D3D runtime causing Pitch to be invalid
					// for ATI1N and ATI2N, so we'll just use plain memory copy in this case.
					memcpy(rect.pBits, src, sliceSize);
				} else {
					for (int y = 0; y < rows; y++){
						memcpy(((ubyte *) rect.pBits) + y * rect.Pitch, src + y * rowSize, rowSize);
					}
				}
				texture->UnlockRect(mipMapLevel);
			}
		}
		mipMapLevel++;
	}
	CHECK_HEAP
}
void LoadAtlas(const char* fn, LPDIRECT3DTEXTURE9* pAtlasTexture, D3DCOLOR cc)
{
#if 0
     V(D3DXCreateTextureFromFileExA(g_pd3dDevice,
				fn,
				D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
				0, D3DFMT_A8R8G8B8,
				D3DPOOL_DEFAULT,
				D3DX_DEFAULT, D3DX_DEFAULT,
				D3DCOLOR_RGBA(0,0,0,255), NULL, NULL,
				(LPDIRECT3DTEXTURE9 *)pAtlasTexture));
#else
     ImageD3DGL img;
     img.loadFromFile(fn);
     LoadAtlas(img, pAtlasTexture,cc);
#endif
}
#if 0
void LoadAtlas(ImageD3DGL& atlas, LPDIRECT3DTEXTURE9* pAtlasTexture, D3DCOLOR cc)
{
  atlas.decompress();
  atlas.convert(FORMAT_RGB8);
  atlas.convert(FORMAT_RGBA8);
#if 0
  atlas.saveDDS("AtlasXXX.dds");
#else
#if 1
     LPDIRECT3DTEXTURE9 pTexture=NULL;
     LoadTexture(atlas,(LPDIRECT3DBASETEXTURE9*)&pTexture);
     SaveTexture(pTexture,"AtlasXXX.dds",D3DXIFF_DDS);
     SAFE_RELEASE(pTexture);
#else
     LPDIRECT3DTEXTURE9 pTexture=NULL;
     LoadTexture(atlas,(LPDIRECT3DBASETEXTURE9*)&pTexture);
	LPD3DXBUFFER pBuf=NULL;
		HRESULT hr=D3DXSaveTextureToFileInMemory(&pBuf, D3DXIFF_DDS,  pTexture, NULL);
		SAFE_RELEASE(pTexture);
			if(hr==D3D_OK) {
			//SAFE_RELEASE( pTexture);

			BYTE *bytePointer=(BYTE*)pBuf->GetBufferPointer();
			unsigned int nSize = pBuf->GetBufferSize();
#if 0
      Stream.Write(nSize);
			for(unsigned int i=0 ; i<nSize ; ++i) {
				Stream.Write(bytePointer[i]);
			}
#else
#if 0
   V(D3DXCreateTextureFromFileInMemory(
				       g_pd3dDevice,
				       bytePointer,
				       nSize,
				       (LPDIRECT3DTEXTURE9 *)pAtlasTexture
				       ));
#else
     V(D3DXCreateTextureFromFileInMemoryEx(
				       g_pd3dDevice,
				       bytePointer,
				       nSize,
				D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
				0, D3DFMT_A8R8G8B8,
				D3DPOOL_DEFAULT,
				D3DX_DEFAULT, D3DX_DEFAULT,
				D3DCOLOR_RGBA(0,0,0,255), NULL, NULL,
				(LPDIRECT3DTEXTURE9 *)&pAtlasTexture));
#endif
#endif
			SAFE_RELEASE(pBuf);
		}
			else
				LOG_PRINT("TexMem:hr=%d\n",hr);
#endif
#endif
#if 1
     V(D3DXCreateTextureFromFileExA(g_pd3dDevice,
				"AtlasXXX.dds",
				D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
				0, D3DFMT_A8R8G8B8,
				D3DPOOL_DEFAULT,
				D3DX_DEFAULT, D3DX_DEFAULT,
				D3DCOLOR_RGBA(0,0,0,255), NULL, NULL,
				(LPDIRECT3DTEXTURE9 *)pAtlasTexture));
#endif
}
#endif
#endif
void DumpD3DCAPS9()
{
    /* Device Info */
    //LOG_PRINT("AdapterOrdinal=%d\n", g_caps.AdapterOrdinal);

    /* Caps from DX7 Draw */
    //LOG_PRINT("Caps=%d\n", g_caps.Caps);
    //LOG_PRINT("Caps2=%d\n", g_caps.Caps2);
    //LOG_PRINT("Caps3=%d\n", g_caps.Caps3);
    //LOG_PRINT("PresentationIntervals=%d\n", g_caps.PresentationIntervals);

    /* Cursor Caps */
    //LOG_PRINT("CursorCaps=%d\n", g_caps.CursorCaps);

    /* 3D Device Caps */
    //LOG_PRINT("DevCaps=%d\n", g_caps.DevCaps);

    //LOG_PRINT("PrimitiveMiscCaps=%d\n", g_caps.PrimitiveMiscCaps);
    //LOG_PRINT("RasterCaps=%d\n", g_caps.RasterCaps);
    //LOG_PRINT("ZCmpCaps=%d\n", g_caps.ZCmpCaps);
    //LOG_PRINT("SrcBlendCaps=%d\n", g_caps.SrcBlendCaps);
    //LOG_PRINT("DestBlendCaps=%d\n", g_caps.DestBlendCaps);
    //LOG_PRINT("AlphaCmpCaps=%d\n", g_caps.AlphaCmpCaps);
    //LOG_PRINT("ShadeCaps=%d\n", g_caps.ShadeCaps);
    //LOG_PRINT("TextureCaps=%d\n", g_caps.TextureCaps);
    //LOG_PRINT("TextureFilterCaps=%d\n", g_caps.TextureFilterCaps);	    // D3DPTFILTERCAPS for IDirect3DTexture9's
    //LOG_PRINT("CubeTextureFilterCaps=%d\n", g_caps.CubeTextureFilterCaps);	// D3DPTFILTERCAPS for IDirect3DCubeTexture9's
    //LOG_PRINT("VolumeTextureFilterCaps=%d\n", g_caps.VolumeTextureFilterCaps);	  // D3DPTFILTERCAPS for IDirect3DVolumeTexture9's
    //LOG_PRINT("TextureAddressCaps=%d\n", g_caps.TextureAddressCaps);	     // D3DPTADDRESSCAPS for IDirect3DTexture9's
    //LOG_PRINT("VolumeTextureAddressCaps=%d\n", g_caps.VolumeTextureAddressCaps);   // D3DPTADDRESSCAPS for IDirect3DVolumeTexture9's

    //LOG_PRINT("LineCaps=%d\n", g_caps.LineCaps);		   // D3DLINECAPS

    //LOG_PRINT("MaxTextureWidth=%d\n", g_caps.MaxTextureWidth);
    //LOG_PRINT("MaxTextureHeight=%d\n", g_caps.MaxTextureHeight);
	//LOG_PRINT("MaxVolumeExtent=%d\n", g_caps.MaxVolumeExtent);

    //LOG_PRINT("MaxTextureRepeat=%d\n", g_caps.MaxTextureRepeat);
    //LOG_PRINT("MaxTextureAspectRatio=%d\n", g_caps.MaxTextureAspectRatio);
    //LOG_PRINT("MaxAnisotropy=%d\n", g_caps.MaxAnisotropy);
    //LOG_PRINT("MaxVertexW=%f\n", g_caps.MaxVertexW);

    //LOG_PRINT("GuardBandLeft=%f\n", g_caps.GuardBandLeft);
    //LOG_PRINT("GuardBandTop=%f\n", g_caps.GuardBandTop);
    //LOG_PRINT("GuardBandRight=%f\n", g_caps.GuardBandRight);
    //LOG_PRINT("GuardBandBottom=%f\n", g_caps.GuardBandBottom);

    //LOG_PRINT("ExtentsAdjust=%f\n", g_caps.ExtentsAdjust);
    //LOG_PRINT("StencilCaps=%d\n", g_caps.StencilCaps);

    //LOG_PRINT("FVFCaps=%d\n", g_caps.FVFCaps);
    //LOG_PRINT("TextureOpCaps=%d\n", g_caps.TextureOpCaps);
    //LOG_PRINT("MaxTextureBlendStages=%d\n", g_caps.MaxTextureBlendStages);
    //LOG_PRINT("MaxSimultaneousTextures=%d\n", g_caps.MaxSimultaneousTextures);

    //LOG_PRINT("VertexProcessingCaps=%d\n", g_caps.VertexProcessingCaps);
    //LOG_PRINT("MaxActiveLights=%d\n", g_caps.MaxActiveLights);
    //LOG_PRINT("MaxUserClipPlanes=%d\n", g_caps.MaxUserClipPlanes);
    //LOG_PRINT("MaxVertexBlendMatrices=%d\n", g_caps.MaxVertexBlendMatrices);
    //LOG_PRINT("MaxVertexBlendMatrixIndex=%d\n", g_caps.MaxVertexBlendMatrixIndex);

    //LOG_PRINT("MaxPointSize=%f\n", g_caps.MaxPointSize);

    //LOG_PRINT("MaxPrimitiveCount=%d\n", g_caps.MaxPrimitiveCount);	    // std::max number of primitives per DrawPrimitive call
    //LOG_PRINT("MaxVertexIndex=%d\n", g_caps.MaxVertexIndex);
    //LOG_PRINT("MaxStreams=%d\n", g_caps.MaxStreams);
    //LOG_PRINT("MaxStreamStride=%d\n", g_caps.MaxStreamStride);		  // std::max stride for SetStreamSource

    //LOG_PRINT("VertexShaderVersion=%d\n", g_caps.VertexShaderVersion);
    //LOG_PRINT("MaxVertexShaderConst=%d\n", g_caps.MaxVertexShaderConst);	 // number of vertex shader constant registers

    //LOG_PRINT("PixelShaderVersion=%d\n", g_caps.PixelShaderVersion);
    //LOG_PRINT("PixelShader1xMaxValue=%f\n", g_caps.PixelShader1xMaxValue);	// std::max value storable in registers of ps.1.x shaders

    // Here are the DX9 specific ones
    //LOG_PRINT("DevCaps2=%d\n", g_caps.DevCaps2);

    //LOG_PRINT("MaxNpatchTessellationLevel=%f\n", g_caps.MaxNpatchTessellationLevel);
    //LOG_PRINT("Reserved5=%d\n", g_caps.Reserved5);

    //LOG_PRINT("MasterAdapterOrdinal=%d\n", g_caps. MasterAdapterOrdinal);	// ordinal of master adaptor for adapter group
    //LOG_PRINT("AdapterOrdinalInGroup=%d\n", g_caps. AdapterOrdinalInGroup);	 // ordinal inside the adapter group
    //LOG_PRINT("NumberOfAdaptersInGroup=%d\n", g_caps. NumberOfAdaptersInGroup);    // number of adapters in this adapter group (only if master)
    //LOG_PRINT("DeclTypes=%d\n", g_caps.DeclTypes);		    // Data types, supported in vertex declarations
    //LOG_PRINT("NumSimultaneousRTs=%d\n", g_caps.NumSimultaneousRTs);	     // Will be at least 1
    //LOG_PRINT("StretchRectFilterCaps=%d\n", g_caps.StretchRectFilterCaps);	// Filter caps supported by StretchRect
    //LOG_PRINT("VertexTextureFilterCaps=%d\n", g_caps.VertexTextureFilterCaps);	  // D3DPTFILTERCAPS for IDirect3DTexture9's for texture, used in vertex shaders
    //LOG_PRINT("MaxVShaderInstructionsExecuted=%d\n", g_caps.MaxVShaderInstructionsExecuted); // maximum number of vertex shader instructions that can be executed
    //LOG_PRINT("MaxPShaderInstructionsExecuted=%d\n", g_caps.MaxPShaderInstructionsExecuted); // maximum number of pixel shader instructions that can be executed
    //LOG_PRINT("MaxVertexShader30InstructionSlots=%d\n", g_caps.MaxVertexShader30InstructionSlots);
    //LOG_PRINT("MaxPixelShader30InstructionSlots=%d\n", g_caps.MaxPixelShader30InstructionSlots);

/*
    D3DDEVTYPE	DeviceType);
	D3DVSHADERCAPS2_0 VS20Caps);
    D3DPSHADERCAPS2_0 PS20Caps);
*/
}
HRESULT DrawFPS(int x, int y)
{
			g_dwTime = timeGetTime();
						g_dwFPS++;
			g_dwLoopFrames++;
			if (g_dwTime - g_dwLastPerfTime > 1000.0)
			{
				sprintf(g_strStats, "Framerate: %d fps", g_dwFPS);
				g_dwFPS = 0;
				g_dwLastPerfTime = g_dwTime;
			}
			RECT destRect1;
				SetRect( &destRect1, x, y, 0, 0 );
				HRESULT hr=g_pd3dxFont->DrawText( NULL,g_strStats, -1, &destRect1, DT_NOCLIP,
				   D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );

	return hr;
}
#ifdef INCLUDECAMERA
HRESULT DrawYawPitch(int x, int y, Camera* pCamera)
{
    RECT destRect1;
	HRESULT hr;
	char buf[256];
if(!pCamera)
{
#if 0
		float yaw=g_fSpinX/**0.5f*//D3DX_PI*360.0f;
		yaw-=int(yaw/360)*360.0f;
		if(yaw<0.0f)yaw+=360.0f;

				float pitch=g_fSpinY/**0.5f*//D3DX_PI*360.0f;
		pitch-=int(pitch/360)*360.0f;
		if(pitch<0.0f)pitch+=360.0f;
#endif
	sprintf(buf, "Yaw: %f", g_fSpinX);
    SetRect( &destRect1, x, y, 0, 0 );
	hr=g_pd3dxFont->DrawText( NULL,buf, -1, &destRect1, DT_NOCLIP,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
	sprintf(buf, "Pitch: %f", g_fSpinY);
    SetRect( &destRect1, x, y+20, 0, 0 );
	hr=g_pd3dxFont->DrawText( NULL,buf, -1, &destRect1, DT_NOCLIP,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );

	sprintf(buf, "g_fX: %f", g_fX);
    SetRect( &destRect1, x, y+40, 0, 0 );
	hr=g_pd3dxFont->DrawText( NULL,buf, -1, &destRect1, DT_NOCLIP,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
	sprintf(buf, "g_fY: %f", g_fY);
    SetRect( &destRect1, x, y+60, 0, 0 );
	hr=g_pd3dxFont->DrawText( NULL,buf, -1, &destRect1, DT_NOCLIP,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
	sprintf(buf, "g_fZ: %f", g_fZ);
    SetRect( &destRect1, x, y+80, 0, 0 );
	hr=g_pd3dxFont->DrawText( NULL,buf, -1, &destRect1, DT_NOCLIP,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
}else{
		float yaw=pCamera->m_yaw/**0.5f*//D3DX_PI*360.0f;
		yaw-=int(yaw/360)*360.0f;
		if(yaw<0.0f)yaw+=360.0f;

				float pitch=pCamera->m_pitch/**0.5f*//D3DX_PI*360.0f;
		pitch-=int(pitch/360)*360.0f;
		if(pitch<0.0f)pitch+=360.0f;

	sprintf(buf, "Yaw: %f", yaw);
    SetRect( &destRect1, x, y, 0, 0 );
	hr=g_pd3dxFont->DrawText( NULL,buf, -1, &destRect1, DT_NOCLIP,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
	sprintf(buf, "Pitch: %f", pitch);
    SetRect( &destRect1, x, y+20, 0, 0 );
	hr=g_pd3dxFont->DrawText( NULL,buf, -1, &destRect1, DT_NOCLIP,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );

	sprintf(buf, "g_fX: %f", pCamera->_pos.x);
    SetRect( &destRect1, x, y+40, 0, 0 );
	hr=g_pd3dxFont->DrawText( NULL,buf, -1, &destRect1, DT_NOCLIP,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
	sprintf(buf, "g_fY: %f", pCamera->_pos.y);
    SetRect( &destRect1, x, y+60, 0, 0 );
	hr=g_pd3dxFont->DrawText( NULL,buf, -1, &destRect1, DT_NOCLIP,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
	sprintf(buf, "g_fZ: %f", pCamera->_pos.z);
    SetRect( &destRect1, x, y+80, 0, 0 );
	hr=g_pd3dxFont->DrawText( NULL,buf, -1, &destRect1, DT_NOCLIP,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
}
	return hr;
}
#endif
HRESULT DrawText(unsigned int x, unsigned int y, const char *pszFormat, ...)
{
	char	buf[MAX_BUF_SIZE];
	buf[0]='\0';
	va_list arglist;
	va_start(arglist, pszFormat);
	vsprintf(&buf[strlen(buf)], pszFormat, arglist);
	va_end(arglist);

    RECT destRect1;
	HRESULT hr;
    SetRect( &destRect1, x, y, 0, 0 );
	hr=g_pd3dxFont->DrawText( NULL,buf, -1, &destRect1, DT_NOCLIP,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
	return hr;
}
HRESULT DrawSmallText(unsigned int x, unsigned int y, const char *pszFormat, ...)
{
	char	buf[MAX_BUF_SIZE];
	buf[0]='\0';
	va_list arglist;
	va_start(arglist, pszFormat);
	vsprintf(&buf[strlen(buf)], pszFormat, arglist);
	va_end(arglist);

    RECT destRect1;
	HRESULT hr;
    SetRect( &destRect1, x, y, 0, 0 );
	hr=g_pd3dxSmallFont->DrawText( NULL,buf, -1, &destRect1, DT_NOCLIP,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
	return hr;
}
float sign7(float s)
{
	if (s<0.0f)
		return -1.0f;
	else
		return 1.0f;
}
float swapy(float y)
{
	#if 0
	return -y;
	#elif 1
	return -(y-1.0f);
	#elif 0
	return y;
	#endif
}
void SetVertex5(float * v, D3DXVECTOR2 aQuadSize, D3DXVECTOR2 aQuadPosition, D3DXVECTOR2 aViewport)
{
	float tx = sign7(v[0]);
	float ty = sign7(v[1]);
	//ty=swapy(ty);
#if 0
	v[0]=(tx+1)*(aQuadSize.x/aViewport.x)+(2*aQuadPosition.x/aViewport.x)-1;
	v[1]=(ty+1)*(aQuadSize.y/aViewport.y)+(2*aQuadPosition.y/aViewport.y)-1;
#else
	v[5*0+0]=-aQuadPosition.x/aViewport.x;
	v[5*0+1]=(aQuadPosition.y-aViewport.y)/aViewport.y;
	v[5*0+2]=0.0f;

	v[5*1+0]=(-aQuadPosition.x+2*aQuadSize.x-aViewport.x)/aViewport.x;
	v[5*1+1]=(aQuadPosition.y-aViewport.y)/aViewport.y;
	v[5*1+2]=0.0f;

	v[5*2+0]=(-aQuadPosition.x-aViewport.x)/aViewport.x;
	v[5*2+1]=(aQuadPosition.y-aViewport.y-2*aQuadSize.y)/aViewport.y;
	v[5*2+2]=0.0f;

	v[5*3+0]=(-aQuadPosition.x+2*aQuadSize.x-aViewport.x)/aViewport.x;
	v[5*3+1]=(aQuadPosition.y-aViewport.y-2*aQuadSize.y)/aViewport.y;
	v[5*3+2]=0.0f;

	v[5*0+3]=-1;
	v[5*0+4]=1;

	v[5*1+3]=1;
	v[5*1+4]=1;

	v[5*2+3]=-1;
	v[5*2+4]=-1;

	v[5*3+3]=1;
	v[5*3+4]=-1;
#endif
//		v[1]=aViewport.y-v[1];



	v[3]=(tx+1.0)/2.0;
	v[4]=(ty-1.0)*-0.5;
	//v[4]=swapy(v[4]);
}
/*
	  -1.0f, 1.0f, 0.0f, tx, ty
	  ,1.0f, 1.0f, 0.0f, tx+stx, ty
	  ,-1.0f,-1.0f, 0.0f, tx, ty+sty
	  , 1.0f,-1.0f, 0.0f, tx+stx, ty+sty
*/
void SetVertex3(float * v, D3DXVECTOR2 aQuadSize, D3DXVECTOR2 aQuadPosition, D3DXVECTOR2 aViewport)
{
	float tx = sign7(v[0]);
	float ty = sign7(v[1]);
	//ty=swapy(ty);
#if 0
	v[0]=(tx+1)*(aQuadSize.x/aViewport.x)+(2*aQuadPosition.x/aViewport.x)-1;
	v[1]=(ty+1)*(aQuadSize.y/aViewport.y)+(2*aQuadPosition.y/aViewport.y)-1;
#else
	v[5*0+0]=(-aQuadPosition.x-aViewport.x)/aViewport.x;
	v[5*0+1]=(aQuadPosition.y-aViewport.y)/aViewport.y;
	v[5*0+2]=0.0f;

	v[5*1+0]=(-aQuadPosition.x+2*aQuadSize.x-aViewport.x)/aViewport.x;
	v[5*1+1]=(aQuadPosition.y-aViewport.y)/aViewport.y;
	v[5*1+2]=0.0f;

	v[5*2+0]=(-aQuadPosition.x-aViewport.x)/aViewport.x;
	v[5*2+1]=(aQuadPosition.y-aViewport.y-2*aQuadSize.y)/aViewport.y;
	v[5*2+2]=0.0f;

	v[5*3+0]=(-aQuadPosition.x+2*aQuadSize.x-aViewport.x)/aViewport.x;
	v[5*3+1]=(aQuadPosition.y-aViewport.y-2*aQuadSize.y)/aViewport.y;
	v[5*3+2]=0.0f;


#endif

}
#ifdef INCLUDEIMAGE
void DrawImage(ImageD3DGL& img, int x, int y, int sx, int sy, bool InScene, float tx, float ty, float stx, float sty)
{
	LPDIRECT3DBASETEXTURE9 pTexture=NULL;
	LoadTexture(img,(LPDIRECT3DBASETEXTURE9*)&pTexture);
	DrawTexture((LPDIRECT3DBASETEXTURE9 *)&pTexture, x, y, sx, sy, InScene, tx, ty, stx, sty);
	SAFE_RELEASE(pTexture);
}
#endif
D3DXVECTOR4 SampleTexture(LPDIRECT3DTEXTURE9 pTexture,D3DXVECTOR2 uv, bool normalized)
{
	//DWORD t1=timeGetTime();

	D3DXVECTOR4 s=D3DXVECTOR4();
	int idx=pTexture->Width*int(pTexture->Height*uv.y)+int(uv.x*pTexture->Width);
 if(!normalized)
 idx=pTexture->Width*int(uv.y)+int(uv.x);
	BYTE* pT=0;
	D3DLOCKED_RECT rect;
	V(pTexture->LockRect(0,&rect,0,0));
	pT=(BYTE* )rect.pBits;
	switch(pTexture->Format)
	{
		case D3DFMT_R32F:
			s.x=((float*)pT)[idx*1+0];
		break;
		case D3DFMT_L8:
			s.x=pT[idx*1+0];
			s.y=pT[idx*1+0];
			s.z=pT[idx*1+0];
			s.w=pT[idx*1+0];
		break;
		case D3DFMT_X8R8G8B8:
		case D3DFMT_X8B8G8R8:
		case D3DFMT_R8G8B8:
			s.x=pT[idx*3+0];
			s.y=pT[idx*3+1];
			s.z=pT[idx*3+2];
			if(1)//((s.x!=0)||(s.y!=0)||(s.z!=0))
			s.w=255.0f;
			else
			s.w=0.0f;
		break;

		case D3DFMT_A8R8G8B8:
		case D3DFMT_A8B8G8R8:
			s.x=pT[idx*4+0];
			s.y=pT[idx*4+1];
			s.z=pT[idx*4+2];
			s.w=pT[idx*4+3];
		break;
		default:
		DBG_HALT;
		break;
	};
	V(pTexture->UnlockRect(0));
 /*
	DWORD t2=timeGetTime();
	LOG_PRINT("Pick took %f seconds\n",(t2-t1)/1000.0f);
	LOG_PRINT("uv.x=%f\n",uv.x);
	LOG_PRINT("uv.y=%f\n",uv.y);
	LOG_PRINT("x=%f\n",s.x);
	LOG_PRINT("y=%f\n",s.y);
	LOG_PRINT("z=%f\n",s.z);
	LOG_PRINT("w=%f\n",s.w);
   */
   return s;
}
D3DXVECTOR4 GPUSampleTexture(LPDIRECT3DTEXTURE9 pTexture,D3DXVECTOR2 uv, bool normalized, bool InScene)//Slow and returns wrong value
{
  DWORD t1=timeGetTime();
  const char* fx=   "\n" \
	     "float2 uv;\n" \
	     "int channels;\n" \
	     "bool changeRBchannels;\n"\
	     "texture sampleTexture;\n" \
	     "sampler2D Sampler = sampler_state\n" \
	     "{\n" \
	     "	  TextureD3DGL = <sampleTexture>;\n" \
	     "#if 1\n" \
	     "	  AddressU=CLAMP;\n" \
	     "	  AddressV=CLAMP;\n" \
	     "	  AddressW=CLAMP;\n" \
	     "	  MagFilter=POINT;\n" \
	     "	  MinFilter=POINT;\n" \
	     "	  MipFilter=NONE;\n" \
	     "#else\n" \
	     "	  AddressU=WRAP;\n" \
	     "	  AddressV=WRAP;\n" \
	     "	  AddressW=WRAP;\n" \
	     "	  MagFilter=LINEAR;\n" \
	     "	  MinFilter=LINEAR;\n" \
	     "	  MipFilter=LINEAR;\n" \
	     "#endif\n" \
	     "};\n" \
	     "struct VS_INPUT\n" \
	     "{\n" \
	     "	  float4 position	: POSITION;\n" \
	     "	float2 texture0 : TEXCOORD0;\n" \
	     "};\n" \
	     "struct VS_OUTPUT\n" \
	     "{\n" \
	     "	  float4 hposition : POSITION;\n" \
	     "	float2 texture0  : TEXCOORD0;\n" \
	     "};\n" \
	     "VS_OUTPUT myvs( const VS_INPUT IN)\n" \
	     "{\n" \
	     "	  VS_OUTPUT OUT;\n" \
	     "	OUT.hposition = IN.position;\n" \
	     "	OUT.texture0 = uv;//IN.texture0;\n" \
	     "	return OUT;\n" \
	     "}\n" \
	     "float4 myps( VS_OUTPUT IN ):COLOR0\n" \
	     "{\n" \
	     "	  float4 color = tex2D( Sampler, IN.texture0 );\n" \
	     "	   if(channels==1)\n"\
	     "	   {\n"\
	     "	       color.y=color.x;\n"\
	     "	       color.z=color.x;\n"\
	     "	       color.w=color.x;\n"\
	     "	   }\n"\
	     "	   if(channels==3)\n"\
	     "	    {\n"\
	     "	      //float t=color.z;\n"\
	     "	      //color.z=color.x;\n"\
	     "	      //color.x=t;\n"\
	     "	      color.w=1;\n"\
	     "	    }\n"\
	     "	  return color;\n" \
	     "}\n" \
	     "technique Technique0\n" \
	     "{\n" \
	     "	  pass Pass0\n" \
	     "	  {\n" \
	     "		Lighting = FALSE;\n" \
	     "		CullMode = CCW;\n" \
	     "		VertexShader = compile vs_2_0 myvs();\n" \
	     "		PixelShader  = compile ps_2_0 myps();\n" \
	     "	  }\n" \
	     "}\n";
  HRESULT hr;
 LPDIRECT3DSURFACE9 fbColor;
 LPDIRECT3DSURFACE9 fbDepth;
 static LPD3DXEFFECT pEffect = NULL;
 static LPDIRECT3DTEXTURE9	m_pDynamicTexture  = NULL;
 static LPDIRECT3DTEXTURE9	m_pShadowTexture  = NULL;
 static LPDIRECT3DSURFACE9	m_pShadowTextureS  = NULL;
 static LPDIRECT3DSURFACE9	m_pTextureSurface  = NULL;
 const int RENDERTOSURFACE_WIDTH  = 1;
 const int RENDERTOSURFACE_HEIGHT = 1;

	V(g_pd3dDevice->GetRenderTarget(0, &fbColor));
	V(g_pd3dDevice->GetDepthStencilSurface(&fbDepth));
 if(m_pDynamicTexture  == NULL)
 {
   LoadEffectFromString(fx,&pEffect);
   //Direct3D9: (ERROR) :Lock is not supported for textures allocated with POOL_DEFAULT unless they are marked D3DUSAGE_DYNAMIC.

   V(
D3DXCreateTexture( g_pd3dDevice,
				    RENDERTOSURFACE_WIDTH,
				    RENDERTOSURFACE_HEIGHT,
							1,
							D3DUSAGE_DYNAMIC,
							D3DFMT_A8R8G8B8,
							D3DPOOL_SYSTEMMEM,
							&m_pShadowTexture );

	   );
	hr = D3DXCreateTexture( g_pd3dDevice,
				    RENDERTOSURFACE_WIDTH,
				    RENDERTOSURFACE_HEIGHT,
							1,
							D3DUSAGE_RENDERTARGET,
							D3DFMT_A8R8G8B8,
							D3DPOOL_DEFAULT,
							&m_pDynamicTexture );
	if( FAILED(hr) )
	{
		LOG_PRINT("Failed to create a texture with the D3DUSAGE_RENDERTARGET usage flag set!\n");
		DBG_HALT;
    }
    m_pDynamicTexture->GetSurfaceLevel( 0, &m_pTextureSurface );
    m_pShadowTexture->GetSurfaceLevel( 0, &m_pShadowTextureS );
 }
    //D3DXMatrixIdentity( &identity );
  D3DXVECTOR4 r=D3DXVECTOR4();
  float verts[20];
  float verts2[] = {
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f
		,1.0f, 1.0f, 0.0f, 1.0f, 1.0f
		,-1.0f,-1.0f, 0.0f, 0.0f, 0.0f
		, 1.0f,-1.0f, 0.0f, 1.0f, 0.0f
  };
  memcpy(verts,verts2,20*sizeof(float));

	V(g_pd3dDevice->SetRenderTarget(0, m_pTextureSurface));
	V(g_pd3dDevice->SetDepthStencilSurface(NULL));
    V(g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET  /*| D3DCLEAR_ZBUFFER*/, D3DCOLOR_COLORVALUE(1.0f,0.0f,1.0f,1.0f), 1.0f, 0 ));
if(!InScene)
{
  V(g_pd3dDevice->BeginScene());
}
	V(pEffect->SetTechnique( "Technique0"));

	D3DXVECTOR2 uv2=uv;
	if(!normalized)
	{
	  uv2.x=uv.x/pTexture->Width;
	  uv2.y=uv.y/pTexture->Height;
	}
	V(pEffect->SetFloatArray( "uv", (float*)&uv2.x,2 ));
	V(pEffect->SetTexture( "sampleTexture", pTexture ));
	switch(((LPDIRECT3DTEXTURE9)pTexture)->Format)
	{
		case D3DFMT_R32F:
		     V(pEffect->SetInt( "channels", 1 ));
		break;
		case D3DFMT_L8:
		     V(pEffect->SetInt( "channels", 1 ));
		break;
  case D3DFMT_X8R8G8B8:
  case D3DFMT_X8B8G8R8:
  case D3DFMT_R8G8B8:
       V(pEffect->SetInt( "channels", 3 ));
		break;
		case D3DFMT_A8R8G8B8:
		case D3DFMT_A8B8G8R8:
       V(pEffect->SetInt( "channels", 4 ));
		break;
		default:
		DBG_HALT;
		break;
	};

	V(g_pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1));
	UINT uPasses;
    V(pEffect->Begin( &uPasses, 0 ));
    //for( UINT uPass = 0; uPass < uPasses; ++uPass )
	V(pEffect->BeginPass( /*uPass*/0 ));

   V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, verts, 5*sizeof(float)));
	V(pEffect->EndPass());
    V(pEffect->End());

if(!InScene)
{
   V(g_pd3dDevice->EndScene());
   V(g_pd3dDevice->Present( NULL, NULL, NULL, NULL ));
}
	V(g_pd3dDevice->SetRenderTarget(0, fbColor));
	V(g_pd3dDevice->SetDepthStencilSurface(fbDepth));

	V(g_pd3dDevice->GetRenderTargetData(m_pTextureSurface,m_pShadowTextureS));//Efficiency

	D3DLOCKED_RECT rect;
	m_pShadowTextureS->LockRect(&rect,0,0 );
	r.x=((BYTE*)rect.pBits)[0];
	r.y=((BYTE*)rect.pBits)[1];
	r.z=((BYTE*)rect.pBits)[2];
	r.w=((BYTE*)rect.pBits)[3];
	m_pShadowTextureS->UnlockRect( );
	SAFE_RELEASE(fbColor);
	SAFE_RELEASE(fbDepth);
 //SAFE_RELEASE(g_pDynamicTexture);
 //SAFE_RELEASE(m_pShadowTexture );
 //SAFE_RELEASE(m_pShadowTextureS );
	//SAFE_RELEASE(m_pTextureSurface);
 //SAFE_RELEASE(pEffect);
	DWORD t2=timeGetTime();
	LOG_PRINT("GPUPick took %f seconds\n",(t2-t1)/1000.0f);
	LOG_PRINT("uv.x=%f\n",uv.x);
	LOG_PRINT("uv.y=%f\n",uv.y);
	LOG_PRINT("x=%f\n",r.x);
	LOG_PRINT("y=%f\n",r.y);
	LOG_PRINT("z=%f\n",r.z);
	LOG_PRINT("w=%f\n",r.w);

  return r;
}
void DrawTexture(LPDIRECT3DBASETEXTURE9* pTexture, int x, int y, int sx, int sy, bool InScene, float tx, float ty, float stx, float sty)
	{
	//D3DVIEWPORT9 viewPort0;
    //V(g_pd3dDevice->GetViewport( &viewPort0 ));

    if(0)
    {
	D3DVIEWPORT9 viewPort1;
	viewPort1.X=0;
	viewPort1.Y=0;
	viewPort1.Width=width;
	viewPort1.Height=height;
	viewPort1.MinZ=0;
	viewPort1.MaxZ=1;
    V(g_pd3dDevice->SetViewport( &viewPort1 ));
    }

#if 1
	D3DVIEWPORT9 viewPort;
    V(g_pd3dDevice->GetViewport( &viewPort ));
if(!InScene)
{
	  V(g_pd3dDevice->BeginScene());
}
float verts[20];
	float qx=(2*float(x))/viewPort.Width-1.0f;
	float qy=1.0f-(2*(float(y+sy)))/viewPort.Height;
	float dx=float(sx)/viewPort.Width*2.0f;
	float dy=float(sy)/viewPort.Height*2.0f;
bool fq=((tx==0.0f)&&(ty==0.0f)&&(stx==1.0f)&&(sty==1.0f));
if(fq)
{
	float verts2[] = {
#if 0
		-1.0f, 1.0f, 0.0f, 0.0f, 0.0f
								,1.0f, 1.0f, 0.0f, 1.0f, 0.0f
							   ,-1.0f,-1.0f, 0.0f, 0.0f, 1.0f
							   , 1.0f,-1.0f, 0.0f, 1.0f, 1.0f
#else
		qx,qy+dy,0.0f,0.0f, 0.0f
		,qx+dx,qy+dy,0.0f,1.0f, 0.0f
		,qx,qy,0.0f,0.0f, 1.0f
		,qx+dx,qy,0.0f,1.0f, 1.0f
#endif
							   };
	memcpy(verts,verts2,20*sizeof(float));
}
else
{
	float verts2[] = {
#if 1
		qx,qy+dy,0.0f,tx,ty
		,qx+dx,qy+dy,0.0f,tx+stx,ty
		,qx,qy,0.0f,tx,ty+sty
		,qx+dx,qy,0.0f,tx+stx,ty+sty
#else
		qx,qy+dy,0.0f,0.0f, 0.0f
		,qx+dx,qy+dy,0.0f,1.0f, 0.0f
		,qx,qy,0.0f,0.0f, 1.0f
		,qx+dx,qy,0.0f,1.0f, 1.0f
#endif
	};
	memcpy(verts,verts2,20*sizeof(float));
}
	D3DXMATRIX matP,matV,matW;
	g_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matP );
	g_pd3dDevice->GetTransform( D3DTS_VIEW, &matV );
	g_pd3dDevice->GetTransform( D3DTS_WORLD, &matW );

	IDirect3DBaseTexture9* Tex0;
	g_pd3dDevice->GetTexture(0,&Tex0);
	g_pd3dDevice->SetTexture(0,*pTexture);
/*
	DebugMsg("x=%f\n",verts[0]);
	DebugMsg("y=%f\n",verts[1]);
	DebugMsg("u=%f\n",verts[3]);
	DebugMsg("v=%f\n",verts[4]);
*/
#if 0
	D3DXVECTOR2 QuadSize(sx,sy);
	D3DXVECTOR2 QuadPosition(x,y);
	D3DXVECTOR2 Viewport(viewPort.Width, viewPort.Height);
if(fq)
{
	QuadPosition.x=Viewport.x-QuadPosition.x-QuadSize.x;
	//QuadPosition.y=Viewport.y-QuadPosition.y-QuadSize.y;

	SetVertex5(verts+0, QuadSize, QuadPosition, Viewport);
#if 0
	SetVertex5(verts+5, QuadSize, QuadPosition, Viewport);
	SetVertex5(verts+10, QuadSize, QuadPosition, Viewport);
	SetVertex5(verts+15, QuadSize, QuadPosition, Viewport);
#endif
}
else
{
	//QuadPosition.x=Viewport.x-QuadPosition.x-QuadSize.x;
	QuadPosition.y=Viewport.y-QuadPosition.y-QuadSize.y;

	SetVertex3(verts+0, QuadSize, QuadPosition, Viewport);
#if 0
	SetVertex3(verts+5, QuadSize, QuadPosition, Viewport);
	SetVertex3(verts+10, QuadSize, QuadPosition, Viewport);
	SetVertex3(verts+15, QuadSize, QuadPosition, Viewport);
#endif
#if 0
	verts[0+1]=Viewport.y-verts[0+1];
	verts[5+1]=Viewport.y-verts[5+1];
	verts[10+1]=Viewport.y-verts[10+1];
	verts[15+1]=Viewport.y-verts[15+1];
#elif 0
	verts[0+1]=1.0f-verts[0+1];
	verts[5+1]=1.0f-verts[5+1];
	verts[10+1]=1.0f-verts[10+1];
	verts[15+1]=1.0f-verts[15+1];
#endif
}
#endif
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	D3DXMATRIX identity;
	D3DXMatrixIdentity( &identity );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &identity );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &identity );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &identity );
	g_pd3dDevice->SetVertexDeclaration( NULL );
	g_pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);
/*
	for(DWORD i=0;i<4;i++)
	{
		//LOG_PRINT("x=%f\n",*(verts+i*5+0));
		//LOG_PRINT("y=%f\n",*(verts+i*5+1));
		//LOG_PRINT("z=%f\n",*(verts+i*5+2));
		//LOG_PRINT("u=%f\n",*(verts+i*5+3));
		//LOG_PRINT("v=%f\n",*(verts+i*5+4));
	}*/
	V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, verts, 5*sizeof(float)));

	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matP );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matV );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matW );
	g_pd3dDevice->SetTexture(0,Tex0);
	if(Tex0)Tex0->Release();
if(!InScene)
{
	V(g_pd3dDevice->EndScene());
	V(g_pd3dDevice->Present( NULL, NULL, NULL, NULL ));
}
 //   V(g_pd3dDevice->SetViewport( &viewPort0 ));
#else
if(g_pQuadEffect==NULL)
{
char fx[]=
"struct VS_INPUT\n"
"{\n"
"	float2 position  : POSITION;\n"
"};\n"
"struct VS_OUTPUT\n"
"{\n"
"	float4 hposition : POSITION;\n"
"	float2 texcoord0 : TEXCOORD0;\n"
"};\n"
"VS_OUTPUT mainVS( VS_INPUT IN )\n"
"{\n"
"	VS_OUTPUT OUT;\n"
"	float4 v = float4( IN.position.x,\n"
"			       IN.position.y,\n"
"					   0,\n"
"					   1.0f );\n"
"	v.xy = sign(v.xy);\n"
"	OUT.hposition = v;\n"
"    OUT.texcoord0.x=(v.x+1.0)/2.0;\n"
"    OUT.texcoord0.y=(v.y-1.0)*-0.5;\n"
"    return OUT;\n"
"}\n"
"struct PS_OUTPUT\n"
"{\n"
"	float4 color : COLOR;\n"
"};\n"
"sampler testTexture;\n"
"PS_OUTPUT mainPS( VS_OUTPUT IN )\n"
"{\n"
"	PS_OUTPUT OUT;\n"
"	OUT.color = tex2D( testTexture, IN.texcoord0 );\n"
"	return OUT;\n"
"}\n"
"technique technique0\n"
"{\n"
"	pass pass0\n"
"	{\n"
"	Sampler[0] = (testTexture); // Needed by pixel shader\n"
"	VertexShader = compile vs_3_0 mainVS( );\n"
"	PixelShader = compile ps_3_0 mainPS( );\n"
"	}\n"
"}";
		  LPD3DXBUFFER pBufferErrors = NULL;
	HRESULT hr = D3DXCreateEffect(g_pd3dDevice, fx, strlen(fx), 0, 0, 0, 0, &g_pQuadEffect,&pBufferErrors);
	if( FAILED(hr) )
	{
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		//LOG_PRINT( "Fx Compile Error\n");
		//LOG_PRINT( (const char*)pCompilErrors);
		DBG_HALT;
	}
}
  LPDIRECT3DSURFACE9 cachedfbColor=NULL;
  LPDIRECT3DSURFACE9 cachedfbDepth=NULL;
  V(g_pd3dDevice->GetRenderTarget(0, &cachedfbColor));
  V(g_pd3dDevice->GetDepthStencilSurface(&cachedfbDepth));
  V(g_pd3dDevice->SetRenderTarget(0, g_fbColor));
  V(g_pd3dDevice->SetDepthStencilSurface(g_fbDepth));

    D3DVIEWPORT9 cachedViewPort;
    D3DVIEWPORT9 viewPort;
    viewPort.MinZ	 = 0.0f;
    viewPort.MaxZ	 = 1.0f;
    viewPort.Width  = sx;
    viewPort.Height = sy;
    viewPort.X	 = x;
    viewPort.Y	 = y;
    g_pd3dDevice->GetViewport( &cachedViewPort );
    V(g_pd3dDevice->SetViewport( &viewPort ));

      V(g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			 D3DCOLOR_COLORVALUE(1.0f,0.0f,1.0f,0.0f), 1.0f, 0 ));
if(!InScene)
{
	  V(g_pd3dDevice->BeginScene());
}
	  LPDIRECT3DBASETEXTURE9 pCachedTexture;
	g_pd3dDevice->GetTexture(0, &pCachedTexture);
	g_pd3dDevice->SetTexture(0, pTexture);
	g_pQuadEffect->SetTechnique( "technique0" );
	UINT uPasses;
    g_pQuadEffect->Begin( &uPasses, 0 );
    for( UINT uPass = 0; uPass < uPasses; ++uPass )
    {
	g_pQuadEffect->BeginPass( uPass );
		g_pd3dDevice->SetVertexDeclaration( NULL );
		g_pd3dDevice->SetFVF(D3DFVF_XYZ);
		static float verts[] = {-1.0f, 1.0f, 0.0f,1.0f, 1.0f, 0.0f,-1.0f,-1.0f, 0.0f, 1.0f,-1.0f, 0.0f};
		V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, verts, 3*sizeof(float)));
	g_pQuadEffect->EndPass();
    }
    g_pQuadEffect->End();
if(!InScene)
{
	V(g_pd3dDevice->EndScene());
	V(g_pd3dDevice->Present( NULL, NULL, NULL, NULL ));
}
   g_pd3dDevice->SetTexture(0, pCachedTexture);
   SAFE_RELEASE(pCachedTexture);
  V(g_pd3dDevice->SetRenderTarget(0, cachedfbColor));
  V(g_pd3dDevice->SetDepthStencilSurface(cachedfbDepth));
    V(g_pd3dDevice->SetViewport( &cachedViewPort ));
    SAFE_RELEASE(cachedfbColor);
    SAFE_RELEASE(cachedfbDepth);
    SAFE_RELEASE(g_pQuadEffect);
#endif
	}
void SampleTexture(BYTE** vl, DWORD& nl,LPDIRECT3DTEXTURE9 pTexture)
{
	//DWORD t1=timeGetTime();
  D3DXVECTOR4 s=D3DXVECTOR4();

	nl=pTexture->Width*pTexture->Height;
	*vl=new BYTE[nl*4];
	//DWORD i=0;
	BYTE* pT=0;
	D3DLOCKED_RECT rect;
	V(pTexture->LockRect(0,&rect,0,0));
	pT=(BYTE* )rect.pBits;

	for(DWORD y=0;y<pTexture->Height;y++)
	{
	for(DWORD x=0;x<pTexture->Width;x++)
	{
	  s=D3DXVECTOR4(0.0f,0.0f,0.0f,0.0f);
	int idx=pTexture->Width*y+x;

	switch(pTexture->Format)
	{
		 case D3DFMT_R32F:
				     s.x=((float*)pT)[idx*1+0];
		 break;
		 case D3DFMT_L8:
			s.x=pT[idx*1+0];
			s.y=pT[idx*1+0];
			s.z=pT[idx*1+0];
			s.w=pT[idx*1+0];
		break;
		case D3DFMT_X8R8G8B8:
		case D3DFMT_X8B8G8R8:
		case D3DFMT_R8G8B8:
			s.x=pT[idx*3+0];
			s.y=pT[idx*3+1];
			s.z=pT[idx*3+2];
			if(1)//((s.x!=0)||(s.y!=0)||(s.z!=0))
			s.w=255.0f;
			else
			s.w=0.0f;
		break;

		case D3DFMT_A8R8G8B8:
		case D3DFMT_A8B8G8R8:
			s.x=pT[idx*4+0];
			s.y=pT[idx*4+1];
			s.z=pT[idx*4+2];
			s.w=pT[idx*4+3];
		break;
		default:
		DBG_HALT;
		break;
	};
	(*vl)[idx*4+3]=s.w;
	(*vl)[idx*4+1]=s.x;
	(*vl)[idx*4+2]=s.y;
	(*vl)[idx*4+0]=s.z;
	}}
	V(pTexture->UnlockRect(0));
	/*
	DWORD t2=timeGetTime();
	LOG_PRINT("Pick took %f seconds\n",(t2-t1)/1000.0f);
	LOG_PRINT("uv.x=%f\n",uv.x);
	LOG_PRINT("uv.y=%f\n",uv.y);
	LOG_PRINT("x=%f\n",s.x);
	LOG_PRINT("y=%f\n",s.y);
	LOG_PRINT("z=%f\n",s.z);
	LOG_PRINT("w=%f\n",s.w);
	*/
}
void GPUSampleTexture(BYTE** vl, DWORD& nl, LPDIRECT3DTEXTURE9 pTexture, bool InScene)//Slow and returns wrong value
{
 LPDIRECT3DSURFACE9 fbColor;
 LPDIRECT3DSURFACE9 fbDepth;
 D3DVIEWPORT9 cachedViewPort;
D3DVIEWPORT9 newViewPort;
g_pd3dDevice->GetViewport( &cachedViewPort );
newViewPort.X=0;
newViewPort.Y=0;
newViewPort.Width=pTexture->Width;
newViewPort.Height=pTexture->Height;
newViewPort.MinZ=0;
newViewPort.MaxZ=1;
V(g_pd3dDevice->SetViewport( &newViewPort ));

  const char* fx=   "\n" \
	     "int channels;\n" \
	     "bool changeRBchannels;\n"\
	     "texture sampleTexture;\n" \
	     "sampler2D Sampler = sampler_state\n" \
	     "{\n" \
	     "	  TextureD3DGL = <sampleTexture>;\n" \
	     "#if 1\n" \
	     "	  AddressU=CLAMP;\n" \
	     "	  AddressV=CLAMP;\n" \
	     "	  AddressW=CLAMP;\n" \
	     "	  MagFilter=POINT;\n" \
	     "	  MinFilter=POINT;\n" \
	     "	  MipFilter=NONE;\n" \
	     "#else\n" \
	     "	  AddressU=WRAP;\n" \
	     "	  AddressV=WRAP;\n" \
	     "	  AddressW=WRAP;\n" \
	     "	  MagFilter=LINEAR;\n" \
	     "	  MinFilter=LINEAR;\n" \
	     "	  MipFilter=LINEAR;\n" \
	     "#endif\n" \
	     "};\n" \
	     "struct VS_INPUT\n" \
	     "{\n" \
	     "	  float4 position	: POSITION;\n" \
	     "	float2 texture0 : TEXCOORD0;\n" \
	     "};\n" \
	     "struct VS_OUTPUT\n" \
	     "{\n" \
	     "	  float4 hposition : POSITION;\n" \
	     "	float2 texture0  : TEXCOORD0;\n" \
	     "};\n" \
	     "VS_OUTPUT myvs( const VS_INPUT IN)\n" \
	     "{\n" \
	     "	  VS_OUTPUT OUT;\n" \
	     "	OUT.hposition = IN.position;\n" \
	     "	OUT.texture0 = IN.texture0;\n" \
	     "	return OUT;\n" \
	     "}\n" \
	     "float4 myps( VS_OUTPUT IN ):COLOR0\n" \
	     "{\n" \
	     "	  float4 color = tex2D( Sampler, IN.texture0 );\n" \
	     "	   if(channels==1)\n"\
	     "	   {\n"\
	     "	       color.y=color.x;\n"\
	     "	       color.z=color.x;\n"\
	     "	       color.w=color.x;\n"\
	     "	   }\n"\
	     "	   if(channels==3)\n"\
	     "	    {\n"\
	     "	      //float t=color.z;\n"\
	     "	      //color.z=color.x;\n"\
	     "	      //color.x=t;\n"\
	     "	      color.w=1;\n"\
	     "	    }\n"\
	     "	  return color;\n" \
	     "}\n" \
	     "technique Technique0\n" \
	     "{\n" \
	     "	  pass Pass0\n" \
	     "	  {\n" \
	     "		Lighting = FALSE;\n" \
	     "		CullMode = CCW;\n" \
	     "		VertexShader = compile vs_2_0 myvs();\n" \
	     "		PixelShader  = compile ps_2_0 myps();\n" \
	     "	  }\n" \
	     "}\n";

	nl=pTexture->Width*pTexture->Height;
	*vl=new BYTE[nl*4];
//	int idx=pTexture->Width*y+x;
  //DWORD t1=timeGetTime();
  HRESULT hr;
   static LPD3DXEFFECT pEffect = NULL;
 LPDIRECT3DTEXTURE9	 m_pDynamicTexture  = NULL;
 LPDIRECT3DTEXTURE9	 m_pShadowTexture  = NULL;
 LPDIRECT3DSURFACE9	 m_pShadowTextureS  = NULL;
 LPDIRECT3DSURFACE9	 m_pTextureSurface  = NULL;
 const int RENDERTOSURFACE_WIDTH  = pTexture->Width;
 const int RENDERTOSURFACE_HEIGHT = pTexture->Height;

	V(g_pd3dDevice->GetRenderTarget(0, &fbColor));
	V(g_pd3dDevice->GetDepthStencilSurface(&fbDepth));
 if(pEffect  == NULL)
 {
   LoadEffectFromString(fx,&pEffect);
 }
    //Direct3D9: (ERROR) :Lock is not supported for textures allocated with POOL_DEFAULT unless they are marked D3DUSAGE_DYNAMIC.

   V(
D3DXCreateTexture( g_pd3dDevice,
				    RENDERTOSURFACE_WIDTH,
				    RENDERTOSURFACE_HEIGHT,
							1,
							D3DUSAGE_DYNAMIC,
							D3DFMT_A8R8G8B8,
							D3DPOOL_SYSTEMMEM,
							&m_pShadowTexture );

	   );
	hr = D3DXCreateTexture( g_pd3dDevice,
				    RENDERTOSURFACE_WIDTH,
				    RENDERTOSURFACE_HEIGHT,
							1,
							D3DUSAGE_RENDERTARGET,
							D3DFMT_A8R8G8B8,
							D3DPOOL_DEFAULT,
							&m_pDynamicTexture );
	if( FAILED(hr) )
	{
		LOG_PRINT("Failed to create a texture with the D3DUSAGE_RENDERTARGET usage flag set!\n");
		DBG_HALT;
    }
    m_pDynamicTexture->GetSurfaceLevel( 0, &m_pTextureSurface );
    m_pShadowTexture->GetSurfaceLevel( 0, &m_pShadowTextureS );

    //D3DXMatrixIdentity( &identity );
  D3DXVECTOR4 r=D3DXVECTOR4();
  float verts[20];
  float verts2[] = {
		-1.0f, 1.0f, 0.0f, 0.0f, 0.0f
		,1.0f, 1.0f, 0.0f, 1.0f, 0.0f
		,-1.0f,-1.0f, 0.0f, 0.0f, 1.0f
		, 1.0f,-1.0f, 0.0f, 1.0f, 1.0f
  };
  memcpy(verts,verts2,20*sizeof(float));

	V(g_pd3dDevice->SetRenderTarget(0, m_pTextureSurface));
	V(g_pd3dDevice->SetDepthStencilSurface(NULL));
     V(g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET /*| D3DCLEAR_ZBUFFER*/, D3DCOLOR_COLORVALUE(1.0f,0.0f,1.0f,1.0f), 1.0f, 0 ));
if(!InScene)
{
  V(g_pd3dDevice->BeginScene());
}
	V(pEffect->SetTechnique( "Technique0"));

	V(pEffect->SetTexture( "sampleTexture", pTexture ));
	switch(((LPDIRECT3DTEXTURE9)pTexture)->Format)
	{
		case D3DFMT_R32F:
		     V(pEffect->SetInt( "channels", 1 ));
		break;
		case D3DFMT_L8:
	V(pEffect->SetInt( "channels", 1 ));
		break;
  case D3DFMT_X8R8G8B8:
  case D3DFMT_X8B8G8R8:
  case D3DFMT_R8G8B8:
       V(pEffect->SetInt( "channels", 3 ));
		break;
		case D3DFMT_A8R8G8B8:
		case D3DFMT_A8B8G8R8:
       V(pEffect->SetInt( "channels", 4 ));
		break;
		default:
		DBG_HALT;
		break;
	};

	g_pd3dDevice->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);
	UINT uPasses;
    V(pEffect->Begin( &uPasses, 0 ));
    //for( UINT uPass = 0; uPass < uPasses; ++uPass )
	V(pEffect->BeginPass( /*uPass*/0 ));

   V(g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, verts, 5*sizeof(float)));
	V(pEffect->EndPass());
    V(pEffect->End());

if(!InScene)
{
   V(g_pd3dDevice->EndScene());
   V(g_pd3dDevice->Present( NULL, NULL, NULL, NULL ));
}

	V(g_pd3dDevice->SetViewport( &cachedViewPort ));
	V(g_pd3dDevice->SetRenderTarget(0, fbColor));
	V(g_pd3dDevice->SetDepthStencilSurface(fbDepth));

	V(g_pd3dDevice->GetRenderTargetData(m_pTextureSurface,m_pShadowTextureS));//Efficiency

	D3DLOCKED_RECT rect;
	m_pShadowTextureS->LockRect(&rect,0,0 );
	//DWORD i=0;
	for(DWORD y=0;y<pTexture->Height;y++)
	{
	for(DWORD x=0;x<pTexture->Width;x++)
	{
	      r=D3DXVECTOR4(0.0f,0.0f,0.0f,0.0f);
	       DWORD idx=pTexture->Width*y+x;
	r.x=((BYTE*)rect.pBits)[idx*4+0];
	r.y=((BYTE*)rect.pBits)[idx*4+1];
	r.z=((BYTE*)rect.pBits)[idx*4+2];
	r.w=((BYTE*)rect.pBits)[idx*4+3];
//	  LOG_PRINT("i/nl=%d/%d\n",i,nl);
	(*vl)[idx*4+0]=r.w;
	(*vl)[idx*4+1]=r.z;
	(*vl)[idx*4+2]=r.y;
	(*vl)[idx*4+3]=r.x;

	}}
	m_pShadowTextureS->UnlockRect( );

	SAFE_RELEASE(fbColor);
	SAFE_RELEASE(fbDepth);
	SAFE_RELEASE(m_pDynamicTexture);
	SAFE_RELEASE(m_pShadowTexture );
	SAFE_RELEASE(m_pShadowTextureS );
	SAFE_RELEASE(m_pTextureSurface);
 //SAFE_RELEASE(pEffect);
/*
	DWORD t2=timeGetTime();
	LOG_PRINT("GPUPick took %f seconds\n",(t2-t1)/1000.0f);
	LOG_PRINT("uv.x=%f\n",uv.x);
	LOG_PRINT("uv.y=%f\n",uv.y);
	LOG_PRINT("x=%f\n",r.x);
	LOG_PRINT("y=%f\n",r.y);
	LOG_PRINT("z=%f\n",r.z);
	LOG_PRINT("w=%f\n",r.w);
*/
}

void DeInitD3D9()
{
#ifdef INCLUDECAMERA
 if(g_pCamera!=0) delete g_pCamera;
#endif
 SAFE_RELEASE(g_pd3dxFont);
 SAFE_RELEASE(g_pd3dxSmallFont);
 SAFE_RELEASE(g_pEffect);
 SAFE_RELEASE(g_pMesh);
 SAFE_RELEASE(g_pEffect);
 SAFE_RELEASE(g_pQuadEffect);
 SAFE_RELEASE(g_pd3dDevice);
 SAFE_RELEASE(g_pD3D);
}
void DumpMesh(LPDIRECT3DVERTEXBUFFER9 pVertexBuffer, LPDIRECT3DINDEXBUFFER9 pIndexBuffer, DWORD vertexCount, DWORD faceCount, DWORD dwStride, const char* filename)
{
	ofstream myfile;
	if(filename!=0)
	{
	  myfile.open (filename);
	}
	if(filename==0)
	{
		//LOG_PRINT("vertexCount=%d\n",vertexCount);
	}
	else
	{
	  myfile << "DWORD vertexCount=" << vertexCount << ";\n";
	  myfile << "float vertices[" << vertexCount*3 << "]={\n";
	}

	BYTE *pFirstPosition;
	pVertexBuffer->Lock(0,0,(void **) &pFirstPosition,D3DLOCK_READONLY);
	//pMesh->LockVertexBuffer(0,(void **) &pFirstPosition);
	for (DWORD i = 0; i < vertexCount; i++)
	{
	D3DXVECTOR3 pos=D3DXVECTOR3(((D3DXVECTOR3*)(pFirstPosition+dwStride*i))->x,((D3DXVECTOR3*)(pFirstPosition+dwStride*i))->y,((D3DXVECTOR3*)(pFirstPosition+dwStride*i))->z);
	if(filename==0)
	{
	//LOG_PRINT("pos.x=%f\n",pos.x);
	//LOG_PRINT("pos.y=%f\n",pos.y);
	//LOG_PRINT("pos.z=%f\n",pos.z);
	}
	else
	{
		myfile << pos.x << "f, " << pos.y << "f, " << pos.z << "f,\n";
	}
	}
	pVertexBuffer->Unlock();
	//pMesh->UnlockVertexBuffer();

	if(filename==0)
	{
		//LOG_PRINT("indexCount=%d\n",faceCount*3);
	}
	else
	{
		myfile << "};\n";
	myfile << "DWORD indexCount=" << faceCount*3 << ";\n";
	  myfile << "DWORD indices[" << faceCount*3 << "]={\n";
	}


  D3DINDEXBUFFER_DESC ibdesc;
  pIndexBuffer->GetDesc (&ibdesc);
  D3DFORMAT fmt = ibdesc.Format;

	if(fmt==D3DFMT_INDEX32)
	{
		DWORD* pInd;
		pIndexBuffer->Lock(0,0,(void **) &pInd,D3DLOCK_READONLY);
		//pMesh->LockIndexBuffer(0,(void **) &pInd);
		for (DWORD i = 0; i < faceCount; i++)
		{
		if(filename==0)
		{
			//LOG_PRINT("%d %d %d\n",pInd[3*i+0],pInd[3*i+1],pInd[3*i+2]);
		}
		else
		{
		  myfile << pInd[3*i+0] << ", " << pInd[3*i+1] << ", " << pInd[3*i+2] << ",\n";
		}
		}
		pIndexBuffer->Unlock();
		//pMesh->UnlockIndexBuffer();
	}
	else
	{
		WORD* pInd;
		pIndexBuffer->Lock(0,0,(void **) &pInd,D3DLOCK_READONLY);
		//pMesh->LockIndexBuffer(0,(void **) &pInd);
		for (DWORD i = 0; i < faceCount; i++)
		{
		if(filename==0)
		{
			//LOG_PRINT("%d %d %d\n",pInd[3*i+0],pInd[3*i+1],pInd[3*i+2]);
		}
		else
		{
		  myfile << pInd[3*i+0] << ", " << pInd[3*i+1] << ", " << pInd[3*i+2] << ",\n";
		}
		}
		pIndexBuffer->Unlock();
		//pMesh->UnlockIndexBuffer();
	}
	if(filename!=0)
	{
		myfile << "};\n";
	  myfile.close();
	}
}
void DumpMesh(LPD3DXMESH pMesh, const char* filename)
{
	LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9 pIndexBuffer;
	DWORD vertexCount=pMesh->GetNumVertices();
	DWORD faceCount=pMesh->GetNumFaces();
	DWORD dwStride=pMesh->GetNumBytesPerVertex();
	V(pMesh->GetVertexBuffer( &pVertexBuffer ) );
	V(pMesh->GetIndexBuffer( &pIndexBuffer ) );
	DumpMesh(pVertexBuffer, pIndexBuffer, vertexCount, faceCount, dwStride, filename);
	SAFE_RELEASE(pVertexBuffer);
	SAFE_RELEASE(pIndexBuffer);
}
#if 1
bool SphereInFrustum( float x, float y, float z, float radius)
{
	D3DXPLANE m_frustum[6];
      D3DXMATRIX matWorld, matView, matProjection;
//	V(g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld));
      V(g_pd3dDevice->GetTransform(D3DTS_VIEW,	&matView));
      V(g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProjection));

D3DXMATRIX viewProjection;
D3DXMatrixMultiply( &viewProjection, &matView, &matProjection );
//D3DXMatrixMultiply( &viewProjection, &m_mView, &m_mProjection );
// Left plane
m_frustum[0].a = viewProjection._14 + viewProjection._11;
m_frustum[0].b = viewProjection._24 + viewProjection._21;
m_frustum[0].c = viewProjection._34 + viewProjection._31;
m_frustum[0].d = viewProjection._44 + viewProjection._41;

// Right plane
m_frustum[1].a = viewProjection._14 - viewProjection._11;
m_frustum[1].b = viewProjection._24 - viewProjection._21;
m_frustum[1].c = viewProjection._34 - viewProjection._31;
m_frustum[1].d = viewProjection._44 - viewProjection._41;

// Top plane
m_frustum[2].a = viewProjection._14 - viewProjection._12;
m_frustum[2].b = viewProjection._24 - viewProjection._22;
m_frustum[2].c = viewProjection._34 - viewProjection._32;
m_frustum[2].d = viewProjection._44 - viewProjection._42;

// Bottom plane
m_frustum[3].a = viewProjection._14 + viewProjection._12;
m_frustum[3].b = viewProjection._24 + viewProjection._22;
m_frustum[3].c = viewProjection._34 + viewProjection._32;
m_frustum[3].d = viewProjection._44 + viewProjection._42;

// Near plane
m_frustum[4].a = viewProjection._13;
m_frustum[4].b = viewProjection._23;
m_frustum[4].c = viewProjection._33;
m_frustum[4].d = viewProjection._43;

// Far plane
m_frustum[5].a = viewProjection._14 - viewProjection._13;
m_frustum[5].b = viewProjection._24 - viewProjection._23;
m_frustum[5].c = viewProjection._34 - viewProjection._33;
m_frustum[5].d = viewProjection._44 - viewProjection._43;

// Normalize planes
for ( int i = 0; i < 6; i++ )
{
D3DXPlaneNormalize( &m_frustum[i], &m_frustum[i] );

}


for ( int i = 0; i < 6; i++ )
{
if ( D3DXPlaneDotCoord( &m_frustum[i], &D3DXVECTOR3(x,y,z) ) + radius < 0 )
{
// Outside the frustum, reject it!
return FALSE;

}

}
return TRUE;

}
/*
  ////////////////////////////////////////////////////////////////////////////////////

//										  \\

// This is a simple Frustum Function, which is for DirectX3D, its used for	 \\

// tutorial purposes, and demonstrates how you can implement the Frustum Check	  \\

// using DirectX.  How it works is, you pass in the coordinate for the vertex	  \\

// point your checking, then if its in the frustum, it will return true, else it  \\

// not, so will return false....and you can take the appropriate action.	   \\

//										  \\

\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
*/
bool PointInFrustum(float x, float y, float z)
{

      D3DXMATRIX matWorld, matView, matProjection;



      V(g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld));

      V(g_pd3dDevice->GetTransform(D3DTS_VIEW,	&matView));

      V(g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProjection));

      // I chaned this line, so you could change the projection view here, adjusting

      // the near and far plane...alternatively you can just use the GetTransform

      // DX call, and use the pre-set one.

      //D3DXMatrixPerspectiveFovLH( &matProjection, D3DX_PI/2, 1.0f, 1.0f, 30.0f );



      D3DXMATRIX Cull = matWorld * matView * matProjection;



      D3DXVECTOR4 vNode = D3DXVECTOR4(x,y,z,1);



      D3DXVECTOR4 vTestVert;

      D3DXVec4Transform(&vTestVert, &vNode, &Cull);



      if( abs(vTestVert.x) > abs(vTestVert.w) ) // Outside our X Frustum Plane

	    return false;



      if( abs(vTestVert.y) > abs(vTestVert.w) ) // Outside our Y Frustum Plane

	    return false;



      if( (vTestVert.z<0.0f) || (vTestVert.z>vTestVert.w)) // Outside our z Frustum Plane

	    return false;



      return true;

}
#define isfin _finite
#define isinf _isinf
#define isnan _isnan
/*
#define isfin(X) (!isinf(X) && !isnan(X))
*/
#define SOMESTRANGEFLOATTHING 100000000.0f
HRESULT ComputeBoundingSphere(CONST D3DXVECTOR3 *pFirstPosition0, DWORD NumVertices, DWORD dwStride, D3DXVECTOR3 *pCenter, FLOAT *pRadius)
{
  //LOG_PRINT("implement %s:%d\n",__FUNCTION__,__LINE__);
    BYTE* pFirstPosition=(BYTE*)pFirstPosition0;
    *pRadius=0.0f;
    *pCenter=D3DXVECTOR3(0.0f,0.0f,0.0f);
    DWORD nv=0;
	DWORD iInfinite=0;
	DWORD iNaN=0;
	DWORD iUninitialized=0;
	for (DWORD i = 0; i < NumVertices; i++)
    {
	D3DXVECTOR3 pos=D3DXVECTOR3(((D3DXVECTOR3*)(pFirstPosition+dwStride*i))->x,((D3DXVECTOR3*)(pFirstPosition+dwStride*i))->y,((D3DXVECTOR3*)(pFirstPosition+dwStride*i))->z);
	float l=D3DXVec3Length(&pos);
	if(isfin(pos.x)&&isfin(pos.y)&&isfin(pos.z)&&(l<SOMESTRANGEFLOATTHING))
	{
	(*pCenter).x = (*pCenter).x + pos.x;
	(*pCenter).y = (*pCenter).y + pos.y;
	(*pCenter).z = (*pCenter).z + pos.z;
	nv++;
	}else
	{
		if((isnan(pos.x)||isnan(pos.y)||isnan(pos.z)))
			iNaN++;
		else
		{
		if(!(isfin(pos.x)&&isfin(pos.y)&&isfin(pos.z)))
			iInfinite++;
		}
		if(l>SOMESTRANGEFLOATTHING)
			iUninitialized++;
	}
    }
	if(nv!=NumVertices)
		//LOG_PRINT("Num valid vertices: %d/%d\n",nv,NumVertices);
	if(iInfinite)
		//LOG_PRINT("iInfinite: %d\n",iInfinite);
	if(iNaN)
		//LOG_PRINT("iNaN: %d\n",iNaN);
	if(iUninitialized)
		//LOG_PRINT("iUninitialized: %d\n",iUninitialized);

	if(nv>0)
	{
    (*pCenter).x = (*pCenter).x / nv;
    (*pCenter).y = (*pCenter).y / nv;
    (*pCenter).z = (*pCenter).z / nv;
	}
	if(!(isfin((*pCenter).x)&&isfin((*pCenter).y)&&isfin((*pCenter).z)))*pCenter=D3DXVECTOR3(0.0f,0.0f,0.0f);
	for (DWORD i = 0; i < NumVertices; i++)
    {
	D3DXVECTOR3 pos=D3DXVECTOR3(((D3DXVECTOR3*)(pFirstPosition+dwStride*i))->x,((D3DXVECTOR3*)(pFirstPosition+dwStride*i))->y,((D3DXVECTOR3*)(pFirstPosition+dwStride*i))->z)-(*pCenter);
	if(isfin(pos.x)&&isfin(pos.y)&&isfin(pos.z))
	{
		float l=D3DXVec3Length(&pos);
		if(isfin(l)&&(l<SOMESTRANGEFLOATTHING))
			*pRadius = std::max(*pRadius, l);
	}
    }
/*
    //LOG_PRINT("radius=%f\n",*pRadius);
    //LOG_PRINT("center.x=%f\n",(*pCenter).x);
    //LOG_PRINT("center.y=%f\n",(*pCenter).y);
    //LOG_PRINT("center.z=%f\n",(*pCenter).z);
*/
  return D3D_OK;
}
HRESULT ComputeBoundingBox(CONST D3DXVECTOR3 *pFirstPosition0, DWORD NumVertices, DWORD dwStride, D3DXVECTOR3 *pMin, D3DXVECTOR3 *pMax)
{
  //LOG_PRINT("implement %s:%d\n",__FUNCTION__,__LINE__);
	BYTE* pFirstPosition=(BYTE*)pFirstPosition0;

	//*pMin=D3DXVECTOR3(0.0f,0.0f,0.0f);
    //*pMax=D3DXVECTOR3(0.0f,0.0f,0.0f);
	*pMin = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	*pMax = D3DXVECTOR3(FLT_MIN, FLT_MIN, FLT_MIN);

    for (DWORD i = 0; i < NumVertices; i++)
    {
	D3DXVECTOR3 pos=D3DXVECTOR3(((D3DXVECTOR3*)(pFirstPosition+dwStride*i))->x,((D3DXVECTOR3*)(pFirstPosition+dwStride*i))->y,((D3DXVECTOR3*)(pFirstPosition+dwStride*i))->z);
	float l=D3DXVec3Length(&pos);
	if(isfin(pos.x)&&isfin(pos.y)&&isfin(pos.z)&&(l<SOMESTRANGEFLOATTHING))
	{
	(*pMin).x = std::min((*pMin).x, pos.x);
	(*pMax).x = std::max((*pMax).x, pos.x);
	(*pMin).y = std::min((*pMin).y, pos.y);
	(*pMax).y = std::max((*pMax).y, pos.y);
	(*pMin).z = std::min((*pMin).z, pos.z);
	(*pMax).z = std::max((*pMax).z, pos.z);
	}
    }
	//if(*pMin == D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX))*pMin = D3DXVECTOR3(0.0f,0.0f,0.0f);
	//if(*pMax == D3DXVECTOR3(FLT_MIN, FLT_MIN, FLT_MIN))*pMax = D3DXVECTOR3(0.0f,0.0f,0.0f);
/*
    //LOG_PRINT("std::min.x=%f\n",(*pMin).x);
    //LOG_PRINT("std::min.y=%f\n",(*pMin).y);
    //LOG_PRINT("std::min.z=%f\n",(*pMin).z);
    //LOG_PRINT("std::max.x=%f\n",(*pMax).x);
    //LOG_PRINT("std::max.y=%f\n",(*pMax).y);
    //LOG_PRINT("std::max.z=%f\n",(*pMax).z);
*/
  return D3D_OK;
}
void ComputeBounds(LPDIRECT3DVERTEXBUFFER9 pVertexBuffer, DWORD VertexCount, DWORD VertexSizeInBytes, D3DXVECTOR3* m_min2, D3DXVECTOR3* m_max2, D3DXVECTOR3* m_center2, float* m_radius2, D3DXMATRIX& scale)
{
	float m_scale=1.0f;
	BYTE * vv = 0;
	pVertexBuffer->Lock( 0, VertexCount*VertexSizeInBytes, (void**)&vv, D3DLOCK_READONLY );
	ComputeBoundingBox(
			       (D3DXVECTOR3*)vv,
			       VertexCount,
			       VertexSizeInBytes,
			       m_min2, m_max2);
      ComputeBoundingSphere(
		(D3DXVECTOR3*)vv,
		VertexCount,
		VertexSizeInBytes,
		 m_center2,
		 m_radius2);
	  pVertexBuffer->Unlock();

	  const float SCALECOFFICIENT=1.73f;
	m_scale = SCALECOFFICIENT / (*m_radius2);
	D3DXMatrixScaling(&scale,m_scale,m_scale,m_scale);
    //LOG_PRINT("_min2.x=%f\n",(*m_min2).x);
    //LOG_PRINT("_min2.y=%f\n",(*m_min2).y);
    //LOG_PRINT("_min2.z=%f\n",(*m_min2).z);
    //LOG_PRINT("_max2.x=%f\n",(*m_max2).x);
    //LOG_PRINT("_max2.y=%f\n",(*m_max2).y);
    //LOG_PRINT("_max2.z=%f\n",(*m_max2).z);
    //LOG_PRINT("_radius2=%f\n",*m_radius2);
    //LOG_PRINT("_center2.x=%f\n",(*m_center2).x);
    //LOG_PRINT("_center2.y=%f\n",(*m_center2).y);
    //LOG_PRINT("_center2.z=%f\n",(*m_center2).z);
}
void ComputeBounds(LPD3DXMESH pMesh, D3DXVECTOR3* m_min2, D3DXVECTOR3* m_max2, D3DXVECTOR3* m_center2, float* m_radius2, D3DXMATRIX& scale)
{
	LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
	DWORD VertexCount;
	DWORD VertexSizeInBytes;

	VertexCount=pMesh->GetNumVertices();
	VertexSizeInBytes=pMesh->GetNumBytesPerVertex();
	V(pMesh->GetVertexBuffer( &pVertexBuffer ) );

	ComputeBounds(pVertexBuffer, VertexCount, VertexSizeInBytes, m_min2, m_max2, m_center2, m_radius2, scale);
	SAFE_RELEASE(pVertexBuffer);
}
/*
Source Code

The code below generates a four-component tangent T in which the handedness of the local coordinate system is stored as ±1 in the w-coordinate.
The bitangent vector B is then given by B = (N × T) · Tw.

#include "Vector4D.h"


struct Triangle
{
    unsigned short  index[3];
};


void CalculateTangentArray(long vertexCount, const Point3D *vertex, const Vector3D *normal,
	const Point2D *texcoord, long triangleCount, const Triangle *triangle, Vector4D *tangent)
{
    Vector3D *tan1 = new Vector3D[vertexCount * 2];
    Vector3D *tan2 = tan1 + vertexCount;
    ZeroMemory(tan1, vertexCount * sizeof(Vector3D) * 2);

    for (long a = 0; a < triangleCount; a++)
    {
	long i1 = triangle->index[0];
	long i2 = triangle->index[1];
	long i3 = triangle->index[2];

	const Point3D& v1 = vertex[i1];
	const Point3D& v2 = vertex[i2];
	const Point3D& v3 = vertex[i3];

	const Point2D& w1 = texcoord[i1];
	const Point2D& w2 = texcoord[i2];
	const Point2D& w3 = texcoord[i3];

	float x1 = v2.x - v1.x;
	float x2 = v3.x - v1.x;
	float y1 = v2.y - v1.y;
	float y2 = v3.y - v1.y;
	float z1 = v2.z - v1.z;
	float z2 = v3.z - v1.z;

	float s1 = w2.x - w1.x;
	float s2 = w3.x - w1.x;
	float t1 = w2.y - w1.y;
	float t2 = w3.y - w1.y;

	float r = 1.0F / (s1 * t2 - s2 * t1);
	Vector3D sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
		(t2 * z1 - t1 * z2) * r);
	Vector3D tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
		(s1 * z2 - s2 * z1) * r);

	tan1[i1] += sdir;
	tan1[i2] += sdir;
	tan1[i3] += sdir;

	tan2[i1] += tdir;
	tan2[i2] += tdir;
	tan2[i3] += tdir;

	triangle++;
    }

    for (long a = 0; a < vertexCount; a++)
    {
	const Vector3D& n = normal[a];
	const Vector3D& t = tan1[a];

	// Gram-Schmidt orthogonalize
	tangent[a] = (t - n * Dot(n, t)).Normalize();

	// Calculate handedness
	tangent[a].w = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
    }

    delete[] tan1;
}
*/
/*
Note that DirectX expects to find the structure fields in a specific order;
if you swap two variables, the vertex buffer will not be read in the correct way,
and the geometry will appear corrupted. This is the order you must use:
   1. Position
   2. RHW value (only for already transformed vertices)
   3. Blending values (up to 5)
   4. Vertex normal
   5. Vertex point size
   6. Diffuse color
   7. Specular color
   8. TextureD3DGL coordinates (up to 8 pairs of UV coordinates)
*/
/*
typedef enum _D3DDECLUSAGE
{
    D3DDECLUSAGE_POSITION = 0,
    D3DDECLUSAGE_BLENDWEIGHT,	// 1
    D3DDECLUSAGE_BLENDINDICES,	// 2
    D3DDECLUSAGE_NORMAL,	// 3
    D3DDECLUSAGE_PSIZE, 	// 4
    D3DDECLUSAGE_TEXCOORD,	// 5
    D3DDECLUSAGE_TANGENT,	// 6
    D3DDECLUSAGE_BINORMAL,	// 7
    D3DDECLUSAGE_TESSFACTOR,	// 8
    D3DDECLUSAGE_POSITIONT,	// 9
    D3DDECLUSAGE_COLOR, 	// 10
    D3DDECLUSAGE_FOG,		// 11
    D3DDECLUSAGE_DEPTH, 	// 12
    D3DDECLUSAGE_SAMPLE,	// 13
} D3DDECLUSAGE;
*/
void DecodeDecl(D3DVERTEXELEMENT9 aOldDecl[])//LPD3DXMESH pMesh)
{
	bool bHadPosition = false;
	bool bHadNormal = false;
	bool bHadPSize = false;
	bool bHadColor = false;
    bool bHadTex[9];
    WORD bTexSize[9];
	WORD wTexCount=0;
	for( UINT index = 0; index < 9; ++index )
	{
	bHadTex[index]=false;
	bTexSize[index];
	}
	bool bHadBiNormal = false;
    bool bHadTangent = false;
	/*
	D3DVERTEXELEMENT9 aOldDecl[MAX_FVF_DECL_SIZE];
	//((LPDIRECT3DVERTEXDECLARATION9)pVertexDeclaration)->GetDeclaration(aOldDecl);
	if( pMesh && SUCCEEDED( pMesh->GetDeclaration( aOldDecl ) ) )
    {*/
	for( UINT index = 0; index < GetDeclLength( aOldDecl ); ++index )
	{
	    if( aOldDecl[index].Usage == D3DDECLUSAGE_POSITION )
		bHadPosition = true;
	    if( aOldDecl[index].Usage == D3DDECLUSAGE_NORMAL )
		bHadNormal = true;
	    if( aOldDecl[index].Usage == D3DDECLUSAGE_PSIZE)
		bHadPSize = true;
	    if( aOldDecl[index].Usage == D3DDECLUSAGE_COLOR)
		bHadColor = true;

	    if( aOldDecl[index].Usage == D3DDECLUSAGE_TEXCOORD )
	    {
			bHadTex[wTexCount]=true;
			switch(aOldDecl[index].Type)
			{
				case D3DDECLTYPE_FLOAT1:
					bTexSize[wTexCount]=1*sizeof(float);
					break;
				case D3DDECLTYPE_FLOAT2:
					bTexSize[wTexCount]=2*sizeof(float);
					break;
				case D3DDECLTYPE_FLOAT3:
					bTexSize[wTexCount]=3*sizeof(float);
					break;
				case D3DDECLTYPE_FLOAT4:
					bTexSize[wTexCount]=4*sizeof(float);
					break;
				default:
					bHadTex[wTexCount]=false;
					bTexSize[wTexCount]=0;
					break;
			};
			wTexCount++;
		}

	    if( aOldDecl[index].Usage == D3DDECLUSAGE_BINORMAL )
		bHadBiNormal = true;
	    if( aOldDecl[index].Usage == D3DDECLUSAGE_TANGENT )
		bHadTangent = true;
	}
    //}
	if (bHadPosition) LOG_PRINT("Decl Had Position\n");
	if (bHadNormal) LOG_PRINT("Decl Had Normal\n");
	if (bHadPSize) LOG_PRINT("Decl Had PSize\n");
	if (bHadColor) LOG_PRINT("Decl Had Color\n");
	for( UINT index = 0; index < 9; ++index )
	{
	if(bHadTex[index]) LOG_PRINT("Decl Had Tex%d Float%d\n",index,bTexSize[index]/4);
	}
	if (bHadBiNormal) LOG_PRINT("Decl Had BiNormal\n");
    if (bHadTangent) LOG_PRINT("Decl Had Tangent\n");

}
/*
#define D3DFVF_RESERVED0	0x001
#define D3DFVF_POSITION_MASK	0x400E
#define D3DFVF_XYZ		0x002
#define D3DFVF_XYZRHW		0x004
#define D3DFVF_XYZB1		0x006
#define D3DFVF_XYZB2		0x008
#define D3DFVF_XYZB3		0x00a
#define D3DFVF_XYZB4		0x00c
#define D3DFVF_XYZB5		0x00e
#define D3DFVF_XYZW		0x4002

#define D3DFVF_NORMAL		0x010
#define D3DFVF_PSIZE		0x020
#define D3DFVF_DIFFUSE		0x040
#define D3DFVF_SPECULAR 	0x080

#define D3DFVF_TEXCOUNT_MASK	0xf00
#define D3DFVF_TEXCOUNT_SHIFT	8
#define D3DFVF_TEX0		0x000
#define D3DFVF_TEX1		0x100
#define D3DFVF_TEX2		0x200
#define D3DFVF_TEX3		0x300
#define D3DFVF_TEX4		0x400
#define D3DFVF_TEX5		0x500
#define D3DFVF_TEX6		0x600
#define D3DFVF_TEX7		0x700
#define D3DFVF_TEX8		0x800

#define D3DFVF_LASTBETA_UBYTE4	 0x1000
#define D3DFVF_LASTBETA_D3DCOLOR 0x8000

#define D3DFVF_RESERVED2	 0x6000  // 2 reserved bits

*/
/*
// Macros to set texture coordinate format bits in the FVF id

#define D3DFVF_TEXTUREFORMAT2 0 	// Two floating point values
#define D3DFVF_TEXTUREFORMAT1 3 	// One floating point value
#define D3DFVF_TEXTUREFORMAT3 1 	// Three floating point values
#define D3DFVF_TEXTUREFORMAT4 2 	// Four floating point values

#define D3DFVF_TEXCOORDSIZE3(CoordIndex) (D3DFVF_TEXTUREFORMAT3 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE2(CoordIndex) (D3DFVF_TEXTUREFORMAT2)
#define D3DFVF_TEXCOORDSIZE4(CoordIndex) (D3DFVF_TEXTUREFORMAT4 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE1(CoordIndex) (D3DFVF_TEXTUREFORMAT1 << (CoordIndex*2 + 16))
*/
void DecodeFVF(DWORD FVF)
{
	bool bHadPosition = false;
	bool bHadRHW = false;
    bool bHadB1 = false;
    bool bHadB2 = false;
    bool bHadB3 = false;
    bool bHadB4 = false;
    bool bHadB5 = false;
    bool bHadNormal = false;
    bool bHadPSize = false;
    bool bHadDiffuse = false;
    bool bHadSpecular = false;

	if( FVF & D3DFVF_XYZ )
		bHadPosition = true;
	else if( FVF & D3DFVF_XYZRHW )
		bHadRHW = true;
	else if( FVF & D3DFVF_XYZB1 )
		bHadB1 = true;
	else if( FVF & D3DFVF_XYZB2 )
		bHadB2 = true;
	else if( FVF & D3DFVF_XYZB3 )
		bHadB3 = true;
	else if( FVF & D3DFVF_XYZB4 )
		bHadB4 = true;
	else if( FVF & D3DFVF_XYZB5 )
		bHadB5 = true;

	if( FVF & D3DFVF_NORMAL )
		bHadNormal = true;
	if( FVF & D3DFVF_PSIZE )
		bHadPSize = true;
	if( FVF & D3DFVF_DIFFUSE )
		bHadDiffuse = true;
	if( FVF & D3DFVF_SPECULAR )
		bHadSpecular = true;

	if (bHadPosition) LOG_PRINT("FVF Had Position\n");
	if (bHadRHW) LOG_PRINT("FVF Had RHW\n");
    if (bHadB1) LOG_PRINT("FVF Had B1\n");
    if (bHadB2) LOG_PRINT("FVF Had B2\n");
    if (bHadB3) LOG_PRINT("FVF Had B3\n");
    if (bHadB4) LOG_PRINT("FVF Had B4\n");
    if (bHadB5) LOG_PRINT("FVF Had B5\n");
	if (bHadNormal) LOG_PRINT("FVF Had Normal\n");
	if (bHadPSize) LOG_PRINT("FVF Had PSize\n");
    if (bHadDiffuse) LOG_PRINT("FVF Had Diffuse\n");
    if (bHadSpecular) LOG_PRINT("FVF Had Specular\n");

	DWORD texture = FVF >> 16;
	for (DWORD i=0;i<((FVF&D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);i++)
	{
		DWORD size=0;
		switch (texture & 3)
		{
			case D3DFVF_TEXTUREFORMAT1:
				size=size + sizeof(FLOAT);
				break;
			case D3DFVF_TEXTUREFORMAT2:
				size=size + sizeof(FLOAT)*2;
				break;
			case D3DFVF_TEXTUREFORMAT3:
				size=size + sizeof(FLOAT)*3;
				break;
			case D3DFVF_TEXTUREFORMAT4:
				size=size + sizeof(FLOAT)*4;
				break;
		}
		texture = texture >>2;
		//LOG_PRINT("FVF Had Tex%d Float%d\n",i,size/4);
	}
}
void CreateTangentSpace(LPDIRECT3DVERTEXBUFFER9* pVertexBuffer, LPDIRECT3DINDEXBUFFER9 pIndexBuffer, DWORD VertexSizeInBytes, DWORD VertexCount, DWORD TriangleCount, LPDIRECT3DVERTEXDECLARATION9* ppVertexDeclaration, DWORD& VertexSizeInBytes2)
{
	if(pIndexBuffer==0)return;
	DWORD t1=timeGetTime();
	typedef struct _Vertexpntduv
	{
		D3DXVECTOR3 pos;	 // vertex position
		D3DXVECTOR3 norm;	 // vertex normal
//		DWORD diffuse;	  // Diffuse color of vertex
		D3DXVECTOR2 uv; 	 // texture coordinates
		D3DXVECTOR3 binorm;	 // vertex normal
		D3DXVECTOR3 tang;  // vertex tangent
	} Vertexpntduv;
	const D3DVERTEXELEMENT9 VertexElements[] =
		{
			 {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},

		{ 0, 3 * sizeof(float),D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 6 * sizeof(float),D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 8 * sizeof(float),D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_BINORMAL, 0 },
		{ 0, 11 * sizeof(float),D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TANGENT, 0 },

			 D3DDECL_END()
		};
	V(g_pd3dDevice->CreateVertexDeclaration( VertexElements, ppVertexDeclaration ));

	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer4;
	DWORD m_nvert2=VertexCount;
	DWORD m_ntri2=TriangleCount;
	DWORD sizeofvertex2=sizeof(Vertexpntduv);

	//LOG_PRINT("m_nvert2=%d\n",m_nvert2);
	//LOG_PRINT("m_ntri2=%d\n",m_ntri2);
	//LOG_PRINT("vsib=%d\n",VertexSizeInBytes);
	//LOG_PRINT("sizeofvertex2=%d\n",sizeofvertex2);

	if (FAILED(g_pd3dDevice->CreateVertexBuffer( m_nvert2*sizeof(Vertexpntduv),
			//D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
			D3DUSAGE_DYNAMIC,
			/*Vertexpntduv::FVF*/0,
			D3DPOOL_DEFAULT, &m_pVertexBuffer4, NULL ) ) )
		{
			//LOG_PRINT("Error CreateVertexBuffer\n");
			DBG_HALT;
		}
	BYTE * v = 0;
	(*pVertexBuffer)->Lock( 0, 0/*VertexCount*(*VertexSizeInBytes)*/, (void**)&v, D3DLOCK_READONLY );
	Vertexpntduv *pVert;
	m_pVertexBuffer4->Lock(0,0,(void **) &pVert,D3DLOCK_DISCARD);

	DWORD i=0;
	DWORD vSize = VertexSizeInBytes/sizeof(BYTE);//pMesh->GetNumBytesPerVertex();
	DWORD* nor1=new DWORD[m_nvert2];
	D3DXVECTOR3 *tan1 = new D3DXVECTOR3[m_nvert2 * 2];
	D3DXVECTOR3 *tan2 = tan1 + m_nvert2;
	ZeroMemory(tan1, m_nvert2 * sizeof(D3DXVECTOR3) * 2);

	for(i=0;i<m_nvert2;i++)
	{
	pVert[i].pos.x=((D3DXVECTOR3*)(v+vSize*i))->x;
	pVert[i].pos.y=((D3DXVECTOR3*)(v+vSize*i))->y;
	pVert[i].pos.z=((D3DXVECTOR3*)(v+vSize*i))->z;
//	  pVert[i].diffuse=D3DCOLOR_COLORVALUE( 0.0, 1.0, 0.0, 1.0 );
	pVert[i].norm = D3DXVECTOR3(0.0f,0.0f,0.0f);
	nor1[i]=0;
	}

	m_pVertexBuffer4->Unlock();
	(*pVertexBuffer)->Unlock();

/*
Zero out all vertex normals.
Loop through all faces, and calculate their normals. Add the face normal to the normals of the vertices it refences.
Loop through all vertrices and normalize.
*/
	m_pVertexBuffer4->Lock(0,0,(void **) &pVert,0);

	WORD *pInd16;
	DWORD *pInd32;
				long i1;
				long i2;
				long i3;
  D3DINDEXBUFFER_DESC ibdesc;
  pIndexBuffer->GetDesc (&ibdesc);
  D3DFORMAT fmt = ibdesc.Format;

  if(fmt==D3DFMT_INDEX32)
	pIndexBuffer->Lock(0,0,(void **) &pInd32,D3DLOCK_READONLY);
  else
	pIndexBuffer->Lock(0,0,(void **) &pInd16,D3DLOCK_READONLY);

	  for(unsigned int z=0; z<m_ntri2/*/3*/; z++)
	{
	  D3DXVECTOR3 normal=D3DXVECTOR3(1.0f,1.0f,1.0f);

		if(fmt==D3DFMT_INDEX32)
		{
			i1 = pInd32[3*z];
			i2 = pInd32[3*z+1];
			i3 = pInd32[3*z+2];
		}
		else
		{
			i1 = pInd16[3*z];
			i2 = pInd16[3*z+1];
			i3 = pInd16[3*z+2];
		}

	  if((i1<m_nvert2)&&(i2<m_nvert2)&&(i3<m_nvert2))
	  {
		  normal=GetTriangeNormal(&pVert[i1].pos, &pVert[i2].pos, &pVert[i3].pos);
		  pVert[i1].norm=pVert[i1].norm+normal;
		  pVert[i2].norm=pVert[i2].norm+normal;
		  pVert[i3].norm=pVert[i3].norm+normal;
		  nor1[i1]=nor1[i1]+1;
		  nor1[i2]=nor1[i2]+1;
		  nor1[i3]=nor1[i3]+1;
	  }
	}

	for(i=0; i<m_nvert2; i++)
	{
	 pVert[i].norm.x /= (float)nor1[i];
	 pVert[i].norm.y /= (float)nor1[i];
	 pVert[i].norm.z /= (float)nor1[i];
	 D3DXVec3Normalize(&pVert[i].norm, &pVert[i].norm);
	 pVert[i].uv=D3DXVECTOR2(asinf(pVert[i].norm.x)/D3DX_PI+0.5f,asinf(pVert[i].norm.y)/D3DX_PI+0.5f);
	 pVert[i].tang=D3DXVECTOR3(0.0f,0.0f,0.0f);
	}

	for(unsigned int z=0; z<m_ntri2/*/3*/; z++)
	{
		bool cmp=false;
		if(fmt==D3DFMT_INDEX32)
			  cmp=((pInd32[3*z]<m_nvert2)&&(pInd32[3*z+1]<m_nvert2)&&(pInd32[3*z+2]<m_nvert2));
		else
			  cmp=((pInd16[3*z]<m_nvert2)&&(pInd16[3*z+1]<m_nvert2)&&(pInd16[3*z+2]<m_nvert2));
		if(cmp)
		{
		if(fmt==D3DFMT_INDEX32)
		{
			i1 = pInd32[3*z];
			i2 = pInd32[3*z+1];
			i3 = pInd32[3*z+2];
		}
		else
		{
			i1 = pInd16[3*z];
			i2 = pInd16[3*z+1];
			i3 = pInd16[3*z+2];
		}

			D3DXVECTOR3& v1 = pVert[i1].pos;
			 D3DXVECTOR3& v2 = pVert[i2].pos;
			 D3DXVECTOR3& v3 = pVert[i3].pos;
			const float& w1x = pVert[i1].uv.x;
			const float& w2x = pVert[i2].uv.x;
			const float& w3x = pVert[i3].uv.x;
			const float& w1y = pVert[i1].uv.y;
			const float& w2y = pVert[i2].uv.y;
			const float& w3y = pVert[i3].uv.y;
			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;
			float s1 = w2x - w1x;
			float s2 = w3x - w1x;
			float t1 = w2y - w1y;
			float t2 = w3y - w1y;
			float r = 1.0F / (s1 * t2 - s2 * t1);
			D3DXVECTOR3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r);
			D3DXVECTOR3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r);
			tan1[i1] += sdir;
			tan1[i2] += sdir;
			tan1[i3] += sdir;
			tan2[i1] += tdir;
			tan2[i2] += tdir;
			tan2[i3] += tdir;
		}
	}

	for(i=0; i<m_nvert2; i++)
	{
		 D3DXVECTOR3 n = pVert[i].norm;
		 D3DXVECTOR3 t = tan1[i];
		// Gram-Schmidt orthogonalize
	/*
		D3DXVec3Cross(&vNormal, &v1, &v2);
		D3DXVec3Normalize(&vNormal, &vNormal);
	float viewAngle = D3DXVec3Dot(&curCameraDir, &pImposter->cameraDir);
	*/
		D3DXVECTOR3 n2=(t - n * D3DXVec3Dot(&n, &t));
		D3DXVec3Normalize(&n2,&n2);
		pVert[i].tang = n2;
		// Calculate handedness
		D3DXVECTOR3 vCross;
		D3DXVec3Cross(&vCross, &n, &t);
		float pVertitangw = (D3DXVec3Dot(&vCross, &tan2[i]) < 0.0F) ? -1.0F : 1.0F;

		//The bitangent vector B is then given by B = (N × T) · Tw.
		D3DXVec3Cross(&vCross, &n, &t);
		pVert[i].binorm=vCross*pVertitangw;
	}
	delete[] tan1;
	delete[] nor1;
	m_pVertexBuffer4->Unlock();
	pIndexBuffer->Unlock();
	SAFE_RELEASE(*pVertexBuffer);
	*pVertexBuffer=m_pVertexBuffer4;
	VertexSizeInBytes2=sizeof(Vertexpntduv);
	DWORD t2=timeGetTime();
	//LOG_PRINT("CloneVertexBufferFVF took %f seconds\n",(t2-t1)/1000.0f);
}
void CreateTangentSpace(LPD3DXMESH* pMesh, LPDIRECT3DVERTEXDECLARATION9* ppVertexDeclaration, DWORD& VertexSizeInBytes)
{
	LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9 pIndexBuffer;
	DWORD vertexCount=(*pMesh)->GetNumVertices();
	DWORD faceCount=(*pMesh)->GetNumFaces();
	DWORD dwStride=(*pMesh)->GetNumBytesPerVertex();
	V((*pMesh)->GetVertexBuffer( &pVertexBuffer ) );
	V((*pMesh)->GetIndexBuffer( &pIndexBuffer ) );
//	CreateTangentSpace(&pVertexBuffer, pIndexBuffer, dwStride, vertexCount, faceCount, ppVertexDeclaration);
	CreateTangentSpace(&pVertexBuffer, pIndexBuffer, dwStride, vertexCount, faceCount, ppVertexDeclaration, VertexSizeInBytes);

	SAFE_RELEASE(pVertexBuffer);
	SAFE_RELEASE(pIndexBuffer);
}
HRESULT LoadMesh(const char* file, LPD3DXMESH* ppMesh, DWORD* pNumMaterials, LPDIRECT3DTEXTURE9* ppMeshTextures, D3DMATERIAL9** ppMeshMaterials)
{
	//CHECK_HEAP
	char texture7[256];

	char path7[256];
	path7[0]='\0';
	strcpy(path7,file);
	int l1 = strrchr(path7, '\\')-path7+1;
	int l2 = strrchr(path7, '/')-path7+1;
	if(l1<0) l1=0;
	if(l2<0) l2=0;
	int l=std::max(l1,l2);
	path7[l] = '\0';

	//LOG_PRINT("path %s\n",path7);

	std::vector<const char*> textures;
	LPD3DXBUFFER pMaterialBuffer;
	HRESULT hr = D3DXLoadMeshFromX( file, D3DXMESH_MANAGED, g_pd3dDevice, NULL, &pMaterialBuffer, NULL, pNumMaterials, ppMesh );
	//LOG_PRINT("mesh %s\n",file);
	//LOG_PRINT("*ppMesh %d\n",*ppMesh);
	if ( FAILED( hr ) )
	{
		//LOG_PRINT("%s failed\n",file);
		return hr;
	}

	//LOG_PRINT("*pNumMaterials=%d\n",*pNumMaterials);
	//LOG_PRINT("pMaterialBuffer=%d\n",pMaterialBuffer);

    if( pMaterialBuffer && (*pNumMaterials) > 0 )
    {

	// Store material and texture information
#if __D3DGL__
	D3DXMATERIAL* pMaterials = (D3DXMATERIAL*)pMaterialBuffer;//->GetBufferPointer();
#else
	D3DXMATERIAL* pMaterials = (D3DXMATERIAL*)pMaterialBuffer->GetBufferPointer();
#endif


	(*ppMeshMaterials) = new D3DMATERIAL9[*pNumMaterials];
//LPDIRECT3DTEXTURE9 *meshTextures  = new LPDIRECT3DTEXTURE9[numMaterials];
	//LOG_PRINT("ppMeshTextures=%d\n",ppMeshTextures);

	ppMeshTextures = new LPDIRECT3DTEXTURE9[*pNumMaterials];

	//LOG_PRINT("ppMeshTextures=%d\n",ppMeshTextures);

		//LOG_PRINT("*pNumMaterials %d\n",*pNumMaterials);

	// Copy the materials and textures from the buffer to the member arrays
	for ( DWORD i = 0; i < *pNumMaterials; i++ )
	{
		(*ppMeshMaterials)[i] = pMaterials[i].MatD3D;
		(*ppMeshMaterials)[i].Ambient = (*ppMeshMaterials)[i].Diffuse;

		//LOG_PRINT("pMaterials[%d].pTextureFilename=%s\n",i,pMaterials[i].pTextureFilename);
		// Create the texture if it exists
		ppMeshTextures[i] = NULL;
		if( pMaterials[i].pTextureFilename && (strcmp("pMaterials[i].pTextureFilename","")!=0))
		{
			bool skip=false;
			for ( DWORD j = 0; j < textures.size(); j++ )
			{
			  if(strcmp(textures[j],pMaterials[i].pTextureFilename)==0)
			  {
				skip=true;
				break;
			  }
			}
			if(!skip)
			{
			textures.push_back(pMaterials[i].pTextureFilename);
			  texture7[0]='\0';
			strcat(texture7,path7);
			//LOG_PRINT("texture %s\n",texture7);
			strcat(texture7,pMaterials[i].pTextureFilename);
			//LOG_PRINT("texture %s\n",texture7);




	    hr = D3DXCreateTextureFromFileEx(g_pd3dDevice,
					texture7,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					0,
					0,
					D3DFMT_UNKNOWN,
					D3DPOOL_MANAGED,
					D3DX_FILTER_LINEAR,
					D3DX_FILTER_LINEAR,
					0,
					NULL,
					NULL,
					&ppMeshTextures[i]
					);
				if (FAILED(hr))
				{
					ppMeshTextures[i] = NULL;
					//LOG_PRINT("%s failed\n",pMaterials[i].pTextureFilename);
										continue;
				}
				else
								{
									//LOG_PRINT("%s succeeded\n",pMaterials[i].pTextureFilename);
								}






			}
		}
	}
}
	//CHECK_HEAP
	// Don’t need this no more!
//???D3DGL	pMaterialBuffer->Release();
	return D3D_OK;
}
HRESULT RenderMesh(LPD3DXMESH ppMesh, DWORD pNumMaterials, LPDIRECT3DTEXTURE9* ppMeshTextures, D3DMATERIAL9* pMeshMaterials)
{
	HRESULT hr=D3D_OK;
	for ( DWORD i = 0; i < pNumMaterials; i++ )
	{
	g_pd3dDevice->SetMaterial( &pMeshMaterials[i] );
	g_pd3dDevice->SetTexture( 0, ppMeshTextures[i] );
	hr=ppMesh->DrawSubset( i );
	if(FAILED(hr))
		return hr;
	}
	return hr;
}
HRESULT RenderMesh(LPDIRECT3DVERTEXBUFFER9 pVertexBuffer, LPDIRECT3DINDEXBUFFER9 pIndexBuffer, DWORD VertexSizeInBytes, DWORD VertexCount, DWORD TriangleCount, LPDIRECT3DVERTEXDECLARATION9 pVertexDeclaration, DWORD FVF)
{
	HRESULT hr=D3D_OK;
	if(pVertexDeclaration!=0)
	{
		g_pd3dDevice->SetFVF(0);
		g_pd3dDevice->SetVertexDeclaration(pVertexDeclaration);
	}
	else if(FVF!=0)
	{
		g_pd3dDevice->SetVertexDeclaration(NULL);
		g_pd3dDevice->SetFVF(FVF);
	}
	g_pd3dDevice->SetStreamSource(0,pVertexBuffer, 0, VertexSizeInBytes);
	if (pIndexBuffer != NULL)
	{
		g_pd3dDevice->SetIndices(pIndexBuffer);
		hr=g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, VertexCount, 0, TriangleCount);
	}
	else
		hr=g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, TriangleCount);

	if(pVertexDeclaration!=0)
		g_pd3dDevice->SetVertexDeclaration(NULL);
	else if(FVF!=0)
		g_pd3dDevice->SetFVF(0);

	return hr;
}
HRESULT RenderMesh(LPD3DXMESH pMesh, DWORD VertexSizeInBytes, LPDIRECT3DVERTEXDECLARATION9 pVertexDeclaration, DWORD FVF)
{
	//ei toimi???
	LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9 pIndexBuffer;
	DWORD vertexCount=pMesh->GetNumVertices();
	DWORD faceCount=pMesh->GetNumFaces();
	DWORD dwStride=pMesh->GetNumBytesPerVertex();
	V(pMesh->GetVertexBuffer( &pVertexBuffer ) );
	V(pMesh->GetIndexBuffer( &pIndexBuffer ) );
	HRESULT hr=RenderMesh(pVertexBuffer, pIndexBuffer, VertexSizeInBytes, vertexCount, faceCount, pVertexDeclaration,FVF);
	SAFE_RELEASE(pVertexBuffer);
	SAFE_RELEASE(pIndexBuffer);
	return hr;
}
HRESULT CloneMesh(LPD3DXMESH pMesh, DWORD Options, CONST D3DVERTEXELEMENT9 *pDeclaration/*, LPDIRECT3DDEVICE9 pD3DDevice*/, LPD3DXMESH* ppCloneMesh)
{
  //LOG_PRINT("%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);
	//LOG_PRINT("1 pMesh=%d\n",pMesh);

	if( pMesh==0) return E_FAIL;
	//LOG_PRINT("2 pMesh=%d\n",pMesh);

	D3DVERTEXELEMENT9 Decl[MAX_FVF_DECL_SIZE];

	D3DVERTEXELEMENT9 Decl2[MAX_FVF_DECL_SIZE];
	DWORD n=/*D3DX*/GetDeclLength(pDeclaration);
	memcpy(Decl2,pDeclaration,(n+1)*sizeof(D3DVERTEXELEMENT9));
	//LOG_PRINT("3 pMesh=%d\n",pMesh);

	pMesh->GetDeclaration( Decl);

	//LOG_PRINT("4 pMesh=%d\n",pMesh);

	//LOG_PRINT("Decl:\n");
	DecodeDecl(Decl);
	//LOG_PRINT("Decl2:\n");
	DecodeDecl(Decl2);

	DWORD VertexSizeInBytes=VertexSizeInBytesFromDecl(Decl);

	//LOG_PRINT("VertexSizeInBytes=%d\n",VertexSizeInBytes);
	//LOG_PRINT("5 pMesh=%d\n",pMesh);
	DWORD nf=pMesh->GetNumFaces();
	//LOG_PRINT("nf=%d\n",nf);
	DWORD nv=pMesh->GetNumVertices();
	//LOG_PRINT("nv=%d\n",nv);

	if(nv>65535) Options|=D3DXMESH_32BIT;

	HRESULT hr=D3DXCreateMesh( nf,nv, Options, pDeclaration, g_pd3dDevice, ppCloneMesh );

	//LOG_PRINT("D3DXCreateMesh hr=%d\n",hr);

	//LOG_PRINT("pMesh=%d\n",pMesh);
	//LOG_PRINT("(*ppCloneMesh)=%d\n",(*ppCloneMesh));
	DWORD dwStride1=pMesh->GetNumBytesPerVertex();
	DWORD dwStride2=(*ppCloneMesh)->GetNumBytesPerVertex();

	//LOG_PRINT("dwStride1=%d\n",dwStride1);
	//LOG_PRINT("dwStride2=%d\n",dwStride2);

	BYTE *pFirstPosition1;
	BYTE *pFirstPosition2;
	hr=pMesh->LockVertexBuffer(0,(void **) &pFirstPosition1);
	//LOG_PRINT("pMesh->LockVertexBuffer hr=%d\n",hr);

	hr=(*ppCloneMesh)->LockVertexBuffer(0,(void **) &pFirstPosition2);
	//LOG_PRINT("(*ppCloneMesh)->LockVertexBuffer hr=%d\n",hr);

	CloneVertexBuffer(dwStride1, dwStride2, Decl, Decl2, pFirstPosition1, pFirstPosition2, nv);
//	//LOG_PRINT("CloneVertexBuffer hr=%d\n",hr);
	(*ppCloneMesh)->UnlockVertexBuffer();
	pMesh->UnlockVertexBuffer();

	LPDIRECT3DINDEXBUFFER9 pIndexBuffer;
	V(pMesh->GetIndexBuffer( &pIndexBuffer ) );
	LPDIRECT3DINDEXBUFFER9 pCloneIndexBuffer;
	V((*ppCloneMesh)->GetIndexBuffer( &pCloneIndexBuffer ) );
#if 0
	WORD *pInd16=0;
	DWORD *pInd32=0;
	WORD *pCloneInd16=0;
	DWORD *pCloneInd32=0;
	LockIndexBuffer(pIndexBuffer, pInd16, pInd32);
	LockIndexBuffer(pCloneIndexBuffer, pCloneInd16, pCloneInd32);
	dwStride1=(pInd16!=0)?4:8;
	dwStride2=(pCloneInd16!=0)?4:8;
	pFirstPosition1=(BYTE)(pInd16!=0)?pInd16:pInd32;
	pFirstPosition2=(BYTE)(pCloneInd16!=0)?pCloneInd16:pCloneInd32;
	CloneIndexBuffer(dwStride1, dwStride2, pFirstPosition1, pFirstPosition2, nf);
#else
	BYTE* ppInd;
	DWORD strideInBytes;
	LockIndexBuffer(pIndexBuffer, &ppInd, &strideInBytes);
	BYTE* ppCloneInd;
	DWORD cloneStrideInBytes;
	LockIndexBuffer(pCloneIndexBuffer, &ppCloneInd, &cloneStrideInBytes);
	CloneIndexBuffer(strideInBytes, cloneStrideInBytes, ppInd, ppCloneInd, nf);
#endif

	pIndexBuffer->Unlock();
	SAFE_RELEASE(pIndexBuffer);
	pCloneIndexBuffer->Unlock();
	SAFE_RELEASE(pCloneIndexBuffer);
	return D3D_OK;
}
HRESULT CloneMeshFVF(LPD3DXMESH pMesh, DWORD Options, DWORD FVF2/*, LPDIRECT3DDEVICE9 pD3DDevice*/, LPD3DXMESH* ppCloneMesh)
{
  //LOG_PRINT("%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);
	if( pMesh==0) return E_FAIL;
	DWORD FVF=pMesh->GetFVF();
	//D3DVERTEXELEMENT9 Decl[MAX_FVF_DECL_SIZE];
	//pMesh->GetDeclaration(Decl);

	DWORD VertexSizeInBytes=VertexSizeInBytesFromFVF(FVF);

	DWORD nf=pMesh->GetNumFaces();
	DWORD nv=pMesh->GetNumVertices();

	if(nv>65535) Options|=D3DXMESH_32BIT;

	HRESULT hr=D3DXCreateMeshFVF( nf,nv, Options, FVF, g_pd3dDevice, ppCloneMesh );

	DWORD dwStride1=pMesh->GetNumBytesPerVertex();
	DWORD dwStride2=(*ppCloneMesh)->GetNumBytesPerVertex();
	BYTE *pFirstPosition1;
	BYTE *pFirstPosition2;
	pMesh->LockVertexBuffer(0,(void **) &pFirstPosition1);
	(*ppCloneMesh)->LockVertexBuffer(0,(void **) &pFirstPosition2);
	CloneVertexBuffer(dwStride1, dwStride2, FVF, FVF2, pFirstPosition1, pFirstPosition2, nv);
	(*ppCloneMesh)->UnlockVertexBuffer();
	pMesh->UnlockVertexBuffer();
	LPDIRECT3DINDEXBUFFER9 pIndexBuffer;
	V(pMesh->GetIndexBuffer( &pIndexBuffer ) );
	LPDIRECT3DINDEXBUFFER9 pCloneIndexBuffer;
	V((*ppCloneMesh)->GetIndexBuffer( &pCloneIndexBuffer ) );

#if 0
	WORD *pInd16=0;
	DWORD *pInd32=0;
	WORD *pCloneInd16=0;
	DWORD *pCloneInd32=0;
	LockIndexBuffer(pIndexBuffer, pInd16, pInd32);
	LockIndexBuffer(pCloneIndexBuffer, pCloneInd16, pCloneInd32);
	dwStride1=(pInd16!=0)?4:8;
	dwStride2=(pCloneInd16!=0)?4:8;
	pFirstPosition1=(BYTE)(pInd16!=0)?pInd16:pInd32;
	pFirstPosition2=(BYTE)(pCloneInd16!=0)?pCloneInd16:pCloneInd32;
	CloneIndexBuffer(dwStride1, dwStride2, pFirstPosition1, pFirstPosition2, nf);
#else
	BYTE* ppInd;
	DWORD strideInBytes;
	LockIndexBuffer(pIndexBuffer, &ppInd, &strideInBytes);
	BYTE* ppCloneInd;
	DWORD cloneStrideInBytes;
	LockIndexBuffer(pCloneIndexBuffer, &ppCloneInd, &cloneStrideInBytes);
	CloneIndexBuffer(strideInBytes, cloneStrideInBytes, ppInd, ppCloneInd, nf);
#endif


	pIndexBuffer->Unlock();
	SAFE_RELEASE(pIndexBuffer);
	pCloneIndexBuffer->Unlock();
	SAFE_RELEASE(pCloneIndexBuffer);
	return D3D_OK;
}
UINT /*WINAPI D3DX*/GetDeclLength(
  CONST D3DVERTEXELEMENT9 * pDecl
)
{
	UINT Length=0;
	bool loop=true;
	while(loop)
	{
		if(
			(pDecl[Length].Stream==0xFF)&&
			(pDecl[Length].Offset==0)&&
			(pDecl[Length].Type==D3DDECLTYPE_UNUSED)&&
			(pDecl[Length].Method==0)&&
			(pDecl[Length].Usage==0)&&
			(pDecl[Length].UsageIndex==0))
			return Length;
		else
			Length++;
	}
	return Length;
}
HRESULT /*WINAPI D3DX*/DeclaratorFromFVF(
  DWORD FVF,
  D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE]
)
{
	WORD offset=0;
	DWORD Length=0;
	if( FVF & D3DFVF_XYZ )
	{
			Declaration[Length].Stream=0;
			Declaration[Length].Offset=offset;
			Declaration[Length].Type=D3DDECLTYPE_FLOAT3;
			Declaration[Length].Method=D3DDECLMETHOD_DEFAULT;
			Declaration[Length].Usage=D3DDECLUSAGE_POSITION;
			Declaration[Length++].UsageIndex=0;
			offset+=3*sizeof(float);
	}
/*
	else if( FVF & D3DFVF_XYZRHW )
	else if( FVF & D3DFVF_XYZB1 )
	else if( FVF & D3DFVF_XYZB2 )
	else if( FVF & D3DFVF_XYZB3 )
	else if( FVF & D3DFVF_XYZB4 )
	else if( FVF & D3DFVF_XYZB5 )
*/
	if( FVF & D3DFVF_NORMAL )
	{
			Declaration[Length].Stream=0;
			Declaration[Length].Offset=offset;
			Declaration[Length].Type=D3DDECLTYPE_FLOAT3;
			Declaration[Length].Method=D3DDECLMETHOD_DEFAULT;
			Declaration[Length].Usage=D3DDECLUSAGE_NORMAL;
			Declaration[Length++].UsageIndex=0;
			offset+=3*sizeof(float);
	}
	if( FVF & D3DFVF_PSIZE )
	{
			Declaration[Length].Stream=0;
			Declaration[Length].Offset=offset;
			Declaration[Length].Type=D3DDECLTYPE_FLOAT1;
			Declaration[Length].Method=D3DDECLMETHOD_DEFAULT;
			Declaration[Length].Usage=D3DDECLUSAGE_PSIZE;
			Declaration[Length++].UsageIndex=0;
			offset+=1*sizeof(float);
	}
	if( FVF & D3DFVF_DIFFUSE )
	{
			Declaration[Length].Stream=0;
			Declaration[Length].Offset=offset;
			Declaration[Length].Type=D3DDECLTYPE_FLOAT3;
			Declaration[Length].Method=D3DDECLMETHOD_DEFAULT;
			Declaration[Length].Usage=D3DDECLUSAGE_POSITION;
			Declaration[Length++].UsageIndex=0;
			offset+=sizeof(DWORD);
	}
	//if( FVF & D3DFVF_SPECULAR )

	WORD texCoordCount=0;
	for( UINT index = 0; index < 9; ++index )
	{
	if(FVF&D3DFVF_TEXCOORDSIZE1(index))
	{
			Declaration[Length].Stream=0;
			Declaration[Length].Offset=offset;
			Declaration[Length].Method=D3DDECLMETHOD_DEFAULT;
			Declaration[Length].Usage=D3DDECLUSAGE_TEXCOORD;

			Declaration[Length].UsageIndex=index;
			Declaration[Length++].Type=D3DDECLTYPE_FLOAT1;
			offset+=1*sizeof(float);
			texCoordCount++;
	}
	if(FVF&D3DFVF_TEXCOORDSIZE2(index))
	{
			Declaration[Length].Stream=0;
			Declaration[Length].Offset=offset;
			Declaration[Length].Method=D3DDECLMETHOD_DEFAULT;
			Declaration[Length].Usage=D3DDECLUSAGE_TEXCOORD;

			Declaration[Length].UsageIndex=index;
			Declaration[Length++].Type=D3DDECLTYPE_FLOAT2;
			offset+=2*sizeof(float);
			texCoordCount++;
	}
	if(FVF&D3DFVF_TEXCOORDSIZE3(index))
	{
			Declaration[Length].Stream=0;
			Declaration[Length].Offset=offset;
			Declaration[Length].Method=D3DDECLMETHOD_DEFAULT;
			Declaration[Length].Usage=D3DDECLUSAGE_TEXCOORD;

			Declaration[Length].UsageIndex=index;
			Declaration[Length++].Type=D3DDECLTYPE_FLOAT3;
			offset+=3*sizeof(float);
			texCoordCount++;
	}
	if(FVF&D3DFVF_TEXCOORDSIZE4(index))
	{
			Declaration[Length].Stream=0;
			Declaration[Length].Offset=offset;
			Declaration[Length].Method=D3DDECLMETHOD_DEFAULT;
			Declaration[Length].Usage=D3DDECLUSAGE_TEXCOORD;

			Declaration[Length].UsageIndex=index;
			Declaration[Length++].Type=D3DDECLTYPE_FLOAT4;
			offset+=4*sizeof(float);
			texCoordCount++;
	}
	}
/*
	switch(texCoordCount)
	{
		case 1:
			assert(FVF&D3DFVF_TEX1);
			break;
		case 2:
			assert(FVF&D3DFVF_TEX2);
			break;
		case 3:
			assert(FVF&D3DFVF_TEX3);
			break;
		case 4:
			assert(FVF&D3DFVF_TEX4);
			break;
		case 5:
			assert(FVF&D3DFVF_TEX5);
			break;
		case 6:
			assert(FVF&D3DFVF_TEX6);
			break;
		case 7:
			assert(FVF&D3DFVF_TEX7);
			break;
		case 8:
			assert(FVF&D3DFVF_TEX8);
			break;

	}
*/

	Declaration[Length].Stream=0xFF;
	Declaration[Length].Offset=0;
	Declaration[Length].Type=D3DDECLTYPE_UNUSED;
	Declaration[Length].Method=0;
	Declaration[Length].Usage=0;
	Declaration[Length++].UsageIndex=0;

	return D3D_OK;
}
HRESULT /*WINAPI D3DX*/FVFFromDeclarator(
  CONST LPD3DVERTEXELEMENT9 * pDeclaration,
  DWORD * pFVF
)
{
	*pFVF=0;
	WORD CoordIndex=0;
	for( UINT index = 0; index < GetDeclLength( *pDeclaration ); ++index )
	{
	    if( (*pDeclaration[index]).Usage == D3DDECLUSAGE_POSITION )
		*pFVF|=D3DFVF_XYZ;
	    if( (*pDeclaration[index]).Usage == D3DDECLUSAGE_NORMAL )
		*pFVF|=D3DFVF_NORMAL;
	    if( (*pDeclaration[index]).Usage == D3DDECLUSAGE_PSIZE)
		*pFVF|=D3DFVF_PSIZE;
	    if( (*pDeclaration[index]).Usage == D3DDECLUSAGE_COLOR)
		*pFVF|=D3DFVF_DIFFUSE;

	    if( (*pDeclaration[index]).Usage == D3DDECLUSAGE_TEXCOORD )
	    {
			switch((*pDeclaration[index]).Type)
			{
				case D3DDECLTYPE_FLOAT1:
					*pFVF|=D3DFVF_TEXCOORDSIZE1(CoordIndex++);
					break;
				case D3DDECLTYPE_FLOAT2:
					*pFVF|=D3DFVF_TEXCOORDSIZE2(CoordIndex++);
					break;
				case D3DDECLTYPE_FLOAT3:
					*pFVF|=D3DFVF_TEXCOORDSIZE3(CoordIndex++);
					break;
				case D3DDECLTYPE_FLOAT4:
					*pFVF|=D3DFVF_TEXCOORDSIZE4(CoordIndex++);
					break;
			};
		}

	}
	switch(CoordIndex)
	{
		case 1:
			*pFVF|=D3DFVF_TEX1;
			break;
		case 2:
			*pFVF|=D3DFVF_TEX2;
			break;
		case 3:
			*pFVF|=D3DFVF_TEX3;
			break;
		case 4:
			*pFVF|=D3DFVF_TEX4;
			break;
		case 5:
			*pFVF|=D3DFVF_TEX5;
			break;
		case 6:
			*pFVF|=D3DFVF_TEX6;
			break;
		case 7:
			*pFVF|=D3DFVF_TEX7;
			break;
		case 8:
			*pFVF|=D3DFVF_TEX8;
			break;
		/*default:
			*pFVF|=D3DFVF_TEX0;
			break;*/
	}

	return D3D_OK;
}
/*
//-------------------------------------------------------------------

// Flexible vertex format bits
//
#define D3DFVF_RESERVED0	0x001
#define D3DFVF_POSITION_MASK	0x400E
#define D3DFVF_XYZ		0x002
#define D3DFVF_XYZRHW		0x004
#define D3DFVF_XYZB1		0x006
#define D3DFVF_XYZB2		0x008
#define D3DFVF_XYZB3		0x00a
#define D3DFVF_XYZB4		0x00c
#define D3DFVF_XYZB5		0x00e
#define D3DFVF_XYZW		0x4002

#define D3DFVF_NORMAL		0x010
#define D3DFVF_PSIZE		0x020
#define D3DFVF_DIFFUSE		0x040
#define D3DFVF_SPECULAR 	0x080

#define D3DFVF_TEXCOUNT_MASK	0xf00
#define D3DFVF_TEXCOUNT_SHIFT	8
#define D3DFVF_TEX0		0x000
#define D3DFVF_TEX1		0x100
#define D3DFVF_TEX2		0x200
#define D3DFVF_TEX3		0x300
#define D3DFVF_TEX4		0x400
#define D3DFVF_TEX5		0x500
#define D3DFVF_TEX6		0x600
#define D3DFVF_TEX7		0x700
#define D3DFVF_TEX8		0x800

#define D3DFVF_LASTBETA_UBYTE4	 0x1000
#define D3DFVF_LASTBETA_D3DCOLOR 0x8000

#define D3DFVF_RESERVED2	 0x6000  // 2 reserved bits

//---------------------------------------------------------------------
// Vertex Shaders
//

// Vertex shader declaration

// Vertex element semantics
//
typedef enum _D3DDECLUSAGE
{
    D3DDECLUSAGE_POSITION = 0,
    D3DDECLUSAGE_BLENDWEIGHT,	// 1
    D3DDECLUSAGE_BLENDINDICES,	// 2
    D3DDECLUSAGE_NORMAL,	// 3
    D3DDECLUSAGE_PSIZE, 	// 4
    D3DDECLUSAGE_TEXCOORD,	// 5
    D3DDECLUSAGE_TANGENT,	// 6
    D3DDECLUSAGE_BINORMAL,	// 7
    D3DDECLUSAGE_TESSFACTOR,	// 8
    D3DDECLUSAGE_POSITIONT,	// 9
    D3DDECLUSAGE_COLOR, 	// 10
    D3DDECLUSAGE_FOG,		// 11
    D3DDECLUSAGE_DEPTH, 	// 12
    D3DDECLUSAGE_SAMPLE,	// 13
} D3DDECLUSAGE;

#define MAXD3DDECLUSAGE 	D3DDECLUSAGE_SAMPLE
#define MAXD3DDECLUSAGEINDEX	15
#define MAXD3DDECLLENGTH	64 // does not include "end" marker vertex element

typedef enum _D3DDECLMETHOD
{
    D3DDECLMETHOD_DEFAULT = 0,
    D3DDECLMETHOD_PARTIALU,
    D3DDECLMETHOD_PARTIALV,
    D3DDECLMETHOD_CROSSUV,    // Normal
    D3DDECLMETHOD_UV,
    D3DDECLMETHOD_LOOKUP,		// Lookup a displacement map
    D3DDECLMETHOD_LOOKUPPRESAMPLED,	// Lookup a pre-sampled displacement map
} D3DDECLMETHOD;

#define MAXD3DDECLMETHOD D3DDECLMETHOD_LOOKUPPRESAMPLED

// Declarations for _Type fields
//
typedef enum _D3DDECLTYPE
{
    D3DDECLTYPE_FLOAT1	  =  0,  // 1D float expanded to (value, 0., 0., 1.)
    D3DDECLTYPE_FLOAT2	  =  1,  // 2D float expanded to (value, value, 0., 1.)
    D3DDECLTYPE_FLOAT3	  =  2,  // 3D float expanded to (value, value, value, 1.)
    D3DDECLTYPE_FLOAT4	  =  3,  // 4D float
    D3DDECLTYPE_D3DCOLOR  =  4,  // 4D packed unsigned bytes mapped to 0. to 1. range
				 // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
    D3DDECLTYPE_UBYTE4	  =  5,  // 4D unsigned byte
    D3DDECLTYPE_SHORT2	  =  6,  // 2D signed short expanded to (value, value, 0., 1.)
    D3DDECLTYPE_SHORT4	  =  7,  // 4D signed short

// The following types are valid only with vertex shaders >= 2.0


    D3DDECLTYPE_UBYTE4N   =  8,  // Each of 4 bytes is normalized by dividing to 255.0
    D3DDECLTYPE_SHORT2N   =  9,  // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
    D3DDECLTYPE_SHORT4N   = 10,  // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
    D3DDECLTYPE_USHORT2N  = 11,  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
    D3DDECLTYPE_USHORT4N  = 12,  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
    D3DDECLTYPE_UDEC3	  = 13,  // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
    D3DDECLTYPE_DEC3N	  = 14,  // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
    D3DDECLTYPE_FLOAT16_2 = 15,  // Two 16-bit floating point values, expanded to (value, value, 0, 1)
    D3DDECLTYPE_FLOAT16_4 = 16,  // Four 16-bit floating point values
    D3DDECLTYPE_UNUSED	  = 17,  // When the type field in a decl is unused.
} D3DDECLTYPE;

#define MAXD3DDECLTYPE	    D3DDECLTYPE_UNUSED

typedef struct _D3DVERTEXELEMENT9
{
    WORD    Stream;	// Stream index
    WORD    Offset;	// Offset in the stream in bytes
    BYTE    Type;	// Data type
    BYTE    Method;	// Processing method
    BYTE    Usage;	// Semantics
    BYTE    UsageIndex; // Semantic index
} D3DVERTEXELEMENT9, *LPD3DVERTEXELEMENT9;

// This is used to initialize the last vertex element in a vertex declaration
// array
//
#define D3DDECL_END() {0xFF,0,D3DDECLTYPE_UNUSED,0,0,0}

// Maximum supported number of texture coordinate sets
#define D3DDP_MAXTEXCOORD   8

//---------------------------------------------------------------------
// Values for IDirect3DDevice9::SetStreamSourceFreq's Setting parameter
//---------------------------------------------------------------------
#define D3DSTREAMSOURCE_INDEXEDDATA  (1<<30)
#define D3DSTREAMSOURCE_INSTANCEDATA (2<<30)
*/
HRESULT /*D3DX*/ComputeTangentFrame(
  ID3DXMesh * pMesh,
  DWORD dwOptions
)
{
	return /*D3DX*/ComputeTangentFrameEx(pMesh, D3DDECLUSAGE_TEXCOORD, 0,
      D3DDECLUSAGE_BINORMAL, 0, D3DDECLUSAGE_TANGENT, 0,
      D3DDECLUSAGE_NORMAL, 0,
      dwOptions | D3DXTANGENT_GENERATE_IN_PLACE,
      NULL, 0.01f, 0.25f, 0.01f, NULL, NULL);
}

HRESULT /*D3DX*/ComputeNormals(
  LPD3DXBASEMESH pMesh,
  CONST DWORD * pAdjacency
)
{
	return /*D3DX*/ComputeTangentFrameEx( (ID3DXMesh *)pMesh,
			   D3DX_DEFAULT,
			   0,
			   D3DX_DEFAULT,
			   0,
			   D3DX_DEFAULT,
			   0,
			   D3DDECLUSAGE_NORMAL,
			   0,
			   D3DXTANGENT_GENERATE_IN_PLACE | D3DXTANGENT_CALCULATE_NORMALS,
			   pAdjacency,
			   -1.01f,
			   -0.01f,
			   -1.01f,
			   NULL,
			   NULL);
}
/*
D3DXMESH

Flags used to specify creation options for a mesh.

typedef enum D3DXMESH
{
    D3DXMESH_32BIT = 0x001,
    D3DXMESH_DONOTCLIP = 0x002,
    D3DXMESH_POINTS = 0x004,
    D3DXMESH_RTPATCHES = 0x008,
    D3DXMESH_NPATCHES = 0x4000,
    D3DXMESH_VB_SYSTEMMEM = 0x010,
    D3DXMESH_VB_MANAGED = 0x020,
    D3DXMESH_VB_WRITEONLY = 0x040,
    D3DXMESH_VB_DYNAMIC = 0x080,
    D3DXMESH_VB_SOFTWAREPROCESSING = 0x8000,
    D3DXMESH_IB_SYSTEMMEM = 0x100,
    D3DXMESH_IB_MANAGED = 0x200,
    D3DXMESH_IB_WRITEONLY = 0x400,
    D3DXMESH_IB_DYNAMIC = 0x800,
    D3DXMESH_IB_SOFTWAREPROCESSING = 0x10000,
    D3DXMESH_VB_SHARE = 0x1000,
    D3DXMESH_USEHWONLY = 0x2000,
    D3DXMESH_SYSTEMMEM = 0x110,
    D3DXMESH_MANAGED = 0x220,
    D3DXMESH_WRITEONLY = 0x440,
    D3DXMESH_DYNAMIC = 0x880,
    D3DXMESH_SOFTWAREPROCESSING = 0x18000,
} D3DXMESH, *LPD3DXMESH;

Constants

D3DXMESH_32BIT
    The mesh has 32-bit indices instead of 16-bit indices. See Remarks.
D3DXMESH_DONOTCLIP
    Use the D3DUSAGE_DONOTCLIP usage flag for vertex and index buffers.
D3DXMESH_POINTS
    Use the D3DUSAGE_POINTS usage flag for vertex and index buffers.
D3DXMESH_RTPATCHES
    Use the D3DUSAGE_RTPATCHES usage flag for vertex and index buffers.
D3DXMESH_NPATCHES
    Specifying this flag causes the vertex and index buffer of the mesh to be created with D3DUSAGE_NPATCHES flag. This is required if the mesh object is to be rendered using N-patch enhancement using Direct3D.
D3DXMESH_VB_SYSTEMMEM
    Use the D3DPOOL_SYSTEMMEM usage flag for vertex buffers.
D3DXMESH_VB_MANAGED
    Use the D3DPOOL_MANAGED usage flag for vertex buffers.
D3DXMESH_VB_WRITEONLY
    Use the D3DUSAGE_WRITEONLY usage flag for vertex buffers.
D3DXMESH_VB_DYNAMIC
    Use the D3DUSAGE_DYNAMIC usage flag for vertex buffers.
D3DXMESH_VB_SOFTWAREPROCESSING
    Use the D3DUSAGE_SOFTWAREPROCESSING usage flag for vertex buffers.
D3DXMESH_IB_SYSTEMMEM
    Use the D3DPOOL_SYSTEMMEM usage flag for index buffers.
D3DXMESH_IB_MANAGED
    Use the D3DPOOL_MANAGED usage flag for index buffers.
D3DXMESH_IB_WRITEONLY
    Use the D3DUSAGE_WRITEONLY usage flag for index buffers.
D3DXMESH_IB_DYNAMIC
    Use the D3DUSAGE_DYNAMIC usage flag for index buffers.
D3DXMESH_IB_SOFTWAREPROCESSING
    Use the D3DUSAGE_SOFTWAREPROCESSING usage flag for index buffers.
D3DXMESH_VB_SHARE
    Forces the cloned meshes to share vertex buffers.
D3DXMESH_USEHWONLY
    Use hardware processing only. For mixed-mode device, this flag will cause the system to use hardware (if supported in hardware) or will default to software processing.
D3DXMESH_SYSTEMMEM
    Equivalent to specifying both D3DXMESH_VB_SYSTEMMEM and D3DXMESH_IB_SYSTEMMEM.
D3DXMESH_MANAGED
    Equivalent to specifying both D3DXMESH_VB_MANAGED and D3DXMESH_IB_MANAGED.
D3DXMESH_WRITEONLY
    Equivalent to specifying both D3DXMESH_VB_WRITEONLY and D3DXMESH_IB_WRITEONLY.
D3DXMESH_DYNAMIC
    Equivalent to specifying both D3DXMESH_VB_DYNAMIC and D3DXMESH_IB_DYNAMIC.
D3DXMESH_SOFTWAREPROCESSING
    Equivalent to specifying both D3DXMESH_VB_SOFTWAREPROCESSING and D3DXMESH_IB_SOFTWAREPROCESSING.

Remarks

A 32-bit mesh (D3DXMESH_32BIT) can theoretically support (2^32)-1 faces and vertices. However, allocating memory for a mesh that large on a 32-bit operating system is not practical.
Requirements

Header: Declared in D3dx9mesh.h.
*/
/*
    D3DXMESH_32BIT = 0x001,

    D3DXMESH_DONOTCLIP = 0x002,
    D3DXMESH_POINTS = 0x004,
    D3DXMESH_RTPATCHES = 0x008,
    D3DXMESH_NPATCHES = 0x4000,
    D3DXMESH_VB_SHARE = 0x1000,
    D3DXMESH_USEHWONLY = 0x2000,

    D3DXMESH_VB_SYSTEMMEM = 0x010,
    D3DXMESH_VB_MANAGED = 0x020,
    D3DXMESH_VB_WRITEONLY = 0x040,
    D3DXMESH_VB_DYNAMIC = 0x080,
    D3DXMESH_VB_SOFTWAREPROCESSING = 0x8000,

    D3DXMESH_IB_SYSTEMMEM = 0x100,
    D3DXMESH_IB_MANAGED = 0x200,
    D3DXMESH_IB_WRITEONLY = 0x400,
    D3DXMESH_IB_DYNAMIC = 0x800,
    D3DXMESH_IB_SOFTWAREPROCESSING = 0x10000,

    D3DXMESH_SYSTEMMEM = 0x110,
    D3DXMESH_MANAGED = 0x220,
    D3DXMESH_WRITEONLY = 0x440,
    D3DXMESH_DYNAMIC = 0x880,
    D3DXMESH_SOFTWAREPROCESSING = 0x18000,
*/
struct Mesh
{
	LPDIRECT3DINDEXBUFFER9 IndexBuffer;
	LPDIRECT3DVERTEXBUFFER9 VertexBuffer;
	DWORD NumFaces;
	DWORD NumVertices;
	DWORD FVF;
	D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
};
	Mesh mesh;

HRESULT /*WINAPI D3DX*/CreateMeshFVF(
	DWORD NumFaces,
	DWORD NumVertices,
	DWORD Options,
	DWORD FVF,
	LPDIRECT3DDEVICE9 pD3DDevice,
	LPD3DXMESH* ppMesh)
{
	mesh.NumFaces=NumFaces;
	mesh.NumVertices=NumVertices;
	mesh.FVF=FVF;
	DeclaratorFromFVF(FVF,mesh.Declaration);
	DWORD sv,si;
	D3DFORMAT fi;
	DWORD VBUSAGE,IBUSAGE;
	/*D3DPOOL*/DWORD VBPOOL,IBPOOL;
	VBUSAGE=0;
	IBUSAGE=0;
	VBPOOL=0;
	IBPOOL=0;

	if(Options&D3DXMESH_VB_SYSTEMMEM) VBPOOL|=D3DPOOL_SYSTEMMEM;
	if(Options&D3DXMESH_VB_MANAGED) VBPOOL|=D3DPOOL_MANAGED;
	if(Options&D3DXMESH_VB_WRITEONLY) VBUSAGE|=D3DUSAGE_WRITEONLY;
	if(Options&D3DXMESH_VB_DYNAMIC) VBUSAGE|=D3DUSAGE_DYNAMIC;
	if(Options&D3DXMESH_VB_SOFTWAREPROCESSING) VBUSAGE|=D3DUSAGE_SOFTWAREPROCESSING;

	if(Options&D3DXMESH_IB_SYSTEMMEM) IBPOOL|=D3DPOOL_SYSTEMMEM;
	if(Options&D3DXMESH_IB_MANAGED) IBPOOL|=D3DPOOL_MANAGED;
	if(Options&D3DXMESH_IB_WRITEONLY) IBUSAGE|=D3DUSAGE_WRITEONLY;
	if(Options&D3DXMESH_IB_DYNAMIC) IBUSAGE|=D3DUSAGE_DYNAMIC;
	if(Options&D3DXMESH_IB_SOFTWAREPROCESSING) IBUSAGE|=D3DUSAGE_SOFTWAREPROCESSING;

	if(Options&D3DXMESH_SYSTEMMEM)
	{
		VBPOOL|=D3DPOOL_SYSTEMMEM;
		IBPOOL|=D3DPOOL_SYSTEMMEM;
	}
	if(Options&D3DXMESH_MANAGED)
	{
		VBPOOL|=D3DPOOL_MANAGED;
		IBPOOL|=D3DPOOL_MANAGED;
	}
	if(Options&D3DXMESH_WRITEONLY)
	{
		VBUSAGE|=D3DUSAGE_WRITEONLY;
		IBUSAGE|=D3DUSAGE_WRITEONLY;
	}
	if(Options&D3DXMESH_DYNAMIC)
	{
		VBUSAGE|=D3DUSAGE_DYNAMIC;
		IBUSAGE|=D3DUSAGE_DYNAMIC;
	}
	if(Options&D3DXMESH_SOFTWAREPROCESSING)
	{
		VBUSAGE|=D3DUSAGE_SOFTWAREPROCESSING;
		IBUSAGE|=D3DUSAGE_SOFTWAREPROCESSING;
	}
	sv=VertexSizeInBytesFromFVF(FVF);
	if(D3DXMESH_32BIT|Options)
	{
		si=4;
		fi=D3DFMT_INDEX32;
	}
	else
	{
		si=2;
		fi=D3DFMT_INDEX16;
	}
    g_pd3dDevice->CreateVertexBuffer(sv*NumVertices, VBUSAGE, 0,(D3DPOOL)VBPOOL, &mesh.VertexBuffer, NULL );
    g_pd3dDevice->CreateIndexBuffer(NumFaces*3*si, IBUSAGE, fi, (D3DPOOL)IBPOOL, &mesh.IndexBuffer, NULL);
	return D3D_OK;
}
HRESULT /*WINAPI D3DX*/CreateMesh(
	DWORD NumFaces,
	DWORD NumVertices,
	DWORD Options,
	CONST D3DVERTEXELEMENT9 *pDeclaration,
	LPDIRECT3DDEVICE9 pD3DDevice,
	LPD3DXMESH* ppMesh)

{
	mesh.NumFaces=NumFaces;
	mesh.NumVertices=NumVertices;
	mesh.FVF=GetFVF(pDeclaration);
	DWORD n=/*D3DX*/GetDeclLength(pDeclaration);
	memcpy(mesh.Declaration,pDeclaration,(n+1)*sizeof(D3DVERTEXELEMENT9));
//	FVFFromDeclarator(&pDeclaration,&mesh.FVF);

	DWORD sv,si;
	D3DFORMAT fi;
	DWORD VBUSAGE,IBUSAGE;
	/*D3DPOOL*/DWORD VBPOOL,IBPOOL;
	VBUSAGE=0;
	IBUSAGE=0;
	VBPOOL=0;
	IBPOOL=0;
	sv=VertexSizeInBytesFromDecl(mesh.Declaration);

	if(Options&D3DXMESH_VB_SYSTEMMEM) VBPOOL|=D3DPOOL_SYSTEMMEM;
	if(Options&D3DXMESH_VB_MANAGED) VBPOOL|=D3DPOOL_MANAGED;
	if(Options&D3DXMESH_VB_WRITEONLY) VBUSAGE|=D3DUSAGE_WRITEONLY;
	if(Options&D3DXMESH_VB_DYNAMIC) VBUSAGE|=D3DUSAGE_DYNAMIC;
	if(Options&D3DXMESH_VB_SOFTWAREPROCESSING) VBUSAGE|=D3DUSAGE_SOFTWAREPROCESSING;

	if(Options&D3DXMESH_IB_SYSTEMMEM) IBPOOL|=D3DPOOL_SYSTEMMEM;
	if(Options&D3DXMESH_IB_MANAGED) IBPOOL|=D3DPOOL_MANAGED;
	if(Options&D3DXMESH_IB_WRITEONLY) IBUSAGE|=D3DUSAGE_WRITEONLY;
	if(Options&D3DXMESH_IB_DYNAMIC) IBUSAGE|=D3DUSAGE_DYNAMIC;
	if(Options&D3DXMESH_IB_SOFTWAREPROCESSING) IBUSAGE|=D3DUSAGE_SOFTWAREPROCESSING;

	if(Options&D3DXMESH_SYSTEMMEM)
	{
		VBPOOL|=D3DPOOL_SYSTEMMEM;
		IBPOOL|=D3DPOOL_SYSTEMMEM;
	}
	if(Options&D3DXMESH_MANAGED)
	{
		VBPOOL|=D3DPOOL_MANAGED;
		IBPOOL|=D3DPOOL_MANAGED;
	}
	if(Options&D3DXMESH_WRITEONLY)
	{
		VBUSAGE|=D3DUSAGE_WRITEONLY;
		IBUSAGE|=D3DUSAGE_WRITEONLY;
	}
	if(Options&D3DXMESH_DYNAMIC)
	{
		VBUSAGE|=D3DUSAGE_DYNAMIC;
		IBUSAGE|=D3DUSAGE_DYNAMIC;
	}
	if(Options&D3DXMESH_SOFTWAREPROCESSING)
	{
		VBUSAGE|=D3DUSAGE_SOFTWAREPROCESSING;
		IBUSAGE|=D3DUSAGE_SOFTWAREPROCESSING;
	}
	if(D3DXMESH_32BIT|Options)
	{
		si=4;
		fi=D3DFMT_INDEX32;
	}
	else
	{
		si=2;
		fi=D3DFMT_INDEX16;
	}
    g_pd3dDevice->CreateVertexBuffer(sv*NumVertices, VBUSAGE, 0,(D3DPOOL)VBPOOL, &mesh.VertexBuffer, NULL );
    g_pd3dDevice->CreateIndexBuffer(NumFaces*3*si, IBUSAGE, fi, (D3DPOOL)IBPOOL, &mesh.IndexBuffer, NULL);
	return D3D_OK;
}
/*DECLSPEC_EXPORT*/  HRESULT /*WINAPI D3DX*/ComputeTangentFrameEx(ID3DXMesh *pMesh, DWORD dwTextureInSemantic, DWORD dwTextureInIndex, DWORD dwUPartialOutSemantic, DWORD dwUPartialOutIndex, DWORD dwVPartialOutSemantic, DWORD dwVPartialOutIndex, DWORD dwNormalOutSemantic, DWORD dwNormalOutIndex, DWORD dwOptions, CONST DWORD *pdwAdjacency, FLOAT fPartialEdgeThreshold, FLOAT fSingularPointThreshold, FLOAT fNormalEdgeThreshold, ID3DXMesh **ppMeshOut, ID3DXBuffer **ppVertexMapping)
{
  //LOG_PRINT("implement %s:%d\n",__FUNCTION__,__LINE__);return D3D_OK;
	DWORD t1=timeGetTime();

	LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9 pIndexBuffer;
	DWORD VertexCount=pMesh->GetNumVertices();
	DWORD TriangleCount=pMesh->GetNumFaces();
	DWORD VertexSizeInBytes=pMesh->GetNumBytesPerVertex();
	V(pMesh->GetVertexBuffer( &pVertexBuffer ) );
	V(pMesh->GetIndexBuffer( &pIndexBuffer ) );

	D3DVERTEXELEMENT9 Decl[MAX_FVF_DECL_SIZE];
	pMesh->GetDeclaration(Decl);

	bool bCalculateTexCoord=false;
    bool bCalculateBiNormals=false;
    bool bCalculateTangents=false;
    bool bCalculateNormals=false;

	if((D3DDECLUSAGE_TEXCOORD==D3DX_DEFAULT)&&(D3DDECLUSAGE_NORMAL==dwNormalOutSemantic))
		bCalculateTexCoord=true;
	else if(D3DDECLUSAGE_TEXCOORD==dwTextureInSemantic)
		bCalculateTexCoord=false;
    if(D3DDECLUSAGE_BINORMAL==dwUPartialOutSemantic)
		bCalculateBiNormals=true;
    if(D3DDECLUSAGE_TANGENT==dwVPartialOutSemantic)
		bCalculateTangents=true;
    if(D3DDECLUSAGE_NORMAL==dwNormalOutSemantic)
		bCalculateNormals=true;
	int PositionOffset=GetDeclOffset(Decl,D3DDECLUSAGE_POSITION);
	int NormalOffset=GetDeclOffset(Decl,D3DDECLUSAGE_NORMAL);
	int DiffuseOffset=GetDeclOffset(Decl,D3DDECLUSAGE_COLOR);
	int TexOffset=GetDeclTexOffset(Decl);//???
	int BiNormalOffset=GetDeclOffset(Decl,D3DDECLUSAGE_BINORMAL);
	int TangentOffset=GetDeclOffset(Decl,D3DDECLUSAGE_TANGENT);

	DWORD m_nvert2=VertexCount;
	DWORD m_ntri2=TriangleCount;

	//LOG_PRINT("m_nvert2=%d\n",m_nvert2);
	//LOG_PRINT("m_ntri2=%d\n",m_ntri2);
	//LOG_PRINT("vsib=%d\n",VertexSizeInBytes);

	BYTE * v = 0;
	pVertexBuffer->Lock( 0, 0/*VertexCount*(*VertexSizeInBytes)*/, (void**)&v, 0 );

	DWORD i=0;
	DWORD vSize = VertexSizeInBytes/sizeof(BYTE);//pMesh->GetNumBytesPerVertex();
	DWORD* nor1=new DWORD[m_nvert2];

	for(i=0;i<m_nvert2;i++)
	{
		if(bCalculateNormals)
		{
	if(NormalOffset!=-1)
	{
		*((D3DXVECTOR3*)(v+vSize*i+NormalOffset))=D3DXVECTOR3(0.0f,0.0f,0.0f);
	}
	}
/*
	if(( FVF & D3DFVF_DIFFUSE )&&(DiffuseOffset!=-1)		)
	{
		*((DWORD*)(v+vSize*i+DiffuseOffset))=D3DCOLOR_COLORVALUE( 0.0, 1.0, 0.0, 1.0 );
	}
	if(( FVF & D3DFVF_SPECULAR )&&(DiffuseOffset!=-1)		)
	{
		*((DWORD*)(v+vSize*i+SpecularOffset))=D3DCOLOR_COLORVALUE( 0.0, 1.0, 0.0, 1.0 );
	}
*/
	nor1[i]=0;
	}

	//pVertexBuffer->Unlock();

/*
Zero out all vertex normals.
Loop through all faces, and calculate their normals. Add the face normal to the normals of the vertices it refences.
Loop through all vertrices and normalize.
*/
	//pVertexBuffer->Lock(0,0,(void **) &pVert,0);

	WORD *pInd16;
	DWORD *pInd32;
				long i1;
				long i2;
				long i3;
  D3DINDEXBUFFER_DESC ibdesc;
  pIndexBuffer->GetDesc (&ibdesc);
  D3DFORMAT fmt = ibdesc.Format;

  if(fmt==D3DFMT_INDEX32)
	pIndexBuffer->Lock(0,0,(void **) &pInd32,D3DLOCK_READONLY);
  else
	pIndexBuffer->Lock(0,0,(void **) &pInd16,D3DLOCK_READONLY);

	  for(unsigned int z=0; z<m_ntri2/*/3*/; z++)
	{
	  D3DXVECTOR3 normal=D3DXVECTOR3(1.0f,1.0f,1.0f);

		if(fmt==D3DFMT_INDEX32)
		{
			i1 = pInd32[3*z];
			i2 = pInd32[3*z+1];
			i3 = pInd32[3*z+2];
		}
		else
		{
			i1 = pInd16[3*z];
			i2 = pInd16[3*z+1];
			i3 = pInd16[3*z+2];
		}

	  if((i1<m_nvert2)&&(i2<m_nvert2)&&(i3<m_nvert2))
	  {
		  D3DXVECTOR3 pos1=*((D3DXVECTOR3*)(v+vSize*i1+PositionOffset));
		  D3DXVECTOR3 pos2=*((D3DXVECTOR3*)(v+vSize*i2+PositionOffset));
		  D3DXVECTOR3 pos3=*((D3DXVECTOR3*)(v+vSize*i3+PositionOffset));
		  normal=GetTriangeNormal(&pos1, &pos2, &pos3);
		if(bCalculateNormals&&(NormalOffset!=-1))
		{
		  *((D3DXVECTOR3*)(v+vSize*i1+NormalOffset))+=normal;
		  *((D3DXVECTOR3*)(v+vSize*i2+NormalOffset))+=normal;
		  *((D3DXVECTOR3*)(v+vSize*i3+NormalOffset))+=normal;
		}
		  nor1[i1]=nor1[i1]+1;
		  nor1[i2]=nor1[i2]+1;
		  nor1[i3]=nor1[i3]+1;
	  }
	}

	for(i=0; i<m_nvert2; i++)
	{
	 if(bCalculateNormals&&(NormalOffset!=-1))
	{
		((D3DXVECTOR3*)(v+vSize*i+NormalOffset))->x /= (float)nor1[i];
		((D3DXVECTOR3*)(v+vSize*i+NormalOffset))->y /= (float)nor1[i];
		((D3DXVECTOR3*)(v+vSize*i+NormalOffset))->z /= (float)nor1[i];
	 D3DXVec3Normalize(((D3DXVECTOR3*)(v+vSize*i+NormalOffset)),((D3DXVECTOR3*)(v+vSize*i+NormalOffset)));
	}
	if(bCalculateTexCoord&&(TexOffset!=-1))
	{
	 ((D3DXVECTOR3*)(v+vSize*i+TexOffset))->x=asinf(((D3DXVECTOR3*)(v+vSize*i+NormalOffset))->x)/D3DX_PI+0.5f;
	 ((D3DXVECTOR3*)(v+vSize*i+TexOffset))->y=asinf(((D3DXVECTOR3*)(v+vSize*i+NormalOffset))->y)/D3DX_PI+0.5f;
	}
	if(bCalculateTangents&&(TangentOffset!=-1))
	{
	 ((D3DXVECTOR3*)(v+vSize*i+TangentOffset))->x=0.0f;
	 ((D3DXVECTOR3*)(v+vSize*i+TangentOffset))->y=0.0f;
	 ((D3DXVECTOR3*)(v+vSize*i+TangentOffset))->z=0.0f;
	}
	}

	if((bCalculateBiNormals&&(BiNormalOffset!=-1))||(bCalculateTangents&&(TangentOffset!=-1)))
	{
	D3DXVECTOR3 *tan1 = new D3DXVECTOR3[m_nvert2 * 2];
	D3DXVECTOR3 *tan2 = tan1 + m_nvert2;
	ZeroMemory(tan1, m_nvert2 * sizeof(D3DXVECTOR3) * 2);

	for(unsigned int z=0; z<m_ntri2/*/3*/; z++)
	{
		bool cmp=false;
		if(fmt==D3DFMT_INDEX32)
			  cmp=((pInd32[3*z]<m_nvert2)&&(pInd32[3*z+1]<m_nvert2)&&(pInd32[3*z+2]<m_nvert2));
		else
			  cmp=((pInd16[3*z]<m_nvert2)&&(pInd16[3*z+1]<m_nvert2)&&(pInd16[3*z+2]<m_nvert2));
		if(cmp)
		{
		if(fmt==D3DFMT_INDEX32)
		{
			i1 = pInd32[3*z];
			i2 = pInd32[3*z+1];
			i3 = pInd32[3*z+2];
		}
		else
		{
			i1 = pInd16[3*z];
			i2 = pInd16[3*z+1];
			i3 = pInd16[3*z+2];
		}

			D3DXVECTOR3 v1 = D3DXVECTOR3(((D3DXVECTOR3*)(v+vSize*i1+0))->x,((D3DXVECTOR3*)(v+vSize*i1+0))->y,((D3DXVECTOR3*)(v+vSize*i1+0))->z);
			D3DXVECTOR3 v2 = D3DXVECTOR3(((D3DXVECTOR3*)(v+vSize*i2+0))->x,((D3DXVECTOR3*)(v+vSize*i2+0))->y,((D3DXVECTOR3*)(v+vSize*i2+0))->z);
			D3DXVECTOR3 v3 = D3DXVECTOR3(((D3DXVECTOR3*)(v+vSize*i3+0))->x,((D3DXVECTOR3*)(v+vSize*i3+0))->y,((D3DXVECTOR3*)(v+vSize*i3+0))->z);
			const float& w1x = ((D3DXVECTOR2*)(v+vSize*i1+TexOffset))->x;
			const float& w2x = ((D3DXVECTOR2*)(v+vSize*i2+TexOffset))->x;
			const float& w3x = ((D3DXVECTOR2*)(v+vSize*i3+TexOffset))->x;
			const float& w1y = ((D3DXVECTOR2*)(v+vSize*i1+TexOffset))->y;
			const float& w2y = ((D3DXVECTOR2*)(v+vSize*i2+TexOffset))->y;
			const float& w3y = ((D3DXVECTOR2*)(v+vSize*i3+TexOffset))->y;
			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;
			float s1 = w2x - w1x;
			float s2 = w3x - w1x;
			float t1 = w2y - w1y;
			float t2 = w3y - w1y;
			float r = 1.0F / (s1 * t2 - s2 * t1);
			D3DXVECTOR3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r);
			D3DXVECTOR3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r);
			tan1[i1] += sdir;
			tan1[i2] += sdir;
			tan1[i3] += sdir;
			tan2[i1] += tdir;
			tan2[i2] += tdir;
			tan2[i3] += tdir;
		}
	}

	for(i=0; i<m_nvert2; i++)
	{
		 D3DXVECTOR3 n = *((D3DXVECTOR3*)(v+vSize*i+NormalOffset));
		 D3DXVECTOR3 t = tan1[i];
		// Gram-Schmidt orthogonalize
	/*
		D3DXVec3Cross(&vNormal, &v1, &v2);
		D3DXVec3Normalize(&vNormal, &vNormal);
	float viewAngle = D3DXVec3Dot(&curCameraDir, &pImposter->cameraDir);
	*/
		D3DXVECTOR3 n2=(t - n * D3DXVec3Dot(&n, &t));
		D3DXVec3Normalize(&n2,&n2);
		if(bCalculateTangents)
		{
		((D3DXVECTOR3*)(v+vSize*i+TangentOffset))->x = n2.x;
		((D3DXVECTOR3*)(v+vSize*i+TangentOffset))->y = n2.y;
		((D3DXVECTOR3*)(v+vSize*i+TangentOffset))->z = n2.z;
		}
		// Calculate handedness
		D3DXVECTOR3 vCross;
		D3DXVec3Cross(&vCross, &n, &t);
		float pVertitangw = (D3DXVec3Dot(&vCross, &tan2[i]) < 0.0F) ? -1.0F : 1.0F;

		//The bitangent vector B is then given by B = (N × T) · Tw.
		D3DXVec3Cross(&vCross, &n, &t);
		vCross*=pVertitangw;
		if(bCalculateBiNormals)
		{
		((D3DXVECTOR3*)(v+vSize*i+BiNormalOffset))->x=vCross.x;
		((D3DXVECTOR3*)(v+vSize*i+BiNormalOffset))->y=vCross.y;
		((D3DXVECTOR3*)(v+vSize*i+BiNormalOffset))->z=vCross.z;
		}
	}
	delete[] tan1;
	delete[] nor1;
}
	pVertexBuffer->Unlock();
	pIndexBuffer->Unlock();

	SAFE_RELEASE(pVertexBuffer);
	SAFE_RELEASE(pIndexBuffer);

	DWORD t2=timeGetTime();
	//LOG_PRINT("CloneVertexBufferFVF took %f seconds\n",(t2-t1)/1000.0f);

	return D3D_OK;
}
#if __D3DGL__
#else
void LockIndexBuffer(LPDIRECT3DINDEXBUFFER9 pIndexBuffer, WORD *pInd16, DWORD *pInd32)
{
	D3DINDEXBUFFER_DESC ibdesc;
	pIndexBuffer->GetDesc (&ibdesc);
	D3DFORMAT fmt = ibdesc.Format;
	if(fmt==D3DFMT_INDEX32)
		pIndexBuffer->Lock(0,0,(void **) &pInd32, D3DLOCK_READONLY);
	else
		pIndexBuffer->Lock(0,0,(void **) &pInd16, D3DLOCK_READONLY);
}
void LockIndexBuffer(LPDIRECT3DINDEXBUFFER9 pIndexBuffer, BYTE** ppInd, DWORD *strideInBytes)
{
	pIndexBuffer->Lock(0,0,(void **) ppInd, D3DLOCK_READONLY);
	D3DINDEXBUFFER_DESC ibdesc;
	pIndexBuffer->GetDesc (&ibdesc);
	D3DFORMAT fmt = ibdesc.Format;
	if(fmt==D3DFMT_INDEX32)
		*strideInBytes=4;
	else
		*strideInBytes=2;
}
DWORD VertexSizeInBytesFromDecl(D3DVERTEXELEMENT9 aOldDecl[])
{
	DWORD size=0;
	for( UINT index = 0; index < GetDeclLength( aOldDecl ); ++index )
	{
	    if( aOldDecl[index].Usage == D3DDECLUSAGE_POSITION )
		size+=3*sizeof(float);
	    if( aOldDecl[index].Usage == D3DDECLUSAGE_NORMAL )
		size+=3*sizeof(float);
	    if( aOldDecl[index].Usage == D3DDECLUSAGE_PSIZE)
		size+=1*sizeof(float);
	    if( aOldDecl[index].Usage == D3DDECLUSAGE_COLOR)
		size+=sizeof(DWORD);

	    if(( aOldDecl[index].Usage == D3DDECLUSAGE_TEXCOORD )&&((aOldDecl[index].UsageIndex>=0)&&(aOldDecl[index].UsageIndex<9)))
	    {
			switch(aOldDecl[index].Type)
			{
				case D3DDECLTYPE_FLOAT1:
					size+=1*sizeof(float);
					break;
				case D3DDECLTYPE_FLOAT2:
					size+=2*sizeof(float);
					break;
				case D3DDECLTYPE_FLOAT3:
					size+=3*sizeof(float);
					break;
				case D3DDECLTYPE_FLOAT4:
					size+=4*sizeof(float);
					break;
				default:
					size+0;
					break;
			};
		}

	    if( aOldDecl[index].Usage == D3DDECLUSAGE_BINORMAL )
		size+=3*sizeof(float);
	    if( aOldDecl[index].Usage == D3DDECLUSAGE_TANGENT )
		size+=3*sizeof(float);
	}
	return size;
}
DWORD VertexSizeInBytesFromFVF(DWORD FVF)
{
	DWORD size=0;
	if( FVF & D3DFVF_XYZ )
		size+=3*sizeof(float);
	else if( FVF & D3DFVF_XYZRHW )
		size+=4*sizeof(float);
	else if( FVF & D3DFVF_XYZB1 )
		size+=4*sizeof(float);
	else if( FVF & D3DFVF_XYZB2 )
		size+=5*sizeof(float);
	else if( FVF & D3DFVF_XYZB3 )
		size+=6*sizeof(float);
	else if( FVF & D3DFVF_XYZB4 )
		size+=7*sizeof(float);
	else if( FVF & D3DFVF_XYZB5 )
		size+=8*sizeof(float);
	if( FVF & D3DFVF_NORMAL )
		size+=3*sizeof(float);
	if( FVF & D3DFVF_PSIZE )
		size+=1*sizeof(float);
	if( FVF & D3DFVF_DIFFUSE )
		size+=sizeof(DWORD);
	if( FVF & D3DFVF_SPECULAR )
		size+=sizeof(DWORD);

	DWORD texture = FVF >> 16;
	for (DWORD i=0;i<((FVF&D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);i++)
	{
		switch (texture & 3)
		{
			case D3DFVF_TEXTUREFORMAT1:
				size=size + sizeof(FLOAT);
				break;
			case D3DFVF_TEXTUREFORMAT2:
				size=size + sizeof(FLOAT)*2;
				break;
			case D3DFVF_TEXTUREFORMAT3:
				size=size + sizeof(FLOAT)*3;
				break;
			case D3DFVF_TEXTUREFORMAT4:
				size=size + sizeof(FLOAT)*4;
				break;
		}
		texture = texture >>2;
	}
	return size;
}
void CloneIndexBuffer(DWORD dwStride1, DWORD dwStride2, BYTE* pFirstPosition1, BYTE* pFirstPosition2, DWORD nf)
{

	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("dwStride1=%d\n",dwStride1);
	//LOG_PRINT("dwStride2=%d\n",dwStride2);
	//LOG_PRINT("pFirstPosition1=%d\n",pFirstPosition1);
	//LOG_PRINT("pFirstPosition2=%d\n",pFirstPosition2);
	//LOG_PRINT("nf=%d\n",nf);

	if((dwStride1==dwStride2))
	{
		memcpy(pFirstPosition2,pFirstPosition1,nf*3*dwStride1);
	}
	else
	{
	for (DWORD i = 0; i < nf*3; i++)
	{
		if((dwStride1==4)&&(dwStride2==4))
			*((WORD*)(pFirstPosition2+dwStride2*i))=*((WORD*)(pFirstPosition1+dwStride1*i));
		else if((dwStride1==8)&&(dwStride2==8))
			*((DWORD*)(pFirstPosition2+dwStride2*i))=*((DWORD*)(pFirstPosition1+dwStride1*i));
		else if((dwStride1==8)&&(dwStride2==4))
			*((DWORD*)(pFirstPosition2+dwStride2*i))=*((WORD*)(pFirstPosition1+dwStride1*i));
		else if((dwStride1==4)&&(dwStride2==8))
			*((WORD*)(pFirstPosition2+dwStride2*i))=*((DWORD*)(pFirstPosition1+dwStride1*i));
	}
	}
}
void CloneVertexBuffer(DWORD dwStride1, DWORD dwStride2, DWORD FVF, DWORD FVF2, BYTE* pFirstPosition1, BYTE* pFirstPosition2, DWORD nv)
{
  D3DVERTEXELEMENT9 Declaration1[MAX_FVF_DECL_SIZE];
  D3DVERTEXELEMENT9 Declaration2[MAX_FVF_DECL_SIZE];
 /*D3DX*/DeclaratorFromFVF(FVF,Declaration1);
 /*D3DX*/DeclaratorFromFVF(FVF2,Declaration2);
 CloneVertexBuffer(dwStride1, dwStride2, Declaration1, Declaration2, pFirstPosition1, pFirstPosition2, nv);
}
void CloneVertexBuffer(DWORD dwStride1, DWORD dwStride2, D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE], D3DVERTEXELEMENT9 Declaration2[MAX_FVF_DECL_SIZE], BYTE* pFirstPosition1, BYTE* pFirstPosition2, DWORD nv)
{
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("dwStride1=%d\n",dwStride1);
	//LOG_PRINT("dwStride2=%d\n",dwStride2);
	//LOG_PRINT("pFirstPosition1=%d\n",pFirstPosition1);
	//LOG_PRINT("pFirstPosition2=%d\n",pFirstPosition2);
	//LOG_PRINT("nv=%d\n",nv);

	//LOG_PRINT("DecodeDecl:\n");
	DecodeDecl(Declaration);
	//LOG_PRINT("DecodeDecl:\n");
	DecodeDecl(Declaration2);
	      /*
	if((dwStride1==dwStride2)&&(FVF==FVF2))
	{
		memcpy(pFirstPosition2,pFirstPosition1,nv*dwStride1);
	}
	else*/
	{
	int coloroffset1=GetDeclOffset(Declaration,D3DDECLUSAGE_COLOR);
	int coloroffset2=GetDeclOffset(Declaration2,D3DDECLUSAGE_COLOR);

	int texoffset1=GetDeclTexOffset(Declaration);
	int texoffset2=GetDeclTexOffset(Declaration2);

	int positionoffset1=GetDeclOffset(Declaration,D3DDECLUSAGE_POSITION);
	int positionoffset2=GetDeclOffset(Declaration2,D3DDECLUSAGE_POSITION);

	int normaloffset1=GetDeclOffset(Declaration,D3DDECLUSAGE_NORMAL);
	int normaloffset2=GetDeclOffset(Declaration2,D3DDECLUSAGE_NORMAL);
	int binormaloffset1=GetDeclOffset(Declaration,D3DDECLUSAGE_BINORMAL);
	int binormaloffset2=GetDeclOffset(Declaration2,D3DDECLUSAGE_BINORMAL);
	int tangentoffset1=GetDeclOffset(Declaration,D3DDECLUSAGE_TANGENT);
	int tangentoffset2=GetDeclOffset(Declaration2,D3DDECLUSAGE_TANGENT);

	for (DWORD i = 0; i < nv; i++)
	{
	if(( positionoffset1!=-1 )&&( positionoffset2!=1 ))
	{
	((D3DXVECTOR3*)(pFirstPosition2+dwStride2*i))->x=((D3DXVECTOR3*)(pFirstPosition1+dwStride1*i))->x;
	((D3DXVECTOR3*)(pFirstPosition2+dwStride2*i))->y=((D3DXVECTOR3*)(pFirstPosition1+dwStride1*i))->y;
	((D3DXVECTOR3*)(pFirstPosition2+dwStride2*i))->z=((D3DXVECTOR3*)(pFirstPosition1+dwStride1*i))->z;
	}
	if(( normaloffset1!=-1 )&&( normaloffset2!=1 ))
	{
		if((normaloffset1!=-1)&&(normaloffset2!=-1))
		{
		((D3DXVECTOR3*)(pFirstPosition2+dwStride2*i+normaloffset2))->x=((D3DXVECTOR3*)(pFirstPosition1+dwStride1*i+normaloffset1))->x;
		((D3DXVECTOR3*)(pFirstPosition2+dwStride2*i+normaloffset2))->y=((D3DXVECTOR3*)(pFirstPosition1+dwStride1*i+normaloffset1))->y;
		((D3DXVECTOR3*)(pFirstPosition2+dwStride2*i+normaloffset2))->z=((D3DXVECTOR3*)(pFirstPosition1+dwStride1*i+normaloffset1))->z;
		}
	}

	if(( coloroffset1!=-1 )&&( coloroffset2!=1 ))
	{
		*((DWORD*)(pFirstPosition2+dwStride2*i+coloroffset2))=*((DWORD*)(pFirstPosition1+dwStride1*i+coloroffset1));
	}

		if((texoffset1!=-1)&&(texoffset2!=-1))
		{
		((D3DXVECTOR2*)(pFirstPosition2+dwStride2*i+texoffset2))->x=((D3DXVECTOR2*)(pFirstPosition1+dwStride1*i+texoffset1))->x;
		((D3DXVECTOR2*)(pFirstPosition2+dwStride2*i+texoffset2))->y=((D3DXVECTOR2*)(pFirstPosition1+dwStride1*i+texoffset1))->y;
		}

	if(( binormaloffset1!=-1 )&&( binormaloffset2!=1 ))
	{
		((D3DXVECTOR3*)(pFirstPosition2+dwStride2*i+binormaloffset2))->x=((D3DXVECTOR3*)(pFirstPosition1+dwStride1*i+binormaloffset1))->x;
		((D3DXVECTOR3*)(pFirstPosition2+dwStride2*i+binormaloffset2))->y=((D3DXVECTOR3*)(pFirstPosition1+dwStride1*i+binormaloffset1))->y;
		((D3DXVECTOR3*)(pFirstPosition2+dwStride2*i+binormaloffset2))->z=((D3DXVECTOR3*)(pFirstPosition1+dwStride1*i+binormaloffset1))->z;
	}
	if((tangentoffset1!=-1)&&(tangentoffset2!=-1))
	{
		((D3DXVECTOR3*)(pFirstPosition2+dwStride2*i+tangentoffset2))->x=((D3DXVECTOR3*)(pFirstPosition1+dwStride1*i+tangentoffset1))->x;
		((D3DXVECTOR3*)(pFirstPosition2+dwStride2*i+tangentoffset2))->y=((D3DXVECTOR3*)(pFirstPosition1+dwStride1*i+tangentoffset1))->y;
		((D3DXVECTOR3*)(pFirstPosition2+dwStride2*i+tangentoffset2))->z=((D3DXVECTOR3*)(pFirstPosition1+dwStride1*i+tangentoffset1))->z;
	}
	}
	}
}
D3DXVECTOR3 GetTriangeNormal(D3DXVECTOR3* vVertex1, D3DXVECTOR3* vVertex2, D3DXVECTOR3* vVertex3)
{
    D3DXVECTOR3 vNormal;
    D3DXVECTOR3 v1;
    D3DXVECTOR3 v2;
    D3DXVec3Subtract(&v1, vVertex2, vVertex1);
    D3DXVec3Subtract(&v2, vVertex3, vVertex1);
    D3DXVec3Cross(&vNormal, &v1, &v2);
    D3DXVec3Normalize(&vNormal, &vNormal);
    return vNormal;
}
DWORD GetFVF(CONST D3DVERTEXELEMENT9 *aOldDecl)
{
	DWORD FVF=0;
	WORD CoordIndex=0;
	for( UINT index = 0; index < GetDeclLength( aOldDecl ); ++index )
	{
	    if( aOldDecl[index].Usage == D3DDECLUSAGE_POSITION )
		FVF|=D3DFVF_XYZ;
	    if( aOldDecl[index].Usage == D3DDECLUSAGE_NORMAL )
		FVF|=D3DFVF_NORMAL;
	    if( aOldDecl[index].Usage == D3DDECLUSAGE_PSIZE)
		FVF|=D3DFVF_PSIZE;
	    if( aOldDecl[index].Usage == D3DDECLUSAGE_COLOR)
		FVF|=D3DFVF_DIFFUSE;

	    if( aOldDecl[index].Usage == D3DDECLUSAGE_TEXCOORD )
	    {
			switch(aOldDecl[index].Type)
			{
				case D3DDECLTYPE_FLOAT1:
					FVF|=D3DFVF_TEXCOORDSIZE1(CoordIndex++);
					break;
				case D3DDECLTYPE_FLOAT2:
					FVF|=D3DFVF_TEXCOORDSIZE2(CoordIndex++);
					break;
				case D3DDECLTYPE_FLOAT3:
					FVF|=D3DFVF_TEXCOORDSIZE3(CoordIndex++);
					break;
				case D3DDECLTYPE_FLOAT4:
					FVF|=D3DFVF_TEXCOORDSIZE4(CoordIndex++);
					break;
			};
		}
	}
	switch(CoordIndex)
	{
		case 1:
			FVF|=D3DFVF_TEX1;
			break;
		case 2:
			FVF|=D3DFVF_TEX2;
			break;
		case 3:
			FVF|=D3DFVF_TEX3;
			break;
		case 4:
			FVF|=D3DFVF_TEX4;
			break;
		case 5:
			FVF|=D3DFVF_TEX5;
			break;
		case 6:
			FVF|=D3DFVF_TEX6;
			break;
		case 7:
			FVF|=D3DFVF_TEX7;
			break;
		case 8:
			FVF|=D3DFVF_TEX8;
			break;
		/*default:
			FVF|=D3DFVF_TEX0;
			break;*/
	}
	return FVF;
}
int GetFVFOffset(DWORD FVF, DWORD FVFComponent, int TEXComponent)
{
	int FVFOffset=0;
	// Mask position
	DWORD dwPosition = FVF & D3DFVF_POSITION_MASK;

	// Position is always at the start
	if (FVFComponent != 0)
		if(dwPosition&FVFComponent) return FVFOffset;

	if( FVF & D3DFVF_XYZ )
		FVFOffset+=3*sizeof(float);
	else if( FVF & D3DFVF_XYZRHW )
		FVFOffset+=4*sizeof(float);
	else if( FVF & D3DFVF_XYZB1 )
		FVFOffset+=4*sizeof(float);
	else if( FVF & D3DFVF_XYZB2 )
		FVFOffset+=5*sizeof(float);
	else if( FVF & D3DFVF_XYZB3 )
		FVFOffset+=6*sizeof(float);
	else if( FVF & D3DFVF_XYZB4 )
		FVFOffset+=7*sizeof(float);
	else if( FVF & D3DFVF_XYZB5 )
		FVFOffset+=8*sizeof(float);
	if ((FVFComponent & D3DFVF_NORMAL)&&(FVF&D3DFVF_NORMAL)) return FVFOffset;
	if( FVF & D3DFVF_NORMAL )
		FVFOffset+=3*sizeof(float);
	if ((FVFComponent & D3DFVF_PSIZE)&&(FVF&D3DFVF_PSIZE)) return FVFOffset;
	if( FVF & D3DFVF_PSIZE )
		FVFOffset+=1*sizeof(float);
	if ((FVFComponent & D3DFVF_DIFFUSE)&&(FVF&D3DFVF_DIFFUSE)) return FVFOffset;
	if( FVF & D3DFVF_DIFFUSE )
		FVFOffset+=1*sizeof(DWORD);
	if ((FVFComponent & D3DFVF_SPECULAR)&&(FVF&D3DFVF_SPECULAR)) return FVFOffset;
	if( FVF & D3DFVF_SPECULAR )
		FVFOffset+=1*sizeof(DWORD);
	DWORD texture = FVF >> 16;
	for (DWORD i=0;i<((FVF&D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);i++)
	{
		if (TEXComponent == i) return FVFOffset;
		switch (texture & 3)
		{
			case D3DFVF_TEXTUREFORMAT1:
				FVFOffset+=1*sizeof(float);
				break;
			case D3DFVF_TEXTUREFORMAT2:
				FVFOffset+=2*sizeof(float);
				break;
			case D3DFVF_TEXTUREFORMAT3:
				FVFOffset+=3*sizeof(float);
				break;
			case D3DFVF_TEXTUREFORMAT4:
				FVFOffset+=4*sizeof(float);
				break;
		}
		texture = texture >>2;
	}
	return -1;
}
int GetDeclOffset(CONST D3DVERTEXELEMENT9 *pDeclaration, BYTE Usage, int UsageIndex)
{
	for( UINT index = 0; index < GetDeclLength( pDeclaration ); ++index )
	{
	    if(( pDeclaration[index].Usage == Usage )&&(Usage != D3DDECLUSAGE_TEXCOORD))
		return pDeclaration[index].Offset;
	    else if(( pDeclaration[index].Usage == D3DDECLUSAGE_TEXCOORD )&&(Usage == D3DDECLUSAGE_TEXCOORD))
	    {
		if(pDeclaration[index].UsageIndex == UsageIndex)
		      return pDeclaration[index].Offset;
	    }
	}
	return -1;
}
int GetDeclTexOffset(CONST D3DVERTEXELEMENT9 *pDeclaration)
{
  return GetDeclOffset(pDeclaration, D3DDECLUSAGE_TEXCOORD, 0);
}
int GetFVFTexOffset(DWORD FVF)
{
	int FVFOffset=0;
    //FVFOffset=GetFVFComponentOffset(FVF, D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0),0);
    FVFOffset=GetFVFOffset(FVF, D3DFVF_TEXCOORDSIZE2(0), 0);
	return FVFOffset;
	/*
	BYTE cTexCoords = (BYTE)((FVF & D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
	for(WORD CoordIndex=0;CoordIndex<cTexCoords;CoordIndex++)
	{
		//if(FVF&D3DFVF_TEXCOORDSIZE1(CoordIndex)) //LOG_PRINT("FVF Had Tex%d Float%d\n",CoordIndex,1);
		if(FVF&D3DFVF_TEXCOORDSIZE2(CoordIndex)) //LOG_PRINT("FVF Had Tex%d Float%d\n",CoordIndex,2);
		//if(FVF&D3DFVF_TEXCOORDSIZE3(CoordIndex)) //LOG_PRINT("FVF Had Tex%d Float%d\n",CoordIndex,3);
		//if(FVF&D3DFVF_TEXCOORDSIZE4(CoordIndex)) //LOG_PRINT("FVF Had Tex%d Float%d\n",CoordIndex,4);
	}
	*/
}
#endif
#endif
/*
DECLARE_INTERFACE_(ID3DXBuffer, IUnknown)
{
    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // ID3DXBuffer
    STDMETHOD_(LPVOID, GetBufferPointer)(THIS) PURE;
    STDMETHOD_(DWORD, GetBufferSize)(THIS) PURE;
};
DECLARE_INTERFACE_(ID3DXConstantTable, IUnknown)
{
    // IUnknown
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    // Buffer
    STDMETHOD_(LPVOID, GetBufferPointer)(THIS) PURE;
    STDMETHOD_(DWORD, GetBufferSize)(THIS) PURE;

    // Descs
    STDMETHOD(GetDesc)(THIS_ D3DXCONSTANTTABLE_DESC *pDesc) PURE;
    STDMETHOD(GetConstantDesc)(THIS_ D3DXHANDLE hConstant, D3DXCONSTANT_DESC *pConstantDesc, UINT *pCount) PURE;
    STDMETHOD_(UINT, GetSamplerIndex)(THIS_ D3DXHANDLE hConstant) PURE;

    // Handle operations
    STDMETHOD_(D3DXHANDLE, GetConstant)(THIS_ D3DXHANDLE hConstant, UINT Index) PURE;
    STDMETHOD_(D3DXHANDLE, GetConstantByName)(THIS_ D3DXHANDLE hConstant, LPCSTR pName) PURE;
    STDMETHOD_(D3DXHANDLE, GetConstantElement)(THIS_ D3DXHANDLE hConstant, UINT Index) PURE;

    // Set Constants
    STDMETHOD(SetDefaults)(THIS_ LPDIRECT3DDEVICE9 pDevice) PURE;
    STDMETHOD(SetValue)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, LPCVOID pData, UINT Bytes) PURE;
    STDMETHOD(SetBool)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, BOOL b) PURE;
    STDMETHOD(SetBoolArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST BOOL* pb, UINT Count) PURE;
    STDMETHOD(SetInt)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, INT n) PURE;
    STDMETHOD(SetIntArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST INT* pn, UINT Count) PURE;
    STDMETHOD(SetFloat)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, FLOAT f) PURE;
    STDMETHOD(SetFloatArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST FLOAT* pf, UINT Count) PURE;
    STDMETHOD(SetVector)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXVECTOR4* pVector) PURE;
    STDMETHOD(SetVectorArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXVECTOR4* pVector, UINT Count) PURE;
    STDMETHOD(SetMatrix)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXMATRIX* pMatrix) PURE;
    STDMETHOD(SetMatrixArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXMATRIX* pMatrix, UINT Count) PURE;
    STDMETHOD(SetMatrixPointerArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXMATRIX** ppMatrix, UINT Count) PURE;
    STDMETHOD(SetMatrixTranspose)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXMATRIX* pMatrix) PURE;
    STDMETHOD(SetMatrixTransposeArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXMATRIX* pMatrix, UINT Count) PURE;
    STDMETHOD(SetMatrixTransposePointerArray)(THIS_ LPDIRECT3DDEVICE9 pDevice, D3DXHANDLE hConstant, CONST D3DXMATRIX** ppMatrix, UINT Count) PURE;
};
HRESULT LoadMesh(const char* file, LPD3DXMESH* ppMesh, DWORD* pNumMaterials, LPDIRECT3DTEXTURE9* ppMeshTextures, D3DMATERIAL9** ppMeshMaterials)
{
	char texture7[256];

	char path7[256];
	path7[0]='\0';
	strcpy(path7,file);
	int l1 = strrchr(path7, '\\')-path7+1;
	int l2 = strrchr(path7, '/')-path7+1;
	if(l1<0) l1=0;
	if(l2<0) l2=0;
	int l=std::max(l1,l2);
	path7[l] = '\0';

	//LOG_PRINT("path %s\n",path7);

	std::vector<const char*> textures;
	LPD3DXBUFFER pMaterialBuffer;
	HRESULT hr = D3DXLoadMeshFromX( file, D3DXMESH_MANAGED, g_pd3dDevice, NULL, &pMaterialBuffer, NULL, pNumMaterials, ppMesh );
	//LOG_PRINT("mesh %s\n",file);
	//LOG_PRINT("*ppMesh %d\n",*ppMesh);
	if ( FAILED( hr ) )
	{
		//LOG_PRINT("%s failed\n",file);
		return hr;
	}

	//LOG_PRINT("*pNumMaterials=%d\n",*pNumMaterials);
	//LOG_PRINT("pMaterialBuffer=%d\n",pMaterialBuffer);

	// Store material and texture information
	D3DXMATERIAL* pMaterials = (D3DXMATERIAL*)pMaterialBuffer;////???D3DGL->GetBufferPointer();
	(*ppMeshMaterials) = new D3DMATERIAL9[*pNumMaterials];
	ppMeshTextures = new LPDIRECT3DTEXTURE9[*pNumMaterials];

		//LOG_PRINT("*pNumMaterials %d\n",*pNumMaterials);

	// Copy the materials and textures from the buffer to the member arrays
	for ( DWORD i = 0; i < *pNumMaterials; i++ )
	{
		(*ppMeshMaterials)[i] = pMaterials[i].MatD3D;
		(*ppMeshMaterials)[i].Ambient = (*ppMeshMaterials)[i].Diffuse;
		// Create the texture if it exists
		ppMeshTextures[i] = NULL;
		if( pMaterials[i].pTextureFilename && (strcmp("pMaterials[i].pTextureFilename","")!=0))
		{
			bool skip=false;
			for ( DWORD j = 0; j < textures.size(); j++ )
			{
			  if(strcmp(textures[j],pMaterials[i].pTextureFilename)==0)
			  {
				skip=true;
				break;
			  }
			}
			if(!skip)
			{
			textures.push_back(pMaterials[i].pTextureFilename);
			  texture7[0]='\0';
			strcat(texture7,path7);
			//LOG_PRINT("texture %s\n",texture7);
			strcat(texture7,pMaterials[i].pTextureFilename);
			//LOG_PRINT("texture %s\n",texture7);
			if ( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice,texture7, &ppMeshTextures[i] ) ) )
			{
				//LOG_PRINT("%s failed\n",pMaterials[i].pTextureFilename);
				return hr;
			}
			}
		}
	}
	// Don’t need this no more!
//???D3DGL	pMaterialBuffer->Release();
	return D3D_OK;
}
void AddShaders(
    const char* nameVS,
    const char* namePS,
	LPDIRECT3DVERTEXSHADER9      VS,
	LPD3DXCONSTANTTABLE	     CTVS,
	LPDIRECT3DPIXELSHADER9		 PS,
	LPD3DXCONSTANTTABLE		 CTPS,
	D3DXMACRO* defines
	)
	{
	HRESULT hr;
    LPD3DXBUFFER pCode;
    DWORD dwShaderFlags = 0;
	LPD3DXBUFFER pBufferErrors = NULL;
     #if defined( DEBUG ) || defined( _DEBUG )
    // Set the V(D3DXSHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows
    // the shaders to be optimized and to run exactly the way they will run in
    // the release configuration of this program.
    dwShaderFlags |= D3DXSHADER_DEBUG;
    #endif
    #ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_SKIPOPTIMIZATION|D3DXSHADER_DEBUG;
    #endif
    #ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_SKIPOPTIMIZATION|D3DXSHADER_DEBUG;
    #endif
/#
D3DXMACRO defines[2] =
    {
	{ "__HLSL__", "" },
	{ NULL, NULL }
    };
#/
    hr=D3DXCompileShaderFromFile( nameVS, defines,NULL,  "main",
				    "vs_2_0", dwShaderFlags, &pCode,
				    &pBufferErrors, &CTVS );
    if( FAILED(hr) )
	{
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		//LOG_PRINT( "Vertex ShaderD3DGL Compile Error\n");
		 //LOG_PRINT( (const char*)pCompilErrors);
	}
    V(g_pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(),
				      &VS ));
    pCode->Release();
    hr = D3DXCompileShaderFromFile( namePS, defines,NULL,  "main",
				    "ps_2_0", dwShaderFlags, &pCode,
				    &pBufferErrors, &CTPS );
    if( FAILED(hr) )
	{
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		//LOG_PRINT("Pixel ShaderD3DGL Compile Error\n");
		//LOG_PRINT( (const char*)pCompilErrors);
	}
    V(g_pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(),
				      &PS ));
    pCode->Release();
}
*/
void EnableAlpha(bool bBlend)
{
/*
       g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE);
       g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
       g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
       g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
       g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
       g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
       g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
*/
	    // Disable lighting
    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    // Enable alpha testing for transparent blits
    g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    if(bBlend)
    {
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
  }
}
#if 0
    g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

  static int bb=0;
  if(KeyPressed('B')) bb^=1;
  if(bb==1)
  {
    // Enable alpha blending for the smoke particles
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
  }

    g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

#endif
void DisableAlpha(bool bBlend)
{
/*
       g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE);//D3DZB_FALSE);//
       g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
       g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
       g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
*/
    // Disable alpha testing and alpha blending
    g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    if(bBlend)
    {
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    }
}

#if 1
/*
To capture the screen (Win32) use CreateDC("DISPLAY",0,0,0) (and DeleteDC() when done)... and then either GetPixel() or GetDIBits().
You'll have to create memory DCs and Compatible Bitmaps if you go the GetDIBits() route.
*//*
void SaveDIB(DWORD DibSize, DWORD ImageOffset, const char* pSrcDib, const char* lpszPathName)
{
	// Save source DIB to file

	DWORD dwDibSize = DibSize;

	// Initialize bitmap file header
    BITMAPFILEHEADER bmpFileHeader;
	::ZeroMemory(&bmpFileHeader, sizeof(BITMAPFILEHEADER));
    bmpFileHeader.bfType    = 0x4D42;
	bmpFileHeader.bfSize	= sizeof(BITMAPFILEHEADER) + dwDibSize;
    bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + ImageOffset;

	// Create bitmap file
	BOOL bResult = FALSE;

		fstream binary_file(lpszPathName, ios::out|ios::binary);//|ios::app);
		binary_file.write((const char *)&bmpFileHeader, sizeof(BITMAPFILEHEADER));
		binary_file.write((const char *)pSrcDib, dwDibSize);
		binary_file.close();
		bResult = TRUE;

}*/
BOOL WriteDIB( LPTSTR szFile, HANDLE hDIB)
{
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFOHEADER	lpbi;

	if (!hDIB)
		return FALSE;

	fstream binary_file(szFile, ios::out|ios::binary);//|ios::app);

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	int nColors = 1 << lpbi->biBitCount;

	// Fill in the fields of the file header
	hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize		= GlobalSize (hDIB) + sizeof( hdr );
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= (DWORD) (sizeof( hdr ) + lpbi->biSize +
						nColors * sizeof(RGBQUAD));

	// Write the file header
	binary_file.write( (const char*)&hdr, sizeof(hdr) );

	// Write the DIB header and the bits
	binary_file.write( (const char*)lpbi, GlobalSize(hDIB) );
	binary_file.close();

	return TRUE;
}

struct matriz {

int width;
int height;
int x;
int y;
COLORREF  *tela;
};
void SaveScreenShot2(const char* fn)
{
	if((fn!=0)||((fn==0)&&(KeyPressed('X'))))
	{
  LPDIRECT3DSURFACE9 fbColor;
  V(g_pd3dDevice->GetRenderTarget(0, &fbColor));
   if(fn!=0)
  {
	  char buf[256];
	  sprintf(buf,"../%s.jpg",fn);
	  //LOG_PRINT("%s\n",buf);
  V(D3DXSaveSurfaceToFile(buf,
			      D3DXIFF_JPG,
			      fbColor,
			      NULL,
			      NULL));
	}
	else if(strlen(title)>0)
  {
	  char buf[256];
	  sprintf(buf,"../%s.jpg",title);
	  //LOG_PRINT("%s\n",buf);
  V(D3DXSaveSurfaceToFile(buf,
			      D3DXIFF_JPG,
			      fbColor,
			      NULL,
			      NULL));
	}
	else
	{
		//LOG_PRINT("ScreenShot.jpg");
  V(D3DXSaveSurfaceToFile("ScreenShot.jpg",
			      D3DXIFF_JPG,
			      fbColor,
			      NULL,
			      NULL));
	}
	SAFE_RELEASE(fbColor);
	}
}
#if 0
void SaveScreenShot3(const char* fn)
{
struct matriz *m;
#if 1
HWND	hDesktopWnd = GetDesktopWindow();
int    nScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
int    nScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
int    nX = 0;
int    nY = 0;
#else
HWND	hDesktopWnd = g_hWnd;
RECT rect;
//GetWindowRect(GetDesktopWindow(), &rect);
GetWindowRect(g_hWnd, &rect);
int    nScreenWidth = rect.bottom-rect.top;
int    nScreenHeight = rect.right-rect.left;
int    nX = rect.left;
int    nY = rect.top;
#endif
m = (matriz*)malloc(sizeof(struct matriz));
m->tela = (COLORREF*)malloc(nScreenWidth*nScreenHeight*sizeof(COLORREF));
m->width =    nScreenWidth;
m->height =    nScreenHeight;
m->x =	  nX;
m->y =	  nY;

if(hDesktopWnd == NULL)
{
//LOG_PRINT("ERRO GetWindowDC\n");
return;
}
HDC    hDesktopDC = GetDC(hDesktopWnd);
if(hDesktopDC == NULL)
{
//LOG_PRINT("ERRO GetDC\n");
return;
}
HDC  hCaptureDC = CreateCompatibleDC(hDesktopDC);
if( hCaptureDC == NULL)
{
//LOG_PRINT("ERRO CreateCompatibleDC\n");
return;
}

HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, nScreenWidth,
nScreenHeight);
if( hCaptureBitmap == NULL)
{
//LOG_PRINT("ERRO CreateCompatibleBitmap\n");
return;
}

if(SelectObject(hCaptureDC,hCaptureBitmap) == NULL)
{
//LOG_PRINT("ERRO SelectObject\n");
return;
}


if(!BitBlt(hCaptureDC,nX,nY,nScreenWidth,nScreenHeight,hDesktopDC,0,0,SRCCOPY))
{
//LOG_PRINT("ERRO BitBlt\n");
return;
}

BITMAP bmp;
GetObject(hCaptureBitmap, sizeof(BITMAP), &bmp);

BITMAPINFO info = {sizeof(info)};
info.bmiHeader.biWidth = bmp.bmWidth;
info.bmiHeader.biHeight = -bmp.bmHeight;
info.bmiHeader.biPlanes = 1;
info.bmiHeader.biBitCount = 24;
info.bmiHeader.biCompression = BI_RGB;
LPBYTE buffer = (LPBYTE)malloc(bmp.bmWidth * bmp.bmHeight * 3);  // RGB triples

GetDIBits(hCaptureDC, hCaptureBitmap, 0, bmp.bmHeight, buffer, &info, DIB_RGB_COLORS);
SIZE_T sz = sizeof(BITMAPINFO) + bmp.bmWidth * bmp.bmHeight * 3;
HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, sz);
LPBYTE b = (LPBYTE)GlobalLock(h);
memcpy(b, &info, sizeof(info) - sizeof(RGBQUAD));
memcpy(b + sizeof(info) - sizeof(RGBQUAD), buffer, bmp.bmWidth * bmp.bmHeight * 3);

GlobalUnlock(h);

//SaveDIB(bmp.bmWidth * bmp.bmHeight * 3, 0, (const char *)buffer, "D:/C++GameSamples/D3DvsOGL/D3D/dx9_NatureScene/ss.bmp");
	WriteDIB(
		"D:/C++GameSamples/D3DvsOGL/D3D/dx9_NatureScene/ss.bmp",
		, HANDLE hDIB);

OpenClipboard(hDesktopWnd);
EmptyClipboard();
SetClipboardData(CF_DIB, h);
CloseClipboard();
#if 0
for(int y = 0; y < nScreenHeight; y++)
{
for(int x = 0; x < nScreenWidth; x++)
{
COLORREF c = RGB(buffer[(x + y * bmp.bmHeight * 3) + 2],
buffer[(x + y * bmp.bmHeight * 3) + 1],
buffer[(x + y * bmp.bmHeight * 3) + 0]);
m->tela[x + y * m->width] = c;

BYTE R;
BYTE G;
BYTE B;
R = GetRValue(c);
G = GetGValue(c);
B = GetBValue(c);
printf("\n [%4d, %4d] %08x RGB(%3d, %3d,  %3d)", x, y, c, R ,G, B);
}
}
#endif
free(buffer);
return;
}
/*
int _tmain(int argc, _TCHAR* argv[])
{
matriz * result = GetMatrix();
if(result != NULL)
{
free(result->tela);
free(result);
}
return 0;
}
*/
#endif
#endif
//ImageD3DGL imgS;
#ifdef INCLUDEIMAGE
int SaveDCToBitmap(HWND hStatic, const char *pszFile) {
	BITMAPFILEHEADER bmfh = {0};
    BITMAPINFOHEADER bmih = {0};
	RECT rc = {0};
	HDC hdc1;
	HDC hdc2;
    HBITMAP aBmp;
    BITMAPINFO bi;
    HGDIOBJ OldObj;
    void *dibvalues;
    HANDLE fileHandle;
    DWORD bytes_write;
    DWORD bytes_written;
	int w;
	int h;

	hdc1 = GetWindowDC(hStatic);
	hdc2 = CreateCompatibleDC(hdc1);
	GetWindowRect(hStatic, &rc);
	w = rc.right-rc.left;
	h = rc.bottom-rc.top;

	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = w;
	bmih.biHeight = h;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = BI_RGB;
	bmih.biSizeImage = ((((bmih.biWidth * bmih.biBitCount) + 31) & ~31) >> 3) * bmih.biHeight;
	bi.bmiHeader = bmih;

	aBmp = CreateDIBSection(hdc1, &bi ,DIB_RGB_COLORS, (void**)&dibvalues, NULL, NULL);
	OldObj = SelectObject(hdc2, aBmp);
	BitBlt(hdc2, 0, 0, w, h, hdc1, 0, 0, SRCCOPY);

	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	bmfh.bfSize = (3*bmih.biHeight*bmih.biWidth)+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	bmfh.bfType = 0x4d42;

	if(1)
	{
		ImageD3DGL imgS;
		//LOG_PRINT("1 pixels=%d\n",imgS.getPixels());
	ubyte *fDest = imgS.create(FORMAT_RGBA8, bmih.biWidth,bmih.biHeight, 1, 1);
	for(DWORD y=0;y<bmih.biHeight;y++)
	{
	for(DWORD x=0;x<bmih.biWidth;x++)
	{
		fDest[4*(y*bmih.biWidth+x)+0]=((BYTE*)dibvalues)[3*(y*bmih.biWidth+x)+0];
		fDest[4*(y*bmih.biWidth+x)+1]=((BYTE*)dibvalues)[3*(y*bmih.biWidth+x)+1];
		fDest[4*(y*bmih.biWidth+x)+2]=((BYTE*)dibvalues)[3*(y*bmih.biWidth+x)+2];
		fDest[4*(y*bmih.biWidth+x)+3]=0;//((BYTE*)dibvalues)[3*(y*bmih.biWidth+x)+3];
	}
	}
	LPDIRECT3DTEXTURE9 pTex=0;
	LPDIRECT3DSURFACE9 pSurf=0;
		//LOG_PRINT("2 pixels=%d\n",imgS.getPixels());
	LoadTexture(imgS, (LPDIRECT3DBASETEXTURE9 *)&pTex);
			//LOG_PRINT("3 pixels=%d\n",imgS.getPixels());
	imgS.setPixels(0);
			//LOG_PRINT("4 pixels=%d\n",imgS.getPixels());
	pTex->GetSurfaceLevel(0, &pSurf);
  LPDIRECT3DSURFACE9 fbColor;
  V(g_pd3dDevice->GetRenderTarget(0, &fbColor));

  V(D3DXSaveSurfaceToFile(pszFile,
			      D3DXIFF_JPG,
			      fbColor,
			      NULL,
			      NULL));

	SAFE_RELEASE(pSurf);
	SAFE_RELEASE(pTex);
	SAFE_RELEASE(fbColor);
	}
	else
	{
	fileHandle = CreateFile(pszFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	bytes_write = sizeof(BITMAPFILEHEADER);
	WriteFile(fileHandle, &bmfh, bytes_write, &bytes_written, NULL);
	bytes_write = sizeof(BITMAPINFOHEADER);
	WriteFile(fileHandle, &bmih, bytes_write, &bytes_written, NULL);
	bytes_write = bmih.biSizeImage;
	WriteFile(fileHandle, (void*)dibvalues, bytes_write, &bytes_written, NULL);

	CloseHandle(fileHandle);
	}
    DeleteObject(SelectObject(hdc2,OldObj));
    DeleteDC(hdc2);
	ReleaseDC(hStatic, hdc1);

	return 0;
}
#endif
void SaveScreenShot(const char* fn)
{
	if((fn!=0)||((fn==0)&&(KeyPressed('X'))))
	{
   if(fn!=0)
  {
	  char buf[256];
	  sprintf(buf,"../%s.jpg",fn);
	  //LOG_PRINT("%s\n",buf);
	  #ifdef INCLUDEIMAGE
		SaveDCToBitmap(g_hWnd, buf);
#else
//		SaveScreenShot3(buf);
#endif
	}
	else if(strlen(title)>0)
  {
	  char buf[256];
	  sprintf(buf,"../%s.jpg",title);
	  //LOG_PRINT("%s\n",buf);
	  #ifdef INCLUDEIMAGE
		SaveDCToBitmap(g_hWnd, buf);
#else
//		SaveScreenShot3(buf);
#endif
   }
	else
	{
		//LOG_PRINT("ScreenShot.jpg");

	  #ifdef INCLUDEIMAGE
		SaveDCToBitmap(g_hWnd, "ScreenShot.jpg");
#else
//		SaveScreenShot3("ScreenShot.jpg");
#endif


	}
	}
}
#if 0
void PrintSphe(D3DXVECTOR3 c1)
{
	D3DXVECTOR3 c2;

	float radius=sqrt(c1.x*c1.x+c1.y*c1.y+c1.z*c1.z);
	float phi = asinf(c1.z / radius);
	float theta;//

#if 0
	theta = acosf(c1.x / (radius * cosf(phi)));
#else
	if(0.0<=c1.y)
		theta = acosf(c1.x / (radius* cosf(phi)));
	else if(c1.y<0.0f)
		theta = -acosf(c1.x /(radius*cosf(phi)));
#endif

	c2.x = radius * cosf(phi) * cosf(theta);
	c2.y = radius * cosf(phi) * sinf(theta);
	c2.z = radius * sinf(phi);// = should be identical to starting cZ value
/*
	LOG_PRINT("c1.x=%f\n",c1.x);
	LOG_PRINT("c1.y=%f\n",c1.y);
	LOG_PRINT("c1.z=%f\n",c1.z);
	LOG_PRINT("c2.x=%f\n",c2.x);
	LOG_PRINT("c2.y=%f\n",c2.y);
	LOG_PRINT("c2.z=%f\n",c2.z);
	*/
}
#endif
void CartesianToSpherical(D3DXVECTOR3& c1, float* radius, float* phi, float* theta)
{
	*radius=sqrt(c1.x*c1.x+c1.y*c1.y+c1.z*c1.z);
	*phi = asinf(c1.z / (*radius));
#if 0
	*theta = acosf(c1.x / ((*radius) * cosf((*phi))));
#else
	if(0.0<=c1.y)
		*theta = acosf(c1.x / ((*radius)* cosf((*phi))));
	else if(c1.y<0.0f)
		*theta = -acosf(c1.x /((*radius)*cosf((*phi))));
#endif
}
D3DXVECTOR3 SphericalToCartesian(float radius, float phi, float theta)
{
	D3DXVECTOR3 c2;
    c2.x = radius * cosf(phi) * cosf(theta);
    c2.y = radius * cosf(phi) * sinf(theta);
    c2.z = radius * sinf(phi);
	return c2;
}
void ClearTexture(LPDIRECT3DTEXTURE9 tex, float f)
{/*
	if((tex->Usage&D3DUSAGE_DYNAMIC)==0)
	{
		LOG_PRINT("TextureD3DGL not dynamic\n");
		DBG_HALT;
	}*/
		switch(tex->Format)
		{
		case D3DFMT_A8:
			break;
		case D3DFMT_A8R8G8B8:
			break;

		case D3DFMT_A8B8G8R8:
			break;


		case D3DFMT_R32F:
			break;
		case D3DFMT_A32B32G32R32F:
			break;
		default:
			LOG_PRINT("Unrecognized texture format %d\n",tex->Format);
			DBG_HALT;
		};
	BYTE fb=f/255.0f;//???
	D3DLOCKED_RECT rect;
    V(tex->LockRect(0, &rect, NULL, 0));
    for (int y = 0; y < tex->Height; y++)
	{
    for (int x = 0; x < tex->Width; x++)
	{
		//memcpy(((unsigned char*) rect.pBits) + y * rect.Pitch, pQuantTable + y * rowSize, rowSize);
		switch(tex->Format)
		{
		case D3DFMT_A8:
			*(((BYTE*) rect.pBits) + y * rect.Pitch+x)=fb;
			break;
		case D3DFMT_A8R8G8B8:
			*(((BYTE*) rect.pBits) + y * rect.Pitch+4*x+0)=fb;
			*(((BYTE*) rect.pBits) + y * rect.Pitch+4*x+1)=0;
			*(((BYTE*) rect.pBits) + y * rect.Pitch+4*x+2)=0;
			*(((BYTE*) rect.pBits) + y * rect.Pitch+4*x+3)=1;
			break;

		case D3DFMT_A8B8G8R8:
			*(((BYTE*) rect.pBits) + y * rect.Pitch+4*x+2)=fb;
			*(((BYTE*) rect.pBits) + y * rect.Pitch+4*x+1)=0;
			*(((BYTE*) rect.pBits) + y * rect.Pitch+4*x+0)=0;
			*(((BYTE*) rect.pBits) + y * rect.Pitch+4*x+3)=1;
			break;


		case D3DFMT_R32F:
			*((float*)((BYTE*) rect.pBits) + y * rect.Pitch+x)=f;
			break;
		case D3DFMT_A32B32G32R32F:
			*((float*)((BYTE*) rect.pBits) + y * rect.Pitch+4*x+0)=f;
			*((float*)((BYTE*) rect.pBits) + y * rect.Pitch+4*x+1)=0;
			*((float*)((BYTE*) rect.pBits) + y * rect.Pitch+4*x+2)=0;
			*((float*)((BYTE*) rect.pBits) + y * rect.Pitch+4*x+3)=1;
			break;

		}
	}
	}
    V(tex->UnlockRect(0));
}
void ClearSurface(LPDIRECT3DSURFACE9 surf, DWORD color)
{
  LPDIRECT3DSURFACE9 cachedfbColor=NULL;
  LPDIRECT3DSURFACE9 cachedfbDepth=NULL;
  V(g_pd3dDevice->GetRenderTarget(0, &cachedfbColor));
  V(g_pd3dDevice->GetDepthStencilSurface(&cachedfbDepth));
  V(g_pd3dDevice->SetRenderTarget(0, surf));
  V(g_pd3dDevice->SetDepthStencilSurface(NULL));

    V(g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET /*| D3DCLEAR_ZBUFFER*/, /*D3DCOLOR_COLORVALUE(0.2f,0.0f,0.0f,0.0f)*/color, 1.0f, 0 ));

  V(g_pd3dDevice->SetRenderTarget(0, cachedfbColor));
  V(g_pd3dDevice->SetDepthStencilSurface(cachedfbDepth));
  SAFE_RELEASE(cachedfbColor);
  SAFE_RELEASE(cachedfbDepth);
}
void ClearSurface(LPDIRECT3DSURFACE9 surf, LPDIRECT3DTEXTURE9 src)
{
  LPDIRECT3DSURFACE9 cachedfbColor=NULL;
  LPDIRECT3DSURFACE9 cachedfbDepth=NULL;
  V(g_pd3dDevice->GetRenderTarget(0, &cachedfbColor));
  V(g_pd3dDevice->GetDepthStencilSurface(&cachedfbDepth));
  V(g_pd3dDevice->SetRenderTarget(0, surf));
  V(g_pd3dDevice->SetDepthStencilSurface(NULL));

  D3DVIEWPORT9 viewPort;
  g_pd3dDevice->GetViewport( &viewPort );
  DrawTexture((LPDIRECT3DBASETEXTURE9*)&src,0,0,viewPort.Width,viewPort.Height);

  V(g_pd3dDevice->SetRenderTarget(0, cachedfbColor));
  V(g_pd3dDevice->SetDepthStencilSurface(cachedfbDepth));
  SAFE_RELEASE(cachedfbColor);
  SAFE_RELEASE(cachedfbDepth);
}
/*
void ClearTexture(LPDIRECT3DTEXTURE9 tex, LPDIRECT3DTEXTURE9 src)
{
	  LPDIRECT3DTEXTURE9 tex2=NULL;
	if(tex->Usage&D3DUSAGE_DYNAMIC)
  {
	  V(g_pd3dDevice->CreateTexture(tex->Width,tex->Height, 1, D3DUSAGE_RENDERTARGET|D3DUSAGE_DYNAMIC, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &tex2, NULL));

	D3DLOCKED_RECT rect;
    V(tex->LockRect(0, &rect, NULL, 0));
	D3DLOCKED_RECT rect2;
    V(tex2->LockRect(0, &rect2, NULL, 0));
    for (int y = 0; y < tex->Height; y++)
	{
    for (int x = 0; x < tex->Width; x++)
	{
		//memcpy(((unsigned char*) rect.pBits) + y * rect.Pitch, pQuantTable + y * rowSize, rowSize);
		switch(tex->Format)
		{
		case D3DFMT_A8:
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+0)=0;
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+1)=0;
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+2)=0;
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+3)=float(*((BYTE*) rect.pBits) + y * rect.Pitch+4*x)/255.0f;
			break;
		case D3DFMT_A8R8G8B8:
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+0)=float(*((BYTE*) rect.pBits) + y * rect.Pitch+4*x+0)/255.0f;
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+1)=float(*((BYTE*) rect.pBits) + y * rect.Pitch+4*x+1)/255.0f;
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+2)=float(*((BYTE*) rect.pBits) + y * rect.Pitch+4*x+2)/255.0f;
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+3)=float(*((BYTE*) rect.pBits) + y * rect.Pitch+4*x+3)/255.0f;
			break;
		case D3DFMT_R32F:
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+0)=*((float*)((BYTE*) rect.pBits) + y * rect.Pitch+4*x+0);
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+1)=0;
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+2)=0;
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+3)=0;
			break;
		case D3DFMT_A32B32G32R32F:
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+0)=*((float*)((BYTE*) rect.pBits) + y * rect.Pitch+4*x+0);
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+1)=*((float*)((BYTE*) rect.pBits) + y * rect.Pitch+4*x+1);
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+2)=*((float*)((BYTE*) rect.pBits) + y * rect.Pitch+4*x+2);
			*((float*)((BYTE*) rect2.pBits) + y * rect2.Pitch+4*x+3)=*((float*)((BYTE*) rect.pBits) + y * rect.Pitch+4*x+3);
			break;
		default:
			LOG_PRINT("Unrecognized texture format %d\n",tex->Format);
			DBG_HALT;
			break;
		}
	}
	}
    V(tex->UnlockRect(0));
    V(tex2->UnlockRect(0));

	  tex=tex2;
  }
  if(tex->Usage&D3DUSAGE_RENDERTARGET)
  {
  LPDIRECT3DSURFACE9 pSurf=0;
  tex->GetSurfaceLevel(0,&pSurf);
 ClearSurface(pSurf, src);
 SAFE_RELEASE(pSurf);
  }
  if(tex->Usage&D3DUSAGE_DYNAMIC)
  {
	  SAFE_RELEASE(tex2);
  }
}
*/
void UpdateSubresource(LPDIRECT3DTEXTURE9 pTexture,UINT DstSubresource,const D3DBOX *pDstBox,const void *pSrcData,UINT SrcRowPitch,UINT SrcDepthPitch)
{

	D3DLOCKED_RECT pLockedRect;
	V(pTexture->LockRect(/*0*/DstSubresource,&pLockedRect,NULL,0));
#if 0
  if(pLockedRect.Pitch/pTexture->Width!=SrcRowPitch/*/pDstTexture->Width???*/)
  {
    LOG_PRINT("pLockedRect.Pitch/pTexture->Width!=SrcRowPitch/pDstTexture->Width %d!=%d\n",pSrcLockedRect.Pitch/pSrcTexture->Width,SrcRowPitch/pDstTexture->Width);
    DBG_HALT;
    }
#endif
	DWORD size=pLockedRect.Pitch/pTexture->Width;//1;//4;//???
	//DWORD size=SrcRowPitch/(pDstBox->Right-pDstBox->Left);//???
	//LOG_PRINT("size=%d\n",size);
	for (int y = pDstBox->Top; y < pDstBox->Bottom+1; y++)
	{
		#if 0
		for (int x = pDstBox->Left; x < pDstBox->Right; x++)
		{
			memcpy(
				((BYTE *) pLockedRect.pBits) + y * pLockedRect.Pitch+x*size,
				((BYTE*)pSrcData) + (y - pDstBox->Top) * SrcRowPitch+(x - pDstBox->Left)*size,
				size*sizeof(BYTE));//???
		}
		#else
		//try {
			memcpy(
				((BYTE *) pLockedRect.pBits) + y * pLockedRect.Pitch+pDstBox->Left*size,
				((BYTE*)pSrcData) + (y - pDstBox->Top) * SrcRowPitch,
				size*(pDstBox->Right-pDstBox->Left+1)*sizeof(BYTE));//???
		/*} catch(...)
		{
			LOG_PRINT("Exeption: Access Violation?");
			DWORD size=pLockedRect.Pitch/pTexture->Width;
			LOG_PRINT("size     =%d\n",size);
			LOG_PRINT("pitch    =%d\n",pLockedRect.Pitch);
			LOG_PRINT("width    =%d\n",pTexture->Width);

			LOG_PRINT("pBits    =%d\n",pLockedRect.pBits);
			LOG_PRINT("pSrcData =%d\n",pSrcData);

		}*/
		#endif
	}
	V(pTexture->UnlockRect(/*0*/DstSubresource));
}
void CopySubresourceRegion(
  LPDIRECT3DTEXTURE9 pDstTexture,
  //UINT DstSubresource,
  UINT DstX,
  UINT DstY,
  UINT DstZ,
  LPDIRECT3DSURFACE9 pSrcTexture,
//  UINT SrcSubresource,
  const D3DBOX *pSrcBox
)
//CopySubresourceRegion( g_pStackTexture[level], level, pDstBlocks[i * 2], pDstBlocks[i * 2 + 1], 0, ppBakedTexturesRTV[level], 0, &srcBox );
{
	if(pSrcTexture->Format!=pDstTexture->Format) DBG_HALT;

	D3DLOCKED_RECT pSrcLockedRect;
	V(pSrcTexture->LockRect(&pSrcLockedRect,NULL,0));
	D3DLOCKED_RECT pDstLockedRect;
	V(pDstTexture->LockRect(0,&pDstLockedRect,NULL,0));


  if(pSrcLockedRect.Pitch/pSrcTexture->Width!=pDstLockedRect.Pitch/pDstTexture->Width)
  {
    LOG_PRINT("pSrcLockedRect.Pitch/pSrcTexture->Width!=pDstLockedRect.Pitch/pDstTexture->Width %d!=%d\n",pSrcLockedRect.Pitch/pSrcTexture->Width,pDstLockedRect.Pitch/pDstTexture->Width);
    DBG_HALT;
    }

	DWORD size=pSrcLockedRect.Pitch/pSrcTexture->Width;//4;//???
	//DWORD size=pSrcLockedRect.Pitch/(pSrcBox->Right-pSrcBox->Left)/sizeof(BYTE);//???
	//LOG_PRINT("size=%d\n",size);
	for (int y = pSrcBox->Top; y < pSrcBox->Bottom/*+1???*/; y++)
	{
	  #if 0
		for (int x = pSrcBox->Left; x < pSrcBox->Right; x++)
		{
			memcpy(
				((BYTE *) pDstLockedRect.pBits) + (y-pSrcBox->Top+DstY) * pDstLockedRect.Pitch+(x-pSrcBox->Left+DstX)*size,
				((BYTE *) pSrcLockedRect.pBits) + y * pSrcLockedRect.Pitch+x*size,
				size*sizeof(BYTE));
		}
	 #else
			memcpy(
				((BYTE *) pDstLockedRect.pBits) + (y-pSrcBox->Top+DstY) * pDstLockedRect.Pitch+/*(0-pSrcBox->Left+*/DstX/*)*/*size,
				((BYTE *) pSrcLockedRect.pBits) + y * pSrcLockedRect.Pitch+pSrcBox->Left*size,
				size*(pSrcBox->Right-pSrcBox->Left/*+1???*/)*sizeof(BYTE));
	 #endif
	}
/*
				((BYTE *) pDstLockedRect.pBits) + (y-pSrcBox->Top+DstY) * pDstLockedRect.Pitch+(x-pSrcBox->Left+DstX)*size,
				((BYTE *) pSrcLockedRect.pBits) + y * pSrcLockedRect.Pitch+x*size,

		   ((BYTE *) pDstLockedRect.pBits) + (y-pSrcBox->Top+DstY) * pDstLockedRect.Pitch+(x-pSrcBox->Left+DstX)*size,
		   ((BYTE *) pDstLockedRect.pBits) + (y-pSrcBox->Top+DstY) * pDstLockedRect.Pitch+(x+1-pSrcBox->Left+DstX)*size,

*/
	V(pSrcTexture->UnlockRect());
	V(pDstTexture->UnlockRect(0));
}
void CopySubresourceRegion(
  LPDIRECT3DTEXTURE9 pDstTexture,
  //UINT DstSubresource,
  UINT DstX,
  UINT DstY,
  UINT DstZ,
  LPDIRECT3DTEXTURE9 pSrcTexture,
//  UINT SrcSubresource,
  const D3DBOX *pSrcBox
)
{
  LPDIRECT3DSURFACE9 pSrcS=NULL;

  pSrcTexture->GetSurfaceLevel(0,&pSrcS);

 CopySubresourceRegion(
  pDstTexture,
  //UINT DstSubresource,
  DstX,
  DstY,
  DstZ,
  pSrcS,
//  UINT SrcSubresource,
  pSrcBox
);
SAFE_RELEASE(pSrcS);
}
void SaveSurface(LPDIRECT3DSURFACE9 pS,const char* fn,D3DXIMAGE_FILEFORMAT f)
{
	char pszFile[256];
	if(f==D3DXIFF_DDS)
	sprintf(pszFile,"%s.dds",fn);
	else if(f==D3DXIFF_JPG)
	sprintf(pszFile,"%s.jpg",fn);

  else if(f==D3DXIFF_BMP)
  sprintf(pszFile,"%s.bmp",fn);
  else if(f==D3DXIFF_TGA)
  sprintf(pszFile,"%s.tga",fn);
  else if(f==D3DXIFF_PNG)
  sprintf(pszFile,"%s.png",fn);
  else if(f==D3DXIFF_PPM)
  sprintf(pszFile,"%s.ppm",fn);
  else if(f==D3DXIFF_DIB)
  sprintf(pszFile,"%s.dib",fn);
  else if(f==D3DXIFF_HDR)
  sprintf(pszFile,"%s.hdr",fn);
  else if(f==D3DXIFF_PFM)
  sprintf(pszFile,"%s.pfm",fn);

	else
		DBG_HALT;

	V(D3DXSaveSurfaceToFile(pszFile,
			      f,
			      pS,
			      NULL,
			      NULL));
}
void ConvertTo(const char* fn,D3DXIMAGE_FILEFORMAT f)
{
  LPDIRECT3DTEXTURE9 pTexture=0;

  char nfn[256];
  if(f==D3DXIFF_DDS)
  sprintf(nfn,"%s.dds",fn);
  else if(f==D3DXIFF_JPG)
  sprintf(nfn,"%s.jpg",fn);

  else if(f==D3DXIFF_BMP)
  sprintf(nfn,"%s.bmp",fn);
  else if(f==D3DXIFF_TGA)
  sprintf(nfn,"%s.tga",fn);
  else if(f==D3DXIFF_PNG)
  sprintf(nfn,"%s.png",fn);
  else if(f==D3DXIFF_PPM)
  sprintf(nfn,"%s.ppm",fn);
  else if(f==D3DXIFF_DIB)
  sprintf(nfn,"%s.dib",fn);
  else if(f==D3DXIFF_HDR)
  sprintf(nfn,"%s.hdr",fn);
  else if(f==D3DXIFF_PFM)
  sprintf(nfn,"%s.pfm",fn);

  else DBG_HALT;
#if 0
#if 0
  LoadTexture(fn,(LPDIRECT3DBASETEXTURE9*)&pTexture,FORMAT_RGBA8);
#else
  ImageD3DGL img;
  img.loadFromFile(fn);
  img.decompress();

  img.removeMipMaps(1, 1);//int mipMapsToKeep = ALL_MIPMAPS);
  img.convert(FORMAT_RGBA8);
  img.resize(256,256,1);
#if 0
  BYTE* p=img.getPixels();

  for(unsigned int y=0;y<img.height;y++)
  {
  for(unsigned int x=0;x<img.width;x++)
  {
	int idx=y*img.width+x;
	if(p[idx*4+3]>0)
	{
		BYTE a=p[idx*4+3];
/*
		LOG_PRINT("R=%d\n",p[idx*4+0]);
		LOG_PRINT("G=%d\n",p[idx*4+1]);
		LOG_PRINT("B=%d\n",p[idx*4+2]);
		LOG_PRINT("A=%d\n",p[idx*4+3]);
		*/
		p[idx*4+3]=0;//255;

		if(p[idx*4+0]!=a) p[idx*4+0]=0;
		if(p[idx*4+1]!=a) p[idx*4+1]=0;
		if(p[idx*4+2]!=a) p[idx*4+2]=0;

		//DBG_HALT;
	}
	else
	{
		p[idx*4+0]=0;
		p[idx*4+1]=0;
		p[idx*4+2]=0;
		p[idx*4+3]=0;
	}

  }
}
#endif
  //img.convert(FORMAT_RGB8);

  LoadTexture(img,(LPDIRECT3DBASETEXTURE9*)&pTexture,FORMAT_RGBA8);
#endif
#else
		V(
		//D3DXCreateTextureFromFile(g_pd3dDevice, file, &texture)
	    D3DXCreateTextureFromFileEx(
					g_pd3dDevice,
					fn,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					0,
					0,
					D3DFMT_A8R8G8B8,//D3DFMT_UNKNOWN,
					D3DPOOL_MANAGED,
					D3DX_FILTER_LINEAR,
					D3DX_FILTER_LINEAR,
					0,
					NULL,
					NULL,
					&pTexture
					)
		);
#endif
  SaveTexture(pTexture,nfn,f);
}
void SaveTexture(LPDIRECT3DTEXTURE9 pTexture,const char* fn,D3DXIMAGE_FILEFORMAT f)
{
	LPDIRECT3DSURFACE9 pS=NULL;
	pTexture->GetSurfaceLevel(0,&pS);
#if 0
	char pszFile[256];
	sprintf(pszFile,"%s.dds",fn);
  V(D3DXSaveSurfaceToFile(pszFile,
			      D3DXIFF_DDS,
			      pS,
			      NULL,
			      NULL));
#else
	SaveSurface(pS, fn,f);
#endif
  SAFE_RELEASE(pS);
}
#if 1//def INCLUDEIMAGE
void Save3Image(const char* fn)
{
//	ImageD3DGL img;
	char pszFile[256];
	sprintf(pszFile,"%s.dds",fn);

	//img.loadFromFile(fn);
//	img.loadJPG(fn);
	LPDIRECT3DTEXTURE9 pTexture=NULL;
	LPDIRECT3DSURFACE9 pS=NULL;

//	img.resize(2048,1024,1);
//	LoadTexture(img,(LPDIRECT3DBASETEXTURE9*)&pTexture);
	V(D3DXCreateTextureFromFileEx(g_pd3dDevice, pszFile,2048,1024, 1,0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pTexture));
	pTexture->GetSurfaceLevel(0,&pS);
	sprintf(pszFile,"%s2k.jpg",fn);
  V(D3DXSaveSurfaceToFile(pszFile,
			      D3DXIFF_JPG,
			      pS,
			      NULL,
			      NULL));
  SAFE_RELEASE(pS);
	SAFE_RELEASE(pTexture);

	//img.resize(1024,512,1);
	//LoadTexture(img,(LPDIRECT3DBASETEXTURE9*)&pTexture);
	V(D3DXCreateTextureFromFileEx(g_pd3dDevice, pszFile,1024,512, 1,0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pTexture));
	pTexture->GetSurfaceLevel(0,&pS);
	sprintf(pszFile,"%s1k.jpg",fn);
  V(D3DXSaveSurfaceToFile(pszFile,
			      D3DXIFF_JPG,
			      pS,
			      NULL,
			      NULL));
  SAFE_RELEASE(pS);
	SAFE_RELEASE(pTexture);

	//img.resize(512,256,1);
	//LoadTexture(img,(LPDIRECT3DBASETEXTURE9*)&pTexture);
	V(D3DXCreateTextureFromFileEx(g_pd3dDevice, pszFile,512,256, 1,0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pTexture));
	pTexture->GetSurfaceLevel(0,&pS);
	sprintf(pszFile,"%s0k.jpg",fn);
  V(D3DXSaveSurfaceToFile(pszFile,
			      D3DXIFF_JPG,
			      pS,
			      NULL,
			      NULL));
  SAFE_RELEASE(pS);
	SAFE_RELEASE(pTexture);

}
#endif
/*
typedef enum _D3DFORMAT
{
    D3DFMT_UNKNOWN		=  0,

    D3DFMT_R8G8B8		= 20,
    D3DFMT_A8R8G8B8		= 21,
    D3DFMT_X8R8G8B8		= 22,
    D3DFMT_R5G6B5		= 23,
    D3DFMT_X1R5G5B5		= 24,
    D3DFMT_A1R5G5B5		= 25,
    D3DFMT_A4R4G4B4		= 26,
    D3DFMT_R3G3B2		= 27,
    D3DFMT_A8			= 28,
    D3DFMT_A8R3G3B2		= 29,
    D3DFMT_X4R4G4B4		= 30,
    D3DFMT_A2B10G10R10		= 31,
    D3DFMT_A8B8G8R8		= 32,
    D3DFMT_X8B8G8R8		= 33,
    D3DFMT_G16R16		= 34,
    D3DFMT_A2R10G10B10		= 35,
    D3DFMT_A16B16G16R16 	= 36,

    D3DFMT_A8P8 		= 40,
    D3DFMT_P8			= 41,

    D3DFMT_L8			= 50,
    D3DFMT_A8L8 		= 51,
    D3DFMT_A4L4 		= 52,

    D3DFMT_V8U8 		= 60,
    D3DFMT_L6V5U5		= 61,
    D3DFMT_X8L8V8U8		= 62,
    D3DFMT_Q8W8V8U8		= 63,
    D3DFMT_V16U16		= 64,
    D3DFMT_A2W10V10U10		= 67,

    D3DFMT_UYVY 		= MAKEFOURCC('U', 'Y', 'V', 'Y'),
    D3DFMT_R8G8_B8G8		= MAKEFOURCC('R', 'G', 'B', 'G'),
    D3DFMT_YUY2 		= MAKEFOURCC('Y', 'U', 'Y', '2'),
    D3DFMT_G8R8_G8B8		= MAKEFOURCC('G', 'R', 'G', 'B'),
    D3DFMT_DXT1 		= MAKEFOURCC('D', 'X', 'T', '1'),
    D3DFMT_DXT2 		= MAKEFOURCC('D', 'X', 'T', '2'),
    D3DFMT_DXT3 		= MAKEFOURCC('D', 'X', 'T', '3'),
    D3DFMT_DXT4 		= MAKEFOURCC('D', 'X', 'T', '4'),
    D3DFMT_DXT5 		= MAKEFOURCC('D', 'X', 'T', '5'),

    D3DFMT_D16_LOCKABLE 	= 70,
    D3DFMT_D32			= 71,
    D3DFMT_D15S1		= 73,
    D3DFMT_D24S8		= 75,
    D3DFMT_D24X8		= 77,
    D3DFMT_D24X4S4		= 79,
    D3DFMT_D16			= 80,

    D3DFMT_D32F_LOCKABLE	= 82,
    D3DFMT_D24FS8		= 83,


    D3DFMT_L16			= 81,

    D3DFMT_VERTEXDATA		=100,
    D3DFMT_INDEX16		=101,
    D3DFMT_INDEX32		=102,

    D3DFMT_Q16W16V16U16 	=110,

    D3DFMT_MULTI2_ARGB8 	= MAKEFOURCC('M','E','T','1'),

    // Floating point surface formats

    // s10e5 formats (16-bits per channel)
    D3DFMT_R16F 		= 111,
    D3DFMT_G16R16F		= 112,
    D3DFMT_A16B16G16R16F	= 113,

    // IEEE s23e8 formats (32-bits per channel)
    D3DFMT_R32F 		= 114,
    D3DFMT_G32R32F		= 115,
    D3DFMT_A32B32G32R32F	= 116,

    D3DFMT_CxV8U8		= 117,


    D3DFMT_FORCE_DWORD		=0x7fffffff
} D3DFORMAT;

*/
/*
    D3DFMT_A8R8G8B8		= 21,

    D3DFMT_A1R5G5B5		= 25,
    D3DFMT_A4R4G4B4		= 26,

    D3DFMT_A8			= 28,
    D3DFMT_A8R3G3B2		= 29,

    D3DFMT_A2B10G10R10		= 31,
    D3DFMT_A8B8G8R8		= 32,
    D3DFMT_X8B8G8R8		= 33,

    D3DFMT_A2R10G10B10		= 35,
    D3DFMT_A16B16G16R16 	= 36,
    D3DFMT_A8P8 		= 40,
    D3DFMT_A8L8 		= 51,
    D3DFMT_A4L4 		= 52,
    D3DFMT_A2W10V10U10		= 67,
    D3DFMT_MULTI2_ARGB8 	= MAKEFOURCC('M','E','T','1'),
    D3DFMT_A16B16G16R16F	= 113,
    D3DFMT_A32B32G32R32F	= 116,
*/
void AddAlpha(LPDIRECT3DTEXTURE9 pTexture, float a)
{
  D3DLOCKED_RECT rect;
  pTexture->LockRect(0, &rect, NULL, 0);
  for(unsigned int y=0;y<pTexture->Height;y++)
	{
  for(unsigned int x=0;x<pTexture->Width;x++)
	{
   int idx=pTexture->Width*y+x;
   switch(pTexture->Format)
   {
     case D3DFMT_A8R8G8B8:
	  ((BYTE*)rect.pBits)[idx*4+0]=BYTE(a);
	  break;
     case D3DFMT_A8B8G8R8:
	  ((BYTE*)rect.pBits)[idx*4+0]=BYTE(a);
	  break;
     default:
     DBG_HALT;

   };
 }
 }
  pTexture->UnlockRect(0);
}
/*
typedef enum _D3DFORMAT
{
    D3DFMT_UNKNOWN		=  0,

    D3DFMT_R8G8B8		= 20,
    D3DFMT_A8R8G8B8		= 21,
    D3DFMT_X8R8G8B8		= 22,
    D3DFMT_R5G6B5		= 23,
    D3DFMT_X1R5G5B5		= 24,
    D3DFMT_A1R5G5B5		= 25,
    D3DFMT_A4R4G4B4		= 26,
    D3DFMT_R3G3B2		= 27,
    D3DFMT_A8			= 28,
    D3DFMT_A8R3G3B2		= 29,
    D3DFMT_X4R4G4B4		= 30,
    D3DFMT_A2B10G10R10		= 31,
    D3DFMT_A8B8G8R8		= 32,
    D3DFMT_X8B8G8R8		= 33,
    D3DFMT_G16R16		= 34,
    D3DFMT_A2R10G10B10		= 35,
    D3DFMT_A16B16G16R16 	= 36,

    D3DFMT_A8P8 		= 40,
    D3DFMT_P8			= 41,

    D3DFMT_L8			= 50,
    D3DFMT_A8L8 		= 51,
    D3DFMT_A4L4 		= 52,

    D3DFMT_V8U8 		= 60,
    D3DFMT_L6V5U5		= 61,
    D3DFMT_X8L8V8U8		= 62,
    D3DFMT_Q8W8V8U8		= 63,
    D3DFMT_V16U16		= 64,
    D3DFMT_A2W10V10U10		= 67,

    D3DFMT_UYVY 		= MAKEFOURCC('U', 'Y', 'V', 'Y'),
    D3DFMT_R8G8_B8G8		= MAKEFOURCC('R', 'G', 'B', 'G'),
    D3DFMT_YUY2 		= MAKEFOURCC('Y', 'U', 'Y', '2'),
    D3DFMT_G8R8_G8B8		= MAKEFOURCC('G', 'R', 'G', 'B'),
    D3DFMT_DXT1 		= MAKEFOURCC('D', 'X', 'T', '1'),
    D3DFMT_DXT2 		= MAKEFOURCC('D', 'X', 'T', '2'),
    D3DFMT_DXT3 		= MAKEFOURCC('D', 'X', 'T', '3'),
    D3DFMT_DXT4 		= MAKEFOURCC('D', 'X', 'T', '4'),
    D3DFMT_DXT5 		= MAKEFOURCC('D', 'X', 'T', '5'),

    D3DFMT_D16_LOCKABLE 	= 70,
    D3DFMT_D32			= 71,
    D3DFMT_D15S1		= 73,
    D3DFMT_D24S8		= 75,
    D3DFMT_D24X8		= 77,
    D3DFMT_D24X4S4		= 79,
    D3DFMT_D16			= 80,

    D3DFMT_D32F_LOCKABLE	= 82,
    D3DFMT_D24FS8		= 83,


    D3DFMT_L16			= 81,

    D3DFMT_VERTEXDATA		=100,
    D3DFMT_INDEX16		=101,
    D3DFMT_INDEX32		=102,

    D3DFMT_Q16W16V16U16 	=110,

    D3DFMT_MULTI2_ARGB8 	= MAKEFOURCC('M','E','T','1'),

    // Floating point surface formats

    // s10e5 formats (16-bits per channel)
    D3DFMT_R16F 		= 111,
    D3DFMT_G16R16F		= 112,
    D3DFMT_A16B16G16R16F	= 113,

    // IEEE s23e8 formats (32-bits per channel)
    D3DFMT_R32F 		= 114,
    D3DFMT_G32R32F		= 115,
    D3DFMT_A32B32G32R32F	= 116,

    D3DFMT_CxV8U8		= 117,


    D3DFMT_FORCE_DWORD		=0x7fffffff
} D3DFORMAT;
*/
/*
    case D3DFMT_A8:
	 break;
    case D3DFMT_L8:
	 break;
    case D3DFMT_P8:
	 break;
    case D3DFMT_A8L8:
	 break;
    case D3DFMT_A8P8:
	 break;
    case D3DFMT_R8G8B8:
	 break;
    case D3DFMT_X8R8G8B8:
	 break;
    case D3DFMT_X8B8G8R8:
	 break;
    case D3DFMT_A8R8G8B8:
	 break;
    case D3DFMT_A8B8G8R8:
	 break;
    case D3DFMT_L16:
	 break;
    case D3DFMT_G16R16:
	 break;
    case D3DFMT_A16B16G16R16:
	 break;
    case D3DFMT_R16F:
	 break;
    case D3DFMT_G16R16F:
	 break;
    case D3DFMT_A16B16G16R16F:
	 break;
    case D3DFMT_R32F:
	 break;
    case D3DFMT_G32R32F:
	 break;
    case D3DFMT_A32B32G32R32F:
	 break;
*/
#ifdef INCLUDEIMAGE
VOID GetARGB(PVOID p, UINT i, D3DFORMAT f, FLOAT& a, FLOAT& r, FLOAT& g, FLOAT& b)
{
    a=0.0f;
    r=0.0f;
    g=0.0f;
    b=0.0f;
    switch(f)
    {
    case D3DFMT_A8:
    case D3DFMT_L8:
    case D3DFMT_P8:
	 a=((BYTE*)p)[i];
	 break;
    case D3DFMT_A8L8:
    case D3DFMT_A8P8:
	 a=((BYTE*)p)[2*i];
	 r=((BYTE*)p)[2*i+1];
	 break;
    case D3DFMT_R8G8B8:
    case D3DFMT_X8R8G8B8:
	 r=((BYTE*)p)[3*i];
	 g=((BYTE*)p)[3*i+1];
	 b=((BYTE*)p)[3*i+2];
	 break;
    case D3DFMT_X8B8G8R8:
	 b=((BYTE*)p)[3*i];
	 g=((BYTE*)p)[3*i+1];
	 r=((BYTE*)p)[3*i+2];
	 break;
    case D3DFMT_A8R8G8B8:
	 a=((BYTE*)p)[4*i];
	 r=((BYTE*)p)[4*i+1];
	 g=((BYTE*)p)[4*i+2];
	 b=((BYTE*)p)[4*i+3];
	 break;
    case D3DFMT_A8B8G8R8:
	 a=((BYTE*)p)[4*i];
	 b=((BYTE*)p)[4*i+1];
	 g=((BYTE*)p)[4*i+2];
	 r=((BYTE*)p)[4*i+3];
	 break;
    case D3DFMT_L16:
	 r=((short int*)p)[4*i];
	 break;
    case D3DFMT_G16R16:
	 g=((short int*)p)[4*i];
	 r=((short int*)p)[4*i+1];
	 break;
    case D3DFMT_A16B16G16R16:
	 a=((short int*)p)[4*i];
	 b=((short int*)p)[4*i+1];
	 g=((short int*)p)[4*i+2];
	 r=((short int*)p)[4*i+3];
	 break;
    case D3DFMT_R16F:
	 r=((half*)p)[4*i];
	 break;
    case D3DFMT_G16R16F:
	 g=((half*)p)[4*i];
	 r=((half*)p)[4*i+1];
	 break;
    case D3DFMT_A16B16G16R16F:
	 a=((half*)p)[4*i];
	 b=((half*)p)[4*i+1];
	 g=((half*)p)[4*i+2];
	 r=((half*)p)[4*i+3];
	 break;
    case D3DFMT_R32F:
	 r=((float*)p)[4*i];
	 break;
    case D3DFMT_G32R32F:
	 g=((float*)p)[4*i];
	 r=((float*)p)[4*i+1];
	 break;
    case D3DFMT_A32B32G32R32F:
	 a=((float*)p)[4*i];
	 b=((float*)p)[4*i+1];
	 g=((float*)p)[4*i+2];
	 r=((float*)p)[4*i+3];
	 break;
    default:
	    DBG_HALT;
    };
}
VOID SetARGB(PVOID p, UINT i, D3DFORMAT f, FLOAT a, FLOAT r, FLOAT g, FLOAT b)
{
    switch(f)
    {
    case D3DFMT_A8:
    case D3DFMT_L8:
    case D3DFMT_P8:
	 ((BYTE*)p)[i]=BYTE(a);
	 break;
    case D3DFMT_A8L8:
    case D3DFMT_A8P8:
	 ((BYTE*)p)[2*i]=BYTE(a);
	 ((BYTE*)p)[2*i+1]=BYTE(r);
	 break;
    case D3DFMT_R8G8B8:
    case D3DFMT_X8R8G8B8:
	 ((BYTE*)p)[3*i]=BYTE(r);
	 ((BYTE*)p)[3*i+1]=BYTE(g);
	 ((BYTE*)p)[3*i+2]=BYTE(b);
	 break;
    case D3DFMT_X8B8G8R8:
	 ((BYTE*)p)[3*i]=BYTE(b);
	 ((BYTE*)p)[3*i+1]=BYTE(g);
	 ((BYTE*)p)[3*i+2]=BYTE(r);
	 break;
    case D3DFMT_A8R8G8B8:
	 ((BYTE*)p)[4*i]=BYTE(a);
	 ((BYTE*)p)[4*i+1]=BYTE(r);
	 ((BYTE*)p)[4*i+2]=BYTE(g);
	 ((BYTE*)p)[4*i+3]=BYTE(b);
	 break;
    case D3DFMT_A8B8G8R8:
	 ((BYTE*)p)[4*i+0]=BYTE(a);
	 ((BYTE*)p)[4*i+1]=BYTE(b);
	 ((BYTE*)p)[4*i+2]=BYTE(g);
	 ((BYTE*)p)[4*i+3]=BYTE(r);
	 break;
    case D3DFMT_L16:
	 ((short int*)p)[4*i]=short int(r);
	 break;
    case D3DFMT_G16R16:
	 ((short int*)p)[4*i]=short int(g);
	 ((short int*)p)[4*i+1]=short int(r);
	 break;
    case D3DFMT_A16B16G16R16:
	 ((short int*)p)[4*i]=short int(a);
	 ((short int*)p)[4*i+1]=short int(b);
	 ((short int*)p)[4*i+2]=short int(g);
	 ((short int*)p)[4*i+3]=short int(r);
	 break;
    case D3DFMT_R16F:
	 ((half*)p)[4*i]=half(r);
	 break;
    case D3DFMT_G16R16F:
	 ((half*)p)[4*i]=half(g);
	 ((half*)p)[4*i+1]=half(r);
	 break;
    case D3DFMT_A16B16G16R16F:
	 ((half*)p)[4*i]=half(a);
	 ((half*)p)[4*i+1]=half(b);
	 ((half*)p)[4*i+2]=half(g);
	 ((half*)p)[4*i+3]=half(r);
	 break;
    case D3DFMT_R32F:
	 ((FLOAT*)p)[4*i]=r;
	 break;
    case D3DFMT_G32R32F:
	 ((FLOAT*)p)[4*i]=g;
	 ((FLOAT*)p)[4*i+1]=r;
	 break;
    case D3DFMT_A32B32G32R32F:
	 ((FLOAT*)p)[4*i]=a;
	 ((FLOAT*)p)[4*i+1]=b;
	 ((FLOAT*)p)[4*i+2]=g;
	 ((FLOAT*)p)[4*i+3]=r;
	 break;
    default:
	    DBG_HALT;
    };
}
#endif
void  R2VBOrVTFSupport(bool& useR2VB, bool& supportsHalf)
{

	HRESULT hr=0;
	supportsHalf = false;

	// Check for R2VB support
	if (g_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, (D3DFORMAT) MAKEFOURCC('R','2','V','B')) == D3D_OK){
		useR2VB = true;
		if (g_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F) == D3D_OK){
			if (g_caps.DeclTypes & D3DDTCAPS_FLOAT16_4) supportsHalf = true;
		}

	} else {
		useR2VB = false;

		// Check for VS3.0 support
		if (g_caps.VertexShaderVersion >= D3DVS_VERSION(3,0)){

			// Check for VTF support for halfs and floats
			if (g_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DUSAGE_QUERY_VERTEXTEXTURE, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F) == D3D_OK){
				if (g_caps.DeclTypes & D3DDTCAPS_FLOAT16_4) supportsHalf = true;
			} else if (g_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DUSAGE_QUERY_VERTEXTEXTURE, D3DRTYPE_TEXTURE, D3DFMT_A32B32G32R32F) != D3D_OK){
				LOG_PRINT("This demo requires R2VB or VTF support.");
				return;
			}

		} else {
			LOG_PRINT("This demo requires R2VB or VTF support.");
			return;
		}
	}
}
bool DebugRuntime()
{
	// Detect whether the debug runtime is active
	bool debugRuntime = false;
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Direct3D", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS){
		DWORD size = sizeof(DWORD);
		DWORD value;
		if (RegQueryValueEx(hKey, "LoadDebugRuntime", NULL, NULL, (LPBYTE) &value, &size) == ERROR_SUCCESS){
			debugRuntime = (value != 0);
		}
		RegCloseKey(hKey);
	}
 return debugRuntime;
}
/*
void FillTexture(LPDIRECT3DTEXTURE9 pTexture,
	       void* pData)
{
  D3DLOCKED_RECT rect;
  pTexture->LockRect(0, &rect, NULL, 0);
  for(unsigned int y=0;y<pTexture->Height;y++)
	{
  for(unsigned int x=0;x<pTexture->Width;x++)
	{
   int idx=pTexture->Width*y+x;
   switch(pTexture->Format)
   {
     case D3DFMT_A8R8G8B8:
	  ((BYTE*)rect.pBits)[idx*4+0]=BYTE(a);
	  break;
     case D3DFMT_A8B8G8R8:
	  ((BYTE*)rect.pBits)[idx*4+0]=BYTE(a);
	  break;
     default:
     DBG_HALT;

   };
 }
 }
  pTexture->UnlockRect(0);
}
*/

void CompileshadersFromFiles(
		 const char* vertex_shaderfile,  const char* mainvs,
		 const char* pixel_shaderfile,	const char* mainps,
	LPDIRECT3DVERTEXSHADER9*      pVertexShader,
	LPD3DXCONSTANTTABLE*	      pConstantTableVS,
	LPDIRECT3DPIXELSHADER9* pPixelShader,
	LPD3DXCONSTANTTABLE*	pConstantTablePS,SM sm
		 )
{
    HRESULT hr=0;
	LPD3DXBUFFER pCode;
    DWORD dwShaderFlags = 0;
	LPD3DXBUFFER pBufferErrors = NULL;
     #if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DXSHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows
    // the shaders to be optimized and to run exactly the way they will run in
    // the release configuration of this program.
    dwShaderFlags |= D3DXSHADER_DEBUG;
    #endif

    #ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_SKIPOPTIMIZATION|D3DXSHADER_DEBUG;
    #endif
    #ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_SKIPOPTIMIZATION|D3DXSHADER_DEBUG;
    #endif
/*
D3DXMACRO defines[2] =
    {
	{ "__HLSL__", "" },
	{ NULL, NULL }
    };
*/
if(vertex_shaderfile && mainvs)
{
    // Assemble the vertex shader from the file
    switch(sm)
	{
		case 	SM_1_0: 
	hr=D3DXCompileShaderFromFile( vertex_shaderfile, defines,NULL,  mainvs,
				    "vs_1_0", dwShaderFlags, &pCode,
				    &pBufferErrors, pConstantTableVS );
		break;
		case 	SM_2_0: 
	hr=D3DXCompileShaderFromFile( vertex_shaderfile, defines,NULL,  mainvs,
				    "vs_2_0", dwShaderFlags, &pCode,
				    &pBufferErrors, pConstantTableVS );
		break;
		case 	SM_3_0: 
	hr=D3DXCompileShaderFromFile( vertex_shaderfile, defines,NULL,  mainvs,
				    "vs_3_0", dwShaderFlags, &pCode,
				    &pBufferErrors, pConstantTableVS );
		break;
		default:
			DBG_HALT;
			break;
};
    if( FAILED(hr) )
	{
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		LOG_PRINT("Vertex ShaderD3DGL Compile Error\n");
		LOG_PRINT( (const char*)pCompilErrors);
				DBG_HALT;
	}

    // Create the vertex shader
    V(g_pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(),
				      pVertexShader ));
    pCode->Release();
}
    //
    // Create a HLSL based pixel shader.
    //
if(pixel_shaderfile && mainps)
{
    // Assemble the vertex shader from the file
    switch(sm)
	{
		case 	SM_1_0: 
    hr=D3DXCompileShaderFromFile( pixel_shaderfile, defines,NULL,  mainps,
				    "ps_1_0", dwShaderFlags, &pCode,
				    &pBufferErrors, pConstantTablePS );
	break;
		case 	SM_2_0: 
    hr=D3DXCompileShaderFromFile( pixel_shaderfile, defines,NULL,  mainps,
				    "ps_2_0", dwShaderFlags, &pCode,
				    &pBufferErrors, pConstantTablePS );
	break;
		case 	SM_3_0: 
		    hr=D3DXCompileShaderFromFile( pixel_shaderfile, defines,NULL,  mainps,
				    "ps_3_0", dwShaderFlags, &pCode,
				    &pBufferErrors, pConstantTablePS );
	break;
		default:
			DBG_HALT;
};


    if( FAILED(hr) )
	{
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		LOG_PRINT("Pixel ShaderD3DGL Compile Error\n");
		LOG_PRINT( (const char*)pCompilErrors);
				DBG_HALT;
	}

    // Create the vertex shader
    V(g_pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(),
				      pPixelShader ));
    pCode->Release();
}
}
void SetSamplerState(int i, TextureFilterParams ss)
{
	if(i>-1)
	{
		V(g_pd3dDevice->SetSamplerState(i, D3DSAMP_MINFILTER, ss.minFilter));
		V(g_pd3dDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, ss.magFilter));
		V(g_pd3dDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, ss.mipFilter));

		V(g_pd3dDevice->SetSamplerState(i, D3DSAMP_ADDRESSU, ss.wrapS));
		V( g_pd3dDevice->SetSamplerState(i, D3DSAMP_ADDRESSV, ss.wrapT));
		V(g_pd3dDevice->SetSamplerState(i, D3DSAMP_ADDRESSW, ss.wrapR));

		V(g_pd3dDevice->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, ss.maxAniso));

		V(g_pd3dDevice->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *(DWORD *) &ss.lod));
	}
}
/*
enum {
	D3DXRS_SAMPLER
};
*/
int GetSamplerIndex(LPD3DXCONSTANTTABLE pConstants, const char* UniformName)
{
    int i=-1;
	D3DXHANDLE handle=0;
	if(pConstants && (handle = pConstants->GetConstantByName(NULL,UniformName)))
      {
      D3DXCONSTANT_DESC constDesc;
      UINT count = 1;
      pConstants->GetConstantDesc(handle,&constDesc,&count);

      if(constDesc.RegisterSet == D3DXRS_SAMPLER)
	i = constDesc.RegisterIndex;
      }
	
    return i;
}
void SetSamplerState(LPD3DXCONSTANTTABLE pConstants, const char* UniformName, TextureFilterParams ss)
{
	int i=-1;
	i=GetSamplerIndex(pConstants, UniformName);
	if(i!=-1)SetSamplerState(i, ss);
}
int CreateSamplerState(
	D3DTEXTUREFILTERTYPE minFilter,
	D3DTEXTUREFILTERTYPE magFilter,
	D3DTEXTUREFILTERTYPE mipFilter,
	D3DTEXTUREADDRESS wrapS,
	D3DTEXTUREADDRESS wrapT,
	D3DTEXTUREADDRESS wrapR,
	DWORD maxAniso,
	float lod)
{
	TextureFilterParams tfp=TextureFilterParams(
		minFilter,magFilter,mipFilter,
		wrapS,wrapT,wrapR,maxAniso,lod);
	g_SamplerStates.push_back(tfp);
	return g_SamplerStates.size();
}
void SetSamplerState(int sampler, int state)
{
	if(sampler>-1&&sampler<g_SamplerStates.size()&&state>-1)
		SetSamplerState(sampler, g_SamplerStates[state]);
}
int EraseSamplerState(int i)
{
	if(i<g_SamplerStates.size()&&i>-1)
		g_SamplerStates.erase( g_SamplerStates.begin()+i);
	return g_SamplerStates.size();
}
#if 0
int CreateTexture(const char * file, bool atlas, D3DFORMAT f, unsigned int w,
				  unsigned int h, DWORD u, int atfp)
{
  LPDIRECT3DBASETEXTURE9 ret=0;
  LoadTexture(file, &ret, atlas, f, w, h, u);
  /*
  TextureFilterParams tfp;
  if(atfp>-1&&atfp< g_SamplerStates.size())
  tfp=g_SamplerStates[atfp];
  else
  tfp=TextureFilterParams();
*/
  /*
	TextureD3DGL(LPDIRECT3DBASETEXTURE9 atexture=0,
	int afilter=-1,
	LPDIRECT3DSURFACE9 *artSurfaces=0,
	int anRTSurfaces=0,
	int awidth=0, int aheight=0,
	D3DFORMAT aformat=D3DFMT_X8R8G8B8,
	DWORD ausage=0,
	D3DPOOL apool=D3DPOOL_DEFAULT,
	unsigned int aflags=0
  */
  TextureD3DGL t=TextureD3DGL(
	ret,
	atfp,
	0,//LPDIRECT3DSURFACE9 *rtSurfaces;
	0,//int nRTSurfaces;
	w, h,
	f,
	u);
	//D3DPOOL pool;
	//unsigned int flags;

  g_Textures.push_back(t);
  return g_Textures.size();
}
int CreateTexture(const ImageD3DGL &img, bool atlas, FORMAT f, unsigned int w,
				  unsigned int h, DWORD u, int atfp)
{
  LPDIRECT3DBASETEXTURE9 ret=0;
  /*
  TextureFilterParams tfp;
  if(atfp>-1&&atfp< g_SamplerStates.size())
  tfp=g_SamplerStates[atfp];
  else
  tfp=TextureFilterParams();
  */
  LoadTexture(img, &ret, atlas, f, w, h, u);
    TextureD3DGL t=TextureD3DGL(
	ret,
	atfp,
	0,//LPDIRECT3DSURFACE9 *rtSurfaces;
	0,//int nRTSurfaces;
	w, h,
	f,
	u);
	//D3DPOOL pool;
	//unsigned int flags;
   g_Textures.push_back(t);
  return g_Textures.size();

}
#endif
void SetTexture(int sampler, int texture, int state)
{
  if(sampler>-1&&sampler< g_SamplerStates.size()&&texture>-1&&texture< g_Textures.size())
  {
  V(g_pd3dDevice->SetTexture(sampler,g_Textures[texture].texture));
  if(state!=-1)
  SetSamplerState(sampler, state);
  else if(g_Textures[texture].filter>-1&&g_Textures[texture].filter<g_SamplerStates.size())
  SetSamplerState(sampler, g_Textures[texture].filter);
  else
  SetSamplerState(sampler, TextureFilterParams());
  }
}
int EraseTexture(int i)
{
	if(i>-1&&i<g_Textures.size())
		g_Textures.erase( g_Textures.begin()+i);
	return g_Textures.size();
}
