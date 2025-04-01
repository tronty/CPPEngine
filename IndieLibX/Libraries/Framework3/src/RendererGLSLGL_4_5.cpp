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
//ifndef ANDROID
#include <Framework3/RendererGLSLGL_4_5.h>
#ifdef __Framework3GL4_5_H__

#include <Framework3/Hlsl2Glsl_Translate.h>
#include <Framework3/Glsl_Translate.h>
#include <Framework3/GLShaders.h>
#if 0
#include "VulkanTools.h"
#endif

RendererGL4_5::RendererGL4_5(bool fullScreen, int argc, char *argv[]):RendererGL_1_1()
{
    	width=STX_Service::GetWindowInstance()->Getwidth();
    	height=STX_Service::GetWindowInstance()->Getheight();
	SHADING_LANGUAGE_VERSION_MAJOR=3;
	SHADING_LANGUAGE_VERSION_MINOR=3;
	mOk=false;
	scaleBias2D=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
			uniformFuncs[CONSTANT_FLOAT] = (void *) glUniform1fv;
			uniformFuncs[CONSTANT_VEC2]  = (void *) glUniform2fv;
			uniformFuncs[CONSTANT_VEC3]  = (void *) glUniform3fv;
			uniformFuncs[CONSTANT_VEC4]  = (void *) glUniform4fv;
			uniformFuncs[CONSTANT_INT]   = (void *) glUniform1iv;
			uniformFuncs[CONSTANT_IVEC2] = (void *) glUniform2iv;
			uniformFuncs[CONSTANT_IVEC3] = (void *) glUniform3iv;
			uniformFuncs[CONSTANT_IVEC4] = (void *) glUniform4iv;
			uniformFuncs[CONSTANT_BOOL]  = (void *) glUniform1iv;
			uniformFuncs[CONSTANT_BVEC2] = (void *) glUniform2iv;
			uniformFuncs[CONSTANT_BVEC3] = (void *) glUniform3iv;
			uniformFuncs[CONSTANT_BVEC4] = (void *) glUniform4iv;
			uniformFuncs[CONSTANT_MAT2]  = (void *) glUniformMatrix2fv;
			uniformFuncs[CONSTANT_MAT3]  = (void *) glUniformMatrix3fv;
			uniformFuncs[CONSTANT_MAT4]  = (void *) glUniformMatrix4fv;
			bsupportsHalf=false;
			bsupportsR2VB=false;
			overrideR2VB=true;
			overrideHalf=true;
			mOk=true;
}
RendererGL4_5::~RendererGL4_5(){}

#define GETFMT(flags) (FORMAT) (flags >> 26)
#define GL_RG_ATI                           0x883C
#define GL_TEXTURE_RECTANGLE_EXT            0x84F5

extern bool GL_ATI_texture_compression_3dc_Flag;
extern bool GL_EXT_texture_compression_s3tc_Flag;
extern bool GL_ARB_texture_compression_Flag;

		static const GLint internalFormats[] =
		{
			0,
			GL_INTENSITY8,
			GL_LUMINANCE8_ALPHA8,
			GL_RGB8,
			GL_RGBA8,
			GL_INTENSITY16,
			GL_LUMINANCE16_ALPHA16,
			GL_RGB16,
			GL_RGBA16,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			GL_INTENSITY_FLOAT16_ATI,
			GL_LUMINANCE_ALPHA_FLOAT16_ATI,
			GL_RGB_FLOAT16_ATI,
			GL_RGBA_FLOAT16_ATI,
			GL_INTENSITY_FLOAT32_ATI,
			GL_LUMINANCE_ALPHA_FLOAT32_ATI,
			GL_RGB_FLOAT32_ATI,
			GL_RGBA_FLOAT32_ATI,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, GL_RGB5,
			GL_RGBA4,
			GL_RGB10_A2,
			GL_DEPTH_COMPONENT16,
			GL_DEPTH_COMPONENT24,
			GL_DEPTH24_STENCIL8_EXT,
			0, GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
			GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
			GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
			0,
#ifdef _MSC_VER
GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI,
#else
0,
#endif
		}
		;
		static const GLenum srcFormats[] =
		{
			0, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA
		}
		;
		static const GLenum srcTypes[] =
		{
			0,
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_BYTE,
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_SHORT,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			GL_FLOAT,
			GL_FLOAT,
			GL_FLOAT,
			GL_FLOAT,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0, 0, 0, GL_UNSIGNED_SHORT_5_6_5,
			GL_UNSIGNED_SHORT_4_4_4_4_REV,
			GL_UNSIGNED_INT_2_10_10_10_REV,
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_INT,
			GL_UNSIGNED_INT_24_8_EXT,
			0, 0,
			0,
			0,
			0,
			0,
		}
		;

		static const char* internalFormats_[] =
		{
			0,
			"GL_INTENSITY8",
			"GL_LUMINANCE8_ALPHA8",
			"GL_RGB8",
			"GL_RGBA8",
			"GL_INTENSITY16",
			"GL_LUMINANCE16_ALPHA16",
			"GL_RGB16",
			"GL_RGBA16",
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			"GL_INTENSITY_FLOAT16_ATI",
			"GL_LUMINANCE_ALPHA_FLOAT16_ATI",
			"GL_RGB_FLOAT16_ATI",
			"GL_RGBA_FLOAT16_ATI",
			"GL_INTENSITY_FLOAT32_ATI",
			"GL_LUMINANCE_ALPHA_FLOAT32_ATI",
			"GL_RGB_FLOAT32_ATI",
			"GL_RGBA_FLOAT32_ATI",
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "GL_RGB5",
			"GL_RGBA4",
			"GL_RGB10_A2",
			"GL_DEPTH_COMPONENT16",
			"GL_DEPTH_COMPONENT24",
			"GL_DEPTH24_STENCIL8_EXT",
			0, "GL_COMPRESSED_RGB_S3TC_DXT1_EXT",
			"GL_COMPRESSED_RGBA_S3TC_DXT3_EXT",
			"GL_COMPRESSED_RGBA_S3TC_DXT5_EXT",
			0,
#ifdef _MSC_VER
"GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI",
#else
0,
#endif
		}
		;
		static const char* srcFormats_[] =
		{
			0, "GL_LUMINANCE", "GL_LUMINANCE_ALPHA", "GL_RGB", "GL_RGBA"
		}
		;
		static const char* srcTypes_[] =
		{
			0,
			"GL_UNSIGNED_BYTE",
			"GL_UNSIGNED_BYTE",
			"GL_UNSIGNED_BYTE",
			"GL_UNSIGNED_BYTE",
			"GL_UNSIGNED_SHORT",
			"GL_UNSIGNED_SHORT",
			"GL_UNSIGNED_SHORT",
			"GL_UNSIGNED_SHORT",
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			"GL_FLOAT",
			"GL_FLOAT",
			"GL_FLOAT",
			"GL_FLOAT",
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0, 0, 0, "GL_UNSIGNED_SHORT_5_6_5",
			"GL_UNSIGNED_SHORT_4_4_4_4_REV",
			"GL_UNSIGNED_INT_2_10_10_10_REV",
			"GL_UNSIGNED_SHORT",
			"GL_UNSIGNED_INT",
			"GL_UNSIGNED_INT_24_8_EXT",
			0, 0,
			0,
			0,
			0,
			0,
		}
		;

void RendererGL4_5::changeFrontFace(const GLenum frontFace)
{
	if (frontFace != currentFrontFace)
	{
		glFrontFace(currentFrontFace = frontFace);
				checkGlError("");
	}
}

