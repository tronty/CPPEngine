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
#include <Framework3/RendererGLES_3_2.h>
#ifdef __Framework3GLES3_2_H__
#include <Framework3/Hlsl2Glsl_Translate.h>
#include <Framework3/GLShaders.h>

#define LOG_FNLN_X
#define LOG_PRINT_X

#define GETFMT(flags) (FORMAT) (flags >> 26)
#define GL_RG_ATI                           0x883C
#define GL_TEXTURE_RECTANGLE_EXT            0x84F5
bool GL_ATI_texture_compression_3dc_Flag = false;
bool GL_EXT_texture_compression_s3tc_Flag   = false;
bool GL_ARB_texture_compression_Flag  = false;

RendererGLES3_2::RendererGLES3_2(bool fullScreen, int argc, char *argv[]):RendererGLES()
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
RendererGLES3_2::~RendererGLES3_2(){}

const GLenum glPrim_GLES[] =
{
	GL_TRIANGLES,
	GL_TRIANGLE_FAN,
	GL_TRIANGLE_STRIP,
	0,//???GL_QUADS,
	GL_LINES,
	GL_LINE_STRIP,


	GL_LINE_LOOP,
	GL_POINTS,
}
;
GLenum usages_GLES3_2[] =
		{
			GL_STATIC_DRAW,
			GL_DYNAMIC_DRAW,
			GL_STREAM_DRAW,
		}
		;
		GLint minFilters_3_2_GLES[] =
		{
			GL_NEAREST,
			GL_LINEAR,
			GL_LINEAR_MIPMAP_NEAREST,
			GL_LINEAR_MIPMAP_LINEAR,
			GL_LINEAR_MIPMAP_NEAREST,
			GL_LINEAR_MIPMAP_LINEAR,
		}
		;

		static const GLint internalFormats_GLES[] =
		{

			0,
			0,//???GL_INTENSITY8,
			GL_LUMINANCE_ALPHA,
#if defined(SDL_1_2_15_BUILD) && !defined(USE_GLES_SHADERS)
			GL_RGB8,
			GL_RGBA8,
#elif defined(SDL_2_0_5_BUILD)// || defined(USE_GLES_SHADERS)
			GL_RGB8_OES,
			GL_RGBA8_OES,
#elif defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
			GL_RGB,
			GL_RGBA,
#else
			GL_RGB8,
			GL_RGBA8,
#endif
			0,//???GL_INTENSITY16,
			0,//???GL_LUMINANCE16_ALPHA16,
			0,//???GL_RGB16,
			0,//???GL_RGBA16,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,//???GL_INTENSITY_FLOAT16_ATI,
			0,//???GL_LUMINANCE_ALPHA_FLOAT16_ATI,
			0,//???GL_RGB_FLOAT16_ATI,
			0,//???GL_RGBA_FLOAT16_ATI,
			0,//???GL_INTENSITY_FLOAT32_ATI,
			0,//???GL_LUMINANCE_ALPHA_FLOAT32_ATI,
			0,//???GL_RGB_FLOAT32_ATI,
			0,//???GL_RGBA_FLOAT32_ATI,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//???GL_RGB5,
			GL_RGBA4,
			0,//???GL_RGB10_A2,
			0,//???GL_DEPTH_COMPONENT16,
#if defined(SDL_1_2_15_BUILD) && !defined(USE_GLES_SHADERS)
			GL_DEPTH_COMPONENT24,
			GL_DEPTH24_STENCIL8,
#elif !defined(_LINUX) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
			GL_DEPTH_COMPONENT24,
			GL_DEPTH24_STENCIL8,
#else
			0,
			0,
#endif
			0,
#if !defined(_MSC_VER) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
			GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
#else
			0,
#endif
			0,//???GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
			0,//???GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
			0,
#ifdef _MSC_VER
GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI,
#else
0,
#endif
		}
		;
		static const GLenum srcFormats_GLES[] =
		{
			0, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA
		}
		;
		static const GLenum srcTypes_GLES[] =
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
			0, 0, 0, 0,//???GL_UNSIGNED_SHORT_5_6_5,
			0,//???GL_UNSIGNED_SHORT_4_4_4_4_REV,
			0,//???GL_UNSIGNED_INT_2_10_10_10_REV,
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_INT,
#if defined(_LINUX) && !defined(USE_GLES_SHADERS)
			GL_UNSIGNED_INT_24_8,
#elif !defined(_LINUX) && !defined(__APPLE__)
			GL_UNSIGNED_INT_24_8,
#else
			0,
#endif
			0, 0,
			0,
			0,
			0,
			0,
		}
		;


		static const char* internalFormats_GLES_[] =
		{

			0,
			0,//???"GL_INTENSITY8",
			"GL_LUMINANCE_ALPHA",
#if defined(SDL_1_2_15_BUILD) && !defined(USE_GLES_SHADERS)
			"GL_RGB8",
			"GL_RGBA8",
#elif defined(SDL_2_0_5_BUILD)// || defined(USE_GLES_SHADERS)
			"GL_RGB8_OES",
			"GL_RGBA8_OES",
#elif defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
			"GL_RGB",
			"GL_RGBA",
#else
			"GL_RGB8",
			"GL_RGBA8",
#endif
			0,//???GL_INTENSITY16,
			0,//???GL_LUMINANCE16_ALPHA16,
			0,//???GL_RGB16,
			0,//???GL_RGBA16,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,//???GL_INTENSITY_FLOAT16_ATI,
			0,//???GL_LUMINANCE_ALPHA_FLOAT16_ATI,
			0,//???GL_RGB_FLOAT16_ATI,
			0,//???GL_RGBA_FLOAT16_ATI,
			0,//???GL_INTENSITY_FLOAT32_ATI,
			0,//???GL_LUMINANCE_ALPHA_FLOAT32_ATI,
			0,//???GL_RGB_FLOAT32_ATI,
			0,//???GL_RGBA_FLOAT32_ATI,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//???GL_RGB5,
			"GL_RGBA4",
			0,//???GL_RGB10_A2,
			0,//???GL_DEPTH_COMPONENT16,
#if defined(SDL_1_2_15_BUILD) && !defined(USE_GLES_SHADERS)
			"GL_DEPTH_COMPONENT24",
			"GL_DEPTH24_STENCIL8",
#elif !defined(_LINUX) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
			"GL_DEPTH_COMPONENT24",
			"GL_DEPTH24_STENCIL8",
#else
			0,
			0,
#endif
			0,
#if !defined(_MSC_VER) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
			"GL_COMPRESSED_RGB_S3TC_DXT1_EXT",
#else
			0,
#endif
			0,//???GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
			0,//???GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
			0,
#ifdef _MSC_VER
"GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI",
#else
0,
#endif
		}
		;
		static const char* srcFormats_GLES_[] =
		{
			0, "GL_LUMINANCE", "GL_LUMINANCE_ALPHA", "GL_RGB", "GL_RGBA"
		}
		;
		static const char* srcTypes_GLES_[] =
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
			0, 0, 0, 0,//???GL_UNSIGNED_SHORT_5_6_5,
			0,//???GL_UNSIGNED_SHORT_4_4_4_4_REV,
			0,//???GL_UNSIGNED_INT_2_10_10_10_REV,
			"GL_UNSIGNED_SHORT",
			"GL_UNSIGNED_INT",
