/*
 *  IInput.h
 *  Kore-Engine
 *
 *  Description: Holds all the input core
 *
 *  Created by Sean Chapel on 2/1/06.
 *  Copyright 2006 Seoushi Games. All rights reserved.
 *
 */
/*
  Copyright (c) 2015 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <STX/STX.h>
#include <STX/STXCPP.h>

#if 0//defined(SDL_2_0_5_BUILD) 
int SDLK_KP0=0;
int SDLK_KP1=0;
int SDLK_KP2=0;
int SDLK_KP3=0;
int SDLK_KP4=0;
int SDLK_KP5=0;
int SDLK_KP6=0;
int SDLK_KP7=0;
int SDLK_KP8=0;
int SDLK_KP9=0;
int SDLK_NUMLOCK=0;
int SDLK_SCROLLOCK=0;
int SDLK_RSUPER=0;
int SDLK_PRINT=0;
int SDLK_BREAK=0;
int SDL_BUTTON_WHEELUP=0;
int SDL_BUTTON_WHEELDOWN=0;
#endif

float IInput::m_fSpinX=0.0f;
float IInput::m_fSpinY=0.0f;
float IInput::m_fX=0.0f;
float IInput::m_fY=0.0f;
float IInput::m_fZ=0.0f;
float  IInput::m_fSpinX_L=0.0f;
float  IInput::m_fSpinY_L=0.0f;
float  IInput::m_fSpinX_R=0.0f;
float  IInput::m_fSpinY_R=0.0f;
Sint32 IInput::m_iWheelY=0;

float & IInput::GetSpinY_L(void)
{
        return m_fSpinY_L;
}
float & IInput::GetSpinX_L(void)
{
        return m_fSpinX_L;
}
float & IInput::GetSpinX(void){return m_fSpinX;}
float & IInput::GetSpinY(void){return m_fSpinY;}
float & IInput::GetX(void){return m_fX;}
float & IInput::GetY(void){return m_fY;}
float & IInput::GetZ(void){return m_fZ;}
float & IInput::GetSpinX_R(void){return m_fSpinX_R;}
float & IInput::GetSpinY_R(void){return m_fSpinY_R;}

#if defined(SDL_1_2_15_BUILD) || defined(SDL_2_0_5_BUILD)
#define SDLKtoKEY_MACRO(X) case SDLK_##X: return STX_KEY_##X;

eKey SDLKtoKEY(int key)
{
	switch(key)
	{
		SDLKtoKEY_MACRO(UNKNOWN		)//= SDLK_UNKNOWN,
		//SDLKtoKEY_MACRO(FIRST		)//= SDLK_FIRST,
		SDLKtoKEY_MACRO(BACKSPACE	)//= SDLK_BACKSPACE,
		SDLKtoKEY_MACRO(TAB			)//= SDLK_TAB,
		SDLKtoKEY_MACRO(CLEAR		)//= SDLK_CLEAR,
		SDLKtoKEY_MACRO(RETURN		)//= SDLK_RETURN,
		SDLKtoKEY_MACRO(PAUSE		)//= SDLK_PAUSE,
		SDLKtoKEY_MACRO(ESCAPE		)//= SDLK_ESCAPE,
		SDLKtoKEY_MACRO(SPACE		)//= SDLK_SPACE,
		SDLKtoKEY_MACRO(EXCLAIM		)//= SDLK_EXCLAIM,
		SDLKtoKEY_MACRO(QUOTEDBL	)//= SDLK_QUOTEDBL,
		SDLKtoKEY_MACRO(HASH		)//= SDLK_HASH,
		SDLKtoKEY_MACRO(DOLLAR		)//= SDLK_DOLLAR,
		SDLKtoKEY_MACRO(AMPERSAND		)//= SDLK_AMPERSAND,
		SDLKtoKEY_MACRO(QUOTE		)//= SDLK_QUOTE,
		SDLKtoKEY_MACRO(LEFTPAREN	)//= SDLK_LEFTPAREN,
		SDLKtoKEY_MACRO(RIGHTPAREN	)//= SDLK_RIGHTPAREN,
		SDLKtoKEY_MACRO(ASTERISK	)//= SDLK_ASTERISK,
		SDLKtoKEY_MACRO(PLUS		)//= SDLK_PLUS,
		SDLKtoKEY_MACRO(COMMA		)//= SDLK_COMMA,
		SDLKtoKEY_MACRO(MINUS		)//= SDLK_MINUS,
		SDLKtoKEY_MACRO(PERIOD		)//= SDLK_PERIOD,
		SDLKtoKEY_MACRO(SLASH		)//= SDLK_SLASH,
		SDLKtoKEY_MACRO(0			)//= SDLK_0,
		SDLKtoKEY_MACRO(1			)//= SDLK_1,
		SDLKtoKEY_MACRO(2			)//= SDLK_2,
		SDLKtoKEY_MACRO(3			)//= SDLK_3,
		SDLKtoKEY_MACRO(4			)//= SDLK_4,
		SDLKtoKEY_MACRO(5			)//= SDLK_5,
		SDLKtoKEY_MACRO(6			)//= SDLK_6,
		SDLKtoKEY_MACRO(7			)//= SDLK_7,
		SDLKtoKEY_MACRO(8			)//= SDLK_8,
		SDLKtoKEY_MACRO(9			)//= SDLK_9,
		SDLKtoKEY_MACRO(COLON		)//= SDLK_COLON,
		SDLKtoKEY_MACRO(SEMICOLON	)//= SDLK_SEMICOLON,
		SDLKtoKEY_MACRO(LESS		)//= SDLK_LESS,
		SDLKtoKEY_MACRO(EQUALS		)//= SDLK_EQUALS,
		SDLKtoKEY_MACRO(GREATER		)//= SDLK_GREATER,
		SDLKtoKEY_MACRO(QUESTION	)//= SDLK_QUESTION,
		SDLKtoKEY_MACRO(AT			)//= SDLK_AT,


		/*
		Skip uppercase letters
		*/
		SDLKtoKEY_MACRO(LEFTBRACKET		)//= SDLK_LEFTBRACKET,
		SDLKtoKEY_MACRO(BACKSLASH		)//= SDLK_BACKSLASH,
		SDLKtoKEY_MACRO(RIGHTBRACKET	)//= SDLK_RIGHTBRACKET,
		SDLKtoKEY_MACRO(CARET			)//= SDLK_CARET,
		SDLKtoKEY_MACRO(UNDERSCORE		)//= SDLK_UNDERSCORE,
		SDLKtoKEY_MACRO(BACKQUOTE		)//= SDLK_BACKQUOTE,
		SDLKtoKEY_MACRO(a			)//= SDLK_a,
		SDLKtoKEY_MACRO(b			)//= SDLK_b,
		SDLKtoKEY_MACRO(c			)//= SDLK_c,
		SDLKtoKEY_MACRO(d			)//= SDLK_d,
		SDLKtoKEY_MACRO(e			)//= SDLK_e,
		SDLKtoKEY_MACRO(f			)//= SDLK_f,
		SDLKtoKEY_MACRO(g			)//= SDLK_g,
		SDLKtoKEY_MACRO(h			)//= SDLK_h,
		SDLKtoKEY_MACRO(i			)//= SDLK_i,
		SDLKtoKEY_MACRO(j			)//= SDLK_j,
		SDLKtoKEY_MACRO(k			)//= SDLK_k,
		SDLKtoKEY_MACRO(l			)//= SDLK_l,
		SDLKtoKEY_MACRO(m			)//= SDLK_m,
		SDLKtoKEY_MACRO(n			)//= SDLK_n,
		SDLKtoKEY_MACRO(o			)//= SDLK_o,
		SDLKtoKEY_MACRO(p			)//= SDLK_p,
		SDLKtoKEY_MACRO(q			)//= SDLK_q,
		SDLKtoKEY_MACRO(r			)//= SDLK_r,
		SDLKtoKEY_MACRO(s			)//= SDLK_s,
		SDLKtoKEY_MACRO(t			)//= SDLK_t,
		SDLKtoKEY_MACRO(u			)//= SDLK_u,
		SDLKtoKEY_MACRO(v			)//= SDLK_v,
		SDLKtoKEY_MACRO(w			)//= SDLK_w,
		SDLKtoKEY_MACRO(x			)//= SDLK_x,
		SDLKtoKEY_MACRO(y			)//= SDLK_y,
		SDLKtoKEY_MACRO(z			)//= SDLK_z,
		SDLKtoKEY_MACRO(DELETE		)//= SDLK_DELETE,
		/* End of ASCII mapped keysyms */

		/* Numeric keypad */
		#if !defined(SDL_2_0_5_BUILD) 
		SDLKtoKEY_MACRO(KP0			)//= SDLK_KP0,
		SDLKtoKEY_MACRO(KP1			)//= SDLK_KP1,
		SDLKtoKEY_MACRO(KP2			)//= SDLK_KP2,
		SDLKtoKEY_MACRO(KP3			)//= SDLK_KP3,
		SDLKtoKEY_MACRO(KP4			)//= SDLK_KP4,
		SDLKtoKEY_MACRO(KP5			)//= SDLK_KP5,
		SDLKtoKEY_MACRO(KP6			)//= SDLK_KP6,
		SDLKtoKEY_MACRO(KP7			)//= SDLK_KP7,
		SDLKtoKEY_MACRO(KP8			)//= SDLK_KP8,
		SDLKtoKEY_MACRO(KP9			)//= SDLK_KP9,
		#endif		
		SDLKtoKEY_MACRO(KP_PERIOD		)//= SDLK_KP_PERIOD,
		SDLKtoKEY_MACRO(KP_DIVIDE		)//= SDLK_KP_DIVIDE,
		SDLKtoKEY_MACRO(KP_MULTIPLY		)//= SDLK_KP_MULTIPLY,
		SDLKtoKEY_MACRO(KP_MINUS		)//= SDLK_KP_MINUS,
		SDLKtoKEY_MACRO(KP_PLUS		)//= SDLK_KP_PLUS,
		SDLKtoKEY_MACRO(KP_ENTER		)//= SDLK_KP_ENTER,
		SDLKtoKEY_MACRO(KP_EQUALS		)//= SDLK_KP_EQUALS,

		/* Arrows + Home/End pad */
		SDLKtoKEY_MACRO(UP			)//= SDLK_UP,
		SDLKtoKEY_MACRO(DOWN		)//= SDLK_DOWN,
		SDLKtoKEY_MACRO(RIGHT		)//= SDLK_RIGHT,
		SDLKtoKEY_MACRO(LEFT		)//= SDLK_LEFT,
		SDLKtoKEY_MACRO(INSERT		)//= SDLK_INSERT,
		SDLKtoKEY_MACRO(HOME		)//= SDLK_HOME,
		SDLKtoKEY_MACRO(END			)//= SDLK_END,
		SDLKtoKEY_MACRO(PAGEUP		)//= SDLK_PAGEUP,
		SDLKtoKEY_MACRO(PAGEDOWN		)//= SDLK_PAGEDOWN,

		/* Function keys */
		SDLKtoKEY_MACRO(F1			)//= SDLK_F1,
		SDLKtoKEY_MACRO(F2			)//= SDLK_F2,
		SDLKtoKEY_MACRO(F3			)//= SDLK_F3,
		SDLKtoKEY_MACRO(F4			)//= SDLK_F4,
		SDLKtoKEY_MACRO(F5			)//= SDLK_F5,
		SDLKtoKEY_MACRO(F6			)//= SDLK_F6,
		SDLKtoKEY_MACRO(F7			)//= SDLK_F7,
		SDLKtoKEY_MACRO(F8			)//= SDLK_F8,
		SDLKtoKEY_MACRO(F9			)//= SDLK_F9,
		SDLKtoKEY_MACRO(F10			)//= SDLK_F10,
		SDLKtoKEY_MACRO(F11			)//= SDLK_F11,
		SDLKtoKEY_MACRO(F12			)//= SDLK_F12,
		SDLKtoKEY_MACRO(F13			)//= SDLK_F13,
		SDLKtoKEY_MACRO(F14			)//= SDLK_F14,
		SDLKtoKEY_MACRO(F15			)//= SDLK_F15,

		/* Key state modifier keys */
		#if !defined(SDL_2_0_5_BUILD) 
		SDLKtoKEY_MACRO(NUMLOCK		)//= SDLK_NUMLOCK,
		#endif
		SDLKtoKEY_MACRO(CAPSLOCK		)//= SDLK_CAPSLOCK,
		#if !defined(SDL_2_0_5_BUILD) 
		SDLKtoKEY_MACRO(SCROLLOCK		)//= SDLK_SCROLLOCK,
		#endif
		SDLKtoKEY_MACRO(RSHIFT		)//= SDLK_RSHIFT,
		SDLKtoKEY_MACRO(LSHIFT		)//= SDLK_LSHIFT,
		SDLKtoKEY_MACRO(RCTRL		)//= SDLK_RCTRL,
		SDLKtoKEY_MACRO(LCTRL		)//= SDLK_LCTRL,
		SDLKtoKEY_MACRO(RALT		)//= SDLK_RALT,
		SDLKtoKEY_MACRO(LALT		)//= SDLK_LALT,
		#if !defined(SDL_2_0_5_BUILD) 
		//SDLKtoKEY_MACRO(RMETA		)//= SDLK_RMETA,
		//SDLKtoKEY_MACRO(LMETA		)//= SDLK_LMETA,
		//SDLKtoKEY_MACRO(LSUPER		)//= SDLK_LSUPER,		/* Left "Windows" key */
		SDLKtoKEY_MACRO(RSUPER		)//= SDLK_RSUPER,		/* Right "Windows" key */
		#endif
		/* Miscellaneous function keys */
		SDLKtoKEY_MACRO(HELP		)//= SDLK_HELP,
		#if !defined(SDL_2_0_5_BUILD) 
		SDLKtoKEY_MACRO(PRINT		)//= SDLK_PRINT,
		#endif
		SDLKtoKEY_MACRO(SYSREQ		)//= SDLK_SYSREQ,
		#if !defined(SDL_2_0_5_BUILD) 
		SDLKtoKEY_MACRO(BREAK		)//= SDLK_BREAK,
		#endif
		SDLKtoKEY_MACRO(MENU		)//= SDLK_MENU,
		/* Add any other keys here */
	};
	return STX_KEY_UNKNOWN;
}
#if 0
    /* Print all information about a key event */
    void PrintKeyInfo( SDL_KeyboardEvent *key ){
        /* Is it a release or a press? */
        if( key->type == SDL_KEYUP )
            printf( "Release:- " );
        else
            printf( "Press:- " );

        /* Print the hardware scancode first */
        printf( "Scancode: 0x%02X", key->keysym.scancode );
        /* Print the name of the key */
        printf( ", Name: %s", SDL_GetKeyName( key->keysym.sym ) );
        /* We want to print the unicode info, but we need to make */
        /* sure its a press event first (remember, release events */
        /* don't have unicode info                                */
        if( key->type == SDL_KEYDOWN ){
            /* If the Unicode value is less than 0x80 then the    */
            /* unicode value can be used to get a printable       */
            /* representation of the key, using (char)unicode.    */
            //printf(", Unicode: " );
            if( key->keysym.unicode < 0x80 && key->keysym.unicode > 0 ){
                printf( "%c (0x%04X)", (char)key->keysym.unicode,
                        key->keysym.unicode );
            }
            else{
                printf( "? (0x%04X)", key->keysym.unicode );
            }
        }
        printf( "\n" );
        /* Print modifier info */
        PrintModifiers( key->keysym.mod );
    }

    /* Print modifier info */
    void PrintModifiers( SDLMod mod ){
        printf( "Modifers: " );

        /* If there are none then say so and return */
        if( mod == KMOD_NONE ){
            printf( "None\n" );
            return;
        }

        /* Check for the presence of each SDLMod value */
        /* This looks messy, but there really isn't    */
        /* a clearer way.                              */
        if( mod & KMOD_NUM ) printf( "NUMLOCK " );
        if( mod & KMOD_CAPS ) printf( "CAPSLOCK " );
        if( mod & KMOD_LCTRL ) printf( "LCTRL " );
        if( mod & KMOD_RCTRL ) printf( "RCTRL " );
        if( mod & KMOD_RSHIFT ) printf( "RSHIFT " );
        if( mod & KMOD_LSHIFT ) printf( "LSHIFT " );
        if( mod & KMOD_RALT ) printf( "RALT " );
        if( mod & KMOD_LALT ) printf( "LALT " );
        if( mod & KMOD_CTRL ) printf( "CTRL " );
        if( mod & KMOD_SHIFT ) printf( "SHIFT " );
        if( mod & KMOD_ALT ) printf( "ALT " );
        printf( "\n" );
    }
