/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
/**
 *  \file stx_stdinc.h
 *
 *  This is a general header that includes C language support.
 */

#ifndef _stx_stdinc_h
#define _stx_stdinc_h

// define USE_TINYXML 1

#undef __LOG_glDrawElements_

#if defined(__APPLE__)
#include <string>
/*
cannot combine with previous 'char16_t' declaration specifier
typedef __char16_t char16_t;
                   ^
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../include/c++/v1/__config:264:20: error: cannot combine with previous 'char32_t' declaration specifier
typedef __char32_t char32_t;
*/
#include <vector>
#endif

#define deleteObject(A){ if(A){ delete   A; A = NULL; } }
#define deleteArray(A) { if(A){ delete[] A; A = NULL; } }

typedef unsigned char ubyte;

#include <limits.h>
#if !defined(__APPLE__) //OS_IPHONE) && !defined(IPHONE_SIMULATOR)
#include <cmath>
#else
#include <math.h>
#endif
#if !defined(SWIG) && !defined(__APPLE__)
#include <algorithm>
#endif

#if !defined(_MSC_VER)
#include <stdint.h>
typedef void* HANDLE;
#elif 1//!defined(__MINGW32__) && !defined(__MINGW64__)
#include <windows.h>
#include <mmsystem.h>
typedef WORD       __WORD__;
typedef DWORD       __DWORD__;
#else
typedef void* HANDLE;
typedef HANDLE HWND;
typedef HANDLE HDC;
typedef HANDLE HINSTANCE;
typedef HANDLE HMODULE;
typedef char CHAR;
typedef CHAR *PSTR, *LPSTR;

#define LARGE_INTEGER int
#define GUID int
typedef int WINBOOL;
typedef const wchar_t* LPCWSTR;
typedef const char* LPTSTR;

typedef int BOOL;
typedef int LONG;
typedef unsigned int ULONG;
typedef LONG HRESULT;

typedef struct _RECT {
  LONG left;
  LONG top;
  LONG right;
  LONG bottom;
} RECT, *PRECT, *LPRECT;

typedef unsigned short __WORD__;
typedef unsigned int __DWORD__;
typedef unsigned int __D3DCOLOR__;
typedef short SHORT;

BOOL GetWindowRect(HWND hWnd, LPRECT lpRect);
__DWORD__ timeGetTime();
#endif
#include <stddef.h>
#include <signal.h>

#if defined(ANDROID)
#include <unistd.h>
//Call: sleep(seconds) or usleep(microseconds)
#endif

#if /*!defined(WIN_PHONE_APP) && */!defined (QT_BUILD)
#if defined(ANDROID) || defined(WIN_PHONE_APP) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR) || defined(__MINGW32__) || defined(__MINGW64__)
    #undef SDL_1_2_15_BUILD
    #undef SDL_2_0_5_BUILD
    //define GL_CODE_BUILD 1

typedef int8_t Sint8;
typedef uint8_t Uint8;
typedef int16_t Sint16;
typedef uint16_t Uint16;

    typedef int32_t Sint32;
    typedef uint32_t Uint32;
    typedef int64_t Sint64;
    typedef uint64_t Uint64;

#elif defined(_MSC_VER) || defined(__APPLE__)
    #include <SDL.h>
    #include <SDL_syswm.h>
    #define SDL_2_0_5_BUILD 1
#elif 0//defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
    #include <SDL.h>
    #define SDL_2_0_5_BUILD 1
#elif 1
    #include <SDL2/SDL.h>
#if defined(SDL_VIDEO_DRIVER_X11) || defined(SDL_VIDEO_DRIVER_WINDOWS)
    #include <SDL2/SDL_syswm.h>
#endif
    #define SDL_2_0_5_BUILD 1
#else
    #include <SDL/SDL.h>
#if defined(SDL_VIDEO_DRIVER_X11)
    #include <SDL/SDL_syswm.h>
#endif
    #define SDL_1_2_15_BUILD 1