#if defined(_LINUX) && !defined(USE_GLES_SHADERS)
			"GL_UNSIGNED_INT_24_8",
#elif !defined(_LINUX) && !defined(__APPLE__)
			"GL_UNSIGNED_INT_24_8",
#else
			0,
#endif
			0, 0,
			0,
			0,
			0,
			0,
		}
		;
void RendererGLES3_2::resetVrtl(const unsigned int flags)
{
			reset0(flags);
		if (flags & RESET_TEX){
		for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++){
			selectedTextures[i] = TEXTURE_NONE;
		}
	}

}
void RendererGLES3_2::resetToDefaults()
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
void RendererGLES3_2::applyTextures()
{
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
			currentTextures[i] = texture;
		}
	}
}
void RendererGLES3_2::applySamplerStates()
{
}

typedef GLvoid (APIENTRY *UNIFORM_FUNC)(GLint location, GLsizei count, const void *value);
typedef GLvoid (APIENTRY *UNIFORM_MAT_FUNC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void RendererGLES3_2::applyConstants()
{
	if (currentShader != SHADER_NONE)
	{
		for (unsigned int i = 0; i < (shaders[currentShader]).uniforms.size(); i++)
		{
			ConstantShaderGLSLGL3* uni = (&(shaders[currentShader]).uniforms[0]) + i;
			//if ((uni)->dirty) // ???
			{
/*
GL_APICALL void           GL_APIENTRY glUniform1fv (GLint location, GLsizei count, const GLfloat* v);
GL_APICALL void           GL_APIENTRY glUniform1iv (GLint location, GLsizei count, const GLint* v);
GL_APICALL void           GL_APIENTRY glUniform2fv (GLint location, GLsizei count, const GLfloat* v);
GL_APICALL void           GL_APIENTRY glUniform2iv (GLint location, GLsizei count, const GLint* v);
GL_APICALL void           GL_APIENTRY glUniform3fv (GLint location, GLsizei count, const GLfloat* v);
GL_APICALL void           GL_APIENTRY glUniform3iv (GLint location, GLsizei count, const GLint* v);
GL_APICALL void           GL_APIENTRY glUniform4fv (GLint location, GLsizei count, const GLfloat* v);
GL_APICALL void           GL_APIENTRY glUniform4iv (GLint location, GLsizei count, const GLint* v);
GL_APICALL void           GL_APIENTRY glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_APICALL void           GL_APIENTRY glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_APICALL void           GL_APIENTRY glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
*/
				if ((uni)->type >= CONSTANT_MAT2)
				{
					((UNIFORM_MAT_FUNC) uniformFuncs[(uni)->type])((uni)->location, (uni)->nElements, GL_TRUE, reinterpret_cast<float*>(&uni->data[0]));
				}
				else
				{
					((UNIFORM_FUNC) uniformFuncs[(uni)->type])((uni)->location, (uni)->nElements, reinterpret_cast<float*>(&uni->data[0]));
				}
				(uni)->dirty = false;
			}
		}
	}
}

void RendererGLES3_2::setShaderConstantRaw(const char *name, const void *data, const int size)
{
// GLint glGetUniformLocation (GLuint program, const GLchar *name);
/*
GL_APICALL void           GL_APIENTRY glUniform1fv (GLint location, GLsizei count, const GLfloat* v);
GL_APICALL void           GL_APIENTRY glUniform1iv (GLint location, GLsizei count, const GLint* v);
GL_APICALL void           GL_APIENTRY glUniform2fv (GLint location, GLsizei count, const GLfloat* v);
GL_APICALL void           GL_APIENTRY glUniform2iv (GLint location, GLsizei count, const GLint* v);
GL_APICALL void           GL_APIENTRY glUniform3fv (GLint location, GLsizei count, const GLfloat* v);
GL_APICALL void           GL_APIENTRY glUniform3iv (GLint location, GLsizei count, const GLint* v);
GL_APICALL void           GL_APIENTRY glUniform4fv (GLint location, GLsizei count, const GLfloat* v);
GL_APICALL void           GL_APIENTRY glUniform4iv (GLint location, GLsizei count, const GLint* v);
GL_APICALL void           GL_APIENTRY glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_APICALL void           GL_APIENTRY glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GL_APICALL void           GL_APIENTRY glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
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
					stx_memcpy(&uni->data[0], data, size);//ABC123
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

void RendererGLES3_2::setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState, const int vtstream, const VertexBufferID vtbuffer)
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
BlendStateID RendererGLES3_2::addBlendState(const int srcFactorRGB, const int destFactorRGB, 
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
DepthStateID RendererGLES3_2::addDepthState(const bool depthTest, const bool depthWrite, 
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
AlphaStateID RendererGLES3_2::addAlphaState(const int func, const float ref)
{

	AlphaStateGL blendState;
    blendState.blendEnable = true;
	blendState.alphaFunc=GEQUAL;
	alphaStates.push_back(blendState);
	return alphaStates.size()-1;
}
IndexBufferID RendererGLES3_2::addIndexBufferVrtl(const unsigned int nIndices, const unsigned int indexSize, 
				 const BufferAccess bufferAccess,
				 const void *data, IndexBufferID aold)
{


			unsigned int size = nIndices * indexSize;

if(aold>-1)
{

	LOG_PRINT_X("glDeleteBuffers:\n");
	glDeleteBuffers(1,  &indexBuffers[aold].vboIB);
   	checkGlError("");
//

	indexBuffers[aold].vboIB=0;
	LOG_PRINT_X("glGenBuffers:\n");
	glGenBuffers(1, &indexBuffers[aold].vboIB);
   	checkGlError("");
	


	LOG_PRINT_X("glBindBuffer:\n");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[aold].vboIB);
   	checkGlError("");
	


	indexBuffers[aold].nIndices = nIndices;
	indexBuffers[aold].indexSize = indexSize;
	indexBuffers[aold].data=(BYTE*)data;
}
	IndexBufferGL ib;

if(aold==-1)
{
	GLuint vbb;

	ib.nIndices = nIndices;
	ib.indexSize = indexSize;;
	ib.data=(BYTE*)data;

	LOG_PRINT_X("glGenBuffers:\n");
	glGenBuffers(1, &vbb);
   	checkGlError("");
	
	ib.vboIB=vbb;


	LOG_PRINT_X("glBindBuffer:\n");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbb);
   	checkGlError("");
	


	indexBuffers.push_back(ib);
}

//if(old>-1)
	//indexBuffers[old].size=size;

	if(data>=0) {

	//ib.data.resize(size);
	//stx_memcpy((void*)&ib.data[0],data,size);//ABC123
	//LOG_FNLN;
	LOG_PRINT("glBufferData: size=%d\n", size);
	LOG_PRINT("glBufferData: data=%d\n", data);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usages_GLES3_2[bufferAccess]);
   	checkGlError("");
	
}


	LOG_PRINT_X("glBindBuffer:\n");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   	checkGlError("");
	


			if(aold==-1)
			{

				//LOG_PRINT_X("vbid=%d\n",vertexBuffers.size()-1);
				return indexBuffers.size()-1;
			}
			else if((aold>=0)&&(aold<indexBuffers.size()))
{
return aold;
}
else
	return -1;
}
RasterizerStateID RendererGLES3_2::addRasterizerStateVrtl(const int cullMode, const int fillMode, 
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
TextureID RendererGLES3_2::addRenderTargetVrtl(const int width, const int height, const int depth, 
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
			LOG_PRINT_X("glGenTextures:\n");
			glGenTextures(1, &tex.glTexID);
   checkGlError("");
			LOG_PRINT_X("glBindTexture:\n");
			glBindTexture(tex.glTarget, tex.glTexID);
   checkGlError("");
			setupSampler(tex.glTarget, samplerStates[samplerState]);
			tex.samplerState = samplerState;
			TextureID rt = textures.add(tex);
			setRenderTargetSize(rt, width, height);
			LOG_PRINT_X("glBindTexture:\n");
			glBindTexture(tex.glTarget, 0);
   checkGlError("");
			return rt;
}
TextureID RendererGLES3_2::addRenderDepthVrtl(const int width, const int height, const int arraySize, 
				const FORMAT format, 
				const int msaaSamples, const SamplerStateID samplerState, unsigned int flags)
{

			if (arraySize > 1) return false;
			TextureGL tex;
			stx_memset(&tex, 0, sizeof(TextureGL));
			tex.glTarget = GL_RENDERBUFFER;
			tex.format = format;
			LOG_PRINT_X("glGenRenderbuffers:\n");
			glGenRenderbuffers(1, &tex.glDepthID);
   checkGlError("");
			TextureID rt = textures.add(tex);
			setRenderTargetSize(rt, width, height);
			return rt;
}
SamplerStateID RendererGLES3_2::addSamplerStateVrtl(const Filter filter, const AddressMode s, 
					const AddressMode t, 
					const AddressMode r, const float lod, const unsigned int maxAniso, 
					const int compareFunc, const float *border_color)
{

			if (compareFunc != 0 || border_color != 0) return SS_NONE;
			SamplerStateGL samplerState;
			samplerState.minFilter = minFilters_3_2_GLES[filter];
			samplerState.magFilter = (filter == NEAREST)? GL_NEAREST : GL_LINEAR;
			samplerState.wrapS = (s == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
			samplerState.wrapT = (t == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
			samplerState.wrapR = (r == WRAP)? GL_REPEAT : GL_CLAMP_TO_EDGE;
			samplerState.aniso = hasAniso(filter)? min((unsigned int) maxAnisotropic, maxAniso) : 1;
			samplerState.lod = lod;
			samplerStates.push_back(samplerState);
			return samplerStates.size()-1;
}
VertexBufferID RendererGLES3_2::addVertexBufferVrtl(const int size, const BufferAccess bufferAccess, 
							const void *data,
					 		VertexBufferID aold)
{

if(aold>-1)
{

	//LOG_FNLN;
	LOG_PRINT("glDeleteBuffers:\n");
	glDeleteBuffers(1,  &vertexBuffers[aold].vboVB);
   	checkGlError("");
//

	vertexBuffers[aold].vboVB=0;
	vertexBuffers[aold].data=(BYTE*)data;
	//LOG_FNLN;
	LOG_PRINT("glGenBuffers:\n");
	glGenBuffers(1, &vertexBuffers[aold].vboVB);
   	checkGlError("");
	
	vertexBuffers[aold].size=size;


	//LOG_FNLN;
	LOG_PRINT("glBindBuffer:\n");
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[aold].vboVB);
  	 checkGlError("");
	

}
	VertexBufferGL vb;

if(aold==-1)
{
	vb.size = size;
/*
LOG_PRINT_X("(void*)&vb.data[0]=%x\n",(void*)&vb.data[0]);
LOG_PRINT_X("data=%x\n",data);
LOG_PRINT_X("size=%x\n",size);
*/
	GLuint vbb;

	//LOG_FNLN;
	LOG_PRINT("glGenBuffers:\n");
	glGenBuffers
//ARB
(1, &vbb);
   checkGlError("");
	
	vb.vboVB=vbb;
	vb.data=(BYTE*)data;
	vertexBuffers.push_back(vb);



	//LOG_FNLN;
	LOG_PRINT("glBindBuffer:\n");
	glBindBuffer(GL_ARRAY_BUFFER, vbb);
   	checkGlError("");
	

}

	if(data>=0) {
/////
	//vb.data.resize(size);
	//stx_memcpy((void*)&vb.data[0],data,size);//ABC123
	//LOG_FNLN;
	LOG_PRINT("glBufferData: size=%d\n", size);
	LOG_PRINT("glBufferData: data=%x\n", data);
	glBufferData(GL_ARRAY_BUFFER, size, data, usages_GLES3_2[bufferAccess]);
   	checkGlError("");
	
}


	//LOG_FNLN;
	LOG_PRINT("glBindBuffer:\n");
	glBindBuffer(GL_ARRAY_BUFFER, 0);
   	checkGlError("");
	


/*
LOG_PRINT_X("vbb=%d\n",vbb);
LOG_PRINT_X("size=%d\n",size);
LOG_PRINT_X("data=%x\n",data);
*/


			if(aold==-1)
			{

				//LOG_PRINT_X("vbid=%d\n",vertexBuffers.size()-1);
				return vertexBuffers.size()-1;
			}
			else if((aold>=0)&&(aold<vertexBuffers.size()))
{
//LOG_PRINT_X("vbid=%d\n",old);
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
//LOG_PRINT_X("vbid=%d\n",-1);

			//DBG_HALT;
			return -1;
}}
VertexFormatID RendererGLES3_2::addVertexFormatVrtl(const FormatDesc *formatDesc, const unsigned int nAttribs, 
							const ShaderID shader)
{
#if !defined(__APPLE_)
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
						LOG_PRINT_X("TYPE_GENERIC:\n");
					break;
					case TYPE_TANGENT:
						LOG_PRINT_X("TYPE_TANGENT:\n");
					break;
					case TYPE_BINORMAL:
						LOG_PRINT_X("TYPE_BINORMAL:\n");
					break;
					case TYPE_VERTEX:
						LOG_PRINT_X("TYPE_VERTEX:\n");
					break;
					case TYPE_NORMAL:
						LOG_PRINT_X("TYPE_NORMAL:\n");
					break;
					case TYPE_TEXCOORD:
						LOG_PRINT_X("TYPE_TEXCOORD:\n");
					break;
				};

				switch (formatDesc[i].type)
				{
					case TYPE_GENERIC:
					case TYPE_TANGENT:
					case TYPE_BINORMAL:

					case TYPE_VERTEX:
					//if(vertexFormat.pBase==0) vertexFormat.pBase=&vertexBuffers[vertexFormat.vertex.stream].data[0];
					case TYPE_NORMAL:
					case TYPE_TEXCOORD:
					if(formatDesc[i].type==TYPE_TEXCOORD) nTexCoord++;


					vertexFormat.generic[nGeneric].stream = stream;
					vertexFormat.generic[nGeneric].size   = formatDesc[i].size;
					vertexFormat.generic[nGeneric].offset = vertexFormat.vertexSize[stream];
					vertexFormat.generic[nGeneric].format = formatDesc[i].format;

LOG_PRINT_X("nGeneric=%d\n", nGeneric);
LOG_PRINT_X("stream  =%d\n", stream);
LOG_PRINT_X("size    =%d\n", formatDesc[i].size);
LOG_PRINT_X("offset  =%d\n", vertexFormat.vertexSize[stream]);
LOG_PRINT_X("format  =%d\n", formatDesc[i].format);

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
				LOG_PRINT_X("vertexSize=%d\n", vertexFormat.vertexSize[stream]);
			}
			vertexFormat.shader=shader;
			vertexFormat.maxGeneric = nGeneric;
			vertexFormat.maxTexCoord = nTexCoord;
			vertexFormats.push_back(vertexFormat);
			return vertexFormats.size()-1;
