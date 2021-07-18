#if defined(METAL)
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
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#ifndef __Framework3Metal_H__
#define __Framework3Metal_H__

// Metal includes
#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>

struct ShaderMetal: public Shader
{
	ShaderMetal():Shader()
	{
	}
	virtual ~ShaderMetal(){}
};

struct TextureMetal: public Texture
{
	TextureMetal():Texture()
	{
		glTexID=0;
		glDepthID=0;
		glTarget=0;
		glTargetW=0;
		components=0;
		wrap=0;
		glFormat=0;
		flags=0;
		lod=0.0f;
		mipMapped=false;
		samplerState=-1;
		TU=0;
	}
	virtual ~TextureMetal(){}

	//???union {
	unsigned int glTexID;
	unsigned int glDepthID;
	//???};
		//union {
	unsigned int glTarget;
	unsigned int glTargetW;
		//};
	unsigned int components;
	unsigned int wrap;
	unsigned int glFormat;
	unsigned int flags;
	float lod;
	bool mipMapped;
	SamplerStateID samplerState;
	int TU;
	BYTE* Lock(int m, int& pitch)//int& slicePitch;
;
	void Unlock(int m)
;

};
struct SamplerStateMetal: public SamplerState
{
	SamplerStateMetal():SamplerState()
	{
		minFilter=0;
		magFilter=0;
		wrapS=0;
		wrapT=0;
		wrapR=0;
		aniso=0;
		lod=0.0f;
	}
	virtual ~SamplerStateMetal(){}
	int minFilter;
	int magFilter;
	int wrapS;
	int wrapT;
	int wrapR;
	int aniso;
	float lod;
};

struct Attrib {
	int stream;
	int size;
	AttributeFormat format;
	int offset;
};
#define MAX_GENERIC 8
#define MAX_TEXCOORD 8
struct VertexFormatMetal: public VertexFormat
{
	VertexFormatMetal():VertexFormat()
	{
		maxGeneric=0;
        	maxTexCoord=0;
		pBase=0;
	}
	virtual ~VertexFormatMetal(){}
        Attrib generic[MAX_GENERIC];
        Attrib texCoord[MAX_TEXCOORD];
        Attrib vertex;
        Attrib normal;
        int vertexSize[MAX_VERTEXSTREAM];
        int maxGeneric;
        int maxTexCoord;
	ShaderID shader;
	BYTE* pBase;
};
struct VertexBufferMetal: public VertexBuffer
{
	VertexBufferMetal():VertexBuffer()
	{
		vboVB=0;
		size=0;
		data=0;
        	//data.resize(0);
	}
	virtual ~VertexBufferMetal(){}
	unsigned int vboVB;
	unsigned int size;
	BYTE* Lock(){}
	void Unlock(){}
	BYTE* data;
};
struct IndexBufferMetal: public IndexBuffer
{
	IndexBufferMetal():IndexBuffer()
	{
       		vboIB=0;
        	nIndices=0;
        	indexSize=0;
		data=0;
        	//data.resize(0);
	}
	virtual ~IndexBufferMetal(){}
        unsigned int vboIB;
        unsigned int nIndices;
        unsigned int indexSize;
	BYTE* Lock(){}
	void Unlock(){}
	//std::vector<BYTE> data;
	BYTE* data;
};
struct AlphaStateMetal: public AlphaState
{
	AlphaStateMetal():AlphaState()
{
		blendEnable=false;
		alphaFunc=0;
	}
	virtual ~AlphaStateMetal(){}
	bool blendEnable;
	int alphaFunc;
};
struct BlendStateMetal: public BlendState
{
	BlendStateMetal():BlendState()
	{
		mask=0;
        	blendEnable=false;
       		alphaEnable=false;
		alphaToCoverageEnable=false;
	}
	virtual ~BlendStateMetal(){}
        unsigned int srcFactorRGB;
        unsigned int dstFactorRGB;
        unsigned int srcFactorAlpha;
        unsigned int dstFactorAlpha;
        unsigned int blendModeRGB;
        unsigned int blendModeAlpha;
        int mask;
        bool blendEnable;
        bool alphaEnable;
	bool alphaToCoverageEnable;
};
struct DepthStateMetal: public DepthState
{
	DepthStateMetal():DepthState()
	{
        	depthFunc=0;
        	depthTest=false;
        	depthWrite=false;
	}
	virtual ~DepthStateMetal(){}
        int depthFunc;
        bool depthTest;
        bool depthWrite;
};
struct RasterizerStateMetal: public RasterizerState
{
	RasterizerStateMetal():RasterizerState()
	{
		cullMode=0;
        	fillMode=0;
       		multiSample=false;
        	scissor=false;
	}
	virtual ~RasterizerStateMetal(){}
        int cullMode;
        int fillMode;
        bool multiSample;
        bool scissor;
};

