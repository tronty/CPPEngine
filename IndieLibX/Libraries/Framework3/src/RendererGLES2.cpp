/* * * * * * * * * * * * * Author's note * * * * * * * * * * * ***
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

#include <Framework3/RendererGLES2.h>
#ifdef  __FrameworkGLES2_H__
#include <Framework3/Hlsl2Glsl_Translate.h>
#if 0
#include <Framework3/Glsl_Translate.h>
#endif
#include <Framework3/GLShaders.h>

typedef GLvoid (APIENTRY *UNIFORM_FUNC)(GLint location, GLsizei count, const void *value);
typedef GLvoid (APIENTRY *UNIFORM_MAT_FUNC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define LOG_FNLN_X
#define LOG_PRINT_X

		void RendererGLES2::changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, const intptr offset)
		{
			VertexFormatGL cvf = vertexFormats[selectedVertexFormat];
			char *base = (char *)vertexBuffers[vertexBuffer].data;

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
LOG_FNLN_X;
LOG_PRINT_X("glVertexAttribPointer:\n");
LOG_PRINT_X("index=%d\n",ii);
LOG_PRINT_X("size=%d,%d\n",i, cvf.generic[i].size);
LOG_PRINT_X("type=%d\n",glTypes[cvf.generic[i].format]);
LOG_PRINT_X("stride=%d\n", vertexSize);
LOG_PRINT_X("base=%x\n", base);
LOG_PRINT_X("pointer=%x\n", base + cvf.generic[i].offset);

							checkGlError("");
							ii--;
						}
					}
				}

				currentVertexBuffers[stream] = vertexBuffer;

				currentOffsets[stream] = offset;

				activeVertexFormat[stream] = currentVertexFormat;
			}
		}
void RendererGLES2::applyConstants()
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

					LOG_PRINT_X("uni->name:%s\n",uni->name.c_str());
					LOG_PRINT_X("uni->type:%d\n",uni->type);
					LOG_PRINT_X("uni->location:%d\n",uni->location);
					LOG_PRINT_X("uni->nElements:%d\n",uni->nElements);
					LOG_PRINT_X("uni->data:%x\n",reinterpret_cast<BYTE*>(&uni->data[0]));

					((UNIFORM_MAT_FUNC) uniformFuncs[(uni)->type])((uni)->location, (uni)->nElements, GL_FALSE, reinterpret_cast<float*>(&uni->data[0]));
					checkGlError("");

				}
				else
				{
					LOG_PRINT_X("uni->name:%s:%s:%d\n",uni->name.c_str(),__FUNCTION__,__LINE__);
					((UNIFORM_FUNC) uniformFuncs[(uni)->type])((uni)->location, (uni)->nElements, reinterpret_cast<float*>(&uni->data[0]));
					checkGlError("");
				}
				(uni)->dirty = false;
			}
		}
	}
}

int RendererGLES2::getSamplerIndex(ShaderID shd, const char* textureName)
{
	if((shd<0)||(shd>(shaders.size()-1)))
		return -1;
	return getSamplerUnit(shd, textureName, shaders[shd].samplers);
}

void RendererGLES2::setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState, const int vtstream, const VertexBufferID vtbuffer)
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
unsigned int RendererGLES2::DrawPrimitive(Primitives PrimitiveType,UINT StartVertex,UINT PrimitiveCount, const unsigned int flags)
		{
		//if(!((currentVertexBuffers[0]>-1)))
		if(!((selectedVertexBuffers[0]>-1)))
			return 0;
			apply();
			if(glPrim[PrimitiveType])
			{
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
			return 0;
		}
		unsigned int RendererGLES2::DrawIndexedPrimitive(Primitives PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, const unsigned int flags)
		{
//if(!((currentVertexBuffers[0]>-1)&&(currentIndexBuffer>-1)))
if(!((selectedVertexBuffers[0]>-1)&&(selectedIndexBuffer>-1)))
			return 0;
			unsigned short* usd=(unsigned short*) indexBuffers[selectedIndexBuffer].data;
			unsigned int  * uid=(unsigned int*)   indexBuffers[selectedIndexBuffer].data;

			apply();

			if(glPrim[PrimitiveType])
			{
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
			return 0;
		}
void RendererGLES2::changeShader(const ShaderID shader)
		{
			//if (shader != currentShader)
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


#if defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
glUseProgram(handle_to_uint(&shaders[shader].program));
#elif defined(__APPLE__)
glUseProgram(handle_to_uint(&shaders[shader].program));
#else
glUseProgram((shaders[shader]).program);
#endif


					checkGlError("");
					//LOG_PRINT_X("shader %d:glUseProgramObject(%d)\n",shader,(shaders[shader]).program);
				}
				currentShader = shader;
			}
		}
void RendererGLES2::setShaderConstantRaw(const char *name, const void *data, const int size)
		{

LOG_PRINT_X("name:%s\n", name);
LOG_PRINT_X("data:%x\n", data);
LOG_PRINT_X("size:%d\n", size);

			if(selectedShader==-1)return;
			{

LOG_PRINT_X("shaders.size():%d\n", shaders.size());
LOG_PRINT_X("shaders[%d].nUniforms:%d\n", selectedShader, shaders[selectedShader].uniforms.size());
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
		int samplerCompareGLES(const void *sampler0, const void *sampler1)
		{
			return strcmp(((XSampler *) sampler0)->name.c_str(), ((XSampler *) sampler1)->name.c_str());
		}

		bool samplerCompGLESstd(const XSampler & s0, const XSampler &s1)
		{
       if(!(s0.name.c_str()&& s1.name.c_str()))return false;
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
		}
		bool constantCompGLESstd(const ConstantShaderGLSLGL3 & s0, const ConstantShaderGLSLGL3 &s1)
		{
       if(!(s0.name.c_str()&& s1.name.c_str()))return false;
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
		}

		ShaderID RendererGLES2::addGLSLShader(
const char *vsText, const char *gsText, const char *fsText, const char *csText, const char *hsText, const char *dsText,
const char *vsMain, const char *gsMain, const char *fsMain, const char *csMain, const char *hsMain, const char *dsMain, 
                                            	const unsigned int flags)
		{
#if defined(SDL_2_0_5_BUILD)
SDL_GL_MakeCurrent(STX_Service::GetSDLWindow(), STX_Service::GetSDLContext());
#endif

	std::string vs;
	std::string fs;

	const char* def="\n";
	if (def) vs.append(def);
	if (def) fs.append(def);
	if (vsText) vs.append(vsText);
	if (fsText) fs.append(fsText);

	ShaderGLSLGL3 shaderGLES2;
#if defined(USE_HLSL_SHADERS)
/* ???
	ETargetGLSL_ES_100,	 // OpenGL ES 2.0
	ETargetGLSL_110,	 // OpenGL 2.0
	ETargetGLSL_120,
	ETargetGLSL_140,	 // OpenGL 3.1
	ETargetGLSL_ES_300,	 // OpenGL ES 3.0
*/
	const ETargetVersion version=ETargetGLSL_ES_100;
	ShHandle parser;
	std::string vstext;
	bool ret=Hlsl2Glsl_Translate_ (true, vs, vstext, vsName, parser, version);
	std::string fstext;
	     ret=Hlsl2Glsl_Translate_ (false, fs, fstext, psName, parser, version);
        shaderGLES2.program = compileGLShaders(vstext.c_str(), fstext.c_str());
