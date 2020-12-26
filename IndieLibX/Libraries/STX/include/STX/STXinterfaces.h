/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef __INTERFACES__
#define __INTERFACES__

/*
typedef enum _tSWIG
{
	eSWIG,
	eNOTSWIG
} tSWIG;
*/
enum tRenderer
{
	eGLCG_1_1,
	eGLGLSL_1_1,
	eGLGLSL_4_5,
	eGLES2,
	eGLES3_2,
	eD3D9,
	eD3D10,
	eD3D11,
	eD3D12,
	eVULKAN,
	eMETAL
};
typedef enum _tAudio
{
	eDefault_Audio,
	eSDL_Audio,
	eFMOD_Audio,
	eOpenAL_Audio,
	eDummy_Audio
} tAudio;
typedef enum _tWindow
{
	eDefault_Window,
	eSDL_Window,
	eDummy_Window
} tWindow;
typedef enum _tInput
{
	eDefault_Input,
	eSDL_Input,
	eDummy_Input
} tInput;

#include <stdexcept>
#include <string>
#include <map>
#include <vector>

#define STX_tolower(X)  (((X) >= 'A') && ((X) <= 'Z') ? ('a'+((X)-'A')) : (X))
#define STX_toupper(X)  (((X) >= 'a') && ((X) <= 'z') ? ('A'+((X)-'a')) : (X))
#ifndef SWIG
#ifndef __APPLE__
#include <malloc.h>
#endif
#if defined( LINUX ) && !defined(__APPLE__)
#if !defined(CYGWIN64)
#include <mcheck.h>
#endif
#endif
#endif

#if 1//defined(_LINUX)
#define STX_INIT_SEGVCATCH
#define STX_TRY
#define STX_CATCH
#else
#include <segvcatch.h>
#define STX_INIT_SEGVCATCH 	segvcatch::init_segv(); \
				segvcatch::init_fpe();

