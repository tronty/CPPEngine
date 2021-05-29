/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef _STXCPP_H_
#define _STXCPP_H_

#if 1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#elif 0
#define _INC_STDIO
#define _INC_STRING
//define _INC_STDLIB
#define _INC_CTYPE
#define _INC_MEMORY
#endif

#ifdef fopen
#undef fopen
#endif
#ifdef strlen
#undef strlen
#endif

#if defined(_MSC_VER) && !defined(STXCPPINTERNAL_H)
//pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\kernel32.lib")
//pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\gdi32.lib")
//pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\user32.lib")
//pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\comctl32.lib")
//pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\advapi32.lib")
//pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\winmm.lib")
//pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\shell32.lib")
//pragma comment(lib, "C:\\IndieLibX\\lib\\segvcatch.lib")
//pragma comment(lib, "C:\\IndieLibX\\lib\\png_x.lib")
//pragma comment(lib, "C:\\IndieLibX\\lib\\zlib_x.lib")
//pragma comment(lib, "C:\\IndieLibX\\lib\\jpeg_x.lib")
//pragma comment(lib, "C:\\IndieLibX\\lib\\IL_x.lib")
//pragma comment(lib, "C:\\IndieLibX\\lib\\ILU_x.lib")
#endif

#include  <stddef.h>

// This is from Win32's <windef.h>
#if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__) || defined(__LCC__)
	#define STXCPPAPIENTRY __cdecl
	#define STXCPP_PACKSTRUCT
//#elif defined(linux) || defined(MACOSX) || defined(__CYGWIN__) //fix bug 840364
#elif defined( __GNUC__ )
  // this should work for any of the above commented platforms
  // plus any platform using GCC
	#ifdef __MINGW32__
		#define STXCPPAPIENTRY __cdecl
	#else
		#define STXCPPAPIENTRY
	#endif
	#define STXCPP_PACKSTRUCT __attribute__ ((packed))
#else
	#define STXCPPAPIENTRY
	#define STXCPP_PACKSTRUCT
#endif

// This is from Win32's <wingdi.h> and <winnt.h>
#if defined(__LCC__)
	#define STXAPI __stdcall
#elif defined(_MSC_VER) //changed 20031221 to fix bug 840421
	#ifdef STXCPP_BUILD_STATIC_LIB
		#define STXAPI
	#else
		#ifdef BUILD_SHARED
		#ifdef _STXCPP_BUILD_LIBRARY
			#define STXAPI __declspec(dllexport)
		#else
			#define STXAPI __declspec(dllimport)
		#endif
		#else
			#define STXAPI
		#endif
	#endif
#elif __APPLE__
	#define STXAPI
	//extern
#else
	#define STXAPI
#endif

//for SWIG: ???
#ifdef SWIG
#define STXAPI
#define STXCPPAPIENTRY
#endif

#ifdef __cplusplus
STXAPI void STXCPPAPIENTRY ililuInit(bool flip=false);
#endif

#include  "version.h"
#include  <STX/STX.h>

# if !defined( __uintptr_t_defined) && defined(_WIN32)
// /usr/include/stdint.h
#ifdef __STDC99__
#include <stdint.h>
#elif defined (_WIN64)
typedef unsigned __int64 uintptr_t;
#elif defined (_WIN32)
typedef unsigned int uintptr_t;
#else
#if 1//SDL
typedef unsigned int uintptr_t;
#else//stdint
typedef unsigned int uintptr_t;
#endif
#endif
# define __uintptr_t_defined
#endif

#if !defined(_WIN32)&&!defined(SDL_2_0_5_BUILD)&& !defined(WIN_PHONE_APP) && !defined(ANDROID) && !defined(__APPLE__)
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

#endif

//typedef int LONG;
//typedef unsigned int ULONG;
#if !defined(_MSC_VER) && !defined(__MINGW32__) && !defined(__MINGW64__)
typedef int SIZE_T;//???
#endif

