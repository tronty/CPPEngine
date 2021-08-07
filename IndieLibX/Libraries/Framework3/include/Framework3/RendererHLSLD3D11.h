#if defined(D3D11)
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
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#ifndef __Framework3D3D11_H__
#define __Framework3D3D11_H__

#if 1
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
#include <d3dx11.h>

// XInput includes
#include <xinput.h>

// HRESULT translation for Direct3D and other APIs
#include <dxerr.h>
#endif

#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>

#define ROLLING_VB_SIZE (64 * 1024)

/*
#define VB_INVALID (-2)
*/

struct TextureD3D11 : public Texture {
    TextureD3D11() : Texture()
    {
        texture=0;
	    pStaging=0;
	    srv=0;
	    rtv=0;
	    dsv=0;
	    srvArray=0;
	    rtvArray=0;
	    dsvArray=0;
	    texFormat=DXGI_FORMAT_UNKNOWN;
	    srvFormat=DXGI_FORMAT_UNKNOWN;
	    rtvFormat=DXGI_FORMAT_UNKNOWN;
	    dsvFormat=DXGI_FORMAT_UNKNOWN;
	    width=0;
        height=0;
        depth=0;
	    arraySize=0;
	    flags=0;
    }
	virtual ~TextureD3D11(){}
    TextureD3D11(const TextureD3D11& rhs)
    {
        texture=rhs.texture;
	    pStaging=rhs.pStaging;
	    srv=rhs.srv;
	    rtv=rhs.rtv;
	    dsv=rhs.dsv;
	    srvArray=rhs.srvArray;
	    rtvArray=rhs.rtvArray;
	    dsvArray=rhs.dsvArray;
	    texFormat=rhs.texFormat;
	    srvFormat=rhs.srvFormat;
	    rtvFormat=rhs.rtvFormat;
	    dsvFormat=rhs.dsvFormat;
	    width=rhs.width;
        height=rhs.height;
        depth=rhs.depth;
	    arraySize=rhs.arraySize;
	    flags=rhs.flags;
    }
	ID3D11Resource *texture;
	ID3D11Resource *pStaging;
	ID3D11ShaderResourceView *srv;
	ID3D11RenderTargetView   *rtv;
	ID3D11DepthStencilView   *dsv;
	ID3D11ShaderResourceView **srvArray;
	ID3D11RenderTargetView   **rtvArray;
	ID3D11DepthStencilView   **dsvArray;
	DXGI_FORMAT texFormat;
	DXGI_FORMAT srvFormat;
	DXGI_FORMAT rtvFormat;
	DXGI_FORMAT dsvFormat;
	int width, height, depth;
	int arraySize;
	unsigned int flags;

	BYTE* Lock(int m, int& pitch);
	void Unlock(int m);
};

struct ConstantD3D11 : public Constant {
	ConstantD3D11() : Constant()
	{
		name="";
		vsData=0;
		psData=0;
		vsBuffer=0;
		psBuffer=0;
		gsData=0;
		gsBuffer=0;
		hsData=0;
		hsBuffer=0;
		dsData=0;
		dsBuffer=0;
		csData=0;
		csBuffer=0;
        size=0;
        type=0;
	}
	ConstantD3D11(const char *aname)
	{
		ConstantD3D11();
		name = aname;
	}
	virtual ~ConstantD3D11()
	{
	}
	ConstantD3D11(const ConstantD3D11& rhs)
	{
		name = rhs.name;
        size=rhs.size;
        type=rhs.type;
		vsData=rhs.vsData;
		psData=rhs.psData;
		vsBuffer=rhs.vsBuffer;
		psBuffer=rhs.psBuffer;
		gsData=rhs.gsData;
		gsBuffer=rhs.gsBuffer;
		hsData=rhs.hsData;
		hsBuffer=rhs.hsBuffer;
		dsData=rhs.dsData;
		dsBuffer=rhs.dsBuffer;
		csData=rhs.csData;
		csBuffer=rhs.csBuffer;
	}
	std::string name;
    UINT size;
    UINT type;
	ubyte *vsData;
	ubyte *gsData;
	ubyte *psData;
	int vsBuffer;
	int gsBuffer;
	int psBuffer;
	ubyte *hsData;
	int hsBuffer;
	ubyte *dsData;
	int dsBuffer;
	ubyte *csData;
	int csBuffer;
};

