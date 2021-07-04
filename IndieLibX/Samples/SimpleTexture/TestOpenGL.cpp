/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>
#if defined(GLSL1_1)
#ifndef _MSC_VER
#include <Framework3/RendererGLSLGL_1_1.h>

RendererGLSLGL_1_1* g_pRendererGL=0;
GLuint g_textureID = 0;
float g_fSpinX           = 0.0f;
float g_fSpinY           = 0.0f;
#define BUFFER_OFFSET(i) ((char *)NULL + (i)) // taken from the opengl wiki
GLuint vbo = -1;

struct Vertex___
{
    float tu, tv;
    float x, y, z;
};

Vertex___ g_quadVertices[] =
{
    { 0.0f,0.0f, -1.0f,-1.0f, 0.0f },
    { 1.0f,0.0f,  1.0f,-1.0f, 0.0f },
    { 1.0f,1.0f,  1.0f, 1.0f, 0.0f },
    { 0.0f,1.0f, -1.0f, 1.0f, 0.0f }
};

struct BMPImage
{
    int   width;
    int   height;
    char *data;
};

void getBitmapImageData( char *pFileName, BMPImage *pImage )
{
    FILE *pFile = NULL;
    unsigned short nNumPlanes;
    unsigned short nNumBPP;
    int i;

    if( (pFile = fopen(pFileName, "rb") ) == NULL )
        LOG_PRINT("ERROR: getBitmapImageData - %s not found\n",pFileName);

    fseek( pFile, 18, SEEK_CUR );

    if( (i = fread(&pImage->width, 4, 1, pFile) ) != 1 )
      LOG_PRINT("ERROR: getBitmapImageData - Couldn't read width from %s.\n", pFileName);

    if( (i = fread(&pImage->height, 4, 1, pFile) ) != 1 )
        LOG_PRINT("ERROR: getBitmapImageData - Couldn't read height from %s.\n", pFileName);

    if( (fread(&nNumPlanes, 2, 1, pFile) ) != 1 )
       LOG_PRINT("ERROR: getBitmapImageData - Couldn't read plane count from %s.\n", pFileName);
 
    if( nNumPlanes != 1 )
     LOG_PRINT( "ERROR: getBitmapImageData - Plane count from %s is not 1: %u\n", pFileName, nNumPlanes );

    if( (i = fread(&nNumBPP, 2, 1, pFile)) != 1 )
      LOG_PRINT( "ERROR: getBitmapImageData - Couldn't read BPP from %s.\n", pFileName );
   
    if( nNumBPP != 24 )
       LOG_PRINT( "ERROR: getBitmapImageData - BPP from %s is not 24: %u\n", pFileName, nNumBPP );

    fseek( pFile, 24, SEEK_CUR );

    int nTotalImagesize = (pImage->width * pImage->height) * 3;

    pImage->data = (char*) malloc( nTotalImagesize );
    
    if( (i = fread(pImage->data, nTotalImagesize, 1, pFile) ) != 1 )
      LOG_PRINT("ERROR: getBitmapImageData - Couldn't read image data from %s.\n", pFileName);
 
    char charTemp;
    for( i = 0; i < nTotalImagesize; i += 3 )
    { 
        charTemp = pImage->data[i];
        pImage->data[i] = pImage->data[i+2];
        pImage->data[i+2] = charTemp;
    }
}

void loadTexture( void )    
{
    BMPImage textureImage;

	std::string fn=stx_convertpath("/test.bmp");
 
    getBitmapImageData( fn.c_str(), &textureImage );

    glGenTextures( 1, &g_textureID );
    glBindTexture( GL_TEXTURE_2D, g_textureID );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, 3, textureImage.width, textureImage.height, 
                  0, GL_RGB, GL_UNSIGNED_BYTE, textureImage.data );
}

int init(const char* aTitle)
{
glGenBuffers( 1, &vbo );

glBindBuffer( GL_ARRAY_BUFFER, vbo );
glBufferData( GL_ARRAY_BUFFER, sizeof( Vertex___ ) * 4, g_quadVertices, GL_STATIC_DRAW );

glEnable( GL_TEXTURE_2D );

	return 0;
}

