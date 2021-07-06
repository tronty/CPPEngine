/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <iostream>
#include <fstream>
using namespace std;
#include <Framework3/IRenderer.h>
#include <hlslcc.h>

#if 0
typedef enum
{
    LANG_DEFAULT,// Depends on the HLSL shader model.
    LANG_ES_100, LANG_ES_FIRST=LANG_ES_100,
    LANG_ES_300,
	LANG_ES_310, LANG_ES_LAST = LANG_ES_310,
    LANG_120, LANG_GL_FIRST = LANG_120,
    LANG_130,
    LANG_140,
    LANG_150,
    LANG_330,
    LANG_,
    LANG_410,
    LANG_420,
    LANG_430,
    LANG_440, LANG_GL_LAST = LANG_440,
	LANG_METAL,
} GLLang;
#endif

int TranslateHLSLFromMem(const char* filename, const char* entry)
{
	int success = 0;
	unsigned int flags = 0;
	GLLang language = LANG_440;
	if((strcmp(entry,"vs_3_0")==0) || (strcmp(entry,"ps_3_0")==0))
		language = LANG_330;
	const GlExtensions *extensions = new GlExtensions();
	GLSLCrossDependencyData* dependencies=new GLSLCrossDependencyData();
	HLSLccSamplerPrecisionInfo samplerPrecisions;
	HLSLccReflection reflectionCallbacks;
	GLSLShader* result = new GLSLShader();
	success = TranslateHLSLFromFile(filename, flags, language, extensions, dependencies, samplerPrecisions, reflectionCallbacks, result);
	LOG_PRINT("result:\n%s\n", result->sourceCode.c_str());
	return success;
}

int ApplicationLogic()
{
	TranslateHLSLFromMem("../fxc/BasicHLSL10.vs.o","vs_4_0");
	TranslateHLSLFromMem("../fxc/BasicHLSL10.ps.o","ps_4_0");
	TranslateHLSLFromMem("../fxc/BasicHLSL11.vs.o","vs_5_0");
	TranslateHLSLFromMem("../fxc/BasicHLSL11.ps.o","ps_5_0");
	TranslateHLSLFromMem("../fxc/BasicHLSL9.vs.o","vs_3_0");
	TranslateHLSLFromMem("../fxc/BasicHLSL9.ps.o","ps_3_0");
	return 0;
}

