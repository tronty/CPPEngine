#if defined(D3D9)
#ifndef __Framework3D3D9_H__
#define __Framework3D3D9_H__ 1
#include <STX/STX.h>
#include <Framework3/IRenderer.h>

/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*
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

//include <Mmsystem.h>
#define D3D_DEBUG_INFO
#if 0
// Direct3D11 includes
#include <d3dcommon.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
//include <xnamath.h>
// XInput includes
#include <xinput.h>

#ifdef DX10//???
#include <d3d10_1.h>
#else
#include <d3d10.h>
#endif
#endif
#include <d3d9.h>
#if _MSC_VER <= 1600
#include <d3dx9.h>
#else
typedef const char* D3DXHANDLE;
#endif

//if(DIRECT3D_VERSION > 0x0900)
   #if (D3DX_SDK_VERSION >= 42) //aug 2009 sdk and up there is no dxerr9 anymore
      #include <Dxerr.h>
      //pragma comment (lib,"DxErr.lib")
   #else
      //include <dxerr9.h>
      #define DXGetErrorString(hr)
      //DXGetErrorString9(hr)
      #define DXGetErrorDescription(hr)
      //DXGetErrorDescription9(hr)
      //pragma comment (lib,"Dxerr9.lib")
   #endif
//endif

#include <Framework3/RendererHLSLCg.h>
//include <Cg/Cg.h>
//include <Cg/CgD3D9.h>

#if _DEBUG
#define V_RETURN(arg) {HRESULT hr7=arg;if(hr7!=D3D_OK){LOG_PRINT("%s:%d:hr=%d\n",__FUNCTION__,__LINE__,hr7);DBG_HALT;}}
#define V(arg) {HRESULT hr7=arg;if(hr7!=D3D_OK){LOG_PRINT("%s:%d:hr=%d\n",__FUNCTION__,__LINE__,hr7);DBG_HALT;}}
#define SOK(arg) {HRESULT hr7=arg;if(hr7!=S_OK){LOG_PRINT("%s:%d:hr=%d\n",__FUNCTION__,__LINE__,hr7);DBG_HALT;}}
#else
#define V_RETURN(arg) arg;
#define V(arg) arg;
#endif
struct ParameterHLSL9 : public ShaderParameter
{
	D3DXHANDLE param;
	ParameterHLSL9():ShaderParameter(){}
	ParameterHLSL9(D3DXHANDLE aparam){param=aparam;}
	virtual ~ParameterHLSL9(){}
	//virtual bool cgIsParameterReferenced(CGparameter& parameter);
};
struct ShaderFXD3D9 : public Shader
{
	bool m_InPass;
	bool GetInPass(){return m_InPass;}
	void SetInPass(bool aInPass){m_InPass=aInPass;}

	LPD3DXEFFECTCOMPILER  m_pCompiler;
	ID3DXEffect*	m_pEffect;
	D3DXHANDLE		m_pCurrentTechnique;
	ShaderFXD3D9():Shader(){}
	virtual ~ShaderFXD3D9(){}
	ShaderFXD3D9(const char* pProgramFileNameV,const char* pProgramFileNameF,const char* mainV,const char* mainF, bool txt=false
		, const char* TechniqueName=0);

	virtual void GetParams(const char* name,ShaderParameter* paramV,ShaderParameter* paramF,bool& V, bool& F);
	virtual void GetParamsI(const int idx,ShaderParameter* paramV,ShaderParameter* paramF,bool& V, bool& F);

};
struct ConstantD3D9 : public Constant {
	ConstantD3D9() : Constant(){name="";vsReg=0;psReg=0;}
	virtual ~ConstantD3D9(){}
        std::string name;
        int vsReg;
        int psReg;
};

inline int constantCompD3D9(const void *s0, const void *s1){
        return strcmp(((ConstantD3D9 *) s0)->name.c_str(), ((ConstantD3D9 *) s1)->name.c_str());
}

struct SamplerD3D9 : public Sampler {
	SamplerD3D9() : Sampler(),vtexture(false){}
	virtual ~SamplerD3D9(){}

