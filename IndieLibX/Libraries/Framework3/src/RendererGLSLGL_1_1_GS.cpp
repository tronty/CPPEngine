/* * * * * * * * * * * * * Author's note * * * * * * * * * * * **
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

#include <Framework3/RendererGLSLGL_1_1.h>
#ifdef __FrameworkGLSLGL_1_1_H__
#include <Framework3/Hlsl2Glsl_Translate.h>
#include <Framework3/GLShaders.h>

#ifdef __GS__

#ifdef __APPLE__
#define LOG_FNLN_OSX
#define LOG_PRINT_OSX
//define glGetShaderiv
//define glGetShaderInfoLog
#else
#define LOG_FNLN_OSX
#define LOG_PRINT_OSX
#endif

#if 0
#define LOG_PRINT
#define LOG_FNLN
#define LOG_PRINT
#define LOG_FNLN
#define LOG_FNLN_NONE
#define LOG_PRINT_NONE
#define LOG_FNLN_X
#define LOG_PRINT_X
#elif 1
#define STX_PRINT(...) printf(__VA_ARGS__)
#define STX_FNLN printf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
#define LOG_PRINT(...)
#define LOG_FNLN
#define LOG_FNLN_NONE
#define LOG_PRINT_NONE
#define LOG_FNLN_X
#define LOG_PRINT_X
#else
#define LOG_PRINT(...)
#define LOG_FNLN
#define LOG_FNLN_X
#define LOG_PRINT_X
#endif

//include <Framework3/TestFBO.h>
typedef GLvoid (APIENTRY *UNIFORM_FUNC)(GLint location, GLsizei count, const void *value);
typedef GLvoid (APIENTRY *UNIFORM_MAT_FUNC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

//void RendererGL_1_1::
		void RendererGLSLGL_1_1::changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, const intptr offset)
		{
#ifdef __LOG_glDrawElements__
	LOG_FNLN;
#endif
	if(vertexBuffer==-1)
	{
		#if 0
		currentVertexBuffers[stream] = -1;
		currentOffsets[stream] = 0;
		activeVertexFormat[stream] = -1;
		#endif
		return;
	}
			const GLsizei glTypes[] =
			{
				GL_FLOAT,
				0, GL_UNSIGNED_BYTE,
			};
#if !defined(USE_HLSL_SHADERS)
			VertexFormatGL cvf = vertexFormats[selectedVertexFormat];
			int vertexSize = cvf.vertexSize[stream];
			if(!vertexSize) return;
			int nGeneric=cvf.maxGeneric;
			int nAttribs=cvf.maxGeneric;
			int nTexCoord=cvf.maxTexCoord;
			char *base = (char *)&vertexBuffers[vertexBuffer].data;//(char *) offset;
			for (unsigned int i = 0; i < nAttribs; i++)
			{
				unsigned int location=shaders[selectedShader].attribs[i].location;
				glVertexAttribPointer(location, cvf.generic[i].size, glTypes[cvf.generic[i].format], GL_FALSE, vertexSize, base + cvf.generic[i].offset);
				checkGlError("");
				glEnableVertexAttribArray (location);
				checkGlError("");
			}
			currentVertexBuffers[stream] = vertexBuffer;
			currentOffsets[stream] = offset;
			activeVertexFormat[stream] = currentVertexFormat;
#else
			GLuint vbo = 0;
			if (vertexBuffer != VB_NONE) vbo = (vertexBuffers[vertexBuffer]).vboVB;


	if(1){
	if(m_bDebug)
	{
		LOG_FNLN;
		LOG_PRINT("changeVertexBufferVrtl(%d, %d %d)\n", stream, vertexBuffer, offset);
	}}

#if defined(LINUX)
			if (vbo != currentVBO)
#endif
			{
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, currentVBO = vbo);
				checkGlError("");
			}
#if defined(LINUX)
			//if (vertexBuffer != currentVertexBuffers[stream] || offset != currentOffsets[stream] || currentVertexFormat != activeVertexFormat[stream])
#endif
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
// glColorPointer(3, glTypes[cvf.color.format], vertexSize, base + cvf.color.offset);
// glColorPointer(4, glTypes[cvf.color.format], vertexSize, base + cvf.color.offset);
					for (int i = 0; i < MAX_GENERIC; i++)
					{
						if (cvf.generic[i].stream == stream && cvf.generic[i].size)
						{
							glVertexAttribPointerARB(i, cvf.generic[i].size, glTypes[cvf.generic[i].format], GL_TRUE, vertexSize, base + cvf.generic[i].offset);
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

void RendererGLSLGL_1_1::applyConstants(){
	if(currentShader>=shaders.size()) return;
	if (currentShader != SHADER_NONE){
		for (unsigned int i = 0; i < shaders[currentShader].uniforms.size(); i++){
			ConstantShaderGLSLGL3* uni = &shaders[currentShader].uniforms[0] + i;
			if (uni->dirty){
				if (uni->type >= CONSTANT_MAT2){
					((UNIFORM_MAT_FUNC) uniformFuncs[uni->type])(uni->location, uni->nElements, GL_TRUE, (float *) &uni->data[0]);
				} else {
					((UNIFORM_FUNC) uniformFuncs[uni->type])(uni->location, uni->nElements, (float *) &uni->data[0]);
				}
				uni->dirty = false;
			}
		}
	}
}

int RendererGLSLGL_1_1::getSamplerIndex(ShaderID shd, const char* textureName)
{
	if((shd<0)||(shd>(shaders.size()-1)))
		return -1;
	int unit = -1;
	for(unsigned int i=0;i<shaders[shd].samplers.size();i++)
	{
		if(shaders[shd].samplers[i].name==textureName)
		{
			unit=shaders[shd].samplers[i].unit;
			break;
		}
	}
	return unit;
}

void RendererGLSLGL_1_1::setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState, const int vtstream, const VertexBufferID vtbuffer)
{
	if(selectedShader == -1)
		return;
	if(texture == -1)
		return;
	int unit = -1;
	for(unsigned int i=0;i<shaders[selectedShader].samplers.size();i++)
	{
		if(shaders[selectedShader].samplers[i].name==textureName)
		{
			unit=shaders[selectedShader].samplers[i].unit;
			break;
		}
	}
	#ifdef __X7__
	if (unit<0)
		return;
	glActiveTextureARB(GL_TEXTURE0 + unit);
	glEnable(textures[texture].glTarget);
	glBindTexture(textures[texture].glTarget, textures[texture].glTexID);
	if(1){
	if(m_bDebug){LOG_PRINT("glBindTexture(%x, %d);\n", textures[texture].glTarget, textures[texture].glTexID);}
	}
	#else
	if (unit>-1)
	{
		selectedTextures[unit] = texture;
		if(samplerState>-1)
			selectedSamplerStates[unit] = samplerState;
		else
			selectedSamplerStates[unit] = Getlinear(); // ???
	}
	if(0)
	{
		LOG_FNLN;
		LOG_PRINT("Texturename:%s:\n", textureName);
		LOG_PRINT("Textureunit:%d:\n", unit);
	}
	#endif
}
const GLenum glPrimRendererGLSLGL_1_1[] =
{
	GL_TRIANGLES,
	GL_TRIANGLE_FAN,
	GL_TRIANGLE_STRIP,
	GL_QUADS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_POINTS,
};
const char* glPrimRendererGLSLGL_1_1_[] =
{
	"GL_TRIANGLES",
	"GL_TRIANGLE_FAN",
	"GL_TRIANGLE_STRIP",
	"GL_QUADS",
	"GL_LINES",
	"GL_LINE_STRIP",
	"GL_LINE_LOOP",
	"GL_POINTS",
};
unsigned int RendererGLSLGL_1_1::DrawPrimitive(Primitives PrimitiveType,UINT StartVertex,UINT PrimitiveCount, const unsigned int flags)
{
	if(!((selectedVertexBuffers[0]>-1)))
		return 0;
	apply();

	if(0){
	if(1)//m_bDebug)
	{
		LOG_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		LOG_PRINT("glDrawArrays:\n");
		LOG_PRINT("PrimitiveType=%d\n", PrimitiveType);
		LOG_PRINT("glPrimRendererGL_1_1[PrimitiveType]=%d\n", glPrimRendererGLSLGL_1_1[PrimitiveType]);
		LOG_PRINT("glPrimRendererGL_1_1_[PrimitiveType]=%d\n", glPrimRendererGLSLGL_1_1_[PrimitiveType]);
		LOG_PRINT("firstVertex=%d\n", StartVertex);
		LOG_PRINT("nVertices=%d\n", getVertexCount(PrimitiveType,PrimitiveCount));
	}}
	glDrawArrays(	glPrimRendererGLSLGL_1_1[PrimitiveType], StartVertex,
#if 0
			PrimitiveCount*getIndicesCount(PrimitiveType)
#else
			getVertexCount(PrimitiveType,PrimitiveCount)
#endif
	);
	checkGlError("");
	reset(flags);
	nDrawCalls++;
	return 0;
}
unsigned int RendererGLSLGL_1_1::DrawIndexedPrimitive(Primitives PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, const unsigned int flags)
{
	if(!((selectedVertexBuffers[0]>-1)&&(selectedIndexBuffer>-1)))
		return 0;
	apply();
	if(indexBuffers.size()<=selectedIndexBuffer) return 0;
	unsigned int indexSize=(indexBuffers[selectedIndexBuffer]).indexSize;


	if(0){
	if(1)//m_bDebug)
	{
		LOG_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		LOG_PRINT("glDrawElements:\n");
		LOG_PRINT("PrimitiveType=%x\n", PrimitiveType);
		LOG_PRINT("glPrimRendererGL_1_1[PrimitiveType]=%x\n", glPrimRendererGLSLGL_1_1[PrimitiveType]);
		LOG_PRINT("glPrimRendererGL_1_1[PrimitiveType]=%s\n", glPrimRendererGLSLGL_1_1_[PrimitiveType]);
		LOG_PRINT("nIndices=%d\n", getIndicesCount(PrimitiveType)*primCount);
		LOG_PRINT("indexSize=%d\n", indexSize);
		LOG_PRINT("firstIndex=%d\n", startIndex);
		LOG_PRINT("BUFFER_OFFSET(indexSize * firstIndex)=%d\n", BUFFER_OFFSET(indexSize * startIndex));
	}}

	glDrawElements(	glPrimRendererGLSLGL_1_1[PrimitiveType],
			getIndicesCount(PrimitiveType)*primCount,
			(indexSize==2)? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
			BUFFER_OFFSET(indexSize * startIndex));
	checkGlError("");
	reset(flags);
	nDrawCalls++;
	return 0;
}
void RendererGLSLGL_1_1::changePixelShader(const ShaderID shader)
{
	return;
}
void RendererGLSLGL_1_1::changeVertexShader(const ShaderID shader)
{
	return;
}
void RendererGLSLGL_1_1::changeGeometryShader(const ShaderID shader)
{
	return;
}
void RendererGLSLGL_1_1::changeHullShader(const ShaderID shader)
{
	return;
}
void RendererGLSLGL_1_1::changeDomainShader(const ShaderID shader)
{
	return;
}
void RendererGLSLGL_1_1::changeComputeShader(const ShaderID shader)
{
	return;
}
void RendererGLSLGL_1_1::changeShader(const ShaderID shader)
{
	LOG_FNLN;
	LOG_PRINT("changeShader:%x\n", shader);
	if((shader<0)||(shader>=shaders.size()))
		return;
	#if defined(LINUX)
	if (shader != currentShader)
	#endif
	{



	if(1){
	if(m_bDebug)
	{
		LOG_FNLN;
		LOG_PRINT("changeShader(%d)\n", shader);
	}}

		if (shader == SHADER_NONE){
#if 0
			glUseProgramObjectARB(0);
#else
			glUseProgram(handle_to_uint(0));
#endif
			//LOG_FNLN_NONE;
			//LOG_PRINT("glUseProgram(0);\n");


	if(1){
	if(m_bDebug){LOG_PRINT("glUseProgram(0);\n");}
	}


		} else {
#if 0
			glUseProgramObjectARB(shaders[shader].program);
#else
			glUseProgram(handle_to_uint(shaders[shader].program));
#endif
			//LOG_FNLN_NONE;
			//LOG_PRINT_NONE("shaders[%d].program:%x\n", shader, shaders[shader].program);
			//LOG_PRINT("glUseProgram(%d);\n", shaders[shader].program);


	if(1){
	if(m_bDebug){LOG_PRINT("glUseProgram(%d);\n", shaders[shader].program);}
	}
		}
		currentShader = shader;
	}
}
#if 1
void RendererGLSLGL_1_1::setShaderConstantRaw(const char *name, const void *data, const int size)
{
	//STX_PRINT("RendererGLSLGL_1_1::setShaderConstantRaw:name=%s\n", name);
	#if 0
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("selectedShader=%x\n", selectedShader);
	LOG_PRINT_NONE("shaders.size()=%x\n", shaders.size());
	LOG_PRINT_NONE("shaders[selectedShader].uniforms.size()=%x\n", shaders[selectedShader].uniforms.size());
	#endif
	if((selectedShader<0)||(selectedShader>=shaders.size()))
		return;
	int minUniform = 0;
	int maxUniform = shaders[selectedShader].uniforms.size() - 1;
	while (minUniform <= maxUniform){
		int currUniform = (minUniform + maxUniform) >> 1;
		int res = stx_strcmp(name, shaders[selectedShader].uniforms[currUniform].name.c_str());
		if (res == 0){
			ConstantShaderGLSLGL3 *uni = (ConstantShaderGLSLGL3*)&(shaders[selectedShader].uniforms[0]) + currUniform;

			if (stx_memcmp(&uni->data[0], data, size)){
				stx_memcpy(&uni->data[0], data, size);
				uni->dirty = true;
			}
			return;

		} else if (res > 0){
			minUniform = currUniform + 1;
		} else {
			maxUniform = currUniform - 1;
		}
	}
			if(1)
			{
			if(minUniform > maxUniform)
			{
				LOG_FNLN;
				LOG_PRINT("%s:selectedShader:%d\nShaderConstant %s not defined!\n", m_title.c_str(), selectedShader, name);
			}
			else
			{
				LOG_FNLN;
				LOG_PRINT("%s:setShaderConstantRaw:\nname:%s\ndata:%x\nsize:%x\n", m_title.c_str(), name, data, size);
			}
			}
}
#else
void RendererGLSLGL_1_1::setShaderConstantRaw(const char *name, const void *data, const int size)
{
	//LOG_PRINT("RendererGLSLGL_1_1::setShaderConstantRaw:name=%s\n", name);
	#if 0
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("selectedShader=%x\n", selectedShader);
	LOG_PRINT_NONE("shaders.size()=%x\n", shaders.size());
	LOG_PRINT_NONE("shaders[selectedShader].uniforms.size()=%x\n", shaders[selectedShader].uniforms.size());
	#endif
	if((selectedShader<0)||(selectedShader>=shaders.size()))
		return;
	#if 0
	#define STX_FNLN
	#define STX_PRINT
	#endif
#if 0
	int minUniform = 0;
	int maxUniform = shaders[selectedShader].uniforms.size() - 1;
#if 0
	STX_FNLN;
	STX_PRINT("minUniform=%d\n", minUniform);
	STX_PRINT("maxUniform=%d\n", maxUniform);
#endif
	while (minUniform <= maxUniform)
	{
		int currUniform = (minUniform + maxUniform) >> 1;
		int res = stx_strcmp(name, shaders[selectedShader].uniforms[currUniform].name.c_str());
#if 0
		STX_PRINT("currUniform=%d\n", currUniform);
		STX_PRINT("name=%s, shaders[%d].uniforms[%d].name.c_str()=%s\n", name, selectedShader, currUniform, shaders[selectedShader].uniforms[currUniform].name.c_str());
#endif
		if (res == 0)
		{
			ConstantShaderGLSLGL3 *uni = (ConstantShaderGLSLGL3*)&(shaders[selectedShader].uniforms[0]) + currUniform;
			//GLint location=uni->index;
			GLint type=uni->type;
			GLint location=currUniform;
#if 0
			STX_PRINT("type=%x\n", type);
			STX_PRINT("location=%x\n", location);
#endif
			if (type >= SAMPLER_1D && type <= SAMPLER_CUBE) // SAMPLER_CUBE ???
			{
				/*
				void glActiveTexture(GLenum textureUnit​);
				void glBindTexture(GLenum target​, GLuint texture​); 
				*/
				continue;
			}
			else if(type==CONSTANT_FLOAT)
			{
				glUniform1fv(location, 1, (const GLfloat*)&uni->data[0]);
				checkGlError("");
			}
			else if(type==CONSTANT_VEC2)
			{
				glUniform2fv(location, 1, (const GLfloat*)&uni->data[0]);
				checkGlError("");
			}
			else if(type==CONSTANT_VEC3)
			{
				glUniform3fv(location, 1, (const GLfloat*)&uni->data[0]);
				checkGlError("");
			}
			else if(type==CONSTANT_VEC4)
			{
				glUniform4fv(location, 1, (const GLfloat*)&uni->data[0]);
				checkGlError("");
			}
			else if(type==CONSTANT_INT)
			{
				glUniform1iv(location, 1, (const GLint*)&uni->data[0]);
				checkGlError("");
			}
			else if(type==CONSTANT_IVEC2)
			{
				glUniform2iv(location, 1, (const GLint*)&uni->data[0]);
				checkGlError("");
			}
			else if(type==CONSTANT_IVEC3)
			{
				glUniform3iv(location, 1, (const GLint*)&uni->data[0]);
				checkGlError("");
			}
			else if(type==CONSTANT_IVEC4)
			{
				glUniform4iv(location, 1, (const GLint*)&uni->data[0]);
				checkGlError("");
			}
		} else if (res > 0){
			minUniform = currUniform + 1;
		} else {
			maxUniform = currUniform - 1;
		}
	}