void RendererGL4_5::setupSampler(GLenum glTarget, const SamplerStateGL &ss)
{
	glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, ss.wrapS);
				checkGlError("");
	if (glTarget != GL_TEXTURE_1D) {glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, ss.wrapT);
				checkGlError("");}
	if (glTarget == GL_TEXTURE_3D) {glTexParameteri(glTarget, GL_TEXTURE_WRAP_R, ss.wrapR);
				checkGlError("");}
	glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, ss.magFilter);
				checkGlError("");






	glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, ss.minFilter);
	if (ss.aniso > 1 && GL_EXT_texture_filter_anisotropic_supported)
	{
		glTexParameteri(glTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, ss.aniso);
				checkGlError("");
	}
}

		void RendererGL4_5::setRenderTargetSize(const TextureID renderTarget, const int width, const int height)
		{
			(textures[renderTarget]).width	= width;
			(textures[renderTarget]).height = height;
			if ((textures[renderTarget]).glTarget == GL_RENDERBUFFER_EXT)
			{
				FORMAT format = (textures[renderTarget]).format;
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, (textures[renderTarget]).glDepthID);
				checkGlError("");
				glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, internalFormats[format], width, height);
				checkGlError("");
				glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
				checkGlError("");
			}
			else
			{
				FORMAT format = (textures[renderTarget]).format;
				GLint internalFormat = internalFormats[format];
				GLenum srcFormat = srcFormats[getChannelCount(format)];
				GLenum srcType = srcTypes[format];
				if (isDepthFormat(format))
				{
					if (isStencilFormat(format))
					{

						srcFormat = GL_DEPTH_STENCIL_EXT;
					}
					else
					{
						srcFormat = GL_DEPTH_COMPONENT;


					}
				}

				if (isFloatFormat(format)) srcType = GL_FLOAT;
				glBindTexture((textures[renderTarget]).glTarget, (textures[renderTarget]).glTexID);
				checkGlError("");
				if ((textures[renderTarget]).flags & CUBEMAP)
				{
					for (int i = GL_TEXTURE_CUBE_MAP_POSITIVE_X; i <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; i++)
					glTexImage2D(i, 0, internalFormat, width, height, 0, srcFormat, srcType, 0);
				checkGlError("");
				}
				else
				{
					glTexImage2D((textures[renderTarget]).glTarget, 0, internalFormat, width, height, 0, srcFormat, srcType, 0);
				checkGlError("");
				}
				glBindTexture((textures[renderTarget]).glTarget, 0);
				checkGlError("");
			}
		}
void RendererGL4_5::resetVrtl(const unsigned int flags)
{
			reset0(flags);
				if (flags & RESET_TEX){
		for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++){
			selectedTextures[i] = TEXTURE_NONE;
		}
	}

}
void RendererGL4_5::resetToDefaults()
{
			reset0ToDefaults();
			for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++)
			{
				currentTextures[i] = TEXTURE_NONE;
			}
			for (unsigned int i = 0; i < MAX_SAMPLERSTATE; i++)
			{
				currentSamplerStates[i] = SS_NONE;
			}
			currentSrcFactorRGB = currentSrcFactorAlpha = ONE;
			currentDstFactorRGB = currentDstFactorAlpha = ZERO;
			currentBlendModeRGB = currentBlendModeAlpha = BM_ADD;
			currentMask = ALL;
			currentBlendEnable = false;
			currentAlphaToCoverageEnable = false;
			currentDepthFunc = LEQUAL;
	currentDepthTestEnable = true;
	currentDepthWriteEnable = true;
			currentCullMode = CULL_NONE;
			currentFillMode = SOLID;
			currentMultiSampleEnable = true;
			currentScissorEnable = false;
}
void RendererGL4_5::applyTextures(){
	for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++){
		TextureID texture = selectedTextures[i];
		TextureID currTex = currentTextures[i];

		if (texture != currTex){
			if (texture == TEXTURE_NONE){
				glDisable(textures[currTex].glTarget);
				glBindTexture(textures[currTex].glTarget, 0);
			} else {
				if (currTex == TEXTURE_NONE){
					glEnable(textures[texture].glTarget);
				} else if (textures[texture].glTarget != textures[currTex].glTarget){
					glDisable(textures[currTex].glTarget);
					glEnable(textures[texture].glTarget);
				}
				glBindTexture(textures[texture].glTarget, textures[texture].glTexID);
				}
			}

			currentTextures[i] = texture;
		}
	}
}
void RendererGL4_5::applySamplerStates()
{
}
typedef GLvoid (APIENTRY *UNIFORM_FUNC)(GLint location, GLsizei count, const void *value);
typedef GLvoid (APIENTRY *UNIFORM_MAT_FUNC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void RendererGL4_5::applyConstants()
{
	if (currentShader != SHADER_NONE)
	{
		for (unsigned int i = 0; i < (shaders[currentShader]).uniforms.size(); i++)
		{
			ConstantShaderGLSLGL3* uni = (&(shaders[currentShader]).uniforms[0]) + i;
			//if ((uni)->dirty) // ???
			{
				if ((uni)->type >= CONSTANT_MAT2)
				{
					((UNIFORM_MAT_FUNC) uniformFuncs[uni->type])(uni->location, uni->nElements, GL_TRUE, reinterpret_cast<float*>(&uni->data[0]));
				}
				else
				{
					((UNIFORM_FUNC) uniformFuncs[uni->type])(uni->location, uni->nElements, reinterpret_cast<float*>(&uni->data[0]));
				}
				(uni)->dirty = false;
			}
		}
	}
}

void RendererGL4_5::setShaderConstantRaw(const char *name, const void *data, const int size)
{
// GLint glGetUniformLocation (GLuint program, const GLchar *name);
/*
GLAPI void APIENTRY glUniform1fv (GLint location, GLsizei count, const GLfloat *value);
GLAPI void APIENTRY glUniform2fv (GLint location, GLsizei count, const GLfloat *value);
GLAPI void APIENTRY glUniform3fv (GLint location, GLsizei count, const GLfloat *value);
GLAPI void APIENTRY glUniform4fv (GLint location, GLsizei count, const GLfloat *value);
GLAPI void APIENTRY glUniform1iv (GLint location, GLsizei count, const GLint *value);
GLAPI void APIENTRY glUniform2iv (GLint location, GLsizei count, const GLint *value);
GLAPI void APIENTRY glUniform3iv (GLint location, GLsizei count, const GLint *value);
GLAPI void APIENTRY glUniform4iv (GLint location, GLsizei count, const GLint *value);
GLAPI void APIENTRY glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GLAPI void APIENTRY glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GLAPI void APIENTRY glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
*/
	if(selectedShader==-1)return;
	{
		int minUniform = 0;
		int maxUniform = (shaders[selectedShader]).uniforms.size() - 1;

		while (minUniform <= maxUniform)
		{
			int currUniform = (minUniform + maxUniform) >> 1;
			int res = strcmp(name, ((shaders[selectedShader]).uniforms[currUniform]).name.c_str());
			if (res == 0)
			{
				ConstantShaderGLSLGL3* uni = (&(shaders[selectedShader]).uniforms[0]) + currUniform;
				if (stx_memcmp(&uni->data[0], data, size))
				{
					uni->data.resize(size);
					stx_memcpy(&uni->data[0], data, size);
					uni->dirty = true;
				}
				return;
			}
			else if (res > 0)
			{
				minUniform = currUniform + 1;
			}
			else
			{
				maxUniform = currUniform - 1;
			}
		}
	}
}

void RendererGL4_5::setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState, const int vtstream, const VertexBufferID vtbuffer)
{
	if(texture==-1) //DBG_HALT;
	if(selectedShader == SHADER_NONE) return;

	int unit = getSamplerUnit(selectedShader, textureName, shaders[selectedShader].samplers);
	if (unit >= 0)
	{
		selectedTextures[unit] = texture;
		selectedSamplerStates[unit] = samplerState;
	}
}
BlendStateID RendererGL4_5::addBlendState(const int srcFactorRGB, const int destFactorRGB, 
				const int srcFactorAlpha, 
				const int destFactorAlpha, const int blendModeRGB, const int blendModeAlpha, 
				const int mask, const bool alphaToCoverage)
{
			BlendStateGL blendState;
			blendState.srcFactorRGB   = srcFactorRGB;
			blendState.dstFactorRGB   = destFactorRGB;
			blendState.srcFactorAlpha = srcFactorAlpha;
			blendState.dstFactorAlpha = destFactorAlpha;
			blendState.blendModeRGB   = blendModeRGB;
			blendState.blendModeAlpha = blendModeAlpha;
			blendState.mask = mask;
			blendState.blendEnable = (srcFactorRGB != ONE || destFactorRGB != ZERO || srcFactorAlpha != ONE || destFactorAlpha != ZERO);
			blendState.alphaEnable = (srcFactorRGB != ONE || destFactorRGB != ZERO || srcFactorAlpha != ONE || destFactorAlpha != ZERO);
			blendState.alphaToCoverageEnable = alphaToCoverage;
			blendStates.push_back(blendState);
			return blendStates.size()-1;
}
DepthStateID RendererGL4_5::addDepthState(const bool depthTest, const bool depthWrite, 
				const int depthFunc, const bool stencilTest,
				const uint8 stencilMask, const int stencilFunc, 
				const int stencilFail, const int depthFail, 
				const int stencilPass)
{
	DepthStateGL depthState;
	depthState.depthTest  = depthTest;
	depthState.depthWrite = depthWrite;
	depthState.depthFunc  = depthFunc;
	depthStates.push_back(depthState);
	return depthStates.size()-1;
}
AlphaStateID RendererGL4_5::addAlphaState(const int func, const float ref)
{
	AlphaStateGL blendState;
    blendState.blendEnable = true;
	blendState.alphaFunc=GEQUAL;
	alphaStates.push_back(blendState);
	return alphaStates.size()-1;
}

