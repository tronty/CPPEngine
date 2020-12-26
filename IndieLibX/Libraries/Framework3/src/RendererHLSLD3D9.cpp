#if 0
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * **
*  |_|	   |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|	*
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _	*
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_	*
*  |_|	   |_| |_|_ _ _|_| |_|	   |_| |_|_ _ _|_|  _ _ _ _|_|	*
*  |_|	   |_|	 |_|_|_|   |_|	   |_|	 |_|_|_|   |_|_|_|_|	*
*								*
*		      http://www.humus.name			*
*								 *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.		   *
*								 *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/RendererHLSLD3D9.h>
#ifdef __Framework3D3D9_H__

#define AUTODEPTHSTENCILFORMAT D3DFMT_D16

#define R2VB_ENABLE  0x70ff0001
#define R2VB_DISABLE 0x70ff0000

#define R2VB_DMAP 0
#define R2VB_VTX0 1
#define R2VB_VTX1 2
#define R2VB_VTX2 3
#define R2VB_VTX3 4

#define R2VB_MAP_STREAM(stream, sampler) (0x71ff0000 | stream | (sampler << 4))
#define R2VB_UNMAP_STREAM(stream) (0x71ff0050 | stream)
// Helper function to stuff a FLOAT into a __DWORD__ argument
inline __DWORD__ FtoDW( FLOAT f ) { return *((__DWORD__*)&f); }

static FORMAT formats[] =
{
	FORMAT_NONE	 ,
	FORMAT_R8	 ,
	FORMAT_RG8	 ,
	FORMAT_RGB8	 ,
	FORMAT_RGBA8	 ,
	FORMAT_R16	 ,
	FORMAT_RG16	 ,
	FORMAT_RGB16	 ,
	FORMAT_RGBA16	 ,
	FORMAT_R8S	 ,
	FORMAT_RG8S	 ,
	FORMAT_RGB8S	 ,
	FORMAT_RGBA8S	 ,
	FORMAT_R16S	 ,
	FORMAT_RG16S	 ,
	FORMAT_RGB16S	 ,
	FORMAT_RGBA16S	 ,
	FORMAT_R16f	 ,
	FORMAT_RG16f	 ,
	FORMAT_RGB16f	 ,
	FORMAT_RGBA16f	 ,
	FORMAT_R32f	 ,
	FORMAT_RG32f	 ,
	FORMAT_RGB32f	 ,
	FORMAT_RGBA32f	 ,
	FORMAT_R16I	 ,
	FORMAT_RG16I	 ,
	FORMAT_RGB16I	 ,
	FORMAT_RGBA16I	 ,
	FORMAT_R32I	 ,
	FORMAT_RG32I	 ,
	FORMAT_RGB32I	 ,
	FORMAT_RGBA32I	 ,
	FORMAT_R16UI	 ,
	FORMAT_RG16UI	 ,
	FORMAT_RGB16UI	 ,
	FORMAT_RGBA16UI  ,
	FORMAT_R32UI	 ,
	FORMAT_RG32UI	 ,
	FORMAT_RGB32UI	 ,
	FORMAT_RGBA32UI  ,
	FORMAT_RGBE8	 ,
	FORMAT_RGB9E5	 ,
	FORMAT_RG11B10F  ,
	FORMAT_RGB565	 ,
	FORMAT_RGBA4	 ,
	FORMAT_RGB10A2	 ,
	FORMAT_D16	 ,
	FORMAT_D24	 ,
	FORMAT_D24S8	 ,
	FORMAT_D32F	 ,
	FORMAT_DXT1	 ,
	FORMAT_DXT3	 ,
	FORMAT_DXT5	 ,
	FORMAT_ATI1N	 ,
	FORMAT_ATI2N   ,

	FORMAT_RGB8	 ,
	FORMAT_RGB8	 ,
	FORMAT_RGBA8	 ,

}
;
static D3DFORMAT formatsd3d9[] =
{
	D3DFMT_UNKNOWN,
	D3DFMT_L8,
	D3DFMT_A8L8,
	D3DFMT_X8R8G8B8,
	D3DFMT_A8R8G8B8,
	D3DFMT_L16,
	D3DFMT_G16R16,
	D3DFMT_UNKNOWN, D3DFMT_A16B16G16R16,
	D3DFMT_UNKNOWN,
	D3DFMT_V8U8,
	D3DFMT_UNKNOWN,
	D3DFMT_Q8W8V8U8,
	D3DFMT_UNKNOWN,
	D3DFMT_V16U16,
	D3DFMT_UNKNOWN,
	D3DFMT_Q16W16V16U16,
	D3DFMT_R16F,
	D3DFMT_G16R16F,
	D3DFMT_UNKNOWN, D3DFMT_A16B16G16R16F,
	D3DFMT_R32F,
	D3DFMT_G32R32F,
	D3DFMT_UNKNOWN, D3DFMT_A32B32G32R32F,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN, D3DFMT_UNKNOWN, D3DFMT_UNKNOWN, D3DFMT_R5G6B5,
	D3DFMT_A4R4G4B4,
	D3DFMT_A2B10G10R10,
	D3DFMT_D16,
	D3DFMT_D24X8,
	D3DFMT_D24S8,
	D3DFMT_D32F_LOCKABLE,
	D3DFMT_DXT1,
	D3DFMT_DXT3,
	D3DFMT_DXT5,
	(D3DFORMAT) '1ITA', (D3DFORMAT) '2ITA',

	D3DFMT_R8G8B8,
//	D3DFMT_B8G8R8,
	D3DFMT_X8B8G8R8,
	D3DFMT_A8B8G8R8,

}
;

void RendererHLSLD3D9::GetInfo()
{
#if 0
	// Adapter
	D3DADAPTER_IDENTIFIER9 *mAdapter = new D3DADAPTER_IDENTIFIER9;
	d3d->GetAdapterIdentifier (0, 0, mAdapter);

	// ----- d3d Version -----

	SetVersion("DXSDK June 2010");

	// ----- Vendor -----	
	
	unsigned int mVId = mAdapter->VendorId;

	switch (mVId)
	{
		case VENDORID_NVIDIA:
		{
			SetVendor("Nvidia");
			break;
		}
		case VENDORID_ATI:
		{
			SetVendor("ATI Technologies Inc");
			break;
		}
		default:
		{
			SetVendor("?");
			break;
		}
	}	

	// ----- Renderer -----

	// Chip
	SetRenderer(mAdapter->Description);

	// Free D3DADAPTER_IDENTIFIER9 adapter
	delete mAdapter;

	// Caps
	D3DCAPS9 mD3dcap;
	d3d->GetDeviceCaps (&mD3dcap);

	// Antialiasing D3DRS_ANTIALIASEDLINEENABLE
	if (mD3dcap.LineCaps & D3DLINECAPS_ANTIALIAS) 
		//mInfo.mAntialiasing = 1;
		SetAntialiasing(1);
	else
		SetAntialiasing(0);

	// Max texture size
	SetMaxTextureSize(mD3dcap.MaxTextureWidth);

	// Vertex Shader version
	SetVertexShaderVersion(mD3dcap.VertexShaderVersion);

	// Pixel Shader version
	SetPixelShaderVersion(mD3dcap.PixelShaderVersion);
#endif
}

void RendererHLSLD3D9::setup2DMode(const float left, const float right, const float top, const float bottom){
	scaleBias2D.x = 2.0f / (right - left);
	scaleBias2D.y = 2.0f / (top - bottom);
	scaleBias2D.z = -1.0f;
	scaleBias2D.w =  1.0f;
}
float RendererHLSLD3D9::getTextWidth(const FontID font, const char *str, int length) const {
	if (font < 0) return 0;
    if (length < 0) length = (int) stx_strlen(str);

	Character *chars = fonts[font].chars;

	float len = 0;
	for (int i = 0; i < length; i++){
		len += chars[(unsigned char) str[i]].ratio;
	}

	return len;
}

bool RendererHLSLD3D9::createFrameBufferSurfaces()
{
	HRESULT hr=dev->GetRenderTarget(0, &fbColor);
	if (hr != D3D_OK) return false;
	V(dev->GetDepthStencilSurface(&fbDepth));
	return true;
}
bool RendererHLSLD3D9::releaseFrameBufferSurfaces()
{
	if (fbColor) fbColor->Release();
	if (fbDepth) fbDepth->Release();
	return true;
}
void RendererHLSLD3D9::setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState

		, const int vtstream
		, const VertexBufferID vtbuffer

								  )
{
	if(texture==-1) DBG_HALT;
	int unit = getSamplerUnit(selectedShader, textureName);
	if (unit >= 0)
	{
		selectedTextures[unit] = texture;
		selectedSamplerStates[unit] = samplerState;
if(textures[texture].vtexture)
{
		selectedVTStreams[unit] = vtstream;
		selectedVTBuffers[unit] = vtbuffer;
}
	}
}
void RendererHLSLD3D9::setTextureSlice(const char *textureName, const TextureID texture, const int slice)
{
}
LPDIRECT3DBASETEXTURE9 RendererHLSLD3D9::getD3DTexture(const TextureID texture) const
{
	return (textures[texture]).texture;
}
RendererHLSLD3D9::RendererHLSLD3D9(
			bool fullScreen,
int argc, char *argv[]
) : mOk(false), RendererHLSLCg(argc, argv),m_pD3D(0),m_pd3dDevice(0)
{
	HRESULT hr = E_FAIL;

    	width=STX_Service::GetWindowInstance()->Getwidth();
    	height=STX_Service::GetWindowInstance()->Getheight();

	SHADING_LANGUAGE_VERSION_MAJOR=3;
	SHADING_LANGUAGE_VERSION_MINOR=3;

	LOG_PRINT("ST:RendererHLSLD3D9::RendererHLSLD3D9\n");
	//std::string str=miscParams["window"]

	LPDIRECT3DDEVICE9 d3ddev=0;
	D3DCAPS9 d3dcaps;
	if((m_pD3D!=0)&&(m_pd3dDevice!=0)) return;

/*
			opt = miscParams->find("externalWindowHandle");
			if(opt != miscParams->end())
				STX_Service::GetWindowInstance()->GetHWND() = (HWND)StringConverter::parseUnsignedInt(opt->second);
*/

	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
//
	hr=m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps);
	if(FAILED(hr)) return;
	stx_memcpy(&m_caps,&d3dcaps,sizeof(D3DCAPS9));
	D3DDISPLAYMODE d3ddm;
	hr=m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );
	if(FAILED(hr)) return;
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed		     = TRUE;
	d3dpp.SwapEffect	     = D3DSWAPEFFECT_COPY;
	d3dpp.BackBufferFormat	     = d3ddm.Format;
	m_fmt= d3ddm.Format;
	m_fmt= d3ddm.Format;
	d3dpp.BackBufferWidth	 = width;
	d3dpp.BackBufferHeight	 = height;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = AUTODEPTHSTENCILFORMAT;
	d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.BackBufferCount=1;
	__DWORD__ behaviorFlags=0;
	if( m_caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
	behaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
	behaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	m_d3dpp=d3dpp;
	hr=m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, STX_Service::GetWindowInstance()->GetHWND(),
	behaviorFlags,
	&d3dpp, &d3ddev );
	if(FAILED(hr)) return;

	ZERO		    = D3DBLEND_ZERO;
	ONE		    = D3DBLEND_ONE;
	SRC_COLOR	    = D3DBLEND_SRCCOLOR;
	ONE_MINUS_SRC_COLOR = D3DBLEND_INVSRCCOLOR;
	DST_COLOR	    = D3DBLEND_DESTCOLOR;
	ONE_MINUS_DST_COLOR = D3DBLEND_INVDESTCOLOR;
	SRC_ALPHA	    = D3DBLEND_SRCALPHA;
	ONE_MINUS_SRC_ALPHA = D3DBLEND_INVSRCALPHA;
	DST_ALPHA	    = D3DBLEND_DESTALPHA;
	ONE_MINUS_DST_ALPHA = D3DBLEND_INVDESTALPHA;
	SRC_ALPHA_SATURATE  = D3DBLEND_SRCALPHASAT;
	BM_ADD		    = D3DBLENDOP_ADD;
	BM_SUBTRACT	    = D3DBLENDOP_SUBTRACT;
	BM_REVERSE_SUBTRACT = D3DBLENDOP_REVSUBTRACT;
	BM_MIN		    = D3DBLENDOP_MIN;
	BM_MAX		    = D3DBLENDOP_MAX;
	NEVER	 = D3DCMP_NEVER;
	LESS	 = D3DCMP_LESS;
	EQUAL	 = D3DCMP_EQUAL;
	LEQUAL	 = D3DCMP_LESSEQUAL;
	GREATER  = D3DCMP_GREATER;
	NOTEQUAL = D3DCMP_NOTEQUAL;
	GEQUAL	 = D3DCMP_GREATEREQUAL;
	ALWAYS	 = D3DCMP_ALWAYS;
	KEEP	 = D3DSTENCILOP_KEEP;
	SET_ZERO = D3DSTENCILOP_ZERO;
	REPLACE  = D3DSTENCILOP_REPLACE;
	INVERT	 = D3DSTENCILOP_INVERT;
	INCR	 = D3DSTENCILOP_INCR;
	DECR	 = D3DSTENCILOP_DECR;
	INCR_SAT = D3DSTENCILOP_INCRSAT;
	DECR_SAT = D3DSTENCILOP_DECRSAT;
	CULL_NONE  = D3DCULL_NONE;
	CULL_BACK  = D3DCULL_CCW;
	CULL_FRONT = D3DCULL_CW;
	SOLID = D3DFILL_SOLID;
	WIREFRAME = D3DFILL_WIREFRAME;
	dev = d3ddev;
	createFrameBufferSurfaces();
	if (d3dcaps.PixelShaderVersion >= D3DPS_VERSION(1,0))
	{
		nImageUnits = 4;
		if (d3dcaps.PixelShaderVersion >= D3DPS_VERSION(1,4)) nImageUnits = 6;
		if (d3dcaps.PixelShaderVersion >= D3DPS_VERSION(2,0)) nImageUnits = 16;
	}
	else
	{
		nImageUnits = d3dcaps.MaxSimultaneousTextures;
	}
	maxAnisotropic = d3dcaps.MaxAnisotropy;
	nMRTs = d3dcaps.NumSimultaneousRTs;
	if (nMRTs > MAX_MRTS) nMRTs = MAX_MRTS;
	plainShader = SHADER_NONE;
	plainVF = VF_NONE;
	eventQuery = 0;
	setD3Ddefaults();
	resetToDefaults();
	if (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, (D3DFORMAT) MAKEFOURCC('R','2','V','B')) == D3D_OK)
	{
		bsupportsR2VB = true;
		if (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F) == D3D_OK)
		{
			if (caps.DeclTypes & D3DDTCAPS_FLOAT16_4) bsupportsHalf = true;
		}
	}
	else
	{
		bsupportsR2VB = false;
		if (caps.VertexShaderVersion >= D3DVS_VERSION(3,0))
		{
			if (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DUSAGE_QUERY_VERTEXTEXTURE, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F) == D3D_OK)
			{
				if (caps.DeclTypes & D3DDTCAPS_FLOAT16_4) bsupportsHalf = true;
			}
		}
	}
	overrideR2VB=true;
	overrideHalf=true;

			//defaultFont = addFont(FONT_PATH"Future.dds", FONT_PATH"Future.font", linearClamp);