#endif
#if 1
bool SDLInput::OnKeyPress	(eKey pKey)
{
	return IsKeyPressed(pKey);
}

bool SDLInput::OnKeyRelease	(eKey pKey)
{
	return !OnKeyPress(pKey);
}
#endif
bool SDLInput::Quit(void)
{
	return m_quit;
}

	//
	// Tells if a key is inbetween being pressed and released
	//
	bool SDLInput::IsKeyHeld(eKey Key)
	{
		return false;
	}

	//
	// Tells if a key is pressed
	bool SDLInput::IsKeyDown(eKey aKey)
	{
		return OnKeyPress(aKey);
	}

	//
	// Tells if a key is released
	//
	bool SDLInput::IsKeyUp(eKey aKey)
	{
		return OnKeyRelease(aKey);
	}

bool SDLInput::OnShiftKeyPress(){return (event.key.keysym.mod & KMOD_SHIFT);}
bool SDLInput::OnLShiftKeyPress(){return (event.key.keysym.mod & KMOD_LSHIFT);}
bool SDLInput::OnRShiftKeyPress(){return (event.key.keysym.mod & KMOD_RSHIFT);}

bool SDLInput::OnCTRLKeyPress(){return (event.key.keysym.mod & KMOD_CTRL);}
bool SDLInput::OnLCTRLKeyPress(){return (event.key.keysym.mod & KMOD_LCTRL);}
bool SDLInput::OnRCTRLKeyPress(){return (event.key.keysym.mod & KMOD_RCTRL);}