#include <stdlib.h>

	/*
Memory Leaks
Docs are confusing Indexed entries talk about MFC No clear description Go to
"Visual C++ Documentation/Using Visual C++/Visual C++ Programmers Guide/Debugging/Debugging
Techniques.Problems and Solutions/Solving Buffer Overwrites and Memory Leaks"
(6.0) Include order is important Some things redefine malloc and free, etc.
Step 1, include in global header file
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
Step 2, enable checks in WinMain:
// Enables tracking and reporting on shutdown.
_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
_CrtSetReportMode ( _CRT_ERROR, _CRTDBG_MODE_DEBUG);
It may list file/line number Will list leak number set
{,,msvcrtd.dll}_crtBreakAlloc = n (leak number) Or, look for ASCII clues.
Memory Corruption
Heap Corruption Enable heap checking (slow) {,,msvcrtd.dll}_crtDbgFlag = 5 Data BPs may be useful
*/
/*
Where are std::std::min() and std::std::max()?
The Standard Library defines the two template functions std::std::min() and std::std::max()
in the <algorithm> header. In general, you should use these template functions for
calculating the std::min and std::max values of a pair.
Unfortunately, Visual C++ does not define these function templates.
This is because the names std::min and std::max clash with the traditional std::min and std::max macros defined in
<windows>. As a workaround, Visual C++ defines two alternative templates with identical
functionality called _cpp_min() and _cpp_max().
You can use them instead of std::std::min() and std::std::max().
To disable the generation of the std::min and std::max macros in Visual C++,
#define NOMINMAX before #including <windows>.
Danny Kalev
*/
#if 1//ndef SWIG
//std::std::min std::std::max:
#undef min
#undef max
#if 1
#if 1//ndef ANDROID

#undef min
#undef max
#ifdef __cplusplus
#ifndef SWIG
#include <algorithm>
#endif
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <queue>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#ifndef __APPLE__
#include <regex>
#include <tuple>
#endif
#include <iterator>
#include <functional> 
#include <cctype>
#include <locale>
//ostream:
#include <cstdlib>
using namespace std;
#endif

#endif
#endif

#include <float.h>
#if defined(_WIN32) || defined(__MINGW32__)
#include <direct.h>
//else
//include <sys/dir.h>
#endif
#endif
#define SAFE_RELEASE(p) 	{ if(p) { (p)->Release(); (p) = 0; } }
#define SAFE_DELETE(p)		{ if(p) { delete(p); (p) = 0; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[](p); (p) = 0; } }
#define SAFE_FREE(p)		{ if(p) { free(p); (p) = 0; } }
#define FAST_DELETE(p)		{ delete(p); }

//include <Mmsystem.h>
#if _MSC_VER >= 1400
// To make MSVC 2005 happy
#pragma warning (disable: 4996)
#  define assume(x) __assume(x)
#  define no_alias __declspec(noalias)
#else
#  define assume(x)
#  define no_alias
#endif
// Include some standard files
#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
//  define _WIN32_WINNT 0x0500
#if !defined(_WIN32_WINNT) && !defined(_CHICAGO_)
#define  _WIN32_WINNT   0x0600
#endif
#  ifndef WINVER
#     define WINVER 0x0500
#  endif
#elif !defined(_WIN32)&&!defined(SDL_2_0_5_BUILD)&& !defined(WIN_PHONE_APP) && !defined(ANDROID) && !defined(__APPLE__)
#  include <X11/keysym.h>
#  include <stdio.h>
#elif defined(__APPLE__)
#endif
//#include <limits.h>
#include <float.h>
#define FOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))   /**< creates a FOURCC code */
// Define some sized types
typedef unsigned char uint8;
typedef   signed char  int8;
typedef unsigned short uint16;
typedef   signed short  int16;
typedef unsigned int uint32;
typedef   signed int  int32;
#include <stddef.h>
typedef ptrdiff_t intptr;
#ifdef _WIN32
	typedef   signed __int64  int64;
	typedef unsigned __int64 uint64;
#else
	typedef   signed long long  int64;
	typedef unsigned long long uint64;
#endif
typedef unsigned char ubyte;
typedef unsigned short ushort;
typedef unsigned int uint;
#ifdef _DEBUG
#  ifndef DEBUG
#  define DEBUG
#endif
//???endif
// Define some useful macros
#define MCHAR2(a, b) (a | (b << 8))
#define MCHAR4(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))
#define _STR_(x) #x
#define DEFINE_STR(x) "#define " #x " " _STR_(x) "\n"
#define elementsOf(x) (sizeof(x) / sizeof(x[0]))
#define offsetOf(strct, member) uint(((strct *) 0)->member)
#ifdef _WIN32
#define forceinline __forceinline
#define alignment(x) __declspec(align(x))
#else
#define forceinline inline
#define alignment(x) __attribute__ ((aligned(x)))
#endif