#define STX_TRY try \
{

#define STX_CATCH } \
	catch (std::runtime_error &e) { \
        STX_PRINTF("Caught a runtime_error exception: " \
                  << e.what () << '\n'; \
	STX_STACK_TRACE; \
    } \
    catch (std::exception &e) { \
        STX_PRINTF("Caught an exception of an unexpected type: " \
                  << e.what () << '\n'; \
	STX_STACK_TRACE; \
    }  \
    catch (...) { \
        STX_PRINTF("Caught an unknown exception\n"; \
	STX_STACK_TRACE; \
    }
#endif
#ifndef ANDROID
#if 0
#define STXString std::string
#else
//template<class T, size_t n>
//class STXArray: public std::array<T,n>
#if 0
template<class T,size_t x>
class STXArray: public
#if defined( _MSC_VER) || defined(QT_BUILD)
std::vector<T>
#else
std::array<T,x>
#endif
{
	public:
		STXArray(const T* al):
#if defined( _MSC_VER) || defined(QT_BUILD)
std::vector<T>(x)
#else
std::array<T,x>()
#endif
		{
			unsigned int i=0;
			while(i<x)
			{
				T* p=
#if defined( _MSC_VER) || defined(QT_BUILD)
std::vector<T>
#else
std::array<T,x>
#endif
				::data()+i;
				*(p)=*(al+i);
				i++;
			}
		}
		STXArray():
#if defined( _MSC_VER) || defined(QT_BUILD)
std::vector<T>(x)
#else
std::array<T,x>()
#endif
		{
			//???fill<T>(0);
		}
		virtual ~STXArray(){}
#ifdef _DEBUG
		T& operator[](int index)
		{
			if(index>=
			#if defined( _MSC_VER) || defined(QT_BUILD)
std::vector<T>
#else
std::array<T,x>
#endif
			::size()) STX_STACK_TRACE;
			return
#if defined( _MSC_VER) || defined(QT_BUILD)
std::vector<T>
#else
std::array<T,x>
#endif
			::at(index);
		}

		const T& operator[](int index) const
		{
			if(index>=
#if defined( _MSC_VER) || defined(QT_BUILD)
std::vector<T>
#else
std::array<T,x>
#endif
			::size()) STX_STACK_TRACE;
			return
#if defined( _MSC_VER) || defined(QT_BUILD)
std::vector<T>
#else
std::array<T,x>
#endif
			::at(index);
		}
#endif
};
template<class T,size_t x>
class STX1DArray: public
STXArray<T,x>
{
	public:
		STX1DArray():STXArray<T,x>()
		{
			//???STXArray<T,x>::fill(0);
		}
		virtual ~STX1DArray(){}
#ifdef _DEBUG
		T& operator[](int index)
		{
			if(index>=STXArray<T,x>::size()) STX_STACK_TRACE;
			return STXArray<T,x>::at(index);
		}

		const T& operator[](int index) const
		{
			if(index>=STXArray<T,x>::size()) STX_STACK_TRACE;
			return STXArray<T,x>::at(index);
		}
#endif
};
template<class T,size_t x, size_t y>
class STX2DArray
{
	protected:
		STXArray<STXArray<T,x>,y> m_a;
	public:
		unsigned int GetNX(){return x;}
		unsigned int GetNY(){return y;}
		STX2DArray()
		{
			for(unsigned int j=0;j<y;j++)
			for(unsigned int i=0;i<x;i++)
				*(m_a[j].data()+i)=0;

		}
		virtual ~STX2DArray(){}
		T* data()
		{
			return m_a.data();
		}

		STXArray<T,x>& operator[](int index)
		{

			if(index>=m_a.size()) STX_STACK_TRACE;
			return m_a[index];
		}

		const STXArray<T,x>& operator[](int index) const
		{
			if(index>=m_a.size()) STX_STACK_TRACE;
			return m_a[index];
		}
};
template<class T>
class STXVector : public std::vector<T>
{
	public:
		STXVector(const T al[]):std::vector<T>()
		{
			unsigned int i=0;
			unsigned int x=elementsOf(al);
			//STX_PRINTF("x=%d\n",x);
			STX_STACK_TRACE;
			while(i<x)
			{
				std::vector<T>::push_back(*(al+i));
				i++;
			}
		}
		STXVector(size_t n=0):std::vector<T>(n){}
		virtual ~STXVector(){}
#ifdef _DEBUG
		T& operator[](int index)
		{
			if(index>=std::vector<T>::size()) STX_STACK_TRACE;
			return std::vector<T>::at(index);
		}

		const T& operator[](int index) const
		{
			if(index>=std::vector<T>::size()) STX_STACK_TRACE;
			return std::vector<T>::at(index);
		}
#endif
};
template<class T>
class STXList : public std::list<T>
{
	public:
		STXList():std::list<T>(){}
		virtual ~STXList(){}

		T& operator[](int index)
		{
			if(index>=std::list<T>::size()) STX_STACK_TRACE;
			#if defined(QT_BUILD)
			return std::list<T>::at(index);
			#else
			return at(index);
			#endif
		}

		const T& operator[](int index) const
		{
			if(index>=std::list<T>::size()) STX_STACK_TRACE;
			#if defined(QT_BUILD)
			return std::list<T>::at(index);
			#else
			return at(index);
			#endif
		}

		std::
#if defined(QT_BUILD)
			_List_iterator<T>
#elif defined( _MSC_VER)
			list<T>::iterator
#else
			_List_iterator<T>
#endif
			 iter(int index)
		{
			if(index>=std::list<T>::size()) STX_STACK_TRACE;
			int pos=0;
			std::_List_iterator<T> ait2b=std::list<T>::begin();
			std::_List_iterator<T> ait2e=std::list<T>::end();
			std::_List_iterator<T> ii;
			for(ii=ait2b;ii!=ait2e;ii++)
			{
 				if (!(pos < std::list<T>::size()))
					throw std::out_of_range();
					//return std::list<T>::end();//???
				else if(index==pos)
					break;
				pos++;
			}
			return ii;
		}
		T& at(int index) const
		{
			return *iter(index);
		}

void remove( const T& value ){}

template< class UnaryPredicate >
void remove_if( UnaryPredicate p ){}

};
#endif
#endif
#if !defined(SWIG) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
template<

    class Key,
    class T,
    class Compare = std::less<Key>,
    class Allocator = std::allocator<std::pair<const Key, T> >
>
class STXMap : public std::map<Key,T,Compare,Allocator>
{
	public:
		STXMap():std::map<Key,T,Compare,Allocator>(){}
		virtual ~STXMap(){}
};
class STXString : public std::string
{
	public:
		STXString(const int n);
		STXString();
		STXString(const char* str);
		STXString(char* str);

		//operator const char *() const;
		STXString(std::string& str);
		STXString(const STXString& rhs);
		//STXString& operator=(const STXString& rhs);
		STXString& operator=(const int& rhs);

		operator const char*() const {return std::string::c_str();}
		operator char*() const {return const_cast<char*>(std::string::c_str());}

		void tolower();
		void toupper();
		~STXString();

	const STXString& operator+=(const STXString& s);
	const int count(const char c) const;

	const STXString find(const STXString& what) const;
	const STXString after(const STXString& what) const;
	const STXString before(const STXString& what) const;
	void append(const STXString& what){std::string::append(what.c_str());}
	char* str() const;
	void replace(const STXString& afrom, const STXString& ato);

};
#endif
#endif

#if defined(linux) && !defined(__APPLE__) && !defined(WIN_PHONE_APP) && !defined(ANDROID) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR)
#if defined(SDL_VIDEO_DRIVER_X11)
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#endif
#endif

struct STXAPI IWindow
{
protected:
	int x,y;
	unsigned int bpp,width, height;
	std::string title;

#if defined(SDL_VIDEO_DRIVER_WINDOWS) || defined(__MINGW32__) || defined(__MINGW64__)
	HWND m_hWnd;
    	HINSTANCE m_hinstance;
	HDC m_hdc;
public:
	HWND GetHWND(){return m_hWnd;}
	HINSTANCE GetHINSTANCE(){return m_hinstance;}
	HDC GetHDC(){return m_hdc;}
#elif defined(SDL_VIDEO_DRIVER_X11) //|| defined(OS_IPHONE)
	Display* m_dpy;
	Window m_win;
public:
	Display* GetDisplay(){return m_dpy;}
	Window GetWindow(){return m_win;}
#elif defined(LINUX) && !defined(__APPLE__)
	xcb_connection_t* m_connection;
	xcb_window_t      m_window;
public:
	xcb_connection_t* GetConnection(){return m_connection;}
	xcb_window_t GetWindow(){return m_window;}
#endif
public:
	void ReadConfigurationFile(std::string& atitle);
	void UpdateWindowArgs();
	virtual int Getbpp(){return bpp;}
	virtual int Getwidth(){return width;}
	virtual int Getheight(){return height;}
	virtual int GetWidth(){return width;}
	virtual int GetHeight(){return height;}

	virtual void GetWindowPosition(int* x, int* y, unsigned int* z, unsigned int* w)=0;

	virtual void HotKey_ToggleFullScreen(void){}
	virtual void HotKey_ToggleGrab(void){}
	virtual void HotKey_Iconify(void){}
#if defined(SDL_VIDEO_DRIVER_WINDOWS) || defined(__MINGW32__) || defined(__MINGW64__)
	IWindow():m_hWnd(0),title(""){}
#elif defined(SDL_VIDEO_DRIVER_X11)
	IWindow():m_dpy(0), m_win(0),title(""){}
#endif
	virtual ~IWindow(){}
	virtual void ShowCursor (bool pEnable)=0;
	virtual void SetCaption(const char * buf)=0;
	const char* GetCaption(){return title.c_str();}
	virtual void Init(unsigned int awidth, unsigned int aheight, unsigned int abpp, const char* atitle)=0;
	virtual void ReSize(int awidth, int aheight, int abpp)=0;
	//virtual void Init2(int awidth, int aheight, int abpp)=0;
	//virtual int Quit(int returnCode=0)=0;
};
#if 1//ndef SWIG
struct STXAPI DummyWindow : public IWindow
{
public:
	DummyWindow():IWindow(){}
	virtual ~DummyWindow(){}
	virtual void ShowCursor (bool pEnable){}
	virtual void SetCaption(const char * buf)
    {
        if(!buf)
            return;    
        title=buf;
    }
	const char* GetCaption(){return title.c_str();}
	virtual void Init(unsigned int awidth, unsigned int aheight, unsigned int abpp, const char* atitle)
	{
		width=awidth;
		height=aheight;
		bpp=abpp;
	}

	virtual void GetWindowPosition(int* x, int* y, unsigned int* z, unsigned int* w)
	{
		*x=0;
		*y=0;
		*w=*x+width;
		*z=*y+height;
	}
	virtual void ReSize(int awidth, int aheight, int abpp)
	{
		width=awidth;
		height=aheight;
		bpp=abpp;
	}
};


typedef unsigned int             FMOD_INITFLAGS;
#define FMOD_INIT_NORMAL                  0x00000000 /* All platforms - Initialize normally */
#endif
class STXAPI IAudio
{
public:
	IAudio(){}
	virtual ~IAudio(){}
public:
	//-------------------------------------------------
	// Public Functions for this Class
	//-------------------------------------------------

	virtual void				Update			( ){}
	virtual bool				Init			(int  maxchannels = 64, FMOD_INITFLAGS  flags = FMOD_INIT_NORMAL){return true;}
virtual bool					Load			(const char* file, const char* szName)=0;
	virtual bool				Play			(const char* szName, int loopCount = 1)=0;
	virtual void				Stop			(const char* szName)=0;
	virtual void				StopAll			( )=0;
	virtual bool				IsPlaying		(const char* szName)=0;
	virtual int					GetNumSounds	( )		=0;
};
#if 1//ndef SWIG

class STXAPI DummyAudio : public IAudio
{
public:
	DummyAudio():IAudio(){}
	virtual ~DummyAudio(){}
public:
	//-------------------------------------------------
	// Public Functions for this Class
	//-------------------------------------------------
	virtual bool				Init			(int  maxchannels = 64, FMOD_INITFLAGS  flags = FMOD_INIT_NORMAL){return true;}
	virtual bool				Load			(const char* szFilename, const char* szName){return true;}
	virtual void				Update			( ){}
	virtual bool				Play			(const char* szName, int loopCount = 1){return false;}
	virtual void				Stop			(const char* szName){}
	virtual void				StopAll			( ){}
	virtual bool				IsPlaying		(const char* szName){return false;}
	virtual int				GetNumSounds		( ){return 0;}
};
#endif
struct STXAPI STX_TIMER
{
	virtual void Reset(){}
	STX_TIMER(){}
	virtual ~STX_TIMER(){}
	virtual Uint32 stx_GetTicks(STX_TIMER* _Timer=0)=0;
	virtual void stx_Delay(Uint32 ms,STX_TIMER* _Timer=0)=0;
};

#if 1//def SWIG
#ifndef __APPLE__
struct STXAPI STX_Dummy_TIMER : public  STX_TIMER
{
protected:
public:
	virtual void Reset();
	STX_Dummy_TIMER();
	virtual ~STX_Dummy_TIMER();
	virtual Uint32 stx_GetTicks(STX_TIMER* _Timer=0);
	virtual void stx_Delay(Uint32 ms,STX_TIMER* _Timer=0);
};
#endif

#include <Image.h>

#if defined (QT_BUILD)
#define STX_KEY_MACRO(X) STX_KEY_##X=QtSTX::Key_##X
#else
#define STX_KEY_MACRO(X) STX_KEY_##X=SDLK_##X	
#endif

#define KEYTOCHAR(X) "KEY_##X"

typedef enum _eKey
{
#if !defined(ANDROID) && !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR) && !defined(WIN_PHONE_APP) && !defined(__MINGW32__) && !defined(__MINGW64__)
	STX_KEY_UNKNOWN	=SDLK_UNKNOWN,
#if defined (SDL_1_2_15_BUILD)
	STX_KEY_NUMLOCK	=SDLK_NUMLOCK,
	STX_KEY_SCROLLOCK	=SDLK_SCROLLOCK,
	STX_KEY_RSUPER	=SDLK_RSUPER,
	STX_KEY_PRINT	=SDLK_PRINT,
	STX_KEY_BREAK	=SDLK_BREAK,
#endif
	STX_KEY_ESCAPE	=SDLK_ESCAPE,
	STX_KEY_KP_EQUALS	=SDLK_KP_EQUALS,
	STX_KEY_UP		=SDLK_UP,
	STX_KEY_DOWN	=SDLK_DOWN,
	STX_KEY_RIGHT	=SDLK_RIGHT,
	STX_KEY_LEFT	=SDLK_LEFT,
	STX_KEY_INSERT	=SDLK_INSERT,
	STX_KEY_HOME	=SDLK_HOME,
	STX_KEY_END		=SDLK_END,
	STX_KEY_PAGEUP	=SDLK_PAGEUP,
	STX_KEY_PAGEDOWN	=SDLK_PAGEDOWN,

	STX_KEY_KP_PERIOD	=SDLK_KP_PERIOD,
	STX_KEY_KP_DIVIDE	=SDLK_KP_DIVIDE,
	STX_KEY_KP_MULTIPLY	=SDLK_KP_MULTIPLY,
	STX_KEY_KP_MINUS	=SDLK_KP_MINUS,
	STX_KEY_KP_PLUS	=SDLK_KP_PLUS,
	STX_KEY_KP_ENTER	=SDLK_KP_ENTER,

	STX_KEY_DELETE	=SDLK_DELETE,
	STX_KEY_CAPSLOCK	=SDLK_CAPSLOCK,
	STX_KEY_RSHIFT	=SDLK_RSHIFT,
	STX_KEY_LSHIFT	=SDLK_LSHIFT,
	STX_KEY_RCTRL	=SDLK_RCTRL,
	STX_KEY_LCTRL	=SDLK_LCTRL,
	STX_KEY_RALT	=SDLK_RALT,
	STX_KEY_LALT	=SDLK_LALT,
	STX_KEY_HELP	=SDLK_HELP,
	STX_KEY_SYSREQ	=SDLK_SYSREQ,
	STX_KEY_MENU	=SDLK_MENU,

	STX_KEY_BACKSPACE	=SDLK_BACKSPACE,
	STX_KEY_TAB		=SDLK_TAB,
	STX_KEY_CLEAR	=SDLK_CLEAR,
	STX_KEY_RETURN	=SDLK_RETURN,
	STX_KEY_PAUSE	=SDLK_PAUSE,
	STX_KEY_SPACE	=SDLK_SPACE,
	STX_KEY_EXCLAIM	=SDLK_EXCLAIM,
	STX_KEY_QUOTEDBL	=SDLK_QUOTEDBL,
	STX_KEY_HASH	=SDLK_HASH,
	STX_KEY_DOLLAR	=SDLK_DOLLAR,
	STX_KEY_AMPERSAND	=SDLK_AMPERSAND,
	STX_KEY_QUOTE	=SDLK_QUOTE,
	STX_KEY_LEFTPAREN	=SDLK_LEFTPAREN,
	STX_KEY_RIGHTPAREN	=SDLK_RIGHTPAREN,
	STX_KEY_ASTERISK	=SDLK_ASTERISK,
	STX_KEY_PLUS	=SDLK_PLUS,
	STX_KEY_COMMA	=SDLK_COMMA,
	STX_KEY_MINUS	=SDLK_MINUS,
	STX_KEY_PERIOD	=SDLK_PERIOD,
	STX_KEY_SLASH	=SDLK_SLASH,
	STX_KEY_COLON	=SDLK_COLON,
	STX_KEY_SEMICOLON	=SDLK_SEMICOLON,
	STX_KEY_LESS	=SDLK_LESS,
	STX_KEY_EQUALS	=SDLK_EQUALS,
	STX_KEY_GREATER	=SDLK_GREATER,
	STX_KEY_QUESTION	=SDLK_QUESTION,
	STX_KEY_AT		=SDLK_AT,
	STX_KEY_LEFTBRACKET	=SDLK_LEFTBRACKET,
	STX_KEY_BACKSLASH	=SDLK_BACKSLASH,
	STX_KEY_RIGHTBRACKET=SDLK_RIGHTBRACKET,
	STX_KEY_CARET	=SDLK_CARET,
	STX_KEY_UNDERSCORE	=SDLK_UNDERSCORE,
	STX_KEY_BACKQUOTE	=SDLK_BACKQUOTE,

	STX_KEY_F1		=SDLK_F1,
	STX_KEY_F2		=SDLK_F2,
	STX_KEY_F3		=SDLK_F3,
	STX_KEY_F4		=SDLK_F4,
	STX_KEY_F5		=SDLK_F5,
	STX_KEY_F6		=SDLK_F6,
	STX_KEY_F7		=SDLK_F7,
	STX_KEY_F8		=SDLK_F8,
	STX_KEY_F9		=SDLK_F9,
	STX_KEY_F10		=SDLK_F10,
	STX_KEY_F11		=SDLK_F11,
	STX_KEY_F12		=SDLK_F12,
	STX_KEY_F13		=SDLK_F13,
	STX_KEY_F14		=SDLK_F14,
	STX_KEY_F15		=SDLK_F15,
#if defined(SDL_2_0_5_BUILD)
	STX_KEY_KP0		=SDLK_KP_0,
	STX_KEY_KP1		=SDLK_KP_1,
	STX_KEY_KP2		=SDLK_KP_2,
	STX_KEY_KP3		=SDLK_KP_3,
	STX_KEY_KP4		=SDLK_KP_4,
	STX_KEY_KP5		=SDLK_KP_5,
	STX_KEY_KP6		=SDLK_KP_6,
	STX_KEY_KP7		=SDLK_KP_7,
	STX_KEY_KP8		=SDLK_KP_8,
	STX_KEY_KP9		=SDLK_KP_9,
#elif defined(SDL_1_2_15_BUILD)
        KEY_KP0		=SDLK_KP0,
    KEY_KP1		=SDLK_KP1,
    KEY_KP2		=SDLK_KP2,
    KEY_KP3		=SDLK_KP3,
    KEY_KP4		=SDLK_KP4,
    KEY_KP5		=SDLK_KP5,
    KEY_KP6		=SDLK_KP6,
    KEY_KP7		=SDLK_KP7,
    KEY_KP8		=SDLK_KP8,
    KEY_KP9		=SDLK_KP9,
#endif
	STX_KEY_0		=SDLK_0,
	STX_KEY_1		=SDLK_1,
	STX_KEY_2		=SDLK_2,
	STX_KEY_3		=SDLK_3,
	STX_KEY_4		=SDLK_4,
	STX_KEY_5		=SDLK_5,
	STX_KEY_6		=SDLK_6,
	STX_KEY_7		=SDLK_7,
	STX_KEY_8		=SDLK_8,
	STX_KEY_9		=SDLK_9,

	STX_KEY_a		=SDLK_a,
	STX_KEY_b		=SDLK_b,
	STX_KEY_c		=SDLK_c,
	STX_KEY_d		=SDLK_d,
	STX_KEY_e		=SDLK_e,
	STX_KEY_f		=SDLK_f,
	STX_KEY_g		=SDLK_g,
	STX_KEY_h		=SDLK_h,
	STX_KEY_i		=SDLK_i,
	STX_KEY_j		=SDLK_j,
	STX_KEY_k		=SDLK_k,
	STX_KEY_l		=SDLK_l,
	STX_KEY_m		=SDLK_m,
	STX_KEY_n		=SDLK_n,
	STX_KEY_o		=SDLK_o,
	STX_KEY_p		=SDLK_p,
	STX_KEY_q		=SDLK_q,
	STX_KEY_r		=SDLK_r,
	STX_KEY_s		=SDLK_s,
	STX_KEY_t		=SDLK_t,
	STX_KEY_u		=SDLK_u,
	STX_KEY_v		=SDLK_v,
	STX_KEY_w		=SDLK_w,
	STX_KEY_x		=SDLK_x,
	STX_KEY_y		=SDLK_y,
	STX_KEY_z		=SDLK_z,
	STX_KEY_PERCENT	=SDLK_PERCENT,
#else
	STX_KEY_UNKNOWN	,
	STX_KEY_ESCAPE	,//=SDLK_ESCAPE,
	STX_KEY_KP_EQUALS	,//=SDLK_KP_EQUALS,
	STX_KEY_UP		,//=SDLK_UP,
	STX_KEY_DOWN	,//=SDLK_DOWN,
	STX_KEY_RIGHT	,//=SDLK_RIGHT,
	STX_KEY_LEFT	,//=SDLK_LEFT,
	STX_KEY_INSERT	,//=SDLK_INSERT,
	STX_KEY_HOME	,//=SDLK_HOME,
	STX_KEY_END		,//=SDLK_END,
	STX_KEY_PAGEUP	,//=SDLK_PAGEUP,
	STX_KEY_PAGEDOWN	,//=SDLK_PAGEDOWN,

	STX_KEY_KP_PERIOD	,//=SDLK_KP_PERIOD,
	STX_KEY_KP_DIVIDE	,//=SDLK_KP_DIVIDE,
	STX_KEY_KP_MULTIPLY	,//=SDLK_KP_MULTIPLY,
	STX_KEY_KP_MINUS	,//=SDLK_KP_MINUS,
	STX_KEY_KP_PLUS	,//=SDLK_KP_PLUS,
	STX_KEY_KP_ENTER	,//=SDLK_KP_ENTER,

	STX_KEY_DELETE	,//=SDLK_DELETE,
	STX_KEY_NUMLOCK	,//=SDLK_NUMLOCK,
	STX_KEY_CAPSLOCK	,//=SDLK_CAPSLOCK,
	STX_KEY_SCROLLOCK	,//=SDLK_SCROLLOCK,
	STX_KEY_RSHIFT	,//=SDLK_RSHIFT,
	STX_KEY_LSHIFT	,//=SDLK_LSHIFT,
	STX_KEY_RCTRL	,//=SDLK_RCTRL,
	STX_KEY_LCTRL	,//=SDLK_LCTRL,
	STX_KEY_RALT	,//=SDLK_RALT,
	STX_KEY_LALT	,//=SDLK_LALT,
	STX_KEY_RSUPER	,//=SDLK_RSUPER,
	STX_KEY_HELP	,//=SDLK_HELP,
	STX_KEY_PRINT	,//=SDLK_PRINT,
	STX_KEY_SYSREQ	,//=SDLK_SYSREQ,
	STX_KEY_BREAK	,//=SDLK_BREAK,
	STX_KEY_MENU	,//=SDLK_MENU,

	STX_KEY_BACKSPACE	,//=SDLK_BACKSPACE,
	STX_KEY_TAB		,//=SDLK_TAB,
	STX_KEY_CLEAR	,//=SDLK_CLEAR,
	STX_KEY_RETURN	,//=SDLK_RETURN,
	STX_KEY_PAUSE	,//=SDLK_PAUSE,
	STX_KEY_SPACE	,//=SDLK_SPACE,
	STX_KEY_EXCLAIM	,//=SDLK_EXCLAIM,
	STX_KEY_QUOTEDBL	,//=SDLK_QUOTEDBL,
	STX_KEY_HASH	,//=SDLK_HASH,
	STX_KEY_DOLLAR	,//=SDLK_DOLLAR,
	STX_KEY_AMPERSAND	,//=SDLK_AMPERSAND,
	STX_KEY_QUOTE	,//=SDLK_QUOTE,
	STX_KEY_LEFTPAREN	,//=SDLK_LEFTPAREN,
	STX_KEY_RIGHTPAREN	,//=SDLK_RIGHTPAREN,
	STX_KEY_ASTERISK	,//=SDLK_ASTERISK,
	STX_KEY_PLUS	,//=SDLK_PLUS,
	STX_KEY_COMMA	,//=SDLK_COMMA,
	STX_KEY_MINUS	,//=SDLK_MINUS,
	STX_KEY_PERIOD	,//=SDLK_PERIOD,
	STX_KEY_SLASH	,//=SDLK_SLASH,
	STX_KEY_COLON	,//=SDLK_COLON,
	STX_KEY_SEMICOLON	,//=SDLK_SEMICOLON,
	STX_KEY_LESS	,//=SDLK_LESS,
	STX_KEY_EQUALS	,//=SDLK_EQUALS,
	STX_KEY_GREATER	,//=SDLK_GREATER,
	STX_KEY_QUESTION	,//=SDLK_QUESTION,
	STX_KEY_AT		,//=SDLK_AT,
	STX_KEY_LEFTBRACKET	,//=SDLK_LEFTBRACKET,
	STX_KEY_BACKSLASH	,//=SDLK_BACKSLASH,
	STX_KEY_RIGHTBRACKET,//=SDLK_RIGHTBRACKET,
	STX_KEY_CARET	,//=SDLK_CARET,
	STX_KEY_UNDERSCORE	,//=SDLK_UNDERSCORE,
	STX_KEY_BACKQUOTE	,//=SDLK_BACKQUOTE,

	STX_KEY_F1		,//=SDLK_F1,
	STX_KEY_F2		,//=SDLK_F2,
	STX_KEY_F3		,//=SDLK_F3,
	STX_KEY_F4		,//=SDLK_F4,
	STX_KEY_F5		,//=SDLK_F5,
	STX_KEY_F6		,//=SDLK_F6,
	STX_KEY_F7		,//=SDLK_F7,
	STX_KEY_F8		,//=SDLK_F8,
	STX_KEY_F9		,//=SDLK_F9,
	STX_KEY_F10		,//=SDLK_F10,
	STX_KEY_F11		,//=SDLK_F11,
	STX_KEY_F12		,//=SDLK_F12,
	STX_KEY_F13		,//=SDLK_F13,
	STX_KEY_F14		,//=SDLK_F14,
	STX_KEY_F15		,//=SDLK_F15,

	STX_KEY_KP0		,//=SDLK_KP0,
	STX_KEY_KP1		,//=SDLK_KP1,
	STX_KEY_KP2		,//=SDLK_KP2,
	STX_KEY_KP3		,//=SDLK_KP3,
	STX_KEY_KP4		,//=SDLK_KP4,
	STX_KEY_KP5		,//=SDLK_KP5,
	STX_KEY_KP6		,//=SDLK_KP6,
	STX_KEY_KP7		,//=SDLK_KP7,
	STX_KEY_KP8		,//=SDLK_KP8,
	STX_KEY_KP9		,//=SDLK_KP9,

	STX_KEY_0		,//=SDLK_0,
	STX_KEY_1		,//=SDLK_1,
	STX_KEY_2		,//=SDLK_2,
	STX_KEY_3		,//=SDLK_3,
	STX_KEY_4		,//=SDLK_4,
	STX_KEY_5		,//=SDLK_5,
	STX_KEY_6		,//=SDLK_6,
	STX_KEY_7		,//=SDLK_7,
	STX_KEY_8		,//=SDLK_8,
	STX_KEY_9		,//=SDLK_9,

	STX_KEY_a		,//=SDLK_a,
	STX_KEY_b		,//=SDLK_b,
	STX_KEY_c		,//=SDLK_c,
	STX_KEY_d		,//=SDLK_d,
	STX_KEY_e		,//=SDLK_e,
	STX_KEY_f		,//=SDLK_f,
	STX_KEY_g		,//=SDLK_g,
	STX_KEY_h		,//=SDLK_h,
	STX_KEY_i		,//=SDLK_i,
	STX_KEY_j		,//=SDLK_j,
	STX_KEY_k		,//=SDLK_k,
	STX_KEY_l		,//=SDLK_l,
	STX_KEY_m		,//=SDLK_m,
	STX_KEY_n		,//=SDLK_n,
	STX_KEY_o		,//=SDLK_o,
	STX_KEY_p		,//=SDLK_p,
	STX_KEY_q		,//=SDLK_q,
	STX_KEY_r		,//=SDLK_r,
	STX_KEY_s		,//=SDLK_s,
	STX_KEY_t		,//=SDLK_t,
	STX_KEY_u		,//=SDLK_u,
	STX_KEY_v		,//=SDLK_v,
	STX_KEY_w		,//=SDLK_w,
	STX_KEY_x		,//=SDLK_x,
	STX_KEY_y		,//=SDLK_y,
	STX_KEY_z		,//=SDLK_z,
#endif
	// Vulkan:
	STX_KEY_KPADD,
	STX_KEY_KPSUB,
	STX_KEY_W,
	STX_KEY_S,
	STX_KEY_A,
	STX_KEY_D,
	STX_KEY_P,
	//STX_KEY_PERCENT	,//=SDLK_PERCENT,
} eKey;

#if defined(QT_BUILD)
/* Key state modifier keys */
#define STX_KEY_NUMLOCK 	STX_KEY_CONTROL
#define STX_KEY_CAPSLOCK 	STX_KEY_CONTROL
#define STX_KEY_SCROLLOCK 	STX_KEY_CONTROL
#define STX_KEY_RSHIFT	STX_KEY_CONTROL
#define STX_KEY_LSHIFT 	STX_KEY_CONTROL
#define STX_KEY_RCTRL 	STX_KEY_CONTROL
#define STX_KEY_LCTRL 	STX_KEY_CONTROL
#define STX_KEY_RALT 	STX_KEY_CONTROL
#define STX_KEY_LALT 	STX_KEY_CONTROL
#define STX_KEY_RMETA 	STX_KEY_CONTROL
#define STX_KEY_LMETA 	STX_KEY_CONTROL
#define STX_KEY_LSUPER 	STX_KEY_CONTROL
#define STX_KEY_RSUPER 	STX_KEY_CONTROL
#endif

/**
 * \brief Enumeration of valid key mods (possibly OR'd together).
 */
typedef enum
{
#if !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR) && !defined(ANDROID)
    STX_KMOD_NONE=KMOD_NONE,
    STX_KMOD_LSHIFT=KMOD_LSHIFT,
    STX_KMOD_RSHIFT=KMOD_RSHIFT,
    STX_KMOD_LCTRL=KMOD_LCTRL,
    STX_KMOD_RCTRL=KMOD_RCTRL,
    STX_KMOD_LALT=KMOD_LALT,
    STX_KMOD_RALT=KMOD_RALT,
    STX_KMOD_LGUI=KMOD_LGUI,
    STX_KMOD_RGUI=KMOD_RGUI,
    STX_KMOD_NUM=KMOD_NUM,
    STX_KMOD_CAPS=KMOD_CAPS,
    STX_KMOD_MODE=KMOD_MODE,
    STX_KMOD_RESERVED=KMOD_RESERVED
#else
    STX_KMOD_NONE,
    STX_KMOD_LSHIFT,
    STX_KMOD_RSHIFT,
    STX_KMOD_LCTRL,
    STX_KMOD_RCTRL,
    STX_KMOD_LALT,
    STX_KMOD_RALT,
    STX_KMOD_LGUI,
    STX_KMOD_RGUI,
    STX_KMOD_NUM,
    STX_KMOD_CAPS,
    STX_KMOD_MODE,
    STX_KMOD_RESERVED
#endif
} STX_Keymod;

#if !defined(OS_IPHONE) && !defined(IPHONE_SIMULATOR) && !defined(ANDROID)
#define STX_KMOD_CTRL (STX_KMOD_RESERVED+1)
#define STX_KMOD_SHIFT (STX_KMOD_RESERVED+2)
#define STX_KMOD_ALT (STX_KMOD_RESERVED+3)
#define STX_KMOD_GUI (STX_KMOD_RESERVED+4)
#else
#endif

//if(event.key.keysym.mod & KMOD_SHIFT)

	typedef enum _eKeyButton {
		STX_KEY_BUTTON_PRESSED
	} eKeyButton;

	typedef enum _eJoyButton {
		STX_JOYBUTTONDOWN,
		STX_JOYBUTTONUP
	} eJoyButton;

	typedef enum _eMouseButton {
       STX_MOUSE_MOTION,
       STX_MOUSE_BUTTONDOWN,
       STX_MOUSE_BUTTONUP,
	   STX_MBUTTON_LEFT,
	   STX_MBUTTON_RIGHT,
	   STX_MBUTTON_MIDDLE,
	   STX_MBUTTON_WHEELUP,
	   STX_MBUTTON_WHEELDOWN
	} eMouseButton;

#endif
class IInput
	{
		public:
public:
virtual void SetKey(unsigned int a){}
virtual void SetMouseX(int a){}
virtual void SetMouseY(int a){}
virtual void SetMouseLB(int a){}
virtual void SetMouseMB(int a){}
virtual void SetMouseRB(int a){}
virtual void SetKeyDown(eKey a){}
virtual void SetKeyUp(eKey a){}
     static float m_fSpinX;
     static float m_fSpinY;
     static float m_fX;
     static float m_fY;
     static float m_fZ;
     static float  m_fSpinX_L;
     static float  m_fSpinY_L;
     static float  m_fSpinX_R;
     static float  m_fSpinY_R;
     static Sint32 m_iWheelY;
			/// Default Constructor
			IInput(){};

			/// Default Destructor
			virtual ~IInput(){};


			/// Tells if a key is inbetween being pressed and released
			virtual bool IsKeyHeld(eKey Key)=0;

			/// Tells if a key is pressed
			virtual bool IsKeyDown(eKey Key)=0;

			/// Tells if a key is released
			virtual bool IsKeyUp(eKey Key)=0;

			/// Updates keyboard events

			virtual bool Update()=0;

virtual bool OnShiftKeyPress()=0;
virtual bool OnLShiftKeyPress()=0;
virtual bool OnRShiftKeyPress()=0;

virtual bool OnCTRLKeyPress()=0;
virtual bool OnLCTRLKeyPress()=0;
virtual bool OnRCTRLKeyPress()=0;

virtual bool OnALTKeyPress()=0;
virtual bool OnLALTKeyPress()=0;
virtual bool OnRALTKeyPress()=0;

virtual bool OnNUMKeyPress()=0;
virtual bool OnCAPSKeyPress()=0;
virtual bool OnMODEKeyPress()=0;

	virtual bool OnKeyPress				(eKey pKey)=0;
	virtual bool OnKeyRelease			(eKey pKey)=0;
	virtual bool IsKeyPressed			(eKey pKey)=0;
	virtual bool IsKeyPressed			(eKey pKey, unsigned int pTime)=0;

	virtual bool IsKeyPressed(void)=0;
	virtual bool IsKeyReleased(void)=0;

	virtual bool IsWindowMoved(void)=0;
	virtual bool IsWindowResized(void)=0;
	virtual bool IsWindowMinimised(void)=0;
	virtual bool IsWindowMaximised(void)=0;
	virtual bool IsWindowClosed(void)=0;

	virtual bool IsMouseClick(void)=0;
	virtual bool IsMouseDoubleClick(void)=0;
	virtual bool IsMouseButtonPressed(void)=0;
	virtual bool IsMouseButtonReleased(void)=0;
	virtual bool IsMouseButton			()=0;
	virtual bool IsMouseMotion			()=0;
	virtual int GetMouseX				()=0;
	virtual int GetMouseY				()=0;
	virtual bool OnMouseButtonPress		(eMouseButton pMouseButton)=0;
	virtual bool OnMouseButtonRelease	(eMouseButton pMouseButton)=0;
	virtual bool IsMouseButtonPressed	(eMouseButton pMouseButton)=0;
	virtual bool IsMouseButtonPressed	(eMouseButton pMouseButton, unsigned int pTime)=0;

	virtual float& GetSpinX();
	virtual float& GetSpinY();
	virtual float& GetX();
	virtual float& GetY();
	virtual float& GetZ();
	virtual float& GetSpinX_L();
	virtual float& GetSpinY_L();
	virtual float& GetSpinX_R();
	virtual float& GetSpinY_R();

	virtual int GetJoyX()=0;
	virtual int GetJoyY()=0;
	virtual bool IsJoyAxisMoving()=0;
	virtual bool IsJoyBallMoving()=0;
	virtual bool IsJoyHatMoving()=0;
	virtual bool IsJoyButton			()=0;

	virtual Uint8 GetAxisJoystickDeviceIndex()=0;	/**< The joystick device index */
	virtual Uint8 GetAxisIndex()=0;	/**< The joystick axis index */
	virtual Sint16 GetAxisValue()=0;	/**< The axis value (range: -32768 to 32767) */

	virtual Uint8 GetBallJoystickDeviceIndex()=0;	/**< The joystick device index */
	virtual Uint8 GetBallIndex()=0;	/**< The joystick trackball index */
	virtual Sint16 GetBallXRel()=0;	/**< The relative motion in the X direction */
	virtual Sint16 GetBallYRel()=0;	/**< The relative motion in the Y direction */

	virtual Uint8 GetHatJoystickDeviceIndex()=0;	/**< The joystick device index */
	virtual Uint8 GetHatIndex()=0;	/**< The joystick hat index */
	virtual Uint8 GetHatValue()=0;	/**< The hat position value:*/

	virtual Uint8 GetButtonJoystickDeviceIndex()=0;	/**< The joystick device index */
	virtual Uint8 GetButtonIndex()=0;	/**< The joystick button index */
	virtual Uint8 GetButtonState()=0;	/**< SDL_PRESSED or SDL_RELEASED */
	virtual bool Quit ()=0;
	virtual int getKeyID()=0;
	virtual char getKeyChar()=0;
	virtual Sint32 getKeyCode()=0;
	};
#if 1//def SWIG

class DummyInput : public IInput
	{
		private:
			std::map<int, char> m_Keystates;	/**< Holds the state of the keys */
		protected:
int MouseX;
int MouseY;
int MouseLB;
int MouseMB;
int MouseRB;
unsigned int Key;
		public:
/*
case SDL_KEYUP:
					m_Keystates[SDLKtoKEY(event.key.keysym.sym)] = 'u';
case SDL_KEYDOWN:
					m_Keystates[SDLKtoKEY(event.key.keysym.sym)] = 'd';

 void TextEdit::keyPressEvent(QKeyEvent *e)
 {
     if (c && c->popup()->isVisible()) {
         // The following keys are forwarded by the completer to the widget
        switch (e->key()) {

*/
virtual void SetKeyDown(eKey a){m_Keystates[a]='d';}
virtual void SetKeyUp(eKey a){m_Keystates[a]='u';}
virtual void SetKey(unsigned int a){Key=a;}
virtual void SetMouseX(int a){MouseX=a;}
virtual void SetMouseY(int a){MouseY=a;}
virtual void SetMouseLB(int a){MouseLB=a;}
virtual void SetMouseMB(int a){MouseMB=a;}
virtual void SetMouseRB(int a){MouseRB=a;}
			/// Default Constructor
                        DummyInput():IInput()
{
Key=0;
MouseX=0;
MouseY=0;
MouseLB=0;
MouseMB=0;
MouseRB=0;
}
virtual void clear()
{
Key=0;
MouseX=0;
MouseY=0;
MouseLB=0;
MouseMB=0;
MouseRB=0;
}

			/// Default Destructor
                        virtual ~DummyInput(){}

			/// Updates keyboard events
			virtual bool Update()
{
/*
for( std::map<int, char>::iterator itr = m_Keystates.begin(); itr != m_Keystates.end(); itr++)
		{
			//put no status flag
			if( itr->second == 'u' )
			{
			}
		}
*/
		//check to see if any keys haven't been released but were press
		// ie being held
/*
		for( std::map<int, char>::iterator itr = m_Keystates.begin(); itr != m_Keystates.end(); itr++)
		{
			//put no status flag
			if( itr->second == 'u' )
			{
				bool keyFound = false;
				for(int i = 0; i < keys.size(); i++)
				{
					if(keys[i] == itr->first)
					{
						keyFound = true;
						break;
					}
				}

				if( !keyFound )
				{
					itr->second = 'n';
				}
			}
			else if( itr->second == 'd')
			{
				bool keyFound = false;
				for(int i = 0; i < keys.size(); i++)
				{
					if(keys[i] == itr->first)
					{
						keyFound = true;
						break;
					}
				}

				if( !keyFound )
				{
					itr->second = 'h';
				}
			}
		}
*/

return true;
}

virtual bool OnShiftKeyPress(){return false;}
virtual bool OnLShiftKeyPress(){return false;}
virtual bool OnRShiftKeyPress(){return false;}

virtual bool OnCTRLKeyPress(){return false;}
virtual bool OnLCTRLKeyPress(){return false;}
virtual bool OnRCTRLKeyPress(){return false;}

virtual bool OnALTKeyPress(){return false;}
virtual bool OnLALTKeyPress(){return false;}
virtual bool OnRALTKeyPress(){return false;};

virtual bool OnNUMKeyPress(){return false;}
virtual bool OnCAPSKeyPress(){return false;}
virtual bool OnMODEKeyPress(){return false;}

#if 1
virtual bool OnKeyPress	(eKey pKey)
{
if(m_Keystates[pKey] == 'd')
		{
			return true;
		}
	return false;
}

virtual bool OnKeyRelease	(eKey pKey)
{
if(m_Keystates[pKey] == 'u')
		{
			return true;
		}
	return false;
}
	//
	// Tells if a key is inbetween being pressed and released
	//
	virtual bool IsKeyHeld(eKey Key)
	{
		if(m_Keystates[Key] == 'h')
		{
			return true;
		}

		return false;
	}

	//
	// Tells if a key is pressed
	virtual bool IsKeyDown(eKey Key)
	{
		if(m_Keystates[Key] == 'd')
		{
			return true;
		}

		return false;
	}

	//
	// Tells if a key is released
	//
	virtual bool IsKeyUp(eKey Key)
	{
		if(m_Keystates[Key] == 'u')
		{
			return true;
		}

		return false;
	}
	virtual bool IsKeyPressed(eKey a){return IsKeyDown(a);}
	virtual bool IsKeyPressed(eKey a, unsigned int){return IsKeyDown(a);}
#else			/// Tells if a key is inbetween being pressed and released
			virtual bool IsKeyHeld(eKey Key){return false;}

			/// Tells if a key is pressed
			virtual bool IsKeyDown(eKey Key){return false;}

			/// Tells if a key is released
			virtual bool IsKeyUp(eKey Key){return false;}

	virtual bool OnKeyPress				(eKey pKey){return false;}
	virtual bool OnKeyRelease			(eKey pKey){return false;}
	virtual bool IsKeyPressed			(eKey pKey){return false;}
	virtual bool IsKeyPressed			(eKey pKey, unsigned int pTime){return false;}
#endif
	virtual bool IsKeyPressed(void){return false;}
	virtual bool IsKeyReleased(void){return false;}

	virtual bool IsWindowMoved(void){return false;}
	virtual bool IsWindowResized(void){return false;}
	virtual bool IsWindowMinimised(void){return false;}
	virtual bool IsWindowMaximised(void){return false;}
	virtual bool IsWindowClosed(void){return false;}

	virtual bool IsMouseClick(void){return false;}
	virtual bool IsMouseDoubleClick(void){return false;}
	virtual bool IsMouseButtonPressed(void){return false;}
	virtual bool IsMouseButtonReleased(void){return false;}
	virtual bool IsMouseButton			(){return MouseLB||MouseMB||MouseRB;}
	virtual bool IsMouseMotion			(){return false;}
	virtual int GetMouseX				(){return MouseX;}
	virtual int GetMouseY				(){return MouseY;}
	virtual bool OnMouseButtonPress		(eMouseButton pMouseButton)
{
if((pMouseButton==STX_MBUTTON_LEFT)&&(MouseLB)) return true;
if((pMouseButton==STX_MBUTTON_RIGHT)&&(MouseRB)) return true;
if((pMouseButton==STX_MBUTTON_MIDDLE)&&(MouseMB)) return true;
return false;
}
	virtual bool OnMouseButtonRelease	(eMouseButton pMouseButton){
//if((pMouseButton==STX_MBUTTON_LEFT)&&(!MouseLB)) return true;//???
//if((pMouseButton==STX_MBUTTON_RIGHT)&&(!MouseRB)) return true;
//if((pMouseButton==STX_MBUTTON_MIDDLE)&&(!MouseMB)) return true;
return false;
}
	virtual bool IsMouseButtonPressed	(eMouseButton pMouseButton){
if((pMouseButton==STX_MBUTTON_LEFT)&&(MouseLB)) return true;
if((pMouseButton==STX_MBUTTON_RIGHT)&&(MouseRB)) return true;
if((pMouseButton==STX_MBUTTON_MIDDLE)&&(MouseMB)) return true;
return false;
}
	virtual bool IsMouseButtonPressed	(eMouseButton pMouseButton, unsigned int pTime){
if((pMouseButton==STX_MBUTTON_LEFT)&&(MouseLB)) return true;
if((pMouseButton==STX_MBUTTON_RIGHT)&&(MouseRB)) return true;
if((pMouseButton==STX_MBUTTON_MIDDLE)&&(MouseMB)) return true;
return false;
}

	virtual int GetJoyX(){return 0;}
	virtual int GetJoyY(){return 0;}
	virtual bool IsJoyAxisMoving(){return false;}
	virtual bool IsJoyBallMoving(){return false;}
	virtual bool IsJoyHatMoving(){return false;}
	virtual bool IsJoyButton			(){return false;}

	virtual Uint8 GetAxisJoystickDeviceIndex(){return 0;}	/**< The joystick device index */
	virtual Uint8 GetAxisIndex(){return 0;}	/**< The joystick axis index */
	virtual Sint16 GetAxisValue(){return 0;}	/**< The axis value (range: -32768 to 32767) */

	virtual Uint8 GetBallJoystickDeviceIndex(){return 0;}	/**< The joystick device index */
	virtual Uint8 GetBallIndex(){return 0;}	/**< The joystick trackball index */
	virtual Sint16 GetBallXRel(){return 0;}	/**< The relative motion in the X direction */
	virtual Sint16 GetBallYRel(){return 0;}	/**< The relative motion in the Y direction */

	virtual Uint8 GetHatJoystickDeviceIndex(){return 0;}	/**< The joystick device index */
	virtual Uint8 GetHatIndex(){return 0;}	/**< The joystick hat index */
	virtual Uint8 GetHatValue(){return 0;}	/**< The hat position value:*/

	virtual Uint8 GetButtonJoystickDeviceIndex(){return 0;}	/**< The joystick device index */
	virtual Uint8 GetButtonIndex(){return 0;}	/**< The joystick button index */
	virtual Uint8 GetButtonState(){return 0;}	/**< SDL_PRESSED or SDL_RELEASED */
	virtual bool Quit (){return false;}
	virtual int getKeyID(){return 0;}
	virtual char getKeyChar(){return '\0';}
	virtual Sint32 getKeyCode(){return 0;}
    };
#if defined(SDL_1_2_15_BUILD) || defined(SDL_2_0_5_BUILD)
extern SDL_Event event;
	class SDLInput : public IInput
	{
		public:
			/// Default Constructor
			SDLInput(){m_quit=false;}

			/// Default Destructor
			virtual ~SDLInput(){}

			/// Tells if a key is inbetween being pressed and released
			virtual bool IsKeyHeld(eKey Key);

			/// Tells if a key is pressed
			virtual bool IsKeyDown(eKey Key);

			/// Tells if a key is released
			virtual bool IsKeyUp(eKey Key);

			/// Updates keyboard events
			virtual bool Update();

virtual bool OnShiftKeyPress(){return (event.key.keysym.mod & KMOD_SHIFT);}
virtual bool OnLShiftKeyPress(){return (event.key.keysym.mod & KMOD_LSHIFT);}
virtual bool OnRShiftKeyPress(){return (event.key.keysym.mod & KMOD_RSHIFT);}

virtual bool OnCTRLKeyPress(){return (event.key.keysym.mod & KMOD_CTRL);}
virtual bool OnLCTRLKeyPress(){return (event.key.keysym.mod & KMOD_LCTRL);}
virtual bool OnRCTRLKeyPress(){return (event.key.keysym.mod & KMOD_RCTRL);}

virtual bool OnALTKeyPress(){return (event.key.keysym.mod & KMOD_ALT);}
virtual bool OnLALTKeyPress(){return (event.key.keysym.mod & KMOD_LALT);}
virtual bool OnRALTKeyPress(){return (event.key.keysym.mod & KMOD_RALT);}

virtual bool OnNUMKeyPress(){return (event.key.keysym.mod & KMOD_NUM);}
virtual bool OnCAPSKeyPress(){return (event.key.keysym.mod & KMOD_CAPS);}
virtual bool OnMODEKeyPress(){return (event.key.keysym.mod & KMOD_MODE);}

	virtual bool OnKeyPress				(eKey pKey);
	virtual bool OnKeyRelease			(eKey pKey);
	virtual bool IsKeyPressed			(eKey pKey);
	virtual bool IsKeyPressed			(eKey pKey, unsigned int pTime);

	virtual bool IsKeyPressed(void);
	virtual bool IsKeyReleased(void);

	virtual bool IsWindowMoved(void);
	virtual bool IsWindowResized(void);
	virtual bool IsWindowMinimised(void);
	virtual bool IsWindowMaximised(void);
	virtual bool IsWindowClosed(void);

	virtual bool IsMouseClick(void);
	virtual bool IsMouseDoubleClick(void);

	virtual bool IsMouseButtonPressed(void);
	virtual bool IsMouseButtonReleased(void);
	virtual bool IsMouseButton			();
	virtual bool IsMouseMotion			();
	virtual int GetMouseX				();
	virtual int GetMouseY				();
	virtual bool MouseButtonPressed		(eMouseButton pMouseButton);
	virtual bool OnMouseButtonPress		(eMouseButton pMouseButton);
	virtual bool OnMouseButtonRelease	(eMouseButton pMouseButton);
	virtual bool IsMouseButtonPressed	(eMouseButton pMouseButton);
	virtual bool IsMouseButtonPressed	(eMouseButton pMouseButton, unsigned int pTime);


	virtual int GetJoyX();
	virtual int GetJoyY();
	virtual bool IsJoyAxisMoving();
	virtual bool IsJoyBallMoving();
	virtual bool IsJoyHatMoving();
	virtual bool IsJoyButton			();

	virtual Uint8 GetAxisJoystickDeviceIndex();	/**< The joystick device index */
	virtual Uint8 GetAxisIndex();	/**< The joystick axis index */
	virtual Sint16 GetAxisValue();	/**< The axis value (range: -32768 to 32767) */

	virtual Uint8 GetBallJoystickDeviceIndex();	/**< The joystick device index */
	virtual Uint8 GetBallIndex();	/**< The joystick trackball index */
	virtual Sint16 GetBallXRel();	/**< The relative motion in the X direction */
	virtual Sint16 GetBallYRel();	/**< The relative motion in the Y direction */

	virtual Uint8 GetHatJoystickDeviceIndex();	/**< The joystick device index */
	virtual Uint8 GetHatIndex();	/**< The joystick hat index */
	virtual Uint8 GetHatValue();	/**< The hat position value:*/

	virtual Uint8 GetButtonJoystickDeviceIndex();	/**< The joystick device index */
	virtual Uint8 GetButtonIndex();	/**< The joystick button index */
	virtual Uint8 GetButtonState();	/**< SDL_PRESSED or SDL_RELEASED */
	virtual bool Quit ();
	virtual int getKeyID();
	virtual char getKeyChar();
	virtual Sint32 getKeyCode();

		private:

			std::map<int, char> m_Keystates;	/**< Holds the state of the keys */
			int m_MouseX;
			int m_MouseY;
			std::vector< char > m_MouseButtons;
			bool m_quit;


	};
#endif

struct STX_Service
{
protected:
    #if defined(SDL_2_0_5_BUILD) && !defined(SWIG)
    static SDL_Window* g_SDL_window;
    static SDL_GLContext g_SDL_context;
public:
    static SDL_Window* GetSDLWindow(){return g_SDL_window;}
    static SDL_GLContext GetSDLContext(){return g_SDL_context;}
    static void SetSDLWindow(SDL_Window* a){g_SDL_window=a;}
    static void SetSDLContext(SDL_GLContext& a){g_SDL_context=a;}
    #endif
public:
#if 0
static bool m_ReplayOn;
static unsigned int m_ReplayIndex;
static bool IsReplayOn(){return m_ReplayOn;}
static bool ReplayOn(){return m_ReplayOn=true;ResetReplayIndex();}
static bool ReplayOff(){return m_ReplayOn=false;ResetReplayIndex();}
static unsigned int GetReplayIndex(){return m_ReplayIndex;}
static void ResetReplayIndex(){m_ReplayIndex=0;}
static unsigned int IncrementReplayIndex(){return m_ReplayIndex++;}
#endif
#if 0
virtual int renderReplay()
{
list::pop_front
list::push_back
list::pop_back
struct ReplayFrame
{
	IND_Camera2d Camera2d;
	IND_Camera3d Camera3d;
	D3DFROMWINEVIEWPORT9 ViewPort2d;
	D3DFROMWINEVIEWPORT9 ViewPort3d;

	std::vector <IND_Entity2d> mListEntities2d  [NUM_LAYERS];
	std::vector <IND_Entity3d> mListEntities3d;
};
std::list<ReplayFrame> frames;
	ReplayFrame frame;
	frame.Camera2d=CIndieLib::Instance()->Render->GetCamera2d;
	frame.Camera3d=CIndieLib::Instance()->Render->GetCamera3d;
	frame.ViewPort2d=CIndieLib::Instance()->Render->GetViewPort2d;
	frame.ViewPort3d=CIndieLib::Instance()->Render->GetViewPort3d;
	for(unsigned int i=0;i<NUM_LAYERS;i++)
	frame.mListEntities2d  [i]=CIndieLib::Instance()->Entity2dManager->GetEntityVector(i);
	frame.mListEntities3d  =CIndieLib::Instance()->Entity3dManager->GetEntityVector();
		if(frames.size()) frames.pop_front();
		frames.push_back(frame);

		// ----- Render  -----
	vector <IND_Entity2d*> *mListEntities2d  [NUM_LAYERS];
	vector <IND_Entity3d*> *mListEntities3d;

		mI->Render->BeginScene ();

		mI->Render->SetViewPort2d (frames[i].ViewPort2d.X, frames[i].ViewPort2d.Y, frames[i].ViewPort2d.Width, frames[i].ViewPort2d.Height);
		mI->Render->SetCamera2d (&frames[i].Camera2d);
		mI->Entity2dManager->RenderEntities2d ();

		mI->Render->SetViewPort3d (frames[i].ViewPort3d.X, frames[i].ViewPort3d.Y, frames[i].ViewPort3d.Width, frames[i].ViewPort3d.Height);
		mI->Render->SetCamera3d (&frames[i].Camera3d);
		mI->Entity3dManager->RenderEntities3d ();

		mI->Render->EndScene ();


}
#endif
#if !defined(SWIG)
static std::string LoadTxtFile_(const std::string aFileName, std::string& file);
static void LoadTxtFile(const std::string aFileName, std::vector<std::string>& file);
static std::string LoadTxtFile(const std::string aFileName, std::string& contents);
static void WriteTxtFile(const char* aFileName, const char* contents);
static std::vector<BYTE> ReadFile(const char* filename);
#endif

static void CloseWindow();
static void CloseInput();
static void CloseAudio();
static void CloseTimer();

static void CloseAll();
#if !defined(SWIG)
static IWindow* s_pWindow;
static IInput* s_pInput;
static IAudio * s_pAudio;
static STX_TIMER* s_pTimer;
#endif
public:
static Uint32 GetDelay();
static void PrintDelay(const char* file="", const char* function="", int line=0);
#if 0
static IWindow* GetWindowInstance(tWindow a=eDummy_Window);
#elif defined(SDL_1_2_15_BUILD) || defined(SDL_2_0_5_BUILD)
static IWindow* GetWindowInstance(tWindow a=eSDL_Window);
#else
static IWindow* GetWindowInstance(tWindow a=eDummy_Window);
#endif

#if 0
static IInput* GetInputInstance(tInput a=eDummy_Input);
#elif defined(SDL_1_2_15_BUILD) || defined(SDL_2_0_5_BUILD)
static IInput* GetInputInstance(tInput a=eSDL_Input);
#else
static IInput* GetInputInstance(tInput a=eDummy_Input);
#endif

#if defined(SDL_1_2_15_BUILD) || defined(SDL_2_0_5_BUILD)
static IAudio * GetAudioInstance(tAudio a=eSDL_Audio);
#else
static IAudio * GetAudioInstance(tAudio a=eOpenAL_Audio);
#endif


static STX_TIMER* GetTimerInstance();
//static STX_STDINC* GetSTDINCInstance();
};

#if defined(SDL_1_2_15_BUILD) || defined(SDL_2_0_5_BUILD)
struct STXAPI STX_SDL_TIMER : public STX_TIMER
{
	STX_SDL_TIMER():STX_TIMER(){}
	virtual ~STX_SDL_TIMER(){}
	virtual Uint32 stx_GetTicks(STX_TIMER* _Timer);
	virtual void stx_Delay(Uint32 ms,STX_TIMER* _Timer);
};
#endif
#endif
#if defined(SDL_VIDEO_DRIVER_WINDOWS) || defined(__MINGW32__) || defined(__MINGW64__)
HINSTANCE __GetINSTANCE__();
HWND __GetHWND__();
#endif
/*
std::string stx_itoa(const unsigned int& ai);
*/
std::vector<std::string> stx_split(std::string const &input);

template<typename Out>
void stx_split(const std::string &s, char delim, Out result);

std::vector<std::string> stx_split(const std::string &s, char delim);

std::string stx_join(std::vector<std::string>& v, char delim);

std::vector<std::string> stx_split(const std::string &s, const std::string &delim);

std::string stx_join(std::vector<std::string>& v, const std::string &delim);

// trim from start
std::string &stx_ltrim(std::string &s);

// trim from end
std::string &stx_rtrim(std::string &s);

// trim from both ends
std::string &stx_trim(std::string &s);

void stx_GetAngles(float& aAngleX, float& aAngleY, float mSpeedRotation=0.05f);


struct Vertex
{
	D3DXFROMWINEVECTOR3 position;
	D3DXFROMWINEVECTOR2 texcoord;
	Vertex(D3DXFROMWINEVECTOR3 aposition=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR2 atexcoord=D3DXFROMWINEVECTOR2(0.0f, 0.0f))
	{
		position=aposition;
		texcoord=atexcoord;
	}
	Vertex(const Vertex& rhs)
	{
		position=rhs.position;
		texcoord=rhs.texcoord;
	}
};

struct VertexC
{
	D3DXFROMWINEVECTOR3 position;
	D3DXFROMWINEVECTOR4 color;
	D3DXFROMWINEVECTOR2 texcoord;
	VertexC(D3DXFROMWINEVECTOR3 aposition=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR4 acolor=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), D3DXFROMWINEVECTOR2 atexcoord=D3DXFROMWINEVECTOR2(0.0f, 0.0f))
	{
		position=aposition;
		color=acolor;
		texcoord=atexcoord;
	}
	VertexC(const VertexC& rhs)
	{
		position=rhs.position;
		color=rhs.color;
		texcoord=rhs.texcoord;
	}
};

struct Vertex332
{
	D3DXFROMWINEVECTOR3 position;
	D3DXFROMWINEVECTOR3 color;
	D3DXFROMWINEVECTOR2 texcoord;
	Vertex332(D3DXFROMWINEVECTOR3 aposition=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3 acolor=D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f), D3DXFROMWINEVECTOR2 atexcoord=D3DXFROMWINEVECTOR2(0.0f, 0.0f))
	{
		position=aposition;
		color=acolor;
		texcoord=atexcoord;
	}
	Vertex332(const Vertex332& rhs)
	{
		position=rhs.position;
		color=rhs.color;
		texcoord=rhs.texcoord;
	}
};



struct Vertex332N
{
	D3DXFROMWINEVECTOR3 position;
	D3DXFROMWINEVECTOR3 normal;
	D3DXFROMWINEVECTOR2 uv;
	Vertex332N()
	{
		position=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		normal=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		uv=D3DXFROMWINEVECTOR2(0.0f, 0.0f);
	}
	Vertex332N(const Vertex332N& rhs)
	{
		position=rhs.position;
		normal=rhs.normal;
		uv=rhs.uv;
	}
};

    struct VertexPositionTexture
    {
        D3DXFROMWINEVECTOR3 Position;
        D3DXFROMWINEVECTOR2 TexCoord;
        VertexPositionTexture(D3DXFROMWINEVECTOR3 aPosition, D3DXFROMWINEVECTOR2 aTexCoord)
        {
            Position = aPosition;
            TexCoord = aTexCoord;
        }
	VertexPositionTexture()
        {
            Position = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
            TexCoord = D3DXFROMWINEVECTOR2(0.0f, 0.0f);
        }
	VertexPositionTexture(const VertexPositionTexture& rhs)
	{
            Position = rhs.Position;
            TexCoord = rhs.TexCoord;
	}
	};

    struct stx_VertexPositionTexture
    {
        D3DXFROMWINEVECTOR3 Position;
        D3DXFROMWINEVECTOR2 Tex;
	stx_VertexPositionTexture(	D3DXFROMWINEVECTOR3 position = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), 
					D3DXFROMWINEVECTOR2 tex=D3DXFROMWINEVECTOR2(0.0f, 0.0f))
        {
            Position = position;
	    Tex = tex;
        }
	stx_VertexPositionTexture(const stx_VertexPositionTexture& rhs)
        {
            Position = rhs.Position;
	    Tex = rhs.Tex;
        }
    };

    struct stx_VertexPositionNormalTexture_
    {
        D3DXFROMWINEVECTOR3 Position;
        D3DXFROMWINEVECTOR3 Normal;
        D3DXFROMWINEVECTOR2 Tex;
	stx_VertexPositionNormalTexture_(	D3DXFROMWINEVECTOR3 position = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), 
					D3DXFROMWINEVECTOR3 normal = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f),
					D3DXFROMWINEVECTOR2 tex=D3DXFROMWINEVECTOR2(0.0f, 0.0f))
        {
            Position = position;
            Normal = normal;
	    Tex = tex;
        }
	stx_VertexPositionNormalTexture_(const stx_VertexPositionNormalTexture_& rhs)
        {
            Position = rhs.Position;
            Normal = rhs.Normal;
	    Tex = rhs.Tex;
        }
    };

    struct stx_VertexPositionColor
    {
        D3DXFROMWINEVECTOR3 Position;
	D3DXFROMWINEVECTOR4 Color;
        stx_VertexPositionColor(D3DXFROMWINEVECTOR3 aPosition, D3DXFROMWINEVECTOR4 aColor)
        {
            Position = aPosition;
	    Color = aColor;
        }
	stx_VertexPositionColor()
        {
            Position = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
	    Color = D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
        }
        stx_VertexPositionColor(const stx_VertexPositionColor& rhs)
        {
            Position = rhs.Position;
	    Color = rhs.Color;
        }
	};

    struct stx_VertexPositionColor3Texture
    {
        D3DXFROMWINEVECTOR3 Position;
        D3DXFROMWINEVECTOR3 Color3;
        D3DXFROMWINEVECTOR2 Tex;
	stx_VertexPositionColor3Texture(	D3DXFROMWINEVECTOR3 position = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), 
					D3DXFROMWINEVECTOR3 color3 = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f),
					D3DXFROMWINEVECTOR2 tex=D3DXFROMWINEVECTOR2(0.0f, 0.0f))
        {
            Position = position;
            Color3 = color3;
	    Tex = tex;
        }
	stx_VertexPositionColor3Texture(const stx_VertexPositionColor3Texture& rhs)
        {
            Position = rhs.Position;
            Color3 = rhs.Color3;
	    Tex = rhs.Tex;
        }
    };

    struct stx_VertexPositionColor4Texture
    {
        D3DXFROMWINEVECTOR3 Position;
        D3DXFROMWINEVECTOR4 Color4;
        D3DXFROMWINEVECTOR2 Tex;
	stx_VertexPositionColor4Texture(	D3DXFROMWINEVECTOR3 position = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), 
					D3DXFROMWINEVECTOR4 color4 = D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f),
					D3DXFROMWINEVECTOR2 tex=D3DXFROMWINEVECTOR2(0.0f, 0.0f))
        {
            Position = position;
            Color4 = color4;
	    Tex = tex;
        }
	stx_VertexPositionColor4Texture(const stx_VertexPositionColor4Texture& rhs)
        {
            Position = rhs.Position;
            Color4 = rhs.Color4;
	    Tex = rhs.Tex;
        }
    };

    struct stx_VertexPositionNormalTexture
    {
        D3DXFROMWINEVECTOR3 Position;
        D3DXFROMWINEVECTOR3 Normal;
        D3DXFROMWINEVECTOR3 BiNormal;
        D3DXFROMWINEVECTOR3 Tangent;
        D3DXFROMWINEVECTOR3 Color;
        D3DXFROMWINEVECTOR2 Tex;
	stx_VertexPositionNormalTexture(	D3DXFROMWINEVECTOR3 position = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), 
					D3DXFROMWINEVECTOR3 normal = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f),
					D3DXFROMWINEVECTOR2 tex=D3DXFROMWINEVECTOR2(0.0f, 0.0f),
					D3DXFROMWINEVECTOR3 binormal = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f),
					D3DXFROMWINEVECTOR3 tangent = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f))
        {
            Position = position;
            Normal = normal;
            BiNormal = binormal;
	    Tangent = tangent;
		Color=D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f);
	    Tex = tex;
        }
	stx_VertexPositionNormalTexture(const stx_VertexPositionNormalTexture& rhs)
        {
            Position = rhs.Position;
            Normal = rhs.Normal;
            BiNormal = rhs.BiNormal;
	    Tangent = rhs.Tangent;
	    Color=rhs.Color;
	    Tex = rhs.Tex;
        }
    };

    struct stx_VertexPositionNormalNormal
    {
        D3DXFROMWINEVECTOR3 Position;
        D3DXFROMWINEVECTOR3 Normal0;
        D3DXFROMWINEVECTOR3 Normal1;
	stx_VertexPositionNormalNormal(	D3DXFROMWINEVECTOR3 position = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), 
					D3DXFROMWINEVECTOR3 normal0 = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f),
					D3DXFROMWINEVECTOR3 normal1 = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f))
        {
            Position = position;
            Normal0 = normal0;
		D3DXFROMWINEVec3Normalize(&Normal0,&Position);
            Normal1 = normal1;
        }
	stx_VertexPositionNormalNormal(const stx_VertexPositionNormalNormal& rhs)
        {
            Position = rhs.Position;
            Normal0 = rhs.Normal0;
            Normal1 = rhs.Normal1;
        }
    };

    struct stx_VertexPositionNormalBiNormalTangentColor3Texture
    {
        D3DXFROMWINEVECTOR3 Position;
        D3DXFROMWINEVECTOR3 Normal;
        D3DXFROMWINEVECTOR3 BiNormal;
        D3DXFROMWINEVECTOR3 Tangent;
        D3DXFROMWINEVECTOR3 Color3;
        D3DXFROMWINEVECTOR2 Tex;
	stx_VertexPositionNormalBiNormalTangentColor3Texture(	D3DXFROMWINEVECTOR3 position = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), 
					D3DXFROMWINEVECTOR3 normal = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f),
					D3DXFROMWINEVECTOR2 tex=D3DXFROMWINEVECTOR2(0.0f, 0.0f),
					D3DXFROMWINEVECTOR3 binormal = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f),
					D3DXFROMWINEVECTOR3 tangent = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), 
					D3DXFROMWINEVECTOR3 color3 = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f))
        {
            Position = position;
            Normal = normal;
            BiNormal = binormal;
	    Tangent = tangent;
		Color3=color3;
	    Tex = tex;
        }
	stx_VertexPositionNormalBiNormalTangentColor3Texture(const stx_VertexPositionNormalBiNormalTangentColor3Texture& rhs)
        {
            Position = rhs.Position;
            Normal = rhs.Normal;
            BiNormal = rhs.BiNormal;
	    Tangent = rhs.Tangent;
	    Color3=rhs.Color3;
	    Tex = rhs.Tex;
        }
    };

    struct stx_VertexPositionNormalBiNormalTangentColor4Texture
    {
        D3DXFROMWINEVECTOR3 Position;
        D3DXFROMWINEVECTOR3 Normal;
        D3DXFROMWINEVECTOR3 BiNormal;
        D3DXFROMWINEVECTOR3 Tangent;
        D3DXFROMWINEVECTOR4 Color4;
        D3DXFROMWINEVECTOR2 Tex;
	stx_VertexPositionNormalBiNormalTangentColor4Texture(	D3DXFROMWINEVECTOR3 position = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), 
					D3DXFROMWINEVECTOR3 normal = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f),
					D3DXFROMWINEVECTOR2 tex=D3DXFROMWINEVECTOR2(0.0f, 0.0f),
					D3DXFROMWINEVECTOR3 binormal = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f),
					D3DXFROMWINEVECTOR3 tangent = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), 
					D3DXFROMWINEVECTOR4 color4 = D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f))
        {
            Position = position;
            Normal = normal;
            BiNormal = binormal;
	    Tangent = tangent;
		Color4=color4;
	    Tex = tex;
        }
	stx_VertexPositionNormalBiNormalTangentColor4Texture(const stx_VertexPositionNormalBiNormalTangentColor4Texture& rhs)
        {
            Position = rhs.Position;
            Normal = rhs.Normal;
            BiNormal = rhs.BiNormal;
	    Tangent = rhs.Tangent;
	    Color4=rhs.Color4;
	    Tex = rhs.Tex;
        }
    };