LOG_PRINT("RendererHLSLD3D9\n");
	

	if(!FAILED(hr)) mOk=true;
}
RendererHLSLD3D9::~RendererHLSLD3D9()
{
	if (eventQuery) eventQuery->Release();
	releaseFrameBufferSurfaces();
	for (unsigned int i = 0; i < shaders.getCount(); i++)
	{
		if (((ShaderHLSLNOFX3D3D9)shaders[i]).vertexShader) ((ShaderHLSLNOFX3D3D9)shaders[i]).vertexShader->Release();
		if (((ShaderHLSLNOFX3D3D9)shaders[i]).pixelShader) ((ShaderHLSLNOFX3D3D9)shaders[i]).pixelShader->Release();
		if (((ShaderHLSLNOFX3D3D9)shaders[i]).vsConstants) ((ShaderHLSLNOFX3D3D9)shaders[i]).vsConstants->Release();
		if (((ShaderHLSLNOFX3D3D9)shaders[i]).psConstants) ((ShaderHLSLNOFX3D3D9)shaders[i]).psConstants->Release();
		for (unsigned int j = 0; j < ((ShaderHLSLNOFX3D3D9)shaders[i]).nSamplers; j++)
		{
			(shaders[i]).samplers[j].name.clear();
		}
		for (unsigned int j = 0; j < ((ShaderHLSLNOFX3D3D9)shaders[i]).nConstants; j++)
		{
			(shaders[i]).constants[j].name.clear();
		}
		((ShaderHLSLNOFX3D3D9)shaders[i]).samplers.clear();
		((ShaderHLSLNOFX3D3D9)shaders[i]).constants.clear();
	}
	for (unsigned int i = 0; i < vertexFormats.getCount(); i++)
	{
		if ((vertexFormats[i]).vertexDecl) (vertexFormats[i]).vertexDecl->Release();
	}
	for (unsigned int i = 0; i < vertexBuffers.getCount(); i++)
	{
		if ((vertexBuffers[i]).vertexBuffer) (vertexBuffers[i]).vertexBuffer->Release();
	}
	for (unsigned int i = 0; i < indexBuffers.getCount(); i++)
	{
		if ((indexBuffers[i]).indexBuffer) (indexBuffers[i]).indexBuffer->Release();
	}
	for (unsigned int i = 0; i < textures.getCount(); i++)
	{
		removeTexture(i);
	}
			shaders.clear();
		textures.clear();
		vertexBuffers.clear();
		indexBuffers.clear();
		vertexFormats.clear();
		samplerStates.clear();
		blendStates.clear();
		alphaStates.clear();
		depthStates.clear();
		rasterizerStates.clear();
}
void RendererHLSLD3D9::resetToDefaults()
{
	reset0ToDefaults();
	for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++)
	{
		currentTextures[i] = TEXTURE_NONE;
	}
	for (unsigned int i = 0; i < MAX_SAMPLERSTATE; i++)
	{
		currentSamplerStates[i] = SS_NONE;
	}
	currentSrcFactorRGB = currentSrcFactorAlpha = ONE;
	currentDstFactorRGB = currentDstFactorAlpha = ZERO;
	currentBlendModeRGB = currentBlendModeAlpha = BM_ADD;
	currentMask = ALL;
	currentBlendEnable = false;
	currentDepthFunc = LESS;
	currentDepthTestEnable = true;
	currentDepthWriteEnable = true;
	currentCullMode = CULL_NONE;
	currentFillMode = SOLID;
	currentMultiSampleEnable = true;
	currentScissorEnable = false;
	currentDepthRT = FB_DEPTH;
	stx_memset(vsRegs, 0, sizeof(vsRegs));
	stx_memset(psRegs, 0, sizeof(psRegs));
	minVSDirty = 256;
	maxVSDirty = -1;
	minPSDirty = 224;
	maxPSDirty = -1;
}
void RendererHLSLD3D9::resetVrtl(const unsigned int flags)
{
	reset0(flags);
	if (flags & RESET_TEX)
	{
		for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++)
		{
			selectedTextures[i] = TEXTURE_NONE;
		}
	}
	if (flags & RESET_SS)
	{
		for (unsigned int i = 0; i < MAX_SAMPLERSTATE; i++)
		{
			selectedSamplerStates[i] = SS_NONE;
		}
	}
}
void RendererHLSLD3D9::setD3Ddefaults()
{
	V(dev->SetRenderState(D3DRS_LIGHTING, FALSE));
	V(dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
	V(dev->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE));

	// Alpha color
	//V(dev->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (0, 0, 0, 255)));

for (unsigned int i = 0; i < nImageUnits; i++)
{
#if 1
		//OPAQUE:
			// Alphablending and alpha test = OFF
	//V(dev->SetRenderState (D3DRS_ALPHABLENDENABLE,	FALSE));
	//V(dev->SetRenderState (D3DRS_ALPHATESTENABLE,	FALSE));
			// Initializes the tinting and alpha values of previous iterations
	V(dev->SetTextureStageState (i, D3DTSS_COLOROP,	D3DTOP_MODULATE));
	V(dev->SetTextureStageState (i, D3DTSS_COLORARG1,  D3DTA_TEXTURE));
	V(dev->SetTextureStageState (i, D3DTSS_COLORARG2,  D3DTA_CURRENT));
	V(dev->SetTextureStageState (i, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1));
	V(dev->SetTextureStageState (i, D3DTSS_ALPHAARG1, D3DTA_TEXTURE));
	V(dev->SetTextureStageState (i, D3DTSS_ALPHAARG2, D3DTA_CURRENT));
//elif 0
			// Alpha
				// Alpha color
	//V(dev->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (pR, pG, pB, pA)));
				// Alpha-blending = ON
	//V(dev->SetRenderState (D3DRS_ALPHABLENDENABLE, TRUE));
				// Set source alpha and destination alpha
	//V(dev->SetRenderState (D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA));
	//V(dev->SetRenderState (D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));
				// Alpha
	V(dev->SetTextureStageState (i, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1));
	V(dev->SetTextureStageState (i, D3DTSS_ALPHAARG1,  D3DTA_TFACTOR));
#endif




#if 0
	// Alpha
	V(dev->SetTextureStageState (i, D3DTSS_ALPHAARG1,  D3DTA_TFACTOR));

#if 1
	//V(dev->SetRenderState(D3DRS_ZENABLE, FALSE));
	V(dev->SetTextureStageState(i, D3DTSS_COLOROP,   D3DTOP_SELECTARG1));//D3DTOP_MODULATE));
	V(dev->SetTextureStageState(i, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1));//D3DTOP_MODULATE));
	V(dev->SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TEXTURE));
#if 1
	V(dev->SetTextureStageState(i, D3DTSS_COLORARG2, D3DTA_DIFFUSE));
#elif 0
	V(dev->SetTextureStageState(i, D3DTSS_COLORARG2, D3DTA_CURRENT));
#elif 0
	V(dev->SetTextureStageState(i, D3DTSS_COLORARG2, D3DTA_TFACTOR));
#endif
#endif
#endif

	if (maxAnisotropic > 1)
	{
			V(dev->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, maxAnisotropic));
	}
}
}

const D3DPRIMITIVETYPE d3d9Prim[] = {
	D3DPT_TRIANGLELIST,
	D3DPT_TRIANGLEFAN,
	D3DPT_TRIANGLESTRIP,
	D3DPT_UNDEFINED, // Quads not supported
	D3DPT_LINELIST,
	D3DPT_LINESTRIP,
	D3DPT_UNDEFINED, // Line loops not supported
	D3DPT_POINTLIST

drawQuadVrtl(const float x0, const float y0, const float x1, const float y1, const float s0, const float t0, const float s1, const float t1, const D3DXFROMWINEVECTOR4 &color)

};

unsigned int RendererHLSLD3D9::DrawPrimitive(Primitives PrimitiveType,UINT StartVertex,UINT PrimitiveCount, const unsigned int flags)
{
		//if(!((currentVertexBuffers[0]>-1)))
		if(!((selectedVertexBuffers[0]>-1)))
			return;
        apply();
	if(D3DPT_UNDEFINED!=d3d9Prim[PrimitiveType])
		V(m_pd3dDevice->DrawPrimitive(d3d9Prim[PrimitiveType],StartVertex,PrimitiveCount));
	reset(flags);
	nDrawCalls++;
}
unsigned int RendererHLSLD3D9::DrawIndexedPrimitive(Primitives PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, const unsigned int flags)
{
//if(!((currentVertexBuffers[0]>-1)&&(currentIndexBuffer>-1)))
if(!((selectedVertexBuffers[0]>-1)&&(selectedIndexBuffer>-1)))
	return;
	apply();
	if(D3DPT_UNDEFINED!=d3d9Prim[PrimitiveType])
		V(m_pd3dDevice->DrawIndexedPrimitive(d3d9Prim[PrimitiveType],BaseVertexIndex,MinVertexIndex,NumVertices,startIndex, primCount));
	reset(flags);
	nDrawCalls++;
}
void RendererHLSLD3D9::BeginSceneVrtl(bool aB)
{
	V(m_pd3dDevice->BeginScene());
}
void RendererHLSLD3D9::EndSceneVrtl(bool aB)
{
	OnEndScene();
	V(m_pd3dDevice->EndScene());
}
void RendererHLSLD3D9::Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors)
{
	__DWORD__ f=0;
	if(color)
	{
		f|=D3DCLEAR_TARGET;
	}
	if(depth)
	{
		f|=D3DCLEAR_ZBUFFER;
	}
	if(f)
	{
		V(m_pd3dDevice->Clear(0, 0, f,
		D3DXFROMWINECOLOR(colors.x, colors.y, colors.z, colors.w), 1.0f, 0));
	}
}
void RendererHLSLD3D9::PresentVrtl(bool aB)
{
		bool b=true;//???((aB&&(m_pInput!=0)) || ((!aB)&&(m_pInput==0)));
if(b)
{
	IRenderer::OnPresent();
	#if !defined(WIN_PHONE_APP)
	V(m_pd3dDevice->Present(0, 0, 0, 0));
	#endif
}
}
void RendererHLSLD3D9::captureScreenshot(Image3& img)
{
	HRESULT hr = S_OK;
	if( !m_pd3dDevice )
	return ;
	IDirect3DSurface9* pBackBuffer = 0;
	V_RETURN( m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ) );
	//hr = D3DXSaveSurfaceToFileA( szFileName, D3DXIFF_JPG, pBackBuffer, 0, 0 );
	D3DSURFACE_DESC Desc;
	pBackBuffer->GetDesc(&Desc);
	D3DLOCKED_RECT lockedRect;
	RECT rect;
	rect.top=0;
	rect.left=0;
	rect.bottom=Desc.Height;
	rect.right=Desc.Width;

	pBackBuffer->LockRect(&lockedRect,&rect,0);

