/* * * * * * * * * * * * * Author's note * * * * * * * * * * * **
*  |_|	   |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|	*
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _	*
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_	*
*  |_|	   |_| |_|_ _ _|_| |_|	   |_| |_|_ _ _|_|  _ _ _ _|_|	*
*  |_|	   |_|	 |_|_|_|   |_|	   |_|	 |_|_|_|   |_|_|_|_|	*
*								*
*		      http://www.humus.name			*
*								 *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.		   *
*								 *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <STX/STX.h>

//include <Framework3/OpenGLExtensions.h>
#include <Framework3/IRenderer.h>
//using namespace CG;
//include <ResourceReuseCache.h>
#include <STX/LuaUtil.h>
using namespace LuaUtil;

#if 1

#if !defined( _WIN32 ) && !defined(SDL_2_0_5_BUILD) && !defined(DBG_HALT)
#define DBG_HALT
#endif

#define USEXML 1

#if 1
#define STX_PRINT(...) printf(__VA_ARGS__)
#define STX_FNLN printf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
#define LOG_FNLN
#define LOG_PRINT
#define LOG_FNLN_NONE
#define LOG_PRINT_NONE
#define LOG_FNLN_X
#define LOG_PRINT_X
#else
#define LOG_PRINT(...) LOG_PRINT(__VA_ARGS__);
#define LOG_FNLN LOG_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__)
#define LOG_FNLN_X
#define LOG_PRINT_X
#endif

#if 0//defined(ANDROID)
#ifdef __cplusplus
extern "C" {
#endif
int init(unsigned int width, unsigned int height);
	void init3(unsigned int width, unsigned int height)
{
    IRenderer* r=IRenderer::GetRendererInstance(width, height, "SimpleTexture");		
	IInput*    i=STX_Service::GetInputInstance();
	init(width, height);
}

	void loop3()
{
	while (!STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_ESCAPE) && !STX_Service::GetInputInstance()->Quit())
	{
		STX_Service::GetInputInstance()->Update();
		render();
	}
}
JNIEXPORT void JNICALL Java_org_libsdl_app_SDLActivity_nativeInit_(JNIEnv* env, jclass jcls, jint width, jint height)
{
    init3(width, height);
}
JNIEXPORT void JNICALL Java_org_libsdl_app_SDLActivity_nativeLoop(JNIEnv* env, jclass jcls)
{
    loop3();
}
#ifdef __cplusplus
	};
#endif
#endif



#if 0//defined(ANDROID)
#ifdef __cplusplus
extern "C" {
#endif
static volatile int doterminate;
void closemutex(int sig);
static SDL_threadID mainthread;
int SDLCALL
Run1(void *data)
{
    if (SDL_ThreadID() == mainthread)
        signal(SIGTERM, closemutex);
    while (!doterminate) {
        LOG_PRINT_X("Process %lu ready to work\n", SDL_ThreadID());
        if (SDL_LockMutex(mutex) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't lock mutex: %s", SDL_GetError());
            stx_exit(1);
        }
        LOG_PRINT_X("Process %lu, working!\n", SDL_ThreadID());

	IRenderer* r=IRenderer::GetRendererInstance(640, 512, "NDK_SDL2");
	IInput*    i=STX_Service::GetInputInstance();

        LOG_PRINT_X("Process %lu, done!\n", SDL_ThreadID());
        if (SDL_UnlockMutex(mutex) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't unlock mutex: %s", SDL_GetError());
            stx_exit(1);
        }
        /* If this sleep isn't done, then threads may starve */
        SDL_Delay(10);
    }
    if (SDL_ThreadID() == mainthread && doterminate) {
        LOG_PRINT_X("Process %lu:  raising SIGTERM\n", SDL_ThreadID());
        raise(SIGTERM);
    }
    return (0);
}
int SDLCALL
Run3(void *data)
{
    if (SDL_ThreadID() == mainthread)
        signal(SIGTERM, closemutex);
    while (!doterminate) {
        LOG_PRINT_X("Process %lu ready to work\n", SDL_ThreadID());
        if (SDL_LockMutex(mutex) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't lock mutex: %s", SDL_GetError());
            stx_exit(1);
        }
        LOG_PRINT_X("Process %lu, working!\n", SDL_ThreadID());
	while (!STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_ESCAPE) && !STX_Service::GetInputInstance()->Quit())
	{
		STX_Service::GetInputInstance()->Update();
		render();
	}
        LOG_PRINT_X("Process %lu, done!\n", SDL_ThreadID());
        if (SDL_UnlockMutex(mutex) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't unlock mutex: %s", SDL_GetError());
            stx_exit(1);
        }
        /* If this sleep isn't done, then threads may starve */
        SDL_Delay(10);
    }
    if (SDL_ThreadID() == mainthread && doterminate) {
        LOG_PRINT_X("Process %lu:  raising SIGTERM\n", SDL_ThreadID());
        raise(SIGTERM);
    }
    return (0);
}
#ifdef __cplusplus
	};
#endif
#endif

	bool IRenderer::SDL=false;
//namespace STX
//{

//};
// ImageLib lib (for loading images and applying filters)

unsigned int IRenderer::width=800;
unsigned int IRenderer::height=600;
#ifdef _MSCVER
unsigned int IRenderer::bpp=32;
#else
unsigned int IRenderer::bpp=24;
#endif
#ifdef USEXML 
	TiXmlElement* _IRenderer_BodyElement=0;
#else
	LuaScript _IRenderer_script;
#endif
bool IRenderer::m_bLuaScript=false;
bool IRenderer::m_bDebug=true;

// --------------------------------------------------------------------------------
//									 Private methods
// --------------------------------------------------------------------------------

/*
*************************
Initialized the input flags
*************************
*/
Shader::~Shader(){}

	IRenderer::~IRenderer()
	{
	};

#include <Framework3/RendererHLSLCg.h>
D3DXFROMWINEVECTOR4& RendererHLSLCg::GetscaleBias2D()
{
	return scaleBias2D;
}
#define ADDHLSLSHADER addShader
void RendererHLSLCg::InitTexN(ShaderID& s, VertexFormatID& vf)
{
	LOG_FNLN;if(vf==-1)
	{
		std::string fn = Framework3ShaderFactory::GetShader("texSHD0N");
		s = addShader(fn.c_str(), "main", "main", 0, eHLSL_Shader);
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
			0, TYPE_NORMAL,   FORMAT_FLOAT, 4,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
		}
		;
		vf = addVertexFormat(format, elementsOf(format), s);
	}
}
void RendererHLSLCg::InitTex2N(ShaderID& s, VertexFormatID& vf)
{
	LOG_FNLN;if(vf==-1)
	{
		std::string fn = Framework3ShaderFactory::GetShader("texSHD2N");
		s = addShader(fn.c_str(), "main", "main", 0, eHLSL_Shader);
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
			0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
		}
		;
		vf = addVertexFormat(format, elementsOf(format), s);
	}
}
void RendererHLSLCg::InitTex3N(ShaderID& s, VertexFormatID& vf)
{
	LOG_FNLN;if(vf==-1)
	{
		std::string fn = Framework3ShaderFactory::GetShader("texSHD1N");
		s = addShader(fn.c_str(), "main", "main", 0, eHLSL_Shader);
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
			0, TYPE_NORMAL,   FORMAT_FLOAT, 4,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
		}
		;
		vf = addVertexFormat(format, elementsOf(format), s);
	}
}
void RendererHLSLCg::InitPlainN(ShaderID& s, VertexFormatID& vf)
{
	LOG_FNLN;if(vf==-1)
	{
		std::string fn = Framework3ShaderFactory::GetShader("plainSHD1N");
		s = addShader(fn.c_str(), "main", "main", 0, eHLSL_Shader);
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
			0, TYPE_NORMAL,   FORMAT_FLOAT, 4,
		}
		;
		vf = addVertexFormat(format, elementsOf(format), s);
	}
}
void RendererHLSLCg::InitPlain3N(ShaderID& s, VertexFormatID& vf)
{
	LOG_FNLN;if(vf==-1)
	{
		std::string fn = Framework3ShaderFactory::GetShader("plainSHD2N");
		s = addShader(fn.c_str(), "main", "main", 0, eHLSL_Shader);
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
			0, TYPE_NORMAL,   FORMAT_FLOAT, 4,
		}
		;
		vf = addVertexFormat(format, elementsOf(format), s);
	}
}
void RendererHLSLCg::InitTex(ShaderID& s, VertexFormatID& vf)
{
	LOG_FNLN;if(vf==-1)
	{
		std::string fn = Framework3ShaderFactory::GetShader("texSHD0");
		s = addShader(fn.c_str(), "main", "main", 0, eHLSL_Shader);
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
		}
		;
		vf = addVertexFormat(format, elementsOf(format), s);
	}
}
void RendererHLSLCg::InitTex3(ShaderID& s, VertexFormatID& vf)
{
	LOG_FNLN;if(vf==-1)
	{
		std::string fn = Framework3ShaderFactory::GetShader("texSHD1");
		s = addShader(fn.c_str(), "main", "main", 0, eHLSL_Shader);
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
		}
		;
		vf = addVertexFormat(format, elementsOf(format), s);
	}
}
void RendererHLSLCg::InitPlain(ShaderID& s, VertexFormatID& vf)
{
	LOG_FNLN;if(vf==-1)
	{
		std::string fn = Framework3ShaderFactory::GetShader("plainSHD1");
		s = addShader(fn.c_str(), "main", "main", 0, eHLSL_Shader);
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		}
		;
		vf = addVertexFormat(format, elementsOf(format), s);
	}
}
void RendererHLSLCg::InitPlain3(ShaderID& s, VertexFormatID& vf)
{
	LOG_FNLN;if(vf==-1)
	{
		std::string fn = Framework3ShaderFactory::GetShader("plainSHD2");
		s = addShader(fn.c_str(), "main", "main", 0, eHLSL_Shader);
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		}
		;
		vf = addVertexFormat(format, elementsOf(format), s);
	}
}
int IRenderer::fps=0;
#define FPS 60
Uint32 waittime = 1000.0f/FPS;
Uint32 framestarttime = 0;
Sint32 delaytime;

void IRenderer::OnBeginScene()
{
	m_nBeginScene++;
}
void IRenderer::OnEndScene()
{
	m_nEndScene++;

	static bool bRender=false;
	if(STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_p))
		bRender=!bRender;
	if(!bRender)
		return;
	int vpw=GetViewportWidth();
	int vph=GetViewportHeight()/2;
	unsigned int n=GetTextureCount();
	float ss=32.0f;
	
	unsigned int ny=vph/ss;
	unsigned int nx=vpw/ss;
	TextureID c=0;
	for(unsigned int xx=0;xx<nx;xx++)
	for(unsigned int yy=0;yy<ny;yy++)
	{
	if(c>n) continue;
	D3DXFROMWINEVECTOR2 aposition(ss*xx,ss*yy);
	D3DXFROMWINEVECTOR2 asize(ss,ss);
	TexVertex dest[4];
	float x=aposition.x;
	float y=aposition.y;
	dest[0].position = D3DXFROMWINEVECTOR2(x + asize.x, y);
	dest[0].texCoord = D3DXFROMWINEVECTOR2(1.0f, 0.0f);
	dest[1].position = D3DXFROMWINEVECTOR2(x + asize.x, y + asize.y);
	dest[1].texCoord = D3DXFROMWINEVECTOR2(1.0f, 1.0f);
	dest[2].position = D3DXFROMWINEVECTOR2(x, y);
	dest[2].texCoord = D3DXFROMWINEVECTOR2(0.0f, 0.0f);
	dest[3].position = D3DXFROMWINEVECTOR2(x, y + asize.y);
	dest[3].texCoord = D3DXFROMWINEVECTOR2(0.0f, 1.0f);
	unsigned int ret=drawTextured(PRIM_TRIANGLE_STRIP, dest, 2, c++, GetblendSrcAlpha(), GetnoDepthTest());
	}	
}

void IRenderer::OnClear()
{
#if 0
	if(1)
	{
		LOG_PRINT("IRenderer::OnClear:Title:%s\n", STX_Service::GetWindowInstance()->GetCaption());
	}
#endif
}
void IRenderer::OnPresent(IInput* Input)
{
#if 0
	if(1)
	{
		LOG_PRINT("IRenderer::OnPresent:Title:%s\n", STX_Service::GetWindowInstance()->GetCaption());
		if(m_nBeginScene!=m_nEndScene)
		{
			LOG_PRINT("m_nBeginScene=%d\n", m_nBeginScene);
			LOG_PRINT("m_nEndScene=%d\n", m_nEndScene);
		}
		if((m_nBeginScene==0)&&(m_nEndScene==0))
		{
			LOG_PRINT("m_nBeginScene=%d\n", m_nBeginScene);
			LOG_PRINT("m_nEndScene=%d\n", m_nEndScene);
		}
		stx_exit(0);
	}
#endif
{    
    static float framesPerSecond    = 0.0f;       // This will store our fps
    static float lastTime   = 0.0f;       // This will hold the time from the last frame
    float currentTime = timeGetTime() * 0.001f;    
    ++framesPerSecond;
    if( currentTime - lastTime > 1.0f )
    {
        lastTime = currentTime;
        framesPerSecond = 0;
		fps = (int)framesPerSecond;
    }
}	//LOG_FNLN_NONE;
		if(Input) Input->Update ();

	//LOG_FNLN_NONE;
		static int w=STX_Service::GetWindowInstance()->Getwidth();
	static int h=STX_Service::GetWindowInstance()->GetHeight();
	if((STX_Service::GetWindowInstance()->Getwidth()!=w)||
	   (STX_Service::GetWindowInstance()->Getheight()!=h))
	{
		w=STX_Service::GetWindowInstance()->Getwidth();

		h=STX_Service::GetWindowInstance()->GetHeight();
		s_pRenderer->setViewport(0,0,w,h);
	}
//LOG_FNLN_NONE;
		
/*
bool done = 0;
while(!done)
{
    while(SDL_PollEvent(&event))
    {
        ...
    }
*/
    // Do stuff

    //SDL_Flip(screen);

    delaytime = waittime - (timeGetTime() - framestarttime);
    if(0)//???delaytime > 0)
        STX_Service::GetTimerInstance()->stx_Delay((Uint32)delaytime);
    framestarttime = timeGetTime();
#ifndef _MSC_VER
	//LOG_FNLN_NONE;
		if(STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_x))
	{		
		char buffer [256];
		buffer [0]='\0';
#if !defined(__MINGW32__) && !defined(__MINGW64__)
		//LOG_FNLN_NONE;
		time_t rawtime;
		struct tm * timeinfo;

		time (&rawtime);
		timeinfo = localtime (&rawtime);

		strftime (buffer,256,"%F_%r", timeinfo);
#endif
	char buf[1024] = {0};
	//stx_strlcpy(buf, "Framework3 - ", 1024);
	stx_strlcat(buf, m_title.c_str(), 1024);
	stx_strlcat(buf, " ", 1024);
	#if 0
	stx_strlcat(buf, IRenderer::s_pRenderer->getString(), 1024);
	if(sizeof(void *) == 4)
		stx_strlcat(buf, " - 32 bit ", 1024);
	else if(sizeof(void *) == 8)
		stx_strlcat(buf, " - 64 bit ", 1024);
	#endif
	stx_strlcat(buf, buffer, 1024);
#if 0
{
    const Uint32 format = SDL_PIXELFORMAT_ARGB8888;
    const int width = 256;
    const int height = 256;
    auto renderer = sdl2Core->GetRenderer();
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, format);
    SDL_RenderReadPixels(renderer, NULL, format, surface->pixels, surface->pitch);
    SDL_SaveBMP(surface, buffer2);
    SDL_FreeSurface(surface);
}
#elif 1
		IRenderer::GetRendererInstance()->SaveScreenShot(buf);
#endif
	}
#endif
//LOG_FNLN_NONE;
		
/*
	IND_Camera2d mCamera2d (CFrmaework3::Instance()->Window->GetWidth () / 2, CFrmaework3::Instance()->Window->GetHeight() / 2);
	IND_Camera3d mCamera3d (0.0f, 1.4f, -4.0f);
	mCamera3d.SetAspect ((float) CFrmaework3::Instance()->Window->GetWidth () / CFrmaework3::Instance()->Window->GetHeight());

		CFrmaework3::Instance()->Render->SetViewPort2d (0, 0, CFrmaework3::Instance()->Window->GetWidth () , CFrmaework3::Instance()->Window->GetHeight());
		CFrmaework3::Instance()->Render->SetCamera2d (&mCamera2d);
		CFrmaework3::Instance()->Entity2dManager->RenderEntities2d ();

		CFrmaework3::Instance()->Render->SetViewPort3d (0, 0, CFrmaework3::Instance()->Window->GetWidth () , CFrmaework3::Instance()->Window->GetHeight());
		CFrmaework3::Instance()->Render->SetCamera3d (&mCamera3d);
		CFrmaework3::Instance()->LightManager->Update ();
		CFrmaework3::Instance()->Entity3dManager->RenderEntities3d ();
*/
}

#if 0//def _WIN32
OSVERSIONINFOEX m_osver;
#define BUFSIZE 80
int IRenderer::WinVer()//int argc, char *argv[])
{
#if 0
	LOG_PRINT_X("Registry Versions:\n");
   BOOL bOsVersionInfoEx;
   // Try calling GetVersionEx() using the OSVERSIONINFOEX structure.
   // If that fails, try using the OSVERSIONINFO structure.
   ZeroMemory(&m_osver, sizeof(OSVERSIONINFOEX));
   m_osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
   if(!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *) &m_osver)))
   {
      m_osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
      if(!GetVersionEx((OSVERSIONINFO *) &m_osver))
	return FALSE;
     // else
       //LOG_PRINT_X("Buffer for the structure size is OK.\n");
   }
   //else
     //  LOG_PRINT_X("GetVersionEx() is OK.\n");
   switch (m_osver.dwPlatformId)
   {
      //Test for the Windows NT product family.
      case VER_PLATFORM_WIN32_NT:
	 // Test for the specific product family.
	 if(m_osver.dwMajorVersion == 6 && m_osver.dwMinorVersion == 1)
	    LOG_PRINT_X("Windows 7\n");

	 if(m_osver.dwMajorVersion == 6 && m_osver.dwMinorVersion == 0)
	    LOG_PRINT_X("Windows Server 2008\n");
	 if(m_osver.dwMajorVersion == 6 && m_osver.dwMinorVersion == 0)
	    LOG_PRINT_X("Windows Vista\n");
	 if(m_osver.dwMajorVersion == 5 && m_osver.dwMinorVersion == 2)
	    LOG_PRINT_X("Windows Server 2003 with SP1, Windows XP with SP2\n");
	 if(m_osver.dwMajorVersion == 5 && m_osver.dwMinorVersion == 1)
	    LOG_PRINT_X("Windows Server 2003, Windows XP\n");
	 if(m_osver.dwMajorVersion == 5 && m_osver.dwMinorVersion == 0)
	    LOG_PRINT_X("Windows 2000\n");
	 if(m_osver.dwMajorVersion <= 4)
	    LOG_PRINT_X("Windows NT\n");
	 // Test for specific product on Windows NT 4.0 SP6 and later.
	 if(bOsVersionInfoEx)
	 {
	    // Test for the workstation type.
	    if(m_osver.wProductType == VER_NT_WORKSTATION)
	    {
	       if(m_osver.dwMajorVersion == 4)
		  LOG_PRINT_X("Workstation 4.0\n");
	       else if(m_osver.wSuiteMask & VER_SUITE_PERSONAL)
		  LOG_PRINT_X("Home Edition\n");
	       else
		  LOG_PRINT_X("Professional\n");
	    }
	    // Test for the server type.
	    else if(m_osver.wProductType == VER_NT_SERVER || m_osver.wProductType == VER_NT_DOMAIN_CONTROLLER)
	    {
	       if((m_osver.dwMajorVersion == 5) && (m_osver.dwMinorVersion == 2))
	       {
		  if(m_osver.wSuiteMask & VER_SUITE_DATACENTER)
		     LOG_PRINT_X("Datacenter Edition\n");
		  else if(m_osver.wSuiteMask & VER_SUITE_ENTERPRISE)
		     LOG_PRINT_X("Enterprise Edition\n");
		  else if(m_osver.wSuiteMask == VER_SUITE_BLADE)
		     LOG_PRINT_X("Web Edition\n");
		  else
		     LOG_PRINT_X("Standard Edition\n");
	       }
	       else if((m_osver.dwMajorVersion == 5) && (m_osver.dwMinorVersion == 0))
	       {
		  if(m_osver.wSuiteMask & VER_SUITE_DATACENTER)
		     LOG_PRINT_X("Datacenter Server\n");
		  else if(m_osver.wSuiteMask & VER_SUITE_ENTERPRISE)
		     LOG_PRINT_X("Advanced Server\n");
		  else
		     LOG_PRINT_X("Server\n");
	       }
	       // Windows NT 4.0
	       else
	       {
		  if(m_osver.wSuiteMask & VER_SUITE_ENTERPRISE)
		     LOG_PRINT_X("Server 4.0, Enterprise Edition\n");
		  else
		     LOG_PRINT_X("Server 4.0\n");
	       }
	    }
	 }
	 // Test for specific product on Windows NT 4.0 SP5 and earlier
	 else
	 {
	    HKEY hKey;
	    char szProductType[BUFSIZE];
	    __DWORD__ dwBufLen=BUFSIZE;
	    LONG lRet;
	    lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\ProductOptions", 0, KEY_QUERY_VALUE, &hKey);
	    if(lRet != ERROR_SUCCESS)
	       return FALSE;
	    else
	      LOG_PRINT_X("RegOpenKeyEx() is OK.\n");
	    lRet = RegQueryValueExA(hKey, "ProductType", 0, 0, (LPBYTE)szProductType, &dwBufLen);
	    if((lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE))
	       return FALSE;
	    else
	      LOG_PRINT_X("RegQueryValueEx() is OK.\n");
	    RegCloseKey(hKey);
	    if(lstrcmpiA("WINNT", szProductType) == 0)
	       LOG_PRINT_X("Workstation ");
	    if(lstrcmpiA("LANMANNT", szProductType) == 0)
	       LOG_PRINT_X("Server ");
	    if(lstrcmpiA("SERVERNT", szProductType) == 0)
	       LOG_PRINT_X("Advanced Server ");
	    LOG_PRINT_X("%d.%d ", m_osver.dwMajorVersion, m_osver.dwMinorVersion);
	 }
	 // Display service pack (if any) and build number.
	 if(m_osver.dwMajorVersion == 4 && lstrcmpiW(m_osver.szCSDVersion, L"Service Pack 6") == 0)
	 {
	    HKEY hKey;
	    LONG lRet;
	    // Test for SP6 versus SP6a.
	    lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\WindowsNT\\CurrentVersion\\Hotfix\\Q246009", 0, KEY_QUERY_VALUE, &hKey);
	    if(lRet == ERROR_SUCCESS)
	    {
	      LOG_PRINT_X("RegOpenKeyEx() is OK.\n");
	      LOG_PRINT_X("Service Pack 6a (Build %d)\n", m_osver.dwBuildNumber & 0xFFFF);
	    }
	    // Windows NT 4.0 prior to SP6a
	    else
	    {
	       LOG_PRINT_X("%s (Build %d)\n", m_osver.szCSDVersion, m_osver.dwBuildNumber & 0xFFFF);
	    }
	     RegCloseKey(hKey);
	 }
	// Windows NT 3.51 and earlier or Windows 2000 and later
	 else
	 {
	    LOG_PRINT_X("%s (Build %d)\n", m_osver.szCSDVersion, m_osver.dwBuildNumber & 0xFFFF);
	 }
	 break;
      // Test for the Windows 95 product family.
      case VER_PLATFORM_WIN32_WINDOWS:
	 if(m_osver.dwMajorVersion == 4 && m_osver.dwMinorVersion == 0)
	 {
	     LOG_PRINT_X("Microsoft Windows 95 ");
	     if(m_osver.szCSDVersion[1] == 'C' || m_osver.szCSDVersion[1] == 'B')
		LOG_PRINT_X("OSR2 ");
	 }
	 if(m_osver.dwMajorVersion == 4 && m_osver.dwMinorVersion == 10)
	 {
	     LOG_PRINT_X("Microsoft Windows 98 ");
	     if(m_osver.szCSDVersion[1] == 'A')
		LOG_PRINT_X("SE ");
	 }
	 if((m_osver.dwMajorVersion == 4) && (m_osver.dwMinorVersion == 90))
	 {
	     LOG_PRINT_X("Microsoft Windows Millennium Edition\n");
	 }
	 break;
      case VER_PLATFORM_WIN32s:
	 LOG_PRINT_X("Microsoft Win32s\n");
	 break;
   }
#endif
   return TRUE;
}
#endif
#if 0
void IRenderer::Init(int awidth, int aheight, const char* atitle, tRenderer rtb
,int argc, char *argv[]
)
{
	;

	//g_pApp->WorkDirectory();

	//if(g_pApp->screen==0)
	{
#ifdef _WIN32
	LOG_PRINT_X("Platform SDK Versions:\n");
/*
	//Minimum system required	Value for NTDDI_VERSION
	//Windows 7	NTDDI_WIN7
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows Server 2008	NTDDI_WS08\n");
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows Vista with Service Pack 1 (SP1)	NTDDI_VISTASP1\n");
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows Vista NTDDI_VISTA\n");
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows Server 2003 with Service Pack 2 (SP2) NTDDI_WS03SP2\n");
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows Server 2003 with Service Pack 1 (SP1) NTDDI_WS03SP1\n");
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows Server 2003	NTDDI_WS03\n");
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows XP with Service Pack 3 (SP3)	NTDDI_WINXPSP3\n");
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows XP with Service Pack 2 (SP2)	NTDDI_WINXPSP2\n");
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows XP with Service Pack 1 (SP1)	NTDDI_WINXPSP1\n");
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows XP	NTDDI_WINXP\n");
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows 2000 with Service Pack 4 (SP4)	NTDDI_WIN2KSP4\n");
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows 2000 with Service Pack 3 (SP3)	NTDDI_WIN2KSP3\n");
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows 2000 with Service Pack 2 (SP2)	NTDDI_WIN2KSP2\n");
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows 2000 with Service Pack 1 (SP1)	NTDDI_WIN2KSP1\n");
	if(NTDDI_VERSION>=0x0) LOG_PRINT_X("Windows 2000	NTDDI_WIN2K\n");
*/
	//The following tables describe the legacy macros in use by the Windows header files.
	//Minimum system required	Minimum value for _WIN32_WINNT and WINVER
	if(_WIN32_WINNT>=0x0601) LOG_PRINT_X("Windows 7\n");
	if(_WIN32_WINNT>=0x0600) LOG_PRINT_X("Windows Server 2008\n");
	if(_WIN32_WINNT>=0x0600) LOG_PRINT_X("Windows Vista\n");
	if(_WIN32_WINNT>=0x0502) LOG_PRINT_X("Windows Server 2003 with SP1, Windows XP with SP2\n");
	if(_WIN32_WINNT>=0x0501) LOG_PRINT_X("Windows Server 2003, Windows XP\n");
	if(_WIN32_WINNT>=0x0500) LOG_PRINT_X("Windows 2000\n");
	if(_WIN32_WINNT>=0x0400) LOG_PRINT_X("Windows NT 4.0\n");
/*
	if(_WIN32_WINDOWS>=0x0500) LOG_PRINT_X("Windows ME\n");
	if(_WIN32_WINDOWS>=0x0410) LOG_PRINT_X("Windows 98\n");
	if(_WIN32_WINDOWS>=0x0400) LOG_PRINT_X("Windows 95\n");
*/
	//Minimum version required	Minimum value of _WIN32_IE
	if(_WIN32_IE>=0x0800) LOG_PRINT_X("Internet Explorer 8.0\n");
	if(_WIN32_IE>=0x0700) LOG_PRINT_X("Internet Explorer 7.0\n");
	if(_WIN32_IE>=0x0603) LOG_PRINT_X("Internet Explorer 6.0 SP2\n");
	if(_WIN32_IE>=0x0601) LOG_PRINT_X("Internet Explorer 6.0 SP1\n");
	if(_WIN32_IE>=0x0600) LOG_PRINT_X("Internet Explorer 6.0\n");
	if(_WIN32_IE>=0x0550) LOG_PRINT_X("Internet Explorer 5.5\n");
	if(_WIN32_IE>=0x0501) LOG_PRINT_X("Internet Explorer 5.01\n");
	if(_WIN32_IE>=0x0500) LOG_PRINT_X("Internet Explorer 5.0, 5.0a, 5.0b\n");
	//WinVer();
#endif
;
//LOG_PRINT_X("ST:IRenderer::Init\n");
		IRenderer::GetRendererInstance( rtb,false,argc,argv);

	STX_Service::GetWindowInstance()->SetCaption(
		atitle
		);

#if 0
	s_pRenderer->SetviewportWidth(GetViewportWidth());
	s_pRenderer->SetviewportHeight(GetViewportHeight());
#elif 0//???defined(_WIN32)//???

	s_pRenderer->setViewport(0,0,
		width,
		height);
#endif
//???
;
	//g_pApp->initEffect();
	//	g_pApp->initShader();
//	g_pApp->SetWindow(STX_Service::GetWindowInstance()->GetHWND());
}
#if 0
	if(g_pFrmaework3==0)
	{
		g_pFrmaework3 = CFrmaework3::Instance();
	CFrmaework3::Instance()->Init (rtb);
	D3DXFROMWINEMATRIX mMatView;
	D3DXFROMWINEMatrixIdentity (&mMatView);
	SetTransform (TS_VIEW, &mMatView);
	}
#endif
	}
#endif
int constantTypeSizes[CONSTANT_TYPE_COUNT] =
{
	sizeof(float),
	sizeof(D3DXFROMWINEVECTOR2),
	sizeof(D3DXFROMWINEVECTOR3),
	sizeof(D3DXFROMWINEVECTOR4),
	sizeof(int),
	sizeof(int) * 2,
	sizeof(int) * 3,
	sizeof(int) * 4,
	sizeof(int),
	sizeof(int) * 2,
	sizeof(int) * 3,
	sizeof(int) * 4,
	sizeof(float)*8,
	sizeof(float)*12,
	sizeof(D3DXFROMWINEMATRIX),
}
;


	int IRenderer::ZERO;
	int IRenderer::ONE;
	int IRenderer::SRC_COLOR;
	int IRenderer::ONE_MINUS_SRC_COLOR;
	int IRenderer::DST_COLOR;
	int IRenderer::ONE_MINUS_DST_COLOR;
	int IRenderer::SRC_ALPHA;
	int IRenderer::ONE_MINUS_SRC_ALPHA;
	int IRenderer::DST_ALPHA;
	int IRenderer::ONE_MINUS_DST_ALPHA;
	int IRenderer::SRC_ALPHA_SATURATE;
	int IRenderer::BM_ADD;
	int IRenderer::BM_SUBTRACT;
	int IRenderer::BM_REVERSE_SUBTRACT;
	int IRenderer::BM_MIN;
	int IRenderer::BM_MAX;
	int IRenderer::NEVER;
	int IRenderer::LESS;
	int IRenderer::EQUAL;
	int IRenderer::LEQUAL;
	int IRenderer::GREATER;
	int IRenderer::NOTEQUAL;
	int IRenderer::GEQUAL;
	int IRenderer::ALWAYS;
	int IRenderer::KEEP;
	int IRenderer::SET_ZERO;
	int IRenderer::REPLACE;
	int IRenderer::INVERT;
	int IRenderer::INCR;
	int IRenderer::DECR;
	int IRenderer::INCR_SAT;
	int IRenderer::DECR_SAT;
	int IRenderer::CULL_NONE;
	int IRenderer::CULL_BACK;
	int IRenderer::CULL_FRONT;
	int IRenderer::SOLID;
	int IRenderer::WIREFRAME;
#if 0
	VArray <Quad> RendererHLSLCg::Quads;
	VArray <Cube> RendererHLSLCg::Cubes;
	VArray <FastBuffers> RendererHLSLCg::FastBuffersv;

	VArray <CTextureLoader> RendererHLSLCg::textureLoaders;
	VArray <CVertexBufferLoader> RendererHLSLCg::vertexBufferLoaders;
	VArray <CIndexBufferLoader> RendererHLSLCg::indexBufferLoaders;
#endif
	FontID RendererHLSLCg::defaultFont=-1;
	VArray <TexFont> RendererHLSLCg::fonts;

	float RendererHLSLCg::vv[16];
	float RendererHLSLCg::ww[16];

	std::vector<D3DXFROMWINEMATRIX> RendererHLSLCg::s_pV;
	std::vector<D3DXFROMWINEMATRIX> RendererHLSLCg::s_mvV;

#if 0
	int RendererHLSLCg::viewportWidth=0;
	int	RendererHLSLCg::viewportHeight=0;
	int RendererHLSLCg::viewportX=0;
	int	RendererHLSLCg::viewportY=0;