bool SDLInput::OnALTKeyPress(){return (event.key.keysym.mod & KMOD_ALT);}
bool SDLInput::OnLALTKeyPress(){return (event.key.keysym.mod & KMOD_LALT);}
bool SDLInput::OnRALTKeyPress(){return (event.key.keysym.mod & KMOD_RALT);}

bool SDLInput::OnNUMKeyPress(){return (event.key.keysym.mod & KMOD_NUM);}
bool SDLInput::OnCAPSKeyPress(){return (event.key.keysym.mod & KMOD_CAPS);}
bool SDLInput::OnMODEKeyPress(){return (event.key.keysym.mod & KMOD_MODE);}
	//
	// Updates keyboard events
	/// @returns True is returned on an Exit key being pressed (ctrl+c, window being closed)
	/// Note: this also handles basic window events
	///
#if 0
	SDL_Event sdl_event;
	std::list<std::list<SDL_Event> > sdl_events(10);
#endif
/*
list::pop_front
list::push_back
list::pop_back
*/
		
	bool SDLInput::Update()
	{
		//SDL_Event event;
		//std::vector< eKey > keys;
		m_Key=STX_KEY_UNKNOWN;m_Scancode='\0';
		m_quit=false;
		m_iWheelY=0;
		//cGuiManager* Gui = Singleton<cGuiManager>::GetSingletonPtr();

    // Event handler
    SDL_Event event;

    // While application is running
    //while (!m_quit)
    {
        // Handle events on queue
        while (SDL_PollEvent(&event) != 0) {
            // User requests quit
 			switch( event.type )
			{
#if !defined(SDL_2_0_5_BUILD) 
				case SDL_VIDEORESIZE:
					STX_Service::GetWindowInstance()->ReSize(event.resize.w, event.resize.h,STX_Service::GetWindowInstance()->Getbpp());
				break;
#endif
				case SDL_QUIT:
					m_quit=true;
					stx_exit(0);
					return true;
					break;
				case SDL_KEYDOWN:
					m_Key=SDLKtoKEY(event.key.keysym.sym);
					//m_Scancode=event.key.keysym.scancode;
					//printf("m_Scancode=%x\n", m_Scancode);
					if(m_Key==STX_KEY_ESCAPE)
					{
						m_quit=true;
						stx_exit(0);
						return true;
					}
					#if 0
 			switch( event.key.keysym.sym)
			{
      case SDLK_F1:
        std::cout << "F1 key pressed\n";
        break;
      case SDLK_F2:
        std::cout << "F2 key pressed\n";
        break;
        };
 			switch( m_Key)
			{
      case STX_KEY_F1:
        std::cout << "STX_KEY_F1 key pressed\n";
        break;
      case STX_KEY_F2:
        std::cout << "STX_KEY_F2 key pressed\n";
        break;
        };
					break;
				case SDL_KEYUP:
					break;
#endif
                case SDL_MOUSEMOTION:
                	 {	int mouseX, mouseY;
                		m_MouseState = SDL_GetMouseState(&mouseX, &mouseY);
                    		m_MouseX = mouseX;
                    		m_MouseY = mouseY;}
                    break;
                case SDL_MOUSEBUTTONUP:
						//do input core stuff here
                    break;
                case SDL_MOUSEBUTTONDOWN:
                	 {	int mouseX, mouseY;
                		m_MouseState = SDL_GetMouseState(&mouseX, &mouseY);
                    		m_MouseX = mouseX;
                    		m_MouseY = mouseY;}
                    break;
#ifdef SDL_2_0_5_BUILD
		case SDL_MOUSEWHEEL:
            		//m_iWheelX=event.wheel.x;
            		m_iWheelY=event.wheel.y;
/*
    			if (event.wheel.y < 0)
        			std::cout << "MOUSE : WHEEL DOWN" << std::endl;
    			else
        			std::cout << "MOUSE : WHEEL UP" << std::endl;
*/
			break;
#endif
				default:
					break;
			}
        }
#if 0
        // Get keyboard state
        const Uint8* m_KeyboardState = SDL_GetKeyboardState(nullptr);
        if (m_KeyboardState[SDL_SCANCODE_F1])
            				{
						m_Key=SDLK_F1;
						return true;
					}
					if (m_KeyboardState[SDL_SCANCODE_F2])
            				{
						m_Key=SDLK_F2;
						return true;
					}
            				if (m_KeyboardState[SDL_SCANCODE_UP])
            				{
						m_Key=SDLK_UP;
						return true;
					}
					if (m_KeyboardState[SDL_SCANCODE_DOWN])
            				{
						m_Key=SDLK_DOWN;
						return true;
					}
            				if (m_KeyboardState[SDL_SCANCODE_PAGEUP])
            				{
						m_Key=SDLK_PAGEUP;
						return true;
					}
					if (m_KeyboardState[SDL_SCANCODE_PAGEDOWN])
            				{
						m_Key=SDLK_PAGEDOWN;
						return true;

					}

        // Get modifier keys state
        SDL_Keymod m_modState = SDL_GetModState();

			if ( (event.key.keysym.sym == SDLK_g) &&
			     (m_modState & KMOD_CTRL) ) {
				STX_Service::GetWindowInstance()->HotKey_ToggleGrab();
			}
			if ( (event.key.keysym.sym == SDLK_z) &&
			     (m_modState & KMOD_CTRL) ) {
				STX_Service::GetWindowInstance()->HotKey_Iconify();
			}
			if ( (event.key.keysym.sym == SDLK_RETURN) &&
			     (m_modState & KMOD_ALT) ) {
				STX_Service::GetWindowInstance()->HotKey_ToggleFullScreen();
			}
#endif

    }
            				
		#if 0
		if(sdl_events.size()) sdl_events.pop_front();
		sdl_events.push_back(eventlst);
		#endif

float x=0.0f;//319.5f;//9.8875f;//*2.0f*PI/360.0f;
float y=0.0f;//300.96f;//=6.836f/2.0f;//*2.0f*PI/360.0f;

#if defined(SDL_2_0_5_BUILD) 
	UINT width = 0;
	UINT height = 0;
#else
	const SDL_VideoInfo * vi=SDL_GetVideoInfo();
	UINT width = vi->current_w;
	UINT height = vi->current_h;
#endif
x+=m_MouseX//GetMouseX()
//*2.0f*PI
*360.0f*1.1f
/ width;
y+=m_MouseY//GetMouseY()
//*PI
*180.0f*1.1f
/ height;
/*
//! Left mouse button
#define IND_MBUTTON_LEFT 0
//! Right mouse button
#define IND_MBUTTON_RIGHT 1
//! Middle mouse button
#define IND_MBUTTON_MIDDLE 2
//! Wheelup mouse button
#define IND_MBUTTON_WHEELUP 3
//! Wheeldown mouse button
#define IND_MBUTTON_WHEELDOWN 4
*/


	m_fX=x;
	m_fY=y;
if(IsMouseButtonPressed(STX_MBUTTON_WHEELDOWN))
	  m_fZ=y;
if(IsMouseButtonPressed(STX_MBUTTON_LEFT))
{
	  m_fSpinX=x;
      m_fSpinY=y;
	  m_fSpinX_L=x;
      m_fSpinY_L=y;
}
if(IsMouseButtonPressed(STX_MBUTTON_RIGHT))
{
      m_fSpinX_R=x;
      m_fSpinY_R=y;
}
/*
	for(unsigned int i=0;i<keys.size();i++)
	{
		//printf("keys[%d]= %c\n", i, keys[i]);
	} */
		return false;
	}

