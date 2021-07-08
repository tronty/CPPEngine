/*****************************************************************************************
/* File: IND_Window.h
/* Desc: Manages the window (uses SDL for creating the window)
/*****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (info@pixelartgames.com) 

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


#ifndef _IND_WINDOW_H_
#define _IND_WINDOW_H_


// --------------------------------------------------------------------------------
//									  IND_Window
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Window IND_Window
\ingroup Main
Class for creating the window, click in ::IND_Window to see all the methods of this class.
*/

/*@{*/	

/*!
This class manages the window where the render will be initialized.
*/		
class DLL_EXP IND_Window
{
public:

	// ----- Init/End -----

	IND_Window		(): mOk(false)	{ }
	~IND_Window		()				{ End(); }

	bool	Init    (const char *pTitle, int pWidth, int pHeight, int pBpp, bool pVsync, bool pFullscreen);
	void	End     ();

	//! IsOk
	/*!
	This function returns 1 (true) if the manager is correctly initialized.
	*/	
	bool	IsOk	()			 const { return mOk; }

	// ----- Atributos -----

	//! This function returns the title of the window.
	char  *GetTitle	()			{ return mAttributes.mTitle;		}
	//! This function returns the width of the window.
	int GetWidth		()		{ return mAttributes.mWidth;		}
	//! This function returns the height of the window.
	int GetHeight		()		{ return mAttributes.mHeight;		}
	//! This function returns the window color quality. 32 or 16 bits.
	int GetBpp			()		{ return mAttributes.mBits;			}
	//! This function returns 1 (true) if the window has activated vsync.
	bool IsVsync		()		{ return mAttributes.mVsync;		}
	//! This function returns 1 (true) if the window is set to full screen.
	bool IsFullScreen	()		{ return mAttributes.mFullscreen;	}
	//! This function returns the window HWND.
	//HWND GetWnd			()		{ return mAttributes.mWnd;			}
	// This function returns the window HINSTANCE.
	//HINSTANCE GetInst	()		{ return mAttributes.mInst;			}

	// ----- Public methods -----

	void SetTitle		(const char *pTitle);
	void Cursor			(bool pEnable);


#if !defined(SWIG)
	// Attributes
	struct structAttributes
	{
		int mWidth;
		int mHeight;
		int mBits;
		bool mVsync;
		bool mFullscreen;
		char *mTitle;
		//HWND mWnd;
	};
	struct structAttributes mAttributes;
#endif
private:

	// ----- Private -----

	bool mOk;
	// ----- Private methods -----

	bool CreateMyWindow		(const char *pTitle, int pWidth, int pHeight, int pBpp, bool pVsync, bool pFullscreen);
    bool Register			();
	bool Create				(const char *pTitle, int pWidth, int pHeight, int pBpp, bool pVsync, bool pFullscreen);
	void DestroyIndieLibWindow	();
	void InitAttribWindow		();

	void InitVars ();
	void FreeVars ();

};
/*@}*/

#endif // _IND_WINDOW_H_
