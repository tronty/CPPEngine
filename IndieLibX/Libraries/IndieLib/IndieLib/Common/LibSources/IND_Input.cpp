#if 0
/*****************************************************************************************
 * File: IND_Input.h
 * Desc: Input class (wrapping SDL input)
 *****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (javierlopezpro@gmail.com)

This library is free software you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free Software
Foundation either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this library if not, write to the Free Software Foundation, Inc., 59 Temple Place,
Suite 330, Boston, MA 02111-1307 USA 
*/
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
	
*/

// ----- Includes -----



#include <CIndieLib.h>

#include <IND_Input.h>


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
\b Operation:
This function returns 1 (true) if the adminstrator is successfully initialized.
Must be called before using any method.
*/
bool IND_Input::Init (IND_Render *pRender)
{
	End ()
	FreeVars ()

	InitVars ()

	mOk = true

	mRender = pRender

	return mOk
}


/*!
\b Operation:

This function frees the manager and all the objects that it contains.
*/
void IND_Input::End()
{
	if (mOk)
	{
		FreeVars ()

		mOk = false
	}
}


// --------------------------------------------------------------------------------
//									  Public methods
// --------------------------------------------------------------------------------

/*!
\b Operation:

Update input function. This method has to be called everytime in the game loop.
*/
	  float IND_Input::m_fSpinX=0.0f
      float IND_Input::m_fSpinY=0.0f
      float IND_Input::m_fX=0.0f
      float IND_Input::m_fY=0.0f
      float IND_Input::m_fZ=0.0f
      float  IND_Input::m_fSpinX_L=0.0f
      float  IND_Input::m_fSpinY_L=0.0f
      float  IND_Input::m_fSpinX_R=0.0f
      float  IND_Input::m_fSpinY_R=0.0f


