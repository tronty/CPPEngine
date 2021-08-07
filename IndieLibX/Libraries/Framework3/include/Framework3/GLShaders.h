#ifndef __GLShaders__
#define __GLShaders__
/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/importgl.h>
#include <Framework3/RendererGLDS.h>

inline GLuint compileGLShaders(const char* vs, const char* fs)
{
	std::string vsText_="";
	std::string fsText_="";
#if !defined(USE_HLSL_SHADERS)
	#if 1
	vsText_.append("#version 440 core\n");
	fsText_.append("#version 440 core\n");
	#else
	if((strstr(vsText_.c_str(), "#version ")==0) && (strstr(fsText_.c_str(), "#version ")==0))
	{
	const char *version = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);
				checkGlError("");
	char versionString[16];
	if (version)
	{
		int major = atoi(version);
		int minor = atoi(stx_strchr(version, '.') + 1);
		stx_snprintf(versionString, 16, "#version %d%d\n", major, minor);
	}

	vsText_.append(versionString);
	fsText_.append(versionString);
	}
	#endif
#endif
	vsText_.append(vs);
	fsText_.append(fs);
	const char* vsTextp=vsText_.c_str();
	const char* fsTextp=fsText_.c_str();

	LOG_FNLN;
	LOG_PRINT("\nvsTextp:\n%s\n", vsTextp);
	LOG_PRINT("\nfsTextp:\n%s\n", fsTextp);

	GLuint program = glCreateProgram();
				checkGlError("");
        GLuint cfs = glCreateShader(GL_FRAGMENT_SHADER);
				checkGlError("");
	const char** fsTextpp=&fsTextp;
        glShaderSource(cfs, 1, fsTextpp, NULL);
				checkGlError("");
        glCompileShader(cfs);
				checkGlError("");

        GLuint cvs = glCreateShader(GL_VERTEX_SHADER);
				checkGlError("");
	const char** vsTextpp=&vsTextp;
        glShaderSource(cvs, 1, vsTextpp, NULL);
				checkGlError("");
        glCompileShader(cvs);
				checkGlError("");

        glAttachShader(program, cvs);
				checkGlError("");
        glAttachShader(program, cfs);
				checkGlError("");

        glLinkProgram(program);
				checkGlError("");
	return program;
}
inline void reflectGLShader(GLuint aprogram, std::vector<XSampler>& samplers, std::vector<ConstantShaderGLSLGL3>& uniforms)
{
	glUseProgram(aprogram);
				checkGlError("");
	GLint uniformCount, maxLength;
	glGetProgramiv(aprogram, GL_ACTIVE_UNIFORMS, &uniformCount);
				checkGlError("");
	glGetProgramiv(aprogram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
				checkGlError("");
					
	samplers.resize(uniformCount);
	uniforms.resize(uniformCount);
					
	int nSamplers = 0;
	int nUniforms = 0;

	std::vector<char> bbuf(maxLength);
	char *name = (char *)&bbuf[0];

#if defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
	GLint prog=handle_to_uint(&aprogram);
#elif defined(__APPLE__)
	GLint prog=handle_to_uint(&aprogram);
#else
	GLint prog=aprogram;
#endif
	for(unsigned int i = 0; i < uniformCount; i++)
	{
		GLenum type;
		GLint length, size;
		#if defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
		glGetActiveUniform(handle_to_uint(&aprogram), i, maxLength, &length, &size, &type, name);
		#elif defined(__APPLE__)
		glGetActiveUniform(handle_to_uint(&aprogram), i, maxLength, &length, &size, &type, name);
		#else
		glGetActiveUniform(aprogram, i, maxLength, &length, &size, &type, name);
		#endif
				checkGlError("");
		if (type >= GL_SAMPLER_2D && type <= GL_SAMPLER_CUBE)
		{
			#if defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
			GLint location = glGetUniformLocation(handle_to_uint(&aprogram), name);
			#elif defined(__APPLE__)
			GLint location = glGetUniformLocation(handle_to_uint(&aprogram), name);
			#else
			GLint location = glGetUniformLocation(aprogram, name);
			#endif
				checkGlError("");
			glUniform1i(location, nSamplers);
				checkGlError("");
			samplers[nSamplers].location = nSamplers;
			samplers[nSamplers].name=std::string( name);
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
					
					#if defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
					uniforms[nUniforms].location = glGetUniformLocation(handle_to_uint(&aprogram), name);
					#elif defined(__APPLE__)
					uniforms[nUniforms].location = glGetUniformLocation(handle_to_uint(&aprogram), name);
					#else
					uniforms[nUniforms].location = glGetUniformLocation(aprogram, name);
					#endif
				checkGlError("");
					uniforms[nUniforms].type = getConstantType_GL(type);
					uniforms[nUniforms].nElements = size;
					uniforms[nUniforms].name=std::string( name);
					nUniforms++;
				}
				else if (bracket != 0 && bracket[1] > '0')
				{
					*bracket = '\0';
					for (int i = nUniforms - 1; i >= 0; i--)
					{
						if (strcmp(uniforms[i].name.c_str(), name) == 0)
						{
							int index = atoi(bracket + 1) + 1;
							if (index > uniforms[i].nElements)
								uniforms[i].nElements = index;
						}
					}
				}
			}
		}
	}
}
#if !defined(VULKAN) && !defined(METAL) && !defined(D3D12) && !defined(D3D11) && !defined(D3D10) && !defined(D3D9) 
/*
inline void setupSamplerGL(GLenum glTarget, const SamplerStateGL &ss)
{
	glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, ss.wrapS);
	//if (glTarget != GL_TEXTURE_1D) glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, ss.wrapT);
	#if !defined(GL_TEXTURE_3D_OES)
	if (glTarget == GL_TEXTURE_3D)
		glTexParameteri(glTarget, GL_TEXTURE_WRAP_R, ss.wrapR);
	#else
	if (glTarget == GL_TEXTURE_3D_OES)
		glTexParameteri(glTarget, GL_TEXTURE_WRAP_R_OES, ss.wrapR);
	#endif
	glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, ss.magFilter);
   	glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, ss.minFilter);
   	if (ss.aniso > 1 && GL_EXT_texture_filter_anisotropic_supported)
		glTexParameteri(glTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, ss.aniso);
}
*/
inline GLuint addTexture(Image3* img, const bool useMips, const SamplerStateID samplerState, unsigned int flags)
{
#ifdef GL_RGBA8_OES
		static const GLint internalFormats_GL[] =
		{

		0,
		0,//???GL_INTENSITY8,
		GL_LUMINANCE_ALPHA,
		GL_RGB8_OES,
		GL_RGBA8_OES,
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
		GL_DEPTH_COMPONENT24_OES,
		GL_DEPTH24_STENCIL8_OES,
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
		static const GLenum srcFormats_GL[] =
		{
		0, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA
		}
		;
		static const GLenum srcTypes_GL[] =
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
		GL_UNSIGNED_INT_24_8_OES,
		0, 0,
		0,
		0,
		0,
		0,
		}
		;
	static const char* srcFormats_GL_[] =
	{
		0, "GL_LUMINANCE", "GL_LUMINANCE_ALPHA", "GL_RGB", "GL_RGBA"
	};
	static const char* srcTypes_GL_[] =
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
		"GL_UNSIGNED_INT_24_8_OES",
		0, 0,
		0,
		0,
		0,
		0,
	};
	static const char* internalFormats_GL_[] =
	{
		0,
		0,//???"GL_INTENSITY8",
		"GL_LUMINANCE_ALPHA",
		"GL_RGB8_OES",
		"GL_RGBA8_OES",
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
		"GL_DEPTH_COMPONENT24_OES",
		"GL_DEPTH24_STENCIL8_OES",
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
	};