void render()
{
	g_pRendererGL->BeginScene();
 
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glClearColor(0.0,1.0,0.0,0.0); 
  
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0.0f, 0.0f, -5.0f );
    glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );
    
    glBindTexture( GL_TEXTURE_2D, g_textureID );

glEnableClientState( GL_VERTEX_ARRAY );
glEnableClientState( GL_TEXTURE_COORD_ARRAY );

glBindBuffer( GL_ARRAY_BUFFER, vbo );
glVertexPointer( 3, GL_FLOAT, sizeof( Vertex___ ), 0 );
glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex___ ), BUFFER_OFFSET( 12 ) );

glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

glDisableClientState( GL_VERTEX_ARRAY );
glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	g_pRendererGL->EndScene();
	g_pRendererGL->Present();
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("TestOpenGL");
	g_pRendererGL = (RendererGLSLGL_1_1*)r;

	LOG_PRINT("GL_VERSION=%s\n", (const char *) glGetString(GL_VERSION));
	LOG_PRINT("GL_VENDOR=%s\n", (const char *) glGetString(GL_VENDOR));
	LOG_PRINT("GL_RENDERER=%s\n", (const char *) glGetString(GL_RENDERER));
	LOG_PRINT("GL_SHADING_LANGUAGE_VERSION=%s\n", (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION));

	#if 1
    std::ofstream out("./OpenGL.log");
	out << "GL_VERSION:\n"; out << (const char *) glGetString(GL_VERSION);out << "\n";
	out << "GL_VENDOR:\n"; out << (const char *) glGetString(GL_VENDOR);out << "\n";
	out << "GL_RENDERER:\n"; out << (const char *) glGetString(GL_RENDERER);out << "\n";
	out << "GL_SHADING_LANGUAGE_VERSION:\n"; out << (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);out << "\n";
    out.close();
	#endif
	init("");
	while (!STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_ESCAPE) && !STX_Service::GetInputInstance()->Quit())
	{
		STX_Service::GetInputInstance()->Update();
		render();
	}
	return 0;
}
#else
#include <d3dcommon.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <xinput.h>

#define V_RETURN

#include <Framework3/RendererHLSLD3D11.h>

RendererHLSLD3D11*		g_pRenderer11 = NULL;
ID3D11VertexShader*         g_pVertexShader11 = NULL;
ID3D11PixelShader*          g_pPixelShader11 = NULL;
ID3D11InputLayout*          g_pLayout11 = NULL;
ID3D11SamplerState*         g_pSamLinear = NULL;
ID3D11DepthStencilState * 	g_pDSState = NULL;
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
		UINT compileFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR | D3DCOMPILE_SKIP_OPTIMIZATION;
		//compileFlags|=D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;
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

ID3D11ShaderResourceView *createSRV(ID3D11Resource *resource)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ID3D11ShaderResourceView *srv;
	D3D11_TEXTURE2D_DESC desc2d;
	((ID3D11Texture2D *) resource)->GetDesc(&desc2d);
	srvDesc.Format = desc2d.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = desc2d.MipLevels;
	g_pRenderer11->GetDevice()->CreateShaderResourceView(resource, &srvDesc, &srv);
	return srv;
}

void addTexture(Image3 &img)
{
	unsigned int nMipMaps = img.getMipMapCount();
	unsigned int nSlices = 1;
	unsigned int arraySize = 1;
	static D3D11_SUBRESOURCE_DATA texData[1124];
	D3D11_SUBRESOURCE_DATA *dest = texData;
		for (unsigned int k = 0; k < nSlices; k++){
			for (unsigned int i = 0; i < nMipMaps; i++){
				unsigned int pitch, slicePitch;
					pitch = img.getWidth(i) * 4;
					slicePitch = pitch * img.getHeight(i);
				dest->pSysMem = img.getPixels(i/*, n*/) + k * slicePitch;
				dest->SysMemPitch = pitch;
				dest->SysMemSlicePitch = slicePitch;
				dest++;
			}
		}
	HRESULT hr;
		D3D11_TEXTURE2D_DESC desc;
		desc.Width  = img.getWidth();
		desc.Height = img.getHeight();
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.MipLevels = nMipMaps;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;//desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
			desc.ArraySize = 1;
			desc.MiscFlags = 0;
		hr = g_pRenderer11->GetDevice()->CreateTexture2D(&desc, texData, (ID3D11Texture2D **) &g_pTexture);
	g_pTextureRV = createSRV(g_pTexture);
}

