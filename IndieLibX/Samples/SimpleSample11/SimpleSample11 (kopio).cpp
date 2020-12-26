/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#if 0
// Direct3D11 includes
#include <d3dcommon.h>
#include <dxgi.h>
#ifdef WindowsKits
#include <d3d11_2.h>
#else
#include <d3d11.h>
#endif

#include <D3DCompiler.h>

#ifndef WindowsKits
//include <d3dx11.h>
#endif
#include <d3d11shader.h>
#include <d3dx11effect.h>

//include <xnamath.h>
// XInput includes
#else
// Direct3D11 includes
#include <d3dcommon.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
//include <d3dx11.h>

// XInput includes
#include <xinput.h>

// HRESULT translation for Direct3D and other APIs
//include <dxerr.h>
#endif

#define V_RETURN

#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>
#include <Framework3/RendererHLSLD3D11.h>

RendererHLSLD3D11*		g_pRenderer11 = NULL;
ID3D11VertexShader*         g_pVertexShader11 = NULL;
ID3D11PixelShader*          g_pPixelShader11 = NULL;
ID3D11InputLayout*          g_pLayout11 = NULL;
ID3D11SamplerState*         g_pSamLinear = NULL;
ID3D11Texture2D*                    g_pTexture = NULL;
ID3D11ShaderResourceView*           g_pTextureRV = NULL;
ID3D11Buffer*                       g_pVertexBuffer = NULL;
ID3D11Buffer*                       g_pIndexBuffer = NULL;

int Compile(const char* vsFile=0, const char* vsMain=0, const char* fsFile=0, const char* fsMain=0)
{
	const char* fileName=0;
	unsigned int feature_level = D3D_FEATURE_LEVEL_11_0;
	std::string vsText;
	std::string fsText;
	{std::ifstream in(vsFile, std::ios::in | std::ios::binary);
  	if (in)
    		vsText=(std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));}
	{std::ifstream in(fsFile, std::ios::in | std::ios::binary);
  	if (in)
    		fsText=(std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));}
	ID3DBlob *shaderBuf = NULL;
	ID3DBlob *errorsBuf = NULL;
    #if 0
	UINT compileFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_ENABLE_STRICTNESS;// | D3D11_SHADER_DEBUG | D3D11_SHADER_SKIP_OPTIMIZATION;
		#else
		UINT compileFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY | D3DCOMPILE_SKIP_OPTIMIZATION;
		#if defined( DEBUG ) || defined( _DEBUG )
		compileFlags |= D3DCOMPILE_DEBUG;
		#endif
		#endif

	if (vsText.length())
	{
		const char *target = (feature_level == D3D_FEATURE_LEVEL_11_0)? "vs_5_0" : (feature_level == D3D_FEATURE_LEVEL_10_1)? "vs_4_1" : "vs_4_0";

		D3DCompile(vsText.c_str(), vsText.length(), fileName, NULL, NULL, vsMain, target, compileFlags, 0, &shaderBuf, &errorsBuf);
		{
			g_pRenderer11->GetDevice()->CreateVertexShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), NULL, &g_pVertexShader11);
			{
				//D3DGetInputSignatureBlob(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), &shader.inputSignature);
			}
		}
		const D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		V_RETURN(g_pRenderer11->GetDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), &g_pLayout11));
		SAFE_RELEASE(shaderBuf);
		SAFE_RELEASE(errorsBuf);

		if (g_pVertexShader11 == NULL)
			return -1;
	}

	if (fsText.length())
	{
		const char *target = (feature_level == D3D_FEATURE_LEVEL_11_0)? "ps_5_0" : (feature_level == D3D_FEATURE_LEVEL_10_1)? "ps_4_1" : "ps_4_0";

		D3DCompile(fsText.c_str(), fsText.length(), fileName, NULL, NULL, fsMain, target, compileFlags, 0, &shaderBuf, &errorsBuf);
		{
			g_pRenderer11->GetDevice()->CreatePixelShader(shaderBuf->GetBufferPointer(), shaderBuf->GetBufferSize(), NULL, &g_pPixelShader11);
			{
			}
		}
		SAFE_RELEASE(shaderBuf);
		SAFE_RELEASE(errorsBuf);

		if (g_pPixelShader11 == NULL)
			return -1;
	}
	return 0;
}

void MakeCheckerboard()
{
    D3D11_TEXTURE2D_DESC tdesc;
    D3D11_SUBRESOURCE_DATA tbsd;
    int w = 512;
    int h = 512;
    int bpp = 4;
    int *buf = new int[w*h];
    // filling the image
    for(int i=0;i<h;i++)
        for(int j=0;j<w;j++)
        {
            if((i&32)==(j&32))
                buf[i*w+j] = 0x00000000;
            else
                buf[i*w+j] = 0xffffffff;
        }
    // setting up D3D11_SUBRESOURCE_DATA 
    tbsd.pSysMem = (void *)buf;
    tbsd.SysMemPitch = w*bpp ;
    tbsd.SysMemSlicePitch = w*h*bpp ; // Not needed since this is a 2d texture

    // setting up D3D11_TEXTURE2D_DESC 
    tdesc.Width  = w;
    tdesc.Height = h;
    tdesc.MipLevels = 1;
    tdesc.ArraySize = 1;
    tdesc.SampleDesc.Count   = 1;
    tdesc.SampleDesc.Quality = 0;
    tdesc.Usage     = D3D11_USAGE_DEFAULT;
    tdesc.Format    = DXGI_FORMAT_R8G8B8A8_UNORM;
    tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    tdesc.CPUAccessFlags = 0;
    tdesc.MiscFlags      = 0;

    // checking inputs
    if ( g_pRenderer11->GetDevice()->CreateTexture2D(&tdesc, &tbsd, NULL) == S_FALSE )
        std::cout << "Inputs correct" << std::endl;
    else
        std::cout << "wrong inputs" << std::endl;

    // create the texture
    if(FAILED(g_pRenderer11->GetDevice()->CreateTexture2D(&tdesc,&tbsd,&tex)))
    {
        std::cout << "Failed" << std::endl;
        return(0);
    }
    else
        std::cout << "Success" << std::endl;

    delete[] buf;
}