BYTE* d=img.create(FORMAT_RGB8,Desc.Width,Desc.Height,1,1);
stx_memcpy(d,lockedRect.pBits,3*Desc.Width*Desc.Height);
/*
		int l=0;
 LPCSTR fn=szFileName;
        l=int(stx_strrchr(fn,'.')-fn);

		std::string fns(fn);
		if(l)fns.resize(l);
					char bufpng[256];
                    stx_snprintf(bufpng,256,"%s.png",fns.c_str());
					img.saveImageLibImage(bufpng,false);
	pBackBuffer->UnlockRect();

*/
	SAFE_RELEASE( pBackBuffer );
}

void RendererHLSLD3D9::SaveScreenShotVrtl(const char* fn)
{
		if(fn==0)return;

		{
			Image3 img;
			char buf[256];
			char gifbuf[256];
			if(fn!=0)
			{
                stx_snprintf(buf,256,"../%s.jpg",fn);
                stx_snprintf(gifbuf,256,"../%s.gif",fn);

			}
			else
			{
                stx_snprintf(buf,256,"../%s","ScreenShot.jpg");
                stx_snprintf(gifbuf,256,"../%s","ScreenShot.gif");
			}

			captureScreenshot(img);
			img.saveImageLibImage(buf,false);
			RendererHLSLCg::OpenILToGIF(img,gifbuf);
		}
}

void RendererHLSLD3D9::changeShader(const ShaderID shader)
{
	if (shader != currentShader)
	{
		if (shader == SHADER_NONE)
		{
			V(dev->SetVertexShader(0));
			V(dev->SetPixelShader(0));
		}
		else
		{
			V(dev->SetVertexShader(((ShaderHLSLNOFX3D3D9)shaders[shader]).vertexShader));
			V(dev->SetPixelShader(((ShaderHLSLNOFX3D3D9)shaders[shader]).pixelShader));
		}
		currentShader = shader;
	}
}
void RendererHLSLD3D9::changeVertexFormat(const VertexFormatID vertexFormat)
{
	if (vertexFormat != currentVertexFormat)
	{
		if (vertexFormat != VF_NONE)
		{
			V(dev->SetVertexDeclaration((vertexFormats[vertexFormat]).vertexDecl));
			if (currentVertexFormat != VF_NONE)
			{
				for (int i = 0; i < MAX_VERTEXSTREAM; i++)
				{
					if (
					(vertexFormats[vertexFormat]).vertexSize[i]
					!= (vertexFormats[currentVertexFormat]).vertexSize[i])
					{
						currentVertexBuffers[i] = VB_INVALID;
					}
				}
			}
		}
		currentVertexFormat = vertexFormat;
	}
}
void RendererHLSLD3D9::changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, const intptr offset)
{
	if(currentVertexFormat==-1) return;//???
	//if (vertexBuffer != currentVertexBuffers[stream] || offset != currentOffsets[stream])
	{
		if (vertexBuffer == VB_NONE)
		{
			V(dev->SetStreamSource(stream, 0, 0, 0));
		}
		else
		{
			V(dev->SetStreamSource(stream, (vertexBuffers[vertexBuffer]).vertexBuffer, (UINT) offset, (vertexFormats[currentVertexFormat]).vertexSize[stream]));
		}
		currentVertexBuffers[stream] = vertexBuffer;
		currentOffsets[stream] = offset;
	}
}
void RendererHLSLD3D9::changeIndexBuffer(const IndexBufferID indexBuffer)
{
	////if (indexBuffer != currentIndexBuffer)
	{
		if (indexBuffer == IB_NONE)
		{
			V(dev->SetIndices(0));
		}
		else
		{
			V(dev->SetIndices((indexBuffers[indexBuffer]).indexBuffer));
		}
		currentIndexBuffer = indexBuffer;
	}
}
void RendererHLSLD3D9::changeDepthStateVrtl(const DepthStateID depthState, const unsigned int stencilRef){
	if (depthState != currentDepthState){
		if (depthState == DS_NONE){
			if (!currentDepthTestEnable){
				dev->SetRenderState(D3DRS_ZENABLE, TRUE);
				currentDepthTestEnable = true;
			}

			if (!currentDepthWriteEnable){
				dev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
				currentDepthWriteEnable = true;
			}

			if (currentDepthFunc != LESS){
				dev->SetRenderState(D3DRS_ZFUNC, currentDepthFunc = LESS);
			}
		} else {
			if (depthStates[depthState].depthTest){
				if (!currentDepthTestEnable){
					dev->SetRenderState(D3DRS_ZENABLE, TRUE);
					currentDepthTestEnable = true;
				}
				if (depthStates[depthState].depthWrite != currentDepthWriteEnable){
					dev->SetRenderState(D3DRS_ZWRITEENABLE, (currentDepthWriteEnable = depthStates[depthState].depthWrite)? TRUE : FALSE);
				}
				if (depthStates[depthState].depthFunc != currentDepthFunc){
					dev->SetRenderState(D3DRS_ZFUNC, currentDepthFunc = depthStates[depthState].depthFunc);
				}
			} else {
				if (currentDepthTestEnable){
					dev->SetRenderState(D3DRS_ZENABLE, FALSE);
					currentDepthTestEnable = false;
				}
			}
		}

		currentDepthState = depthState;
	}

	if (stencilRef != currentStencilRef){
		dev->SetRenderState(D3DRS_STENCILREF, stencilRef);
		currentStencilRef = stencilRef;
	}
}
void RendererHLSLD3D9::changeRasterizerState(const RasterizerStateID rasterizerState)
{
	if (rasterizerState != currentRasterizerState)
	{
		RasterizerStateD3D9 state;
		if (rasterizerState == RS_NONE)
		{
			state.cullMode = CULL_NONE;
			state.fillMode = SOLID;
			state.multiSample = true;
			state.scissor = false;
		}
		else
		{
			state = (rasterizerStates[rasterizerState]);
		}
		if (state.cullMode != currentCullMode)
		{
			V(dev->SetRenderState(D3DRS_CULLMODE, currentCullMode = state.cullMode));
		}
		if (state.fillMode != currentFillMode)
		{
			V(dev->SetRenderState(D3DRS_FILLMODE, currentFillMode = state.fillMode));
		}
		if (state.multiSample != currentMultiSampleEnable)
		{
			V(dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, currentMultiSampleEnable = state.multiSample));
		}
		if (state.scissor != currentScissorEnable)
		{
			V(dev->SetRenderState(D3DRS_SCISSORTESTENABLE, currentScissorEnable = state.scissor));
		}
		currentRasterizerState = rasterizerState;
	}
}
void RendererHLSLD3D9::setShaderConstantRaw(const char *name, const void *data, const int size)
{
	if(selectedShader==-1)return;
	int minConstant = 0;
	int maxConstant = shaders[selectedShader].nConstants - 1;
	std::vector<ConstantD3D9 > constants = shaders[selectedShader].constants;
	while (minConstant <= maxConstant)
	{
		int currConstant = (minConstant + maxConstant) >> 1;
		if(!constants[currConstant].name.c_str())return;//???
		int res = strcmp(name, constants[currConstant].name.c_str());
		if (res == 0)
		{
			ConstantD3D9 c = constants[currConstant];
			if (c.vsReg >= 0)
			{
				if (stx_memcmp(vsRegs + c.vsReg, data, size))
				{
					stx_memcpy(vsRegs + c.vsReg, data, size);
					int r0 = c.vsReg;
					int r1 = c.vsReg + ((size + 15) >> 4);
					if (r0 < minVSDirty) minVSDirty = r0;
					if (r1 > maxVSDirty) maxVSDirty = r1;
				}
			}
			if (c.psReg >= 0)
			{
				if (stx_memcmp(psRegs + c.psReg, data, size))
				{
					stx_memcpy(psRegs + c.psReg, data, size);
					int r0 = c.psReg;
					int r1 = c.psReg + ((size + 15) >> 4);
					if (r0 < minPSDirty) minPSDirty = r0;
					if (r1 > maxPSDirty) maxPSDirty = r1;
				}
			}
			return;
		}
		else if (res > 0)
		{
			minConstant = currConstant + 1;
		}
		else
		{
			maxConstant = currConstant - 1;
		}
	}
	#ifdef _DEBUG
	char str[256];
    stx_snprintf(str,256, "Invalid constant \"%s\"\n", name);
	LOG_PRINT("%s", str);
	#endif
}
#if 0
bool constantCompD3D9std(const ConstantD3D9 & s0, const ConstantD3D9 &s1)
{
	if(!(s0.name&&s1.name)) return false;
	bool r=strcmp(s0.name, s1.name)?false:true;
	return r;
}
bool samplerCompD3D9std(const SamplerD3D9 & s0, const SamplerD3D9 &s1)
{
	if(!(s0.name&&s1.name)) return false;
	bool r=strcmp(s0.name, s1.name)?false:true;
	return r;
}
#else
bool constantCompD3D9std(const ConstantD3D9 & s0, const ConstantD3D9 &s1)
{
       if(!(s0.name.c_str()&& s1.name.c_str()))return false;
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
}
bool samplerCompD3D9std(const SamplerD3D9 & s0, const SamplerD3D9 &s1)
{
       if(!(s0.name.c_str()&& s1.name.c_str()))return false;
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
}
#endif

ShaderID RendererHLSLD3D9::addShaderHLSLVrtl(	const std::string& aFileName, 
						std::vector<std::string>& avMain, 
						std::vector<ShaderType>& avType)
{
	return SHADER_NONE;
}