struct stVertex2
	{
		D3DXFROMWINEVECTOR3 pos;
		D3DXFROMWINEVECTOR3 norm;
		D3DXFROMWINEVECTOR2 tex;
	};
		struct VERTEX
		{
			D3DXFROMWINEVECTOR3 pos;
			D3DXFROMWINEVECTOR3 col;
			VERTEX()
			{
				pos=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
				col=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
			}
			VERTEX(D3DXFROMWINEVECTOR3& apos, D3DXFROMWINEVECTOR3& acol)
			{
				pos=apos;
				col=acol;
			}
			VERTEX(const VERTEX& rhs)
			{
				pos=rhs.pos;
				col=rhs.col;
			}
		};



struct line_vertex{

    D3DXFROMWINEVECTOR3 pos;  // The transformed(screen space) position for the vertex.

    D3DXFROMWINEVECTOR4 colour;	 // The vertex colour.

};
struct line_vertex4{
	line_vertex4()
	{
		pos=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		colour=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	}
    D3DXFROMWINEVECTOR4 pos;  // The transformed(screen space) position for the vertex.
    D3DXFROMWINEVECTOR4 colour;	 // The vertex colour.
};
struct line_vertex3{
	line_vertex3()
	{
		pos=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		colour=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	}
    D3DXFROMWINEVECTOR3 pos;  // The transformed(screen space) position for the vertex.
    D3DXFROMWINEVECTOR4 colour;	 // The vertex colour.
};
typedef int TextureID; 
#if 1
struct stx_Edge {
	stx_Edge()
	{
		v0=0;
		v1=0;
		count=0;
	};
	stx_Edge(unsigned int iv0, unsigned int iv1, D3DXFROMWINEVECTOR3 norm)
	{		
		v0 = iv0;
		v1 = iv1;
		norm1=norm;//???
	}
	stx_Edge(const stx_Edge& rhs)
	{		
		v0=rhs.v0;
		v1=rhs.v1;
		count=rhs.count;
		norm0=rhs.norm0;
		norm1=rhs.norm1;
	}
#if 0
	float d0;
	float d1;
#else
	unsigned int v0;
	unsigned int v1;
	unsigned int count;
	D3DXFROMWINEVECTOR3 norm0;
	D3DXFROMWINEVECTOR3 norm1;
#endif
};