struct RendererMetal : public RendererHLSLCg
{
	RendererMetal(bool fullScreen=false, int argc=0, char *argv[]=0);
	virtual ~RendererMetal(){}
	protected:
		bool mOk;
	public:
		virtual unsigned int GetTextureCount(){return textures.size();}
		virtual unsigned int GetShaderCount(){return shaders.size();}
	virtual Texture* GetTexture(TextureID id)
	{
		if(id==-1)
			return 0;
		if(id>=textures.size())
			return 0;
		return &textures[id];
	}
		virtual const char* getString(){return "Metal";}
		virtual bool IsOK(){return mOk;}
	virtual void resetToDefaults();
	void setD3Ddefaults(){}
	TextureID addTexture(Image3 &img, const SamplerStateID samplerState = SS_NONE, unsigned int flags = 0);
	bool resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize){return false;}
	bool generateMipMaps(const TextureID renderTarget){return false;}
	int removeTexture(const TextureID texture){return 0;}
	virtual ShaderID addHLSLShaderVrtl(const char *vsText, const char *gsText, const char *fsText, const char *vsName, const char *psName, const int vsLine, const int gsLine, const int fsLine,
                const char *header, const char *extra, const char *fileName, const char **attributeNames, const int nAttributes, const unsigned int flags);

virtual ShaderID addShaderHLSLVrtl(const char* aFileName, std::vector<std::string> avMain, std::vector<ShaderType> avType){return SHADER_NONE;}

	BlendStateID addBlendState(const int srcFactorRGB, const int destFactorRGB, const int srcFactorAlpha, const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, const int mask = ALL, const bool alphaToCoverage = false);
	DepthStateID addDepthState(const bool depthTest, const bool depthWrite, const int depthFunc = LEQUAL, const bool stencilTest = false,
		const uint8 stencilMask = 0xFF, const int stencilFunc = ALWAYS, const int stencilFail = KEEP, const int depthFail = KEEP, const int stencilPass = KEEP);
	void setTextureSlice(const char *textureName, const TextureID texture, const int slice){}
	void applyTextures();
	void setSamplerState(const char *samplerName, const SamplerStateID samplerState){}
	void applySamplerStates();
	void setShaderConstantRaw(const char *name, const void *data, const int size);
	void applyConstants();
	virtual void changeToMainFramebuffer();
	void changeShader(const ShaderID shaderID);
	void changeVertexFormat(const VertexFormatID vertexFormatID);
	void changeIndexBuffer(const IndexBufferID indexBufferID);
	void changeCullFace(const int cullFace);
	void changeRasterizerState(const RasterizerStateID rasterizerState);
	void clear(const bool clearColor, const bool clearDepth, const bool clearStencil, const float *color, const float depth, const unsigned int stencil);
	void clearRenderTarget(const TextureID renderTarget, const D3DXFROMWINEVECTOR4 &color, const int slice = NO_SLICE);
	void clearDepthTarget(const TextureID depthTarget, const float depth = 1.0f, const int slice = NO_SLICE);