        std::string name;
        unsigned int index;
		bool vtexture;
};

inline int samplerCompD3D9(const void *s0, const void *s1){
        return strcmp(((SamplerD3D9 *) s0)->name.c_str(), ((SamplerD3D9 *) s1)->name.c_str());
}
struct ShaderHLSLNOFX3D3D9 : public Shader
{
	ShaderHLSLNOFX3D3D9() : Shader(){}
	virtual ~ShaderHLSLNOFX3D3D9(){}

void LoadTexture(const Image3* img, LPDIRECT3DBASETEXTURE9* ret, bool atlas=false, FORMAT f=FORMAT_RGB8, unsigned int w=0, unsigned int h=0, __DWORD__ u=0);

        LPDIRECT3DVERTEXSHADER9 vertexShader;
        LPDIRECT3DPIXELSHADER9 pixelShader;
        ID3DXConstantTable *vsConstants;
        ID3DXConstantTable *psConstants;

        std::vector<ConstantD3D9 > constants;
		std::vector<SamplerD3D9 > samplers;

        unsigned int nConstants;
        unsigned int nSamplers;

};
struct ShaderHLSLNOFXD3D9 : public Shader
{
        LPDIRECT3DVERTEXSHADER9 vertexShader;
        LPDIRECT3DPIXELSHADER9 pixelShader;
        ID3DXConstantTable *vsConstants;
        ID3DXConstantTable *psConstants;

        std::vector<Constant *> constants;
        std::vector<Sampler*> samplers;

        unsigned int nConstants;
        unsigned int nSamplers;





		LPDIRECT3DVERTEXSHADER9      m_pVertexShader;
		LPD3DXCONSTANTTABLE	     m_pConstantTableVS;
		LPDIRECT3DPIXELSHADER9 m_pPixelShader;
		LPD3DXCONSTANTTABLE    m_pConstantTablePS;

	ShaderHLSLNOFXD3D9():Shader(){}
	virtual ~ShaderHLSLNOFXD3D9()
	{
    if( m_pVertexShader != 0 )
	{
		m_pVertexShader->Release();
	m_pVertexShader = 0;
	}

    if( m_pConstantTableVS != 0 )
	{
		m_pConstantTableVS->Release();
	m_pConstantTableVS = 0;
	}

    if( m_pPixelShader != 0 )
	{
		m_pPixelShader->Release();
	m_pPixelShader = 0;
	}

    if( m_pConstantTablePS != 0 )
	{
		m_pConstantTablePS->Release();
	m_pConstantTablePS = 0;
	}
		}
	ShaderHLSLNOFXD3D9(const char* pProgramFileNameV,const char* pProgramFileNameF,const char* mainV,const char* mainF, bool txt=false
		, const char* TechniqueName=0);

	virtual void GetParams(const char* name,ShaderParameter* paramV,ShaderParameter* paramF,bool& V, bool& F);
	virtual void GetParamsI(const int idx,ShaderParameter* paramV,ShaderParameter* paramF,bool& V, bool& F);

};