bool operator == (const stx_Edge &e0, const stx_Edge &e1);
bool operator != (const stx_Edge &e0, const stx_Edge &e1);

bool operator < (const stx_Edge &e0, const stx_Edge &e1);

bool constantCompEdgestd(const stx_Edge& s0, const stx_Edge &s1);

template <class T>
struct stx_OrderedSet
{
	std::vector<T> v;
	int current;
	stx_OrderedSet():current(-1){}
	virtual ~stx_OrderedSet(){}
	void insert(const T &edge){v.push_back(edge);current=v.size()-1;}
	bool goToObject(const T &edge)
	{
		#if 0 // ???
		std::vector<T>::iterator it = std::find(v.begin(), v.end(), edge);
		if ( it != v.end() )
		{
			current=distance(v.begin(), it);
			return true;
		}
		#else
		for(unsigned int i=current;i<v.size();i++) // current ???
		{
			if(v[i]==edge)
			{
				current=i;
				return true;
			}
		}
		#endif
		return false;
	}

	T& getCurrent()
	{
		if(current<v.size())
			return v[current];
		//return T;
	}
	bool next()
	{
		if(current<v.size())
		{
			current++;
			return true;
		}
		else
		{
			current=0;
			return true;
		}
		return false;
	}
	void goToFirst()
	{
		if(v.size())
			current=0;
	}
	void sort()
	{
#if 0
			if(shader.constants.getCount()>1) qsort((void*)shader.constants.getArray(), shader.nConstants, sizeof(ConstantD3D11), constantCompD3D11);
#else
			std::sort(v.begin(), v.end(), constantCompEdgestd);
#endif
	}
};
//include "stx_OrderedSet.h"

