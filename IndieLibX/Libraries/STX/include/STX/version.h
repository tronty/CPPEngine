/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __MSVERION__
#define __MSVERION__
#if !defined(SWIG) && !defined(WIN8_APP) && !defined(WIN_PHONE_APP)

#ifndef NTDDI_WIN7
#define NTDDI_WIN7 0x06010000
#endif

#ifndef NTDDI_WS08
#define NTDDI_WS08 0x06000100
#endif

#ifndef NTDDI_VISTASP1
#define NTDDI_VISTASP1 0x06000100
#endif

#ifndef NTDDI_VISTA
#define NTDDI_VISTA 0x06000000
#endif

#ifndef NTDDI_WS03SP2
#define NTDDI_WS03SP2 0x05020200
#endif

#ifndef NTDDI_WS03SP1
#define NTDDI_WS03SP1 0x05020100
#endif

#ifndef NTDDI_WS03
#define NTDDI_WS03 0x05020000
#endif

#ifndef NTDDI_WINXPSP3
#define NTDDI_WINXPSP3 0x05010300
#endif

#ifndef NTDDI_WINXPSP2
#define NTDDI_WINXPSP2 0x05010200
#endif

#ifndef NTDDI_WINXPSP1
#define NTDDI_WINXPSP1 0x05010100
#endif

#ifndef NTDDI_WINXP
#define NTDDI_WINXP 0x05010000
#endif

#ifndef NTDDI_WIN2KSP4
#define NTDDI_WIN2KSP4 0x05000400
#endif

#ifndef NTDDI_WIN2KSP3
#define NTDDI_WIN2KSP3 0x05000300
#endif

#ifndef NTDDI_WIN2KSP2
#define NTDDI_WIN2KSP2 0x05000200
#endif

#ifndef NTDDI_WIN2KSP1
#define NTDDI_WIN2KSP1 0x05000100
#endif

#ifndef NTDDI_WIN2K
#define NTDDI_WIN2K 0x05000000
#endif



#ifndef _WIN32_WINNT_WIN7
#define _WIN32_WINNT_WIN7 0x0601
#endif

#ifndef _WIN32_WINNT_WS08
#define _WIN32_WINNT_WS08 0x0600
#endif

#ifndef _WIN32_WINNT_VISTA
#define _WIN32_WINNT_VISTA 0x0600
#endif

#ifndef _WIN32_WINNT_WS03
#define _WIN32_WINNT_WS03 0x0502
#endif

#ifndef _WIN32_WINNT_WINXP
#define _WIN32_WINNT_WINXP 0x0501
#endif

#ifndef _WIN32_WINNT_WIN2K
#define _WIN32_WINNT_WIN2K 0x0500
#endif




#ifndef _WIN32_IE_IE80
#define _WIN32_IE_IE80 0x0800
#endif

#ifndef _WIN32_IE_IE70
#define _WIN32_IE_IE70 0x0700
#endif

#ifndef _WIN32_IE_IE60SP2
#define _WIN32_IE_IE60SP2 0x0603
#endif

#ifndef _WIN32_IE_IE60SP1
#define _WIN32_IE_IE60SP1 0x0601
#endif

#ifndef _WIN32_IE_IE60
#define _WIN32_IE_IE60 0x0600
#endif

#ifndef _WIN32_IE_IE55
#define _WIN32_IE_IE55 0x0550
#endif

#ifndef _WIN32_IE_IE501
#define _WIN32_IE_IE501 0x0501
#endif

#ifndef _WIN32_IE_IE50
#define _WIN32_IE_IE50 0x0500
#endif

#if 0
#define NTDDI_VERSION1 NTDDI_WINXP
#undef VISTA_AND_WINDOWS7
#else
#define NTDDI_VERSION1 NTDDI_VISTA
#define VISTA_AND_WINDOWS7 NTDDI_VERSION1
#endif
#ifndef WINVER
#define WINVER NTDDI_VERSION1
#define _WIN32_WINNT NTDDI_VERSION1
#endif
#endif
#if 1//defined( VISTA_AND_WINDOWS7)//||(D3DXFROMWINE_SDK_VERSION >= 42)

