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
#define STX_PRINT
#define STX_FNLN
#define LOG_PRINT
#define LOG_FNLN
#define LOG_FNLN_NONE
#define LOG_PRINT_NONE
#define LOG_FNLN_X
#define LOG_PRINT_X
#elif 1
#define STX_PRINT(...)
#define STX_FNLN
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
		printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		printf("glDrawArrays:\n");
		printf("PrimitiveType=%d\n", PrimitiveType);
		printf("glPrimRendererGL_1_1[PrimitiveType]=%d\n", glPrimRendererGLSLGL_1_1[PrimitiveType]);
		printf("glPrimRendererGL_1_1_[PrimitiveType]=%d\n", glPrimRendererGLSLGL_1_1_[PrimitiveType]);
		printf("firstVertex=%d\n", StartVertex);
		printf("nVertices=%d\n", getVertexCount(PrimitiveType,PrimitiveCount));
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
		printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		printf("glDrawElements:\n");
		printf("PrimitiveType=%x\n", PrimitiveType);
		printf("glPrimRendererGL_1_1[PrimitiveType]=%x\n", glPrimRendererGLSLGL_1_1[PrimitiveType]);
		printf("glPrimRendererGL_1_1[PrimitiveType]=%s\n", glPrimRendererGLSLGL_1_1_[PrimitiveType]);
		printf("nIndices=%d\n", getIndicesCount(PrimitiveType)*primCount);
		printf("indexSize=%d\n", indexSize);
		printf("firstIndex=%d\n", startIndex);
		printf("BUFFER_OFFSET(indexSize * firstIndex)=%d\n", BUFFER_OFFSET(indexSize * startIndex));
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

GLint RendererGLSLGL_1_1__addGLSLShader(ShaderType typ, std::vector<std::string>& sText, ShaderGLSLGL3& shaderGL1_1, GLint& infoLogPos, char* infoLog)
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
		glAttachObjectARB(shaderGL1_1.program, shd);
		checkGlError("");
	}
	else
	{
		infoLogPos += stx_snprintf(infoLog + infoLogPos,2048, "Vertex shader error:\n");
	}
	glGetInfoLogARB(shaderGL1_1.shader[eVertexShader], sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
	checkGlError("");
	infoLogPos += len;
	switch(typ)
	{
		case ePixelShader:
			shaderGL1_1.shader[ePixelShader]=shd;
			break;
		case eVertexShader:
			shaderGL1_1.shader[eVertexShader]=shd;
			break;
		#ifndef __APPLE__
		case eGeometryShader:
			shaderGL1_1.shader[eGeometryShader]=shd;
			break;
		#endif
		#if 0
		case eHullShader:
			shaderGL1_1.shader[eHullShader]=shd;
			break;
		case eDomainShader:
			shaderGL1_1.shader[eDomainShader]=shd;
			break;
		case eComputeShader:
			shaderGL1_1.shader[eComputeShader]=shd;
			break;
		#endif
	};
	return sResult;
}