#else
	int minUniform = 0;
	int maxUniform = shaders[selectedShader].uniforms.size() - 1;
	while (minUniform <= maxUniform){
		int currUniform = (minUniform + maxUniform) >> 1;
		int res = stx_strcmp(name, shaders[selectedShader].uniforms[currUniform].name.c_str());
		STX_FNLN;
		STX_PRINT("%s:%s\n", name, shaders[selectedShader].uniforms[currUniform].name.c_str());
		STX_PRINT("%d:%d:%x:%d\n", res, currUniform, data, size);
		STX_PRINT("shaders[%d].uniforms.size()=%d\n", selectedShader, shaders[selectedShader].uniforms.size());
		if (res == 0){
			STX_PRINT("currUniform=%d\n", currUniform);
			STX_PRINT("selectedShader)=%d\n", selectedShader);
			STX_PRINT("shaders.size()=%d\n", shaders.size());
			STX_PRINT("shaders[%d].uniforms.size()=%d\n", selectedShader, shaders[selectedShader].uniforms.size());
			ConstantShaderGLSLGL3 *uni = (ConstantShaderGLSLGL3*)&(shaders[selectedShader].uniforms[0]) + currUniform;
			
			STX_PRINT("uni=%x\n", uni);
			STX_PRINT("&uni->data[0]=%x\n", &uni->data[0]);

			STX_PRINT("uni->name=%s\n", uni->name.c_str());
			STX_PRINT("uni->data.size()=%d\n", uni->data.size());
			STX_PRINT("uni->location=%d\n", uni->location);
			STX_PRINT("uni->type=%d\n", uni->type);
			STX_PRINT("uni->nElements=%d\n", uni->nElements);
			STX_PRINT("uni->dirty=%d\n", uni->dirty);

			STX_PRINT("data=%x\n", data);
			STX_PRINT("size=%d\n", size);
			uni->data.resize(size);
			STX_PRINT("uni->data.size()=%d\n", uni->data.size());
			if(uni->data.size()>0){
			if (stx_memcmp(&uni->data[0], data, size)){
				stx_memcpy(&uni->data[0], data, size);
				uni->dirty = true;
			}}
			else STX_PRINT("uni->data.size()<=0 !\n");
			return;

		} else if (res > 0){
			minUniform = currUniform + 1;
		} else {
			maxUniform = currUniform - 1;
		}
	}