void IND_Input::Update()
{
	static SDL_Event mEvent
	CKey::key=0

	// ----- Flags initialization -----

	for (int i = 0 i < 132 i++)
	{
		mKeys [i].mPressed = 0
		mKeys [i].mReleased = 0
	}

	mMouse.mMouseMotion = 0
	mMouse.mMouseButtons [IND_MBUTTON_LEFT].mPressed = 0
	mMouse.mMouseButtons [IND_MBUTTON_LEFT].mReleased = 0
	mMouse.mMouseButtons [IND_MBUTTON_RIGHT].mPressed = 0
	mMouse.mMouseButtons [IND_MBUTTON_RIGHT].mReleased = 0
	mMouse.mMouseButtons [IND_MBUTTON_MIDDLE].mPressed = 0
	mMouse.mMouseButtons [IND_MBUTTON_MIDDLE].mReleased = 0
	mMouse.mMouseButtons [IND_MBUTTON_WHEELUP].mPressed = 0
	mMouse.mMouseButtons [IND_MBUTTON_WHEELUP].mReleased = 0
	mMouse.mMouseButtons [IND_MBUTTON_WHEELDOWN].mPressed = 0
	mMouse.mMouseButtons [IND_MBUTTON_WHEELDOWN].mReleased = 0

	// ----- Update -----

	while (SDL_PollEvent (&mEvent))
	{
		// ----- When minimized or focus lost -----

		while (!(SDL_GetAppState () & SDL_APPACTIVE) || !(SDL_GetAppState () & SDL_APPINPUTFOCUS))
		{
			SDL_WaitEvent(&mEvent)
			mRender->ResetTimer()
		}

	mMouse.mMouseMotion = 0
	mMouse.mMouseButton=0
	mMouse.mMouseWheel=0
	mMouse.mbMouseButton=0
	mJoy.mJoyAxisMotion = 0
	mJoy.mJoyBallMotion = 0
	mJoy.mJoyHatMotion = 0
	mJoy.mJoyButton=0

		// ----- Mouse ------

		SDL_GetMouseState (&mMouse.mMouseX, &mMouse.mMouseY)

		switch (mEvent.type)
		{
			// ----- Window closed ------

			case SDL_QUIT: mQuit = 1 break

			// ----- Window focus changed -----

			/*
			case SDL_ACTIVEEVENT:
			if (mEvent.active.state & SDL_APPACTIVE )
			{
				// Application iconified
				if( mEvent.active.gain == 0 )
				{

				}
				// Application restored
				else
				{

				}
			}
			break
			*/


			case SDL_JOYAXISMOTION: mJoy.mJoyAxisMotion = 1
				mJoy.AxisJoystickDeviceIndex=mEvent.jaxis.which
				mJoy.axisIndex=mEvent.jaxis.axis
				mJoy.axisvalue=mEvent.jaxis.value
				break
			case SDL_JOYBALLMOTION: mJoy.mJoyBallMotion = 1
				mJoy.BallJoystickDeviceIndex=mEvent.jball.which
				mJoy.ballIndex=mEvent.jball.ball
				mJoy.ballxrel=mEvent.jball.xrel
				mJoy.ballyrel=mEvent.jball.yrel
				break
			case SDL_JOYHATMOTION: mJoy.mJoyHatMotion = 1
				mJoy.HatJoystickDeviceIndex=mEvent.jhat.which
				mJoy.hatIndex=mEvent.jhat.hat
				mJoy.hatvalue=mEvent.jhat.value
				break
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
	//mEvent.jaxis
	//mEvent.jball
	//mEvent.jhat
	mJoy.AxisJoystickDeviceIndex=mEvent.jbutton.which	/**< The joystick device index */
	mJoy.buttonIndex=mEvent.jbutton.button	/**< The joystick button index */
	switch (mEvent.jbutton.state)
	{
	case SDL_PRESSED:
		mJoy.buttonState=IND_JOYBUTTONDOWN	/**< SDL_PRESSED or SDL_RELEASED */
		mJoy.mJoyButton=1
				break
	case SDL_RELEASED:
		mJoy.buttonState=IND_JOYBUTTONUP	/**< SDL_PRESSED or SDL_RELEASED */
				break
	}
			//case SDL_JOYEVENTMASK: mMouse.mMouseMotion = 1	break

			// ----- Mouse motion -----

			case SDL_MOUSEMOTION: mMouse.mMouseMotion = 1	break

			// ----- Mouse buttons -----

			case SDL_MOUSEBUTTONDOWN:
				mMouse.mbMouseButton=1
			switch (mEvent.button.button)
			{
				case SDL_BUTTON_LEFT: mMouse.mMouseButtons [IND_MBUTTON_LEFT].SetState (IND_MBUTTON_PRESSED) mMouse.mMouseButton=1 break
				case SDL_BUTTON_RIGHT: mMouse.mMouseButtons [IND_MBUTTON_RIGHT].SetState (IND_MBUTTON_PRESSED) mMouse.mMouseButton=1 break
				case SDL_BUTTON_MIDDLE: mMouse.mMouseButtons [IND_MBUTTON_MIDDLE].SetState (IND_MBUTTON_PRESSED) mMouse.mMouseButton=1 break
				case SDL_BUTTON_WHEELUP: mMouse.mMouseButtons [IND_MBUTTON_WHEELUP].SetState (IND_MBUTTON_PRESSED) mMouse.mMouseWheel=1 break
				case SDL_BUTTON_WHEELDOWN: mMouse.mMouseButtons [IND_MBUTTON_WHEELDOWN].SetState (IND_MBUTTON_PRESSED) mMouse.mMouseWheel=1 break

			}
			break

			case SDL_MOUSEBUTTONUP:
			switch (mEvent.button.button)
			{
				case SDL_BUTTON_LEFT: mMouse.mMouseButtons [IND_MBUTTON_LEFT].SetState (IND_MBUTTON_NOT_PRESSED) mMouse.mMouseButton=1 break
				case SDL_BUTTON_RIGHT: mMouse.mMouseButtons [IND_MBUTTON_RIGHT].SetState (IND_MBUTTON_NOT_PRESSED) mMouse.mMouseButton=1 break
				case SDL_BUTTON_MIDDLE: mMouse.mMouseButtons [IND_MBUTTON_MIDDLE].SetState (IND_MBUTTON_NOT_PRESSED) mMouse.mMouseButton=1 break
				case SDL_BUTTON_WHEELUP: mMouse.mMouseButtons [IND_MBUTTON_WHEELUP].SetState (IND_MBUTTON_NOT_PRESSED) mMouse.mMouseWheel=1 break
				case SDL_BUTTON_WHEELDOWN: mMouse.mMouseButtons [IND_MBUTTON_WHEELDOWN].SetState (IND_MBUTTON_NOT_PRESSED) mMouse.mMouseWheel=1 break
			}
			break
		}

		// ----- Keyboard ------
		CKey::key=mEvent.key.keysym.sym

		switch (mEvent.key.keysym.sym)
		{
		case SDLK_a:			mKeys [IND_A].SetState (mEvent.type)  break
			case SDLK_b:			mKeys [IND_B].SetState (mEvent.type)  break
			case SDLK_c:			mKeys [IND_C].SetState (mEvent.type)  break
			case SDLK_d:			mKeys [IND_D].SetState (mEvent.type)  break
			case SDLK_e:			mKeys [IND_E].SetState (mEvent.type)  break
			case SDLK_f:			mKeys [IND_F].SetState (mEvent.type)  break
			case SDLK_g:			mKeys [IND_G].SetState (mEvent.type)  break
			case SDLK_h:			mKeys [IND_H].SetState (mEvent.type)  break
			case SDLK_i:			mKeys [IND_I].SetState (mEvent.type)  break
			case SDLK_j:			mKeys [IND_J].SetState (mEvent.type)  break
			case SDLK_k:			mKeys [IND_K].SetState (mEvent.type)  break
			case SDLK_l:			mKeys [IND_L].SetState (mEvent.type)  break
			case SDLK_m:			mKeys [IND_M].SetState (mEvent.type)  break



			case SDLK_n:			mKeys [IND_N].SetState (mEvent.type)  break
			case SDLK_o:			mKeys [IND_O].SetState (mEvent.type)  break
			case SDLK_p:			mKeys [IND_P].SetState (mEvent.type)  break
			case SDLK_q:			mKeys [IND_Q].SetState (mEvent.type)  break
			case SDLK_r:			mKeys [IND_R].SetState (mEvent.type)  break
			case SDLK_s:			mKeys [IND_S].SetState (mEvent.type)  break
			case SDLK_t:			mKeys [IND_T].SetState (mEvent.type)  break
			case SDLK_u:			mKeys [IND_U].SetState (mEvent.type)  break
			case SDLK_v:			mKeys [IND_V].SetState (mEvent.type)  break
			case SDLK_w:			mKeys [IND_W].SetState (mEvent.type)  break
			case SDLK_x:			mKeys [IND_X].SetState (mEvent.type)  break
			case SDLK_y:			mKeys [IND_Y].SetState (mEvent.type)  break
			case SDLK_z:			mKeys [IND_Z].SetState (mEvent.type)  break

			case SDLK_BACKSPACE:	mKeys [IND_BACKSPACE].SetState (mEvent.type)  break
			case SDLK_TAB:			mKeys [IND_TAB].SetState (mEvent.type)  break
			case SDLK_CLEAR:		mKeys [IND_CLEAR].SetState (mEvent.type)  break
			case SDLK_RETURN:		mKeys [IND_RETURN].SetState (mEvent.type)  break
			case SDLK_PAUSE:		mKeys [IND_PAUSE].SetState (mEvent.type)  break
			case SDLK_ESCAPE:		mKeys [IND_ESCAPE].SetState (mEvent.type)  break
			case SDLK_SPACE:		mKeys [IND_SPACE].SetState (mEvent.type)  break
			case SDLK_EXCLAIM:		mKeys [IND_EXCLAIM].SetState (mEvent.type)  break
			case SDLK_QUOTEDBL:		mKeys [IND_QUOTEDBL].SetState (mEvent.type)  break
			case SDLK_HASH:			mKeys [IND_HASH].SetState (mEvent.type)  break
			case SDLK_DOLLAR:		mKeys [IND_DOLLAR].SetState (mEvent.type)  break
			case SDLK_AMPERSAND:	mKeys [IND_AMPERSAND].SetState (mEvent.type)  break
			case SDLK_QUOTE:		mKeys [IND_QUOTE].SetState (mEvent.type)  break
			case SDLK_LEFTPAREN:	mKeys [IND_LEFTPAREN].SetState (mEvent.type)  break
			case SDLK_RIGHTPAREN:	mKeys [IND_RIGHTPAREN].SetState (mEvent.type)  break
			case SDLK_ASTERISK:		mKeys [IND_ASTERISK].SetState (mEvent.type)  break
			case SDLK_PLUS:			mKeys [IND_PLUS].SetState (mEvent.type)  break
			case SDLK_COMMA:		mKeys [IND_COMMA].SetState (mEvent.type)  break
			case SDLK_MINUS:		mKeys [IND_MINUS].SetState (mEvent.type)  break
			case SDLK_PERIOD:		mKeys [IND_PERIOD].SetState (mEvent.type)  break
			case SDLK_SLASH:		mKeys [IND_SLASH].SetState (mEvent.type)  break

			case SDLK_0:			mKeys [IND_0].SetState (mEvent.type)  break
			case SDLK_1:			mKeys [IND_1].SetState (mEvent.type)  break
			case SDLK_2:			mKeys [IND_2].SetState (mEvent.type)  break
			case SDLK_3:			mKeys [IND_3].SetState (mEvent.type)  break
			case SDLK_4:			mKeys [IND_4].SetState (mEvent.type)  break
			case SDLK_5:			mKeys [IND_5].SetState (mEvent.type)  break
			case SDLK_6:			mKeys [IND_6].SetState (mEvent.type)  break
			case SDLK_7:			mKeys [IND_7].SetState (mEvent.type)  break
			case SDLK_8:			mKeys [IND_8].SetState (mEvent.type)  break
			case SDLK_9:			mKeys [IND_9].SetState (mEvent.type)  break

			case SDLK_COLON:		mKeys [IND_COLON].SetState (mEvent.type)  break
			case SDLK_SEMICOLON:	mKeys [IND_SEMICOLON].SetState (mEvent.type)  break
			case SDLK_LESS:			mKeys [IND_LESS].SetState (mEvent.type)  break
			case SDLK_EQUALS:		mKeys [IND_EQUALS].SetState (mEvent.type)  break
			case SDLK_GREATER:		mKeys [IND_GREATER].SetState (mEvent.type)  break
			case SDLK_QUESTION:		mKeys [IND_QUESTION].SetState (mEvent.type)  break
			case SDLK_AT:			mKeys [IND_AT].SetState (mEvent.type)  break
			case SDLK_LEFTBRACKET:	mKeys [IND_LEFTBRACKET].SetState (mEvent.type)  break
			case SDLK_BACKSLASH:	mKeys [IND_BACKSLASH].SetState (mEvent.type)  break
			case SDLK_RIGHTBRACKET:	mKeys [IND_RIGHTBRACKET].SetState (mEvent.type)  break
			case SDLK_CARET:		mKeys [IND_CARET].SetState (mEvent.type)  break
			case SDLK_UNDERSCORE:	mKeys [IND_UNDERSCORE].SetState (mEvent.type)  break
			case SDLK_BACKQUOTE:	mKeys [IND_BACKQUOTE].SetState (mEvent.type)  break

			case SDLK_DELETE:		mKeys [IND_DELETE].SetState (mEvent.type)  break
			case SDLK_KP0:			mKeys [IND_K0].SetState (mEvent.type)  break
			case SDLK_KP1:			mKeys [IND_K1].SetState (mEvent.type)  break
			case SDLK_KP2:			mKeys [IND_K2].SetState (mEvent.type)  break
			case SDLK_KP3:			mKeys [IND_K3].SetState (mEvent.type)  break
			case SDLK_KP4:			mKeys [IND_K4].SetState (mEvent.type)  break
			case SDLK_KP5:			mKeys [IND_K5].SetState (mEvent.type)  break
			case SDLK_KP6:			mKeys [IND_K6].SetState (mEvent.type)  break
			case SDLK_KP7:			mKeys [IND_K7].SetState (mEvent.type)  break
			case SDLK_KP8:			mKeys [IND_K8].SetState (mEvent.type)  break
			case SDLK_KP9:			mKeys [IND_K9].SetState (mEvent.type)  break

			case SDLK_KP_PERIOD:	mKeys [IND_KPERIOD].SetState (mEvent.type)  break
			case SDLK_KP_DIVIDE:	mKeys [IND_KDIVIDE].SetState (mEvent.type)  break
			case SDLK_KP_MULTIPLY:	mKeys [IND_KMULTIPLY].SetState (mEvent.type)  break
			case SDLK_KP_MINUS:		mKeys [IND_KMINUS].SetState (mEvent.type)  break
			case SDLK_KP_PLUS:		mKeys [IND_KPLUS].SetState (mEvent.type)  break
			case SDLK_KP_ENTER:		mKeys [IND_KENTER].SetState (mEvent.type)  break
			case SDLK_KP_EQUALS:	mKeys [IND_KEQUALS].SetState (mEvent.type)  break

			case SDLK_UP:			mKeys [IND_KEYUP].SetState (mEvent.type)  break
			case SDLK_DOWN:			mKeys [IND_KEYDOWN].SetState (mEvent.type)  break
			case SDLK_RIGHT:		mKeys [IND_KEYRIGHT].SetState (mEvent.type)  break
			case SDLK_LEFT:			mKeys [IND_KEYLEFT].SetState (mEvent.type)  break
			case SDLK_INSERT:		mKeys [IND_INSERT].SetState (mEvent.type)  break
			case SDLK_HOME:			mKeys [IND_HOME].SetState (mEvent.type)  break
			case SDLK_END:			mKeys [IND_END].SetState (mEvent.type)  break
			case SDLK_PAGEUP:		mKeys [IND_PAGEUP].SetState (mEvent.type)  break
			case SDLK_PAGEDOWN:		mKeys [IND_PAGEDOWN].SetState (mEvent.type)  break

			case SDLK_F1:			mKeys [IND_F1].SetState (mEvent.type)  break
			case SDLK_F2:			mKeys [IND_F2].SetState (mEvent.type)  break
			case SDLK_F3:			mKeys [IND_F3].SetState (mEvent.type)  break
			case SDLK_F4:			mKeys [IND_F4].SetState (mEvent.type)  break
			case SDLK_F5:			mKeys [IND_F5].SetState (mEvent.type)  break
			case SDLK_F6:			mKeys [IND_F6].SetState (mEvent.type)  break
			case SDLK_F7:			mKeys [IND_F7].SetState (mEvent.type)  break
			case SDLK_F8:			mKeys [IND_F8].SetState (mEvent.type)  break
			case SDLK_F9:			mKeys [IND_F9].SetState (mEvent.type)  break
			case SDLK_F10:			mKeys [IND_F10].SetState (mEvent.type)  break
			case SDLK_F11:			mKeys [IND_F11].SetState (mEvent.type)  break
			case SDLK_F12:			mKeys [IND_F12].SetState (mEvent.type)  break
			case SDLK_F13:			mKeys [IND_F13].SetState (mEvent.type)  break
			case SDLK_F14:			mKeys [IND_F14].SetState (mEvent.type)  break
			case SDLK_F15:			mKeys [IND_F15].SetState (mEvent.type)  break

			case SDLK_NUMLOCK:		mKeys [IND_NUMLOCK].SetState (mEvent.type)  break
			case SDLK_CAPSLOCK:		mKeys [IND_CAPSLOCK].SetState (mEvent.type)  break
			case SDLK_SCROLLOCK:	mKeys [IND_SCROLLOCK].SetState (mEvent.type)  break
			case SDLK_RSHIFT:		mKeys [IND_RSHIFT].SetState (mEvent.type)  break
			case SDLK_LSHIFT:		mKeys [IND_LSHIFT].SetState (mEvent.type)  break
			case SDLK_RCTRL:		mKeys [IND_RCTRL].SetState (mEvent.type)  break
			case SDLK_LCTRL:		mKeys [IND_LCTRL].SetState (mEvent.type)  break
			case SDLK_RALT:			mKeys [IND_RALT].SetState (mEvent.type)  break
			case SDLK_LALT:			mKeys [IND_LALT].SetState (mEvent.type)  break
			case SDLK_RMETA:		mKeys [IND_RMETA].SetState (mEvent.type)  break
			case SDLK_LMETA:		mKeys [IND_LMETA].SetState (mEvent.type)  break
			case SDLK_LSUPER:		mKeys [IND_LSUPER].SetState (mEvent.type)  break
			case SDLK_RSUPER:		mKeys [IND_RSUPER].SetState (mEvent.type)  break
			case SDLK_MODE:			mKeys [IND_MODE].SetState (mEvent.type)  break
			case SDLK_HELP:			mKeys [IND_HELP].SetState (mEvent.type)  break
			case SDLK_PRINT:		mKeys [IND_PRINT].SetState (mEvent.type)  break
			case SDLK_SYSREQ:		mKeys [IND_SYSREQ].SetState (mEvent.type)  break
			case SDLK_BREAK:		mKeys [IND_BREAK].SetState (mEvent.type)  break
			case SDLK_MENU:			mKeys [IND_MENU].SetState (mEvent.type)  break
			case SDLK_POWER:		mKeys [IND_POWER].SetState (mEvent.type)  break
			case SDLK_EURO:			mKeys [IND_EURO].SetState (mEvent.type)  break
		}
	}
float x=0.0f//319.5f//9.8875f//*2.0f*PI/360.0f
float y=0.0f//300.96f//=6.836f/2.0f//*2.0f*PI/360.0f

x+=IRenderer::GetInputInstance()->GetMouseX()
//*2.0f*PI
*360.0f*1.1f
/ CIndieLib::Instance()->Window->GetWidth()
y+=IRenderer::GetInputInstance()->GetMouseY()
//*PI
*180.0f*1.1f
/ CIndieLib::Instance()->Window->GetHeight()
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


	  m_fX=x
      m_fY=y
if(IRenderer::GetInputInstance()->IsMouseButtonPressed(IND_MBUTTON_WHEELDOWN))
	  m_fZ=y
if(IRenderer::GetInputInstance()->IsMouseButtonPressed(IND_MBUTTON_LEFT))
{
	  m_fSpinX=x
      m_fSpinY=y
	  m_fSpinX_L=x
      m_fSpinY_L=y
}
if(IRenderer::GetInputInstance()->IsMouseButtonPressed(IND_MBUTTON_RIGHT))
{
      m_fSpinX_R=x
      m_fSpinY_R=y
}
}


