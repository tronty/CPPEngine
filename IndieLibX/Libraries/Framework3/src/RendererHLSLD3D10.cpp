/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
  Copyright (c) 2015 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/RendererHLSLD3D10.h>
#ifdef __Framework3D3D10_H__

static DXGI_FORMAT formatsd3d10[] =
{
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_R8_UNORM,
	DXGI_FORMAT_R8G8_UNORM,
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R16_UNORM,
	DXGI_FORMAT_R16G16_UNORM,
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_UNORM,
	DXGI_FORMAT_R8_SNORM,
	DXGI_FORMAT_R8G8_SNORM,
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R8G8B8A8_SNORM,
	DXGI_FORMAT_R16_SNORM,
	DXGI_FORMAT_R16G16_SNORM,
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_SNORM,
	DXGI_FORMAT_R16_FLOAT,
	DXGI_FORMAT_R16G16_FLOAT,
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R32_FLOAT,
	DXGI_FORMAT_R32G32_FLOAT,
	DXGI_FORMAT_R32G32B32_FLOAT,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	DXGI_FORMAT_R16_SINT,
	DXGI_FORMAT_R16G16_SINT,
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_SINT,
	DXGI_FORMAT_R32_SINT,
	DXGI_FORMAT_R32G32_SINT,
	DXGI_FORMAT_R32G32B32_SINT,
	DXGI_FORMAT_R32G32B32A32_SINT,
	DXGI_FORMAT_R16_UINT,
	DXGI_FORMAT_R16G16_UINT,
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R16G16B16A16_UINT,
	DXGI_FORMAT_R32_UINT,
	DXGI_FORMAT_R32G32_UINT,
	DXGI_FORMAT_R32G32B32_UINT,
	DXGI_FORMAT_R32G32B32A32_UINT,
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
	DXGI_FORMAT_R11G11B10_FLOAT,
	DXGI_FORMAT_B5G6R5_UNORM,
	DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_D16_UNORM,
	DXGI_FORMAT_D24_UNORM_S8_UINT,
	DXGI_FORMAT_D24_UNORM_S8_UINT,
	DXGI_FORMAT_D32_FLOAT,
	DXGI_FORMAT_BC1_UNORM,
	DXGI_FORMAT_BC2_UNORM,
	DXGI_FORMAT_BC3_UNORM,
	DXGI_FORMAT_BC4_UNORM,
	DXGI_FORMAT_BC5_UNORM,
}
;

ID3D10Device* RendererHLSLD3D10::RendererHLSLD3D10::GetDevice()=0;

RendererHLSLD3D10::RendererHLSLD3D10(
			bool fullScreen,
int argc, char *argv[]
) : mOk(false), RendererHLSLCg(argc, argv)
{
	HRESULT hr = E_FAIL;
	RendererHLSLD3D10::GetDevice()=0;

    		width=STX_Service::GetWindowInstance()->Getwidth();
    		height=STX_Service::GetWindowInstance()->Getheight();

		SHADING_LANGUAGE_VERSION_MAJOR=4;
		SHADING_LANGUAGE_VERSION_MINOR=0;
#if 0
		ID3D10Device *d3ddev=0;
		UINT createDeviceFlags = 0;
		#ifdef _DEBUG
		createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
		#endif
		D3D10_DRIVER_TYPE driverTypes[] =
		{
			D3D10_DRIVER_TYPE_HARDWARE,
		}
		;
		UINT numDriverTypes = sizeof( driverTypes ) / sizeof( driverTypes[0] );
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory( &sd, sizeof( sd ) );
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = STX_Service::GetWindowInstance()->GetHWND();
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
		{
			m_driverType = driverTypes[driverTypeIndex];
			#if 1//ndef USE_D3D10_1
			hr = D3D10CreateDeviceAndSwapChain( 0, m_driverType, 0, createDeviceFlags,
			D3D10_SDK_VERSION, &sd, &m_pSwapChain, &d3ddev );
			#else
			hr = D3D10CreateDeviceAndSwapChain1( 0, m_driverType, 0, createDeviceFlags,
			fl10,
			D3D10_1_SDK_VERSION, &sd, &m_pSwapChain, &d3ddev );
			#endif
			if( SUCCEEDED( hr ) ) break;
		}
#else
	DXGI_SWAP_CHAIN_DESC scDesc;
	scDesc.BufferDesc.Width = STX_Service::GetWindowInstance()->GetWidth();
	scDesc.BufferDesc.Height = STX_Service::GetWindowInstance()->GetHeight();
	scDesc.BufferDesc.RefreshRate.Numerator = 60;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = 1;
	scDesc.OutputWindow = STX_Service::GetWindowInstance()->GetHWND();
	scDesc.Windowed = true;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scDesc.Flags = 0;
	UINT createDeviceFlags = 0;
#if defined(DEBUG)  || defined(_DEBUG)
	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif
	hr=D3D10CreateDeviceAndSwapChain(0, D3D10_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, D3D10_SDK_VERSION, &scDesc, &m_pSwapChain, &RendererHLSLD3D10::GetDevice());
#endif
		if( FAILED( hr ) ) return;
		hr = m_pSwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), ( LPVOID* )&m_pBackBuffer );
		if( FAILED( hr ) )
		{
			LOG_PRINT("g_pSwapChain->GetBuffer failed\n");
			return;
		}
		hr = RendererHLSLD3D10::GetDevice()->CreateRenderTargetView( m_pBackBuffer, 0, &RTV );
		LOG_PRINT("RTV %x %d\n",RTV,RTV);
		if( FAILED( hr ) )
		{
			LOG_PRINT("Creation of rendertarget failed\n");
			return;
		}
		D3D10_TEXTURE2D_DESC descDepth;
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D10_USAGE_DEFAULT;
		descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = RendererHLSLD3D10::GetDevice()->CreateTexture2D( &descDepth, 0, &m_pDepthBuffer );
		if( FAILED( hr ) )
		{
			LOG_PRINT("Creation of Depthstencil failed\n");
			return;
		}
		D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = RendererHLSLD3D10::GetDevice()->CreateDepthStencilView( m_pDepthBuffer, &descDSV, &DSV );
		if( FAILED( hr ) )
		{
			LOG_PRINT("Creation of DepthstencilView failed\n");
			return;
		}
		RendererHLSLD3D10::GetDevice()->OMSetRenderTargets( 1, &RTV, DSV );
		D3D10_VIEWPORT vp;
		vp.Width = width;
		vp.Height = height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		RendererHLSLD3D10::GetDevice()->RSSetViewports( 1, &vp );
		eventQuery = 0;
		nImageUnits = 16;
		maxAnisotropic = 16;
		nMRTs = 8;
		if (nMRTs > MAX_MRTS) nMRTs = MAX_MRTS;
		plainShader = SHADER_NONE;
		plainVF = VF_NONE;
		rollingVB = VB_NONE;
		rollingVBOffset = 0;
		ZERO		    = D3D10_BLEND_ZERO;
		ONE		    = D3D10_BLEND_ONE;
		SRC_COLOR	    = D3D10_BLEND_SRC_COLOR;
		ONE_MINUS_SRC_COLOR = D3D10_BLEND_INV_SRC_COLOR;
		DST_COLOR	    = D3D10_BLEND_DEST_COLOR;
		ONE_MINUS_DST_COLOR = D3D10_BLEND_INV_DEST_COLOR;
		SRC_ALPHA	    = D3D10_BLEND_SRC_ALPHA;
		ONE_MINUS_SRC_ALPHA = D3D10_BLEND_INV_SRC_ALPHA;
		DST_ALPHA	    = D3D10_BLEND_DEST_ALPHA;
		ONE_MINUS_DST_ALPHA = D3D10_BLEND_INV_DEST_ALPHA;
		SRC_ALPHA_SATURATE  = D3D10_BLEND_SRC_ALPHA_SAT;
		BM_ADD		    = D3D10_BLEND_OP_ADD;
		BM_SUBTRACT	    = D3D10_BLEND_OP_SUBTRACT;
		BM_REVERSE_SUBTRACT = D3D10_BLEND_OP_REV_SUBTRACT;
		BM_MIN		    = D3D10_BLEND_OP_MIN;
		BM_MAX		    = D3D10_BLEND_OP_MAX;
		NEVER	 = D3D10_COMPARISON_NEVER;
		LESS	 = D3D10_COMPARISON_LESS;
		EQUAL	 = D3D10_COMPARISON_EQUAL;
		LEQUAL	 = D3D10_COMPARISON_LESS_EQUAL;
		GREATER  = D3D10_COMPARISON_GREATER;
		NOTEQUAL = D3D10_COMPARISON_NOT_EQUAL;
		GEQUAL	 = D3D10_COMPARISON_GREATER_EQUAL;
		ALWAYS	 = D3D10_COMPARISON_ALWAYS;
		KEEP	 = D3D10_STENCIL_OP_KEEP;
		SET_ZERO = D3D10_STENCIL_OP_ZERO;
		REPLACE  = D3D10_STENCIL_OP_REPLACE;
		INVERT	 = D3D10_STENCIL_OP_INVERT;
		INCR	 = D3D10_STENCIL_OP_INCR;
		DECR	 = D3D10_STENCIL_OP_DECR;
		INCR_SAT = D3D10_STENCIL_OP_INCR_SAT;
		DECR_SAT = D3D10_STENCIL_OP_DECR_SAT;
		CULL_NONE  = D3D10_CULL_NONE;
		CULL_BACK  = D3D10_CULL_BACK;
		CULL_FRONT = D3D10_CULL_FRONT;
		SOLID = D3D10_FILL_SOLID;
		WIREFRAME = D3D10_FILL_WIREFRAME;
		setD3Ddefaults();
		resetToDefaults();

		for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++){
			selectedTexturesVS[i] = TEXTURE_NONE;
			selectedTexturesGS[i] = TEXTURE_NONE;
			selectedTexturesPS[i] = TEXTURE_NONE;
			selectedTextureSlicesVS[i] = NO_SLICE;
			selectedTextureSlicesGS[i] = NO_SLICE;
			selectedTextureSlicesPS[i] = NO_SLICE;
		}

		for (unsigned int i = 0; i < MAX_SAMPLERSTATE; i++){
			selectedSamplerStatesVS[i] = SS_NONE;
			selectedSamplerStatesGS[i] = SS_NONE;
			selectedSamplerStatesPS[i] = SS_NONE;
		}

				//defaultFont = addFont(FONT_PATH"Future.dds", FONT_PATH"Future.font", linearClamp);
LOG_PRINT("RendererHLSLD3D10\n");
/*
		SetVersion("");
		SetVendor("");
		SetRenderer("");
		SetAntialiasing(false);
		SetMaxTextureSize(4096);
		SetVertexShaderVersion(3);
		SetPixelShaderVersion(3);
*/
		if(!FAILED(hr)) mOk=true;
	}

RendererHLSLD3D10::~RendererHLSLD3D10(){
	RendererHLSLD3D10::GetDevice()->ClearState();

	if (eventQuery) eventQuery->Release();


/*
	releaseFrameBufferSurfaces();
*/

	// Delete shaders
	for (unsigned int i = 0; i < shaders.getCount(); i++){
		if (shaders[i].vertexShader  ) shaders[i].vertexShader->Release();
		if (shaders[i].geometryShader) shaders[i].geometryShader->Release();
		if (shaders[i].pixelShader   ) shaders[i].pixelShader->Release();
		if (shaders[i].inputSignature) shaders[i].inputSignature->Release();

		for (unsigned int k = 0; k < shaders[i].nVSCBuffers; k++){
			shaders[i].vsConstants[k]->Release();
			delete shaders[i].vsConstMem[k];
		}
		for (unsigned int k = 0; k < shaders[i].nGSCBuffers; k++){
			shaders[i].gsConstants[k]->Release();
			delete shaders[i].gsConstMem[k];
		}
		for (unsigned int k = 0; k < shaders[i].nPSCBuffers; k++){
			shaders[i].psConstants[k]->Release();
			delete shaders[i].psConstMem[k];
		}
		delete shaders[i].vsConstants;
		delete shaders[i].gsConstants;
		delete shaders[i].psConstants;
		delete shaders[i].vsConstMem;
		delete shaders[i].gsConstMem;
		delete shaders[i].psConstMem;

		/*
		for (unsigned int k = 0; k < shaders[i].nConstants; k++){
			delete shaders[i].constants[k].name;
		}
		*/
		//delete shaders[i].constants;

		/*
		for (unsigned int k = 0; k < shaders[i].nTextures; k++){
			delete shaders[i].textures[k].name;
		}
		*/
		//delete shaders[i].textures;

		/*
		for (unsigned int k = 0; k < shaders[i].nSamplers; k++){
			delete shaders[i].samplers[k].name;
		}
		*/
		//delete shaders[i].samplers;

		delete shaders[i].vsDirty;
		delete shaders[i].gsDirty;
		delete shaders[i].psDirty;
	}

    // Delete vertex formats
	for (unsigned int i = 0; i < vertexFormats.getCount(); i++){
		if (vertexFormats[i].inputLayout) vertexFormats[i].inputLayout->Release();
	}

    // Delete vertex buffers
	for (unsigned int i = 0; i < vertexBuffers.getCount(); i++){
		if (vertexBuffers[i].vertexBuffer) vertexBuffers[i].vertexBuffer->Release();
	}

	// Delete index buffers
	for (unsigned int i = 0; i < indexBuffers.getCount(); i++){
		if (indexBuffers[i].indexBuffer) indexBuffers[i].indexBuffer->Release();
	}

	// Delete samplerstates
	for (unsigned int i = 0; i < samplerStates.getCount(); i++){
		if (samplerStates[i].samplerState) samplerStates[i].samplerState->Release();
	}

	// Delete blendstates
	for (unsigned int i = 0; i < blendStates.getCount(); i++){
		if (blendStates[i].blendState) blendStates[i].blendState->Release();
	}

	// Delete depthstates
	for (unsigned int i = 0; i < depthStates.getCount(); i++){
		if (depthStates[i].dsState) depthStates[i].dsState->Release();
	}

	// Delete rasterizerstates
	for (unsigned int i = 0; i < rasterizerStates.getCount(); i++){
		if (rasterizerStates[i].rsState) rasterizerStates[i].rsState->Release();
	}

	// Delete textures
	for (unsigned int i = 0; i < textures.getCount(); i++){
		removeTexture(i);
	}

//	if (rollingVB) rollingVB->Release();
}

void RendererHLSLD3D10::resetVrtl(const unsigned int flags){
	reset0(flags);
	if (flags & RESET_TEX){
		for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++){
			selectedTexturesVS[i] = TEXTURE_NONE;
			selectedTexturesGS[i] = TEXTURE_NONE;
			selectedTexturesPS[i] = TEXTURE_NONE;
			selectedTextureSlicesVS[i] = NO_SLICE;
			selectedTextureSlicesGS[i] = NO_SLICE;
			selectedTextureSlicesPS[i] = NO_SLICE;
		}
	}

	if (flags & RESET_SS){
		for (unsigned int i = 0; i < MAX_SAMPLERSTATE; i++){
			selectedSamplerStatesVS[i] = SS_NONE;
			selectedSamplerStatesGS[i] = SS_NONE;
			selectedSamplerStatesPS[i] = SS_NONE;
		}
	}
}

void RendererHLSLD3D10::resetToDefaults(){
	reset0ToDefaults();
	for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++){
		currentTexturesVS[i] = TEXTURE_NONE;
		currentTexturesGS[i] = TEXTURE_NONE;
		currentTexturesPS[i] = TEXTURE_NONE;
		currentTextureSlicesVS[i] = NO_SLICE;
		currentTextureSlicesGS[i] = NO_SLICE;
		currentTextureSlicesPS[i] = NO_SLICE;
	}

	for (unsigned int i = 0; i < MAX_SAMPLERSTATE; i++){
		currentSamplerStatesVS[i] = SS_NONE;
		currentSamplerStatesGS[i] = SS_NONE;
		currentSamplerStatesPS[i] = SS_NONE;
	}

	// TODO: Fix ...
	currentRasterizerState = -2;


/*
	currentDepthRT = FB_DEPTH;
*/
}

void RendererHLSLD3D10::setD3Ddefaults(){
/*
	// Set some of my preferred defaults
	RendererHLSLD3D10::GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	RendererHLSLD3D10::GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	RendererHLSLD3D10::GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	if (maxAnisotropic > 1){
		for (unsigned int i = 0; i < nImageUnits; i++){
			RendererHLSLD3D10::GetDevice()->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, maxAnisotropic);
		}
	}
*/
}

/*bool createRenderTarget(LPDIRECT3DDEVICE9 RendererHLSLD3D10::GetDevice(), Texture &tex){
	if (isDepthFormat(tex.format)){
		if (tex.surfaces == 0) tex.surfaces = new LPDIRECT3DSURFACE9;
		if (RendererHLSLD3D10::GetDevice()->CreateDepthStencilSurface(tex.width, tex.height, formats[tex.format], D3DMULTISAMPLE_NONE, 0, FALSE, tex.surfaces, 0) != D3D_OK){
			delete tex.surfaces;

			LOG_PRINT("Couldn't create depth surface");
			return false;
		}
	} else {
		bool mipMapped = (tex.filter.mipFilter != D3DTEXF_NONE);

		if (tex.flags & CUBEMAP){
			if (RendererHLSLD3D10::GetDevice()->CreateCubeTexture(tex.width, mipMapped? 0 : 1, tex.usage, formats[tex.format], D3DPOOL_DEFAULT, (LPDIRECT3DCUBETEXTURE9 *) &tex.texture, 0) != D3D_OK){
				LOG_PRINT("Couldn't create render target");
				return false;
			}

			if (tex.surfaces == 0) tex.surfaces = new LPDIRECT3DSURFACE9[6];
			for (unsigned int i = 0; i < 6; i++){
				((LPDIRECT3DCUBETEXTURE9) tex.texture)->GetCubeMapSurface((D3DCUBEMAP_FACES) i, 0, &tex.surfaces[i]);
			}
		} else {
			if (RendererHLSLD3D10::GetDevice()->CreateTexture(tex.width, tex.height, mipMapped? 0 : 1, tex.usage, formats[tex.format], D3DPOOL_DEFAULT, (LPDIRECT3DTEXTURE9 *) &tex.texture, 0) != D3D_OK){
				LOG_PRINT("Couldn't create render target");
				return false;
			}
			if (tex.surfaces == 0) tex.surfaces = new LPDIRECT3DSURFACE9;
			((LPDIRECT3DTEXTURE9) tex.texture)->GetSurfaceLevel(0, tex.surfaces);
		}
	}

	return true;
}
*/

/*bool RendererHLSLD3D10::resetDevice(){
	for (unsigned int i = 0; i < textures.getCount(); i++){
		if (textures[i].surfaces){
			int n = (textures[i].flags & CUBEMAP)? 6 : 1;

			if (textures[i].texture) textures[i].texture->Release();
			for (int k = 0; k < n; k++){
				textures[i].surfaces[k]->Release();
			}
		}
	}

	if (!releaseFrameBufferSurfaces()) return false;

	if (RendererHLSLD3D10::GetDevice()->Reset(&d3dpp) != D3D_OK){
		LOG_PRINT("Device reset failed");
		return false;
	}

	if (!createFrameBufferSurfaces()) return false;

	resetToDefaults();
	setD3Ddefaults();

	for (unsigned int i = 0; i < textures.getCount(); i++){
		if (textures[i].surfaces){
			createRenderTarget(RendererHLSLD3D10::GetDevice(), textures[i]);
		}
	}

	return true;
}*/

