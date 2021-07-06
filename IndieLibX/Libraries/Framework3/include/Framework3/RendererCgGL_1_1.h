#if defined(GLCG1_1)
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#ifndef __FrameworkCgGL_1_1_H__
#define __FrameworkCgGL_1_1_H__ 1

#include <Framework3/RendererGL_1_1.h>
#if 0
//namespace CG
//{
//	enum RENDERERAPI CGprofile;
typedef struct RENDERERAPI _CGcontext *CGcontext;
typedef struct RENDERERAPI _CGprogram *CGprogram;
typedef struct RENDERERAPI _CGparameter *CGparameter;
typedef struct RENDERERAPI _CGobj *CGobj;
typedef struct RENDERERAPI _CGbuffer *CGbuffer;
typedef struct RENDERERAPI _CGeffect *CGeffect;
typedef struct RENDERERAPI _CGtechnique *CGtechnique;
typedef struct RENDERERAPI _CGpass *CGpass;
typedef struct RENDERERAPI _CGstate *CGstate;
typedef struct RENDERERAPI _CGstateassignment *CGstateassignment;
typedef struct RENDERERAPI _CGannotation *CGannotation;
typedef void *CGhandle;

typedef int CGbool;
#define CG_FALSE ((CGbool)0)
#define CG_TRUE  ((CGbool)1)

//	enum RENDERERAPI CGprofile;
typedef struct RENDERERAPI _CGcontext *CGcontext;
typedef struct RENDERERAPI _CGprogram *CGprogram;
typedef struct RENDERERAPI _CGparameter *CGparameter;
typedef struct RENDERERAPI _CGobj *CGobj;
typedef struct RENDERERAPI _CGbuffer *CGbuffer;
typedef struct RENDERERAPI _CGeffect *CGeffect;
typedef struct RENDERERAPI _CGtechnique *CGtechnique;
typedef struct RENDERERAPI _CGpass *CGpass;
typedef struct RENDERERAPI _CGstate *CGstate;
typedef struct RENDERERAPI _CGstateassignment *CGstateassignment;
typedef struct RENDERERAPI _CGannotation *CGannotation;
typedef void *CGhandle;
//};
#endif
#include <Cg/cg.h>
#include <Cg/cgGL.h>

struct AttribShaderCgGL3 : public Constant {
	AttribShaderCgGL3() : Constant()
    {
        name="";
        data.resize(0);
        index=0;
        loc=0;
        size=0;
	    offset=0;
        type=CONSTANT_NONE;
		nElements=0;
        dirty=false;
    }
	AttribShaderCgGL3(const AttribShaderCgGL3& rhs)
    {
        param=rhs.param;
        name=rhs.name;
        data=rhs.data;
        index=rhs.index;
        loc=rhs.loc;
        size=rhs.size;
	    offset=rhs.offset;
        type=rhs.type;
		nElements=rhs.nElements;
        dirty=rhs.dirty;
    }
	virtual ~AttribShaderCgGL3(){}
		CGparameter param;
        std::string name;
        std::vector<ubyte> data;
        unsigned int index;
        unsigned int loc;
        unsigned int size;
	unsigned int offset;
        ConstantType type;
		//CGparameterclass type;
        int nElements;
        bool dirty;
};
inline int attribCompShaderCgGL3(const void *s0, const void *s1){
    return stx_strncmp(((AttribShaderCgGL3 *) s0)->name.c_str(), ((AttribShaderCgGL3 *) s1)->name.c_str(),stx_strlen(((AttribShaderCgGL3 *) s1)->name.c_str())+1);
}

struct ConstantShaderCgGL3 : public Constant {
	ConstantShaderCgGL3() : Constant()
    {
        name="";
        data.resize(0);
        index=0;
        type=CONSTANT_NONE;
		nElements=0;
        dirty=false;
    }
	ConstantShaderCgGL3(const ConstantShaderCgGL3& rhs)
    {
        param=rhs.param;
        name=rhs.name;
        data=rhs.data;
        index=rhs.index;
        type=rhs.type;
		nElements=rhs.nElements;
        dirty=rhs.dirty;
    }
	virtual ~ConstantShaderCgGL3(){}
		CGparameter param;
        std::string name;
        std::vector<ubyte> data;
        unsigned int index;
        ConstantType type;
		//CGparameterclass type;
        int nElements;
        bool dirty;
};

inline int constantCompShaderCgGL3(const void *s0, const void *s1){
    return stx_strncmp(((ConstantShaderCgGL3 *) s0)->name.c_str(), ((ConstantShaderCgGL3 *) s1)->name.c_str(),stx_strlen(((ConstantShaderCgGL3 *) s1)->name.c_str())+1);
}

