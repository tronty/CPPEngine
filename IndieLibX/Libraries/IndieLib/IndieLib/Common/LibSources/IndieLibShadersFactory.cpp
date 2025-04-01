/*
  Copyright (c) 2017 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <string>
#include <Framework3/IRenderer.h>

#ifndef _MSC_VER
struct IndieLibShadersFactory
{
	static std::string GetShader(const char* aShaderName);
};
#endif
std::string IndieLibShadersFactory::GetShader(const char* aShaderName)
{
	std::string contents;
#if 0//defined(USE_GLES_SHADERS) && (defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR))
	if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD1.gles", contents);
	else if(stx_strcmp(aShaderName,"texSHD2")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD2.gles", contents);
	else if(stx_strcmp(aShaderName,"texSHD3")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD3.gles", contents);
#elif 0//defined(USE_GLES_SHADERS) && defined(_LINUX)
	if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD1.lnxgles", contents);
	else if(stx_strcmp(aShaderName,"texSHD2")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD2.lnxgles", contents);
	else if(stx_strcmp(aShaderName,"texSHD3")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD3.lnxgles", contents);
#elif 1//defined(USE_HLSL_SHADERS)
	if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD1.hlsl", contents);
	else if(stx_strcmp(aShaderName,"texSHD2")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD2.hlsl", contents);
	else if(stx_strcmp(aShaderName,"texSHD3")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD3.hlsl", contents);
#elif 0
	if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD1.glsl", contents);
	else if(stx_strcmp(aShaderName,"texSHD2")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD2.glsl", contents);
	else if(stx_strcmp(aShaderName,"texSHD3")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD3.glsl", contents);
#endif
	char buf[64];
	stx_snprintf(buf, 64, "[IndieLibShadersFactory::GetShader(%s)]", aShaderName);
	contents=buf;
	return contents;
}