TextureID RendererHLSLD3D10::addTexture(ID3D10Resource *resource, unsigned int flags){
	TextureD3D10 tex;
	stx_memset(&tex, 0, sizeof(tex));

	tex.texture = resource;
	tex.srv = createSRV(resource);
	tex.flags = flags;

	D3D10_RESOURCE_DIMENSION type;
	resource->GetType(&type);

	switch (type){
		case D3D10_RESOURCE_DIMENSION_TEXTURE1D:
			D3D10_TEXTURE1D_DESC desc1d;
			((ID3D10Texture1D *) resource)->GetDesc(&desc1d);

			tex.width  = desc1d.Width;
			tex.height = 1;
			tex.depth  = 1;
			break;
		case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
			D3D10_TEXTURE2D_DESC desc2d;
			((ID3D10Texture2D *) resource)->GetDesc(&desc2d);

			tex.width  = desc2d.Width;
			tex.height = desc2d.Height;
			tex.depth  = 1;
			break;
		case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
			D3D10_TEXTURE3D_DESC desc3d;
			((ID3D10Texture3D *) resource)->GetDesc(&desc3d);

			tex.width  = desc3d.Width;
			tex.height = desc3d.Height;
			tex.depth  = desc3d.Depth;
			break;
	}

	return textures.add(tex);
}


TextureID RendererHLSLD3D10::addTexture(Image3 &img, const SamplerStateID samplerState, unsigned int flags){
	TextureD3D10 tex;
	stx_memset(&tex, 0, sizeof(tex));

	switch (img.getFormat()){
		case FORMAT_RGB8:
			img.convert(FORMAT_RGBA8);
//			img.convert(FORMAT_RGB10A2);
			break;
		case FORMAT_RGB16:
			img.convert(FORMAT_RGBA16);
			break;
		case FORMAT_RGB16F:
			img.convert(FORMAT_RGBA16F);
			break;
		case FORMAT_RGB32F:
			img.convert(FORMAT_RGBA32F);
			break;
	}

	FORMAT format = img.getFormat();
	unsigned int nMipMaps = img.getMipMapCount();
	unsigned int nSlices = img.isCube()? 6 : 1;
	unsigned int arraySize = img.getArraySize();

	static D3D10_SUBRESOURCE_DATA texData[1024];
	D3D10_SUBRESOURCE_DATA *dest = texData;
	//for (unsigned int n = 0; n < arraySize; n++){
		for (unsigned int k = 0; k < nSlices; k++){
			for (unsigned int i = 0; i < nMipMaps; i++){
				unsigned int pitch, slicePitch;
				if (isCompressedFormat(format)){
					pitch = ((img.getWidth(i) + 3) >> 2) * getBytesPerBlock(format);
					slicePitch = pitch * ((img.getHeight(i) + 3) >> 2);
				} else {
					pitch = img.getWidth(i) * getBytesPerPixel(format);
					slicePitch = pitch * img.getHeight(i);
				}

				dest->pSysMem = img.getPixels(i/*, n*/) + k * slicePitch;
				dest->SysMemPitch = pitch;
				dest->SysMemSlicePitch = slicePitch;
				dest++;
			}
		}
	//}

	tex.texFormat = formatsd3d10[format];
	if (flags & SRGB){
		// Change to the matching sRGB format
		switch (tex.texFormat){
			case DXGI_FORMAT_R8G8B8A8_UNORM: tex.texFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; break;
			case DXGI_FORMAT_BC1_UNORM: tex.texFormat = DXGI_FORMAT_BC1_UNORM_SRGB; break;
			case DXGI_FORMAT_BC2_UNORM: tex.texFormat = DXGI_FORMAT_BC2_UNORM_SRGB; break;
			case DXGI_FORMAT_BC3_UNORM: tex.texFormat = DXGI_FORMAT_BC3_UNORM_SRGB; break;
		}
	}

	HRESULT hr;
	if (img.is1D()){
		D3D10_TEXTURE1D_DESC desc;
		desc.Width  = img.getWidth();
		desc.Format = tex.texFormat;
		desc.MipLevels = nMipMaps;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		desc.ArraySize = 1;
		desc.MiscFlags = 0;
		if((img.getFormat()>=FORMAT_R16F) && (FORMAT_RGBA32F<=img.getFormat()))
		{
			desc.Usage = D3D10_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		}
		else
		{
			desc.Usage = D3D10_USAGE_IMMUTABLE;
			desc.CPUAccessFlags = 0;
		}
		hr = RendererHLSLD3D10::GetDevice()->CreateTexture1D(&desc, texData, (ID3D10Texture1D **) &tex.texture);
	} else if (img.is2D() || img.isCube()){
		D3D10_TEXTURE2D_DESC desc;
		desc.Width  = img.getWidth();
		desc.Height = img.getHeight();
		desc.Format = tex.texFormat;
		desc.MipLevels = nMipMaps;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		if((img.getFormat()>=FORMAT_R16F) && (FORMAT_RGBA32F<=img.getFormat()))
		{
			desc.Usage = D3D10_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		}
		else
		{
			desc.Usage = D3D10_USAGE_IMMUTABLE;
			desc.CPUAccessFlags = 0;
		}
		if (img.isCube()){
			desc.ArraySize = 6 * arraySize;
			desc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;
		} else {
			desc.ArraySize = 1;
			desc.MiscFlags = 0;
		}

		hr = RendererHLSLD3D10::GetDevice()->CreateTexture2D(&desc, texData, (ID3D10Texture2D **) &tex.texture);
	} else if (img.is3D()){
		D3D10_TEXTURE3D_DESC desc;
		desc.Width  = img.getWidth();
		desc.Height = img.getHeight();
		desc.Depth  = img.getDepth();
		desc.Format = tex.texFormat;
		desc.MipLevels = nMipMaps;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		desc.MiscFlags = 0;
		if((img.getFormat()>=FORMAT_R16F) && (FORMAT_RGBA32F<=img.getFormat()))
		{
			desc.Usage = D3D10_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		}
		else
		{
			desc.Usage = D3D10_USAGE_IMMUTABLE;
			desc.CPUAccessFlags = 0;
		}
		hr = RendererHLSLD3D10::GetDevice()->CreateTexture3D(&desc, texData, (ID3D10Texture3D **) &tex.texture);
	}

	if (FAILED(hr)){
		LOG_PRINT("Couldn't create texture");
		return TEXTURE_NONE;
	}

	tex.srvFormat = tex.texFormat;
	tex.srv = createSRV(tex.texture, tex.srvFormat);

	return textures.add(tex);
}

TextureID RendererHLSLD3D10::addRenderTargetVrtl(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags){
	TextureD3D10 tex;
	stx_memset(&tex, 0, sizeof(tex));

	tex.width  = width;
	tex.height = height;
	tex.depth  = depth;
	tex.arraySize = arraySize;
	tex.flags  = flags;
	tex.texFormat = formatsd3d10[format];

	if (depth == 1){
		D3D10_TEXTURE2D_DESC desc;
		desc.Width  = width;
		desc.Height = height;
		desc.Format = tex.texFormat;
		desc.MipLevels = mipMapCount;
		desc.SampleDesc.Count = msaaSamples;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
		if (flags & CUBEMAP){
			desc.ArraySize = 6;
			desc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;
		} else {
			desc.ArraySize = arraySize;
			desc.MiscFlags = 0;
		}
		if (flags & USE_MIPGEN){
			desc.MiscFlags |= D3D10_RESOURCE_MISC_GENERATE_MIPS;
		}
		if((format>=FORMAT_R16F) && (FORMAT_RGBA32F<=format))
		{
			desc.Usage = D3D10_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		}
		else
		{
			desc.Usage = D3D10_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
		}
		if (FAILED(RendererHLSLD3D10::GetDevice()->CreateTexture2D(&desc, 0, (ID3D10Texture2D **) &tex.texture))){
			LOG_PRINT("Couldn't create render target");
			return TEXTURE_NONE;
		}
	} else {
		D3D10_TEXTURE3D_DESC desc;
		desc.Width  = width;
		desc.Height = height;
		desc.Depth  = depth;
		desc.Format = tex.texFormat;
		desc.MipLevels = mipMapCount;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
		desc.MiscFlags = 0;
		if (flags & USE_MIPGEN){
			desc.MiscFlags |= D3D10_RESOURCE_MISC_GENERATE_MIPS;
		}
		if((format>=FORMAT_R16F) && (FORMAT_RGBA32F<=format))
		{
			desc.Usage = D3D10_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		}
		else
		{
			desc.Usage = D3D10_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
		}
		if (FAILED(RendererHLSLD3D10::GetDevice()->CreateTexture3D(&desc, 0, (ID3D10Texture3D **) &tex.texture))){
			LOG_PRINT("Couldn't create render target");
			return TEXTURE_NONE;
		}
	}


	tex.srvFormat = tex.texFormat;
	tex.rtvFormat = tex.texFormat;
	tex.srv = createSRV(tex.texture, tex.srvFormat);
	tex.rtv = createRTV(tex.texture, tex.rtvFormat);

	int sliceCount = (depth == 1)? arraySize : depth;

	if (flags & SAMPLE_SLICES){
		tex.srvArray = new ID3D10ShaderResourceView *[sliceCount];
		for (int i = 0; i < sliceCount; i++){
			tex.srvArray[i] = createSRV(tex.texture, tex.srvFormat, i);
		}
	}

	if (flags & RENDER_SLICES){
		tex.rtvArray = new ID3D10RenderTargetView *[sliceCount];

		for (int i = 0; i < sliceCount; i++){
			tex.rtvArray[i] = createRTV(tex.texture, tex.rtvFormat, i);
		}
	}

	return textures.add(tex);
}

TextureID RendererHLSLD3D10::addRenderDepthVrtl(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags){
	TextureD3D10 tex;
	stx_memset(&tex, 0, sizeof(tex));

	tex.width  = width;
	tex.height = height;
	tex.depth  = 1;
	tex.arraySize = arraySize;
	tex.flags  = flags;
	tex.texFormat = tex.dsvFormat = formatsd3d10[format];

	D3D10_TEXTURE2D_DESC desc;
	desc.Width  = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = msaaSamples;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	if (flags & CUBEMAP){
		desc.ArraySize = 6;
		desc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;
	} else {
		desc.ArraySize = arraySize;
		desc.MiscFlags = 0;
	}

	if (flags & SAMPLE_DEPTH){
		switch (tex.dsvFormat){
			case DXGI_FORMAT_D16_UNORM:
				tex.texFormat = DXGI_FORMAT_R16_TYPELESS;
				tex.srvFormat = DXGI_FORMAT_R16_UNORM;
				break;
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
				tex.texFormat = DXGI_FORMAT_R24G8_TYPELESS;
				tex.srvFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;
			case DXGI_FORMAT_D32_FLOAT:
				tex.texFormat = DXGI_FORMAT_R32_TYPELESS;
				tex.srvFormat = DXGI_FORMAT_R32_FLOAT;
				break;
		}
		desc.BindFlags |= D3D10_BIND_SHADER_RESOURCE;
	}
	desc.Format = tex.texFormat;

	if (FAILED(RendererHLSLD3D10::GetDevice()->CreateTexture2D(&desc, 0, (ID3D10Texture2D **) &tex.texture))){
		LOG_PRINT("Couldn't create depth target");
		return TEXTURE_NONE;
	}

	tex.dsv = createDSV(tex.texture, tex.dsvFormat);
	if (flags & RENDER_SLICES){
		tex.dsvArray = new ID3D10DepthStencilView *[arraySize];
		for (int i = 0; i < arraySize; i++){
			tex.dsvArray[i] = createDSV(tex.texture, tex.dsvFormat, i);
		}
	}

	if (flags & SAMPLE_DEPTH){
		tex.srv = createSRV(tex.texture, tex.srvFormat);

		if (flags & SAMPLE_SLICES){
			tex.srvArray = new ID3D10ShaderResourceView *[arraySize];
			for (int i = 0; i < arraySize; i++){
				tex.srvArray[i] = createSRV(tex.texture, tex.srvFormat, i);
			}
		}
	}

	return textures.add(tex);
}

bool RendererHLSLD3D10::resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize){
	D3D10_RESOURCE_DIMENSION type;
	textures[renderTarget].texture->GetType(&type);

	switch (type){
/*		case D3D10_RESOURCE_DIMENSION_TEXTURE1D:
			D3D10_TEXTURE1D_DESC desc1d;
			((ID3D10Texture1D *) textures[renderTarget].texture)->GetDesc(&desc1d);

			desc1d.Width     = width;
			desc1d.ArraySize = arraySize;
			break;*/
		case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
			D3D10_TEXTURE2D_DESC desc2d;
			((ID3D10Texture2D *) textures[renderTarget].texture)->GetDesc(&desc2d);

			desc2d.Width     = width;
			desc2d.Height    = height;
			desc2d.ArraySize = arraySize;
			desc2d.MipLevels = mipMapCount;

			textures[renderTarget].texture->Release();
			if (FAILED(RendererHLSLD3D10::GetDevice()->CreateTexture2D(&desc2d, 0, (ID3D10Texture2D **) &textures[renderTarget].texture))){
				LOG_PRINT("Couldn't create render target");
				return false;
			}
			break;
/*		case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
			D3D10_TEXTURE3D_DESC desc3d;
			((ID3D10Texture3D *) textures[renderTarget].texture)->GetDesc(&desc3d);

			desc3d.Width  = width;
			desc3d.Height = height;
			desc3d.Depth  = depth;
			break;*/
		default:
			return false;
	}

	if (textures[renderTarget].rtv){
		textures[renderTarget].rtv->Release();
		textures[renderTarget].rtv = createRTV(textures[renderTarget].texture, textures[renderTarget].rtvFormat);
	}
	if (textures[renderTarget].dsv){
		textures[renderTarget].dsv->Release();
		textures[renderTarget].dsv = createDSV(textures[renderTarget].texture, textures[renderTarget].dsvFormat);
	}
	if (textures[renderTarget].srv){
		textures[renderTarget].srv->Release();
		textures[renderTarget].srv = createSRV(textures[renderTarget].texture, textures[renderTarget].srvFormat);
	}
	if (textures[renderTarget].rtvArray){
		for (int i = 0; i < textures[renderTarget].arraySize; i++){
			textures[renderTarget].rtvArray[i]->Release();
		}
		if (arraySize != textures[renderTarget].arraySize){
			delete [] textures[renderTarget].rtvArray;
			textures[renderTarget].rtvArray = new ID3D10RenderTargetView *[arraySize];
		}
		for (int i = 0; i < arraySize; i++){
			textures[renderTarget].rtvArray[i] = createRTV(textures[renderTarget].texture, textures[renderTarget].rtvFormat, i);
		}
	}
	if (textures[renderTarget].dsvArray){
		for (int i = 0; i < textures[renderTarget].arraySize; i++){
			textures[renderTarget].dsvArray[i]->Release();
		}
		if (arraySize != textures[renderTarget].arraySize){
			delete [] textures[renderTarget].dsvArray;
			textures[renderTarget].dsvArray = new ID3D10DepthStencilView *[arraySize];
		}
		for (int i = 0; i < arraySize; i++){
			textures[renderTarget].dsvArray[i] = createDSV(textures[renderTarget].texture, textures[renderTarget].dsvFormat, i);
		}
	}
	if (textures[renderTarget].srvArray){
		for (int i = 0; i < textures[renderTarget].arraySize; i++){
			textures[renderTarget].srvArray[i]->Release();
		}
		if (arraySize != textures[renderTarget].arraySize){
			delete [] textures[renderTarget].srvArray;
			textures[renderTarget].srvArray = new ID3D10ShaderResourceView *[arraySize];
		}
		for (int i = 0; i < arraySize; i++){
			textures[renderTarget].srvArray[i] = createSRV(textures[renderTarget].texture, textures[renderTarget].srvFormat, i);
		}
	}

	textures[renderTarget].width  = width;
	textures[renderTarget].height = height;
	textures[renderTarget].depth  = depth;
	textures[renderTarget].arraySize = arraySize;

	return true;
}

bool RendererHLSLD3D10::generateMipMaps(const TextureID renderTarget){
	RendererHLSLD3D10::GetDevice()->GenerateMips(textures[renderTarget].srv);

	return true;
}

int RendererHLSLD3D10::removeTexture(const TextureID texture){
	SAFE_RELEASE(textures[texture].texture);
	SAFE_RELEASE(textures[texture].srv);
	SAFE_RELEASE(textures[texture].rtv);
	SAFE_RELEASE(textures[texture].dsv);

	int sliceCount = (textures[texture].depth == 1)? textures[texture].arraySize : textures[texture].depth;

	if (textures[texture].srvArray){
		for (int i = 0; i < sliceCount; i++){
			textures[texture].srvArray[i]->Release();
		}
		delete [] textures[texture].srvArray;
		textures[texture].srvArray = 0;
	}
	if (textures[texture].rtvArray){
		for (int i = 0; i < sliceCount; i++){
			textures[texture].rtvArray[i]->Release();
		}
		delete [] textures[texture].rtvArray;
		textures[texture].rtvArray = 0;
	}
	if (textures[texture].dsvArray){
		for (int i = 0; i < sliceCount; i++){
			textures[texture].dsvArray[i]->Release();
		}
		delete [] textures[texture].dsvArray;
		textures[texture].dsvArray = 0;
	}
	return 0;
}
#if 1
	bool samplerCompD3D10std(const SamplerD3D10 & s0, const SamplerD3D10 &s1)
	{
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
	}
	bool constantCompD3D10std(const ConstantD3D10&	s0, const ConstantD3D10 &s1)
	{
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
	}
#endif

ID3D10ShaderReflection* RendererHLSLD3D10::CompileShader(ShaderType shaderType, ShaderD3D10& shader, const char* vsText, const char* vsMain, const char* target){return 0;}

ShaderID RendererHLSLD3D10::addHLSLShaderVrtl(
const char *vsText_, const char *gsText, const char *fsText_, const char *csText, const char *hsText, const char *dsText,
const char *vsMain, const char *gsMain, const char *fsMain, const char *csMain, const char *hsMain, const char *dsMain,
                                            	const unsigned int flags)
		{
	const char* fileName = "";
	unsigned int feature_level = D3D_FEATURE_LEVEL_10_1;
    
    const char* def="#define __HLSL4__ 1\n#define ROW_MAJOR row_major\n#define MVPSEMANTIC\n#define WSIGN +\n";
    const char* defvs="#undef TEX2D\nfloat4 GetPixelColor_(Texture2D<float4> atex, float2 aTexCoord)\n{\n\tuint width_;\n\tuint height_;\n\tatex.GetDimensions(width_, height_);\n\tfloat2 dim_;\n\tdim_.x=float(width_-1);\n\tdim_.y=float(height_-1);\n\treturn atex.Load(int3(dim_.x*aTexCoord.x, dim_.y*aTexCoord.y, 0));\n}\n#define SAMPLE2D(TEX, TEXCOORD) GetPixelColor_(TEX, TEXCOORD)\n#define SAMPLER2D Texture2D<float4>\n";
#if 0
	#if 1
	// Gamma correct
	OUT.color.rgb = pow(OUT.color.rgb, float3(0.4545));
	#elif 0
	// Gamma from 9.99 to 0.1
	float gamma=1.5;
	OUT.color.rgb=pow(abs(OUT.color.rgb), 1.0 / gamma);
	#endif
	OUT.color.a=1.0;
#endif

	std::string vsText=def;
	std::string fsText=def;
    const char* deffs="float3 GammaCorrect3(float3 aColor)\n{\n\treturn aColor;\n}\nfloat4 GammaCorrect4(float4 aColor)\n{\n\treturn aColor;\n}\n";
	vsText.append(defvs);
	fsText.append(deffs);
	if(vsText_)
		vsText.append(vsText_);
	if(fsText_)
		fsText.append(fsText_);

		ShaderD3D10 shader;
		stx_memset(&shader, 0, sizeof(ShaderD3D10));

	ID3D10Blob *shaderBuf = NULL;
	ID3D10Blob *errorsBuf = NULL;

	ID3D10ShaderReflection *vsRefl = NULL;
	ID3D10ShaderReflection *gsRefl = NULL;
	ID3D10ShaderReflection *psRefl = NULL;

	UINT compileFlags = D3D10_SHADER_PACK_MATRIX_ROW_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;// | D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
    compileFlags = D3D10_SHADER_PACK_MATRIX_ROW_MAJOR | D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;

	if (vsText.length()){
		#if 0
    		std::ofstream out("./vsText.txt");
    		out << "\nvsText:\n";
    		out << vsText;
    		out << "\nmainvs:\n";
    		out << vsMain;
    		out.close();
		#endif
        const char *target = vs10;
#if 0//def USE_D3D10_1
		// Use D3DX functions so we can compile to SM4.1
		if (SUCCEEDED(D3DX10CompileFromMemory(vsText.c_str(), vsText.length(), fileName, NULL, NULL, vsMain, "vs_4_1", compileFlags, 0, NULL, &shaderBuf, &errorsBuf, NULL))){
#else
		if (SUCCEEDED(D3D10CompileShader(vsText.c_str(), vsText.length(), fileName, NULL, NULL, vsMain, target, compileFlags, &shaderBuf, &errorsBuf))){
#endif
			if (SUCCEEDED(RendererHLSLD3D10::GetDevice()->CreateVertexShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), &shader.vertexShader))){
				D3D10GetInputSignatureBlob(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), &shader.inputSignature);
				D3D10ReflectShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), &vsRefl);