inline int constantCompD3D11(const void *s0, const void *s1){
        return strcmp(((ConstantD3D11 *) s0)->name.c_str(), ((ConstantD3D11 *) s1)->name.c_str());
}

struct SamplerD3D11 : public Sampler {
	SamplerD3D11() : Sampler()
	{
		name="";
		vsIndex=0;
		psIndex=0;
		gsIndex=0;
		hsIndex=0;
		dsIndex=0;
		csIndex=0;
	}
	SamplerD3D11(const char *aname)
	{
		SamplerD3D11();
		name = aname;
	}
	virtual ~SamplerD3D11()
	{
	}
	SamplerD3D11(const SamplerD3D11& rhs)
	{
		name = rhs.name;
		vsIndex=rhs.vsIndex;
		psIndex=rhs.psIndex;
		gsIndex=rhs.gsIndex;
		hsIndex=rhs.hsIndex;
		dsIndex=rhs.dsIndex;
		csIndex=rhs.csIndex;
	}
	std::string name;
	int vsIndex;
	int gsIndex;
	int psIndex;
	int hsIndex;
	int dsIndex;
	int csIndex;
};

inline int samplerCompD3D11(const void *s0, const void *s1){
        return strcmp(((SamplerD3D11 *) s0)->name.c_str(), ((SamplerD3D11 *) s1)->name.c_str());
}

struct ShaderD3D11 : public Shader {
    ShaderD3D11() : Shader()
    {
        vertexShader=0;
        pixelShader=0;
        geometryShader=0;
        hullShader=0;
        domainShader=0;
        computeShader=0;
        nVSCBuffers=0;
        nGSCBuffers=0;
        nPSCBuffers=0;
        nHSCBuffers=0;
        nDSCBuffers=0;
        nCSCBuffers=0;
        nConstants=0;
        nTextures=0;
        nSamplers=0;
        inputSignature=0;
    }
	virtual ~ShaderD3D11(){}
    ShaderD3D11(const ShaderD3D11& rhs)
    {
	    vertexShader=rhs.vertexShader;
        pixelShader=rhs.pixelShader;
        geometryShader=rhs.geometryShader;
        hullShader=rhs.hullShader;
        domainShader=rhs.domainShader;
        computeShader=rhs.computeShader;
        inputSignature=rhs.inputSignature;
        nVSCBuffers=rhs.nVSCBuffers;
        nGSCBuffers=rhs.nGSCBuffers;
        nPSCBuffers=rhs.nPSCBuffers;
        nHSCBuffers=rhs.nHSCBuffers;
        nDSCBuffers=rhs.nDSCBuffers;
        nCSCBuffers=rhs.nCSCBuffers;
        nConstants=rhs.nConstants;
        nTextures=rhs.nTextures;
        nSamplers=rhs.nSamplers;

        vsConstants=rhs.vsConstants; // ???
	    gsConstants=rhs.gsConstants;
        psConstants=rhs.psConstants; // ???
        hsConstants=rhs.hsConstants;
        dsConstants=rhs.dsConstants;
        csConstants=rhs.csConstants;
        constants=rhs.constants;
        textures=rhs.textures;
        samplers=rhs.samplers;

	    vsConstMem=rhs.vsConstMem; // ???
	    gsConstMem=rhs.gsConstMem;
	    psConstMem=rhs.psConstMem; // ???
	    hsConstMem=rhs.hsConstMem;
	    dsConstMem=rhs.dsConstMem;
	    csConstMem=rhs.csConstMem;

	    vsDirty=rhs.vsDirty;
	    gsDirty=rhs.gsDirty;
	    psDirty=rhs.psDirty;
	    hsDirty=rhs.hsDirty;
	    dsDirty=rhs.dsDirty;
	    csDirty=rhs.csDirty;
    }
	ID3D11VertexShader *vertexShader;
	ID3D11PixelShader *pixelShader;
	ID3D11GeometryShader *geometryShader;
	ID3D11HullShader *hullShader;
	ID3D11DomainShader *domainShader;
	ID3D11ComputeShader *computeShader;
	ID3DBlob *inputSignature;

