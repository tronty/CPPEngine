#ifndef __RendererGLDS_H__
#define __RendererGLDS_H__

/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*
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

#include <STX/STX.h>
#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>
#ifdef HAS_GLCG
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#endif

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *) 0 + (i))
#endif
/*
extern int GL_ARB_shading_language_100_supported;
extern int GL_ARB_fragment_shader_supported;
extern int GL_ARB_fragment_program_supported;
extern int GL_ARB_draw_buffers_supported;
extern int GL_EXT_texture_filter_anisotropic_supported;
*/
#define GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI 0x8837

#if !defined(D3D11) && !defined(D3D10) && !defined(D3D9)
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;

typedef int GLsizei;

#endif
#ifndef RENDERERAPI
#define RENDERERAPI
#endif
struct RENDERERAPI AttribShaderGLSLGL3 : public Constant {
	AttribShaderGLSLGL3() : Constant(){name="";base=0;location=0;unit=0;}
	virtual ~AttribShaderGLSLGL3(){}
	AttribShaderGLSLGL3(const AttribShaderGLSLGL3& rhs)
	{
		name=rhs.name;
        	base=rhs.base;
        	location=rhs.location;
		unit=rhs.unit;
	}
        std::string name;
        //std::vector<ubyte>
	float* base;
        //unsigned int index;
        unsigned int location;
        unsigned int unit;
        //unsigned int size;
	//unsigned int offset;
        //ConstantType type;
		//CGparameterclass type;
        //int nElements;
        //bool dirty;
};
struct RENDERERAPI ConstantShaderGLSLGL3 : public Constant {
	ConstantShaderGLSLGL3() : Constant()
	{
		name="";
		data.resize(0);
		location=0;
		//type;
		nElements=0;
        	dirty=false;
	}
	virtual ~ConstantShaderGLSLGL3(){}
	ConstantShaderGLSLGL3(const ConstantShaderGLSLGL3& rhs)
	{
		name=rhs.name;
        	data=rhs.data;
		location=rhs.location;
		type=rhs.type;
		nElements=rhs.nElements;
        	dirty=rhs.dirty;
	}
        std::string name;
        std::vector<ubyte> data;
        unsigned int location;
        ConstantType type;
		//CGparameterclass type;
        int nElements;
        bool dirty;
};

#ifdef __APPLE__
#ifdef QT_BUILD
typedef void* GLhandleARB;
#else
//typedef void* GLhandleARB;
typedef void* GLhandleARB;
#endif
#else
typedef unsigned int GLhandleARB;
#endif

#if defined(__APPLE__)
/** Convert a GLhandleARB to GLuint */
inline GLuint handle_to_uint(GLhandleARB handle)
{
   /* As of glext.h version 20130624, on Mac OS X, GLhandleARB is defined
    * as a pointer instead of an unsigned int.  We use a union here to do
    * the conversion and assume that the bits we care about are in the least
    * significant bits of the handle, and we're on a little-endian system.
    */
   union handle_uint {
      GLhandleARB handle;
      GLuint ui;
   } temp;
   temp.handle = handle;
   return temp.ui;
}
#else
   #define handle_to_uint
#endif

struct ShaderGLSLGL3 : public Shader
{
	ShaderGLSLGL3() : Shader()
    {
        shader.resize(6);
        mvHLSLText.resize(6);
        mvGLSLText.resize(6);
        mvMain.resize(6);
        mvType.resize(6);
    }
    ShaderGLSLGL3(const ShaderGLSLGL3& rhs)
    {
	program=rhs.program;
        shader=rhs.shader;
        mvHLSLText=rhs.mvHLSLText;
        mvGLSLText=rhs.mvGLSLText;
        mvMain=rhs.mvMain;
        mvType=rhs.mvType;
	//attribs=rhs.attribs;
	uniforms=rhs.uniforms;
	samplers=rhs.samplers;
    }
	virtual ~ShaderGLSLGL3(){}

#if defined(__APPLE__) && !(defined(OS_IPHONE) || defined(IPHONE_SIMULATOR))
	GLhandleARB program;
    std::vector<GLhandleARB> shader;
#else
	GLuint program;
    std::vector<GLuint> shader;
#endif
        //std::vector<AttribShaderGLSLGL3  > attribs;
	    std::vector<ConstantShaderGLSLGL3 > uniforms;
        std::vector<XSampler> samplers;
		std::vector<std::string> mvHLSLText;
		std::vector<std::string> mvGLSLText;
        std::vector<std::string> mvMain;
        std::vector<ShaderType> mvType;
};
#if !defined(_DEBUG) || (__APPLE__)
#define glerror
#define checkGlError
#elif 0//defined(ANDROID)
#define glerror
#define checkGlError
#else
#define checkGlError { \
    GLint errGL = glGetError(); \
    if (errGL != GL_NO_ERROR) { \
        LOG_PRINT("GL error:\nfile=%s:\nfn=%s:\nline=%d:\n 0x%08x\n", __FILE__, __FUNCTION__, __LINE__, errGL); \
    } \
}