#ifdef _DEBUG
				if (flags & ASSEMBLY){
					ID3D10Blob *disasm = NULL;
					if (SUCCEEDED(D3D10DisassembleShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), FALSE, NULL, &disasm))){
						LOG_PRINT("%s\n", (const char *) disasm->GetBufferPointer());
					}
					SAFE_RELEASE(disasm);
				}
#endif
			}
		} else {
			ErrorMsg((const char *) errorsBuf->GetBufferPointer());
		}
		SAFE_RELEASE(shaderBuf);
		SAFE_RELEASE(errorsBuf);

		if (shader.vertexShader == NULL) return SHADER_NONE;
	}

	if (gsText != NULL){
#if 0//def USE_D3D10_1
		// Use D3DX functions so we can compile to SM4.1
		if (SUCCEEDED(D3DX10CompileFromMemory(gsText, stx_strlen(gsText), fileName, NULL, NULL, gsMain, "gs_4_1", compileFlags, 0, NULL, &shaderBuf, &errorsBuf, NULL))){
#else
		if (SUCCEEDED(D3D10CompileShader(gsText, stx_strlen(gsText), fileName, NULL, NULL, gsMain, "gs_4_0", compileFlags, &shaderBuf, &errorsBuf))){
#endif
			if (SUCCEEDED(RendererHLSLD3D10::GetDevice()->CreateGeometryShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), &shader.geometryShader))){
				D3D10ReflectShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), &gsRefl);
#ifdef _DEBUG
				if (flags & ASSEMBLY){
					ID3D10Blob *disasm = NULL;
					if (SUCCEEDED(D3D10DisassembleShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), FALSE, NULL, &disasm))){
						LOG_PRINT("%s\n", (const char *) disasm->GetBufferPointer());
					}
					SAFE_RELEASE(disasm);
				}
#endif
			}
		} else {
			ErrorMsg((const char *) errorsBuf->GetBufferPointer());
		}
		SAFE_RELEASE(shaderBuf);
		SAFE_RELEASE(errorsBuf);

		if (shader.geometryShader == NULL) return SHADER_NONE;
	}

	if (fsText.length()){
		#if 0
    		std::ofstream out("./fsText.txt");
    		out << "\nfsText:\n";
    		out << fsText;
    		out << "\nmainfs:\n";
    		out << fsMain;
    		out.close();
		#endif
        const char *target = ps10;
#if 0//def USE_D3D10_1
		// Use D3DX functions so we can compile to SM4.1
		if (SUCCEEDED(D3DX10CompileFromMemory(fsText.c_str(), fsText.length(), fileName, NULL, NULL, fsMain, "ps_4_1", compileFlags, 0, NULL, &shaderBuf, &errorsBuf, NULL))){
#else
		if (SUCCEEDED(D3D10CompileShader(fsText.c_str(), fsText.length(), fileName, NULL, NULL, fsMain, target, compileFlags, &shaderBuf, &errorsBuf))){
#endif
			if (SUCCEEDED(RendererHLSLD3D10::GetDevice()->CreatePixelShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), &shader.pixelShader))){
				D3D10ReflectShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), &psRefl);
#ifdef _DEBUG
				if (flags & ASSEMBLY){
					ID3D10Blob *disasm = NULL;
					if (SUCCEEDED(D3D10DisassembleShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), FALSE, NULL, &disasm))){
						LOG_PRINT("%s\n", (const char *) disasm->GetBufferPointer());
					}
					SAFE_RELEASE(disasm);
				}
#endif
			}
		} else {
			ErrorMsg((const char *) errorsBuf->GetBufferPointer());
		}
		SAFE_RELEASE(shaderBuf);
		SAFE_RELEASE(errorsBuf);

		if (shader.pixelShader == NULL) return SHADER_NONE;
	}

	D3D10_SHADER_DESC vsDesc, gsDesc, psDesc;
	if (vsRefl){
		vsRefl->GetDesc(&vsDesc);

		if (vsDesc.ConstantBuffers){
			shader.nVSCBuffers = vsDesc.ConstantBuffers;
			shader.vsConstants = new ID3D10Buffer *[shader.nVSCBuffers];
			shader.vsConstMem = new ubyte *[shader.nVSCBuffers];
			shader.vsDirty = new bool[shader.nVSCBuffers];
		}
	}
	if (gsRefl){
		gsRefl->GetDesc(&gsDesc);

		if (gsDesc.ConstantBuffers){
			shader.nGSCBuffers = gsDesc.ConstantBuffers;
			shader.gsConstants = new ID3D10Buffer *[shader.nGSCBuffers];
			shader.gsConstMem = new ubyte *[shader.nGSCBuffers];
			shader.gsDirty = new bool[shader.nGSCBuffers];
		}
	}
	if (psRefl){
		psRefl->GetDesc(&psDesc);

		if (psDesc.ConstantBuffers){
			shader.nPSCBuffers = psDesc.ConstantBuffers;
			shader.psConstants = new ID3D10Buffer *[shader.nPSCBuffers];
			shader.psConstMem = new ubyte *[shader.nPSCBuffers];
			shader.psDirty = new bool[shader.nPSCBuffers];
		}
	}

	D3D10_SHADER_BUFFER_DESC sbDesc;

	D3D10_BUFFER_DESC cbDesc;
	cbDesc.Usage = D3D10_USAGE_DEFAULT;//D3D10_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;//D3D10_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;

	VArray <ConstantD3D10> constants;

	for (uint i = 0; i < shader.nVSCBuffers; i++){
		vsRefl->GetConstantBufferByIndex(i)->GetDesc(&sbDesc);

		cbDesc.ByteWidth = sbDesc.Size;
		RendererHLSLD3D10::GetDevice()->CreateBuffer(&cbDesc, NULL, &shader.vsConstants[i]);

		shader.vsConstMem[i] = new ubyte[sbDesc.Size];
		for (uint k = 0; k < sbDesc.Variables; k++){
			D3D10_SHADER_VARIABLE_DESC vDesc;
			vsRefl->GetConstantBufferByIndex(i)->GetVariableByIndex(k)->GetDesc(&vDesc);

			ConstantD3D10 constant;
			constant.name=vDesc.Name;
			    constant.type=vDesc.Type;
			    constant.size=vDesc.Size;
			constant.vsData = shader.vsConstMem[i] + vDesc.StartOffset;
			constant.gsData = NULL;
			constant.psData = NULL;
			constant.vsBuffer = i;
			constant.gsBuffer = -1;
			constant.psBuffer = -1;
			constants.add(constant);
		}

		shader.vsDirty[i] = false;
	}
	uint maxConst = constants.getCount();
	for (uint i = 0; i < shader.nGSCBuffers; i++){
		gsRefl->GetConstantBufferByIndex(i)->GetDesc(&sbDesc);

		cbDesc.ByteWidth = sbDesc.Size;
		RendererHLSLD3D10::GetDevice()->CreateBuffer(&cbDesc, NULL, &shader.gsConstants[i]);

		shader.gsConstMem[i] = new ubyte[sbDesc.Size];
		for (uint k = 0; k < sbDesc.Variables; k++){
			D3D10_SHADER_VARIABLE_DESC vDesc;
			gsRefl->GetConstantBufferByIndex(i)->GetVariableByIndex(k)->GetDesc(&vDesc);

			int merge = -1;
			for (uint i = 0; i < maxConst; i++){
				if (strcmp(constants[i].name.c_str(), vDesc.Name) == 0){
					merge = i;
					break;
				}
			}

			if (merge < 0){
				ConstantD3D10 constant;
			    constant.name=vDesc.Name;
			    constant.type=vDesc.Type;
			    constant.size=vDesc.Size;
				constant.vsData = NULL;
				constant.gsData = shader.gsConstMem[i] + vDesc.StartOffset;
				constant.psData = NULL;
				constant.vsBuffer = -1;
				constant.gsBuffer = i;
				constant.psBuffer = -1;
				constants.add(constant);
			} else {
				constants[merge].gsData = shader.gsConstMem[i] + vDesc.StartOffset;
				constants[merge].gsBuffer = i;
			}
		}

		shader.gsDirty[i] = false;
	}
	maxConst = constants.getCount();
	for (uint i = 0; i < shader.nPSCBuffers; i++){
		psRefl->GetConstantBufferByIndex(i)->GetDesc(&sbDesc);

		cbDesc.ByteWidth = sbDesc.Size;
		RendererHLSLD3D10::GetDevice()->CreateBuffer(&cbDesc, NULL, &shader.psConstants[i]);

		shader.psConstMem[i] = new ubyte[sbDesc.Size];
		for (uint k = 0; k < sbDesc.Variables; k++){
			D3D10_SHADER_VARIABLE_DESC vDesc;
			psRefl->GetConstantBufferByIndex(i)->GetVariableByIndex(k)->GetDesc(&vDesc);

			int merge = -1;
			for (uint i = 0; i < maxConst; i++){
				if (strcmp(constants[i].name.c_str(), vDesc.Name) == 0){
					merge = i;
					break;
				}
			}

			if (merge < 0){
				ConstantD3D10 constant;
			    constant.name=vDesc.Name;
			    constant.type=vDesc.Type;
			    constant.size=vDesc.Size;
				constant.vsData = NULL;
				constant.gsData = NULL;
				constant.psData = shader.psConstMem[i] + vDesc.StartOffset;
				constant.vsBuffer = -1;
				constant.gsBuffer = -1;
				constant.psBuffer = i;
				constants.add(constant);
			} else {
				constants[merge].psData = shader.psConstMem[i] + vDesc.StartOffset;
				constants[merge].psBuffer = i;
			}
		}

		shader.psDirty[i] = false;
	}

	shader.nConstants = constants.getCount();
	shader.constants=constants;
    for(unsigned int i=0;i<constants.getCount();i++)
    {
	    stx_Variables::AddUniform(shaders.size(), constant[i].name, constant[i].size, getConstantType(constant[i].type));
    }
#if 1
	if(shader.constants.getCount()>1) qsort((void*)shader.constants.getArray(), shader.nConstants, sizeof(ConstantD3D10), constantCompD3D10);
#else
	std::sort((shader).constants.begin(), (shader).constants.end(), constantCompD3D10std);
#endif

	uint nMaxVSRes = vsRefl? vsDesc.BoundResources : 0;
	uint nMaxGSRes = gsRefl? gsDesc.BoundResources : 0;
	uint nMaxPSRes = psRefl? psDesc.BoundResources : 0;

	int maxResources = nMaxVSRes + nMaxGSRes + nMaxPSRes;
	if (maxResources){
		shader.textures.resize(maxResources);
		shader.samplers.resize(maxResources);
		shader.nTextures = 0;
		shader.nSamplers = 0;

		D3D10_SHADER_INPUT_BIND_DESC siDesc;
		for (uint i = 0; i < nMaxVSRes; i++){
			vsRefl->GetResourceBindingDesc(i, &siDesc);

			if (siDesc.Type == D3D10_SIT_TEXTURE){
				shader.textures[shader.nTextures].name=siDesc.Name;
				shader.textures[shader.nTextures].vsIndex = siDesc.BindPoint;
				shader.textures[shader.nTextures].gsIndex = -1;
				shader.textures[shader.nTextures].psIndex = -1;
				shader.nTextures++;
			} else if (siDesc.Type == D3D10_SIT_SAMPLER){
				shader.samplers[shader.nSamplers].name=siDesc.Name;
				shader.samplers[shader.nSamplers].vsIndex = siDesc.BindPoint;
				shader.samplers[shader.nSamplers].gsIndex = -1;
				shader.samplers[shader.nSamplers].psIndex = -1;						
                    stx_Variables::AddSampler(shaders.size(), siDesc.Name, siDesc.BindPoint);
				shader.nSamplers++;
			}
		}
		uint maxTexture = shader.nTextures;
		uint maxSampler = shader.nSamplers;
		for (uint i = 0; i < nMaxGSRes; i++){
			gsRefl->GetResourceBindingDesc(i, &siDesc);

			if (siDesc.Type == D3D10_SIT_TEXTURE){
				int merge = -1;
				for (uint i = 0; i < maxTexture; i++){
					if (strcmp(shader.textures[i].name.c_str(), siDesc.Name) == 0){
						merge = i;
						break;
					}
				}
				if (merge < 0){
					shader.textures[shader.nTextures].name=siDesc.Name;
					shader.textures[shader.nTextures].vsIndex = -1;
					shader.textures[shader.nTextures].gsIndex = siDesc.BindPoint;
					shader.textures[shader.nTextures].psIndex = -1;
					shader.nTextures++;
				} else {
					shader.textures[merge].gsIndex = siDesc.BindPoint;
				}
			} else if (siDesc.Type == D3D10_SIT_SAMPLER){
				int merge = -1;
				for (uint i = 0; i < maxSampler; i++){
					if (strcmp(shader.samplers[i].name.c_str(), siDesc.Name) == 0){
						merge = i;
						break;
					}
				}
				if (merge < 0){
					shader.samplers[shader.nSamplers].name=siDesc.Name;
					shader.samplers[shader.nSamplers].vsIndex = -1;
					shader.samplers[shader.nSamplers].gsIndex = siDesc.BindPoint;
					shader.samplers[shader.nSamplers].psIndex = -1;						
                    stx_Variables::AddSampler(shaders.size(), siDesc.Name, siDesc.BindPoint);
					shader.nSamplers++;
				} else {
					shader.samplers[merge].gsIndex = siDesc.BindPoint;
				}
			}
		}
		maxTexture = shader.nTextures;
		maxSampler = shader.nSamplers;
		for (uint i = 0; i < nMaxPSRes; i++){
			psRefl->GetResourceBindingDesc(i, &siDesc);

			if (siDesc.Type == D3D10_SIT_TEXTURE){
				int merge = -1;
				for (uint i = 0; i < maxTexture; i++){
					if (strcmp(shader.textures[i].name.c_str(), siDesc.Name) == 0){
						merge = i;
						break;
					}
				}
				if (merge < 0){
					shader.textures[shader.nTextures].name=siDesc.Name;
					shader.textures[shader.nTextures].vsIndex = -1;
					shader.textures[shader.nTextures].gsIndex = -1;
					shader.textures[shader.nTextures].psIndex = siDesc.BindPoint;
					shader.nTextures++;
				} else {
					shader.textures[merge].psIndex = siDesc.BindPoint;
				}
			} else if (siDesc.Type == D3D10_SIT_SAMPLER){
				int merge = -1;
				for (uint i = 0; i < maxSampler; i++){
					if (strcmp(shader.samplers[i].name.c_str(), siDesc.Name) == 0){
						merge = i;
						break;
					}
				}
				if (merge < 0){
					shader.samplers[shader.nSamplers].name=siDesc.Name;
					shader.samplers[shader.nSamplers].vsIndex = -1;
					shader.samplers[shader.nSamplers].gsIndex = -1;
					shader.samplers[shader.nSamplers].psIndex = siDesc.BindPoint;						
                    stx_Variables::AddSampler(shaders.size(), siDesc.Name, siDesc.BindPoint);
					shader.nSamplers++;
				} else {
					shader.samplers[merge].psIndex = siDesc.BindPoint;
				}
			}
		}
		shader.textures.resize(shader.nTextures);
		shader.samplers.resize(shader.nSamplers);
#if 1
	if(shader.textures.getCount()>1) qsort((void*)shader.textures.getArray(), shader.nTextures, sizeof(SamplerD3D10), samplerCompD3D10);
	if(shader.samplers.getCount()>1) qsort((void*)shader.samplers.getArray(), shader.nSamplers, sizeof(SamplerD3D10), samplerCompD3D10);
#else
	std::sort((shader).textures.begin(), (shader).textures.end(), samplerCompD3D10std);
	std::sort((shader).samplers.begin(), (shader).samplers.end(), samplerCompD3D10std);
#endif
	}

	if (vsRefl) vsRefl->Release();
	if (gsRefl) gsRefl->Release();
	if (psRefl) psRefl->Release();

	return shaders.add(shader);
}

VertexFormatID RendererHLSLD3D10::addVertexFormatVrtl(const FormatDesc *formatDesc, const unsigned int nAttribs, const ShaderID shader){
	if (shader < 0) return -1;
	static const DXGI_FORMAT formats[][4] = {
		DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R16_FLOAT, DXGI_FORMAT_R16G16_FLOAT, DXGI_FORMAT_UNKNOWN,         DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_R8_UNORM,  DXGI_FORMAT_R8G8_UNORM,   DXGI_FORMAT_UNKNOWN,         DXGI_FORMAT_R8G8B8A8_UNORM,
	};

	static const char *semantics[] = {
		0,
		"Position",
		"Texcoord",
		"Normal",
		//"Color",
		"Tangent",
		"Binormal",
	};


	int index[6];
	stx_memset(index, 0, sizeof(index));

	VertexFormatD3D10 vf;
	stx_memset(vf.vertexSize, 0, sizeof(vf.vertexSize));

	D3D10_INPUT_ELEMENT_DESC *desc = new D3D10_INPUT_ELEMENT_DESC[nAttribs];

	// Fill the vertex element array
	for (unsigned int i = 0; i < nAttribs; i++){
		int stream = formatDesc[i].stream;
		int size = formatDesc[i].size;
		desc[i].InputSlot = stream;
		desc[i].AlignedByteOffset = vf.vertexSize[stream];
		desc[i].SemanticName = semantics[formatDesc[i].type];
		desc[i].SemanticIndex = index[formatDesc[i].type]++;
		desc[i].Format = formats[formatDesc[i].format][size - 1];
		desc[i].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		desc[i].InstanceDataStepRate = 0;

		vf.vertexSize[stream] += size * getFormatSize(formatDesc[i].format);
	}

	HRESULT hr = RendererHLSLD3D10::GetDevice()->CreateInputLayout(desc, nAttribs, shaders[shader].inputSignature->GetBufferPointer(), shaders[shader].inputSignature->GetBufferSize(), &vf.inputLayout);
	delete desc;

	if (FAILED(hr)){
		LOG_PRINT("Couldn't create vertex declaration");
		return VF_NONE;
	}

	return vertexFormats.add(vf);
}

