/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <iostream>
#include <string>
#include <regex>
#include <iterator>
#include <STX/STX.h>
#include <STX/STXCPP.h>
#include <Framework3/Hlsl2Glsl_Translate.h>

// uniform float4 position : POSITION;
/*
Varying Shader Inputs and Semantics

Varying input parameters (of a top-level shader function) must be marked either with a semantic or uniform keyword indicating the value is constant for the execution of the shader. If a top-level shader input is not marked with a semantic or uniform keyword, then the shader will fail to compile.
*/

struct _shaderinput
{
	bool uniform;
	string type;
	string name;
	string semantic;
	int i;
	_shaderinput()
	{
		uniform=false;
		type="";
		name="";
		semantic="";
		i=-1;
	}
	_shaderinput(const _shaderinput& rhs)
	{
		uniform=rhs.uniform;
		type=rhs.type;
		name=rhs.name;
		semantic=rhs.semantic;
		i=rhs.i;
	}
	virtual ~_shaderinput(){}
};

struct _struct
{
	string name;
	vector<_shaderinput> shaderinputs;
	_struct()
	{
		name="";
	}
	_struct(const _struct& rhs)
	{
		name=rhs.name;
		shaderinputs=rhs.shaderinputs;
	}
	virtual ~_struct(){}
};
vector<_struct> structs;
inline void split(const string &s, char delim, vector<string> &elems) {
	
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

std::vector<std::string> split(std::string const &input) { 
    std::istringstream buffer(input);
    std::vector<std::string> ret((std::istream_iterator<std::string>(buffer)), 
                                 std::istream_iterator<std::string>());
    return ret;
}

bool _is(const char* x)
{
	return  (((x) >= 'a') && ((x) <= 'z')) || 
		(((x) >= 'A') && ((x) <= 'Z')) || 
                (((x) >= '0') && ((x) <= '9')) || 
                ((x) == '{') || 
                ((x) == '[') || 
                ((x) == ']') || 
                ((x) == '}') || 
                ((x) == '(') || 
                ((x) == ')') || 
                ((x) == '_') || 
                ((x) == ':') || 
                ((x) == ';') || 
                ((x) == '\t') || 
                ((x) == '\n');
}

std::string trim(std::string a)
{
	char const *str=a.c_str();
  // Trim leading non-letters
  while(!_is(*str)) str++;

  // Trim trailing non-letters
  char const *end = str + strlen(str) - 1;
  while(end > str && !_is(*end)) end--;

  return std::string(str, end+1);
}

void replace(std::string& str)
{
#if 0
	{//float2 texCoord: TEXCOORD0;
	//std::regex e1("(float[\\d]+\\s+\\w+)\\s*\\:\\s*POSITION([\\d]*)");
	std::regex e1("(float[\\d]+\\s+\\w+)\\s*\\:\\s*TEXCOORD([\\d]*)");
	std::string str2="";
	str2=std::regex_replace (str2,e1,"varying $1$2");

	std::regex e2("\\[Vertex shader\\]");
	std::regex e3("\\[Fragment shader\\]");
	str=std::regex_replace (str,e2,"[Vertex shader]\\n"+str2);
	str=std::regex_replace (str,e3,"[Fragment shader]\\n"+str2);}
#endif
       {std::regex e("\\:\\s*POSITION[\\d]*\\b");
	str=std::regex_replace (str,e,"");
	}{std::regex e("\\:\\s*TEXCOORD[\\d]*");
	str=std::regex_replace (str,e,"");
	}{std::regex e("\\:\\s*COLOR[\\d]*\\b");
	str=std::regex_replace (str,e,"");
	}{std::regex e("\\bmul\\((.*?),(.*?)\\)");
	str=std::regex_replace (str,e,"$1*$2");
	}{std::regex e("\\bfloat2\\b");
	str=std::regex_replace (str,e,"vec2");
	}{std::regex e("\\bfloat3\\b");
	str=std::regex_replace (str,e,"vec3");
	}{std::regex e("\\bfloat4\\b");
	str=std::regex_replace (str,e,"vec4");
	}{std::regex e("\\bfloat4x4\\b");
	str=std::regex_replace (str,e,"mat4");
	}{std::regex e("\\bsampler\\b");
	str=std::regex_replace (str,e,"sampler");
	}{std::regex e("\\btex\\b");
	str=std::regex_replace (str,e,"texture");
	}{std::regex e("\\bsampler1D\\b");
	str=std::regex_replace (str,e,"sampler1D");
	}{std::regex e("\\btex1D\\b");
	str=std::regex_replace (str,e,"texture1D");
	}{std::regex e("\\bsampler2D\\b");
	str=std::regex_replace (str,e,"sampler2D");
	}{std::regex e("\\btex2D\\b");
	str=std::regex_replace (str,e,"texture2D");
	}{std::regex e("\\bsampler3D\\b");
	str=std::regex_replace (str,e,"sampler3D");
	}{std::regex e("\\btex3D\\b");
	str=std::regex_replace (str,e,"texture3D");
	}
}

int addHLSLShader(const char *shaderText0, const char *vsName, const char *psName, 
			std::string& vsglsl1, std::string& fsglsl1)
{
	char *shaderText=new char[stx_strlen(shaderText0)+1];
	stx_strlcpy(shaderText,shaderText0, stx_strlen(shaderText0)+1);

	int res = -1;

		char *def = strstr(shaderText, "[Header]");
		char *vs = strstr(shaderText, "[Vertex shader]");
		char *fs = strstr(shaderText, "[Fragment shader]");

		char *header = 0;
		if(def)
			header = def+8;
		else
			header = (shaderText[0] != '[')? shaderText : 0;

		int vsLine = 0;
		if (vs != 0){
			*vs = '\0';
			vs += 15;
			while (*vs == '\r' || *vs == '\n') vs++;

			char *str = shaderText;
			while (str < vs){
                if (*str == '\n') vsLine++;
				str++;
			}
		}

		int fsLine = 0;
		if (fs != 0){
			*fs = '\0';
			fs += 17;
			while (*fs == '\r' || *fs == '\n') fs++;

			char *str = shaderText;
			while (str < fs){
                if (*str == '\n') fsLine++;
				str++;
			}
		}
	#if 0
		LOG_PRINT("\nheader:\n%s\n", header);
		LOG_PRINT("\nvs:\n%s\n", vs);
		LOG_PRINT("\nfs:\n%s\n", fs);
	#endif
	
	// res = addHLSLShader(vs, 0, fs, vsName, psName, vsLine, 0, fsLine, header, extra, shaderText, attributeNames, nAttributes, flags);

	ShHandle parser;
	vsglsl1=header;
	fsglsl1=header;
	vsglsl1.append(vs);
	fsglsl1.append(fs);
	std::string vsglsl;
	std::string fsglsl;

	bool ret=Hlsl2Glsl_Translate_ (true,vsglsl1,vsglsl,vsName,parser,ETargetGLSL_110);

	#if 0
	LOG_PRINT("\nvs.HLSL:\n\"%s\"\n",vs);
	LOG_PRINT("\nvs.GLSL:\n\"%s\"\n",vsglsl.c_str());
	#endif

	ret=Hlsl2Glsl_Translate_(false,fsglsl1,fsglsl,psName,parser,ETargetGLSL_110);
	
	#if 0
	LOG_PRINT("\nfs.HLSL:\n\"%s\"\n",fs);
	LOG_PRINT("\nfs.GLSL:\n\"%s\"\n",fsglsl.c_str());
	#endif

	return 0;
}

void run2(const std::string& acontents, const std::string& amain)
{
	std::string contents=acontents;
	std::cmatch cm;
	char buf[256];
	stx_snprintf(buf, 256, "([^;}].*?)\\s+%s\\s*\\(.*?\\)\0", amain.c_str());
    regex exp(buf);
    smatch res;
    string str = contents;

    string::const_iterator searchStart( str.cbegin() );
    while ( regex_search( searchStart, str.cend(), res, exp ) )
    {
        string str___=res[0];
	{
		std::cmatch cm;
    		stx_snprintf(buf, 256, "([^;}].*?)\\s+%s\\s*\\((.*?)\\)\0", amain.c_str());
		regex exp_(buf);
		std::regex_match ( str___.c_str(), cm, exp_, std::regex_constants::match_default );
		string str1 = cm[1];
		string str2 = cm[2];
		LOG_PRINT("sig: %s main %s\n", str1.c_str(), str2.c_str());
#if 0
		std::vector<std::string> args;
		split(str2, ',', args);
		for(unsigned int i=0;i<args.size();i++)
		{
			std::vector<std::string> arg=split(args[i]);
			for(unsigned int j=0;j<arg.size();j++)
			{
				LOG_PRINT("arg[%d] %s\n", j, arg[j]);
			}
			LOG_PRINT("args[%d] %s\n", i, args[i]);
		}
#endif
	}
        searchStart += res.position() + res.length();
    }
}

void run(const std::string& acontents, const std::string& amainvs, const std::string& amainfs)
{
	std::string contents=acontents;

      	{std::regex e("\\bROW_MAJOR\\b");
	contents=std::regex_replace (contents,e,"");}
	{std::regex e("\\bMVPSEMANTIC\\b");
	contents=std::regex_replace (contents,e,"");}


      	{std::regex e("\\};");
	contents=std::regex_replace (contents,e,"}");}
      	{std::regex e("(\\w+)\\s+;");
	contents=std::regex_replace (contents,e,"$1;");}


	std::vector<std::string> elems;
	split(contents, '\n', elems);
	std::string contents3=elems[0];
	for(unsigned int i=1;i<elems.size();i++)
		contents3+='\n'+trim(elems[i]);
//LOG_FNLN;
	contents=contents3;
	// ^ and $
{
    regex exp("\\bstruct\\b\\s+\\b\\w+\\b");
    smatch res;
    string str = contents;
    string::const_iterator searchStart( str.cbegin() );
    while ( regex_search( searchStart, str.cend(), res, exp ) )
    {
	_struct s;
        string str___=res[0];
	{
		std::cmatch cm;
    		regex exp_("\\bstruct\\b\\s+\\b(\\w+)\\b");
		std::regex_match ( str___.c_str(), cm, exp_, std::regex_constants::match_default );

		s.name=cm[1];
	}
	structs.push_back(s);
        searchStart += res.position() + res.length();
    }
}
//LOG_FNLN;
	unsigned int i=0;
if(1)
{
//LOG_FNLN;
    regex exp("(float[\\d]+)\\s+(\\w+)\\s*:\\s*(POSITION[\\d]*)");
//LOG_FNLN;
    smatch res;
    string str = contents;
//LOG_FNLN;

    string::const_iterator searchStart( str.cbegin() );
//LOG_FNLN;
    while ( regex_search( searchStart, str.cend(), res, exp ) )
    {
//LOG_FNLN;
	_shaderinput shaderinput;
	string str___=res[0];
	if(1)
	{
//LOG_FNLN;
		std::cmatch cm;
//LOG_FNLN;
    		regex exp_("(float[\\d]+)\\s+(\\w+)\\s*:\\s*(POSITION[\\d]*)");
//LOG_FNLN;
		std::regex_match ( str___.c_str(), cm, exp_, std::regex_constants::match_default );
//LOG_FNLN;
//LOG_PRINT("cm[1]=%x\n", cm[1]);

		shaderinput.type=cm[1];
//LOG_FNLN;
//LOG_PRINT("cm[2]=%x\n", cm[2]);
		shaderinput.name=cm[2];
//LOG_FNLN;
//LOG_PRINT("cm[3]=%x\n", cm[3]);
		shaderinput.semantic=cm[3];
		if(shaderinput.semantic.length()>8)
			shaderinput.i=atoi(shaderinput.semantic.c_str()+8);
//LOG_FNLN;

		//LOG_PRINT("sig: %s main %s\n", str1.c_str(), str2.c_str());
	}
//LOG_FNLN;
	structs[i].shaderinputs.push_back(shaderinput);
//LOG_FNLN;
       searchStart += res.position() + res.length();
//LOG_FNLN;
//LOG_PRINT("i=%d\n", i);
	i++;
    }
}
//LOG_FNLN;
	i=0;
if(1)
{
    regex exp("(float[\\d]+)\\s+(\\w+)\\s*:\\s*(TEXCOORD[\\d]*)");
    smatch res;
    string str = contents;

    string::const_iterator searchStart( str.cbegin() );
    while ( regex_search( searchStart, str.cend(), res, exp ) )
    {
	_shaderinput shaderinput;
	string str___=res[0];
	if(1)
	{
		std::cmatch cm;
    		regex exp_("(float[\\d]+)\\s+(\\w+)\\s*:\\s*(TEXCOORD[\\d]*)");
		std::regex_match ( str___.c_str(), cm, exp_, std::regex_constants::match_default );

		shaderinput.type=cm[1];
		shaderinput.name=cm[2];
		shaderinput.semantic=cm[3];
		if(shaderinput.semantic.length()>8)
			shaderinput.i=atoi(shaderinput.semantic.c_str()+8);

		{
			LOG_PRINT("\ttype:    %s\n", shaderinput.type.c_str());
			LOG_PRINT("\tname:    %s\n", shaderinput.name.c_str());
			LOG_PRINT("\tsemantic:%s\n", shaderinput.semantic.c_str());
			LOG_PRINT("\ti:       %d\n", shaderinput.i);
		}
	}
	structs[i].shaderinputs.push_back(shaderinput);
        searchStart += res.position() + res.length();
	i++;
    }
}
//LOG_FNLN;
if(1)
{	
	run2(acontents, amainvs);	
	run2(acontents, amainfs);
}

// regex exp("\\buniform\\b(.*?);");


//LOG_FNLN;
	for(unsigned int j=0;j<structs.size();j++)
	{
		LOG_PRINT("struct name:%s\n", structs[j].name.c_str());
		// vector<_shaderinput> shaderinputs;
		for(unsigned int k=0;k<structs[j].shaderinputs.size();k++)
		{
			LOG_PRINT("\ttype:    %s\n", structs[j].shaderinputs[k].type.c_str());
			LOG_PRINT("\tname:    %s\n", structs[j].shaderinputs[k].name.c_str());
			LOG_PRINT("\tsemantic:%s\n", structs[j].shaderinputs[k].semantic.c_str());
			LOG_PRINT("\ti:       %d\n", structs[j].shaderinputs[k].i);
		}
	}
//LOG_FNLN;
	replace(contents);
//LOG_FNLN;
	std::string vs;
	std::string fs;

	//addHLSLShader(contents.c_str(), "main", "main", vs ,fs);

	//LOG_PRINT("\nreplace(contents):\n\"%s\"\n",contents.c_str());
}
int ApplicationLogic()
{
	Hlsl2Glsl_Initialize ();

	std::string contents;
	if(1)
	{
		contents =STX_Service::LoadTxtFile("/SimpleTexture/SimpleTexture.shd", contents);
		run(contents, "mainVS", "mainPS");
	}
	else
	{
		contents=STX_Service::LoadTxtFile("/Smoke/VolumeRenderer.fx", contents);
		run(contents, "VS_RAYCAST_QUAD", "PS_RAYCAST_QUAD");
	}

	return 0;
}