void RendererGLSLGL_1_1::linkGLSLShader(std::vector<std::string>& sText, ShaderGLSLGL3& shaderGL1_1)
{
	char infoLog[2048];
	GLint infoLogPos = 0;
	GLint fsResult=RendererGLSLGL_1_1__addGLSLShader(ePixelShader, sText, shaderGL1_1, infoLogPos, infoLog);
	GLint vsResult=RendererGLSLGL_1_1__addGLSLShader(eVertexShader, sText, shaderGL1_1, infoLogPos, infoLog);
	GLint gsResult=RendererGLSLGL_1_1__addGLSLShader(eGeometryShader, sText, shaderGL1_1, infoLogPos, infoLog);
	GLint hsResult=RendererGLSLGL_1_1__addGLSLShader(eHullShader, sText, shaderGL1_1, infoLogPos, infoLog);
	GLint dsResult=RendererGLSLGL_1_1__addGLSLShader(eDomainShader, sText, shaderGL1_1, infoLogPos, infoLog);
	GLint csResult=RendererGLSLGL_1_1__addGLSLShader(eComputeShader, sText, shaderGL1_1, infoLogPos, infoLog);
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
	glLinkProgramARB(shaderGL1_1.program);
#else
	glLinkProgram(shaderGL1_1.program);
#endif
	checkGlError("");
	glGetObjectParameterivARB(shaderGL1_1.program, GL_OBJECT_LINK_STATUS_ARB, &linkResult);
	checkGlError("");
	glGetInfoLogARB(shaderGL1_1.program, sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
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
		glUseProgram(handle_to_uint(shaderGL1_1.program));
		#else
		glUseProgram(shaderGL1_1.program);
		#endif
		checkGlError("");
	}
}
void RendererGLSLGL_1_1::reflectGLSLShader(std::vector<std::string>& sText, GLuint aProgram, ShaderGLSLGL3& shaderGL1_1)
{
    linkGLSLShader(sText, shaderGL1_1);

					GLint uniformCount, maxLength;
			GLint numActiveAttribs,maxAttribNameLength = 0;
			glGetObjectParameterivARB(shaderGL1_1.program, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &uniformCount);
				checkGlError("");
					glGetObjectParameterivARB(shaderGL1_1.program, GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &maxLength);
				checkGlError("");
			shaderGL1_1.samplers.resize(uniformCount);
			shaderGL1_1.uniforms.resize(uniformCount);
			//shaderGL1_1.attribs.resize(uniformCount);
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
glGetActiveUniform(handle_to_uint(shaderGL1_1.program), i, maxLength, &length, &size, &type, name);
#else
glGetActiveUniform(shaderGL1_1.program, i, maxLength, &length, &size, &type, name);
#endif
				checkGlError("");
						if (type >= GL_SAMPLER_1D && type <= GL_SAMPLER_2D_RECT_SHADOW_ARB)
						{
#ifdef __APPLE__
GLint location = glGetUniformLocation(handle_to_uint(shaderGL1_1.program), name);
#else
GLint location = glGetUniformLocation(shaderGL1_1.program, name);
#endif
				checkGlError("");
							glUniform1i(location, nSamplers);
				checkGlError("");
						shaderGL1_1.samplers[nSamplers].location = location;
						shaderGL1_1.samplers[nSamplers].unit=nSamplers;
						shaderGL1_1.samplers[nSamplers].name=std::string( name);
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
shaderGL1_1.uniforms[nUniforms].location = glGetUniformLocation(handle_to_uint(shaderGL1_1.program), name);
#else
shaderGL1_1.uniforms[nUniforms].location = glGetUniformLocation(shaderGL1_1.program, name);
#endif
				checkGlError("");
									shaderGL1_1.uniforms[nUniforms].type = getConstantType(type);
									shaderGL1_1.uniforms[nUniforms].nElements = size;
									shaderGL1_1.uniforms[nUniforms].name=std::string( name);
						stx_Variables::AddUniform(shaders.size(), name, size, getConstantType(type));
									nUniforms++;
								}
								else if (bracket != 0 && bracket[1] > '0')
								{
									*bracket = '\0';
									for (int i = nUniforms - 1; i >= 0; i--)
									{
										if (strcmp(shaderGL1_1.uniforms[i].name.c_str(), name) == 0)
										{
											int index = atoi(bracket + 1) + 1;
											if (index > shaderGL1_1.uniforms[i].nElements)
											{
												shaderGL1_1.uniforms[i].nElements = index;
											}
										}
									}
								}
							}
						}
					}

			shaderGL1_1.samplers.resize(nSamplers);
			shaderGL1_1.uniforms.resize(nUniforms);
			//shaderGL1_1.attribs.resize(nAttribs);
			if(shaderGL1_1.samplers.size()>1) std::sort(shaderGL1_1.samplers.begin(), shaderGL1_1.samplers.end(), samplerCompGL2std);
			if(shaderGL1_1.uniforms.size()>1) std::sort(shaderGL1_1.uniforms.begin(), shaderGL1_1.uniforms.end(), constantCompGL2std);
			for (int i = 0; i < nUniforms; i++)
					{
						int constantSize = constantTypeSizes[shaderGL1_1.uniforms[i].type] * shaderGL1_1.uniforms[i].nElements;
						shaderGL1_1.uniforms[i].data.resize(constantSize);
						stx_memset((void*)&shaderGL1_1.uniforms[i].data[0], 0, constantSize);
						shaderGL1_1.uniforms[i].dirty = false;
					}
}