/*!
\b Operation:

This method returns true if the user closes the window.
*/
bool IND_Input::Quit ()
{
	return mQuit==1
}


/*!
\b Parameters:

\arg \b pKey				Key that we want to check. See ::IND_Key.

\b Operation:

This function returns true if the key passed as \b pKey parameter is pressed.
*/
bool IND_Input::OnKeyPress	(IND_Key pKey)
{
	return mKeys [pKey].mPressed
}


/*!
\b Parameters:

\arg \b pKey				Key that we want to check. See ::IND_Key.

\b Operation:

This function returns true if the key passed as \b pKey parameter is released.
*/
bool IND_Input::OnKeyRelease	(IND_Key pKey)
{
	return mKeys [pKey].mReleased
}


/*!
\b Parameters:

\arg \b pKey				Key that we want to check. See ::IND_Key.

\b Operation:

This function returns true if the key passed as \b pKey parameter is being pressed.
*/
bool IND_Input::IsKeyPressed	(IND_Key pKey)
{
	if (mKeys [pKey].mKeyState == IND_KEY_PRESSED)
	{
		return 1;
	}

	return 0;
}


/*!
\b Parameters:

\arg \b pKey				Key that we want to check. See ::IND_Key.
\arg \b pTime				Time that has to pass in milliseconds.

\b Operation:

This function returns true if the key passed as \b pKey parameter has been pressed \b pTime milliseconds.
*/
bool IND_Input::IsKeyPressed	(IND_Key pKey, unsigned int pTime)
{
	if (mKeys [pKey].mKeyState == IND_KEY_PRESSED && mKeys [pKey].mTimer.GetTicks() > pTime)
	{
		mKeys [pKey].mTimer.Start()
		return 1;
	}

	return 0;
}