ShaderID RendererHLSLD3D9::addHLSLShaderVrtl( 	std::vector<std::string>& avText,
                                            	std::vector<std::string>& avMain,
                                            	std::vector<ShaderType>& avType,
                                            	const unsigned int flags)
		{
    if(avMain.size()!=avType.size()) return SHADER_NONE;
			const char *vsText0=0;
			const char *fsText0=0;
		const char *vsName=0;
		const char *psName=0;
    for(unsigned int i=0;i<avType.size();i++)
    {
        switch(i)
        {
            case ePixelShader:
                fsText0=avText[i].c_str();
                psName=avMain[i].c_str();
                break;
            case eVertexShader:
                vsText0=avText[i].c_str();
                vsName=avMain[i].c_str();
                break;
            case eGeometryShader:
                break;
	        case eHullShader:
                break;
	        case eDomainShader:
                break;
	        case eComputeShader:
                break;
        };
    }
    const char* header=0;
		if (vsText0 == 0 && fsText0 == 0) return SHADER_NONE;

	std::string vsText;
	std::string fsText;
	const char * def="#define ROW_MAJOR row_major\n#define MVPSEMANTIC\n #define WSIGN +\n#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) tex2Dlod\(TEX\, float4\(TEXCOORD\.x\, TEXCOORD\.y\, 0.0\, 0.0\)\)\n#define SAMPLER2D sampler2D\n";
	if (def) vsText.append(def);
	if (def) fsText.append(def);
	if (vsText0) vsText.append(vsText0);
	if (fsText0) fsText.append(fsText0);
	//LOG_PRINT("\nvs:\n%s\n", vsText.c_str());
	//LOG_PRINT("\nfs:\n%s\n", fsText.c_str());

	const char * mainvs=vsName;
	const char * mainps=psName;
	LPD3DXBUFFER shaderBuf = 0;
	LPD3DXBUFFER errorsBuf = 0;
	ShaderHLSLNOFX3D3D9 shader;
	shader.vertexShader = 0;
	shader.pixelShader  = 0;
	shader.vsConstants  = 0;
	shader.psConstants  = 0;
	char buf[65536];
							
	#if 0
	shader.filename=fileName;
	shader.vsTxt=-1;
	shader.fsTxt=-1;
	shader.vsMain=vsName;
	shader.fsMain=psName;
	shader.id=shaders.size();
	#endif

	if (vsText != "")
	{
        stx_snprintf(buf,65536,"\n%s\n",vsText.c_str());
		OutputDebugStringA(buf);
		const char *profile = D3DXGetVertexShaderProfile(dev);
		if (D3DXCompileShader(vsText.c_str(), vsText.length(), 0, 0, mainvs, profile, D3DXSHADER_DEBUG | D3DXSHADER_PACKMATRIX_ROWMAJOR, &shaderBuf, &errorsBuf, &shader.vsConstants) == D3D_OK)
		{
			V(dev->CreateVertexShader((__DWORD__ *) shaderBuf->GetBufferPointer(), &shader.vertexShader));
			#ifdef DEBUG
			LPD3DXBUFFER disasm;
			D3DXDisassembleShader((__DWORD__ *) shaderBuf->GetBufferPointer(), FALSE, 0, &disasm);
			char *str = (char *) disasm->GetBufferPointer();
			while (*str)
			{
				while (*str == '\n' || *str == '\r') *str++;
				char *endStr = str;
				while (*endStr && *endStr != '\n' && *endStr != '\r') *endStr++;
				if (*str != '#')
				{
					*endStr = '\0';
					LOG_PRINT("%s", str);
				}
				str = endStr + 1;
			}
			if (disasm != 0) disasm->Release();
			#endif
		}
		else
		{
			ErrorMsg((const char *) errorsBuf->GetBufferPointer());
		}
		if (shaderBuf != 0) shaderBuf->Release();
		if (errorsBuf != 0) errorsBuf->Release();
		if (shader.vertexShader == 0) return SHADER_NONE;
	}
	if (fsText != "")
	{
        stx_snprintf(buf,65536,"\n%s\n",fsText.c_str());
		OutputDebugStringA(buf);
		const char *profile = D3DXGetPixelShaderProfile(dev);
		if (D3DXCompileShader(fsText.c_str(), fsText.length(), 0, 0, mainps, profile, D3DXSHADER_DEBUG | D3DXSHADER_PACKMATRIX_ROWMAJOR, &shaderBuf, &errorsBuf, &shader.psConstants) == D3D_OK)
		{
			V(dev->CreatePixelShader((__DWORD__ *) shaderBuf->GetBufferPointer(), &shader.pixelShader));
			#ifdef DEBUG
			LPD3DXBUFFER disasm;
			D3DXDisassembleShader((__DWORD__ *) shaderBuf->GetBufferPointer(), FALSE, 0, &disasm);
			char *str = (char *) disasm->GetBufferPointer();
			while (*str)
			{
				while (*str == '\n' || *str == '\r') *str++;
				char *endStr = str;
				while (*endStr && *endStr != '\n' && *endStr != '\r') *endStr++;
				if (*str != '#')
				{
					*endStr = '\0';
					LOG_PRINT("%s", str);
				}
				str = endStr + 1;
			}
			if (disasm != 0) disasm->Release();
			#endif
		}
		else
		{
			ErrorMsg((const char *) errorsBuf->GetBufferPointer());
		}
		if (shaderBuf != 0) shaderBuf->Release();
		if (errorsBuf != 0) errorsBuf->Release();
		if (shader.pixelShader == 0) return SHADER_NONE;
	}
	D3DXCONSTANTTABLE_DESC vsDesc, psDesc;
	shader.vsConstants->GetDesc(&vsDesc);
	shader.psConstants->GetDesc(&psDesc);
	unsigned int count = vsDesc.Constants + psDesc.Constants;
	std::vector<SamplerD3D9  > samplers;
	std::vector<ConstantD3D9 > constants;
	samplers.resize(count);
	constants.resize(count);
	unsigned int nSamplers	= 0;
	unsigned int nConstants = 0;
	D3DXCONSTANT_DESC cDesc;
	for (unsigned int i = 0; i < vsDesc.Constants; i++)
	{
		UINT count = 1;
		shader.vsConstants->GetConstantDesc(shader.vsConstants->GetConstant(0, i), &cDesc, &count);
        size_t length = stx_strlen(cDesc.Name);
		if (cDesc.Type >= D3DXPT_SAMPLER && cDesc.Type <= D3DXPT_SAMPLERCUBE)
		{
		   // TODO: Vertex samplers not yet supported ...
		   LOG_PRINT("VertexSampler=%s\n",cDesc.Name);

			//(samplers[nSamplers]);
			(samplers[nSamplers]).name = std::string(cDesc.Name);
			(samplers[nSamplers]).index = cDesc.RegisterIndex;
			samplers[nSamplers].vtexture=true;

			nSamplers++;

			//merge




		}
		else
		{
			//(constants[nConstants]);
			(constants[nConstants]).name = std::string(cDesc.Name);
			(constants[nConstants]).vsReg = cDesc.RegisterIndex;
			(constants[nConstants]).psReg = -1;
			nConstants++;
		}
	}

	unsigned int nVSConsts = nConstants;
	for (unsigned int i = 0; i < psDesc.Constants; i++)
	{
		UINT count = 1;
		shader.psConstants->GetConstantDesc(shader.psConstants->GetConstant(0, i), &cDesc, &count);
        size_t length = stx_strlen(cDesc.Name);
		if (cDesc.Type >= D3DXPT_SAMPLER && cDesc.Type <= D3DXPT_SAMPLERCUBE)
		{
int merge = -1;
			for (unsigned int i = 0; i < nVSConsts; i++)
			{
				if (strcmp(constants[i].name.c_str(), cDesc.Name) == 0)
				{
					merge = i;
					break;
				}
			}
			if (merge < 0)
			{
			//(samplers[nSamplers]);
			(samplers[nSamplers]).name = std::string(cDesc.Name);
			(samplers[nSamplers]).index = cDesc.RegisterIndex;
			samplers[nSamplers].vtexture=false;
			}
			else
			{
			samplers[merge].vtexture=false;//???
			}
			nSamplers++;
		}
		else
		{
			int merge = -1;
			for (unsigned int i = 0; i < nVSConsts; i++)
			{
				if (strcmp(constants[i].name.c_str(), cDesc.Name) == 0)
				{
					merge = i;
					break;
				}
			}
			if (merge < 0)
			{
				(constants[nConstants]).name = std::string(cDesc.Name);
				(constants[nConstants]).vsReg = -1;
				(constants[nConstants]).psReg = cDesc.RegisterIndex;
			}
			else
			{
				(constants[merge]).psReg = cDesc.RegisterIndex;
			}
			nConstants++;
		}
	}
	samplers.resize(nSamplers);
	constants.resize(nConstants);

	#if 0
	qsort(&(SamplerD3D9)samplers[0],  nSamplers,  sizeof(SamplerD3D9),  samplerCompD3D9);

	qsort(&constants[0], nConstants, sizeof, constantCompD3D9);
	#else

	if(samplers.size()>1) std::sort(samplers.begin(), samplers.end(), samplerCompD3D9std);

	if(constants.size()>1) std::sort(constants.begin(), constants.end(), constantCompD3D9std);

	#endif

	shader.constants  = constants;
	shader.samplers   = samplers;
	shader.nConstants = nConstants;
	shader.nSamplers  = nSamplers;
	shaders.push_back(shader);
	return shaders.size()-1;
}
VertexFormatID RendererHLSLD3D9::addVertexFormatVrtl(const FormatDesc *formatDesc, const unsigned int nAttribs, const ShaderID shader)
{
	static const D3DDECLTYPE types[][4] =
	{
		D3DDECLTYPE_FLOAT1, D3DDECLTYPE_FLOAT2,    D3DDECLTYPE_FLOAT3, D3DDECLTYPE_FLOAT4,
		D3DDECLTYPE_UNUSED, D3DDECLTYPE_FLOAT16_2, D3DDECLTYPE_UNUSED, D3DDECLTYPE_FLOAT16_4,
		D3DDECLTYPE_UNUSED, D3DDECLTYPE_UNUSED,    D3DDECLTYPE_UNUSED, D3DDECLTYPE_UBYTE4N,
	}
	;
	/*
	DXGI_FORMAT_R32_UINT DXGI_FORMAT_R32G32_UINT
	D3DDECLTYPE_D3DCOLOR unsigned int
	FORMAT_UBYTE
	*/
	static const D3DDECLUSAGE usages[] =
	{
		(D3DDECLUSAGE) (-1),
		D3DDECLUSAGE_POSITION,
		D3DDECLUSAGE_TEXCOORD,
		D3DDECLUSAGE_NORMAL,
		//D3DDECLUSAGE_D3DCOLOR, ie a BYTE ???
		D3DDECLUSAGE_TANGENT,
		D3DDECLUSAGE_BINORMAL,
	}
	;
	int index[6];
	stx_memset(index, 0, sizeof(index));
	VertexFormatD3D9 vf;
	stx_memset(&vf.vertexSize, 0, sizeof(vf.vertexSize));
	D3DVERTEXELEMENT9 *vElem = new D3DVERTEXELEMENT9[nAttribs + 1];
	for (unsigned int i = 0; i < nAttribs; i++)
	{
		int stream = formatDesc[i].stream;
		int size = formatDesc[i].size;
		vElem[i].Stream = stream;
		vElem[i].Offset = vf.vertexSize[stream];
		vElem[i].Type = types[formatDesc[i].format][size - 1];
		vElem[i].Method = D3DDECLMETHOD_DEFAULT;
		vElem[i].Usage = usages[formatDesc[i].type];
		vElem[i].UsageIndex = index[formatDesc[i].type]++;
		vf.vertexSize[stream] += size * getFormatSize(formatDesc[i].format);
	}
	stx_memset(vElem + nAttribs, 0, sizeof(D3DVERTEXELEMENT9));
	vElem[nAttribs].Stream = 0xFF;
	vElem[nAttribs].Type = D3DDECLTYPE_UNUSED;
	HRESULT hr = dev->CreateVertexDeclaration(vElem, &vf.vertexDecl);
	delete vElem;
	if (hr != D3D_OK)
	{
		ErrorMsg("Couldn't create vertex declaration");
		return VF_NONE;
	}
	vertexFormats.push_back(vf);
	return vertexFormats.size()-1;
}
__DWORD__ usages[] =
{
	0,
	D3DUSAGE_DYNAMIC,
	D3DUSAGE_DYNAMIC,
}
;
VertexBufferID RendererHLSLD3D9::addVertexBufferVrtl(const int size, const BufferAccess bufferAccess, const void *data, VertexBufferID old)//, const IDataProcessor* p)
{
	VertexBufferD3D9 vb;
	vb.size = size;
	vb.usage = usages[bufferAccess];
	bool dynamic = (vb.usage & D3DUSAGE_DYNAMIC) != 0;
	if (dev->CreateVertexBuffer(size, vb.usage, 0, dynamic? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &vb.vertexBuffer, 0) != D3D_OK)
	{
		ErrorMsg("Couldn't create vertex buffer");
		return VB_NONE;
	}
	if (data != 0)
	{
		void *dest;
		if (vb.vertexBuffer->Lock(0, size, &dest, dynamic? D3DLOCK_DISCARD : 0) == D3D_OK)
		{
			stx_memcpy(dest, data, size);
			vb.vertexBuffer->Unlock();
		}
	}
//???
#if 0
		if(vertexBufferProcessors.size())
		{
			vertexBufferProcessors[
			vertexBufferProcessors.size()-1//???
].m_ppBuffer9
=&
vb.vertexBuffer;
		}
#else
/*
if(p) ((CVertexBufferProcessorD3D9*)p)->m_ppBuffer9
=&
vb.vertexBuffer;
*/
#endif
	if(old==-1)
	{
		vertexBuffers.push_back(vb);
		return vertexBuffers.size()-1;
	}
	else if(old<vertexBuffers.size())//???
	{
		deleteVertexBuffer(old);
		vertexBuffers[old]=vb;
		return old;
	}
	return -1;
}
IndexBufferID RendererHLSLD3D9::addIndexBufferVrtl(const unsigned int nIndices, const unsigned int indexSize, const BufferAccess bufferAccess, const void *data, IndexBufferID old)//, const IDataProcessor* p)
{
	IndexBufferD3D9 ib;
	ib.nIndices = nIndices;
	ib.indexSize = indexSize;
	ib.usage = usages[bufferAccess];
	bool dynamic = (ib.usage & D3DUSAGE_DYNAMIC) != 0;
	unsigned int size = nIndices * indexSize;
	if (dev->CreateIndexBuffer(size, ib.usage, indexSize == 2? D3DFMT_INDEX16 : D3DFMT_INDEX32, dynamic? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &ib.indexBuffer, 0) != D3D_OK)
	{
		ErrorMsg("Couldn't create index buffer");
		return IB_NONE;
	}
	if (data != 0)
	{
		void *dest;
		if (ib.indexBuffer->Lock(0, size, &dest, dynamic? D3DLOCK_DISCARD : 0) == D3D_OK)
		{
			stx_memcpy(dest, data, size);
			ib.indexBuffer->Unlock();
		}
		else
		{
			ErrorMsg("Couldn't lock index buffer");
		}
	}
//???
#if 0
	if(indexBufferProcessors.size())
		{
			indexBufferProcessors[
			indexBufferProcessors.size()-1//???
].m_ppBuffer9
=&
ib.indexBuffer;
		}
#else
/*
	if(p) ((CIndexBufferProcessorD3D9*)p)->m_ppBuffer9
=&
ib.indexBuffer;*/
#endif
	if(old==-1)
	{
		indexBuffers.push_back(ib);

		return indexBuffers.size()-1;
	}
	else
	{
		deleteIndexBuffer(old);
		indexBuffers[old]=ib;
		return old;
	}
	return -1;
}
SamplerStateID RendererHLSLD3D9::addSamplerStateVrtl(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod, const unsigned int maxAniso, const int compareFunc, const float *border_color)
{
	SamplerStateD3D9 samplerState;
	samplerState.minFilter = hasAniso(filter)? D3DTEXF_ANISOTROPIC : (filter != NEAREST)? D3DTEXF_LINEAR : D3DTEXF_POINT;
	samplerState.magFilter = hasAniso(filter)? D3DTEXF_ANISOTROPIC : (filter != NEAREST)? D3DTEXF_LINEAR : D3DTEXF_POINT;
	samplerState.mipFilter = (filter == TRILINEAR || filter == TRILINEAR_ANISO)? D3DTEXF_LINEAR : hasMipmaps(filter)? D3DTEXF_POINT : D3DTEXF_NONE;
	samplerState.wrapS = (s == WRAP)? D3DTADDRESS_WRAP : (s == CLAMP)? D3DTADDRESS_CLAMP : D3DTADDRESS_BORDER;
	samplerState.wrapT = (t == WRAP)? D3DTADDRESS_WRAP : (t == CLAMP)? D3DTADDRESS_CLAMP : D3DTADDRESS_BORDER;
	samplerState.wrapR = (r == WRAP)? D3DTADDRESS_WRAP : (r == CLAMP)? D3DTADDRESS_CLAMP : D3DTADDRESS_BORDER;
	samplerState.maxAniso = min((unsigned int) maxAnisotropic, maxAniso);
	samplerState.lod = lod;
	if (border_color)
	{
		samplerState.borderColor = D3DCOLOR_ARGB(
		uint32(border_color[3] * 255.0f + 0.5f),
		uint32(border_color[0] * 255.0f + 0.5f),
		uint32(border_color[1] * 255.0f + 0.5f),
		uint32(border_color[2] * 255.0f + 0.5f));
	}
	else
	{
		samplerState.borderColor = 0;
	}
	samplerStates.push_back(samplerState);
	return samplerStates.size()-1;
}
BlendStateID RendererHLSLD3D9::addBlendState(const int srcFactorRGB, const int destFactorRGB, const int srcFactorAlpha, const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, const int mask, const bool alphaToCoverage)
{
	BlendStateD3D9 blendState;
	blendState.srcFactorRGB = srcFactorRGB;
	blendState.dstFactorRGB = destFactorRGB;
	blendState.blendModeRGB = blendModeRGB;
	blendState.srcFactorAlpha = srcFactorAlpha;
	blendState.dstFactorAlpha = destFactorAlpha;
	blendState.blendModeAlpha = blendModeAlpha;
	blendState.mask = mask;
	blendState.blendEnable = (srcFactorRGB != ONE || destFactorRGB != ZERO || srcFactorAlpha != ONE || destFactorAlpha != ZERO);
    	blendState.alphaEnable = (srcFactorRGB != ONE || destFactorRGB != ZERO || srcFactorAlpha != ONE || destFactorAlpha != ZERO);//???
	blendStates.push_back(blendState);
	return blendStates.size()-1;
}
DepthStateID RendererHLSLD3D9::addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc,
const bool stencilTest, const uint8 stencilMask, const int stencilFunc,
const int stencilFail, const int depthFail, const int stencilPass)
{
	DepthStateD3D9 depthState;
	depthState.depthTest  = depthTest;
	depthState.depthWrite = depthWrite;
	depthState.depthFunc  = depthFunc;
	depthStates.add(depthState);
	return depthStates.size()-1;
}
RasterizerStateID RendererHLSLD3D9::addRasterizerStateVrtl(const int cullMode, const int fillMode, const bool multiSample, const bool scissor)
{
	RasterizerStateD3D9 rasterizerState;
	rasterizerState.cullMode = cullMode;
	rasterizerState.fillMode = fillMode;
	rasterizerState.multiSample = multiSample;
	rasterizerState.scissor = scissor;
	rasterizerStates.push_back(rasterizerState);
	return rasterizerStates.size()-1;
}
int getPow2(const int x)
{
	int i = 1;
	while (i < x) i += i;
	if (4 * x < 3 * i) i >>= 1;
	return i;
}
int RendererHLSLD3D9::GetTextureWidth(TextureID id)
{
	if(id<textures.size())
		return textures[id].width;
	return 0;
}
int RendererHLSLD3D9::GetTextureHeight(TextureID id)
{
	if(id<textures.size())
		return textures[id].height;
	return 0;
}
TextureID RendererHLSLD3D9::addTexture(Image3* img, const bool useMips, const SamplerStateID samplerState, unsigned int flags)
{
	TextureD3D9 tex;
	stx_memset(&tex, 0, sizeof(TextureD3D9));
	//img->BindImage();
	tex.ss=samplerState;
	tex.width=img->getWidth();
	tex.height=img->getHeight();
	tex.depth=img->getDepth();
	if(useMips&&img->getMipMapCount()<2)
		img->createMipMaps();
	tex.mipMapped = (img->getMipMapCount() > 1);
	tex.mipMapLevels=img->getMipMapCount();
	FORMAT format = img->getFormat();
		tex.format=format;
/*    if (format == FORMAT_RGB8)
	{
	ilEnable(IL_USE_KEY_COLOUR);
	ILclampf Alpha=0.0f;//???
	ilKeyColour(0.0f, 0.0f, 0.0f, Alpha);

	ilConvertImage(//ilGetInteger(IL_IMAGE_FORMAT)FORMAT_RGBA8
	IL_RGBA, ilGetInteger(IL_IMAGE_TYPE));
	ilSetAlpha(1.0f);
	tex.format = FORMAT_RGBA8;
	}
*/
	format=tex.format;
    //???if (ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL)>=3) iluSwapColours();
    //???if (format == FORMAT_RGB8 || format == FORMAT_RGBA8)
    //img->swap(0, 2);
#if 0
					if(alpha>0.0f)
					{
#if 1
						//if(getChannelCount(format)==4){img->removeChannels(true,true,true,false);}
						//format = img->getFormat();
						if(getChannelCount(format)==3)
						{
							img->
								addChannel
								//setAlpha
								(alpha);
						}
#else
						img->format(FORMAT_RGBA8);
						img->setAlpha(alpha);
						format = img->getFormat();
#endif

					}
#endif
	if (img->isCube())
	{
		tex.dim=3;
		if (dev->CreateCubeTexture(img->getWidth(), img->getMipMapCount(), 0, formatsd3d9[format], D3DPOOL_MANAGED, (LPDIRECT3DCUBETEXTURE9 *) &tex.texture, 0) != D3D_OK)
		{
			ErrorMsg("Couldn't create cubemap");
			return false;
		}
	}
	else if (img->is3D())
	{
		tex.dim=3;
		if (dev->CreateVolumeTexture(img->getWidth(), img->getHeight(), img->getDepth(), img->getMipMapCount(), 0, formatsd3d9[format], D3DPOOL_MANAGED, (LPDIRECT3DVOLUMETEXTURE9 *) &tex.texture, 0) != D3D_OK)
		{
			ErrorMsg("Couldn't create volumetric texture");
			return false;
		}
	}
	else
	{
		tex.dim=2;
		D3DFORMAT form = formatsd3d9[format];
		if (dev->CreateTexture(img->getWidth(), img->getHeight(), img->getMipMapCount(), 0, formatsd3d9[format], D3DPOOL_MANAGED, (LPDIRECT3DTEXTURE9 *) &tex.texture, 0) != D3D_OK)
		{
			ErrorMsg("Couldn't create texture");
			return false;
		}
	}


    unsigned char *src;
	int mipMapLevel = 0;
	while ((src = img->getPixels(mipMapLevel)) != 0)
	{
		//img->BindImage(mipMapLevel);
		int size = img->getMipMappedSize(mipMapLevel, 1);

		if (img->is3D())
		{
			D3DLOCKED_BOX box;
			if (((LPDIRECT3DVOLUMETEXTURE9) tex.texture)->LockBox(mipMapLevel, &box, 0, 0) == D3D_OK)
			{
				stx_memcpy(box.pBits, src, size);
				((LPDIRECT3DVOLUMETEXTURE9) tex.texture)->UnlockBox(mipMapLevel);
			}
		}
		else if (img->isCube())
		{
			size /= 6;
			D3DLOCKED_RECT rect;
			for (int i = 0; i < 6; i++)
			{
				if (((LPDIRECT3DCUBETEXTURE9) tex.texture)->LockRect((D3DCUBEMAP_FACES) i, mipMapLevel, &rect, 0, 0) == D3D_OK)
				{
					stx_memcpy(rect.pBits, src, size);
					((LPDIRECT3DCUBETEXTURE9) tex.texture)->UnlockRect((D3DCUBEMAP_FACES) i, mipMapLevel);
				}
				src += size;
			}
		}
		else
		{
			D3DLOCKED_RECT rect;
			if (((LPDIRECT3DTEXTURE9) tex.texture)->LockRect(mipMapLevel, &rect, 0, 0) == D3D_OK)
			{
				stx_memcpy(rect.pBits, (void*)img->getPixels(mipMapLevel), size);
/*
if (ilGetInteger(IL_IMAGE_CHANNELS)==3)
	ilCopyPixels(0, 0, 0, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 1, IL_RGB, IL_UNSIGNED_BYTE, rect.pBits);
else if (ilGetInteger(IL_IMAGE_CHANNELS)==4)
	ilCopyPixels(0, 0, 0, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 1, IL_RGBA, IL_UNSIGNED_BYTE, rect.pBits);
*/

				((LPDIRECT3DTEXTURE9) tex.texture)->UnlockRect(mipMapLevel);

			}
		}
		if(!useMips)
			break;
		mipMapLevel++;
	}
/*
		if(textureProcessors.size())
		{
			textureProcessors[
			textureProcessors.size()-1//???//???
].m_ppTexture9
=(IDirect3DTexture9 **)&
tex.texture;
		}*/

	return textures.add(tex);
}
bool createRenderTarget(LPDIRECT3DDEVICE9 dev, TextureD3D9 &tex)
{
	if (isDepthFormat(tex.format))
	{
		if (tex.surfaces == 0) tex.surfaces = new LPDIRECT3DSURFACE9;
		if (dev->CreateDepthStencilSurface(tex.width, tex.height, formatsd3d9[tex.format], D3DMULTISAMPLE_NONE, 0, FALSE, tex.surfaces, 0) != D3D_OK)
		{
			delete tex.surfaces;
			ErrorMsg("Couldn't create depth surface");
			return false;
		}
	}
	else
	{
		if (tex.flags & CUBEMAP)
		{
			if (dev->CreateCubeTexture(tex.width, tex.mipMapped? 0 : 1, tex.usage, formatsd3d9[tex.format], D3DPOOL_DEFAULT, (LPDIRECT3DCUBETEXTURE9 *) &tex.texture, 0) != D3D_OK)
			{
				ErrorMsg("Couldn't create render target");
				return false;
			}
			if (tex.surfaces == 0) tex.surfaces = new LPDIRECT3DSURFACE9[6];
			for (unsigned int i = 0; i < 6; i++)
			{
				((LPDIRECT3DCUBETEXTURE9) tex.texture)->GetCubeMapSurface((D3DCUBEMAP_FACES) i, 0, &tex.surfaces[i]);
			}
		}
		else
		{
			if (dev->CreateTexture(tex.width, tex.height, tex.mipMapped? 0 : 1, tex.usage, formatsd3d9[tex.format], D3DPOOL_DEFAULT, (LPDIRECT3DTEXTURE9 *) &tex.texture, 0) != D3D_OK)
			{
				ErrorMsg("Couldn't create render target");
				return false;
			}
			if (tex.surfaces == 0) tex.surfaces = new LPDIRECT3DSURFACE9;
			((LPDIRECT3DTEXTURE9) tex.texture)->GetSurfaceLevel(0, tex.surfaces);
		}
	}
	return true;
}
TextureID RendererHLSLD3D9::addRenderTargetVrtl(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags)
{
	if (depth > 1 || arraySize > 1) return TEXTURE_NONE;
	if (msaaSamples > 1 || mipMapCount > 1) return TEXTURE_NONE;
	TextureD3D9 tex;
	stx_memset(&tex, 0, sizeof(TextureD3D9));
	tex.width  = width;
	tex.height = height;
	tex.format = format;
	tex.usage  = D3DUSAGE_RENDERTARGET;
	tex.flags  = flags;
	tex.mipMapped = false;
	if (flags & R2VB) tex.usage |= D3DUSAGE_DMAP;
	if (createRenderTarget(dev, tex))
	{
		return textures.add(tex);
	}
	else
	{
		return TEXTURE_NONE;
	}
}
TextureID RendererHLSLD3D9::addRenderDepthVrtl(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags)
{
	if (msaaSamples > 1) return TEXTURE_NONE;
	TextureD3D9 tex;
	stx_memset(&tex, 0, sizeof(TextureD3D9));
	tex.format = format;
	tex.width  = width;
	tex.height = height;
	if (createRenderTarget(dev, tex))
	{
		return textures.add(tex);
	}
	else
	{
		return TEXTURE_NONE;
	}
}
bool RendererHLSLD3D9::resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize)
{
	if (depth > 1 || arraySize > 1 || mipMapCount > 1) return false;
	if ((textures[renderTarget]).surfaces)
	{
		int n = ((textures[renderTarget]).flags & CUBEMAP)? 6 : 1;
		if ((textures[renderTarget]).texture) (textures[renderTarget]).texture->Release();
		for (int k = 0; k < n; k++)
		{
			(textures[renderTarget]).surfaces[k]->Release();
		}
	}
	(textures[renderTarget]).width	= width;
	(textures[renderTarget]).height = height;
	return createRenderTarget(dev, (textures[renderTarget]));
}
void RendererHLSLD3D9::applyTextures()
{
	for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++){
		TextureID texture = selectedTextures[i];
		if (texture != currentTextures[i]){
			if (texture == TEXTURE_NONE){
				dev->SetTexture(i, 0);
				//LOG_PRINT("texture=%d,%d\n",i, 0);
			} else {
				dev->SetTexture(i, textures[texture].texture);
				//LOG_PRINT("texture=%d,%d\n",i, textures[texture].texture);
			}
			currentTextures[i] = texture;
		}
	}
}