#define glerror { \
    GLint errGL = glGetError(); \
    if (errGL != GL_NO_ERROR) { \
        LOG_PRINT("GL error:\nfile=%s:\nfn=%s:\nline=%d:\n 0x%08x\n", __FILE__, __FUNCTION__, __LINE__, errGL); \
    } \
}
#endif
#ifndef _MSC_VER
extern int GL_ARB_shading_language_100_supported;
extern int GL_ARB_fragment_shader_supported;
extern int GL_ARB_fragment_program_supported;
extern int GL_ARB_draw_buffers_supported;
extern int GL_EXT_texture_filter_anisotropic_supported;
#endif

//using namespace GL2;
#if 0
struct CTextureProcessorGL: public CTextureProcessor
{
	CTextureProcessorGL(int& id):CTextureProcessor(id){}
	virtual ~CTextureProcessorGL(){}
};
struct CVertexBufferProcessorGL: public CVertexBufferProcessor
{
	CVertexBufferProcessorGL(int& id):CVertexBufferProcessor(id){}
	virtual ~CVertexBufferProcessorGL(){}
};
struct CIndexBufferProcessorGL: public CIndexBufferProcessor
{
	CIndexBufferProcessorGL(int& id):CIndexBufferProcessor(id){}
	virtual ~CIndexBufferProcessorGL(){}
};
#else
//include "ContentLoadersGL2.h"
#endif
//namespace CG
//{
//typedef struct RENDERERAPI _CGparameter *CGparameter;
//};
struct TextureGL: public Texture
{
	TextureGL():Texture()
	{
		glTexID=0;
		glDepthID=0;
		glTarget=0;
		glTargetW=0;
		components=0;
		wrap=0;
		glFormat=0;
		flags=0;
		lod=0.0f;
		mipMapped=false;
		samplerState=-1;
		TU=0;
	}
	virtual ~TextureGL(){}

	//???union {
	GLuint glTexID;
	GLuint glDepthID;
	//???};
		//union {
	GLuint glTarget;
	GLuint glTargetW;
		//};
	GLuint components;
	GLuint wrap;
	GLuint glFormat;
	unsigned int flags;
	float lod;
	bool mipMapped;
	SamplerStateID samplerState;
	int TU;
	BYTE* Lock(int m, int& pitch)//int& slicePitch;
;
	void Unlock(int m)
;
};
struct SamplerStateGL: public SamplerState
{
	SamplerStateGL():SamplerState()
	{
		minFilter=0;
		magFilter=0;
		wrapS=0;
		wrapT=0;
		wrapR=0;
		aniso=0;
		lod=0.0f;
	}
	virtual ~SamplerStateGL(){}
	GLint minFilter;
	GLint magFilter;
	GLint wrapS;
	GLint wrapT;
	GLint wrapR;
	GLint aniso;
	float lod;
};