/*!
\b Operation:

This function returns the true if the mouse is moving
*/
bool IND_Input::IsMouseMotion ()
{
	return mMouse.mMouseMotion
}


/*!
\b Operation:

This function returns the horizontal position of the mouse in screen pixels
*/
int IND_Input::GetMouseX ()
{
	return mMouse.mMouseX
}


/*!
\b Operation:

This function returns the vertical position of the mouse in screen pixels
*/
int IND_Input::GetMouseY ()
{
	return mMouse.mMouseY
}


/*!
\b Parameters:

\arg \b pMouseButton				Mouse button that we want to check. See ::IND_MouseButton.

\b Operation:

This function returns true if the mouse button passed as \b pMouseButton is pressed. It will return true again only if the button
is released and pressed again.
*/
bool IND_Input::OnMouseButtonPress (IND_MouseButton pMouseButton)
{
	return mMouse.mMouseButtons [pMouseButton].mPressed
}


/*!
\b Parameters:

\arg \b pMouseButton				Mouse button that we want to check. See ::IND_MouseButton.

\b Operation:

This function returns true if the mouse button passed as \b pMouseButton is released. It will return true again only if the button
is pressed and released again.
*/
bool IND_Input::OnMouseButtonRelease	(IND_MouseButton pMouseButton)
{
	return mMouse.mMouseButtons [pMouseButton].mReleased
}


