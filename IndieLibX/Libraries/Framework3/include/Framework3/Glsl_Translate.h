/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __Glsl_Translate__
#define __Glsl_Translate__

#include <string>
#include <vector>
#include <hlsl2glsl.h>
#include <Framework3/Hlsl2Glsl_Translate.h>

enum ttShader
{
	eGLES_Shader_,
	eGLSL_Shader_,
	eMETAL_Shader_,
	eHLSL_Shader_
};

inline void WriteTxtFile(const std::string aFileName, std::string& contents)
{
	//std::string fn=stx_convertpath(aFileName);
    std::ofstream outputStream;
    outputStream.open(aFileName.c_str(), ofstream::binary);
    outputStream << contents;
    outputStream.close();
}
inline std::string GetCompiledShaderText_(ShHandle parser)
{
	std::string txt = Hlsl2Glsl_GetShader (parser);
	
	int count = Hlsl2Glsl_GetUniformCount (parser);
	if (count > 0)
	{
		const ShUniformInfo* uni = Hlsl2Glsl_GetUniformInfo(parser);
		txt += "\n// uniforms:\n";
		for (int i = 0; i < count; ++i)
		{
			LOG_PRINT("\n// %s:%s type %d arrsize %d", uni[i].name, uni[i].semantic?uni[i].semantic:"<none>\n", uni[i].type, uni[i].arraySize);
/*
			txt += buf;

			if (uni[i].registerSpec)
			{
				txt += " register ";
				txt += uni[i].registerSpec;
			}

			txt += "\n";
*/
		}
	}
	
	return txt;
}
#if defined(_MSC_VER)
	#define GLSLANGVALIDATOR "C:/Windows/System32/glslangValidator.exe"
	#define SPIRV_CROSS "C:/Windows/System32/spirv_cross.exe"
#else
	#define GLSLANGVALIDATOR "glslangValidator"
	#define SPIRV_CROSS "spirv_cross"
#endif
inline int Glsl_Translate(const bool bvs, const std::string& glsl_, const std::string& main, ttShader aShader, ShHandle& parser, std::string& out)
{
  std::string version;
	std::string glsl_2;

#ifndef __APPLE__
	{std::regex e("\\btexture2D\\b");
	glsl_2=std::regex_replace (glsl_,e,"texture");}
#else
	glsl_2=replaceAll(glsl_, "texture2D", "texture");
#endif

  std::string glsl=version+glsl_2;

  //STX_Service::
  int i;
  if (system(NULL)) puts ("Ok");
    else exit (EXIT_FAILURE);

  if (bvs)
  {
  WriteTxtFile("tmp.vert", glsl);
  i=system (GLSLANGVALIDATOR" -H -V -e main -o tmp.spv tmp.vert");//-S <stage>
  if(aShader==eGLES_Shader_)
  	i=system (SPIRV_CROSS" --version 310 --es tmp.spv --output tmp.vert.GLES --entry main");
  if(aShader==eGLSL_Shader_)
  	i=system (SPIRV_CROSS" --version 140 tmp.spv --output tmp.vert.GLSL --entry main");
  if(aShader==eMETAL_Shader_)
  	i=system (SPIRV_CROSS" --version 140 --msl tmp.spv --output tmp.vert.METAL --entry main");
  if(aShader==eHLSL_Shader_)
  	i=system (SPIRV_CROSS" --version 140 --hlsl tmp.spv --output tmp.vert.HLSL --entry main");
  } else { 
  WriteTxtFile("tmp.frag", glsl);
  i=system (GLSLANGVALIDATOR" -H -V -e main -o tmp.spv tmp.frag");//-S <stage>
  if(aShader==eGLES_Shader_)
  	i=system (SPIRV_CROSS" --version 310 --es tmp.spv --output tmp.frag.GLES --entry main");
  else if(aShader==eGLSL_Shader_)
  	i=system (SPIRV_CROSS" --version 140 tmp.spv --output tmp.frag.GLSL --entry main");
  else if(aShader==eMETAL_Shader_)
  	i=system (SPIRV_CROSS" --version 140 --msl tmp.spv --output tmp.frag.METAL --entry main");
  else if(aShader==eHLSL_Shader_)
  	i=system (SPIRV_CROSS" --version 140 --hlsl tmp.spv --output tmp.frag.HLSL --entry main");
  }

  return 0;
}

inline int Hlsl_Translate(const bool bvs, const std::string& hlsl, const std::string& main, ttShader aShader, ShHandle& parser, std::string& out)
{
  std::string glsl_;

  if(aShader==eGLES_Shader_)
  {
  	if (bvs)
        	Hlsl2Glsl_Translate_ (true,hlsl,glsl_,main,parser,ETargetGLSL_ES_300);
  	else
        	Hlsl2Glsl_Translate_ (false,hlsl,glsl_,main,parser,ETargetGLSL_ES_300);
#ifndef __APPLE__
	{std::regex e("#version 310\\b");
	glsl_=std::regex_replace (glsl_,e,"#version 310 es");}
#else
	glsl_=replaceAll(glsl_, "#version 310", "#version 310 es");
#endif
  }
  else
  {
  	if (bvs)
        	Hlsl2Glsl_Translate_ (true,hlsl,glsl_,main,parser,ETargetGLSL_140);
  	else
        	Hlsl2Glsl_Translate_ (false,hlsl,glsl_,main,parser,ETargetGLSL_140);
  }

#ifndef __APPLE__
	{std::regex e("\\btexture2D\\b");
	glsl_=std::regex_replace (glsl_,e,"texture");}
#else
	glsl_=replaceAll(glsl_, "texture2D", "texture");
#endif




  Glsl_Translate(bvs, glsl_, main, aShader, parser, out);
  return 0;
}
#endif

