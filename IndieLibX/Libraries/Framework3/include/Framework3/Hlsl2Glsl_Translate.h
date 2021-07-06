/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __Hlsl2Glsl_Translate__
#define __Hlsl2Glsl_Translate__
#if defined(USE_HLSL_SHADERS) 
//&& !defined(ANDROID)
#define USE_HLSL_SHADERS_ANDROID 1
#include <hlsl2glsl.h>

enum TestRun {
	VERTEX,
	FRAGMENT,
	BOTH,
	VERTEX_120,
	FRAGMENT_120,
	VERTEX_FAILURES,
	FRAGMENT_FAILURES,
	NUM_RUN_TYPES
};

const bool kIsVertexShader[NUM_RUN_TYPES] = {
	true,
	false,
	false,
	true,
	false,
	true,
	false,
};

const EShLanguage kTypeLangs[NUM_RUN_TYPES] = {
	EShLangVertex,
	EShLangFragment,
	EShLangCount,
	EShLangVertex,
	EShLangFragment,
	EShLangVertex,
	EShLangFragment,
};
inline std::string GetCompiledShaderText(ShHandle parser)
{
	std::string txt = Hlsl2Glsl_GetShader (parser);
	
	int count = Hlsl2Glsl_GetUniformCount (parser);
	if (count > 0)
	{
		const ShUniformInfo* uni = Hlsl2Glsl_GetUniformInfo(parser);
		txt += "\n// uniforms:\n";
		for (int i = 0; i < count; ++i)
		{
			char buf[1000];
			snprintf(buf,1000,"// %s:%s type %d arrsize %d", uni[i].name, uni[i].semantic?uni[i].semantic:"<none>", uni[i].type, uni[i].arraySize);
			txt += buf;

			if (uni[i].registerSpec)
			{
				txt += " register ";
				txt += uni[i].registerSpec;
			}

			txt += "\n";
		}
	}
	
	return txt;
}
inline void replace_string (std::string& target, const std::string& search, const std::string& replace, size_t startPos)
{
	if (search.empty())
		return;
	
	std::string::size_type p = startPos;
	while ((p = target.find (search, p)) != std::string::npos)
	{
		target.replace (p, search.size (), replace);
		p += replace.size ();
	}
}


