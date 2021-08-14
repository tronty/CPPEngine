/*
 *  cInput.h
 *  Kore-Engine
 *
 *  Description: Holds all the input core
 *
 *  Created by Sean Chapel on 2/1/06.
 *  Copyright 2006 Seoushi Games. All rights reserved.
 *
 */
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef CINPUT
#define CINPUT

	class cInput
	{
		public:
			/// Default Constructor
			cInput();

			/// Default Destructor
			~cInput();

			/// Tells if a key is inbetween being pressed and released
			bool IsKeyHeld(eKey Key);

			/// Tells if a key is pressed
			bool IsKeyDown(eKey Key);

			/// Tells if a key is released
			bool IsKeyUp(eKey Key);

			/// Updates keyboard events
			bool Update();

	bool OnShiftKeyPress(){return false;}
	bool OnLShiftKeyPress(){return false;}
	bool OnRShiftKeyPress(){return false;}

	bool OnCTRLKeyPress(){return false;}
	bool OnLCTRLKeyPress(){return false;}
	bool OnRCTRLKeyPress(){return false;}

	bool OnALTKeyPress(){return false;}
	bool OnLALTKeyPress(){return false;}
	bool OnRALTKeyPress(){return false;}

	bool OnNUMKeyPress(){return false;}
	bool OnCAPSKeyPress(){return false;}
	bool OnMODEKeyPress(){return false;}

	bool OnKeyPress				(eKey pKey);
	bool OnKeyRelease			(eKey pKey);
	bool IsKeyPressed			(eKey pKey);
	bool IsKeyPressed			(eKey pKey, unsigned int pTime);

	bool IsMouseButton			();
	bool IsMouseMotion			();
	int GetMouseX				();
	int GetMouseY				();
	bool OnMouseButtonPress		(eMouseButton pMouseButton);
	bool OnMouseButtonRelease	(eMouseButton pMouseButton);
	bool IsMouseButtonPressed	(eMouseButton pMouseButton);
	bool IsMouseButtonPressed	(eMouseButton pMouseButton, unsigned int pTime);

	float& GetSpinX();
	float& GetSpinY();
	float& GetX();
	float& GetY();
	float& GetZ();
	float& GetSpinX_L();
	float& GetSpinY_L();
	float& GetSpinX_R();
	float& GetSpinY_R();

	int GetJoyX();
	int GetJoyY();
	bool IsJoyAxisMoving();
	bool IsJoyBallMoving();
	bool IsJoyHatMoving();
	bool IsJoyButton			();

	Uint8 GetAxisJoystickDeviceIndex();	/**< The joystick device index */
	Uint8 GetAxisIndex();	/**< The joystick axis index */
	Sint16 GetAxisValue();	/**< The axis value (range: -32768 to 32767) */

	Uint8 GetBallJoystickDeviceIndex();	/**< The joystick device index */
	Uint8 GetBallIndex();	/**< The joystick trackball index */
	Sint16 GetBallXRel();	/**< The relative motion in the X direction */
	Sint16 GetBallYRel();	/**< The relative motion in the Y direction */

	Uint8 GetHatJoystickDeviceIndex();	/**< The joystick device index */
	Uint8 GetHatIndex();	/**< The joystick hat index */
	Uint8 GetHatValue();	/**< The hat position value:*/

	Uint8 GetButtonJoystickDeviceIndex();	/**< The joystick device index */
	Uint8 GetButtonIndex();	/**< The joystick button index */
	Uint8 GetButtonState();	/**< SDL_PRESSED or SDL_RELEASED */
	bool Quit ();
	int getKeyID();
	char getKeyChar();
		private:

			std::map<int, char> m_Keystates;	/**< Holds the state of the keys */
			int m_MouseX;
			int m_MouseY;
			std::vector< char > m_MouseButtons;
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
	};
#endif

