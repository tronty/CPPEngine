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
  Copyright (c) 2017 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/RendererMetal.h>
#ifdef __Framework3Metal_H__

RendererMetal::RendererMetal(bool fullScreen, int argc, char *argv[])
		:RendererHLSLCg(argc, argv)
{
    	width=STX_Service::GetWindowInstance()->Getwidth();
    	height=STX_Service::GetWindowInstance()->Getheight();
	SHADING_LANGUAGE_VERSION_MAJOR=3;
	SHADING_LANGUAGE_VERSION_MINOR=3;
	mOk=false;
	scaleBias2D=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
}

void RendererMetal::resetVrtl(const unsigned int flags){}
void RendererMetal::resetToDefaults(){}
void RendererMetal::applyTextures(){}
void RendererMetal::applySamplerStates(){}
void RendererMetal::applyConstants(){}

void RendererMetal::setShaderConstantRaw(const char *name, const void *data, const int size){}
TextureID RendererMetal::addTexture(Image3 &img, const SamplerStateID samplerState, 
					unsigned int flags){return -1;}
TextureID RendererMetal::addTexture(Image3* img, const bool useMipMaps, 
					const SamplerStateID samplerState, 
					unsigned int flags){return -1;}
ShaderID RendererMetal::addHLSLShaderVrtl(
const char *vsText_, const char *gsText, const char *fsText_, const char *csText, const char *hsText, const char *dsText,
const char *vsMain, const char *gsMain, const char *fsMain, const char *csMain, const char *hsMain, const char *dsMain, 
                                            	const unsigned int flags){return -1;}

BlendStateID RendererMetal::addBlendState(const int srcFactorRGB, const int destFactorRGB, 
				const int srcFactorAlpha, 
				const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, 
				const int mask, const bool alphaToCoverage){return -1;}
DepthStateID RendererMetal::addDepthState(const bool depthTest, const bool depthWrite, 
				const int depthFunc, const bool stencilTest,
				const uint8 stencilMask, const int stencilFunc, 
				const int stencilFail, const int depthFail, 
				const int stencilPass){return -1;}
AlphaStateID RendererMetal::addAlphaState(const int func, const float ref){return -1;}
IndexBufferID RendererMetal::addIndexBufferVrtl(const unsigned int nIndices, const unsigned int indexSize, 
				 const BufferAccess bufferAccess,
				 const void *data, IndexBufferID old){return -1;}
				//, const IDataProcessor* p){return 0;}
RasterizerStateID RendererMetal::addRasterizerStateVrtl(const int cullMode, const int fillMode, 
						const bool multiSample, 
						const bool scissor){return -1;}
TextureID RendererMetal::addRenderTargetVrtl(const int width, const int height, const int depth, 
				const int mipMapCount, 
				const int arraySize, const FORMAT format, const int msaaSamples, 
				const SamplerStateID samplerState, unsigned int flags){return -1;}
TextureID RendererMetal::addRenderDepthVrtl(const int width, const int height, const int arraySize, 
				const FORMAT format, 
				const int msaaSamples, const SamplerStateID samplerState, unsigned int flags){return -1;}
SamplerStateID RendererMetal::addSamplerStateVrtl(const Filter filter, const AddressMode s, 
					const AddressMode t, 
					const AddressMode r, const float lod, const unsigned int maxAniso, 
					const int compareFunc, const float *border_color){return -1;}
VertexBufferID RendererMetal::addVertexBufferVrtl(const int size, const BufferAccess bufferAccess, 
							const void *data,
					 		VertexBufferID old){return -1;}
							//, const IDataProcessor* p){return 0;}
VertexFormatID RendererMetal::addVertexFormatVrtl(const FormatDesc *formatDesc, const unsigned int nAttribs, 
							const ShaderID shader){return -1;}
void RendererMetal::changeToMainFramebuffer(){}
void RendererMetal::changeShader(const ShaderID shaderID){}
void RendererMetal::changeVertexFormat(const VertexFormatID vertexFormatID){}
void RendererMetal::changeIndexBuffer(const IndexBufferID indexBufferID){}
void RendererMetal::changeCullFace(const int cullFace){}
void RendererMetal::changeRasterizerState(const RasterizerStateID rasterizerState){}
void RendererMetal::changeBlendStateVrtl(const BlendStateID blendState, const unsigned int sampleMask){}
void RendererMetal::changeDepthStateVrtl(const DepthStateID depthState, const unsigned int stencilRef){}
void RendererMetal::changeRenderTargetsVrtl(const TextureID *colorRTs, const unsigned int nRenderTargets, 
						const TextureID depthRT,
				 		const int depthSlice, const int *slices){}
void RendererMetal::changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, 
						const intptr offset){}
void RendererMetal::changeAlphaState(const AlphaStateID alphaState){}
void RendererMetal::clear(const bool clearColor, const bool clearDepth, const bool clearStencil, 
				const float *color, 
				const float depth, const unsigned int stencil){}
void RendererMetal::clearRenderTarget(const TextureID renderTarget, const D3DXFROMWINEVECTOR4 &color, 
						const int slice){}
void RendererMetal::clearDepthTarget(const TextureID depthTarget, const float depth, 
						const int slice){}
void RendererMetal::setTexture(const char *textureName, const TextureID texture, 
					const SamplerStateID samplerState,
					const int vtstream,
					const VertexBufferID vtbuffer
				)
{
}
void RendererMetal::Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors){}
void RendererMetal::viewport(int x, int y, int viewportWidth, int viewportHeight){}
unsigned int RendererMetal::drawPrimitives(	const Primitives primitives,
				const unsigned int firstv, const unsigned int countv,
				const unsigned int firsti, const unsigned int counti,
				const ubyte* vbdata, const unsigned int vbsize,
				const ubyte* ibdata, const unsigned int ibsize)
{
	LOG_FNLN;
	LOG_PRINT("IMPLEMENT\n");
	return 0;
}
#endif

