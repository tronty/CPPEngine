/*****************************************************************************************
 * File: IND_Input.h
 * Desc: Input class (wrapping SDL input)
 *****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (javierlopezpro@gmail.com)

This library is free software; you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place,
Suite 330, Boston, MA 02111-1307 USA 
*/
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
	
*/

#ifndef _IND_INPUT_
#define _IND_INPUT_
#include <IND_Timer.h>
#include <IND_Render.h>
#endif
#if 0
// ----- Includes -----

#include <IND_Timer.h>
#include <IND_Render.h>

// --------------------------------------------------------------------------------
//									      CKey
// --------------------------------------------------------------------------------

// This class stores information about a keyboard key
class CKey
{
public:

	// ----- Init/End -----

	CKey ()
	{
		mTimer.Start ();
		mKeyState = IND_KEY_NOT_PRESSED;
	}

	~CKey	()	{}

	// ----- Public methods -----

	// Sets the key state
	void SetState (IND_KeyState pKeyState)
	{
		// If the key was pressed and is not pressed anymore we set the flag "released"
		if (mKeyState == IND_KEY_PRESSED && pKeyState == IND_KEY_NOT_PRESSED)
			mReleased = 1;

		// If the key was not pressed and now is pressed we set the flag "pressed"
		if (mKeyState == IND_KEY_NOT_PRESSED && pKeyState == IND_KEY_PRESSED)
			mPressed = 1;

		// Set the new state
		mKeyState = pKeyState;

		// If the key is pressed, we reset the timer
		if (pKeyState == IND_KEY_PRESSED)
		{
			mTimer.Start ();
		}
	}

	bool mReleased;
	bool mPressed;
	static int key;
	IND_Key mKey;
	IND_KeyState mKeyState;
	IND_Timer mTimer;
};


// --------------------------------------------------------------------------------
//									      CMouse
// --------------------------------------------------------------------------------

// This class stores information about a mouse button
class CMouseButton
{
public:

	// ----- Init/End -----

	CMouseButton	()
	{
		mTimer.Start ();
		mButtonState = IND_MBUTTON_NOT_PRESSED;
	}

	~CMouseButton	()	{}

	// ----- Methods -----

	// Sets the button state
	void SetState (IND_MouseButtonState pButtonState)
	{
		// If the button was pressed and is not being pressed anymore we set the flag "released"
		if (mButtonState == IND_MBUTTON_PRESSED && pButtonState == IND_MBUTTON_NOT_PRESSED)
			mReleased = 1;

		// If the button was not pressed and is now being pressed we set the flag "pressed"
		if (mButtonState == IND_MBUTTON_NOT_PRESSED && pButtonState == IND_MBUTTON_PRESSED)
			mPressed = 1;

		// Sets the new state
		mButtonState = pButtonState;

		// If the button is pressed, we reset the timer
		if (pButtonState == IND_MBUTTON_PRESSED)
		{
			mTimer.Start ();
		}
	}

	bool mPressed;
	bool mReleased;
	IND_MouseButton mButton;
	IND_MouseButtonState mButtonState;
	IND_Timer mTimer;
};


// Class that stores information about the mouse
class CMouse
{
public:

	// ----- Init/End -----

	CMouse	()	{}
	~CMouse	()	{}

	int mMouseX, mMouseY;
	bool mMouseMotion;
	//bool mbMouseButton;

	//int mMouseMotion;
	int mMouseButton;
	int mbMouseButton;
	int mMouseWheel;

	CMouseButton mMouseButtons [5];
};
class CJoy
{
public:

	// ----- Init/End -----

	CJoy	()	{}
	~CJoy	()	{}

	int mJoyX, mJoyY;
	bool mJoyAxisMotion;
	bool mJoyBallMotion;
	bool mJoyHatMotion;
	bool mJoyButton;

	Uint8 AxisJoystickDeviceIndex;	/**< The joystick device index */
	Uint8 axisIndex;	/**< The joystick axis index */
	Sint16 axisvalue;	/**< The axis value (range: -32768 to 32767) */

	Uint8 BallJoystickDeviceIndex;	/**< The joystick device index */
	Uint8 ballIndex;	/**< The joystick trackball index */
	Sint16 ballxrel;	/**< The relative motion in the X direction */
	Sint16 ballyrel;	/**< The relative motion in the Y direction */

	Uint8 HatJoystickDeviceIndex;	/**< The joystick device index */
	Uint8 hatIndex;	/**< The joystick hat index */
	Uint8 hatvalue;	/**< The hat position value:*/

	Uint8 ButtonJoystickDeviceIndex;	/**< The joystick device index */
	Uint8 buttonIndex;	/**< The joystick button index */
	Uint8 buttonState;	/**< SDL_PRESSED or SDL_RELEASED */
};

// --------------------------------------------------------------------------------
//									 IND_Input
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Input IND_Input
\ingroup Input
IND_Input class for having input from keyboard and mouse. Click in IND_Input to see all the methods of this class.
*/
/*@{*/

class INDIELIBAPI IInput
{
public:

	// ----- Init/End -----

	IInput		()						{ }
	virtual ~IInput		()									{ }

	// ----- Public methods -----

	/** @name Input common
	*
	*/
	//@{

	virtual void Update		()=0;
	//@}

	/** @name Keyboard
	*
	*/
	//@{
	virtual bool OnKeyPress				(IND_Key pKey)=0;
	virtual bool OnKeyRelease			(IND_Key pKey)=0;
	virtual bool IsKeyPressed			(IND_Key pKey)=0;
	virtual bool IsKeyPressed			(IND_Key pKey, unsigned int pTime)=0;
	//@}

