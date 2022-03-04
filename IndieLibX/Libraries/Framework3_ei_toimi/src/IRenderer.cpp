/*
  Copyright (c) 2022 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#if 0
#include <Framework3/importgl.h>
#include <Framework3/RendererGLSLGL_1_1.h>
#ifdef __FrameworkGLSLGL_1_1_H__
#include <Framework3/Hlsl2Glsl_Translate.h>
#endif
#include <Framework3/GLShaders.h>

//define STRUCT__ IRenderer
#define STRUCT__ RendererGLSLGL_1_1

		int samplerCompareGL2__IRenderer(const void *sampler0, const void *sampler1)
		{
			return strcmp(((XSampler *) sampler0)->name.c_str(), ((XSampler *) sampler1)->name.c_str());
		}
		ConstantType getConstantType__IRenderer(GLenum type)
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

		bool samplerCompGL2std__IRenderer(const XSampler & s0, const XSampler &s1)
		{
       if(!(s0.name.c_str()&& s1.name.c_str()))return false;
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
		}
		bool constantCompGL2std__IRenderer(const ConstantShaderGLSLGL3 & s0, const ConstantShaderGLSLGL3 &s1)
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
		bool attribCompGL2std__IRenderer(const AttribShaderGLSLGL3 & s0, const AttribShaderGLSLGL3 &s1)
		{
       if(!(s0.name.c_str()&& s1.name.c_str()))return false;
       int i=strcmp(s0.name.c_str(), s1.name.c_str());
       if(i>0)
	      return false;
       if(i<0)
	      return true;
       return false;
		}

ShaderID STRUCT__::addGLSLShader(
const char *vsText0, const char *gsText0, const char *fsText0, const char *csText0, const char *hsText0, const char *dsText0,
const char *vsName, const char *gsMain, const char *psName, const char *csMain, const char *hsMain, const char *dsMain,
                                            	const unsigned int flags)
		{
		//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    const char* header=0;
	std::string vsText="";
	std::string fsText="";
	std::string gsText="";
	std::string csText="";
	std::string hsText="";
	std::string dsText="";
	char versionString[16];
	#if 0
	const char *version = (const char *) glGetString(GL_VERSION);
	if (version)
	{
		int major = atoi(version);

		int minor = atoi(stx_strchr(version, '.') + 1);
		STX_PRINT("GL_VERSION: %d.%d\n", major, minor);
	}
	version = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);
	if (version)
	{
		int major = atoi(version);
		int minor = atoi(stx_strchr(version, '.') + 1);
		STX_PRINT("GL_SHADING_LANGUAGE_VERSION: %d.%d\n", major, minor);
	}
	#else
	versionString[0]='\0';
	#endif

	const char * def=versionString;
	if (def) vsText.append(def);
	if (def) fsText.append(def);
	if (def) gsText.append(def);
	if (def) csText.append(def);
	if (def) hsText.append(def);
	if (def) dsText.append(def);

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
		printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		printf("\nvsText:\n%s\n", vsText.c_str());
		printf("\nfsText:\n%s\n", fsText.c_str());
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
		//printf("\nvsText:\n%s\n", vsText.c_str());
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
			{
            			char* s=new char(32768);
				s[0]='\0';
            			glGetInfoLogARB(shaderGL1_1.shader[eVertexShader],32768,NULL,s);
            			if(s) if(stx_strlen(s)) printf("Compile Log: %s\n", s);
            			delete[] s;
			}
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
			//printf("\nfsText:\n%s\n", fsText.c_str());
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
			{
            			char* s=new char(32768);
				s[0]='\0';
            			glGetInfoLogARB(shaderGL1_1.shader[ePixelShader],32768,NULL,s);
            			if(s) if(stx_strlen(s)) printf("Compile Log: %s\n", s);
            			delete[] s;
			}
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
			{
	    			char* s=new char(32768);
				s[0]='\0';
            			glGetInfoLogARB(shaderGL1_1.program,32768,NULL,s);
            			if(s) if(stx_strlen(s)) printf("Link Log: %s\n", s);
            			delete[] s;
			}
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
									shaderGL1_1.uniforms[nUniforms].type = getConstantType__IRenderer(type);
									shaderGL1_1.uniforms[nUniforms].nElements = size;
									shaderGL1_1.uniforms[nUniforms].name=std::string(name);									stx_Variables::AddUniform(shaders.size(), name, size, getConstantType__IRenderer(type)); // ???
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
#if 0
{
	printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	printf("\nvsText:\n%s\n", vsText.c_str());
	printf("\nfsText:\n%s\n", fsText.c_str());
}
#endif
			return shaders.size()-1;
				}
			}
#if 0
{
	printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	printf("\nvsText:\n%s\n", vsText.c_str());
	printf("\nfsText:\n%s\n", fsText.c_str());
}
#endif
			return SHADER_NONE;
		}
//}
#endif
ShaderID STRUCT__::addShader(  const char* shaderText_, 
                                        const char* vsMain, 
                                        const char* fsMain,
                             	const char *defines, 
                                        const unsigned int flags)
{
	ShaderID res = SHADER_NONE;
	std::string shaderText;
	if(shaderText_)
		shaderText=shaderText_;
	std::string header, vsStr2, fsStr2;
	std::size_t foundGLSL = shaderText.find("[GLSL]");
	std::size_t foundHLSL = shaderText.find("[HLSL]");
	std::size_t foundVS = shaderText.find("[Vertex shader]");
	std::size_t foundPS = shaderText.find("[Fragment shader]");
#if 0
	printf("\nfoundGLSL=%d\n", foundGLSL);
	printf("foundHLSL=%d\n", foundHLSL);
	printf("foundVS=%d\n", foundVS);
	printf("foundPS=%d\n", foundPS);
	printf("std::string::npos=%d\n", std::string::npos);
#endif
#if 1
	char* fsMain_="main";
	if		(((std::string::npos==foundGLSL) &&
		 	  (std::string::npos==foundHLSL) &&
			  (std::string::npos==foundVS) &&
			  (std::string::npos==foundPS)) ||
			 ((std::string::npos!=foundGLSL) &&
		 	  (std::string::npos==foundHLSL) &&
			  (std::string::npos==foundVS) &&
			  (std::string::npos==foundPS)))
	{
		//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		#if 0
		if(	std::string::npos!=shaderText.find("void mainImage("))
			fsMain_="mainImage";
		#endif
		vsStr2.append(	"struct VsOut {\n"
				"    vec4 position;\n"
				"    vec2 uv;\n"
				"};\n"
				"struct VsIn2 {\n"
				"    vec2 position;\n"
				"    vec2 uv;\n"
				"};\n"
				"struct VsIn3 {\n"
				"    vec3 position;\n"
				"    vec3 Normal;\n"
				"    vec3 Binormal;\n"
				"    vec3 Tangent;\n"
				"    vec3 Color;\n"
				"    vec2 uv;\n"
				"};\n"
				"varying vec2 xlv_TEXCOORD0;\n"
				"uniform mat4 worldViewProj;\n"
				"//layout(location = 0) in mat4  worldViewProj;\n"
				"VsOut main2( in VsIn2 In ) {\n"
				"    VsOut Out = VsOut(vec4(0.0, 0.0, 0.0, 0.0), vec2(0.0, 0.0));\n"
				"    Out.position = vec4( In.position.x, In.position.y, 0.0, 1.0);\n"
				"    Out.uv.x = In.uv.x;\n"
				"    Out.uv.y = In.uv.y;\n"
				"    return Out;\n"
				"}\n"
				"//layout(location = 0) out vec2  xlv_TEXCOORD0;\n"
				"void main() {\n"
				"    VsOut xl_retval;\n"
				"    VsIn2 xlt_In;\n"
				"    xlt_In.position = vec2(gl_Vertex);\n"
				"    xlt_In.uv = vec2(gl_MultiTexCoord0);\n"
				"    xl_retval = main2( xlt_In);\n"
				"    gl_Position = vec4(xl_retval.position);\n"
				"    xlv_TEXCOORD0 = vec2(xl_retval.uv);\n"
				"}\n");
		fsStr2.append(  "uniform vec3      iResolution;\n"
				"uniform vec4      iMouse;\n"
				"uniform float     iTime;\n"
				"uniform vec3      resolution;\n"
				"uniform vec4      mouse;\n"
				"uniform float     time;\n"
				"uniform float     iGlobalTime;\n"
				"uniform vec4      iDate;\n"
				"uniform float     iSampleRate;\n"
				"uniform vec3      iChannelResolution[4];\n"
				"uniform float     iChannelTime[4];\n"
				"uniform vec2      ifFragCoordOffsetUniform;\n"
				"uniform float     iTimeDelta;\n"
				"uniform int       iFrame;\n"
				"uniform float     iFrameRate;\n"
				"struct Channel {\n"
				"    vec3  resolution;\n"
				"    float   time;\n"
				"};\n"
				"varying vec2 xlv_TEXCOORD0;\n"
				"#define mainImage main\n");
		if(std::string::npos!=foundGLSL)
			fsStr2.append(shaderText.c_str()+6);
		else
			fsStr2.append(shaderText);
#if 0
		//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		printf("\nvs:\n%s\n", vsStr2.c_str());
		printf("\nfs:\n%s\n", fsStr2.c_str());
		printf("\nfsMain:\n%s\n", fsMain);
#endif
		res=	//addHLSLShaderVrtl
			addGLSLShader
			( vsStr2.c_str(), 0, fsStr2.c_str(), 0, 0, 0,
                        "main", 0, fsMain_, 0, 0, 0, flags);
		return res;
	}
#endif
	if((foundVS!=std::string::npos)&&(foundPS!=std::string::npos))
	{
		//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	std::string stringToBeSplitted=shaderText;
	std::vector<std::string> delimeters;
	delimeters.push_back("[Vertex shader]");
	delimeters.push_back("[Fragment shader]");
	int startIndex = 0;
	int  endIndex = 0;
	if( (endIndex = stringToBeSplitted.find(delimeters[0], startIndex)) < stringToBeSplitted.size() )
	{
		std::string val = stringToBeSplitted.substr(startIndex, endIndex - startIndex);
		header=val;
		vsStr2=header;
		fsStr2=header;
	}
	startIndex = endIndex;
	endIndex = 0;
	if( (endIndex = stringToBeSplitted.find(delimeters[1], startIndex)) < stringToBeSplitted.size() )
	{
		std::string val = stringToBeSplitted.substr(startIndex+delimeters[1].length()-1, endIndex - (startIndex+delimeters[1].length()));
		vsStr2.append(val);
	}
	{
		std::string val = stringToBeSplitted.substr(endIndex+2+delimeters[0].length());
		fsStr2.append(val);
	}
	}
	else if((std::string::npos==foundGLSL) &&
		(std::string::npos!=foundHLSL))
	{
		//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		vsStr2.append(	"#define ROW_MAJOR row_major\n"
				"#define MVPSEMANTIC\n"
				"#define WSIGN +\n"   
				"struct VsOut {\n"
				"    float4 position;\n"
				"    float2 uv;\n"
				"};\n"
				"struct VsIn2 {\n"
				"    float2 position;\n"
				"    float2 uv;\n"
				"};\n"
				"struct VsIn3 {\n"
				"    float3 position;\n"
				"    float3 Normal;\n"
				"    float3 Binormal;\n"
				"    float3 Tangent;\n"
				"    float3 Color;\n"
				"    float2 uv;\n"
				"};\n"
				"float4x4 worldViewProj;\n"
				"VsOut main2( in VsIn2 In ) {\n"
				"    VsOut Out = VsOut(float4(0.0, 0.0, 0.0, 0.0), float2(0.0, 0.0));\n"
				"    Out.position = float4( In.position.x, In.position.y, 0.0, 1.0);\n"
				"    Out.uv.x = In.uv.x;\n"
				"    Out.uv.y = 1.0-In.uv.y;\n"
				"    return Out;\n"
				"}\n"
				"VsOut main() {\n"
				"    VsOut xl_retval;\n"
				"    VsIn2 xlt_In;\n"
				"    xlt_In.position = float2(gl_Vertex);\n"
				"    xlt_In.uv = float2(gl_MultiTexCoord0);\n"
				"    return main2( xlt_In);\n"
				"}\n");
		fsStr2.append(  "float3      iResolution;\n"
				"float4      iMouse;\n"
				"float     iTime;\n"
				"float3      resolution;\n"
				"float4      mouse;\n"
				"float     time;\n"
				"float     iGlobalTime;\n"
				"float4      iDate;\n"
				"float     iSampleRate;\n"
				"float3      iChannelResolution[4];\n"
				"float     iChannelTime[4];\n"
				"float2      ifFragCoordOffsetUniform;\n"
				"float     iTimeDelta;\n"
				"int       iFrame;\n"
				"float     iFrameRate;\n"
				"struct Channel {\n"
				"    float3  resolution;\n"
				"    float   time;\n"
				"};\n"
				"#define mainImage main\n");
		if(std::string::npos!=foundHLSL)
			fsStr2.append(shaderText.c_str()+6);
		else
			fsStr2.append(shaderText);
	}
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    	res=addHLSLShaderVrtl(  vsStr2.c_str(), 0, fsStr2.c_str(), 0, 0, 0,
                        	vsMain, 0, fsMain, 0, 0, 0, flags);
	return res;
}
#endif

