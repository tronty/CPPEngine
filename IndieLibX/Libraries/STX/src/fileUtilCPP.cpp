/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
/*
ILboolean ILAPIENTRY ilutD3D10TexFromFileInMemory(ID3D10Device *Device, void *Lump, ILuint Size, ID3D10Texture2D **Texture)
{
	iBindImageTemp();
	if (!ilLoadL(IL_TYPE_UNKNOWN, Lump, Size))
		return IL_FALSE;

	*Texture = ilutD3D10Texture(Device);

	return IL_TRUE;
}
*/
/*
ASSIMP_API const C_STRUCT aiScene* aiImportFileFromMemory(
    const char* pBuffer,
    unsigned int pLength,
    unsigned int pFlags,
    const char* pHint);

ASSIMP_API const C_STRUCT aiScene* aiImportFileFromMemoryWithProperties(
    const char* pBuffer,
    unsigned int pLength,
    unsigned int pFlags,
    const char* pHint,
    const C_STRUCT aiPropertyStore* pProps);
*/
#include <STX/STX.h>
using namespace std;

#if 0//defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
#define LOG_FNLN_X LOG_FNLN
#define LOG_PRINT_X LOG_PRINT
#else
#define LOG_FNLN_X
#define LOG_PRINT_X
#endif

#if defined(_MSC_VER)
#define search 	'/'
#define repl 	'\\'
#else
#define search 	'\\'
#define repl	'/'
#endif

#if defined(ANDROID)
#elif defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
#elif defined(__APPLE__)
#include <stdio.h>
#include <mach-o/dyld.h>
#include <libgen.h>
#endif

#include <sstream>

#if 1
#define STX_FNLN
#define STX_PRINT
#define LOG_FNLN
#define LOG_PRINT
#endif

