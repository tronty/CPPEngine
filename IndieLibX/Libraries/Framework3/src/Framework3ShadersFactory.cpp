/*
  Copyright (c) 2017 Tuomas Roenty   http://www.tuomasronty.fi/
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
		return STX_Service::LoadTxtFile("/Framework3/texSHD0.GLES.hlsl", contents);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD1.GLES.hlsl", contents);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1.GLES.hlsl", contents);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2.GLES.hlsl", contents);
#elif 0//defined(USE_GLES_SHADERS) && defined(_LINUX)
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD0.GLES.LNX.hlsl", contents);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD1.GLES.LNX.hlsl", contents);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1.GLES.LNX.hlsl", contents);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2.GLES.LNX.hlsl", contents);
#elif 1//defined(USE_HLSL_SHADERS)
	if(stx_strcmp(aShaderName,"texSHD0")==0){STX_PRINT("Shader file:%s\n", "/Framework3/texSHD0.hlsl");
		return STX_Service::LoadTxtFile("/Framework3/texSHD0.hlsl", contents);}
	else if(stx_strcmp(aShaderName,"texSHD1")==0){STX_PRINT("Shader file:%s\n", "/Framework3/texSHD1.hlsl");
		return STX_Service::LoadTxtFile("/Framework3/texSHD1.hlsl", contents);}
	else if(stx_strcmp(aShaderName,"plainSHD1")==0){STX_PRINT("Shader file:%s\n", "/Framework3/plainSHD1.hlsl");
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1.hlsl", contents);}
	else if(stx_strcmp(aShaderName,"plainSHD2")==0){STX_PRINT("Shader file:%s\n", "/Framework3/plainSHD2.hlsl");
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2.hlsl", contents);}
	else if(stx_strcmp(aShaderName,"texSHD0N")==0){STX_PRINT("Shader file:%s\n", "/Framework3/texSHD0N.hlsl");
		return STX_Service::LoadTxtFile("/Framework3/texSHD0N.hlsl", contents);}
	else if(stx_strcmp(aShaderName,"texSHD1N")==0){STX_PRINT("Shader file:%s\n", "/Framework3/texSHD1N.hlsl");
		return STX_Service::LoadTxtFile("/Framework3/texSHD1N.hlsl", contents);}
	else if(stx_strcmp(aShaderName,"texSHD2N")==0){STX_PRINT("Shader file:%s\n", "/Framework3/texSHD2N.hlsl");
		return STX_Service::LoadTxtFile("/Framework3/texSHD2N.hlsl", contents);}
	else if(stx_strcmp(aShaderName,"plainSHD1N")==0){STX_PRINT("Shader file:%s\n", "/Framework3/plainSHD1N.hlsl");
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1N.hlsl", contents);}
	else if(stx_strcmp(aShaderName,"plainSHD2N")==0){STX_PRINT("Shader file:%s\n", "/Framework3/plainSHD2N.hlsl");
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2N.hlsl", contents);}
#elif 0
	if(stx_strcmp(aShaderName,"texSHD0")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD0.GLSL.hlsl", contents);
	else if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/texSHD1.GLSL.hlsl", contents);
	else if(stx_strcmp(aShaderName,"plainSHD1")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD1.GLSL.hlsl", contents);
	else if(stx_strcmp(aShaderName,"plainSHD2")==0)
		return STX_Service::LoadTxtFile("/Framework3/plainSHD2.GLSL.hlsl", contents);
#endif
	char buf[64];
	stx_snprintf(buf, 64, "[Framework3ShaderFactory::GetShader(%s)]", aShaderName);
	contents=buf;
	return contents;
}