#endif
#else
#ifdef _MSC_VER
typedef signed __int8		Sint8;
typedef unsigned __int8		Uint8;
typedef signed __int16		Sint16;
typedef unsigned __int16	Uint16;
#endif
typedef int32_t	    		Sint32;
typedef uint32_t		Uint32;
#if HAVE_64
#ifdef _MSC_VER
typedef signed __int64		int64_t;
typedef unsigned __int64	uint64_t;
typedef signed __int64		Sint64;
typedef unsigned __int64	Uint64;
#endif
typedef long long int		int64_t;
typedef long long unsigned int	uint64_t;
typedef long long int	        Sint64;
typedef long long unsigned int	Uint64;
#endif
//elif defined(WIN_PHONE_APP) || defined(WIN8_APP) || defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
/**
 * \brief A signed 8-bit integer type.
 */
typedef int8_t Sint8;
/**
 * \brief An unsigned 8-bit integer type.
 */
typedef uint8_t Uint8;
/**
 * \brief A signed 16-bit integer type.
 */
typedef int16_t Sint16;
/**
 * \brief An unsigned 16-bit integer type.
 */
typedef uint16_t Uint16;
/**
 * \brief A signed 32-bit integer type.
 */
typedef int32_t Sint32;
/**
 * \brief An unsigned 32-bit integer type.
 */
typedef uint32_t Uint32;
#if HAVE_64
/**
 * \brief A signed 64-bit integer type.
 */
typedef int64_t Sint64;
/**
 * \brief An unsigned 64-bit integer type.
 */
typedef uint64_t Uint64;
#endif
#endif

/* @} *//* Basic data types */

#if defined(_MSC_VER)
#define APPLICATIONRESOURCES "\\\\IndieLib_resources"
#else
#define APPLICATIONRESOURCES "/IndieLib_resources"
#endif

#if defined(SWIG) || defined(__MINGW32__) || defined(__MINGW64__)
#define DBG_HALT {stx_exit(-1);}
#else
#ifndef DBG_HALT
#ifdef _WIN32
#define DBG_HALT __asm{ int 0x10 }
#elif defined (LINUX)
//define DBG_HALT {stx_exit(-1);}
//define DBG_HALT {raise(SIGABRT);}
#define DBG_HALT {__asm__("int $3");}
#else
#define DBG_HALT
#endif
#endif
#endif

#ifdef _MSC_VER
#include <functional>
#endif
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <string.h>
#include <memory.h>

#if defined(__cplusplus) && !defined(__APPLE__)
#include <ctype.h>
#include <string>
#endif

#ifndef _MSC_VER
#include <stddef.h>
#endif

typedef enum _stx_bool
{
    stx_FALSE = 0,
    stx_TRUE = 1
} stx_bool;

#define STXEXTERN
#define STXDECLSPEC
#define STXCALL
#define STXAPI
#define STXAPIENTRY

STXEXTERN STXDECLSPEC std::string STXCALL stx_GetFileExtension(const std::string& path);
STXEXTERN STXDECLSPEC std::string STXCALL stx_GetFileName(const std::string& path);

STXEXTERN STXDECLSPEC std::string STXCALL stx_convertpath(const std::string& szPath);

STXEXTERN STXDECLSPEC bool STXCALL str_ends_with(const char * str, const char * suffix);

STXEXTERN STXDECLSPEC bool STXCALL str_ends_with(const std::string& s, const std::string& suffix);

