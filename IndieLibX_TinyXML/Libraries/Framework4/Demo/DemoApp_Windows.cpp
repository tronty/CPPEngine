
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

#include "DemoApp.h"

#include "../Windows/Resource.h"
/*
#include "../CPU.h"
#include <direct.h>

#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")
*/

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include <stdio.h>


extern DemoApp *app; // Should come up with something prettier than this

#define GETX(l) (int(l & 0xFFFF))
#define GETY(l) (int(l) >> 16)

bool g_Active = true;

void InitializeInput(HWND hwnd)
{
	RAWINPUTDEVICE devices[] =
	{
		{ 1, 2, RIDEV_INPUTSINK, hwnd }, // Mouse
		{ 1, 6, RIDEV_INPUTSINK, hwnd }, // Keyboard
	};

	BOOL result = RegisterRawInputDevices(devices, elementsof(devices), sizeof(RAWINPUTDEVICE));
	ASSERT(result);
}


LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		PAINTSTRUCT paint;
		BeginPaint(hwnd, &paint);
		g_Active = !IsRectEmpty(&paint.rcPaint);
		EndPaint(hwnd, &paint);
		break;

	case WM_INPUT:
		{
			HRAWINPUT hRawInput = (HRAWINPUT) lParam;

			RAWINPUT raw_input;

			UINT size = sizeof(raw_input);
			GetRawInputData(hRawInput, RID_INPUT, &raw_input, &size, sizeof(RAWINPUTHEADER));

			const bool has_focus = (GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT);


			if (raw_input.header.dwType == RIM_TYPEKEYBOARD)
			{
				const bool pressed = ((raw_input.data.keyboard.Flags & RI_KEY_BREAK) == 0);

				if (has_focus)
				{
					app->OnKey(raw_input.data.keyboard.VKey, pressed);

//	 				char str[256];
// 					sprintf(str, "Keyboard: %d, Flags: %d\n", raw_input.data.keyboard.VKey, raw_input.data.keyboard.Flags);
// 					OutputDebugStringA(str);
				}

				app->UpdateKey(raw_input.data.keyboard.VKey, pressed);

			}
			else if (raw_input.header.dwType == RIM_TYPEMOUSE)
			{
// 				char str[256];
// 				sprintf(str, "Mouse: %d, %d\n", raw_input.data.mouse.lLastX, raw_input.data.mouse.lLastY);
// 				OutputDebugStringA(str);

				if (has_focus)
				{
					app->UpdateMouse(raw_input.data.mouse.lLastX, raw_input.data.mouse.lLastY);
				}

				//app->OnMouseButton(raw_input.data.mouse.
				//raw_input.data.mouse
			}

			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		break;
	case WM_MOUSEMOVE:
		app->OnMouseMove(GETX(lParam), GETY(lParam));
		break;
	case WM_LBUTTONDOWN:
		app->OnMouseButton(GETX(lParam), GETY(lParam), MOUSE_LEFT, true);
 		break;
	case WM_LBUTTONUP:
		app->OnMouseButton(GETX(lParam), GETY(lParam), MOUSE_LEFT, false);
		break;
	case WM_RBUTTONDOWN:
		app->OnMouseButton(GETX(lParam), GETY(lParam), MOUSE_RIGHT, true);
 		break;
	case WM_RBUTTONUP:
		app->OnMouseButton(GETX(lParam), GETY(lParam), MOUSE_RIGHT, false);
		break;
	case WM_MBUTTONDOWN:
		app->OnMouseButton(GETX(lParam), GETY(lParam), MOUSE_MIDDLE, true);
		break;
	case WM_MBUTTONUP:
		app->OnMouseButton(GETX(lParam), GETY(lParam), MOUSE_MIDDLE, false);
		break;
	case WM_MOUSEWHEEL:
		static int scroll;
		int s;

		scroll += GET_WHEEL_DELTA_WPARAM(wParam);
		s = scroll / WHEEL_DELTA;
		scroll %= WHEEL_DELTA;

		POINT point;
		point.x = GETX(lParam);
		point.y = GETY(lParam);
		ScreenToClient(hwnd, &point);

		if (s != 0)
			app->OnMouseWheel(point.x, point.y, s);
		break;
	case WM_SETFOCUS:
		app->SetInputFocus(true);
		break;
	case WM_KILLFOCUS:
		app->SetInputFocus(false);
		break;
	case WM_SIZE:
		// Ignore WM_SIZE during window creation
		//if (app->GetDevice()->GetWindow() != nullptr)
		if (app->GetDevice() != nullptr)
		{
			app->OnSize(LOWORD(lParam), HIWORD(lParam));
		}
		break;
/*	case WM_WINDOWPOSCHANGED:
		if ((((LPWINDOWPOS) lParam)->flags & SWP_NOSIZE) == 0)
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
			int w = rect.right - rect.left;
			int h = rect.bottom - rect.top;
			if (w > 0 && h > 0) app->onSize(w, h);
		}
		break;*/
	/*case WM_SYSKEYDOWN:
		if ((lParam & (1 << 29)) && wParam == KEY_ENTER)
		{
			app->toggleFullscreen();
		}
		else
		{
			app->OnKey((unsigned int) wParam, true);
		}
		break;*/
	case WM_SYSKEYUP:
		app->OnKey((unsigned int) wParam, false);
		break;
	case WM_CREATE:
		ShowWindow(hwnd, SW_SHOW);
		InitializeInput(hwnd);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE /*hLastInst*/, LPSTR /*lpszCmdLine*/, int /*nCmdShow*/)
{
#ifdef _DEBUG
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // Get current flag
	flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
	flag |= _CRTDBG_CHECK_ALWAYS_DF; // Turn on CrtCheckMemory
//	flag |= _CRTDBG_DELAY_FREE_MEM_DF;
	_CrtSetDbgFlag(flag); // Set flag to the new value
#endif

//	initCPU();

	// Make sure we're running in the exe's path
	// TODO: Make unicode
	char path[MAX_PATH];
	if (GetModuleFileNameA(nullptr, path, sizeof(path)))
	{
		char *slash = strrchr(path, '\\');
		if (slash) *slash = '\0';
		//chdir(path);
		SetCurrentDirectoryA(path);
	}

	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = 0;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hThisInst;
	wc.hIcon = LoadIcon(hThisInst, MAKEINTRESOURCE(IDI_MAINICON));
	wc.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = L"Humus";
	wc.hIconSm = nullptr;

	ATOM win_class = RegisterClassEx(&wc);
	if (!win_class) return 0;

	//app->setInstance(hThisInst);

#if 0
	//JOYINFO joyInfo;
	//bool useJoystick = (joyGetPos(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR);
	JOYCAPS joyCaps;
	DWORD joyFlags = 0;
	float xScale = 0, xBias = 0;
	float yScale = 0, yBias = 0;
	float zScale = 0, zBias = 0;
	float rScale = 0, rBias = 0;
	float uScale = 0, uBias = 0;
	float vScale = 0, vBias = 0;
	if (joyGetDevCaps(JOYSTICKID1, &joyCaps, sizeof(joyCaps)) == JOYERR_NOERROR)
	{
		joyFlags = JOY_RETURNX | JOY_RETURNY | JOY_RETURNBUTTONS;
		xScale = 2.0f / float(int(joyCaps.wXmax) - int(joyCaps.wXmin));
		xBias  = 1.0f - joyCaps.wXmax * xScale;
		yScale = 2.0f / float(int(joyCaps.wYmax) - int(joyCaps.wYmin));
		yBias  = 1.0f - joyCaps.wYmax * yScale;

		if (joyCaps.wCaps & JOYCAPS_HASZ)
		{
			joyFlags |= JOY_RETURNZ;
			zScale = 2.0f / float(int(joyCaps.wZmax) - int(joyCaps.wZmin));
			zBias  = 1.0f - joyCaps.wZmax * zScale;
		}
		if (joyCaps.wCaps & JOYCAPS_HASR)
		{
			joyFlags |= JOY_RETURNR;
			rScale = 2.0f / float(int(joyCaps.wRmax) - int(joyCaps.wRmin));
			rBias  = 1.0f - joyCaps.wRmax * rScale;
		}
		if (joyCaps.wCaps & JOYCAPS_HASU)
		{
			joyFlags |= JOY_RETURNU;
			uScale = 2.0f / float(int(joyCaps.wUmax) - int(joyCaps.wUmin));
			uBias  = 1.0f - joyCaps.wUmax * uScale;
		}
		if (joyCaps.wCaps & JOYCAPS_HASV)
		{
			joyFlags |= JOY_RETURNV;
			vScale = 2.0f / float(int(joyCaps.wVmax) - int(joyCaps.wVmin));
			vBias  = 1.0f - joyCaps.wVmax * vScale;
		}
	}
#endif

	// Initialize timer
	//app->initTime();

	//app->loadConfig();
	//app->initGUI();

	/*
		Force the main thread to always run on CPU 0.
		This is done because on some systems QueryPerformanceCounter returns a bit different counter values
		on the different CPUs (contrary to what it's supposed to do), which can cause negative frame times
		if the thread is scheduled on the other CPU in the next frame. This can cause very jerky behavior and
		appear as if frames return out of order.
	*/
	SetThreadAffinityMask(GetCurrentThread(), 1);

	MSG msg;
	msg.wParam = 0;

	if (app->Create())
	{
		if (app->Init())
		{
			app->Start();

			//app->resetCamera();

	/*			
			app->loadConfig();

			if (!app->initCaps()) break;
			if (!app->initAPI()) break;

			if (!app->load())
			{
				app->closeWindow(true, false);
			}*/

			bool quit = false;
			for (;;)
			{

				//InvalidateRect(app->GetRenderer()->GetWindow(), nullptr, FALSE);

				while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
				{
					if (msg.message == WM_QUIT)
					{
						quit = true;
					}

					//TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				if (quit)
					break;

				if (g_Active)
				{
					/*
						Joystick support
					*/
#if 0
					if (joyFlags)
					{
						static DWORD lastButtons = 0;
						static DWORD lastXpos = 0, lastYpos = 0, lastZpos = 0;
						static DWORD lastRpos = 0, lastUpos = 0, lastVpos = 0;

						JOYINFOEX joyInfo;
						joyInfo.dwSize = sizeof(joyInfo);
						joyInfo.dwFlags = joyFlags;

						if (joyGetPosEx(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR)
						{
							DWORD changed = lastButtons ^ joyInfo.dwButtons;
							if (changed)
							{
								for (uint i = 0; i < joyCaps.wNumButtons; i++)
								{
									// Only call App for buttons that changed
									if (changed & 1)
									{
										app->onJoystickButton(i, ((joyInfo.dwButtons >> i) & 1) != 0);
									}
									changed >>= 1;
								}

								lastButtons = joyInfo.dwButtons;
							}
							if ((joyInfo.dwFlags & JOY_RETURNX) && joyInfo.dwXpos != lastXpos)
							{
								app->onJoystickAxis(0, joyInfo.dwXpos * xScale + xBias);
								lastXpos = joyInfo.dwXpos;
							}
							if ((joyInfo.dwFlags & JOY_RETURNY) && joyInfo.dwYpos != lastYpos)
							{
								app->onJoystickAxis(1, joyInfo.dwYpos * yScale + yBias);
								lastYpos = joyInfo.dwYpos;
							}
							if ((joyInfo.dwFlags & JOY_RETURNZ) && joyInfo.dwZpos != lastZpos)
							{
								app->onJoystickAxis(2, joyInfo.dwZpos * zScale + zBias);
								lastZpos = joyInfo.dwZpos;
							}
							if ((joyInfo.dwFlags & JOY_RETURNR) && joyInfo.dwRpos != lastRpos)
							{
								app->onJoystickAxis(3, joyInfo.dwRpos * rScale + rBias);
								lastRpos = joyInfo.dwRpos;
							}
							if ((joyInfo.dwFlags & JOY_RETURNU) && joyInfo.dwUpos != lastUpos)
							{
								app->onJoystickAxis(4, joyInfo.dwUpos * uScale + uBias);
								lastUpos = joyInfo.dwUpos;
							}
							if ((joyInfo.dwFlags & JOY_RETURNV) && joyInfo.dwVpos != lastVpos)
							{
								app->onJoystickAxis(5, joyInfo.dwVpos * vScale + vBias);
								lastVpos = joyInfo.dwVpos;
							}
						}
					}
#endif

					app->MakeFrame();
				}
				else
				{
					Sleep(100);
				}
			}

		}

		app->Exit();
	}

	app->Destroy();
//	delete app;

	//UnregisterClass((LPCWSTR) win_class, hThisInst);

	return (int) msg.wParam;
}