GLenum usagesGL_4_5[] =
		{
			GL_STATIC_DRAW,
			GL_DYNAMIC_DRAW,
			GL_STREAM_DRAW,
		}
		;
IndexBufferID RendererGL4_5::addIndexBufferVrtl(const unsigned int nIndices, const unsigned int indexSize, 
				 const BufferAccess bufferAccess,
				 const void *data, IndexBufferID aold)
{
			unsigned int size = nIndices * indexSize;

if(aold>-1)
{

	glDeleteBuffers(1,  &indexBuffers[aold].vboIB);
				checkGlError("");
//

	indexBuffers[aold].vboIB=0;
	glGenBuffers(1, &indexBuffers[aold].vboIB);
				checkGlError("");


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[aold].vboIB);
				checkGlError("");


	indexBuffers[aold].nIndices = nIndices;
	indexBuffers[aold].indexSize = indexSize;
}

	IndexBufferGL ib;
if(aold==-1)
{
	GLuint vbb=0;
	ib.nIndices = nIndices;
	ib.indexSize = indexSize;

	glGenBuffers(1, &vbb);
				checkGlError("");

	ib.vboIB=vbb;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbb);
				checkGlError("");


	indexBuffers.push_back(ib);
}

//if(old>-1)
	//indexBuffers[old].size=size;

	if(data) {

ib.data=(BYTE*)data;
glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usagesGL_4_5[bufferAccess]);
				checkGlError("");

}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				checkGlError("");


			if(aold==-1)
			{

				//LOG_PRINT("vbid=%d\n",vertexBuffers.size()-1);
				return indexBuffers.size()-1;
			}
			else if((aold>=0)&&(aold<indexBuffers.size()))
{
return aold;
}
else
	return -1;
}
RasterizerStateID RendererGL4_5::addRasterizerStateVrtl(const int cullMode, const int fillMode, 
						const bool multiSample, 
						const bool scissor)
{

			RasterizerStateGL rasterizerState;
			rasterizerState.cullMode = cullMode;
			rasterizerState.fillMode = fillMode;
			rasterizerState.multiSample = multiSample;
			rasterizerState.scissor = scissor;
			rasterizerStates.push_back(rasterizerState);
			return rasterizerStates.size()-1;
}
TextureID RendererGL4_5::addRenderTargetVrtl(const int width, const int height, const int depth, 
				const int mipMapCount, 
				const int arraySize, const FORMAT format, const int msaaSamples, 
				const SamplerStateID samplerState, unsigned int flags)
{
			if (isCompressedFormat(format) || arraySize > 1) return false;
			if (mipMapCount > 1) return false;
			TextureGL tex;
			stx_memset(&tex, 0, sizeof(TextureGL));
			tex.glTarget = (flags & CUBEMAP)? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
			tex.flags  = flags;
			tex.format = format;
			glGenTextures(1, &tex.glTexID);
				checkGlError("");
			glBindTexture(tex.glTarget, tex.glTexID);
				checkGlError("");

			setupSampler(tex.glTarget, samplerStates[samplerState]);
			tex.samplerState = samplerState;
			TextureID rt = textures.add(tex);
			setRenderTargetSize(rt, width, height);
			glBindTexture(tex.glTarget, 0);
				checkGlError("");
			return rt;
}
TextureID RendererGL4_5::addRenderDepthVrtl(const int width, const int height, const int arraySize, 
				const FORMAT format, 
				const int msaaSamples, const SamplerStateID samplerState, unsigned int flags)
{
			if (arraySize > 1) return false;
			TextureGL tex;
			stx_memset(&tex, 0, sizeof(TextureGL));
			tex.glTarget = GL_RENDERBUFFER;
			tex.format = format;
			glGenRenderbuffers(1, &tex.glDepthID);
				checkGlError("");
			TextureID rt = textures.add(tex);
			setRenderTargetSize(rt, width, height);
			return rt;
}

		GLint minFilters_4_5[] =
		{
			GL_NEAREST,
			GL_LINEAR,
			GL_LINEAR_MIPMAP_NEAREST,
			GL_LINEAR_MIPMAP_LINEAR,
			GL_LINEAR_MIPMAP_NEAREST,
			GL_LINEAR_MIPMAP_LINEAR,
		}
		;