#ifdef __cplusplus
extern "C" {
#endif

int getNativeHeapSize();
int getNativeHeapAllocatedSize();
int getNativeHeapFreeSize();

#if defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
const char *iosDocumentsFolder();
void iosLog(const char * msg);
#endif
STXEXTERN STXDECLSPEC const char* STXCALL stx_convertpathC(const char* szPath);
STXEXTERN STXDECLSPEC char * STXCALL stx_getcwd(char * _Path);
STXEXTERN STXDECLSPEC size_t STXCALL stx_strlen(const char *str);
STXEXTERN STXDECLSPEC size_t STXCALL stx_strlcat(char *dst, const char *src, size_t maxlen);
STXEXTERN STXDECLSPEC size_t STXCALL stx_strlcpy(char *dst, const char *src, size_t maxlen);
STXEXTERN STXDECLSPEC int STXCALL stx_strcmp(const char *str1, const char *str2);
#ifndef _MSC_VER
STXEXTERN STXDECLSPEC int STXCALL stx_min(int x, int y);
#endif
#if 0
STXEXTERN STXDECLSPEC void STXCALL CloseSTXWindow();
STXEXTERN STXDECLSPEC void STXCALL STXSwapBuffers();
STXEXTERN STXDECLSPEC void STXCALL OpenSTXWindow(unsigned int awidth, unsigned int aheight, unsigned int abpp);
#endif

#if 0//defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
namespace std
{
	inline double fabs(double f)
	{
    		if (f < 0) return -f;
    		return f;
	}
};
#endif

#ifdef __cplusplus
}
#endif

STXEXTERN STXDECLSPEC void *STXCALL stx_malloc(size_t size);
STXEXTERN STXDECLSPEC void *STXCALL stx_calloc(size_t nmemb, size_t size);
STXEXTERN STXDECLSPEC void *STXCALL stx_realloc(void *mem, size_t size);
STXEXTERN STXDECLSPEC void STXCALL stx_free(void *mem);

STXEXTERN STXDECLSPEC char *STXCALL stx_getenv(const char *name);
//STXEXTERN STXDECLSPEC int STXCALL stx_setenv(const char *name, const char *value, int overwrite);

STXEXTERN STXDECLSPEC void STXCALL stx_qsort(void *base, size_t nmemb, size_t size, int (*compare) (const void *, const void *));

STXEXTERN STXDECLSPEC int STXCALL stx_abs(int x);

#ifndef _MSC_VER
STXEXTERN STXDECLSPEC int STXCALL stx_min(int x, int y);
STXEXTERN STXDECLSPEC int STXCALL stx_max(int x, int y);
#endif

STXEXTERN STXDECLSPEC int STXCALL stx_isdigit(int x);
STXEXTERN STXDECLSPEC int STXCALL stx_isspace(int x);
STXEXTERN STXDECLSPEC int STXCALL stx_toupper(int x);
STXEXTERN STXDECLSPEC int STXCALL stx_tolower(int x);

STXEXTERN STXDECLSPEC void *STXCALL stx_memset(void *dst, int c, size_t len);

STXEXTERN STXDECLSPEC void *STXCALL stx_memcpy(void *dst, const void *src, size_t len);

STXEXTERN STXDECLSPEC void *STXCALL stx_memmove(void *dst, const void *src, size_t len);
STXEXTERN STXDECLSPEC int STXCALL stx_memcmp(const void *s1, const void *s2, size_t len);

STXEXTERN STXDECLSPEC size_t STXCALL stx_wcslen(const wchar_t *wstr);
STXEXTERN STXDECLSPEC size_t STXCALL stx_wcslcpy(wchar_t *dst, const wchar_t *src, size_t maxlen);
STXEXTERN STXDECLSPEC size_t STXCALL stx_wcslcat(wchar_t *dst, const wchar_t *src, size_t maxlen);

STXEXTERN STXDECLSPEC size_t STXCALL stx_strlen(const char *str);
STXEXTERN STXDECLSPEC size_t STXCALL stx_strlcpy(char *dst, const char *src, size_t maxlen);
STXEXTERN STXDECLSPEC size_t STXCALL stx_utf8strlcpy(char *dst, const char *src, size_t dst_bytes);
STXEXTERN STXDECLSPEC size_t STXCALL stx_strlcat(char *dst, const char *src, size_t maxlen);
STXEXTERN STXDECLSPEC char *STXCALL stx_strdup(const char *str);
STXEXTERN STXDECLSPEC char *STXCALL stx_strrev(char *str);
STXEXTERN STXDECLSPEC char *STXCALL stx_strupr(char *str);
STXEXTERN STXDECLSPEC char *STXCALL stx_strlwr(char *str);
STXEXTERN STXDECLSPEC char *STXCALL stx_strchr(const char *str, int c);
STXEXTERN STXDECLSPEC char *STXCALL stx_strrchr(const char *str, int c);
STXEXTERN STXDECLSPEC char *STXCALL stx_strstr(const char *haystack, const char *needle);