void RendererHLSLD3D9::setSamplerState(const char *samplerName, const SamplerStateID samplerState)
{
	int unit = getSamplerUnit(selectedShader, samplerName);
	if (unit >= 0)
	{
		selectedSamplerStates[unit] = samplerState;
	}
}
void RendererHLSLD3D9::applySamplerStates()
{
	for (unsigned int i = 0; i < MAX_SAMPLERSTATE; i++)
	{
		SamplerStateID samplerState = selectedSamplerStates[i];
		if (samplerState != currentSamplerStates[i])
		{
			SamplerStateD3D9 ss;
			SamplerStateD3D9 css;
			if (samplerState == SS_NONE)
			{
				ZeroMemory(&ss ,sizeof(SamplerStateD3D9));
				ss.magFilter = D3DTEXF_POINT;
				ss.minFilter = D3DTEXF_POINT;
				ss.mipFilter = D3DTEXF_NONE;
				ss.wrapS = D3DTADDRESS_WRAP;
				ss.wrapT = D3DTADDRESS_WRAP;
				ss.wrapR = D3DTADDRESS_WRAP;
				ss.maxAniso = 1;
			}
			else
			{
				ss = samplerStates[samplerState];
			}
			if (currentSamplerStates[i] == SS_NONE)
			{
				ZeroMemory(&css ,sizeof(SamplerStateD3D9));
				css.magFilter = D3DTEXF_POINT;
				css.minFilter = D3DTEXF_POINT;
				css.mipFilter = D3DTEXF_NONE;
				css.wrapS = D3DTADDRESS_WRAP;
				css.wrapT = D3DTADDRESS_WRAP;
				css.wrapR = D3DTADDRESS_WRAP;
				css.maxAniso = 1;
			}
			else
			{
				css = samplerStates[currentSamplerStates[i]];
			}
			if (ss.minFilter != css.minFilter)
			{
				V(dev->SetSamplerState(i, D3DSAMP_MINFILTER, ss.minFilter))
			}
			;
			if (ss.magFilter != css.magFilter)
			{
				V(dev->SetSamplerState(i, D3DSAMP_MAGFILTER, ss.magFilter))
			}
			;
			if (ss.mipFilter != css.mipFilter)
			{
				V(dev->SetSamplerState(i, D3DSAMP_MIPFILTER, ss.mipFilter))
			}
			;
			if (ss.wrapS != css.wrapS)
			{
				V(dev->SetSamplerState(i, D3DSAMP_ADDRESSU, ss.wrapS))
			}
			;
			if (ss.wrapT != css.wrapT)
			{
				V(dev->SetSamplerState(i, D3DSAMP_ADDRESSV, ss.wrapT))
			}
			;
			if (ss.wrapR != css.wrapR)
			{
				V(dev->SetSamplerState(i, D3DSAMP_ADDRESSW, ss.wrapR))
			}
			;
			if (ss.maxAniso != css.maxAniso)
			{
				V(dev->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, ss.maxAniso))
			}
			;
			if (ss.lod != css.lod)
			{
				V(dev->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *(__DWORD__ *) &ss.lod))
			}
			;
			if (ss.borderColor != css.borderColor)
			{
				V(dev->SetSamplerState(i, D3DSAMP_BORDERCOLOR, ss.borderColor))
			}
			;
			currentSamplerStates[i] = samplerState;
		}
	}
}
void RendererHLSLD3D9::applyConstants()
{
	if (minVSDirty < maxVSDirty)
	{
		V(dev->SetVertexShaderConstantF(minVSDirty, (const float *) (vsRegs + minVSDirty), maxVSDirty - minVSDirty));
		minVSDirty = 256;
		maxVSDirty = -1;
	}
	if (minPSDirty < maxPSDirty)
	{
		V(dev->SetPixelShaderConstantF(minPSDirty, (const float *) (psRegs + minPSDirty), maxPSDirty - minPSDirty));
		minPSDirty = 224;
		maxPSDirty = -1;
	}
}