struct SamplerShaderCgGL3 : public Sampler {
	SamplerShaderCgGL3() : Sampler()
    {
        name="";
        index=0;
        texID=-1;
        m_bEnabled=false;
    }
	SamplerShaderCgGL3(const SamplerShaderCgGL3& rhs)
    {
        name=rhs.name;
        index=rhs.index;
        param=rhs.param;
        texID=rhs.texID;
        m_bEnabled=rhs.m_bEnabled;
    }
	virtual ~SamplerShaderCgGL3(){}
        std::string name;
        unsigned int index;
	CGparameter param;
	bool m_bEnabled;
	//GLuint glTexID;
	TextureID texID;
};

inline int samplerCompShaderCgGL3(const void *s0, const void *s1){
        return stx_strncmp(((SamplerShaderCgGL3 *) s0)->name.c_str(),
			((SamplerShaderCgGL3 *) s1)->name.c_str()
            ,stx_strlen(((SamplerShaderCgGL3 *) s1)->name.c_str())+1);
}

struct ShaderCgGL3;
struct RendererCgGL_1_1 : public RendererGL_1_1
{
	protected:
		bool mOk;
	public:
		virtual unsigned int GetShaderCount(){return shaders.size();}
		virtual const char* getString(){return "GLCg_1_1";}
		virtual bool IsOK(){return mOk;}
	void disApplyProfiles();
	void disApplyTextures();
			virtual void setTexture(const char *textureName, const TextureID texture, const SamplerStateID samplerState=-1
		, const int vtstream=-1
		, const VertexBufferID vtbuffer=-1
		);
	void applyTextures();
	virtual void resetVrtl(const unsigned int flags);
void applyConstants();
unsigned int DrawPrimitive(Primitives PrimitiveType,UINT StartVertex,UINT PrimitiveCount, const unsigned int flags);
unsigned int DrawIndexedPrimitive(Primitives PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount, const unsigned int flags);
void changeShader(const ShaderID shader);
void changePixelShader(const ShaderID shader);
void changeVertexShader(const ShaderID shader);
void changeGeometryShader(const ShaderID shader);
void changeHullShader(const ShaderID shader);
void changeDomainShader(const ShaderID shader);
void changeComputeShader(const ShaderID shader);
virtual stx_Effect* addEffectFromFile(const std::string& aFileName, const char* aDefines=0);
void setShaderConstantRaw(const char *name, const void *data, const int size);

virtual ShaderID addHLSLShaderVrtl(
const char *vsText, const char *gsText, const char *fsText, const char *csText, const char *hsText, const char *dsText,
const char *vsMain, const char *gsMain, const char *fsMain, const char *csMain, const char *hsMain, const char *dsMain, 
                                            	const unsigned int flags);
	VArray <ShaderCgGL3*> shaders;
	void changeVertexBufferVrtl(const int stream, const VertexBufferID vertexBuffer, const intptr offset);
        RendererCgGL_1_1(bool fullScreen=false,
		int argc=0, char *argv[]=0);
	virtual ~RendererCgGL_1_1();
	virtual int getSamplerIndex(ShaderID shd, const char* aName);
    static CGcontext GetCgContext(){return m_CgContext;}
protected:
    static CGcontext m_CgContext;
};
#endif
#if 1
#if 0
#define ParameterCg CGparameter
#else
#if 0
struct Shader;
struct ShaderParameter;
#else

#endif
struct ParameterCg : public ShaderParameter
{
	CGparameter param;
//	ParameterCg();

		ParameterCg():ShaderParameter(){}


	ParameterCg(CGparameter& aparam){param=aparam;}
	virtual ~ParameterCg(){}
	//virtual bool cgIsParameterReferenced(CGparameter& parameter);
};

#endif
struct SShader : public Shader
{
	std::vector<ShaderParameter*> VProgramParameters;
	std::vector<ShaderParameter*> FProgramParameters;
	std::vector<std::string> VProgramParameterNames;
	std::vector<std::string> FProgramParameterNames;
	std::vector<int> VProgramParameterTypes;
	std::vector<int> FProgramParameterTypes;
	std::vector<int> VProgramParameterTU;
	std::vector<int> FProgramParameterTU;
//	SShader();
SShader():Shader(){}
	virtual ~SShader();
	/*{
		for(int i=0;i<VProgramParameters.size();i++)
			delete VProgramParameters[i];
		for(int i=0;i<FProgramParameters.size();i++)
			delete FProgramParameters[i];
	}*/
	virtual int VProgramParameterNamesSize();
	virtual int FProgramParameterNamesSize();

	virtual void GetParams(const char* name,CGparameter& paramV,CGparameter& paramF,bool& V, bool& F);
	virtual void GetParamsI(const int idx,CGparameter& paramV,CGparameter& paramF,bool& V, bool& F);

};
#endif
#endif