#endif
			if(1)
			{
			if(minUniform > maxUniform)
			{
				STX_FNLN;
				STX_PRINT("%s:selectedShader:%d\nShaderConstant %s not defined!\n", m_title.c_str(), selectedShader, name);
			}
			else
			{
				STX_FNLN;
				STX_PRINT("%s:setShaderConstantRaw:\nname:%s\ndata:%x\nsize:%x\n", m_title.c_str(), name, data, size);
			}
			}
}
#endif


		int samplerCompareGL2(const void *sampler0, const void *sampler1)
		{
			return strcmp(((XSampler *) sampler0)->name.c_str(), ((XSampler *) sampler1)->name.c_str());
		}
		ConstantType getConstantType(GLenum type)
		{
			switch (type)
			{
				case GL_FLOAT:		return CONSTANT_FLOAT;
				case GL_FLOAT_VEC2_ARB: return CONSTANT_VEC2;
				case GL_FLOAT_VEC3_ARB: return CONSTANT_VEC3;
				case GL_FLOAT_VEC4_ARB: return CONSTANT_VEC4;
				case GL_INT:		return CONSTANT_INT;
				case GL_INT_VEC2_ARB:	return CONSTANT_IVEC2;
				case GL_INT_VEC3_ARB:	return CONSTANT_IVEC3;
				case GL_INT_VEC4_ARB:	return CONSTANT_IVEC4;
				case GL_BOOL_ARB:	return CONSTANT_BOOL;
				case GL_BOOL_VEC2_ARB:	return CONSTANT_BVEC2;
				case GL_BOOL_VEC3_ARB:	return CONSTANT_BVEC3;
				case GL_BOOL_VEC4_ARB:	return CONSTANT_BVEC4;
				case GL_FLOAT_MAT2_ARB: return CONSTANT_MAT2;
				case GL_FLOAT_MAT3_ARB: return CONSTANT_MAT3;
				case GL_FLOAT_MAT4_ARB: return CONSTANT_MAT4;
			}
			return (ConstantType) -1;
		}

#if 1
		bool samplerCompGL2std(const XSampler & s0, const XSampler &s1)
		{
       if(!(s0.name.c_str()&& s1.name.c_str()))return false;
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
		}
		bool constantCompGL2std(const ConstantShaderGLSLGL3 & s0, const ConstantShaderGLSLGL3 &s1)
		{
       if(!(s0.name.c_str()&& s1.name.c_str()))return false;
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
		}
#if 0
		bool attribCompGL2std(const AttribShaderGLSLGL3 & s0, const AttribShaderGLSLGL3 &s1)
		{
       if(!(s0.name.c_str()&& s1.name.c_str()))return false;
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
		}
#endif
#endif
#if 0
inline void active_vertex_shader_inputs(GLuint prog)
	{
	//GLuint prog=shaders[id/*currentShader*/]).program;
	  char *name;
	  GLint active_attribs, max_length;
	  unsigned i;

	  glGetProgramiv(prog, GL_ACTIVE_ATTRIBUTES, &active_attribs);
				checkGlError("");
	  glGetProgramiv(prog, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length);
				checkGlError("");

	  name = (char*)malloc(max_length + 1);
	  //LOG_PRINT_NONE("Active vertex shader inputs:\n");
	  GLint stride=0;
	  for (i = 0; i < active_attribs; i++) {
	    GLint size;
	    GLenum type;

	    glGetActiveAttrib(prog, i, max_length + 1, 0,
	                      &size, &type, name);
				checkGlError("");
		stride+=size;
		};
	  for (i = 0; i < active_attribs; i++) {
	    GLint size;
	    GLenum type;

	    glGetActiveAttrib(prog, i, max_length + 1, 0,
	                      &size, &type, name);
				checkGlError("");
	    //LOG_PRINT_NONE("Vertex input attribute %s of type %d size %d is at location %d, stride %d\n", name, type, size, glGetAttribLocation(prog, name),stride);
	  }
	  free(name);
	}
#endif