int SDLInput::GetMouseX(void)
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	m_fX=x;
	m_fY=y;
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	//printf("SDLInput::GetMouseX()=%d\n", x);
	return x;
}
int SDLInput::GetMouseY(void)
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	m_fX=x;
	m_fY=y;
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	//printf("SDLInput::GetMouseY()=%d\n", y);
	return y;
}


bool SDLInput::IsWindowMoved(void)
{
#if defined(SDL_2_0_5_BUILD)
    if (event.type == SDL_WINDOWEVENT)
	if (event.window.event==SDL_WINDOWEVENT_MOVED)
		return true;
#endif
	return false;
}

bool SDLInput::IsWindowResized(void)
{
#if defined(SDL_2_0_5_BUILD)
    if (event.type == SDL_WINDOWEVENT)
	if (event.window.event==SDL_WINDOWEVENT_RESIZED)
		return true;
#endif
	return false;
}

bool SDLInput::IsWindowMinimised(void)
{
#if defined(SDL_2_0_5_BUILD)
    if (event.type == SDL_WINDOWEVENT)
	if (event.window.event==SDL_WINDOWEVENT_MINIMIZED)
		return true;
#endif
	return false;
}

bool SDLInput::IsWindowMaximised(void)
{
#if defined(SDL_2_0_5_BUILD)
    if (event.type == SDL_WINDOWEVENT)
	if (event.window.event==SDL_WINDOWEVENT_MAXIMIZED)
		return true;
#endif
	return false;
}