void stx_findEdge(stx_OrderedSet<stx_Edge>& edges, const stx_Edge &edge);
#endif
	struct stx_Mesh
	{
		std::string m_sName;
		std::vector<stx_VertexPositionNormalTexture> vertices;
		std::vector<stx_VertexPositionNormalNormal> linevertices;
		std::vector<__WORD__> indices;
		std::vector<unsigned int> textures_;
		std::vector<TextureID> textures;
		std::vector<std::string> textureNames_;
		std::vector<std::string> textureNames;
		std::vector<__WORD__> faces;
		std::vector<std::vector<__WORD__>> rindices;
		stx_OrderedSet<stx_Edge> edges;
void initNormals()
{
#if 1
	LOG_FNLN;
	LOG_PRINT("indices.size()=%d\n", indices.size());
	unsigned int i;
	if(indices.size()>=3)
	for (i = 0; i < indices.size(); i += 3){
	if(1){
		D3DXFROMWINEVECTOR3 v0, v1, v2;
		unsigned short ii=0;
		if(i<indices.size())
		{
			ii=indices[i];
			if(ii<linevertices.size())
				v0 = linevertices[ii].Position;
		}
		if((i+1)<indices.size())
		{
			ii=indices[i+1];
			if(ii<linevertices.size())
				v1 = linevertices[ii].Position;
		}
		if((i+2)<indices.size())
		{
			ii=indices[i+2];
			if(ii<linevertices.size())
				v2 = linevertices[ii].Position;
		}
		D3DXFROMWINEVECTOR3 normal;// = normalize(cross(v1 - v0, v2 - v0));
		D3DXFROMWINEVECTOR3 v1_ = v1 - v0;
		D3DXFROMWINEVECTOR3 v2_ = v2 - v0;
		D3DXFROMWINEVec3Cross(&normal, &v1_, &v2_);
		D3DXFROMWINEVec3Normalize(&normal, &normal);
		stx_findEdge(edges, stx_Edge(indices[i    ], indices[i + 1], normal));
		stx_findEdge(edges, stx_Edge(indices[i + 1], indices[i + 2], normal));
		stx_findEdge(edges, stx_Edge(indices[i + 2], indices[i    ], normal));
	}}
	unsigned int c = 0;
	edges.goToFirst();
	do {
		if (edges.getCurrent().count == 1){
			c++;
		}
	} while (edges.next());
	linevertices.resize(2 * c);
	edges.goToFirst();
	stx_VertexPositionNormalNormal* dest=&linevertices[0];
	do {
		if (edges.getCurrent().count == 1){
			dest[0].Position = vertices[edges.getCurrent().v0].Position;
			dest[1].Position = vertices[edges.getCurrent().v1].Position;
			dest[0].Normal0 = edges.getCurrent().norm0;
			dest[1].Normal0 = edges.getCurrent().norm0;
			dest[0].Normal1 = edges.getCurrent().norm1;
			dest[1].Normal1 = edges.getCurrent().norm1;
			dest += 2;
		}
	} while (edges.next());
	//PRIM_LINES
#endif
}

		stx_Mesh(){}
		virtual ~stx_Mesh()
		{
			vertices.clear();
			indices.clear();
			textures_.clear();
			textures.clear();
			textureNames.clear();
			textureNames_.clear();
			faces.clear();
			rindices.clear();
		}
		stx_Mesh(const stx_Mesh& rhs)
        	{
			m_sName=rhs.m_sName;
			vertices=rhs.vertices;
			indices=rhs.indices;
			textures_=rhs.textures_;
			textures=rhs.textures;
			textureNames=rhs.textureNames;
			textureNames_=rhs.textureNames_;
			faces=rhs.faces;
			rindices=rhs.rindices;
		}
	};

