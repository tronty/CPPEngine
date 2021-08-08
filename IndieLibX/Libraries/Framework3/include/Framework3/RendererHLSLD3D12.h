#if defined(D3D12)
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

#ifndef __Framework3D3D12_H__
#define __Framework3D3D12_H__

// Direct3D12 includes
#ifdef _MSC_VER
//include <d3d12.h>
#endif
#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>

#define ROLLING_VB_SIZE (64 * 1024)

struct TextureD3D12 : public Texture {
	int width, height, depth;
	int arraySize;
	unsigned int flags;

	BYTE* Lock(int m, int& pitch);
	void Unlock(int m);
};

struct ConstantD3D12 : public Constant {
	ConstantD3D12() : Constant()
	{
		name="";
		vsData=0;
		psData=0;
		vsBuffer=0;
		psBuffer=0;
	}
	ConstantD3D12(const char *aname)
	{
		ConstantD3D12();
		name = aname;
	}
	virtual ~ConstantD3D12()
	{
	}
	ConstantD3D12(const ConstantD3D12& rhs)
	{
		name = rhs.name;
		vsData=rhs.vsData;
		psData=rhs.psData;
		vsBuffer=rhs.vsBuffer;
		psBuffer=rhs.psBuffer;
	}
	std::string name;
	ubyte *vsData;
	//ubyte *gsData;
	ubyte *psData;
	int vsBuffer;
	//int gsBuffer;
	int psBuffer;
};

inline int constantCompD3D12(const void *s0, const void *s1){
        return strcmp(((ConstantD3D12 *) s0)->name.c_str(), ((ConstantD3D12 *) s1)->name.c_str());
}

struct SamplerD3D12 : public Sampler {
	SamplerD3D12() : Sampler()
	{
		name="";
		vsIndex=0;
		psIndex=0;
	}
	SamplerD3D12(const char *aname)
	{
		SamplerD3D12();
		name = aname;
	}
	virtual ~SamplerD3D12()
	{
	}
	SamplerD3D12(const SamplerD3D12& rhs)
	{
		name = rhs.name;
		vsIndex=rhs.vsIndex;
		psIndex=rhs.psIndex;
	}
	std::string name;
	int vsIndex;
	//int gsIndex;
	int psIndex;
};

inline int samplerCompD3D12(const void *s0, const void *s1){
        return strcmp(((SamplerD3D12 *) s0)->name.c_str(), ((SamplerD3D12 *) s1)->name.c_str());
}

struct ShaderD3D12 : public Shader {
	unsigned int nVSCBuffers;
	//unsigned int nGSCBuffers;
	unsigned int nPSCBuffers;

	VArray<ConstantD3D12> constants;
	VArray<SamplerD3D12> textures;
	VArray<SamplerD3D12> samplers;

	unsigned int nConstants;
	unsigned int nTextures;
	unsigned int nSamplers;

	ubyte **vsConstMem;
	//ubyte **gsConstMem;
	ubyte **psConstMem;

	bool *vsDirty;
	//bool *gsDirty;
	bool *psDirty;
};

struct VertexFormatD3D12 : public VertexFormat {
	unsigned int vertexSize[MAX_VERTEXSTREAM];
};

struct VertexBufferD3D12 : public VertexBuffer
{
	VertexBufferD3D12():VertexBuffer()
	{
	}
	virtual ~VertexBufferD3D12(){}
	int size;
	BYTE* Lock(){}
	void Unlock(){}
};
struct IndexBufferD3D12 : public IndexBuffer
{
	IndexBufferD3D12():IndexBuffer()
	{
	}
	virtual ~IndexBufferD3D12(){}
	unsigned int nIndices;
	unsigned int indexSize;
	BYTE* Lock(){}
	void Unlock(){}

};

struct AlphaStateD3D12 : public AlphaState
{
	AlphaStateD3D12():AlphaState()
	{
		blendEnable=false;
	}
	virtual ~AlphaStateD3D12(){}
	int alphaFunc;
	bool blendEnable;
};
struct SamplerStateD3D12 : public SamplerState {
};