#else
	shaderGLES2.program = compileGLShaders(vs.c_str(), fs.c_str());
#endif
	reflectGLShader(shaderGLES2.program, shaderGLES2.samplers, shaderGLES2.uniforms);

	shaders.push_back(shaderGLES2);
	return shaders.size()-1;
}

		ShaderID RendererGLES2::addHLSLShaderVrtl(
const char *vsText0, const char *gsText, const char *fsText0, const char *csText, const char *hsText, const char *dsText,
const char *vsMain, const char *gsMain, const char *fsMain, const char *csMain, const char *hsMain, const char *dsMain, 
                                            	const unsigned int flags)
		{
    const char* header=0;
#if 0
	std::string vsText;
	std::string fsText;
	const char * def="#define WSIGN +\n#define ROW_MAJOR\n#define MVPSEMANTIC\n#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) texture2Dlod\(TEX, TEXCOORD, 0.0\)\)\n#define SAMPLER2D sampler2D\n";
	if (def) vsText.append(def);
	if (def) fsText.append(def);
#if defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
	const char * def2="#ifdef GL_ES\nprecision mediump float;\n#endif\n";
	vsText.append(def2);
	fsText.append(def2);
#endif
	if (header) vsText.append(header);
	if (vsText0) vsText.append(vsText0);
	if (header) fsText.append(header);
	if (fsText0) fsText.append(fsText0);
	//LOG_PRINT_X("\nvs:\n%s\n", vsText.c_str());
	//LOG_PRINT_X("\nfs:\n%s\n", fsText.c_str());

	ShaderID id=-1;

		id=addGLSLShader(
			vsText.c_str(),
			0,
			fsText.c_str(),
		vsName, psName,
		vsLine, gsLine, fsLine,
		header, extra, fileName, attributeNames, nAttributes, flags);

		return id;
#else
	const char* def="#define WSIGN +\n#define ROW_MAJOR\n#define MVPSEMANTIC\n#define fract frac\n#define mix lerp\n#define atan(x,y) atan2(y,x)\n";
	std::string vs=def;
	std::string fs=def;

	const char* deffs="float3 GammaCorrect3(float3 aColor)\n{\n\treturn aColor;\n}\nfloat4 GammaCorrect4(float4 aColor)\n{\n\treturn aColor;\n}\n";
#if 0
	const char* defvs="#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) texture2Dlod\(TEX, TEXCOORD, 0.0\)\)\n#define SAMPLER2D sampler2D\n";
#else
	const char* defvs="#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) tex2Dlod\(TEX\, float4\(TEXCOORD\.x\, TEXCOORD\.y\, 0.0\, 0.0\)\)\n#define SAMPLER2D sampler2D\n";
#endif
	vs.append(defvs);
	fs.append(deffs);
	if (vsText0) vs.append(vsText0);
	if (fsText0) fs.append(fsText0);

	ShaderGLSLGL3 shaderGLES2;
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
	const ETargetVersion version=ETargetGLSL_ES_100;
	ShHandle parser;
	std::string vstext;
	bool ret=Hlsl2Glsl_Translate_ (true, vs, vstext, vsName, parser, version);
	std::string fstext;
	     ret=Hlsl2Glsl_Translate_ (false, fs, fstext, psName, parser, version);
        shaderGLES2.program = compileGLShaders(vstext.c_str(), fstext.c_str());
#else
	shaderGLES2.program = compileGLShaders(vs.c_str(), fs.c_str());
#endif
        reflectGLShader(shaderGLES2.program, shaderGLES2.samplers, shaderGLES2.uniforms);

	shaders.push_back(shaderGLES2);
	return shaders.size()-1;
#endif
}