struct TextureD3D9 : public Texture
{
	TextureD3D9():Texture(),vtexture(false),useR2VB(false){}
	virtual ~TextureD3D9(){}
		LPDIRECT3DBASETEXTURE9 texture;
	LPDIRECT3DSURFACE9 *surfaces;
	unsigned int flags;
	__DWORD__ usage;
	bool mipMapped;
	bool vtexture;
	bool useR2VB;
	//	TextureFilter filter;
//	float lod;
	BYTE* Lock(int m, int& pitch)
	{
		D3DLOCKED_RECT m_pLockedRect;
		D3DLOCKED_BOX m_pLockedBox;
        BYTE* m_pLockedData=0;
		switch(dim)
		{
		case 2:
		((IDirect3DTexture9*)texture)->LockRect( m, &m_pLockedRect, 0, 0 );
		pitch=m_pLockedRect.Pitch;//???
        m_pLockedData=(BYTE *)m_pLockedRect.pBits;
		break;
		case 3:
		((IDirect3DVolumeTexture9*)texture)->LockBox( m, &m_pLockedBox, 0, 0 );
		pitch=m_pLockedBox.RowPitch;//???
		m_pLockedData=(BYTE *)m_pLockedBox.pBits;
		break;
		};
		return m_pLockedData;
	}
	void Unlock(int m)
	{
		switch(dim)
		{
		case 2:
		((IDirect3DTexture9*)texture)->UnlockRect( m);
		break;
		case 3:
		((IDirect3DVolumeTexture9*)texture)->UnlockBox( m);
		break;
		};
	}
};
struct VertexFormatD3D9 : public VertexFormat
{
	VertexFormatD3D9():VertexFormat(){}
	virtual ~VertexFormatD3D9(){}
	LPDIRECT3DVERTEXDECLARATION9 vertexDecl;
	unsigned int vertexSize[MAX_VERTEXSTREAM];
};
struct VertexBufferD3D9 : public VertexBuffer
{
	VertexBufferD3D9():VertexBuffer(){}
	virtual ~VertexBufferD3D9(){}
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
	int size;
	__DWORD__ usage;
	BYTE* Lock()
	{
		BYTE* m_pLockedData=0;
		vertexBuffer->Lock( 0, 0, ( void** )&m_pLockedData, 0 );
		return m_pLockedData;
	}
	void Unlock()
	{
		vertexBuffer->Unlock();
	}
};
struct IndexBufferD3D9 : public IndexBuffer
{
	IndexBufferD3D9():IndexBuffer(){}
	virtual ~IndexBufferD3D9(){}
	LPDIRECT3DINDEXBUFFER9 indexBuffer;
	unsigned int nIndices;
	unsigned int indexSize;
	__DWORD__ usage;
	BYTE* Lock()
	{
		BYTE* m_pLockedData=0;
		indexBuffer->Lock( 0, 0, ( void** )&m_pLockedData, 0 );
		return m_pLockedData;
	}
	void Unlock()
	{
		indexBuffer->Unlock();
	}
};
struct AlphaStateD3D9 : public AlphaState
{
	AlphaStateD3D9():AlphaState()
	{
		blendEnable=false;
	}
	virtual ~AlphaStateD3D9(){}
	bool blendEnable;
	int alphaFunc;
};
struct BlendStateD3D9 : public BlendState
{
	BlendStateD3D9():BlendState(){}
	virtual ~BlendStateD3D9(){}
        int srcFactorRGB;
        int dstFactorRGB;
        int blendModeRGB;
        int srcFactorAlpha;
        int dstFactorAlpha;
        int blendModeAlpha;
        int mask;
        bool blendEnable;
        bool alphaEnable;
};
struct DepthStateD3D9 : public DepthState
{
	DepthStateD3D9():DepthState(){}
	virtual ~DepthStateD3D9(){}
	int depthFunc;
	bool depthTest;
	bool depthWrite;
};
struct RasterizerStateD3D9 : public RasterizerState
{
	RasterizerStateD3D9():RasterizerState(){}
	virtual ~RasterizerStateD3D9(){}
        int cullMode;
        int fillMode;
        bool multiSample;
        bool scissor;
};
struct SamplerStateD3D9 : public SamplerState
{
	SamplerStateD3D9():SamplerState(){}
	virtual ~SamplerStateD3D9(){}
        D3DTEXTUREFILTERTYPE minFilter;
        D3DTEXTUREFILTERTYPE magFilter;
        D3DTEXTUREFILTERTYPE mipFilter;
        D3DTEXTUREADDRESS wrapS;
        D3DTEXTUREADDRESS wrapT;
        D3DTEXTUREADDRESS wrapR;
        __DWORD__ maxAniso;
        float lod;
        __D3DCOLOR__ borderColor;
};

//include "ContentLoadersD3D9.h"

struct RendererHLSLD3D9 : public RendererHLSLCg
{

	protected:
		bool mOk;
	public:
		virtual const char* getString(){return "D3D9";}
		virtual bool IsOK(){return mOk;}
		
