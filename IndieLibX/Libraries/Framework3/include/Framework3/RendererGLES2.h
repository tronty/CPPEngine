#if defined(GLES2)
#ifndef __FrameworkGLES2_H__
#define __FrameworkGLES2_H__ 1
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

#include <Framework3/RendererGLES.h>
//typedef void* ShHandle;
inline int constantCompShaderGLES2(const void *s0, const void *s1){
        return strcmp(((ConstantShaderGLSLGL3 *) s0)->name.c_str(), ((ConstantShaderGLSLGL3 *) s1)->name.c_str());
}

inline int samplerCompShaderGLES2(const void *s0, const void *s1){
        return strcmp(((XSampler *) s0)->name.c_str(), ((XSampler *) s1)->name.c_str());
}

struct RendererGLES2 : public RendererGLES
{
	protected:
		bool mOk;
	public:		
		virtual unsigned int GetShaderCount(){return shaders.size();}
		virtual const char* getString(){return "GLES2";}
		virtual bool IsOK(){return mOk;}
	virtual float GetMaxPointSpriteSize();
	virtual void BeginPointSpriteRendering();
	virtual void EndPointSpriteRendering();
			virtual void setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState=-1
		, const int vtstream=-1
		, const VertexBufferID vtbuffer=-1
		);

	void applyTextures();
void applyConstants();
unsigned int DrawPrimitive(Primitives PrimitiveType,UINT StartVertex,UINT PrimitiveCount, const unsigned int flags);
unsigned int DrawIndexedPrimitive(Primitives PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, const unsigned int flags);
void changeShader(const ShaderID shader);
void setShaderConstantRaw(const char *name, const void *data, const int size);

ShaderID addGLSLShader(
const char *vsText, const char *gsText, const char *fsText, const char *csText, const char *hsText, const char *dsText,
const char *vsMain, const char *gsMain, const char *fsMain, const char *csMain, const char *hsMain, const char *dsMain, 
                                            	const unsigned int flags);
virtual ShaderID addHLSLShaderVrtl(
const char *vsText, const char *gsText, const char *fsText, const char *csText, const char *hsText, const char *dsText,
const char *vsMain, const char *gsMain, const char *fsMain, const char *csMain, const char *hsMain, const char *dsMain, 
                                            	const unsigned int flags);
VArray <ShaderGLSLGL3> shaders;

	void changeIndexBuffer(const IndexBufferID indexBuffer);
	void changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, const intptr offset);
        RendererGLES2(bool fullScreen=false,
		int argc=0, char *argv[]=0);

	virtual ~RendererGLES2();
	virtual int getSamplerIndex(ShaderID shd, const char* aName);
        void *uniformFuncs[CONSTANT_TYPE_COUNT];
};
#endif
#endif