#else
	D3DFROMWINEVIEWPORT9 RendererHLSLCg::m_viewport;
#endif
	float RendererHLSLCg::m_fYaw;
	float RendererHLSLCg::m_fPitch;
	float RendererHLSLCg::m_fRoll;
	D3DXFROMWINEVECTOR3 RendererHLSLCg::m_eye;
	D3DXFROMWINEVECTOR3 RendererHLSLCg::m_center;
	D3DXFROMWINEVECTOR3 RendererHLSLCg::m_up;
	float IRenderer::m_fieldOfView;
    float IRenderer::m_aspectRatio;
    float IRenderer::m_zNear;
    float IRenderer::m_zFar;

	unsigned int IRenderer::m_nBeginScene=0;
	unsigned int IRenderer::m_nEndScene=0;

	void RendererHLSLCg::debug(void)
	{
		if(currentShader!=-1) 				LOG_PRINT_X("currentShader		=%x\n", currentShader);
		if(selectedShader!=-1) 				LOG_PRINT_X("selectedShader		=%x\n", selectedShader);
		if(currentVertexFormat!=-1) 			LOG_PRINT_X("currentVertexFormat		=%x\n", currentVertexFormat);
		if(selectedVertexFormat!=-1) 			LOG_PRINT_X("selectedVertexFormat		=%x\n", selectedVertexFormat);

		for(unsigned int i=0;i<MAX_VERTEXSTREAM;i++)
		{
			if(currentVertexBuffers[i]!=-1) 	LOG_PRINT_X("currentVertexBuffers[%d]	=%x\n", i, currentVertexBuffers[i]);
			if(selectedVertexBuffers[i]!=-1) 	LOG_PRINT_X("selectedVertexBuffers[%d]	=%x\n", i, selectedVertexBuffers[i]);
			if(currentOffsets[i]!=-1) 		LOG_PRINT_X("currentOffsets[%d]		=%x\n", i, currentOffsets[i]);
			if(selectedOffsets[i]!=-1) 		LOG_PRINT_X("selectedOffsets[%d]		=%x\n", i, selectedOffsets[i]);
		}
		
		if(currentIndexBuffer!=-1) 			LOG_PRINT_X("currentIndexBuffer		=%x\n", currentIndexBuffer);
		if(selectedIndexBuffer!=-1) 			LOG_PRINT_X("selectedIndexBuffer		=%x\n", selectedIndexBuffer);

		if(currentDepthState!=-1) 			LOG_PRINT_X("currentDepthState		=%x\n", currentDepthState);
		if(selectedDepthState!=-1) 			LOG_PRINT_X("selectedDepthState		=%x\n", selectedDepthState);
	
		if(currentBlendState!=-1) 			LOG_PRINT_X("currentBlendState		=%x\n", currentBlendState);
		if(selectedBlendState!=-1) 			LOG_PRINT_X("selectedBlendState		=%x\n", selectedBlendState);
		if(currentAlphaState!=-1) 			LOG_PRINT_X("currentAlphaState		=%x\n", currentAlphaState);
		if(selectedAlphaState!=-1) 			LOG_PRINT_X("selectedAlphaState		=%x\n", selectedAlphaState);
		if(currentRasterizerState!=-1) 			LOG_PRINT_X("currentRasterizerState	=%x\n", currentRasterizerState);
		if(selectedRasterizerState!=-1) 		LOG_PRINT_X("selectedRasterizerState	=%x\n", selectedRasterizerState);
/*
		unsigned int currentStencilRef, selectedStencilRef;
		unsigned int currentSampleMask, selectedSampleMask;
		TextureID currentColorRT[MAX_MRTS], currentDepthRT;
		int currentColorRTSlice[MAX_MRTS], currentDepthSlice;
		unsigned int nCurrentRenderTargets;
*/
	}

#if 0
	ObjectID RendererHLSLCg::addFastBuffer(ShaderID sid)
	{
		static int id=-1;
			FastBuffers mesh;
			mesh.init(sid);
			id=FastBuffersv.add(mesh);
		return id;
	}
	void RendererHLSLCg::renderFastBuffer(ObjectID id)
	{
		if(!(id<FastBuffersv.size()))return;
		FastBuffersv[id].render();
	}
	ObjectID RendererHLSLCg::addQuad(ShaderID sid)
	{
		static int id=-1;
			Quad quad;
			quad.init(sid);
			id=Quads.add(quad);
		return id;
	}
	void RendererHLSLCg::renderQuad(ObjectID id)
	{
		if(!(id<Quads.size()))return;
		Quads[id].render();
	}
	ObjectID RendererHLSLCg::addCube(ShaderID sid)
	{
		static int id=-1;
			Cube cube;
			cube.init(sid);
			id=Cubes.add(cube);
		return id;
	}
	void RendererHLSLCg::renderCube(ObjectID id)
	{
		if(!(id<Cubes.size()))return;
		Cubes[id].render();
	}
#endif
	void RendererHLSLCg::buildLookAtMatrix(D3DXFROMWINEVECTOR3 at, IInput* Input)
	{
#if 0
		D3DXFROMWINEVECTOR3 look(0,0,1);
		D3DXFROMWINEVECTOR3 up(0,1,0);
		D3DXFROMWINEMATRIX R;
	//R.rotateXYZ(Input->GetSpinY_L()/180.0f*D3DXFROMWINE_PI+1.5f*D3DXFROMWINE_PI,-Input->
	//	GetSpinX_L()/180.0f*D3DXFROMWINE_PI+1.5f*D3DXFROMWINE_PI,0.0f);

	m_fYaw=Input->GetSpinY_L()/180.0f*D3DXFROMWINE_PI+1.5f*D3DXFROMWINE_PI;
	m_fPitch=-Input->GetSpinX_L()/180.0f*D3DXFROMWINE_PI+1.5f*D3DXFROMWINE_PI;
	m_fRoll=0.0f;

	D3DXFROMWINEVECTOR3 look2=R*look;
	D3DXFROMWINEVECTOR3 up2=R*up;
	D3DXFROMWINEVECTOR3 look3=at+100.0f*look2;
	D3DXFROMWINEVECTOR3 up3=at+up2;
	//look3=normalize(look3);
	up3=normalize(up3);

	buildLookAtMatrix(
		at,
		look3,
		up3
		);
#elif 0
	mv=
	rotateXY(g_pApp->m_fSpinY_L/180.0f*D3DXFROMWINE_PI, -g_pApp->m_fSpinX_L/180.0f*D3DXFROMWINE_PI)
	//*translate(-at.x,-at.y,-at.z);
	*translate(-at);
	m_fYaw=g_pApp->m_fSpinY_L/180.0f*D3DXFROMWINE_PI;
	m_fPitch=-g_pApp->m_fSpinX_L/180.0f*D3DXFROMWINE_PI;
	m_fRoll=0.0f;
#endif
	}

unsigned int RendererHLSLCg::RenderTexture(TextureID id, D3DXFROMWINEVECTOR2 aposition, D3DXFROMWINEVECTOR2 asize, D3DXFROMWINEVECTOR2 texposition, D3DXFROMWINEVECTOR2 texsize)
{
	InitTex(texShader,texVF);
	D3DXFROMWINEVECTOR4 col(1.0f, 1.0f, 1.0f, 1.0f);
	TexVertex dest[4];
	float x=aposition.x;
	float y=aposition.y;
	dest[0].position = D3DXFROMWINEVECTOR2(x + asize.x, y);
	dest[0].texCoord = D3DXFROMWINEVECTOR2(texposition.x+texsize.x, texposition.y);
	dest[1].position = D3DXFROMWINEVECTOR2(x + asize.x, y + asize.y);
	dest[1].texCoord = D3DXFROMWINEVECTOR2(texposition.x+texsize.x, texposition.y+texsize.y);
	dest[2].position = D3DXFROMWINEVECTOR2(x, y);
	dest[2].texCoord = D3DXFROMWINEVECTOR2(texposition.x, texposition.y);
	dest[3].position = D3DXFROMWINEVECTOR2(x, y + asize.y);
	dest[3].texCoord = D3DXFROMWINEVECTOR2(texposition.x, texposition.y+texsize.y);
	setShader(GettexShader());
	setVertexFormat(texVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", col);

	setTexture("Base", id, (*GetTexture(id)).ss);

	setBlendState(GetblendSrcAlpha());
	setDepthState(GetnoDepthTest());
	return DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &dest[0], &dest[0], sizeof(TexVertex));
}

unsigned int RendererHLSLCg::drawTexturedVrtl(const Primitives primitives, TexVertex *vertices, const unsigned int nVertices, const TextureID texture, const BlendStateID blendState, const DepthStateID depthState, const D3DXFROMWINEVECTOR4 *color)
{
	//
	InitTex(texShader,texVF);
	D3DXFROMWINEVECTOR4 col = D3DXFROMWINEVECTOR4(1, 1, 1, 1);
	if (color) col = *color;
	setShader(GettexShader());
	setVertexFormat(texVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", col);

	setTexture("Base", texture, (*GetTexture(texture)).ss);

	setBlendState(blendState);
	setDepthState(depthState);
	setRasterizerState(cullBack);
	//setVertexFormat(GettexVF());
	int pc=getPrimitiveCount(primitives, nVertices);
	int pcR=getPrimitiveCountR(primitives, nVertices);
	//apply();
	return DrawPrimitiveUP(primitives,
		//pc
		 nVertices
		, vertices, vertices, 4*sizeof(float));
	//???setVertexFormat(-1);
}
unsigned int RendererHLSLCg::drawPlain(const Primitives primitives, D3DXFROMWINEVECTOR2 *vertices, const unsigned int nVertices, const BlendStateID blendState, const DepthStateID depthState, const D3DXFROMWINEVECTOR4 *color){
	//
	InitPlain(plainShader,plainVF);

	D3DXFROMWINEVECTOR4 col = D3DXFROMWINEVECTOR4(1, 1, 1, 1);
	if (color) col = *color;

	//reset();
	setShader(plainShader);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", col);
	setBlendState(blendState);
	setDepthState(depthState);
	setVertexFormat(plainVF);
	//apply();

	return DrawPrimitiveUP(
		primitives,
		getPrimitiveCount(primitives, nVertices),
		vertices,vertices, sizeof(D3DXFROMWINEVECTOR2));

}

unsigned int RendererHLSLCg::drawTextureVrtl(TextureID texture, int x, int y, int sx, int sy, bool InScene, float tx, float ty, float stx, float sty)
{
	//
	InitTex(texShader,texVF);

	float qx=x;
	float qy=y;
	float dx=sx;
	float dy=sy;

	std::vector<float> vertices(16);
	bool fq=((tx==0.0f)&&(ty==0.0f)&&(stx==1.0f)&&(sty==1.0f));
	{
		if(fq)
		{
			float verts2[] =
			{
				qx+dx,qy,1.0f, 0.0f
				,qx+dx,qy+dy,1.0f, 1.0f
				,qx,qy,0.0f, 0.0f
				,qx,qy+dy,0.0f, 1.0f
			}
			;
			stx_memcpy(&vertices[0],verts2,16*sizeof(float));
		}
		else
		{
			float verts2[] =
			{
				qx+dx,qy,tx+stx,ty
				,qx+dx,qy+dy,tx+stx,ty+sty
				,qx,qy,tx,ty
				,qx,qy+dy,tx,ty+sty
			}
			;
			stx_memcpy(&vertices[0],verts2,16*sizeof(float));

		}
	}
	if(!InScene)
	{
		BeginScene();
	}
	setShader(texShader);
	setVertexFormat(texVF);
	setTexture("Base", texture, (*GetTexture(texture)).ss);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setBlendState(GetblendSrcAlpha());
	setDepthState(GetnoDepthTest());
	setRasterizerState(cullBack);
	//apply();
//	DrawPrimitive(PRIM_TRIANGLE_STRIP, 0, 2);
	unsigned int ret=DrawPrimitiveUP(
		PRIM_TRIANGLE_STRIP,
		getPrimitiveCount(PRIM_TRIANGLE_STRIP, 4),

		&vertices[0], &vertices[0], 4*sizeof(float));
	if(!InScene)
	{
		EndScene();

	}
	return ret;
}

unsigned int RendererHLSLCg::drawFPS()
{
	char str[16];
    stx_snprintf(str,16, "%d", fps);
	drawText(str, 8, GetViewportHeight()-8*10.0f, 30, 38, defaultFont, linearClamp, blendSrcAlpha, noDepthTest);
	return 0;//???
}

std::map<std::string, TextureID> IRenderer::m_Texture_cache;

TextureID IRenderer::find_from_Texture_cache(const char* fileName)
{
	std::map<std::string, TextureID>::iterator it;
	it = m_Texture_cache.find(fileName);
	if (it != m_Texture_cache.end())
	{
		return it->second;
	}
	return -1;
}

void IRenderer::insert_to_Texture_cache(const char* fileName, TextureID id)
{
	std::map<std::string, TextureID>::iterator it;
	it = m_Texture_cache.find(fileName);
	if (it == m_Texture_cache.end())
		m_Texture_cache.insert ( std::pair<std::string,TextureID>(fileName, id) );
}

//define LOG_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__)
TextureID IRenderer::addImageLibTexture(const char *fileName0,
										  //const unsigned int mips
										  const bool useMipMaps
										  , const SamplerStateID samplerState, bool bFlipY, unsigned int flags, ubyte R, ubyte G, ubyte B_, ubyte A_)
{
LOG_FNLN;
	std::string f=fileName0;

	std::string f2=stx_convertpath(f);

    const char* fileName=f2.c_str();
    
	if(!stx_fileExists(fileName))
	{
		LOG_PRINT("Title: %s\n", m_title.c_str());
		LOG_PRINT("IRenderer::ImageLibTexturee:1181:\n");
		LOG_PRINT("\tLoading texture %s failed\n", fileName);
		DBG_HALT;
	}
    
	//LOG_FNLN_NONE;
	//LOG_PRINT_NONE("fileName=%s\n", fileName);

	TextureID id=TEXTURE_NONE;
	std::map<std::string, TextureID>::iterator it;
	it = m_Texture_cache.find(fileName);
	if (0)//it != m_Texture_cache.end())
	{
		id=it->second;
	}
	else
	{
	Image3 img;
		std::string path = fileName;
        	LOG_PRINT("IRenderer::addImageLibTexture:::%s\n", path.c_str());
		if (img.loadImageLibImage(path.c_str(), useMipMaps))
	{
		if(bFlipY) 
			img.flipY();
		/*
		#if 1
		std::string Alpha = "False";
		#ifndef USEXML
		if(!m_bLuaScript)
		{
			Alpha = _IRenderer_script.get<std::string>("Alpha");
		}
		#else
			if(_IRenderer_BodyElement)
				Alpha = _IRenderer_BodyHandle.FirstChild("Alpha")->ToElement()->Attribute("Value");
		#endif
		if(Alpha=="True")
			img.addAlphaChannel();
		#else
		unsigned int srcChannels = getChannelCount(img.getFormat());
		if (srcChannels==3)
			img.addAlphaChannel();
		#endif
		*/
		id=addTexture(&img,useMipMaps, samplerState, flags);
	}
	else
	{
		std::string path = stx_LowerUpper(fileName, false, false);
        	LOG_PRINT("IRenderer::addImageLibTexture:::%s\n", path.c_str());
		if (img.loadImageLibImage(path.c_str(), useMipMaps))
			id=addTexture(&img,useMipMaps, samplerState, flags);
		else
		{
			std::string path = stx_LowerUpper(fileName, true, true);
        		LOG_PRINT("IRenderer::addImageLibTexture:::%s\n", path.c_str());
			if (img.loadImageLibImage(path.c_str(), useMipMaps))
				id=addTexture(&img,useMipMaps, samplerState, flags);
			else
			{
				std::string path = stx_LowerUpper(fileName, true, false);
        			LOG_PRINT("IRenderer::addImageLibTexture:::%s\n", path.c_str());
				if (img.loadImageLibImage(path.c_str(), useMipMaps))
					id=addTexture(&img,useMipMaps, samplerState, flags);
				else
				{
					std::string path = stx_LowerUpper(fileName, false, true);
        				LOG_PRINT("IRenderer::addImageLibTexture:::%s\n", path.c_str());
					if (img.loadImageLibImage(path.c_str(), useMipMaps))
						id=addTexture(&img,useMipMaps, samplerState, flags);
					else
					{
						LOG_FNLN;
        					LOG_PRINT("Failed: Couldn't open \"%s\"\n", fileName);
        					id=TEXTURE_NONE;
					}
				}
			}
		}
	}}
	if (it == m_Texture_cache.end())
		m_Texture_cache.insert ( std::pair<std::string,TextureID>(fileName, id) );
	//if(IRenderer::m_bDebug) LOG_PRINT_NONE("TextureID:%d\n", id);
	
	#ifndef _MSC_VER
	Texture* pTex=GetTexture(id);
	pTex->m_sFileName=fileName;
	#endif

	return id;
}
TextureID RendererHLSLCg::addTexture(const char *fileName0, const bool useMipMaps, const SamplerStateID samplerState, unsigned int flags)
{
	TextureID id=IRenderer::GetRendererInstance()->addImageLibTexture(fileName0, useMipMaps,
				samplerState, flags);
	if(0)//(IRenderer::m_bDebug)
	{
		LOG_FNLN_NONE;
		LOG_PRINT_NONE("fileName=%s\n", fileName0);
		LOG_PRINT_NONE("TextureID id=%d\n", id);
	}
	return id;
}

#if 0
TextureID RendererHLSLCg::addCubemapVrtl(const char **fileNames, const bool useMipMaps, const SamplerStateID samplerState, const int nArraySlices, unsigned int flags)
{
	Image3 img;
	if (img.loadSlicedImage(fileNames, 0, nArraySlices))
	{
		//if (img.getFormat() == FORMAT_RGBE8) img.unpackImage();
		if (useMipMaps && img.getMipMapCount() <= 1) img.createMipMaps();
		return IRenderer::GetRendererInstance()->addTexture(&img,true, samplerState, flags);
	}
	else
	{
		char str[1024];
        int n = stx_snprintf(str,1024, "Couldn't open cubemap:\n");
		for (int i = 0; i < 6 * nArraySlices; i++)
		{
            n += stx_snprintf(str + n,1024, "%s\n", fileNames[i]);
		}
		ErrorMsg(str);
		return TEXTURE_NONE;
	}
}
TextureID RendererHLSLCg::addNormalMap(const char *fileName, const FORMAT destFormat, const bool useMipMaps, const SamplerStateID samplerState, float sZ, float mipMapScaleZ, unsigned int flags)
{
	Image3 img;
	unsigned int loadFlags = 0;
	if (!useMipMaps) loadFlags |= DONT_LOAD_MIPMAPS;
	if (img.loadImage(fileName, loadFlags))

	{


		if (useMipMaps && img.getMipMapCount() <= 1) img.createMipMaps();
		if (img.toNormalMap(destFormat, sZ, mipMapScaleZ))
		{
			return addTexture(&img,
				true, samplerState, flags);
		}
	}
	else
	{
		char str[256];
        stx_snprintf(str,256, "Couldn't open \"%s\"", fileName);
		ErrorMsg(str);
	}
	return TEXTURE_NONE;
}
#endif
ShaderID IRenderer::addShaderFromFile(	const char* fileName, 
                                            const char* vsMain, 
                                            const char* fsMain,
                             		    const char *defines,
                                            const unsigned int aflags)
{
	unsigned int flags=aflags;
	tShader shaderType=eHLSL_Shader;
	const char *extension = strrchr(fileName, '.');
	if (stx_strcasecmp(extension, ".glsl") == 0)
		shaderType=eGLSL_Shader;
	else if (stx_strcasecmp(extension, ".gles") == 0)
		shaderType=eGLES_Shader;
	flags=shaderType;
#if 0
	switch(shaderType)
	{
		case eGLSL_Shader:
			printf("eGLSL_Shader:\n");
			break;
		case eHLSL_Shader:
			printf("eHLSL_Shader:\n");
			break;
		case eGLES_Shader:
			printf("eGLES_Shader:\n");
			break;
	};
#endif
	std::string fn;
    if(fileName)
        fn=stx_convertpath(fileName);
	STX_PRINT("\nShader file:%s\n", fn.c_str());
	std::string contents, contents2;
	std::ifstream in(fn.c_str(), std::ios::in | std::ios::binary);
  	if (in)
    		contents=(std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));		
	ShaderID id=addShader(contents.c_str(), vsMain, fsMain, 0, flags);
	if(!stx_fileExists(fn.c_str()))
	{
		LOG_PRINT("Title: %s\n", m_title.c_str());
		LOG_PRINT("IRenderer::addShaderFromFile:1349:\n");
		LOG_PRINT("\tLoading shader %s failed\n", fn.c_str());
		DBG_HALT;
	}
	return id;
}
int RendererHLSLCg__formatSize[] =
	{
		sizeof(float), sizeof(float)/2, sizeof(ubyte)
	}
	;
int RendererHLSLCg::getFormatSize(const AttributeFormat format) const
{
	return RendererHLSLCg__formatSize[format];
}
FontID RendererHLSLCg::addFont(const char *textureFile, const char *fontFile, const SamplerStateID samplerState)
{/*
		static FileCache fileCache;
		int i=fileCache.begin(fontFile);//???
		if(i!=-1)
			return i;
		else
		{*/



	std::string f=fontFile;

        std::string fn=stx_convertpath(f);



	//LOG_PRINT_NONE("RendererHLSLCg::addFont:fopen=%s\n",fn.c_str());
        FILE *file = fopen(fn.c_str(), "rb");

	if (file == 0) return FONT_NONE;

	TexFont afont;
	TexFont* font=&afont;

	unsigned int version = 0;
    fread(&version, sizeof(version), 1, file);
    fread(font->chars, sizeof(font->chars), 1, file);
    fclose(file);

	//LOG_PRINT_NONE("textureFile=%s\n",textureFile);

		font->texture = IRenderer::GetRendererInstance()->addImageLibTexture(textureFile, false, samplerState);

	//LOG_PRINT_NONE("font->texture=%d\n",font->texture);

	if (font->texture == TEXTURE_NONE) return FONT_NONE;

	RendererHLSLCg::fonts.add(afont);

//	 fileCache.end(font->texture);//???
//		}
	return RendererHLSLCg::fonts.size()-1;
}

//CResourceReuseCache*	    RendererHLSLCg::g_pResourceReuseCache=0;
#if 0
#include <zip.h>

zip* APKArchive;
static void loadAPK (const char* apkPath) {
  LOG_PRINT_X("Loading APK %s\n", apkPath);
  APKArchive = zip_open(apkPath, 0, 0);
  if (APKArchive == 0) {
    LOG_PRINT_X("Error loading APK");
    return;
  }

  //Just for debug, print APK contents
  int numFiles = zip_get_num_files(APKArchive);
  for (int i=0; i<numFiles; i++) {
    const char* name = zip_get_name(APKArchive, i, 0);
    if (name == 0) {
      LOG_PRINT_X("Error reading zip file name at index %i : %s", zip_strerror(APKArchive));
      return;
    }
	if (name[stx_strlen(name)-1]=='/') continue;

	char buf[1024];
	stx_strlcpy(buf,name,1024);
    char* fc1=stx_strrchr(buf,'/');
    char* fc2=stx_strrchr(buf,'\\');
    char* fc=stx_max(fc1,fc2);
    buf[int(fc-buf)]='\0';
    if (fc)
	{
    stx_mkdirs(buf);
	}
    struct zip_stat st;
    zip_stat_init(&st);
    zip_stat(APKArchive, name, 0, &st);
    char *contents = new char[st.size];

        int err = 0;
    zip_file *f = zip_fopen(APKArchive, name, 0);

    FILE* ff=fopen(name, "wb");
    zip_fread(f, contents, st.size);
    fwrite(contents, st.size,1,ff);
    zip_fclose(f);
    fclose(ff);
	delete[] contents;

  }
}

int zxz(const char* apkPath)
{
    //Open the ZIP archive
    int err = 0;
    zip *z = zip_open(apkPath, 0, &err);

    //Search for the file of given name
    const char *name = "beetleship.png";
    struct zip_stat st;
    zip_stat_init(&st);
    zip_stat(z, name, 0, &st);

    //Alloc memory for its uncompressed contents
    char *contents = new char[st.size];

    //Read the compressed file
    zip_file *f = zip_fopen(z, "beetleship.png", 0);
    zip_fread(f, contents, st.size);
    zip_fclose(f);

    //And close the archive
    zip_close(z);
	delete[] contents;
}
#endif
int RendererHLSLCg::WorkDirectory(
	int argc, char *argv[])
{
	STX_INIT_SEGVCATCH;

	width=IRenderer::width;
	height=IRenderer::height;
	bpp=IRenderer::bpp;

#if 0
	if(miscParams)
	{

		if((*miscParams)["width"].length())
		{
		width=atoi((*miscParams)["width"].c_str());
		}
		if((*miscParams)["height"].length())
		{
		height=atoi((*miscParams)["height"].c_str());
		}
		if((*miscParams)["bpp"].length())
		{
		bpp=atoi((*miscParams)["bpp"].c_str());
		}
		if((*miscParams)["window"].length())
		{
			std::string str=((*miscParams)["window"]).c_str();
			char* c=(char*)str.c_str();
            char* s=stx_strchr(c,':');
			if(s)c[int(s-str.c_str())]='\0';
		m_dpy=(Display*)atoi(c);
		m_win=*(Window*)atoi(s+1);;

		}
for(int i=0;i<argc;i++)
{
	std::string buf=argv[i];

	buf=buf.substr(0,5);
	if(strcmp(buf.c_str(),"-width")==0)
		{
			std::string str=argv[i];
			char* c=(char*)str.c_str();
            char* s=stx_strchr(c,'=');
			width=atoi(s+1);;
		}
	buf=argv[i];
	buf=buf.substr(0,6);
	if(strcmp(buf.c_str(),"-height")==0)
		{
			std::string str=argv[i];
			char* c=(char*)str.c_str();
            char* s=stx_strchr(c,'=');
			width=atoi(s+1);;
		}
	buf=argv[i];
	buf=buf.substr(0,3);
	if(strcmp(buf.c_str(),"-bpp")==0)
		{
			std::string str=argv[i];
			char* c=(char*)str.c_str();
            char* s=stx_strchr(c,'=');
			width=atoi(s+1);;
		}
	buf=argv[i];
	buf=buf.substr(0,6);
	if(strcmp(buf.c_str(),"-window")==0)
		{
			std::string str=argv[i];
			char* c=(char*)str.c_str();
            char* s=stx_strchr(c,'=');
			if(s)
			{
			std::string str2=s+1;
			char* c=(char*)str2.c_str();
            char* s=stx_strchr(c,':');
			if(s)c[int(s-str2.c_str())]='\0';
		m_dpy=(Display*)atoi(c);
		m_win=*(Window*)atoi(s+1);;
			}
		}

}

	}
#endif

	STX_Service::GetWindowInstance()->Init(width,height,bpp,"");
#if !defined(WIN_PHONE_APP) && !defined(ANDROID) && !defined(IPHONE_SIMULATOR) && !defined(OS_IPHONE)
	if(
#if defined(_WIN32)
		STX_Service::GetWindowInstance()->GetHWND()==0
#elif defined(QT_BUILD) || defined(SDL_2_0_5_BUILD)
		0
#else
		STX_Service::GetWindowInstance()->GetDisplay()==0
#endif
		){


	}
#endif
	return 0;
}

void RendererHLSLCg::RenderTexVrtl(TextureID id, D3DXFROMWINEVECTOR2 aposition, D3DXFROMWINEVECTOR2 asize, D3DXFROMWINEVECTOR2 texposition, D3DXFROMWINEVECTOR2 texsize)
{
	static ShaderID shd = IRenderer::GetRendererInstance()->addShaderFromFile("/Texture/Texture.hlsl", "main", "main", 0, eHLSL_Shader);
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	static VertexFormatID vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	D3DXFROMWINEVECTOR4 col(1.0f, 1.0f, 1.0f, 1.0f);
	TexVertex dest[4];
	float x=aposition.x;
	float y=aposition.y;
	dest[0].position = D3DXFROMWINEVECTOR2(x + asize.x, y);
	dest[0].texCoord = D3DXFROMWINEVECTOR2(texposition.x+texsize.x, texposition.y);
	dest[1].position = D3DXFROMWINEVECTOR2(x + asize.x, y + asize.y);
	dest[1].texCoord = D3DXFROMWINEVECTOR2(texposition.x+texsize.x, texposition.y+texsize.y);
	dest[2].position = D3DXFROMWINEVECTOR2(x, y);
	dest[2].texCoord = D3DXFROMWINEVECTOR2(texposition.x, texposition.y);
	dest[3].position = D3DXFROMWINEVECTOR2(x, y + asize.y);
	dest[3].texCoord = D3DXFROMWINEVECTOR2(texposition.x, texposition.y+texsize.y);
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias", IRenderer::GetRendererInstance()->GetscaleBias2D());
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", col);
	IRenderer::GetRendererInstance()->setTexture("Base", id);
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &dest[0], &dest[0], sizeof(TexVertex));
}

std::vector<ShaderID> stx_Variables__m_vShaders;
std::vector<stx_Effect::stx_Variable> stx_Variables__m_vUniforms;
unsigned int stx_Variables::GetUniformNElements(ShaderID id, const std::string& name)
{
	for(unsigned int i=0;i<stx_Variables__m_vShaders.size();i++)
		if(stx_Variables__m_vShaders[i]==id)
			return stx_Variables__m_vUniforms[i].NElements;
	return -1;
}
unsigned int stx_Variables::GetUniformType(ShaderID id, const std::string& name)
{
	for(unsigned int i=0;i<stx_Variables__m_vShaders.size();i++)
		if(stx_Variables__m_vShaders[i]==id)
			return stx_Variables__m_vUniforms[i].Type;
	return -1;
}
unsigned int stx_Variables::GetUniformLocation(ShaderID id, const std::string& name)
{
	for(unsigned int i=0;i<stx_Variables__m_vShaders.size();i++)
		if(stx_Variables__m_vShaders[i]==id)
			return stx_Variables__m_vUniforms[i].m_iLocation;
	return -1;
}
void stx_Variables::AddUniform(ShaderID id, const std::string& name, unsigned int nElements, unsigned int type)
{
	for(unsigned int i=0;i<stx_Variables__m_vUniforms.size();i++)
		if(stx_Variables__m_vUniforms[i].m_sName==name)
			return;
	stx_Variables__m_vShaders.push_back(id);
	stx_Effect::stx_Variable v;
        v.m_sName=name;
        stx_Variables__m_vUniforms.push_back(v);
}
void stx_Variables::AddSampler(ShaderID id, const std::string& name, unsigned int location)
{
	for(unsigned int i=0;i<stx_Variables__m_vUniforms.size();i++)
		if(stx_Variables__m_vUniforms[i].m_sName==name)
			return;
	stx_Variables__m_vShaders.push_back(id);
	stx_Effect::stx_Variable v;
        v.m_sName=name;
        v.m_texID=-1;
        v.m_iLocation=location;
        stx_Variables__m_vUniforms.push_back(v);
}