	ID3D11Buffer **vsConstants; // ???
	ID3D11Buffer **gsConstants;
	ID3D11Buffer **psConstants; // ???
	ID3D11Buffer **hsConstants;
	ID3D11Buffer **dsConstants;
	ID3D11Buffer **csConstants;
	unsigned int nVSCBuffers;
	unsigned int nGSCBuffers;
	unsigned int nPSCBuffers;
	unsigned int nHSCBuffers;
	unsigned int nDSCBuffers;
	unsigned int nCSCBuffers;

	VArray<ConstantD3D11> constants; // ???
	VArray<SamplerD3D11> textures; // ???
	VArray<SamplerD3D11> samplers;

	unsigned int nConstants;
	unsigned int nTextures;
	unsigned int nSamplers;

	ubyte **vsConstMem; // ???
	ubyte **gsConstMem;
	ubyte **psConstMem; // ???

	bool *vsDirty;
	bool *gsDirty;
	bool *psDirty;

	ubyte **hsConstMem;
	bool *hsDirty;
	ubyte **dsConstMem;
	bool *dsDirty;
	ubyte **csConstMem;
	bool *csDirty;
};

struct VertexFormatD3D11 : public VertexFormat {
    VertexFormatD3D11(): VertexFormat() {inputLayout=0;}
	virtual ~VertexFormatD3D11(){}
    VertexFormatD3D11(const VertexFormatD3D11& rhs)
    {
        inputLayout=rhs.inputLayout;
        for(unsigned int i=0;i<MAX_VERTEXSTREAM;i++)
            vertexSize[i]=rhs.vertexSize[i];
    }
	ID3D11InputLayout *inputLayout;
	unsigned int vertexSize[MAX_VERTEXSTREAM];
};

struct VertexBufferD3D11 : public VertexBuffer
{
		//friend RendererHLSLD3D11;
	VertexBufferD3D11():VertexBuffer()
    {
        vertexBuffer=0;
	uav=0;
	    size=0;
    }
    VertexBufferD3D11(const VertexBufferD3D11& rhs)
    {
        vertexBuffer=rhs.vertexBuffer;
        uav=rhs.uav;
        size=rhs.size;
    }
	virtual ~VertexBufferD3D11(){}
	ID3D11Buffer *vertexBuffer;
	ID3D11UnorderedAccessView *uav;
	int size;
	BYTE* Lock();
	void Unlock();
};
struct IndexBufferD3D11 : public IndexBuffer
{
		//friend RendererHLSLD3D11;
	IndexBufferD3D11():IndexBuffer()
    {
        indexBuffer=0;
	    nIndices=0;
	    indexSize=0;
    }
	virtual ~IndexBufferD3D11(){}
    IndexBufferD3D11(const IndexBufferD3D11& rhs)
    {
        indexBuffer=rhs.indexBuffer;
        nIndices=rhs.nIndices;
        indexSize=rhs.indexSize;
    }
	ID3D11Buffer *indexBuffer;
	unsigned int nIndices;
	unsigned int indexSize;
	BYTE* Lock();
	void Unlock();

};

struct AlphaStateD3D11 : public AlphaState
{
	AlphaStateD3D11():AlphaState()
	{
        alphaFunc=0;
		blendEnable=false;
	}
	virtual ~AlphaStateD3D11(){}
	int alphaFunc;
	bool blendEnable;
};
struct SamplerStateD3D11 : public SamplerState {
    SamplerStateD3D11() : SamplerState(){samplerState=0;}
    SamplerStateD3D11(const SamplerStateD3D11& rhs){samplerState=rhs.samplerState;}
	ID3D11SamplerState *samplerState;
};

struct BlendStateD3D11 : public BlendState {
    BlendStateD3D11() : BlendState(){blendState=0;}
    BlendStateD3D11(const BlendStateD3D11& rhs){blendState=rhs.blendState;}
	ID3D11BlendState *blendState;
};

struct DepthStateD3D11 : public DepthState {
    DepthStateD3D11() : DepthState(){dsState=0;}
    DepthStateD3D11(const DepthStateD3D11& rhs){dsState=rhs.dsState;}
	ID3D11DepthStencilState *dsState;
};