bool SDLInput::IsWindowClosed(void)
{
#if defined(SDL_2_0_5_BUILD)
    if (event.type == SDL_WINDOWEVENT)
	if (event.window.event==SDL_WINDOWEVENT_CLOSE)
		return true;
#endif
	return false;
}

Sint32 SDLInput::getKeyCode()
{
	if(m_Key)
	{
		#if 0
		printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		printf("Key m_Key=%x pressed", m_Key);
		#endif
		return m_Key;
	}
	return '\0';
}
int SDLInput::getKeyID(void){return getKeyCode();}
char SDLInput::getKeyChar(void){return getKeyCode();}

bool SDLInput::IsKeyPressed(void)
{
	if(m_Key)
		return true;
	return false;
}

bool SDLInput::IsKeyReleased(void)
{
	return !IsKeyPressed();
}

bool SDLInput::IsMouseButtonPressed(void)
{
	return IsMouseButton();
}

bool SDLInput::IsMouseButtonReleased(void)
{
	return !IsMouseButton();
}

bool SDLInput::IsMouseClick(void)
{
	return IsMouseButton();
}

bool SDLInput::IsMouseDoubleClick(void)
{
	return false;
}
bool SDLInput::IsMouseButton(void)
{
	m_MouseState=SDL_GetMouseState(NULL, NULL);
	if(m_MouseState & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		//printf("SDL_BUTTON_LEFT\n");
		//printf("m_fX=%f\n", m_fX);
		//printf("m_fY=%f\n", m_fY);
		return true;
	}
	if(m_MouseState & SDL_BUTTON(SDL_BUTTON_RIGHT))
	{
		//printf("SDL_BUTTON_RIGHT\n");
		//printf("m_fX=%f\n", m_fX);
		//printf("m_fY=%f\n", m_fY);
		return true;
	}
	if(m_MouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE))
	{
		//printf("SDL_BUTTON_MIDDLE\n");
		//printf("m_fX=%f\n", m_fX);
		//printf("m_fY=%f\n", m_fY);
		return true;
	}
	#if defined(SDL_1_2_15_BUILD)
	if(m_MouseState & SDL_BUTTON(SDL_BUTTON_WHEELUP))
	{
		//printf("SDL_BUTTON_WHEELUP\n");
		//printf("m_fX=%f\n", m_fX);
		//printf("m_fY=%f\n", m_fY);
		return true;
	}
	if(m_MouseState & SDL_BUTTON(SDL_BUTTON_WHEELDOWN))
	{
		//printf("SDL_BUTTON_WHEELDOWN\n");
		//printf("m_fX=%f\n", m_fX);
		//printf("m_fY=%f\n", m_fY);
		return true;
	}
	#endif
	return false;
}
bool SDLInput::IsMouseMotion(void)
{
	return (event.motion.state==SDL_MOUSEMOTION);//???
}
bool SDLInput::IsKeyPressed(eKey e,unsigned int)
{
	if(m_Key!=e) return false;
	#if 0
		printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		printf("Key e=%x pressed", e);
		printf("Key m_Key=%x pressed", m_Key);
	#endif
	return true;
}
bool SDLInput::IsKeyPressed(eKey e)
{
	return IsKeyPressed(e, 0);
}