	/** @name Mouse
	*
	*/
	//@{
	virtual bool IsMouseButton			()=0;
	virtual bool IsMouseMotion			()=0;
	virtual int GetMouseX				()=0;
	virtual int GetMouseY				()=0;
	virtual bool OnMouseButtonPress		(IND_MouseButton pMouseButton)=0;
	virtual bool OnMouseButtonRelease	(IND_MouseButton pMouseButton)=0;
	virtual bool IsMouseButtonPressed	(IND_MouseButton pMouseButton)=0;
	virtual bool IsMouseButtonPressed	(IND_MouseButton pMouseButton, unsigned int pTime)=0;
	//@}
	virtual float& GetSpinX()=0;
	virtual float& GetSpinY()=0;
	virtual float& GetX()=0;
	virtual float& GetY()=0;
	virtual float& GetZ()=0;
	virtual float& GetSpinX_L()=0;
	virtual float& GetSpinY_L()=0;
	virtual float& GetSpinX_R()=0;
	virtual float& GetSpinY_R()=0;

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

};
/*!
\b IND_Input is a wrapper class of SDL input functions for giving IndieLib the possibility of
keyboard and mouse Input.
*/
class INDIELIBAPI IND_Input : public IInput
{
public:

	// ----- Init/End -----

	IND_Input		(): IInput(),mOk(false)						{ }
	virtual ~IND_Input		()									{ End(); }

	bool	Init    (IND_Render *pRender);
	void	End     ();
	bool	IsOk	() { return mOk; }

	// ----- Public methods -----

	/** @name Input common
	*
	*/
	//@{

	void Update		();
	bool Quit		();
	//@}

	/** @name Keyboard
	*
	*/
	//@{
	bool OnKeyPress				(IND_Key pKey);
	bool OnKeyRelease			(IND_Key pKey);
	bool IsKeyPressed			(IND_Key pKey);
	bool IsKeyPressed			(IND_Key pKey, unsigned int pTime);
	//@}

	/** @name Mouse
	*
	*/
	//@{
	virtual bool IsMouseButton			(){return mMouse.mbMouseButton==1;}
	bool IsMouseMotion			();
	int GetMouseX				();
	int GetMouseY				();
	bool OnMouseButtonPress		(IND_MouseButton pMouseButton);
	bool OnMouseButtonRelease	(IND_MouseButton pMouseButton);
	bool IsMouseButtonPressed	(IND_MouseButton pMouseButton);
	bool IsMouseButtonPressed	(IND_MouseButton pMouseButton, unsigned int pTime);
	//@}
	float& GetSpinX(){return m_fSpinX;}
	float& GetSpinY(){return m_fSpinY;}
	float& GetX(){return m_fX;}
	float& GetY(){return m_fY;}
	float& GetZ(){return m_fZ;}
	float& GetSpinX_L(){return m_fSpinX_L;}
	float& GetSpinY_L(){return m_fSpinY_L;}
	float& GetSpinX_R(){return m_fSpinX_R;}
	float& GetSpinY_R(){return m_fSpinY_R;}

	int GetJoyX(){return mJoy.mJoyX;}
	int GetJoyY(){return mJoy.mJoyY;}
	bool IsJoyAxisMoving(){return mJoy.mJoyAxisMotion;}
	bool IsJoyBallMoving(){return mJoy.mJoyBallMotion;}
	bool IsJoyHatMoving(){return mJoy.mJoyHatMotion;}
	virtual bool IsJoyButton			(){return mJoy.mJoyButton;}

	Uint8 GetAxisJoystickDeviceIndex(){return mJoy.AxisJoystickDeviceIndex;}	/**< The joystick device index */
	Uint8 GetAxisIndex(){return mJoy.axisIndex;}	/**< The joystick axis index */
	Sint16 GetAxisValue(){return mJoy.axisvalue;}	/**< The axis value (range: -32768 to 32767) */

	Uint8 GetBallJoystickDeviceIndex(){return mJoy.BallJoystickDeviceIndex;}	/**< The joystick device index */
	Uint8 GetBallIndex(){return mJoy.ballIndex;}	/**< The joystick trackball index */
	Sint16 GetBallXRel(){return mJoy.ballxrel;}	/**< The relative motion in the X direction */
	Sint16 GetBallYRel(){return mJoy.ballyrel;}	/**< The relative motion in the Y direction */

	Uint8 GetHatJoystickDeviceIndex(){return mJoy.HatJoystickDeviceIndex;}	/**< The joystick device index */
	Uint8 GetHatIndex(){return mJoy.hatIndex;}	/**< The joystick hat index */
	Uint8 GetHatValue(){return mJoy.hatvalue;}	/**< The hat position value:*/

	Uint8 GetButtonJoystickDeviceIndex(){return mJoy.ButtonJoystickDeviceIndex;}	/**< The joystick device index */
	Uint8 GetButtonIndex(){return mJoy.buttonIndex;}	/**< The joystick button index */
	Uint8 GetButtonState(){return mJoy.buttonState;}	/**< SDL_PRESSED or SDL_RELEASED */

		int getKeyID(){return CKey::key;}
		char getKeyChar(){return CKey::key;}
	// ----- Objects -----
private:
     static float m_fSpinX;
     static float m_fSpinY;
     static float m_fX;
     static float m_fY;
     static float m_fZ;
     static float  m_fSpinX_L;
     static float  m_fSpinY_L;
     static float  m_fSpinX_R;
     static float  m_fSpinY_R;

	CKey mKeys [132];
	CMouse mMouse;
	CJoy mJoy;


	// ----- Private -----

	IND_Render *mRender;
	bool mOk;
	bool mQuit;


	// ----- Private methods -----

	void				InitFlags		();
	void				InitVars		();
	void				FreeVars		();
};
/*@}*/

#endif // _IND_INPUT_