	virtual Texture* GetTexture(TextureID id)
	{
		if(id==-1)
			return 0;
		if(id>=textures.size())
			return 0;
		return &textures[id];
	}
	void GetInfo();
	virtual BlendStateID getBlendStateVrtl(const TextureID texture)
	{
		if(texture>=textures.size())
			return -1;
	if(textures[texture].format==FORMAT_RGB8)
		return IRenderer::GetRendererInstance()->GetblendOneOne();
	else
		return IRenderer::GetRendererInstance()->GetblendSrcAlpha();
	}
	virtual unsigned int drawArrays(const Primitives primitives, const int firstVertex, const int nVertices);
	virtual unsigned int drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices);


	virtual unsigned int drawPrimitivesVrtl(	const Primitives primitives,
						const unsigned int firstv, const unsigned int countv,
						const unsigned int firsti, const unsigned int counti,
				const ubyte* vbdata, const unsigned int vbsize,
				const ubyte* ibdata, const unsigned int ibsize);
	virtual unsigned int GetLevelCount(TextureID id);
	virtual void GetLevelDesc(TextureID id, unsigned int level,SURFACE_DESC* sDesc);
	virtual void GetDesc(VertexBufferID id,VERTEXBUFFER_DESC* sDesc);
	virtual void GetDesc(IndexBufferID id, INDEXBUFFER_DESC* sDesc);

float	GetMaxPointSpriteSize();
void	BeginPointSpriteRendering();
void	EndPointSpriteRendering();

	virtual BYTE *fillVertexBufferVrtl(const VertexBufferID id, BYTE*  m_pData, int l);
	virtual BYTE *fillIndexBufferVrtl(const IndexBufferID id, BYTE*  m_pData, int l);

	////FontID defaultFont;
	////FontID GetDefaultFont(){return defaultFont;}

	////virtual VArray <TexFont*>& GetFonts(){return fonts;}
	////virtual TexFont* GetFont(unsigned int i){return fonts.GetAt(i);}

	D3DXFROMWINEVECTOR4 scaleBias2D;
	D3DXFROMWINEVECTOR4& GetscaleBias2D(){return scaleBias2D;}

	virtual int GetTextureWidth(TextureID id);
	virtual int GetTextureHeight(TextureID id);

	virtual void viewport(int x, int y, int viewportWidth, int viewportHeight);

virtual int texturePitchVrtl(const TextureID id,int level);
BYTE * lockTexture(const TextureID id,int level, int& pitch);
BYTE * lockVertexBuffer(const VertexBufferID id);
BYTE * lockIndexBuffer(const IndexBufferID id);
virtual BYTE * unlockTextureVrtl(const TextureID id,int level);
BYTE * unlockVertexBuffer(const VertexBufferID id);
BYTE * unlockIndexBuffer(const IndexBufferID id);

    ProcessorID addTextureProcessor( TextureID& ppTexture, int g_SkipMips );
    HRESULT ProcessTexture( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes );
    HRESULT LockTextureDeviceObject(ProcessorID pProcessor);
    HRESULT CopyToResourceTexture(ProcessorID pProcessor);
    HRESULT UnLockTextureDeviceObject(ProcessorID pProcessor);

    ProcessorID addVertexBufferProcessor( VertexBufferID& ppBuffer, int iSizeBytes,   void* pData);
    HRESULT ProcessVertexBuffer( ProcessorID pProcessor,void*pLocalData, SIZE_T Bytes );
    HRESULT LockVertexBufferDeviceObject(ProcessorID pProcessor);
    HRESULT CopyToResourceVertexBuffer(ProcessorID pProcessor);
    HRESULT UnLockVertexBufferDeviceObject(ProcessorID pProcessor);

    ProcessorID addIndexBufferProcessor( IndexBufferID& ppBuffer, int iSizeBytes, void* pData);
    HRESULT ProcessIndexBuffer(ProcessorID pProcessor, void*pLocalData, SIZE_T Bytes );
    HRESULT LockIndexBufferDeviceObject(ProcessorID pProcessor);
    HRESULT CopyToResourceIndexBuffer(ProcessorID pProcessor);
    HRESULT UnLockIndexBufferDeviceObject(ProcessorID pProcessor);

    BYTE* LockTexture(TextureID id, int m, int& pitch);
    void UnlockTexture(TextureID id, int m);
    BYTE* LockVertexBuffer(VertexBufferID id);
    void UnLockVertexBuffer(VertexBufferID id);
    BYTE* LockIndexBuffer(IndexBufferID id);
    void UnLockIndexBuffer(IndexBufferID id);

	void	CreateTextureFromFile_Serial(  CHAR* szFileName,
						TextureID* ppTexture,
						//, void* pContext
						CPackedFile*		    g_PackFile,
						UINT g_SkipMips
						);