void stx_NormalizeMesh(stx_Mesh& m);

#if 1
	/* 	a cube inside the mesh ?
		test all cube corners ray-triange collision test
		cast a ray from a cube corner to the opposite cube corners
		inside or surface cube (neigbors list size 6?)
Triangulate the points to make a closed tesselated volume (ask again if you don't know how). Then fire a "ray" from your test point, and count its intersections with the set of triangles. If the number of intersections is odd, the test point is inside the volume.
	*/
#if 1
#define stx_innerProduct(v,q) D3DXFROMWINEVec3Dot(&v, &q)
#define stx_crossProduct(a,b,c) D3DXFROMWINEVec3Cross(&a, &b, &c)
#define stx_vector(a,b,c) \
	(a).x = (b).x - (c).x;	\
	(a).y = (b).y - (c).y;	\
	(a).z = (b).z - (c).z;
#endif
// http://www.lighthouse3d.com/tutorials/maths/ray-triangle-intersection/
bool stx_RayIntersectsTriangle(	const D3DXFROMWINEVECTOR3& p, 
					const D3DXFROMWINEVECTOR3& d,
					const D3DXFROMWINEVECTOR3& v0, 
					const D3DXFROMWINEVECTOR3& v1, 
					const D3DXFROMWINEVECTOR3& v2, float& t
					);
#endif

// To find orientation of ordered triplet (p1, p2, p3).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int stx_orientation3(const D3DXFROMWINEVECTOR3& p1, const D3DXFROMWINEVECTOR3& p2, const D3DXFROMWINEVECTOR3& p3);

int stx_orientation4(const D3DXFROMWINEVECTOR4& p1, const D3DXFROMWINEVECTOR4& p2, const D3DXFROMWINEVECTOR4& p3);

bool stx_EPSILON2(const D3DXFROMWINEVECTOR2& v1, const D3DXFROMWINEVECTOR2& v2);

bool stx_EPSILON3(const D3DXFROMWINEVECTOR3& v1, const D3DXFROMWINEVECTOR3& v2);

void stx_stdvector3(std::vector<D3DXFROMWINEVECTOR3>& vertices, std::vector<D3DXFROMWINEVECTOR3>& normals, std::vector<D3DXFROMWINEVECTOR2>& texcoords, std::vector<__WORD__>& indices);

D3DXFROMWINEVECTOR2 stx_CalculatePositionalSphericalMapping(D3DXFROMWINEVECTOR3& a);

struct stx_Material
{
	std::string m_TextureName;
	D3DXFROMWINEVECTOR3 m_Color;
	stx_Material(std::string aTextureName="", D3DXFROMWINEVECTOR3 aColor=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f))
	{
		m_TextureName=aTextureName;
		m_Color=aColor;
	}
	stx_Material(const stx_Material& rhs)
	{
		m_TextureName=rhs.m_TextureName;
		m_Color=rhs.m_Color;
	}
};

struct stx_Texture
{
	std::vector<stx_Material> m_Material;
	std::vector<__WORD__> m_Indices;
	stx_Texture(std::vector<stx_Material> aMaterial, std::vector<__WORD__> aIndices)
	{
		m_Material=aMaterial;
		m_Indices=aIndices;
	}
	stx_Texture(const stx_Texture& rhs)
	{
		m_Material=rhs.m_Material;
		m_Indices=rhs.m_Indices;
	}
	virtual ~stx_Texture()
	{
		m_Material.clear();
		m_Indices.clear();
	}
};