void CreateDepthStencilState()
{
	const bool depthTest=false; 
	const bool depthWrite=false;
	const int depthFunc = D3D11_COMPARISON_LESS_EQUAL;
	const bool stencilTest = false;
	const uint8 stencilMask = 0xFF;
	const int stencilFunc =  D3D11_COMPARISON_ALWAYS;
	const int stencilFail = D3D11_STENCIL_OP_KEEP;
	const int depthFail = D3D11_STENCIL_OP_KEEP;
	const int stencilPass = D3D11_STENCIL_OP_KEEP;
	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = (BOOL) depthTest;
	desc.DepthWriteMask = depthWrite? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = (D3D11_COMPARISON_FUNC) depthFunc;
	desc.StencilEnable = (BOOL) stencilTest;
	desc.StencilReadMask  = stencilMask;
	desc.StencilWriteMask = stencilMask;
	desc.BackFace. StencilFunc = (D3D11_COMPARISON_FUNC) stencilFunc;
	desc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC) stencilFunc;
	desc.BackFace. StencilDepthFailOp = (D3D11_STENCIL_OP) depthFail;
	desc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP) depthFail;
	desc.BackFace. StencilFailOp = (D3D11_STENCIL_OP) stencilFail;
	desc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP) stencilFail;
	desc.BackFace. StencilPassOp = (D3D11_STENCIL_OP) stencilPass;
	desc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP) stencilPass;
	g_pRenderer11->GetDevice()->CreateDepthStencilState(&desc, &g_pDSState);
}

int init(const char* aTitle)
{
	Compile("../../IndieLib_resources/SimpleSample11/SimpleSample.hlsl", "RenderSceneVS", "../../IndieLib_resources/SimpleSample11/SimpleSample.hlsl", "RenderScenePS");

    D3D11_SAMPLER_DESC samDesc;
    ZeroMemory( &samDesc, sizeof(samDesc) );
    samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.MaxAnisotropy = 1;
    samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samDesc.MaxLOD = D3D11_FLOAT32_MAX;
    V_RETURN( g_pRenderer11->GetDevice()->CreateSamplerState( &samDesc, &g_pSamLinear ) );
	CreateDepthStencilState();

	Image3 tex;
	tex.loadImageLibImage("../../IndieLib_resources/test.bmp");
	addTexture(tex);

	float vertices[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };

    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(float) * elementsOf(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
    V_RETURN( g_pRenderer11->GetDevice()->CreateBuffer( &bd, &InitData, &g_pVertexBuffer ));

    UINT stride = sizeof(float)*elementsOf(vertices);
    UINT offset = 0;
    g_pRenderer11->GetDeviceContext()->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

    WORD indices[] =
    {
        //0,1,2,3
	0,1,2,2,1,3
    };

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( WORD ) * elementsOf(indices);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    V_RETURN( g_pRenderer11->GetDevice()->CreateBuffer( &bd, &InitData, &g_pIndexBuffer ));
    g_pRenderer11->GetDeviceContext()->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
    //g_pRenderer11->GetDeviceContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
    g_pRenderer11->GetDeviceContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	return 0;
}

void render()
{
	g_pRenderer11->BeginScene();
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
    g_pRenderer11->GetDeviceContext()->IASetInputLayout( g_pLayout11 );
    g_pRenderer11->GetDeviceContext()->VSSetShader( g_pVertexShader11, NULL, 0 );
    g_pRenderer11->GetDeviceContext()->PSSetShader( g_pPixelShader11, NULL, 0 );
    g_pRenderer11->GetDeviceContext()->PSSetShaderResources( 0, 1, &g_pTextureRV );
    g_pRenderer11->GetDeviceContext()->PSSetSamplers( 0, 1, &g_pSamLinear );
	g_pRenderer11->GetDeviceContext()->OMSetDepthStencilState(g_pDSState, 0);
    	//g_pRenderer11->GetDeviceContext()->DrawIndexed( 4, 0, 0 );
    	g_pRenderer11->GetDeviceContext()->DrawIndexed( 6, 0, 0 );
	g_pRenderer11->EndScene();
	g_pRenderer11->Present();
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
#endif
#endif