D3D10_USAGE usage[] = {
	D3D10_USAGE_IMMUTABLE,
	D3D10_USAGE_DEFAULT,
	D3D10_USAGE_DYNAMIC,
};

VertexBufferID RendererHLSLD3D10::addVertexBufferVrtl(const int size, const BufferAccess bufferAccess, const void *data, VertexBufferID old){
	VertexBufferD3D10 vb;
	vb.size = size;

	D3D10_BUFFER_DESC desc;
	desc.Usage = usage[bufferAccess];
	desc.ByteWidth = size;
	desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = (bufferAccess == DYNAMIC)? D3D10_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = data;
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;

	if (FAILED(RendererHLSLD3D10::GetDevice()->CreateBuffer(&desc, data? &vbData : 0, &vb.vertexBuffer))){
        LOG_PRINT("Couldn't create vertex buffer");
		return VB_NONE;
	}

	return vertexBuffers.add(vb);
}

IndexBufferID RendererHLSLD3D10::addIndexBufferVrtl(const unsigned int nIndices, const unsigned int indexSize, const BufferAccess bufferAccess, const void *data, IndexBufferID old){
	IndexBufferD3D10 ib;
	ib.indexSize = indexSize;
	ib.nIndices = nIndices;

	D3D10_BUFFER_DESC desc;
	desc.Usage = usage[bufferAccess];
	desc.ByteWidth = nIndices * indexSize;
	desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = (bufferAccess == DYNAMIC)? D3D10_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA ibData;
	ibData.pSysMem = data;
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;

	if (FAILED(RendererHLSLD3D10::GetDevice()->CreateBuffer(&desc, data? &ibData : 0, &ib.indexBuffer))){
        LOG_PRINT("Couldn't create vertex buffer");
		return IB_NONE;
	}

	return indexBuffers.add(ib);
}

D3D10_FILTER filters[] = {
	D3D10_FILTER_MIN_MAG_MIP_POINT,
	D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT,
	D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT,
	D3D10_FILTER_MIN_MAG_MIP_LINEAR,
	D3D10_FILTER_ANISOTROPIC,
	D3D10_FILTER_ANISOTROPIC,
};

D3D10_TEXTURE_ADDRESS_MODE address_modes[] = {
	D3D10_TEXTURE_ADDRESS_WRAP,
	D3D10_TEXTURE_ADDRESS_CLAMP,
	D3D10_TEXTURE_ADDRESS_BORDER,
};

SamplerStateID RendererHLSLD3D10::addSamplerStateVrtl(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod, const unsigned int maxAniso, const int compareFunc, const float *border_color){
	SamplerStateD3D10 samplerState;

	D3D10_SAMPLER_DESC desc;
	desc.Filter = filters[filter];
	if (compareFunc){
		desc.Filter = (D3D10_FILTER) (desc.Filter | 0x80);
	}
	desc.ComparisonFunc = (D3D10_COMPARISON_FUNC) compareFunc;
	desc.AddressU = address_modes[s];
	desc.AddressV = address_modes[t];
	desc.AddressW = address_modes[r];
	desc.MipLODBias = lod;
	desc.MaxAnisotropy = hasAniso(filter)? maxAniso : 1;
	if (border_color)
	{
		desc.BorderColor[0] = border_color[0];
		desc.BorderColor[1] = border_color[1];
		desc.BorderColor[2] = border_color[2];
		desc.BorderColor[3] = border_color[3];
	}
	else
	{
		desc.BorderColor[0] = 0;
		desc.BorderColor[1] = 0;
		desc.BorderColor[2] = 0;
		desc.BorderColor[3] = 0;
	}
	desc.MinLOD = 0;
	desc.MaxLOD = hasMipmaps(filter)? D3D10_FLOAT32_MAX : 0;

	if (FAILED(RendererHLSLD3D10::GetDevice()->CreateSamplerState(&desc, &samplerState.samplerState))){
		LOG_PRINT("Couldn't create samplerstate");
		return SS_NONE;
	}

	return samplerStates.add(samplerState);
}

BlendStateID RendererHLSLD3D10::addBlendState(const int srcFactorRGB, const int destFactorRGB, const int srcFactorAlpha, const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, const int mask, const bool alphaToCoverage){
	BlendStateD3D10 blendState;

	BOOL blendEnable =
		srcFactorRGB != D3D10_BLEND_ONE || destFactorRGB != D3D10_BLEND_ZERO ||
		srcFactorAlpha != D3D10_BLEND_ONE || destFactorAlpha != D3D10_BLEND_ZERO;

	D3D10_BLEND_DESC desc;
	desc.AlphaToCoverageEnable = (BOOL) alphaToCoverage;
	desc.BlendOp = (D3D10_BLEND_OP) blendModeAlpha;
	desc.SrcBlend = (D3D10_BLEND) srcFactorRGB;
	desc.DestBlend = (D3D10_BLEND) destFactorRGB;
	desc.BlendOpAlpha = (D3D10_BLEND_OP) blendModeAlpha;
	desc.SrcBlendAlpha = (D3D10_BLEND) srcFactorAlpha;
	desc.DestBlendAlpha = (D3D10_BLEND) destFactorAlpha;

	stx_memset(&desc.BlendEnable, 0, sizeof(desc.BlendEnable));
	stx_memset(&desc.RenderTargetWriteMask, 0, sizeof(desc.RenderTargetWriteMask));
	desc.BlendEnable[0] = blendEnable;
	desc.RenderTargetWriteMask[0] = mask;

	if (FAILED(RendererHLSLD3D10::GetDevice()->CreateBlendState(&desc, &blendState.blendState))){
		LOG_PRINT("Couldn't create blendstate");
		return BS_NONE;
	}

	return blendStates.add(blendState);
}

DepthStateID RendererHLSLD3D10::addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc,
											   const bool stencilTest, const uint8 stencilMask, const int stencilFunc,
											   const int stencilFail, const int depthFail, const int stencilPass){
	DepthStateD3D10 depthState;

	D3D10_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = (BOOL) depthTest;
	desc.DepthWriteMask = depthWrite? D3D10_DEPTH_WRITE_MASK_ALL : D3D10_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = (D3D10_COMPARISON_FUNC) depthFunc;
	desc.StencilEnable = (BOOL) stencilTest;
	desc.StencilReadMask  = stencilMask;
	desc.StencilWriteMask = stencilMask;
	desc.BackFace. StencilFunc = (D3D10_COMPARISON_FUNC) stencilFunc;
	desc.FrontFace.StencilFunc = (D3D10_COMPARISON_FUNC) stencilFunc;
	desc.BackFace. StencilDepthFailOp = (D3D10_STENCIL_OP) depthFail;
	desc.FrontFace.StencilDepthFailOp = (D3D10_STENCIL_OP) depthFail;
	desc.BackFace. StencilFailOp = (D3D10_STENCIL_OP) stencilFail;
	desc.FrontFace.StencilFailOp = (D3D10_STENCIL_OP) stencilFail;
	desc.BackFace. StencilPassOp = (D3D10_STENCIL_OP) stencilPass;
	desc.FrontFace.StencilPassOp = (D3D10_STENCIL_OP) stencilPass;

	if (FAILED(RendererHLSLD3D10::GetDevice()->CreateDepthStencilState(&desc, &depthState.dsState))){
		LOG_PRINT("Couldn't create depthstate");
		return DS_NONE;
	}

	return depthStates.add(depthState);
}

RasterizerStateID RendererHLSLD3D10::addRasterizerStateVrtl(const int cullMode, const int fillMode, const bool multiSample, const bool scissor){
	RasterizerStateD3D10 rasterizerState;

	D3D10_RASTERIZER_DESC desc;
	desc.CullMode = (D3D10_CULL_MODE) cullMode;
	desc.FillMode = (D3D10_FILL_MODE) fillMode;
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.AntialiasedLineEnable = FALSE;
	desc.DepthClipEnable = TRUE;
	desc.MultisampleEnable = (BOOL) multiSample;
	desc.ScissorEnable = (BOOL) scissor;

	if (FAILED(RendererHLSLD3D10::GetDevice()->CreateRasterizerState(&desc, &rasterizerState.rsState))){
		LOG_PRINT("Couldn't create rasterizerstate");
		return RS_NONE;
	}

	return rasterizerStates.add(rasterizerState);
}

int RendererHLSLD3D10::getSamplerIndex(ShaderID shd, const char* name)
{
	if((shd<0)||(shd>(shaders.size()-1)))
		return -1;
	const SamplerD3D10 *samplers=shaders[shd].textures.getArray();
	const int count=shaders[shd].nTextures;

	int minSampler = 0;
	int maxSampler = count - 1;
	// Do a quick lookup in the sorted table with a binary search
	while (minSampler <= maxSampler){
		int currSampler = (minSampler + maxSampler) >> 1;
        int res = strcmp(name, samplers[currSampler].name.c_str());
		if (res == 0){
			return currSampler;
		} else if (res > 0){
            minSampler = currSampler + 1;
		} else {
            maxSampler = currSampler - 1;
		}
	}
	return 0;
}

const SamplerD3D10 *getSampler(const SamplerD3D10 *samplers, const int count, const char *name){
	int minSampler = 0;
	int maxSampler = count - 1;

	// Do a quick lookup in the sorted table with a binary search
	while (minSampler <= maxSampler){
		int currSampler = (minSampler + maxSampler) >> 1;
        int res = strcmp(name, samplers[currSampler].name.c_str());
		if (res == 0){
			return samplers + currSampler;
		} else if (res > 0){
            minSampler = currSampler + 1;
		} else {
            maxSampler = currSampler - 1;
		}
	}

	return 0;
}

void RendererHLSLD3D10::setTextureSlice(const char *textureName, const TextureID texture, const int slice){
	ASSERT(selectedShader != SHADER_NONE);

	const SamplerD3D10 *s = getSampler(shaders[selectedShader].textures.getArray(), shaders[selectedShader].nTextures, textureName);
	if (s){
		if (s->vsIndex >= 0){
			selectedTexturesVS[s->vsIndex] = texture;
			selectedTextureSlicesVS[s->vsIndex] = slice;
		}/*
		if (s->gsIndex >= 0){
			selectedTexturesGS[s->gsIndex] = texture;
			selectedTextureSlicesGS[s->gsIndex] = slice;
		}*/
		if (s->psIndex >= 0){
			selectedTexturesPS[s->psIndex] = texture;
			selectedTextureSlicesPS[s->psIndex] = slice;
		}
	}
#ifdef _DEBUG
	else {
		char str[256];
		sprintf(str, "Invalid texture \"%s\"", textureName);
		LOG_PRINT("%s\n", str);
	}
#endif
}


bool fillSRV(ID3D10ShaderResourceView **dest, int &min, int &max, const TextureID selectedTextures[], TextureID currentTextures[], const TextureID selectedTextureSlices[], TextureID currentTextureSlices[], const TextureD3D10 *textures){
	min = 0;
	do {
		if (selectedTextures[min] != currentTextures[min] || selectedTextureSlices[min] != currentTextureSlices[min]){
			max = MAX_TEXTUREUNIT;
			do {
				max--;
			} while (selectedTextures[max] == currentTextures[max] && selectedTextureSlices[max] != currentTextureSlices[max]);

			for (int i = min; i <= max; i++){
				if (selectedTextures[i] != TEXTURE_NONE){
					if (selectedTextureSlices[i] == NO_SLICE){
						*dest++ = textures[selectedTextures[i]].srv;
					} else {
						*dest++ = textures[selectedTextures[i]].srvArray[selectedTextureSlices[i]];
					}
				} else {
					*dest++ = 0;
				}
				currentTextures[i] = selectedTextures[i];
				currentTextureSlices[i] = selectedTextureSlices[i];
			}
			return true;
		}
		min++;
	} while (min < MAX_TEXTUREUNIT);

	return false;
}

void RendererHLSLD3D10::applyTextures(){
	ID3D10ShaderResourceView *srViews[MAX_TEXTUREUNIT];

	int min, max;
	int size=textures.getCount();
	if(size)
	if (fillSRV(srViews, min, max, selectedTexturesVS, currentTexturesVS, selectedTextureSlicesVS, currentTextureSlicesVS, textures.getArray())){
		RendererHLSLD3D10::GetDevice()->VSSetShaderResources(min, max - min + 1, srViews);
	}
	//if (fillSRV(srViews, min, max, selectedTexturesGS, currentTexturesGS, selectedTextureSlicesGS, currentTextureSlicesGS, textures.getArray())){
	//	RendererHLSLD3D10::GetDevice()->GSSetShaderResources(min, max - min + 1, srViews);
	//}
	if(size)
	if (fillSRV(srViews, min, max, selectedTexturesPS, currentTexturesPS, selectedTextureSlicesPS, currentTextureSlicesPS, textures.getArray())){
		RendererHLSLD3D10::GetDevice()->PSSetShaderResources(min, max - min + 1, srViews);
		RendererHLSLD3D10::GetDevice()->VSSetShaderResources(min, max - min + 1, srViews);
	}
}

void RendererHLSLD3D10::setSamplerState(const char *samplerName, const SamplerStateID samplerState){
	ASSERT(selectedShader != SHADER_NONE);

	const SamplerD3D10 *s = getSampler(shaders[selectedShader].samplers.getArray(), shaders[selectedShader].nSamplers, samplerName);
	if (s){
		if (s->vsIndex >= 0) selectedSamplerStatesVS[s->vsIndex] = samplerState;
		//if (s->gsIndex >= 0) selectedSamplerStatesGS[s->gsIndex] = samplerState;
		if (s->psIndex >= 0) selectedSamplerStatesPS[s->psIndex] = samplerState;
	}
#ifdef _DEBUG
	else {
		char str[256];
		sprintf(str, "Invalid samplerstate \"%s\"", samplerName);
		LOG_PRINT("%s\n", str);
	}
#endif
}

bool fillSS(ID3D10SamplerState **dest, int &min, int &max, const SamplerStateID aselectedSamplerStates[], SamplerStateID acurrentSamplerStates[], const SamplerStateD3D10 *asamplerStates){
	min = 0;
	do {
		if (aselectedSamplerStates[min] != acurrentSamplerStates[min]){
			max = MAX_SAMPLERSTATE;
			do {
				max--;
			} while (aselectedSamplerStates[max] == acurrentSamplerStates[max]);

			for (int i = min; i <= max; i++){
				if (aselectedSamplerStates[i] != SS_NONE){
					*dest++ = asamplerStates[aselectedSamplerStates[i]].samplerState;
				} else {
					*dest++ = 0;
				}
				acurrentSamplerStates[i] = aselectedSamplerStates[i];
			}
			return true;
		}
		min++;
	} while (min < MAX_SAMPLERSTATE);

	return false;
}

void RendererHLSLD3D10::applySamplerStates(){
	ID3D10SamplerState *samplers[MAX_SAMPLERSTATE];

	int min, max;
	int size=samplerStates.getCount();
	if(size)
	if (fillSS(samplers, min, max, selectedSamplerStatesVS, currentSamplerStatesVS, samplerStates.getArray())){
		RendererHLSLD3D10::GetDevice()->VSSetSamplers(min, max - min + 1, samplers);
	}
	if(size)
	if (fillSS(samplers, min, max, selectedSamplerStatesGS, currentSamplerStatesGS, samplerStates.getArray())){
		RendererHLSLD3D10::GetDevice()->GSSetSamplers(min, max - min + 1, samplers);
	}
	if(size)
	if (fillSS(samplers, min, max, selectedSamplerStatesPS, currentSamplerStatesPS, samplerStates.getArray())){
		RendererHLSLD3D10::GetDevice()->PSSetSamplers(min, max - min + 1, samplers);
	}
}

void RendererHLSLD3D10::setShaderConstantRaw(const char *name, const void *data, const int size){
	int minConstant = 0;
	if(selectedShader==-1) return;
	int maxConstant = shaders[selectedShader].nConstants - 1;
	ConstantD3D10 *constants = shaders[selectedShader].constants.getArray();

	// Do a quick lookup in the sorted table with a binary search
	while (minConstant <= maxConstant){
		int currConstant = (minConstant + maxConstant) >> 1;
		if (currConstant == -1) return;
		int res = strcmp(name, constants[currConstant].name.c_str());
		if (res == 0){
			ConstantD3D10 *c = constants + currConstant;

			if (c->vsData){
				if (stx_memcmp(c->vsData, data, size)){
					stx_memcpy(c->vsData, data, size);
					shaders[selectedShader].vsDirty[c->vsBuffer] = true;
				}
			}/*
			if (c->gsData){
				if (stx_memcmp(c->gsData, data, size)){
					stx_memcpy(c->gsData, data, size);
					shaders[selectedShader].gsDirty[c->gsBuffer] = true;
				}
			}*/
			if (c->psData){
				if (stx_memcmp(c->psData, data, size)){
					stx_memcpy(c->psData, data, size);
					shaders[selectedShader].psDirty[c->psBuffer] = true;
				}
			}
			return;

		} else if (res > 0){
			minConstant = currConstant + 1;
		} else {
			maxConstant = currConstant - 1;
		}
	}

#ifdef _DEBUG
	char str[256];
	sprintf(str, "Invalid constant \"%s\"", name);
	LOG_PRINT("%s\n", str);
#endif
}

void RendererHLSLD3D10::applyConstants(){
	if (currentShader != SHADER_NONE){
		ShaderD3D10 *shader = &shaders[currentShader];

		for (unsigned int i = 0; i < shader->nVSCBuffers; i++){
			if (shader->vsDirty[i]){
				RendererHLSLD3D10::GetDevice()->UpdateSubresource(shader->vsConstants[i], 0, 0, shader->vsConstMem[i], 0, 0);
				shader->vsDirty[i] = false;
			}
		}
		for (unsigned int i = 0; i < shader->nGSCBuffers; i++){
			if (shader->gsDirty[i]){
				RendererHLSLD3D10::GetDevice()->UpdateSubresource(shader->gsConstants[i], 0, 0, shader->gsConstMem[i], 0, 0);
				shader->gsDirty[i] = false;
			}
		}
		for (unsigned int i = 0; i < shader->nPSCBuffers; i++){
			if (shader->psDirty[i]){
				RendererHLSLD3D10::GetDevice()->UpdateSubresource(shader->psConstants[i], 0, 0, shader->psConstMem[i], 0, 0);
				shader->psDirty[i] = false;
			}
		}
	}
}