/*!
\b Parameters:

\arg \b pMouseButton				Mouse button that we want to check. See ::IND_MouseButton.

\b Operation:

This function returns true if the mouse button passed as \b pMouseButton is being pressed.
*/
bool IND_Input::IsMouseButtonPressed (IND_MouseButton pMouseButton)
{
	if (mMouse.mMouseButtons [pMouseButton].mButtonState == IND_MBUTTON_PRESSED)
	{
		return 1;
	}

	return 0;
}


/*!
\b Parameters:

\arg \b pMouseButton		Mouse button that we want to check. See ::IND_MouseButton.
\arg \b pTime				Time that has to pass in milliseconds.

\b Operation:

This function returns true if the mouse button passed as \b pMouseButton parameter has been pressed \b pTime milliseconds.
*/
bool IND_Input::IsMouseButtonPressed (IND_MouseButton pMouseButton, unsigned int pTime)
{
	if (mMouse.mMouseButtons [pMouseButton].mButtonState == IND_MBUTTON_PRESSED && mMouse.mMouseButtons [pMouseButton].mTimer.GetTicks() > pTime)
	{
		mMouse.mMouseButtons [pMouseButton].mTimer.Start()
		return 1;
	}

	return 0;
}


// --------------------------------------------------------------------------------
//									 Private methods
// --------------------------------------------------------------------------------