struct RasterizerStateD3D11 : public RasterizerState {
    RasterizerStateD3D11() : RasterizerState(){rsState=0;}
    RasterizerStateD3D11(const RasterizerStateD3D11& rhs){rsState=rhs.rsState;}
	ID3D11RasterizerState *rsState;
};
class RendererHLSLD3D11 : public RendererHLSLCg
{
	protected:
		bool mOk;
	public:
		virtual unsigned int GetTextureCount(){return textures.size();}
	virtual FORMAT GetTextureFormat(const TextureID texture)
	{
		if(texture>=textures.size())
			return FORMAT_NONE;
		return textures[texture].format;
	}
		virtual unsigned int GetShaderCount(){return shaders.size();}

	virtual Texture* GetTexture(TextureID id)
	{
		if(id==-1)
			return 0;
		if(id>=textures.size())
			return 0;
		return &textures[id];
	}
		virtual const char* getString(){return "D3D11";}
		virtual bool IsOK(){return mOk;}
	virtual void resetToDefaults();
	void setD3Ddefaults();

//	bool resetDevice();

	TextureID addTexture(ID3D11Resource *resource, unsigned int flags = 0);
	TextureID addTexture(Image3 &img, const SamplerStateID samplerState = SS_NONE, unsigned int flags = 0);

	bool resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize);
	bool generateMipMaps(const TextureID renderTarget);

	int removeTexture(const TextureID texture);

	ID3D11ShaderReflection* CompileShader(ShaderType shaderType, ShaderD3D11& shader, const char* vsText=0, const char* vsMain=0, const char* target=0);

virtual ShaderID addHLSLShaderVrtl(
const char *vsText, const char *gsText, const char *fsText, const char *csText, const char *hsText, const char *dsText,
const char *vsMain, const char *gsMain, const char *fsMain, const char *csMain, const char *hsMain, const char *dsMain,
                                            	const unsigned int flags);

	BlendStateID addBlendState(const int srcFactorRGB, const int destFactorRGB, const int srcFactorAlpha, const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, const int mask = ALL, const bool alphaToCoverage = false);
	DepthStateID addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc = LEQUAL, const bool stencilTest = false,
		const uint8 stencilMask = 0xFF, const int stencilFunc = ALWAYS, const int stencilFail = KEEP, const int depthFail = KEEP, const int stencilPass = KEEP);

    IndexBufferID addIndexBuffer(ID3D11Buffer* buffer, const uint nIndices, const uint indexSize);

        BufferID addBufferVrtl(const STX_BUFFER_DESC      *pDesc, const STX_SUBRESOURCE_DATA *pInitialData);

    SamplerStateID addSamplerState(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod, const uint maxAniso, const int compareFunc, const float *border_color);
    RasterizerStateID addRasterizerState(const int cullMode, const int fillMode, const bool multiSample, const bool scissor, const float depthBias, const float slopeDepthBias);

	void setTextureSlice(const char *textureName, const TextureID texture, const int slice);
	void applyTextures();

	void setSamplerState(const char *samplerName, const SamplerStateID samplerState);
	void applySamplerStates();

	void setShaderConstantRaw(const char *name, const void *data, const int size);
	void applyConstants();

//	void changeTexture(const unsigned int imageUnit, const TextureID textureID);
	virtual void changeToMainFramebuffer();
	void changeShader(const ShaderID shaderID);
	void changeVertexFormat(const VertexFormatID vertexFormatID);
	void changeIndexBuffer(const IndexBufferID indexBufferID);
	void changeCullFace(const int cullFace);