SamplerStateID RendererGL4_5::addSamplerStateVrtl(const Filter filter, const AddressMode s, 
					const AddressMode t, 
					const AddressMode r, const float lod, const unsigned int maxAniso, 
					const int compareFunc, const float *border_color)
{
			if (compareFunc != 0 || border_color != 0) return SS_NONE;
			SamplerStateGL samplerState;
			samplerState.minFilter = minFilters_4_5[filter];
			samplerState.magFilter = (filter == NEAREST)? GL_NEAREST : GL_LINEAR;
			samplerState.wrapS = (s == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
			samplerState.wrapT = (t == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
			samplerState.wrapR = (r == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
			samplerState.aniso = hasAniso(filter)? min((unsigned int) maxAnisotropic, maxAniso) : 1;
			samplerState.lod = lod;
			samplerStates.push_back(samplerState);
			return samplerStates.size()-1;
}
VertexBufferID RendererGL4_5::addVertexBufferVrtl(const int size, const BufferAccess bufferAccess, 
							const void *data,
					 		VertexBufferID aold)
{

if(aold>-1)
{


	glDeleteBuffers(1,  &vertexBuffers[aold].vboVB);
				checkGlError("");

//

	vertexBuffers[aold].vboVB=0;
	glGenBuffers(1, &vertexBuffers[aold].vboVB);
				checkGlError("");


	vertexBuffers[aold].size=size;


	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[aold].vboVB);
				checkGlError("");


}


VertexBufferGL vb;

if(aold==-1)
{

	vb.size = size;
	GLuint vbb=0;


	glGenBuffers
//
(1, &vbb);
				checkGlError("");


	vb.vboVB=vbb;
	vertexBuffers.push_back(vb);



	glBindBuffer(GL_ARRAY_BUFFER, vbb);
				checkGlError("");


}


	if(data) {


vb.data=(BYTE*)data;

glBufferData(GL_ARRAY_BUFFER, size, data, usagesGL_4_5[bufferAccess]);
				checkGlError("");


}


	glBindBuffer(GL_ARRAY_BUFFER, 0);
				checkGlError("");



/*
LOG_PRINT("vbb=%d\n",vbb);
LOG_PRINT("size=%d\n",size);
LOG_PRINT("data=%d\n",data);
*/


			if(aold==-1)
			{


				//LOG_PRINT("vbid=%d\n",vertexBuffers.size()-1);
				return vertexBuffers.size()-1;
			}
			else if((aold>=0)&&(aold<vertexBuffers.size()))
{

//LOG_PRINT("vbid=%d\n",old);
return aold;
}
#if 0
			else if(0)
			{
				VertexBufferID tmpVertexBuffers[MAX_VERTEXSTREAM];
				intptr tmpOffsets[MAX_VERTEXSTREAM];
				for(int i=0;i<MAX_VERTEXSTREAM;i++)
				{
					tmpVertexBuffers[i]=currentVertexBuffers[i];
					tmpOffsets[i]=currentOffsets[i];
				}
				//int vf=currentVertexFormat;
				int vf=selectedVertexFormat;
				setVertexFormat(-1);
				for(int i=0;i<MAX_VERTEXSTREAM;i++)
				{
					if(-1!=currentVertexBuffers[i])
					{
						glBindBuffer(GL_ARRAY_BUFFER, currentVBO = 0);
				checkGlError("");

						currentVertexBuffers[i] = -1;
					}
				}
				deleteVertexBuffer(old);//???
				vertexBuffers[old]=vb;
				setVertexFormat(vf);
				for(int i=0;i<MAX_VERTEXSTREAM;i++)
				{
					setVertexBuffer(i,tmpVertexBuffers[i],tmpOffsets[i]);
				}
				return old;
			}
#endif
			else
{

//LOG_PRINT("vbid=%d\n",-1);

			//DBG_HALT;
			return -1;
}
}
VertexFormatID RendererGL4_5::addVertexFormatVrtl(const FormatDesc *formatDesc, const unsigned int nAttribs, 
							const ShaderID shader)
{

#if defined(USE_HLSL_SHADERS)
			VertexFormatGL vertexFormat;
			stx_memset(&vertexFormat, 0, sizeof(vertexFormat));
			unsigned int nGeneric  = 0;
			unsigned int nTexCoord = 0;

			for (unsigned int i = 0; i < nAttribs; i++)
			{
				if (formatDesc[i].type == TYPE_VERTEX)
				{
					nGeneric = 1;
					break;
				}
			}

			for (unsigned int i = 0; i < nAttribs; i++)
			{
				int stream = formatDesc[i].stream;
				switch (formatDesc[i].type)
				{
					case TYPE_GENERIC:
					case TYPE_TANGENT:
					case TYPE_BINORMAL:
					vertexFormat.generic[nGeneric].stream = stream;
					vertexFormat.generic[nGeneric].size   = formatDesc[i].size;
					vertexFormat.generic[nGeneric].offset = vertexFormat.vertexSize[stream];
					vertexFormat.generic[nGeneric].format = formatDesc[i].format;
					nGeneric++;
					break;
					case TYPE_VERTEX:
					vertexFormat.vertex.stream = stream;
					vertexFormat.vertex.size   = formatDesc[i].size;
					vertexFormat.vertex.offset = vertexFormat.vertexSize[stream];
					vertexFormat.vertex.format = formatDesc[i].format;
					break;
					case TYPE_NORMAL:
					vertexFormat.normal.stream = stream;
					vertexFormat.normal.size   = formatDesc[i].size;
					vertexFormat.normal.offset = vertexFormat.vertexSize[stream];
					vertexFormat.normal.format = formatDesc[i].format;
					break;
					case TYPE_TEXCOORD:
					vertexFormat.texCoord[nTexCoord].stream = stream;
					vertexFormat.texCoord[nTexCoord].size	 = formatDesc[i].size;
					vertexFormat.texCoord[nTexCoord].offset = vertexFormat.vertexSize[stream];
					vertexFormat.texCoord[nTexCoord].format = formatDesc[i].format;
					nTexCoord++;
					break;
				}
				vertexFormat.vertexSize[stream] += formatDesc[i].size * getFormatSize(formatDesc[i].format);
			}

			vertexFormat.maxGeneric = nGeneric;
			vertexFormat.maxTexCoord = nTexCoord;
			vertexFormats.push_back(vertexFormat);

			return vertexFormats.size()-1;
#else

			VertexFormatGL vertexFormat;

			stx_memset(&vertexFormat, 0, sizeof(vertexFormat));

			unsigned int nGeneric  = 0;
			unsigned int nTexCoord = 0;

			/*
			for (unsigned int i = 0; i < nAttribs; i++)
			{
				int stream = formatDesc[i].stream;
				if (formatDesc[i].type == TYPE_VERTEX)
				{
					vertexFormat.generic[nGeneric].stream = stream;
					vertexFormat.generic[nGeneric].size   = formatDesc[i].size;
					vertexFormat.generic[nGeneric].offset = vertexFormat.vertexSize[stream];
					vertexFormat.generic[nGeneric].format = formatDesc[i].format;
					nGeneric = 1;
					break;
				}
			}*/

			for (unsigned int i = 0; i < nAttribs; i++)
			{
				int stream = formatDesc[i].stream;
				switch (formatDesc[i].type)
				{
					case TYPE_GENERIC:
					case TYPE_TANGENT:
					case TYPE_BINORMAL:

					case TYPE_VERTEX:
					case TYPE_NORMAL:
					case TYPE_TEXCOORD:
					if(formatDesc[i].type==TYPE_TEXCOORD) nTexCoord++;


					vertexFormat.generic[nGeneric].stream = stream;
					vertexFormat.generic[nGeneric].size   = formatDesc[i].size;
					vertexFormat.generic[nGeneric].offset = vertexFormat.vertexSize[stream];
					vertexFormat.generic[nGeneric].format = formatDesc[i].format;
					nGeneric++;
					break;/*
					case TYPE_VERTEX:
					vertexFormat.vertex.stream = stream;
					vertexFormat.vertex.size   = formatDesc[i].size;
					vertexFormat.vertex.offset = vertexFormat.vertexSize[stream];
					vertexFormat.vertex.format = formatDesc[i].format;
					break;
					case TYPE_NORMAL:
					vertexFormat.normal.stream = stream;
					vertexFormat.normal.size   = formatDesc[i].size;
					vertexFormat.normal.offset = vertexFormat.vertexSize[stream];
					vertexFormat.normal.format = formatDesc[i].format;
					break;
					case TYPE_TEXCOORD:
					vertexFormat.texCoord[nTexCoord].stream = stream;
					vertexFormat.texCoord[nTexCoord].size	 = formatDesc[i].size;
					vertexFormat.texCoord[nTexCoord].offset = vertexFormat.vertexSize[stream];
					vertexFormat.texCoord[nTexCoord].format = formatDesc[i].format;
					nTexCoord++;
					break;*/
				}

				vertexFormat.vertexSize[stream] += formatDesc[i].size * getFormatSize(formatDesc[i].format);
			}

			vertexFormat.shader=shader;

			vertexFormat.maxGeneric = nGeneric;

#ifdef USE_HLSL_SHADERS
			vertexFormat.maxTexCoord = nTexCoord;
#endif

			vertexFormats.push_back(vertexFormat);

			return vertexFormats.size()-1;

#endif
}
void RendererGL4_5::changeToMainFramebuffer()
{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
				checkGlError("");
			glViewport(m_viewport.X,m_viewport.Y, m_viewport.Width, m_viewport.Height);
				checkGlError("");
			changeFrontFace(GL_CW);
}
void RendererGL4_5::changeShader(const ShaderID shader)
{
			if (shader != currentShader)
			{
				if (shader == SHADER_NONE)
				{
					glUseProgram(0);
				checkGlError("");
					//LOG_PRINT("glUseProgramObject(0)\n");
				}
				else
				{
/*
../../../Libraries/Framework3/src/RendererGLSLGL_4_5.cpp:changeShader:965
glUseProgramObject(shaders[0].program=1)
GL error:
file=../../../Libraries/Framework3/src/RendererGLSLGL_4_5.cpp:
fn=changeShader:
line=968:
 0x00000502
*/
LOG_FNLN;
LOG_PRINT("shaders.size()=%x\n", shaders.size());
LOG_PRINT("glUseProgramObject(shaders[%x].program=%x)\n", shader, shaders[shader].program);
					glUseProgram((shaders[shader]).program);
				checkGlError("");
					//LOG_PRINT("shader %d:glUseProgramObject(%d)\n",shader,(shaders[shader]).program);
				}
				currentShader = shader;
			}
}
void RendererGL4_5::changeVertexFormat(const VertexFormatID vertexFormat)
{
	static VertexFormatGL zero;
	VertexFormatGL curr = zero, sel = zero;
	if (currentVertexFormat != VF_NONE) curr = vertexFormats[currentVertexFormat];
	if (vertexFormat != VF_NONE) sel = vertexFormats[vertexFormat];
	if ( sel.vertex.size && !curr.vertex.size)
	{
		glEnableClientState (GL_VERTEX_ARRAY);
		checkGlError("");
	}
	if (!sel.vertex.size &&  curr.vertex.size)
	{
		glDisableClientState(GL_VERTEX_ARRAY);
		checkGlError("");
	}
	if ( sel.normal.size && !curr.normal.size) {glEnableClientState(GL_NORMAL_ARRAY);
							checkGlError("");}
	if (!sel.normal.size &&  curr.normal.size) {glDisableClientState(GL_NORMAL_ARRAY);
							checkGlError("");}
	for (int i = 0; i < MAX_GENERIC; i++)
	{
		if ( sel.generic[i].size && !curr.generic[i].size) {glEnableVertexAttribArray(i);
									checkGlError("");}
		if (!sel.generic[i].size &&  curr.generic[i].size) {glDisableVertexAttribArray(i);
									checkGlError("");}
	}
	for (int i = 0; i < MAX_TEXCOORD; i++)
	{
		if ((sel.texCoord[i].size > 0) ^ (curr.texCoord[i].size > 0))
		{
			glClientActiveTexture(GL_TEXTURE0 + i);
			checkGlError("");
			if (sel.texCoord[i].size > 0)
			{
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				checkGlError("");
			}
			else
			{
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				checkGlError("");
			}
		}
	}
	currentVertexFormat = vertexFormat;
}
void RendererGL4_5::changeIndexBuffer(const IndexBufferID indexBuffer)
{
	////if (indexBuffer != currentIndexBuffer)
			{
				if (indexBuffer == IB_NONE)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				checkGlError("");
				}
				else
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (indexBuffers[indexBuffer]).vboIB);
				checkGlError("");
				}
				currentIndexBuffer = indexBuffer;
			}
}
void RendererGL4_5::changeRasterizerState(const RasterizerStateID rasterizerState)
{

			if (rasterizerState != currentRasterizerState)
			{
				RasterizerStateGL state;
				if (rasterizerState == RS_NONE)
				{
					state.cullMode = CULL_NONE;
					state.fillMode = SOLID;
					state.multiSample = true;
					state.scissor = false;
				}
				else
				{
					state = (rasterizerStates[rasterizerState]);
				}
				if (state.cullMode != currentCullMode)
				{
					if (state.cullMode == CULL_NONE)
					{
						glDisable(GL_CULL_FACE);
				checkGlError("");
					}

					else
					{
						if (currentCullMode == CULL_NONE) {glEnable(GL_CULL_FACE);
				checkGlError("");}
						glCullFace(state.cullMode);
				checkGlError("");
					}
					currentCullMode = state.cullMode;
				}
				if (state.fillMode != currentFillMode)
				{
					glPolygonMode(GL_FRONT_AND_BACK, currentFillMode = state.fillMode);
				checkGlError("");
				}
				if (state.multiSample != currentMultiSampleEnable)
				{
					if (state.multiSample)
					{
						glEnable(GL_MULTISAMPLE);
				checkGlError("");
					}
					else
					{
						glDisable(GL_MULTISAMPLE);
				checkGlError("");
					}
					currentMultiSampleEnable = state.multiSample;
				}
				if (state.scissor != currentScissorEnable)
				{
					if (state.scissor)
					{
						glEnable(GL_SCISSOR_TEST);
				checkGlError("");
					}
					else
					{
						glDisable(GL_SCISSOR_TEST);
				checkGlError("");
					}
					currentScissorEnable = state.scissor;
				}
				currentRasterizerState = rasterizerState;
			}
}
void RendererGL4_5::changeBlendStateVrtl(const BlendStateID blendState, const unsigned int sampleMask)
{

        if (blendState != currentBlendState){
            if (blendState == BS_NONE || !blendStates[blendState].blendEnable){
                if (currentBlendEnable){
                    glDisable(GL_BLEND);
				checkGlError("");
                    currentBlendEnable = false;
                }
            } else {
                if (blendStates[blendState].blendEnable){
                    if (!currentBlendEnable){
                        glEnable(GL_BLEND);
				checkGlError("");
                        currentBlendEnable = true;
                    }
                    if (blendStates[blendState].srcFactorRGB   != currentSrcFactorRGB ||
                        blendStates[blendState].dstFactorRGB   != currentDstFactorRGB ||
                        blendStates[blendState].srcFactorAlpha != currentSrcFactorAlpha ||
                        blendStates[blendState].dstFactorAlpha != currentDstFactorAlpha){

                        glBlendFuncSeparate(
                            currentSrcFactorRGB   = blendStates[blendState].srcFactorRGB,
                            currentDstFactorRGB   = blendStates[blendState].dstFactorRGB,
                            currentSrcFactorAlpha = blendStates[blendState].srcFactorAlpha,
                            currentDstFactorAlpha = blendStates[blendState].dstFactorAlpha);
				checkGlError("");
                    }
                    if (blendStates[blendState].blendModeRGB   != currentBlendModeRGB ||
                        blendStates[blendState].blendModeAlpha != currentBlendModeAlpha){
                        glBlendEquationSeparate(
                            currentBlendModeRGB   = blendStates[blendState].blendModeRGB,
                            currentBlendModeAlpha = blendStates[blendState].blendModeAlpha);
				checkGlError("");
                    }
                }
            }

            int mask = ALL;
            bool alphaToCoverage = false;
            if (blendState != BS_NONE){
                mask = blendStates[blendState].mask;
                alphaToCoverage = blendStates[blendState].alphaToCoverageEnable;
            }

            if (mask != currentMask){
                glColorMask((mask & RED) & 1, ((mask & GREEN) >> 1) & 1, ((mask & BLUE) >> 2) & 1, ((mask & ALPHA) >> 3) & 1);
				checkGlError("");

                currentMask = mask;
            }

            if (alphaToCoverage != currentAlphaToCoverageEnable)
            {
                if (alphaToCoverage)
                    {glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
				checkGlError("");}
                else
                    {glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
				checkGlError("");}

                currentAlphaToCoverageEnable = alphaToCoverage;
            }

            currentBlendState = blendState;
        }
}
void RendererGL4_5::changeDepthStateVrtl(const DepthStateID depthState, const unsigned int stencilRef)
{


	if (depthState != currentDepthState){
		if (depthState == DS_NONE){
			if (!currentDepthTestEnable){
				glEnable(GL_DEPTH_TEST);
				currentDepthTestEnable = true;
			}

			if (!currentDepthWriteEnable){
				glDepthMask(GL_TRUE);
				currentDepthWriteEnable = true;
			}

			if (currentDepthFunc != LEQUAL){
				glDepthFunc(currentDepthFunc = LEQUAL);
			}
		} else {
			if (depthStates[depthState].depthTest){
				if (!currentDepthTestEnable){
					glEnable(GL_DEPTH_TEST);
					currentDepthTestEnable = true;
				}
				if (depthStates[depthState].depthWrite != currentDepthWriteEnable){
					glDepthMask((currentDepthWriteEnable = depthStates[depthState].depthWrite)? GL_TRUE : GL_FALSE);
				}
				if (depthStates[depthState].depthFunc != currentDepthFunc){
					glDepthFunc(currentDepthFunc = depthStates[depthState].depthFunc);
				}
			} else {
				if (currentDepthTestEnable){
					glDisable(GL_DEPTH_TEST);
					currentDepthTestEnable = false;
				}
			}
		}

		currentDepthState = depthState;
	}
}
void RendererGL4_5::changeRenderTargetsVrtl(const TextureID *colorRTs, const unsigned int nRenderTargets, 
						const TextureID depthRT,
				 		const int depthSlice, const int *slices)
{

			apply();
			if (fbo == 0) {glGenFramebuffers(1, &fbo);
				checkGlError("");}
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
				checkGlError("");
			for (unsigned int i = 0; i < nRenderTargets; i++)
			{
				TextureID colorRT = colorRTs[i];
				if ((textures[colorRT]).flags & CUBEMAP)
				{
					if (colorRT != currentColorRT[i] || currentColorRTSlice[i] != slices[i])
					{
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + slices[i], (textures[colorRT]).glTexID, 0);
				checkGlError("");
						currentColorRTSlice[i] = slices[i];
					}
				}
				else
				{
					if (colorRT != currentColorRT[i])
					{
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, (textures[colorRT]).glTexID, 0);
				checkGlError("");
						currentColorRT[i] = colorRT;
					}
				}
			}
			if (nRenderTargets != nCurrentRenderTargets)
			{
				for (unsigned int i = nRenderTargets; i < nCurrentRenderTargets; i++)
				{
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
				checkGlError("");
					currentColorRT[i] = TEXTURE_NONE;
					currentColorRTSlice[i] = NO_SLICE;
				}
				if (nRenderTargets == 0)
				{
					glDrawBuffer(GL_NONE);
				checkGlError("");
					glReadBuffer(GL_NONE);
				checkGlError("");
				}
				else
				{
					glDrawBuffers(nRenderTargets, drawBuffers);
				checkGlError("");
					glReadBuffer(GL_COLOR_ATTACHMENT0);
				checkGlError("");
				}
				nCurrentRenderTargets = nRenderTargets;
			}
			if (depthRT != currentDepthRT)
			{
				if (depthRT >= 0 && (textures[depthRT]).glTarget != GL_RENDERBUFFER)
				{
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, (textures[depthRT]).glTexID, 0);
				checkGlError("");
					if (isStencilFormat((textures[depthRT]).format))
					{
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, (textures[depthRT]).glTexID, 0);
				checkGlError("");
					}
					else
					{
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
				checkGlError("");
					}
				}
				else
				{
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, (depthRT < 0)? 0 : (textures[depthRT]).glTexID);
				checkGlError("");
					if (depthRT >= 0 && isStencilFormat((textures[depthRT]).format))
					{
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, (textures[depthRT]).glTexID);
				checkGlError("");
					}
					else
					{
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
				checkGlError("");
					}
				}
				currentDepthRT = depthRT;
			}
			if (nRenderTargets > 0)
			{
				if ((textures[colorRTs[0]]).flags & CUBEMAP)
				{
					changeFrontFace(GL_CCW);
				}
				else
				{
					changeFrontFace(GL_CW);
				}
				glViewport(0, 0, (textures[colorRTs[0]]).width, (textures[colorRTs[0]]).height);
				checkGlError("");
			}
			else if(depthRT >= 0)
			{
				changeFrontFace(GL_CW);
				glViewport(0, 0, (textures[depthRT]).width, (textures[depthRT]).height);
				checkGlError("");
			}
			reset(0);
}
void RendererGL4_5::changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, 
						const intptr offset)
{

#if !defined(USE_HLSL_SHADERS)

			VertexFormatGL cvf = vertexFormats[selectedVertexFormat];

			int vertexSize = cvf.vertexSize[stream];

			if(!vertexSize) return;

			int nGeneric=cvf.maxGeneric;

			int nAttribs=cvf.maxGeneric;

			int nTexCoord=cvf.maxTexCoord;

			char *base = (char *)&vertexBuffers[vertexBuffer].data;//(char *) offset;

			const GLsizei glTypes[] =
			{
				GL_FLOAT,
				0, GL_UNSIGNED_BYTE,
			};

			for (unsigned int i = 0; i < nAttribs; i++)
			{

				unsigned int loc=shaders[selectedShader].attribs[i].loc;
				glVertexAttribPointer(loc, cvf.generic[i].size, glTypes[cvf.generic[i].format], GL_FALSE, vertexSize, base + cvf.generic[i].offset);
				checkGlError("");
				glEnableVertexAttribArray (loc);
				checkGlError("");
			}


			currentVertexBuffers[stream] = vertexBuffer;

			currentOffsets[stream] = offset;

			activeVertexFormat[stream] = //current
			selectedVertexFormat;

#else
			const GLsizei glTypes[] =
			{
				GL_FLOAT,
				0, GL_UNSIGNED_BYTE,
			}
			;
			GLuint vbo = 0;

			if (vertexBuffer != VB_NONE) vbo = (vertexBuffers[vertexBuffer]).vboVB;

			if (vbo != currentVBO)
			{
				glBindBuffer(GL_ARRAY_BUFFER, currentVBO = vbo);
				checkGlError("");
			}

			//if (vertexBuffer != currentVertexBuffers[stream] || offset != currentOffsets[stream] || currentVertexFormat != activeVertexFormat[stream])
			{
				if (currentVertexFormat != VF_NONE)
				{

					char *base = (char *) offset;

					VertexFormatGL cvf = vertexFormats[currentVertexFormat];

					int vertexSize = cvf.vertexSize[stream];

					if (cvf.vertex.stream == stream && cvf.vertex.size)
					{

						glVertexPointer(cvf.vertex.size, glTypes[cvf.vertex.format], vertexSize, base + cvf.vertex.offset);
				checkGlError("");
					}
					if (cvf.normal.stream == stream && cvf.normal.size)
					{

						glNormalPointer(glTypes[cvf.normal.format], vertexSize, base + cvf.normal.offset);
				checkGlError("");
					}
					for (int i = 0; i < MAX_GENERIC; i++)
					{

						if (cvf.generic[i].stream == stream && cvf.generic[i].size)
						{

							glVertexAttribPointer(i, cvf.generic[i].size, glTypes[cvf.generic[i].format], GL_TRUE, vertexSize, base + cvf.generic[i].offset);
				checkGlError("");
						}
					}
					for (int i = 0; i < MAX_TEXCOORD; i++)
					{

						if (cvf.texCoord[i].stream == stream && cvf.texCoord[i].size)
						{

							glClientActiveTexture(GL_TEXTURE0 + i);
				checkGlError("");

							glTexCoordPointer(cvf.texCoord[i].size, glTypes[cvf.texCoord[i].format], vertexSize, base + cvf.texCoord[i].offset);
				checkGlError("");
						}
					}
				}

				currentVertexBuffers[stream] = vertexBuffer;

				currentOffsets[stream] = offset;

				activeVertexFormat[stream] = currentVertexFormat;

			}
#endif
}
void RendererGL4_5::changeAlphaState(const AlphaStateID blendState)
{

	if (blendState != currentAlphaState)
	{
		if (blendState == -1 || !(alphaStates[blendState]).blendEnable)
		{
			if (currentAlphaEnable)
			{
				glDisable(GL_ALPHA_TEST);
				checkGlError("");
    			currentAlphaEnable = false;
			}
		}
		else
		{
			if ((alphaStates[blendState]).blendEnable)
			{
				if (!currentAlphaEnable)
				{
					glEnable(GL_ALPHA_TEST);
				checkGlError("");
    				currentAlphaEnable = true;
    //  reference = clamp(referenceArg, 0.0f, 1.0f);
    glAlphaFunc(GL_GEQUAL, 0.5f);//???

				}
			}
		}
		currentAlphaState = blendState;

}
}
void RendererGL4_5::clear(const bool clearColor, const bool clearDepth, const bool clearStencil, 
				const float *color, 
				const float depth, const unsigned int stencil)
{
/*
        static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
        static const GLfloat one = 1.0f;
        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glClearBufferfv(GL_COLOR, 0, green);
        glClearBufferfv(GL_DEPTH, 0, &one);
*/
#if (!defined(QT_BUILD))
			if(color)
			{
				glClearColor(color[0],color[1],color[2],color[3]);
				checkGlError("");
			}
			if(depth)
			{
				glClearDepth(1.0f);
				checkGlError("");
			}
			int f=0;
			if(color) f|=GL_COLOR_BUFFER_BIT;
			if(depth) f|=GL_DEPTH_BUFFER_BIT;
			glClear(f);
				checkGlError("");
#endif
}
void RendererGL4_5::Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors)
{
/*
        static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
        static const GLfloat one = 1.0f;
        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glClearBufferfv(GL_COLOR, 0, green);
        glClearBufferfv(GL_DEPTH, 0, &one);
*/
        static const GLfloat color_[] = { colors.x, colors.y, colors.z, colors.w };
        glClearBufferfv(GL_COLOR, 0, color_);
				checkGlError("");
/*
if(color)
			{
				glClearColor(color[0],color[1],color[2],color[3]);
				checkGlError("");
			}
			if(depth)
			{
				glClearDepth(1.0f);
				checkGlError("");
			}
			int f=0;
			if(color) f|=GL_COLOR_BUFFER_BIT;
			if(depth) f|=GL_DEPTH_BUFFER_BIT;
			glClear(f);
*/
}
void RendererGL4_5::viewport(int x, int y, int viewportWidth, int viewportHeight)
{
	glViewport(x, y, viewportWidth, viewportHeight);
				checkGlError("");
}