struct Attrib {
	int stream;
	int size;
	AttributeFormat format;
	int offset;
};
#define MAX_GENERIC 8
#define MAX_TEXCOORD 8
struct VertexFormatGL: public VertexFormat
{
	VertexFormatGL():VertexFormat()
	{
		maxGeneric=0;
        	maxTexCoord=0;
		for(unsigned int i=0;i<MAX_VERTEXSTREAM;i++)
		{
			vertexSize[i]=0;
		}
		shader=-1;
		pBase=0;
	}
	virtual ~VertexFormatGL(){}
        Attrib generic[MAX_GENERIC];
        Attrib texCoord[MAX_TEXCOORD];
        Attrib vertex;
        Attrib normal;
        int vertexSize[MAX_VERTEXSTREAM];
        int maxGeneric;
        int maxTexCoord;
	ShaderID shader;
	BYTE* pBase;
};
struct VertexBufferGL: public VertexBuffer
{
	VertexBufferGL():VertexBuffer()
	{
		vboVB=0;
		size=0;
		data=0;
        	//data.resize(0);
	}
	virtual ~VertexBufferGL(){}
	GLuint vboVB;
	GLsizei size;
	BYTE* Lock();
	void Unlock();
	BYTE* data;
};
struct IndexBufferGL: public IndexBuffer
{
	IndexBufferGL():IndexBuffer()
	{
       		vboIB=0;
        	nIndices=0;
        	indexSize=0;
		data=0;
        	//data.resize(0);
	}
	virtual ~IndexBufferGL(){}
        GLuint vboIB;
        unsigned int nIndices;
        unsigned int indexSize;
	BYTE* Lock();
	void Unlock();
	//std::vector<BYTE> data;
	BYTE* data;
};
struct AlphaStateGL: public AlphaState
{
	AlphaStateGL():AlphaState()
{
		blendEnable=false;
		alphaFunc=0;
	}
	virtual ~AlphaStateGL(){}
	bool blendEnable;
	int alphaFunc;
};
struct BlendStateGL: public BlendState
{
	BlendStateGL():BlendState()
	{
		mask=0;
        	blendEnable=false;
       		alphaEnable=false;
		alphaToCoverageEnable=false;
	}
	virtual ~BlendStateGL(){}
        GLenum srcFactorRGB;
        GLenum dstFactorRGB;
        GLenum srcFactorAlpha;
        GLenum dstFactorAlpha;
        GLenum blendModeRGB;
        GLenum blendModeAlpha;
        int mask;
        bool blendEnable;
        bool alphaEnable;
	bool alphaToCoverageEnable;
};
struct DepthStateGL: public DepthState
{
	DepthStateGL():DepthState()
	{
        	depthFunc=0;
        	depthTest=false;
        	depthWrite=false;
	}
	virtual ~DepthStateGL(){}
        int depthFunc;
        bool depthTest;
        bool depthWrite;
};
struct RasterizerStateGL: public RasterizerState
{
	RasterizerStateGL():RasterizerState()
	{
		cullMode=0;
        	fillMode=0;
       		multiSample=false;
        	scissor=false;
	}
	virtual ~RasterizerStateGL(){}
        int cullMode;
        int fillMode;
        bool multiSample;
        bool scissor;
};

#ifndef GL_FLOAT_MAT4
#define GL_INT					0x1404
#define GL_FLOAT				0x1406
#define GL_FLOAT_VEC2                     0x8B50
#define GL_FLOAT_VEC3                     0x8B51
#define GL_FLOAT_VEC4                     0x8B52
#define GL_INT_VEC2                       0x8B53
#define GL_INT_VEC3                       0x8B54
#define GL_INT_VEC4                       0x8B55
#define GL_BOOL                           0x8B56
#define GL_BOOL_VEC2                      0x8B57
#define GL_BOOL_VEC3                      0x8B58
#define GL_BOOL_VEC4                      0x8B59
#define GL_FLOAT_MAT2                     0x8B5A
#define GL_FLOAT_MAT3                     0x8B5B
#define GL_FLOAT_MAT4                     0x8B5C
#define GL_SAMPLER_1D                     0x8B5D
#define GL_SAMPLER_2D                     0x8B5E
#define GL_SAMPLER_3D                     0x8B5F
#define GL_SAMPLER_CUBE                   0x8B60
#endif

		inline ConstantType getConstantType_GL(GLenum type)
		{
			switch (type)
			{
				case GL_FLOAT:		return CONSTANT_FLOAT;
				case GL_FLOAT_VEC2: return CONSTANT_VEC2;
				case GL_FLOAT_VEC3: return CONSTANT_VEC3;
				case GL_FLOAT_VEC4: return CONSTANT_VEC4;
				case GL_INT:		return CONSTANT_INT;
				case GL_INT_VEC2:	return CONSTANT_IVEC2;
				case GL_INT_VEC3:	return CONSTANT_IVEC3;
				case GL_INT_VEC4:	return CONSTANT_IVEC4;
				case GL_BOOL:	return CONSTANT_BOOL;
				case GL_BOOL_VEC2:	return CONSTANT_BVEC2;
				case GL_BOOL_VEC3:	return CONSTANT_BVEC3;
				case GL_BOOL_VEC4:	return CONSTANT_BVEC4;
				case GL_FLOAT_MAT2: return CONSTANT_MAT2;
				case GL_FLOAT_MAT3: return CONSTANT_MAT3;
				case GL_FLOAT_MAT4: return CONSTANT_MAT4;
			}
			return (ConstantType) -1;
		}