void	CreateVertexBuffer_Serial(  VertexBufferID& ppBuffer, UINT iSizeBytes,
//					    __DWORD__ Usage, D3DFORMAT ibFormat, D3DPOOL Pool,
						void* pData
//					    void* pContext
						);
void	CreateIndexBuffer_Serial(  IndexBufferID& ppBuffer, UINT iSizeBytes,
//					    __DWORD__ Usage, D3DFORMAT ibFormat, D3DPOOL Pool,
						void* pData
//					    void* pContext
						);

protected:
#if 0
	VArray <TextureD3D9*> textures;
	VArray <ShaderHLSLNOFX3D3D9*> shaders;
	VArray <VertexBufferD3D9*> vertexBuffers;
	VArray <IndexBufferD3D9*> indexBuffers;
	VArray <VertexFormatD3D9*> vertexFormats;
	VArray <SamplerStateD3D9*> samplerStates;
	VArray <BlendStateD3D9*> blendStates;
	VArray <DepthStateD3D9*> depthStates;
	VArray <RasterizerStateD3D9*> rasterizerStates;
#else
	////VArray <TexFont*> fonts;
	public:
	VArray <TextureD3D9> textures;
	VArray <ShaderHLSLNOFX3D3D9> shaders;
	VArray <VertexBufferD3D9> vertexBuffers;
	VArray <IndexBufferD3D9> indexBuffers;
	VArray <VertexFormatD3D9> vertexFormats;
	VArray <SamplerStateD3D9> samplerStates;
	VArray <BlendStateD3D9> blendStates;
	VArray <AlphaStateD3D9> alphaStates;
	VArray <DepthStateD3D9> depthStates;
	VArray <RasterizerStateD3D9> rasterizerStates;

#if 0
	VArray <CTextureProcessorD3D9> textureProcessors;
	VArray <CVertexBufferProcessorD3D9> vertexBufferProcessors;
	VArray <CIndexBufferProcessorD3D9> indexBufferProcessors;
#endif

#endif

virtual void buildPerspectiveMatrix(float fieldOfView,
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
	//p=m;

}
        virtual void buildLookAtMatrix(float eyex, float eyey, float eyez,
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
	float f=c1.x*c1.x+c1.y*c1.y+c1.z*c1.z;
	if(f<=0.0000001f) return;
	float radius=0.0f; float phi=0.0f; float theta=0.0f;
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
			virtual void setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState=-1
		, const int vtstream=-1
		, const VertexBufferID vtbuffer=-1
		);
        void setTextureSlice(const char *textureName, const TextureID texture, const int slice);

        LPDIRECT3DBASETEXTURE9 getD3DTexture(const TextureID texture) const;

		virtual void applyConstants();

        virtual void changeRenderTargetsVrtl(const TextureID *colorRTs, const unsigned int nRenderTargets, const TextureID depthRT, const int depthSlice, const int *slices);
        virtual void changeToMainFramebuffer();

        TextureID addTexture(Image3* img, const bool useMips, const SamplerStateID samplerState, unsigned int flags);

        TextureID addRenderTargetVrtl(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags);
        TextureID addRenderDepthVrtl(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags);

        bool resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize);
        bool generateMipMaps(const TextureID renderTarget);

        void LoadTexture(const Image3* img, LPDIRECT3DBASETEXTURE9* ret, bool atlas=false, FORMAT f=FORMAT_RGB8, unsigned int w=0, unsigned int h=0, __DWORD__ u=0);

	int removeTexture(const TextureID texture);
	UINT32 GetAvailableTextureMem();