const GLenum glPrim[] =
{
	GL_TRIANGLES,
	GL_TRIANGLE_FAN,
	GL_TRIANGLE_STRIP,
	GL_QUADS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_POINTS,
}
;
unsigned int RendererGL4_5::DrawPrimitive(Primitives PrimitiveType,UINT StartVertex,UINT PrimitiveCount, const unsigned int flags)
		{
		//if(!((currentVertexBuffers[0]>-1)))
		if(!((selectedVertexBuffers[0]>-1)))
			return;
			//
			
			apply();
			//glDrawArrays(glPrim[PrimitiveType], StartVertex,getVertexCount(PrimitiveType,PrimitiveCount));

				//LOG_PRINT("=%d\n",getVertexCount(PrimitiveType,PrimitiveCount));

				//LOG_PRINT("=%d\n",selectedVertexFormat);

			glDrawArrays(glPrim[PrimitiveType], StartVertex,
#if 0
				PrimitiveCount*getIndicesCount(PrimitiveType)
#else
				getVertexCount(PrimitiveType,PrimitiveCount)
#endif
				);
				checkGlError("");

			reset(flags);
			nDrawCalls++;
		}
		unsigned int RendererGL4_5::DrawIndexedPrimitive(Primitives PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, const unsigned int flags)
		{
//if(!((currentVertexBuffers[0]>-1)&&(currentIndexBuffer>-1)))
if(!((selectedVertexBuffers[0]>-1)&&(selectedIndexBuffer>-1)))
	return;
			apply();
			glDrawElements(
			glPrim[PrimitiveType],
			getIndicesCount(PrimitiveType)*primCount,
			((indexBuffers[selectedIndexBuffer]).indexSize==2)? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
			BUFFER_OFFSET((indexBuffers[selectedIndexBuffer]).indexSize * startIndex));
				checkGlError("");

			reset(flags);
			nDrawCalls++;
		}

