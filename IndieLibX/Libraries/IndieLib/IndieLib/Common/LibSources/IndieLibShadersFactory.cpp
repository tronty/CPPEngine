/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <string>
#include <Framework3/IRenderer.h>

struct IndieLibShadersFactory
{
	static std::string GetShader(const char* aShaderName);
};

std::string IndieLibShadersFactory::GetShader(const char* aShaderName)
{
	std::string contents;
#if 0//defined(USE_GLES_SHADERS) && (defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR))
	if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD1.GLES.shd", contents);
	else if(stx_strcmp(aShaderName,"texSHD2")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD2.GLES.shd", contents);
	else if(stx_strcmp(aShaderName,"texSHD3")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD3.GLES.shd", contents);
#elif 0//defined(USE_GLES_SHADERS) && defined(_LINUX)
	if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD1.GLES.LNX.shd", contents);
	else if(stx_strcmp(aShaderName,"texSHD2")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD2.GLES.LNX.shd", contents);
	else if(stx_strcmp(aShaderName,"texSHD3")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD3.GLES.LNX.shd", contents);
#elif 1//defined(USE_HLSL_SHADERS)
	if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD1.HLSL.shd", contents);
	else if(stx_strcmp(aShaderName,"texSHD2")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD2.HLSL.shd", contents);
	else if(stx_strcmp(aShaderName,"texSHD3")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD3.HLSL.shd", contents);
#elif 0
	if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD1.GLSL.shd", contents);
	else if(stx_strcmp(aShaderName,"texSHD2")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD2.GLSL.shd", contents);
	else if(stx_strcmp(aShaderName,"texSHD3")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD3.GLSL.shd", contents);
#endif
	char buf[64];
	stx_snprintf(buf, 64, "[IndieLibShadersFactory::GetShader(%s)]", aShaderName);
	contents=buf;
	return contents;
}

