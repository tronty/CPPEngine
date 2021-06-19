#if defined(GLSL1_1)
#ifndef __FrameworkGLSLGL_1_1_H__
#define __FrameworkGLSLGL_1_1_H__ 1
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

#include <Framework3/RendererGL_1_1.h>
//typedef void* ShHandle;

struct RendererGLSLGL_1_1 : public RendererGL_1_1
{
	protected:
		bool mOk;
	public:		
		virtual unsigned int GetShaderCount(){return shaders.size();}
		virtual const char* getString(){return "GLSL_1_1";}
		virtual bool IsOK(){return mOk;}
	void applyTextures();
void applyConstants();
			virtual void setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState=-1
		, const int vtstream=-1
		, const VertexBufferID vtbuffer=-1
		);

unsigned int DrawPrimitive(Primitives PrimitiveType,UINT StartVertex,UINT PrimitiveCount, const unsigned int flags);
unsigned int DrawIndexedPrimitive(Primitives PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, const unsigned int flags);
void changeShader(const ShaderID shader);
void changePixelShader(const ShaderID shader);
void changeVertexShader(const ShaderID shader);
void changeGeometryShader(const ShaderID shader);
void changeHullShader(const ShaderID shader);
void changeDomainShader(const ShaderID shader);
void changeComputeShader(const ShaderID shader);
void setShaderConstantRaw(const char *name, const void *data, const int size);

ShaderID addGLSLShader(
const char *vsText, const char *gsText, const char *fsText, const char *csText, const char *hsText, const char *dsText,
const char *vsMain, const char *gsMain, const char *fsMain, const char *csMain, const char *hsMain, const char *dsMain, 
                                            	const unsigned int flags);
virtual ShaderID addHLSLShaderVrtl(
const char *vsText, const char *gsText, const char *fsText, const char *csText, const char *hsText, const char *dsText,
const char *vsMain, const char *gsMain, const char *fsMain, const char *csMain, const char *hsMain, const char *dsMain, 
                                            	const unsigned int flags);
    void changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, const intptr offset);
	void LOGGLTarget(GLenum target);
        RendererGLSLGL_1_1(bool fullScreen=false,
		int argc=0, char *argv[]=0);

	virtual ~RendererGLSLGL_1_1();
	virtual int getSamplerIndex(ShaderID shd, const char* aName);
protected:
	void linkGLSLShader(std::vector<std::string>& sText, ShaderGLSLGL3& shaderGL1_1);
    void reflectGLSLShader(std::vector<std::string>& sText, GLuint aProgram, ShaderGLSLGL3& shaderGL1_1);
	VArray <ShaderGLSLGL3> shaders;
        void *uniformFuncs[CONSTANT_TYPE_COUNT];
};
#endif
#endif