RendererHLSLCg::RendererHLSLCg(int argc, char *argv[]):IRenderer()
{
	//LOG_PRINT_X("ST:RendererHLSLCg::RendererHLSLCg\n");
	WorkDirectory(argc,argv);
    // Create the texture reuse cache
   //if(g_pResourceReuseCache==0)
//	   g_pResourceReuseCache=new CResourceReuseCache(this  );
//    if( !g_pResourceReuseCache )
//	return E_OUTOFMEMORY;

	nImageUnits = 1;
	nMRTs = 1;
	maxAnisotropic = 1;

	m_viewport=D3DFROMWINEVIEWPORT9();
	m_viewport.Width  = width;
	m_viewport.Height = height;
	m_viewport.X = 0;
	m_viewport.Y = 0;
	m_viewport.MinZ = 0.0f;
	m_viewport.MaxZ = 1.0f;

	D3DXFROMWINEMatrixIdentity((D3DXFROMWINEMATRIX*)pp);
	D3DXFROMWINEMatrixIdentity((D3DXFROMWINEMATRIX*)vv);
	D3DXFROMWINEMatrixIdentity((D3DXFROMWINEMATRIX*)ww);
	fontBuffer.resize(0);
	fontBufferCount = 0;
	#if 0//???def PROFILE
	GLprofileFrame = false;
	GLprofileReset = true;
	GLprofileString[0] = '\0';
	#endif
/*
	texShader = SHADER_NONE;
			texVF = VF_NONE;
*/
	/*
		FormatDesc pcAttribs[] = {
		0,TYPE_VERTEX,   FORMAT_FLOAT,    3,
		0,TYPE_TEXCOORD,    FORMAT_FLOAT, 4,
	};
	FormatDesc pctAttribs[] = {
		0,TYPE_VERTEX,   FORMAT_FLOAT,    3,
		0,TYPE_TEXCOORD,    FORMAT_FLOAT, 4,
		0,TYPE_TEXCOORD, FORMAT_FLOAT,    2,
	};

	pcVertexFormat  = addVertexFormat(pcAttribs,  elementsOf(pcAttribs));
	pctVertexFormat = addVertexFormat(pctAttribs, elementsOf(pctAttribs));
*/

	m_VertexPointer=0;
   m_NormalPointer=0;
   m_TexCoordPointer=0;
   m_VertexOff=0;
   m_NormalOff=0;
   m_TexCoordOff=0;
   m_VertexW=0;
   m_NormalW=0;
   m_TexCoordW=0;

	//ililuInit();
//	m_pInput=0;

	RendererHLSLCg::vb=-1;
	RendererHLSLCg::ib=-1;
		RendererHLSLCg::defaultFont=-1;
		RendererHLSLCg::fonts.clear();
		RendererHLSLCg::fonts.empty();

#if 0
	Entity2dManager=new FW3_Entity2dManager();
	Entity3dManager=new FW3_Entity3dManager();
	AnimationManager=new FW3_AnimationManager();
	SurfaceManager=new FW3_SurfaceManager();
	FontManager=new FW3_FontManager();
	LightManager=new FW3_LightManager();
	MeshManager=new FW3_MeshManager();
#endif
	currentVertexFormat=-1;
	currentShader=-1;
	selectedShader=-1;

	currentPixelShader=-1;
	selectedPixelShader=-1;
	currentVertexShader=-1;
	selectedVertexShader=-1;
	currentGeometryShader=-1;
	selectedGeometryShader=-1;
	currentHullShader=-1;
	selectedHullShader=-1;
	currentDomainShader=-1;
	selectedDomainShader=-1;
	currentComputeShader=-1;
	selectedComputeShader=-1;

	currentVertexFormat=-1;
	selectedVertexFormat=-1;
	currentIndexBuffer=-1;
	selectedIndexBuffer=-1;
	currentDepthState=-1;
	selectedDepthState=-1;
	currentBlendState=-1;
	selectedBlendState=-1;
	currentAlphaState=-1;
	selectedAlphaState=-1;
	currentRasterizerState=-1;
	selectedRasterizerState=-1;
	currentDepthRT=-1;
	currentStencilRef=0;
	selectedStencilRef=0;
	currentSampleMask=0;
	selectedSampleMask=0;	
	for (unsigned int i = 0; i < MAX_VERTEXSTREAM; i++)
	{
		currentVertexBuffers[i]=-1;
		currentOffsets[i]=-1;
		activeVertexFormat[i]=-1;
		selectedVertexBuffers[i]=-1;
		selectedOffsets[i]=-1;
	}	
	for (unsigned int i = 0; i < MAX_MRTS; i++)
	{
		currentColorRT[i]=-1;
		currentColorRTSlice[i]=0;
	}	
	for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++)
	{
        	currentTextures[i]=-1;
        	textureLod[i]=0.0f;
		selectedTextures[i]=-1;
	}	
	for (unsigned int i = 0; i < MAX_SAMPLERSTATE; i++)
	{
        	currentSamplerStates[i]=-1;
		selectedSamplerStates[i]=-1;
	}
	currentDepthSlice=0;
	unsigned int nCurrentRenderTargets=0;

        currentMask=0;
        currentBlendEnable=false;
        currentAlphaEnable=false;
        currentAlphaToCoverageEnable=false;

        currentDepthFunc=0;
        currentDepthTestEnable=false;
        currentDepthWriteEnable=false;

        currentMultiSampleEnable=false;
        currentScissorEnable=false;
        currentCullMode=0;
        currentFillMode=0;

	blendSrcAlpha=-1;
	blendOneOne=-1;
	blendOneOneReverseSubstract=-1;
	enableAlpha=-1;
	disableAlpha=-1;
	noDepthTest=-1;
	noDepthWrite=-1;
	//depthTest=-1;
	//depthWrite=-1;
	cullNone=-1;
	cullBack=-1;
	cullFront=-1;
	fillSolid=-1;
	fillWireFrame=-1;

	scaleBias2D=D3DXFROMWINEVECTOR4(0.0f,0.0f,0.0f,0.0f);

	texShader = -1;
	texVF = -1;
	texShader3 = -1;
	texVF3 = -1;
	plainShader = -1;
	plainVF = -1;
	plainShader3 = -1;
	plainVF3 = -1;
}
RendererHLSLCg::~RendererHLSLCg()
{
	fontBuffer.resize(0);
//	delete g_pResourceReuseCache;

}
	D3DXFROMWINEMATRIX RendererHLSLCg::Getpp()
	{
		return pp;
	}
	D3DXFROMWINEMATRIX RendererHLSLCg::Getvv()
	{
		return vv;
	}

	D3DXFROMWINEMATRIX RendererHLSLCg::Getww()
	{
		return ww;
	}

void RendererHLSLCg::reset0ToDefaults()
{
	currentShader = SHADER_NONE;
	currentVertexFormat = VF_NONE;
	for (unsigned int i = 0; i < MAX_VERTEXSTREAM; i++)
	{
		currentVertexBuffers[i] = VB_NONE;
		currentOffsets[i] = 0;
	}
	currentIndexBuffer = IB_NONE;
	currentDepthState = DS_NONE;
	currentStencilRef = 0;
	currentBlendState = BS_NONE;
	currentAlphaState = -1;
	currentSampleMask = ~0;
	currentRasterizerState = RS_NONE;
	currentColorRT[0] = FB_COLOR;
	currentColorRTSlice[0] = NO_SLICE;
	for (unsigned int i = 1; i < nMRTs; i++)
	{
		currentColorRT[i] = TEXTURE_NONE;
		currentColorRTSlice[i] = NO_SLICE;
	}
	#if 1
	for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++)
		currentTextures[i] = TEXTURE_NONE;
	#if 0
	setResetFlag(0);
	#endif
	#endif

	currentDepthRT = FB_DEPTH;
	currentDepthSlice = NO_SLICE;
	nCurrentRenderTargets = 1;
	reset(RESET_ALL);//???
	#if 0
	setResetFlag(0);
	#endif
	#ifdef DEBUG
	wasReset = true;
	#endif
	nearestClamp = addSamplerState(NEAREST, CLAMP, CLAMP, CLAMP);
	linearClamp = addSamplerState(LINEAR, CLAMP, CLAMP, CLAMP);
	bilinearClamp = addSamplerState(BILINEAR, CLAMP, CLAMP, CLAMP);
	trilinearClamp = addSamplerState(TRILINEAR, CLAMP, CLAMP, CLAMP);
	nearest = addSamplerState(NEAREST, WRAP, WRAP, WRAP);
	linearAniso = addSamplerState(LINEAR, WRAP, WRAP, WRAP);
	bilinearAniso = addSamplerState(BILINEAR, WRAP, WRAP, WRAP);
	trilinearAniso = addSamplerState(TRILINEAR, WRAP, WRAP, WRAP);

	noDepthTest  = addDepthState(false, false);
	noDepthWrite = addDepthState(true,  false);
	cullNone  = addRasterizerState(CULL_NONE);
	cullBack  = addRasterizerState(CULL_BACK);
	cullFront = addRasterizerState(CULL_FRONT);
	fillSolid = addRasterizerState(CULL_FRONT,SOLID);
	fillWireFrame = addRasterizerState(CULL_FRONT,WIREFRAME);
	blendSrcAlpha = -1;
	blendOneOne = -1;
	blendOneOneReverseSubstract = -1;
	//ifndef USE_D3D10_1
//if(!dynamic_cast<RendererHLSLD3D11*>(IRenderer::GetRendererInstance()))//???
{
	blendSrcAlpha = addBlendState(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	blendOneOne = addBlendState(ONE, ONE,BM_ADD);
	blendOneOneReverseSubstract=addBlendState(ONE, ONE, BM_REVERSE_SUBTRACT);
}
    enableAlpha = addAlphaState();
    //disableAlpha = addAlphaState(false);
	//endif
}

#include <Framework3/rend_device_state.h>
#if 0//#ifdef _MSC_VER
void RendererHLSLCg::apply()
{
#if 0//def DEBUG
	if (!wasReset){
		outputDebugString("Warning: reset() not called before apply()");
	}
	wasReset = false;
#endif
	if (selectedShader != DONTCARE){
		changeShader(selectedShader);
		applyConstants();
	}

/*	for (uint i = 0; i < nImageUnits; i++){
		if (selectedTextures[i] != DONTCARE) changeTexture(i, selectedTextures[i]);
	}*/
	applyTextures();

/*	for (uint i = 0; i < MAX_SAMPLERSTATE; i++){
		if (selectedSamplerStates[i] != DONTCARE) changeSamplerState(i, selectedSamplerStates[i]);
	}*/
	applySamplerStates();

	changeVertexFormat(selectedVertexFormat);
	for (uint i = 0; i < MAX_VERTEXSTREAM; i++){
		if (selectedVertexBuffers[i] != DONTCARE){
			changeVertexBuffer(i, selectedVertexBuffers[i], selectedOffsets[i]);
		}
	}
	if (selectedIndexBuffer != DONTCARE) changeIndexBuffer(selectedIndexBuffer);

	if (selectedDepthState != DONTCARE){
		changeDepthState(selectedDepthState, selectedStencilRef);
	}
	if (selectedBlendState != DONTCARE){
		changeBlendState(selectedBlendState, selectedSampleMask);
	}
	if (selectedRasterizerState != DONTCARE) changeRasterizerState(selectedRasterizerState);

//	reset();
}
#elif !defined(_MSC_VER)
void RendererHLSLCg::apply()
{
	#ifdef DEBUG
	//debug();
	if (!wasReset)
	{
		LOG_PRINT_X("%s", "Warning: reset() not called before apply()");
	}
	wasReset = false;
	#endif
/*
	for (unsigned int i = 0; i < MAX_VERTEXSTREAM; i++)
	{
		currentVertexBuffers[i]=-1;
		currentOffsets[i]=-1;
		activeVertexFormat[i]=-1;
		selectedVertexBuffers[i]=-1;
		selectedOffsets[i]=-1;
	}	
	for (unsigned int i = 0; i < MAX_MRTS; i++)
	{
		currentColorRT[i]=-1;
		currentColorRTSlice[i]=0;
        	currentTextures[i]=-1;
        	currentSamplerStates[i]=-1;
        	textureLod[i]=0.0f;
		selectedTextures[i]=-1;
		selectedSamplerStates[i]=-1;
	}
*/
	viewport(m_viewport.X,m_viewport.Y,m_viewport.Width,m_viewport.Height);
	//if (selectedShader != DONTCARE) // -2 // ???

	#if 0
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("selectedShader:%x\n", selectedShader);
	LOG_PRINT_NONE("currentShader:%x\n", currentShader);
	LOG_PRINT_NONE("selectedVertexFormat:%x\n", selectedVertexFormat);
	LOG_PRINT_NONE("currentVertexFormat:%x\n", currentVertexFormat);
	#if 0
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("selectedVertexFormat:%x\n", selectedVertexFormat);
	LOG_PRINT_NONE("currentVertexFormat:%x\n", currentVertexFormat);
	LOG_PRINT_NONE("selectedIndexBuffer:%x\n", selectedIndexBuffer);
	LOG_PRINT_NONE("currentIndexBuffer:%x\n", currentIndexBuffer);

	VertexBufferID currentVertexBuffers[MAX_VERTEXSTREAM], selectedVertexBuffers[MAX_VERTEXSTREAM];

        TextureID currentTextures[MAX_TEXTUREUNIT], selectedTextures[MAX_TEXTUREUNIT];
        SamplerStateID currentSamplerStates[MAX_SAMPLERSTATE], selectedSamplerStates[MAX_SAMPLERSTATE];
        
        VertexFormatID activeVertexFormat[MAX_VERTEXSTREAM];

	DepthStateID currentDepthState, selectedDepthState;
	BlendStateID currentBlendState, selectedBlendState;
	AlphaStateID currentAlphaState, selectedAlphaState;
	RasterizerStateID currentRasterizerState, selectedRasterizerState;

	TextureID currentColorRT[MAX_MRTS], currentDepthRT;

	BlendStateID blendSrcAlpha,blendOneOne,blendOneOneReverseSubstract;

	AlphaStateID enableAlpha,disableAlpha;

	#endif
	#endif

	static ShaderID _Shader=-1;
	static VertexFormatID _VertexFormat=-1;
	static IndexBufferID _IndexBuffer=-1;
	static DepthStateID _DepthState=-1;
	static unsigned int _StencilRef=0;
	static BlendStateID _BlendState=-1;
	static AlphaStateID _AlphaState=-1;
	static unsigned int _SampleMask=0;
	static RasterizerStateID _RasterizerState=-1;
	static bool _Inited=false;
	static VertexBufferID _VertexBuffers[MAX_VERTEXSTREAM];
	static intptr _Offsets[MAX_VERTEXSTREAM];
    static TextureID _Textures[MAX_TEXTUREUNIT];
    static float _textureLod[MAX_TEXTUREUNIT];
    static SamplerStateID _SamplerStates[MAX_SAMPLERSTATE];
    if(!_Inited)
    {
	for (unsigned int i = 0; i < MAX_VERTEXSTREAM; i++)
	{
		_VertexBuffers[i]=-1;
		_Offsets[i]=-1;
	}	
	for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++)
	{
        	_Textures[i]=-1;
        	_textureLod[i]=0.0f;
	}	
	for (unsigned int i = 0; i < MAX_SAMPLERSTATE; i++)
	{
        	_SamplerStates[i]=-1;
	} /*
	for (unsigned int i = 0; i < MAX_MRTS; i++)
	{
		currentColorRT[i]=-1;
		currentColorRTSlice[i]=0;
	}	*/
        _Inited=true;
    }

LOG_FNLN;
LOG_PRINT("selectedShader:%x\n", selectedShader);
LOG_PRINT("selectedVertexFormat:%x\n", selectedVertexFormat);
LOG_PRINT("_Shader:%x\n", _Shader);
LOG_PRINT("_VertexFormat:%x\n", _VertexFormat);

	if(selectedShader>-1)
		_Shader=selectedShader;
	else
		_Shader=-1;
	if(selectedVertexFormat>-1)
		_VertexFormat=selectedVertexFormat;
	else
		_VertexFormat=-1;
	if(selectedIndexBuffer>-1)
		_IndexBuffer=selectedIndexBuffer;
	else
		_IndexBuffer=-1;
	if(selectedDepthState>-1)
    {
		_DepthState=selectedDepthState;
        _StencilRef=selectedStencilRef;
    }
	else
    {
		_DepthState=-1;
	    _StencilRef=0;
    }
	if(selectedBlendState>-1)
    {
		_BlendState=selectedBlendState;
	    _SampleMask=selectedSampleMask;
    }
	else
    {
		_BlendState=-1;
	    _SampleMask=0;
    }
	if(selectedAlphaState>-1)
        _AlphaState=selectedAlphaState;
	else
		_AlphaState=-1;
	if(selectedRasterizerState>-1)
		_RasterizerState=selectedRasterizerState;
	else
		_RasterizerState=-1;

	for (unsigned int i = 0; i < MAX_VERTEXSTREAM; i++)
	{
        if(selectedVertexBuffers[i]>-1)
        {
		    _VertexBuffers[i]=selectedVertexBuffers[i];
		    _Offsets[i]=selectedOffsets[i];
        }
        else
        {
		    _VertexBuffers[i]=-1;
		    _Offsets[i]=-1;
        }
	}	
	for (unsigned int i = 0; i < MAX_TEXTUREUNIT; i++)
	{
        if(selectedTextures[i]>-1)
        {
		    _Textures[i]=selectedTextures[i];
        	_textureLod[i]=textureLod[i];
        }
        else
        {
		    _Textures[i]=-1;
        	_textureLod[i]=0.0f;
        }
	}	
	for (unsigned int i = 0; i < MAX_SAMPLERSTATE; i++)
	{
        if(selectedSamplerStates[i]>-1)
            _SamplerStates[i]=selectedSamplerStates[i];
        else
            _SamplerStates[i]=-1;
	}

LOG_FNLN;
LOG_PRINT("selectedShader:%x\n", selectedShader);
LOG_PRINT("selectedVertexFormat:%x\n", selectedVertexFormat);
LOG_PRINT("_Shader:%x\n", _Shader);
LOG_PRINT("_VertexFormat:%x\n", _VertexFormat);
#if 0 // ???
		_Shader=0; // ???
 		_VertexFormat=0; // ???