void RendererHLSLD3D10::changeRenderTargetsVrtl(const TextureID *colorRTs, const unsigned int nRenderTargets, const TextureID depthRT, const int depthSlice, const int *slices){
	// Reset bound textures
	for (int i = 0; i < MAX_TEXTUREUNIT; i++){
		selectedTexturesVS[i] = TEXTURE_NONE;
		selectedTexturesGS[i] = TEXTURE_NONE;
		selectedTexturesPS[i] = TEXTURE_NONE;
	}
	applyTextures();

	ID3D10RenderTargetView *rtv[16];
	ID3D10DepthStencilView *dsv;

	if (depthRT == FB_DEPTH){
		dsv = depthBufferDSV;
	} else if (depthRT == TEXTURE_NONE){
		dsv = 0;
	} else if (depthSlice == NO_SLICE){
		dsv = textures[depthRT].dsv;
	} else {
		dsv = textures[depthRT].dsvArray[depthSlice];
	}
	currentDepthRT = depthRT;
	currentDepthSlice = depthSlice;

	for (unsigned int i = 0; i < nRenderTargets; i++){
		TextureID rt = colorRTs[i];
		int slice = NO_SLICE;
		if (slices == 0 || slices[i] == NO_SLICE){
			if (rt == FB_COLOR){
				rtv[i] = backBufferRTV;
			} else {
				rtv[i] = textures[rt].rtv;
			}
		} else {
			slice = slices[i];
			rtv[i] = textures[rt].rtvArray[slice];
		}

		currentColorRT[i] = rt;
		currentColorRTSlice[i] = slice;
	}

	for (unsigned int i = nRenderTargets; i < MAX_MRTS; i++){
		currentColorRT[i] = TEXTURE_NONE;
		currentColorRTSlice[i] = NO_SLICE;
	}

	RendererHLSLD3D10::GetDevice()->OMSetRenderTargets(nRenderTargets, rtv, dsv);

	D3D10_VIEWPORT vp0;
	UINT nvp0=1;
	RendererHLSLD3D10::GetDevice()->RSGetViewports(&nvp0, &vp0);

	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;

	if (nRenderTargets > 0){
		TextureID rt = colorRTs[0];
		if (rt == FB_COLOR){
			vp.Width  = vp0.Width;
			vp.Height = vp0.Height;
		} else {
			vp.Width  = textures[rt].width;
			vp.Height = textures[rt].height;
		}
	} else {
		vp.Width  = textures[depthRT].width;
		vp.Height = textures[depthRT].height;
	}
	RendererHLSLD3D10::GetDevice()->RSSetViewports(1, &vp);
}

void RendererHLSLD3D10::changeToMainFramebuffer(){
	// Reset bound textures
	for (int i = 0; i < MAX_TEXTUREUNIT; i++){
		selectedTexturesVS[i] = TEXTURE_NONE;
		selectedTexturesGS[i] = TEXTURE_NONE;
		selectedTexturesPS[i] = TEXTURE_NONE;
	}
	applyTextures();

	RendererHLSLD3D10::GetDevice()->OMSetRenderTargets(1, &backBufferRTV, depthBufferDSV);

	D3D10_VIEWPORT vp = { 0, 0, IRenderer::GetViewportWidth(), IRenderer::GetViewportHeight(), 0.0f, 1.0f };
	RendererHLSLD3D10::GetDevice()->RSSetViewports(1, &vp);

	currentColorRT[0] = FB_COLOR;
	currentColorRTSlice[0] = NO_SLICE;


	for (unsigned int i = 1; i < MAX_MRTS; i++){
		currentColorRT[i] = TEXTURE_NONE;
		currentColorRTSlice[i] = NO_SLICE;
	}
	currentDepthRT = FB_DEPTH;
	currentDepthSlice = NO_SLICE;
}

void RendererHLSLD3D10::changeShader(const ShaderID shaderID){
	if (shaderID != currentShader){
		if (shaderID == SHADER_NONE){
			RendererHLSLD3D10::GetDevice()->PSSetShader(0);
			RendererHLSLD3D10::GetDevice()->VSSetShader(0);
			RendererHLSLD3D10::GetDevice()->GSSetShader(0);
		} else {
			if(shaders[shaderID].pixelShader) RendererHLSLD3D10::GetDevice()->PSSetShader(shaders[shaderID].pixelShader);
			if(shaders[shaderID].vertexShader) RendererHLSLD3D10::GetDevice()->VSSetShader(shaders[shaderID].vertexShader);
			if(shaders[shaderID].geometryShader) RendererHLSLD3D10::GetDevice()->GSSetShader(shaders[shaderID].geometryShader);

			if (shaders[shaderID].nPSCBuffers) RendererHLSLD3D10::GetDevice()->PSSetConstantBuffers(0, shaders[shaderID].nPSCBuffers, shaders[shaderID].psConstants);
			if (shaders[shaderID].nVSCBuffers) RendererHLSLD3D10::GetDevice()->VSSetConstantBuffers(0, shaders[shaderID].nVSCBuffers, shaders[shaderID].vsConstants);
			if (shaders[shaderID].nGSCBuffers) RendererHLSLD3D10::GetDevice()->GSSetConstantBuffers(0, shaders[shaderID].nGSCBuffers, shaders[shaderID].gsConstants);
		}

		currentShader = shaderID;
	}
}

void RendererHLSLD3D10::changeVertexFormat(const VertexFormatID vertexFormatID){
	if (vertexFormatID != currentVertexFormat){
		if (vertexFormatID == VF_NONE){
			RendererHLSLD3D10::GetDevice()->IASetInputLayout(0);
		} else {
			RendererHLSLD3D10::GetDevice()->IASetInputLayout(vertexFormats[vertexFormatID].inputLayout);

			/*if (currentVertexFormat != VF_NONE){
				for (int i = 0; i < MAX_VERTEXSTREAM; i++){
					if (vertexFormats[vertexFormatID].vertexSize[i] != vertexFormats[currentVertexFormat].vertexSize[i]){
						currentVertexBuffers[i] = VB_INVALID;
					}
				}
			}*/
		}

		currentVertexFormat = vertexFormatID;
	}
}

void RendererHLSLD3D10::changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBufferID, const intptr offset){
	//if(stream>=vertexBuffers.size()) return;
	//if(stream>=currentOffsets.size()) return;
	{
		UINT strides[1];
		UINT offsets[1];
		if (vertexBufferID == VB_NONE){
			strides[0] = 0;
			offsets[0] = 0;
			ID3D10Buffer *null[] = { 0 };
			RendererHLSLD3D10::GetDevice()->IASetVertexBuffers(stream, 1, null, strides, offsets);
		} else {
			if ((vertexBufferID<0) || (vertexBuffers.size()<=vertexBufferID)) return;
			if(currentVertexFormat < 0) return;
			if (vertexFormats.size() <= currentVertexFormat) return;
			//if (vertexSize.size() <= stream) return;
			strides[0] = vertexFormats[currentVertexFormat].vertexSize[stream];
			offsets[0] = (UINT) offset;
			RendererHLSLD3D10::GetDevice()->IASetVertexBuffers(stream, 1, &vertexBuffers[vertexBufferID].vertexBuffer, strides, offsets);
		}

		currentVertexBuffers[stream] = vertexBufferID;
		currentOffsets[stream] = offset;
	}
}

void RendererHLSLD3D10::changeIndexBuffer(const IndexBufferID indexBufferID){
	if (indexBufferID != currentIndexBuffer){
		if (indexBufferID == IB_NONE){
			RendererHLSLD3D10::GetDevice()->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
		} else if (indexBuffers.size() > indexBufferID) {
			DXGI_FORMAT format = indexBuffers[indexBufferID].indexSize == 2? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			RendererHLSLD3D10::GetDevice()->IASetIndexBuffer(indexBuffers[indexBufferID].indexBuffer, format, 0);
		}

		currentIndexBuffer = indexBufferID;
	}
}

void RendererHLSLD3D10::changeBlendStateVrtl(const BlendStateID blendState, const unsigned int sampleMask){
	if (blendState != currentBlendState || sampleMask != currentSampleMask){
		if (blendState == BS_NONE){
			RendererHLSLD3D10::GetDevice()->OMSetBlendState(0, D3DXFROMWINEVECTOR4(0, 0, 0, 0), sampleMask);
		} else {
			RendererHLSLD3D10::GetDevice()->OMSetBlendState(blendStates[blendState].blendState, D3DXFROMWINEVECTOR4(0, 0, 0, 0), sampleMask);
		}

		currentBlendState = blendState;
		currentSampleMask = sampleMask;
	}
}

void RendererHLSLD3D10::changeDepthStateVrtl(const DepthStateID depthState, const unsigned int stencilRef){
	if (depthState != currentDepthState || stencilRef != currentStencilRef){
		if (depthState == DS_NONE){
			RendererHLSLD3D10::GetDevice()->OMSetDepthStencilState(0, stencilRef);
		} else {
			RendererHLSLD3D10::GetDevice()->OMSetDepthStencilState(depthStates[depthState].dsState, stencilRef);
		}

		currentDepthState = depthState;
		currentStencilRef = stencilRef;
	}
}

void RendererHLSLD3D10::changeRasterizerState(const RasterizerStateID rasterizerState){
	if (rasterizerState != currentRasterizerState){
		if (rasterizerState == RS_NONE){
			//RendererHLSLD3D10::GetDevice()->RSSetState(0);
			RendererHLSLD3D10::GetDevice()->RSSetState(rasterizerStates[0].rsState);
		} else {
			RendererHLSLD3D10::GetDevice()->RSSetState(rasterizerStates[rasterizerState].rsState);
		}

		currentRasterizerState = rasterizerState;
	}
}

void RendererHLSLD3D10::clear(const bool clearColor, const bool clearDepth, const bool clearStencil, const float *color, const float depth, const unsigned int stencil){
	if (clearColor){
		if (currentColorRT[0] == FB_COLOR){
			RendererHLSLD3D10::GetDevice()->ClearRenderTargetView(backBufferRTV, color);
		}

		for (int i = 0; i < MAX_MRTS; i++){
			if (currentColorRT[i] >= 0){
				if (currentColorRTSlice[i] == NO_SLICE){
					RendererHLSLD3D10::GetDevice()->ClearRenderTargetView(textures[currentColorRT[i]].rtv, color);
				} else {
					RendererHLSLD3D10::GetDevice()->ClearRenderTargetView(textures[currentColorRT[i]].rtvArray[currentColorRTSlice[i]], color);
				}
			}
		}
	}
	if (clearDepth || clearStencil){
		UINT clearFlags = 0;
		if (clearDepth)   clearFlags |= D3D10_CLEAR_DEPTH;
		if (clearStencil) clearFlags |= D3D10_CLEAR_STENCIL;

		if (currentDepthRT == FB_DEPTH){
			RendererHLSLD3D10::GetDevice()->ClearDepthStencilView(depthBufferDSV, clearFlags, depth, stencil);
		} else if (currentDepthRT >= 0){
			if (currentDepthSlice == NO_SLICE){
				RendererHLSLD3D10::GetDevice()->ClearDepthStencilView(textures[currentDepthRT].dsv, clearFlags, depth, stencil);
			} else {
				RendererHLSLD3D10::GetDevice()->ClearDepthStencilView(textures[currentDepthRT].dsvArray[currentDepthSlice], clearFlags, depth, stencil);
			}
		}
	}
}

void RendererHLSLD3D10::clearRenderTarget(const TextureID renderTarget, const D3DXFROMWINEVECTOR4 &color, const int slice){
	if (slice == NO_SLICE){
		RendererHLSLD3D10::GetDevice()->ClearRenderTargetView(textures[renderTarget].rtv, color);
	} else {
		RendererHLSLD3D10::GetDevice()->ClearRenderTargetView(textures[renderTarget].rtvArray[slice], color);
	}
}

void RendererHLSLD3D10::clearDepthTarget(const TextureID depthTarget, const float depth, const int slice){
	if (slice == NO_SLICE){
		RendererHLSLD3D10::GetDevice()->ClearDepthStencilView(textures[depthTarget].dsv, D3D10_CLEAR_DEPTH, depth, 0);
	} else {
		RendererHLSLD3D10::GetDevice()->ClearDepthStencilView(textures[depthTarget].dsvArray[slice], D3D10_CLEAR_DEPTH, depth, 0);
	}
}


const D3D10_PRIMITIVE_TOPOLOGY d3d10Prim[] = {
	D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED, // Triangle fans not supported
	D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
	D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED, // Quads not supported
	D3D10_PRIMITIVE_TOPOLOGY_LINELIST,
	D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP,
	D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED, // Line loops not supported
	D3D10_PRIMITIVE_TOPOLOGY_POINTLIST,
};

unsigned int RendererHLSLD3D10::drawArrays(const Primitives primitives, const int firstVertex, const int nVertices){
	if(D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED==d3d10Prim[primitives]) return 0;
	apply();
	RendererHLSLD3D10::GetDevice()->IASetPrimitiveTopology(d3d10Prim[primitives]);
	RendererHLSLD3D10::GetDevice()->Draw(nVertices, firstVertex);
	resetVrtl(RESET_ALL);
	return 0;
}

unsigned int RendererHLSLD3D10::drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices){
	if(D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED==d3d10Prim[primitives]) return 0;
	apply();
	RendererHLSLD3D10::GetDevice()->IASetPrimitiveTopology(d3d10Prim[primitives]);
	RendererHLSLD3D10::GetDevice()->DrawIndexed(nIndices, firstIndex, 0);
	resetVrtl(RESET_ALL);
	return 0;
}

void RendererHLSLD3D10::setup2DMode(const float left, const float right, const float top, const float bottom){
	scaleBias2D.x = 2.0f / (right - left);
	scaleBias2D.y = 2.0f / (top - bottom);
	scaleBias2D.z = -1.0f;
	scaleBias2D.w =  1.0f;
}

