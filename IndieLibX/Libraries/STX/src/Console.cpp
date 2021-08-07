/*
 * TAR::Engine - Free C++ 3D engine ;
 * Copyright (C) 2007 Tommi Roenty ;
 * This program is free software;
 * you can redistribute it and/or ;
 * modify it under the terms of the GNU General Public License ;
 * as published by the Free Software Foundation;
 * either version 2 ;
 * of the License, or (at your option) any later version. ;
 * This program is distributed in the hope that it will be useful, ;
 * but WITHOUT ANY WARRANTY;
 * without even the implied warranty of ;
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the ;
 * GNU General Public License for more details. ;
 * You should have received a copy of the GNU General Public License ;
 * along with this program;
 * if not, write to the Free Software ;
 * Foundation, Inc., ;
 * 59 Temple Place - Suite 330, ;
 * Boston, MA 02111-1307, USA. ;
 * E-mail : tommi.ronty@iki.fi ;
 */
#if defined(_WIN32)
#ifndef CONSOLE_H
#define CONSOLE_H
#if (!defined(__MINGW32__) && !defined(__MINGW64__))
#include <io.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#endif
#if !defined(__MINGW32__) && !defined(__MINGW64__)
#if defined(_WIN32)
#include <direct.h>
#else
#include <sys/dir.h>
#endif
#endif
using namespace std;
#define MAX_CONSOLE_LINES 500
#pragma warning( disable : 4311 )
class Console
{
	public:
		Console()
		{
			Init();
		}
		~Console()
		{
			DeInit();
		}
		static void Console::Init(void)
		{
#if defined(_WIN32) && (!defined(__MINGW32__) && !defined(__MINGW64__))
#if 0
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w+", stdout);
	freopen_s(&stream, "CONOUT$", "w+", stderr);
	SetConsoleTitle(TEXT(title.c_str()));
#else
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			int				hConHandle;
			int				lStdHandle;
			CONSOLE_SCREEN_BUFFER_INFO	coninfo;
			FILE				*fp;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			AllocConsole();
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
			coninfo.dwSize.Y = MAX_CONSOLE_LINES;
			SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
			lStdHandle = (int) GetStdHandle(STD_OUTPUT_HANDLE);
			hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
			fp = _fdopen(hConHandle, "w");
			*stdout = *fp;
			setvbuf(stdout, 0, _IONBF, 0);
			lStdHandle = (int) GetStdHandle(STD_INPUT_HANDLE);
			hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
			fp = _fdopen(hConHandle, "r");
			*stdin = *fp;
			setvbuf(stdin, 0, _IONBF, 0);
			lStdHandle = (int) GetStdHandle(STD_ERROR_HANDLE);
			hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
			fp = _fdopen(hConHandle, "w");
			*stderr = *fp;
			setvbuf(stderr, 0, _IONBF, 0);
			ios::sync_with_stdio();
#endif
#endif
		}
		static void Console::DeInit(void)
		{
#if defined(_WIN32) && (!defined(__MINGW32__) && !defined(__MINGW64__))
			/*~~~~~~~~~~~~~~~~~*/
			//char	cstring[256];
			/*~~~~~~~~~~~~~~~~~*/
			/*
			 * try { LuaObject sObj = SLua::State()->GetGlobal("gets");
			 * if(sObj.IsBoolean()) if(sObj.GetBoolean()) gets(cstring);
			 * } catch(LuaException ex) { LOG_PRINT("%s:%d:%s\n", __FUNCTION__, __LINE__, ex.GetErrorMessage());
			 */
			FreeConsole();
#endif
		}
};
#pragma warning( default : 4311 )
#endif
#endif