#else
		static const GLint internalFormats_GL[] =
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
		static const GLenum srcFormats_GL[] =
		{
		0, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA
		}
		;
		static const GLenum srcTypes_GL[] =
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
	static const char* srcFormats_GL_[] =
	{
		0, "GL_LUMINANCE", "GL_LUMINANCE_ALPHA", "GL_RGB", "GL_RGBA"
	};
	static const char* srcTypes_GL_[] =
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
	};
	static const char* internalFormats_GL_[] =
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
	};
#endif

	GLuint texglTexID=0;
	GLuint texglTarget=0;			
	if(useMips&&img->getMipMapCount()<2)
		img->createMipMaps();
			
	const FORMAT format = img->getFormat();

	GLenum srcFormat = srcFormats_GL[getChannelCount(format)];
	GLenum srcType = srcTypes_GL[format];
	GLint internalFormat = internalFormats_GL[format];

	const char* srcFormat_ = srcFormats_GL_[getChannelCount(format)];
	const char* srcType_ = srcTypes_GL_[format];
	const char* internalFormat_ = internalFormats_GL_[format];
			
	if ((flags & HALF_FLOAT) != 0 && format >= FORMAT_I32f && format <= FORMAT_RGBA32f)
		internalFormat = internalFormats_GL[format - (FORMAT_I32f - FORMAT_I16f)];
			
	//tex.lod = (samplerStates[samplerState]).lod;
	////texglTarget = img->isCube()? GL_TEXTURE_CUBE_MAP : img->is3D()? GL_TEXTURE_3D : img->is2D()? GL_TEXTURE_2D : 0;//???GL_TEXTURE_1D;
	//texglTarget = img->is2D()? GL_TEXTURE_2D : 0;//???GL_TEXTURE_1D;
			
	glGenTextures(1, &texglTexID);
				checkGlError("");
	glBindTexture(texglTarget, texglTexID);
				checkGlError("");
	// ??? setupSamplerGL(texglTarget, samplerStates[samplerState]);
	//tex.samplerState = samplerState;
	const ubyte *src;
	int mipMapLevel = 0;
	while ((src = (useMips? img->getPixels(mipMapLevel) : img->getPixels())) != 0)
	{
		if (img->isCube())
		{
			int size = img->getMipMappedSize(mipMapLevel, 1) / 6;
			for (unsigned int i = 0; i < 6; i++)
			{
				if (isCompressedFormat(format))
					glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, size, (BYTE *)(src + i * size));
				else
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, (BYTE *)(src + i * size));
				checkGlError("");		
			}
		}
		else if (img->is3D())
		{
			if (isCompressedFormat(format)){
				glCompressedTexImage3D(texglTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), img->getDepth(mipMapLevel), 0, img->getMipMappedSize(mipMapLevel, 1), (BYTE *)(src));checkGlError("");}
			else{
				glTexImage3D(texglTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), img->getDepth(mipMapLevel), 0, srcFormat, srcType, (BYTE *)(src));
				checkGlError("");}
		}
		else if (img->is2D())
			if (isCompressedFormat(format)){
				glCompressedTexImage2D(texglTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, img->getMipMappedSize(mipMapLevel, 1), (BYTE *)(src));checkGlError("");}
			else{
				glTexImage2D(texglTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), img->getHeight(mipMapLevel), 0, srcFormat, srcType, (BYTE *)(src));
				checkGlError("");}
		else{
			glTexImage1D(texglTarget, mipMapLevel, internalFormat, img->getWidth(mipMapLevel), 0, srcFormat, srcType, (BYTE *)(src));
				checkGlError("");}
		if(!useMips)
			break;
		mipMapLevel++;
		}
		glBindTexture(texglTarget, 0);
				checkGlError("");
		return texglTexID;
	}
#endif
#endif