ShaderID RendererGL4_5::addShaderHLSLVrtl(	const std::string& aFileName, 
						std::vector<std::string>& avMain, 
						std::vector<ShaderType>& avType)
{
	return SHADER_NONE;
}

		ShaderID RendererGL4_5::addHLSLShaderVrtl(	std::vector<std::string>& avText,
                                            			std::vector<std::string>& avMain,
                                            			std::vector<ShaderType>& avType,
                                            			const unsigned int flags)
		{
	LOG_FNLN;
	for(unsigned int i=0;i<avText.size();i++)
	{
		LOG_PRINT("\navText[%d]:\n%s\n", i, avText[i].c_str());
	}
	for(unsigned int i=0;i<avMain.size();i++)
	{
		LOG_PRINT("\navMain[%d]:\n%s\n", i, avMain[i].c_str());
	}
    if(avMain.size()!=avType.size()) return SHADER_NONE;
			const char *vsText0=0;
			const char *fsText0=0;
		const char *vsName=0;
		const char *psName=0;
    for(unsigned int i=0;i<avType.size();i++)
    {
        switch(i)
        {
            case ePixelShader:
                fsText0=avText[i].c_str();
                psName=avMain[i].c_str();
                break;
            case eVertexShader:
                vsText0=avText[i].c_str();
                vsName=avMain[i].c_str();
                break;
            case eGeometryShader:
                break;
	        case eHullShader:
                break;
	        case eDomainShader:
                break;
	        case eComputeShader:
                break;
        };
    }
#if defined(USE_HLSL_SHADERS)
	std::string vs;
	std::string fs;
	const char* def="#define WSIGN +\n#define ROW_MAJOR\n#define MVPSEMANTIC\n#define fract frac\n#define mix lerp\n#define atan(x,y) atan2(y,x)\n#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) tex2D\(TEX\, float4\(TEXCOORD\.x\, TEXCOORD\.y\, 0.0\, 0.0\)\)\n#define SAMPLER2D sampler2D\n";
	if (def) vs.append(def);
	if (def) fs.append(def);
	if (vsText0) vs.append(vsText0);
	if (fsText0) fs.append(fsText0);

    std::vector<std::string> avText_;
    std::vector<std::string> avMain_;
    std::vector<ShaderType> avType_;
    avText_.push_back(fs);
    avMain_.push_back(psName);
    avType_.push_back(ePixelShader);
    avText_.push_back(vs);
    avMain_.push_back(vsName);
    avType_.push_back(eVertexShader);

	LOG_FNLN;
	for(unsigned int i=0;i<avText_.size();i++)
	{
		LOG_PRINT("\navText_[%d]:\n%s\n", i, avText_[i].c_str());
	}
	for(unsigned int i=0;i<avMain_.size();i++)
	{
		LOG_PRINT("\navMain_[%d]:\n%s\n", i, avMain_[i].c_str());
	}
	std::string vs;
	std::string fs;
	const char* def="#define WSIGN +\n#define ROW_MAJOR\n#define MVPSEMANTIC\n#define fract frac\n#define mix lerp\n#define atan(x,y) atan2(y,x)\n#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) tex2D\(TEX\, float4\(TEXCOORD\.x\, TEXCOORD\.y\, 0.0\, 0.0\)\)\n#define SAMPLER2D sampler2D\n";
	if (def) vs.append(def);
	if (def) fs.append(def);
	if (vsText) vs.append(vsText0);
	if (fsText) fs.append(fsText0);
#if 0
	VkShaderModule shaderModule0=loadShaderGLSL(vs.c_str(), device, VK_SHADER_STAGE_VERTEX_BIT);
	VkShaderModule shaderModule1=loadShaderGLSL(fs.c_str(), device, VK_SHADER_STAGE_FRAGMENT_BIT);
#endif

	ShaderGLSLGL3 shaderGL4_5;
#if defined(USE_HLSL_SHADERS)
	static bool Hlsl2Glsl_init=true;
if(Hlsl2Glsl_init)
{
	Hlsl2Glsl_init=false;
	Hlsl2Glsl_Initialize ();
}
/* ???
	ETargetGLSL_ES_100,	 // OpenGL ES 2.0
	ETargetGLSL_110,	 // OpenGL 2.0
	ETargetGLSL_120,
	ETargetGLSL_140,	 // OpenGL 3.1
	ETargetGLSL_ES_300,	 // OpenGL ES 3.0
*/
	const ETargetVersion version=ETargetGLSL_140;
	ShHandle parser;
	std::string vstext;
	bool ret=Hlsl2Glsl_Translate_ (true, vs, vstext, vsName, parser, version);
	std::string fstext;
	     ret=Hlsl2Glsl_Translate_ (false, fs, fstext, fsName, parser, version);
#if 1
	{std::regex e("#version 140\\b");
	vstext=std::regex_replace (vstext,e,"#version 440 core");}

	{std::regex e("#version 140\\b");
	fstext=std::regex_replace (fstext,e,"#version 440 core");}
#endif
        shaderGL4_5.program = compileGLShaders(vstext.c_str(), fstext.c_str());
#else
	shaderGL4_5.program = compileGLShaders(vs.c_str(), fs.c_str());
#endif
	reflectGLShader(shaderGL4_5.program, shaderGL4_5.samplers, shaderGL4_5.uniforms);
	LOG_FNLN;
	LOG_PRINT("compileGLShaders:shaderGL4_5.program:%x\n", shaderGL4_5.program);
#if 0
	VkShaderModule shaderStages0 = loadShaderGLSL("", device, VK_SHADER_STAGE_VERTEX_BIT);
	VkShaderModule shaderStages1 = loadShaderGLSL("", device, VK_SHADER_STAGE_FRAGMENT_BIT);
#endif

#if 0//!defined(USE_GLES_SHADERS) && !defined(USE_HLSL_SHADERS)
#if defined(_MSC_VER)
	if(stx_fileExists("C:/Windows/System32/glslangValidator.exe") && stx_fileExists("C:/Windows/System32/spirv_cross.exe"))
#elif defined(LINUX) || defined(__APPLE__)
	if(stx_fileExists("/usr/bin/glslangValidator") && stx_fileExists("/usr/bin/spirv_cross"))
#else
	if(1)
#endif
	{
	int rr=0;
	std::string outvs;
	std::string outfs;
	std::string hlslvs=vstext;
	std::string hlslfs=fstext;
#if 0
	rr=Glsl_Translate(true, hlslvs, vsName, eGLSL_Shader_, parser, outvs);
	rr=Glsl_Translate(false, hlslfs, fsName, eGLSL_Shader_, parser, outfs);
	LOG_PRINT("\nGLSLvs:\n%s\n", outvs.c_str());
	LOG_PRINT("\nGLSLfs:\n%s\n", outfs.c_str());
	rr=Glsl_Translate(true, hlslvs, vsName, eHLSL_Shader_, parser, outvs);
	rr=Glsl_Translate(false, hlslfs, fsName, eHLSL_Shader_, parser, outfs);
	LOG_PRINT("\nHLSLvs:\n%s\n", outvs.c_str());
	LOG_PRINT("\nHLSLfs:\n%s\n", outfs.c_str());
	rr=Glsl_Translate(true, hlslvs, vsName, eGLES_Shader_, parser, outvs);
	rr=Glsl_Translate(false, hlslfs, fsName, eGLES_Shader_, parser, outfs);
	LOG_PRINT("\nGLESvs:\n%s\n", outvs.c_str());
	LOG_PRINT("\nGLESfs:\n%s\n", outfs.c_str());
#endif
	rr=Glsl_Translate(true, hlslvs, vsName, eMETAL_Shader_, parser, outvs);
	rr=Glsl_Translate(false, hlslfs, fsName, eMETAL_Shader_, parser, outfs);
	LOG_PRINT("\nMETALvs:\n%s\n", outvs.c_str());
	LOG_PRINT("\nMETALfs:\n%s\n", outfs.c_str());
	}
#endif
	shaders.push_back(shaderGL4_5);
	return shaders.size()-1;
}

