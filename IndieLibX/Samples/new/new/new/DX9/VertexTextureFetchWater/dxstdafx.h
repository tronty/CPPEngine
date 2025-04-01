/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//--------------------------------------------------------------------------------------
// File: DxStdAfx.h
//
// Desc: Standard includes and precompiled headers for DXUT
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef DXSDK_STDAFX_H
#define DXSDK_STDAFX_H

#ifndef STRICT
#define STRICT
#endif

// If app hasn't choosen, set to work with Windows 98, Windows Me, Windows 2000, Windows XP and beyond
#ifndef WINVER
#define WINVER         0x0410
#endif
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410 
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0500 
#endif

#pragma warning( disable : 4100 ) // disable unreference formal parameter warnings for /W4 builds

#include <windows.h>
#include <assert.h>
#include <wchar.h>
#include <mmsystem.h>
#include <commctrl.h> // for InitCommonControls() 
#include <shellapi.h> // for ExtractIcon()
#include <new.h>      // for placement new
#include <math.h>      
#include <limits.h>      
#include <stdio.h>
#include <multimon.h> 

// CRT's memory leak detection
#if defined(DEBUG) | defined(_DEBUG)
#include <crtdbg.h>
#endif


// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
#if defined(DEBUG) | defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

// Direct3D includes
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>

// DirectSound includes
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>

#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>

#include <DXUT/DXUT.h>
#include <DXUT/DXUTmisc.h>
#include <DXUT/DXUTenum.h>
#include <DXUT/DXUTmesh.h>
#include <DXUT/DXUTgui.h>
#include <DXUT/DXUTsettingsDlg.h>
#include <DXUT/DXUTSound.h>

#if defined(DEBUG) | defined(_DEBUG)
#ifndef V
#define (x)           { hr = x; if( FAILED(hr) ) { DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
#endif
#ifndef V_RETURN
#define (x)    { hr = x; if( FAILED(hr) ) { return DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
#endif
#else
#ifndef V
#define (x)           { hr = x; }
#endif
#ifndef V_RETURN
#define (x)    { hr = x; if( FAILED(hr) ) { return hr; } }
#endif
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=0; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=0; } }
#endif    
#ifndef ////////////////SAFE_RELEASE
#define ////////////////SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=0; } }
#endif

#endif // !defined(DXSDK_STDAFX_H)