//	void changeSamplerState(const unsigned int samplerUnit, const SamplerStateID samplerState);
	void changeRasterizerState(const RasterizerStateID rasterizerState);


	void clear(const bool clearColor, const bool clearDepth, const bool clearStencil, const float *color, const float depth, const unsigned int stencil);
	void clearRenderTarget(const TextureID renderTarget, const D3DXFROMWINEVECTOR4 &color, const int slice = NO_SLICE);
	void clearDepthTarget(const TextureID depthTarget, const float depth = 1.0f, const int slice = NO_SLICE);

	//unsigned int drawArrays(const Primitives primitives, const int firstVertex, const int nVertices);
	//unsigned int drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices);


	virtual unsigned int drawPrimitivesVrtl(	const Primitives primitives,
						const unsigned int firstv, const unsigned int countv,
						const unsigned int firsti, const unsigned int counti,
				const ubyte* vbdata, const unsigned int vbsize,
				const ubyte* ibdata, const unsigned int ibsize);

	//void setup2DMode(const float left, const float right, const float top, const float bottom);
	//unsigned int drawPlain(const Primitives primitives, D3DXFROMWINEVECTOR2 *vertices, const unsigned int nVertices, const BlendStateID blendState, const DepthStateID depthState, const D3DXFROMWINEVECTOR4 *color = 0);
	//unsigned int drawTextured(const Primitives primitives, TexVertex *vertices, const unsigned int nVertices, const TextureID texture, const SamplerStateID samplerState, const BlendStateID blendState, const DepthStateID depthState, const D3DXFROMWINEVECTOR4 *color = 0);

	void setFrameBuffer(ID3D11RenderTargetView *colorRTV, ID3D11DepthStencilView *depthDSV){
		backBufferRTV  = colorRTV;
		depthBufferDSV = depthDSV;
	}

	ID3D11Resource *getResource(const TextureID texture) const;

	void flush();
	void finish();

			virtual void setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState=-1
		, const int vtstream=-1
		, const VertexBufferID vtbuffer=-1
		);
			virtual void SaveScreenShotVrtl(const char* fn);
			virtual void BeginSceneVrtl(bool aB){};
			virtual void EndSceneVrtl(bool aB){OnEndScene();}
			virtual void PresentVrtl(bool aB);

			virtual IndexBufferID addIndexBufferVrtl(const unsigned int nIndices, const unsigned int indexSize, const BufferAccess bufferAccess, const void *data, IndexBufferID old);//, const IDataProcessor* p);
			virtual RasterizerStateID addRasterizerStateVrtl(const int cullMode, const int fillMode, const bool multiSample, const bool scissor);
			virtual TextureID addRenderTargetVrtl(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags);
			virtual TextureID addRenderDepthVrtl(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags);
			virtual SamplerStateID addSamplerStateVrtl(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod, const unsigned int maxAniso, const int compareFunc, const float *border_color);
			virtual VertexBufferID addVertexBufferVrtl(const int size, const BufferAccess bufferAccess, const void *data, VertexBufferID old);//, const IDataProcessor* p);
			virtual VertexFormatID addVertexFormatVrtl(const FormatDesc *formatDesc, const unsigned int nAttribs, const ShaderID shader);
			virtual void changeBlendStateVrtl(const BlendStateID blendState, const unsigned int sampleMask);
	        virtual void changeDepthStateVrtl(const DepthStateID depthState, const unsigned int stencilRef);
	        virtual void changeRenderTargetsVrtl(const TextureID *colorRTs, const unsigned int nRenderTargets, const TextureID depthRT, const int depthSlice, const int *slices);
	        virtual void changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, const intptr offset);
	        virtual BYTE *fillVertexBufferVrtl(const VertexBufferID id, BYTE*  m_pData, int l);
			virtual BYTE *fillIndexBufferVrtl(const IndexBufferID id, BYTE*  m_pData, int l);

	D3DXFROMWINEVECTOR4& GetscaleBias2D(){return scaleBias2D;}

			virtual BlendStateID getBlendStateVrtl(const TextureID texture)
			{
		if(texture>=textures.size())
			return -1;
				if(textures[texture].format==FORMAT_RGB8)
					return IRenderer::GetRendererInstance()->GetblendOneOne();
				else
					return IRenderer::GetRendererInstance()->GetblendSrcAlpha();
			}
	        virtual void resetVrtl(const unsigned int flags);
	        virtual int texturePitchVrtl(const TextureID id,int level);
			virtual BYTE *unlockTextureVrtl(const TextureID id,int level);

			virtual void setup2DMode(const float left, const float right, const float top, const float bottom);
			virtual unsigned int drawArrays(const Primitives primitives, const int firstVertex, const int nVertices);
			virtual unsigned int drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices);

			virtual void Init0(void){}

			RendererHLSLD3D11(bool fullScreen=false, int argc=0, char *argv[]=0);
			virtual ~RendererHLSLD3D11();
            virtual stx_Effect* addEffectFromFile(const std::string& aFileName, const char* aDefines=0);
			virtual int getSamplerIndex(ShaderID shd, const char* aName);

			bool captureScreenshot(Image3* img);
			virtual void captureScreenshot(Image3& img);
			void SaveScreenShot3(const char* fn);

			float	GetMaxPointSpriteSize(){IMPLEMENT;return -1.0;}
			void	BeginPointSpriteRendering(){IMPLEMENT;}
			void	EndPointSpriteRendering(){IMPLEMENT;}

			AlphaStateID addAlphaState(const int func, const float ref);
			virtual void changeAlphaState(const AlphaStateID alphaState);

		    virtual unsigned int DrawPrimitive(Primitives PrimitiveType, UINT StartVertex, UINT PrimitiveCount, const unsigned int flags);
		    virtual unsigned int DrawIndexedPrimitive(Primitives PrimitiveType,
		                INT BaseVertexIndex,
		                UINT MinVertexIndex,
		                UINT NumVertices,
		                UINT startIndex,
		                UINT primCount, const unsigned int flags);


		    virtual void Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors);
		    virtual float getTextWidth(const FontID font, const char *str, int length) const;

			virtual unsigned int GetLevelCount(TextureID id);
			virtual void GetLevelDesc(TextureID id, unsigned int level,SURFACE_DESC* sDesc);
			virtual void GetDesc(VertexBufferID id,VERTEXBUFFER_DESC* sDesc);
			virtual void GetDesc(IndexBufferID id, INDEXBUFFER_DESC* sDesc);

	    	virtual BYTE* LockTexture(TextureID id, int m, int& pitch);
	    	virtual void UnlockTexture(TextureID id, int m);
	    	virtual BYTE* LockVertexBuffer(VertexBufferID id);
	    	virtual void UnLockVertexBuffer(VertexBufferID id);
	    	virtual BYTE* LockIndexBuffer(IndexBufferID id);
	    	virtual void UnLockIndexBuffer(IndexBufferID id);

			virtual int GetTextureWidth(TextureID id);
			virtual int GetTextureHeight(TextureID id);
			virtual UINT32 GetAvailableTextureMem();
			virtual int deleteVertexBuffer(const VertexBufferID vertexBuffer);
			virtual int deleteIndexBuffer(const IndexBufferID indexBuffer);

			virtual BYTE* lockTexture(const TextureID texture, int level, int& pitch);
			virtual BYTE* lockVertexBuffer(const VertexBufferID vertexBuffer);
			virtual BYTE* lockIndexBuffer(const IndexBufferID indexBuffer);

			virtual BYTE* unlockVertexBuffer(const VertexBufferID vertexBuffer);
			virtual BYTE* unlockIndexBuffer(const IndexBufferID indexBuffer);

			virtual void buildPerspectiveMatrix(float fieldOfView,
                                   float aspectRatio,
                                   float zNear, float zFar);//,D3DXFROMWINEMATRIX& m)

        virtual void buildLookAtMatrix(float eyex, float eyey, float eyez,
                              float centerx, float centery, float centerz,
                              float upx, float upy, float upz);//,D3DXFROMWINEMATRIX& m)

			virtual TextureID addTexture(Image3* img, const bool useMipMaps=false, const SamplerStateID samplerState = SS_NONE, unsigned int flags = 0);
			virtual void viewport(int x, int y, int viewportWidth, int viewportHeight);