STXEXTERN STXDECLSPEC char *STXCALL stx_itoa(int value, char *str, int radix);
STXEXTERN STXDECLSPEC char *STXCALL stx_uitoa(unsigned int value, char *str, int radix);
STXEXTERN STXDECLSPEC char *STXCALL stx_ltoa(int value, char *str, int radix);
STXEXTERN STXDECLSPEC char *STXCALL stx_ultoa(unsigned int value, char *str, int radix);

STXEXTERN STXDECLSPEC int STXCALL stx_atoi(const char *str);
STXEXTERN STXDECLSPEC double STXCALL stx_atof(const char *str);
STXEXTERN STXDECLSPEC int STXCALL stx_strtol(const char *str, char **endp, int base);
STXEXTERN STXDECLSPEC unsigned int STXCALL stx_strtoul(const char *str, char **endp, int base);
#if HAVE_64
STXEXTERN STXDECLSPEC char *STXCALL stx_lltoa(Sint64 value, char *str, int radix);
STXEXTERN STXDECLSPEC char *STXCALL stx_ulltoa(Uint64 value, char *str, int radix);
STXEXTERN STXDECLSPEC Sint64 STXCALL stx_strtoll(const char *str, char **endp, int base);
STXEXTERN STXDECLSPEC Uint64 STXCALL stx_strtoull(const char *str, char **endp, int base);
#endif
STXEXTERN STXDECLSPEC double STXCALL stx_strtod(const char *str, char **endp);

STXEXTERN STXDECLSPEC int STXCALL stx_strcmp(const char *str1, const char *str2);
STXEXTERN STXDECLSPEC int STXCALL stx_strncmp(const char *str1, const char *str2, size_t maxlen);
STXEXTERN STXDECLSPEC int STXCALL stx_strcasecmp(const char *str1, const char *str2);
STXEXTERN STXDECLSPEC int STXCALL stx_strncasecmp(const char *str1, const char *str2, size_t len);

STXEXTERN STXDECLSPEC int STXCALL stx_sscanf(const char *text, const char *fmt, ...);
#ifndef SWIG
STXEXTERN STXDECLSPEC int STXCALL stx_vsscanf(const char *text, const char *fmt, va_list ap);
STXEXTERN STXDECLSPEC int STXCALL stx_vsnprintf(char *text, size_t maxlen, const char *fmt, va_list ap);
#endif
STXEXTERN STXDECLSPEC int STXCALL stx_snprintf(char *text, size_t maxlen, const char *fmt, ...);

#ifndef HAVE_M_PI
#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288   /* pi */
#endif
#endif
/*
STXEXTERN STXDECLSPEC double STXCALL stx_acos(double x);
STXEXTERN STXDECLSPEC double STXCALL stx_asin(double x);
STXEXTERN STXDECLSPEC double STXCALL stx_atan(double x);
STXEXTERN STXDECLSPEC double STXCALL stx_atan2(double x, double y);
STXEXTERN STXDECLSPEC double STXCALL stx_ceil(double x);
STXEXTERN STXDECLSPEC double STXCALL stx_copysign(double x, double y);
STXEXTERN STXDECLSPEC double STXCALL stx_cos(double x);
STXEXTERN STXDECLSPEC float STXCALL stx_cosf(float x);
STXEXTERN STXDECLSPEC double STXCALL stx_fabs(double x);
STXEXTERN STXDECLSPEC double STXCALL stx_floor(double x);
STXEXTERN STXDECLSPEC double STXCALL stx_log(double x);
STXEXTERN STXDECLSPEC double STXCALL stx_pow(double x, double y);
STXEXTERN STXDECLSPEC double STXCALL stx_scalbn(double x, int n);
STXEXTERN STXDECLSPEC double STXCALL stx_sin(double x);
STXEXTERN STXDECLSPEC float STXCALL stx_sinf(float x);
STXEXTERN STXDECLSPEC double STXCALL stx_sqrt(double x);
*/
//define stx_itoa(value, string, radix)	stx_ltoa((int)value, string, radix)

