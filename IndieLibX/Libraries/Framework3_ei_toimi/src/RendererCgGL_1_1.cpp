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

#include <Framework3/RendererCgGL_1_1.h>

#ifdef __FrameworkCgGL_1_1_H__
#include <Framework3/CgReflection.h>
CGcontext RendererCgGL_1_1::m_CgContext;
CGprogram g_programfs;
std::vector<std::string> g_textureNames;
std::vector<TextureID> g_textures;

#define LOG_PRINT_(...) printf(__VA_ARGS__)
#define LOG_FNLN_ printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__)
#define STX_PRINT_(...) printf(__VA_ARGS__)
#define STX_FNLN_ printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__)

  void MyCgErrorCallback( void ){
	const char *situation=0;
  CGerror error;
  const char *string = cgGetLastErrorString(&error);
  
  if (error != CG_NO_ERROR) {
    if (error == CG_COMPILER_ERROR) {
      LOG_PRINT(
             "Error: %s\n\n"
             "Cg compiler output...\n%s", string,
             cgGetLastListing(RendererCgGL_1_1::GetCgContext()));
    } else {
      LOG_PRINT(
              "Error: %s\n", string);
    }
    //stx_exit(1);
  }
  }

struct ShaderCgGL3 : public Shader
{
	void disApplyTextures();
	ShaderCgGL3() : Shader()
    {
        nAttribs=0;
	    nUniforms=0;
        nSamplers=0;
        profile.resize(6);
        program.resize(6);
        mvType.resize(6);
	compiled.push_back(false);
	compiled.push_back(false);
	compiled.push_back(false);
	compiled.push_back(false);
	compiled.push_back(false);
	compiled.push_back(false);
    }
	virtual ~ShaderCgGL3(){}
    ShaderCgGL3(const ShaderCgGL3& rhs)
    {
	compiled=rhs.compiled;
        mvType=rhs.mvType;
	    profile=rhs.profile;
	    program=rhs.program;
	    attribs=rhs.attribs;
	    uniforms=rhs.uniforms;
	    samplers=rhs.samplers;
        nAttribs=rhs.nAttribs;
	    nUniforms=rhs.nUniforms;
        nSamplers=rhs.nSamplers;
    }
#if 0
        std::vector<GLhandleARB> program;
        std::vector<GLhandleARB> shader;
#else
        std::vector<CGprofile*> profile;
        std::vector<CGprogram> program;
#endif
	std::vector<bool> compiled;
        std::vector<ShaderType> mvType;
	    std::vector<AttribShaderCgGL3 > attribs;
	    std::vector<ConstantShaderCgGL3 > uniforms;
        std::vector<SamplerShaderCgGL3  > samplers;
        unsigned int nAttribs;
	    unsigned int nUniforms;
        unsigned int nSamplers;
};
#ifdef _WIN32

#endif
#if 0
typedef GLvoid (APIENTRY *UNIFORM_FUNC)(GLint location, GLsizei count, const void *value);
typedef GLvoid (APIENTRY *UNIFORM_MAT_FUNC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
#else
typedef GLvoid (APIENTRY *UNIFORM_FUNC)(CGparameter param, const float *v);
typedef GLvoid (APIENTRY *UNIFORM_MAT_FUNC)(CGparameter param, const float *matrix);
#endif
//void RendererGL_1_1::
void RendererCgGL_1_1::changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, const intptr offset)
		{
			#if 1
#if 1
            if((selectedShader<0) || (shaders.size()<=selectedShader)) return;
			int minAttrib = 0;
			int maxAttrib = (shaders[selectedShader])->nAttribs - 1;
			const GLsizei glTypes[] =
			{
				GL_FLOAT,
				0, GL_UNSIGNED_BYTE,
			}
			;
			{
			VertexFormatGL cvf = vertexFormats[currentVertexFormat];
			int vertexSize = cvf.vertexSize[stream];
			//LOG_FNLN;
			for (int i = minAttrib; i < maxAttrib; i++)
			{
				int currAttrib = (minAttrib + maxAttrib) >> 1;
				int offset_x=((shaders[selectedShader])->attribs[currAttrib]).offset;
				//LOG_FNLN;
				//LOG_PRINT("stream=%d\n",stream);
				//LOG_PRINT("offset=%d\n",offset_x);
				if (stream==offset_x)//(res == 0)
				{


			AttribShaderCgGL3* att = (&(shaders[selectedShader])->attribs[0]) + currAttrib;

					//LOG_FNLN;
					att->dirty = true;

					return;
				}
			}
			}
			#endif
				//LOG_FNLN;
				if (currentVertexFormat != VF_NONE)
				{
					char *base = (char *) offset;
					VertexFormatGL cvf = vertexFormats[currentVertexFormat];
					int vertexSize = cvf.vertexSize[stream];

					//LOG_FNLN;
					for (int i = minAttrib; i < maxAttrib; i++)
					{
						//LOG_FNLN;
						if ((shaders[selectedShader])->attribs[i].dirty&&(cvf.generic[i].stream == stream && cvf.generic[i].size))
						{
							//LOG_FNLN;
							glVertexAttribPointer(i, cvf.generic[i].size, glTypes[cvf.generic[i].format], GL_TRUE, vertexSize, base + cvf.generic[i].offset);
				checkGlError("");
							glEnableVertexAttribArray (i);
				checkGlError("");
							shaders[selectedShader]->attribs[i].dirty=false;
/*
// Load the vertex position
   glVertexAttribPointer ( att->loc, offset_x, GL_FLOAT,
                           GL_FALSE, vertexSize, vVertices );
   // Load the texture coordinate
   glVertexAttribPointer ( att->loc, offset_x, GL_FLOAT,
                           GL_FALSE, vertexSize, &vVertices[3] );

   glEnableVertexAttribArray ( att->loc );
   glEnableVertexAttribArray ( att->loc );
*/						}
					}
				}

				currentVertexBuffers[stream] = vertexBuffer;
				currentOffsets[stream] = offset;
				activeVertexFormat[stream] = currentVertexFormat;
			#else
			const GLsizei glTypes[] =
			{
				GL_FLOAT,
				0, GL_UNSIGNED_BYTE,
			}
			;
			GLuint vbo = 0;
			if (vertexBuffer != VB_NONE) vbo = (vertexBuffers[vertexBuffer]).vboVB;
			//if (vbo != currentVBO)
			{
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, currentVBO = vbo);
				checkGlError("");
			}
			//if (vertexBuffer != currentVertexBuffers[stream] || offset != currentOffsets[stream] || currentVertexFormat != activeVertexFormat[stream])
			{
				if (currentVertexFormat != VF_NONE)
				{
					char *base = (char *) offset;
					VertexFormatGL cvf = vertexFormats[currentVertexFormat];
					int vertexSize = cvf.vertexSize[stream];
					/*
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
					*/
					for (int i = 0; i < MAX_GENERIC; i++)
					{
						if (cvf.generic[i].stream == stream && cvf.generic[i].size)
						{
							glVertexAttribPointerARB(i, cvf.generic[i].size, glTypes[cvf.generic[i].format], GL_TRUE, vertexSize, base + cvf.generic[i].offset);
				checkGlError("");
							/*{
							GLint loc = glGetAttribLocation ( prog, name);
							glEnableVertexAttribArray (loc);
				checkGlError("");
							}*/
						}
					}
					/*
					for (int i = 0; i < MAX_TEXCOORD; i++)
					{
						if (cvf.texCoord[i].stream == stream && cvf.texCoord[i].size)
						{
							glClientActiveTexture(GL_TEXTURE0 + i);
				checkGlError("");
							glTexCoordPointer(cvf.texCoord[i].size, glTypes[cvf.texCoord[i].format], vertexSize, base + cvf.texCoord[i].offset);
				checkGlError("");
						}
					}*/
				}
				currentVertexBuffers[stream] = vertexBuffer;
				currentOffsets[stream] = offset;
				activeVertexFormat[stream] = currentVertexFormat;
			}
			#endif
		}