#endif
	#ifdef _MSC_VER
	if(_Shader > -1)
		changeShader(_Shader);
	if(_VertexFormat > -1)
		changeVertexFormat(_VertexFormat);
	applyConstants();
	applyTextures();
	applySamplerStates();
	for (unsigned int i = 0; i < MAX_VERTEXSTREAM; i++)
	{
		//if (selectedVertexBuffers[i] != currentVertexBuffers[i]) // != DONTCARE)
		//if (_VertexBuffers[i] != DONTCARE)
		if (_VertexBuffers[i] > -1)
			changeVertexBuffer(i, _VertexBuffers[i], _Offsets[i]);
	}
	if (_IndexBuffer > -1)
		changeIndexBuffer(_IndexBuffer);
	if (_DepthState > -1)
		changeDepthState(_DepthState, _StencilRef);
	if (_AlphaState > -1)
		BlendingAlphaTest::GetInstance()->SetAlphaTest(true);
	if (_BlendState > -1)
		changeBlendState(_BlendState, _SampleMask);
	if (_RasterizerState > -1)
		changeRasterizerState(_RasterizerState);
	return;
	#endif
	if (_Shader > -1)
	{
LOG_FNLN;
		#ifndef __X7__
			if(m_bDebug)
					LOG_FNLN_NONE;
		#ifndef _MSC_VER
		changeShader(_Shader);
		#if 0
		changePixelShader(selectedPixelShader);
		changeVertexShader(selectedVertexShader);
		changeGeometryShader(selectedGeometryShader);
		changeHullShader(selectedHullShader);
		changeDomainShader(selectedDomainShader);
		changeComputeShader(selectedComputeShader);
		#endif
		#ifndef GLSL1_1
		if(_VertexFormat > -1)
			changeVertexFormat(_VertexFormat);
		#endif
		#endif
		#endif
			if(m_bDebug)
					LOG_FNLN_NONE;
		applyConstants();
			if(m_bDebug)
					LOG_FNLN_NONE;
	//}
#if 1//ndef _MSC_VER
	applyTextures();
	applySamplerStates();
#endif
		#ifndef __X7__
		#ifdef GLSL1_1
		if(_VertexFormat > -1)
			changeVertexFormat(_VertexFormat);
		#endif
	for (unsigned int i = 0; i < MAX_VERTEXSTREAM; i++)
	{
	#if 0
	LOG_FNLN_NONE;
	if(selectedVertexBuffers[i]>-1) LOG_PRINT_NONE("selectedVertexBuffers[%d]:%x\n", i, selectedVertexBuffers[i]);
	if(currentVertexBuffers[i]>-1) LOG_PRINT_NONE("currentVertexBuffers[%d]:%x\n", i, currentVertexBuffers[i]);
	if(selectedOffsets[i]) LOG_PRINT_NONE("selectedOffsets[%d]:%x\n", i, selectedOffsets[i]);
	if(currentOffsets[i]) LOG_PRINT_NONE("currentOffsets[%d]:%x\n", i, currentOffsets[i]);
	#endif
#if 1//ndef _MSC_VER
		//if (selectedVertexBuffers[i] != currentVertexBuffers[i]) // != DONTCARE)
		//if (_VertexBuffers[i] != DONTCARE)
		if (_VertexBuffers[i] != -1)
		{
LOG_PRINT("%d, _VertexBuffers[%d], _Offsets[%d]\n", i, i, _VertexBuffers[i], i, _Offsets[i]);
			changeVertexBuffer(i, _VertexBuffers[i], _Offsets[i]);
		}
#endif
	}
		#endif
	/*
{
	int nvertices=0;//???
	int s=0;
	if(m_VertexPointer) s+=m_VertexW*sizeof(float);
	if(m_NormalPointer) s+=m_NormalW*sizeof(float);
	if(m_TexCoordPointer) s+=m_TexCoordW*sizeof(float);
	BYTE* vertices=new BYTE[nvertices*s];
	for(unsigned int i=0;i<nvertices;i++)
	{
		int o=0;
		if(m_VertexPointer)
		{
		stx_memcpy(&vertices[i*s+o],&((BYTE*)m_VertexPointer)[i*s],m_VertexW*sizeof(float));
		o+=m_VertexW*sizeof(float);
		}
		if(m_NormalPointer)
		{
		stx_memcpy(&vertices[i*s+o],&((BYTE*)m_NormalPointer)[i*s],m_VertexW*sizeof(float));
		o+=m_NormalW*sizeof(float);
		}
		if(m_TexCoordPointer)
		{
		stx_memcpy(&vertices[i*s+o],&((BYTE*)m_TexCoordPointer)[i*s],m_VertexW*sizeof(float));
		o+=m_TexCoordW*sizeof(float);
		}

	}
	static VertexBufferID vb=-1;
	vb=addVertexBuffer(nvertices*s,STATIC,vertices,vb);
	delete[] vertices;

	changeVertexBuffer(0, vb, s);
		m_VertexPointer=0;
   m_NormalPointer=0;
   m_TexCoordPointer=0;
   m_VertexOff=0;
   m_NormalOff=0;
   m_TexCoordOff=0;
   m_VertexW=0;
   m_NormalW=0;
   m_TexCoordW=0;
}
	*/
		#ifndef __X7__
	if (_IndexBuffer != DONTCARE) changeIndexBuffer(_IndexBuffer);
			//LOG_FNLN_NONE;
	if (_DepthState != DONTCARE)
	{
			//LOG_FNLN_NONE;

		changeDepthState(_DepthState, _StencilRef);
			//LOG_FNLN_NONE;
	}
			//LOG_FNLN_NONE;
 #if 1
	if (_AlphaState != DONTCARE)
	{
        //changeAlphaState(selectedAlphaState);
        BlendingAlphaTest::GetInstance()->SetAlphaTest(true);
	}
	if (_BlendState != DONTCARE)
	{
        changeBlendState(_BlendState, _SampleMask);
        //BlendingAlphaTest::GetInstance()->SetBlending( true );
	}
#endif
	if (_RasterizerState != DONTCARE) changeRasterizerState(_RasterizerState);
		#endif
	}
}
#else
void RendererHLSLCg::apply()
{
	#ifdef DEBUG
	//debug();
	if (!wasReset)
	{
		LOG_PRINT_X("%s", "Warning: reset() not called before apply()");
	}
	wasReset = false;
	#endif
/*
	for (unsigned int i = 0; i < MAX_VERTEXSTREAM; i++)
	{
		currentVertexBuffers[i]=-1;
		currentOffsets[i]=-1;
		activeVertexFormat[i]=-1;
		selectedVertexBuffers[i]=-1;
		selectedOffsets[i]=-1;
	}	
	for (unsigned int i = 0; i < MAX_MRTS; i++)
	{
		currentColorRT[i]=-1;
		currentColorRTSlice[i]=0;
        	currentTextures[i]=-1;
        	currentSamplerStates[i]=-1;
        	textureLod[i]=0.0f;
		selectedTextures[i]=-1;
		selectedSamplerStates[i]=-1;
	}
*/
	viewport(m_viewport.X,m_viewport.Y,m_viewport.Width,m_viewport.Height);
	//if (selectedShader != DONTCARE) // -2 // ???

	#if 0
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("selectedShader:%x\n", selectedShader);
	LOG_PRINT_NONE("currentShader:%x\n", currentShader);
	LOG_PRINT_NONE("selectedVertexFormat:%x\n", selectedVertexFormat);
	LOG_PRINT_NONE("currentVertexFormat:%x\n", currentVertexFormat);
	#if 0
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("selectedVertexFormat:%x\n", selectedVertexFormat);
	LOG_PRINT_NONE("currentVertexFormat:%x\n", currentVertexFormat);
	LOG_PRINT_NONE("selectedIndexBuffer:%x\n", selectedIndexBuffer);
	LOG_PRINT_NONE("currentIndexBuffer:%x\n", currentIndexBuffer);

	VertexBufferID currentVertexBuffers[MAX_VERTEXSTREAM], selectedVertexBuffers[MAX_VERTEXSTREAM];

        TextureID currentTextures[MAX_TEXTUREUNIT], selectedTextures[MAX_TEXTUREUNIT];
        SamplerStateID currentSamplerStates[MAX_SAMPLERSTATE], selectedSamplerStates[MAX_SAMPLERSTATE];
        
        VertexFormatID activeVertexFormat[MAX_VERTEXSTREAM];

	DepthStateID currentDepthState, selectedDepthState;
	BlendStateID currentBlendState, selectedBlendState;
	AlphaStateID currentAlphaState, selectedAlphaState;
	RasterizerStateID currentRasterizerState, selectedRasterizerState;

	TextureID currentColorRT[MAX_MRTS], currentDepthRT;

	BlendStateID blendSrcAlpha,blendOneOne,blendOneOneReverseSubstract;

	AlphaStateID enableAlpha,disableAlpha;

	#endif
	#endif

	if (selectedShader > -1)
	{
		#ifndef __X7__
			if(m_bDebug)
					LOG_FNLN_NONE;
		changeShader(selectedShader);
		#endif
			if(m_bDebug)
					LOG_FNLN_NONE;
		applyConstants();
			if(m_bDebug)
					LOG_FNLN_NONE;
	}
	applyTextures();
	applySamplerStates();
		#ifndef __X7__
	changeVertexFormat(selectedVertexFormat);
	for (unsigned int i = 0; i < MAX_VERTEXSTREAM; i++)
	{
	#if 0
	LOG_FNLN_NONE;
	if(selectedVertexBuffers[i]>-1) LOG_PRINT_NONE("selectedVertexBuffers[%d]:%x\n", i, selectedVertexBuffers[i]);
	if(currentVertexBuffers[i]>-1) LOG_PRINT_NONE("currentVertexBuffers[%d]:%x\n", i, currentVertexBuffers[i]);
	if(selectedOffsets[i]) LOG_PRINT_NONE("selectedOffsets[%d]:%x\n", i, selectedOffsets[i]);
	if(currentOffsets[i]) LOG_PRINT_NONE("currentOffsets[%d]:%x\n", i, currentOffsets[i]);
	#endif
		//if (selectedVertexBuffers[i] != currentVertexBuffers[i]) // != DONTCARE)
		if (selectedVertexBuffers[i] != DONTCARE)
		{
			changeVertexBuffer(i, selectedVertexBuffers[i], selectedOffsets[i]);
		}
	}
		#endif
	/*
{
	int nvertices=0;//???
	int s=0;
	if(m_VertexPointer) s+=m_VertexW*sizeof(float);
	if(m_NormalPointer) s+=m_NormalW*sizeof(float);
	if(m_TexCoordPointer) s+=m_TexCoordW*sizeof(float);
	BYTE* vertices=new BYTE[nvertices*s];
	for(unsigned int i=0;i<nvertices;i++)
	{
		int o=0;
		if(m_VertexPointer)
		{
		stx_memcpy(&vertices[i*s+o],&((BYTE*)m_VertexPointer)[i*s],m_VertexW*sizeof(float));
		o+=m_VertexW*sizeof(float);
		}
		if(m_NormalPointer)
		{
		stx_memcpy(&vertices[i*s+o],&((BYTE*)m_NormalPointer)[i*s],m_VertexW*sizeof(float));
		o+=m_NormalW*sizeof(float);
		}
		if(m_TexCoordPointer)
		{
		stx_memcpy(&vertices[i*s+o],&((BYTE*)m_TexCoordPointer)[i*s],m_VertexW*sizeof(float));
		o+=m_TexCoordW*sizeof(float);
		}

	}
	static VertexBufferID vb=-1;
	vb=addVertexBuffer(nvertices*s,STATIC,vertices,vb);
	delete[] vertices;

	changeVertexBuffer(0, vb, s);
		m_VertexPointer=0;
   m_NormalPointer=0;
   m_TexCoordPointer=0;
   m_VertexOff=0;
   m_NormalOff=0;
   m_TexCoordOff=0;
   m_VertexW=0;
   m_NormalW=0;
   m_TexCoordW=0;
}
	*/
		#ifndef __X7__
	if (selectedIndexBuffer != DONTCARE) changeIndexBuffer(selectedIndexBuffer);
			//LOG_FNLN_NONE;
	if (selectedDepthState != DONTCARE)
	{
			//LOG_FNLN_NONE;

		changeDepthState(selectedDepthState, selectedStencilRef);
			//LOG_FNLN_NONE;
	}
			//LOG_FNLN_NONE;
 #if 1
	if (selectedAlphaState != DONTCARE)
	{
        //changeAlphaState(selectedAlphaState);
        BlendingAlphaTest::GetInstance()->SetAlphaTest(true);
	}
	if (selectedBlendState != DONTCARE)
	{
        changeBlendState(selectedBlendState, selectedSampleMask);
        //BlendingAlphaTest::GetInstance()->SetBlending( true );
	}
#endif
	if (selectedRasterizerState != DONTCARE) changeRasterizerState(selectedRasterizerState);
		#endif
}
#endif
void RendererHLSLCg::reset0(const unsigned int aflags)
{
#if 0
	if(m_pInput)
	{
		bool bLoop=true;
		while(bLoop)

		{
			m_pInput->Update();

			//if(m_pInput->IsKeyPressed(IND_M)) m_pInput=0;
			if(m_pInput->IsKeyPressed(IND_N)) bLoop=false;
			if(m_pInput->IsKeyPressed(IND_B)) DBG_HALT;
			//if(m_pInput->IsKeyPressed(IND_S)) m_pInput=0;
		}

//		EndScene(true);
		Present(true);
		LOG_PRINT_X("Present\n");
//		BeginScene(true);
		Clear(  true,false,D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
	}
#endif

	#if 0
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("selectedShader:%x\n", selectedShader);
	LOG_PRINT_NONE("currentShader:%x\n", currentShader);
	LOG_PRINT_NONE("selectedVertexFormat:%x\n", selectedVertexFormat);
	LOG_PRINT_NONE("currentVertexFormat:%x\n", currentVertexFormat);
	#endif

	#if 1
	unsigned int flags=aflags;
	#else
	unsigned int flags=getResetFlag();
	#endif

	#if 0
	if((selectedShader==-1) && (selectedVertexFormat==-1))
		flags=0;
	#endif

	#if 0
	LOG_FNLN;
	LOG_PRINT("selectedShader:%x\n", selectedShader);
	#endif
	//const unsigned int flags=0; // ???#if 0
	#if 1
	if (flags & RESET_SHADER)
	{
		selectedShader = SHADER_NONE;
		selectedPixelShader = SHADER_NONE;
		selectedVertexShader = SHADER_NONE;
		selectedGeometryShader = SHADER_NONE;
		selectedHullShader = SHADER_NONE;
		selectedDomainShader = SHADER_NONE;
		selectedComputeShader = SHADER_NONE;
	}

	#if 0
	LOG_FNLN;
	LOG_PRINT("selectedShader:%x\n", selectedShader);
	#endif
	if (flags & RESET_VF)
	{
		selectedVertexFormat = VF_NONE;
	}
	#endif
	if (flags & RESET_VB)
	{
		for (unsigned int i = 0; i < MAX_VERTEXSTREAM; i++)
		{
			selectedVertexBuffers[i] = VB_NONE;
			selectedOffsets[i] = 0;
		}
	}
	if (flags & RESET_IB) selectedIndexBuffer = IB_NONE;
	if (flags & RESET_DS)
	{
		selectedDepthState = DS_NONE;
		selectedStencilRef = 0;
	}
	if (flags & RESET_BS)
	{
		selectedBlendState = BS_NONE;
		selectedSampleMask = ~0;
	}
	if (flags & RESET_AS) selectedAlphaState = -1;
	if (flags & RESET_RS) selectedRasterizerState = RS_NONE;
	#if 0
	setResetFlag(0);
	#endif
	#ifdef DEBUG
	wasReset = true;
	#endif

	m_VertexPointer=0;
   m_NormalPointer=0;
   m_TexCoordPointer=0;
   m_VertexOff=0;
   m_NormalOff=0;
   m_TexCoordOff=0;
   m_VertexW=0;
   m_NormalW=0;
   m_TexCoordW=0;

}

TextureID RendererHLSLCg::addTextureVrtl(int Width, int Height, bool MipLevels, FORMAT format, const SamplerStateID samplerState,const unsigned int flags)
{
		//int bpp=getBytesPerPixel(format);
		//int is=getImageSize(format, Width, Height, 1, MipLevels);
		Image3 img;
		//int size = getMipMappedSize(0, MipLevelss);
		img.create(format,Width, Height, 1, MipLevels);
		//.setPixels( new ubyte[is]);
		return addTexture(&img,MipLevels,samplerState,flags);
}

unsigned int RendererHLSLCg::DrawPrimitiveUP(Primitives PrimitiveType,
UINT PrimitiveCount,
CONST void* pVertexStreamZeroData0,
CONST void* pVertexStreamZeroData,
UINT VertexStreamZeroStride, const unsigned int flags)
{
	int NumVertices=getPrimitiveCountR(PrimitiveType, PrimitiveCount);
	int StartVertex=int((((BYTE*)pVertexStreamZeroData)-((BYTE*)pVertexStreamZeroData0))/VertexStreamZeroStride);
	//static VertexBufferID vb = -1;

#if defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
	//LOG_FNLN_NONE;
	LOG_PRINT_NONE("vb_=%x\n", vb);
#endif

	if(vb==-1)
	{
	vb = addVertexBuffer(
		(NumVertices+StartVertex) * VertexStreamZeroStride,
		DYNAMIC,
		pVertexStreamZeroData0);
	} else {
	vb = addVertexBuffer(
		(NumVertices+StartVertex) * VertexStreamZeroStride,
		DYNAMIC,
		pVertexStreamZeroData0,
		vb);
	}

	setVertexBufferVrtl(0,vb);

	unsigned int ret=DrawPrimitive(PrimitiveType,StartVertex,
	PrimitiveCount,flags);
	if(ret){
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("ret=%x\n",ret);
	LOG_PRINT_NONE("PrimitiveType=%x\n",PrimitiveType);
	LOG_PRINT_NONE("PrimitiveCount=%d\n",PrimitiveCount);
	LOG_PRINT_NONE("NumVertices=%d\n", NumVertices);
	LOG_PRINT_NONE("StartVertex=%d\n", StartVertex);
	LOG_PRINT_NONE("VertexStreamZeroStride=%d\n", VertexStreamZeroStride);
	LOG_PRINT_NONE("pVertexStreamZeroData0=%x\n", pVertexStreamZeroData0);
	LOG_PRINT_NONE("flags=%d\n", flags);
	LOG_PRINT_NONE("vb=%d\n", vb);
	}
	return ret;
}

VertexBufferID RendererHLSLCg::vb = -1;
IndexBufferID RendererHLSLCg::ib = -1;

unsigned int RendererHLSLCg::DrawIndexedPrimitiveUPVrtl(Primitives PrimitiveType,
UINT MinVertexIndex,
UINT NumVertices,
UINT PrimitiveCount,
CONST void* pIndexData0,
CONST void* pIndexData,
STX_INDEXFORMAT IndexDataFormat,
CONST void* pVertexStreamZeroData0,
CONST void* pVertexStreamZeroData,
UINT VertexStreamZeroStride, const unsigned int flags)
{
	//if(m_bDebug)
	//	LOG_FNLN_NONE;
	if(!VertexStreamZeroStride) return 1;//???
	int StartVertex=int((((BYTE*)pVertexStreamZeroData)-((BYTE*)pVertexStreamZeroData0))/VertexStreamZeroStride);
	int startIndex=int((((BYTE*)pIndexData)-((BYTE*)pIndexData0))/IndexDataFormat);

	//static VertexBufferID vb = -1;
	//static IndexBufferID ib = -1;
	vb = addVertexBuffer(
		(NumVertices+StartVertex) * VertexStreamZeroStride,
		STATIC,
		pVertexStreamZeroData0,
		vb);
	ib = addIndexBuffer(
		startIndex+3*PrimitiveCount ,
		IndexDataFormat,
		STATIC,
		pIndexData0,
		ib);

	setVertexBufferVrtl(0,vb);
	setIndexBuffer(ib);
	unsigned int ret=DrawIndexedPrimitive(
		PrimitiveType,StartVertex,
	MinVertexIndex,
	NumVertices,startIndex,
	PrimitiveCount,flags);
	if(ret){
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("ret=%x\n",ret);
	LOG_PRINT_NONE("PrimitiveType=%x\n",PrimitiveType);
	LOG_PRINT_NONE("StartVertex=%d\n", StartVertex);
	LOG_PRINT_NONE("MinVertexIndex=%d\n", MinVertexIndex);
	LOG_PRINT_NONE("NumVertices=%d\n", NumVertices);
	LOG_PRINT_NONE("startIndex=%d\n", startIndex);
	LOG_PRINT_NONE("PrimitiveCount=%d\n",PrimitiveCount);
	LOG_PRINT_NONE("flags=%d\n", flags);
	LOG_PRINT_NONE("vb=%d\n", vb);
	LOG_PRINT_NONE("VertexStreamZeroStride=%d\n", VertexStreamZeroStride);
	LOG_PRINT_NONE("pVertexStreamZeroData0=%x\n", pVertexStreamZeroData0);
	LOG_PRINT_NONE("IndexDataFormat=%d\n", IndexDataFormat);
	LOG_PRINT_NONE("pIndexData0=%x\n", pIndexData0);
	LOG_PRINT_NONE("ib=%d\n", ib);
	}
	return ret;
}
	void RendererHLSLCg::DumpRenderer()
	{
		LOG_PRINT_X("%s\n",getString());
	}
unsigned int RendererHLSLCg::DrawFPSVrtl(int x, int y)
{

	char str[16];
    stx_snprintf(str,16, "%d", fps);
	drawText(str, x, GetViewportHeight()-y*10.0f, 30, 38,
	defaultFont,
	GetlinearClamp(),
	GetblendSrcAlpha(),
	GetnoDepthTest());
	return 0;//???
}
	void RendererHLSLCg::getExtents(FontID font,char *string, float x, float y, const float charWidth, const float charHeight, const float skewX , const float skewY ,
		//const FONT_HADJUST hAdjust , const FONT_VADJUST vAdjust ,
		float *left , float *right , float *top , float *bottom )
	{
	}
float RendererHLSLCg::getStringWidth(FontID font,const char* str, const float charWidth)

{
	if(!RendererHLSLCg::fonts.size()) return 0.0f;
	float startx = 0.0f;
	float x = 0.0f;
	while (*str)
	{
		if (*str == '\n')
		{
			//y += charHeight;
			x = startx;
		}
		else
		{
//			LOG_PRINT_X("RendererHLSLCg::fonts.size=%d\n",RendererHLSLCg::fonts.size());
			//TexFont f=;
			Character chr = RendererHLSLCg::fonts[font].chars[*(unsigned char *) str];
			float charWidth=RendererHLSLCg::fonts[font].chars[*(unsigned char *) str].x1-
				RendererHLSLCg::fonts[font].chars[*(unsigned char *) str].x0;
			float cw = charWidth * chr.ratio;
			x += cw;
		}
		str++;
	}
	return x;
}
void RendererHLSLCg::fillTextBuffer(TexVertex *dest, const char *str, float x, float y, const float charWidth, const float charHeight, const FontID font)
{
	float startx = x;
	while (*str)
	{
		if (*str == '\n')
		{
			y += charHeight;
			x = startx;
		}
		else
		{
//			LOG_PRINT_X("RendererHLSLCg::fonts.size=%d\n",RendererHLSLCg::fonts.size());
			//TexFont f=;
			Character chr = RendererHLSLCg::fonts[font].chars[*(unsigned char *) str];
			float cw = charWidth * chr.ratio;
#if 0
			dest[0].position = D3DXFROMWINEVECTOR2(x, y);
			dest[0].texCoord = D3DXFROMWINEVECTOR2(chr.x0, chr.y0);
			dest[1].position = D3DXFROMWINEVECTOR2(x + cw, y);
			dest[1].texCoord = D3DXFROMWINEVECTOR2(chr.x1, chr.y0);
			dest[2].position = D3DXFROMWINEVECTOR2(x, y + charHeight);
			dest[2].texCoord = D3DXFROMWINEVECTOR2(chr.x0, chr.y1);
			dest[3].position = D3DXFROMWINEVECTOR2(x, y + charHeight);
			dest[3].texCoord = D3DXFROMWINEVECTOR2(chr.x0, chr.y1);
			dest[4].position = D3DXFROMWINEVECTOR2(x + cw, y);
			dest[4].texCoord = D3DXFROMWINEVECTOR2(chr.x1, chr.y0);
			dest[5].position = D3DXFROMWINEVECTOR2(x + cw, y + charHeight);
			dest[5].texCoord = D3DXFROMWINEVECTOR2(chr.x1, chr.y1);
#else
			dest[0].position = D3DXFROMWINEVECTOR2(x + cw, y);
			dest[0].texCoord = D3DXFROMWINEVECTOR2(chr.x1, chr.y0);
			dest[1].position = D3DXFROMWINEVECTOR2(x + cw, y + charHeight);
			dest[1].texCoord = D3DXFROMWINEVECTOR2(chr.x1, chr.y1);
			dest[2].position = D3DXFROMWINEVECTOR2(x, y);
			dest[2].texCoord = D3DXFROMWINEVECTOR2(chr.x0, chr.y0);
			dest[3].position = D3DXFROMWINEVECTOR2(x, y + charHeight);
			dest[3].texCoord = D3DXFROMWINEVECTOR2(chr.x0, chr.y1);
#endif
			dest += 4;
			x += cw;
		}
		str++;
	}
}
unsigned int RendererHLSLCg::drawText(const char *str, float x, float y, const float charWidth,
						const float charHeight,
						const FontID& font,
						const SamplerStateID samplerState, const BlendStateID blendState, const DepthStateID depthState, const D3DXFROMWINEVECTOR4 col_)
{
#if 0//def _MSC_VER
    return 0;
#endif
//	if(strcmp(str,"Visibility")==0)DBG_HALT;
//	if(strcmp(str,"FPS Monitor")==0)DBG_HALT;
	FontID ffont= font;
	if (font == FONT_NONE){
	if (defaultFont == FONT_NONE)
	{
		//return false;
#if 0
		defaultFont = addFont(FONT_PATH"Future.dds", FONT_PATH"Future.font", linearClamp);
#else
		std::string 	fn1="/Framework3/Future.dds";
				fn1="/Framework3/Future.png";
		std::string fn2="/Framework3/Future.font";

		defaultFont = addFont(fn1.c_str(), fn2.c_str(), linearClamp);

#if 1
	//LOG_PRINT_NONE("fn1=%s\n", fn1.c_str());
	//LOG_PRINT_NONE("fn2=%s\n", fn2.c_str());
	//LOG_PRINT_NONE("defaultFont=%x\n", defaultFont);
	if (defaultFont == FONT_NONE)
	{
		//LOG_PRINT_NONE("defaultFont==-1\n");
		//LOG_PRINT_NONE("Exiting!\n");
		stx_exit(0);
 	}
#endif
#endif
		ffont=defaultFont;

    }else{ffont=font;}}

	unsigned int n = 4 * getTextQuads(str);

	if (n == 0) return true;

	if (n > fontBufferCount)
	{

		fontBuffer.resize( n );

		fontBufferCount = n;
	}

	fillTextBuffer(
#if !defined(_MSC_VER) && !defined(ANDROID) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
		fontBuffer.data()

#else
		&fontBuffer[0]
#endif
		, str, x, y, charWidth, charHeight, ffont);

	/*
	drawTextured(PRIM_TRIANGLES,
#ifndef _MSC_VER
		fontBuffer.data()
#else
		&fontBuffer[0]
#endif
	, n, RendererHLSLCg::fonts[ffont]->texture, samplerState, blendState, depthState);
	*/
/*
drawTextured(
	const Primitives primitives, 
	TexVertex *vertices, 
	const unsigned int nVertices, 
	const TextureID texture, 
	const BlendStateID blendState = -1, 
	const DepthStateID depthState = -1, 
	const D3DXFROMWINEVECTOR4 *color = 0
*/
	return drawTextured(PRIM_TRIANGLE_STRIP,
#if !defined(_MSC_VER) && !defined(ANDROID) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
		fontBuffer.data()
#else
		&fontBuffer[0]
#endif
		, n-2, RendererHLSLCg::fonts[ffont].texture, blendState, depthState, &col_);
}
#define MAX_BUF_SIZE 1024
unsigned int RendererHLSLCg::DrawText(unsigned int x, unsigned int y, const char *pszFormat, ...)
{
	char	buf[MAX_BUF_SIZE]={0};
	buf[0]='\0';
	va_list arglist;
	va_start(arglist, pszFormat);
    vsprintf(&buf[stx_strlen(buf)], pszFormat, arglist);
	va_end(arglist);


		drawText(buf, x, y, 15, 18, defaultFont, linearClamp, blendSrcAlpha, noDepthTest);
	return 0;
}
unsigned int RendererHLSLCg::DrawSmallText(unsigned int x, unsigned int y, const char *pszFormat, ...)
{
	char	buf[MAX_BUF_SIZE]={0};
	buf[0]='\0';
	va_list arglist;
	va_start(arglist, pszFormat);
    vsprintf(&buf[stx_strlen(buf)], pszFormat, arglist);
	va_end(arglist);
		drawText(buf, x, y, 7, 9, defaultFont, linearClamp, blendSrcAlpha, noDepthTest);
	return 0;
}
extern "C"
{
int rgb2gifMain(char *FileName0, unsigned int Width, unsigned int Height, unsigned char* Data);
};
void RendererHLSLCg::OpenILToGIF(Image3& img, const char* fn)
{
				FORMAT formats=img.getFormat();
				char bbuf[1024];
				sprintf(bbuf,"%s.gif",fn);
				//rgb2gifMain(bbuf,img.getWidth(),img.getHeight(),(unsigned char*)img.getPixels());
}

unsigned int RendererHLSLCg::getTextQuads(const char *str) const
{
	unsigned int n = 0;
	while (*str)
	{
		if (*str != '\n') n++;
		str++;
	}
	return n;
}
void RendererHLSLCg::setShaderConstant1i(const char *name, const int constant)
{
	//ASSERT(selectedShader != SHADER_NONE);
	setShaderConstantRaw(name, &constant, sizeof(constant));
}
void RendererHLSLCg::setShaderConstant1f(const char *name, const float constant)
{
	//ASSERT(selectedShader != SHADER_NONE);
	setShaderConstantRaw(name, &constant, sizeof(constant));
}
void RendererHLSLCg::setShaderConstant2f(const char *name, const D3DXFROMWINEVECTOR2 &constant)
{
	//ASSERT(selectedShader != SHADER_NONE);
	setShaderConstantRaw(name, &constant, sizeof(constant));
}
void RendererHLSLCg::setShaderConstant3f(const char *name, const D3DXFROMWINEVECTOR3 &constant)
{
	//ASSERT(selectedShader != SHADER_NONE);
	setShaderConstantRaw(name, &constant, sizeof(constant));
}
void RendererHLSLCg::setShaderConstant4f(const char *name, const D3DXFROMWINEVECTOR4 &constant)
{
	//ASSERT(selectedShader != SHADER_NONE);
	setShaderConstantRaw(name, &constant, sizeof(constant));
}
void RendererHLSLCg::setShaderConstant4x4f(const char *name, const D3DXFROMWINEMATRIX &constant)
{
	//ASSERT(selectedShader != SHADER_NONE);
#if 0
	D3DXFROMWINEMATRIX constant2=constant;
	//constant2.setTranspose();//???
	D3DXFROMWINEMatrixIdentity(&constant2);
	setShaderConstantRaw(name, &constant2, sizeof(constant2));
#else
	setShaderConstantRaw(name, &constant, sizeof(constant));
	//LOG_PRINT_X("%s\n",name);
#endif
}

void RendererHLSLCg::setShaderConstantArray1f(const char *name, const float *constant, const unsigned int count)
{
	//ASSERT(selectedShader != SHADER_NONE);
	setShaderConstantRaw(name, constant, count * sizeof(float));
}
void RendererHLSLCg::setShaderConstantArray2f(const char *name, const D3DXFROMWINEVECTOR2 *constant, const unsigned int count)
{
	//ASSERT(selectedShader != SHADER_NONE);
	setShaderConstantRaw(name, constant, count * sizeof(D3DXFROMWINEVECTOR2));
}
void RendererHLSLCg::setShaderConstantArray3f(const char *name, const D3DXFROMWINEVECTOR3 *constant, const unsigned int count)
{
	//ASSERT(selectedShader != SHADER_NONE);
	setShaderConstantRaw(name, constant, count * sizeof(D3DXFROMWINEVECTOR3));
}
void RendererHLSLCg::setShaderConstantArray4f(const char *name, const D3DXFROMWINEVECTOR4 *constant, const unsigned int count)
{
	//ASSERT(selectedShader != SHADER_NONE);
	setShaderConstantRaw(name, constant, count * sizeof(D3DXFROMWINEVECTOR4));
}
void RendererHLSLCg::setShaderConstantArray4x4f(const char *name, const D3DXFROMWINEMATRIX *constant, const unsigned int count)
{
	//ASSERT(selectedShader != SHADER_NONE);
	setShaderConstantRaw(name, constant, count * sizeof(D3DXFROMWINEMATRIX));
}
#if 0
// Transform (x, y, z, 0) by matrix.  If you transforming a normal by a
// non-affine matrix, the matrix you pass to this function should be the
// transpose of the inverse of the matrix you would use to transform a coord.
D3DXFROMWINEVECTOR3* D3DXFROMWINEVECTOR3TransformNormal
    ( D3DXFROMWINEVECTOR3 *pOut, CONST D3DXFROMWINEVECTOR3 *pV, CONST D3DXFROMWINEMATRIX *pM )
{
    D3DXFROMWINEVECTOR4 X;
    D3DXFROMWINEVECTOR4 Y;
    D3DXFROMWINEVECTOR4 Z;
	D3DXFROMWINEVECTOR4 pOut0;
#if 0
    Z = XMVectorSplatZ(V);
    Y = XMVectorSplatY(V);
    X = XMVectorSplatX(V);
#else
	D3DXFROMWINEVECTOR3 V=*pV;
	Z[0]=V.z;
	Z[1]=V.z;
	Z[2]=V.z;

	Y[0]=V.y;
	Y[1]=V.y;
	Y[2]=V.y;

	X[0]=V.x;
	X[1]=V.x;
	X[2]=V.x;
#endif
    pOut0 = Z * ((*pM).rows[2]);//XMVectorMultiply(Z, M.r[2]);
    pOut0 = Y*((*pM).rows[1])+ pOut0;//XMVectorMultiplyAdd(Y, M.r[1], Result);
    pOut0 = X*((*pM).rows[0])+ pOut0;//XMVectorMultiplyAdd(X, M.r[0], Result);

	pOut->x=pOut0.x;
	pOut->y=pOut0.y;
	pOut->z=pOut0.z;

	return pOut;

}
#endif
VOID RendererHLSLCg::UpdateViewer(
                                                   D3DXFROMWINEVECTOR3 &pos, float* quat)
{
#if 0
        D3DXFROMWINEVECTOR3 v;
        m_eye = pos;
		D3DXFROMWINEMATRIX m_matRot;
		Quaternion2Mat4(m_matRot, quat);

        v = D3DXFROMWINEVECTOR3(0.0f, 0.0f, -1.0f);
        m_center=m_matRot*v;
        v = D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f);
        m_up=m_matRot*v;

        D3DXFROMWINEMATRIX m1, m2;
        m1.setScales( 1.0f, 1.0f, -1.0f);
        m2.rotateX( D3DXFROMWINE_PI / 2.0f);
        m2=m1*m2;

        m_center=m2*m_center;
        m_up=m2*m_up;

        m_center = normalize(m_center);
        m_up = normalize(m_up);
//        m_vCross = normalize(cross(m_up, m_center));
const FLOAT NEAR_CLIP = 0.001f;//150.0f;

const FLOAT FAR_CLIP = 1200000.0f;

const FLOAT FOV = D3DXFROMWINE_PI / 4.0f;

	buildPerspectiveMatrix(

		180.0f/4,

		float(GetViewportWidth())/GetViewportHeight(),

		0.1f,//NEAR_CLIP,

		FAR_CLIP

		);


#if 1
	D3DXFROMWINEVECTOR3 c=m_eye+100.0f*m_center;
#else
	static D3DXFROMWINEVECTOR3 m_eye0=m_eye;
	D3DXFROMWINEVECTOR3 c=m_eye+100.0f*(m_eye-m_eye0);
	m_up=D3DXFROMWINEVECTOR3(0,1,0);
#endif
		buildLookAtMatrix(
			m_eye.x,
			m_eye.y,
			m_eye.z,
			c.x,
			c.y,
			c.z,
			m_up.x,
			m_up.y,
			m_up.z
			);
#endif
}
#if 0
HRESULT RendererHLSLCg::LoadTexture(LoaderID pLoader)
{

	return textureLoaders[pLoader].Load();
}
HRESULT RendererHLSLCg::DecompressTexture(LoaderID  pLoader, void** pLocalData, SIZE_T* Bytes )
{
	return textureLoaders[pLoader].Decompress(pLocalData, Bytes);
}
HRESULT RendererHLSLCg::LoadVertexBuffer(LoaderID pLoader)
{
	return vertexBufferLoaders[pLoader].Load();
}
HRESULT RendererHLSLCg::DecompressVertexBuffer(LoaderID  pLoader, void** pLocalData, SIZE_T* Bytes )
{
	return vertexBufferLoaders[pLoader].Decompress(pLocalData, Bytes);
}
HRESULT RendererHLSLCg::LoadIndexBuffer(LoaderID pLoader)
{
	return indexBufferLoaders[pLoader].Load();
}
HRESULT RendererHLSLCg::DecompressIndexBuffer(LoaderID  pLoader, void** pLocalData, SIZE_T* Bytes )
{
	return indexBufferLoaders[pLoader].Decompress(pLocalData, Bytes);
}








LoaderID RendererHLSLCg::addTextureLoader( CHAR* szFileName, CPackedFile* g_PackFile )
	{
		static int id=-1;

			CTextureLoader tl( szFileName, g_PackFile );
			id=textureLoaders.add(tl);
		return id;
	}
LoaderID RendererHLSLCg::addVertexBufferLoader()
	{
		static int id=-1;
			CVertexBufferLoader tl;
			id=vertexBufferLoaders.add(tl);
		return id;
	}
LoaderID RendererHLSLCg::addIndexBufferLoader()
	{
		static int id=-1;
			CIndexBufferLoader tl;
			id=indexBufferLoaders.add(tl);
		return id;
	}
void RendererHLSLCg::SetProcessorResourceError(ProcessorID pProcessor){IMPLEMENT;}
void RendererHLSLCg::DestroyProcessor(ProcessorID pProcessor){IMPLEMENT;}
void RendererHLSLCg::DestroyLoader(ProcessorID pLoader){IMPLEMENT;}
#endif
void D3DXFROMWINECreateTextureFromFile(
	const char* n,
	TextureID* id,
	bool cm,
	SamplerStateID si)
{
	if(si==-1) si=IRenderer::GetRendererInstance()->Getlinear();
	*id=IRenderer::GetRendererInstance()->addImageLibTexture(
		n,
		cm,




		si,0);

}
struct PCVertex {
	D3DXFROMWINEVECTOR2 vertex;
	//D3DXFROMWINEVECTOR4 color;
};

struct PCTVertex {
	D3DXFROMWINEVECTOR2 vertex;

	//D3DXFROMWINEVECTOR4 color;
	D3DXFROMWINEVECTOR2 texCoord;
};
struct PCVertex3 {
	D3DXFROMWINEVECTOR3 vertex;
	//D3DXFROMWINEVECTOR4 color;
};

struct PCTVertex3 {
	D3DXFROMWINEVECTOR3 vertex;

	//D3DXFROMWINEVECTOR4 color;
	D3DXFROMWINEVECTOR2 texCoord;
};
struct PCNVertex {
	D3DXFROMWINEVECTOR2 vertex;
	D3DXFROMWINEVECTOR4 color;
};

struct PCNTVertex {
	D3DXFROMWINEVECTOR2 vertex;
	D3DXFROMWINEVECTOR4 color;
	D3DXFROMWINEVECTOR2 texCoord;
};
struct PCNVertex3 {
	D3DXFROMWINEVECTOR3 vertex;
	D3DXFROMWINEVECTOR4 color;
};

struct PCNTVertex3 {
	D3DXFROMWINEVECTOR3 vertex;
	D3DXFROMWINEVECTOR4 color;
	D3DXFROMWINEVECTOR2 texCoord;
};
#if 1
unsigned int RendererHLSLCg::drawLineVrtl(const float x0, const float y0, const float x1, const float y1, const D3DXFROMWINEVECTOR4 &color, const float lineWidth){
	D3DXFROMWINEVECTOR2 no;
	D3DXFROMWINEVECTOR2 ni(y1 - y0, x1 - x0);
	D3DXFROMWINEVec2Normalize(&no,&ni);
	D3DXFROMWINEVECTOR2 d = 0.5f * lineWidth * no;
	D3DXFROMWINEVECTOR2 v0(x0, y0);
	D3DXFROMWINEVECTOR2 v1(x1, y1);

	unsigned int col = toBGRA(color);
	PCVertex vertices[] = {
		D3DXFROMWINEVECTOR2(v0 + d),// color,
		D3DXFROMWINEVECTOR2(v0 - d),// color,
		D3DXFROMWINEVECTOR2(v1 + d),// color,
		D3DXFROMWINEVECTOR2(v1 - d),// color,
	};


	InitPlain(plainShader, plainVF);
	setShader(plainShader);
	setVertexFormat(plainVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", color);
	setDepthState(GetnoDepthTest());
	return DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, vertices, vertices, sizeof(PCVertex));
}
	TextureID RendererHLSLCg::addRenderDepth(const int width, const int height, const int depthBits){
		return addRenderDepth(width, height, 1, depthBits <= 16? FORMAT_D16 : FORMAT_D24);
	}