D3DFORMAT m_fmt;
D3DPRESENT_PARAMETERS m_d3dpp;

union
        {
                LPDIRECT3DSURFACE9 m_fbColor;
                LPDIRECT3DSURFACE9 fbColor;
        };
        union
        {
        LPDIRECT3DSURFACE9 fbDepth;
                LPDIRECT3DSURFACE9 m_fbDepth;
        };

	RendererHLSLD3D9(bool fullScreen=false,
		int argc=0, char *argv[]=0);

	virtual ~RendererHLSLD3D9();
#if 0
	BYTE* GetVertexBufferProcessorData(ProcessorID ID)
	{return (BYTE *)vertexBufferProcessors[ID].m_pLockedData;}
	BYTE* GetIndexBufferProcessorData(ProcessorID ID)
	{return (BYTE *)indexBufferProcessors[ID].m_pLockedData;}
	BYTE* GetTextureProcessorData(ProcessorID ID, UINT i)
	{return (BYTE *)textureProcessors[ID].m_pLockedRects[i].pBits;}
	UINT GetTextureProcessorPitch(ProcessorID ID, UINT i)
	{return textureProcessors[ID].m_pLockedRects[i].Pitch;}
#endif
        void applyTextures();

        void setSamplerState(const char *samplerName, const SamplerStateID samplerState);
        void applySamplerStates();

        void setShaderConstantRaw(const char *name, const void *data, const int size);

virtual ShaderID addHLSLShaderVrtl( std::vector<std::string>& avText,
                                    std::vector<std::string>& avMain,
                                    std::vector<ShaderType>& avType,
                                    const unsigned int flags);
virtual ShaderID addShaderHLSLVrtl(	const std::string& aFileName, 
					std::vector<std::string>& avMain, 
					std::vector<ShaderType>& avType);

        VertexFormatID addVertexFormatVrtl(const FormatDesc *formatDesc, const unsigned int nAttribs, const ShaderID shader);
        VertexBufferID addVertexBufferVrtl(const int size, const BufferAccess bufferAccess, const void *data, VertexBufferID old);//, const IDataProcessor* p);
        IndexBufferID addIndexBufferVrtl(const unsigned int nIndices, const unsigned int indexSize, const BufferAccess bufferAccess, const void *data, IndexBufferID old);//, const IDataProcessor* p);

		int deleteVertexBuffer(const VertexBufferID vertexBuffer);
		int deleteIndexBuffer(const IndexBufferID indexBuffer);

    AlphaStateID addAlphaState(
		const int func,
		const float ref
		);
	void changeAlphaState(const AlphaStateID alphaState);

        SamplerStateID addSamplerStateVrtl(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod, const unsigned int maxAniso, const int compareFunc, const float *border_color);
        BlendStateID addBlendState(const int srcFactorRGB, const int destFactorRGB, const int srcFactorAlpha, const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, const int mask, const bool alphaToCoverage);
        DepthStateID addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc, const bool stencilTest,


                const uint8 stencilMask, const int stencilFunc, const int stencilFail, const int depthFail, const int stencilPass);
        RasterizerStateID addRasterizerStateVrtl(const int cullMode, const int fillMode, const bool multiSample, const bool scissor);

        void changeShader(const ShaderID shader);
        void changeVertexFormat(const VertexFormatID vertexFormat);
        void changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, const intptr offset);
        void changeIndexBuffer(const IndexBufferID indexBuffer);

//      void changeSamplerState(const unsigned int samplerUnit, const SamplerStateID samplerState);
        void changeBlendStateVrtl(const BlendStateID blendState, const unsigned int sampleMask);
        void changeDepthStateVrtl(const DepthStateID depthState, const unsigned int stencilRef);
        void changeRasterizerState(const RasterizerStateID rasterizerState);

    virtual void SaveScreenShotVrtl(const char* fn);
	virtual void captureScreenshot(Image3& img);
	virtual void Init0(void);

