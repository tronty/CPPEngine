#if defined(VULKAN)
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

#ifndef __Framework3Vulkan_H__
#define __Framework3Vulkan_H__

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>

// Vulkan includes
#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>

#define TINY_RENDERER_VK 1
#define TINY_RENDERER_IMPLEMENTATION
#if defined(TINY_RENDERER_DX)
    #include "tinydx.h"
#elif defined(TINY_RENDERER_VK)
    #include "tinyvk.h"
#endif
/*
#define LC_IMAGE_IMPLEMENTATION
#include "lc_image.h"
*/
struct RENDERERAPI UniformVulkan : public Constant {
	UniformVulkan() : Constant()
	{
		name="";
		data.resize(0);
		location=0;
		offset=0;
		//type;
		nElements=0;
        	dirty=false;
	}
	virtual ~UniformVulkan(){}
        std::string name;
        std::vector<ubyte> data;
        unsigned int location;
        unsigned int offset;
        ConstantType type;
        int nElements;
        bool dirty;
};
struct ShaderVulkan: public Shader
{
	ShaderVulkan():Shader()
	{
		//m_shader = nullptr;
		program=0;
	}
	virtual ~ShaderVulkan(){}
	unsigned int program;
	tr_shader_program* m_shader;

	std::vector<UniformVulkan> uniforms;
};

struct TextureVulkan: public Texture
{
	TextureVulkan():Texture()
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
		m_texture = nullptr;
	}
	virtual ~TextureVulkan(){}

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
	BYTE* Lock(int m, int& pitch){return 0;}
	void Unlock(int m){}
	tr_texture* m_texture;

};
struct SamplerStateVulkan: public SamplerState
{
	SamplerStateVulkan():SamplerState()
	{
		minFilter=0;
		magFilter=0;
		wrapS=0;
		wrapT=0;
		wrapR=0;
		aniso=0;
		lod=0.0f;
		m_sampler = nullptr;
	}
	virtual ~SamplerStateVulkan(){}
	int minFilter;
	int magFilter;
	int wrapS;
	int wrapT;
	int wrapR;
	int aniso;
	float lod;
	tr_sampler* m_sampler;
};

struct AttribVulkan {
	int stream;
	int size;
	AttributeFormat format;
	int offset;
};
#define MAX_GENERIC 8
#define MAX_TEXCOORD 8
struct VertexFormatVulkan: public VertexFormat
{
	VertexFormatVulkan():VertexFormat()
	{
		maxGeneric=0;
        	maxTexCoord=0;
		pBase=0;
		m_pipeline = nullptr;
		m_desc_set = nullptr;
	}
	virtual ~VertexFormatVulkan(){}
        AttribVulkan generic[MAX_GENERIC];
        AttribVulkan texCoord[MAX_TEXCOORD];
        AttribVulkan vertex;
        AttribVulkan normal;
        int vertexSize[MAX_VERTEXSTREAM];
        int maxGeneric;
        int maxTexCoord;
	ShaderID shader;
	BYTE* pBase;
	tr_pipeline*        m_pipeline;
	tr_descriptor_set*  m_desc_set;
};
struct VertexBufferVulkan: public VertexBuffer
{
	VertexBufferVulkan():VertexBuffer()
	{
		vboVB=0;
		size=0;
		data=0;
        	//data.resize(0);
		m_rect_vertex_buffer = nullptr;
	}
	virtual ~VertexBufferVulkan(){}
	unsigned int vboVB;
	unsigned int size;
	BYTE* Lock(){return 0;}
	void Unlock(){}
	BYTE* data;
	tr_buffer* m_rect_vertex_buffer;
};
struct IndexBufferVulkan: public IndexBuffer
{
	IndexBufferVulkan():IndexBuffer()
	{
       		vboIB=0;
        	nIndices=0;
        	indexSize=0;
		data=0;
        	//data.resize(0);
		m_rect_index_buffer=nullptr;
	}
	virtual ~IndexBufferVulkan(){}
        unsigned int vboIB;
        unsigned int nIndices;
        unsigned int indexSize;
	BYTE* Lock(){return 0;}
	void Unlock(){}
	//std::vector<BYTE> data;
	BYTE* data;
	tr_buffer* m_rect_index_buffer;
};
struct AlphaStateVulkan: public AlphaState
{
	AlphaStateVulkan():AlphaState()
{
		blendEnable=false;
		alphaFunc=0;
	}
	virtual ~AlphaStateVulkan(){}
	bool blendEnable;
	int alphaFunc;
};
struct BlendStateVulkan: public BlendState
{
	BlendStateVulkan():BlendState()
	{
		mask=0;
        	blendEnable=false;
       		alphaEnable=false;
		alphaToCoverageEnable=false;
	}
	virtual ~BlendStateVulkan(){}
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
struct DepthStateVulkan: public DepthState
{
	DepthStateVulkan():DepthState()
	{
        	depthFunc=0;
        	depthTest=false;
        	depthWrite=false;
	}
	virtual ~DepthStateVulkan(){}
        int depthFunc;
        bool depthTest;
        bool depthWrite;
};
struct RasterizerStateVulkan: public RasterizerState
{
	RasterizerStateVulkan():RasterizerState()
	{
		cullMode=0;
        	fillMode=0;
       		multiSample=false;
        	scissor=false;
	}
	virtual ~RasterizerStateVulkan(){}
        int cullMode;
        int fillMode;
        bool multiSample;
        bool scissor;
};

void renderer_log(tr_log_type type, const char* msg, const char* component);
static void platform_log(const char* s);
#if defined(TINY_RENDERER_VK)
VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug(
    VkDebugReportFlagsEXT      flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t                   object,
    size_t                     location,
    int32_t                    messageCode,
    const char*                pLayerPrefix,
    const char*                pMessage,
    void*                      pUserData
);
#endif

struct RendererVulkan : public RendererHLSLCg
{
	RendererVulkan(bool fullScreen=false, int argc=0, char *argv[]=0);
	virtual ~RendererVulkan();
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
		virtual const char* getString(){return "Vulkan";}
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
	VArray <TextureVulkan> textures;
	VArray <ShaderVulkan> shaders;
	VArray <VertexBufferVulkan> vertexBuffers;
	VArray <IndexBufferVulkan> indexBuffers;
	VArray <VertexFormatVulkan> vertexFormats;
	VArray <SamplerStateVulkan> samplerStates;
	VArray <BlendStateVulkan> blendStates;
	VArray <AlphaStateVulkan> alphaStates;
	VArray <DepthStateVulkan> depthStates;
	VArray <RasterizerStateVulkan> rasterizerStates;

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

#endif // _VulkanRENDERER_H_
#endif