unsigned int RendererHLSLCg::drawBezierVrtl(D3DXFROMWINEVECTOR2 anchor1, D3DXFROMWINEVECTOR2 anchor2, D3DXFROMWINEVECTOR2 control1, D3DXFROMWINEVECTOR2 control2, const D3DXFROMWINEVECTOR4 &color, const float lineWidth)
{
	std::vector<D3DXFROMWINEVECTOR2> points;
	points.push_back(D3DXFROMWINEVECTOR2(anchor1.x,anchor1.y));
 	// store values where to lineTo
	D3DXFROMWINEVECTOR2 pos;
	//loop through 100 steps of the curve
	for (unsigned int u_ = 0; u_ < 100; u_++)
	{ 
		float u=u_*0.01f;
		pos.x = powf(u,3)*(anchor2.x+3*(control1.x-control2.x)-anchor1.x)
			+3*powf(u,2)*(anchor1.x-2*control1.x+control2.x)
			+3*u*(control1.x-anchor1.x)+anchor1.x;
		pos.y = powf(u,3)*(anchor2.y+3*(control1.y-control2.y)-anchor1.y)
			+3*powf(u,2)*(anchor1.y-2*control1.y+control2.y)
			+3*u*(control1.y-anchor1.y)+anchor1.y;
		points.push_back(pos);
		points.push_back(pos);
	} 
	points.push_back(D3DXFROMWINEVECTOR2(anchor2.x,anchor2.y));
	unsigned int col = toBGRA(color);
	InitPlain(plainShader, plainVF);
	setShader(plainShader);
	setVertexFormat(plainVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", color);
	setDepthState(GetnoDepthTest());
	return DrawPrimitiveUP(PRIM_LINES, (points.size()-2)/2+2, &points[0], &points[0], sizeof(D3DXFROMWINEVECTOR2));
}
#if 1
unsigned int RendererHLSLCg::drawParaboleVrtl(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const D3DXFROMWINEVECTOR4 &color, const float lineWidth)
{
	std::vector<D3DXFROMWINEVECTOR2> points;
	float denom = (x1-x2) * (x1-x3) * (x2-x3);
	float a     = (x3 * (y2-y1) + x2 * (y1-y3) + x1 * (y3-y2)) / denom;
	float b     = (x3*x3 * (y1-y2) + x2*x2 * (y3-y1) + x1*x1 * (y2-y3)) / denom;
	float c     = (x2 * x3 * (x2-x3) * y1+x3 * x1 * (x3-x1) * y2+x1 * x2 * (x1-x2) * y3) / denom;
#if 0
	unsigned int N=60;
	for(unsigned int i=0;i<N;i++)
	{
		float x_val=-0.5f*N/3.0f+1.0f*i;
		points.push_back(D3DXFROMWINEVECTOR2(x_val, fabs(a*x_val*x_val+b*x_val+c)));
		LOG_PRINT("%f\t%f\n", points[points.size()-1].x, points[points.size()-1].y);
	}
	unsigned int col = toBGRA(color);
	InitPlain(plainShader, plainVF);
	setShader(plainShader);
	setVertexFormat(plainVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", color);
	setDepthState(GetnoDepthTest());
	return DrawPrimitiveUP(PRIM_POINTS, points.size(), &points[0], &points[0], sizeof(D3DXFROMWINEVECTOR2));
#else
	bool first = true;
	unsigned int prev_x;
	unsigned int prev_y;
	for(unsigned int y=0;y<STX_Service::GetWindowInstance()->GetHeight();y++)
	{
        	for(unsigned int x=0;x<STX_Service::GetWindowInstance()->GetWidth();x++)
		{
            		//if(x == y*y - 30*y + 450)
			if(x == a*y*y + b*y + c)
			{
                		if(first)
				{
                    			first = false;
                   			prev_x = x;
                   			prev_y = y;
                    			continue;
				}
				points.push_back(D3DXFROMWINEVECTOR2(prev_y, prev_x));
				points.push_back(D3DXFROMWINEVECTOR2(y, x));
                		prev_x = x;
                		prev_y = y;
			}
		}
	}
	unsigned int col = toBGRA(color);
	InitPlain(plainShader, plainVF);
	setShader(plainShader);
	setVertexFormat(plainVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", color);
	setDepthState(GetnoDepthTest());
	return DrawPrimitiveUP(PRIM_LINES, (points.size()-2)/2+2, &points[0], &points[0], sizeof(D3DXFROMWINEVECTOR2));
#endif
}
#if 0
parabola equation: y=a(x-x0)(x+x0)
		   a=y/((x-x0)(x+x0))=0.25/p
		   p=0.25*((x-x0)(x+x0))/y
#endif
unsigned int RendererHLSLCg::drawParaboleVrtl(const float xpos, const float x0, const float k, const D3DXFROMWINEVECTOR4 &color, const float lineWidth)
{
	// y = a(x-h)2+k. (h,k) is the vertex
	std::vector<D3DXFROMWINEVECTOR2> points;
	float prev_x;
	float prev_y;
	bool first = true;
	unsigned int N=100;
	float dx=2.0f*x0/float(N);
	float a = -k/(x0*x0);
	for(unsigned int i=0;i<N;i++)
	{
		float x=-x0+dx*float(i);
		float y = a*(x-x0)*(x+x0);
                if(first)
		{
                    	first = false;
                   	prev_x = x;
                   	prev_y = y;
                    	continue;
		}
#if 0
		points.push_back(D3DXFROMWINEVECTOR2(prev_x+xpos, prev_y-0.5f*GetViewportHeight()));
		points.push_back(D3DXFROMWINEVECTOR2(x+xpos, y-0.5f*GetViewportHeight()));
#else
		points.push_back(D3DXFROMWINEVECTOR2(prev_x+xpos, prev_y));
		points.push_back(D3DXFROMWINEVECTOR2(x+xpos, y));
#endif
	}
	unsigned int col = toBGRA(color);
	if(points.size())
	{
		InitPlain(plainShader, plainVF);
		setShader(plainShader);
		setVertexFormat(plainVF);
		setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
		setShaderConstant4f("scaleBias", GetscaleBias2D());
		setShaderConstant4f("colorRGBA", color);
		setDepthState(GetnoDepthTest());
		DrawPrimitiveUP(PRIM_LINES, (points.size()-2)/2+2, &points[0], &points[0], sizeof(D3DXFROMWINEVECTOR2));
	}
	return 0;
}
unsigned int RendererHLSLCg::drawParaboleVrtl(const float p, const float h, const unsigned int k, const D3DXFROMWINEVECTOR4 &color, const float lineWidth)
{
	// y = a(x-h)2+k. (h,k) is the vertex
	std::vector<D3DXFROMWINEVECTOR2> points;
	std::vector<D3DXFROMWINEVECTOR2> points2;
	bool first = true;
	float prev_x;
	float prev_y;

	//LOG_PRINT("k=%ld\n", k);
	//LOG_PRINT("STX_Service::GetWindowInstance()->GetHeight()=%d\n", STX_Service::GetWindowInstance()->GetHeight());

	for(unsigned int y=k;y<STX_Service::GetWindowInstance()->GetHeight();y++)
	{
        	//for(unsigned int x=0;x<STX_Service::GetWindowInstance()->GetWidth();x++)
		{
            		//if(x == y*y - 30*y + 450)
			float x = (y-h)*(y-h)*0.25f/p+k;
			{
                		if(first)
				{
                    			first = false;
                   			prev_x = x;
                   			prev_y = y;
                    			continue;
				}
				points.push_back(D3DXFROMWINEVECTOR2(prev_y, prev_x));
				points.push_back(D3DXFROMWINEVECTOR2(y, x));
				points2.push_back(D3DXFROMWINEVECTOR2(prev_y, -prev_x));
				points2.push_back(D3DXFROMWINEVECTOR2(y, -x));
                		prev_x = x;
                		prev_y = y;
			}
		}
	}
	//points2.clear();
	unsigned int col = toBGRA(color);
	if(points.size())
	{
	InitPlain(plainShader, plainVF);
	setShader(plainShader);
	setVertexFormat(plainVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", color);
	setDepthState(GetnoDepthTest());
	       DrawPrimitiveUP(PRIM_LINES, (points.size()-2)/2+2, &points[0], &points[0], sizeof(D3DXFROMWINEVECTOR2));
	}
	if(points2.size())
	{
	InitPlain(plainShader, plainVF);
	setShader(plainShader);
	setVertexFormat(plainVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", color);
	setDepthState(GetnoDepthTest());
		DrawPrimitiveUP(PRIM_LINES, (points2.size()-2)/2+2, &points2[0], &points2[0], sizeof(D3DXFROMWINEVECTOR2));
	}
	return 0;
}

unsigned int RendererHLSLCg::drawParaboleVrtl(const float a, const float b, const float c, const float ___, const D3DXFROMWINEVECTOR4 &color, const float lineWidth)
{
/*
 		Cartesian equation:	Parametrically:	
Ellipse:	x2/a2 + y2/b2 = 1	x = a cos(t), y = b sin(t)	x2b2+y2a2=a2b2
Parabole:	y = ax2 + bx + c 

		ax2 - y + bx = - c

Hyperbole:	x2/a2 - y2/b2 = 1	x = a sec(t), y = b tan(t)	x2b2-y2a2=a2b2

Conic:		f(x,y)=Ax2+Bxy+Cy2+Dx+Ey+F
*/
	std::vector<D3DXFROMWINEVECTOR2> points;
	bool first = true;
	unsigned int prev_x;
	unsigned int prev_y;
	for(unsigned int y=0;y<STX_Service::GetWindowInstance()->GetHeight();y++)
	{
        	for(unsigned int x=0;x<STX_Service::GetWindowInstance()->GetWidth();x++)
		{
            		//if(x == y*y - 30*y + 450)
			if(x == a*y*y + b*y + c)
			{
                		if(first)
				{
                    			first = false;
                   			prev_x = x;
                   			prev_y = y;
                    			continue;
				}
				points.push_back(D3DXFROMWINEVECTOR2(prev_y, prev_x));
				points.push_back(D3DXFROMWINEVECTOR2(y, x));
                		prev_x = x;
                		prev_y = y;
			}
		}
	}
	unsigned int col = toBGRA(color);
	InitPlain(plainShader, plainVF);
	setShader(plainShader);
	setVertexFormat(plainVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", color);
	setDepthState(GetnoDepthTest());
	return DrawPrimitiveUP(PRIM_LINES, (points.size()-2)/2+2, &points[0], &points[0], sizeof(D3DXFROMWINEVECTOR2));
}
#endif
unsigned int RendererHLSLCg::drawEllipseVrtl(const float originx, const float originy, const float width, const float height, const D3DXFROMWINEVECTOR4 &color, const float lineWidth)
{
	std::vector<D3DXFROMWINEVECTOR2> points;
/*
 		Cartesian equation:	Parametrically:	
Ellipse:	x2/a2 + y2/b2 = 1	x = a cos(t), y = b sin(t)	x2b2+y2a2=a2b2
Parabola:	y = ax2 + bx + c 
Hyperbola:	x2/a2 - y2/b2 = 1	x = a sec(t), y = b tan(t)	x2b2-y2a2=a2b2

Conic:		f(x,y)=Ax2+Bxy+Cy2+Dx+Ey+F
*/
#if 1
	int hh = height * height;
	int ww = width * width;
	int hhww = hh * ww;
	int x0 = width;
	int dx = 0;
	// do the horizontal diameter
	for (int x = -width; x <= width; x++)
    		points.push_back(D3DXFROMWINEVECTOR2(originx + x, originy));
	// now do both halves at the same time, away from the diameter
	for (int y = 1; y <= height; y++)
	{
		int x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more
    		for ( ; x1 > 0; x1--)
        		if (x1*x1*hh + y*y*ww <= hhww)
            			break;
    		dx = x0 - x1;  // current approximation of the slope
    		x0 = x1;
    		for (int x = -x0; x <= x0; x++)
    		{
        		points.push_back(D3DXFROMWINEVECTOR2(originx + x, originy - y));
        		points.push_back(D3DXFROMWINEVECTOR2(originx + x, originy + y));
    		}
	}
#else /* One of Abrash's ellipse algorithms  */
	int x=originx;
	int y=originy;
	int a=width;
	int b=height;

    int wx, wy;
    int thresh;
    int asq = a * a;
    int bsq = b * b;
    int xa, ya;

    points.push_back(D3DXFROMWINEVECTOR2(x, y+b));
    points.push_back(D3DXFROMWINEVECTOR2(x, y-b));

    wx = 0;
    wy = b;
    xa = 0;
    ya = asq * 2 * b;
    thresh = asq / 4 - asq * b;

    for (;;) {
        thresh += xa + bsq;

        if (thresh >= 0) {
            ya -= asq * 2;
            thresh -= ya;
            wy--;
        }

        xa += bsq * 2;
        wx++;

        if (xa >= ya)
          break;
    }
#endif
	unsigned int col = toBGRA(color);
	InitPlain(plainShader, plainVF);
	setShader(plainShader);
	setVertexFormat(plainVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", color);
	setDepthState(GetnoDepthTest());
	return DrawPrimitiveUP(PRIM_POINTS, points.size(), &points[0], &points[0], sizeof(D3DXFROMWINEVECTOR2));
}

unsigned int RendererHLSLCg::drawHyperboleVrtl(const float originx, const float originy, const float width, const float height, const D3DXFROMWINEVECTOR4 &color, const float lineWidth)
{
	std::vector<D3DXFROMWINEVECTOR2> points;
/*
 		Cartesian equation:	Parametrically:	
Ellipse:	x2/a2 + y2/b2 = 1	x = a cos(t), y = b sin(t)	x2b2+y2a2=a2b2
Parabole:	y = ax2 + bx + c 
Hyperbole:	x2/a2 - y2/b2 = 1	x = a sec(t), y = b tan(t)	x2b2-y2a2=a2b2

Conic:		f(x,y)=Ax2+Bxy+Cy2+Dx+Ey+F
*/
	int hh = height * height;
	int ww = width * width;
	int hhww = hh * ww;
	int x0 = width;
	int dx = 0;
	// do the horizontal diameter
	for (int x = -width; x <= width; x++)
    		points.push_back(D3DXFROMWINEVECTOR2(originx + x, originy));
	// now do both halves at the same time, away from the diameter
	for (int y = 1; y <= height; y++)
	{
		int x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more
    		for ( ; x1 > 0; x1--)
        		if (x1*x1*hh - y*y*ww <= hhww)
            			break;
    		dx = x0 - x1;  // current approximation of the slope
    		x0 = x1;
    		for (int x = -x0; x <= x0; x++)
    		{
        		points.push_back(D3DXFROMWINEVECTOR2(originx + x, originy - y));
        		points.push_back(D3DXFROMWINEVECTOR2(originx + x, originy + y));
    		}
	}
	unsigned int col = toBGRA(color);
	InitPlain(plainShader, plainVF);
	setShader(plainShader);
	setVertexFormat(plainVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", color);
	setDepthState(GetnoDepthTest());
	return DrawPrimitiveUP(PRIM_POINTS, points.size(), &points[0], &points[0], sizeof(D3DXFROMWINEVECTOR2));
}
unsigned int RendererHLSLCg::drawRectVrtl(const float x0, const float y0, const float x1, const float y1, const D3DXFROMWINEVECTOR4 &color, const float lineWidth)
{
#if 0
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("x0=%f\n", x0);
	LOG_PRINT_NONE("y0=%f\n", y0);
	LOG_PRINT_NONE("x1=%f\n", x1);
	LOG_PRINT_NONE("y1=%f\n", y1);
	LOG_PRINT_NONE("lineWidth=%f\n", lineWidth);
#endif
	unsigned int col = toBGRA(color);
	InitPlain(plainShader, plainVF);
	setShader(plainShader);
	setVertexFormat(plainVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", color);
	setDepthState(GetnoDepthTest());
#if 0
	Color4fv(color);
	if (lineWidth > 0){
		glLineWidth(lineWidth);
		Begin(GL_LINE_LOOP);
	} else {
		Begin(GL_TRIANGLE_FAN);
	}
	Vertex2f(x0, y0);
	Vertex2f(x1, y0);
	Vertex2f(x1, y1);
	Vertex2f(x0, y1);
	End();
}
#elif 0
	if (lineWidth > 0){
		float d = 0.5f * lineWidth;
		PCVertex vertices[] = {
			float3(x0 - d, y0 - d, 0), col,
			float3(x0 + d, y0 + d, 0), col,
			float3(x1 + d, y0 - d, 0), col,
			float3(x1 - d, y0 + d, 0), col,
			float3(x1 + d, y1 + d, 0), col,
			float3(x1 - d, y1 - d, 0), col,
			float3(x0 - d, y1 + d, 0), col,
			float3(x0 + d, y1 - d, 0), col,
			float3(x0 - d, y0 - d, 0), col,
			float3(x0 + d, y0 + d, 0), col,
		};
		dev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 8, vertices, sizeof(PCVertex));
	} else {
		PCVertex vertices[] = {
			float3(x0, y0, 0), col,
			float3(x1, y0, 0), col,
			float3(x1, y1, 0), col,
			float3(x0, y1, 0), col,
		};
		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(PCVertex));
	}
#elif 0
	if (lineWidth > 0){
		float d = 0.5f * lineWidth;
		PCVertex vertices[] = {
			D3DXFROMWINEVECTOR2(x0 - d, y0 - d),// color,
			D3DXFROMWINEVECTOR2(x0 + d, y0 + d),// color,
			D3DXFROMWINEVECTOR2(x1 + d, y0 - d),// color,
			D3DXFROMWINEVECTOR2(x1 - d, y0 + d),// color,
			D3DXFROMWINEVECTOR2(x1 + d, y1 + d),// color,
			D3DXFROMWINEVECTOR2(x1 - d, y1 - d),// color,
			D3DXFROMWINEVECTOR2(x0 - d, y1 + d),// color,
			D3DXFROMWINEVECTOR2(x0 + d, y1 - d),// color,

			D3DXFROMWINEVECTOR2(x0 - d, y0 - d),// color,
			D3DXFROMWINEVECTOR2(x0 + d, y0 + d),// color,
		};

		DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 8, vertices, vertices, sizeof(PCVertex));
	} else {
#ifndef _MSC_VER
		PCVertex vertices[] = {
			D3DXFROMWINEVECTOR2(x0, y0), //color,
			D3DXFROMWINEVECTOR2(x1, y0), //color,
			D3DXFROMWINEVECTOR2(x1, y1), //color,
			D3DXFROMWINEVECTOR2(x0, y1), //color,
		};
		DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(PCVertex));
#else
		PCVertex vertices[] = {
			D3DXFROMWINEVECTOR2(x1, y0);
			D3DXFROMWINEVECTOR2(x1, y1);
			D3DXFROMWINEVECTOR2(x0, y0);
			D3DXFROMWINEVECTOR2(x0, y0);
			D3DXFROMWINEVECTOR2(x1, y1);
			D3DXFROMWINEVECTOR2(x0, y1);
		};
		DrawPrimitiveUP(PRIM_TRIANGLES, 2, vertices, vertices, sizeof(PCVertex));
#endif
	}
#else
#ifndef _MSC_VER
	PCVertex vertices[] = {
		D3DXFROMWINEVECTOR2(x0, y0),
		D3DXFROMWINEVECTOR2(x1, y0),
		D3DXFROMWINEVECTOR2(x1, y1),
		D3DXFROMWINEVECTOR2(x0, y1),
	};
	DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(PCVertex));
#else
		PCVertex vertices[] = {
			D3DXFROMWINEVECTOR2(x1, y0),
			D3DXFROMWINEVECTOR2(x1, y1),
			D3DXFROMWINEVECTOR2(x0, y0),
			D3DXFROMWINEVECTOR2(x0, y0),
			D3DXFROMWINEVECTOR2(x1, y1),
			D3DXFROMWINEVECTOR2(x0, y1),
		};
		DrawPrimitiveUP(PRIM_TRIANGLES, 2, vertices, vertices, sizeof(PCVertex));
#endif
#endif
	return 0;
}

unsigned int RendererHLSLCg::drawRoundRectVrtl(const float x0, const float y0, const float x1, const float y1, const float r, const D3DXFROMWINEVECTOR4 &color, const float lineWidth){
	unsigned int col = toBGRA(color);

	bool lines = (lineWidth > 0);
	float d = 0.5f * lineWidth;

	PCVertex vertices[42];

	int k = 0;
	// Four arcs
	for (int j = 0; j < 4; j++){
		// Arc center
		float x = (j >= 1 && j <= 2)? x0 + r : x1 - r;
		float y = (j > 1)? y0 + r : y1 - r;

		// Arc
		for (int i = 0; i <= 4; i++){
			float ang = (4 * j + i) * 2 * D3DXFROMWINE_PI / 16;
			float cosA = cosf(ang);
			float sinA = sinf(ang);

			if (lines){
				vertices[2 * k    ].vertex = D3DXFROMWINEVECTOR2(x + (r + d) * cosA, y + (r + d) * sinA);
				vertices[2 * k + 1].vertex = D3DXFROMWINEVECTOR2(x + (r - d) * cosA, y + (r - d) * sinA);
				//vertices[2 * k    ].color = color;
				//vertices[2 * k + 1].color = color;
			} else {
				vertices[k].vertex = D3DXFROMWINEVECTOR2(x + r * cosA, y + r * sinA);
				//vertices[k].color = color;
			}
			k++;
		}
	}

	InitPlain(plainShader, plainVF);
	setShader(plainShader);
	setVertexFormat(plainVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", color);
	setDepthState(GetnoDepthTest());
	if (lines){
		// Full loop
		vertices[40] = vertices[0];
		vertices[41] = vertices[1];
		DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 40, vertices, vertices, sizeof(PCVertex));
	} else {
		DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 18, vertices, vertices, sizeof(PCVertex));
	}
	return 0;
}
Primitives g_prim=PRIM_TRIANGLES;
D3DXFROMWINEVECTOR4 g_color(1.0f,1.0f,1.0f,1.0f);
//PCTVertex* g_vertices=0;
std::vector<D3DXFROMWINEVECTOR4> g_colors;
std::vector<D3DXFROMWINEVECTOR3> g_vertices;
std::vector<D3DXFROMWINEVECTOR2> g_texCoords;

void RendererHLSLCg::Color3f(float x,float  y, float  z){g_colors.push_back(D3DXFROMWINEVECTOR4(x,y,z,1.0f));}
void RendererHLSLCg::Color4f(float x,float  y, float  z ,float  w){g_colors.push_back(D3DXFROMWINEVECTOR4(x,y,z,1.0f));}//w);}
void RendererHLSLCg::Color3fv(const float* v){g_colors.push_back(D3DXFROMWINEVECTOR4(v[0],v[1],v[2],1.0f));}
void RendererHLSLCg::Color4fv(const float* v){g_colors.push_back(D3DXFROMWINEVECTOR4(v[0],v[1],v[2],v[3]));}//v[3]);}

void RendererHLSLCg::Vertex2fv(const float* v){g_vertices.push_back(D3DXFROMWINEVECTOR3(v[0],v[1],0.0f));}
void RendererHLSLCg::Vertex3fv(const float* v){g_vertices.push_back(D3DXFROMWINEVECTOR3(v[0],v[1],v[2]));}
void RendererHLSLCg::Vertex4fv(const float* v){g_vertices.push_back(D3DXFROMWINEVECTOR3(v[0],v[1],v[2]));}


void RendererHLSLCg::Begin(Primitives prim)
{
	g_prim=prim;
}
void RendererHLSLCg::Vertex2i(int x,int y)
{
	g_vertices.push_back(D3DXFROMWINEVECTOR3(x,y,0));
}
void RendererHLSLCg::Vertex3i(int x,int y, int z)
{
	g_vertices.push_back(D3DXFROMWINEVECTOR3(x,y,x));
}
void RendererHLSLCg::Vertex4i(int x,int y, int z ,int w)
{
	g_vertices.push_back(D3DXFROMWINEVECTOR3(x,y,z));
}
void RendererHLSLCg::Vertex2f(float x,float  y)
{
	g_vertices.push_back(D3DXFROMWINEVECTOR3(x,y,0));
}
void RendererHLSLCg::Vertex3f(float x,float  y, float  z)
{
	g_vertices.push_back(D3DXFROMWINEVECTOR3(x,y,z));
}
void RendererHLSLCg::Vertex4f(float x,float  y, float  z ,float  w)
{
	g_vertices.push_back(D3DXFROMWINEVECTOR3(x,y,z));
}
void RendererHLSLCg::TexCoord2f(float x,float  y)
{
	g_texCoords.push_back(D3DXFROMWINEVECTOR2(x,y));
}
unsigned int RendererHLSLCg::getIndicesCount(const Primitives primitives){
	switch (primitives){
		case PRIM_TRIANGLES:
			return 3;
		case PRIM_TRIANGLE_FAN:
                        return 3;
		case PRIM_TRIANGLE_STRIP:
			return 3;
    		case PRIM_QUADS:
    			return 4;
		case PRIM_LINES:
			return 2;
		case PRIM_LINE_STRIP:
			return 2;
    		case PRIM_LINE_LOOP:
    			return 0;
		case PRIM_POINTS:
			return 1;
		};
	return 0;
}
unsigned int RendererHLSLCg::getPrimitiveCount(const Primitives primitives, const int count){
	switch (primitives){
		case PRIM_TRIANGLES:
			return count / 3;
		case PRIM_TRIANGLE_FAN:
                        return count - 2;
		case PRIM_TRIANGLE_STRIP:
			return count - 2;
    		case PRIM_QUADS:
    			return count / 4;
		case PRIM_LINES:
			return count / 2;
		case PRIM_LINE_STRIP:
			return count - 1;
    		case PRIM_LINE_LOOP:
    			return 0;
		case PRIM_POINTS:
			return count;
		};
	return 0;
}
unsigned int RendererHLSLCg::getPrimitiveCountR(const Primitives primitives, const int count){
	switch (primitives){
		case PRIM_TRIANGLES:
			return count * 3;
		case PRIM_TRIANGLE_FAN:
			return count + 2;
		case PRIM_TRIANGLE_STRIP:
			return count +  2;
        	case PRIM_QUADS:
        		return count * 4;
		case PRIM_LINES:
			return count * 2;
		case PRIM_LINE_STRIP:
			return count + 1;
        	case PRIM_LINE_LOOP:
        		return 0;
		case PRIM_POINTS:
			return count;
		};
	return 0;
}
unsigned int RendererHLSLCg::getVertexCount(const Primitives primitives, const int  NumPrim)
{
	switch(primitives)
	{
		case PRIM_TRIANGLES:		return NumPrim * 3;
		case PRIM_TRIANGLE_FAN: 	return NumPrim + 2;
		case PRIM_TRIANGLE_STRIP:	return NumPrim + 2;
		case PRIM_QUADS:		return NumPrim * 4;
		case PRIM_LINES:		return NumPrim * 2;
		case PRIM_LINE_STRIP:		return NumPrim + 1;
		case PRIM_LINE_LOOP:		return 0;
		case PRIM_POINTS:		return NumPrim;
	default:				return 0;
	}
	return 0;
}


void RendererHLSLCg::EndVrtl(const TextureID id, const D3DXFROMWINEMATRIX* pMVP, const char* aXFile, const char* aTextureFile)
{
	std::vector<PCNTVertex> PCTvertices;
	std::vector<PCNVertex> PCvertices;
	std::vector<PCNTVertex3> PCTvertices3;
	std::vector<PCNVertex3> PCvertices3;

	if(pMVP)
	{
		if(g_texCoords.size())
		{
			PCTvertices3.resize(g_vertices.size());
			InitTex3N(texShader3, texVF3);
			setShader(texShader3);
			setVertexFormat(texVF3);
			for(unsigned int i=0;i<g_vertices.size();i++)
			{
				PCTvertices3[i].vertex=g_vertices.at(i);
				if(i<g_texCoords.size())PCTvertices3[i].texCoord=D3DXFROMWINEVECTOR2(
					g_texCoords.at(i).x,
					1.0f-g_texCoords.at(i).y//???-1
				);
				if(i<g_colors.size())PCTvertices3[i].color=g_colors.at(i);
				else PCTvertices3[i].color=g_color;
			}
		}
		else
		{
			PCvertices3.resize(g_vertices.size());
			InitPlain3N(plainShader3, plainVF3);
			setShader(plainShader3);
			setVertexFormat(plainVF3);
			for(unsigned int i=0;i<g_vertices.size();i++)
			{
				PCvertices3[i].vertex=g_vertices.at(i);
				if(i<g_colors.size())PCvertices3[i].color=g_colors.at(i);
				else PCvertices3[i].color=g_color;
			}
		}
	}
	else
	{
		if(g_texCoords.size())
		{
			PCTvertices.resize(g_vertices.size());
			InitTexN(texShader, texVF);
			setShader(texShader);
			setVertexFormat(texVF);
			for(unsigned int i=0;i<g_vertices.size();i++)
			{
				PCTvertices[i].vertex=D3DXFROMWINEVECTOR2(g_vertices.at(i).x, g_vertices.at(i).y);
				if(i<g_texCoords.size())PCTvertices[i].texCoord=D3DXFROMWINEVECTOR2(
					g_texCoords.at(i).x,
					1.0f-g_texCoords.at(i).y//???-1
				);
				if(i<g_colors.size())PCTvertices[i].color=g_colors.at(i);
				else PCTvertices[i].color=g_color;
			}
		}
		else
		{
			PCvertices.resize(g_vertices.size());
			InitPlainN(plainShader, plainVF);
			setShader(plainShader);
			setVertexFormat(plainVF);
			for(unsigned int i=0;i<g_vertices.size();i++)
			{
				PCvertices[i].vertex=D3DXFROMWINEVECTOR2(g_vertices.at(i).x, g_vertices.at(i).y);
				if(i<g_colors.size())PCvertices[i].color=g_colors.at(i);
				else PCvertices[i].color=g_color;
			}
		}
	}

	int nn=g_vertices.size();
/*
	if(g_prim==PRIM_TRIANGLE_STRIP)
		nn=g_vertices.size()-2;
*/

	int count=g_vertices.size();
	switch (g_prim){
		case PRIM_POINTS:
			nn=count;
			break;
		case PRIM_LINES:
			//LOG_PRINT_NONE("PRIM_LINES\n");
			nn=count / 2;
			break;
		case PRIM_LINE_STRIP:
			nn=count - 1;
			break;
		case PRIM_TRIANGLES:
			nn=count / 3;
			break;
		case PRIM_QUADS:
			//LOG_PRINT_NONE("PRIM_QUADS\n");
#ifdef _MSC_VER
			g_prim=PRIM_TRIANGLE_FAN;
			nn=count - 2;
#else
			g_prim=PRIM_QUADS;
			nn=count / 4;
#endif
			break;
		case PRIM_TRIANGLE_STRIP:
			nn=count - 2;
			break;
				case PRIM_TRIANGLE_FAN:
						nn=count - 2;
						break;
        case PRIM_LINE_LOOP:
        break;
		};

	/*getPrimitiveCountR(g_prim,
		g_vertices.size());
	if(g_prim==PRIM_LINES)
		nn=g_vertices.size();
*/
	/*
	switch (g_prim){
		case PRIM_POINTS:
			return g_vertices.size();
		case PRIM_LINES:
			return g_vertices.size() * 2;
				case PRIM_LINE_STRIP:
			return g_vertices.size() + 1;
		case PRIM_TRIANGLES:
			return g_vertices.size() * 3;
		case PRIM_TRIANGLE_STRIP:
			return g_vertices.size();// - 2;
				case PRIM_TRIANGLE_FAN:
						return g_vertices.size() + 2;
		};

*/
/*
	switch(g_prim)
	{
		case PRIM_LINES:
			LOG_PRINT_X("\n",);
			break;
	}:
*/

			#if 0
			LOG_FNLN_NONE;
			LOG_PRINT_NONE("nn:%d\n",nn);
			#endif
	#if 0
	if(aXFile)
	{
		std::vector<D3DXFROMWINEVECTOR3> g_normals;
		std::vector<__WORD__> g_indices;

	std::vector<__WORD__> v;	
	for(unsigned int i=0;i<m_Turtle.mIndices.size()/3;i++)
		v.push_back(0);
	std::vector<Texture_> Textures;
	Textures.push_back(std::make_tuple(aTextureFile, v));

	stx_writeXFile(aXFile, g_vertices, g_texCoords, Textures);

	}
	#endif
	g_vertices.clear();
	g_texCoords.clear();
	g_colors.clear();
	if(pMVP)
	{
		setShaderConstant4x4f("worldViewProj", *pMVP);
		if(id!=-1)setTexture("Base",id,(*GetTexture(id)).ss);
		if(PCTvertices3.size())
		{			
			//LOG_PRINT_NONE("PCTvertices3.size()=%d\n", PCTvertices3.size());
			DrawPrimitiveUP(g_prim,
			nn, &PCTvertices3[0], &PCTvertices3[0], sizeof(PCNTVertex3));

			#if 0
			LOG_PRINT_NONE("PCTvertices3.size()=%d\n", PCTvertices3.size());
			for(unsigned int z=0;z<PCTvertices3.size();z++)
			{
				LOG_PRINT_NONE("PCTvertices3[%d].vertex.x=%f\n", z, PCTvertices3[z].vertex.x);
				LOG_PRINT_NONE("PCTvertices3[%d].vertex.y=%f\n", z, PCTvertices3[z].vertex.y);
				LOG_PRINT_NONE("PCTvertices3[%d].vertex.z=%f\n", z, PCTvertices3[z].vertex.z);
			}
			#endif

		}
		else
		{
			//LOG_PRINT_NONE("PCvertices3.size()=%d\n", PCvertices3.size());
			DrawPrimitiveUP(g_prim,
			nn, &PCvertices3[0], &PCvertices3[0], sizeof(PCNVertex3));

			#if 0
			LOG_PRINT_NONE("PCvertices3.size()=%d\n", PCvertices3.size());
			for(unsigned int z=0;z<PCvertices3.size();z++)
			{
				LOG_PRINT_NONE("PCvertices3[%d].vertex.x=%f\n", z, PCvertices3[z].vertex.x);
				LOG_PRINT_NONE("PCvertices3[%d].vertex.y=%f\n", z, PCvertices3[z].vertex.y);
				LOG_PRINT_NONE("PCvertices3[%d].vertex.z=%f\n", z, PCvertices3[z].vertex.z);
			}
			#endif
		}
	}
	else
	{
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	D3DXFROMWINEVECTOR4 col(1.0f, 1.0f, 1.0f, 1.0f);
	//setShaderConstant4f("colorRGBA", col);
	if(id!=-1)setTexture("Base",id,(*GetTexture(id)).ss);

	//setBlendState(GetblendSrcAlpha());
	setDepthState(GetnoDepthTest());
		if(PCTvertices.size())
		{
			//LOG_PRINT_NONE("PCTvertices.size()=%d\n", PCTvertices.size());			
			DrawPrimitiveUP(g_prim,
			nn, &PCTvertices[0], &PCTvertices[0], sizeof(PCNTVertex));

			#if 0
			LOG_PRINT_NONE("PCTvertices.size()=%d\n", PCTvertices.size());
			for(unsigned int z=0;z<PCTvertices.size();z++)
			{
				LOG_PRINT_NONE("PCTvertices[%d].vertex.x=%f\n", z, PCTvertices[z].vertex.x);
				LOG_PRINT_NONE("PCTvertices[%d].vertex.y=%f\n", z, PCTvertices[z].vertex.y);
			}
			#endif
		}
		else
		{
			//LOG_PRINT_NONE("PCvertices.size()=%d\n", PCvertices.size());
			DrawPrimitiveUP(g_prim,
			nn, &PCvertices[0], &PCvertices[0], sizeof(PCNVertex));

			#if 0
			LOG_PRINT_NONE("PCvertices.size()=%d\n", PCvertices.size());
			for(unsigned int z=0;z<PCvertices.size();z++)
			{
				LOG_PRINT_NONE("PCvertices[%d].vertex.x=%f\n", z, PCvertices[z].vertex.x);
				LOG_PRINT_NONE("PCvertices[%d].vertex.y=%f\n", z, PCvertices[z].vertex.y);
			}
			#endif
		}
/*
	int i1=getPrimitiveCount(g_prim,g_vertices.size());
	int i2=getPrimitiveCountR(g_prim,g_vertices.size());
	LOG_PRINT_X("PC=%d\n",getPrimitiveCount(g_prim,g_vertices.size()));
	LOG_PRINT_X("PCR=%d\n",getPrimitiveCountR(g_prim,g_vertices.size()));
	*/
	}}
unsigned int RendererHLSLCg::drawCircleVrtl(const float x, const float y, const float r, const D3DXFROMWINEVECTOR4 &color, const float lineWidth){
	unsigned int col = toBGRA(color);

	bool lines = (lineWidth > 0);
	float d = 0.5f * lineWidth;

	PCVertex vertices[34];

	for (int i = 0; i < 16; i++){
		float ang = i * 2 * D3DXFROMWINE_PI / 16;
		float cosA = cosf(ang);
		float sinA = sinf(ang);

		if (lines){
			vertices[2 * i    ].vertex = D3DXFROMWINEVECTOR2(x + (r + d) * cosA, y + (r + d) * sinA);
			vertices[2 * i + 1].vertex = D3DXFROMWINEVECTOR2(x + (r - d) * cosA, y + (r - d) * sinA);
			//vertices[2 * i    ].color = color;
			//vertices[2 * i + 1].color = color;
		} else {
			vertices[i].vertex = D3DXFROMWINEVECTOR2(x + r * cosA, y + r * sinA);
			//vertices[i].color = color;
		}
	}

	InitPlain(plainShader, plainVF);
		setShader(plainShader);
		setVertexFormat(plainVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", color);
	setDepthState(GetnoDepthTest());
	unsigned int ret=0;
	if (lines){
		// Full loop
		vertices[32] = vertices[0];
		vertices[33] = vertices[1];
		ret=DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 32, vertices, vertices, sizeof(PCVertex));
	} else {
		ret=DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 14, vertices, vertices, sizeof(PCVertex));
	}
	return ret;
}

unsigned int RendererHLSLCg::drawTriangleVrtl(const float x0, const float y0, const float x1, const float y1, const float x2, const float y2, const D3DXFROMWINEVECTOR4 &color, const float lineWidth){
	unsigned int col = toBGRA(color);
	unsigned int ret=0;
	if (lineWidth > 0){
		drawLine(x0, y0, x1, y1, color, lineWidth);
		drawLine(x1, y1, x2, y2, color, lineWidth);
		drawLine(x2, y2, x0, y0, color, lineWidth);
	} else {
		PCVertex vertices[3] = {
			D3DXFROMWINEVECTOR2(x0, y0),// color,
			D3DXFROMWINEVECTOR2(x1, y1),// color,
			D3DXFROMWINEVECTOR2(x2, y2),// color,
		};
	InitPlain(plainShader, plainVF);
	setShader(plainShader);
	setVertexFormat(plainVF);
	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", color);
	setDepthState(GetnoDepthTest());
	ret=DrawPrimitiveUP(PRIM_TRIANGLES, 1, vertices, vertices, sizeof(PCVertex));
	}
	return ret;
}

unsigned int RendererHLSLCg::drawQuadVrtl(const float x0, const float y0, const float x1, const float y1, const float s0, const float t0, const float s1, const float t1, const D3DXFROMWINEVECTOR4 &color){
	unsigned int col = toBGRA(color);
	unsigned int ret=0;
	#ifndef _MSC_VER
	PCTVertex vertices[4] = {
		D3DXFROMWINEVECTOR2(x0, y0),
		//color,
		D3DXFROMWINEVECTOR2(s0, t0),
		D3DXFROMWINEVECTOR2(x1, y0),
		//color,
		D3DXFROMWINEVECTOR2(s1, t0),
		D3DXFROMWINEVECTOR2(x1, y1),
		//color,
		D3DXFROMWINEVECTOR2(s1, t1),
		D3DXFROMWINEVECTOR2(x0, y1),
		//color,
		D3DXFROMWINEVECTOR2(s0, t1),
	};
	#else
	PCTVertex vertices[6] = {
		D3DXFROMWINEVECTOR2(x1, y0),
		D3DXFROMWINEVECTOR2(s1, t0),
		D3DXFROMWINEVECTOR2(x1, y1),
		D3DXFROMWINEVECTOR2(s1, t1),
		D3DXFROMWINEVECTOR2(x0, y0),
		D3DXFROMWINEVECTOR2(s0, t0),
		D3DXFROMWINEVECTOR2(x0, y0),
		D3DXFROMWINEVECTOR2(s0, t0),
		D3DXFROMWINEVECTOR2(x1, y1),
		D3DXFROMWINEVECTOR2(s1, t1),
		D3DXFROMWINEVECTOR2(x0, y1),
		D3DXFROMWINEVECTOR2(s0, t1),
	};
	#endif
#if 0
	InitPixel(texShader, texVF);
		setShader(texShader);
		setVertexFormat(texVF);
#else
	InitPlain(plainShader, plainVF);
		setShader(plainShader);
		setVertexFormat(plainVF);
#endif

	setup2DMode(0, (float) GetViewportWidth(), 0, (float) GetViewportHeight());
	setShaderConstant4f("scaleBias", GetscaleBias2D());
	setShaderConstant4f("colorRGBA", color);
	setDepthState(GetnoDepthTest());
	#ifdef _MSC_VER
	ret=DrawPrimitiveUP(PRIM_TRIANGLES, 2, vertices, vertices, sizeof(PCTVertex));
	#else
	ret=DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(PCTVertex));
	#endif
	return ret;
}