ID3D10ShaderResourceView *RendererHLSLD3D10::createSRV(ID3D10Resource *resource, DXGI_FORMAT format, const int firstSlice, const int sliceCount){
	D3D10_RESOURCE_DIMENSION type;
	resource->GetType(&type);

#ifdef USE_D3D10_1
	D3D10_SHADER_RESOURCE_VIEW_DESC/*1*/ srvDesc;
	ID3D10ShaderResourceView/*1*/ *srv;
#else
	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ID3D10ShaderResourceView *srv;
#endif

	switch (type){
		case D3D10_RESOURCE_DIMENSION_TEXTURE1D:
			D3D10_TEXTURE1D_DESC desc1d;
			((ID3D10Texture1D *) resource)->GetDesc(&desc1d);

			srvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc1d.Format;
			if (desc1d.ArraySize > 1){
#ifdef USE_D3D10_1
				srvDesc.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURE2DARRAY;
#else
				srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
#endif
				srvDesc.Texture1DArray.FirstArraySlice = 0;
				srvDesc.Texture1DArray.ArraySize = desc1d.ArraySize;
				srvDesc.Texture1DArray.MostDetailedMip = 0;
				srvDesc.Texture1DArray.MipLevels = desc1d.MipLevels;
			} else {
#ifdef USE_D3D10_1
				srvDesc.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURE1D;
#else
				srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE1D;
#endif
				srvDesc.Texture1D.MostDetailedMip = 0;
				srvDesc.Texture1D.MipLevels = desc1d.MipLevels;
			}
			break;
		case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
			D3D10_TEXTURE2D_DESC desc2d;
			((ID3D10Texture2D *) resource)->GetDesc(&desc2d);

			srvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc2d.Format;
			if (desc2d.ArraySize > 1){
				if (desc2d.MiscFlags & D3D10_RESOURCE_MISC_TEXTURECUBE){
#ifdef USE_D3D10_1
					if (desc2d.ArraySize == 6){
						srvDesc.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURECUBE;
						srvDesc.TextureCube.MostDetailedMip = 0;
						srvDesc.TextureCube.MipLevels = desc2d.MipLevels;
					} /*else {
						srvDesc.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURECUBEARRAY;
						srvDesc.TextureCubeArray.MostDetailedMip = 0;
						srvDesc.TextureCubeArray.MipLevels = desc2d.MipLevels;
						srvDesc.TextureCubeArray.First2DArrayFace = 0;
						srvDesc.TextureCubeArray.NumCubes = desc2d.ArraySize / 6;
					}*/
#else
					srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
					srvDesc.TextureCube.MostDetailedMip = 0;
					srvDesc.TextureCube.MipLevels = desc2d.MipLevels;
#endif
				} else {
#ifdef USE_D3D10_1
					srvDesc.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURE2DARRAY;
#else
					srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
#endif
					if (firstSlice < 0){
						srvDesc.Texture2DArray.FirstArraySlice = 0;
						srvDesc.Texture2DArray.ArraySize = desc2d.ArraySize;
					} else {
						srvDesc.Texture2DArray.FirstArraySlice = firstSlice;
						if (sliceCount < 0){
							srvDesc.Texture2DArray.ArraySize = 1;
						} else {
							srvDesc.Texture2DArray.ArraySize = sliceCount;
						}
					}
					srvDesc.Texture2DArray.MostDetailedMip = 0;
					srvDesc.Texture2DArray.MipLevels = desc2d.MipLevels;
				}
			} else {
#ifdef USE_D3D10_1
				srvDesc.ViewDimension = (desc2d.SampleDesc.Count > 1)? D3D10_1_SRV_DIMENSION_TEXTURE2DMS : D3D10_1_SRV_DIMENSION_TEXTURE2D;
#else
				srvDesc.ViewDimension = (desc2d.SampleDesc.Count > 1)? D3D10_SRV_DIMENSION_TEXTURE2DMS : D3D10_SRV_DIMENSION_TEXTURE2D;
#endif
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = desc2d.MipLevels;
			}
			break;
		case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
			D3D10_TEXTURE3D_DESC desc3d;
			((ID3D10Texture3D *) resource)->GetDesc(&desc3d);

			srvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc3d.Format;

#ifdef USE_D3D10_1
			srvDesc.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURE3D;
#else
			srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE3D;
#endif
			srvDesc.Texture3D.MostDetailedMip = 0;
			srvDesc.Texture3D.MipLevels = desc3d.MipLevels;

			break;
		default:
			LOG_PRINT("Unsupported type");
			return 0;
	}

#ifdef USE_D3D10_1
	if (FAILED(RendererHLSLD3D10::GetDevice()->CreateShaderResourceView/*1*/(resource, &srvDesc, &srv))){
#else
	if (FAILED(RendererHLSLD3D10::GetDevice()->CreateShaderResourceView(resource, &srvDesc, &srv))){
#endif
		LOG_PRINT("CreateShaderResourceView failed");
		return 0;
	}

	return srv;
}

ID3D10RenderTargetView *RendererHLSLD3D10::createRTV(ID3D10Resource *resource, DXGI_FORMAT format, const int firstSlice, const int sliceCount){
	D3D10_RESOURCE_DIMENSION type;
	resource->GetType(&type);

	D3D10_RENDER_TARGET_VIEW_DESC rtvDesc;
	ID3D10RenderTargetView *rtv;

	switch (type){
		case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
			D3D10_TEXTURE2D_DESC desc2d;
			((ID3D10Texture2D *) resource)->GetDesc(&desc2d);

			rtvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc2d.Format;
			if (desc2d.ArraySize > 1){
				rtvDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DARRAY;
				if (firstSlice < 0){
					rtvDesc.Texture2DArray.FirstArraySlice = 0;
					rtvDesc.Texture2DArray.ArraySize = desc2d.ArraySize;
				} else {
					rtvDesc.Texture2DArray.FirstArraySlice = firstSlice;
					if (sliceCount < 0){
						rtvDesc.Texture2DArray.ArraySize = 1;
					} else {
						rtvDesc.Texture2DArray.ArraySize = sliceCount;
					}
				}
				rtvDesc.Texture2DArray.MipSlice = 0;
			} else {
				rtvDesc.ViewDimension = (desc2d.SampleDesc.Count > 1)? D3D10_RTV_DIMENSION_TEXTURE2DMS : D3D10_RTV_DIMENSION_TEXTURE2D;
				rtvDesc.Texture2D.MipSlice = 0;
			}
			break;
		case D3D10_RESOURCE_DIMENSION_TEXTURE3D:
			D3D10_TEXTURE3D_DESC desc3d;
			((ID3D10Texture3D *) resource)->GetDesc(&desc3d);

			rtvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc3d.Format;
			rtvDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE3D;
			if (firstSlice < 0){
				rtvDesc.Texture3D.FirstWSlice = 0;
				rtvDesc.Texture3D.WSize = desc3d.Depth;
			} else {
				rtvDesc.Texture3D.FirstWSlice = firstSlice;
				if (sliceCount < 0){
					rtvDesc.Texture3D.WSize = 1;
				} else {
					rtvDesc.Texture3D.WSize = sliceCount;
				}
			}
			rtvDesc.Texture3D.MipSlice = 0;
			break;
		default:
			LOG_PRINT("Unsupported type");
			return 0;
	}

	if (FAILED(RendererHLSLD3D10::GetDevice()->CreateRenderTargetView(resource, &rtvDesc, &rtv))){
		LOG_PRINT("CreateRenderTargetView failed");
		return 0;
	}

	return rtv;
}

ID3D10DepthStencilView *RendererHLSLD3D10::createDSV(ID3D10Resource *resource, DXGI_FORMAT format, const int firstSlice, const int sliceCount){
	D3D10_RESOURCE_DIMENSION type;
	resource->GetType(&type);

	D3D10_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ID3D10DepthStencilView *dsv;

	switch (type){
		case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
			D3D10_TEXTURE2D_DESC desc2d;
			((ID3D10Texture2D *) resource)->GetDesc(&desc2d);

			dsvDesc.Format = (format != DXGI_FORMAT_UNKNOWN)? format : desc2d.Format;
			if (desc2d.ArraySize > 1){
				dsvDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DARRAY;
				if (firstSlice < 0){
					dsvDesc.Texture2DArray.FirstArraySlice = 0;
					dsvDesc.Texture2DArray.ArraySize = desc2d.ArraySize;
					dsvDesc.Texture2DArray.MipSlice = 0;
				} else {
					dsvDesc.Texture2DArray.FirstArraySlice = firstSlice;
					if (sliceCount < 0){
						dsvDesc.Texture2DArray.ArraySize = 1;
					} else {
						dsvDesc.Texture2DArray.ArraySize = sliceCount;
					}
					dsvDesc.Texture2DArray.MipSlice = 0;
				}
			} else {
				dsvDesc.ViewDimension = (desc2d.SampleDesc.Count > 1)? D3D10_DSV_DIMENSION_TEXTURE2DMS : D3D10_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = 0;
			}
			break;
		default:
			LOG_PRINT("Unsupported type");
			return 0;
	}

	if (FAILED(RendererHLSLD3D10::GetDevice()->CreateDepthStencilView(resource, &dsvDesc, &dsv))){
		LOG_PRINT("CreateDepthStencilView failed");
		return 0;
	}

	return dsv;
}

ubyte *RendererHLSLD3D10::mapRollingVB(const unsigned int size){
	ASSERT(size <= ROLLING_VB_SIZE);

	if (rollingVB == VB_NONE) rollingVB = addVertexBuffer(ROLLING_VB_SIZE, DEFAULT);

	ubyte *data = 0;
	D3D10_MAP flag = D3D10_MAP_WRITE_NO_OVERWRITE;
	if (rollingVBOffset + size > ROLLING_VB_SIZE){
		flag = D3D10_MAP_WRITE_DISCARD;
		rollingVBOffset = 0;
	}

	vertexBuffers[rollingVB].vertexBuffer->Map(flag, 0, (void **) &data);

	return data + rollingVBOffset;
}

void RendererHLSLD3D10::unmapRollingVB(const unsigned int size){
	vertexBuffers[rollingVB].vertexBuffer->Unmap();

	rollingVBOffset += size;
}

unsigned int RendererHLSLD3D10::copyToRollingVB(const void *src, const unsigned int size){
	ASSERT(size <= ROLLING_VB_SIZE);

	if (rollingVB == VB_NONE) rollingVB = addVertexBuffer(ROLLING_VB_SIZE, DYNAMIC);

	ubyte *data = 0;
	D3D10_MAP flag = D3D10_MAP_WRITE_NO_OVERWRITE;
	if (rollingVBOffset + size > ROLLING_VB_SIZE){
		flag = D3D10_MAP_WRITE_DISCARD;
		rollingVBOffset = 0;
	}

	unsigned int offset = rollingVBOffset;
	vertexBuffers[rollingVB].vertexBuffer->Map(flag, 0, (void **) &data);
		stx_memcpy(data + offset, src, size);
	vertexBuffers[rollingVB].vertexBuffer->Unmap();
/*
	D3D10_BOX box;
	box.left   = offset;
	box.right  = offset + size;
	box.top    = 0;
	box.bottom = 1;
	box.front  = 0;
	box.back   = 1;

	RendererHLSLD3D10::GetDevice()->UpdateSubresource(vertexBuffers[rollingVB].vertexBuffer, 0, &box, src, 0, 0);
*/
	rollingVBOffset += size;

	return offset;
}

ID3D10Resource *RendererHLSLD3D10::getResource(const TextureID texture) const {
	return textures[texture].texture;
}

void RendererHLSLD3D10::flush(){
	RendererHLSLD3D10::GetDevice()->Flush();
}

void RendererHLSLD3D10::finish(){
	if (eventQuery == 0){
		D3D10_QUERY_DESC desc;
		desc.Query = D3D10_QUERY_EVENT;
		desc.MiscFlags = 0;
		RendererHLSLD3D10::GetDevice()->CreateQuery(&desc, &eventQuery);
	}

	eventQuery->End();

	RendererHLSLD3D10::GetDevice()->Flush();

	BOOL result = FALSE;
	do {
		eventQuery->GetData(&result, sizeof(BOOL), 0);
	} while (!result);
}








UINT32 RendererHLSLD3D10::GetAvailableTextureMem( )
{
	UINT32 AvailableVideoMem=0;
    // Get availble texture memory
    IDXGIDevice* pDXGIDevice = 0;
    IDXGIAdapter* pDXGIAdapter = 0;
    RendererHLSLD3D10::GetDevice()->QueryInterface( IID_IDXGIDevice, ( void** )&pDXGIDevice ) ;
    pDXGIDevice->GetAdapter( &pDXGIAdapter ) ;
    DXGI_ADAPTER_DESC AdapterDesc;
    pDXGIAdapter->GetDesc( &AdapterDesc );
    if( AdapterDesc.DedicatedVideoMemory )
	AvailableVideoMem = AdapterDesc.DedicatedVideoMemory;
    else
	AvailableVideoMem = AdapterDesc.SharedSystemMemory;
    SAFE_RELEASE( pDXGIDevice );
    SAFE_RELEASE( pDXGIAdapter );

	return AvailableVideoMem;
}

			int RendererHLSLD3D10::deleteVertexBuffer(const VertexBufferID vertexBuffer)
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
			int RendererHLSLD3D10::deleteIndexBuffer(const IndexBufferID indexBuffer)
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
int RendererHLSLD3D10::texturePitchVrtl(const TextureID id,int level)
{
	D3D10_MAPPED_TEXTURE2D m_pLockedRects10;
	((ID3D10Texture2D*)textures[id].texture)->Map( level,
		//D3D10_MAP_WRITE,
		D3D10_MAP_WRITE_DISCARD,
		0, &m_pLockedRects10 );
    return m_pLockedRects10.RowPitch;
}
BYTE *RendererHLSLD3D10::lockTexture(const TextureID id,int level, int& pitch)
{
	D3D10_MAPPED_TEXTURE2D m_pLockedRects10;
	/*
//???D3D10: ERROR: ID3D10Texture2D::Map: Map cannot be called with
	MAP_WRITE access, because the Resource was not created with the
		D3D10_CPU_ACCESS_WRITE flag.
		[ RESOURCE_MANIPULATION ERROR #307: TEXTURE2D_MAP_INVALIDMAPTYPE ]
	*/
	((ID3D10Texture2D*)textures[id].texture)->Map( level,
		//D3D10_MAP_WRITE,
		D3D10_MAP_WRITE_DISCARD,
		0, &m_pLockedRects10 );
	pitch=m_pLockedRects10.RowPitch;
	BYTE *b= (BYTE *)m_pLockedRects10.pData;
	return b;
}
BYTE *RendererHLSLD3D10::unlockTextureVrtl(const TextureID id,int level)
{
	((ID3D10Texture2D*)textures[id].texture)->Unmap( level );
	return 0;
}
BYTE *RendererHLSLD3D10::unlockVertexBuffer(const VertexBufferID id)
{
	return 0;
}
BYTE *RendererHLSLD3D10::unlockIndexBuffer(const IndexBufferID id)
{
	return 0;
}
BYTE *RendererHLSLD3D10::fillVertexBufferVrtl(const VertexBufferID id, BYTE*  m_pData, int l)
{
        RendererHLSLD3D10::GetDevice()->UpdateSubresource( vertexBuffers[id].vertexBuffer, 0, 0, m_pData, 0, 0 );
	return 0;
}
BYTE *RendererHLSLD3D10::fillIndexBufferVrtl(const IndexBufferID id, BYTE*  m_pData, int l)
{
        RendererHLSLD3D10::GetDevice()->UpdateSubresource( indexBuffers[id].indexBuffer, 0, 0, m_pData, 0, 0 );
	return 0;
}
BYTE *RendererHLSLD3D10::lockVertexBuffer(const VertexBufferID id){return 0;}
BYTE *RendererHLSLD3D10::lockIndexBuffer(const IndexBufferID id){return 0;}

void RendererHLSLD3D10::viewport(int x, int y, int viewportWidth, int viewportHeight){
        D3D10_VIEWPORT vp = { x,y, viewportWidth, viewportHeight, 0.0f, 1.0f };
        RendererHLSLD3D10::GetDevice()->RSSetViewports(1, &vp);
  m_viewport.X=x;
  m_viewport.Y=y;
  m_viewport.Width=viewportWidth;
  m_viewport.Height=viewportHeight;

}
AlphaStateID RendererHLSLD3D10::addAlphaState(
		const int func,
		const float ref
	)
{
	AlphaStateD3D10 blendState;
    blendState.blendEnable = true;
	blendState.alphaFunc=GEQUAL;
	alphaStates.push_back(blendState);
	return alphaStates.size()-1;
}



    BYTE* RendererHLSLD3D10::LockTexture(TextureID id, int m, int& pitch)
    {
		return textures[id].Lock(m,pitch);
	}
    void RendererHLSLD3D10::UnlockTexture(TextureID id, int m)
    {
		textures[id].Unlock(m);
	}
    BYTE* RendererHLSLD3D10::LockVertexBuffer(VertexBufferID id)
    {
		return vertexBuffers[id].Lock();
	}
    void RendererHLSLD3D10::UnLockVertexBuffer(VertexBufferID id)
    {
		vertexBuffers[id].Unlock();
	}
    BYTE* RendererHLSLD3D10::LockIndexBuffer(IndexBufferID id)
    {
		return indexBuffers[id].Lock();
	}
    void RendererHLSLD3D10::UnLockIndexBuffer(IndexBufferID id)
    {
		indexBuffers[id].Unlock();
	}

	void RendererHLSLD3D10::GetLevelDesc(TextureID id, unsigned int level,SURFACE_DESC* sDesc)
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
	void RendererHLSLD3D10::GetDesc(VertexBufferID id,VERTEXBUFFER_DESC* sDesc)
	{
		sDesc->Size=vertexBuffers[id].size;
	}
	void RendererHLSLD3D10::GetDesc(IndexBufferID id,INDEXBUFFER_DESC* sDesc)
	{
		//sDesc->Size=indexBuffers[id].nIndices*indexBuffers[id].indexSize;
		sDesc->Format=indexBuffers[id].indexSize;
		sDesc->indexSize=indexBuffers[id].indexSize;
		sDesc->nIndices=indexBuffers[id].nIndices;
	}
	unsigned int RendererHLSLD3D10::GetLevelCount(TextureID id)
	{
				return textures[id].mipMapLevels;
	}
float RendererHLSLD3D10::getTextWidth(const FontID font, const char *str, int length) const {
	if (font < 0) return 0;
    if (length < 0) length = (int) stx_strlen(str);

	Character *chars = fonts[font].chars;

	float len = 0;
	for (int i = 0; i < length; i++){
		len += chars[(unsigned char) str[i]].ratio;
	}

	return len;
}

	void RendererHLSLD3D10::captureScreenshot(Image3& img)
	{
		D3D10_TEXTURE2D_DESC desc;
		desc.Width = IRenderer::GetRendererInstance()->GetViewportWidth();
		desc.Height = IRenderer::GetRendererInstance()->GetViewportHeight();
		desc.Format = backBufferFormat;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = 0;
		desc.MipLevels = 1;
		desc.MiscFlags = 0;
		desc.Usage = D3D10_USAGE_STAGING;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
		bool result = false;
		ID3D10Texture2D *texture;
		if (SUCCEEDED(RendererHLSLD3D10::GetDevice()->CreateTexture2D(&desc, 0, &texture)))
		{
			if (msaaSamples > 1)
			{
				ID3D10Texture2D *resolved = 0;
				desc.Usage = D3D10_USAGE_DEFAULT;
				desc.CPUAccessFlags = 0;
				if (SUCCEEDED(RendererHLSLD3D10::GetDevice()->CreateTexture2D(&desc, 0, &resolved)))
				{
					RendererHLSLD3D10::GetDevice()->ResolveSubresource(resolved, 0, m_pBackBuffer, 0, desc.Format);
					RendererHLSLD3D10::GetDevice()->CopyResource(texture, resolved);
					resolved->Release();
				}
			}
			else
			{
				RendererHLSLD3D10::GetDevice()->CopyResource(texture, m_pBackBuffer);
			}
			D3D10_MAPPED_TEXTURE2D map;
			if (SUCCEEDED(texture->Map(0, D3D10_MAP_READ, 0, &map)))
			{
				if (backBufferFormat == DXGI_FORMAT_R10G10B10A2_UNORM)
				{
					uint32 *dst = (uint32 *) img.create(FORMAT_RGB10A2, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight(), 1, 1);
					ubyte *src = (ubyte *) map.pData;
					for (int y = 0; y < IRenderer::GetRendererInstance()->GetViewportHeight(); y++)
					{
						for (int x = 0; x < IRenderer::GetRendererInstance()->GetViewportWidth(); x++)
						{
							dst[x] = ((uint32 *) src)[x] | 0xC0000000;
						}
						dst += IRenderer::GetRendererInstance()->GetViewportWidth();
						src += map.RowPitch;
					}
				}
				else
				{
					ubyte *dst = img.create(FORMAT_RGB8, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight(), 1, 1);
					ubyte *src = (ubyte *) map.pData;
					for (int y = 0; y < IRenderer::GetRendererInstance()->GetViewportHeight(); y++)
					{
						for (int x = 0; x < IRenderer::GetRendererInstance()->GetViewportWidth(); x++)
						{
							dst[3 * x + 0] = src[4 * x + 0];
							dst[3 * x + 1] = src[4 * x + 1];
							dst[3 * x + 2] = src[4 * x + 2];
						}
						dst += IRenderer::GetRendererInstance()->GetViewportWidth() * 3;
						src += map.RowPitch;
					}
				}
				result = true;
				texture->Unmap(0);
			}
			texture->Release();
		}
	}
	void RendererHLSLD3D10::SaveScreenShot3(const char* fn)
	{
		Image3 img;
		captureScreenshot(img);
/*
		ID3D10Resource* id=0;
		D3DX10SaveTextureToFileA(
		id,
		D3DX10_IFF_JPG,
		fn
		);
*/
		int l=0;
        l=int(stx_strrchr(fn,'.')-fn);

		std::string fns(fn);
		if(l)fns.resize(l);
					char bufpng[256];
                    stx_snprintf(bufpng,256,"%s.png",fns.c_str());
					img.saveImageLibImage(bufpng,false);

	}
	void RendererHLSLD3D10::SaveScreenShotVrtl(const char* fn)
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
                stx_snprintf(buf,256,"../%s.jpg","ScreenShot.jpg");
                stx_snprintf(gifbuf,256,"../%s.jpg","ScreenShot.gif");
			}

			captureScreenshot(img);
			img.saveImageLibImage(buf,false);
			RendererHLSLCg::OpenILToGIF(img,gifbuf);
		}
	}

void RendererHLSLD3D10::buildPerspectiveMatrix(float fieldOfView,
                                   float aspectRatio,
                                   float zNear, float zFar)//,D3DXFROMWINEMATRIX& m)
{
buildPerspectiveMatrixD3D(fieldOfView,
                                   aspectRatio,
                                   zNear, zFar,(*(D3DXFROMWINEMATRIX*)pp));
	m_fieldOfView=fieldOfView;
    m_aspectRatio=aspectRatio;
    m_zNear=zNear;
    m_zFar=zFar;
//	p=m;

}
        void RendererHLSLD3D10::buildLookAtMatrix(float eyex, float eyey, float eyez,
                              float centerx, float centery, float centerz,
                              float upx, float upy, float upz)//,D3DXFROMWINEMATRIX& m)
        {
#if 1
buildLookAtMatrixD3D(eyex, eyey, eyez,
                              centerx, centery, centerz,
                              upx, upy, upz,(*(D3DXFROMWINEMATRIX*)vv));
#else
	D3DXFROMWINEVECTOR3 c1=D3DXFROMWINEVECTOR3(centerx-eyex,centery-eyey,centerz-eyez);

		mv=identity4();

if(_isinf(c1.x)) return;
if(_isinf(c1.y)) return;
if(_isinf(c1.z)) return;
	float radius=0.0f; float phi=0.0f; float theta=0.0f;
	float f=c1.x*c1.x+c1.y*c1.y+c1.z*c1.z;
	if(f<=0.0000001f) return;
	radius=sqrt(f);
	phi = asinf(c1.z / (radius));
	if(0.0<=c1.y)
		theta = acosf(c1.x / ((radius)* cosf((phi))));
	else if(c1.y<0.0f)
		theta = -acosf(c1.x /((radius)*cosf((phi))));
	mv=
	rotateXY(phi,theta)
	*translate(-D3DXFROMWINEVECTOR3(eyex, eyey, eyez));
#endif
	m_eye=D3DXFROMWINEVECTOR3(eyex, eyey, eyez);
	m_center=D3DXFROMWINEVECTOR3(centerx, centery, centerz);
    m_up=D3DXFROMWINEVECTOR3(upx, upy, upz);
//mv=m;
        }

int RendererHLSLD3D10::GetTextureWidth(TextureID id)
{
	if(id<textures.size())
		return textures[id].width;
	return 0;
}

int RendererHLSLD3D10::GetTextureHeight(TextureID id)
{
	if(id<textures.size())
		return textures[id].height;
	return 0;
}

void RendererHLSLD3D10::PresentVrtl(bool aB)
{
	bool b=true;//((aB&&(m_pInput!=0)) || ((!aB)&&(m_pInput==0)));
	if(b)
	{

	IRenderer::OnPresent();

	#if !defined(WIN_PHONE_APP)
	m_pSwapChain->Present( 0, 0 );
	#endif
}
}

	unsigned int RendererHLSLD3D10::DrawPrimitive(Primitives PrimitiveType,UINT StartVertex,UINT PrimitiveCount, const unsigned int flags)
	{
		//if(!((currentVertexBuffers[0]>-1)))
		if(!((selectedVertexBuffers[0]>-1)))
			return 0;
		apply();
		drawArrays(		PrimitiveType,
						StartVertex,
						getVertexCount(PrimitiveType,PrimitiveCount));
	resetVrtl(RESET_ALL);
	nDrawCalls++;
	return 0;
	}
	unsigned int RendererHLSLD3D10::DrawIndexedPrimitive(Primitives PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, const unsigned int flags)
	{
//if(!((currentVertexBuffers[0]>-1)&&(currentIndexBuffer>-1)))
if(!((selectedVertexBuffers[0]>-1)&&(selectedIndexBuffer>-1)))
	return 0;
		apply();
		drawElements(	PrimitiveType,
						startIndex,
						getIndicesCount(PrimitiveType)*primCount,
						0,
						NumVertices);
	resetVrtl(RESET_ALL);
	nDrawCalls++;
	return 0;
	}