bool RendererHLSLD3D9::generateMipMaps(const TextureID renderTarget)
{
	return false;
}
void RendererHLSLD3D9::changeRenderTargetsVrtl(const TextureID *colorRTs, const unsigned int nRenderTargets, const TextureID depthRT, const int depthSlice, const int *slices)
{
	for (unsigned int i = 0; i < nRenderTargets; i++)
	{
		TextureID rt = colorRTs[i];
		int face = (slices != 0)? slices[i] : 0;
		if (rt != currentColorRT[i] || face != currentColorRTSlice[i])
		{
			if (face == NO_SLICE)
			{
				V(dev->SetRenderTarget(i, (textures[rt]).surfaces[0]));
			}
			else
			{
				V(dev->SetRenderTarget(i, (textures[rt]).surfaces[face]));
			}
			currentColorRT[i] = rt;
			currentColorRTSlice[i] = face;
		}
	}
	for (unsigned int i = nRenderTargets; i < nMRTs; i++)
	{
		if (currentColorRT[i] != TEXTURE_NONE)
		{
			V(dev->SetRenderTarget(i, 0));
			currentColorRT[i] = TEXTURE_NONE;
		}
	}
	if (depthRT != currentDepthRT)
	{
		if (depthRT == TEXTURE_NONE)
		{
			V(dev->SetDepthStencilSurface(0));
		}
		else if (depthRT == FB_DEPTH)
		{
			V(dev->SetDepthStencilSurface(fbDepth));
		}
		else
		{
			V(dev->SetDepthStencilSurface((textures[depthRT]).surfaces[0]));
		}
		currentDepthRT = depthRT;
	}
}
void RendererHLSLD3D9::changeToMainFramebuffer()
{
	if (currentColorRT[0] != TEXTURE_NONE)
	{
		V(dev->SetRenderTarget(0, fbColor));
		currentColorRT[0] = TEXTURE_NONE;
	}
	for (unsigned int i = 1; i < nMRTs; i++)
	{
		if (currentColorRT[i] != TEXTURE_NONE)
		{
			V(dev->SetRenderTarget(i, 0));
			currentColorRT[i] = TEXTURE_NONE;
		}
	}
	if (currentDepthRT != FB_DEPTH)
	{
		V(dev->SetDepthStencilSurface(fbDepth));
		currentDepthRT = FB_DEPTH;
	}
}

void RendererHLSLD3D9::Init0(void)
{
	if(defaultFont == -1)
	{
		//defaultFont = addFont(FONT_PATH"Future.dds", FONT_PATH"Future.font", linearClamp);
	}
	#if 0
	if((m_pD3D!=0)&&(m_pd3dDevice!=0))return;
	0;
	bool doNotInit=((m_fbColor!=0)||
	(m_fbDepth!=0)||
	(m_pd3dxFont!=0)||
	(m_pd3dxSmallFont!=0)||
	(m_pd3dDevice!=0)||
	(m_pD3D!=0));
	if(doNotInit)return;
	SAFE_RELEASE(m_fbColor);
	SAFE_RELEASE(m_fbDepth);
	SAFE_RELEASE(m_pd3dxFont);
	SAFE_RELEASE(m_pd3dxSmallFont);
	SAFE_RELEASE(m_pd3dDevice);
	SAFE_RELEASE(m_pD3D);

	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

	V(m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_caps));
	D3DDISPLAYMODE d3ddm;
	V(m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ));
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed		     = TRUE;
	d3dpp.SwapEffect	     = D3DSWAPEFFECT_COPY;
	d3dpp.BackBufferFormat	     = d3ddm.Format;
	m_fmt= d3ddm.Format;
	d3dpp.BackBufferWidth	 = IRenderer::GetRendererInstance()->GetViewportWidth();
	d3dpp.BackBufferHeight	 = IRenderer::GetRendererInstance()->GetViewportHeight();
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = AUTODEPTHSTENCILFORMAT;
	d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.BackBufferCount=1;
	__DWORD__ behaviorFlags=0;
	if( m_caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
	behaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
	behaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	m_d3dpp=d3dpp;
	V(m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, STX_Service::GetWindowInstance()->GetHWND(),
	behaviorFlags,
	&d3dpp, &m_pd3dDevice ));
	LOG_PRINT("DirectX Versions:\n");
	LOG_PRINT("DIRECT3D_VERSION=%x\n",DIRECT3D_VERSION);
	#ifdef D3D_DEBUG_INFO
	LOG_PRINT("D3D_SDK_VERSION=%x\n",D3D_SDK_VERSION);
	LOG_PRINT("D3D9b_SDK_VERSION=%x\n",D3D9b_SDK_VERSION);
	#else
	LOG_PRINT("D3D_SDK_VERSION=%d\n",D3D_SDK_VERSION);
	LOG_PRINT("D3D9b_SDK_VERSION=%d\n",D3D9b_SDK_VERSION);
	#endif
	V(m_pd3dDevice->GetRenderTarget(0, &m_fbColor));
	V(m_pd3dDevice->GetDepthStencilSurface(&m_fbDepth));
	HDC hDC;
	int nHeight;
	int nPointSize = 18;
	hDC = GetDC( 0 );
	nHeight = -( MulDiv( nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72 ) );
	ReleaseDC( 0, hDC );
	V(D3DXCreateFont( m_pd3dDevice, nHeight, 0, FW_BOLD, 0, FALSE,
	DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
	DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"),
	&m_pd3dxFont ));
	V(D3DXCreateFont( m_pd3dDevice, nHeight/2, 0, FW_BOLD, 0, FALSE,
	DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
	DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"),
	&m_pd3dxSmallFont ));
	LPDIRECT3DSURFACE9 pBackBuffer = 0;
	V(m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ));
	pBackBuffer->GetDesc( &m_pBackBufferSurfaceDesc );
	pBackBuffer->Release();
	0;
	D3DVIEWPORT9 vp;
	vp.Width = IRenderer::GetRendererInstance()->GetViewportWidth();
	vp.Height = IRenderer::GetRendererInstance()->GetViewportHeight();
	vp.MinZ = 0;
	vp.MaxZ = 1;
	vp.X = 0;
	vp.Y = 0;
	V(m_pd3dDevice->SetViewport(& vp ));

	LOG_PRINT("VP %dx%d\n",vp.Width,vp.Height);
	if (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, (D3DFORMAT) MAKEFOURCC('R','2','V','B')) == D3D_OK)
	{
		bsupportsR2VB = true;
		if (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F) == D3D_OK)
		{
			if (caps.DeclTypes & D3DDTCAPS_FLOAT16_4) bsupportsHalf = true;
		}
	}
	else
	{
		bsupportsR2VB = false;
		if (caps.VertexShaderVersion >= D3DVS_VERSION(3,0))
		{
			if (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DUSAGE_QUERY_VERTEXTEXTURE, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F) == D3D_OK)
			{
				if (caps.DeclTypes & D3DDTCAPS_FLOAT16_4) bsupportsHalf = true;
			}
		}
	}
	ZERO		    = D3DBLEND_ZERO;
	ONE		    = D3DBLEND_ONE;
	SRC_COLOR	    = D3DBLEND_SRCCOLOR;
	ONE_MINUS_SRC_COLOR = D3DBLEND_INVSRCCOLOR;
	DST_COLOR	    = D3DBLEND_DESTCOLOR;
	ONE_MINUS_DST_COLOR = D3DBLEND_INVDESTCOLOR;
	SRC_ALPHA	    = D3DBLEND_SRCALPHA;
	ONE_MINUS_SRC_ALPHA = D3DBLEND_INVSRCALPHA;
	DST_ALPHA	    = D3DBLEND_DESTALPHA;
	ONE_MINUS_DST_ALPHA = D3DBLEND_INVDESTALPHA;
	SRC_ALPHA_SATURATE  = D3DBLEND_SRCALPHASAT;
	BM_ADD		    = D3DBLENDOP_ADD;
	BM_SUBTRACT	    = D3DBLENDOP_SUBTRACT;
	BM_REVERSE_SUBTRACT = D3DBLENDOP_REVSUBTRACT;
	BM_MIN		    = D3DBLENDOP_MIN;
	BM_MAX		    = D3DBLENDOP_MAX;
	NEVER	 = D3DCMP_NEVER;
	LESS	 = D3DCMP_LESS;
	EQUAL	 = D3DCMP_EQUAL;
	LEQUAL	 = D3DCMP_LESSEQUAL;
	GREATER  = D3DCMP_GREATER;
	NOTEQUAL = D3DCMP_NOTEQUAL;
	GEQUAL	 = D3DCMP_GREATEREQUAL;
	ALWAYS	 = D3DCMP_ALWAYS;
	KEEP	 = D3DSTENCILOP_KEEP;
	SET_ZERO = D3DSTENCILOP_ZERO;
	REPLACE  = D3DSTENCILOP_REPLACE;
	INVERT	 = D3DSTENCILOP_INVERT;
	INCR	 = D3DSTENCILOP_INCR;
	DECR	 = D3DSTENCILOP_DECR;
	INCR_SAT = D3DSTENCILOP_INCRSAT;
	DECR_SAT = D3DSTENCILOP_DECRSAT;
	CULL_NONE  = D3DCULL_NONE;
	CULL_BACK  = D3DCULL_CCW;
	CULL_FRONT = D3DCULL_CW;
	SOLID = D3DFILL_SOLID;
	WIREFRAME = D3DFILL_WIREFRAME;
	if (d3dcaps.PixelShaderVersion >= D3DPS_VERSION(1,0))
	{
		nImageUnits = 4;
		if (d3dcaps.PixelShaderVersion >= D3DPS_VERSION(1,4)) nImageUnits = 6;
		if (d3dcaps.PixelShaderVersion >= D3DPS_VERSION(2,0)) nImageUnits = 16;
	}
	else
	{
		nImageUnits = d3dcaps.MaxSimultaneousTextures;
	}
	maxAnisotropic = d3dcaps.MaxAnisotropy;
	nMRTs = d3dcaps.NumSimultaneousRTs;
	if (nMRTs > MAX_MRTS) nMRTs = MAX_MRTS;
	plainShader = SHADER_NONE;
	plainVF = VF_NONE;
	eventQuery = 0;
	setD3Ddefaults();
	resetToDefaults();
	RendererHLSLCg::InitF();
	TestFBOD3D();
	#endif
}