TextureID RendererGL4_5::addTexture(Image3* img, const bool useMipMaps, 
					const SamplerStateID samplerState, 
					unsigned int flags)
{
			TextureGL tex;
			stx_memset(&tex, 0, sizeof(TextureGL));

	// Turn a 2D texture formatted as a cross into a proper cubemap if requested
	if (img->is2D() && (flags & CUBEMAP)) img->crossToCube();

	if (flags & RECTANGLE){
		// Texture rectangles doesn't support mipmaps or repeat mode
		if (img->getMipMapCount()) return TEXTURE_NONE;
		flags |= (CLAMP_S | CLAMP_T);
	}

	// Generate mipmaps if they are missing and the filter requires mipmaps
	if (img->getMipMapCount() <= 1 && img->getMipMapCount()){
		if (isPlainFormat(img->getFormat()))
		if (!img->generateMipMaps()){
			LOG_PRINT("Couldn't generate mipmaps\n");
			return TEXTURE_NONE;
		}
	}

	// Turn a 2D texture into a normal map if requested
	FORMAT destFormat = GETFMT(flags);
	if (img->is2D() && (flags & NORMALMAP)){
		if (!img->toNormalMap(destFormat? destFormat : FORMAT_RGBA8)){
			LOG_PRINT("Couldn't generate normalmap\n");
			return TEXTURE_NONE;
		}
	}

	// Not using GL_CLAMP_TO_EDGE with cubemaps makes little sense
	if (img->isCube()) flags |= (CLAMP_S | CLAMP_T);

	// If the target hardware doesn't support the compressed texture format, just decompress it to a compatible format
	FORMAT format = img->getFormat();
	if (isCompressedFormat(format)){
		if (!GL_ARB_texture_compression_Flag || (format <= FORMAT_DXT5 && !GL_EXT_texture_compression_s3tc_Flag) || (format == FORMAT_ATI2N && !GL_ATI_texture_compression_3dc_Flag)){
			img->decompress();
			format = img->getFormat();
		}
	}

	tex.flags  = flags;
	tex.width  = img->getWidth();
	tex.height = img->getHeight();
	tex.glTarget = img->isCube()? GL_TEXTURE_CUBE_MAP : img->is3D()? GL_TEXTURE_3D : img->is2D()? (flags & RECTANGLE? GL_TEXTURE_RECTANGLE_EXT : GL_TEXTURE_2D) : GL_TEXTURE_1D;
	// Generate a texture
	glGenTextures(1, &tex.glTexID);
				checkGlError("");
	glBindTexture(tex.glTarget, tex.glTexID);
				checkGlError("");
	// ??? setupTextureFilter(tex, filter, flags);

	// Upload it all
	GLenum srcFormat, srcType = srcTypes[format];
	if (isDepthFormat(format)){
		srcFormat = GL_DEPTH_COMPONENT;
	} else if (format == FORMAT_R16f || format == FORMAT_R32f){
		srcFormat = GL_RED;
	} else if (format == FORMAT_RG16f || format == FORMAT_RG32f){
		srcFormat = GL_RG_ATI;
	} else {
		srcFormat = srcFormats[getChannelCount(format)];
	}
	GLint internalFormat = internalFormats[destFormat? destFormat : format];

	if((format>=FORMAT_R16F) && (FORMAT_RGBA32F<=format))
	{
		setupSampler(tex.glTarget, samplerStates[samplerState], true);
		#if 0
            glTexImage2D(tex.glTarget, 0, GL_LUMINANCE32F_ARB, img->getWidth(), img->getHeight(), 0, GL_LUMINANCE, GL_FLOAT, img->getPixels(0));
        #else
		    glTexImage2D(tex.glTarget, 0, internalFormat, img->getWidth(0), img->getHeight(0), 0, srcFormat, srcType, img->getPixels(0));
        #endif
		return textures.add(tex);
	}

	ubyte *src;
	int mipMapLevel = 0;
#if 1
	while ((src = img->getPixels(mipMapLevel)) != NULL){
		if (img->isCube()){
			int size = img->getMipMappedSize(mipMapLevel, 1) / 6;
			for (unsigned int i = 0; i < 6; i++){
				if (isCompressedFormat(format)){
					glCompressedTexImage2DARB(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, size, src + i * size);
				} else {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, src + i * size);
				}
			}
		} else if (img->is3D()){
			if (isCompressedFormat(format)){
				glCompressedTexImage3DARB(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), img->getDepth(mipMapLevel), 0, img->getMipMappedSize(mipMapLevel, 1), src);
			} else {
				glTexImage3D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), img->getDepth(mipMapLevel), 0, srcFormat, srcType, src);
			}
		} else if (img->is2D()){
			if (isCompressedFormat(format)){
				glCompressedTexImage2DARB(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, img->getMipMappedSize(mipMapLevel, 1), src);
			} else {
				glTexImage2D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, src);
			}
		} else {
			glTexImage1D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), 0, srcFormat, srcType, src);
		}