STXEXTERN STXAPI char * STXAPIENTRY stx_ltoa(int value, char *string, int radix);
//define stx_uitoa(value, string, radix)	stx_ultoa((int)value, string, radix)
STXEXTERN STXAPI char * STXAPIENTRY stx_ultoa(unsigned int value, char *string, int radix);

STXEXTERN STXAPI int STXAPIENTRY stx_fileExists(const char* fileName);
STXEXTERN STXAPI int STXAPIENTRY stx_isopened(FILE* _File);
STXEXTERN STXAPI void STXAPIENTRY stx_closeall();
STXEXTERN STXAPI char * STXAPIENTRY stx_getcwd(char * _Path);
STXEXTERN STXAPI int STXAPIENTRY stx_chdir(const char * _Path);
//STXEXTERN STXAPI int STXAPIENTRY stx_mkdir(const char * _Path);
//STXEXTERN STXAPI int STXAPIENTRY stx_rmdir(const char * _Path);

STXEXTERN STXAPI unsigned int STXAPIENTRY stx_randnext(void);
STXEXTERN STXAPI int STXAPIENTRY stx_rand(void);
STXEXTERN STXAPI void STXAPIENTRY stx_srand(unsigned int seed);
STXEXTERN STXAPI void STXAPIENTRY stx_exit(int a);

#ifndef __inttype1__
#define __inttype1__

#ifdef _WIN32
            enum inttype1 {
                DO1_INT,
                DO1_LONG,
                DO1_LONGLONG
            };// _inttype1 = DO_INT;

            enum inttype2 {
                DO2_SHORT,
                DO2_INT,
                DO2_LONG,
                DO2_LONGLONG
            };
//define _INTTYPE1 inttype1
//define _INTTYPE2 inttype2
#else
typedef enum _inttype1 {
                DO1_INT,
                DO1_LONG,
                DO1_LONGLONG
            } inttype1;

typedef            enum _inttype2 {
                DO2_SHORT,
                DO2_INT,
                DO2_LONG,
                DO2_LONGLONG
            } inttype2;
//define _INTTYPE1 _inttype1
//define _INTTYPE2 _inttype2
#endif
#endif

/* Ends C function definitions when using C++ */

#ifndef MAX_PATH
#define MAX_PATH 260
#endif
//define MAX_BUF_SIZE 65535

typedef unsigned char       BYTE;
#ifndef _MSC_VER
typedef unsigned short      __WORD__;
#if 0
typedef unsigned int       __DWORD__;
#else
typedef unsigned int        __DWORD__;
#endif
#endif
typedef const char*         LPCSTR;
typedef unsigned int        UINT;

STXEXTERN STXAPI const char STXAPIENTRY stx_getOsSeparator();
STXEXTERN STXAPI int STXAPIENTRY stx_fileExists(const char* fileName);
//STXEXTERN STXAPI size_t STXAPIENTRY stx_fwrite(const void * _Str,  size_t _Size,  size_t _Count, FILE* _File);


#define stxEPSILON     0.0001f

#define STX_SEEK_CUR    1
#define STX_SEEK_END    2
#define STX_SEEK_SET    0

#ifndef SWIG
#if defined(__cplusplus) && (defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR) || defined(WIN_PHONE_APP))
extern "C" {
#endif
int init(const char* aTitle);
void render();
void deinit();
#if defined(__cplusplus) && (defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR) || defined(WIN_PHONE_APP))
}
#endif
#endif

#if !defined(SWIG)
#ifdef __cplusplus
   extern "C"
#endif
int main(int argc, char *argv[]);