static bool CheckGLSL (bool vertex, ETargetVersion version, const std::string& source)
{
	const char* sourcePtr = source.c_str();
	std::string newSrc;

	
#	if 0//def __APPLE__
	// Mac core context does not accept any older shader versions, so need to switch to
	// either legacy context or core one.
	const bool need3 = (version >= ETargetGLSL_ES_300);
	bool s_GL3Active = false;
	if (need3)
	{
		if (!s_GL3Active)
			CGLSetCurrentContext(s_GLContext3);
		s_GL3Active = true;
	}
	else
	{
		if (s_GL3Active)
			CGLSetCurrentContext(s_GLContext);
		s_GL3Active = false;
	}
#	endif
	
	
	if (version == ETargetGLSL_ES_100 || version == ETargetGLSL_ES_300)
	{
		newSrc.reserve(source.size());
		if (version == ETargetGLSL_ES_300)
		{
			newSrc += "#version 150\n";
		}
		newSrc += "#define lowp\n";
		newSrc += "#define mediump\n";
		newSrc += "#define highp\n";
		if (version == ETargetGLSL_ES_300)
		{
			newSrc += "#define gl_Vertex _glesVertex\n";
			newSrc += "#define gl_Normal _glesNormal\n";
			newSrc += "#define gl_Color _glesColor\n";
			newSrc += "#define gl_MultiTexCoord0 _glesMultiTexCoord0\n";
			newSrc += "#define gl_MultiTexCoord1 _glesMultiTexCoord1\n";
			newSrc += "#define gl_MultiTexCoord2 _glesMultiTexCoord2\n";
			newSrc += "#define gl_MultiTexCoord3 _glesMultiTexCoord3\n";
			newSrc += "in highp vec4 _glesVertex;\n";
			newSrc += "in highp vec3 _glesNormal;\n";
			newSrc += "in lowp vec4 _glesColor;\n";
			newSrc += "in highp vec4 _glesMultiTexCoord0;\n";
			newSrc += "in highp vec4 _glesMultiTexCoord1;\n";
			newSrc += "in highp vec4 _glesMultiTexCoord2;\n";
			newSrc += "in highp vec4 _glesMultiTexCoord3;\n";
			newSrc += "#define gl_FragData _glesFragData\n";
			newSrc += "out lowp vec4 _glesFragData[4];\n";
		}
		if (version < ETargetGLSL_ES_300)
		{
			newSrc += "#define texture2DLodEXT texture2DLod\n";
			newSrc += "#define texture2DProjLodEXT texture2DProjLod\n";
			newSrc += "#define texture2DGradEXT texture2DGradARB\n";
			newSrc += "#define textureCubeLodEXT textureCubeLod\n";
			newSrc += "#define textureCubeGradEXT textureCubeGradARB\n";
			newSrc += "#define gl_FragDepthEXT gl_FragDepth\n";
			newSrc += "#define gl_LastFragData _glesLastFragData\n";
			newSrc += "varying lowp vec4 _glesLastFragData[4];\n";
			newSrc += "float shadow2DEXT (sampler2DShadow s, vec3 p) { return shadow2D(s,p).r; }\n";
			newSrc += "float shadow2DProjEXT (sampler2DShadow s, vec4 p) { return shadow2DProj(s,p).r; }\n";
			newSrc += "#define sampler2DArrayNV sampler2DArray\n";
			newSrc += "#define texture2DArrayNV texture2DArray\n";
			newSrc += "#define texture2DArrayLodNV texture2DArrayLod\n";
		}
		newSrc += source;
		replace_string (newSrc, "GL_EXT_shader_texture_lod", "GL_ARB_shader_texture_lod", 0);
		replace_string (newSrc, "#extension GL_OES_standard_derivatives : require", "", 0);
		replace_string (newSrc, "#extension GL_EXT_shadow_samplers : require", "", 0);
		replace_string (newSrc, "#extension GL_EXT_frag_depth : require", "", 0);
		replace_string (newSrc, "#extension GL_EXT_shader_framebuffer_fetch : require", "", 0);
		replace_string (newSrc, "#extension GL_EXT_draw_buffers : require", "", 0);
		replace_string (newSrc, "GL_EXT_draw_instanced", "GL_ARB_draw_instanced", 0);
		replace_string (newSrc, "GL_NV_texture_array", "GL_EXT_texture_array", 0);
		replace_string (newSrc, "gl_InstanceIDEXT", "gl_InstanceIDARB", 0);
		
		sourcePtr = newSrc.c_str();
	}

	bool res = true;

	return res;
}