int init(const char* aTitle)
{
#if 0
    // Compile the shaders
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    // You should use the lowest possible shader profile for your shader to enable various feature levels. These
    // shaders are simple enough to work well within the lowest possible profile, and will run on all feature levels
    ID3DBlob* pVertexShaderBuffer = NULL;
    V_RETURN( D3DX11CompileFromFile( L"../../IndieLib_resources/SimpleSample11/SimpleSample.hlsl", NULL, NULL, "RenderSceneVS", "vs_4_0_level_9_1", dwShaderFlags, 0, NULL,
                                     &pVertexShaderBuffer, NULL, NULL ) );

    ID3DBlob* pPixelShaderBuffer = NULL;
    V_RETURN( D3DX11CompileFromFile( L"../../IndieLib_resources/SimpleSample11/SimpleSample.hlsl", NULL, NULL, "RenderScenePS", "ps_4_0_level_9_1", dwShaderFlags, 0, NULL,
                                     &pPixelShaderBuffer, NULL, NULL ) );

    V_RETURN( g_pRenderer11->GetDevice()->CreateVertexShader( pVertexShaderBuffer->GetBufferPointer(),
                                              pVertexShaderBuffer->GetBufferSize(), NULL, &g_pVertexShader11 ) );

    V_RETURN( g_pRenderer11->GetDevice()->CreatePixelShader( pPixelShaderBuffer->GetBufferPointer(),
                                             pPixelShaderBuffer->GetBufferSize(), NULL, &g_pPixelShader11 ) );
#else
	Compile("../../IndieLib_resources/SimpleSample11/SimpleSample.hlsl", "RenderSceneVS", "../../IndieLib_resources/SimpleSample11/SimpleSample.hlsl", "RenderScenePS");
#endif

    D3D11_SAMPLER_DESC samDesc;
    ZeroMemory( &samDesc, sizeof(samDesc) );
    samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.MaxAnisotropy = 1;
    samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samDesc.MaxLOD = D3D11_FLOAT32_MAX;
    V_RETURN( g_pRenderer11->GetDevice()->CreateSamplerState( &samDesc, &g_pSamLinear ) );

#if 1
	MakeCheckerboard();
#elif 0
    V_RETURN( D3DX11CreateShaderResourceViewFromFile( g_pRenderer11->GetDevice(), L"../../IndieLib_resources/test.bmp", NULL, NULL, &g_pTextureRV, NULL ));
#elif 0
	Image3 tex;
	tex.loadImageLibImage("../../IndieLib_resources/test.bmp");
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = tex.getWidth();
	desc.Height = tex.getHeight();
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	std::vector<ubyte> pixels(tex.getWidth()*tex.getHeight()*4);
	D3D11_SUBRESOURCE_DATA texData;
	texData.pSysMem = tex.getPixels();
	texData.pSysMem = &pixels[0];
	texData.SysMemPitch = 0;
	texData.SysMemSlicePitch = 0;
	g_pRenderer11->GetDevice()->CreateTexture2D( &desc, &texData, &g_pTexture );
#elif 0
	Image3 tex;
	tex.loadImageLibImage("../../IndieLib_resources/test.bmp");
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = 256;
	desc.Height = 256;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	ID3D11Texture2D* pTexture = NULL;
	g_pRenderer11->GetDevice()->CreateTexture2D(&desc, NULL, &pTexture);
#endif
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_RESOURCE_DIMENSION type;
        pTexture->GetDesc( &desc );
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = desc.MipLevels -1;
	g_pRenderer11->GetDevice()->CreateShaderResourceView( g_pTexture, &srvDesc, &g_pTextureRV );

	float vertices[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };

    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(float) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
    V_RETURN( g_pRenderer11->GetDevice()->CreateBuffer( &bd, &InitData, &g_pVertexBuffer ));

    UINT stride = sizeof(float)*4;
    UINT offset = 0;
    g_pRenderer11->GetDeviceContext()->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

    WORD indices[] =
    {
        0,1,2,3
    };

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( WORD ) * 4;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    V_RETURN( g_pRenderer11->GetDevice()->CreateBuffer( &bd, &InitData, &g_pIndexBuffer ));
    g_pRenderer11->GetDeviceContext()->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
    g_pRenderer11->GetDeviceContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
    g_pRenderer11->GetDeviceContext()->IASetInputLayout( g_pLayout11 );
    g_pRenderer11->GetDeviceContext()->VSSetShader( g_pVertexShader11, NULL, 0 );
    g_pRenderer11->GetDeviceContext()->PSSetShader( g_pPixelShader11, NULL, 0 );
    g_pRenderer11->GetDeviceContext()->PSSetShaderResources( 0, 1, &g_pTextureRV );
    g_pRenderer11->GetDeviceContext()->PSSetSamplers( 0, 1, &g_pSamLinear );
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
    	g_pRenderer11->GetDeviceContext()->DrawIndexed( 4, 0, 0 );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("SimpleTexture11");
	g_pRenderer11 = (RendererHLSLD3D11*)r;
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