#else
	return -1;
#endif
}
void RendererGLES3_2::changeToMainFramebuffer()
{

			LOG_PRINT_X("glBindFramebuffer:\n");
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
   checkGlError("");
			LOG_PRINT_X("glViewport:\n");
			glViewport(m_viewport.X,m_viewport.Y, m_viewport.Width, m_viewport.Height);
   checkGlError("");
			changeFrontFace(GL_CW);
}
void RendererGLES3_2::changeShader(const ShaderID shader)
{
	//glUseProgram ( userData->programObject );
			if (shader != currentShader)
			{
				if (shader == SHADER_NONE)
				{
					LOG_PRINT_X("glUseProgram(0):%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
					glUseProgram(0);
					checkGlError("");
					//LOG_PRINT_X("glUseProgramObject(0)\n");
				}
				else
				{
					LOG_PRINT_X("glUseProgram(%d):%s:%d\n",(shaders[shader]).program,__FUNCTION__,__LINE__);
					


#ifdef __APPLE__
glUseProgram(handle_to_uint(&(shaders[shader]).program));
#else
glUseProgram((shaders[shader]).program);
#endif


					checkGlError("");
					//LOG_PRINT_X("shader %d:glUseProgramObject(%d)\n",shader,(shaders[shader]).program);
				}
				currentShader = shader;
			}
}
void RendererGLES3_2::changeVertexFormat(const VertexFormatID vertexFormat)
		{
#if 1//defined(USE_HLSL_SHADERS)
			//if (vertexFormat != currentVertexFormat)
			{
				int iii=0;
				static VertexFormatGL zero;
				VertexFormatGL curr = zero, sel = zero;
				if (currentVertexFormat != VF_NONE) curr = vertexFormats[currentVertexFormat];
				if (vertexFormat != VF_NONE) sel = vertexFormats[vertexFormat];
				if ( sel.vertex.size && !curr.vertex.size)
				{
					//glEnableClientState (GL_VERTEX_ARRAY);
					//LOG_FNLN_X;
					LOG_PRINT_X("glEnableVertexAttribArray(%d)\n",iii);
					glEnableVertexAttribArray(iii);
				checkGlError("");
					iii++;
				}
				if (!sel.vertex.size &&  curr.vertex.size)
				{
					//glDisableClientState(GL_VERTEX_ARRAY);
					//LOG_FNLN_X;
					LOG_PRINT_X("glDisableVertexAttribArray(%d)\n",iii);
					glDisableVertexAttribArray(iii);
				checkGlError("");
					iii++;
				}
				if ( sel.normal.size && !curr.normal.size){ //glEnableClientState (GL_NORMAL_ARRAY);
					
					//LOG_FNLN_X;
					LOG_PRINT_X("glEnableVertexAttribArray(%d)\n",iii);
					glEnableVertexAttribArray(iii);
				checkGlError("");
					iii++;}
				if (!sel.normal.size &&  curr.normal.size){ //glDisableClientState(GL_NORMAL_ARRAY);
					
					//LOG_FNLN_X;
					LOG_PRINT_X("glDisableVertexAttribArray(%d)\n",iii);
					glDisableVertexAttribArray(iii);
				checkGlError("");
					iii++;}
				for (int i = 0; i < MAX_GENERIC; i++)
				{
					if ( sel.generic[i].size && !curr.generic[i].size)
					{
					//LOG_FNLN_X;
					LOG_PRINT_X("glEnableVertexAttribArray(%d)\n",iii);
					glEnableVertexAttribArray(iii);
				checkGlError("");
					iii++;}
					if (!sel.generic[i].size &&  curr.generic[i].size){
					//LOG_FNLN_X;
					LOG_PRINT_X("glDisableVertexAttribArray(%d)\n",iii);
					glDisableVertexAttribArray(iii);
				checkGlError("");
					iii++;}
				}
				for (int i = 0; i < MAX_TEXCOORD; i++)
				{
					if ((sel.texCoord[i].size > 0) ^ (curr.texCoord[i].size > 0))
					{
						//glClientActiveTexture(GL_TEXTURE0 + i);
						if (sel.texCoord[i].size > 0)
						{
							//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
							//LOG_FNLN_X;
							LOG_PRINT_X("glEnableVertexAttribArray(%d)\n",iii);
							glEnableVertexAttribArray(iii);
				checkGlError("");
							iii++;
						}
						else
						{
							//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
							//LOG_FNLN_X;
							LOG_PRINT_X("glDisableVertexAttribArray(%d)\n",iii);
							glDisableVertexAttribArray(iii);
				checkGlError("");
							iii++;
						}
					}
				}
				currentVertexFormat = vertexFormat;
			}
#endif
		}
void RendererGLES3_2::changeIndexBuffer(const IndexBufferID indexBuffer)
{

			////if (indexBuffer != currentIndexBuffer)
			{
				if (indexBuffer == IB_NONE)
				{
					LOG_PRINT_X("glBindBuffer:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	  				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   					checkGlError("");
				}
				else
				{
					LOG_PRINT_X("glBindBuffer:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	  				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (indexBuffers[indexBuffer]).vboIB);
   					checkGlError("");
				}
				currentIndexBuffer = indexBuffer;
			}
}
void RendererGLES3_2::changeRasterizerState(const RasterizerStateID rasterizerState)
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
						LOG_PRINT_X("glDisable:\n");
						glDisable(GL_CULL_FACE);
   checkGlError("");
					}

					else
					{
						if (currentCullMode == CULL_NONE) {glEnable(GL_CULL_FACE);
   checkGlError("");}
						LOG_PRINT_X("glCullFace:\n");
						glCullFace(state.cullMode);
   checkGlError("");
					}
					currentCullMode = state.cullMode;
				}
#if defined(SDL_1_2_15_BUILD) && !defined(USE_GLES_SHADERS)
					glPolygonMode(GL_FRONT_AND_BACK, currentFillMode = state.fillMode);
   checkGlError("");
				}
				if (state.multiSample != currentMultiSampleEnable)
				{
					if (state.multiSample)
					{
						LOG_PRINT_X("glEnable:\n");
						glEnable(GL_MULTISAMPLE);
   checkGlError("");
					}
					else
					{
						LOG_PRINT_X("glDisable:\n");
						glDisable(GL_MULTISAMPLE);
   checkGlError("");
					}
					currentMultiSampleEnable = state.multiSample;
				}