#endif


static FORMAT formats3[] =
{
	FORMAT_NONE	 ,
	FORMAT_R8	 ,
	FORMAT_RG8	 ,
	FORMAT_RGB8	 ,
	FORMAT_RGBA8	 ,
	FORMAT_R16	 ,
	FORMAT_RG16	 ,
	FORMAT_RGB16	 ,
	FORMAT_RGBA16	 ,
	FORMAT_R8S	 ,
	FORMAT_RG8S	 ,
	FORMAT_RGB8S	 ,
	FORMAT_RGBA8S	 ,
	FORMAT_R16S	 ,
	FORMAT_RG16S	 ,
	FORMAT_RGB16S	 ,
	FORMAT_RGBA16S	 ,
	FORMAT_R16f	 ,
	FORMAT_RG16f	 ,
	FORMAT_RGB16f	 ,



	FORMAT_RGBA16f	 ,
	FORMAT_R32f	 ,
	FORMAT_RG32f	 ,
	FORMAT_RGB32f	 ,
	FORMAT_RGBA32f	 ,
	FORMAT_R16I	 ,
	FORMAT_RG16I	 ,
	FORMAT_RGB16I	 ,
	FORMAT_RGBA16I	 ,
	FORMAT_R32I	 ,
	FORMAT_RG32I	 ,
	FORMAT_RGB32I	 ,
	FORMAT_RGBA32I	 ,
	FORMAT_R16UI	 ,
	FORMAT_RG16UI	 ,
	FORMAT_RGB16UI	 ,
	FORMAT_RGBA16UI  ,
	FORMAT_R32UI	 ,
	FORMAT_RG32UI	 ,
	FORMAT_RGB32UI	 ,
	FORMAT_RGBA32UI  ,
	FORMAT_RGBE8	 ,
	FORMAT_RGB9E5	 ,
	FORMAT_RG11B10F  ,
	FORMAT_RGB565	 ,
	FORMAT_RGBA4	 ,
	FORMAT_RGB10A2	 ,
	FORMAT_D16	 ,
	FORMAT_D24	 ,
	FORMAT_D24S8	 ,
	FORMAT_D32F	 ,

	FORMAT_DXT1	 ,
	FORMAT_DXT3	 ,
	FORMAT_DXT5	 ,
	FORMAT_ATI1N	 ,
	FORMAT_ATI2N   ,


	FORMAT_RGB8	 ,
	FORMAT_RGB8	 ,
	FORMAT_RGBA8	 ,

}
;
static D3DFORMAT formats3d3d9[] =
{
	D3DFMT_UNKNOWN,
	D3DFMT_L8,
	D3DFMT_A8L8,
	D3DFMT_X8R8G8B8,
	D3DFMT_A8R8G8B8,
	D3DFMT_L16,
	D3DFMT_G16R16,
	D3DFMT_UNKNOWN, D3DFMT_A16B16G16R16,
	D3DFMT_UNKNOWN,
	D3DFMT_V8U8,
	D3DFMT_UNKNOWN,
	D3DFMT_Q8W8V8U8,
	D3DFMT_UNKNOWN,
	D3DFMT_V16U16,
	D3DFMT_UNKNOWN,
	D3DFMT_Q16W16V16U16,
	D3DFMT_R16F,
	D3DFMT_G16R16F,
	D3DFMT_UNKNOWN, D3DFMT_A16B16G16R16F,
	D3DFMT_R32F,
	D3DFMT_G32R32F,
	D3DFMT_UNKNOWN, D3DFMT_A32B32G32R32F,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN, D3DFMT_UNKNOWN, D3DFMT_UNKNOWN, D3DFMT_R5G6B5,
	D3DFMT_A4R4G4B4,
	D3DFMT_A2B10G10R10,
	D3DFMT_D16,
	D3DFMT_D24X8,
	D3DFMT_D24S8,
	D3DFMT_D32F_LOCKABLE,
	D3DFMT_DXT1,
	D3DFMT_DXT3,
	D3DFMT_DXT5,
#ifdef _WIN32
	(D3DFORMAT) '1ITA',
	(D3DFORMAT) '2ITA',
#else
	D3DFMT_UNKNOWN,
	D3DFMT_UNKNOWN,
#endif
	D3DFMT_R8G8B8,
//	D3DFMT_B8G8R8,
	D3DFMT_X8B8G8R8,
	D3DFMT_A8B8G8R8,

}
;
FORMAT FORMAT3FromD3D (UINT D3DFORMAT )

{
	for(unsigned int i=0;i<59;i++)

		if(D3DFORMAT==formats3d3d9[i]) return formats3[i];
	return FORMAT_NONE;
}

/*






	PRIM_TRIANGLES	    = 0,
	PRIM_TRIANGLE_FAN   = 1,
	PRIM_TRIANGLE_STRIP = 2,
	PRIM_QUADS	    = 3,
	PRIM_LINES	    = 4,
	PRIM_LINE_STRIP     = 5,
	PRIM_LINE_LOOP	    = 6,
	PRIM_POINTS	    = 7,


	GL_TRIANGLES,
	GL_TRIANGLE_FAN,
	GL_TRIANGLE_STRIP,
	GL_QUADS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_POINTS,

		pt=D3DPT_TRIANGLELIST;
		pt=D3DPT_TRIANGLEFAN;
		pt=D3DPT_TRIANGLESTRIP;
		pt=(D3DPRIMITIVETYPE)0;
		pt=D3DPT_LINELIST;
		pt=D3DPT_LINESTRIP;
		pt=(D3DPRIMITIVETYPE)0;
		pt=D3DPT_POINTLIST;

		D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED, D3D10_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP,
		D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED, D3D10_PRIMITIVE_TOPOLOGY_POINTLIST,

		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED, D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
*/
#if 0
void RendererHLSLCg::XXX(Primitives prim,Primitives& rprim, int& rnv,BYTE& rb, int& rvs)
{
		BYTE* bi=0;
		int is=0;
		int ni=0;
			if(selectedIndexBuffer!=-1)
			{
			bi=indexBuffers[selectedIndexBuffer].Lock();
			is=indexBuffers[selectedIndexBuffer].indexSize;
			ni=indexBuffers[selectedIndexBuffer].nIndices;
			}

		BYTE* bv=vertexBuffers[selectedVertexBuffer].Lock();//all vertex buffers???
		vs=vertexBuffers[selectedVertexBuffer].indexSize;
		nv=vertexBuffers[selectedVertexBuffer].nIndices;
		VertexBufferID vb;
		if(prim==PRIM_QUADS)
		{
		if(!bi)
		{
			b=new BYTE[vs*nv];
			stx_memcpy(b,bv,vs*nv);
			rnv=nv;
		}
		else
		{
			int ic=getIndicesCount(prim);
			rnv=getVertexCount(prim,getPrimitiveCount(ni))*getPrimitiveCount(prim,ni);
			b=new BYTE[vs*rnv];
			__DWORD__ dwi=0;
			__WORD__ wi=0;
			BYTE* d;
			for(unsigned int i=0;i<ni;i=i+ic)
			{
				if(is==4)
				{
					d=&bv[vs*dwi];
					stx_memcpy(b+dwi*vs,d,vs*ic);
					dwi++;
				}
				else
				{
					d=&bv[vs*wi];
					stx_memcpy(b+wi*vs,d,vs*ic);
					wi++;
				}
							}
		}
			rprim=PRIM_TRIANGLE_STRIP;
			rb=b;
			rvs=vs;
		}
		else if(prim=PRIM_LINE_LOOP)
		{
		if(!bi)
		{
			b=new BYTE[(vs+1)*nv];
			stx_memcpy(b,bv,vs*nv);
			stx_memcpy(b+vs*nv,bv,vs);
			rnv=nv+1;
		}
		else
		{
			int ic=getIndicesCount(prim);
			rnv=1+getVertexCount(prim,getPrimitiveCount(ni))*getPrimitiveCount(prim,ni);
			b=new BYTE[vs*rnv];
			__DWORD__ dwi=0;
			__WORD__ wi=0;
			BYTE* d;

			for(unsigned int i=0;i<ni;i=i+ic)
			{
				if(is==4)
				{
					d=&bv[vs*dwi];
					stx_memcpy(b+dwi*vs,d,vs*ic);
					dwi++;
				}
				else
				{
					d=&bv[vs*wi];
					stx_memcpy(b+wi*vs,d,vs*ic);
					wi++;
				}
			stx_memcpy(b+vs*nv,bv,vs*is);
		}
		}
}

void Rendererdraw(Primitives prim)
{
			BYTE* b;
		int vs;
		int nv;
		Primitives& rprim;
		int rnv;
		BYTE rb;
		int rvs;
			if((!dynamic_cast<RendererGL_1_1*>(this))&&(prim==PRIM_LINE_LOOP))
			{
				XXX(prim,rprim, rnv,rb, rvs);
			}
		if((!dynamic_cast<RendererGL_1_1*>(this))&&(prim==PRIM_QUADS))
		{
			XXX(prim,rprim, rnv,rb, rvs);
							DrawPrimitiveUP(PRIM_TRIANGLE_STRIP,nv,b,b,vs);
		}
		//else
		//DrawIndexedPrimitiveUP(PRIM_TRIANGLE_STRIP,0,nv,np,bi,bi,f,b,b,vs);
		/*
    void DrawIndexedPrimitiveUP(Primitives PrimitiveType,
		UINT MinVertexIndex,
		UINT NumVertices,
		UINT PrimitiveCount,
		CONST void* pIndexData0,
		CONST void* pIndexData,
		STX_INDEXFORMAT IndexDataFormat,
		CONST void* pVertexStreamZeroData0,
		CONST void* pVertexStreamZeroData,
		UINT VertexStreamZeroStride, const unsigned int flags=RESET_ALL);
		*/
		}
}

#endif

#if defined(ANDROID)
#elif defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
#elif defined(__APPLE__)
#include <stdio.h>
#include <mach-o/dyld.h>
#include <libgen.h>
#endif
#if defined(USE_GLES_SHADERS) || defined(IPHONE_SIMULATOR) || defined(OS_IPHONE) || defined(ANDROID)
#include <Framework3/RendererGLES2.h>
#include <Framework3/RendererGLES_3_2.h>
#include <Framework3/RendererMetal.h>
#else
#include <Framework3/RendererHLSLD3D12.h>
#include <Framework3/RendererHLSLD3D11.h>
#include <Framework3/RendererHLSLD3D10.h>
#include <Framework3/RendererHLSLD3D9.h>
#include <Framework3/RendererGLSLGL_1_1.h>
#include <Framework3/RendererGLSLGL_4_5.h>
#include <Framework3/RendererVulkan.h>
#include <Framework3/RendererCgGL_1_1.h>
#endif
std::string IRenderer::m_title="";
std::string IRenderer::Str="False_";
IRenderer* IRenderer::GetRendererInstance(
		const char* atitle, tRenderer art,
			bool fullScreen
		,int argc, char *argv[])
{
	if(IRenderer::s_pRenderer)
	{
		return IRenderer::s_pRenderer;
	}

	m_title=atitle;
	LOG_START;
	#if defined(LINUX) || defined(__APPLE__)
	STX_PRINT("\nTitle: %s\n", atitle);
	#endif
	char path1[MAX_PATH];
	stx_getcwd(path1);
	LOG_PRINT_NONE("1path=%s\n", path1);
	std::string str=path1;

//1path=/home/tronty/X4/IndieLibX/scripting
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
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
#endif

#if defined(OS_IPHONE) || defined(IPHONE_SIMULATOR) || defined(ANDROID)
#else // defined(__APPLE__)
	char *slash = stx_strrchr(str.c_str(), '/');
	slash = stx_strrchr(slash, '/');
	LOG_PRINT_NONE("1slash=%s\n", slash);
if(stx_strcmp(slash, "/scripting")!=0)
#endif
{
	// Make sure we're running in the exe's path
	char path[MAX_PATH];
	path[0]='\0';
	char *slash = 0;
#if defined(ANDROID)
#elif defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
#elif defined(__APPLE__)
	uint32_t bufsize = MAX_PATH;
	_NSGetExecutablePath(path, &bufsize);
	chdir(path);
	//LOG_PRINT_NONE("chdir(%s)\n", path);
#elif defined(LINUX)
	// Make sure we're running in the exe's directory
	if (realpath("/proc/self/exe", path)){
		//LOG_PRINT_NONE("path=%s\n", path);
		char *slash = stx_strrchr(path, '/');
		if (slash) *slash = '\0';
		chdir(path);
		//LOG_PRINT_NONE("chdir(%s)\n", path);
	}
#elif defined(_MSC_VER)
	if (GetModuleFileName(NULL, path, sizeof(path)))
	{
		char *slash = stx_strrchr(path, '\\');
		//LOG_PRINT_NONE("path=%s\n", path);
		if (slash) *slash = '\0';
        	chdir(path);
		//LOG_PRINT_NONE("chdir(%s)\n", path);
	}
#endif
}

	char path2[MAX_PATH];
	stx_getcwd(path2);
	LOG_PRINT_NONE("2path=%s\n", path2);

	std::string title_=atitle;
        STX_Service::GetWindowInstance()->ReadConfigurationFile(title_);

	IRenderer::width=STX_Service::GetWindowInstance()->GetWidth();
	IRenderer::height=STX_Service::GetWindowInstance()->GetHeight();

	#if 0
	char path[MAX_PATH];
	stx_getcwd(path);
	LOG_FNLN_NONE;
	LOG_PRINT_NONE("path=%s\n", path);

	if(!(//(	stx_strcmp(path,"~/IndieLibX/scripting")==0) &&
		stx_fileExists("./IndieLib.pyc") &&
		stx_fileExists("./_IndieLib.so") &&
		stx_fileExists("./IndieLib_wrap.o") &&
		stx_fileExists("./IndieLib_wrap.cxx") &&
		stx_fileExists("./IndieLib.py")))
	#endif
	std::string Renderer = "GLSL1_1";
	{
	std::string Debug = "False";
#ifndef USEXML
	m_bLuaScript=_IRenderer_script.Load("/Framework3/Framework3.lua");
	if(!m_bLuaScript)
	{
		Debug = _IRenderer_script.get<std::string>("Debug");
		Renderer = _IRenderer_script.get<std::string>("Renderer");
	}
#else
{
	std::string fn=stx_convertpath("/Framework3/Framework3.xml");
    TiXmlDocument doc(fn.c_str());
    if (!doc.LoadFile())
	{//LOG_PRINT("!doc.LoadFile(%s)\n", fn.c_str());
      stx_exit(0);}
	#ifdef _MSC_VER
	static 
	#endif
	TiXmlHandle docHandle(&doc);
    Renderer = docHandle.FirstChild("Body").FirstChild("Renderer").Element()->Attribute("Value");
}
#endif
		if(Debug=="True")
			m_bDebug=true;
	}
	printf("Renderer=%s\n", Renderer.c_str());
#if 0
	unsigned int c=1;
	while(c<2)
	{
#ifdef __Framework3GLES3_2_H__
	IRenderer::s_pRenderer=new RendererGLES3_2(fullScreen,argc,argv);
	if(IRenderer::s_pRenderer->IsOK()) break;
#endif
#ifdef  __FrameworkGLES2_H__
	IRenderer::s_pRenderer=new RendererGLES2(fullScreen,argc,argv);
	if(IRenderer::s_pRenderer->IsOK()) break;
#endif
#if defined(__Framework3Vulkan_H__)		
	IRenderer::s_pRenderer=new RendererVulkan(fullScreen,argc,argv);
	if(IRenderer::s_pRenderer->IsOK()) break;
#endif
#if defined(__Framework3Metal_H__)		
	IRenderer::s_pRenderer=new RendererMetal(fullScreen,argc,argv);
	if(IRenderer::s_pRenderer->IsOK()) break;
#endif
#if defined(__Framework3D3D12_H__)		
	IRenderer::s_pRenderer=new RendererHLSLD3D12(fullScreen,argc,argv);
	if(IRenderer::s_pRenderer->IsOK()) break;
#endif
#if defined(__Framework3D3D11_H__)		
	IRenderer::s_pRenderer=new RendererHLSLD3D11(fullScreen,argc,argv);
	if(IRenderer::s_pRenderer->IsOK()) break;
#endif
#if defined(__Framework3D3D10_H__)		
	IRenderer::s_pRenderer=new RendererHLSLD3D10(fullScreen,argc,argv);
	if(IRenderer::s_pRenderer->IsOK()) break;
#endif
#if defined(__Framework3D3D9_H__)		
	IRenderer::s_pRenderer=new RendererHLSLD3D9(fullScreen,argc,argv);
	if(IRenderer::s_pRenderer->IsOK()) break;
#endif
#ifdef __Framework3GL4_5_H__
	IRenderer::s_pRenderer=new RendererGL4_5(fullScreen,argc,argv);
	if(IRenderer::s_pRenderer->IsOK()) break;
#endif
#ifdef __FrameworkCgGL_1_1_H__
	IRenderer::s_pRenderer=new RendererCgGL_1_1(fullScreen,argc,argv);
	if(IRenderer::s_pRenderer->IsOK()) break;
#endif
#ifdef  __FrameworkGLSLGL_1_1_H__
	IRenderer::s_pRenderer=new RendererGLSLGL_1_1(fullScreen,argc,argv);
	if(IRenderer::s_pRenderer->IsOK()) break;
#endif
	c++;
	stx_exit(0);
	}
#else
	#ifdef  __FrameworkGLES2_H__
	//if(Renderer=="GLES2")
	{
		IRenderer::s_pRenderer=new RendererGLES2(fullScreen,argc,argv);
		if(!IRenderer::s_pRenderer->IsOK()) stx_exit(0);
	}
	#else
	#ifdef  __FrameworkGLSLGL_1_1_H__
	if(Renderer=="GLSL1_1")
	{
		IRenderer::s_pRenderer=new RendererGLSLGL_1_1(fullScreen,argc,argv);
		if(!IRenderer::s_pRenderer->IsOK()) stx_exit(0);
	}
	#endif
	#ifdef __FrameworkCgGL_1_1_H__
	if(Renderer=="GLCG1_1")
	{
		IRenderer::s_pRenderer=new RendererCgGL_1_1(fullScreen,argc,argv);
		if(!IRenderer::s_pRenderer->IsOK()) stx_exit(0);
	}
	#endif
	#if defined(__Framework3D3D11_H__)
	if(Renderer=="D3D11")
	{
		IRenderer::s_pRenderer=new RendererHLSLD3D11(fullScreen,argc,argv);
		if(!IRenderer::s_pRenderer->IsOK()) stx_exit(0);
	}
	#endif
	#endif
	if(!IRenderer::s_pRenderer)
		stx_exit(0);
#endif
	char buf[1024] = {0};
	stx_strlcpy(buf, "Framework3 - ", 1024);
	stx_strlcat(buf, atitle, 1024);
	stx_strlcat(buf, " - ", 1024);
	stx_strlcat(buf, IRenderer::s_pRenderer->getString(), 1024);

	if(sizeof(void *) == 4)
		stx_strlcat(buf, " - 32 bit", 1024);
	else if(sizeof(void *) == 8)
		stx_strlcat(buf, " - 64 bit", 1024);

	STX_Service::GetWindowInstance()->SetCaption(buf);
	IRenderer::s_pRenderer->setViewport(0, 0, 
		STX_Service::GetWindowInstance()->Getwidth(), 
		STX_Service::GetWindowInstance()->Getheight());
{
	std::string fn=stx_convertpath("/Framework3/Framework3.xml");
    TiXmlDocument doc(fn.c_str());
    if (!doc.LoadFile())
	{//LOG_PRINT("!doc.LoadFile(%s)\n", fn.c_str());
      stx_exit(0);}
	#ifdef _MSC_VER
	static 
	#endif
	TiXmlHandle docHandle(&doc);
    Str = docHandle.FirstChild("Body").FirstChild("Debug").Element()->Attribute("Value");
}

	#if !defined(ANDROID) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
	LOG_PRINT("SHADING_LANGUAGE_VERSION=(%d, %d)\n", IRenderer::s_pRenderer->GetSHADING_LANGUAGE_VERSION_MAJOR(), IRenderer::s_pRenderer->GetSHADING_LANGUAGE_VERSION_MINOR());
	#elif 0
	LOG_PRINT("SHADING_LANGUAGE_VERSION=(2, 0)\n");
	#endif

	return IRenderer::s_pRenderer;
}
#if 0
	const char* IRenderer::getString()
	{
		static char buf[256] = {0};
		static char os[10] = {0};
#if defined(WIN_PHONE_APP)
        stx_snprintf(os,256,"Windows Phone\0");
#elif defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
        stx_snprintf(os,256,"iPhone\0");
#elif defined(ANDROID)
        stx_snprintf(os,256,"android\0");
#elif defined(QT_BUILD)
        stx_snprintf(os,256,"Symbian\0");
#elif defined(_WIN32) || defined(WIN8_APP)
        stx_snprintf(os,256,"Windows\0");
#elif defined(__GNUC__) && !defined(__MINGW32__)
        stx_snprintf(os,256,"Linux\0");
#endif

#if 0
#if !defined(SDL_2_0_5_BUILD)
#if defined( __Framework3D3D9_H__)
    if(dynamic_cast<RendererHLSLD3D9*>(IRenderer::GetRendererInstance())) stx_snprintf(buf,256,"%s D3D9\0", os);
#elif defined(__Framework3D3D10_H__)
    if(dynamic_cast<RendererHLSLD3D10*>(IRenderer::GetRendererInstance())) stx_snprintf(buf,256,"%s D3D10\0", os);
#elif defined(__Framework3D3D11_H__)
    if(dynamic_cast<RendererHLSLD3D11*>(IRenderer::GetRendererInstance())) stx_snprintf(buf,256,"%s D3D11\0", os);
#elif defined(__FrameworkGLSLGL_1_1_H__)
    if(dynamic_cast<RendererGLSLGL_1_1*>(IRenderer::GetRendererInstance())) stx_snprintf(buf,256,"%s GLSLGL_1_1\0", os);
#elif defined(__FrameworkGLES2_H__)
    if(dynamic_cast<RendererGLES2*>(IRenderer::GetRendererInstance())) stx_snprintf(buf,256,"%s GLES2\0", os);
#elif defined(__FrameworkCgGL_1_1_H__)
    if(dynamic_cast<RendererCgGL_1_1*>(IRenderer::GetRendererInstance())) stx_snprintf(buf,256,"%s CgGL_1_1\0", os);
	#endif

#else
stx_snprintf(buf,256,"%s GLSLGL_1_1\0", os);
#endif
#endif
	return buf;
	}
#endif

IRenderer* IRenderer::s_pRenderer=0;

int RendererHLSLCg::addAnimation(char* afile){return -1;}
int RendererHLSLCg::addMesh(char* afile){return -1;}
int RendererHLSLCg::addSound(char* afile, char* aname){return -1;}

void RendererHLSLCg::deleteAnimation(char* afile){}
void RendererHLSLCg::deleteMesh(char* afile){}

void RendererHLSLCg::deleteSound(char* afile, char* aname){}

void RendererHLSLCg::renderAnimation(int id, int x, int y, int sx, int sy){}
void RendererHLSLCg::renderMesh(int id, D3DXFROMWINEMATRIX mvp){}
void RendererHLSLCg::playSound(int id){}
void RendererHLSLCg::stopSound(int id){}

CTimer::CTimer(){mStartTime=0;}
CTimer::~CTimer(){}
CTimer::CTimer(const CTimer& rhs){mStartTime=rhs.mStartTime;}
float CTimer::GetTime()
{
	if(mStartTime>0)
		return 1000.0f*(timeGetTime()-mStartTime);
	return 0.0f;
}
void CTimer::Begin(){mStartTime=timeGetTime();}
void CTimer::Stop(){mStartTime=0;}
bool CTimer::IsRunning(){return (mStartTime>0);}
void CTimer::UpdateAll(float aElapsedTime)
{
	//???
}

CAnimSequence::CAnimSequence(){}
CAnimSequence::~CAnimSequence(){}
CAnimSequence::CAnimSequence(const CAnimSequence& rhs)
{
	m_Sequence=rhs.m_Sequence;
	m_Duration=rhs.m_Duration;
	m_Then=rhs.m_Then;
}
void CAnimSequence::AddFrame(const char* aImageFileName, float aDuration)
{
	TextureID id=IRenderer::GetRendererInstance()->addImageLibTexture(aImageFileName, false, 
			IRenderer::GetRendererInstance()->Getlinear());
	m_Sequence.push_back(id);
	m_Duration.push_back(aDuration);
	m_Then.push_back(0.0f);
}

unsigned int CAnimSequence::Size()
{
	return m_Sequence.size();
}

CSprite::CSprite()
{
	m_Size=1.0f;
	m_index=0;
	m_dt=0;
}
CSprite::~CSprite(){}
CSprite::CSprite(const CSprite& rhs){m_Size=rhs.m_Size;m_Sequence=rhs.m_Sequence;}
void CSprite::Render(D3DFROMWINEMATRIX& aViewMatrix)
{
	if(!m_Sequence.Size()) return;
	if(m_index>(m_Sequence.Size()-1))
		m_index=0;

	IRenderer::GetRendererInstance()->InitTex(IRenderer::GetRendererInstance()->GettexShader(),IRenderer::GetRendererInstance()->GettexVF());	
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GettexVF());

	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias", IRenderer::GetRendererInstance()->GetscaleBias2D());
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1.0f,1.0f,1.0f,1.0f));
	//IRenderer::GetRendererInstance()->setShaderConstant1f("invertY", 1.0f);

	IRenderer::GetRendererInstance()->setTexture("Base", m_Sequence.m_Sequence[m_index]);
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullBack());
	//		   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));


	__DWORD__ now=timeGetTime();
	__DWORD__ dt=now-m_Sequence.m_Then[m_index];
	if (dt>m_dt)
	{
		m_dt+=m_Sequence.m_Duration[m_index];
		m_index++;
		m_Sequence.m_Then[m_index]=now;
	}
}
void CSprite::SetAnim(CAnimSequence& aAnim){m_Sequence=aAnim;}
void CSprite::SetSize(float aSize){m_Size=aSize;}

/*
GL_LINE_LOOP,
GL_QUADS,
D3DPT_TRIANGLEFAN

     __DWORD__   MaxPrimitiveCount;          // max number of primitives per DrawPrimitive call
    __DWORD__   MaxVertexIndex;
    __DWORD__   MaxStreams;
    __DWORD__   MaxStreamStride;            // max stride for SetStreamSource

GL_MAX_ELEMENTS_INDICES
GL_MAX_ELEMENTS_VERTICES

# GL_LINE_STRIP— Use this primitive to draw a sequence of connected line segments. OpenGL renders a line segment between the first and second vertices, between the second and third, between the third and fourth, and so on. If the application specifies n vertices, OpenGL renders n–1 line segments.
# GL_LINE_LOOP— Use this primitive to close a line strip. OpenGL renders this primitive like a GL_LINE_STRIP with the addition of a closing line segment between the final and first vertices.

A triangle fan is similar to a triangle strip, except that all the triangles share one vertex, as shown in the following illustration.
The system uses vertices v2, v3, and v1 to draw the first triangle; v3, v4, and v1 to draw the second triangle; v4, v5, and v1 to draw the third triangle; and so on. When flat shading is enabled, the system shades the triangle with the color from its first vertex.

	GL_TRIANGLES,
	GL_TRIANGLE_FAN,
	GL_TRIANGLE_STRIP,
	GL_QUADS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_POINTS,

		pt=D3DPT_TRIANGLELIST;
		pt=D3DPT_TRIANGLEFAN;
		pt=D3DPT_TRIANGLESTRIP;
		pt=(D3DPRIMITIVETYPE)0;
		pt=D3DPT_LINELIST;
		pt=D3DPT_LINESTRIP;
		pt=(D3DPRIMITIVETYPE)0;
		pt=D3DPT_POINTLIST

		D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED, 	D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED, 			D3D10_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP,
		D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED, D3D10_PRIMITIVE_TOPOLOGY_POINTLIST,
*/
/*
GL_MAX_3D_TEXTURE_SIZE





                            params returns one value,
                            a rough estimate of the largest 3D texture that the GL can handle.
                            The value must be at least 16.
                            If the GL version is 1.2 or greater, use
                            GL_PROXY_TEXTURE_3D to determine if a texture is too large.
                            See glTexImage3D.

GL_MAX_CLIENT_ATTRIB_STACK_DEPTH





                            params returns one value indicating the maximum supported depth
                            of the client attribute stack.
                            See glPushClientAttrib.

GL_MAX_ATTRIB_STACK_DEPTH





                            params returns one value,
                            the maximum supported depth of the attribute stack. The value must be
                            at least 16.
                            See glPushAttrib.

GL_MAX_CLIP_PLANES





                            params returns one value,
                            the maximum number of application-defined clipping planes. The value must be at least 6.
                            See glClipPlane.

GL_MAX_COLOR_MATRIX_STACK_DEPTH





                            params returns one value, the maximum supported depth of the color matrix
                            stack.  The value must be at least 2.
                            See glPushMatrix.

GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS





                            params returns one value, the maximum supported texture image units that
                            can be used to access texture maps from the vertex shader and the fragment processor combined.
                            If both the vertex shader and the fragment processing stage access the same texture image
                            unit, then that counts as using two texture image units against this limit.
                            The value must be at least 2.
                            See glActiveTexture.

GL_MAX_CUBE_MAP_TEXTURE_SIZE





                            params returns one value.
                            The value gives a rough estimate of the largest cube-map texture that
                            the GL can handle. The value must be at least 16.
                            If the GL version is 1.3 or greater, use GL_PROXY_TEXTURE_CUBE_MAP
                            to determine if a texture is too large.
                            See glTexImage2D.

GL_MAX_DRAW_BUFFERS





                            params returns one value, the maximum number
                            of simultaneous output colors allowed from a fragment shader using the
                            gl_FragData built-in array.  The value must be at least 1.
                            See glDrawBuffers.

GL_MAX_ELEMENTS_INDICES





                            params returns one value,
                            the recommended maximum number of vertex array indices.
                            See glDrawRangeElements.

GL_MAX_ELEMENTS_VERTICES





                            params returns one value,
                            the recommended maximum number of vertex array vertices.
                            See glDrawRangeElements.

GL_MAX_EVAL_ORDER





                            params returns one value,
                            the maximum equation order supported by 1D and 2D
                            evaluators. The value must be at least 8.

                            See glMap1 and glMap2.

GL_MAX_FRAGMENT_UNIFORM_COMPONENTS





                            params returns one value,
                            the maximum number of individual floating-point, integer, or boolean values that can be held
                            in uniform variable storage for a fragment shader. The value must be at least 64.
                            See glUniform.

GL_MAX_LIGHTS





                            params returns one value,
                            the maximum number of lights. The value must be at least 8.
                            See glLight.

GL_MAX_LIST_NESTING





                            params returns one value,
                            the maximum recursion depth allowed during display-list
                            traversal. The value must be at least 64.
                            See glCallList.

GL_MAX_MODELVIEW_STACK_DEPTH





                            params returns one value,
                            the maximum supported depth of the modelview matrix stack. The value must
                            be at least 32.
                            See glPushMatrix.

GL_MAX_NAME_STACK_DEPTH





                            params returns one value,
                            the maximum supported depth of the selection name stack. The value must be at least 64.
                            See glPushName.

GL_MAX_PIXEL_MAP_TABLE





                            params returns one value,
                            the maximum supported size of a glPixelMap lookup table.
                            The value must be at least 32.
                            See glPixelMap.

GL_MAX_PROJECTION_STACK_DEPTH





                            params returns one value, the maximum supported depth of the projection
                            matrix stack. The value must be at least 2.
                            See glPushMatrix.

GL_MAX_TEXTURE_COORDS





                            params returns one value,
                            the maximum number of texture coordinate sets available to vertex and fragment shaders.
                            The value must be at least 2.
                            See glActiveTexture and
                            glClientActiveTexture.

GL_MAX_TEXTURE_IMAGE_UNITS





                            params returns one value, the maximum supported texture image units that
                            can be used to access texture maps from the fragment shader.
                            The value must be at least 2.
                            See glActiveTexture.

GL_MAX_TEXTURE_LOD_BIAS





                            params returns one value,
                            the maximum, absolute value of the texture level-of-detail bias.  The
                            value must be at least 4.

GL_MAX_TEXTURE_SIZE





                            params returns one value.
                            The value gives a rough estimate of the largest texture that
                            the GL can handle. The value must be at least 64.
                            If the GL version is 1.1 or greater, use
                            GL_PROXY_TEXTURE_1D or GL_PROXY_TEXTURE_2D
                            to determine if a texture is too large.
                            See glTexImage1D and glTexImage2D.

GL_MAX_TEXTURE_STACK_DEPTH





                            params returns one value,
                            the maximum supported depth of the texture matrix stack. The value must be at least 2.
                            See glPushMatrix.

GL_MAX_TEXTURE_UNITS





                            params returns a single value indicating the number of conventional
                            texture units supported. Each conventional texture unit includes both a texture coordinate set
                            and a texture image unit.  Conventional texture units may be used for fixed-function (non-shader)
                            rendering.  The value must be at least 2.  Additional texture coordinate sets and texture
                            image units may be accessed from vertex and fragment shaders.
                            See glActiveTexture and
                            glClientActiveTexture.

GL_MAX_VARYING_FLOATS





                            params returns one value,
                            the maximum number of interpolators available for processing varying variables used by
                            vertex and fragment shaders. This value represents the number of individual floating-point
                            values that can be interpolated; varying variables declared as vectors, matrices, and arrays
                            will all consume multiple interpolators. The value must be at least 32.

GL_MAX_VERTEX_ATTRIBS





                            params returns one value,
                            the maximum number of 4-component generic vertex attributes accessible to a vertex shader.
                            The value must be at least 16.
                            See glVertexAttrib.

GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS





                            params returns one value, the maximum supported texture image units that
                            can be used to access texture maps from the vertex shader. The value may be 0.
                            See glActiveTexture.

GL_MAX_VERTEX_UNIFORM_COMPONENTS





                            params returns one value,
                            the maximum number of individual floating-point, integer, or boolean values that can be held
                            in uniform variable storage for a vertex shader. The value must be at least 512.
                            See glUniform.

GL_MAX_VIEWPORT_DIMS





                            params returns two values:
                            the maximum supported width and height of the viewport.
                            These must be at least as large as the visible dimensions of the display
                            being rendered to.
                            See glViewport.

*/
stx_Effect::stx_Pass::stx_Texture_::stx_Texture_()
{
	//<Texture name="Base" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
	texID=-1;
        m_iDimension=2;
        m_bUseMipmap=false;
        ss=-1;
}
stx_Effect::stx_Pass::stx_Texture_::~stx_Texture_(){}
stx_Effect::stx_Pass::stx_Texture_::stx_Texture_(const stx_Texture_& rhs)
{
	m_sName=rhs.m_sName;
        m_sFile=rhs.m_sFile;
	texID=rhs.texID;
        m_iDimension=rhs.m_iDimension;
        m_bUseMipmap=rhs.m_bUseMipmap;
        ss=rhs.ss;
}
stx_Effect::stx_Pass::stx_Pass()
		{
			m_pEffect=0;
			Shader=-1;
        		VertexFormat=-1;
			SamplerState=-1;
			BlendState=-1;
			AlphaState=-1;
        		DepthStencilState=-1;
        		RasterizerState=-1;
		}