// LNX: $HOME/Asiakirjat/IndieLib_resources
// OSX: $HOME/Documents/IndieLib_resources
// MSC: C:\\Users\\Tommi\\Documents\\IndieLib_resources
// NDK: IndieLib_resources
const char* pathIL()
{
#if defined(__MINGW32__) || defined(__MINGW64__)
	return "C:\\\\IndieLib_resources";
#else
	// Make sure we're running in the exe's path
	static char path[MAX_PATH];
	path[0]='\0';
	char *slash = 0;
#if defined(ANDROID)
	//stx_strlcpy(path, "/mnt/sdcard/", MAX_PATH);
#elif defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
    	stx_strlcpy(path, iosDocumentsFolder(), MAX_PATH);
	LOG_PRINT("iosDocumentsFolder=%s\n", path);
#elif defined(__APPLE__)
	uint32_t bufsize = MAX_PATH;
	_NSGetExecutablePath(path, &bufsize);
	chdir(path);
	path[0]='\0';
	stx_strlcpy(path, "./Documents", MAX_PATH);
#elif defined(LINUX)
	// Make sure we're running in the exe's directory
	if (realpath("/proc/self/exe", path)){
		char *slash = stx_strrchr(path, '/');
		if (slash) *slash = '\0';
		chdir(path);
		path[0]='\0';
	}
	stx_strlcpy(path, "../..", MAX_PATH);
#elif defined(_MSC_VER)
	if (GetModuleFileName(NULL, path, sizeof(path)))
	{
		char *slash = stx_strrchr(path, '\\');
		if (slash) *slash = '\0';
        	chdir(path);
		path[0]='\0';
	}
	stx_strlcpy(path, "..\\\\..", MAX_PATH);
#endif
#if !defined(ANDROID)
	stx_strlcat(path, "/IndieLib_resources", MAX_PATH);
#else
	stx_strlcat(path, "IndieLib_resources", MAX_PATH);
#endif
	if (path) 
	{
#if defined(ANDROID)
		LOG_PRINT_X("File=%s\n", path);
#endif
		return path;
	}
	return 0;
#endif
}
std::string stx_convertpath(const std::string& szPath_)
{
	{
		const std::string s = " ";
		std::string::size_type n = 0;
		if ( ( n = szPath_.find( s, n ) ) != std::string::npos )
		{
			LOG_PRINT("STX:fileUtilCPP.cpp:stx_convertpath:\n");
			LOG_PRINT("Spaces detected in a filepath!\n");
			LOG_PRINT("This may lead to a memory corruption!\n");
			LOG_PRINT("Trying to escape spaces!\n");
			LOG_PRINT("szPath_=%s\n", szPath_.c_str());
		}
	}
	std::string szPath;
	if((szPath_[0]=='.')&&((szPath_[1]=='/')||(szPath_[1]=='\\')))
		szPath+=std::string("/")+szPath_.substr(2,szPath_.length());
	else if((szPath_[0]!='/')&&(szPath_[0]!='\\'))
		szPath+=std::string("/")+szPath_;
	else
		szPath+=szPath_;
#if defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
    	std::string str=std::string(iosDocumentsFolder());
#elif defined(__APPLE__)
	std::string str=std::string("../../../../../IndieLib_resources");
#elif defined(ANDROID)
	std::string str=std::string("IndieLib_resources");
#elif defined(__MINGW32__) || defined(__MINGW64__)
	std::string str=std::string("C:\\\\IndieLib_resources");
#elif defined(LINUX)
	std::string str=std::string("../../IndieLib_resources");
#elif defined(_MSC_VER)
	std::string str=std::string("..\\\\..\\\\IndieLib_resources");
#endif
#if 0 // ???
	STX_PRINT("szPath.substr(0,26)=%s\n", szPath.substr(0,26).c_str());
	if(szPath.substr(0,26)!="/../../IndieLib_resources/")
		str+=szPath;
	else
		str=szPath;
#else
	str+=szPath;
#endif
	{
		const std::string s = "\\ ";
		const std::string t = " ";
		std::string::size_type n = 0;
		while ( ( n = str.find( s, n ) ) != std::string::npos )
		{
    			str.replace( n, s.size(), t );
    			n += t.size();
		}
	}
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
	{
		const std::string s = "/";
		const std::string t = "\\\\";
		std::string::size_type n = 0;
		while ( ( n = str.find( s, n ) ) != std::string::npos )
		{
    			str.replace( n, s.size(), t );
    			n += t.size();
		}
	}
	{
		const std::string s = "\\.\\";
		const std::string t = "\\";
		std::string::size_type n = 0;
		while ( ( n = str.find( s, n ) ) != std::string::npos )
		{
    			str.replace( n, s.size(), t );
    			n += t.size();
		}
	}
#else
	std::replace( str.begin(), str.end(), '\\', '/');
	{
		const std::string s = "//";
		const std::string t = "/";
		std::string::size_type n = 0;
		while ( ( n = str.find( s, n ) ) != std::string::npos )
		{
    			str.replace( n, s.size(), t );
    			n += t.size();
		}
	}
	{
		const std::string s = "/./";
		const std::string t = "/";
		std::string::size_type n = 0;
		while ( ( n = str.find( s, n ) ) != std::string::npos )
		{
    			str.replace( n, s.size(), t );
    			n += t.size();
		}
	}
#endif
	{
		const std::string s = " ";
		const std::string t = "\\ ";
		std::string::size_type n = 0;
		while ( ( n = str.find( s, n ) ) != std::string::npos )
		{
    			str.replace( n, s.size(), t );
    			n += t.size();
		}
	}

#if defined(__APPLE__) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
	stx_ReplaceAll(str, "../../../../../IndieLib_resources/../../../../../IndieLib_resources/", "../../../../../IndieLib_resources/"); // ???
#elif defined(_MSC_VER)
	stx_ReplaceAll(str, "..\\\\..\\\\IndieLib_resources\\\\..\\\\..\\\\IndieLib_resources\\\\", "..\\\\..\\\\IndieLib_resources\\\\"); 
#else
	stx_ReplaceAll(str, "../../IndieLib_resources/../../IndieLib_resources/", "../../IndieLib_resources/"); // ???
#endif
	if(!stx_fileExists(str.c_str()))
	{
		LOG_PRINT("STX:fileUtilCPP.cpp:stx_convertpath:\n");
		LOG_PRINT("!stx_fileExists(%s)\n", str.c_str());
		//LOG_PRINT("Exiting to prevent memory corruption!\n");
#if 0
		FILE * pFileTXT = fopen ("../../log.txt","a");
 		fprintf (pFileTXT, "%s\n", str.c_str());
 		fclose (pFileTXT);
#endif
		//stx_exit(0);
	}

	return str;
}

std::string& stx_p_convertpath(const std::string& szPath)
{
	static std::string tmp;
	tmp="";
	tmp.append(pathIL());
	tmp.append(szPath);

	std::string from;
	std::string to;
#if defined(_MSC_VER)
	from="/";
	to="\\\\";
#else
	from="\\\\";
	to="/";
#endif
	stx_ReplaceAll(tmp, from, to);	

	from=pathIL();
	from.append(pathIL());
	to=pathIL();
	stx_ReplaceAll(tmp, from, to);
#if defined(_MSC_VER)
	from="\\\\.\\\\";
	to="\\\\";
#else
	from="/./";
	to="/";
#endif
	stx_ReplaceAll(tmp, from, to);

	LOG_PRINT("%s:\n%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	LOG_PRINT("%s:\n", tmp.c_str());

	return tmp;
}

const char* STXCALL stx_convertpathC(const char* szPath)
{
	//LOG_PRINT("%s:\n%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	//LOG_PRINT("%s:\n", szPath);
	std::string szPath2=szPath;
	std::string szPath3=stx_convertpath(szPath2);
	if(stx_strcmp(szPath3.c_str(),"")==0)
	{
		szPath3="../IndieLib_resources/";
		szPath3.append(szPath);
	}
	LOG_PRINT("%s:\n%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	LOG_PRINT("%s:\n", szPath3.c_str());
	return szPath3.c_str();
}

