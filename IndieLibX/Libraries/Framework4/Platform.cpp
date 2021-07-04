
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Platform.h"

// Utility functions

void ErrorMsg(const char *string)
{
	MessageBoxA(nullptr, string, "Error", MB_OK | MB_ICONERROR);
}

void WarningMsg(const char *string)
{
	MessageBoxA(nullptr, string, "Warning", MB_OK | MB_ICONWARNING);
}

void InfoMsg(const char *string)
{
	MessageBoxA(nullptr, string, "Information", MB_OK | MB_ICONINFORMATION);
}

#ifdef DEBUG

#include <stdio.h>

bool FailedAssert(char *file, int line, char *statement)
{
	static bool debug = true;

	if (debug)
	{
		char str[1024];

		sprintf_s(str, "Failed: (%s)\n\nFile: %s\nLine: %d\n\n", statement, file, line);

		if (IsDebuggerPresent())
		{
			strcat(str, "Debug?");
			int res = MessageBoxA(nullptr, str, "Assert failed", MB_YESNOCANCEL | MB_ICONERROR);
			if (res == IDYES)
			{
				return true;
			}
			if (res == IDNO)
			{
				return false;
			}
			else if (res == IDCANCEL)
			{
				debug = false;
			}
		}
		else
		{
			strcat(str, "Display more asserts?");
			if (MessageBoxA(nullptr, str, "Assert failed", MB_YESNO | MB_ICONERROR | MB_DEFBUTTON2) != IDYES)
			{
				debug = false;
			}
		}
	}

	return debug;
}

void DebugString(const char *fmt, ...)
{
	char str[256];

	va_list argp;
	va_start(argp, fmt);

	uint n = vsnprintf(str, sizeof(str) - 2, fmt, argp);

	va_end(argp);

	if (n > sizeof(str) - 2)
		n = sizeof(str) - 2;

	str[n] = '\n';
	str[n + 1] = '\0';

	OutputDebugStringA(str);
}

#endif // DEBUG