int RendererGLSLGL_1_1::___addGLSLShader___(ShaderType typ, std::vector<std::string>& sText, ShaderGLSLGL3* shaderGL1_1, int& infoLogPos, char* infoLog)
{
	if(typ>eGeometryShader)
		return -1;
#if defined(__APPLE__) && !(defined(OS_IPHONE) || defined(IPHONE_SIMULATOR))
        GLhandleARB shd;
#else
        GLuint shd;
#endif
	switch(typ)
	{
		case ePixelShader:
			shd = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
			break;
		case eVertexShader:
			shd = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
			break;
		#ifndef __APPLE__
		case eGeometryShader:
			shd = glCreateShaderObjectARB(GL_GEOMETRY_SHADER_ARB);
			break;
		#endif
		#if 0
		case eHullShader:
			shd = glCreateShaderObject(GL_TESS_CONTROL_SHADER);
			break;
		case eDomainShader:
			shd = glCreateShaderObject(GL_TESS_EVALUATION_SHADER);
			break;
		case eComputeShader:
			shd = glCreateShaderObject(GL_COMPUTE_SHADER);
			break;
		#endif
	};
        ////stx_snprintf(line,16, "//line %d\n", vsLine);
	checkGlError("");
	const GLchar *shaderStrings[6];
	int strIndex = 0;
	GLint len = 0;
	shaderStrings[strIndex++] = sText[typ].c_str();
#ifdef __APPLE__
	glShaderSourceARB(shd, strIndex, shaderStrings, 0);
#else
	glShaderSource(shd, strIndex, shaderStrings, 0);
#endif
	checkGlError("");
#ifdef __APPLE__
	glCompileShaderARB(shd);
#else
	glCompileShader(shd);
#endif
	checkGlError("");
	GLint sResult=0;
	glGetObjectParameterivARB(shd, GL_OBJECT_COMPILE_STATUS_ARB, &sResult);
	checkGlError("");
	if (sResult)
	{
		glAttachObjectARB(shaderGL1_1->program, shd);
		checkGlError("");
	}
	else
	{
		infoLogPos += stx_snprintf(infoLog + infoLogPos,2048, "Vertex shader error:\n");
	}
	glGetInfoLogARB(shaderGL1_1->shader[eVertexShader], sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
	checkGlError("");
	infoLogPos += len;
	switch(typ)
	{
		case ePixelShader:
			shaderGL1_1->shader[ePixelShader]=shd;
			break;
		case eVertexShader:
			shaderGL1_1->shader[eVertexShader]=shd;
			break;
		#ifndef __APPLE__
		case eGeometryShader:
			shaderGL1_1->shader[eGeometryShader]=shd;
			break;
		#endif
		#if 0
		case eHullShader:
			shaderGL1_1->shader[eHullShader]=shd;
			break;
		case eDomainShader:
			shaderGL1_1->shader[eDomainShader]=shd;
			break;
		case eComputeShader:
			shaderGL1_1->shader[eComputeShader]=shd;
			break;
		#endif
	};
	return sResult;
}

void RendererGLSLGL_1_1::linkGLSLShader(std::vector<std::string>& sText, ShaderGLSLGL3* shaderGL1_1)
{
	char infoLog[2048];
	GLint infoLogPos = 0;
	GLint fsResult=RendererGLSLGL_1_1::___addGLSLShader___(ePixelShader, sText, shaderGL1_1, infoLogPos, infoLog);
	GLint vsResult=RendererGLSLGL_1_1::___addGLSLShader___(eVertexShader, sText, shaderGL1_1, infoLogPos, infoLog);
	GLint gsResult=RendererGLSLGL_1_1::___addGLSLShader___(eGeometryShader, sText, shaderGL1_1, infoLogPos, infoLog);
	GLint hsResult=RendererGLSLGL_1_1::___addGLSLShader___(eHullShader, sText, shaderGL1_1, infoLogPos, infoLog);
	GLint dsResult=RendererGLSLGL_1_1::___addGLSLShader___(eDomainShader, sText, shaderGL1_1, infoLogPos, infoLog);
	GLint csResult=RendererGLSLGL_1_1::___addGLSLShader___(eComputeShader, sText, shaderGL1_1, infoLogPos, infoLog);
	#if 0
	if(!(vsResult && fsResult && gsResult && hsResult && dsResult && csResult))
		return;
	#else
	if(!(vsResult && fsResult))
		return;
	#endif
	GLint linkResult;
	GLint len = 0;
#ifdef __APPLE__
	glLinkProgramARB(shaderGL1_1->program);
#else
	glLinkProgram(shaderGL1_1->program);
#endif
	checkGlError("");
	glGetObjectParameterivARB(shaderGL1_1->program, GL_OBJECT_LINK_STATUS_ARB, &linkResult);
	checkGlError("");
	glGetInfoLogARB(shaderGL1_1->program, sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
	checkGlError("");
	infoLogPos += len;
	if (linkResult)
	{
		GLuint currProgram =
		#ifdef __APPLE__
		handle_to_uint((currentShader == SHADER_NONE)? 0 : shaders[currentShader].program);
		#else
		(currentShader == SHADER_NONE)? 0 : (shaders[currentShader]).program;
		#endif
		#ifdef __APPLE__
		glUseProgram(handle_to_uint(shaderGL1_1->program));
		#else
		glUseProgram(shaderGL1_1->program);
		#endif
		checkGlError("");
	}
}
void RendererGLSLGL_1_1::reflectGLSLShader(std::vector<std::string>& sText, int aProgram, ShaderGLSLGL3* shaderGL1_1)
{
    linkGLSLShader(sText, shaderGL1_1);

					GLint uniformCount, maxLength;
			GLint numActiveAttribs,maxAttribNameLength = 0;
			glGetObjectParameterivARB(shaderGL1_1->program, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &uniformCount);
				checkGlError("");
					glGetObjectParameterivARB(shaderGL1_1->program, GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &maxLength);
				checkGlError("");
			shaderGL1_1->samplers.resize(uniformCount);
			shaderGL1_1->uniforms.resize(uniformCount);
			//shaderGL1_1->attribs.resize(uniformCount);
			int nSamplers = 0;
			int nUniforms = 0;
			//int nAttribs = 0;
			std::vector<char> bbuf(maxLength);
			char *name = (char *)&bbuf[0];
			for (int i = 0; i < uniformCount; i++)
					{
						GLenum type;
						GLint length, size;
#ifdef __APPLE__
glGetActiveUniform(handle_to_uint(shaderGL1_1->program), i, maxLength, &length, &size, &type, name);
#else
glGetActiveUniform(shaderGL1_1->program, i, maxLength, &length, &size, &type, name);
#endif
				checkGlError("");
						if (type >= GL_SAMPLER_1D && type <= GL_SAMPLER_2D_RECT_SHADOW_ARB)
						{
#ifdef __APPLE__
GLint location = glGetUniformLocation(handle_to_uint(shaderGL1_1->program), name);
#else
GLint location = glGetUniformLocation(shaderGL1_1->program, name);
#endif
				checkGlError("");
							glUniform1i(location, nSamplers);
				checkGlError("");
						shaderGL1_1->samplers[nSamplers].location = location;
						shaderGL1_1->samplers[nSamplers].unit=nSamplers;
						shaderGL1_1->samplers[nSamplers].name=std::string( name);
                        stx_Variables::AddSampler(shaders.size(), name, location);
							nSamplers++;
						}
						else
						{
							if (strncmp(name, "gl_", 3) != 0)
							{
                                char *bracket = stx_strchr(name, '[');
								if (bracket == 0 || (bracket[1] == '0' && bracket[2] == ']'))
								{
									if (bracket)
									{
										*bracket = '\0';
										length = (GLint) (bracket - name);
									}
#ifdef __APPLE__
shaderGL1_1->uniforms[nUniforms].location = glGetUniformLocation(handle_to_uint(shaderGL1_1->program), name);
#else
shaderGL1_1->uniforms[nUniforms].location = glGetUniformLocation(shaderGL1_1->program, name);
#endif
				checkGlError("");
									shaderGL1_1->uniforms[nUniforms].type = getConstantType(type);
									shaderGL1_1->uniforms[nUniforms].nElements = size;
									shaderGL1_1->uniforms[nUniforms].name=std::string( name);
						stx_Variables::AddUniform(shaders.size(), name, size, getConstantType(type));
									nUniforms++;
								}
								else if (bracket != 0 && bracket[1] > '0')
								{
									*bracket = '\0';
									for (int i = nUniforms - 1; i >= 0; i--)
									{
										if (strcmp(shaderGL1_1->uniforms[i].name.c_str(), name) == 0)
										{
											int index = atoi(bracket + 1) + 1;
											if (index > shaderGL1_1->uniforms[i].nElements)
											{
												shaderGL1_1->uniforms[i].nElements = index;
											}
										}
									}
								}
							}
						}
					}

			shaderGL1_1->samplers.resize(nSamplers);
			shaderGL1_1->uniforms.resize(nUniforms);
			//shaderGL1_1->attribs.resize(nAttribs);
			if(shaderGL1_1->samplers.size()>1) std::sort(shaderGL1_1->samplers.begin(), shaderGL1_1->samplers.end(), samplerCompGL2std);
			if(shaderGL1_1->uniforms.size()>1) std::sort(shaderGL1_1->uniforms.begin(), shaderGL1_1->uniforms.end(), constantCompGL2std);
			for (int i = 0; i < nUniforms; i++)
					{
						int constantSize = constantTypeSizes[shaderGL1_1->uniforms[i].type] * shaderGL1_1->uniforms[i].nElements;
						shaderGL1_1->uniforms[i].data.resize(constantSize);
						stx_memset((void*)&shaderGL1_1->uniforms[i].data[0], 0, constantSize);
						shaderGL1_1->uniforms[i].dirty = false;
					}
}

int RendererGLSLGL_1_1::__addGLSLShader__(const std::string& aVertexSource, const std::string& aFragmentSource, const std::string& aGeometrySource)
{
	STX_FNLN;
	GLint linkResult;
	ShaderGLSLGL3 shaderGL1_1;
	std::string vsText="";
	std::string fsText="";
	std::string gsText="";
	std::string csText="";
	std::string hsText="";
	std::string dsText="";
	char versionString[16];
	STX_FNLN;
	#if 0
	const char *version = (const char *) glGetString(GL_VERSION);
	if (version)
	{

		int major = atoi(version);
		int minor = atoi(stx_strchr(version, '.') + 1);
		LOG_PRINT("GL_VERSION: %d.%d\n", major, minor);
	}
	version = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);
	
	if (version)
	{

	
		int major = atoi(version);

	
		int minor = atoi(stx_strchr(version, '.') + 1);
	
		LOG_PRINT("GL_SHADING_LANGUAGE_VERSION: %d.%d\n", major, minor);
	
		stx_snprintf(versionString, 16, "\n#version %d%d\n", major, minor);
		LOG_PRINT(versionString, 16, "\n#version %d%d\n", major, minor);
	}
	
	#elif 0
	STX_FNLN;
	stx_snprintf(versionString, 16, "\n#version 130\n");
	const char * def=versionString;
	#elif 1
	const char * def="\n#version 130\n";
	#endif
	if(aGeometrySource.length()) def="\n#version 330\n";
	//if(csText.length()) def="\n#version 430\n";
	//if(hsText.length()) def="\n#version 530\n";
	//if(dsText.length()) def="\n#version 530\n";
	
	STX_FNLN;
	if (def) vsText.append(def);
	if (def) fsText.append(def);
	if (def) gsText.append(def);
	if (def) csText.append(def);
	if (def) hsText.append(def);
	if (def) dsText.append(def);

	STX_FNLN;
#if defined(_MSC_VER)
	vsText.append("precision highp float;\n");
	fsText.append("precision highp float;\n");
	gsText.append("precision highp float;\n");
	csText.append("precision highp float;\n");
	hsText.append("precision highp float;\n");
	dsText.append("precision highp float;\n");
#endif
	STX_FNLN;
#if 1
	//const char* def="#define WSIGN +\n#define ROW_MAJOR\n#define MVPSEMANTIC\n#define fract frac\n#define mix lerp\n#define atan(x,y) atan2(y,x)\n";
	//const char* defvs="#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) tex2Dlod\(TEX\, float4\(TEXCOORD\.x\, TEXCOORD\.y\, 0.0\, 0.0\)\)\n#define SAMPLER2D sampler2D\n";
	const char* deffs="vec3 GammaCorrect3(vec3 aColor)\n{\n\treturn aColor;\n}\nvec4 GammaCorrect4(vec4 aColor)\n{\n\treturn aColor;\n}\n#define WSIGN +\n#define ROW_MAJOR\n#define MVPSEMANTIC\n";
	//const char* deffs="#define fract frac\n#define mix lerp\n#define atan(x,y) atan2(y,x)\n";
	if (deffs) vsText.append(deffs);
	if (deffs) fsText.append(deffs);
	if (deffs) gsText.append(deffs);
	if (deffs) csText.append(deffs);
	if (deffs) hsText.append(deffs);
	if (deffs) dsText.append(deffs);
#endif

	STX_FNLN;
	vsText.append(aVertexSource);
	fsText.append(aFragmentSource);
	gsText.append(aGeometrySource); /*
	if (csText0) csText.append(csText0);
	if (hsText0) hsText.append(hsText0);
	if (dsText0) dsText.append(dsText0); */

	//csText="";hsText="";dsText="";
	gsText=""; // ???

	STX_FNLN;
	STX_PRINT("\nvsText:\n%s\n", vsText.c_str());
	STX_PRINT("\nfsText:\n%s\n", fsText.c_str());
	STX_PRINT("\ngsText:\n%s\n", gsText.c_str());

	STX_FNLN;
GLuint vertexShader = 0;
GLuint fragmentShader = 0;
GLuint geometryShader = 0;
if(aVertexSource.length())
{
	STX_FNLN;
// Create an empty vertex shader handle
vertexShader = glCreateShader(GL_VERTEX_SHADER);

// Send the vertex shader source code to GL
// Note that std::string's .c_str is NULL character terminated.
const GLchar *source = (const GLchar *)vsText.c_str();
glShaderSource(vertexShader, 1, &source, 0);

// Compile the vertex shader
glCompileShader(vertexShader);

GLint isCompiled = GL_FALSE;
glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
if(isCompiled == GL_FALSE)
{
	GLint maxLength = 0;
	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

	// The maxLength includes the NULL character
	std::vector<GLchar> infoLog(maxLength);
	glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
	
	// We don't need the shader anymore.
	glDeleteShader(vertexShader);

	// Use the infoLog as you see fit.
	STX_PRINT("\nVertexShader error:\n%s\n", infoLog);
}}

	STX_FNLN;
if(aFragmentSource.length())
{
	STX_FNLN;GLint isCompiled = GL_FALSE;
// Create an empty fragment shader handle
GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

// Send the fragment shader source code to GL
// Note that std::string's .c_str is NULL character terminated.
const GLchar *source = (const GLchar *)fsText.c_str();
glShaderSource(fragmentShader, 1, &source, 0);

// Compile the fragment shader
glCompileShader(fragmentShader);

glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
if (isCompiled == GL_FALSE)
{
	GLint maxLength = 0;
	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

	// The maxLength includes the NULL character
	std::vector<GLchar> infoLog(maxLength);
	glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
	
	// We don't need the shader anymore.
	glDeleteShader(fragmentShader);
	// Either of them. Don't leak shaders.
	glDeleteShader(vertexShader);

	// Use the infoLog as you see fit.
	STX_PRINT("\nFragmentShader error:\n%s\n", infoLog);
}}

	STX_FNLN;
if(aGeometrySource.length())
{
	STX_FNLN;GLint isCompiled = GL_FALSE;
// Create an empty geometry shader handle
geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

// Send the geometry shader source code to GL
// Note that std::string's .c_str is NULL character terminated.
const GLchar *source = (const GLchar *)gsText.c_str();
glShaderSource(geometryShader, 1, &source, 0);

// Compile the geometry shader
glCompileShader(geometryShader);

glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &isCompiled);
if (isCompiled == GL_FALSE)
{
	GLint maxLength = 0;
	glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &maxLength);

	// The maxLength includes the NULL character
	std::vector<GLchar> infoLog(maxLength);
	glGetShaderInfoLog(geometryShader, maxLength, &maxLength, &infoLog[0]);
	
	// We don't need the shader anymore.
	glDeleteShader(fragmentShader);
	// Either of them. Don't leak shaders.
	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);

	// Use the infoLog as you see fit.
	STX_PRINT("\nGeometryShader error:\n%s\n", infoLog);
}
}