void initTime();
float getCurrentTime();
#ifdef _WIN32
#ifdef _WIN32
// Ensure proper handling of for-scope
#  if (_MSC_VER <= 1200)
#    define for if(0); else for
#  else
#    pragma conform(forScope, on)
#    pragma warning(disable: 4258)
#  endif
#endif
#else
#define stricmp(a, b) strcasecmp(a, b)
#endif // LINUX
#define ASSERT(b) assert(b)

/*
 * Copyright (C) 2007 David Adam
 * Copyright (C) 2007 Tony Wasserka
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */




#ifndef SWIG
//#pragma warning( disable : 4251)
#define NOMINMAX //before #including <windows>.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#define _CRTDBG_MAP_ALLOC

//define MAX_BUF_SIZE 65536

#ifdef _WIN32
#include <crtdbg.h>
#endif

//include  <STX/stx_string.h>

#endif

#if defined(ANDROID) || defined(WIN_PHONE_APP) || defined(WIN8_APP)
#define CHECK_HEAP
#elif defined(_WIN32)
void heapdump( void );
void CheckHeap(const char *pszFormat, ...);
class STXAPI Heap
{
    private:
	unsigned int getCombinedSizesOfProcessHeaps(void);
    public:
	Heap();
	virtual ~Heap();
	bool ValidateAllHeaps(void);
	bool ValidateHeap(HANDLE heap);
	unsigned int GetCombinedSizesOfProcessHeaps(void);
	unsigned int GetHeapSize(HANDLE heap);
};
#define CHECK_HEAP { \
    Heap heap; \
    LOG_PRINT("CHECK_HEAP %s:%d\n",__FUNCTION__,__LINE__); \
    if(!heap.ValidateAllHeaps()) \
    { \
        LOG_PRINT("heap corrupted:%s:%d\n",__FUNCTION__,__LINE__); \
        DBG_HALT; \
    } \
    }
//elif !defined(SDL_2_0_5_BUILD)
#elif 0
#include <stdlib.h>
#include <stdio.h>
#ifndef __APPLE__
#include <malloc.h>
#endif
#define CHECK_HEAP { \
int rc=0; \
    if (_HEAPOK != (rc = _heapchk())) { \
       switch(rc) { \
          case _HEAPEMPTY: \
             LOG_PRINT("The heap has not been initialized."); \
             break; \
          case _HEAPBADNODE: \
             LOG_PRINT("A memory node is corrupted or the heap is damaged."); \
             break; \
          case _HEAPBADBEGIN: \
             LOG_PRINT("The heap specified is not valid."); \
             break; \
       } \
        DBG_HALT; \
    } \
}
#endif

#if defined(_WIN32)
#define STX_STACK_TRACE
#define LOG_FNLN7 LOG_PRINT("%s:%d\n",__FILE__,__LINE__);CHECK_HEAP;
#elif !defined(ANDROID)
#define CHECK_HEAP
#define CHECK_HEAP0

/*
#elif 0
#define CHECK_HEAP
#define CHECK_HEAP0
#define STX_STACK_TRACE {}
*/
#endif
#endif
#ifdef __cplusplus
#include  "STXinterfaces.h"
#endif

// __D3DCOLOR__ is equivalent to D3DFMT_A8R8G8B8
#ifdef _WIN32
#include <d3d9types.h>
#else
#ifndef D3DCOLOR_DEFINED
typedef __DWORD__ __D3DCOLOR__;
#define D3DCOLOR_DEFINED
#endif