protected:
	ID3D11ShaderResourceView *createSRV(ID3D11Resource *resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, const int firstSlice = -1, const int sliceCount = -1, ID3D11UnorderedAccessView* uav=0, const BufferAccess bufferAccess=DYNAMIC);
	ID3D11RenderTargetView   *createRTV(ID3D11Resource *resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, const int firstSlice = -1, const int sliceCount = -1);
	ID3D11DepthStencilView   *createDSV(ID3D11Resource *resource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, const int firstSlice = -1, const int sliceCount = -1);
public:
	static ID3D11Device* GetDevice(){return device;}
	static ID3D11DeviceContext* GetDeviceContext(){return ic;}
	static RendererHLSLD3D11* GetRenderer() { return dynamic_cast<RendererHLSLD3D11*>(IRenderer::GetRendererInstance()); }
protected:
	static ID3D11Device* device;
	static ID3D11DeviceContext* ic;
public:
union
{
        ID3D11RenderTargetView *RTV;
        ID3D11RenderTargetView *backBufferRTV;
};
union
{
        ID3D11DepthStencilView *DSV;
        ID3D11DepthStencilView *depthBufferDSV;
};

IDXGISwapChain*                   m_pSwapChain;// = 0;
D3D_DRIVER_TYPE                   m_driverType;// = D3D11_DRIVER_TYPE_0;