ShaderID RendererGLSLGL_1_1::addGLSLShaderVrtl(
const char *vsText0, const char *gsText0, const char *fsText0, const char *csText0, const char *hsText0, const char *dsText0,
const char *vsName, const char *gsMain, const char *psName, const char *csMain, const char *hsMain, const char *dsMain,
                                            	const unsigned int flags)
		{
LOG_FNLN;
    const char* header=0;
	std::string vsText="";
	std::string fsText="";
	std::string gsText="";
	std::string csText="";
	std::string hsText="";
	std::string dsText="";
	char versionString[16];
	
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	#if 0
	const char *version = (const char *) glGetString(GL_VERSION);
	if (version)
	{
		int major = atoi(version);
		int minor = atoi(stx_strchr(version, '.') + 1);
		printf("GL_VERSION: %d.%d\n", major, minor);
	}
	version = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	if (version)
	{
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		int major = atoi(version);
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		int minor = atoi(stx_strchr(version, '.') + 1);
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		printf("GL_SHADING_LANGUAGE_VERSION: %d.%d\n", major, minor);
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		stx_snprintf(versionString, 16, "\n#version %d%d\n", major, minor);
		//stx_snprintf(versionString, 16, "\n#version 130\n");
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		printf("\nversionString=%s\n", versionString);
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	}
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	#elif 1
	stx_snprintf(versionString, 16, "\n#version 130\n");
	#endif

	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	const char * def="\n#version 130\n";//versionString;
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);

	def=versionString;
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	if (def) vsText.append(def);
	if (def) fsText.append(def);
	if (def) gsText.append(def);
	if (def) csText.append(def);
	if (def) hsText.append(def);
	if (def) dsText.append(def);
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);

#if defined(_MSC_VER)
	vsText.append("precision highp float;\n");
	fsText.append("precision highp float;\n");
	gsText.append("precision highp float;\n");
	csText.append("precision highp float;\n");
	hsText.append("precision highp float;\n");
	dsText.append("precision highp float;\n");
#endif
	if (vsText0) vsText.append(vsText0);
	if (fsText0) fsText.append(fsText0);
	if (gsText0) gsText.append(gsText0);
	if (csText0) csText.append(csText0);
	if (hsText0) hsText.append(hsText0);
	if (dsText0) dsText.append(dsText0);
#if 0
	printf("\nvsText:\n%s\n", vsText.c_str());
	printf("\nfsText:\n%s\n", fsText.c_str());
	stx_exit(0);
#endif
	ShaderGLSLGL3 shaderGL1_1;
	char line[16];
	GLint vsResult, fsResult, gsResult, csResult, hsResult, dsResult, linkResult;
	char infoLog[2048];
	GLint len=0, infoLogPos = 0;
	shaderGL1_1.shader[eVertexShader] = 0;
	shaderGL1_1.shader[ePixelShader] = 0;
	shaderGL1_1.shader[eGeometryShader] = 0;
	shaderGL1_1.shader[eComputeShader] = 0;
	shaderGL1_1.shader[eHullShader] = 0;
	shaderGL1_1.shader[eDomainShader] = 0;
	shaderGL1_1.program = glCreateProgramObjectARB();
	checkGlError("");
#if 0 // ???
    std::vector<std::string> sText;
    sText.push_back(fsText0);
    sText.push_back(vsText0);
    reflectGLSLShader(sText, shaderGL1_1.program, shaderGL1_1);
	shaders.push_back(shaderGL1_1);
	return shaders.size()-1;