stx_Effect::stx_Pass::stx_Pass(const char * aFileName,
const char *vsMain, const char *gsMain, const char *fsMain, const char *csMain, const char *hsMain, const char *dsMain)
		{
			Shader=-1;
        		VertexFormat=-1;
			SamplerState=-1;
			BlendState=-1;
			AlphaState=-1;
        		DepthStencilState=-1;
        		RasterizerState=-1;
			#if 1
			Shader=IRenderer::GetRendererInstance()->addShaderFromFile(aFileName, vsMain, fsMain);
			#else
			Shader=IRenderer::GetRendererInstance()->addShaderFromFile(aFileName, vsMain, gsMain, fsMain, csMain, hsMain, dsMain);
			#endif
		}
stx_Effect::stx_Pass::~stx_Pass(){}
		void stx_Effect::stx_Pass::Apply(unsigned int aI)
		{
			#if 1
			unsigned int cPasses=0;
    			unsigned int Value=0;
			unsigned int iPass=aI; // ???
			m_pEffect->Begin(&cPasses, Value);
			m_pEffect->BeginPass(iPass);
			#endif
		}
		stx_Effect::stx_Pass::stx_Pass(const stx_Pass& rhs)
		{
			m_pEffect=rhs.m_pEffect;
			//m_Technique=rhs.m_Technique;
			//m_vTextures=rhs.m_vTextures;
			m_sName=rhs.m_sName;
			Shader=rhs.Shader;

			PixelShaderprofile=rhs.PixelShaderprofile;
			PixelShadermain=rhs.PixelShadermain;
			VertexShaderprofile=rhs.VertexShaderprofile;
			VertexShadermain=rhs.VertexShadermain;
			HullShaderprofile=rhs.HullShaderprofile;
			HullShadermain=rhs.HullShadermain;
			DomainShaderprofile=rhs.DomainShaderprofile;
			DomainShadermain=rhs.DomainShadermain;
			GeometryShaderprofile=rhs.GeometryShaderprofile;
			GeometryShadermain=rhs.GeometryShadermain;
			ComputeShaderprofile=rhs.ComputeShaderprofile;
			ComputeShadermain=rhs.ComputeShadermain;

        		VertexFormat=rhs.VertexFormat;
        		format_=rhs.format_;
			SamplerState=rhs.SamplerState;
			SamplerName=rhs.SamplerName;
			BlendState=rhs.BlendState;
			AlphaState=rhs.AlphaState;
        		DepthStencilState=rhs.DepthStencilState;
        		RasterizerState=rhs.RasterizerState;
		}
		stx_Effect::stx_Variable::stx_Variable()
		{
			m_texID=-1;
			m_iLocation=-1;
			NElements=0;
        		Type=0;
		}
		stx_Effect::stx_Variable::~stx_Variable(){}
		stx_Effect::stx_Variable::stx_Variable(const stx_Variable& rhs)
		{
			m_iLocation=rhs.m_iLocation;
			m_texID=rhs.m_texID;
            		m_sName=rhs.m_sName;
		}
	std::string stx_Effect::stx_Variable::GetName(){return m_sName;}
        TextureID stx_Effect::stx_Variable::GetTextureID(){return m_texID;}
        int stx_Effect::stx_Variable::GetLocation(){return m_iLocation;}
	void stx_Effect::stx_Variable::SetResourceArray( ShaderResourceViewID* id, unsigned int startindex, unsigned int aN )
	{
		for(unsigned int i=startindex;i<aN;i++)
		{
			//int getSamplerIndex(ShaderID shd, const char* aName);
			//SetTexture(id[i]); // sampler name ???
		}
	}
	void stx_Effect::stx_Variable::SetResource( ShaderResourceViewID id ){SetTexture(id);} // sampler name ???
        void stx_Effect::stx_Variable::SetFloat( float aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstant1f(m_sName.c_str(), aValue);
        }
        void stx_Effect::stx_Variable::SetFloatVector( float* aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstantArray4f(m_sName.c_str(), (D3DXFROMWINEVECTOR4*)aValue, elementsOf(aValue));
        }
        void stx_Effect::stx_Variable::SetFloatVectorArray( float* aValue, unsigned int aN)
        {
	        IRenderer::GetRendererInstance()->setShaderConstantArray4f(m_sName.c_str(), (D3DXFROMWINEVECTOR4*)aValue, aN);
        }
        void stx_Effect::stx_Variable::SetIntVectorArray( int* aValue, unsigned int aS, unsigned int aN )
        {
	        IRenderer::GetRendererInstance()->setShaderConstantRaw(m_sName.c_str(), (int*)aValue, elementsOf(aValue));
        }
        void stx_Effect::stx_Variable::SetIntVector( int* aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstantRaw(m_sName.c_str(), (int*)aValue, elementsOf(aValue));
        }
	void stx_Effect::stx_Variable::SetFloatArray( float* aValue, unsigned int aS, unsigned int aN )
        {
	        IRenderer::GetRendererInstance()->setShaderConstantArray1f(m_sName.c_str(), aValue, aN);
        }
        void stx_Effect::stx_Variable::SetMatrix( float* aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstant4x4f(m_sName.c_str(), *((D3DXFROMWINEMATRIX*)aValue));
        }
        void stx_Effect::stx_Variable::SetMatrix( D3DXFROMWINEMATRIX* aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstant4x4f(m_sName.c_str(), *aValue);
        }
        void stx_Effect::stx_Variable::SetVector( D3DXFROMWINEVECTOR4* aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstant4f(m_sName.c_str(), *aValue);
        }
		void stx_Effect::stx_Variable::SetInt(int aValue)
		{
			IRenderer::GetRendererInstance()->setShaderConstant1i(m_sName.c_str(), aValue);
		}
        void stx_Effect::stx_Variable::SetInt( int* aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstant1i(m_sName.c_str(), *aValue);
        }
        void stx_Effect::stx_Variable::SetValue( void* aPtr, unsigned int aSize )
        {
	        IRenderer::GetRendererInstance()->setShaderConstantRaw(m_sName.c_str(), &aPtr, aSize);
        }
        void stx_Effect::stx_Variable::SetTexture( TextureID aValue )
        {
        	m_texID=aValue;
	        IRenderer::GetRendererInstance()->setTexture(m_sName.c_str(), aValue);
        }
        void stx_Effect::stx_Variable::SetBool( bool aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstantRaw(m_sName.c_str(), &aValue, sizeof(bool));
        }
        void stx_Effect::stx_Variable::SetMatrixArray( void* aPtr, unsigned int aSize, unsigned int aN)
	{
	        IRenderer::GetRendererInstance()->setShaderConstantRaw(m_sName.c_str(), &aPtr, sizeof(D3DXFROMWINEMATRIX)*aN);
	}
		stx_Effect::stx_Technique::stx_Technique(stx_Effect* pEffect){m_pEffect=pEffect;}
		stx_Effect::stx_Technique::~stx_Technique(){}
		stx_Effect::stx_Technique::stx_Technique(const stx_Technique& rhs)
		{
			m_pEffect=rhs.m_pEffect;
			m_sName=rhs.m_sName;
			m_vPasses=rhs.m_vPasses;
		}
		const char* stx_Effect::stx_Technique::GetName(){return m_sName.c_str();}
		void stx_Effect::stx_Technique::GetDesc( TECHNIQUE_DESC* techDesc ){if(techDesc)techDesc->Passes=m_vPasses.size();}
		stx_Effect::stx_Pass* stx_Effect::stx_Technique::GetPassByIndex(unsigned int aI)
		{
			if(!m_vPasses.size())
				return 0;
			if((aI+1)>m_vPasses.size())
				return 0;
			return &m_vPasses[aI];
		} // ???
		stx_Effect::stx_Pass* stx_Effect::stx_Technique::GetPassByName(const std::string& aName)
		{
			if(!m_vPasses.size())
				return 0;
			int idx=-1;
			for(unsigned int i=0;i<m_vPasses.size();i++)
			{
				if(m_vPasses[i].m_sName==aName)
				{
					idx=i;
					break;
				}	
			}
			if(idx>-1)
				return &m_vPasses[idx];
			return 0;
		} // ???
	stx_Effect::stx_Effect(){m_iTechnique=-1;m_pFxRootElement=0;}
	stx_Effect::~stx_Effect(){}
	stx_Effect::stx_Effect(const stx_Effect& rhs)
	{
		docHandle=rhs.docHandle;
		m_sFileName=rhs.m_sFileName;
		m_sTechnique=rhs.m_sTechnique;
		m_iTechnique=rhs.m_iTechnique;
		m_vTechniques=rhs.m_vTechniques;
		m_vTextures=rhs.m_vTextures;
	}
        void stx_Effect::SetFloat( const char* aName, float aValue )
        {
	        IRenderer::GetRendererInstance()->setShaderConstant1f(aName, aValue);
        }
        void stx_Effect::SetFloat( const char* aName, float* aValue, const unsigned int N)
        {
        	switch(N)
        	{
        	case 1:
	        	IRenderer::GetRendererInstance()->setShaderConstant1f(aName, *aValue);
	        	break;
        	case 2:
        		{D3DXFROMWINEVECTOR2 v(*aValue, *(aValue+1));
	        	IRenderer::GetRendererInstance()->setShaderConstant2f(aName, v);}
	        	break;
        	case 3:
	        	{D3DXFROMWINEVECTOR3 v(*aValue, *(aValue+1), *(aValue+2));
	        	IRenderer::GetRendererInstance()->setShaderConstant3f(aName, v);}
	        	break;
        	case 4:
	        	{D3DXFROMWINEVECTOR4 v(*aValue, *(aValue+1), *(aValue+2), *(aValue+3));
	        	IRenderer::GetRendererInstance()->setShaderConstant4f(aName, v);}
	        	break;
        	default:
	        	IRenderer::GetRendererInstance()->setShaderConstantArray1f(aName, aValue, N);
	        	break;
	        };
        }

		stx_Effect::stx_Variable* stx_Effect::GetVariableByName( const char* aName )
		{
			if(aName)
			{
				stx_Effect::stx_Variable* v=new stx_Effect::stx_Variable();
				v->m_sName=aName;
				v->m_texID=-1; // ???
				v->m_iLocation=-1; // ???
				v->NElements=1; // ???
				v->Type=0; // ???
				return v;
			}
			return 0;
		} // ???
		
		stx_Effect::stx_Technique* stx_Effect::GetTechniqueByName( const char* aName )
		{
			if(!aName)
				return 0;
			if(!m_vTechniques.size())
				return 0;
			int idx=-1;
			for(unsigned int i=0;i<m_vTechniques.size();i++)
			{
				if(m_vTechniques[i].m_sName==std::string(aName))
				{
					idx=i;
					break;
				}	
			}
			if(idx>-1)
				return &m_vTechniques[idx];
			return 0;
		} // ???

void __GetSamplerState__(const std::string& aName, SamplerStateID& ss)
{
	ss=IRenderer::GetRendererInstance()->Getlinear();
        if(aName=="linear")
                ss=IRenderer::GetRendererInstance()->Getlinear();
        if(aName=="nearestClamp")
                ss=IRenderer::GetRendererInstance()->GetnearestClamp();
        if(aName=="linearClamp")
                ss=IRenderer::GetRendererInstance()->GetlinearClamp();
        if(aName=="bilinearClamp")
                ss=IRenderer::GetRendererInstance()->GetbilinearClamp();
        if(aName=="trilinearClamp")
                ss=IRenderer::GetRendererInstance()->GettrilinearClamp();
        if(aName=="nearest")
                ss=IRenderer::GetRendererInstance()->Getnearest();
        if(aName=="linearAniso")
                ss=IRenderer::GetRendererInstance()->GetlinearAniso();
        if(aName=="bilinearAniso")
                ss=IRenderer::GetRendererInstance()->GetbilinearAniso();
        if(aName=="trilinearAniso")
                ss=IRenderer::GetRendererInstance()->GettrilinearAniso();
}

void ParsePassElement(unsigned int j, TiXmlHandle& passHandle, stx_Effect::stx_Pass& pass)
{
LOG_FNLN;
	TiXmlElement* passElement = passHandle.Element();
	std::string n;
	if(passElement) n=passElement->Attribute("name");
        LOG_PRINT("Pass[%d]=%s\n", j, n.c_str());

	TiXmlElement* BlendStateElement=passHandle.FirstChild("BlendState").Element();
        //if(BlendStateElement){LOG_PRINT("BlendState=%s\n", BlendStateElement->Attribute("name"));}
	TiXmlElement* DepthStencilStateElement=passHandle.FirstChild("DepthStencilState").Element();
        //if(DepthStencilStateElement){LOG_PRINT("DepthStencilState=%s\n", DepthStencilStateElement->Attribute("name"));}
	TiXmlElement* RasterizerStateElement=passHandle.FirstChild("RasterizerState").Element();
        //if(RasterizerStateElement){LOG_PRINT("RasterizerState=%s\n", RasterizerStateElement->Attribute("name"));}
	TiXmlElement* PixelShaderElement=passHandle.FirstChild("PixelShader").Element();
        if(PixelShaderElement)
	{
		pass.PixelShadermain=PixelShaderElement->Attribute("name");
		pass.PixelShaderprofile=PixelShaderElement->Attribute("profile");
        	LOG_PRINT("pass.PixelShadermain=%s\n", pass.PixelShadermain.c_str());
	}
	TiXmlElement* VertexShaderElement=passHandle.FirstChild("VertexShader").Element();
        if(VertexShaderElement)
	{
		pass.VertexShadermain=VertexShaderElement->Attribute("name");
		pass.VertexShaderprofile=VertexShaderElement->Attribute("profile");
        	LOG_PRINT("pass.VertexShadermain=%s\n", pass.VertexShadermain.c_str());
	}
	TiXmlElement* GeometryShaderElement=passHandle.FirstChild("GeometryShader").Element();
        if(GeometryShaderElement)
	{
		pass.GeometryShadermain=GeometryShaderElement->Attribute("name");
		pass.GeometryShaderprofile=GeometryShaderElement->Attribute("profile");
	}
	TiXmlElement* HullShaderElement=passHandle.FirstChild("HullShader").Element();
        if(HullShaderElement)
	{
		pass.HullShadermain=HullShaderElement->Attribute("name");
		pass.HullShaderprofile=HullShaderElement->Attribute("profile");
	}
	TiXmlElement* DomainShaderElement=passHandle.FirstChild("DomainShader").Element();
        if(DomainShaderElement)
	{
		pass.DomainShadermain=DomainShaderElement->Attribute("name");
		pass.DomainShaderprofile=DomainShaderElement->Attribute("profile");
	}
	TiXmlElement* ComputeShaderElement=passHandle.FirstChild("ComputeShader").Element();
        if(ComputeShaderElement)
	{
		pass.ComputeShadermain=ComputeShaderElement->Attribute("name");
		pass.ComputeShaderprofile=ComputeShaderElement->Attribute("profile");
	}
	LOG_FNLN;
	TiXmlElement* StateElement=passHandle.FirstChild("DepthState").Element();
        if(StateElement)
        {
		LOG_FNLN;
        	LOG_PRINT("DepthState:\n");
        	LOG_PRINT("StateElement->Attribute(\"name\")=%s\n", StateElement->Attribute("name"));
            if(std::string(StateElement->Attribute("name"))==std::string("noDepthTest"))
                pass.DepthStencilState=IRenderer::GetRendererInstance()->GetnoDepthTest();
            if(std::string(StateElement->Attribute("name"))==std::string("noDepthWrite"))
                pass.DepthStencilState=IRenderer::GetRendererInstance()->GetnoDepthWrite();
        	LOG_PRINT("pass.DepthStencilState=%x\n", pass.DepthStencilState);
        	LOG_PRINT("IRenderer::GetRendererInstance()->GetnoDepthTest()=%x\n", IRenderer::GetRendererInstance()->GetnoDepthTest());
        }
        StateElement=passHandle.FirstChild("DepthStencilState").Element();
        if(StateElement)
        {
            if(std::string(StateElement->Attribute("name"))==std::string("noDepthTest"))
                pass.DepthStencilState=IRenderer::GetRendererInstance()->GetnoDepthTest();
            if(std::string(StateElement->Attribute("name"))==std::string("noDepthWrite"))
                pass.DepthStencilState=IRenderer::GetRendererInstance()->GetnoDepthWrite();
            if(std::string(StateElement->Attribute("name"))==std::string("noDepthStencil")) // ???
                pass.DepthStencilState=IRenderer::GetRendererInstance()->GetnoDepthWrite();
        }
        StateElement=passHandle.FirstChild("RasterizerState").Element();
        if(StateElement)
        {
            if(std::string(StateElement->Attribute("name"))==std::string("cullNone"))
                pass.RasterizerState=IRenderer::GetRendererInstance()->GetcullNone();
            if(std::string(StateElement->Attribute("name"))==std::string("cullBack"))
                pass.RasterizerState=IRenderer::GetRendererInstance()->GetcullBack();
            if(std::string(StateElement->Attribute("name"))==std::string("cullFront"))
                pass.RasterizerState=IRenderer::GetRendererInstance()->GetcullFront();
            if(std::string(StateElement->Attribute("name"))==std::string("NoCullMS"))
                pass.RasterizerState=IRenderer::GetRendererInstance()->GetcullNone();
            if(std::string(StateElement->Attribute("name"))==std::string("CullBackMS"))
                pass.RasterizerState=IRenderer::GetRendererInstance()->GetcullBack();
            if(std::string(StateElement->Attribute("name"))==std::string("fillSolid"))
                pass.RasterizerState=IRenderer::GetRendererInstance()->GetfillSolid();
            if(std::string(StateElement->Attribute("name"))==std::string("fillWireFrame"))
                pass.RasterizerState=IRenderer::GetRendererInstance()->GetfillWireFrame();
            if(std::string(StateElement->Attribute("name"))==std::string("Solid"))
                pass.RasterizerState=IRenderer::GetRendererInstance()->GetfillSolid();
            if(std::string(StateElement->Attribute("name"))==std::string("Wireframe"))
                pass.RasterizerState=IRenderer::GetRendererInstance()->GetfillWireFrame();
            if(std::string(StateElement->Attribute("name"))==std::string("RSSolid"))
                pass.RasterizerState=IRenderer::GetRendererInstance()->GetfillSolid();
            if(std::string(StateElement->Attribute("name"))==std::string("RSWireframe"))
                pass.RasterizerState=IRenderer::GetRendererInstance()->GetfillWireFrame();
            if(std::string(StateElement->Attribute("name"))==std::string("WireframeMS"))
                pass.RasterizerState=IRenderer::GetRendererInstance()->GetfillWireFrame();
        }
        StateElement=passHandle.FirstChild("BlendState").Element();
        if(StateElement)
        {
            if(std::string(StateElement->Attribute("name"))==std::string("blendSrcAlpha"))
                pass.BlendState=IRenderer::GetRendererInstance()->GetblendSrcAlpha();
            if(std::string(StateElement->Attribute("name"))==std::string("blendOneOne"))
                pass.BlendState=IRenderer::GetRendererInstance()->GetblendOneOne();
            if(std::string(StateElement->Attribute("name"))==std::string("blendOneOneReverseSubstract"))
                pass.BlendState=IRenderer::GetRendererInstance()->GetblendOneOneReverseSubstract();
            if(std::string(StateElement->Attribute("name"))==std::string("NoBlending"))
                pass.BlendState=-1;
        }
        StateElement=passHandle.FirstChild("AlphaState").Element();
        if(StateElement)
        {
            if(std::string(StateElement->Attribute("name"))==std::string("enablecAlpha"))
                pass.AlphaState=IRenderer::GetRendererInstance()->GetenableAlpha();
        }
	pass.m_sName=n;
}

void ParseFormatDescElement(unsigned int z, TiXmlHandle& FormatDescHandle, stx_Effect::stx_Pass& pass)
{
LOG_FNLN;
	TiXmlElement* FormatDescElement = FormatDescHandle.Element();
	if(!FormatDescElement) return;

		LOG_FNLN;
		LOG_PRINT("FormatDesc[%d].Type=%s\n", z, FormatDescElement->Attribute("Type"));
		LOG_PRINT("FormatDesc[%d].Format=%s\n", z, FormatDescElement->Attribute("Format"));

	int stream=0;
	/* AttributeType */ int type=TYPE_TEXCOORD;
	/* AttributeFormat */ int format=FORMAT_FLOAT;
	int size=2;

		format=FORMAT_FLOAT;
		size=2;
		type=TYPE_TEXCOORD;

		if(std::string(FormatDescElement->Attribute("Format"))==std::string("HALF1"))
		{
			format=FORMAT_HALF;
			size=1;

		}

		if(std::string(FormatDescElement->Attribute("Format"))==std::string("HALF2"))

		{

			format=FORMAT_HALF;

			size=2;

		}

		if(std::string(FormatDescElement->Attribute("Format"))==std::string("HALF3"))

		{

			format=FORMAT_HALF;

			size=3;

		}

		if(std::string(FormatDescElement->Attribute("Format"))==std::string("HALF4"))
		{
			format=FORMAT_HALF;
			size=4;
		}

		if(std::string(FormatDescElement->Attribute("Format"))==std::string("FLOAT1"))
		{
			format=FORMAT_FLOAT;
			size=1;
		}
		if(std::string(FormatDescElement->Attribute("Format"))==std::string("FLOAT2"))
		{
			format=FORMAT_FLOAT;
			size=2;
		}
		if(std::string(FormatDescElement->Attribute("Format"))==std::string("FLOAT3"))
		{
			format=FORMAT_FLOAT;
			size=3;
		}
        if(std::string(FormatDescElement->Attribute("Format"))==std::string("FLOAT4"))
        {
            format=FORMAT_FLOAT;
            size=4;
        }
        if(std::string(FormatDescElement->Attribute("Format"))==std::string("UBYTE1"))
		{
			format=FORMAT_UBYTE;
			size=1;
		}
		if(std::string(FormatDescElement->Attribute("Format"))==std::string("UBYTE2"))
		{
			format=FORMAT_UBYTE;
			size=2;
		}
		if(std::string(FormatDescElement->Attribute("Format"))==std::string("UBYTE3"))
		{
			format=FORMAT_UBYTE;

			size=3;
		}
		if(std::string(FormatDescElement->Attribute("Format"))==std::string("UBYTE4"))
		{
			format=FORMAT_UBYTE;
			size=4;
		}
		if(std::string(FormatDescElement->Attribute("Type"))==std::string("VERTEX"))
			type=TYPE_VERTEX;
		if(std::string(FormatDescElement->Attribute("Type"))==std::string("TEXCOORD"))
			type=TYPE_TEXCOORD;
		if(std::string(FormatDescElement->Attribute("Type"))==std::string("NORMAL"))
			type=TYPE_NORMAL;
		if(std::string(FormatDescElement->Attribute("Type"))==std::string("TANGENT"))
			type=TYPE_TANGENT;
		if(std::string(FormatDescElement->Attribute("Type"))==std::string("BINORMAL"))
			type=TYPE_BINORMAL;

		LOG_FNLN;
		LOG_PRINT("stream=%d\n", stream); 
		LOG_PRINT("type=%d\n", type);
		LOG_PRINT("format=%d\n", format);
		LOG_PRINT("size=%d\n", size);
		pass.format_.push_back(stream);
		pass.format_.push_back(type);
		pass.format_.push_back(format);
		pass.format_.push_back(size);
}

void ParseTextureElement(unsigned int i, TiXmlHandle& TextureHandle, stx_Effect::stx_Pass::stx_Texture_& texture)
{
LOG_FNLN;
	texture.m_iDimension=2;
	texture.texID=-1;
	texture.m_bUseMipmap=false;
	texture.ss=-1;
	TiXmlElement* textureElement = TextureHandle.Element();
	if(!textureElement) return;
		std::string n=textureElement->Attribute("sampler");
		std::string f=textureElement->Attribute("file");
        LOG_PRINT("Texture[%d]=%s, %s\n", i, n.c_str(), f.c_str());

	if(textureElement->Attribute("sampler"))
		texture.m_sName=std::string(textureElement->Attribute("sampler"));

	std::string ss;
	if(textureElement->Attribute("SamplerState"))
	{
		ss=textureElement->Attribute("SamplerState");
		__GetSamplerState__(ss, texture.ss);
	}

	if(textureElement->Attribute("file"))
		texture.m_sFile=std::string(textureElement->Attribute("file"));
	if(textureElement->Attribute("dimension"))
		texture.m_iDimension=atoi(textureElement->Attribute("dimension"));
	if(textureElement->Attribute("mipmap"))
	{
		if(std::string(textureElement->Attribute("mipmap"))=="true")
			texture.m_bUseMipmap=true;
		if(std::string(textureElement->Attribute("mipmap"))=="True")
			texture.m_bUseMipmap=true;
		if(std::string(textureElement->Attribute("mipmap"))=="TRUE")
			texture.m_bUseMipmap=true;
	}
	texture.texID=IRenderer::GetRendererInstance()->addImageLibTexture(texture.m_sFile.c_str(), texture.m_bUseMipmap, texture.ss);
}

void ParseTextureElement2(unsigned int i, TiXmlHandle& TextureHandle, stx_Effect::stx_Pass::stx_Texture_& texture_)
{
LOG_FNLN;
	TiXmlElement* textureElement = TextureHandle.Element();
	if(!textureElement) return;
		std::string n=textureElement->Attribute("sampler");
		std::string f=textureElement->Attribute("file");
        LOG_PRINT("Texture[%d]=%s, %s\n", i, n.c_str(), f.c_str());
//<Texture name="Base" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
        if(textureElement->Attribute("name"))
	        texture_.m_sName=textureElement->Attribute("name");
	    if(textureElement->Attribute("file"))
	        texture_.m_sFile=stx_convertpath(textureElement->Attribute("file"));
	    if(textureElement->Attribute("dimension"))
	        texture_.m_iDimension=atoi(textureElement->Attribute("dimension"));
        if(textureElement->Attribute("mipmap")){
        if(std::string(textureElement->Attribute("mipmap"))=="true")

                texture_.m_bUseMipmap=true;
        if(std::string(textureElement->Attribute("mipmap"))=="TRUE")
                texture_.m_bUseMipmap=true;}

        texture_.ss=IRenderer::GetRendererInstance()->Getlinear();
        if(textureElement->Attribute("SamplerState"))
        {
LOG_FNLN;
		std::string ss(textureElement->Attribute("SamplerState"));
		__GetSamplerState__(ss, texture_.ss);
        }
LOG_FNLN;
#if 0
        if(texture_.m_sFile.length())
	        texture_.texID=IRenderer::GetRendererInstance()->addImageLibTexture(
                texture_.m_sFile.c_str(), 
                texture_.m_bUseMipmap, 
                texture_.ss);
            LOG_PRINT("m_vTechniques.size()=%d\n", m_vTechniques.size());
            LOG_PRINT("m_vTechniques[%d].m_vPasses.size()=%d\n", i, m_vTechniques[i].m_vPasses.size());
        	LOG_PRINT("m_vTechniques[%d].m_vPasses[%d]\n", i, j);
	    m_vTechniques[i].m_vPasses[j].m_vTextures.push_back(texture_);
#endif
}

bool SplitString(const std::string& stringToBeSplitted, const std::string& delimeter, std::string& val1, std::string& val)
{
	int startIndex = 0;
	int  endIndex = 0;
	if( (endIndex = stringToBeSplitted.find(delimeter, startIndex)) < stringToBeSplitted.size() )
	{
		val1 = stringToBeSplitted.substr(0, endIndex);
		val = stringToBeSplitted.substr(endIndex+delimeter.length(), stringToBeSplitted.length());
        return true;
	}
	//LOG_PRINT("\nval1:\n%s\n", val1.c_str());
	//LOG_PRINT("\nval:\n%s\n", val.c_str());
    return false;
}
std::string stx_Effect::ParseEffect(const std::string& aFileName, const char* aDefines)
{
	m_sFileName = stx_convertpath(aFileName);
	m_iTechnique = -1;

	if(!stx_fileExists(m_sFileName.c_str()))
	{
		//LOG_PRINT("Title: %s\n", m_title.c_str());
		LOG_PRINT("stx_Effect::ParseEffect:5962:\n");
		LOG_PRINT("\tLoading effect file  %s failed\n", m_sFileName.c_str());
		DBG_HALT;
	}	

	LOG_FNLN;
	LOG_PRINT("\nfileName=%s\n", m_sFileName.c_str());
	std::string tmp;
	std::ifstream in(m_sFileName.c_str(), std::ios::in | std::ios::binary);
	if (in)
		tmp = (std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));

	std::string stringToBeSplitted;
	if (aDefines)
		stringToBeSplitted.append(aDefines);
	stringToBeSplitted.append(tmp);
	std::string delimeter = "[techniques]";
	int startIndex = 0;
	int  endIndex = 0;
	//std::string val1;
	//std::string val;
	if ((endIndex = stringToBeSplitted.find(delimeter, startIndex)) < stringToBeSplitted.size())
	{
		m_sFX = stringToBeSplitted.substr(0, endIndex);
		m_sXML = stringToBeSplitted.substr(endIndex + delimeter.length(), stringToBeSplitted.length());
	}
	LOG_PRINT("\nval1:\n%s\n", m_sFX.c_str());
	LOG_PRINT("\nval:\n%s\n", m_sXML.c_str());

	//{std::ofstream out("./__tmp__.fx");out << m_sFX;out.close();}

