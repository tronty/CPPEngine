#include <stdio.h>
#ifdef _MSC_VER
#include <io.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#include <direct.h>
using namespace std;
#define MAX_CONSOLE_LINES 500
#pragma warning( disable : 4311 )
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#ifdef _WIN32
#pragma comment(linker, "/subsystem:windows")
#define PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#define PlatformSurfaceCreateInfo VkWin32SurfaceCreateInfoKHR
#define PLATFORM_SURFACE_CREATE_INFO VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
#define PLATFORM_ASSIGN_SURFACEINFO \
    create_info.hinstance = GetModuleHandle(NULL); \
    create_info.hwnd = wmInfo.info.win.window
#define PlatformCreateSurface vkCreateWin32SurfaceKHR
#else /* _WIN32 */
#define PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#define PlatformSurfaceCreateInfo VkXlibSurfaceCreateInfoKHR
#define PLATFORM_SURFACE_CREATE_INFO VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
#define PLATFORM_ASSIGN_SURFACEINFO \
    create_info.dpy = wmInfo.info.x11.display; \
    create_info.window = wmInfo.info.x11.window
#define PlatformCreateSurface vkCreateXlibSurfaceKHR
#endif /* _WIN32 */
#include <vulkan/vulkan.h>

VkApplicationInfo applicationInfo;
VkInstanceCreateInfo instanceInfo;
VkInstance instance;

int 
#if defined(_MSC_VER)
//pragma comment(linker, "/ENTRY:WinMainCRTStartup")
//pragma comment(linker, "/SUBSYSTEM:WINDOWS")
 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	{
			int				hConHandle;
			int				lStdHandle;
			CONSOLE_SCREEN_BUFFER_INFO	coninfo;
			FILE				*fp;
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
	}
#elif !defined(_MSC_VER)
 main(int argc, char *argv[])
{
#endif 
    int ret=0;

    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;	// sType
    applicationInfo.pNext = NULL;				// pNext
    applicationInfo.pApplicationName = "Blank Vulkan Window";	// Application name
    applicationInfo.pEngineName = NULL;				// Name of your graphics/game engine
    applicationInfo.engineVersion = 1;				// Engine development version
    applicationInfo.apiVersion = VK_API_VERSION_1_0;		// Vulkan's version

    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;// sType
    instanceInfo.pNext = NULL;					// pNext
    instanceInfo.flags = 0;					// flags, 0 for now
    instanceInfo.pApplicationInfo = &applicationInfo;		// link to vkApplicationInfo object
    instanceInfo.enabledLayerCount = 0;				// disable layers
    instanceInfo.ppEnabledLayerNames = NULL;
    instanceInfo.enabledExtensionCount = 0;			// disable extensions
    instanceInfo.ppEnabledExtensionNames = NULL;

    // Create the instance
    VkResult result = vkCreateInstance(&instanceInfo, NULL, &instance);

    // Check the result and exit the app with error code if not successful
    if (result != VK_SUCCESS) {
        STX_PRINT("Cannot create VkInstance: %d\n", result);
	ret=3;
    } else {
        STX_PRINT("Successfully created VkInstance!\n");
	ret=0;
    }

    // ??? vkDestroyInstance(instance, NULL);				// release

	SDL_Window* SDL_window=0;

	SDL_Init( 	SDL_INIT_TIMER|
			SDL_INIT_AUDIO|
			SDL_INIT_JOYSTICK|
			SDL_INIT_VIDEO);
	SDL_window=SDL_CreateWindow("VulkanTest", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 800, SDL_WINDOW_OPENGL);
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(SDL_window, &wmInfo);
	VkSurfaceKHR _VkSurface;
 	PlatformSurfaceCreateInfo create_info;
    create_info.sType      = PLATFORM_SURFACE_CREATE_INFO;
    create_info.pNext      = NULL;
    create_info.flags      = 0;
	PLATFORM_ASSIGN_SURFACEINFO;
    VkResult vk_res = PlatformCreateSurface(instance, &create_info, NULL, &(_VkSurface));
    if(VK_SUCCESS != vk_res)
    	STX_PRINT("PlatformCreateSurface:VK_SUCCESS != vk_res\n");

	SDL_Event event;
	while(1)
	{
		if(SDL_PollEvent(&event))
		{
			if(event.key.keysym.sym==SDLK_ESCAPE)
				break;
		}
	}
	#ifdef _MSC_VER
	FreeConsole();
	#endif

    return ret;
}