/*
*************************
Initialized the input flags
*************************
*/
void IND_Input::InitFlags ()
{
	for (int i = 0 i < 132 i++)
	{
		mKeys [i].mKey = i
		mKeys [i].mPressed = 0
		mKeys [i].mReleased = 0
	}

	mMouse.mMouseButtons [IND_MBUTTON_LEFT].mPressed = 0
	mMouse.mMouseButtons [IND_MBUTTON_LEFT].mReleased = 0
	mMouse.mMouseButtons [IND_MBUTTON_RIGHT].mPressed = 0
	mMouse.mMouseButtons [IND_MBUTTON_RIGHT].mReleased = 0
	mMouse.mMouseButtons [IND_MBUTTON_MIDDLE].mPressed = 0
	mMouse.mMouseButtons [IND_MBUTTON_MIDDLE].mReleased = 0
	mMouse.mMouseButtons [IND_MBUTTON_WHEELUP].mPressed = 0
	mMouse.mMouseButtons [IND_MBUTTON_WHEELUP].mReleased = 0
	mMouse.mMouseButtons [IND_MBUTTON_WHEELDOWN].mPressed = 0
	mMouse.mMouseButtons [IND_MBUTTON_WHEELDOWN].mReleased = 0
}


/*
*************************
Init vars
*************************
*/
void IND_Input::InitVars ()
{
	mMouse.mMouseMotion = 0
	InitFlags ()
	mQuit = 0
}


/*
*************************
Free memory
*************************
*/
void IND_Input::FreeVars ()
{

}
#endif