#if 0
	{std::ofstream out("./__tmp__.xml");
	out << "<?xml version=\"1.0\"?>\n";
	out << "<Body>\n";
	out << m_sXML;
	out << "\n</Body>\n";
	out.close();}
#endif
#if 0
	std::ifstream in2("./__tmp__.xml", std::ios::in | std::ios::binary);
  	if (in2)
    		val=(std::string((std::istreambuf_iterator<char>(in2)), std::istreambuf_iterator<char>()));
#endif
    LOG_PRINT("techniques:\n%s\n\n", m_sXML.c_str());
#if 0
    TiXmlDocument doc("./__tmp__.xml");
    if (!doc.LoadFile()){
	//LOG_PRINT("!doc.LoadFile(%s)\n", m_sFileName.c_str());
      stx_exit(0);}
	#ifdef _MSC_VER
	static 
	#endif
	TiXmlHandle doc_(&doc);docHandle=&doc_;
#else
	std::string filedata;
	filedata.append("<?xml version=\"1.0\"?>\n");
	filedata.append("<Body>\n");
	if(m_sXML.length()) filedata.append(m_sXML.c_str());
	filedata.append("\n</Body>\n");
    	m_sXML=filedata;
	doc.Parse(m_sXML.c_str(), 0, TIXML_ENCODING_UTF8);
	#ifdef _MSC_VER
	static 
	#endif
	TiXmlHandle doc_(&doc);
	docHandle=&doc_;
#endif
	int i=0;
	TiXmlHandle texturesHandle = docHandle->FirstChild("Body").FirstChild("Textures");
    {int i=1;
	TiXmlHandle textureHandle = texturesHandle.FirstChild("Texture");
	TiXmlElement* textureElement = textureHandle.Element();
	//TiXmlHandle textureHandle = docHandle->FirstChild("Body").FirstChild("Textures").Child("Texture", i++ ).Element();
	while ( textureElement )
	{
	stx_Effect::stx_Pass::stx_Texture_ texture;
    ParseTextureElement(i-1, textureHandle, texture);
	m_vTextures.push_back(texture);

	textureHandle = texturesHandle.Child( "Texture", i++ );
	textureElement = textureHandle.Element();
}}
	TiXmlHandle techniquesHandle = docHandle->FirstChild("Body").FirstChild("Techniques");
	i=1;
	TiXmlHandle techniqueHandle = techniquesHandle.FirstChild("technique");
	TiXmlElement* techniqueElement = techniqueHandle.Element();

	while ( techniqueElement )
	{
	int j=1;
	TiXmlHandle passHandle = techniqueHandle.FirstChild("pass");
	TiXmlElement* passElement = passHandle.Element();
	stx_Technique technique;
	while ( passElement )
	{
	stx_Pass pass;
	pass.m_pEffect=this;
	//pass.m_Technique=technique;
    ParsePassElement(j-1, passHandle, pass);
#if 1
	LOG_FNLN;
	TiXmlHandle vertexFormatHandle = passHandle.FirstChild("VertexFormat");
	int z=1;
	TiXmlHandle FormatDescHandle = vertexFormatHandle.FirstChild("FormatDesc");
	TiXmlElement* FormatDescElement = FormatDescHandle.Element();
	while ( FormatDescElement )
	{
        ParseFormatDescElement(z-1, FormatDescHandle, pass);

	FormatDescHandle = vertexFormatHandle.Child( "FormatDesc", z++ );
	FormatDescElement = FormatDescHandle.Element();
#endif
	}
	technique.m_vPasses.push_back(pass);
	passHandle = techniqueHandle.Child( "pass", j++ );
	passElement = passHandle.Element();
	}

	std::string n=techniqueElement->Attribute("name");
    LOG_PRINT("Technique[%d]=%s\n", i, n.c_str());

	technique.m_sName=n;
	technique.m_pEffect=this;
	m_vTechniques.push_back(technique);

	techniqueHandle = techniquesHandle.Child( "technique", i++ );
	techniqueElement = techniqueHandle.Element();
	}
	return m_sFX;
}
stx_Effect::stx_Effect(const std::string& aFileName, const char* aDefines)
{
	std::string val1=ParseEffect(aFileName, aDefines);
	//TiXmlHandle techniquesHandle = docHandle->FirstChild("Body").FirstChild("Techniques");

    	int i=1;
	TiXmlHandle techniquesHandle = docHandle->FirstChild("Body").FirstChild("Techniques");
	TiXmlHandle techniqueHandle = techniquesHandle.FirstChild("technique");
	TiXmlElement* techniqueElement = techniqueHandle.Element();
	while ( techniqueElement )
	{
	int j=0;
	TiXmlHandle passHandle = techniqueHandle.Child("pass", j++);
	TiXmlElement* passElement = passHandle.Element();
	while ( passElement )
	{
	int k=0;
	TiXmlHandle textureHandle = passHandle.Child("Texture", k++);
	TiXmlElement* textureElement = textureHandle.Element();
	while ( textureElement )
	{
        stx_Effect::stx_Pass::stx_Texture_ texture_;
        ParseTextureElement2(k-1, textureHandle, texture_);
	    textureHandle = passHandle.Child( "Texture", k++ );
	    textureElement = textureHandle.Element();
	}
	passHandle = techniqueHandle.Child( "pass", j++ );
	passElement = passHandle.Element();
	}
	techniqueHandle = techniquesHandle.Child( "technique", i++ );
	techniqueElement = techniqueHandle.Element();
	}
#if 1
    LOG_FNLN;
//stx_exit(0);
#endif
	if(!m_vTechniques.size()) return;
        m_sTechnique=m_vTechniques[0].m_sName;
	m_iTechnique=0;
    for(unsigned int i=0;i<m_vTechniques.size();i++)
    for(unsigned int j=0;j<m_vTechniques[i].m_vPasses.size();j++)
    {
	LOG_FNLN;
        LOG_PRINT("m_vTechniques[%d].m_vPasses[%d].VertexShadermain.length()=%d\n", i, j, m_vTechniques[i].m_vPasses[j].VertexShadermain.length());
        LOG_PRINT("m_vTechniques[%d].m_vPasses[%d].PixelShadermain.length()=%d\n", i, j, m_vTechniques[i].m_vPasses[j].PixelShadermain.length());
        LOG_PRINT("m_vTechniques[%d].m_vPasses[%d].VertexShadermain=%s\n", i, j, m_vTechniques[i].m_vPasses[j].VertexShadermain.c_str());
        LOG_PRINT("m_vTechniques[%d].m_vPasses[%d].PixelShadermain=%s\n", i, j, m_vTechniques[i].m_vPasses[j].PixelShadermain.c_str());
    }
	//stx_exit(0);
for(unsigned int i=0;i<m_vTechniques.size();i++)
{
for(unsigned int j=0;j<m_vTechniques[i].m_vPasses.size();j++)
{
#if 0
        std::string mainVS="mainVS";
        std::string mainPS="mainPS";
	std::string mainGS="mainGS";
	std::string mainCS="mainCS";
	std::string mainHS="mainHS";
	std::string mainDS="mainDS";
#else
        std::string mainVS=m_vTechniques[i].m_vPasses[j].VertexShadermain; // ??? PS or VS
        std::string mainPS=m_vTechniques[i].m_vPasses[j].PixelShadermain; // ??? VS or PS
	std::string mainGS=m_vTechniques[i].m_vPasses[j].GeometryShadermain;
	std::string mainCS=m_vTechniques[i].m_vPasses[j].ComputeShadermain;
	std::string mainHS=m_vTechniques[i].m_vPasses[j].HullShadermain;
	std::string mainDS=m_vTechniques[i].m_vPasses[j].DomainShadermain;
#endif
#if 0
    std::vector<std::string> avText;
    std::vector<std::string> avMain;
    std::vector<ShaderType> avType;
    avText.push_back(val1);
    avText.push_back(val1); // ??? [vertex shader]
    avMain.push_back(mainVS);
    avMain.push_back(mainPS);
    avType.push_back(eVertexShader);
    avType.push_back(ePixelShader);
#endif
#if 0
LOG_FNLN;
LOG_PRINT("\nval1:%s\n", val1.c_str());
LOG_PRINT("\nmainVS:%s\n", mainVS.c_str());
LOG_PRINT("\nmainPS:%s\n", mainPS.c_str());
std::ofstream out("./5640.txt");
out << "\nval1:\n";
out << val1.c_str();
out << "\nmainVS:\n";
out << mainVS.c_str();
out << "\nmainPS:\n";
out << mainPS.c_str();
out.close();
#endif
#if 0
    m_vTechniques[i].m_vPasses[j].Shader = IRenderer::GetRendererInstance()->addShader(
	val1.c_str(), 
	mainVS.c_str(), mainPS.c_str()   // aFileName.c_str()
	); // ???? mainVS <-> mainPS swapped
#elif 1
/*
ShaderID RendererHLSLD3D11::addHLSLShaderVrtl(
const char *vsText_, const char *gsText, const char *fsText_, const char *csText, const char *hsText, const char *dsText,
const char *vsMain, const char *gsMain, const char *fsMain, const char *csMain, const char *hsMain, const char *dsMain,
												const unsigned int flags)
*/
    m_vTechniques[i].m_vPasses[j].Shader = IRenderer::GetRendererInstance()->addHLSLShaderVrtl(
val1.c_str(), val1.c_str(), val1.c_str(), val1.c_str(), val1.c_str(), val1.c_str(), val1.c_str(), val1.c_str(),
mainVS.c_str(), mainGS.c_str(), mainPS.c_str(), mainCS.c_str(), mainHS.c_str(), mainDS.c_str(), mainHS.c_str(), mainDS.c_str(), 0);
#endif
 	LOG_PRINT("m_vTechniques[%d].m_vPasses[%d].Shader=%x\n", i, j, m_vTechniques[i].m_vPasses[j].Shader);
LOG_FNLN;
    if(m_vTechniques[i].m_vPasses[j].format_.size())
	    m_vTechniques[i].m_vPasses[j].VertexFormat = IRenderer::GetRendererInstance()->addVertexFormat((FormatDesc *)&m_vTechniques[i].m_vPasses[j].format_[0], m_vTechniques[i].m_vPasses[j].format_.size()/4, m_vTechniques[i].m_vPasses[j].Shader);
    }}
LOG_FNLN;
	LOG_FNLN;
	LOG_PRINT("m_vTechniques.size()=%d\n", m_vTechniques.size());
	LOG_PRINT("m_vTextures.size()=%d\n", m_vTextures.size());
	for(unsigned int i=0;i<m_vTechniques.size();i++){
		LOG_PRINT("m_vTechniques[%d].m_sName=%s\n", i, m_vTechniques[i].m_sName.c_str());
	for(unsigned int j=0;j<m_vTechniques[i].m_vPasses.size();j++)
	{
		LOG_PRINT("m_vTechniques[%d].m_vPasses[%d].m_sName=%s\n", i, j, m_vTechniques[i].m_vPasses[j].m_sName.c_str());
		LOG_PRINT("m_vTechniques[%d].m_vPasses[%d].Shader=%x\n", i, j, m_vTechniques[i].m_vPasses[j].Shader);
		LOG_PRINT("m_vTechniques[%d].m_vPasses[%d].VertexFormat=%x\n", i, j, m_vTechniques[i].m_vPasses[j].VertexFormat);
		LOG_PRINT("m_vTechniques[%d].m_vPasses[%d].DepthStencilState=%x\n", i, j, m_vTechniques[i].m_vPasses[j].DepthStencilState);
	}}
	for(unsigned int i=0;i<m_vTextures.size();i++)
	{
        	LOG_PRINT("Texture[%d].m_sName=%s\n", i, m_vTextures[i].m_sName.c_str());
        	LOG_PRINT("Texture[%d].texID=%x\n", i, m_vTextures[i].texID);
        	LOG_PRINT("Texture[%d].ss=%x\n", i, m_vTextures[i].ss);
	}
	//stx_exit(0);
}
stx_Effect* RendererHLSLCg::addEffectFromFile(const std::string& aFileName, const char* aDefines)
{
	return new stx_Effect(aFileName, aDefines);
}

void stx_Effect::SetMatrix( const char* aName, D3DXFROMWINEMATRIX* aValue )
{
	IRenderer::GetRendererInstance()->setShaderConstant4x4f(aName, *aValue);
}
void stx_Effect::SetVector( const char* aName, D3DXFROMWINEVECTOR4* aValue )
{
	IRenderer::GetRendererInstance()->setShaderConstant4f(aName, *aValue);
}
#if 0
void stx_Effect::SetFloat( const char* aName, float* aValue )
{
	IRenderer::GetRendererInstance()->setShaderConstant1f(aName, *aValue);
}
#endif
void stx_Effect::SetValue( const char* aName, void* aPtr, unsigned int aSize )
{
	IRenderer::GetRendererInstance()->setShaderConstantRaw(aName, &aPtr, aSize);
}
void stx_Effect::SetTexture( const char* aName, TextureID aValue )
{
	IRenderer::GetRendererInstance()->setTexture(aName, aValue);
}
void stx_Effect::SetBool( const char* aName, bool aValue )
{
	IRenderer::GetRendererInstance()->setShaderConstantRaw(aName, &aValue, sizeof(bool));
}
void stx_Effect::SetTechnique( const char* aName )
{
	m_sTechnique=aName;
}
void stx_Effect::Begin( unsigned int *cPasses, unsigned int aValue )
{
LOG_FNLN;
	*cPasses=0;
	int i=-1;
	int ii=0;
LOG_FNLN;
	while(ii<m_vTechniques.size())
	{
#if 0
		LOG_FNLN;
		LOG_PRINT("m_sTechnique=%s\n", m_sTechnique.c_str());
#endif
		if(m_sTechnique==m_vTechniques[ii].m_sName)
		{
			i=ii;
			break;
		}
		ii++;
	}
LOG_FNLN;
	m_iTechnique=-1;
	if(i<0)
		return;
LOG_FNLN;
	m_iTechnique=i;
#if 1
	LOG_FNLN;
	LOG_PRINT("m_iTechnique=%d\n", i);
	LOG_PRINT("m_vTechniques.size()=%d\n", m_vTechniques.size());
	LOG_PRINT("m_vTechniques[%d].m_vPasses.size()=%d\n", i, m_vTechniques[m_iTechnique].m_vPasses.size());
#endif
	m_sTechnique=m_vTechniques[i].m_sName;
	*cPasses=m_vTechniques[m_iTechnique].m_vPasses.size();
LOG_FNLN;
}
void stx_Effect::BeginPass( unsigned int iPass )
{
LOG_FNLN;
	if(m_iTechnique<0)
		return;
LOG_FNLN;
	if((iPass+1)>m_vTechniques[m_iTechnique].m_vPasses.size())
		return;
LOG_FNLN;
	stx_Pass pass=m_vTechniques[m_iTechnique].m_vPasses[iPass];
LOG_FNLN;
LOG_PRINT("pass.Shader=%x\n", pass.Shader);
	if(pass.Shader>-1)
		IRenderer::GetRendererInstance()->setShader(pass.Shader);
LOG_FNLN;
LOG_PRINT("pass.VertexFormat=%x\n", pass.VertexFormat);
	if(pass.VertexFormat>-1)
		IRenderer::GetRendererInstance()->setVertexFormat(pass.VertexFormat);
LOG_FNLN;
#if 0
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
#else
	if(pass.BlendState>-1)
		IRenderer::GetRendererInstance()->setBlendState(pass.BlendState);
	if(pass.AlphaState>-1)
		IRenderer::GetRendererInstance()->setAlphaState(pass.AlphaState);
	/* if(pass.DepthState>-1)
		IRenderer::GetRendererInstance()->setDepthState(pass.DepthState);
	else */
	if(pass.DepthStencilState>-1)
		IRenderer::GetRendererInstance()->setDepthState(pass.DepthStencilState);
	if(pass.RasterizerState>-1)
		IRenderer::GetRendererInstance()->setRasterizerState(pass.RasterizerState);
#endif
	#if 1
	LOG_FNLN;
        LOG_PRINT("pass.Shader=%x\n", pass.Shader);
        LOG_PRINT("pass.VertexFormat=%x\n", pass.VertexFormat);
        LOG_PRINT("pass.DepthStencilState=%x\n", pass.DepthStencilState);
	for(unsigned int i=0;i<m_vTextures.size();i++)
	{
		#if 1
        	LOG_PRINT("Texture[%d].m_sName=%s\n", i, m_vTextures[i].m_sName.c_str());
        	LOG_PRINT("Texture[%d].texID=%x\n", i, m_vTextures[i].texID);
        	LOG_PRINT("Texture[%d].ss=%x\n", i, m_vTextures[i].ss);
		#endif
		IRenderer::GetRendererInstance()->setTexture(m_vTextures[i].m_sName.c_str(), m_vTextures[i].texID);

	}
	//stx_exit(0);
	#endif
}
void stx_Effect::EndPass()
{
}
void stx_Effect::End()
{
}



	const char* def="#define WSIGN +\n#define ROW_MAJOR\n#define MVPSEMANTIC\n#define fract frac\n#define mix lerp\n#define atan(x,y) atan2(y,x)\n";
	const char* deffs="float3 GammaCorrect3(float3 aColor)\n{\n\treturn aColor;\n}\nfloat4 GammaCorrect4(float4 aColor)\n{\n\treturn aColor;\n}\n";




	const char* glslh      ="#define WSIGN +\n"
				"#define ROW_MAJOR\n"
				"#define MVPSEMANTIC\n"
				"vec3 GammaCorrect3(vec3 aColor){return aColor;}\n"
				"vec4 GammaCorrect4(vec4 aColor){return aColor;}\n"
				"uniform vec3      iResolution;\n"
				"uniform vec4      iMouse;\n"
				"uniform float     iTime;\n"
				"uniform vec3      resolution;\n"
				"uniform vec4      mouse;\n"
				"uniform float     time;\n"
				"uniform float     iGlobalTime;\n"
				"uniform vec4      iDate;\n"
				"uniform float     iSampleRate;\n"
				"uniform vec3      iChannelResolution[4];\n"
				"uniform float     iChannelTime[4];\n"
				"uniform vec2      ifFragCoordOffsetUniform;\n"
				"uniform float     iTimeDelta;\n"
				"uniform int       iFrame;\n"
				"uniform float     iFrameRate;\n"
				"struct Channel {\n"
				"    vec3  resolution;\n"
				"    float   time;\n"
				"};\n";
	const char* hlslh      ="#define WSIGN +\n"
				"#define ROW_MAJOR row_major\n"
				"#define MVPSEMANTIC\n"
				"float3 GammaCorrect3(float3 aColor){return aColor;}\n"
				"float4 GammaCorrect4(float4 aColor){return aColor;}\n"
				"float3      iResolution;\n"
				"float4      iMouse;\n"
				"float     iTime;\n"
				"float3      resolution;\n"
				"float4      mouse;\n"
				"float     time;\n"
				"float     iGlobalTime;\n"
				"float4      iDate;\n"
				"float     iSampleRate;\n"
				"float3      iChannelResolution[4];\n"
				"float     iChannelTime[4];\n"
				"float2      ifFragCoordOffsetUniform;\n"
				"float     iTimeDelta;\n"
				"int       iFrame;\n"
				"float     iFrameRate;\n"
				"struct Channel {\n"
				"    float3  resolution;\n"
				"    float   time;\n"
				"};\n";
	const char* vsStrGLSL=	"struct VsOut {\n"
				"    vec4 position;\n"
				"    vec2 uv;\n"
				"};\n"
				"struct VsIn2 {\n"
				"    vec2 position;\n"
				"    vec2 uv;\n"
				"};\n"
				"struct VsIn3 {\n"
				"    vec3 position;\n"
				"    vec3 Normal;\n"
				"    vec3 Binormal;\n"
				"    vec3 Tangent;\n"
				"    vec3 Color;\n"
				"    vec2 uv;\n"
				"};\n"
				"uniform mat4 worldViewProj;\n"
				"//layout(location = 0) in mat4  worldViewProj;\n"
				"VsOut main2( in VsIn2 In ) {\n"
				"    VsOut Out = VsOut(vec4(0.0, 0.0, 0.0, 0.0), vec2(0.0, 0.0));\n"
				"    Out.position = vec4( In.position.x, In.position.y, 0.0, 1.0);\n"
				"    Out.uv.x = In.uv.x;\n"
				"    Out.uv.y = 1.0-In.uv.y;\n"
				"    return Out;\n"
				"}\n"
				"//layout(location = 0) out vec2  xlv_TEXCOORD0;\n"
				"void main() {\n"
				"    VsOut xl_retval;\n"
				"    VsIn2 xlt_In;\n"
				"    xlt_In.position = vec2(gl_Vertex);\n"
				"    xlt_In.uv = vec2(gl_MultiTexCoord0);\n"
				"    xl_retval = main2( xlt_In);\n"
				"    gl_Position = vec4(xl_retval.position);\n"
				"    //gl_FragCoord = vec2(xl_retval.uv);\n"
				"    xlv_TEXCOORD0 = vec2(xl_retval.uv);\n"
				"}\n";
	const char* vsStrHLSL=	"struct VsOut2 {\n"
				"    float4 position;\n"
				"    float2 uv;\n"
				"};\n"
				"struct VsOut3 {\n"
				"    float4 position;\n"
				"    float3 Normal;\n"
				"    float3 Binormal;\n"
				"    float3 Tangent;\n"
				"    float3 Color;\n"
				"    float2 uv;\n"
				"};\n"
				"struct VsIn2 {\n"
				"    float2 position;\n"
				"    float2 uv;\n"
				"};\n"
				"struct VsIn3 {\n"
				"    float3 position;\n"
				"    float3 Normal;\n"
				"    float3 Binormal;\n"
				"    float3 Tangent;\n"
				"    float3 Color;\n"
				"    float2 uv;\n"
				"};\n"
				"float4x4 worldViewProj;\n"
				"VsOut2 main2( in VsIn2 In ) {\n"
				"    VsOut Out = VsOut(float4(0.0, 0.0, 0.0, 0.0), float2(0.0, 0.0));\n"
				"    Out.position = float4( In.position.x, In.position.y, 0.0, 1.0);\n"
				"    Out.uv.x = In.uv.x;\n"
				"    Out.uv.y = 1.0-In.uv.y;\n"
				"    return Out;\n"
				"}\n"
				"VsOut3 main3( in VsIn3 In ) {\n"
				"    VsOut Out = VsOut(float4(0.0, 0.0, 0.0, 0.0), float2(0.0, 0.0));\n"
				"    Out.position = float4( In.position.x, In.position.y, 0.0, 1.0);\n"
				"    Out.Normal=In.Normal;\n"
				"    Out.Binormal=In.Binormal;\n"
				"    Out.Tangent=In.Tangent;\n"
				"    Out.Color=In.Color;\n"
				"    Out.uv.x = In.uv.x;\n"
				"    Out.uv.y = 1.0-In.uv.y;\n"
				"    return Out;\n"
				"}\n";
struct ShaderInfo
{
	std::string delimeter;
	int position;
	unsigned int length;
	std::string text;
	ShaderInfo()
	{
		position=-1;
		length=0;
	}
	virtual ~ShaderInfo()
	{
		delimeter="";
		position=-1;
		length=0;
		text="";
	}
	ShaderInfo(const ShaderInfo& rhs)
	{
		delimeter=rhs.delimeter;
		position=rhs.position;
		length=rhs.length;
		text=rhs.text;
	}
};

void IRenderer__bubbleSort(std::vector<std::string>& delimeters, std::vector<int>& arr)
{
	int n=arr.size();
    int i, j; 
    for (i = 0; i < n - 1; i++) 
  
        for (j = 0; j < n - i - 1; j++)
	{
            if (arr[j] > arr[j + 1])
	    {
                swap(arr[j], arr[j + 1]);
                swap(delimeters[j], delimeters[j + 1]);
	    }
	}
}
void IRenderer__printArray(std::vector<std::string>& delimeters, std::vector<int>& arr)
{ 
    int i; 
    for (i = 0; i < arr.size(); i++) 
        cout << "RES: " << arr[i] << " " << delimeters[i] << endl; 
}
void IRenderer__findArray(std::string shaderText, std::vector<std::string>& delimeters, const std::string& aDelimeter)
{
	std::size_t found = shaderText.find(aDelimeter);
	if(found!=std::string::npos)
	{
		delimeters.push_back(aDelimeter);
		//arr.push_back(found);
		ShaderInfo shaderInfo;
		shaderInfo.delimeter=aDelimeter;
		shaderInfo.position=found;
		//shaderInfo.length=;
		//shaderInfo.text=shaderText.substr(found, );
	}
}
std::string IRenderer__GetShaderText(std::string& aShaders, std::string& aDelimeter, std::vector<std::string>& delimeters, std::vector<int>& positions)
{
	std::string shaderText=aShaders;
	delimeters.push_back("[Vertex shader]");
	delimeters.push_back("[Fragment shader]");
	delimeters.push_back("[Geometry shader]");
	delimeters.push_back("[Compute shader]");
	delimeters.push_back("[Hull shader]");
	delimeters.push_back("[Domain shader]");
	delimeters.push_back("[TessControl shader]");
	delimeters.push_back("[TessEval shader]");
	
    	unsigned int i=0;
	while(i<delimeters.size())
	{
		std::size_t found = shaderText.find(delimeters[i++].c_str());
		if(std::string::npos!=found)
		{
			positions.push_back(found);
		}
		else
		{
			positions.push_back(std::string::npos);
		}
	}
	unsigned int j = positions.size()-1;
	while((j<9)&&(j>=0)&&positions.size())
	{
		if(-1==positions[j])
		{
			if(j<=positions.size()) {positions.erase (positions.begin()+j);}
			if(j<=delimeters.size()) {delimeters.erase (delimeters.begin()+j);}
		}
		j--;
	}

	IRenderer__bubbleSort(delimeters, positions);
	//IRenderer__printArray(delimeters, positions);

	if(delimeters.size())
		return shaderText;
	else
		return std::string("[Fragment shader]\n")+shaderText;

	return shaderText;
}

#include <Framework3/RendererGLSLGL_1_1.h>
ShaderID IRenderer::addShader(  const char* shaderText_, 
                                        const char* vsMain_, 
                                        const char* fsMain_,
                             	const char *defines, 
                                        const unsigned int aFlags)
{

	ShaderID res = SHADER_NONE;
	const char* vsMain=vsMain_;
	const char* gsMain="main";
	const char* fsMain=fsMain_;
	const char* csMain="main";
	const char* hsMain="main";
	const char* dsMain="main";
	const char* tcsMain="main";
	const char* tesMain="main";

	std::string shaderText;
	if(shaderText_)
		shaderText=shaderText_;
	const char* lslh=0;
	const char* vsStrLSL=0;
	unsigned int flags_=0;

	switch(aFlags)
	{
		case eGLSL_Shader:
			LOG_PRINT("GLSL_Shader\n");
			lslh=glslh;
			vsStrLSL=vsStrGLSL;
			flags_=eGLSL_Shader;
			break;
		case eGLES_Shader:
			LOG_PRINT("GLES_Shader\n");
			lslh=glslh;
			vsStrLSL=vsStrGLSL;
			flags_=eGLES_Shader;
			break;
		default:
			LOG_PRINT("HLSL_Shader\n");
			lslh=hlslh;
			vsStrLSL=vsStrHLSL;
			flags_=eHLSL_Shader;
			break;
	}

	std::size_t foundVS = shaderText.find("[Vertex shader]");
	std::size_t foundFS = shaderText.find("[Fragment shader]");
	std::size_t foundGS = shaderText.find("[Geometry shader]");

	std::size_t foundCS = shaderText.find("[Compute shader]");
	std::size_t foundHS = shaderText.find("[Hull shader]");
	std::size_t foundDS = shaderText.find("[Domain shader]");

	std::size_t foundTCS = shaderText.find("[TessControl shader]");
	std::size_t foundTES = shaderText.find("[TessEval shader]");

	std::size_t found=foundVS+foundFS+foundGS+foundCS+foundHS+foundDS+foundTCS+foundTES;

	std::string vsStr, fsStr, gsStr, csStr, hsStr, dsStr, tcsStr, tesStr;
	std::size_t headerlength=std::string::npos;
	if(foundVS!=std::string::npos) headerlength=std::min(headerlength, foundVS);
	if(foundFS!=std::string::npos) headerlength=std::min(headerlength, foundFS);
	if(foundGS!=std::string::npos) headerlength=std::min(headerlength, foundGS);
	int  endIndex = shaderText.length()-1;
	std::string header = shaderText.substr(0, headerlength);
	if((headerlength==std::string::npos)||(headerlength==0)) header="";
#if 1
	std::string str;std::string key="[Vertex shader]";
	std::vector<std::string> delimeters;
	std::vector<int> positions;
	str=IRenderer__GetShaderText(shaderText, key, delimeters, positions);
	//STX_PRINT("\nfound:\n%d\n", found);
	if(found==-8)
	{
		std::string varying="";
		if(aFlags!=eHLSL_Shader)
			varying="varying vec2 xlv_TEXCOORD0;\n";
		vsStr=varying+vsStrLSL;
		fsStr=varying+lslh+shaderText;
	}
	else
	{
	unsigned int i = 0;
	for (i = 0; i < delimeters.size(); i++)
	{
		if((positions[i]>-1)&&(positions[i]!=std::string::npos))
		{
			std::size_t l2=shaderText.length();
			std::size_t p1=positions[i]+delimeters[i].length();
			if(i<(positions.size()-1))
				l2=positions[i+1]-positions[i]-delimeters[i].length();
	        	//cout << "position: " << positions[i] << ", delimeter:" << delimeters[i] << ", shaderText[" << p1 <<", " << l2 << "]" << endl;

			if(delimeters[i]=="[Vertex shader]") vsStr=lslh+header+shaderText.substr(p1, l2);
			else if(delimeters[i]=="[Fragment shader]") fsStr=lslh+header+shaderText.substr(p1, l2);
			else if(delimeters[i]=="[Geometry shader]") gsStr=header+shaderText.substr(p1, l2);
			else if(delimeters[i]=="[Compute shader]") csStr=header+shaderText.substr(p1, l2);
			else if(delimeters[i]=="[Hull shader]") hsStr=header+shaderText.substr(p1, l2);
			else if(delimeters[i]=="[Domain shader]") dsStr=header+shaderText.substr(p1, l2);
			else if(delimeters[i]=="[TessControl shader]") tcsStr=header+shaderText.substr(p1, l2);
			else if(delimeters[i]=="[TessEval shader]") tesStr=header+shaderText.substr(p1, l2);
		}
	}}
#if 0
	STX_PRINT("\nVS:\n%s\n", vsStr.c_str());
	STX_PRINT("\nFS:\n%s\n", fsStr.c_str());
	STX_PRINT("\nGS:\n%s\n", gsStr.c_str());
	STX_PRINT("\nCS:\n%s\n", csStr.c_str());
	STX_PRINT("\nHS:\n%s\n", hsStr.c_str());
	STX_PRINT("\nDS:\n%s\n", dsStr.c_str());
	STX_PRINT("\nTCS:\n%s\n", tcsStr.c_str());
	STX_PRINT("\nTES:\n%s\n", tesStr.c_str());
#elif 0
	STX_PRINT("\neHLSL_Shader=%x\n", eHLSL_Shader);
	STX_PRINT("\naFlags=%x\n", aFlags);
	stx_exit(0);
#endif
#endif

	if(aFlags==eHLSL_Shader)
		return addHLSLShaderVrtl( 	vsStr.c_str(), gsStr.c_str(), fsStr.c_str(), csStr.c_str(), hsStr.c_str(), dsStr.c_str(), tcsStr.c_str(), tesStr.c_str(),
                        			vsMain, gsMain, fsMain, csMain, hsMain, dsMain, tcsMain, tesMain, flags_);
	else
	{
#if defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
		RendererGLES2* rendererGLES2=0;
		rendererGLES2=(RendererGLES2*) this;
		return rendererGLES2->addGLSLShaderVrtl( 	vsStr.c_str(), gsStr.c_str(), fsStr.c_str(), csStr.c_str(), hsStr.c_str(), dsStr.c_str(), tcsStr.c_str(), tesStr.c_str(),
                        			vsMain, gsMain, fsMain, csMain, hsMain, dsMain, tcsMain, tesMain, flags_);
#else
		RendererGLSLGL_1_1* rendererGLSLGL_1_1=0;
		rendererGLSLGL_1_1=dynamic_cast<RendererGLSLGL_1_1*>(this);
		return rendererGLSLGL_1_1->addGLSLShaderVrtl( 	vsStr.c_str(), gsStr.c_str(), fsStr.c_str(), csStr.c_str(), hsStr.c_str(), dsStr.c_str(), tcsStr.c_str(), tesStr.c_str(),
                        			vsMain, gsMain, fsMain, csMain, hsMain, dsMain, tcsMain, tesMain, flags_);
#endif
    	}
	return -1;
}
#endif