// Vertex and fragment shaders are successfully compiled.
// Now time to link them together into a program.
// Get a program object.
GLuint program = glCreateProgram();

// Attach our shaders to our program
glAttachShader(program, vertexShader);
glAttachShader(program, fragmentShader);
if(aGeometrySource.length()) glAttachShader(program, geometryShader);

// Link our program
glLinkProgram(program);

// Note the different functions here: glGetProgram* instead of glGetShader*.
GLint isLinked = 0;
glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
if (isLinked == GL_FALSE)
{
	GLint maxLength = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

	// The maxLength includes the NULL character
	std::vector<GLchar> infoLog(maxLength);
	glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
	
	// We don't need the program anymore.
	glDeleteProgram(program);
	// Don't leak shaders either.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if(aGeometrySource.length()) glDeleteShader(geometryShader);

	// Use the infoLog as you see fit.
	STX_PRINT("\nLink error:\n%s\n", infoLog);
}

// Always detach shaders after a successful link.
glDetachShader(program, vertexShader);
glDetachShader(program, fragmentShader);
if(aGeometrySource.length()) glDetachShader(program, geometryShader);

return program;
}

void RendererGLSLGL_1_1::buildUniformReferences(int mGLHandle, ShaderGLSLGL3* shaderGL1_1)
	{
STX_FNLN;
			struct UniformReference
			{
				std::string mName;
				bool isReal;
				int mElementCount;
				int mArraySize;
				int mType;
				int mLocation;
				UniformReference()
				{
					mName="";
					isReal=false;
					mElementCount=0;
					mArraySize=0;
					mType=0;
					mLocation=0;
				}
				UniformReference(const UniformReference& rhs)
				{
					mName=rhs.mName;
					isReal=rhs.isReal;
					mElementCount=rhs.mElementCount;
					mArraySize=rhs.mArraySize;
					mType=rhs.mType;
					mLocation=rhs.mLocation;
				}
			};
			UniformReference newUniformReference;
			std::vector<UniformReference> mUniformReferences;
			GLint nUniforms=0;
			GLint nSamplers=0;
STX_FNLN;
		if (1)
		{
			// scan through the active uniforms and add them to the reference list
			GLint uniformCount;
			//GLint location;

			// get the number of active uniforms
			glGetObjectParameterivARB(mGLHandle, GL_OBJECT_ACTIVE_UNIFORMS_ARB,
					&uniformCount);
			STX_PRINT("uniformCount=%d\n", uniformCount);

STX_FNLN;
			// Loop over each of the active uniforms, and add them to the reference container
			// only do this for user defined uniforms, ignore built in gl state uniforms
			for (int index = 0; index < uniformCount; index++)
			{
			#define BUFFERSIZE 100
			#if 0
			char   uniformName[BUFFERSIZE];

			#else
			//glGetObjectParameterivARB(shaderGL1_1->program, GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &maxLength);
			std::vector<char> bbuf(BUFFERSIZE);
			char *uniformName = (char *)&bbuf[0];
			#endif
				glGetActiveUniformARB(mGLHandle, index, BUFFERSIZE, NULL, &newUniformReference.mArraySize, &newUniformReference.mType, uniformName);
				// don't add built in uniforms
				newUniformReference.mLocation = glGetUniformLocationARB(mGLHandle, uniformName);
				STX_PRINT("index=%d, uniformCount=%d\n", index, uniformCount );
				STX_PRINT("(char *)&bbuf[0]=%s\n", (char *)&bbuf[0] );
				STX_PRINT("uniformName=%s\n", uniformName );
				STX_PRINT("newUniformReference.mType=%d\n", newUniformReference.mType );
				STX_PRINT("newUniformReference.mArraySize=%d\n", newUniformReference.mArraySize );
				if (newUniformReference.mLocation >= 0)
				{
					// user defined uniform found, add it to the reference list
					// ??? if(uniformName) newUniformReference.mName = std::string( uniformName );
					STX_PRINT("newUniformReference.mLocation=%d\n", newUniformReference.mLocation );
					STX_PRINT("newUniformReference.mName=%s\n", newUniformReference.mName );
					// decode uniform size and type
					switch (newUniformReference.mType)
					{
					case GL_FLOAT:
						newUniformReference.mType=CONSTANT_FLOAT;
						newUniformReference.isReal = true;
						newUniformReference.mElementCount = 1;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						//stx_Variables::AddSampler(shaders.size(), name, location);
						//stx_Variables::AddUniform(shaders.size(), name, size, getConstantType(type));
						break;

					case GL_FLOAT_VEC2_ARB:
						newUniformReference.mType=CONSTANT_VEC2;
						newUniformReference.isReal = true;
						newUniformReference.mElementCount = 2;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						break;

					case GL_FLOAT_VEC3_ARB:
						newUniformReference.mType=CONSTANT_VEC3;
						newUniformReference.isReal = true;
						newUniformReference.mElementCount = 3;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						break;

					case GL_FLOAT_VEC4_ARB:
						newUniformReference.mType=CONSTANT_VEC4;
						newUniformReference.isReal = true;
						newUniformReference.mElementCount = 4;

						shaderGL1_1->uniforms.resize(shaderGL1_1->uniforms.size()+1);
						shaderGL1_1->uniforms[nUniforms].name=uniformName;
						shaderGL1_1->uniforms[nUniforms].type=CONSTANT_VEC4;
						shaderGL1_1->uniforms[nUniforms].nElements=4;
						// ??? shaderGL1_1->uniforms[nUniforms].data=bbuf;
						shaderGL1_1->uniforms[nUniforms].location;
						shaderGL1_1->uniforms[nUniforms].dirty=false;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						break;

					case GL_INT:
						newUniformReference.mType=CONSTANT_INT;
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 1;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						break;

					case GL_INT_VEC2_ARB:
						newUniformReference.mType=CONSTANT_IVEC2;
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 2;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						break;

					case GL_INT_VEC3_ARB:
						newUniformReference.mType=CONSTANT_IVEC3;
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 3;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						break;

					case GL_INT_VEC4_ARB:
						newUniformReference.mType=CONSTANT_IVEC4;
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 4;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						break;
					case GL_BOOL:
						newUniformReference.mType=CONSTANT_BOOL;
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 1;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						break;
					case GL_BOOL_VEC2:
						newUniformReference.mType=CONSTANT_BVEC2;
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 2;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						break;
					case GL_BOOL_VEC3:
						newUniformReference.mType=CONSTANT_BVEC3;
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 3;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						break;
					case GL_BOOL_VEC4:
						newUniformReference.mType=CONSTANT_BVEC4;
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 4;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						break;

                    case GL_FLOAT_MAT2_ARB:
						newUniformReference.mType=CONSTANT_MAT2;
						newUniformReference.isReal = true;
						newUniformReference.mElementCount = 4;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						break;

                    case GL_FLOAT_MAT3_ARB:
						newUniformReference.mType=CONSTANT_MAT3;
						newUniformReference.isReal = true;
						newUniformReference.mElementCount = 9;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						break;

                    case GL_FLOAT_MAT4_ARB:
						newUniformReference.mType=CONSTANT_MAT4;
						newUniformReference.isReal = true;
						newUniformReference.mElementCount = 16;
						nUniforms++;mUniformReferences.push_back(newUniformReference);
						break;
					case GL_SAMPLER_1D_ARB:
						shaderGL1_1->samplers[nSamplers].type=SAMPLER_1D;
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 1;
						shaderGL1_1->samplers.resize(shaderGL1_1->samplers.size()+1);
						shaderGL1_1->samplers[nSamplers].location = newUniformReference.mLocation;
						shaderGL1_1->samplers[nSamplers].unit=nSamplers;
						shaderGL1_1->samplers[nSamplers].name=std::string(newUniformReference.mName);
						nSamplers++;
						break;
					case GL_SAMPLER_2D_ARB:
						shaderGL1_1->samplers[nSamplers].type=SAMPLER_2D;
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 1;
						shaderGL1_1->samplers.resize(shaderGL1_1->samplers.size()+1);
						shaderGL1_1->samplers[nSamplers].location = newUniformReference.mLocation;
						shaderGL1_1->samplers[nSamplers].unit=nSamplers;
						shaderGL1_1->samplers[nSamplers].name=std::string(newUniformReference.mName);

						shaderGL1_1->samplers.resize(shaderGL1_1->samplers.size()+1);
						shaderGL1_1->samplers[nSamplers].name=std::string(newUniformReference.mName);
						shaderGL1_1->samplers[nSamplers].location=nSamplers;
						shaderGL1_1->samplers[nSamplers].unit=nSamplers;
						shaderGL1_1->samplers[nSamplers].type=SAMPLER_2D;
						nSamplers++;
						break;
					case GL_SAMPLER_3D_ARB:
						shaderGL1_1->samplers[nSamplers].type=SAMPLER_3D;
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 1;
						shaderGL1_1->samplers.resize(shaderGL1_1->samplers.size()+1);
						shaderGL1_1->samplers[nSamplers].location = newUniformReference.mLocation;
						shaderGL1_1->samplers[nSamplers].unit=nSamplers;
						shaderGL1_1->samplers[nSamplers].name=std::string(newUniformReference.mName);
						nSamplers++;
						break;
					case GL_SAMPLER_CUBE_ARB:
						shaderGL1_1->samplers[nSamplers].type=SAMPLER_CUBE;
						newUniformReference.isReal = false;
						newUniformReference.mElementCount = 1;
						shaderGL1_1->samplers.resize(shaderGL1_1->samplers.size()+1);
						shaderGL1_1->samplers[nSamplers].location = newUniformReference.mLocation;
						shaderGL1_1->samplers[nSamplers].unit=nSamplers;
						shaderGL1_1->samplers[nSamplers].name=std::string(newUniformReference.mName);
						nSamplers++;
						break;

					}// end switch	
				} // end if
			} // end for

STX_FNLN;
			//mUniformRefsBuilt = true;
		}
STX_FNLN;
		//shaderGL1_1->uniforms.resize(mUniformReferences.size());

			shaderGL1_1->samplers.resize(nSamplers);
			shaderGL1_1->uniforms.resize(nUniforms);
			//shaderGL1_1->attribs.resize(nAttribs);
#if 1
		STX_PRINT("mUniformReferences.size()=%d\n", mUniformReferences.size());
		STX_PRINT("shaderGL1_1->uniforms.size()=%d\n", shaderGL1_1->uniforms.size());
		STX_PRINT("shaderGL1_1->samplers.size()=%d\n", shaderGL1_1->samplers.size());
#endif
			if(shaderGL1_1->samplers.size()>1) std::sort(shaderGL1_1->samplers.begin(), shaderGL1_1->samplers.end(), samplerCompGL2std);
			if(shaderGL1_1->uniforms.size()>1) std::sort(shaderGL1_1->uniforms.begin(), shaderGL1_1->uniforms.end(), constantCompGL2std);
			for (int i = 0; i < nUniforms; i++)
					{
						STX_PRINT("shaderGL1_1->uniforms[%d].name=%s\n", i, shaderGL1_1->uniforms[i].name.c_str() );
						STX_PRINT("shaderGL1_1->uniforms[%d].type=%d\n", i, shaderGL1_1->uniforms[i].type );
						STX_PRINT("shaderGL1_1->uniforms[%d].nElements=%d\n", i, shaderGL1_1->uniforms[i].nElements );
						int constantSize = constantTypeSizes[shaderGL1_1->uniforms[i].type] * shaderGL1_1->uniforms[i].nElements;
						STX_PRINT("constantSize=%d\n", constantSize);

						shaderGL1_1->uniforms[i].data.resize(constantSize);
						stx_memset((void*)&shaderGL1_1->uniforms[i].data[0], 0, constantSize);
						shaderGL1_1->uniforms[i].dirty = false;
					}
STX_FNLN;
		for (int i = 0; i < mUniformReferences.size(); i++)
		{
			//shaderGL1_1->uniforms.resize(shaderGL1_1->uniforms.size()+1);
			shaderGL1_1->uniforms[i].location = mUniformReferences[i].mLocation;
			shaderGL1_1->uniforms[i].type = mUniformReferences[i].mType;
			shaderGL1_1->uniforms[i].nElements = mUniformReferences[i].mElementCount;
			shaderGL1_1->uniforms[i].name=std::string(mUniformReferences[i].mName);
		}
	//}
STX_FNLN;
}