bool SDLInput::OnMouseButtonPress(eMouseButton e){return MouseButtonPressed(e);}
bool SDLInput::OnMouseButtonRelease(eMouseButton e){return MouseButtonPressed(e);}
bool SDLInput::IsMouseButtonPressed(eMouseButton e,unsigned int){return MouseButtonPressed(e);}
bool SDLInput::IsMouseButtonPressed(eMouseButton e){return MouseButtonPressed(e);}
bool SDLInput::MouseButtonPressed(eMouseButton e)
{
	m_MouseState=SDL_GetMouseState(NULL, NULL);
	if((m_MouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) &&(e==STX_MBUTTON_LEFT))
	{
		//printf("STX_MBUTTON_LEFT\n");
		return true;
	}
	if((m_MouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) &&(e==STX_MBUTTON_RIGHT))
	{
		//printf("STX_MBUTTON_RIGHT\n");
		return true;
	}
	if((m_MouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) &&(e==STX_MBUTTON_MIDDLE))
	{
		//printf("STX_MBUTTON_MIDDLE\n");
		return true;
	}
	#if defined(SDL_1_2_15_BUILD)
	if((m_MouseState & SDL_BUTTON(SDL_BUTTON_WHEELUP)) &&(e==STX_MBUTTON_WHEELUP))
	{
		//printf("STX_MBUTTON_WHEELUP\n");
		return true;
	}
	if((m_MouseState & SDL_BUTTON(SDL_BUTTON_WHEELDOWN)) &&(e==STX_MBUTTON_WHEELDOWN))
	{
		//printf("STX_MBUTTON_WHEELDOWN\n");
		return true;
	}
	#elif defined(SDL_2_0_5_BUILD)
/*
case SDL_MOUSEWHEEL:

    if (evenements.wheel.y < 0)
        std::cout << "MOUSE : WHEEL DOWN" << std::endl;
    else
        std::cout << "MOUSE : WHEEL UP" << std::endl;
break;
*/
	if((m_iWheelY<0) &&(e==STX_MBUTTON_WHEELDOWN)) return true;
	else if(e==STX_MBUTTON_WHEELUP) return true;
	#endif
	return false;
}