#endif
#if 0
	if (hsText.length())
	{
		shaderGL1_1.shader[eHullShader] = glCreateShaderObject(GL_TESS_CONTROL_SHADER);
		checkGlError("");
		const GLchar *shaderStrings[6];
		int strIndex = 0;
		shaderStrings[strIndex++] = hsText.c_str();
		glShaderSource(shaderGL1_1.shader[eHullShader], strIndex, shaderStrings, 0);
		checkGlError("");
		glCompileShader(shaderGL1_1.shader[eHullShader]);
		checkGlError("");
		glGetObjectParameteriv(shaderGL1_1.shader[eHullShader], GL_OBJECT_COMPILE_STATUS, &hsResult);
		checkGlError("");
		if (hsResult)
		{
		    glAttachObject(shaderGL1_1.program, shaderGL1_1.shader[eHullShader]);
			checkGlError("");
		}
		else
		{
			infoLogPos += stx_snprintf(infoLog + infoLogPos,2048, "Hull shader error:\n");
		}
		glGetInfoLogARB(shaderGL1_1.shader[eHullShader], sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
		checkGlError("");
		infoLogPos += len;
	}
	if (dsText.length())
	{
		shaderGL1_1.shader[eDomainShader] = glCreateShaderObject(GL_TESS_EVALUATION_SHADER);
		checkGlError("");
		const GLchar *shaderStrings[6];
		int strIndex = 0;
		shaderStrings[strIndex++] = dsText.c_str();
		glShaderSource(shaderGL1_1.shader[eDomainShader], strIndex, shaderStrings, 0);
		checkGlError("");
		glCompileShader(shaderGL1_1.shader[eDomainShader]);
		checkGlError("");
		glGetObjectParameteriv(shaderGL1_1.shader[eDomainShader], GL_OBJECT_COMPILE_STATUS, &dsResult);
		checkGlError("");
		if (dsResult)
		{
		    glAttachObject(shaderGL1_1.program, shaderGL1_1.shader[eDomainShader]);
			checkGlError("");
		}
		else
		{
			infoLogPos += stx_snprintf(infoLog + infoLogPos,2048, "Domain shader error:\n");
		}
		glGetInfoLogARB(shaderGL1_1.shader[eDomainShader], sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
		checkGlError("");
		infoLogPos += len;
	}
	if (csText.length())
	{
		shaderGL1_1.shader[eComputeShader] = glCreateShaderObject(GL_COMPUTE_SHADER);
		checkGlError("");
		const GLchar *shaderStrings[6];
		int strIndex = 0;
		shaderStrings[strIndex++] = csText.c_str();
		glShaderSource(shaderGL1_1.shader[eComputeShader], strIndex, shaderStrings, 0);
		checkGlError("");
		glCompileShader(shaderGL1_1.shader[eComputeShader]);
		checkGlError("");
		glGetObjectParameteriv(shaderGL1_1.shader[eComputeShader], GL_OBJECT_COMPILE_STATUS, &csResult);
		checkGlError("");
		if (csResult)
		{
		    glAttachObject(shaderGL1_1.program, shaderGL1_1.shader[eComputeShader]);
			checkGlError("");
		}
		else
		{
			infoLogPos += stx_snprintf(infoLog + infoLogPos,2048, "Compute shader error:\n");
		}
		glGetInfoLogARB(shaderGL1_1.shader[eComputeShader], sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
		checkGlError("");
		infoLogPos += len;
	}
	else csResult = GL_TRUE;
	if (gsText.length())
	{
		shaderGL1_1.shader[eGeometryShader] = glCreateShaderObject(GL_GEOMETRY_SHADER);
		checkGlError("");
		const GLchar *shaderStrings[6];
		int strIndex = 0;
		shaderStrings[strIndex++] = gsText.c_str();
		glShaderSource(shaderGL1_1.shader[eGeometryShader], strIndex, shaderStrings, 0);
		checkGlError("");
		glCompileShader(shaderGL1_1.shader[eGeometryShader]);
		checkGlError("");
		glGetObjectParameteriv(shaderGL1_1.shader[eGeometryShader], GL_OBJECT_COMPILE_STATUS, &gsResult);
		checkGlError("");
		if (gsResult)
		{
		    glAttachObject(shaderGL1_1.program, shaderGL1_1.shader[eGeometryShader]);
			checkGlError("");
		}
		else
		{
			infoLogPos += stx_snprintf(infoLog + infoLogPos,2048, "Geometry shader error:\n");
		}
		glGetInfoLogARB(shaderGL1_1.shader[eGeometryShader], sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
		checkGlError("");
		infoLogPos += len;
	}
	else gsResult = GL_TRUE;
#endif
	if (vsText.length())
	{
#if 0
GL_COMPUTE_SHADER
GL_TESS_EVALUATION_SHADER
GL_TESS_CONTROL_SHADER
GL_GEOMETRY_SHADER
GL_VERTEX_SHADER
GL_FRAGMENT_SHADER
GL_GEOMETRY_SHADER_ARB
GL_VERTEX_SHADER_ARB
GL_FRAGMENT_SHADER_ARB
#endif
		shaderGL1_1.shader[eVertexShader] = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		checkGlError("");
		const GLcharARB *shaderStrings[6];
		int strIndex = 0;
		shaderStrings[strIndex++] = vsText.c_str();
		LOG_PRINT("shaderStrings[0]:\n%s\n", shaderStrings[0]);
#if 0
	STX_Service::WriteTxtFile("./vsText.txt", vsText.c_str());
	STX_Service::WriteTxtFile("./mainvs.txt", vsName);
#elif 0
    std::ofstream out("./vsText.txt");
    out << "\nvsText:\n";
    out << vsText.c_str();
    out << "\nmainvs:\n";
    out << vsName;
    out.close();
#endif
		glShaderSourceARB(shaderGL1_1.shader[eVertexShader], strIndex, shaderStrings, 0);
				checkGlError("");
				glCompileShaderARB(shaderGL1_1.shader[eVertexShader]);
				checkGlError("");
				glGetObjectParameterivARB(shaderGL1_1.shader[eVertexShader], GL_OBJECT_COMPILE_STATUS_ARB, &vsResult);
				checkGlError("");
LOG_FNLN;
#ifndef _MSC_VER
			{
            			char* s=new char(32768);
				s[0]='\0';
            			glGetInfoLogARB(shaderGL1_1.shader[eVertexShader],32768,NULL,s);
            			if(s) if(stx_strlen(s)) printf("Compile Log: \n%s\n%s\n", vsText.c_str(), s);
            			delete[] s;
			}
#endif
				if (vsResult)
				{
LOG_FNLN;
			glAttachObjectARB(shaderGL1_1.program, shaderGL1_1.shader[eVertexShader]);
				checkGlError("");
				}
				else
				{
					char log[256];
    					glGetInfoLogARB( shaderGL1_1.shader[eVertexShader], 256, NULL, log);
					printf("Vertex shader error:\n%s\n", log);
				}
				//glGetInfoLogARB(shaderGL1_1.shader[eVertexShader], sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
				checkGlError("");
				infoLogPos += len;
				//printf("\n%s\n", infoLog);
			}
			else vsResult = GL_TRUE;
			#if 0
			if(0){
				GLint blen = 0;	
				GLsizei slen = 0;
				glGetShaderiv(shaderGL1_1.shader[eVertexShader], GL_INFO_LOG_LENGTH , &blen);       
				if (blen > 1)
				{
 					GLchar* compiler_log = (GLchar*)malloc(blen);
 					glGetInfoLogARB(shaderGL1_1.shader[eVertexShader], blen, &slen, compiler_log);
 					printf("\n%s\n", compiler_log);
 					free (compiler_log);
				}
			}else if(0){
				char infobuffer[1000];
				GLsizei infobufferlen = 0;
				glGetInfoLogARB(shaderGL1_1.shader[eVertexShader], 999, &infobufferlen, infobuffer);
				if (infobufferlen != 0) {
					infobuffer[infobufferlen] = 0;
					printf("vertexShader reports: %s \n", infobuffer);
					return -1;
				}
			}else if(0){
			GLint length=0;
			glGetShaderiv(shaderGL1_1.shader[eVertexShader],GL_INFO_LOG_LENGTH,&length);
			std::vector<unsigned char> log(length);
			glGetShaderInfoLog(shaderGL1_1.shader[eVertexShader],200,&length,&log[0]);log[length]='\0';
			if(length>1) printf("%s\n",&log[0]);
			}
			#endif
			if (fsText.length())
			{
			shaderGL1_1.shader[ePixelShader] = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
			const GLcharARB *shaderStrings[6];
			int strIndex = 0;
			shaderStrings[strIndex++] = fsText.c_str();
			LOG_PRINT("shaderStrings[0]:\n%s\n", shaderStrings[0]);
#if 0
	STX_Service::WriteTxtFile("./fsText.txt", fsText.c_str());
	STX_Service::WriteTxtFile("./mainps.txt", psName);
#elif 0
    std::ofstream out("./fsText.txt");
    out << "\nfsText:\n";
    out << fsText.c_str();
    out << "\nmainps:\n";
    out << psName;
    out.close();
#endif
			glShaderSourceARB(shaderGL1_1.shader[ePixelShader], strIndex, shaderStrings, 0);
				checkGlError("");
				glCompileShaderARB(shaderGL1_1.shader[ePixelShader]);
				checkGlError("");
				glGetObjectParameterivARB(shaderGL1_1.shader[ePixelShader], GL_OBJECT_COMPILE_STATUS_ARB, &fsResult);
				checkGlError("");
LOG_FNLN;
#ifndef _MSC_VER
			{
            			char* s=new char(32768);
				s[0]='\0';
            			glGetInfoLogARB(shaderGL1_1.shader[ePixelShader],32768,NULL,s);
            			if(s) if(stx_strlen(s)) printf("Compile Log: \n%s\n%s\n", fsText.c_str(), s);
            			delete[] s;
			}
#endif
				if (fsResult)
				{
LOG_FNLN;
					glAttachObjectARB(shaderGL1_1.program, shaderGL1_1.shader[ePixelShader]);
				checkGlError("");
				}
				else
				{
					char log[256];
    					glGetInfoLogARB( shaderGL1_1.shader[ePixelShader], 256, NULL, log);
					printf("Pixel shader error:\n%s\n", log);
				}
				//glGetInfoLogARB(shaderGL1_1.shader[ePixelShader], sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
				checkGlError("");
				infoLogPos += len;
			}
			else fsResult = GL_TRUE;
#if 0
			if(0){
				GLint blen = 0;	
				GLsizei slen = 0;
				glGetShaderiv(shaderGL1_1.shader[ePixelShader], GL_INFO_LOG_LENGTH , &blen);       
				if (blen > 1)
				{
 					GLchar* compiler_log = (GLchar*)malloc(blen);
 					glGetInfoLogARB(shaderGL1_1.shader[ePixelShader], blen, &slen, compiler_log);
 					printf("\n%s\n", compiler_log);
 					free (compiler_log);
				}
			}else if(0){
				char infobuffer[1000];
				GLsizei infobufferlen = 0;
				glGetInfoLogARB(shaderGL1_1.shader[ePixelShader], 999, &infobufferlen, infobuffer);
				if (infobufferlen != 0) {
					infobuffer[infobufferlen] = 0;
					printf("vertexShader reports: %s \n", infobuffer);
					return -1;
				}
			}else if(0){
			GLint length=0;
			glGetShaderiv(shaderGL1_1.shader[ePixelShader],GL_INFO_LOG_LENGTH,&length);
			std::vector<unsigned char> log(length+1);
			glGetShaderInfoLog(shaderGL1_1.shader[ePixelShader],200,&length,&log[0]);log[length]='\0';
			if(length>1) printf("%s\n",&log[0]);
			}
#endif
LOG_FNLN;
if (vsResult)
    {LOG_PRINT("vsResult=true");}
if (fsResult)
    {LOG_PRINT("fsResult=true");}
			if (1)//(vsResult && fsResult)
			{
LOG_FNLN;
			glLinkProgramARB(shaderGL1_1.program);
				checkGlError("");
#ifndef _MSC_VER
			{
	    			char* s=new char(32768);
				s[0]='\0';
            			glGetInfoLogARB(shaderGL1_1.program,32768,NULL,s);
            			if(s) if(stx_strlen(s)) printf("Link Log: \nvs:\n%s\nfs:\n%s\n%s\n", vsText.c_str(), fsText.c_str(), s);
            			delete[] s;
			}
#endif
				glGetObjectParameterivARB(shaderGL1_1.program, GL_OBJECT_LINK_STATUS_ARB, &linkResult);
				checkGlError("");
				glGetInfoLogARB(shaderGL1_1.program, sizeof(infoLog) - infoLogPos, &len, infoLog + infoLogPos);
				checkGlError("");
				infoLogPos += len;
LOG_FNLN;
			if (linkResult)
				{
LOG_FNLN;
			GLuint currProgram =
#ifdef __APPLE__
handle_to_uint((currentShader == SHADER_NONE)? 0 : shaders[currentShader].program);
#else
(currentShader == SHADER_NONE)? 0 : (shaders[currentShader]).program;
#endif

#ifdef __APPLE__
glUseProgram(handle_to_uint(shaderGL1_1.program));
#else
glUseProgram(shaderGL1_1.program);
#endif
				checkGlError("");
					GLint uniformCount, maxLength;
			GLint numActiveAttribs,maxAttribNameLength = 0;
			glGetObjectParameterivARB(shaderGL1_1.program, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &uniformCount);
				checkGlError("");
					glGetObjectParameterivARB(shaderGL1_1.program, GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &maxLength);
				checkGlError("");
			shaderGL1_1.samplers.resize(uniformCount);
			shaderGL1_1.uniforms.resize(uniformCount);
			//shaderGL1_1.attribs.resize(uniformCount);
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
glGetActiveUniform(handle_to_uint(shaderGL1_1.program), i, maxLength, &length, &size, &type, name);
#else
glGetActiveUniform(shaderGL1_1.program, i, maxLength, &length, &size, &type, name);
#endif
				checkGlError("");
						if (type >= GL_SAMPLER_1D && type <= GL_SAMPLER_2D_RECT_SHADOW_ARB)
						{
#ifdef __APPLE__
GLint location = glGetUniformLocation(handle_to_uint(shaderGL1_1.program), name);
#else
GLint location = glGetUniformLocation(shaderGL1_1.program, name);
#endif
				checkGlError("");
							glUniform1i(location, nSamplers);
				checkGlError("");
						shaderGL1_1.samplers[nSamplers].location = location;
						shaderGL1_1.samplers[nSamplers].unit=nSamplers;
						shaderGL1_1.samplers[nSamplers].name=std::string( name);
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
shaderGL1_1.uniforms[nUniforms].location = glGetUniformLocation(handle_to_uint(shaderGL1_1.program), name);
#else
shaderGL1_1.uniforms[nUniforms].location = glGetUniformLocation(shaderGL1_1.program, name);
#endif
				checkGlError("");
									shaderGL1_1.uniforms[nUniforms].type = getConstantType(type);
									shaderGL1_1.uniforms[nUniforms].nElements = size;
									shaderGL1_1.uniforms[nUniforms].name=std::string(name);									stx_Variables::AddUniform(shaders.size(), name, size, getConstantType(type)); // ???
									nUniforms++;
								}
								else if (bracket != 0 && bracket[1] > '0')
								{
									*bracket = '\0';
									for (int i = nUniforms - 1; i >= 0; i--)
									{
										if (strcmp(shaderGL1_1.uniforms[i].name.c_str(), name) == 0)
										{
											int index = atoi(bracket + 1) + 1;
											if (index > shaderGL1_1.uniforms[i].nElements)
											{
												shaderGL1_1.uniforms[i].nElements = index;
											}
										}
									}
								}
							}
						}
					}
			glUseProgram(currProgram);
				checkGlError("");

	if(1){
	if(m_bDebug){LOG_PRINT("glUseProgram(%d);\n", currProgram);}
	}
    LOG_PRINT("\ninfoLog:\n%s\n", infoLog);

			shaderGL1_1.samplers.resize(nSamplers);
			shaderGL1_1.uniforms.resize(nUniforms);
			//shaderGL1_1.attribs.resize(nAttribs);
			if(shaderGL1_1.samplers.size()>1) std::sort(shaderGL1_1.samplers.begin(), shaderGL1_1.samplers.end(), samplerCompGL2std);
			if(shaderGL1_1.uniforms.size()>1) std::sort(shaderGL1_1.uniforms.begin(), shaderGL1_1.uniforms.end(), constantCompGL2std);
			for (int i = 0; i < nUniforms; i++)
					{
						int constantSize = constantTypeSizes[shaderGL1_1.uniforms[i].type] * shaderGL1_1.uniforms[i].nElements;
						shaderGL1_1.uniforms[i].data.resize(constantSize);
						stx_memset((void*)&shaderGL1_1.uniforms[i].data[0], 0, constantSize);
						shaderGL1_1.uniforms[i].dirty = false;
					}
LOG_FNLN;
			shaders.push_back(shaderGL1_1);
LOG_FNLN;
LOG_PRINT("shaders.size()=%d\n", shaders.size());
//stx_exit(0);
			return shaders.size()-1;
				}
			}
			return SHADER_NONE;
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
	const char* def="#define WSIGN +\n#define ROW_MAJOR\n#define MVPSEMANTIC\n#define fract frac\n#define mix lerp\n#define atan(x,y) atan2(y,x)\n";
#if 0
	const char* defvs="#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) texture2Dlod\(TEX, TEXCOORD, 0.0\)\)\n#define SAMPLER2D sampler2D\n";
#elif 0
	const char* defvs="#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) tex2Dlod\(TEX\, float4\(TEXCOORD\.x\, TEXCOORD\.y\, 0.0\, 0.0\)\)\n#define SAMPLER2D sampler2D\n";
#else
	const char* defvs="#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) tex2Dlod\(TEX\, float4\(TEXCOORD\.x\, TEXCOORD\.y\, 0.0\, 0.0\)\)\n#define SAMPLER2D sampler2D\n";
#endif
	const char* deffs="float3 GammaCorrect3(float3 aColor)\n{\n\treturn aColor;\n}\nfloat4 GammaCorrect4(float4 aColor)\n{\n\treturn aColor;\n}\n";
	std::string vsText=def;
	std::string fsText=def;
    vsText.append(defvs);
    fsText.append(deffs);
	if(vsText_) vsText.append(vsText_);
	if(fsText_) fsText.append(fsText_);
	std::string vsText__;
	std::string fsText__;
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

#if 0
	{FILE * pFile = fopen ("shd.txt","w");
	fprintf (pFile, "%s\n\%s\n", vsText__.c_str(), fsText__.c_str());
   	fclose (pFile);}
#endif
		id=addGLSLShaderVrtl(   vsText__.c_str(), gsText, fsText__.c_str(), csText, hsText, dsText,
                            vsMain, gsMain, fsMain, csMain, hsMain, dsMain,
                            flags);
	}
LOG_FNLN;
#else
LOG_FNLN;
		id=addGLSLShaderVrtl(   vsText.c_str(), gsText, fsText.c_str(), csText, hsText, dsText,
                            vsMain, gsMain, fsMain, csMain, hsMain, dsMain,
                            flags);
LOG_FNLN;
#endif

LOG_FNLN;
LOG_PRINT("shaders.size()=%d\n", shaders.size());
LOG_PRINT("id=%x\n", id);
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

	printf("GL_VERSION=%s\n", (const char *) glGetString(GL_VERSION));
	printf("GL_VENDOR=%s\n", (const char *) glGetString(GL_VENDOR));
	printf("GL_RENDERER=%s\n", (const char *) glGetString(GL_RENDERER));
	printf("GL_SHADING_LANGUAGE_VERSION=%s\n", (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION));

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