void RendererGLSLGL_1_1::dumpUniformReferences(ShaderGLSLGL3* shaderGL1_1)
{
	for (int i = 0; i < shaderGL1_1->uniforms.size(); i++)
	{
		STX_PRINT("shaderGL1_1->uniforms[%d].name=%s\n", i, shaderGL1_1->uniforms[i].name.c_str());
		STX_PRINT("shaderGL1_1->uniforms[%d].location=%d\n", i, shaderGL1_1->uniforms[i].location);
		STX_PRINT("shaderGL1_1->uniforms[%d].type=%d\n", i, shaderGL1_1->uniforms[i].type);
		STX_PRINT("shaderGL1_1->uniforms[%d].nElements=%d\n", i, shaderGL1_1->uniforms[i].nElements);
	}
	for (int i = 0; i < shaderGL1_1->samplers.size(); i++)
	{
		STX_PRINT("shaderGL1_1->samplers[%d].name=%s\n", i, shaderGL1_1->samplers[i].name.c_str());
		STX_PRINT("shaderGL1_1->samplers[%d].location=%d\n", i, shaderGL1_1->samplers[i].location);
		STX_PRINT("shaderGL1_1->samplers[%d].unit=%d\n", i, shaderGL1_1->samplers[i].unit);
	}
}
GLuint GetUniforms(GLuint program, ShaderGLSLGL3* shaderGL1_1)
{
    GLint n, max, i;
			GLint nUniforms=0;
			GLint nSamplers=0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &n);
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max);

    for (i = 0; i < n; i++) {
        GLint size, len;
        GLenum type;
        char name[100];

        glGetActiveUniformARB(program, i, 100, &len, &size, &type, name);

	if(type==GL_SAMPLER_2D_ARB)
	{
		shaderGL1_1->samplers.resize(shaderGL1_1->samplers.size()+1);
        	shaderGL1_1->samplers[nSamplers].name = strdup(name);
        	shaderGL1_1->samplers[nSamplers].type = SAMPLER_2D;
        	shaderGL1_1->uniforms[nSamplers].location = glGetUniformLocation(program, name);
		nSamplers++;
	}
	else if(type==GL_FLOAT_VEC4_ARB)
	{
		shaderGL1_1->uniforms.resize(shaderGL1_1->uniforms.size()+1);
		shaderGL1_1->uniforms[nUniforms].name=strdup(name);
		shaderGL1_1->uniforms[nUniforms].type=CONSTANT_VEC4;
		shaderGL1_1->uniforms[nUniforms].nElements=4;
		int size=4*sizeof(float);
		shaderGL1_1->uniforms[nUniforms].data.resize(size);
#if 0
		//std::vector<char> bbuf(BUFFERSIZE);
		//char *uniformName = (char *)&bbuf[0];
		#define BUFFERSIZE 100
		char uniformName[BUFFERSIZE];
		glGetActiveUniformARB(program, index, BUFFERSIZE, NULL, &newUniformReference.mArraySize, &newUniformReference.mType, uniformName);
#endif
		stx_memcpy((void*)&shaderGL1_1->uniforms[nUniforms].data, (void*)name, size);
		shaderGL1_1->uniforms[nUniforms].location;
		shaderGL1_1->uniforms[nUniforms].dirty=false;
		nUniforms++;
	}
    }

    //uniforms[i].name = NULL; /* end of list */

    return n;
}
ShaderID RendererGLSLGL_1_1::addGLSLShaderVrtl(
const char *vsText0, const char *gsText0, const char *fsText0, const char *csText0, const char *hsText0, const char *dsText0,
const char *vsName, const char *gsMain, const char *psName, const char *csMain, const char *hsMain, const char *dsMain,
                                            	const unsigned int flags)
		{
	STX_FNLN;
	GLint linkResult;
	shaders.resize(shaders.size()+1);
	ShaderGLSLGL3* shaderGL1_1=&shaders[shaders.size()-1];

	std::string vsText, fsText, gsText;
	if(vsText0) vsText.append(vsText0);
	if(fsText0) fsText.append(fsText0);
	if(gsText0) gsText.append(gsText0);

	STX_FNLN;
	shaderGL1_1->program=0;
	shaderGL1_1->program=(int)RendererGLSLGL_1_1::__addGLSLShader__(vsText, fsText, gsText);
	STX_FNLN;
	RendererGLSLGL_1_1::buildUniformReferences(shaderGL1_1->program, shaderGL1_1);
	//GLuint r=GetUniforms(shaderGL1_1->program, shaderGL1_1);
	STX_FNLN;
	RendererGLSLGL_1_1::dumpUniformReferences(shaderGL1_1);

	STX_FNLN;
	// ??? shaders.push_back(shaderGL1_1);
	STX_FNLN;
	return shaders.size()-1;
		}