#ifdef __cplusplus
   extern "C"
#endif
int SDL_main(int argc, char *argv[]);

#if defined(ANDROID)
#define ApplicationLogic() __mainX__(int argc, char *argv[])
#elif defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
#define ApplicationLogic() __mainX__(int argc, char *argv[])
#elif defined(WIN_PHONE_APP)
#define ApplicationLogic() __mainX__(int argc, char *argv[])
#elif defined(_MSC_VER) && !defined(WIN_PHONE_APP)
//pragma comment(linker, "/ENTRY:WinMainCRTStartup")
//pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#define ApplicationLogic() WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#elif !defined(_MSC_VER)
#define ApplicationLogic() main(int argc, char *argv[])
#endif
#endif

#if !defined(_MSC_VER)
#define APPLICATIONLOGIC(X) int main(int argc, char *argv[]) \
{ \
	IRenderer* r=IRenderer::GetRendererInstance(X); \
	init(X); \
	while (!STX_Service::GetInputInstance()->OnKeyPress (KEY_ESCAPE) && !STX_Service::GetInputInstance()->Quit()) \
	{ \
		STX_Service::GetInputInstance()->Update(); \
		render(); \
	} \
	return 0; \
}
#else
#define APPLICATIONLOGIC(X) int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) \
{ \
	IRenderer* r=IRenderer::GetRendererInstance(X); \
	init(X); \
	while (!STX_Service::GetInputInstance()->OnKeyPress (KEY_ESCAPE) && !STX_Service::GetInputInstance()->Quit()) \
	{ \
		STX_Service::GetInputInstance()->Update(); \
		render(); \
	} \
	return 0; \
}
#endif

#if !defined(_MSC_VER) && !defined(__MINGW32__)
__DWORD__ timeGetTime();
#endif