int SDLInput::GetJoyX(void){return 0;}
int SDLInput::GetJoyY(void){return 0;}
bool SDLInput::IsJoyAxisMoving(void){return false;}
bool SDLInput::IsJoyBallMoving(void){return false;}
bool SDLInput::IsJoyHatMoving(void){return false;}
bool SDLInput::IsJoyButton(void){return false;}
unsigned char SDLInput::GetAxisJoystickDeviceIndex(void){return 0;}
unsigned char SDLInput::GetAxisIndex(void){return 0;}
short SDLInput::GetAxisValue(void){return 0;}
unsigned char SDLInput::GetBallJoystickDeviceIndex(void){return 0;}
unsigned char SDLInput::GetBallIndex(void){return 0;}
short SDLInput::GetBallXRel(void){return 0;}
short SDLInput::GetBallYRel(void){return 0;}
unsigned char SDLInput::GetHatJoystickDeviceIndex(void){return 0;}
unsigned char SDLInput::GetHatIndex(void){return 0;}
unsigned char SDLInput::GetHatValue(void){return 0;}
unsigned char SDLInput::GetButtonJoystickDeviceIndex(void){return 0;}
unsigned char SDLInput::GetButtonIndex(void){return 0;}
unsigned char SDLInput::GetButtonState(void){return 0;}
#endif