inline bool Hlsl2Glsl_Translate_ (bool vertex,
	const std::string& asourceStr,
	std::string& text,
	const std::string& main,
	ShHandle& parser,
	const ETargetVersion version=ETargetGLSL_110,
	bool usePrecision=false,
	bool doCheckGLSL=false)
{
	TestRun type=VERTEX;
	if(!vertex) type=FRAGMENT;
	const char* entryPoint=main.c_str();
	unsigned options=0;
	bool res = true;
	
	std::string input = asourceStr;
	parser = Hlsl2Glsl_ConstructCompiler (kTypeLangs[type]);

	const char* sourceStr = asourceStr.c_str();
	//LOG_FNLN;
	//LOG_PRINT("sourceStr=\"%s\"\n", sourceStr);
		
	int parseOk = Hlsl2Glsl_Parse (parser, sourceStr, version, 0, options);
	const char* infoLog = Hlsl2Glsl_GetInfoLog( parser );

	if (parseOk)
	{
		static EAttribSemantic kAttribSemantic[] = {
			EAttrSemTangent,
		};
		static const char* kAttribString[] = {
			"TANGENT",
		};
		Hlsl2Glsl_SetUserAttributeNames (parser, kAttribSemantic, kAttribString, 1);
	/* ???
	ETargetGLSL_ES_100,	 // OpenGL ES 2.0
	ETargetGLSL_110,	 // OpenGL 2.0
	ETargetGLSL_120,
	ETargetGLSL_140,	 // OpenGL 3.1
	ETargetGLSL_ES_300,	 // OpenGL ES 3.0
	// ETargetGLSL_330,

        Version_110, // OpenGL 2.0
        Version_140, // OpenGL 3.1
        Version_150, // OpenGL 3.2
        Version_100_ES, // OpenGL ES 2.0
        Version_300_ES, // OpenGL ES 3.0
*/	
		int translateOk = Hlsl2Glsl_Translate (parser, entryPoint, version, options);
		const char* infoLog = Hlsl2Glsl_GetInfoLog( parser );
		if (translateOk)
		{
			text = GetCompiledShaderText(parser);
			//LOG_FNLN;
			//LOG_PRINT ("%s\n", text.c_str());
			
			for (size_t i = 0, n = text.size(); i != n; ++i)
			{
			   char c = text[i];
			   
			   if (!isascii(c))
			   {
				   LOG_PRINT ("  contains non-ascii '%c' (0x%02X)\n", c, c);
				   res = false;
			   }
			}

			std::string output;

			if (doCheckGLSL && !CheckGLSL (kIsVertexShader[type], version, text))
			{
				res = false;
			}
		}
		else
		{
			LOG_PRINT ("  translate error: %s\n", infoLog);
			res = false;
		}
	}
	else
	{
		LOG_PRINT ("  parse error: %s\n", infoLog);
		res = false;
	}

	Hlsl2Glsl_DestructCompiler (parser);

	return res;

}
inline bool Hlsl2Glsl_Translate_eitoimi (bool vertex,
	const std::string& inputPath,
	std::string& text,
	const std::string& main,
	ShHandle& parser,
	const ETargetVersion version=ETargetGLSL_110,
	bool usePrecision=false,
	bool doCheckGLSL=false)
{
	//text.append("#version 120\n");

	//std::string input=inputPath;

	parser = Hlsl2Glsl_ConstructCompiler (vertex ? EShLangVertex : EShLangFragment);


	const char* sourceStr = inputPath.c_str();


	bool res = true;


	int options = 0;


	if (1)//kDumpShaderAST)
		options |= ETranslateOpIntermediate;


	//if (0)//usePrecision)
	//	options |= ETranslateOpUsePrecision;


/* ???
	ETargetGLSL_ES_100,	 // OpenGL ES 2.0
	ETargetGLSL_110,	 // OpenGL 2.0
	ETargetGLSL_120,
	ETargetGLSL_140,	 // OpenGL 3.1
	ETargetGLSL_ES_300,	 // OpenGL ES 3.0
	// ETargetGLSL_330,

        Version_110, // OpenGL 2.0
        Version_140, // OpenGL 3.1
        Version_150, // OpenGL 3.2
        Version_100_ES, // OpenGL ES 2.0
        Version_300_ES, // OpenGL ES 3.0
*/
	int parseOk = Hlsl2Glsl_Parse (parser, sourceStr, version, NULL, options);

	const char* infoLog = Hlsl2Glsl_GetInfoLog( parser );


	/*if (1)//kDumpShaderAST)
	{
		// write output
		outputPath=infoLog;

	}*/


	if (parseOk)
	{


		static EAttribSemantic kAttribSemantic[] = {
			EAttrSemTangent,
		};


		static const char* kAttribString[] = {
			"TANGENT",
		};


		Hlsl2Glsl_SetUserAttributeNames (parser, kAttribSemantic, kAttribString, 1);


		//Hlsl2Glsl_UseUserVaryings (parser, true);


		int translateOk = Hlsl2Glsl_Translate (parser, main.c_str(), version, options);


		const char* infoLog = Hlsl2Glsl_GetInfoLog( parser );


		if (translateOk)
		{


			text.append(Hlsl2Glsl_GetShader (parser));
			//LOG_FNLN;
			LOG_PRINT ("%s\n", text.c_str());

		}
		else
		{
			LOG_PRINT ("  (XXX) translate error: %s\n", infoLog);

			////LOG_PRINT("\n<1:>\n%s\n%s\n</1:>\n", main.c_str(), sourceStr);

			STX_STACK_TRACE;
			exit(0);//???
			res = false;
		}
	}
	else
	{
		LOG_PRINT ("  (XXX) parse error: %s\n", infoLog);

			////LOG_PRINT("\n<2:>\n%s\n%s\n</2:>\n", main.c_str(), sourceStr);

		//STX_STACK_TRACE;
		exit(0);//???
		res = false;
	}


	Hlsl2Glsl_DestructCompiler (parser);


	return res;
}
inline bool TranslateVertexShader(const std::string& hlsl, const std::string& main, std::string& glsl)
{
	//LOG_PRINT("\nHLSL:\n%s\n",hlsl.c_str());
#if 0
	TDebugOptions debugOptions//=EDebugOpNone;
					=EDebugOpIntermediate;

    // Initialize the HLSL to GLSL translator
    Hlsl2Glsl_Initialize();

    // Construct a vertex shader parser for the translator to use
    ShHandle parserv = Hlsl2Glsl_ConstructParser(EShLangVertex, 0);

    // Construct a Translator to use for final code translation
    ShHandle translator = Hlsl2Glsl_ConstructTranslator(debugOptions);

#if 1
    // Parse the shader  print out the info log on failure
    const char *shaderStrings[1] = { hlsl.c_str() };
    if ( !Hlsl2Glsl_Parse(parserv, shaderStrings, 1, debugOptions) )
    {
       LOG_PRINT( Hlsl2Glsl_GetInfoLog(parserv));
       //DBG_HALT;
    }

    // Now translate the parsed shader
    ShHandle parsers[1] = { parserv };
    if (! Hlsl2Glsl_Translate(translator, parsers, 1, main.c_str(), 0) )
    {
       LOG_PRINT( Hlsl2Glsl_GetInfoLog(translator));
       //DBG_HALT;
    }

    // Finally, get the translated shader source (GLSL)
    const char* text = Hlsl2Glsl_GetShader( translator,EShLangVertex );
#endif
	ShHandle parser = Hlsl2Glsl_ConstructCompiler (1 ? EShLangVertex : EShLangFragment);

	glsl="";
	glsl.append(text);
	LOG_PRINT("\nGLSL:\n%s\n",glsl.c_str());
#if 1
	// ...shutdown
    Hlsl2Glsl_Destruct( parserv );
    Hlsl2Glsl_Destruct( translator );
    Hlsl2Glsl_Finalize( );
#endif
	#endif
	return true;
}
inline bool TranslateFragmentShader(const std::string& hlsl, const std::string& main, std::string& glsl)
{
	//LOG_PRINT("\nHLSL:\n%s\n",hlsl.c_str());
#if 0
	TDebugOptions debugOptions//=EDebugOpNone;
					=EDebugOpIntermediate;

    // Initialize the HLSL to GLSL translator
    //Hlsl2Glsl_Initialize();

    // Construct a vertex shader parser for the translator to use
    //ShHandle parserf = Hlsl2Glsl_ConstructParser(EShLangFragment, 0);

    // Construct a Translator to use for final code translation
    //ShHandle translator = Hlsl2Glsl_ConstructTranslator(debugOptions);

#if 1
    // Parse the shader  print out the info log on failure
    const char *shaderStrings[1] = { hlsl.c_str() };
    if ( !Hlsl2Glsl_Parse(parserf, shaderStrings, 1, debugOptions) )
    {
       LOG_PRINT( Hlsl2Glsl_GetInfoLog(parserf));
       //DBG_HALT;
    }

    // Now translate the parsed shader
    ShHandle parsers[1] = { parserf };
    if (! Hlsl2Glsl_Translate(translator, parsers, 1, 0, main.c_str()) )
    {
       LOG_PRINT( Hlsl2Glsl_GetInfoLog(translator));
       //DBG_HALT;
    }

    // Finally, get the translated shader source (GLSL)
    const char* text = Hlsl2Glsl_GetShader( translator,EShLangFragment );
#endif
	glsl="";

	ShHandle parser = Hlsl2Glsl_ConstructCompiler (0 ? EShLangVertex : EShLangFragment);

	glsl.append(text);
	LOG_PRINT("\nGLSL:\n%s\n",glsl.c_str());
#if 1
    // ...shutdown
    Hlsl2Glsl_Destruct( parserf );
    Hlsl2Glsl_Destruct( translator );
    Hlsl2Glsl_Finalize( );
#endif
	#endif
	return true;
}
#endif
#endif