int SaveDCToBitmap(HWND hStatic, const char *pszFile);
	virtual void setup2DMode(const float left, const float right, const float top, const float bottom);

        virtual unsigned int DrawPrimitive(Primitives PrimitiveType,
                UINT StartVertex,
                UINT PrimitiveCount, const unsigned int flags);
    virtual unsigned int DrawIndexedPrimitive(Primitives PrimitiveType,
                INT BaseVertexIndex,
                UINT MinVertexIndex,
                UINT NumVertices,
                UINT startIndex,
                UINT primCount, const unsigned int flags);
    virtual void BeginSceneVrtl(bool aB);
    virtual void EndSceneVrtl(bool aB);
	virtual void Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors);
    virtual void PresentVrtl(bool aB);
	virtual void resetVrtl(const unsigned int flags);
	virtual void resetToDefaults();
	virtual float getTextWidth(const FontID font, const char *str, int length) const;

	virtual unsigned int GetTextureCount(){return textures.size();}
	virtual FORMAT GetTextureFormat(const TextureID texture)
	{
		if(texture>=textures.size())
			return FORMAT_NONE;
		return textures[texture].format;
	}
		virtual unsigned int GetShaderCount(){return shaders.size();}
protected:
        bool createFrameBufferSurfaces();
        bool releaseFrameBufferSurfaces();
		void setD3Ddefaults();

public:
 union {
D3DCAPS9 m_caps;
D3DCAPS9 caps;
D3DCAPS9 d3dcaps;
        };
        union {
LPDIRECT3D9                     m_pD3D;
LPDIRECT3D9                     d3d;
        };
		union {
        LPDIRECT3DDEVICE9 dev;
        LPDIRECT3DDEVICE9 m_pd3dDevice;
		};
//      TextureFilter *currentFilters;

		int selectedVTStreams[MAX_TEXTUREUNIT];
		int selectedVTBuffers[MAX_TEXTUREUNIT];

        D3DXFROMWINEVECTOR4 vsRegs[256];
        D3DXFROMWINEVECTOR4 psRegs[224];
        int minVSDirty, maxVSDirty;
        int minPSDirty, maxPSDirty;

        LPDIRECT3DQUERY9 eventQuery;
};
//include "core.h"
//include <Framework3/stdlib.h>

inline void ReflectNOFX(LPD3DXCONSTANTTABLE pConstantTable)
{
	char buf[256];
	D3DXCONSTANT_DESC constDesc[32];
	UINT pdwNumConsts = 32;
	UINT i,j;
	for (j=0;j<32;j++)
	{
		D3DXHANDLE handle = pConstantTable->GetConstant(0,j);
		if (handle != 0)
		{
			pConstantTable->GetConstantDesc(handle,constDesc,&pdwNumConsts);
			for (i=0;i<pdwNumConsts;i++)
			{
				if( constDesc[i].RegisterSet == D3DXRS_SAMPLER )
                    stx_snprintf( buf,256,"Sampler %s %d\n",
						constDesc[i].Name,constDesc[i].RegisterIndex);
				else
                    stx_snprintf( buf,256,"Constant %s %d\n",
						constDesc[i].Name,constDesc[i].RegisterIndex);
				OutputDebugStringA(buf);
				if(i+1>=pdwNumConsts)break;
			}
		}
	}
}
inline void ReflectFX(LPD3DXEFFECTCOMPILER	pCompiler, D3DXHANDLE hFunction, LPCSTR pTarget)
{
	__DWORD__ Flags=0;
		LPD3DXBUFFER pShader=0;
		LPD3DXBUFFER pErrorMsgs=0;
		LPD3DXCONSTANTTABLE pConstantTable=0;

	pCompiler->CompileShader(
		hFunction,
		pTarget,
		Flags,
	&pShader,
		&pErrorMsgs,
		&pConstantTable
		);
	ReflectNOFX(pConstantTable);
    SAFE_RELEASE(pShader);
	SAFE_RELEASE(pErrorMsgs);
	SAFE_RELEASE(pConstantTable);
}
#endif
#endif

