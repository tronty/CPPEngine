/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <stdio.h>
#include <stdlib.h>
#include <STX/STX.h>
#include <STX/STXCPP.h>
#include <Framework3/IRenderer.h>

#ifndef __GS__
#define EXT "hlsl"
#else
#define EXT "glsl"
#endif

std::string Framework3ShaderFactory::GetShader(const char* aShaderName)
{
	std::string contents;
#if 0//defined(USE_GLES_SHADERS) && (defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR))
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD0.GLES."EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD1.GLES."EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1.GLES."EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2.GLES."EXT, contents);
#elif 0//defined(USE_GLES_SHADERS) && defined(_LINUX)
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD0.GLES.LNX."EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD1.GLES.LNX."EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1.GLES.LNX."EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2.GLES.LNX."EXT, contents);
#elif 1//defined(USE_HLSL_SHADERS)
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD0."EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD1."EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1."EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2."EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD0N")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD0N."EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD1N")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD1N."EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD2N")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD2N."EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD1N")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1N."EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD2N")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2N."EXT, contents);
#elif 0
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD0.GLSL."EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD1.GLSL."EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1.GLSL."EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2.GLSL."EXT, contents);
#endif
	char buf[64];
	stx_snprintf(buf, 64, "[Framework3ShaderFactory::GetShader(%s)]", aShaderName);
	contents=buf;
	return contents;
}
std::string Framework3ShaderFactory::GetShaderFileName(const char* aShaderName)
{
#if 0//defined(USE_GLES_SHADERS) && (defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR))
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return std::string("/Framework3/texSHD0.GLES.glsl");
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return std::string("/Framework3/texSHD1.GLES.glsl");
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return std::string("/Framework3/plainSHD1.GLES.glsl");
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return std::string("/Framework3/plainSHD2.GLES.glsl");
#elif 0//defined(USE_GLES_SHADERS) && defined(_LINUX)
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return std::string("/Framework3/texSHD0.GLES.LNX.glsl");
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return std::string("/Framework3/texSHD1.GLES.LNX.glsl");
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return std::string("/Framework3/plainSHD1.GLES.LNX.glsl");
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return std::string("/Framework3/plainSHD2.GLES.LNX.glsl");
#elif 1//defined(USE_HLSL_SHADERS)
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return std::string("/Framework3/texSHD0.glsl");
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return std::string("/Framework3/texSHD1.glsl");
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return std::string("/Framework3/plainSHD1.glsl");
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return std::string("/Framework3/plainSHD2.glsl");
	else if(stx_strcmp(aShaderName,"texSHD0N")==0)
		return std::string("/Framework3/texSHD0N.glsl");
	else if(stx_strcmp(aShaderName,"texSHD1N")==0)
		return std::string("/Framework3/texSHD1N.glsl");
	else if(stx_strcmp(aShaderName,"texSHD2N")==0)
		return std::string("/Framework3/texSHD2N.glsl");
	else if(stx_strcmp(aShaderName,"plainSHD1N")==0)
		return std::string("/Framework3/plainSHD1N.glsl");
	else if(stx_strcmp(aShaderName,"plainSHD2N")==0)
		return std::string("/Framework3/plainSHD2N.glsl");
#elif 0
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return std::string("/Framework3/texSHD0.GLSL.glsl");
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return std::string("/Framework3/texSHD1.GLSL.glsl");
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return std::string("/Framework3/plainSHD1.GLSL.glsl");
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return std::string("/Framework3/plainSHD2.GLSL.glsl");
#endif
}