#define LOG_TRACE_ON 1
#define LOG_TAG_NONE
#define LOG_START_NONE
#define LOG_PRINT_NONE(...)
#define LOG_FNLN_NONE
#if defined(_DEBUG) && defined(LOG_TRACE_ON)
#if defined(_MSC_VER)
#define LOG_TAG "libIndieLibX"
#define LOG_START
#define LOG_PRINT(...)
#define LOG_FNLN
#elif defined(_LINUX)
#define LOG_TAG "libIndieLibX"
#define LOG_START
#define LOG_PRINT(...) printf(__VA_ARGS__);
#define LOG_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#elif defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
// See in file /home/tronty/IndieLibX/Libraries/STX/src/fileUtilM.m function iosLog
#define LOG_START iosLog("")
#define LOG_PRINT(...) do { \
    std::string iosbuf; \
    iosbuf.resize(1024); \
    int num_bytes = snprintf(&iosbuf[0], 1024, __VA_ARGS__); \
    if(num_bytes < 1024) iosbuf.resize(num_bytes); \
    iosLog(iosbuf.c_str()); \
} while (0)
#define LOG_FNLN do { \
    std::string iosbuf; \
    iosbuf.resize(1024); \
  	int num_bytes = snprintf(&iosbuf[0], 1024, "%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__); \
	if(num_bytes < 1024) iosbuf.resize(num_bytes); \
    iosLog(iosbuf.c_str()); \
} while (0)
#elif defined(ANDROID)

#if 0
#define LOG_TAG "libIndieLibX"
#define LOG_START
#define LOG_PRINT
#define LOG_FNLN
#else
#include <jni.h>
#include <android/log.h>
#define LOG_TAG "libIndieLibX"
#define LOG_START
#define LOG_PRINT(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__);
//define LOG_PRINT(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__);
#define LOG_FNLN __android_log_print(ANDROID_LOG_INFO,LOG_TAG, "%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#endif
#elif defined(WIN_PHONE_APP)
#define LOG_START
#define LOG_PRINT
#define LOG_FNLN
#elif defined(_MSC_VER)
#define LOG_START
#define LOG_PRINT
#define LOG_FNLN
#else
#if 0
inline void stx_write_to_log_file_start()
{
	char buf[64];
	//stx_snprintf(buf, 64, "%s.debug.log", STX_Service::GetWindowInstance()->GetCaption());
	stx_snprintf(buf, 64, "%s.debug.log", "IndieLibX");
  	//FILE * pFile = fopen (buf, "w");
  	FILE * pFile = fopen (buf, "a+");
  	fprintf(pFile, "");
	fclose (pFile);
}

inline void stx_write_to_log_file(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

	char buf[64];
	//stx_snprintf(buf, 64, "%s.debug.log", STX_Service::GetWindowInstance()->GetCaption());
	stx_snprintf(buf, 64, "%s.debug.log", "IndieLibX");
  	FILE * pFile = fopen (buf, "a+");
  	fprintf(pFile, fmt, ap);
	fclose (pFile);

    va_end(ap);
}

#define LOG_START stx_write_to_log_file_start();
#define LOG_PRINT(...) printf(__VA_ARGS__);stx_write_to_log_file(__VA_ARGS__);
#define LOG_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);stx_write_to_log_file("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#elif 1
#define LOG_TAG "libIndieLibX"
#define LOG_START
#define LOG_PRINT(...) printf(__VA_ARGS__);
#define LOG_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#elif 0
#define LOG_TAG "libIndieLibX"
#define LOG_START
#define LOG_PRINT
#define LOG_FNLN
#endif
#endif
#endif

#define ErrorMsg LOG_PRINT

#ifdef LINUX
#define STX_PRINT(...) printf(__VA_ARGS__)
#define STX_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__)
#else
#define STX_PRINT(...)
#define STX_FNLN
#endif

#ifdef __cplusplus
inline STXEXTERN STXAPI std::string& replaceAll(std::string & data, std::string toSearch, std::string replaceStr)
{
	// Get the first occurrence
	size_t pos = data.find(toSearch);

	// Repeat till end is reached
	while( pos != std::string::npos)
	{
		// Replace this occurrence of Sub String
		data.replace(pos, toSearch.size(), replaceStr);
		// Get the next occurrence from the current position
		pos =data.find(toSearch, pos + replaceStr.size());
	}
	return data;
}
inline STXEXTERN STXAPI void stx_ReplaceAll(std::string & data, std::string toSearch, std::string replaceStr)
{
	// Get the first occurrence
	size_t pos = data.find(toSearch);

	// Repeat till end is reached
	while( pos != std::string::npos)
	{
		// Replace this occurrence of Sub String
		data.replace(pos, toSearch.size(), replaceStr);
		// Get the next occurrence from the current position
		pos =data.find(toSearch, pos + replaceStr.size());
	}
}
#endif

STXEXTERN STXAPI bool STXAPIENTRY stx_isBigEndian();
STXEXTERN STXAPI bool STXAPIENTRY stx_isLittleEndian();
STXEXTERN STXAPI void STXAPIENTRY stx_exit(int a);
STXEXTERN STXAPI void * STXAPIENTRY stx_malloc(size_t size);

STXEXTERN STXAPI void * STXAPIENTRY stx_calloc(size_t nmemb, size_t size);
STXEXTERN STXAPI void * STXAPIENTRY stx_realloc(void *mem, size_t size);
STXEXTERN STXAPI void STXAPIENTRY stx_free(void *mem);
STXEXTERN STXAPI void STXAPIENTRY stx_closeall();
STXEXTERN STXAPI char * STXAPIENTRY stx_getenv(const char *name);
STXEXTERN STXAPI int STXAPIENTRY stx_putenv(const char *variable);
STXEXTERN STXAPI void STXAPIENTRY stx_qsort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
STXEXTERN STXAPI void * STXAPIENTRY stx_memset(void *dst, int c, size_t len);
STXEXTERN STXAPI void * STXAPIENTRY stx_revcpy(void *dst, const void *src, size_t len);
STXEXTERN STXAPI void * STXAPIENTRY stx_memmove(void * _Dst, const void * _Src, size_t _Size);
STXEXTERN STXAPI int STXAPIENTRY stx_memcmp(const void *s1, const void *s2, size_t len);
STXEXTERN STXAPI float STXAPIENTRY stxAbs(float f);
STXEXTERN STXAPI int STXAPIENTRY stxEq(float fValue1, float fValue2);
STXEXTERN STXAPI void * STXAPIENTRY stx_memcpy(void *d, const void *s, size_t l);
STXEXTERN STXAPI char * STXAPIENTRY stx_strtok(char * _Str, const char * _Delim);
STXEXTERN STXAPI char * STXAPIENTRY stx_getcwd(char * _Path);
STXEXTERN STXAPI unsigned int STXAPIENTRY stx_randnext(void);
STXEXTERN STXAPI int STXAPIENTRY stx_rand(void);
STXEXTERN STXAPI void STXAPIENTRY stx_srand(unsigned int seed);
STXEXTERN STXAPI int STXAPIENTRY stx_isopened(FILE* _File);
STXEXTERN STXAPI int STXAPIENTRY stx_fileExists(const char* fileName);
STXEXTERN STXAPI const char STXAPIENTRY stx_getOsSeparator();
STXEXTERN STXAPI const char* STXAPIENTRY stx_getOsSeparatorString();

#ifndef SWIG
#if defined(LINUX) && !defined(__APPLE__)

#include <assert.h>
#if !defined(CYGWIN64)
 #include <execinfo.h>
#include <mcheck.h>
#endif
     #include <stdio.h>
     #include <stdlib.h>

     /* Obtain a backtrace and print it to stdout. */
inline void print_trace(const char *file, int line)
{
#if !defined(CYGWIN64)
    const size_t max_depth = 100;
    size_t stack_depth;
    void *stack_addrs[max_depth];
    char **stack_strings;

    stack_depth = backtrace(stack_addrs, max_depth);
    stack_strings = backtrace_symbols(stack_addrs, stack_depth);

    printf("Call stack from %s:%d:\n", file, line);

    size_t i = 1;
    for (i = 1; i < stack_depth; i++) {
        printf("    %s\n", stack_strings[i]);
    }
    free(stack_strings); // malloc()ed by backtrace_symbols
    //fflush(out);
#endif
}
#define STX_STACK_TRACE print_trace(__FILE__,__LINE__);
#define STX_BACKTRACE print_trace(__FILE__,__LINE__);
#else
inline void print_trace(const char *file, int line)
{
    printf("%s:%d:\n", file, line);
}
#define STX_STACK_TRACE
#define STX_BACKTRACE
#endif
#endif

#define STXASSERT(str) {assert(str);}

#ifdef stx_max
#undef stx_max
#endif
#define stx_max(x, y)	(((x) > (y)) ? (x) : (y))
//define max std::max

#ifdef stx_min
#undef stx_min
#endif
#define stx_min(x, y)	(((x) < (y)) ? (x) : (y))
//define min std::min

typedef enum _STX_bool
{
	STX_FALSE=0,
	STX_TRUE =1
} STX_bool;

#include "Console.h"

#if defined(SDL_VIDEO_DRIVER_X11)
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
extern Display* g_X11_display;
extern Window g_X11_window;
#endif

inline int stx_getClosestPowerOfTwo(int digit)

{

  if(!digit)

    return 1;



  double log2  = log(double(abs(digit)))/log(2.0),

         flog2 = floor(log2),

         frac  = log2 - flog2;



  return (frac < 0.5) ? (int)pow(2.0, flog2) : (int)pow(2.0, flog2 + 1.0);

}

inline STXEXTERN STXAPI double STXAPIENTRY stx_floor(double x)
{
#if defined(_MSC_VER)
	return floor(x);
#elif defined(HAVE_FLOOR)
    return floor(x);
#elif defined(SDL_2_0_5_BUILD) || defined(SDL_1_2_15_BUILD)
    return SDL_uclibc_floor(x);
#else
    return ((int) x);
#endif /* HAVE_FLOOR */
}

#endif