#if 1
TextureID RendererHLSLD3D10::addTexture(Image3* img, const bool useMips, const SamplerStateID samplerState, unsigned int flags)
{return addTexture(*img, samplerState, flags);}
#else
TextureID RendererHLSLD3D10::addTexture(Image3* img, const bool useMips, const SamplerStateID samplerState, unsigned int flags)
{
	TextureD3D10 tex;
	stx_memset(&tex, 0, sizeof(TextureD3D10));
	//img->BindImage();
	tex.ss=samplerState;
	tex.width=img->getWidth();
	tex.height=img->getHeight();
	tex.depth=img->getDepth();
	if(useMips&&img->getMipMapCount()<2)
		img->createMipMaps();
	tex.mipMapLevels=img->getMipMapCount();
#if 1


	switch (img->getFormat()){
		case FORMAT_RGB8:
			img->convert(FORMAT_RGBA8);
//			img->convert(FORMAT_RGB10A2);
			break;
		case FORMAT_RGB16:
			img->convert(FORMAT_RGBA16);
			break;
		case FORMAT_RGB16F:
			img->convert(FORMAT_RGBA16F);
			break;
		case FORMAT_RGB32F:
			img->convert(FORMAT_RGBA32F);
			break;
	}

	FORMAT format = img->getFormat();
	unsigned int nMipMaps = img->getMipMapCount();
	unsigned int nSlices = img->isCube()? 6 : 1;
	unsigned int arraySize = img->getArraySize();

	static D3D10_SUBRESOURCE_DATA texData[1024];
	D3D10_SUBRESOURCE_DATA *dest = texData;
	//for (unsigned int n = 0; n < arraySize; n++){
		for (unsigned int k = 0; k < nSlices; k++){
			for (unsigned int i = 0; i < nMipMaps; i++){
				unsigned int pitch, slicePitch;
				if (isCompressedFormat(format)){
					pitch = ((img->getWidth(i) + 3) >> 2) * getBytesPerBlock(format);
					slicePitch = pitch * ((img->getHeight(i) + 3) >> 2);
				} else {
					pitch = img->getWidth(i) * getBytesPerPixel(format);
					slicePitch = pitch * img->getHeight(i);
				}

				dest->pSysMem = img->getPixels(i/*, n*/) + k * slicePitch;
				dest->SysMemPitch = pitch;
				dest->SysMemSlicePitch = slicePitch;
				dest++;
			}
		}
	//}

	tex.texFormat = formatsd3d10[format];
	if (flags & SRGB){
		// Change to the matching sRGB format
		switch (tex.texFormat){
			case DXGI_FORMAT_R8G8B8A8_UNORM:
				tex.texFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
				break;
			case DXGI_FORMAT_BC1_UNORM: tex.texFormat =
				DXGI_FORMAT_BC1_UNORM_SRGB;
				break;
			case DXGI_FORMAT_BC2_UNORM: tex.texFormat =
				DXGI_FORMAT_BC2_UNORM_SRGB;
				break;
			case DXGI_FORMAT_BC3_UNORM: tex.texFormat =
				DXGI_FORMAT_BC3_UNORM_SRGB;
				break;
		}
	}

	HRESULT hr;
	if (img->is1D()){
		D3D10_TEXTURE1D_DESC desc;
		desc.Width  = img->getWidth();
		desc.Format = tex.texFormat;
		desc.MipLevels = nMipMaps;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		desc.ArraySize = 1;
		desc.MiscFlags = 0;

		if((img->getFormat()>=FORMAT_R16F) && (FORMAT_RGBA32F<=img->getFormat()))
		{
			desc.Usage = D3D10_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		}
		else
		{
			desc.Usage = D3D10_USAGE_IMMUTABLE;
			desc.CPUAccessFlags = 0;
		}

		hr = RendererHLSLD3D10::GetDevice()->CreateTexture1D(&desc, texData, (ID3D10Texture1D **) &tex.texture);
	} else if (img->is2D() || img->isCube()){
		D3D10_TEXTURE2D_DESC desc;
		desc.Width  = img->getWidth();
		desc.Height = img->getHeight();
		desc.Format = tex.texFormat;
		desc.MipLevels = nMipMaps;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		
		if((img->getFormat()>=FORMAT_R16F) && (FORMAT_RGBA32F<=img->getFormat()))
		{
			desc.Usage = D3D10_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		}
		else
		{
			desc.Usage = D3D10_USAGE_IMMUTABLE;
			desc.CPUAccessFlags = 0;
		}
		
		if (img->isCube()){
			desc.ArraySize = 6 * arraySize;
			desc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;
		} else {
			desc.ArraySize = 1;
			desc.MiscFlags = 0;
		}

		hr = RendererHLSLD3D10::GetDevice()->CreateTexture2D(&desc, texData, (ID3D10Texture2D **) &tex.texture);
	} else if (img->is3D()){
		D3D10_TEXTURE3D_DESC desc;
		desc.Width  = img->getWidth();
		desc.Height = img->getHeight();
		desc.Depth  = img->getDepth();
		desc.Format = tex.texFormat;
		desc.MipLevels = nMipMaps;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		desc.MiscFlags = 0;
		
		if((img->getFormat()>=FORMAT_R16F) && (FORMAT_RGBA32F<=img->getFormat()))
		{
			desc.Usage = D3D10_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		}
		else
		{
			desc.Usage = D3D10_USAGE_IMMUTABLE;
			desc.CPUAccessFlags = 0;
		}

		hr = RendererHLSLD3D10::GetDevice()->CreateTexture3D(&desc, texData, (ID3D10Texture3D **) &tex.texture);
	}

	if (FAILED(hr)){
		LOG_PRINT("Couldn't create texture");
		return TEXTURE_NONE;
	}
#else
							switch (img->getFormat())
							{
								case FORMAT_RGB8:
								img->convert(FORMAT_RGBA8);
								break;
								case FORMAT_RGB16:
								img->convert(FORMAT_RGBA16);
								break;
								case FORMAT_RGB16f:
								img->convert(FORMAT_RGBA16f);
								break;
								case FORMAT_RGB32f:
								img->convert(FORMAT_RGBA32f);
								break;
							}
							const FORMAT format = img->getFormat();
										if(alpha>0.0f)
								{
			#if 1
									if(getChannelCount(format)==4){img->removeChannels(true,true,true,false);}
									img->addChannel(alpha);
			#else
									img->format(FORMAT_RGBA8);
									img->setAlpha(alpha);
									format = img->getFormat();
			#endif

								}
							////img->swapChannels2(0, 2);
							//img->swapChannels(0, 2);
				//if (format == FORMAT_RGB8 || format == FORMAT_RGBA8) img->swap(0, 2);

							unsigned int nMipMaps = img->getMipMapCount();
							unsigned int nSlices = img->isCube()? 6 : 1;
							unsigned int arraySize = img->getArraySize();
							static D3D10_SUBRESOURCE_DATA texData[1024];
							D3D10_SUBRESOURCE_DATA *dest = texData;
							for (unsigned int n = 0; n < arraySize; n++)
							{
								for (unsigned int k = 0; k < nSlices; k++)
								{
									for (unsigned int i = 0; i < nMipMaps; i++)
									{
										unsigned int pitch, slicePitch;
										if (isCompressedFormat(format))
										{
											pitch = ((img->getWidth(i) + 3) >> 2) * getBytesPerBlock(format);
											slicePitch = pitch * ((img->getHeight(i) + 3) >> 2);
										}
										else
										{
											pitch = img->getWidth(i) * getBytesPerPixel(format);
											slicePitch = pitch * img->getHeight(i);
										}
										dest->pSysMem = img->getPixels(i) + k * slicePitch;
										dest->SysMemPitch = pitch;
										dest->SysMemSlicePitch = slicePitch;
										dest++;
									}
								}
							}
							tex.texFormat = formatsd3d10[format];
							if (flags & SRGB)
							{
								switch (tex.texFormat)
								{
									case DXGI_FORMAT_R8G8B8A8_UNORM: tex.texFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; break;
									case DXGI_FORMAT_BC1_UNORM: tex.texFormat = DXGI_FORMAT_BC1_UNORM_SRGB; break;
									case DXGI_FORMAT_BC2_UNORM: tex.texFormat = DXGI_FORMAT_BC2_UNORM_SRGB; break;
									case DXGI_FORMAT_BC3_UNORM: tex.texFormat = DXGI_FORMAT_BC3_UNORM_SRGB; break;
								}
							}
							HRESULT hr;
							if (img->is1D())
							{
								tex.dim=1;
								D3D10_TEXTURE1D_DESC desc;
								desc.Width  = img->getWidth();
								desc.Format = tex.texFormat;
								desc.MipLevels = (flags&DYNAMIC)?1:nMipMaps;
								desc.Usage = (flags&DYNAMIC)?

			    //D3D10_USAGE_DEFAULT
				//D3D10_USAGE_IMMUTABLE
				D3D10_USAGE_DYNAMIC
				//D3D10_USAGE_STAGING

			:D3D10_USAGE_DEFAULT;//D3D10_USAGE_IMMUTABLE;
								desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
			//					desc.CPUAccessFlags = 0;
			desc.CPUAccessFlags = (flags&DYNAMIC)? D3D10_CPU_ACCESS_WRITE : 0;


								desc.ArraySize = 1;
								desc.MiscFlags = 0;
			if (flags==STAGING)
			{
			            desc.Usage = D3D10_USAGE_STAGING;
			            desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;//0;
			            desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			            hr = RendererHLSLD3D10::GetDevice()->CreateTexture1D( &desc, 0, (ID3D10Texture1D **)&tex.pStaging );
			}
								hr = RendererHLSLD3D10::GetDevice()->CreateTexture1D(&desc, texData, (ID3D10Texture1D **) &tex.texture);
							}
							else if (img->is2D() || img->isCube())
							{
								tex.dim=2;
								D3D10_TEXTURE2D_DESC desc;
								desc.Width  = img->getWidth();
								desc.Height = img->getHeight();
								desc.Format = tex.texFormat;
								desc.MipLevels = (flags&DYNAMIC)?1:nMipMaps;
								desc.SampleDesc.Count = 1;
								desc.SampleDesc.Quality = 0;
								desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
								desc.Usage = (flags&DYNAMIC)?

			    //D3D10_USAGE_DEFAULT
				//D3D10_USAGE_IMMUTABLE
				D3D10_USAGE_DYNAMIC
				//D3D10_USAGE_STAGING

			:D3D10_USAGE_DEFAULT;//D3D10_USAGE_IMMUTABLE;
			//					desc.CPUAccessFlags = 0;
			desc.CPUAccessFlags = (flags&DYNAMIC)? D3D10_CPU_ACCESS_WRITE : 0;

								if (img->isCube())
								{
									desc.ArraySize = 6 * arraySize;
									desc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;
								}
								else
								{
									desc.ArraySize = 1;
									desc.MiscFlags = 0;
								}


			if (flags==STAGING)
			{
			            desc.Usage = D3D10_USAGE_STAGING;
			            desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;//0;
			            desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			            hr = RendererHLSLD3D10::GetDevice()->CreateTexture2D( &desc, 0, (ID3D10Texture2D **)&tex.pStaging );
			}

								hr = RendererHLSLD3D10::GetDevice()->CreateTexture2D(&desc, texData, (ID3D10Texture2D **) &tex.texture);
							}
							else if (img->is3D())
							{
								tex.dim=3;
								D3D10_TEXTURE3D_DESC desc;
								desc.Width  = img->getWidth();
								desc.Height = img->getHeight();
								desc.Depth  = img->getDepth();
								desc.Format = tex.texFormat;
								desc.MipLevels = (flags&DYNAMIC)?1:nMipMaps;
								desc.Usage = (flags&DYNAMIC)?

			    //D3D10_USAGE_DEFAULT
				//D3D10_USAGE_IMMUTABLE
				D3D10_USAGE_DYNAMIC
				//D3D10_USAGE_STAGING

			:D3D10_USAGE_DEFAULT;//D3D10_USAGE_IMMUTABLE;
								desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
			//					desc.CPUAccessFlags = 0;
			desc.CPUAccessFlags = (flags&DYNAMIC)? D3D10_CPU_ACCESS_WRITE : 0;
								desc.MiscFlags = 0;
			if (flags==STAGING)
			{
			            desc.Usage = D3D10_USAGE_STAGING;
			            desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;//0;
			            desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			            hr = RendererHLSLD3D10::GetDevice()->CreateTexture3D( &desc, 0, (ID3D10Texture3D **)&tex.pStaging );
			}
								hr = RendererHLSLD3D10::GetDevice()->CreateTexture3D(&desc, texData, (ID3D10Texture3D **) &tex.texture);
							}
							if (FAILED(hr))
							{
			/*
			(1508): RESOURCE WARNING: Device needs to create new Texture
			D3D10: ERROR: ID3D10Device::CreateTexture2D: A D3D10_USAGE_IMMUTABLE Resource cannot have any CPUAccessFlags set. The following CPUAccessFlags bits cannot be set in this case: D3D10_CPU_ACCESS_READ (0), D3D10_CPU_ACCESS_WRITE (1). [ STATE_CREATION ERROR #98: CREATETEXTURE2D_INVALIDCPUACCESSFLAGS ]
			First-chance exception at 0x76f142eb in ContentStreaming9-debug.exe: Microsoft C++ exception: _com_error at memory location 0x0012e9b4..
			D3D10: ERROR: ID3D10Device::CreateTexture2D: Returning E_INVALIDARG, meaning invalid parameters were passed. [ STATE_CREATION ERROR #104: CREATETEXTURE2D_INVALIDARG_RETURN ]
			(1508): Couldn't create textureFirst-chance exception at 0x0086326f in ContentStreaming9-debug.exe: 0xC0000005: Access violation reading location 0xffffffff.
			Unhandled exception at 0x0086326f in ContentStreaming9-debug.exe: 0xC0000005: Access violation reading location 0xffffffff.

			D3D10: ERROR: ID3D10Device::CreateTexture2D: A D3D10_USAGE_DYNAMIC Resource must have MipLevels equal to 1. [ STATE_CREATION ERROR #102: CREATETEXTURE2D_INVALIDMIPLEVELS ]
			First-chance exception at 0x76f142eb in ContentStreaming9-debug.exe: Microsoft C++ exception: _com_error at memory location 0x0012e9b4..
			D3D10: ERROR: ID3D10Device::CreateTexture2D: Returning E_INVALIDARG, meaning invalid parameters were passed. [ STATE_CREATION ERROR #104: CREATETEXTURE2D_INVALIDARG_RETURN ]
			(932): Couldn't create textureFirst-chance exception at 0x0086329b in ContentStreaming9-debug.exe: 0xC0000005: Access violation reading location 0xffffffff.
			Unhandled exception at 0x0086329b in ContentStreaming9-debug.exe: 0xC0000005: Access violation reading location 0xffffffff.

			*/
								/*
			D3D10: ERROR: ID3D10Device::CreateShaderResourceView1: A ShaderResourceView cannot be created of a Resource that did not specify the D3D10_BIND_SHADER_RESOURCE BindFlag. [ STATE_CREATION ERROR #129: CREATESHADERRESOURCEVIEW_INVALIDRESOURCE ]
			First-chance exception at 0x77c742eb in ContentStreaming9-debug.exe: Microsoft C++ exception: _com_error at memory location 0x0012e92c..
			D3D10: ERROR: ID3D10Device::CreateShaderResourceView1: Returning E_INVALIDARG, meaning invalid parameters were passed. [ STATE_CREATION ERROR #131: CREATESHADERRESOURCEVIEW_INVALIDARG_RETURN ]
			(3332): CreateShaderResourceView failedFirst-chance exception at 0x00882b83 in ContentStreaming9-debug.exe: 0xC0000005: Access violation reading location 0xffffffff.
			Unhandled exception at 0x00882b83 in ContentStreaming9-debug.exe: 0xC0000005: Access violation reading location 0xffffffff.

								*/
								LOG_PRINT("Couldn't create texture");
								return TEXTURE_NONE;
							}
#endif
							tex.srvFormat = tex.texFormat;
							tex.srv = createSRV(tex.texture, tex.srvFormat);
							return textures.add(tex);
						}
#endif
	void RendererHLSLD3D10::Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors)
	{
		if(color)
		{
			float ClearColor[4] =
			{
				colors.x,colors.y,colors.z,colors.w
			}
			;
			RendererHLSLD3D10::GetDevice()->ClearRenderTargetView( RTV, ClearColor );
		}
		if(depth) RendererHLSLD3D10::GetDevice()->ClearDepthStencilView( DSV, D3D10_CLEAR_DEPTH, 1.0, 0 );
	}

    void RendererHLSLD3D10::changeAlphaState(const AlphaStateID blendState)
    {
    }
void RendererHLSLD3D10::setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState, const int vtstream, const VertexBufferID vtbuffer)
{
	if(texture==-1) return; // ???? DBG_HALT;
	if(selectedShader == SHADER_NONE) return;
	if(!shaders[selectedShader].textures.size()) return; // ????
	const SamplerD3D10* s = (const SamplerD3D10 *)getSampler(shaders[selectedShader].textures.getArray(), (shaders[selectedShader]).nTextures, textureName);
	if (s)
	{
		if (s->vsIndex >= 0)
		{
			selectedTexturesVS[s->vsIndex] = texture;
			selectedTextureSlicesVS[s->vsIndex] = NO_SLICE;
			selectedSamplerStatesVS[s->vsIndex] = samplerState;
		}
		if (s->gsIndex >= 0)
		{
			selectedTexturesGS[s->gsIndex] = texture;
			selectedTextureSlicesGS[s->gsIndex] = NO_SLICE;
			selectedSamplerStatesGS[s->gsIndex] = samplerState;
		}
		if (s->psIndex >= 0)
		{
			selectedTexturesPS[s->psIndex] = texture;
			selectedTextureSlicesPS[s->psIndex] = NO_SLICE;
			selectedSamplerStatesPS[s->psIndex] = samplerState;
		}

	}
	#ifdef _DEBUG
	else
	{
		char str[256];
        stx_snprintf(str,256, "Invalid texture \"%s\"", textureName);
		LOG_PRINT("%s", str);
	}
	#endif
	//setSamplerState(textureName, samplerState);
}
unsigned int RendererHLSLD3D10::drawPrimitivesVrtl(	const Primitives primitives,
				const unsigned int firstv, const unsigned int countv,
				const unsigned int firsti, const unsigned int counti,
				const ubyte* vbdata, const unsigned int vbsize,
				const ubyte* ibdata, const unsigned int ibsize)
{
	//LOG_FNLN;
	//LOG_PRINT("IMPLEMENT\n");
#if 0
	ID3D11Buffer *vertexBuffer=0;
	ID3D11Buffer *indexBuffer=0;

	const unsigned int bufferAccess = DYNAMIC;

	D3D11_BUFFER_DESC desc;
	desc.Usage = usage[bufferAccess];
	desc.ByteWidth = vbsize;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = (bufferAccess == DYNAMIC)? D3D11_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = vbdata;
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;

	if (FAILED(RendererHLSLD3D10::GetDevice()->CreateBuffer(&desc, data? &vbData : 0, &vertexBuffer))){
        LOG_PRINT("Couldn't create vertex buffer");
		return VB_NONE;
	}

	if(ibdata)
	{
	D3D11_BUFFER_DESC desc;
	desc.Usage = usage[bufferAccess];
	desc.ByteWidth = counti * ibsize;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = (bufferAccess == DYNAMIC)? D3D11_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData;
	ibData.pSysMem = ibdata;
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;

	if (FAILED(RendererHLSLD3D10::GetDevice()->CreateBuffer(&desc, data? &ibData : 0, &indexBuffer))){
        LOG_PRINT("Couldn't create index buffer");
		return IB_NONE;
	}
	}

	UINT32 vertexSize = vbsize;
	UINT32 offset = 0;
	ic->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
	DXGI_FORMAT format = indexBuffers[indexBufferID].indexSize < 4? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	if(ibdata)
		ic->IASetIndexBuffer(&indexBuffer, format, 0);
	ic->IASetPrimitiveTopology(d3d11Prim[primitives]);

	if (ibdata)
		ic->DrawIndexed(counti, firsti, firstv);
        else
        	ic->Draw(countv, firstv);

	if (vertexBuffer) { vertexBuffer->Release(); vertexBuffer=NULL;}
	if (indexBuffer) { indexBuffer->Release(); indexBuffer=NULL;}
#else
	return -1;
#endif
}