#endif
				if (state.scissor != currentScissorEnable)
				{
					if (state.scissor)
					{
						LOG_PRINT_X("glEnable:\n");
						glEnable(GL_SCISSOR_TEST);
   checkGlError("");
					}
					else
					{
						LOG_PRINT_X("glDisable:\n");
						glDisable(GL_SCISSOR_TEST);
   checkGlError("");
					}
					currentScissorEnable = state.scissor;
				}
				currentRasterizerState = rasterizerState;
			}
}
void RendererGLES3_2::changeBlendStateVrtl(const BlendStateID blendState, const unsigned int sampleMask)
{

        if (blendState != currentBlendState){
            if (blendState == BS_NONE || !blendStates[blendState].blendEnable){
                if (currentBlendEnable){
                    LOG_PRINT_X("glDisable:\n");
			glDisable(GL_BLEND);
   checkGlError("");
                    currentBlendEnable = false;
                }
            } else {
                if (blendStates[blendState].blendEnable){
                    if (!currentBlendEnable){
                        LOG_PRINT_X("glEnable:\n");
			glEnable(GL_BLEND);
   checkGlError("");
                        currentBlendEnable = true;
                    }
                    if (blendStates[blendState].srcFactorRGB   != currentSrcFactorRGB ||
                        blendStates[blendState].dstFactorRGB   != currentDstFactorRGB ||
                        blendStates[blendState].srcFactorAlpha != currentSrcFactorAlpha ||
                        blendStates[blendState].dstFactorAlpha != currentDstFactorAlpha){

                        LOG_PRINT_X("glBlendFuncSeparate:\n");
			glBlendFuncSeparate(
                            currentSrcFactorRGB   = blendStates[blendState].srcFactorRGB,
                            currentDstFactorRGB   = blendStates[blendState].dstFactorRGB,
                            currentSrcFactorAlpha = blendStates[blendState].srcFactorAlpha,
                            currentDstFactorAlpha = blendStates[blendState].dstFactorAlpha);
   checkGlError("");
                    }
                    if (blendStates[blendState].blendModeRGB   != currentBlendModeRGB ||
                        blendStates[blendState].blendModeAlpha != currentBlendModeAlpha){
                        LOG_PRINT_X("glBlendEquationSeparate:\n");
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
                LOG_PRINT_X("glColorMask:\n");
		glColorMask((mask & RED) & 1, ((mask & GREEN) >> 1) & 1, ((mask & BLUE) >> 2) & 1, ((mask & ALPHA) >> 3) & 1);
   		checkGlError("");

                currentMask = mask;
            }

            if (alphaToCoverage != currentAlphaToCoverageEnable)
            {
                if (alphaToCoverage) {
                    LOG_PRINT_X("glEnable:\n");
			glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
   checkGlError("");
                } else {
                    LOG_PRINT_X("glDisable:\n");
			glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
   checkGlError("");
		}
                currentAlphaToCoverageEnable = alphaToCoverage;
            }

            currentBlendState = blendState;
        }
}
void RendererGLES3_2::changeDepthStateVrtl(const DepthStateID depthState, const unsigned int stencilRef)
{

	// stencilRef currently not used

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
void RendererGLES3_2::changeRenderTargetsVrtl(const TextureID *colorRTs, const unsigned int nRenderTargets, 
						const TextureID depthRT,
				 		const int depthSlice, const int *slices)
{

			apply();
			if (fbo == 0) glGenFramebuffers(1, &fbo);
			LOG_PRINT_X("glBindFramebuffer:\n");
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   checkGlError("");
			for (unsigned int i = 0; i < nRenderTargets; i++)
			{
				TextureID colorRT = colorRTs[i];
				if ((textures[colorRT]).flags & CUBEMAP)
				{
					if (colorRT != currentColorRT[i] || currentColorRTSlice[i] != slices[i])
					{
						LOG_PRINT_X("glFramebufferTexture2D:\n");
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + slices[i], (textures[colorRT]).glTexID, 0);
   checkGlError("");
						currentColorRTSlice[i] = slices[i];
					}
				}
				else
				{
					if (colorRT != currentColorRT[i])
					{
						LOG_PRINT_X("glFramebufferTexture2D:\n");
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
					LOG_PRINT_X("glFramebufferTexture2D:\n");
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
   checkGlError("");
					currentColorRT[i] = TEXTURE_NONE;
					currentColorRTSlice[i] = NO_SLICE;
				}
#if defined(SDL_1_2_15_BUILD) && !defined(USE_GLES_SHADERS)
					glDrawBuffer(GL_NONE);
   checkGlError("");
					LOG_PRINT_X("glDrawBuffer:\n");
					glReadBuffer(GL_NONE);
   checkGlError("");
				}
				else
				{
					LOG_PRINT_X("glDrawBuffers:\n");
					glDrawBuffers(nRenderTargets, drawBuffers);
   checkGlError("");
					LOG_PRINT_X("glReadBuffer:\n");
					glReadBuffer(GL_COLOR_ATTACHMENT0);
   checkGlError("");
				}
#endif
				nCurrentRenderTargets = nRenderTargets;
			}
			if (depthRT != currentDepthRT)
			{
				if (depthRT >= 0 && (textures[depthRT]).glTarget != GL_RENDERBUFFER)
				{
					LOG_PRINT_X("glFramebufferTexture2D:\n");
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, (textures[depthRT]).glTexID, 0);
   checkGlError("");
					if (isStencilFormat((textures[depthRT]).format))
					{
						LOG_PRINT_X("glFramebufferTexture2D:\n");
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, (textures[depthRT]).glTexID, 0);
   checkGlError("");
					}
					else
					{
						LOG_PRINT_X("glFramebufferTexture2D:\n");
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
   checkGlError("");
					}
				}
				else
				{
					LOG_PRINT_X("glFramebufferRenderbuffer:\n");
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, (depthRT < 0)? 0 : (textures[depthRT]).glTexID);
   checkGlError("");
					if (depthRT >= 0 && isStencilFormat((textures[depthRT]).format))
					{
						LOG_PRINT_X("glFramebufferRenderbuffer:\n");
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, (textures[depthRT]).glTexID);
   checkGlError("");
					}
					else
					{
						LOG_PRINT_X("glFramebufferRenderbuffer:\n");
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
				LOG_PRINT_X("glViewport:\n");
				glViewport(0, 0, (textures[colorRTs[0]]).width, (textures[colorRTs[0]]).height);
   checkGlError("");
			}
			else if(depthRT >= 0)
			{
				changeFrontFace(GL_CW);
				LOG_PRINT_X("glViewport:\n");
				glViewport(0, 0, (textures[depthRT]).width, (textures[depthRT]).height);
   checkGlError("");
			}
			reset(0);
}
void RendererGLES3_2::changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, 
						const intptr offset)
{

			VertexFormatGL cvf = vertexFormats[selectedVertexFormat];
			char *base = (char *)vertexBuffers[vertexBuffer].data;

#if 1
			const GLsizei glTypes[] =
			{
				GL_FLOAT,
				0, GL_UNSIGNED_BYTE,
			}
			;
			GLuint vbo = 0;
			
			if (vertexBuffer != VB_NONE) vbo = (vertexBuffers[vertexBuffer]).vboVB;
			{
				if (currentVertexFormat != VF_NONE)
				{

					VertexFormatGL cvf = vertexFormats[currentVertexFormat];

					int vertexSize = cvf.vertexSize[stream];
					
					int ii=0;
					for (int i = 0; i < MAX_GENERIC; i++)
						if (cvf.generic[i].stream == stream && cvf.generic[i].size) ii++;
					ii--;
					for (int i = 0; i < MAX_GENERIC; i++)
					{

						if (cvf.generic[i].stream == stream && cvf.generic[i].size)
						{
			
                            glVertexAttribPointer(ii, cvf.generic[i].size, glTypes[cvf.generic[i].format], GL_FALSE, vertexSize, base + cvf.generic[i].offset);



							checkGlError("");
							ii--;
						}
					}

				}

				currentVertexBuffers[stream] = vertexBuffer;

				currentOffsets[stream] = offset;

				activeVertexFormat[stream] = currentVertexFormat;

			}
#endif
}
void RendererGLES3_2::changeAlphaState(const AlphaStateID blendState)
{

	if (blendState != currentAlphaState)
	{
		if (blendState == -1 || !(alphaStates[blendState]).blendEnable)
		{
			if (currentAlphaEnable)
			{
                #if !defined(SDL_2_0_5_BUILD) && !defined(USE_GLES_SHADERS)
				LOG_PRINT_X("glDisable:\n");
				glDisable(GL_ALPHA_TEST);
   checkGlError("");
				#endif
    			currentAlphaEnable = false;
			}
		}
		else
		{
			if ((alphaStates[blendState]).blendEnable)
			{
				if (!currentAlphaEnable)
				{
                #if !defined(SDL_2_0_5_BUILD) && !defined(USE_GLES_SHADERS)
				LOG_PRINT_X("glEnable:\n");
				glEnable(GL_ALPHA_TEST);
   checkGlError("");
				#endif
    				currentAlphaEnable = true;
    //  reference = clamp(referenceArg, 0.0f, 1.0f);
    #if !defined(SDL_2_0_5_BUILD) && !defined(USE_GLES_SHADERS)
    LOG_PRINT_X("glAlphaFunc:\n");
	glAlphaFunc(GL_GEQUAL, 0.5f);//???
   checkGlError("");
	#endif
				}
			}
		}
		currentAlphaState = blendState;

}
}
void RendererGLES3_2::clear(const bool clearColor, const bool clearDepth, const bool clearStencil, 
				const float *color, 
				const float depth, const unsigned int stencil)
{
/*
   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );
*/

#if defined(SDL_2_0_5_BUILD)
	SDL_GL_MakeCurrent(STX_Service::GetSDLWindow(), STX_Service::GetSDLContext());
#endif

//
#if !defined(SDL_2_0_5_BUILD)
#if (!defined(QT_BUILD))
			if(color)
			{

#if 0//defined(QT_BUILD)
	qglClearColor(color[0],color[1],color[2],color[3]);
   checkGlError("");
#elif 0//defined(SDL_2_0_5_BUILD)
	LOG_PRINT_X("glClearColor:\n");
	glClearColor(color[0],color[1],color[2],color[3]);
   	checkGlError("");
#elif !defined(__APPLE_)

	LOG_PRINT_X("glClearColor:\n");
	glClearColor(color[0],color[1],color[2],color[3]);

   	checkGlError("");
#endif


			}

			if(depth)
			{

#if !defined(USE_GLES_SHADERS)
				LOG_PRINT_X("glClearDepth:\n");
				glClearDepth(1.0f); 
   				checkGlError("");
#endif

			}

			int f=0;

			if(color) f|=GL_COLOR_BUFFER_BIT;

			if(depth) f|=GL_DEPTH_BUFFER_BIT;

			LOG_PRINT_X("glClear:\n");
			glClear(f);
   			checkGlError("");
			
#endif
#endif
}
void RendererGLES3_2::Clear(bool color, bool depth, D3DXFROMWINEVECTOR4 colors)
{
/*
   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );
*/
   if(color)
	glClear ( GL_COLOR_BUFFER_BIT );
/*


#if defined(SDL_2_0_5_BUILD)
	SDL_GL_MakeCurrent(STX_Service::GetSDLWindow(), STX_Service::GetSDLContext());
#endif

//
#if !defined(SDL_2_0_5_BUILD)
#if (!defined(QT_BUILD))
			if(color)
			{

#if !defined(__APPLE_)

	LOG_PRINT_X("glClearColor:\n");
	glClearColor(color[0],color[1],color[2],color[3]);

   	checkGlError("");
#endif


			}

			if(depth)
			{

#if !defined(USE_GLES_SHADERS)
				LOG_PRINT_X("glClearDepth:\n");
				glClearDepth(1.0f); 
   				checkGlError("");
#endif

			}

			int f=0;

			if(color) f|=GL_COLOR_BUFFER_BIT;

			if(depth) f|=GL_DEPTH_BUFFER_BIT;

			LOG_PRINT_X("glClear:\n");
			glClear(f);
   			checkGlError("");
			
#endif
#endif
*/
}
void RendererGLES3_2::viewport(int x, int y, int viewportWidth, int viewportHeight)
{
	glViewport ( x, y, viewportWidth, viewportHeight );
}
const GLenum glPrim[] =
{
	GL_TRIANGLES,
	GL_TRIANGLE_FAN,
	GL_TRIANGLE_STRIP,
	0,//???GL_QUADS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_POINTS,
}
;
unsigned int RendererGLES3_2::DrawPrimitive(Primitives PrimitiveType,UINT StartVertex,UINT PrimitiveCount, const unsigned int flags)
		{
		//if(!((currentVertexBuffers[0]>-1)))
		if(!((selectedVertexBuffers[0]>-1)))
			return;
			apply();

			if(glPrim[PrimitiveType])
			{
			LOG_PRINT_X("glPrim[PrimitiveType]=%d\n",glPrim[PrimitiveType]);
			LOG_PRINT_X("StartVertex=%d\n",StartVertex);
			LOG_PRINT_X("getVertexCount(PrimitiveType,PrimitiveCount)=%d\n",getVertexCount(PrimitiveType,PrimitiveCount));
			LOG_PRINT_X("glDrawArrays:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
			//glDrawArrays (GLenum mode, GLint first, GLsizei count);
			//glDrawArrays (GLenum mode, GLint first, GLsizei count);
			//glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

			glDrawArrays(glPrim[PrimitiveType], StartVertex,
#if 0
				PrimitiveCount*getIndicesCount(PrimitiveType)
#else
				getVertexCount(PrimitiveType,PrimitiveCount)
#endif
				);
			checkGlError("");
			}
			reset(flags);
			nDrawCalls++;
		}
		unsigned int RendererGLES3_2::DrawIndexedPrimitive(Primitives PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, const unsigned int flags)
		{
//if(!((currentVertexBuffers[0]>-1)&&(currentIndexBuffer>-1)))
if(!((selectedVertexBuffers[0]>-1)&&(selectedIndexBuffer>-1)))
	return;
			apply();
			
			if(glPrim[PrimitiveType])
			{
			unsigned short* usd=(unsigned short*) indexBuffers[selectedIndexBuffer].data;
			unsigned int  * uid=(unsigned int*)   indexBuffers[selectedIndexBuffer].data;

			if(indexBuffers[selectedIndexBuffer].indexSize==2){
				glDrawElements(
					glPrim[PrimitiveType],
					getIndicesCount(PrimitiveType)*primCount,
					GL_UNSIGNED_SHORT,
					(usd+startIndex)//???
				);
				checkGlError("");}
			else{
				glDrawElements(
					glPrim[PrimitiveType],
					getIndicesCount(PrimitiveType)*primCount,
					GL_UNSIGNED_INT,
					(uid+startIndex)//???
				);
			checkGlError("");}
			}
			reset(flags);
			nDrawCalls++;
		}
ShaderID RendererGLES3_2::addHLSLShaderVrtl(std::vector<std::string>& avText,
                                            std::vector<std::string>& avMain,
                                            std::vector<ShaderType>& avType,
                                            const unsigned int flags)
{



ShaderID RendererGLES3_2::addShaderHLSLVrtl(	const std::string& aFileName, 
						std::vector<std::string>& avMain, 
						std::vector<ShaderType>& avType)
{
	return SHADER_NONE;
}

		ShaderID RendererGLES3_2::addHLSLShaderVrtl(	std::vector<std::string>& avText,
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
	std::string vs;
	std::string fs;
	const char* def="#define WSIGN +\n#define ROW_MAJOR\n#define MVPSEMANTIC\n#define fract frac\n#define mix lerp\n#define atan(x,y) atan2(y,x)\n#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) tex2D\(TEX\, float4\(TEXCOORD\.x\, TEXCOORD\.y\, 0.0\, 0.0\)\)\n#define SAMPLER2D sampler2D\n";
	if (def) vs.append(def);
	if (def) fs.append(def);
	if (vsText) vs.append(vsText0);
	if (fsText) fs.append(fsText0);

	ShaderGLSLGL3 shaderGLES3_2;
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
	const ETargetVersion version=ETargetGLSL_ES_300;
	ShHandle parser;
	std::string vstext;
	bool ret=Hlsl2Glsl_Translate_ (true, vs, vstext, vsName, parser, version);
	std::string fstext;
	     ret=Hlsl2Glsl_Translate_ (false, fs, fstext, fsName, parser, version);
        shaderGLES3_2.program = compileGLShaders(vstext.c_str(), fstext.c_str());
#else
	shaderGLES3_2.program = compileGLShaders(vs.c_str(), fs.c_str());
#endif
        reflectGLShader(shaderGLES3_2.program, shaderGLES3_2.samplers, shaderGLES3_2.uniforms);

	shaders.push_back(shaderGLES3_2);
	return shaders.size()-1;
}

TextureID RendererGLES3_2::addTexture(Image3* img, const bool useMipMaps, 
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
	glBindTexture(tex.glTarget, tex.glTexID);
	// ??? setupTextureFilter(tex, filter, flags);

	// Upload it all
	GLenum srcFormat, srcType = srcTypes_GLES[format];
	if (isDepthFormat(format)){
		srcFormat = GL_DEPTH_COMPONENT;
	} else if (format == FORMAT_R16f || format == FORMAT_R32f){
		srcFormat = GL_RED;
	} else if (format == FORMAT_RG16f || format == FORMAT_RG32f){
		srcFormat = GL_RG_ATI;
	} else {
		srcFormat = srcFormats_GLES[getChannelCount(format)];
	}
	GLint internalFormat = internalFormats_GLES[destFormat? destFormat : format];

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
	while ((src = img->getPixels(mipMapLevel)) != NULL){
		if (img->isCube()){
			int size = img->getMipMappedSize(mipMapLevel, 1) / 6;
			for (unsigned int i = 0; i < 6; i++){
				if (isCompressedFormat(format)){
					glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, size, src + i * size);
				} else {
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, src + i * size);
				}
			}
		} else if (img->is3D()){
			glTexImage3D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), img->getDepth(mipMapLevel), 0, srcFormat, srcType, src);
		} else if (img->is2D()){
			if (isCompressedFormat(format)){
				glCompressedTexImage2D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, img->getMipMappedSize(mipMapLevel, 1), src);
			} else {
				glTexImage2D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, src);
			}
		} else {
			glTexImage1D(tex.glTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), 0, srcFormat, srcType, src);
		}
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

void RendererGLES3_2::changeCullFace(const int cullFace){}

void RendererGLES3_2::clearRenderTarget(const TextureID renderTarget, const D3DXFROMWINEVECTOR4 &color, 
						const int slice){}
void RendererGLES3_2::clearDepthTarget(const TextureID depthTarget, const float depth, 
						const int slice){}

#endif

