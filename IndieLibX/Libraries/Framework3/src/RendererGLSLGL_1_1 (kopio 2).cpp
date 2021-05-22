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

#include <Framework3/RendererGLSLGL_1_1.h>
#ifdef __FrameworkGLSLGL_1_1_H__
#include <Framework3/Hlsl2Glsl_Translate.h>
#if 0
#include <Framework3/Glsl_Translate.h>
#endif
#include <Framework3/GLShaders.h>

typedef GLvoid (APIENTRY *UNIFORM_FUNC)(GLint location, GLsizei count, const void *value);
typedef GLvoid (APIENTRY *UNIFORM_MAT_FUNC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

#define LOG_FNLN_X
#define LOG_PRINT_X

#if 1
#define STX_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#define STX_PRINT(...) printf(__VA_ARGS__);
#define LOG_FNLN
#define LOG_PRINT
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

		void RendererGLSLGL_1_1::changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, const intptr offset)
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

void RendererGLSLGL_1_1::applyConstants(){
	if (currentShader != SHADER_NONE){
		for (unsigned int i = 0; i < shaders[currentShader].uniforms.size(); i++){
			ConstantShaderGLSLGL3* uni = &shaders[currentShader].uniforms[0] + i;
			if (uni->dirty){
				STX_PRINT("name=%s, loc=%d, nElem=%d, data=%x\n", uni->name.c_str(), uni->location, uni->nElements, (void*) &uni->data[0]);
				if (uni->type >= CONSTANT_MAT2){
					if(&uni->data[0])
					((UNIFORM_MAT_FUNC) uniformFuncs[uni->type])(uni->location, uni->nElements, GL_TRUE, (float *) &uni->data[0]);
				} else {
					if(&uni->data[0])
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
	return getSamplerUnit(shd, textureName, shaders[shd].samplers);
}

void RendererGLSLGL_1_1::setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState, const int vtstream, const VertexBufferID vtbuffer)
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
unsigned int RendererGLSLGL_1_1::DrawPrimitive(Primitives PrimitiveType,UINT StartVertex,UINT PrimitiveCount, const unsigned int flags)
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
		unsigned int RendererGLSLGL_1_1::DrawIndexedPrimitive(Primitives PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, const unsigned int flags)
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
void RendererGLSLGL_1_1::changeShader(const ShaderID shader)
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
#if 1
void RendererGLSLGL_1_1::setShaderConstantRaw(const char *name, const void *data, const int size)
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
			if(name)
			if(uni->name==std::string(name))
				uni->dirty = true;
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
#else
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
			if(name)
			if(uni->name==std::string(name))
				uni->dirty = true;
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
#endif
		int samplerCompareGLSL_1_1(const void *sampler0, const void *sampler1)
		{
			return strcmp(((XSampler *) sampler0)->name.c_str(), ((XSampler *) sampler1)->name.c_str());
		}

		bool samplerCompGLSL_1_1std(const XSampler & s0, const XSampler &s1)
		{
       if(!(s0.name.c_str()&& s1.name.c_str()))return false;
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
		}
		bool constantCompGLSL_1_1std(const ConstantShaderGLSLGL3 & s0, const ConstantShaderGLSLGL3 &s1)
		{
       if(!(s0.name.c_str()&& s1.name.c_str()))return false;
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
		}

		ShaderID RendererGLSLGL_1_1::addGLSLShader(
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

	ShaderGLSLGL3 shaderGL_1_1;
#if defined(USE_HLSL_SHADERS)
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
	bool ret=Hlsl2Glsl_Translate_ (true, vs, vs, vsMain, parser, version);
	std::string fstext;
	     ret=Hlsl2Glsl_Translate_ (false, fs, fs, fsMain, parser, version);
        shaderGL_1_1.program = compileGLShaders(vstext.c_str(), fstext.c_str());
#else
	shaderGL_1_1.program = compileGLShaders(vs.c_str(), fs.c_str());
#endif
	reflectGLShader(shaderGL_1_1.program, shaderGL_1_1.samplers, shaderGL_1_1.uniforms);

	shaders.push_back(shaderGL_1_1);
	return shaders.size()-1;
}

		ShaderID RendererGLSLGL_1_1::addHLSLShaderVrtl(
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

	ShaderGLSLGL3 shaderGL_1_1;
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
	bool ret=Hlsl2Glsl_Translate_ (true, vs, vstext, vsMain, parser, version);
	std::string fstext;
	     ret=Hlsl2Glsl_Translate_ (false, fs, fstext, fsMain, parser, version);
        shaderGL_1_1.program = compileGLShaders(vstext.c_str(), fstext.c_str());
#else
	shaderGL_1_1.program = compileGLShaders(vs.c_str(), fs.c_str());
#endif
        reflectGLShader(shaderGL_1_1.program, shaderGL_1_1.samplers, shaderGL_1_1.uniforms);

	shaders.push_back(shaderGL_1_1);
	return shaders.size()-1;
#endif
}

void RendererGLSLGL_1_1::applyTextures(){
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

#if 0
		RendererGLSLGL_1_1::RendererGLSLGL_1_1(bool fullScreen,
int argc, char *argv[]
) : mOk(false), RendererGL_1_1(fullScreen,argc,argv)
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
		RendererGLSLGL_1_1::~RendererGLSLGL_1_1()
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
				LOG_PRINT_X("glDeleteObjectARB:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	  			glDeleteObjectARB((shaders[i]).vertexShader);
   				checkGlError("");
				LOG_PRINT_X("glDeleteObjectARB:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	  			glDeleteObjectARB((shaders[i]).fragmentShader);
   				checkGlError("");
				LOG_PRINT_X("glDeleteObjectARB:%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	  			glDeleteObjectARB((shaders[i]).program);
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
#endif		
#if 0
		void RendererGLSLGL_1_1::changeIndexBuffer(const IndexBufferID indexBuffer)
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
#endif
#endif

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

	hDc = GetDC( STX_Service::GetWindowInstance()->GetHWND() );
	PixelFormat = ChoosePixelFormat( m_hDC, &pfd );
	SetPixelFormat( m_hDC, PixelFormat, &pfd);

	hGlrc = wglCreateContext( hDc);

	wglMakeCurrent( m_hDC, hGlrc );


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