DXGI_FORMAT backBufferFormat;
DXGI_FORMAT depthBufferFormat;
int msaaSamples;
ID3D11Texture2D *m_pBackBuffer;
ID3D11Texture2D* m_pDepthBuffer;

protected:
	TextureID currentTexturesVS[MAX_TEXTUREUNIT], selectedTexturesVS[MAX_TEXTUREUNIT];
	TextureID currentTexturesGS[MAX_TEXTUREUNIT], selectedTexturesGS[MAX_TEXTUREUNIT];
	TextureID currentTexturesPS[MAX_TEXTUREUNIT], selectedTexturesPS[MAX_TEXTUREUNIT];
	int currentTextureSlicesVS[MAX_TEXTUREUNIT], selectedTextureSlicesVS[MAX_TEXTUREUNIT];
	int currentTextureSlicesGS[MAX_TEXTUREUNIT], selectedTextureSlicesGS[MAX_TEXTUREUNIT];
	int currentTextureSlicesPS[MAX_TEXTUREUNIT], selectedTextureSlicesPS[MAX_TEXTUREUNIT];
	TextureID currentTexturesHS[MAX_TEXTUREUNIT], selectedTexturesHS[MAX_TEXTUREUNIT];
	int currentTextureSlicesHS[MAX_TEXTUREUNIT], selectedTextureSlicesHS[MAX_TEXTUREUNIT];
	TextureID currentTexturesDS[MAX_TEXTUREUNIT], selectedTexturesDS[MAX_TEXTUREUNIT];
	int currentTextureSlicesDS[MAX_TEXTUREUNIT], selectedTextureSlicesDS[MAX_TEXTUREUNIT];
	TextureID currentTexturesCS[MAX_TEXTUREUNIT], selectedTexturesCS[MAX_TEXTUREUNIT];
	int currentTextureSlicesCS[MAX_TEXTUREUNIT], selectedTextureSlicesCS[MAX_TEXTUREUNIT];

	SamplerStateID currentSamplerStatesVS[MAX_SAMPLERSTATE], selectedSamplerStatesVS[MAX_SAMPLERSTATE];
	SamplerStateID currentSamplerStatesGS[MAX_SAMPLERSTATE], selectedSamplerStatesGS[MAX_SAMPLERSTATE];
	SamplerStateID currentSamplerStatesPS[MAX_SAMPLERSTATE], selectedSamplerStatesPS[MAX_SAMPLERSTATE];
	SamplerStateID currentSamplerStatesHS[MAX_SAMPLERSTATE], selectedSamplerStatesHS[MAX_SAMPLERSTATE];
	SamplerStateID currentSamplerStatesDS[MAX_SAMPLERSTATE], selectedSamplerStatesDS[MAX_SAMPLERSTATE];
	SamplerStateID currentSamplerStatesCS[MAX_SAMPLERSTATE], selectedSamplerStatesCS[MAX_SAMPLERSTATE];

private:
	ubyte *mapRollingVB(const unsigned int size);
	void unmapRollingVB(const unsigned int size);
	unsigned int copyToRollingVB(const void *src, const unsigned int size);

	VertexBufferID rollingVB;
	int rollingVBOffset;

	ID3D11Query *eventQuery;public:
	VArray <TextureD3D11> textures;
	VArray <ShaderD3D11> shaders;
	VArray <VertexBufferD3D11> vertexBuffers;
	VArray <IndexBufferD3D11> indexBuffers;
	VArray <VertexFormatD3D11> vertexFormats;
	VArray <SamplerStateD3D11> samplerStates;
	VArray <BlendStateD3D11> blendStates;
	VArray <AlphaStateD3D11> alphaStates;
	VArray <DepthStateD3D11> depthStates;
	VArray <RasterizerStateD3D11> rasterizerStates;
};

#endif // _DIRECT3D11RENDERER_H_
#endif