ShaderID RendererGLSLGL_1_1::addHLSLShaderVrtl(
const char *vsText_, const char *gsText, const char *fsText_, const char *csText, const char *hsText, const char *dsText,
const char *vsMain, const char *gsMain, const char *fsMain, const char *csMain, const char *hsMain, const char *dsMain,
                                            	const unsigned int flags)
{
LOG_FNLN;
	ShaderID id=-1;
/*
	#if 1
	// Gamma correct
	OUT.color.rgb = pow(OUT.color.rgb, float3(0.4545));
	#elif 0
	// Gamma from 9.99 to 0.1
	float gamma=1.5;
	OUT.color.rgb=pow(abs(OUT.color.rgb), 1.0 / gamma);
	#endif
	OUT.color.a=1.0;
*/
	std::string vsText;
	std::string fsText;
	std::string vsText__;
	std::string fsText__;
#if 0
	const char* defvs="#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) texture2Dlod\(TEX, TEXCOORD, 0.0\)\)\n#define SAMPLER2D sampler2D\n";
#elif 0
	const char* defvs="#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) tex2Dlod\(TEX\, float4\(TEXCOORD\.x\, TEXCOORD\.y\, 0.0\, 0.0\)\)\n#define SAMPLER2D sampler2D\n";
#else
	const char* defvs="#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) tex2Dlod\(TEX\, float4\(TEXCOORD\.x\, TEXCOORD\.y\, 0.0\, 0.0\)\)\n#define SAMPLER2D sampler2D\n";
#endif
	if(defvs) vsText.append(defvs);
	if(vsText_) vsText.append(vsText_);
	if(fsText_) fsText.append(fsText_);
#if defined(USE_HLSL_SHADERS)
#if 0
	{
		id=addGLSLShaderVrtl(   vsText_, gsText, fsText_, csText, hsText, dsText,
                            vsMain, gsMain, fsMain, csMain, hsMain, dsMain,
                            flags);
	}
	if(id==-1)
#endif
	{
	static bool Hlsl2Glsl_init=true;
	if(Hlsl2Glsl_init)
	{
		Hlsl2Glsl_init=false;
		Hlsl2Glsl_Initialize ();
	}
	const ETargetVersion version=ETargetGLSL_110;
	ShHandle parser;
LOG_FNLN;
	bool rVS=Hlsl2Glsl_Translate_(true, vsText.c_str(), vsText__, vsMain, parser, version);
	if(!rVS)
		return -1;
LOG_FNLN;
	bool rFS=Hlsl2Glsl_Translate_(false, fsText.c_str(), fsText__, fsMain, parser, version);
	if(!rFS)
		return -1;
LOG_FNLN;
	vsText="";
	fsText="";
#if 0
	STX_PRINT("\nvsText:\n%s\n", vsText.c_str());
	STX_PRINT("\nfsText:\n%s\n", fsText.c_str());
#elif 0
	STX_PRINT("\nvsText__:\n%s\n", vsText__.c_str());
	STX_PRINT("\nfsText__:\n%s\n", fsText__.c_str());
#endif

#if 0
	{FILE * pFile = fopen ("vs.glsl","w");
	fprintf (pFile, "%s\n\%s\n", vsText__.c_str(), fsText__.c_str());
   	fclose (pFile);}
	{FILE * pFile = fopen ("fs.glsl","w");
	fprintf (pFile, "%s\n\%s\n", fsText__.c_str(), fsText__.c_str());
   	fclose (pFile);}
#endif
	std::string vsText___, fsText___;
	if(vsText_) vsText___.append(vsText__);
	if(fsText_) fsText___.append(fsText__);

		id=addGLSLShaderVrtl(   vsText___.c_str(), gsText, fsText___.c_str(), csText, hsText, dsText,
                            vsMain, gsMain, fsMain, csMain, hsMain, dsMain,
                            flags);
	vsText__="";
	fsText__="";
	vsText___="";
	fsText___="";
	}
LOG_FNLN;
#else
LOG_FNLN;
		id=addGLSLShaderVrtl(   vsText.c_str(), gsText, fsText.c_str(), csText, hsText, dsText,
                            vsMain, gsMain, fsMain, csMain, hsMain, dsMain,
                            flags);
	vsText="";
	fsText="";
LOG_FNLN;
#endif

LOG_FNLN;
LOG_PRINT("shaders.size()=%d\n", shaders.size());
//STX_PRINT("id=%x\n", id);
LOG_FNLN;
		return id;
}

		static const GLenum targets_GLSL_1_1[] =
		{
GL_TEXTURE_1D,
GL_TEXTURE_2D,
GL_TEXTURE_3D,
GL_TEXTURE_CUBE_MAP
		};

		static const char* targets_GLSL_1_1_[] =
		{
"GL_TEXTURE_1D",
"GL_TEXTURE_2D",
"GL_TEXTURE_3D",
"GL_TEXTURE_CUBE_MAP"
		};
#define GL_TEXTURE_RECTANGLE_EXT            0x84F5
void RendererGLSLGL_1_1::LOGGLTarget(GLenum target)
{
	if(m_bDebug)
	{
	LOG_FNLN_NONE;
	if(target==GL_TEXTURE_CUBE_MAP)
	{
		LOG_PRINT_NONE("glEnable(GL_TEXTURE_CUBE_MAP)\n");
	}
	else if(target==GL_TEXTURE_3D)
	{
		LOG_PRINT_NONE("glEnable(GL_TEXTURE_3D)\n");
	}
	else if(target==GL_TEXTURE_RECTANGLE_EXT)
	{
		LOG_PRINT_NONE("glEnable(GL_TEXTURE_RECTANGLE_EXT)\n");
	}
	else if(target==GL_TEXTURE_2D)
	{
		LOG_PRINT_NONE("glEnable(GL_TEXTURE_2D)\n");
	}
	else if(target==GL_TEXTURE_1D)
	{
		LOG_PRINT_NONE("glEnable(GL_TEXTURE_1D)\n");
	}
	else
	{
		LOG_PRINT_NONE("glEnable(%x)\n", target);
		LOG_PRINT_NONE("glEnable(%d)\n", target);
	}
	}
}

void RendererGLSLGL_1_1::applyTextures(){
		#ifdef __X7__
		return;
		#endif
	for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++){
		TextureID texture = selectedTextures[i];
		TextureID currTex = currentTextures[i];
		if(texture>=textures.size()) texture=-1;
		if(currTex>=textures.size()) currTex=-1;
		#if defined(LINUX)
		if (texture != currTex)
		#endif
		{
		LOG_FNLN;
			glActiveTextureARB(GL_TEXTURE0 + i);
			if ((currTex>-1) && (texture == TEXTURE_NONE)){
				glDisable(textures[currTex].glTarget);
				glBindTexture(textures[currTex].glTarget, 0);
	LOG_PRINT("glBindTexture(%x, %d);\n", textures[currTex].glTarget, 0);
			} else {
				if ((texture>-1) && (currTex == TEXTURE_NONE)){
					glEnable(textures[texture].glTarget);
				} else if ((currTex > -1) && (texture > -1)) if (textures[texture].glTarget != textures[currTex].glTarget)
				{
						glDisable(textures[currTex].glTarget);
						glEnable(textures[texture].glTarget);
				}
				if (texture>-1){
				glBindTexture(textures[texture].glTarget, textures[texture].glTexID);
            	LOG_PRINT("glBindTexture(%x, %d);\n", textures[texture].glTarget, textures[texture].glTexID);}
				if ((texture>-1) && (textures[texture].lod != textureLod[i])){
					glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, textureLod[i] = textures[texture].lod);
				}
			}

			currentTextures[i] = texture;
		}
	}
}
		RendererGLSLGL_1_1::RendererGLSLGL_1_1(bool fullScreen,
int argc, char *argv[]
) : mOk(false), RendererGL_1_1(fullScreen,argc,argv)
{
    	width=STX_Service::GetWindowInstance()->Getwidth();
    	height=STX_Service::GetWindowInstance()->Getheight();

#ifdef _MSC_VER
	GLuint PixelFormat;
	PIXELFORMATDESCRIPTOR pfd;
	stx_memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize	   = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW |PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	#if 0
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	#else
	pfd.cColorBits = 24;
	pfd.cDepthBits = 32;
	#endif
	pfd.iLayerType = PFD_MAIN_PLANE;

	HDC hdc = GetDC( STX_Service::GetWindowInstance()->GetHWND() );
	PixelFormat = ChoosePixelFormat( hdc, &pfd );
	SetPixelFormat( hdc, PixelFormat, &pfd);

	HGLRC hGlrc = wglCreateContext( hdc);

	wglMakeCurrent( hdc, hGlrc );

	//m_hDC = hdc; // ???


#else
#if 0
	if (info.subsystem!=SDL_SYSWM_X11)
	{
		LOG_PRINT_NONE("System is not X11!\n");
		LOG_PRINT_NONE("%d!=%d\n", (int)info.subsystem , (int)SDL_SYSWM_X11 <<'\n');
		return;
	}
	if(!dpy)
	{
		m_dpy=info.info.x11.display;
		m_win=info.info.x11.window;
		int errorbase,tEventbase;
		if (glXQueryExtension(dpy,&errorbase,&tEventbase) == False)
		{
			LOG_PRINT_NONE("OpenGL through GLX not supported.\n");
			return;
		}
		int configuration[]=
		{
			GLX_DOUBLEBUFFER,GLX_RGBA,GLX_DEPTH_SIZE ,12, GLX_RED_SIZE,1,
			GLX_BLUE_SIZE,1,GLX_GREEN_SIZE,1,None
		}
		;
		XVisualInfo *vi=glXChooseVisual(dpy,DefaultScreen(dpy),configuration);
		if(vi== 0)
		{
			LOG_PRINT_NONE("Could not initialize Visual.\n");
			return;
		}
		ctx=glXCreateContext(dpy,vi,0,True);
		if(ctx== 0)
		{
			LOG_PRINT_NONE("Could not initialize GL context.\n");
			return;
		}
		if (!glXMakeCurrent(m_dpy,m_win,ctx))
		{
			m_dpy=0;
			return;
		}
	}
	#endif
#endif
	//initExtensions(m_dpy);
	importGLInit();

	STX_PRINT("GL_VERSION=%s\n", (const char *) glGetString(GL_VERSION));
	STX_PRINT("GL_VENDOR=%s\n", (const char *) glGetString(GL_VENDOR));
	STX_PRINT("GL_RENDERER=%s\n", (const char *) glGetString(GL_RENDERER));
	STX_PRINT("GL_SHADING_LANGUAGE_VERSION=%s\n", (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION));

	#if 0
    std::ofstream out("./OpenGL.log");
	out << "GL_VERSION:\n"; out << (const char *) glGetString(GL_VERSION);out << "\n";
	out << "GL_VENDOR:\n"; out << (const char *) glGetString(GL_VENDOR);out << "\n";
	out << "GL_RENDERER:\n"; out << (const char *) glGetString(GL_RENDERER);out << "\n";
	out << "GL_SHADING_LANGUAGE_VERSION:\n"; out << (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);out << "\n";
    out.close();
	#endif

			GLint units = 1;
#ifdef _WIN32
			if (GL_ARB_fragment_shader_supported || GL_ARB_fragment_program_supported)
			{
				glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &units);
				checkGlError("");
			}
			else