#include <oleauto.h>
#include <wbemidl.h>

typedef BOOL ( WINAPI* PfnCoSetProxyBlanket )(
IUnknown* pProxy,
__DWORD__ dwAuthnSvc,
__DWORD__ dwAuthzSvc,
OLECHAR* pServerPrincName,
__DWORD__ dwAuthnLevel,
__DWORD__ dwImpLevel,
RPC_AUTH_IDENTITY_HANDLE pAuthInfo,
__DWORD__ dwCapabilities );

UINT32 RendererHLSLD3D9::GetAvailableTextureMem( )
{
	//IDirect3DDevice9* pd3dDevice =dev;
    HRESULT hr=0;
#if 0
    IWbemLocator* pIWbemLocator = 0;
    IWbemServices* pIWbemServices = 0;
    BSTR pNamespace = 0;

    UINT32 dwAdapterRAM = 0;
    bool bFoundViaWMI = false;

    CoInitialize( 0 );

    hr = CoCreateInstance( CLSID_WbemLocator,
			   0,
			   CLSCTX_INPROC_SERVER,
			   IID_IWbemLocator,
			   ( LPVOID* )&pIWbemLocator );

    if( SUCCEEDED( hr ) && pIWbemLocator )
    {
	// Using the locator, connect to WMI in the given namespace.
	pNamespace = SysAllocString( L"\\\\.\\root\\cimv2" );

	hr = pIWbemLocator->ConnectServer( pNamespace, 0, 0, 0L,
					   0L, 0, 0, &pIWbemServices );
	if( SUCCEEDED( hr ) && pIWbemServices != 0 )
	{
	    HINSTANCE hinstOle32 = 0;

	    hinstOle32 = LoadLibraryW( L"ole32.dll" );
	    if( hinstOle32 )
	    {
		PfnCoSetProxyBlanket pfnCoSetProxyBlanket = 0;

		pfnCoSetProxyBlanket = ( PfnCoSetProxyBlanket )GetProcAddress( hinstOle32, "CoSetProxyBlanket" );
		if( pfnCoSetProxyBlanket != 0 )
		{
		    // Switch security level to IMPERSONATE.
		    pfnCoSetProxyBlanket( pIWbemServices,		// proxy
					  RPC_C_AUTHN_WINNT,		// authentication service
					  RPC_C_AUTHZ_NONE,		// authorization service
					  0, 			// server principle name
					  RPC_C_AUTHN_LEVEL_CALL,	// authentication level
					  RPC_C_IMP_LEVEL_IMPERSONATE,	// impersonation level
					  0, 			// identity of the client
					  EOAC_NONE );			// capability flags
		}

		FreeLibrary( hinstOle32 );
	    }

	    IEnumWbemClassObject* pEnumProcessorDevs = 0;
	    BSTR pClassName = 0;

	    pClassName = SysAllocString( L"Win32_VideoController" );

	    hr = pIWbemServices->CreateInstanceEnum( pClassName,
						     0,
						     0, &pEnumProcessorDevs );

	    if( SUCCEEDED( hr ) && pEnumProcessorDevs )
	    {
		IWbemClassObject* pProcessorDev = 0;
		__DWORD__ uReturned = 0;
		BSTR pPropName = 0;

		// Get the first one in the list
		pEnumProcessorDevs->Reset();
		hr = pEnumProcessorDevs->Next( 5000,		 // timeout in 5 seconds
					       1,		 // return just one storage device
					       &pProcessorDev,	 // pointer to storage device
					       &uReturned );	 // number obtained: one or zero

		if( SUCCEEDED( hr ) && uReturned != 0 && pProcessorDev != 0 )
		{
		    pPropName = SysAllocString( L"AdapterRAM" );

		    VARIANT var;
		    hr = pProcessorDev->Get( pPropName, 0L, &var, 0, 0 );
		    if( SUCCEEDED( hr ) )
		    {
			dwAdapterRAM = var.lVal;
			bFoundViaWMI = true;
		    }

		    if( pPropName )
			SysFreeString( pPropName );
		}

		SAFE_RELEASE( pProcessorDev );
	    }

	    if( pClassName )
		SysFreeString( pClassName );
	    SAFE_RELEASE( pEnumProcessorDevs );
	}

	if( pNamespace )
	    SysFreeString( pNamespace );
	SAFE_RELEASE( pIWbemServices );
    }

    SAFE_RELEASE( pIWbemLocator );

    if( bFoundViaWMI )
	return dwAdapterRAM;

    UINT r=dev->GetAvailableTextureMem();
	char buf[256];
    stx_snprintf(buf,256,"AvailableTextureMem=%d\n",r);
	OutputDebugStringA(buf);
#endif
	return hr;
}

int RendererHLSLD3D9::removeTexture(const TextureID texture)
{
	if ((textures[texture]).surfaces)
	{
		int n = ((textures[texture]).flags & CUBEMAP)? 6 : 1;
		for (int k = 0; k < n; k++)
		{
			(textures[texture]).surfaces[k]->Release();
		}
		delete (textures[texture]).surfaces;
		(textures[texture]).surfaces = 0;
	}
	if ((textures[texture]).texture)
	{
		(textures[texture]).texture->Release();
		(textures[texture]).texture = 0;
	}
	int r=0;
	for(unsigned int i=0;i<textures.size();i++)
	{
		if(textures[i].texture) r++;
	}
	return r;
}

int RendererHLSLD3D9::deleteVertexBuffer(const VertexBufferID vertexBuffer)
{
	if(vertexBuffer>=vertexBuffers.size()) return -1;
	if ((vertexBuffers[vertexBuffer]).vertexBuffer)
	{
		(vertexBuffers[vertexBuffer]).vertexBuffer->Release();
		(vertexBuffers[vertexBuffer]).vertexBuffer = 0;
	}
	int r=0;
	for(unsigned int i=0;i<vertexBuffers.size();i++)
	{
		if(vertexBuffers[i].vertexBuffer) r++;
	}
	return r;
}
int RendererHLSLD3D9::deleteIndexBuffer(const IndexBufferID indexBuffer)
{
	if(indexBuffer>=indexBuffers.size()) return -1;
	if ((indexBuffers[indexBuffer]).indexBuffer)
	{
		(indexBuffers[indexBuffer]).indexBuffer->Release();
		(indexBuffers[indexBuffer]).indexBuffer = 0;
	}
	int r=0;
	for(unsigned int i=0;i<indexBuffers.size();i++)
	{
		if(indexBuffers[i].indexBuffer) r++;
	}
	return r;
}

int RendererHLSLD3D9::texturePitchVrtl(const TextureID id,int level)
{
	D3DLOCKED_RECT m_pLockedRects;
	((IDirect3DTexture9*)textures[id].texture)->LockRect( level, &m_pLockedRects, 0, 0 );
	return m_pLockedRects.Pitch;
}
BYTE *RendererHLSLD3D9::lockTexture(const TextureID id,int level, int& pitch)
{
	D3DLOCKED_RECT m_pLockedRects;
	((IDirect3DTexture9*)textures[id].texture)->LockRect( level, &m_pLockedRects, 0, 0 );
	pitch=m_pLockedRects.Pitch;
	return (BYTE *)m_pLockedRects.pBits;
}
BYTE *RendererHLSLD3D9::lockVertexBuffer(const VertexBufferID id)
{
	BYTE* m_pLockedData;
	vertexBuffers[id].vertexBuffer->Lock( 0, 0, ( void** )&m_pLockedData, 0 );
	return m_pLockedData;
}
BYTE *RendererHLSLD3D9::lockIndexBuffer(const IndexBufferID id)
{
	 BYTE* m_pLockedData;
	indexBuffers[id].indexBuffer->Lock( 0, 0, ( void** )&m_pLockedData, 0 );
return m_pLockedData;
}
BYTE *RendererHLSLD3D9::unlockTextureVrtl(const TextureID id,int level)
{
	((IDirect3DTexture9*)textures[id].texture)->UnlockRect(level);
	return 0;
}
BYTE *RendererHLSLD3D9::unlockVertexBuffer(const VertexBufferID id)
{
	vertexBuffers[id].vertexBuffer->Unlock();
		return 0;
}
BYTE *RendererHLSLD3D9::unlockIndexBuffer(const IndexBufferID id)
{
	indexBuffers[id].indexBuffer->Unlock();
		return 0;
}
BYTE *RendererHLSLD3D9::fillVertexBufferVrtl(const VertexBufferID id, BYTE*  m_pData, int l)
{
	BYTE* m_pLockedData;
	vertexBuffers[id].vertexBuffer->Lock( 0, 0, ( void** )&m_pLockedData, 0 );
	stx_memcpy(m_pLockedData,m_pData,l);
	vertexBuffers[id].vertexBuffer->Unlock();
	return 0;
}
BYTE *RendererHLSLD3D9::fillIndexBufferVrtl(const IndexBufferID id, BYTE*  m_pData, int l)
{
	 BYTE* m_pLockedData;
	indexBuffers[id].indexBuffer->Lock( 0, 0, ( void** )&m_pLockedData, 0 );
	stx_memcpy(m_pLockedData,m_pData,l);
	indexBuffers[id].indexBuffer->Unlock();
	return 0;
}
#if 0
HRESULT RendererHLSLD3D9::ProcessTexture( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes )
{
	return textureProcessors[pProcessor].Process( pLocalData, Bytes);
}
HRESULT RendererHLSLD3D9::LockTextureDeviceObject(ProcessorID pProcessor)
{
	return textureProcessors[pProcessor].LockDeviceObject();
}
HRESULT RendererHLSLD3D9::CopyToResourceTexture(ProcessorID pProcessor)
{
	return textureProcessors[pProcessor].CopyToResource();
}
HRESULT RendererHLSLD3D9::UnLockTextureDeviceObject(ProcessorID pProcessor)
{
	return textureProcessors[pProcessor].UnLockDeviceObject();
}


HRESULT RendererHLSLD3D9::ProcessVertexBuffer( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes )
{
	return vertexBufferProcessors[pProcessor].Process( pLocalData, Bytes);
}
HRESULT RendererHLSLD3D9::LockVertexBufferDeviceObject(ProcessorID pProcessor)
{
	return vertexBufferProcessors[pProcessor].LockDeviceObject();
}
HRESULT RendererHLSLD3D9::CopyToResourceVertexBuffer(ProcessorID pProcessor)
{
	return vertexBufferProcessors[pProcessor].CopyToResource();
}
HRESULT RendererHLSLD3D9::UnLockVertexBufferDeviceObject(ProcessorID pProcessor)
{
	return vertexBufferProcessors[pProcessor].UnLockDeviceObject();
}


HRESULT RendererHLSLD3D9::ProcessIndexBuffer( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes )
{
	return indexBufferProcessors[pProcessor].Process( pLocalData, Bytes);
}
HRESULT RendererHLSLD3D9::LockIndexBufferDeviceObject(ProcessorID pProcessor)
{
	return indexBufferProcessors[pProcessor].LockDeviceObject();
}
HRESULT RendererHLSLD3D9::CopyToResourceIndexBuffer(ProcessorID pProcessor)
{
	return indexBufferProcessors[pProcessor].CopyToResource();
}
HRESULT RendererHLSLD3D9::UnLockIndexBufferDeviceObject(ProcessorID pProcessor)
{
	return indexBufferProcessors[pProcessor].UnLockDeviceObject();
}



ProcessorID RendererHLSLD3D9::addTextureProcessor( TextureID& ppTexture, int g_SkipMips )
	{
		static int id=-1;
			CTextureProcessorD3D9 tp( this,ppTexture, g_SkipMips );

			id=textureProcessors.add(tp);
		return id;
	}

ProcessorID RendererHLSLD3D9::addVertexBufferProcessor( VertexBufferID& ppBuffer, int iSizeBytes,   void* pData)
	{
		static int id=-1;
			CVertexBufferProcessorD3D9 tl( this,ppBuffer, iSizeBytes, pData);
			id=vertexBufferProcessors.add(tl);
		return id;
	}

ProcessorID RendererHLSLD3D9::addIndexBufferProcessor( IndexBufferID& ppBuffer, int iSizeBytes, void* pData)
	{
		static int id=-1;
			CIndexBufferProcessorD3D9 tl( this, ppBuffer, iSizeBytes,4/*???*/, pData);
			id=indexBufferProcessors.add(tl);
		return id;
	}
#endif