struct BlendStateD3D12 : public BlendState {
};

struct DepthStateD3D12 : public DepthState {
};

struct RasterizerStateD3D12 : public RasterizerState {
};

struct RendererD3D12 : public RendererHLSLCg
{
	RendererD3D12(bool fullScreen=false, int argc=0, char *argv[]=0);
	virtual ~RendererD3D12();
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
		virtual const char* getString(){return "D3D12";}
		virtual bool IsOK(){return mOk;}
	virtual void resetToDefaults();
	void setD3Ddefaults(){}
	TextureID addTexture(Image3 &img, const SamplerStateID samplerState = SS_NONE, unsigned int flags = 0);
	bool resizeRenderTarget(const TextureID renderTarget, const int width, const int height, const int depth, const int mipMapCount, const int arraySize){return false;}
	bool generateMipMaps(const TextureID renderTarget){return false;}
	int removeTexture(const TextureID texture){return 0;}

virtual ShaderID addHLSLShaderVrtl( std::vector<std::string>& avText,
                                    std::vector<std::string>& avMain,
                                    std::vector<ShaderType>& avType,
                                    const unsigned int flags);
virtual ShaderID addShaderHLSLVrtl(	const std::string& aFileName, 
					std::vector<std::string>& avMain, 
					std::vector<ShaderType>& avType);

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
			virtual void BeginSceneVrtl(bool aB);
			virtual void EndSceneVrtl(bool aB);
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
			virtual unsigned int drawArrays(const Primitives primitives, const int firstVertex, const int nVertices);
			virtual unsigned int drawElements(const Primitives primitives, const int firstIndex, const int nIndices, const int firstVertex, const int nVertices);


	virtual unsigned int drawPrimitivesVrtl(	const Primitives primitives,
						const unsigned int firstv, const unsigned int countv,
						const unsigned int firsti, const unsigned int counti,
				const ubyte* vbdata, const unsigned int vbsize,
				const ubyte* ibdata, const unsigned int ibsize);
			virtual void Init0(void){}
			bool captureScreenshot(Image3* img){return false;}
			virtual void captureScreenshot(Image3& img){}
			void SaveScreenShot3(const char* fn){}
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
			VkShaderModule loadShaderGLSL(const char *shaderSrc_, VkShaderStageFlagBits stage);
			void reflectGLShaderVK(unsigned int aprogram, std::vector<std::string>& uniformnames, std::vector<unsigned int>& uniformoffsets);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t findMemoryType(tr_renderer* p, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
protected:
	D3DXFROMWINEVECTOR4 scaleBias2D;
	VArray <TextureD3D12> textures;
	VArray <ShaderD3D12> shaders;
	VArray <VertexBufferD3D12> vertexBuffers;
	VArray <IndexBufferD3D12> indexBuffers;
	VArray <VertexFormatD3D12> vertexFormats;
	VArray <SamplerStateD3D12> samplerStates;
	VArray <BlendStateD3D12> blendStates;
	VArray <AlphaStateD3D12> alphaStates;
	VArray <DepthStateD3D12> depthStates;
	VArray <RasterizerStateD3D12> rasterizerStates;

	uint32_t frameIdx;

	tr_render_target* render_target;

    tr_fence* image_acquired_fence;
    tr_semaphore* image_acquired_semaphore;
    tr_semaphore* render_complete_semaphores;

	uint32_t      kImageCount;
	VkDevice device;

tr_renderer*        m_renderer;
tr_cmd_pool*        m_cmd_pool;
tr_cmd**            m_cmds;
tr_buffer*          m_uniform_buffer;
uint64_t            s_frame_count;
tr_cmd* cmd;

#define LOG(STR)  { std::stringstream ss; ss << STR << std::endl; \
                    platform_log(ss.str().c_str()); }
};

#endif // _DIRECT3D12RENDERER_H_
#endif