#endif
			{
				glGetIntegerv(GL_MAX_TEXTURE_UNITS, &units);
				checkGlError("");
			}
			nImageUnits = units;
			stx_memset(&textureLod, 0, sizeof(textureLod));
			if (GL_ARB_draw_buffers_supported)
			{
				GLint mrt = 1;
				glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &mrt);
				checkGlError("");
				nMRTs = mrt;
			}
			if (nMRTs > MAX_MRTS) nMRTs = MAX_MRTS;
			for (unsigned int i = 0; i < nMRTs; i++)
			{
				drawBuffers[i] = GL_COLOR_ATTACHMENT0_EXT + i;
			}
			#if 0
			if (GL_EXT_texture_filter_anisotropic_supported)
			{
				GLint aniso = 1;
				glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
				checkGlError("");
				maxAnisotropic = aniso;
			}
			#endif
			fbo = 0;
			currentFrontFace = GL_CW;
			currentVBO = 0;
			texVF	= VF_NONE;
			plainShader = SHADER_NONE;
			plainVF = VF_NONE;
			setGLdefaults();
			resetToDefaults();
			reset(0);
			stx_memset(&activeVertexFormat, VF_NONE, sizeof(activeVertexFormat));
			uniformFuncs[CONSTANT_FLOAT] = (void *) glUniform1fvARB;
			uniformFuncs[CONSTANT_VEC2]  = (void *) glUniform2fvARB;
			uniformFuncs[CONSTANT_VEC3]  = (void *) glUniform3fvARB;
			uniformFuncs[CONSTANT_VEC4]  = (void *) glUniform4fvARB;
			uniformFuncs[CONSTANT_INT]   = (void *) glUniform1ivARB;
			uniformFuncs[CONSTANT_IVEC2] = (void *) glUniform2ivARB;
			uniformFuncs[CONSTANT_IVEC3] = (void *) glUniform3ivARB;
			uniformFuncs[CONSTANT_IVEC4] = (void *) glUniform4ivARB;
			uniformFuncs[CONSTANT_BOOL]  = (void *) glUniform1ivARB;
			uniformFuncs[CONSTANT_BVEC2] = (void *) glUniform2ivARB;
			uniformFuncs[CONSTANT_BVEC3] = (void *) glUniform3ivARB;
			uniformFuncs[CONSTANT_BVEC4] = (void *) glUniform4ivARB;
			uniformFuncs[CONSTANT_MAT2]  = (void *) glUniformMatrix2fvARB;
			uniformFuncs[CONSTANT_MAT3]  = (void *) glUniformMatrix3fvARB;
			uniformFuncs[CONSTANT_MAT4]  = (void *) glUniformMatrix4fvARB;
			bsupportsHalf=false;
			bsupportsR2VB=false;
			overrideR2VB=true;
			overrideHalf=true;

		//defaultFont = addFont(FONT_PATH"Future.dds", FONT_PATH"Future.font", linearClamp);
//LOG_PRINT_NONE("GLSLGL_1_1Renderer\n");
#if 0
    // Initialize the HLSL to GLSL translator
    Hlsl2Glsl_Initialize();

    // Construct a vertex shader parser for the translator to use
    /*ShHandle*/ parserv = Hlsl2Glsl_ConstructParser(EShLangVertex, 0);
    /*ShHandle*/ parserf = Hlsl2Glsl_ConstructParser(EShLangFragment, 0);
	TDebugOptions debugOptions//=EDebugOpNone;
					=EDebugOpIntermediate;

    // Construct a Translator to use for final code translation
    /*ShHandle*/ translator = Hlsl2Glsl_ConstructTranslator(debugOptions);
#endif

			mOk=true;
		}
		void RendererGLSLGL_1_1::DeleteShader(ShaderID id)
		{
			if(id < 0)
				return;
			if(id >= shaders.getCount())
				return;
			glDeleteObjectARB((shaders[id]).shader[eVertexShader]);
			checkGlError("");
			glDeleteObjectARB((shaders[id]).shader[ePixelShader]);
			checkGlError("");
		};
		RendererGLSLGL_1_1::~RendererGLSLGL_1_1()
		{
			STX_TRY;
			apply();
			for (unsigned int i = 0; i < shaders.getCount(); i++)
			{
				for (unsigned int j = 0; j < (shaders[i]).samplers.size(); j++)
				{
					//delete (shaders[i]).samplers[j].name;
				}
				for (unsigned int j = 0; j < (shaders[i]).uniforms.size(); j++)
				{
					//delete (shaders[i]).uniforms[j].name;
					shaders[i].uniforms[j].data.clear();
				}
				(shaders[i]).samplers.clear();
				(shaders[i]).uniforms.clear();
				//(shaders[i]).attribs.clear();
				glDeleteObjectARB((shaders[i]).shader[eVertexShader]);
				checkGlError("");
				glDeleteObjectARB((shaders[i]).shader[ePixelShader]);
				checkGlError("");
                #if 0
				glDeleteObjectARB((shaders[i]).shader[eGeometryShader]);
				checkGlError("");
				glDeleteObjectARB((shaders[i]).shader[eHullShader]);
				checkGlError("");
				glDeleteObjectARB((shaders[i]).shader[eDomainShader]);
				checkGlError("");
				glDeleteObjectARB((shaders[i]).shader[eComputeShader]);
				checkGlError("");
                #endif
				glDeleteObjectARB((shaders[i]).program);
				checkGlError("");
			}
			for (unsigned int i = 0; i < vertexBuffers.getCount(); i++)
			{
				glDeleteBuffersARB(1, &(vertexBuffers[i]).vboVB);
				checkGlError("");
			}
			for (unsigned int i = 0; i < indexBuffers.getCount(); i++)
			{
				glDeleteBuffersARB(1, &(indexBuffers[i]).vboIB);
				checkGlError("");
			}
			for (unsigned int i = 0; i < textures.getCount(); i++)
			{

				removeTexture(i);
			}
			if (fbo) {glDeleteFramebuffersEXT(1, &fbo);
				checkGlError("");}
#if 0
			glDeleteSamplers (GLsizei count, const GLuint *samplers);
			glDeleteProgramsARB (GLsizei n, const GLuint *programs);
			glDeleteObjectARB (GLhandleARB obj);
			glDeleteRenderbuffersEXT (GLsizei n, const GLuint *renderbuffers);
			glDeleteFramebuffersEXT (GLsizei n, const GLuint *framebuffers);
			glDeleteTexturesEXT (GLsizei n, const GLuint *textures);
			glDeleteProgram (GLuint program);
			glDeleteShader (GLuint shader);
#endif
		shaders.clear();
		textures.clear();
		vertexBuffers.clear();
		indexBuffers.clear();
		vertexFormats.clear();
		samplerStates.clear();
		blendStates.clear();
		alphaStates.clear();
		depthStates.clear();
		rasterizerStates.clear();
#if 0
	// ...shutdown
    Hlsl2Glsl_Destruct( parserv );
    Hlsl2Glsl_Destruct( parserf );
	Hlsl2Glsl_Destruct( translator );
    Hlsl2Glsl_Finalize( );
#endif
		STX_CATCH;
		}
#endif
#endif