#if 0
	TextureID addTexture(ID3D11Resource *resource, unsigned int flags = 0){return -1;}
	void setFrameBuffer(ID3D11RenderTargetView *colorRTV, ID3D11DepthStencilView *depthDSV){}
	ID3D11Resource *getResource(const TextureID texture) const{return 0;}
#endif
	void flush(){}
	void finish(){}
			virtual void setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState=-1
		, const int vtstream=-1
		, const VertexBufferID vtbuffer=-1
		);
			virtual void SaveScreenShotVrtl(const char* fn){}
			virtual void BeginSceneVrtl(bool aB){}
			virtual void EndSceneVrtl(bool aB){OnEndScene();}
			virtual void PresentVrtl(bool aB){}
			virtual IndexBufferID addIndexBufferVrtl(const unsigned int nIndices, const unsigned int indexSize, const BufferAccess bufferAccess, const void *data, IndexBufferID old);
			virtual RasterizerStateID addRasterizerStateVrtl(const int cullMode, const int fillMode, const bool multiSample, const bool scissor);
			virtual TextureID addRenderTargetVrtl(const int width, const int height, const int depth, const int mipMapCount, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags);
			virtual TextureID addRenderDepthVrtl(const int width, const int height, const int arraySize, const FORMAT format, const int msaaSamples, const SamplerStateID samplerState, unsigned int flags);
			virtual SamplerStateID addSamplerStateVrtl(const Filter filter, const AddressMode s, const AddressMode t, const AddressMode r, const float lod, const unsigned int maxAniso, const int compareFunc, const float *border_color);
			virtual VertexBufferID addVertexBufferVrtl(const int size, const BufferAccess bufferAccess, const void *data, VertexBufferID old);
			virtual VertexFormatID addVertexFormatVrtl(const FormatDesc *formatDesc, const unsigned int nAttribs, const ShaderID shader);
			virtual void changeBlendStateVrtl(const BlendStateID blendState, const unsigned int sampleMask);
	        virtual void changeDepthStateVrtl(const DepthStateID depthState, const unsigned int stencilRef);
	        virtual void changeRenderTargetsVrtl(const TextureID *colorRTs, const unsigned int nRenderTargets, const TextureID depthRT, const int depthSlice, const int *slices);
	        virtual void changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, const intptr offset);
	        virtual BYTE *fillVertexBufferVrtl(const VertexBufferID id, BYTE*  m_pData, int l){return 0;}
			virtual BYTE *fillIndexBufferVrtl(const IndexBufferID id, BYTE*  m_pData, int l){return 0;}
	D3DXFROMWINEVECTOR4& GetscaleBias2D(){return scaleBias2D;}
			virtual BlendStateID getBlendStateVrtl(const TextureID texture){return -1;}
	        virtual void resetVrtl(const unsigned int flags);
	        virtual int texturePitchVrtl(const TextureID id,int level){return 0;}
			virtual BYTE *unlockTextureVrtl(const TextureID id,int level){return 0;}
			virtual void setup2DMode(const float left, const float right, const float top, const float bottom){}
			virtual unsigned int drawArrays(const Primitives primitives, const int firstVertex, const int nVertices){}
			virtual unsigned int drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices){}


	virtual unsigned int drawPrimitivesVrtl(	const Primitives primitives,
						const unsigned int firstv, const unsigned int countv,
						const unsigned int firsti, const unsigned int counti,
				const ubyte* vbdata, const unsigned int vbsize,
				const ubyte* ibdata, const unsigned int ibsize){}
			virtual void Init0(void){}
			bool captureScreenshot(Image3* img){return false;}
			virtual void captureScreenshot(Image3& img){}
			void SaveScreenShot3(const char* fn){}
			float	GetMaxPointSpriteSize(){IMPLEMENT;return -1.0;}
			void	BeginPointSpriteRendering(){IMPLEMENT;}
			void	EndPointSpriteRendering(){IMPLEMENT;}
			AlphaStateID addAlphaState(const int func, const float ref);
			virtual void changeAlphaState(const AlphaStateID alphaState);
		    virtual unsigned int DrawPrimitive(Primitives PrimitiveType, UINT StartVertex, UINT PrimitiveCount, const unsigned int flags){}
		    virtual unsigned int DrawIndexedPrimitive(Primitives PrimitiveType,
		                INT BaseVertexIndex,
		                UINT MinVertexIndex,
		                UINT NumVertices,
		                UINT startIndex,
		                UINT primCount, const unsigned int flags){}
		    virtual void Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors);
		    virtual float getTextWidth(const FontID font, const char *str, int length) const{return 0.0f;}
			virtual unsigned int GetLevelCount(TextureID id){return 0;}
			virtual void GetLevelDesc(TextureID id, unsigned int level,SURFACE_DESC* sDesc){}
			virtual void GetDesc(VertexBufferID id,VERTEXBUFFER_DESC* sDesc){}
			virtual void GetDesc(IndexBufferID id, INDEXBUFFER_DESC* sDesc){}
	    	virtual BYTE* LockTexture(TextureID id, int m, int& pitch){return 0;}
	    	virtual void UnlockTexture(TextureID id, int m){}
	    	virtual BYTE* LockVertexBuffer(VertexBufferID id){return 0;}
	    	virtual void UnLockVertexBuffer(VertexBufferID id){}
	    	virtual BYTE* LockIndexBuffer(IndexBufferID id){return 0;}
	    	virtual void UnLockIndexBuffer(IndexBufferID id){}
			virtual int GetTextureWidth(TextureID id){return 0;}
			virtual int GetTextureHeight(TextureID id){return 0;}
			virtual UINT32 GetAvailableTextureMem(){return 0;}
			virtual int deleteVertexBuffer(const VertexBufferID vertexBuffer){return 0;}
			virtual int deleteIndexBuffer(const IndexBufferID indexBuffer){return 0;}
			virtual BYTE* lockTexture(const TextureID texture, int level, int& pitch){return 0;}
			virtual BYTE* lockVertexBuffer(const VertexBufferID vertexBuffer){return 0;}
			virtual BYTE* lockIndexBuffer(const IndexBufferID indexBuffer){return 0;}
			virtual BYTE* unlockVertexBuffer(const VertexBufferID vertexBuffer){return 0;}
			virtual BYTE* unlockIndexBuffer(const IndexBufferID indexBuffer){return 0;}
			virtual void buildPerspectiveMatrix(float fieldOfView,
                                   float aspectRatio,
                                   float zNear, float zFar){}//,D3DXFROMWINEMATRIX& m)
        virtual void buildLookAtMatrix(float eyex, float eyey, float eyez,
                              float centerx, float centery, float centerz,
                              float upx, float upy, float upz){}//,D3DXFROMWINEMATRIX& m)
			virtual TextureID addTexture(Image3* img, const bool useMipMaps=false, const SamplerStateID samplerState = SS_NONE, unsigned int flags = 0);
			virtual void viewport(int x, int y, int viewportWidth, int viewportHeight);
protected:
	VArray <TextureMetal> textures;
	VArray <ShaderMetal> shaders;
	VArray <VertexBufferMetal> vertexBuffers;
	VArray <IndexBufferMetal> indexBuffers;
	VArray <VertexFormatMetal> vertexFormats;
	VArray <SamplerStateMetal> samplerStates;
	VArray <BlendStateMetal> blendStates;
	VArray <AlphaStateMetal> alphaStates;
	VArray <DepthStateMetal> depthStates;
	VArray <RasterizerStateMetal> rasterizerStates;
};

#endif // _MetalRENDERER_H_
#endif