#else
	while ((src = img->getPixels(mipMapLevel)) != NULL){
		if (img->isCube()){
			int size = img->getMipMappedSize(mipMapLevel, 1) / 6;
			for (unsigned int i = 0; i < 6; i++){
				if (isCompressedFormat(format)){
					glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, size, src + i * size);
				checkGlError("");
				} else {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, src + i * size);
				checkGlError("");
				}
			}
		} else if (img->is3D()){
			glTexImage3D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), img->getDepth(mipMapLevel), 0, srcFormat, srcType, src);
				checkGlError("");
		} else if (img->is2D()){
			if (isCompressedFormat(format)){
				glCompressedTexImage2D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, img->getMipMappedSize(mipMapLevel, 1), src);
				checkGlError("");
			} else {
				glTexImage2D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, src);
				checkGlError("");
			}
		} else {
			glTexImage1D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), 0, srcFormat, srcType, src);
				checkGlError("");
		}
#endif
		mipMapLevel++;
	}
#if 0
	TextureID thisTex = insertTexture(tex);
	TextureID currTex = currentTextures[currentImageUnit];
	if (currTex != TEXTURE_NONE && (textures[currTex].glTarget == tex.glTarget)){
		// Restore texture binding
		//glBindTexture(textures[currTex].glTarget, textures[currTex].glTexID);
		currentTextures[currentImageUnit] = thisTex;
	}
	return thisTex;
#else

			glBindTexture(tex.glTarget, 0);
				checkGlError("");
			return textures.add(tex);
#endif
}
void RendererGL4_5::changeCullFace(const int cullFace){}

void RendererGL4_5::clearRenderTarget(const TextureID renderTarget, const D3DXFROMWINEVECTOR4 &color, 
						const int slice){}
void RendererGL4_5::clearDepthTarget(const TextureID depthTarget, const float depth, 
						const int slice){}
#endif
#endif
/*

GL error:
file=../../../Libraries/Framework3/include/Framework3/GLShaders.h:
fn=reflectGLShader:
line=76:
 0x00000502
../../../Libraries/Framework3/src/RendererGLSLGL_4_5.cpp:addHLSLShaderVrtl:1706
compileGLShaders:shaderGL4_5.program:1
../../../Libraries/STX/src/fileUtilCPP.cpp:
stx_convertpath:174
../../IndieLib_resources/test.bmp:
../../../Libraries/Framework3/src/RendererGLSLGL_4_5.cpp:changeShader:1021
shaders.size()=1
glUseProgramObject(shaders[0].program=1)
GL error:
file=../../../Libraries/Framework3/src/RendererGLSLGL_4_5.cpp:
fn=changeShader:
line=1025:
 0x00000502

*/