void RendererCgGL_1_1::applyConstants()
{
	if(currentShader>=shaders.size()) return;
	if (currentShader != SHADER_NONE)
	{
		for (unsigned int i = 0; i < shaders[currentShader]->nUniforms; i++)
		{
			ConstantShaderCgGL3* uni = (&shaders[currentShader]->uniforms[0]) + i;
			//if ((uni)->dirty) // ???
			{
#if 0
				if ((uni)->type == CG_PARAMETERCLASS_MATRIX)
				{
					#if 0
					((UNIFORM_MAT_FUNC) uniformFuncs[(uni)->type])
					((uni)->index,
					(uni)->nElements, GL_TRUE,
					(float *) (uni)->data);
					#else
					((UNIFORM_MAT_FUNC) uniformFuncs[(uni)->type])
					((uni)->param,
					(float *) (uni)->data);
					#endif
				}
				else
				{
					#if 0
					((UNIFORM_FUNC) uniformFuncs[(uni)->type])
					((uni)->index,
					(uni)->nElements,
					(float *) (uni)->data);
					#else
					//LOG_PRINT("%x %d\n",(uni)->data,(uni)->data);
					((UNIFORM_FUNC) uniformFuncs[(uni)->type])
					(
					(uni)->param,
					(float *) (uni)->data);
					#endif
				}
#endif
#if 1
LOG_FNLN;
LOG_PRINT("uni->type=%x\n", uni->type);
LOG_PRINT("uni->name=%s\n", uni->name.c_str());
LOG_PRINT("uni->index=%d\n", uni->index);
LOG_PRINT("uni->nElements=%x\n", uni->nElements);
LOG_PRINT("uni->data=%x\n", &uni->data[0]);
#endif
				switch(uni->type)
				{
                case CONSTANT_TYPE_COUNT:
                    break;
				case CONSTANT_FLOAT:
				case CONSTANT_INT:
				case CONSTANT_BOOL:
                    if(uni->nElements>1)
                    {LOG_PRINT("cgGLSetParameterArray1f(%s, %x);\n", uni->name.c_str(), &uni->data[0]);
					cgGLSetParameterArray1f(uni->param,0,uni->nElements,(float *) &uni->data[0]);break;}
                    else
                    {LOG_PRINT("cgGLSetParameter1fv(%s, %x);\n", uni->name.c_str(), &uni->data[0]);
					cgGLSetParameter1fv(uni->param,(float *) &uni->data[0]);break;}

				case CONSTANT_VEC2:
				case CONSTANT_IVEC2:
				case CONSTANT_BVEC2:
                    if(uni->nElements>2)
                    {LOG_PRINT("cgGLSetParameterArray2f(%s, %x);\n", uni->name.c_str(), &uni->data[0]);
					cgGLSetParameterArray2f(uni->param,0,uni->nElements/2,(float *) &uni->data[0]);break;}
                    else
                    {LOG_PRINT("cgGLSetParameter2fv(%s, %x);\n", uni->name.c_str(), &uni->data[0]);
					cgGLSetParameter2fv(uni->param,(float *) &uni->data[0]);break;}


				case CONSTANT_VEC3:
				case CONSTANT_IVEC3:
				case CONSTANT_BVEC3:
                    if(uni->nElements>3)
                    {LOG_PRINT("cgGLSetParameterArray3f(%s, %x);\n", uni->name.c_str(), &uni->data[0]);
					cgGLSetParameterArray3f(uni->param,0,uni->nElements/3,(float *) &uni->data[0]);break;}
                    else
                    {LOG_PRINT("cgGLSetParameter3fv(%s, %x);\n", uni->name.c_str(), &uni->data[0]);
					cgGLSetParameter3fv(uni->param,(float *) &uni->data[0]);break;}

				case CONSTANT_VEC4:
				case CONSTANT_IVEC4:
				case CONSTANT_BVEC4:
                    if(uni->nElements>4)
                    {LOG_PRINT("cgGLSetParameterArray4f(%s, %x);\n", uni->name.c_str(), &uni->data[0]);
					cgGLSetParameterArray4f(uni->param,0,uni->nElements/4,(float *) &uni->data[0]);break;}
                    else
                    {LOG_PRINT("cgGLSetParameter4fv(%s, %x);\n", uni->name.c_str(), &uni->data[0]);
					cgGLSetParameter4fv(uni->param,(float *) &uni->data[0]);break;}

				case CONSTANT_MAT2:
				case CONSTANT_MAT3:
					break;
				case CONSTANT_MAT4:
			{
			float* data=(float*)&uni->data[0];
			#if 0
			D3DXFROMWINEMATRIX W;
			D3DXFROMWINEMatrixIdentity(&W);
			data=&W.m[0][0];
			#endif
                    #if 1 // ???
                    if(uni->nElements>16)
                    {LOG_PRINT("cgGLSetMatrixParameterArrayfr(%s, %x);\n", uni->name.c_str(), data);
					cgGLSetMatrixParameterArrayfr(uni->param, 0, uni->nElements, data);}
                    else
                    {LOG_PRINT("cgGLSetMatrixParameterfr(%s, %x);\n", uni->name.c_str(), data);
					cgGLSetMatrixParameterfr(uni->param, data);}
                    #else
                    if(uni->nElements>16)
                    {LOG_PRINT("cgGLSetMatrixParameterArrayfc(%s, %x);\n", uni->name.c_str(), data);
					cgGLSetMatrixParameterArrayfr(uni->param, 0, uni->nElements, data);}
                    else
                    {LOG_PRINT("cgGLSetMatrixParameterfc(%s, %x);\n", uni->name.c_str(), data);
                    cgGLSetMatrixParameterfc(uni->param, data);}
                    #endif
					}break;
				(uni)->dirty = false;
			}}
		}
	}
}

int RendererCgGL_1_1::getSamplerIndex(ShaderID shd, const char* textureName)
{
	if((shd<0)||(shd>(shaders.size()-1)))
		return -1;
	int unit = -1;
	for(unsigned int i=0;i<shaders[shd]->samplers.size();i++)
	{
		if(shaders[shd]->samplers[i].name==textureName)
		{
			unit=shaders[shd]->samplers[i].index;
			break;
		}
	}
	return unit;
}
void RendererCgGL_1_1::setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState, const int vtstream, const VertexBufferID vtbuffer)
{
	if(texture==-1) return;
	if(textureName==0) return;
    g_textureNames.push_back(textureName);
    g_textures.push_back(texture);
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

unsigned int RendererCgGL_1_1::DrawPrimitive(Primitives PrimitiveType,UINT StartVertex,UINT PrimitiveCount, const unsigned int flags)
{
//LOG_FNLN;
			apply();
//LOG_FNLN;
		//LOG_PRINT("=%d\n",getVertexCount(PrimitiveType,PrimitiveCount));
			LOG_PRINT("glDrawArrays\n");
			glDrawArrays(glPrim[PrimitiveType], StartVertex,
#if 0
				PrimitiveCount*getIndicesCount(PrimitiveType)
#else
				getVertexCount(PrimitiveType,PrimitiveCount)
#endif
				);
//LOG_FNLN;
				checkGlError("");
//LOG_FNLN;
			disApplyProfiles();
			disApplyTextures();
//LOG_FNLN;
			reset(flags);
//LOG_FNLN;
			nDrawCalls++;
            return 0;
		}
		unsigned int RendererCgGL_1_1::DrawIndexedPrimitive(Primitives PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, const unsigned int flags)
		{
			//if((currentIndexBuffer<0)||(currentIndexBuffer>=indexBuffers.size()))return;
			apply();
			#if 0
			LOG_PRINT("=%d\n",			getIndicesCount(PrimitiveType));
			LOG_PRINT("=%d\n",			primCount);
			LOG_PRINT("=%d\n",			(indexBuffers[currentIndexBuffer]).indexSize);
			LOG_PRINT("=%d\n",			BUFFER_OFFSET((indexBuffers[currentIndexBuffer]).indexSize * startIndex));
			#endif
			LOG_PRINT("glDrawElements\n");
			glDrawElements(
			glPrim[PrimitiveType],
			getIndicesCount(PrimitiveType)*primCount,
			((indexBuffers[currentIndexBuffer]).indexSize==2)? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
			BUFFER_OFFSET((indexBuffers[currentIndexBuffer]).indexSize * startIndex));
				checkGlError("");
			disApplyProfiles();
			disApplyTextures();
			reset(flags);
			nDrawCalls++;
            return 0;
		}
void RendererCgGL_1_1::changePixelShader(const ShaderID shader)
{
	return;
}
void RendererCgGL_1_1::changeVertexShader(const ShaderID shader)
{
	return;
}
void RendererCgGL_1_1::changeGeometryShader(const ShaderID shader)
{
	return;
}
void RendererCgGL_1_1::changeHullShader(const ShaderID shader)
{
	return;
}
void RendererCgGL_1_1::changeDomainShader(const ShaderID shader)
{
	return;
}
void RendererCgGL_1_1::changeComputeShader(const ShaderID shader)
{
	return;
}

#if 0
void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  cgGLBindProgram(myCgVertexProgram);
  checkForCgError("binding vertex program");

  cgGLEnableProfile(myCgVertexProfile);
  checkForCgError("enabling vertex profile");

  cgGLBindProgram(myCgFragmentProgram);
  checkForCgError("binding fragment program");

  cgGLEnableProfile(myCgFragmentProfile);
  checkForCgError("enabling fragment profile");

  cgGLEnableTextureParameter(myCgFragmentParam_decal);
  checkForCgError("enable decal texture");

  glBegin(GL_TRIANGLES);
    glTexCoord2f(0, 0);
    glVertex2f(-0.8, 0.8);

    glTexCoord2f(1, 0);
    glVertex2f(0.8, 0.8);

    glTexCoord2f(0.5, 1);
    glVertex2f(0.0, -0.8);
  glEnd();


  cgGLDisableProfile(myCgVertexProfile);
  checkForCgError("disabling vertex profile");

  cgGLDisableProfile(myCgFragmentProfile);
  checkForCgError("disabling fragment profile");

  cgGLDisableTextureParameter(myCgFragmentParam_decal);
  checkForCgError("disabling decal texture");

  glutSwapBuffers();
}
#endif

void RendererCgGL_1_1::changeShader(const ShaderID shader)
		{
LOG_FNLN;
			//if (shader != currentShader)
			{
				#if 1
				if(1)
				{
                            LOG_FNLN_;
					for (unsigned int i = 0; i < shaders[shader]->compiled.size(); i++)
					{
						if(shaders[shader]->compiled[i])
						{
                            				//LOG_PRINT_("cgGLLoadProgram[%d]\n", i);
                            				LOG_PRINT_("cgGLBindProgram[%d]\n", i);
                            				LOG_PRINT_("cgGLEnableProfile[%d]\n", i);
							cgGLLoadProgram( shaders[shader]->program[i]);
							cgGLBindProgram( shaders[shader]->program[i] );
							cgGLEnableProfile( *shaders[shader]->profile[i] );
						}
					}
				}
				#endif
				currentShader = shader;
			}
		}

		void RendererCgGL_1_1::resetVrtl(const unsigned int aflags)
		{
			//LOG_FNLN_NONE;
				reset0(aflags);

	#if 1
	unsigned int flags=aflags;
	#else
	unsigned int flags=getResetFlag();
	#endif
	#if 1	
		if (flags & RESET_TEX){
		for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++){
			selectedTextures[i] = TEXTURE_NONE;
		}
	}
	//setResetFlag(0);
	#endif
		}