void stx_readXFile(const char* aFileName, std::vector<stx_Mesh>& aMeshes);
void stx_writeOBJFile(const char* aFileName, std::vector<stx_Mesh>& aMeshes);

void stx_readOBJFile(const char* aFileName, std::vector<stx_Mesh>& aMeshes);

void stx_writeXFile(const char* aFileName, std::vector<stx_Mesh>& aMeshes);

void stx_writeXFile(	const char* aFileName, 
			std::vector<stx_VertexPositionNormalTexture>& vertices, 
			std::vector<__WORD__>& indices, 
			std::vector<std::string>& textureNames, 
			std::vector<__WORD__>& faces);

D3DXFROMWINEVECTOR3 stx_Vec3Transform(const D3DXFROMWINEMATRIX& aM, const D3DXFROMWINEVECTOR3& av);

D3DXFROMWINEMATRIX stx_GetPivotMatrix(const D3DXFROMWINEVECTOR3& aR, const D3DXFROMWINEVECTOR3& aT);

D3DXFROMWINEMATRIX stx_GetPivotMatrix3(const D3DXFROMWINEVECTOR3& aR, const D3DXFROMWINEVECTOR3& aS, const D3DXFROMWINEVECTOR3& aT);

D3DXFROMWINEMATRIX stx_GetPivotMatrix2(const D3DXFROMWINEVECTOR3& aR, const D3DXFROMWINEVECTOR3& mPivotPos);

#define COLONWITHQUOTES ':'
#define COLONWITHDOUBLEQUOTES ":"

#define CLOSINGPARENTHESISWITHQUOTES ')'
#define CLOSINGPARENTHESISWITHDOUBLEQUOTES ")"

struct stx_Rule
{
	std::string m_RuleName;
	std::string m_RuleBody;
	stx_Rule(std::string aRuleName="", std::string aRuleBody="")
	{
		m_RuleName=aRuleName;
		m_RuleBody=aRuleBody;
	}
	stx_Rule(const stx_Rule& rhs)
	{
		m_RuleName=rhs.m_RuleName;
		m_RuleBody=rhs.m_RuleBody;
	}
	virtual ~stx_Rule(){}
};
std::string stx_ApplyRulesS(const std::vector<stx_Rule>& arules, const std::string& asymbol);
std::vector<std::string> stx_ApplyRulesE(const std::vector<stx_Rule>& arules, const std::string& asymbol);
void stx_DumpRules(std::vector<std::string>& av) ;
std::string stx_GetRule(const std::vector<stx_Rule>& arules, const std::string& arulename);
int stx_GetRulePosition(const std::vector<stx_Rule>& arules, const std::string& arulename);
std::string stx_ApplyRules(std::vector<stx_Rule>& arules, std::string& aaxiom);
std::string stx_GetRuleStr(const std::vector<stx_Rule>& arules, const std::string& axiom);

std::vector<std::string> stx_GetRuleVec(const std::vector<stx_Rule>& arules, const std::string& axiom);

#include "Noise.h"
#define STX_NOISESCALE 0.1482f

#if 1
float clamp(const float v, const float c0, const float c1);
D3DXFROMWINEVECTOR2  clamp(const D3DXFROMWINEVECTOR2 &v, const float c0, const float c1);
D3DXFROMWINEVECTOR2  clamp(const D3DXFROMWINEVECTOR2 &v, const D3DXFROMWINEVECTOR2 &c0, const D3DXFROMWINEVECTOR2 &c1);
D3DXFROMWINEVECTOR3  clamp(const D3DXFROMWINEVECTOR3 &v, const float c0, const float c1);
D3DXFROMWINEVECTOR3  clamp(const D3DXFROMWINEVECTOR3 &v, const D3DXFROMWINEVECTOR3 &c0, const D3DXFROMWINEVECTOR3 &c1);
D3DXFROMWINEVECTOR4  clamp(const D3DXFROMWINEVECTOR4 &v, const float c0, const float c1);
D3DXFROMWINEVECTOR4  clamp(const D3DXFROMWINEVECTOR4 &v, const D3DXFROMWINEVECTOR4 &c0, const D3DXFROMWINEVECTOR4 &c1);
//#define clamp(x, lo, hi) max(min(x, hi), lo)
#define saturate(x) clamp(x, 0, 1)
#define lerp_(t, a, b) (a + t * (b - a))
#endif
Image3 stx_Create2DNoiseTexture(const unsigned int xNSIZE=64, const unsigned int yNSIZE=64);
Image3 stx_Create3DNoiseTexture(const unsigned int xNSIZE=64, const unsigned int yNSIZE=64, const unsigned int zNSIZE=64);
/*
TextureID Create3DNoiseTexture2(const float xNSIZE=NSIZE, const float yNSIZE=NSIZE, const float zNSIZE=NSIZE);
TextureID CreateJitterTexture(const float xNSIZE=NSIZE, const float yNSIZE=NSIZE);*/

D3DXFROMWINEVECTOR3 stx_vec4tovec3(D3DXFROMWINEVECTOR4& a);

void stx_DumpRules2(const std::vector<stx_Rule>& arules) ;
std::string stx_asciireplace(std::string& arulebody);
std::vector<stx_Rule> stx_ParseRuleFile(const std::string& aFileName);

void stx_AddColoredQuad(std::vector<D3DXFROMWINEVECTOR3>& aVertices3, std::vector<__WORD__>& aIndices1, D3DXFROMWINEMATRIX& aM);

void stx_AddTexturedQuad(std::vector<D3DXFROMWINEVECTOR3>& aVertices3, std::vector<D3DXFROMWINEVECTOR2>& aTexcoords2, std::vector<__WORD__>& aIndices1, std::vector<__WORD__>& aIndices2, D3DXFROMWINEMATRIX& aM, unsigned int vtsize=1);

#if 1
void stx_AddColoredCube(std::vector<D3DXFROMWINEVECTOR3>& aVertices3, std::vector<D3DXFROMWINEVECTOR3>& aNormals3, std::vector<__WORD__>& aIndices1, D3DXFROMWINEVECTOR3 aPosition, D3DXFROMWINEVECTOR3 aSize=D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f), D3DXFROMWINEVECTOR3 aAngles=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f));
void stx_AddTexturedCube(std::vector<D3DXFROMWINEVECTOR3>& aVertices3, std::vector<D3DXFROMWINEVECTOR3>& aNormals3, std::vector<D3DXFROMWINEVECTOR2>& aTexcoords2, std::vector<__WORD__>& aIndices1, std::vector<__WORD__>& aIndices2, unsigned int vtsize, D3DXFROMWINEVECTOR3 aPosition, D3DXFROMWINEVECTOR3 aSize=D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f), D3DXFROMWINEVECTOR3 aAngles=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f));
#endif
struct VertexP3TC2
{
	D3DXFROMWINEVECTOR3 m_Position;
	D3DXFROMWINEVECTOR2 m_Texcoord;
	VertexP3TC2(D3DXFROMWINEVECTOR3 aposition=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR2 atexcoord=D3DXFROMWINEVECTOR2(0.0f, 0.0f))
	{
		m_Position=aposition;
		m_Texcoord=atexcoord;
	}
	VertexP3TC2(const VertexP3TC2& rhs)
	{
		m_Position=rhs.m_Position;
		m_Texcoord=rhs.m_Texcoord;
	}
};
void stx_AddQuadP3TC2(std::vector<VertexP3TC2>& aVertices, std::vector<__WORD__>& aIndices, D3DXFROMWINEMATRIX& aM);

void stx_AddCubeP3TC2(std::vector<VertexP3TC2>& aVertices, std::vector<__WORD__>& aIndices, D3DXFROMWINEVECTOR3& aPosition, D3DXFROMWINEVECTOR3& aSize, D3DXFROMWINEVECTOR3& aAngles);
void stx_Dump4x4f(D3DXFROMWINEMATRIX& am, const char* description=0);
void stx_Dump1i(int ai, const char* description=0);
void stx_Dump1f(float af, const char* description=0);
void stx_Dump2f(D3DXFROMWINEVECTOR2& av, const char* description=0);
void stx_Dump3f(D3DXFROMWINEVECTOR3& av, const char* description=0);
void stx_Dump4f(D3DXFROMWINEVECTOR4& av, const char* description=0);

D3DXFROMWINEVECTOR4 stx_GetTextureAtlasPosition(__DWORD__ Type, unsigned int aN, unsigned int iatlassize=4096);

int stx_AddTextureToAtlas(	Image3& atlas,Image3& img, unsigned int iatlassize=4096, 
					unsigned int isubtexturesize=256, 
					unsigned int atlasTextureCount=1);

Image3 stx_CreateAtlas(const char* filenames[], unsigned int iatlassize=4096, bool useMipMaps=false);
Image3 stx_CreateAtlas(std::vector<std::string>& av, unsigned int iatlassize=4096, bool useMipMaps=false);
int stx_CreateAtlas(Image3& atlas, const char* filenames[], unsigned int iatlassize=4096, bool useMipMaps=false);
int stx_CreateAtlas(Image3& atlas, std::vector<std::string>& av, unsigned int iatlassize=4096, bool useMipMaps=false);

#if 0
struct Tuple2i
{
	int x;
	int y;
	Tuple2i(int ax, int ay)
	{
		x=ax;
		y=ay;
	}
	void set(int ax, int ay)
	{
		x=ax;
		y=ay;
	}
};
struct Tuple3i
{
	int x;
	int y;
	int z;
	Tuple3i(int ax, int ay, int az)
	{
		x=ax;
		y=ay;
		z=az;
	}
	void set(int ax, int ay, int az)
	{
		x=ax;
		y=ay;
		z=az;
	}
};
struct Tuple4i
{
	int x;
	int y;
	int z;
	int w;
	Tuple4i(int ax, int ay, int az, int aw)
	{
		x=ax;
		y=ay;
		z=az;
		w=aw;
	}
	void set(int ax, int ay, int az, int aw)
	{
		x=ax;
		y=ay;
		z=az;
		w=aw;
	}
};
#endif
bool stx_is_number(const std::string& s);

D3DXFROMWINEMATRIX stx_ScaleAsset(
	D3DXFROMWINEVECTOR3 aiVecs0=D3DXFROMWINEVECTOR3( 1e10f,  1e10f,  1e10f),
	D3DXFROMWINEVECTOR3 aiVecs1=D3DXFROMWINEVECTOR3(-1e10f, -1e10f, -1e10f));
bool stx_IsNumber(const std::string& s);
std::vector<size_t> stx_SearchListInt(const std::vector<unsigned int>& theArray, int findFor);
std::vector<unsigned int> stx_SearchList(const std::vector<std::string>& theArray, std::string findFor);
#if 0
void printFrequency(std::string& aFilename) ;
#endif
void XFileParser();
#undef _SDKMESH_
#ifdef _SDKMESH_
#include <sdkmesh/obj.h>
#include <sdkmesh/sdkmesh.h>
#endif
int stx_GetSDKMESHFile(	const char* aFileName, std::vector<stx_Mesh>& aMeshes);
void stx_Materials_(std::vector<stx_Material>& aVT, std::vector<__WORD__>& aFaces, std::vector<stx_Mesh>& aMesh);
struct stx_Face
{
	int vertIndex[3];			// indicies for the verts that make up this triangle
	int coordIndex[3];			// indicies for the tex coords to texture this face
};
struct stx_MaterialInfo
{
	char  strName[255];			// The texture name
	char  strFile[255];			// The texture file name (If this is set it's a texture map)
	BYTE  color[3];				// The color of the object (R, G, B)
	int   texureId;				// the texture ID
	float uTile;				// u tiling of texture  (Currently not used)
	float vTile;				// v tiling of texture	(Currently not used)
	float uOffset;			    // u offset of texture	(Currently not used)
	float vOffset;				// v offset of texture	(Currently not used)
};
struct stx_3DObject 
{
	stx_3DObject()
	{
		numOfVerts=0;			// The number of verts in the model
		numOfFaces=0;			// The number of faces in the model
		numTexVertex=0;			// The number of texture coordinates
		materialID=0;
	}
	int  numOfVerts;			// The number of verts in the model
	int  numOfFaces;			// The number of faces in the model
	int  numTexVertex;			// The number of texture coordinates
	int  materialID;			// The texture ID to use, which is the index into our texture array
	bool bHasTexture;			// This is TRUE if there is a texture map for this object
	char strName[255];			// The name of the object
	std::vector<D3DXFROMWINEVECTOR3> pVerts;			// The object's vertices
	std::vector<D3DXFROMWINEVECTOR3> pNormals;		// The object's normals
	std::vector<D3DXFROMWINEVECTOR2> pTexVerts;		// The texture's UV coordinates
	std::vector<stx_Face> pFaces;				// The faces information of the object
};
struct stx_3DModel 
{
	stx_3DModel()
	{
		numOfObjects=0;					// The number of objects in the model
		numOfMaterials=0;
	}
	int numOfObjects;					// The number of objects in the model
	int numOfMaterials;					// The number of materials for the model
	std::vector<stx_MaterialInfo> pMaterials;	// The list of material information (Textures and colors)
	std::vector<stx_3DObject> pObject;			// The object list for our model
};
D3DXFROMWINEVECTOR2 stx_CalculatePositionalSphericalMapping(stx_VertexPositionNormalTexture& a);

class CVector3 
{
public:
	float x, y, z;
};

// This is our 2D point class.  This will be used to store the UV coordinates.
class CVector2 
{
public:
	float x, y;
};

// This is our face structure.  This is is used for indexing into the vertex 
// and texture coordinate arrays.  From this information we know which vertices
// from our vertex array go to which face, along with the correct texture coordinates.
struct tFace
{
	int vertIndex[3];			// indicies for the verts that make up this triangle
	int coordIndex[3];			// indicies for the tex coords to texture this face
};