#if defined(__APPLE__) && (!defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR))
inline void DebugArrays()
{
	GLboolean vertexArrayEnabled;
	GLboolean normalArrayEnabled;
	GLboolean texCoordArrayEnabled;
	GLboolean colorArrayEnabled;
	GLboolean indexArrayEnabled;
	
	GLvoid * vertexArrayPointer;
	GLvoid * normalArrayPointer;
	GLvoid * texCoordArrayPointer;
	GLvoid * colorArrayPointer;
	GLvoid * indexArrayPointer;
	
	glGetPointerv(GL_VERTEX_ARRAY_POINTER, &vertexArrayPointer);
	glGetPointerv(GL_NORMAL_ARRAY_POINTER, &normalArrayPointer);		
	glGetPointerv(GL_TEXTURE_COORD_ARRAY_POINTER, &texCoordArrayPointer);
	glGetPointerv(GL_COLOR_ARRAY_POINTER, &colorArrayPointer);
	glGetPointerv(GL_INDEX_ARRAY_POINTER, &indexArrayPointer);

	glGetBooleanv(GL_VERTEX_ARRAY, &vertexArrayEnabled);
	glGetBooleanv(GL_NORMAL_ARRAY, &normalArrayEnabled);
	glGetBooleanv(GL_TEXTURE_COORD_ARRAY, &texCoordArrayEnabled);
	glGetBooleanv(GL_COLOR_ARRAY, &colorArrayEnabled);
	glGetBooleanv(GL_INDEX_ARRAY, &indexArrayEnabled);
	
	LOG_PRINT("vertexArrayPointer=%x\n", vertexArrayPointer);
	LOG_PRINT("normalArrayPointer=%x\n", normalArrayPointer);		
	LOG_PRINT("texCoordArrayPointer=%x\n", texCoordArrayPointer);
	LOG_PRINT("colorArrayPointer=%x\n", colorArrayPointer);
	LOG_PRINT("indexArrayPointer=%x\n", indexArrayPointer);

	if(vertexArrayEnabled)
		LOG_PRINT("vertexArrayEnabled\n");
	if(normalArrayEnabled)
		LOG_PRINT("normalArrayEnabled\n");
	if(texCoordArrayEnabled)
		LOG_PRINT("texCoordArrayEnabled\n");
	if(colorArrayEnabled)
		LOG_PRINT("colorArrayEnabled\n");
	if(indexArrayEnabled)
		LOG_PRINT("indexArrayEnabled\n");
}

inline bool CheckArrays()
{
	GLboolean vertexArrayEnabled;
	GLboolean normalArrayEnabled;
	GLboolean texCoordArrayEnabled;
	GLboolean colorArrayEnabled;
	GLboolean indexArrayEnabled;
	
	GLvoid * vertexArrayPointer;
	GLvoid * normalArrayPointer;
	GLvoid * texCoordArrayPointer;
	GLvoid * colorArrayPointer;
	GLvoid * indexArrayPointer;
	
	glGetPointerv(GL_VERTEX_ARRAY_POINTER, &vertexArrayPointer);
	glGetPointerv(GL_NORMAL_ARRAY_POINTER, &normalArrayPointer);		
	glGetPointerv(GL_TEXTURE_COORD_ARRAY_POINTER, &texCoordArrayPointer);
	glGetPointerv(GL_COLOR_ARRAY_POINTER, &colorArrayPointer);
	glGetPointerv(GL_INDEX_ARRAY_POINTER, &indexArrayPointer);

	glGetBooleanv(GL_VERTEX_ARRAY, &vertexArrayEnabled);
	glGetBooleanv(GL_NORMAL_ARRAY, &normalArrayEnabled);
	glGetBooleanv(GL_TEXTURE_COORD_ARRAY, &texCoordArrayEnabled);
	glGetBooleanv(GL_COLOR_ARRAY, &colorArrayEnabled);
	glGetBooleanv(GL_INDEX_ARRAY, &indexArrayEnabled);

	if(vertexArrayEnabled)
		if(!vertexArrayPointer)
			return false;
	if(normalArrayEnabled)
		if(!normalArrayPointer)
			return false;
	if(texCoordArrayEnabled)
		if(!texCoordArrayPointer)
			return false;
	if(colorArrayEnabled)
		if(!colorArrayPointer)
			return false;
	if(indexArrayEnabled)
		if(!indexArrayPointer)
			return false;
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
	if(!program)
		return false;

	return true;
}
#else
#define DebugArrays
#define CheckArrays
#endif	
#endif