void RendererGLES2::applyTextures(){
	for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++){
		TextureID texture = selectedTextures[i];
		TextureID currTex = currentTextures[i];

		//if (texture != currTex)
		{
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

		RendererGLES2::RendererGLES2(bool fullScreen,
int argc, char *argv[]
) : mOk(false), RendererGLES(fullScreen,argc,argv)
{
    	width=STX_Service::GetWindowInstance()->Getwidth();
    	height=STX_Service::GetWindowInstance()->Getheight();
	//std::string str=miscParams["window"]
	SHADING_LANGUAGE_VERSION_MAJOR=3;
	SHADING_LANGUAGE_VERSION_MINOR=3;
//


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

	hDc = GetDC( STX_Service::GetWindowInstance()->GetHWND() );
	PixelFormat = ChoosePixelFormat( m_hDC, &pfd );
	SetPixelFormat( m_hDC, PixelFormat, &pfd);

	hGlrc = wglCreateContext( hDc);

	wglMakeCurrent( m_hDC, hGlrc );
#endif
			GLint units = 1;
#if !defined(USE_GLES_SHADERS) && !defined(SDL_2_0_5_BUILD)
			{
				LOG_PRINT_X("glGetIntegerv:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	  			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &units);
   checkGlError("");
			}
#endif
			nImageUnits = units;
			stx_memset(&textureLod, 0, sizeof(textureLod));
#if !defined(USE_GLES_SHADERS) && !defined(SDL_2_0_5_BUILD)
			if (GL_ARB_draw_buffers_supported)
			{
				GLint mrt = 1;
				LOG_PRINT_X("glGetIntegerv:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	  			glGetIntegerv(GL_MAX_DRAW_BUFFERS, &mrt);
   checkGlError("");
				nMRTs = mrt;
			}
#endif
			if (nMRTs > MAX_MRTS) nMRTs = MAX_MRTS;
			for (unsigned int i = 0; i < nMRTs; i++)
			{
				drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
			}
			#ifndef ANDROID
			if (GL_EXT_texture_filter_anisotropic_supported)
			{
				GLint aniso = 1;
				LOG_PRINT_X("glGetIntegerv:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
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
		RendererGLES2::~RendererGLES2()
		{
			//STX_TRY;
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
#if !defined(USE_GLES_SHADERS) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
				LOG_PRINT_X("glDeleteObject:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	  			glDeleteObject((shaders[i]).vertexShader);
   				checkGlError("");
				LOG_PRINT_X("glDeleteObject:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	  			glDeleteObject((shaders[i]).fragmentShader);
   				checkGlError("");
				LOG_PRINT_X("glDeleteObject:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	  			glDeleteObject((shaders[i]).program);
   				checkGlError("");
#endif
			}
			for (unsigned int i = 0; i < vertexBuffers.getCount(); i++)
			{
				LOG_PRINT_X("glDeleteBuffers:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	  			glDeleteBuffers(1, &(vertexBuffers[i]).vboVB);
   checkGlError("");
			}
			for (unsigned int i = 0; i < indexBuffers.getCount(); i++)
			{
				LOG_PRINT_X("glDeleteBuffers:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	  			glDeleteBuffers(1, &(indexBuffers[i]).vboIB);
   checkGlError("");
			}
			for (unsigned int i = 0; i < textures.getCount(); i++)
			{

				removeTexture(i);
			}
			if (fbo) {LOG_PRINT_X("glDeleteFramebuffers:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	  			glDeleteFramebuffers(1, &fbo);}
   			checkGlError("");
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
		//STX_CATCH;
		}

		void RendererGLES2::changeIndexBuffer(const IndexBufferID indexBuffer)
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
float	RendererGLES2::GetMaxPointSpriteSize()
{
//
    // If you want to know the std::max size that a point sprite can be set
    // to, do this.
	//

    // Query for the std::max point size supported by the hardware
    float maxSize = 0.0f;
#if !defined(GLES2)
    LOG_PRINT_X("glGetFloatv:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glGetFloatv( GL_POINT_SIZE_MAX, &maxSize );
   checkGlError("");
    LOG_PRINT_X("glPointSize:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glPointSize( maxSize );
   checkGlError("");
#endif
	return maxSize;
}

void	RendererGLES2::BeginPointSpriteRendering()
{
	//
	// Enabling GL_DEPTH_TEST and setting glDepthMask to GL_FALSE makes the
    // Z-Buffer read-only, which helps remove graphical artifacts generated
    // from  rendering a list of particles that haven't been sorted by
    // distance to the eye.
	//
    // Enabling GL_BLEND and setting glBlendFunc to GL_DST_ALPHA with GL_ONE
    // allows particles, which overlap, to alpha blend with each other
    // correctly.
	//

    LOG_PRINT_X("glEnable:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glEnable( GL_DEPTH_TEST );
   checkGlError("");
    LOG_PRINT_X("glDepthMask:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glDepthMask( GL_FALSE );
   checkGlError("");


	LOG_PRINT_X("glEnable:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glEnable( GL_BLEND );
   checkGlError("");
    LOG_PRINT_X("glBlendFunc:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glBlendFunc( GL_DST_ALPHA, GL_ONE );
   checkGlError("");
#if !defined(GLES2)

    //
	// Set up the OpenGL state machine for using point sprites...
	//

    // This is how will our point sprite's size will be modified by
    // distance from the viewer
    float quadratic[] =  { 1.0f, 0.0f, 0.01f };
    LOG_PRINT_X("glPointParameterfvARB:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glPointParameterfv( GL_POINT_DISTANCE_ATTENUATION, quadratic );
   checkGlError("");

    // Query for the std::max point size supported by the hardware
    float maxSize = 0.0f;
    LOG_PRINT_X("glGetFloatv:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glGetFloatv( GL_POINT_SIZE_MAX, &maxSize );
   checkGlError("");

    // Clamp size to 100.0f or the sprites could get a little too big on some
    // of the newer graphic cards. My ATI card at home supports a std::max point
    // size of 1024.0f!
    if( maxSize > 100.0f )
        maxSize = 100.0f;

    LOG_PRINT_X("glPointSize:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glPointSize( maxSize );
   checkGlError("");//in OTL 10 ???

    // The alpha of a point is calculated to allow the fading of points
    // instead of shrinking them past a defined threshold size. The threshold
    // is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to
    // the minimum and maximum point sizes.
    LOG_PRINT_X("glPointParameterfARB:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glPointParameterf( GL_POINT_FADE_THRESHOLD_SIZE, 60.0f );
   checkGlError("");

    LOG_PRINT_X("glPointParameterfARB:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glPointParameterf( GL_POINT_SIZE_MIN, 1.0f );
   checkGlError("");
    LOG_PRINT_X("glPointParameterfARB:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glPointParameterf( GL_POINT_SIZE_MAX, maxSize );
   checkGlError("");

    // Specify point sprite texture coordinate replacement mode for each
    // texture unit
    LOG_PRINT_X("glTexEnvf:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glTexEnvf( GL_POINT_SPRITE_OES, GL_COORD_REPLACE, GL_TRUE );
   checkGlError("");

    //
	// Render point sprites...
	//
//???if defined(OS_IPHONE)
    LOG_PRINT_X("glEnable:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glEnable( GL_POINT_SPRITE_OES );
   checkGlError("");
#endif
}

void	RendererGLES2::EndPointSpriteRendering()
{
    //
    // Reset OpenGL states...
	//

    LOG_PRINT_X("glDepthMask:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glDepthMask( GL_TRUE );
   checkGlError("");
    LOG_PRINT_X("glDisable:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glDisable( GL_BLEND );
   checkGlError("");
#if !defined(GLES2)
	LOG_PRINT_X("glDisable:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	glDisable( GL_POINT_SPRITE_OES );
   checkGlError("");
#endif
}
#endif
