/*****************************************************************************************
/* File: IND_FontManager.h
/* Desc: Manager de fuentes
/*****************************************************************************************/

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


#ifndef _IND_FONTMANAGER_
#define _IND_FONTMANAGER_

// ----- Includes -----

#include "IND_SurfaceManager.h"
#include "IND_Math.h"
#include "IND_Font.h"
#include <list>


// --------------------------------------------------------------------------------
//									 IND_FontManager
// --------------------------------------------------------------------------------

/*!
\defgroup IND_FontManager IND_FontManager
\ingroup Managers
Manager of IND_Font objects. Click in ::IND_FontManager to see all the methods of this class.
*/
/*@{*/

/*!
This class manages the fonts objects ::IND_Font. Fonts are used to write texts on the screen.

(::IND_Font) manages the fonts 
created with the \b MudFont program. In order to use these fonts, you will need to create a font
using the special modified MudFont editor for \b IndieLib (see tools section in 
http://www.indielib.com). Once the font has been created,
it can be added to the manager using IND_FontManager::Add().

\b Note: The version of \b MudFont that uses \b IndieLib is a modification of the original
source code. You can find it in the \b tools section in the website. <i>(actually, only the
output has been modified in order that \b IndieLib can read it) </i>
  
The \b original MudFont program can be found in http://www.midwinter.com/~lch/programming/mudgefont/

\image html mudge.png Generate your own fonts quickly with MudFont and use them directly in IndieLib (IND_Font)
*/
class IND_SurfaceManager;
class DLL_EXP IND_FontManager
{
public:

	// ----- Init/End -----

	IND_FontManager		(): mOk(false)	{ }
	~IND_FontManager	()				{ End(); }

	bool	Init    (IND_ImageManager *pImageManager, IND_SurfaceManager *pSurfaceManager);
	void	End     ();
	bool	IsOk	() { return mOk; }
			
	// ----- Public methods -----

	bool Add			(IND_Font		*pNewFont,
						const char			*pName,
						const char			*pFile,
						IND_Type		pType,
						IND_Quality		pQuality);

	bool Add			(IND_Font		*pNewFont,
						IND_Image		*pImage,
						const char			*pFile,
						IND_Type		pType,
						IND_Quality		pQuality);

	bool Delete			(IND_Font		*pFo);

private:

	// ----- Private -----
	
	bool mOk;					// Manager initialization flag

	// ----- Enum -----

	enum 
	{
		CHAR_ID,
		OFFSET_X,
		OFFSET_Y,
		WIDTH,
		HEIGHT,
		KW_UNKNOWN,
	};

	// ----- Objects -----

	IND_ImageManager *mImageManager;
	IND_SurfaceManager *mSurfaceManager;

	// ----- Containers -----

	list <IND_Font*> *mListFonts;
	
	// ----- Private methods -----
	
	bool				ParseFont		(IND_Font *pNewFont, const char *pFontName);
	
	void				AddToList		(IND_Font *pNewFont);
	void				DelFromList		(IND_Font *pFo);	

	void				WriteMessage	();
	void				InitVars		();
	void				FreeVars		();
};
/*@}*/
	
#endif // _IND_FONTMANAGER_