#if 0
#if 0
//define fl D3D10_FEATURE_LEVEL_9_3
#define  vs "vs_4_0"
#define  ps "ps_4_0"
#elif 1
#define USE_D3D10_1 1
#if 1//works with Quad10.hlsl
#define fl D3D10_FEATURE_LEVEL_10_1
#define  vs "vs_4_0"
#define  ps "ps_4_0"
#else
#define fl D3D10_FEATURE_LEVEL_9_3
#define  vs "vs_4_0_level_9_3"
#define  ps "ps_4_0_level_9_3"
#endif
#else
#define USE_D3D11 1
/*
Thanks for your response.
I think that the D3D11 runtime expects to receive shaders that are compiled with at least vs_4_0 target,
although the compiler can output shaders to any known D3D target. Unfortunately,
it looks like the runtime interprets the vs_2_0 blob as corrupt rather than seeing that it's simply compiled
with an inappropriate target.
Depending on which feature level you're targeting, you have a choice of shader targets:
vs_4_0, vs_4_0_level_9_1, vs_4_0_level_9_3, vs_5_0
*/
#if 1//works with Quad9.hlsl
#define fl D3D_FEATURE_LEVEL_9_3
#define  vs "vs_4_0_level_9_3"
#define  ps "ps_4_0_level_9_3"
#else//does not work
#define fl D3D_FEATURE_LEVEL_10_1
#define  vs "vs_4_0"
#define  ps "ps_4_0"
#endif
#endif
#endif
#endif

#if 0
#define fl11 D3D_FEATURE_LEVEL_9_3
#define  vs11 "vs_4_0_level_9_3"
#define  ps11 "ps_4_0_level_9_3"
#define  gs11 "gs_4_0"
#define  cs11 "cs_4_0"
#define  hs11 "hs_4_0"
#define  ds11 "ds_4_0"
#elif 0
#define fl11 D3D_FEATURE_LEVEL_10_1
#define  vs11 "vs_4_0"
#define  ps11 "ps_4_0"
#define  gs11 "gs_4_0"
#define  cs11 "cs_4_0"
#define  hs11 "hs_4_0"
#define  ds11 "ds_4_0"
#elif 1
#define fl11 D3D_FEATURE_LEVEL_11_0
#define  vs11 "vs_5_0"
#define  ps11 "ps_5_0"
#define  gs11 "gs_5_0"
#define  cs11 "cs_5_0"
#define  hs11 "hs_5_0"
#define  ds11 "ds_5_0"
//          D3D_FEATURE_LEVEL_11_0
//          D3D_FEATURE_LEVEL_10_1
//          D3D_FEATURE_LEVEL_10_0
//          D3D_FEATURE_LEVEL_9_3
//          D3D_FEATURE_LEVEL_9_2
//          D3D_FEATURE_LEVEL_9_1
#endif

#define USE_D3D10_1 1
#define fl10 D3D10_FEATURE_LEVEL_10_1
#define  vs10 "vs_4_0"
#define  ps10 "ps_4_0"
#define  gs10 "gs_4_0"
#define  cs10 "cs_4_0"

#if defined(_MSC_VER)
#define MEDIA_PATH "C:\\IndieLib_resources\\"
#define MEDIA_PATH_ "\\"
#elif defined(LINUx) || defined(__APPLE__)
#define MEDIA_PATH "../../IndieLib_resources/"
#define MEDIA_PATH_ "/"
#else
#define MEDIA_PATH "../../IndieLib_resources/"
#define MEDIA_PATH_ "/"
#endif
#if 1
#define TEX_PATH   MEDIA_PATH"Textures" MEDIA_PATH_
#define MODEL_PATH MEDIA_PATH"Models" MEDIA_PATH_
#define FONT_PATH  MEDIA_PATH"Fonts" MEDIA_PATH_
#define MUSIC_PATH MEDIA_PATH"Music" MEDIA_PATH_
#define MAP_PATH   MEDIA_PATH"Maps" MEDIA_PATH_
#endif
#endif