// maps unsigned 8 bits/channel to __D3DCOLOR__
#define D3DCOLOR_ARGB(a,r,g,b) \
    ((__D3DCOLOR__)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define D3DCOLOR_RGBA(r,g,b,a) D3DCOLOR_ARGB(a,r,g,b)
#define D3DCOLOR_XRGB(r,g,b)   D3DCOLOR_ARGB(0xff,r,g,b)

#define D3DCOLOR_XYUV(y,u,v)   D3DCOLOR_ARGB(0xff,y,u,v)
#define D3DCOLOR_AYUV(a,y,u,v) D3DCOLOR_ARGB(a,y,u,v)

// maps floating point channels (0.f to 1.f range) to __D3DCOLOR__
#define D3DCOLOR_COLORVALUE(r,g,b,a) \
    D3DCOLOR_RGBA((__DWORD__)((r)*255.f),(__DWORD__)((g)*255.f),(__DWORD__)((b)*255.f),(__DWORD__)((a)*255.f))


#ifndef D3DVECTOR_DEFINED
typedef struct STXAPI _D3DVECTOR {
    float x;
    float y;
    float z;
} D3DVECTOR;
#define D3DVECTOR_DEFINED
#endif

#ifndef D3DCOLORVALUE_DEFINED
typedef struct STXAPI _D3DCOLORVALUE {
    float r;
    float g;
    float b;
    float a;
} D3DCOLORVALUE;
#define D3DCOLORVALUE_DEFINED
#endif

typedef enum STXAPI _D3DLIGHTTYPE {
    D3DLIGHT_POINT          = 1,
    D3DLIGHT_SPOT           = 2,
    D3DLIGHT_DIRECTIONAL    = 3,
    D3DLIGHT_FORCE_DWORD    = 0x7fffffff, /* force 32-bit size enum STXAPI */
} D3DLIGHTTYPE;

typedef struct STXAPI _D3DLIGHT9 {
    D3DLIGHTTYPE    Type;            /* Type of light source */
    D3DCOLORVALUE   Diffuse;         /* Diffuse color of light */
    D3DCOLORVALUE   Specular;        /* Specular color of light */
    D3DCOLORVALUE   Ambient;         /* Ambient color of light */
    D3DVECTOR       Position;         /* Position in world space */
    D3DVECTOR       Direction;        /* Direction in world space */
    float           Range;            /* Cutoff range */
    float           Falloff;          /* Falloff */
    float           Attenuation0;     /* Constant attenuation */
    float           Attenuation1;     /* Linear attenuation */
    float           Attenuation2;     /* Quadratic attenuation */
    float           Theta;            /* Inner angle of spotlight cone */
    float           Phi;              /* Outer angle of spotlight cone */
} D3DLIGHT9;
#ifndef _WIN32
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((__DWORD__)(BYTE)(ch0) | ((__DWORD__)(BYTE)(ch1) << 8) |   \
                ((__DWORD__)(BYTE)(ch2) << 16) | ((__DWORD__)(BYTE)(ch3) << 24 ))
#endif
typedef enum STXAPI _D3DFORMAT
{
    D3DFMT_UNKNOWN              =  0,

    D3DFMT_R8G8B8               = 20,
    D3DFMT_A8R8G8B8             = 21,
    D3DFMT_X8R8G8B8             = 22,
    D3DFMT_R5G6B5               = 23,
    D3DFMT_X1R5G5B5             = 24,
    D3DFMT_A1R5G5B5             = 25,
    D3DFMT_A4R4G4B4             = 26,
    D3DFMT_R3G3B2               = 27,
    D3DFMT_A8                   = 28,
    D3DFMT_A8R3G3B2             = 29,
    D3DFMT_X4R4G4B4             = 30,
    D3DFMT_A2B10G10R10          = 31,
    D3DFMT_A8B8G8R8             = 32,
    D3DFMT_X8B8G8R8             = 33,
    D3DFMT_G16R16               = 34,
    D3DFMT_A2R10G10B10          = 35,
    D3DFMT_A16B16G16R16         = 36,

    D3DFMT_A8P8                 = 40,
    D3DFMT_P8                   = 41,

    D3DFMT_L8                   = 50,
    D3DFMT_A8L8                 = 51,
    D3DFMT_A4L4                 = 52,

    D3DFMT_V8U8                 = 60,
    D3DFMT_L6V5U5               = 61,
    D3DFMT_X8L8V8U8             = 62,
    D3DFMT_Q8W8V8U8             = 63,
    D3DFMT_V16U16               = 64,
    D3DFMT_A2W10V10U10          = 67,

    D3DFMT_UYVY                 = MAKEFOURCC('U', 'Y', 'V', 'Y'),
    D3DFMT_R8G8_B8G8            = MAKEFOURCC('R', 'G', 'B', 'G'),
    D3DFMT_YUY2                 = MAKEFOURCC('Y', 'U', 'Y', '2'),
    D3DFMT_G8R8_G8B8            = MAKEFOURCC('G', 'R', 'G', 'B'),
    D3DFMT_DXT1                 = MAKEFOURCC('D', 'X', 'T', '1'),
    D3DFMT_DXT2                 = MAKEFOURCC('D', 'X', 'T', '2'),
    D3DFMT_DXT3                 = MAKEFOURCC('D', 'X', 'T', '3'),
    D3DFMT_DXT4                 = MAKEFOURCC('D', 'X', 'T', '4'),
    D3DFMT_DXT5                 = MAKEFOURCC('D', 'X', 'T', '5'),

    D3DFMT_D16_LOCKABLE         = 70,
    D3DFMT_D32                  = 71,
    D3DFMT_D15S1                = 73,
    D3DFMT_D24S8                = 75,
    D3DFMT_D24X8                = 77,
    D3DFMT_D24X4S4              = 79,
    D3DFMT_D16                  = 80,

    D3DFMT_D32F_LOCKABLE        = 82,
    D3DFMT_D24FS8               = 83,

/* D3D9Ex only -- */
#if !defined(D3D_DISABLE_9EX)

    /* Z-Stencil formats valid for CPU access */
    D3DFMT_D32_LOCKABLE         = 84,
    D3DFMT_S8_LOCKABLE          = 85,

#endif // !D3D_DISABLE_9EX
/* -- D3D9Ex only */


    D3DFMT_L16                  = 81,

    D3DFMT_VERTEXDATA           =100,
    D3DFMT_INDEX16              =101,
    D3DFMT_INDEX32              =102,

    D3DFMT_Q16W16V16U16         =110,

    D3DFMT_MULTI2_ARGB8         = MAKEFOURCC('M','E','T','1'),

    // Floating point surface formats

    // s10e5 formats (16-bits per channel)
    D3DFMT_R16F                 = 111,
    D3DFMT_G16R16F              = 112,
    D3DFMT_A16B16G16R16F        = 113,

    // IEEE s23e8 formats (32-bits per channel)
    D3DFMT_R32F                 = 114,
    D3DFMT_G32R32F              = 115,
    D3DFMT_A32B32G32R32F        = 116,

    D3DFMT_CxV8U8               = 117,

/* D3D9Ex only -- */
#if !defined(D3D_DISABLE_9EX)

    // Monochrome 1 bit per pixel format
    D3DFMT_A1                   = 118,

    // 2.8 biased fixed point
    D3DFMT_A2B10G10R10_XR_BIAS  = 119,


    // Binary format indicating that the data has no inherent type
    D3DFMT_BINARYBUFFER         = 199,

#endif // !D3D_DISABLE_9EX
/* -- D3D9Ex only */


    D3DFMT_FORCE_DWORD          =0x7fffffff
} D3DFORMAT;
#define D3DFVF_TEXTUREFORMAT2 0	       // Two floating point values
#define D3DFVF_TEXTUREFORMAT1 3	       // One floating point value
#define D3DFVF_TEXTUREFORMAT3 1	       // Three floating point values
#define D3DFVF_TEXTUREFORMAT4 2	       // Four floating point values
#define D3DFVF_TEXCOORDSIZE3(CoordIndex) (D3DFVF_TEXTUREFORMAT3 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE2(CoordIndex) (D3DFVF_TEXTUREFORMAT2)
#define D3DFVF_TEXCOORDSIZE4(CoordIndex) (D3DFVF_TEXTUREFORMAT4 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE1(CoordIndex) (D3DFVF_TEXTUREFORMAT1 << (CoordIndex*2 + 16))

//-------------------------------------------------------------------

// Flexible vertex format bits
//
#define D3DFVF_RESERVED0        0x001
#define D3DFVF_POSITION_MASK    0x400E
#define D3DFVF_XYZ              0x002
#define D3DFVF_XYZRHW           0x004
#define D3DFVF_XYZB1            0x006
#define D3DFVF_XYZB2            0x008
#define D3DFVF_XYZB3            0x00a
#define D3DFVF_XYZB4            0x00c


#define D3DFVF_XYZB5            0x00e
#define D3DFVF_XYZW             0x4002

#define D3DFVF_NORMAL           0x010
#define D3DFVF_PSIZE            0x020
#define D3DFVF_DIFFUSE          0x040
#define D3DFVF_SPECULAR         0x080

#define D3DFVF_TEXCOUNT_MASK    0xf00
#define D3DFVF_TEXCOUNT_SHIFT   8
#define D3DFVF_TEX0             0x000
#define D3DFVF_TEX1             0x100
#define D3DFVF_TEX2             0x200
#define D3DFVF_TEX3             0x300
#define D3DFVF_TEX4             0x400
#define D3DFVF_TEX5             0x500
#define D3DFVF_TEX6             0x600
#define D3DFVF_TEX7             0x700
#define D3DFVF_TEX8             0x800

#define D3DFVF_LASTBETA_UBYTE4   0x1000
#define D3DFVF_LASTBETA_D3DCOLOR 0x8000

#define D3DFVF_RESERVED2         0x6000  // 2 reserved bits

//---------------------------------------------------------------------
// Vertex Shaders
//

// Vertex shader declaration

// Vertex element semantics
//
typedef enum STXAPI _D3DDECLUSAGE
{
    D3DDECLUSAGE_POSITION = 0,
    D3DDECLUSAGE_BLENDWEIGHT,   // 1
    D3DDECLUSAGE_BLENDINDICES,  // 2
    D3DDECLUSAGE_NORMAL,        // 3
    D3DDECLUSAGE_PSIZE,         // 4
    D3DDECLUSAGE_TEXCOORD,      // 5
    D3DDECLUSAGE_TANGENT,       // 6
    D3DDECLUSAGE_BINORMAL,      // 7
    D3DDECLUSAGE_TESSFACTOR,    // 8
    D3DDECLUSAGE_POSITIONT,     // 9
    D3DDECLUSAGE_COLOR,         // 10
    D3DDECLUSAGE_FOG,           // 11
    D3DDECLUSAGE_DEPTH,         // 12
    D3DDECLUSAGE_SAMPLE,        // 13
} D3DDECLUSAGE;

#define MAXD3DDECLUSAGE         D3DDECLUSAGE_SAMPLE
#define MAXD3DDECLUSAGEINDEX    15
#define MAXD3DDECLLENGTH        64 // does not include "end" marker vertex element

typedef enum STXAPI _D3DDECLMETHOD
{
    D3DDECLMETHOD_DEFAULT = 0,
    D3DDECLMETHOD_PARTIALU,
    D3DDECLMETHOD_PARTIALV,
    D3DDECLMETHOD_CROSSUV,    // Normal
    D3DDECLMETHOD_UV,
    D3DDECLMETHOD_LOOKUP,               // Lookup a displacement map
    D3DDECLMETHOD_LOOKUPPRESAMPLED,     // Lookup a pre-sampled displacement map
} D3DDECLMETHOD;

#define MAXD3DDECLMETHOD D3DDECLMETHOD_LOOKUPPRESAMPLED

// Declarations for _Type fields
//
typedef enum STXAPI _D3DDECLTYPE
{
    D3DDECLTYPE_FLOAT1    =  0,  // 1D float expanded to (value, 0., 0., 1.)
    D3DDECLTYPE_FLOAT2    =  1,  // 2D float expanded to (value, value, 0., 1.)
    D3DDECLTYPE_FLOAT3    =  2,  // 3D float expanded to (value, value, value, 1.)
    D3DDECLTYPE_FLOAT4    =  3,  // 4D float
    D3DDECLTYPE_D3DCOLOR  =  4,  // 4D packed unsigned bytes mapped to 0. to 1. range
                                 // Input is in __D3DCOLOR__ format (ARGB) expanded to (R, G, B, A)
    D3DDECLTYPE_UBYTE4    =  5,  // 4D unsigned byte
    D3DDECLTYPE_SHORT2    =  6,  // 2D signed short expanded to (value, value, 0., 1.)
    D3DDECLTYPE_SHORT4    =  7,  // 4D signed short

// The following types are valid only with vertex shaders >= 2.0


    D3DDECLTYPE_UBYTE4N   =  8,  // Each of 4 bytes is normalized by dividing to 255.0
    D3DDECLTYPE_SHORT2N   =  9,  // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
    D3DDECLTYPE_SHORT4N   = 10,  // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
    D3DDECLTYPE_USHORT2N  = 11,  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
    D3DDECLTYPE_USHORT4N  = 12,  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
    D3DDECLTYPE_UDEC3     = 13,  // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
    D3DDECLTYPE_DEC3N     = 14,  // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
    D3DDECLTYPE_FLOAT16_2 = 15,  // Two 16-bit floating point values, expanded to (value, value, 0, 1)
    D3DDECLTYPE_FLOAT16_4 = 16,  // Four 16-bit floating point values
    D3DDECLTYPE_UNUSED    = 17,  // When the type field in a decl is unused.
} D3DDECLTYPE;

#define MAXD3DDECLTYPE      D3DDECLTYPE_UNUSED

typedef struct STXAPI _D3DVERTEXELEMENT9
{
    __WORD__    Stream;     // Stream index
    __WORD__    Offset;     // Offset in the stream in bytes
    BYTE    Type;       // Data type
    BYTE    Method;     // Processing method
    BYTE    Usage;      // Semantics
    BYTE    UsageIndex; // Semantic index
} D3DVERTEXELEMENT9, *LPD3DVERTEXELEMENT9;

// This is used to initialize the last vertex element in a vertex declaration
// array
//
#define D3DDECL_END() {0xFF,0,D3DDECLTYPE_UNUSED,0,0,0}

#endif

#if defined(__GNUC__) && !defined(__MINGW32__)
#include <stdarg.h>
#endif

#define __T(x)      x
#define _T(x)       __T(x)
#define _TEXT(x)    __T(x)

typedef enum
{
    INVALID_PATH = 0,
    UNC_PATH,              /* "//foo" */
    ABSOLUTE_DRIVE_PATH,   /* "c:/foo" */
    RELATIVE_DRIVE_PATH,   /* "c:foo" */
    ABSOLUTE_PATH,         /* "/foo" */
    RELATIVE_PATH,         /* "foo" */
    DEVICE_PATH,           /* "//./foo" */
    UNC_DOT_PATH           /* "//." */
} DOS_PATHNAME_TYPE;

#ifndef _WIN32
#define INFINITE stxINF
//define IMPLEMENT
//define DBG_HALT
#define INT_PTR int
#define D3DSHADER_VERSION_MINOR
#define D3DSHADER_VERSION_MAJOR
struct STXAPI RECT
{
	int left;
	int top;
	int right;
	int bottom;

};
#ifndef _MSC_VER
#ifdef __cplusplus
inline __DWORD__ STXAPI timeGetTime()
{
	return STX_Service::GetTimerInstance()->stx_GetTicks(0);
}
#endif
#endif
#endif

#if 1//ndef SWIG
#ifndef SWIG
STXAPI void STXAPIENTRY DebugNew(const char *pszFormat, ...);
STXAPI void STXAPIENTRY DebugNewDump();
#define STXNEW(a) new ##(a); DebugNew("new %s:%s:%s:%d\n","(a)",__FILE__,__FUNCTION__,__LINE__);
//define STXNEW LOG_PRINT("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__); new
#define STXDELETE(a) LOG_PRINT("delete %s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__); delete ## a
#define STXDELETEARRAY(a) LOG_PRINT("delete[] %s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__); delete[] ## a

#ifdef __cplusplus
template<class T>
struct VArray:public std::vector<T>
{
	VArray():std::vector<T>(){}
	virtual ~VArray()
	{
//		for(unsigned int i=0;i<size();i++)
//			delete	at(i);
	}
#ifdef _WIN32
	size_t getCount() const{return size();}
	size_t add(T t){push_back(t);return size()-1;}
#else
	unsigned int getCount() const{return std::vector<T>::size();}
#if 1//defined(ANDROID) || defined(__APPLE__)
	unsigned int add(T t){std::vector<T>::push_back(t);return std::vector<T>::size()-1;}
#else
	unsigned int add(T t){push_back(t);return std::vector<T>::size()-1;}
#endif
#endif
	void RemoveAll()
	{
		std::vector<T>::clear();
	}
	void Add(T i)
	{
		push_back(i);
	}
	T Remove(int i)
	{
		if(i>=std::vector<T>::size()) DBG_HALT;
		T t;
		if(i<std::vector<T>::size())
		{
			t=std::vector<T>::at(i);
			this->erase (std::vector<T>::begin()+i);
		}
		return t;
	}

    T orderedRemove(int i)
	{
		return Remove(i);
	}

	T GetAt(UINT i) const
	{
		if(i>=std::vector<T>::size()) DBG_HALT;
		return std::vector<T>::at(i);
	}
	T* getArray() const
	{
		return const_cast<T*>(&std::vector<T>::at(0));
	}
        size_t GetSize() const
	{
		return std::vector<T>::size();
	}
};
#endif
#endif
#endif

#endif // _PLATFORM_H_