#if 0
struct stx_EffectHLSLD3D10 : public stx_Effect
{
protected:
	static ID3D10Effect* m_pEffect;
public:
	static ID3D10Effect* GetNativeEffect() { return m_pEffect; }
	struct stx_PassHLSLD3D10
	{
	    struct stx_TextureHLSLD3D10
	    {
		    stx_TextureHLSLD3D10()
		    {
                //<Texture name="Base" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
		        texID=-1;
                m_iDimension=2;
                m_bUseMipmap=false;
                ss=-1;
            }
		    virtual ~stx_TextureHLSLD3D10(){}
		    stx_TextureHLSLD3D10(const stx_TextureHLSLD3D10& rhs)
		    {
			    m_sName=rhs.m_sName;
                m_sFile=rhs.m_sFile;
		        texID=rhs.texID;
                m_iDimension=rhs.m_iDimension;
                m_bUseMipmap=rhs.m_bUseMipmap;
                ss=rhs.ss;
            }
		        std::string m_sName;
		        std::string m_sFile;
		        TextureID texID;
                unsigned int m_iDimension;
                bool m_bUseMipmap;
                SamplerStateID ss;
        };
		stx_PassHLSLD3D10()
		{
        		VertexFormat=-1;
			SamplerState=-1;
			BlendState=-1;
			AlphaState=-1;
        		DepthStencilState=-1;
        		RasterizerState=-1;
		}
		stx_PassHLSLD3D10(const std::string& aFileName, std::vector<std::string>& avMain, std::vector<ShaderType>& avType)
		{
			VertexFormat=-1;
			SamplerState=-1;
			BlendState=-1;
			AlphaState=-1;
        		DepthStencilState=-1;
        		RasterizerState=-1;
                m_pDesc=0;
		}
		virtual ~stx_PassHLSLD3D10(){}
		void Apply(unsigned int aI){} // ???
		stx_PassHLSLD3D10(const stx_PassHLSLD3D10& rhs)
		{
			m_sName=rhs.m_sName;
        		VertexFormat=rhs.VertexFormat;
        		format_=rhs.format_;
			SamplerState=rhs.SamplerState;
			SamplerName=rhs.SamplerName;
			BlendState=rhs.BlendState;
			AlphaState=rhs.AlphaState;
        		DepthStencilState=rhs.DepthStencilState;
        		RasterizerState=rhs.RasterizerState;
                m_pDesc=rhs.m_pDesc;
		}
		std::string m_sName;
		std::vector<int> format_;
        D3D10_INPUT_ELEMENT_DESC* m_pDesc;
        VertexFormatID VertexFormat;
		SamplerStateID SamplerState;
		std::string SamplerName;
		BlendStateID BlendState;
		AlphaStateID AlphaState;
        	DepthStateID DepthStencilState;
        	RasterizerStateID RasterizerState;
		//std::vector<stx_TextureHLSLD3D10> m_vTextures;
	};
	struct stx_VariableHLSLD3D10
	{
        	std::string m_sName;
		stx_VariableHLSLD3D10(){}
		virtual ~stx_VariableHLSLD3D10(){}
		stx_VariableHLSLD3D10(const stx_VariableHLSLD3D10& rhs)
		{
            		m_sName=rhs.m_sName;
		}

	void SetResourceArray( ShaderResourceViewID* id, unsigned int startindex, unsigned int aN )
	{
		for(unsigned int i=startindex;i<aN;i++)
		{
			SetTexture(id[i]); // sampler name ???
		}
	}
	void SetResource( ShaderResourceViewID id ){SetTexture(id);}
        void SetFloat( float aValue )
        {
    		ID3D10EffectScalarVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(m_sName.c_str())->AsScalar();
	        if(pVariable) pVariable->SetFloat(aValue);
        }
        void SetMatrix( float* aValue )
        {
    		ID3D10EffectMatrixVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(m_sName.c_str())->AsMatrix();
	        if(pVariable) pVariable->SetMatrix((float*)aValue);
        }
        void SetMatrix( D3DXFROMWINEMATRIX* aValue )
        {
    		ID3D10EffectMatrixVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(m_sName.c_str())->AsMatrix();
	        if(pVariable) pVariable->SetMatrix((float*)aValue);
        }
        void SetVector( D3DXFROMWINEVECTOR4* aValue )
        {
    		ID3D10EffectVectorVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(m_sName.c_str())->AsVector();
	        if(pVariable) pVariable->SetFloatVector((float*)aValue);
        }
        void SetInt( int* aValue )
        {
    		ID3D10EffectScalarVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(m_sName.c_str())->AsScalar();
	        if(pVariable) pVariable->SetInt(*aValue);
        }
        void SetBool( bool aValue )
        {
    		ID3D10EffectScalarVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(m_sName.c_str())->AsScalar();
	        if(pVariable) pVariable->SetBool(aValue);
        }
        void SetTexture( TextureID aValue )
        {
			ID3D10EffectShaderResourceVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(m_sName.c_str())->AsShaderResource();
	        if(pVariable) pVariable->SetResource( RendererHLSLD3D10::GetRenderer()->textures[aValue].srv );
        }
        void SetValue( void* aPtr, unsigned int aSize )
        {
    		ID3D10EffectScalarVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(m_sName.c_str())->AsScalar();
	        if(pVariable) pVariable->SetRawValue(aPtr, 0, aSize);
        }
        void SetIntVector( int* aValue )
        {
    		ID3D10EffectScalarVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(m_sName.c_str())->AsScalar();
	        if(pVariable) pVariable->SetIntArray(aValue, 0, elementsOf(aValue));
        }
        void SetFloatVector( float* aValue )
        {
    		ID3D10EffectScalarVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(m_sName.c_str())->AsScalar();
	        if(pVariable) pVariable->SetFloatArray(aValue, 0, elementsOf(aValue));
        }
        void SetFloatVectorArray( float* aValue, unsigned int aS, unsigned int aN )
        {
    		ID3D10EffectScalarVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(m_sName.c_str())->AsScalar();
	        if(pVariable) pVariable->SetFloatArray(aValue, aS, aN);
        }
	void SetFloatArray( float* aValue, unsigned int aS, unsigned int aN )
        {
    		ID3D10EffectScalarVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(m_sName.c_str())->AsScalar();
	        if(pVariable) pVariable->SetFloatArray(aValue, aS, aN);
        }
        void SetMatrixArray( void* aPtr, unsigned int aS, unsigned int aN)
        {
    		ID3D10EffectMatrixVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(m_sName.c_str())->AsMatrix();
	        if(pVariable) pVariable->SetMatrixArray((float*)aPtr, aS, aN);
        }
	};
	struct stx_TechniqueHLSLD3D10
	{
		stx_TechniqueHLSLD3D10(stx_EffectHLSLD3D10* pEffect=0){}
		virtual ~stx_TechniqueHLSLD3D10(){}
		stx_TechniqueHLSLD3D10(const stx_TechniqueHLSLD3D10& rhs)
		{
			m_pTechnique=rhs.m_pTechnique;
			m_pInputLayout=rhs.m_pInputLayout;
			m_sName=rhs.m_sName;
			m_vPasses=rhs.m_vPasses;
		}
		void GetDesc( TECHNIQUE_DESC* techDesc ){if(techDesc)techDesc->Passes=m_vPasses.size();}
		stx_PassHLSLD3D10* GetPassByIndex(unsigned int aI)
		{
			if((aI+1)>m_vPasses.size())
				return 0;
			return &m_vPasses[aI];
		} // ???
		stx_PassHLSLD3D10* GetPassByName(const std::string& aName)
		{
			int idx=-1;
			for(unsigned int i=0;i<m_vPasses.size();i++)
			{
				if(m_vPasses[i].m_sName==aName)
				{
					idx=i;
					break;
				}
			}
			if(idx>-1)
				return &m_vPasses[idx];
			return 0;
		} // ???
		std::string m_sName;
		ID3D10EffectTechnique* m_pTechnique;
		ID3D10InputLayout* m_pInputLayout;
		std::vector<stx_PassHLSLD3D10> m_vPasses;
		std::vector<stx_VariableHLSLD3D10> m_vVariables;
	};
	std::vector<stx_EffectHLSLD3D10::stx_PassHLSLD3D10::stx_TextureHLSLD3D10> m_vTextures;
	std::vector<stx_TechniqueHLSLD3D10> m_vTechniques;
	std::string m_sTechnique;
	int  m_iTechnique;
	std::string m_sFileName;
public:
	stx_EffectHLSLD3D10():stx_Effect(){m_iTechnique=-1;m_pEffect=0;}
	stx_EffectHLSLD3D10(const std::string& aFileName, const char* aDefines=0):stx_Effect()
    {
        m_iTechnique=-1;m_pEffect=0;
        std::string str="#define ROW_MAJOR row_major\n#define MVPSEMANTIC\n#define WSIGN +\n#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) TEX\.Load\(TEXCOORD\)\n#define SAMPLER2D Texture2D\n";
	str.append(ParseEffect(aFileName, aDefines));
        str.append("\ntechnique10 Simple\n{\n\tpass P0\n\t{\n\t\tSetVertexShader( CompileShader( vs_4_0, mainVS( ) ) );\n\t\tSetGeometryShader( NULL );\n\t\tSetPixelShader( CompileShader( ps_4_0, mainPS( ) ) );\n\t\tSetRasterizerState( Solid );\n\t\tSetBlendState( DisableBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );\n\t\tSetDepthStencilState( EnableDepth, 0 );\n\t}\n}\n");
        std::ofstream out("./__tmp__.fx");
        out << str.c_str();
        out.close();
		{ID3DBlob* pEffectBuffer = NULL;
		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
		ID3DBlob* pErrorBlob;HREDULT hr = 0;
#if 1
    ID3D10Blob *pErrors;
    hr = D3DX10CreateEffectFromFile("./__tmp__.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, RendererHLSLD3D10::GetDevice(), NULL, NULL, &m_pEffect, &pErrors, &hr);
#elif 0
    hr=D3DX10CreateEffectFromFile( "./__tmp__.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, RendererHLSLD3D10::GetDevice(), NULL,
                                          NULL, &m_pEffect, NULL, NULL ) );
#elif 0
		hr = D3DX10CompileFromFile("./__tmp__.fx", NULL, NULL, "main", "fx_4_0",
			dwShaderFlags, 0, NULL, &pEffectBuffer, &pErrorBlob, NULL);
    if( !FAILED(hr) )
		D3D10CreateEffectFromMemory(pEffectBuffer->GetBufferPointer(),
			pEffectBuffer->GetBufferSize(),
			0,
			RendererHLSLD3D10::GetDevice(), 0,
			&m_pEffect);
#else
		hr = D3DCompileFromFile(L"./__tmp__.fx", NULL, NULL, "main", "fx_4_0", NULL, NULL, &pEffectBuffer, &pErrorBlob);
#endif
		    if( FAILED( hr ) )
    {
        LPCSTR szErrors = NULL;
        if(pErrors)
        {
            szErrors = (LPCSTR)pErrors->GetBufferPointer();
            LOG_PRINT("\n%s\n", szErrors);
            pErrors->Release();
        }
        return E_FAIL;
    }
		SAFE_RELEASE(pErrorBlob);
		SAFE_RELEASE(pEffectBuffer);}
    }
	virtual ~stx_EffectHLSLD3D10(){}
	stx_EffectHLSLD3D10(const stx_EffectHLSLD3D10& rhs)
	{
		m_sFileName=rhs.m_sFileName;
		m_sTechnique=rhs.m_sTechnique;
		m_iTechnique=rhs.m_iTechnique;
		m_vTechniques=rhs.m_vTechniques;
		m_vTextures=rhs.m_vTextures;
	}
        void SetMatrix( const char* aName, D3DXFROMWINEMATRIX* aValue )
        {
    		ID3D10EffectMatrixVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(aName)->AsMatrix();
	        if(pVariable) pVariable->SetMatrix((float*)aValue);
        }
        void SetVector( const char* aName, D3DXFROMWINEVECTOR4* aValue )
        {
    		ID3D10EffectVectorVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(aName)->AsVector();
	        if(pVariable) pVariable->SetFloatVector((float*)aValue);
        }
        void SetFloat( const char* aName, float* aValue )
        {
    		ID3D10EffectScalarVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(aName)->AsScalar();
	        if(pVariable) pVariable->SetFloat(*aValue);
        }
	void SetBool( const char* aName, bool aValue )
        {
    		ID3D10EffectScalarVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(aName)->AsScalar();
	        if(pVariable) pVariable->SetBool(aValue);
        }
	void SetTexture( const char* aName, TextureID aValue )
        {
		ID3D10EffectShaderResourceVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(aName)->AsShaderResource();
	        if(pVariable) pVariable->SetResource( RendererHLSLD3D10::GetRenderer()->textures[aValue].srv );
        }
        void SetValue( const char* aName, void* aPtr, unsigned int aSize )
        {
    		ID3D10EffectScalarVariable* pVariable = stx_EffectHLSLD3D10::GetNativeEffect()->GetVariableByName(aName)->AsScalar();
	        if(pVariable) pVariable->SetInt(*(int*)aPtr);
        }

	void SetTechnique( const char* aName );

		stx_VariableHLSLD3D10* GetVariableByName( const char* aName )
		{
			if(!aName)
				return 0;
			if(m_iTechnique<0)
				return 0;
			if(m_iTechnique>=m_vTechniques.size())
				return 0;
			int idx=-1;
			for(unsigned int i=0;i<m_vTechniques[m_iTechnique].m_vVariables.size();i++)
			{
				if(m_vTechniques[m_iTechnique].m_vVariables[i].m_sName==std::string(aName))
				{
					idx=i;
					break;
				}
			}
			if(idx>-1)
				return &m_vTechniques[m_iTechnique].m_vVariables[idx];
			return 0;
		} // ???

		stx_TechniqueHLSLD3D10* GetTechniqueByName( const char* aName )
		{
			if(!aName)
				return 0;
			int idx=-1;
			for(unsigned int i=0;i<m_vTechniques.size();i++)
			{
				if(m_vTechniques[i].m_sName==std::string(aName))
				{
					idx=i;
					break;
				}
			}
			if(idx>-1)
			{
				// ??? m_vTechniques[idx].m_pTechnique=stx_EffectHLSLD3D10::GetNativeEffect()->GetTechniqueByName( "aName" );
				return &m_vTechniques[idx];
			}
			return 0;
		} // ???
	void Begin( unsigned int *cPasses, unsigned int aValue )
	{
		D3D10_TECHNIQUE_DESC techDesc;
		m_vTechniques[m_iTechnique].m_pTechnique->GetDesc( &techDesc );
    		*cPasses=techDesc.Passes;
	}
	void BeginPass( unsigned int iPass )
	{
    		D3D10_PASS_DESC PassDesc;
			m_vTechniques[m_iTechnique].m_pTechnique->GetPassByIndex( iPass )->GetDesc( &PassDesc );
		if(!m_vTechniques[m_iTechnique].m_pInputLayout)
			RendererHLSLD3D10::GetDevice()->CreateInputLayout( m_vTechniques[m_iTechnique].m_vPasses[iPass].m_pDesc, elementsOf( m_vTechniques[m_iTechnique].m_vPasses[iPass].m_pDesc ),
								PassDesc.pIAInputSignature,
									          PassDesc.IAInputSignatureSize, &m_vTechniques[m_iTechnique].m_pInputLayout );
		RendererHLSLD3D10::GetDevice()->IASetInputLayout(m_vTechniques[m_iTechnique].m_pInputLayout );
		m_vTechniques[m_iTechnique].m_pTechnique->GetPassByIndex( iPass )->Apply( 0 );
	}
        void EndPass();
        void End();
};
ID3D10Effect* stx_EffectHLSLD3D10::m_pEffect = 0;
stx_Effect* RendererHLSLD3D10::addEffectFromFile(const std::string& aFileName, const char* aDefines)
{
    return new stx_EffectHLSLD3D10(aFileName, aDefines);
}
#else
stx_Effect* RendererHLSLD3D10::addEffectFromFile(const std::string& aFileName, const char* aDefines)
{
    return new stx_Effect(aFileName, aDefines);
}
#endif
#endif
/*
1>------ Build started: Project: Framework3, Configuration: Debug Win32 ------
1>cl : Command line warning D9035 : option 'Gm' has been deprecated and will be removed in a future release
1>RendererHLSLD3D10.cpp
1>Framework3.vcxproj -> C:\IndieLibX\DesktopProjects\projectsX\Framework3\..\..\..\lib\Framework3.lib
2>------ Build started: Project: SimpleTexture, Configuration: Debug Win32 ------
2>SimpleTexture.obj : warning LNK4075: ignoring '/EDITANDCONTINUE' due to '/SAFESEH' specification
2>   Creating library ..\..\..\bin\SimpleTexture.lib and object ..\..\..\bin\SimpleTexture.exp
2>Framework3.lib(RendererHLSLD3D10.obj) : error LNK2019: unresolved external symbol _D3D10CreateDeviceAndSwapChain@32 referenced in function "public: __thiscall RendererHLSLD3D10::RendererHLSLD3D10(bool,int,char * * const)" (??0RendererHLSLD3D10@@QAE@_NHQAPAD@Z)
2>Framework3.lib(RendererHLSLD3D10.obj) : error LNK2019: unresolved external symbol _D3D10CompileShader@40 referenced in function "public: virtual int __thiscall RendererHLSLD3D10::addHLSLShaderVrtl(char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,unsigned int)" (?addHLSLShaderVrtl@RendererHLSLD3D10@@UAEHPBD00000000000I@Z)
2>Framework3.lib(RendererHLSLD3D10.obj) : error LNK2019: unresolved external symbol _D3D10DisassembleShader@20 referenced in function "public: virtual int __thiscall RendererHLSLD3D10::addHLSLShaderVrtl(char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,unsigned int)" (?addHLSLShaderVrtl@RendererHLSLD3D10@@UAEHPBD00000000000I@Z)
2>Framework3.lib(RendererHLSLD3D10.obj) : error LNK2019: unresolved external symbol _D3D10ReflectShader@12 referenced in function "public: virtual int __thiscall RendererHLSLD3D10::addHLSLShaderVrtl(char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,unsigned int)" (?addHLSLShaderVrtl@RendererHLSLD3D10@@UAEHPBD00000000000I@Z)
2>Framework3.lib(RendererHLSLD3D10.obj) : error LNK2019: unresolved external symbol _D3D10GetInputSignatureBlob@12 referenced in function "public: virtual int __thiscall RendererHLSLD3D10::addHLSLShaderVrtl(char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,char const *,unsigned int)" (?addHLSLShaderVrtl@RendererHLSLD3D10@@UAEHPBD00000000000I@Z)
2>..\..\..\bin\SimpleTexture.exe : fatal error LNK1120: 5 unresolved externals
2>Done building project "SimpleTexture.vcxproj" -- FAILED.
========== Build: 1 succeeded, 1 failed, 0 up-to-date, 0 skipped ==========
*/
