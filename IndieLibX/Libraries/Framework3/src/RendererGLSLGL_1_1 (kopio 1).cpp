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
#else
#define LOG_FNLN_OSX
#define LOG_PRINT_OSX
#endif

#if 1
#define STX_PRINT
#define STX_FNLN
#define LOG_PRINT
#define LOG_FNLN
#define LOG_FNLN_NONE
#define LOG_PRINT_NONE
#define LOG_FNLN_X
#define LOG_PRINT_X
#elif 0
#define STX_PRINT(...) printf(__VA_ARGS__);
#define STX_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#define LOG_PRINT(...) printf(__VA_ARGS__);
#define LOG_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#define LOG_FNLN_NONE
#define LOG_PRINT_NONE
#define LOG_FNLN_X
#define LOG_PRINT_X
#else
#define LOG_PRINT(...) printf(__VA_ARGS__);
#define LOG_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
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

	if(1){
	if(m_bDebug)
	{
		LOG_FNLN;
		LOG_PRINT("glDrawArrays:\n");
		LOG_PRINT("PrimitiveType=%d\n", PrimitiveType);
		LOG_PRINT("glPrimRendererGL_1_1[PrimitiveType]=%d\n", glPrimRendererGLSLGL_1_1[PrimitiveType]);
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
	unsigned int indexSize=(indexBuffers[selectedIndexBuffer]).indexSize;


	if(1){
	if(m_bDebug)
	{
		LOG_FNLN;
		LOG_PRINT("glDrawElements:\n");
		LOG_PRINT("PrimitiveType=%x\n", PrimitiveType);
		LOG_PRINT("glPrimRendererGL_1_1[PrimitiveType]=%x\n", glPrimRendererGLSLGL_1_1[PrimitiveType]);
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
	                      &sfize, &type, name);
				checkGlError("");
	    //LOG_PRINT_NONE("Vertex input attribute %s of type %d size %d is at location %d, stride %d\n", name, type, size, glGetAttribLocation(prog, name),stride);
	  }
	  free(name);
	}
#endif

void RendererGLSLGL_1_1::applyTextures(){
		#ifdef __X7__
		return;
		#endif
	for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++){
		TextureID texture = selectedTextures[i];
		TextureID currTex = currentTextures[i];
		#if defined(LINUX)
		if (texture != currTex)
		#endif
		{
		LOG_FNLN;
			glActiveTextureARB(GL_TEXTURE0 + i);
			if (texture == TEXTURE_NONE){
				glDisable(textures[currTex].glTarget);
				glBindTexture(textures[currTex].glTarget, 0);
	LOG_PRINT("glBindTexture(%x, %d);\n", textures[currTex].glTarget, 0);
			} else {
				if (currTex == TEXTURE_NONE){
					glEnable(textures[texture].glTarget);
				} else if (textures[texture].glTarget != textures[currTex].glTarget){
					glDisable(textures[currTex].glTarget);
					glEnable(textures[texture].glTarget);
				}
				glBindTexture(textures[texture].glTarget, textures[texture].glTexID);
            LOG_PRINT("glBindTexture(%x, %d);\n", textures[texture].glTarget, textures[texture].glTexID);
				if (textures[texture].lod != textureLod[i]){
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

	LOG_PRINT("GL_VERSION=%s\n", (const char *) glGetString(GL_VERSION));
	LOG_PRINT("GL_VENDOR=%s\n", (const char *) glGetString(GL_VENDOR));
	LOG_PRINT("GL_RENDERER=%s\n", (const char *) glGetString(GL_RENDERER));
	LOG_PRINT("GL_SHADING_LANGUAGE_VERSION=%s\n", (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION));

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
				glDeleteObjectARB((shaders[i]).GeometryShader);
				checkGlError("");
				glDeleteObjectARB((shaders[i]).HullShader);
				checkGlError("");
				glDeleteObjectARB((shaders[i]).DomainShader);
				checkGlError("");
				glDeleteObjectARB((shaders[i]).ComputeShader);
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

