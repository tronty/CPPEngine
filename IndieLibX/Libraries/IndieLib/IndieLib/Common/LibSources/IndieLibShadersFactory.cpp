/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <string>
#include <Framework3/IRenderer.h>

#ifndef _MSC_VER
struct IndieLibShadersFactory
{
	static std::string GetShader(const char* aShaderName);
	static std::string GetShaderFileName(const char* aShaderName);
};
#endif
std::string IndieLibShadersFactory::GetShaderFileName(const char* aShaderName)
{
	if(stx_strcmp(aShaderName,"texSHD1")==0)
		return std::string("/IndieLib/texSHD1."LSL_EXT);
	else if(stx_strcmp(aShaderName,"texSHD2")==0)
		return std::string("/IndieLib/texSHD2."LSL_EXT);
	else if(stx_strcmp(aShaderName,"texSHD3")==0)
		return std::string("/IndieLib/texSHD3."LSL_EXT);
	return "";
}
std::string IndieLibShadersFactory::GetShader(const char* aShaderName)
{
	std::string contents;

	if(stx_strcmp(aShaderName,"texSHD1")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD1."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD2")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD2."LSL_EXT, contents);
	else if(stx_strcmp(aShaderName,"texSHD3")==0)
		return STX_Service::LoadTxtFile("/IndieLib/texSHD3."LSL_EXT, contents);

	char buf[64];
	stx_snprintf(buf, 64, "[IndieLibShadersFactory::GetShader(%s)]", aShaderName);
	contents=buf;
	return contents;
}

