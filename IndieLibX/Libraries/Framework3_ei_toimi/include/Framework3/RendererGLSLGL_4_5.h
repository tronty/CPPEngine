#if defined(GLSL4_5)
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

#ifndef __Framework3GL4_5_H__
#define __Framework3GL4_5_H__

#include <Framework3/RendererGL_1_1.h>

struct RendererGL4_5 : public RendererGL_1_1
{
	RendererGL4_5(bool fullScreen=false, int argc=0, char *argv[]=0);
	virtual ~RendererGL4_5();
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
		virtual const char* getString(){return "GLSL4_5";}
		virtual bool IsOK(){return mOk;}

	void changeFrontFace(const GLenum frontFace);
	void setupSampler(GLenum glTarget, const SamplerStateGL &ss);
	void setRenderTargetSize(const TextureID renderTarget, const int width, const int height);

	virtual void resetToDefaults();

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
	void applyTextures();
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

			virtual void setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState=-1
		, const int vtstream=-1
		, const VertexBufferID vtbuffer=-1);
			virtual void EndSceneVrtl(bool aB){OnEndScene();}
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
	D3DXFROMWINEVECTOR4& GetscaleBias2D(){return scaleBias2D;}
	        virtual void resetVrtl(const unsigned int flags);
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
			virtual TextureID addTexture(Image3* img, const bool useMipMaps=false, const SamplerStateID samplerState = SS_NONE, unsigned int flags = 0=float3(0.0f,0.0f,0.0f));
			virtual void viewport(int x, int y, int viewportWidth, int viewportHeight);
protected:
	VArray <TextureGL> textures;
	VArray <ShaderGLSLGL3> shaders;
	VArray <VertexBufferGL> vertexBuffers;
	VArray <IndexBufferGL> indexBuffers;
	VArray <VertexFormatGL> vertexFormats;
	VArray <SamplerStateGL> samplerStates;
	VArray <BlendStateGL> blendStates;
	VArray <AlphaStateGL> alphaStates;
	VArray <DepthStateGL> depthStates;
	VArray <RasterizerStateGL> rasterizerStates;
	void *uniformFuncs[CONSTANT_TYPE_COUNT];
};

#endif // _OpenGL4_5RENDERER_H_
#endif

