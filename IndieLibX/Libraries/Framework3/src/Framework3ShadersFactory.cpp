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

std::string Framework3ShaderFactory::GetShader(const char* aShaderName)
{
	std::string contents;
#if 0//defined(USE_GLES_SHADERS) && (defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR))
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD0.GLES."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD1.GLES."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1.GLES."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2.GLES."LSL_EXT, contents);
#elif 0//defined(USE_GLES_SHADERS) && defined(_LINUX)
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD0.GLES.LNX."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD1.GLES.LNX."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1.GLES.LNX."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2.GLES.LNX."LSL_EXT, contents);
#elif 1//defined(USE_HLSL_SHADERS)
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD0."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD1."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD0N")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD0N."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD1N")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD1N."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD2N")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD2N."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD1N")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1N."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD2N")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2N."LSL_EXT, contents);
#elif 0
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD0.GLSL."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD1.GLSL."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1.GLSL."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2.GLSL."LSL_EXT, contents);
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
		return std::string("/Framework3/texSHD0.GLES."LSL_EXT);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return std::string("/Framework3/texSHD1.GLES."LSL_EXT);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return std::string("/Framework3/plainSHD1.GLES."LSL_EXT);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return std::string("/Framework3/plainSHD2.GLES."LSL_EXT);
#elif 0//defined(USE_GLES_SHADERS) && defined(_LINUX)
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return std::string("/Framework3/texSHD0.GLES.LNX."LSL_EXT);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return std::string("/Framework3/texSHD1.GLES.LNX."LSL_EXT);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return std::string("/Framework3/plainSHD1.GLES.LNX."LSL_EXT);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return std::string("/Framework3/plainSHD2.GLES.LNX."LSL_EXT);
#elif 1//defined(USE_HLSL_SHADERS)
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return std::string("/Framework3/texSHD0."LSL_EXT);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return std::string("/Framework3/texSHD1."LSL_EXT);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return std::string("/Framework3/plainSHD1."LSL_EXT);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return std::string("/Framework3/plainSHD2."LSL_EXT);
	else if(stx_strcmp(aShaderName,"texSHD0N")==0)
		return std::string("/Framework3/texSHD0N."LSL_EXT);
	else if(stx_strcmp(aShaderName,"texSHD1N")==0)
		return std::string("/Framework3/texSHD1N."LSL_EXT);
	else if(stx_strcmp(aShaderName,"texSHD2N")==0)
		return std::string("/Framework3/texSHD2N."LSL_EXT);
	else if(stx_strcmp(aShaderName,"plainSHD1N")==0)
		return std::string("/Framework3/plainSHD1N."LSL_EXT);
	else if(stx_strcmp(aShaderName,"plainSHD2N")==0)
		return std::string("/Framework3/plainSHD2N."LSL_EXT);
#elif 0
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return std::string("/Framework3/texSHD0.GLSL."LSL_EXT);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return std::string("/Framework3/texSHD1.GLSL."LSL_EXT);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return std::string("/Framework3/plainSHD1.GLSL."LSL_EXT);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return std::string("/Framework3/plainSHD2.GLSL."LSL_EXT);
#endif
}