void RendererHLSLD3D9::viewport(int x, int y, int viewportWidth, int viewportHeight){
    D3DVIEWPORT9 rightViewPort;
    rightViewPort.X	 = x;
    rightViewPort.Y	 = y;
    rightViewPort.Width  = viewportWidth;
    rightViewPort.Height = viewportHeight;
    rightViewPort.MinZ	 = 0.0f;
    rightViewPort.MaxZ	 = 1.0f;
    dev->SetViewport( &rightViewPort );
  m_viewport.X=x;
  m_viewport.Y=y;
  m_viewport.Width=viewportWidth;
  m_viewport.Height=viewportHeight;

}
#if 0
void	RendererHLSLD3D9::CreateTextureFromFile_Serial(  CHAR* szFileName,
						TextureID* ppTexture,
						//, void* pContext
						CPackedFile*		    g_PackFile,
						UINT g_SkipMips
						)
{
    LoaderID pLoader=addTextureLoader( szFileName, g_PackFile );
    ProcessorID pProcessor=addTextureProcessor( *ppTexture,// g_pResourceReuseCache
		 g_SkipMips );
	textureProcessors[pProcessor].SetID(pProcessor);

    void* pLocalData;
    SIZE_T Bytes;
    if( FAILED( RendererHLSLCg::LoadTexture(pLoader) ) )
	goto Error;
    if( FAILED( DecompressTexture( pLoader,&pLocalData, &Bytes ) ) )
	goto Error;
    if( FAILED( ProcessTexture( pLoader,pLocalData, Bytes ) ) )
	goto Error;
    if( FAILED( LockTextureDeviceObject(pProcessor) ) )
	goto Error;
    if( FAILED( CopyToResourceTexture(pProcessor) ) )
	goto Error;
    if( FAILED( UnLockTextureDeviceObject(pProcessor) ) )
	goto Error;

    goto Finish;

Error:
    SetProcessorResourceError(pProcessor);
Finish:
    DestroyProcessor(pProcessor);
    DestroyLoader(pLoader);
}

void	RendererHLSLD3D9::CreateVertexBuffer_Serial(
						VertexBufferID& ppBuffer,
						UINT iSizeBytes,
					    //__DWORD__ Usage, D3DFORMAT ibFormat, D3DPOOL Pool,
						void* pData
					    //void* pContext
						)
{
    LoaderID pLoader=addVertexBufferLoader();
    ProcessorID pProcessor=addVertexBufferProcessor( ppBuffer, iSizeBytes,  //ibFormat,
								   pData );
	vertexBufferProcessors[pProcessor].SetID(pProcessor);

    void* pLocalData;
    SIZE_T Bytes;
    if( FAILED( LoadVertexBuffer(pLoader) ) )
	goto Error;
    if( FAILED( DecompressVertexBuffer(pLoader, &pLocalData, &Bytes ) ) )
	goto Error;
    if( FAILED( ProcessVertexBuffer(pProcessor, pLocalData, Bytes ) ) )
	goto Error;
    if( FAILED( LockVertexBufferDeviceObject(pProcessor) ) )
	goto Error;
    if( FAILED( CopyToResourceVertexBuffer(pProcessor) ) )
	goto Error;
    if( FAILED( UnLockVertexBufferDeviceObject(pProcessor) ) )
	goto Error;

    goto Finish;

Error:
    SetProcessorResourceError(pProcessor);
Finish:
    DestroyProcessor(pProcessor);
    DestroyLoader(pLoader);
}

void	RendererHLSLD3D9::CreateIndexBuffer_Serial(
						IndexBufferID& ppBuffer,
						UINT iSizeBytes,
					    //__DWORD__ Usage, D3DFORMAT ibFormat, D3DPOOL Pool,
						void* pData
					    //void* pContext
						)
{
    LoaderID pLoader=addIndexBufferLoader();
    ProcessorID pProcessor=addIndexBufferProcessor( ppBuffer, iSizeBytes,  //ibFormat,
								   pData );
	indexBufferProcessors[pProcessor].SetID(pProcessor);

    void* pLocalData;
    SIZE_T Bytes;
    if( FAILED( LoadIndexBuffer(pLoader) ) )
	goto Error;
    if( FAILED( DecompressIndexBuffer(pLoader, &pLocalData, &Bytes ) ) )
	goto Error;
    if( FAILED( ProcessIndexBuffer(pProcessor, pLocalData, Bytes ) ) )
	goto Error;
    if( FAILED( LockIndexBufferDeviceObject(pProcessor) ) )
	goto Error;
    if( FAILED( CopyToResourceIndexBuffer(pProcessor) ) )
	goto Error;
    if( FAILED( UnLockIndexBufferDeviceObject(pProcessor) ) )
	goto Error;

    goto Finish;

Error:
    SetProcessorResourceError(pProcessor);
Finish:
    DestroyProcessor(pProcessor);
    DestroyLoader(pLoader);
}
#endif

float	RendererHLSLD3D9::GetMaxPointSpriteSize()
{
	//
    // If you want to know the max size that a point sprite can be set to,
	// and whether or not you can change the size of point sprites in hardware
	// by sending D3DFVF_PSIZE with the FVF, do this.
	//

	float fMaxPointSize = 0.0f;
	bool  bDeviceSupportsPSIZE = false;

    D3DCAPS9 d3dCaps;
    dev->GetDeviceCaps( &d3dCaps );

    fMaxPointSize = d3dCaps.MaxPointSize;

    if( d3dCaps.FVFCaps & D3DFVFCAPS_PSIZE )
        bDeviceSupportsPSIZE = true;
    else
        bDeviceSupportsPSIZE = false;
	return fMaxPointSize;
}

void	RendererHLSLD3D9::BeginPointSpriteRendering()
{
	//
	// Setting D3DRS_ZWRITEENABLE to FALSE makes the Z-Buffer read-only, which
	// helps remove graphical artifacts generated from  rendering a list of
	// particles that haven't been sorted by distance to the eye.
	//
    // Setting D3DRS_ALPHABLENDENABLE to TRUE allows particles, which overlap,
	// to alpha blend with each other correctly.
	//

    dev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

    dev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    dev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	//
	// Set up the render states for using point sprites...
	//

    dev->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );    // Turn on point sprites
    dev->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );    // Allow sprites to be scaled with distance
    dev->SetRenderState( D3DRS_POINTSIZE,     FtoDW(1.0) );  // Float value that specifies the size to use for point size computation in cases where point size is not specified for each vertex.
    dev->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(1.0f) ); // Float value that specifies the minimum size of point primitives. Point primitives are clamped to this size during rendering.
    dev->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.0f) ); // Default 1.0
    dev->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.0f) ); // Default 0.0
    dev->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.0f) ); // Default 0.0
}
void	RendererHLSLD3D9::EndPointSpriteRendering()
{
	//
    // Reset render states...
	//

    dev->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
    dev->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );

    dev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    dev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}

    BYTE* RendererHLSLD3D9::LockTexture(TextureID id, int m, int& pitch)
    {
		return textures[id].Lock(m,pitch);
	}
    void RendererHLSLD3D9::UnlockTexture(TextureID id, int m)
    {
		textures[id].Unlock(m);
	}
    BYTE* RendererHLSLD3D9::LockVertexBuffer(VertexBufferID id)
    {
		return vertexBuffers[id].Lock();
	}
    void RendererHLSLD3D9::UnLockVertexBuffer(VertexBufferID id)
    {
		vertexBuffers[id].Unlock();
	}
    BYTE* RendererHLSLD3D9::LockIndexBuffer(IndexBufferID id)
    {
		return indexBuffers[id].Lock();
	}
    void RendererHLSLD3D9::UnLockIndexBuffer(IndexBufferID id)
    {
		indexBuffers[id].Unlock();
	}

	void RendererHLSLD3D9::GetLevelDesc(TextureID id, unsigned int level,SURFACE_DESC* sDesc)
	{
		sDesc->Width=textures[id].width;
		sDesc->Height=textures[id].height;
		int i=0;
		while(i<level)
		{
		sDesc->Width = sDesc->Width >> 1;
        sDesc->Height = sDesc->Height >> 1;
		i++;
		}
	}
	void RendererHLSLD3D9::GetDesc(VertexBufferID id,VERTEXBUFFER_DESC* sDesc)
	{
		sDesc->Size=vertexBuffers[id].size;
	}
	void RendererHLSLD3D9::GetDesc(IndexBufferID id,INDEXBUFFER_DESC* sDesc)
	{
		//sDesc->Size=indexBuffers[id].nIndices*indexBuffers[id].indexSize;
		sDesc->Format=indexBuffers[id].indexSize;
		sDesc->indexSize=indexBuffers[id].indexSize;
		sDesc->nIndices=indexBuffers[id].nIndices;
	}

	unsigned int RendererHLSLD3D9::GetLevelCount(TextureID id)
	{
				return textures[id].mipMapLevels;
	}



/*
    // Enable alpha testing for transparent blits
    g_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    g_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
    g_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

    // Enable alpha blending for the smoke particles
    g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
*/

void RendererHLSLD3D9::changeBlendStateVrtl(const BlendStateID blendState, const unsigned int sampleMask)
{
    if (blendState != currentBlendState){
        if (blendState == BS_NONE || !blendStates[blendState].blendEnable){
            if (currentBlendEnable){
                dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
                currentBlendEnable = false;
            }
        } else {
            if (blendStates[blendState].blendEnable){
                if (!currentBlendEnable){
                    dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
                    currentBlendEnable = true;
                }
                if (blendStates[blendState].srcFactorRGB != currentSrcFactorRGB){
                    dev->SetRenderState(D3DRS_SRCBLEND, currentSrcFactorRGB = blendStates[blendState].srcFactorRGB);
                }
                if (blendStates[blendState].dstFactorRGB != currentDstFactorRGB){
                    dev->SetRenderState(D3DRS_DESTBLEND, currentDstFactorRGB = blendStates[blendState].dstFactorRGB);
                }
                if (blendStates[blendState].blendModeRGB != currentBlendModeRGB){
                    dev->SetRenderState(D3DRS_BLENDOP, currentBlendModeRGB = blendStates[blendState].blendModeRGB);
                }
                if (blendStates[blendState].srcFactorAlpha != currentSrcFactorAlpha){
                    dev->SetRenderState(D3DRS_SRCBLENDALPHA, currentSrcFactorAlpha = blendStates[blendState].srcFactorRGB);
                }
                if (blendStates[blendState].dstFactorAlpha != currentDstFactorAlpha){
                    dev->SetRenderState(D3DRS_DESTBLENDALPHA, currentDstFactorAlpha = blendStates[blendState].dstFactorRGB);
                }
                if (blendStates[blendState].blendModeAlpha != currentBlendModeAlpha){
                    dev->SetRenderState(D3DRS_BLENDOPALPHA, currentBlendModeAlpha = blendStates[blendState].blendModeRGB);
                }
            }
        }

        int mask = ALL;
        if (blendState != BS_NONE){
            mask = blendStates[blendState].mask;
        }

        if (mask != currentMask){
            dev->SetRenderState(D3DRS_COLORWRITEENABLE, currentMask = mask);
        }

        currentBlendState = blendState;
    }
    if (sampleMask != currentSampleMask){
        dev->SetRenderState(D3DRS_MULTISAMPLEMASK, sampleMask);
        currentSampleMask = sampleMask;
    }
}

void RendererHLSLD3D9::changeAlphaState(const AlphaStateID blendState)
{
	if (blendState != currentAlphaState)
	{
		if (blendState == BS_NONE || !(alphaStates[blendState]).blendEnable)
		{
			if (currentAlphaEnable)
			{
				V(dev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE));
				currentAlphaEnable = false;
			}
		}
		else
		{
			if ((alphaStates[blendState]).blendEnable)
			{
				if (!currentAlphaEnable)
				{
					V(dev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE));
    V(dev->SetRenderState(D3DRS_ALPHAREF, 0x08));
    V(dev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL));

				// Alpha color
	//V(dev->SetRenderState (D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA (pR, pG, pB, pA)));

					currentAlphaEnable = true;
				}
			}
		}
		currentAlphaState = blendState;
	}}
AlphaStateID RendererHLSLD3D9::addAlphaState(
		const int func,
		const float ref

	)
{
	AlphaStateD3D9 blendState;
    blendState.blendEnable = true;
	blendState.alphaFunc=GEQUAL;
	alphaStates.push_back(blendState);
	return alphaStates.size()-1;
}
unsigned int RendererHLSLD3D9::drawArrays(const Primitives primitives, const int firstVertex, const int nVertices){
	if(!d3d9Prim[primitives]) return;
	dev->DrawPrimitive(d3d9Prim[primitives], firstVertex, nVertices);
}

unsigned int RendererHLSLD3D9::drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices){
	if(!d3d9Prim[primitives]) return;
	int indexSize = indexBuffers[currentIndexBuffer].indexSize;

	dev->DrawIndexedPrimitive(d3d9Prim[primitives], 0, firstVertex, nVertices, firstIndex, getPrimitiveCount(primitives, nIndices));
}
unsigned int RendererHLSLD3D9::drawPrimitives(	const Primitives primitives,
				const unsigned int firstv, const unsigned int countv,
				const unsigned int firsti, const unsigned int counti,
				const ubyte* vbdata, const unsigned int vbsize,
				const ubyte* ibdata, const unsigned int ibsize)
{
	LOG_FNLN;
	LOG_PRINT("IMPLEMENT\n");
}
#endif
#endif