// This holds the information for a material.  It may be a texture map of a color.
// Some of these are not used, but I left them because you will want to eventually
// read in the UV tile ratio and the UV tile offset for some models.
struct tMaterialInfo
{
	char  strName[255];			// The texture name
	char  strFile[255];			// The texture file name (If this is set it's a texture map)
	BYTE  color[3];				// The color of the object (R, G, B)
	int   texureId;				// the texture ID
	float uTile;				// u tiling of texture  (Currently not used)
	float vTile;				// v tiling of texture	(Currently not used)
	float uOffset;			    // u offset of texture	(Currently not used)
	float vOffset;				// v offset of texture	(Currently not used)
} ;

// This holds all the information for our model/scene. 
// You should eventually turn into a robust class that 
// has loading/drawing/querying functions like:
// LoadModel(...); DrawObject(...); DrawModel(...); DestroyModel(...);
struct t3DObject 
{
	int  numOfVerts;			// The number of verts in the model
	int  numOfFaces;			// The number of faces in the model
	int  numTexVertex;			// The number of texture coordinates
	int  materialID;			// The texture ID to use, which is the index into our texture array
	bool bHasTexture;			// This is TRUE if there is a texture map for this object
	char strName[255];			// The name of the object
	D3DXFROMWINEVECTOR3  *pVerts;			// The object's vertices
	D3DXFROMWINEVECTOR3  *pNormals;		// The object's normals
	D3DXFROMWINEVECTOR2  *pTexVerts;		// The texture's UV coordinates
	tFace *pFaces;				// The faces information of the object
};

// This holds our model information.  This should also turn into a robust class.
// We use STL's (Standard Template Library) vector class to ease our link list burdens. :)
struct t3DModel 
{
	int numOfObjects;					// The number of objects in the model
	int numOfMaterials;					// The number of materials for the model
	std::vector<tMaterialInfo> pMaterials;	// The list of material information (Textures and colors)
	std::vector<t3DObject> pObject;			// The object list for our model
};

//////////////////////////////	Math Functions  ////////////////////////////////*

// This computes the magnitude of a normal.   (magnitude = sqrt(x^2 + y^2 + z^2)
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))

// This calculates a vector between 2 points and returns the result
CVector3 Vector(CVector3 vPoint1, CVector3 vPoint2);

CVector3 AddVector(CVector3 vVector1, CVector3 vVector2);

// This divides a vector by a single number (scalar) and returns the result
CVector3 DivideVectorByScaler(CVector3 vVector1, float Scaler);


// This returns the cross product between 2 vectors
CVector3 Cross(CVector3 vVector1, CVector3 vVector2);


// This returns the normal of a vector
CVector3 Normalize(CVector3 vNormal);


#if 0
/*
 * Get the list of all files in given directory and its sub directories.
 *
 * Arguments
 * 	dirPath : Path of directory to be traversed
 * 	dirSkipList : List of folder names to be skipped
 *
 * Returns:
 * 	vector containing paths of all the files in given directory and its sub directories
 *
 */
std::vector<std::string> getAllFilesInDir(const std::string &dirPath, 	const std::vector<std::string> dirSkipList = { })
{
 
	// Create a vector of string
	std::vector<std::string> listOfFiles;
	try {
		// Check if given path exists and points to a directory
		if (filesys::exists(dirPath) && filesys::is_directory(dirPath))
		{
			// Create a Recursive Directory Iterator object and points to the starting of directory
			filesys::recursive_directory_iterator iter(dirPath);
 
			// Create a Recursive Directory Iterator object pointing to end.
			filesys::recursive_directory_iterator end;
 
			// Iterate till end
			while (iter != end)
			{
				// Check if current entry is a directory and if exists in skip list
				if (filesys::is_directory(iter->path()) &&
						(std::find(dirSkipList.begin(), dirSkipList.end(), iter->path().filename()) != dirSkipList.end()))
				{
					// Skip the iteration of current directory pointed by iterator
#ifdef USING_BOOST
					// Boost Fileystsem  API to skip current directory iteration
					iter.no_push();
#elif 0
					// c++17 Filesystem API to skip current directory iteration
					iter.disable_recursion_pending();
#endif
				}
				else
				{
					// Add the name in vector
					listOfFiles.push_back(iter->path().string());
				}
 
				error_code ec;
				// Increment the iterator to point to next entry in recursive iteration
				iter.increment(ec);
				if (ec) {
					std::cerr << "Error While Accessing : " << iter->path().string() << " :: " << ec.message() << '\n';
				}
			}
		}
	}
	catch (std::system_error & e)
	{
		std::cerr << "Exception :: " << e.what();
	}
	return listOfFiles;
}
#endif
float stx_cbrt(const float a);


std::string stx_LowerUpper(	const std::string& fileName, 
					//bool aPath=false, 
					bool aName=false, 
					bool aExt=false);

inline FILE *stx_fopen(const char *pathname, const char *mode)
{
	std::string f=std::string(pathname);
	std::string file=stx_convertpath(f);
	return fopen(file.c_str(), mode);
}

#if 1
struct stx_CModelViewerCamera
{
	stx_CModelViewerCamera(){}
	virtual ~stx_CModelViewerCamera(){}
	stx_CModelViewerCamera(const stx_CModelViewerCamera& rhs)
	{
		m_mWorld=rhs.m_mWorld;
		m_mProj=rhs.m_mProj;
		m_mView=rhs.m_mView;
		m_vEye=rhs.m_vEye;
		m_vAt=rhs.m_vAt;
	}
        inline void SetViewParams( D3DXFROMWINEVECTOR4* aEye, D3DXFROMWINEVECTOR4* aAt )
        {
        	D3DXFROMWINEVECTOR3 eye(aEye->x, aEye->y, aEye->z);
        	D3DXFROMWINEVECTOR3 at(aAt->x, aAt->y, aAt->z);
        	SetViewParams( &eye, &at );
        }
        inline void SetViewParams( D3DXFROMWINEVECTOR3* aEye, D3DXFROMWINEVECTOR3* aAt )
        {
        	D3DXFROMWINEVECTOR3 up(0.0f, 1.0f, 0.0f);
        	D3DXFROMWINEMatrixLookAtLH(&m_mView, aEye, aAt, &up);
        }
	inline void SetRadius(float x, float y, float z){}
	inline void SetProjParams( float aFOV, float aAspectRatio, float aNear, float aFar ){D3DXFROMWINEMatrixPerspectiveFovLH(&m_mProj, aFOV, aAspectRatio, aNear, aFar);}
	inline void FrameMove( float aTime ){}
	inline D3DXFROMWINEMATRIX GetWorldMatrix(){return m_mWorld;}
	inline D3DXFROMWINEMATRIX GetProjMatrix(){return m_mProj;}
	inline D3DXFROMWINEMATRIX GetViewMatrix(){return m_mView;}
	inline D3DXFROMWINEVECTOR4 GetEyePt(){return m_vEye;}
	inline D3DXFROMWINEVECTOR3 GetEyePt3(){return D3DXFROMWINEVECTOR3(m_vEye.x, m_vEye.y, m_vEye.z);}	
protected:
	D3DXFROMWINEMATRIX m_mWorld, m_mProj, m_mView;
	D3DXFROMWINEVECTOR4 m_vEye;
	D3DXFROMWINEVECTOR4 m_vAt;
};
struct stx_CFirstPersonCamera
{
	stx_CFirstPersonCamera(){}
	virtual ~stx_CFirstPersonCamera(){}
	stx_CFirstPersonCamera(const stx_CFirstPersonCamera& rhs)
	{
		m_mWorld=rhs.m_mWorld;
		m_mProj=rhs.m_mProj;
		m_mView=rhs.m_mView;
		m_vEye=rhs.m_vEye;
		m_vAt=rhs.m_vAt;
	}
        inline void SetViewParams( D3DXFROMWINEVECTOR4* aEye, D3DXFROMWINEVECTOR4* aAt )
        {
        	D3DXFROMWINEVECTOR3 eye(aEye->x, aEye->y, aEye->z);
        	D3DXFROMWINEVECTOR3 at(aAt->x, aAt->y, aAt->z);
        	SetViewParams( &eye, &at );
        }
        inline void SetViewParams( D3DXFROMWINEVECTOR3* aEye, D3DXFROMWINEVECTOR3* aAt )
        {
        	D3DXFROMWINEVECTOR3 up(0.0f, 1.0f, 0.0f);
        	D3DXFROMWINEMatrixLookAtLH(&m_mView, aEye, aAt, &up);
        }
    	inline void SetRadius(float x, float y, float z){}
	inline void SetProjParams( float aFOV, float aAspectRatio, float aNear, float aFar )
	{
		D3DXFROMWINEMatrixPerspectiveFovLH(&m_mProj, aFOV, aAspectRatio, aNear, aFar);
	}
	inline void FrameMove( float aTime ){}
	inline D3DXFROMWINEMATRIX GetWorldMatrix(){return m_mWorld;}
	inline D3DXFROMWINEMATRIX GetProjMatrix(){return m_mProj;}
	inline D3DXFROMWINEMATRIX GetViewMatrix(){return m_mView;}
	inline void SetProjMatrix(D3DXFROMWINEMATRIX& m){m_mProj=m;}
	inline void SetViewMatrix(D3DXFROMWINEMATRIX& m){m_mView=m;}
	inline D3DXFROMWINEVECTOR4 GetEyePt(){return m_vEye;}
	inline D3DXFROMWINEVECTOR3 GetEyePt3(){return D3DXFROMWINEVECTOR3(m_vEye.x, m_vEye.y, m_vEye.z);}
    inline D3DXFROMWINEVECTOR4 GetLookAtPt(){return m_vAt;}
    inline D3DXFROMWINEVECTOR3 GetLookAtPt3(){return D3DXFROMWINEVECTOR3(m_vAt.x, m_vAt.y, m_vAt.z);}
protected:
	D3DXFROMWINEMATRIX m_mWorld, m_mProj, m_mView;
	D3DXFROMWINEVECTOR4 m_vEye;
	D3DXFROMWINEVECTOR4 m_vAt;
};
//typedef stx_CFirstPersonCamera stx_CBaseCamera;
typedef stx_CModelViewerCamera stx_CBaseCamera;
#endif
#if 0
inline D3DXFROMWINEVECTOR3 stx_rgb2hsv(D3DXFROMWINEVECTOR3 c)
{
    D3DXFROMWINEVECTOR4 K = D3DXFROMWINEVECTOR4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    D3DXFROMWINEVECTOR4 p = mix(D3DXFROMWINEVECTOR4(c.b, c.g, K.w, K.z), D3DXFROMWINEVECTOR4(c.g, c.b, K.x, K.y), step(c.b, c.g));
    D3DXFROMWINEVECTOR4 q = mix(D3DXFROMWINEVECTOR4(p.x, p.y, p.w, c.r), D3DXFROMWINEVECTOR4(c.r, p.y, p.z, p.x), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return D3DXFROMWINEVECTOR3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

inline D3DXFROMWINEVECTOR3 stx_hsv2rgb(D3DXFROMWINEVECTOR3 c)
{
    D3DXFROMWINEVECTOR4 K = D3DXFROMWINEVECTOR4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    D3DXFROMWINEVECTOR3 p = abs(fract(D3DXFROMWINEVECTOR3(c.x, c.x, c.x) + D3DXFROMWINEVECTOR3(K.x, K.y, K.z)) * 6.0 - D3DXFROMWINEVECTOR3(K.w, K.w, K.w));
    return c.z * mix(D3DXFROMWINEVECTOR3(K.x, K.x, K.x), clamp(p - D3DXFROMWINEVECTOR3(K.x, K.x, K.x), 0.0, 1.0), c.y);
}
#endif
#if 0
inline void stx_ParseEffect(const std::string& aFileName, const char* aDefines=0)
{
	std::string m_sFileName=stx_convertpath(aFileName);
	//int m_iTechnique=-1;

	LOG_FNLN;
	LOG_PRINT("\nfileName=%s\n", m_sFileName.c_str());
	std::string tmp;
	std::ifstream in(m_sFileName.c_str(), std::ios::in | std::ios::binary);
  	if (in)
    		tmp=(std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));

    std::string stringToBeSplitted;
    if(aDefines)
        stringToBeSplitted.append(aDefines);
    stringToBeSplitted.append(tmp);
	std::string delimeter="[techniques]";
	int startIndex = 0;
	int  endIndex = 0;
    std::string val1;
    std::string val;
	if( (endIndex = stringToBeSplitted.find(delimeter, startIndex)) < stringToBeSplitted.size() )
	{
		val1 = stringToBeSplitted.substr(0, endIndex);
		val = stringToBeSplitted.substr(endIndex+delimeter.length(), stringToBeSplitted.length());
	}
	LOG_PRINT("\nval1:\n%s\n", val1.c_str());
	LOG_PRINT("\nval:\n%s\n", val.c_str());

    std::ofstream out("./__tmp__.xml");
    //out << "<Body>\n";
    out << val;
    //out << "\n</Body>\n";
    out.close();

	std::ifstream in2("./__tmp__.xml", std::ios::in | std::ios::binary);
  	if (in2)
    		val=(std::string((std::istreambuf_iterator<char>(in2)), std::istreambuf_iterator<char>()));

    LOG_PRINT("techniques:\n%s\n\n", val.c_str());
}
#endif
#if 0
inline double DT()
{
	static __DWORD__ t1=timeGetTime();
	__DWORD__ t2=timeGetTime();
	double t = (t2-t1)/1000.0f;
	t1=t2;
	return t;
}
inline void LOGDT(const char* f=__FILE__, const char* ff=__FUNCTION__, __DWORD__ l=__LINE__)
{
	char buf[1024];
	stx_snprintf(buf, 1024, "%s.log", STX_Service::GetWindowInstance()->GetCaption());
	std::ofstream outfile;
	outfile.open(buf, std::ios_base::app);
	stx_snprintf(buf, 1024, "%s:%s:%s:%d:%f\n", STX_Service::GetWindowInstance()->GetCaption(), f, ff, l, DT());
	outfile << buf; 
	outfile.close();
}
#define LOG_DT LOGDT()
#undef LOG_FNLN
#define LOG_FNLN LOGDT()
#endif
#endif