void RendererCgGL_1_1::setShaderConstantRaw(const char *name, const void *data, const int size)
		{
			if(selectedShader==-1)return;
			int minUniform = 0;
			int maxUniform = shaders[selectedShader]->nUniforms - 1;

			while (minUniform <= maxUniform)
			{
				int currUniform = (minUniform + maxUniform) >> 1;
				int res = strcmp(name, (shaders[selectedShader]->uniforms[currUniform]).name.c_str());
				if (res == 0)
				{


			ConstantShaderCgGL3* uni = (&shaders[selectedShader]->uniforms[0]) + currUniform;

			float m[4][4];
			float d[4][4];

			stx_memcpy(d, data, sizeof(float)*16);
			int i,j;
    for (i=0; i<4; i++)
        for (j=0; j<4; j++)
			m[i][j] =
			d[j][i];

					if (stx_memcmp(&uni->data[0], m, size))
					{
						uni->data.resize(size);
						stx_memcpy((void*)&uni->data[0], m, size);
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

					int samplerCompareGL2Cg(const void *sampler0, const void *sampler1)
		{
			return strcmp(((SamplerShaderCgGL3 *) sampler0)->name.c_str(), ((SamplerShaderCgGL3 *) sampler1)->name.c_str());
		}
		#if 1
		ConstantType getConstantTypeCg(CGtype type)
		{
			switch (type)
			{
				case CG_BOOL	: return CONSTANT_BOOL;
				case CG_INT		: return CONSTANT_INT;
				case CG_HALF	:
				case CG_FLOAT : return CONSTANT_FLOAT;
				case CG_HALF2				:
				case CG_FLOAT2			:
				return CONSTANT_VEC2;
				case CG_HALF3				:
				case CG_FLOAT3			:
				return CONSTANT_VEC3;
				case CG_HALF4				:
				case CG_FLOAT4			:
				return CONSTANT_VEC4;
				case CG_HALF4x4 		:
				case CG_FLOAT4x4		:
                return CONSTANT_MAT4;
                case CG_UNKNOWN_TYPE:
                case CG_STRUCT:
                case CG_ARRAY:
                case CG_TYPELESS_STRUCT:
                case CG_TYPE_START_ENUM:
                case CG_HALF1x1:
                case CG_HALF1x2:
                case CG_HALF1x3:
                case CG_HALF1x4:
                case CG_HALF2x1:
                case CG_HALF2x2:
                case CG_HALF2x3:
                return CONSTANT_INT;//???
			}
			return (ConstantType) -1;
		}
		#endif

		bool samplerCgCompGL2std(const SamplerShaderCgGL3 & s0, const SamplerShaderCgGL3 &s1)
		{
       if(!(s0.name.c_str()&& s1.name.c_str()))return false;
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
		}
		bool attribCgCompGL2std(const AttribShaderCgGL3 & s0, const AttribShaderCgGL3 &s1)
		{
       if(!(s0.name.c_str()&& s1.name.c_str()))return false;
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
		}
		bool constantCgCompGL2std(const ConstantShaderCgGL3 & s0, const ConstantShaderCgGL3 &s1)
		{
       if(!(s0.name.c_str()&& s1.name.c_str()))return false;
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
		}
static void registerLatestProfiles(void);
ShaderID RendererCgGL_1_1::addHLSLShaderVrtl( 	
const char *vsText_, const char *gsText, const char *fsText_, const char *csText, const char *hsText, const char *dsText,
const char *vsMain, const char *gsMain, const char *fsMain, const char *csMain, const char *hsMain, const char *dsMain, 
                                            	const unsigned int flags)
		{
	const char* def = "#define __HLSLCG__ 1\n#define ROW_MAJOR row_major\n#define MVPSEMANTIC\n#define WSIGN +\n";
	const char* deffs = "float3 GammaCorrect3(float3 aColor)\n{\n\treturn aColor;\n}\nfloat4 GammaCorrect4(float4 aColor)\n{\n\treturn aColor;\n}\n";
	const char* defvs = "#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) tex2D\(TEX\, TEXCOORD\)\n#define SAMPLER2D sampler2D\n";
    std::string vsText=def;
    std::string fsText=def;
    vsText.append(defvs);
    fsText.append(deffs);
	if(vsText_) vsText.append(vsText_);
	if(fsText_) fsText.append(fsText_);

	{std::regex e("\\bmatrix\\b");
	vsText=std::regex_replace (vsText,e,"float4x4");}
	{std::regex e("\\bmatrix\\b");
	fsText=std::regex_replace (fsText,e,"float4x4");}

			ShaderCgGL3* shader=new ShaderCgGL3();
			//const GLcharARB *shaderStrings[6];
			int strIndex = 0;
			char line[16];
			GLint vsResult, fsResult, gsResult, hsResult, dsResult;//, linkResult;
			char infoLog[2048];
			GLint infoLogPos = 0;
			if (GL_ARB_shading_language_100_supported)
			{
				static char versionString[16] = {0};
				static bool first = true;
				if (first)
				{
					const char *version = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION_ARB);
					if (version)
					{
						int major = atoi(version);
                        int minor = atoi(stx_strchr(version, '.') + 1);
                        stx_snprintf(versionString,16, "#version %d, %d\n", major, minor);
						LOG_PRINT("%s\n",versionString);
					}
					first = false;
				}
				//shaderStrings[strIndex++] = versionString;
			}

			//???shaderStrings[strIndex++] = line;
			registerLatestProfiles();
#if 0
  CG_PROFILE_GP4FP   = 7010,
  CG_PROFILE_GP4VP   = 7011,
  CG_PROFILE_GP4GP   = 7012,
  CG_PROFILE_GP5FP   = 7017, /* NV_gpu_program5                                          */
  CG_PROFILE_GP5VP   = 7018, /* NV_gpu_program5                                          */
  CG_PROFILE_GP5GP   = 7019, /* NV_gpu_program5                                          */
  CG_PROFILE_GP5TCP  = 7020, /* NV_tessellation_program5 Tessellation control program    */
  CG_PROFILE_GP5TEP  = 7021, /* NV_tessellation_program5 Tessellation evaluation program */
  CG_PROFILE_GS_4_0  = 6169,
  CG_PROFILE_GS_5_0  = 6172,
  CG_PROFILE_HS_5_0  = 6173,
  CG_PROFILE_DS_5_0  = 6174,
#endif
			if (dsText != 0)
			{
				CGprofile shaderprofile;
				if( cgGLIsProfileSupported(CG_PROFILE_DS_5_0) )
				{
					shaderprofile = CG_PROFILE_DS_5_0;
					LOG_PRINT("DP_5_0\n");
				}
				else
				{
					LOG_PRINT("Failed to initialize shader! Hardware doesn't support any of the required shading extensions!\n");
					////DBG_HALT;
					return -1;
				}
				shader->profile[eDomainShader]=new CGprofile(shaderprofile);
				shader->program[eDomainShader] = cgCreateProgram( RendererCgGL_1_1::GetCgContext(),
				CG_SOURCE,
				dsText,
				*(shader->profile[eDomainShader]),
				dsMain,
				0 );
				shader->compiled[eDomainShader] = true;
			}
			else dsResult = GL_TRUE;
			if (hsText != 0)
			{
				CGprofile shaderprofile;
				if( cgGLIsProfileSupported(CG_PROFILE_HS_5_0) )
				{
					shaderprofile = CG_PROFILE_HS_5_0;
					LOG_PRINT("HP_5_0\n");
				}
				else
				{
					LOG_PRINT("Failed to initialize shader! Hardware doesn't support any of the required shading extensions!\n");
					////DBG_HALT;
					return -1;
				}
				shader->profile[eHullShader]=new CGprofile(shaderprofile);
				shader->program[eHullShader] = cgCreateProgram( RendererCgGL_1_1::GetCgContext(),
				CG_SOURCE,
				hsText,
				*(shader->profile[eHullShader]),
				hsMain,
				0 );
				shader->compiled[eHullShader] = true;
			}
			else hsResult = GL_TRUE;

			if (gsText != 0)
			{
				CGprofile shaderprofile;
				if( cgGLIsProfileSupported(CG_PROFILE_GS_5_0) )
				{
					shaderprofile = CG_PROFILE_GS_5_0;
					LOG_PRINT("GP_5_0\n");
				}
				else if( cgGLIsProfileSupported(CG_PROFILE_GS_4_0) )
				{
					shaderprofile = CG_PROFILE_GS_4_0;
					LOG_PRINT("GP_4_0\n");
				}
				else
				{
					LOG_PRINT("Failed to initialize shader! Hardware doesn't support any of the required shading extensions!\n");
					////DBG_HALT;
					return -1;
				}
				shader->profile[eGeometryShader]=new CGprofile(shaderprofile);
				shader->program[eGeometryShader] = cgCreateProgram( RendererCgGL_1_1::GetCgContext(),
				CG_SOURCE,
				gsText,
				*(shader->profile[eGeometryShader]),
				gsMain,
				0 );
				shader->compiled[eGeometryShader] = true;
			}
			else gsResult = GL_TRUE;
			if (vsText.length())
			{
				CGprofile vertexShaderprofile;
				if( cgGLIsProfileSupported(CG_PROFILE_VS_5_0) )
				{
				vertexShaderprofile = CG_PROFILE_VS_5_0;
				LOG_PRINT("VP_5_0\n");
				}
				if( cgGLIsProfileSupported(CG_PROFILE_VS_4_0) )
				{
				vertexShaderprofile = CG_PROFILE_VS_4_0;
				LOG_PRINT("VP_4_0\n");
				}
				if( cgGLIsProfileSupported(CG_PROFILE_VP40) ) // Supports vertex textures. Use with CG_PROFILE_FP40.
				{                                             // and sample with tex2D(sampler2D, float4);
				vertexShaderprofile = CG_PROFILE_VP40;        // Ref. file /IndieLib_resources/new/x/x/R2VB_VTF/18/vpWaterDM.cg
				LOG_PRINT("VP40\n");                          //
				}                                             //
				else if( cgGLIsProfileSupported(CG_PROFILE_VP30) )
				{
				vertexShaderprofile = CG_PROFILE_VP30;
				LOG_PRINT("VP30\n");
				}
				else if( cgGLIsProfileSupported(CG_PROFILE_VP20) )
				{
				vertexShaderprofile = CG_PROFILE_VP20;
				LOG_PRINT("VP20\n");
				}
				else if( cgGLIsProfileSupported(CG_PROFILE_ARBVP1) )
				{
				vertexShaderprofile = CG_PROFILE_ARBVP1;
				LOG_PRINT_("ARBVP1\n");
				}
				else
				{
					LOG_PRINT("Failed to initialize vertex shader! Hardware doesn't support any of the required vertex shading extensions!\n");
					////DBG_HALT;
					return -1;
				}
		#if 0
    		std::ofstream out("./vsText.txt");
    		out << "\nvsText:\n";
    		out << vsText.c_str();
    		out << "\nmainvs:\n";
    		out << vsMain;
    		out.close();
		#endif
				shader->profile[eVertexShader]=new CGprofile(vertexShaderprofile);
				shader->program[eVertexShader] = cgCreateProgram( RendererCgGL_1_1::GetCgContext(),
				CG_SOURCE,
				vsText.c_str(),
				*(shader->profile[eVertexShader]),
				vsMain,
				0 );
				shader->compiled[eVertexShader] = true;
				#if 0
				if (vsResult)
				{
				}
				else
				{
                    infoLogPos += stx_snprintf(infoLog + infoLogPos, "Vertex shader error:\n");
				}
				infoLogPos += len;
				#endif
			}
			else vsResult = GL_TRUE;
			if (fsText.length())
			{
#if 0
  CG_PROFILE_GP4FP   = 7010,
  CG_PROFILE_GP4VP   = 7011,
  CG_PROFILE_GP4GP   = 7012,
  CG_PROFILE_GP5FP   = 7017, /* NV_gpu_program5                                          */
  CG_PROFILE_GP5VP   = 7018, /* NV_gpu_program5                                          */
  CG_PROFILE_GP5GP   = 7019, /* NV_gpu_program5                                          */
  CG_PROFILE_GP5TCP  = 7020, /* NV_tessellation_program5 Tessellation control program    */
  CG_PROFILE_GP5TEP  = 7021, /* NV_tessellation_program5 Tessellation evaluation program */
  CG_PROFILE_GS_4_0  = 6169,
  CG_PROFILE_GS_5_0  = 6172,
  CG_PROFILE_HS_5_0  = 6173,
  CG_PROFILE_DS_5_0  = 6174,
#endif
                //stx_snprintf(line,16, "#line %d\n", fsLine);
				//shaderStrings[strIndex] = psText.c_str();
				CGprofile fragmentShaderprofile;
				if( cgGLIsProfileSupported(CG_PROFILE_PS_5_0) )
				{
				fragmentShaderprofile = CG_PROFILE_PS_5_0;
				LOG_PRINT("FP_5_0\n");
				}
				if( cgGLIsProfileSupported(CG_PROFILE_PS_4_0) )
				{
				fragmentShaderprofile = CG_PROFILE_PS_4_0;
				LOG_PRINT("FP_4_0\n");
				}
				if( cgGLIsProfileSupported(CG_PROFILE_FP40) )
				{
				fragmentShaderprofile = CG_PROFILE_FP40;
				LOG_PRINT("FP40\n");
				}
				else if( cgGLIsProfileSupported(CG_PROFILE_FP30) )
				{
				fragmentShaderprofile = CG_PROFILE_FP30;
				LOG_PRINT("FP30\n");
				}
				else if( cgGLIsProfileSupported(CG_PROFILE_FP20) )
				{
				fragmentShaderprofile = CG_PROFILE_FP20;
				LOG_PRINT("FP20\n");
				}
				else if( cgGLIsProfileSupported(CG_PROFILE_ARBFP1) )
				{
				fragmentShaderprofile = CG_PROFILE_ARBFP1;
				LOG_PRINT_("ARBFP1\n");
				}

				else
				{
					LOG_PRINT("Failed to initialize pixel shader! Hardware doesn't support any of the required pixel shading extensions!\n"
					);
					////DBG_HALT;
					return -1;
				}
				#if 0
					#version 1, 40
					ARBVP1
					ARBFP1
				#endif

		#if 0
    		std::ofstream out("./fsText.txt");
    		out << "\nfsText:\n";
    		out << fsText.c_str();
    		out << "\nmainfs:\n";
    		out << fsMain;
    		out.close();
		#endif
				shader->profile[ePixelShader] = new CGprofile(fragmentShaderprofile);
				shader->program[ePixelShader] = cgCreateProgram( RendererCgGL_1_1::GetCgContext(),
				CG_SOURCE,
				fsText.c_str(),
				*(shader->profile[ePixelShader]),
				fsMain,
				0 );
				shader->compiled[ePixelShader] = true;
                g_programfs=shader->program[ePixelShader];
				#if 0
				if (fsResult)
				{
				}
				else
				{
                    infoLogPos += stx_snprintf(infoLog + infoLogPos, "Fragment shader error:\n");
				}
				infoLogPos += len;
				#endif
			}
			else fsResult = GL_TRUE;
					{
						{
							CgReflection* p=0;
							CgReflection* pv=0;
							CgReflection* pf=0;
							GLint uniformCount=0;
							std::vector<SamplerShaderCgGL3	> samplers;
							std::vector<ConstantShaderCgGL3 > uniforms;
							//std::vector<AttribShaderCgGL3 > attribs;
							for(unsigned int i=0;i<shader->compiled.size();i++)
							{
                                if(!shader->compiled[i])
                                    continue;
								GLint uniformCount_=0;
								CgReflection* p=new CgReflection();
								p->parse(shader->program[i],uniformCount_);
LOG_FNLN_;
LOG_PRINT_("uniformCount_=%d\n", uniformCount_);

								uniformCount+=uniformCount_;
								if(i==eVertexShader)
									pv=p;
								if(i==ePixelShader)
									pf=p;
							}
LOG_FNLN_;
LOG_PRINT_("uniformCount=%d\n", uniformCount);
                            //uniformCount=1;//???
							samplers.resize(uniformCount);
							uniforms.resize(uniformCount);
							//attribs.resize(uniformCount);
							int nSamplers = 0;
							int nUniforms = 0;
							int nAttribs = 0;
							CgReflection* pp=pv;
                if(pv){
							for (int ii = 0; ii < uniformCount; ii++)
							{
								int i=ii;
								if(i>=pv->tu.size())
								{
									pp=pf;
									i=i-pv->tu.size();
								}


LOG_FNLN_;
LOG_PRINT_("pp->n[%d]=%s\n", i, pp->n[i].c_str());

								GLint length=pp->n[i].length();
								if (pp->tu[i] != -1)
								{

									samplers[nSamplers];
									samplers[nSamplers].index = nSamplers;
#if 1
									samplers[nSamplers].param=pp->p[i];
#elif 0
                                    samplers[nSamplers].param = cgGetNamedParameter(shader->vertexShaderprogram, pp->n[i].c_str());
#elif 0
                                    samplers[nSamplers].param = cgGetNamedParameter(shader->fragmentShaderprogram, pp->n[i].c_str());
#endif
									samplers[nSamplers].name=std::string( pp->n[i].c_str());							
                                    stx_Variables::AddSampler(shaders.size(), pp->n[i], nSamplers);
									nSamplers++;

								}
								else
								{
									{
										{

											uniforms[nUniforms];
											uniforms[nUniforms].index = nUniforms;
											uniforms[nUniforms].type = getConstantTypeCg(pp->t[i]);
											uniforms[nUniforms].nElements = pp->rows[i]*pp->columns[i];
#if 1
									uniforms[nUniforms].param=pp->p[i];
#elif 0
                                    uniforms[nUniforms].param = cgGetNamedParameter(shader->vertexShaderprogram, pp->n[i].c_str());
#elif 0
                                    uniforms[nUniforms].param = cgGetNamedParameter(shader->fragmentShaderprogram, pp->n[i].c_str());
#endif
									uniforms[nUniforms].name=std::string(pp->n[i].c_str());								    
                        stx_Variables::AddUniform(shaders.size(), pp->n[i], pp->rows[i]*pp->columns[i], getConstantTypeCg(pp->t[i])); // ???
											nUniforms++;

										}
									}
								}
							}}
							delete p;
                            				LOG_FNLN_;
							for (int i = 0; i < shader->compiled.size(); i++){
							if(shader->compiled[i])
							{
                            					LOG_PRINT_("cgGLLoadProgram[%d]\n", i);
                            					LOG_PRINT_("cgGLBindProgram[%d]\n", i);
                            					LOG_PRINT_("cgGLEnableProfile[%d]\n", i);
								cgGLLoadProgram( shader->program[i]);
								cgGLBindProgram( shader->program[i] );
								cgGLEnableProfile( *shader->profile[i] );
							}}
							samplers.resize(nSamplers);
							uniforms.resize(nUniforms);
							//attribs.resize(nAttribs);

							if(uniforms.size()>1) std::sort(uniforms.begin(), uniforms.end(), constantCgCompGL2std);

							if(samplers.size()>1) std::sort(samplers.begin(), samplers.end(), samplerCgCompGL2std);
							//if(attribs.size()>1) std::sort(attribs.begin(), attribs.end(), attribCgCompGL2std);

							for (int i = 0; i < nUniforms; i++)
							{
								int constantSize = constantTypeSizes[uniforms[i].type] * uniforms[i].nElements;
								uniforms[i].data .resize(constantSize);
								stx_memset((void*)&uniforms[i].data[0], 0, constantSize);
								uniforms[i].dirty = false;
							}
							//shader->attribs  = attribs;
							shader->uniforms  = uniforms;
							shader->samplers  = samplers;
							shader->nAttribs = nAttribs;
							shader->nUniforms = nUniforms;
							shader->nSamplers = nSamplers;

							for (int i = 0; i < samplers.size(); i++)
							{
								LOG_PRINT_("Sampler name=%s\n", samplers[i].name.c_str());
							}
							for (int i = 0; i < uniforms.size(); i++)
							{
								LOG_PRINT_("Uniform name=%s\n", uniforms[i].name.c_str());
							}
#if 0
							for (int i = 0; i < attribs.size(); i++)
							{
								LOG_PRINT_("Attrib name=%s\n", attribs[i].name.c_str());
							}
#endif
							shaders.push_back(shader);
							//stx_exit(0); // ???
							return shaders.size()-1;
						}
					}
#ifdef _MSC_VER
ErrorMsg(infoLog);
#else
					LOG_PRINT("\n%s\n",infoLog);
#endif
					return SHADER_NONE;
				}
void RendererCgGL_1_1::applyTextures()
{
    LOG_FNLN_;
    //if(!g_textures.size()) return;
    for(unsigned int i=0;i<g_textureNames.size();i++)
    {
        CGparameter param = cgGetNamedParameter(g_programfs /* ??? */ , g_textureNames[i].c_str());
	cgGLSetTextureParameter( param, textures[g_textures[i]].glTexID );
  	cgSetSamplerState(param);
        cgGLEnableTextureParameter( param );
        LOG_FNLN_;
        LOG_PRINT_("cgGLSetTextureParameter\n");
        LOG_PRINT_("cgGLEnableTextureParameter(textures[%x].%x)\n", g_textures[i], textures[g_textures[i]].glTexID);
    }
}

void RendererCgGL_1_1::disApplyTextures()
{
    LOG_FNLN_;
    for(unsigned int i=0;i<g_textureNames.size();i++)
    {
        CGparameter param = cgGetNamedParameter(g_programfs /* ??? */, g_textureNames[i].c_str());
        cgGLDisableTextureParameter( param );
        LOG_FNLN_;
        LOG_PRINT_("cgGLDisableTextureParameter(textures[%x].%x)\n", g_textures[i], textures[g_textures[i]].glTexID);
    }
    g_textureNames.clear();
	g_textures.clear();
}

void RendererCgGL_1_1::disApplyProfiles()
{
    if((currentShader<0) || (shaders.size()<=currentShader)) return;
    for (unsigned int i = 0; i < shaders[currentShader]->compiled.size(); i++)
	{
			if(shaders[currentShader]->compiled[i])
			{
                LOG_PRINT_("cgGLDisableProfile[%d]\n", i);
				cgGLDisableProfile( *shaders[currentShader]->profile[i] );
			}
	}
}

void MyCgErrorCallback( void );

		RendererCgGL_1_1::RendererCgGL_1_1(
			bool fullScreen,
int argc, char *argv[]
) : mOk(false), RendererGL_1_1(fullScreen,argc,argv)
{
    	width=STX_Service::GetWindowInstance()->Getwidth();
    	height=STX_Service::GetWindowInstance()->Getheight();
	//std::string str=miscParams["window"]
	//
	SHADING_LANGUAGE_VERSION_MAJOR=5;
	SHADING_LANGUAGE_VERSION_MINOR=0;

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
	HWND w=STX_Service::GetWindowInstance()->GetHWND();
	HDC m_hDC = GetDC( w ); // HDC
	PixelFormat = ChoosePixelFormat( m_hDC, &pfd );
	SetPixelFormat( m_hDC, PixelFormat, &pfd);

	HGLRC hGlrc = wglCreateContext(m_hDC);// HGLRC

	wglMakeCurrent( m_hDC, hGlrc );

	#else
	//initGLee();
#if 0
	if (info.subsystem!=SDL_SYSWM_X11)
	{
		LOG_PRINT("System is not X11!\n");
		LOG_PRINT("%d!=%d\n", (int)info.subsystem , (int)SDL_SYSWM_X11 <<'\n');
		return;
	}

	if(!dpy)
	{
		m_dpy=info.info.x11.display;
		m_win=info.info.x11.window;
		int errorbase,tEventbase;
		if (glXQueryExtension(dpy,&errorbase,&tEventbase) == False)

		{
			LOG_PRINT("OpenGL through GLX not supported.\n");
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
			LOG_PRINT("Could not initialize Visual.\n");
			return;
		}
		ctx=glXCreateContext(dpy,vi,0,True);
		if(ctx== 0)
		{
			LOG_PRINT("Could not initialize GL context.\n");
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
	Display* d=STX_Service::GetWindowInstance()->GetDisplay();
	//initExtensions(d);
	//???initExtensions(hDc);
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
			if (GL_ARB_fragment_shader_supported || GL_ARB_fragment_program_supported)
			{
				glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &units);
				checkGlError(""); // ??? error 0x00000500
			}
			else
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
			if (GL_EXT_texture_filter_anisotropic_supported)
			{
				GLint aniso = 1;
				//glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
				//checkGlError("");
				maxAnisotropic = aniso;
			}
			fbo = 0;
			currentFrontFace = GL_CW;
			currentVBO = 0;
			texVF	= VF_NONE;
			plainShader = SHADER_NONE;
			plainVF = VF_NONE;
			setGLdefaults();
			resetToDefaults();
			stx_memset(&activeVertexFormat, VF_NONE, sizeof(activeVertexFormat));
			#if 0
			uniformFuncs[CONSTANT_FLOAT] = (void *) __glUniform1fvARB;
			uniformFuncs[CONSTANT_VEC2]  = (void *) __glUniform2fvARB;
			uniformFuncs[CONSTANT_VEC3]  = (void *) __glUniform3fvARB;
			uniformFuncs[CONSTANT_VEC4]  = (void *) __glUniform4fvARB;
			uniformFuncs[CONSTANT_INT]   = (void *) __glUniform1ivARB;
			uniformFuncs[CONSTANT_IVEC2] = (void *) __glUniform2ivARB;
			uniformFuncs[CONSTANT_IVEC3] = (void *) __glUniform3ivARB;
			uniformFuncs[CONSTANT_IVEC4] = (void *) __glUniform4ivARB;
			uniformFuncs[CONSTANT_BOOL]  = (void *) __glUniform1ivARB;
			uniformFuncs[CONSTANT_BVEC2] = (void *) __glUniform2ivARB;
			uniformFuncs[CONSTANT_BVEC3] = (void *) __glUniform3ivARB;
			uniformFuncs[CONSTANT_BVEC4] = (void *) __glUniform4ivARB;
			uniformFuncs[CONSTANT_MAT2]  = (void *) __glUniformMatrix2fvARB;
			uniformFuncs[CONSTANT_MAT3]  = (void *) __glUniformMatrix3fvARB;
			uniformFuncs[CONSTANT_MAT4]  = (void *) __glUniformMatrix4fvARB;
			#elif 0

			uniformFuncs[CONSTANT_FLOAT] = (CGGL_API void* CGGLENTRY) cgGLSetParameter1fv;
			uniformFuncs[CONSTANT_VEC2]  = (CGGL_API void* CGGLENTRY) cgGLSetParameter2fv;
			uniformFuncs[CONSTANT_VEC3]  = (CGGL_API void* CGGLENTRY) cgGLSetParameter3fv;
			uniformFuncs[CONSTANT_VEC4]  = (CGGL_API void* CGGLENTRY) cgGLSetParameter4fv;
			uniformFuncs[CONSTANT_INT]   = (CGGL_API void* CGGLENTRY) cgGLSetParameter1fv;
			uniformFuncs[CONSTANT_IVEC2] = (CGGL_API void* CGGLENTRY) cgGLSetParameter2fv;
			uniformFuncs[CONSTANT_IVEC3] = (CGGL_API void* CGGLENTRY) cgGLSetParameter3fv;
			uniformFuncs[CONSTANT_IVEC4] = (CGGL_API void* CGGLENTRY) cgGLSetParameter4fv;
			uniformFuncs[CONSTANT_BOOL]  = (CGGL_API void* CGGLENTRY) cgGLSetParameter1fv;
			uniformFuncs[CONSTANT_BVEC2] = (CGGL_API void* CGGLENTRY) cgGLSetParameter2fv;
			uniformFuncs[CONSTANT_BVEC3] = (CGGL_API void* CGGLENTRY) cgGLSetParameter3fv;
			uniformFuncs[CONSTANT_BVEC4] = (CGGL_API void* CGGLENTRY) cgGLSetParameter4fv;
			uniformFuncs[CONSTANT_MAT2]  = (CGGL_API void* CGGLENTRY) cgGLSetMatrixParameterfr;
			uniformFuncs[CONSTANT_MAT3]  = (CGGL_API void* CGGLENTRY) cgGLSetMatrixParameterfr;
			uniformFuncs[CONSTANT_MAT4]  = (CGGL_API void* CGGLENTRY) cgGLSetMatrixParameterfr;
			#endif
			bsupportsHalf=false;
			bsupportsR2VB=false;
			overrideR2VB=true;
			overrideHalf=true;


		//defaultFont = addFont(FONT_PATH"Future.dds", FONT_PATH"Future.font", linearClamp);

				m_CgContext = cgCreateContext();
  cgGLSetDebugMode(CG_FALSE);
  cgSetParameterSettingMode(RendererCgGL_1_1::GetCgContext(), CG_DEFERRED_PARAMETER_SETTING);
				cgGLRegisterStates(RendererCgGL_1_1::GetCgContext());
				cgSetErrorCallback(MyCgErrorCallback);
				;
LOG_PRINT("CgGL_1_1Renderer\n");
			mOk=true;
		}

		RendererCgGL_1_1::~RendererCgGL_1_1()
		{
			STX_TRY;
			apply();
			for (unsigned int i = 0; i < shaders.getCount(); i++)
			{
				for (unsigned int j = 0; j < shaders[i]->nSamplers; j++)
				{
					//delete shaders[i]->samplers[j].name;
				}
				for (unsigned int j = 0; j < shaders[i]->nUniforms; j++)
				{
					//delete shaders[i]->uniforms[j].name;
					shaders[i]->uniforms[j].data.clear();
				}
				for (unsigned int j = 0; j < shaders[i]->nAttribs; j++)
				{
					shaders[i]->attribs[j].data.clear();
				}
				shaders[i]->attribs.clear();
				shaders[i]->samplers.clear();
				shaders[i]->uniforms.clear();
				#if 0
				__glDeleteObjectARB(shaders[i]->vertexShader);
				__glDeleteObjectARB(shaders[i]->fragmentShader);
				__glDeleteObjectARB(shaders[i]->program);
				#else
			for (unsigned int j = 0; j < 6; j++)
			{
				if(shaders[i]->compiled[j])
					cgDestroyProgram(shaders[i]->program[j]);
			}
				#endif
			}
#ifdef _MSC_VER
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

			if (fbo) {glDeleteFramebuffersEXT(1, &fbo);
				checkGlError("");}
#endif
for (unsigned int i = 0; i < textures.getCount(); i++)
			{
				removeTexture(i);
			}
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
		STX_CATCH;
		}

#if 1
CGeffect    myCgEffect;
CGtechnique myCgTechnique;
static CGprofile latestVertexProfile;
static CGprofile latestFragmentProfile;
/* If requested, over-ride the "latest" profile for the vertex and
   fragment state assignments. */
static void registerLatestProfiles(void)
{
  CGstate vpState, vsState, fpState, psState;

  /* To be comprehensive, change both the OpenGL-style "VertexProgram"
     and Direct3D-style "VertexShader" state names. */
  vpState = cgGetNamedState(RendererCgGL_1_1::GetCgContext(), "VertexProgram");
  vsState = cgGetNamedState(RendererCgGL_1_1::GetCgContext(), "VertexShader");

  assert(CG_PROGRAM_TYPE == cgGetStateType(vpState));
  assert(CG_PROGRAM_TYPE == cgGetStateType(vsState));

  if (latestVertexProfile) {
    cgSetStateLatestProfile(vpState, latestVertexProfile);
    cgSetStateLatestProfile(vsState, latestVertexProfile);
  }

  LOG_PRINT("VertexProgram latest profile = %s\n", cgGetProfileString(cgGetStateLatestProfile(vpState)));
  LOG_PRINT("VertexShader latest profile = %s\n", cgGetProfileString(cgGetStateLatestProfile(vsState)));

  /* To be comprehensive, change both the OpenGL-style "FragmentProgram"
     and Direct3D-style "FragmentShader" state names. */
  fpState = cgGetNamedState(RendererCgGL_1_1::GetCgContext(), "FragmentProgram");
  psState = cgGetNamedState(RendererCgGL_1_1::GetCgContext(), "PixelShader");

  assert(CG_PROGRAM_TYPE == cgGetStateType(fpState));
  assert(CG_PROGRAM_TYPE == cgGetStateType(psState));

  if (latestFragmentProfile) {
    cgSetStateLatestProfile(fpState, latestFragmentProfile);
    cgSetStateLatestProfile(psState, latestFragmentProfile);
  }

  LOG_PRINT("FragmentProgram latest profile = %s\n", cgGetProfileString(cgGetStateLatestProfile(fpState)));
  LOG_PRINT("PixelShader latest profile = %s\n", cgGetProfileString(cgGetStateLatestProfile(psState)));
}

static void checkForCgError_(const char *situation)
{
  CGerror error;
  const char *string = cgGetLastErrorString(&error);
  
  if (error != CG_NO_ERROR) {
    if (error == CG_COMPILER_ERROR) {
      fprintf(stderr,
             "Situation: %s\n"
             "Error: %s\n\n"
             "Cg compiler output...\n%s", situation, string,
             cgGetLastListing(RendererCgGL_1_1::GetCgContext()));
    } else {
      fprintf(stderr,
              "Situation: %s\n"
              "Error: %s", situation, string);
    }
    exit(1);
  }
}

static void initCg(const char* aName)
{
  cgGLRegisterStates(RendererCgGL_1_1::GetCgContext());
  checkForCgError_("registering standard CgFX states");
  registerLatestProfiles();
  checkForCgError_("registering command line latest profile setting for CgFX program states");
  cgGLSetManageTextureParameters(RendererCgGL_1_1::GetCgContext(), CG_TRUE);
  checkForCgError_("manage texture parameters");

  myCgEffect = cgCreateEffectFromFile(RendererCgGL_1_1::GetCgContext(), aName, NULL);
  checkForCgError_("creating bumpdemo.cgfx effect");
  assert(myCgEffect);

  myCgTechnique = cgGetFirstTechnique(myCgEffect);
  while (myCgTechnique && cgValidateTechnique(myCgTechnique) == CG_FALSE) {
    LOG_PRINT("Technique %s did not validate.  Skipping.\n",
      cgGetTechniqueName(myCgTechnique));
    myCgTechnique = cgGetNextTechnique(myCgTechnique);
  }
  if (myCgTechnique) {
    LOG_PRINT("Use technique %s.\n",
      cgGetTechniqueName(myCgTechnique));
  } else {
    LOG_PRINT("No valid technique\n");
    exit(1);
  }
}
struct TECHNIQUE_DESCCgGL_1_1
{
	TECHNIQUE_DESCCgGL_1_1(){Passes=0;}
	virtual ~TECHNIQUE_DESCCgGL_1_1(){}
	TECHNIQUE_DESCCgGL_1_1(const TECHNIQUE_DESCCgGL_1_1& rhs){Passes=rhs.Passes;}
	unsigned int Passes;
};

struct stx_EffectCgGL_1_1 : public stx_Effect
{
//protected:
	struct stx_PassCgGL_1_1
	{
	    struct stx_TextureCgGL_1_1
	    {
		    stx_TextureCgGL_1_1()
		    {
                //<Texture name="Base" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
		        texID=-1;
                m_iDimension=2;
                m_bUseMipmap=false;
                ss=-1;
            }
		    virtual ~stx_TextureCgGL_1_1(){}
		    stx_TextureCgGL_1_1(const stx_TextureCgGL_1_1& rhs)
		    {
			    m_sName=rhs.m_sName;
                m_sFile=rhs.m_sFile;
		        texID=rhs.texID;
                m_iDimension=rhs.m_iDimension;
                m_bUseMipmap=rhs.m_bUseMipmap;
                ss=rhs.ss;
            }
		        std::string m_sName;
		        std::string m_sFile;
		        TextureID texID;
                unsigned int m_iDimension;
                bool m_bUseMipmap;
                SamplerStateID ss;
        };
		stx_PassCgGL_1_1()
		{
        		VertexFormat=-1;
			SamplerState=-1;
			BlendState=-1;
			AlphaState=-1;
        		DepthStencilState=-1;
        		RasterizerState=-1;
		}
		stx_PassCgGL_1_1(const std::string& aFileName, std::vector<std::string>& avMain, std::vector<ShaderType>& avType)
		{
        		VertexFormat=-1;
			SamplerState=-1;
			BlendState=-1;
			AlphaState=-1;
        		DepthStencilState=-1;
        		RasterizerState=-1;
		}
		virtual ~stx_PassCgGL_1_1(){}
		void Apply(unsigned int aI){} // ???
		stx_PassCgGL_1_1(const stx_PassCgGL_1_1& rhs)
		{
			//m_vTextures=rhs.m_vTextures;
			m_sName=rhs.m_sName;
        		VertexFormat=rhs.VertexFormat;
        		format_=rhs.format_;
			SamplerState=rhs.SamplerState;
			SamplerName=rhs.SamplerName;
			BlendState=rhs.BlendState;
			AlphaState=rhs.AlphaState;
        		DepthStencilState=rhs.DepthStencilState;
        		RasterizerState=rhs.RasterizerState;
		}
		std::string m_sName;
		std::vector<int> format_;
        	VertexFormatID VertexFormat;
		SamplerStateID SamplerState;
		std::string SamplerName;
		BlendStateID BlendState;
		AlphaStateID AlphaState;
        	DepthStateID DepthStencilState;
        	RasterizerStateID RasterizerState;
		//std::vector<stx_TextureCgGL_1_1> m_vTextures;
	};
	struct stx_VariableCgGL_1_1
	{
        	std::string m_sName;
		stx_VariableCgGL_1_1(){}
		virtual ~stx_VariableCgGL_1_1(){}
		stx_VariableCgGL_1_1(const stx_VariableCgGL_1_1& rhs)
		{
            m_sName=rhs.m_sName;
		}

	void SetResourceArray( ShaderResourceViewID* id, unsigned int startindex, unsigned int aN )
	{
		for(unsigned int i=startindex;i<aN;i++)
		{
			SetTexture(id[i]); // sampler name ???
		}
	}
	void SetResource( ShaderResourceViewID id ){SetTexture(id);}
        void SetFloat( float aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstant1f(m_sName.c_str(), aValue);
        }
        void SetFloatVector( float* aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstantArray4f(m_sName.c_str(), (D3DXFROMWINEVECTOR4*)aValue, elementsOf(aValue));
        }
        void SetFloatVectorArray( float* aValue, unsigned int aS, unsigned int aN )
        {
	        IRenderer::GetRendererInstance()->setShaderConstantArray4f(m_sName.c_str(), (D3DXFROMWINEVECTOR4*)aValue, aN);
        }
        void SetIntVector( int* aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstantRaw(m_sName.c_str(), (int*)aValue, elementsOf(aValue));
        }
	void SetFloatArray( float* aValue, unsigned int aS, unsigned int aN )
        {
	        IRenderer::GetRendererInstance()->setShaderConstantArray1f(m_sName.c_str(), aValue, aN);
        }
        void SetMatrix( float* aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstant4x4f(m_sName.c_str(), *((D3DXFROMWINEMATRIX*)aValue));
        }
        void SetMatrix( D3DXFROMWINEMATRIX* aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstant4x4f(m_sName.c_str(), *aValue);
        }
        void SetVector( D3DXFROMWINEVECTOR4* aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstant4f(m_sName.c_str(), *aValue);
        }
        void SetInt( int* aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstant1i(m_sName.c_str(), *aValue);
        }
        void SetValue( void* aPtr, unsigned int aSize )
        {
	        IRenderer::GetRendererInstance()->setShaderConstantRaw(m_sName.c_str(), &aPtr, aSize);
        }
        void SetTexture( TextureID aValue )
        {
	        IRenderer::GetRendererInstance()->setTexture(m_sName.c_str(), aValue);
        }
        void SetBool( bool aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstantRaw(m_sName.c_str(), &aValue, sizeof(bool));
        }
        void SetMatrixArray( void* aPtr, unsigned int aSize, unsigned int aN)
	{
	        IRenderer::GetRendererInstance()->setShaderConstantRaw(m_sName.c_str(), &aPtr, sizeof(D3DXFROMWINEMATRIX)*aN);
	}
	};
	struct stx_TechniqueCgGL_1_1
	{
		stx_TechniqueCgGL_1_1(stx_EffectCgGL_1_1* pEffect=0){m_pEffect=pEffect;}
		virtual ~stx_TechniqueCgGL_1_1(){}
		stx_TechniqueCgGL_1_1(const stx_TechniqueCgGL_1_1& rhs)
		{
			m_pEffect=rhs.m_pEffect;
			m_sName=rhs.m_sName;
			m_vPasses=rhs.m_vPasses;
		}
		void GetDesc( TECHNIQUE_DESCCgGL_1_1* techDesc ){if(techDesc)techDesc->Passes=m_vPasses.size();}
		stx_PassCgGL_1_1* GetPassByIndex(unsigned int aI)
		{
			if((aI+1)>m_vPasses.size())
				return 0;
			return &m_vPasses[aI];
		} // ???
		stx_PassCgGL_1_1* GetPassByName(const std::string& aName)
		{
			int idx=-1;
			for(unsigned int i=0;i<m_vPasses.size();i++)
			{
				if(m_vPasses[i].m_sName==aName)
				{
					idx=i;
					break;
				}	
			}
			if(idx>-1)
				return &m_vPasses[idx];
			return 0;
		} // ???
		stx_EffectCgGL_1_1* m_pEffect;
		std::string m_sName;
		std::vector<stx_PassCgGL_1_1> m_vPasses;
		std::vector<stx_VariableCgGL_1_1> m_vVariables;
	};
	std::vector<stx_EffectCgGL_1_1::stx_PassCgGL_1_1::stx_TextureCgGL_1_1> m_vTextures;
	std::vector<stx_TechniqueCgGL_1_1> m_vTechniques;
	std::string m_sTechnique;
	int  m_iTechnique;
	std::string m_sFileName;
    CGeffect m_CGeffect;
public:
	stx_EffectCgGL_1_1():stx_Effect(){m_iTechnique=-1;}
	stx_EffectCgGL_1_1(const std::string& aFileName, const char* aDefines=0):stx_Effect()
    {
	    std::string tmp="#define __HLSLCG__ 1\n#define saturate(x) clamp(x,0.0,1.0)\n#define lerp mix\n#define ROW_MAJOR row_major\n#define MVPSEMANTIC  : state.matrix.mvp\n#define WSIGN -\n#define fract frac\n#define mix lerp\n#define atan(x,y) atan2(y,x)\n#undef TEX2D\n#define SAMPLE2D(TEX, TEXCOORD) tex2D\(TEX\, TEXCOORD\)\n#define SAMPLER2D sampler2D\nfloat3 GammaCorrect3(float3 aColor)\n{\n\treturn aColor;\n}\nfloat4 GammaCorrect4(float4 aColor)\n{\n\treturn aColor;\n}\n";
        tmp.append(stx_Effect::ParseEffect(aFileName, 0));
        //tmp.append("technique Simple\n{\n\tpass P0\n\t{\n\t\tVertexShader = compile vs_3_0 mainVS();\n\t\tPixelShader  = compile ps_3_0 mainPS();\n\t}\n}\n");
        //tmp.append("technique Simple\n{\n\tpass P0\n\t{\n\t\tVertexShader = compile vs_3_0 mainVS();\n\t\tPixelShader  = compile ps_3_0 mainPS();\n\t}\n}\n");
        tmp.append("technique Simple\n{\n\tpass\n\t{\n\t\tVertexProgram   = compile latest mainVS();\n\t\tFragmentProgram = compile latest mainPS();\n\t}\n}\n");
#if 0
        std::ofstream out("./__tmp__.fx");
        out << tmp.c_str();
        out.close();
#endif
        m_CGeffect=cgCreateEffect(RendererCgGL_1_1::GetCgContext(), tmp.c_str(), 0);
    }
	virtual ~stx_EffectCgGL_1_1(){}
	stx_EffectCgGL_1_1(const stx_EffectCgGL_1_1& rhs)
	{
		m_sFileName=rhs.m_sFileName;
		m_sTechnique=rhs.m_sTechnique;
		m_iTechnique=rhs.m_iTechnique;
		m_vTechniques=rhs.m_vTechniques;
		m_vTextures=rhs.m_vTextures;
	}
        void SetMatrix( const char* aName, D3DXFROMWINEMATRIX* aValue );
        void SetVector( const char* aName, D3DXFROMWINEVECTOR4* aValue );
        void SetFloat( const char* aName, float* aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstant1f(aName, *aValue);
        }

        void SetValue( const char* aName, void* aPtr, unsigned int aSize );
	void SetTexture( const char* aName, TextureID aValue );

	void SetBool( const char* aName, bool aValue );
	void SetTechnique( const char* aName );

		stx_VariableCgGL_1_1* GetVariableByName( const char* aName )
		{
			if(!aName)
				return 0;
			if(m_iTechnique<0)
				return 0;
			if(m_iTechnique>=m_vTechniques.size())
				return 0;
			int idx=-1;
			for(unsigned int i=0;i<m_vTechniques[m_iTechnique].m_vVariables.size();i++)
			{
				if(m_vTechniques[m_iTechnique].m_vVariables[i].m_sName==std::string(aName))
				{
					idx=i;
					break;
				}	
			}
			if(idx>-1)
				return &m_vTechniques[m_iTechnique].m_vVariables[idx];
			return 0;
		} // ???
		
		stx_TechniqueCgGL_1_1* GetTechniqueByName( const char* aName )
		{
			if(!aName)
				return 0;
			int idx=-1;
			for(unsigned int i=0;i<m_vTechniques.size();i++)
			{
				if(m_vTechniques[i].m_sName==std::string(aName))
				{
					idx=i;
					break;
				}	
			}
			if(idx>-1)
				return &m_vTechniques[idx];
			return 0;
		} // ???


	void Begin( unsigned int *cPasses, unsigned int aValue );
	void BeginPass( unsigned int iPass );
        void EndPass();
        void End();
};
#else
struct stx_EffectCgGL_1_1 : public stx_Effect
{
//protected:
	struct stx_PassCgGL_1_1
	{
	    struct stx_TextureCgGL_1_1
	    {
		    stx_TextureCgGL_1_1()
		    {
                //<Texture name="Base" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
		        texID=-1;
                m_iDimension=2;
                m_bUseMipmap=false;
                ss=-1;
            }
		    virtual ~stx_TextureCgGL_1_1(){}
		    stx_TextureCgGL_1_1(const stx_TextureCgGL_1_1& rhs)
		    {
			    m_sName=rhs.m_sName;
                m_sFile=rhs.m_sFile;
		        texID=rhs.texID;
                m_iDimension=rhs.m_iDimension;
                m_bUseMipmap=rhs.m_bUseMipmap;
                ss=rhs.ss;
            }
		        std::string m_sName;
		        std::string m_sFile;
		        TextureID texID;
                unsigned int m_iDimension;
                bool m_bUseMipmap;
                SamplerStateID ss;
        };
		stx_PassCgGL_1_1()
		{	
        		VertexFormat=-1;
			SamplerState=-1;
			BlendState=-1;
			AlphaState=-1;
        		DepthStencilState=-1;
        		RasterizerState=-1;
		}
		stx_PassCgGL_1_1(const std::string& aFileName, std::vector<std::string>& avMain, std::vector<ShaderType>& avType)
		{
        		VertexFormat=-1;
			SamplerState=-1;
			BlendState=-1;
			AlphaState=-1;
        		DepthStencilState=-1;
        		RasterizerState=-1;
		}
		virtual ~stx_PassCgGL_1_1(){}
		void Apply(unsigned int aI){} // ???
		stx_PassCgGL_1_1(const stx_PassCgGL_1_1& rhs)
		{
			//m_vTextures=rhs.m_vTextures;
			m_sName=rhs.m_sName;
			m_pEffect=rhs.m_pEffect;

        		VertexFormat=rhs.VertexFormat;
        		format_=rhs.format_;
			SamplerState=rhs.SamplerState;
			SamplerName=rhs.SamplerName;
			BlendState=rhs.BlendState;
			AlphaState=rhs.AlphaState;
        		DepthStencilState=rhs.DepthStencilState;
        		RasterizerState=rhs.RasterizerState;
		}
		std::string m_sName;
		CGeffect m_pEffect;
		std::vector<int> format_;
        	VertexFormatID VertexFormat;
		SamplerStateID SamplerState;
		std::string SamplerName;
		BlendStateID BlendState;
		AlphaStateID AlphaState;
        	DepthStateID DepthStencilState;
        	RasterizerStateID RasterizerState;
		//std::vector<stx_TextureCgGL_1_1> m_vTextures;
	};
	struct stx_VariableCgGL_1_1
	{
        	std::string m_sName;
		CGparameter parameter;
		CGeffect m_pEffect;
		stx_VariableCgGL_1_1(){}
		stx_VariableCgGL_1_1(CGeffect& aEffect, const char* m_pName=0){m_pEffect=aEffect;if(m_pName)parameter=cgGetNamedEffectParameter(m_pEffect, m_pName);}
		virtual ~stx_VariableCgGL_1_1(){}
		stx_VariableCgGL_1_1(const stx_VariableCgGL_1_1& rhs)
		{
            		m_sName=rhs.m_sName;
			m_pEffect=rhs.m_pEffect;
		}

	void SetResourceArray( ShaderResourceViewID* id, unsigned int startindex, unsigned int aN )
	{
		for(unsigned int i=startindex;i<aN;i++)
		{
			SetTexture(id[i]); // sampler name ???
		}
	}
	void SetResource( ShaderResourceViewID id ){SetTexture(id);}
        void SetFloat( float aValue )
        {
		cgSetParameter1f(parameter, aValue);
        }
        void SetMatrix( float* aValue )
        {
		cgSetMatrixParameterfr(parameter, aValue);
        }
        void SetMatrix( D3DXFROMWINEMATRIX* aValue )
        {
		cgSetMatrixParameterfr(parameter, (float*)aValue);
        }
        void SetVector( D3DXFROMWINEVECTOR4* aValue )
        {
		cgSetParameter4fv(parameter, (float*)aValue);
        }
        void SetInt( int* aValue )
        {
		cgSetParameter1i(parameter, aValue);
        }
        void SetTexture( TextureID aValue ){}
        void SetBool( bool aValue ){LOG_FNLN;LOG_PRINT("IMPLEMENT\n");}
        void SetValue( void* aPtr, unsigned int aSize ){LOG_FNLN;LOG_PRINT("IMPLEMENT\n");}
        void SetIntVector( int* aValue ){LOG_FNLN;LOG_PRINT("IMPLEMENT\n");}
        void SetFloatVector( float* aValue ){LOG_FNLN;LOG_PRINT("IMPLEMENT\n");}
        void SetFloatVectorArray( float* aValue, unsigned int aS, unsigned int aN ){LOG_FNLN;LOG_PRINT("IMPLEMENT\n");}
	void SetFloatArray( float* aValue, unsigned int aS, unsigned int aN ){LOG_FNLN;LOG_PRINT("IMPLEMENT\n");}
        void SetMatrixArray( void* aPtr, unsigned int aSize, unsigned int aN){LOG_FNLN;LOG_PRINT("IMPLEMENT\n");}
	};
	struct stx_TechniqueCgGL_1_1
	{
		stx_TechniqueCgGL_1_1(stx_EffectCgGL_1_1* pEffect=0){m_pEffect=pEffect;m_pTechnique=0;}
		virtual ~stx_TechniqueCgGL_1_1(){}
		stx_TechniqueCgGL_1_1(const stx_TechniqueCgGL_1_1& rhs)
		{
			m_pEffect=rhs.m_pEffect;
			m_pTechnique=rhs.m_pTechnique;
			m_sName=rhs.m_sName;
			m_vPasses=rhs.m_vPasses;
		}
		void GetDesc( TECHNIQUE_DESC* techDesc ){if(techDesc)techDesc->Passes=m_vPasses.size();}
		stx_PassCgGL_1_1* GetPassByIndex(unsigned int aI)
		{
			if((aI+1)>m_vPasses.size())
				return 0;
			return &m_vPasses[aI];
		} // ???
		stx_PassCgGL_1_1* GetPassByName(const std::string& aName)
		{
			int idx=-1;
			for(unsigned int i=0;i<m_vPasses.size();i++)
			{
				if(m_vPasses[i].m_sName==aName)
				{
					idx=i;
					break;
				}	
			}
			if(idx>-1)
				return &m_vPasses[idx];
			return 0;
		} // ???
		stx_EffectCgGL_1_1* m_pEffect;
		std::string m_sName;
		CGtechnique m_pTechnique;
		std::vector<stx_PassCgGL_1_1> m_vPasses;
		std::vector<stx_VariableCgGL_1_1> m_vVariables;
	};
	std::vector<stx_EffectCgGL_1_1::stx_PassCgGL_1_1::stx_TextureCgGL_1_1> m_vTextures;
	std::vector<stx_TechniqueCgGL_1_1> m_vTechniques;
	std::string m_sTechnique;
	int  m_iTechnique;
	std::string m_sFileName;
	CGpass pass;
	CGeffect effect;
public:
	stx_EffectCgGL_1_1():stx_Effect(){m_iTechnique=-1;}
	stx_EffectCgGL_1_1(const std::string& aFileName, const char* aDefines=0);
	virtual ~stx_EffectCgGL_1_1(){}
	stx_EffectCgGL_1_1(const stx_EffectCgGL_1_1& rhs)
	{
		m_sFileName=rhs.m_sFileName;
		m_sTechnique=rhs.m_sTechnique;
		m_iTechnique=rhs.m_iTechnique;
		m_vTechniques=rhs.m_vTechniques;
		m_vTextures=rhs.m_vTextures;
		pass=rhs.pass;
		effect=rhs.effect;
	}
        void SetValue( const char* aName, void* aPtr, unsigned int aSize ){LOG_FNLN;LOG_PRINT("IMPLEMENT\n");}
        void SetMatrix( const char* aName, D3DXFROMWINEMATRIX* aValue )
        {
	        CGparameter parameter = cgGetNamedEffectParameter(effect, aName);
		cgSetMatrixParameterfr(parameter, (float*)aValue);
        }
        void SetVector( const char* aName, D3DXFROMWINEVECTOR4* aValue )
        {
	        CGparameter parameter = cgGetNamedEffectParameter(effect, aName);
		cgSetParameter4fv(parameter, (float*)aValue);
        }
        void SetFloat( const char* aName, float* aValue )
        {
	        CGparameter parameter = cgGetNamedEffectParameter(effect, aName);
		cgSetParameter1f(parameter, *aValue);
        }
	void SetBool( const char* aName, bool aValue ){LOG_FNLN;LOG_PRINT("IMPLEMENT\n");}
	void SetTexture( const char* aName, TextureID aValue ){}

	void SetTechnique( const char* aName );

		stx_VariableCgGL_1_1* GetVariableByName( const char* aName )
		{
			if(!aName)
				return 0;
			if(m_iTechnique<0)
				return 0;
			if(m_iTechnique>=m_vTechniques.size())
				return 0;
			int idx=-1;
			for(unsigned int i=0;i<m_vTechniques[m_iTechnique].m_vVariables.size();i++)
			{
				if(m_vTechniques[m_iTechnique].m_vVariables[i].m_sName==std::string(aName))
				{
					idx=i;
					break;
				}	
			}
			if(idx>-1)
				return &m_vTechniques[m_iTechnique].m_vVariables[idx];
			return 0;
		} // ???
		
		stx_TechniqueCgGL_1_1* GetTechniqueByName( const char* aName )
		{
			if(!aName)
				return 0;
			int idx=-1;
			for(unsigned int i=0;i<m_vTechniques.size();i++)
			{
				if(m_vTechniques[i].m_sName==std::string(aName))
				{
					idx=i;
					break;
				}	
			}
			if(idx>-1)
			{
				// ??? m_vTechniques[idx].m_pTechnique=m_pEffect->GetTechniqueByName( "aName" );
				return &m_vTechniques[idx];
			}
			return 0;
		} // ???


  /* 
  pass = cgGetFirstPass(myCgTechnique);
  while (pass) {
    cgSetPassState(pass);
    drawFlatPatch(myTorusSides, myTorusRings);
    cgResetPassState(pass);
    pass = cgGetNextPass(pass);
  } */
	void Begin( unsigned int *cPasses, unsigned int aValue )
	{
		*cPasses=0;
  		CGpass pass_ = cgGetFirstPass(m_vTechniques[m_iTechnique].m_pTechnique);
  		while (pass_) {
			*cPasses++;
    			pass_ = cgGetNextPass(pass_);
  		}
	}
	void BeginPass( unsigned int iPass )
	{
		const char *name=m_vTechniques[m_iTechnique].m_vPasses[iPass].m_sName.c_str();
		pass=cgGetNamedPass(m_vTechniques[m_iTechnique].m_pTechnique, name);
    		cgSetPassState(pass);
	}
        void EndPass(){cgResetPassState(pass);}
        void End(){}
};
#endif
stx_Effect* RendererCgGL_1_1::addEffectFromFile(const std::string& aFileName, const char* aDefines)
{
	return new stx_EffectCgGL_1_1(aFileName, aDefines);
}

#endif
#if 0
		struct ShaderCg : public SShader
{
virtual void DumpUniforms();
virtual CGCGbool interestingCgProgramParameter(CGparameter parameter)
{
  return cgIsParameterReferenced(parameter);
}
#if 1
virtual CGCGbool interestingCgProgramParameters(CGprogram program, CGenum nameSpace)
{
  CGparameter parameter;
  for (parameter = cgGetFirstParameter(program, nameSpace);
    parameter;
    parameter = cgGetNextParameter(parameter)) {
      if (interestingCgProgramParameter(parameter)) {
	return CGCG_TRUE;
      }
    }
    return CGCG_FALSE;
}
#endif
	virtual void saveCgProgram(CGprogram program, CGCGbool unabridged,
									std::vector<ShaderParameter*>& v,
									std::vector<std::string>& s,
									std::vector<int>& i,
  std::vector<int>& tu,
  int& tui);
virtual void saveCgPrograms(CGprogram program,CGcontext context, CGCGbool unabridged,
									std::vector<ShaderParameter*>& v,
									std::vector<std::string>& s,
									std::vector<int>& i,
  std::vector<int>& tu,
  int& tui);
	virtual void SetInt( const char* n, const int f){setShaderConstant1i(n,f);}
	virtual void SetFloat( const char* n, const float f){setShaderConstant1f(n,f);}
	virtual void SetMatrix( const char* n, const mat4* f)
	{
		setShaderConstant4x4f(n,*f);
		//setMVPMatrix(n,f);//???
	}
	virtual void SetVector( const char* n, const float4 f){setShaderConstant4f(n,f);}
	virtual void setShaderConstant1i(const char *name, const int constant);
	virtual void setShaderConstant1f(const char *name, const float constant);
	virtual void setShaderConstant2f(const char *name, const vec2 &constant);
		virtual void setShaderConstant3f(const char *name, const vec3 &constant);
	virtual void setShaderConstant4f(const char *name, const vec4 &constant);
	virtual void setShaderConstant4x4f(const char *name, const mat4 &constant);
	virtual void setShaderConstant1i(const int idx, const int constant);
	virtual void setShaderConstant1f(const int idx, const float constant);
	virtual void setShaderConstant2f(const int idx, const vec2 &constant);
		virtual void setShaderConstant3f(const int idx, const vec3 &constant);
	virtual void setShaderConstant4f(const int idx, const vec4 &constant);
	virtual void setShaderConstant4x4f(const int idx, const mat4 &constant);
	virtual void setShaderConstant1i(ShaderParameter* param, const int constant);
	virtual void setShaderConstant1f(ShaderParameter* param, const float constant);
	virtual void setShaderConstant2f(ShaderParameter* param, const vec2 &constant);
		virtual void setShaderConstant3f(ShaderParameter* param, const vec3 &constant);
	virtual void setShaderConstant4f(ShaderParameter* param, const vec4 &constant);
	virtual void setShaderConstant4x4f(ShaderParameter* param, const mat4 &constant);

	bool vertexS;
	CGeffect m_CgEffect;
    CGtechnique m_CgTechnique;
	CGprofile m_VProfile;
	CGprofile m_FProfile;
	CGprogram m_ProgramV;
	CGprogram m_ProgramF;
	std::string m_ProgramVN;
	std::string m_ProgramFN;
	int vtui,ftui;
	ShaderCg():SShader(),vtui(0),ftui(0),vertexS(false){}
	virtual ~ShaderCg();
	/*
	virtual ShaderID instance(const char* filename)=0;
	virtual ShaderID instance(const char* fileVS,const char* fileFS,const char* mainVS,const char* mainFS)=0;
	virtual ShaderID instanceTxt(const char* txtVS,const char* txtFS,const char* mainVS,const char* mainFS)=0;
*/
	virtual const char *parameterClassString(CGparameterclass pc);
virtual void dumpCgParameterInfo(CGparameter parameter, int unabridged);
virtual void dumpCgProgramParameter(CGparameter parameter, CGCGbool unabridged);
virtual void dumpCgProgramParameters(CGprogram m_Program,
				    CGenum nameSpace,
				    char const *nameSpaceName,
				    CGCGbool unabridged);
virtual void saveCgVProgramParameters();
virtual void saveCgFProgramParameters();
virtual void saveCgParameterInfo(CGparameter parameter, int unabridged,
									std::vector<ShaderParameter*>& v,
									std::vector<std::string>& s,
									std::vector<int>& i,
  std::vector<int>& tu,
  int& tui
									);
virtual void saveCgProgramParameter(CGparameter parameter, CGCGbool unabridged,
									std::vector<ShaderParameter*>& v,
									std::vector<std::string>& s,
									std::vector<int>& i,
  std::vector<int>& tu,
  int& tui									);
virtual void saveCgProgramParameters(CGprogram m_Program,
				    CGenum nameSpace,
				    char const *nameSpaceName,
				    CGCGbool unabridged,
									std::vector<ShaderParameter*>& v,
									std::vector<std::string>& s,
									std::vector<int>& i,
  std::vector<int>& tu,
  int& tui									);
};
#endif

