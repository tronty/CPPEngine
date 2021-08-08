/*****************************************************************************************
/* File: IND_Font.h
/* Desc: Font object
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

#ifndef _IND_FONT_
#define _IND_FONT_

#define MAX_CHARS 4096


// --------------------------------------------------------------------------------
//									 IND_Font
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Font IND_Font
\ingroup Objects
Font class managed by IND_FontManager for drawing texts into the screen, click in IND_Font to see all the methods of this class.
*/
/*@{*/

/*!
\b IND_Font is a bitmap font object from the class ::IND_FontManager. Read the explanation in ::IND_FontManager for having more details.
*/
class DLL_EXP IND_Font
{
public:

	// ----- Public Gets ------

	//! This function returns the number of characters of the font.
	int GetNumChars		()		{ return mFont.mNumChars;	}
	//! This function returns the name of the font configuration file.
	const char *GetFileName	()		{ return mFont.mName.c_str();		}

private:

	// ----- Structures ------

	// LETTER
	struct structLetter
	{
		unsigned char mLetter;					// Character
		int mOffsetX;					// Offset x in the bitmap
		int mOffsetY;					// Offset y in the bitmap
		int mWidthChar;					// Character width			
		int mHeightChar;				// Character height

		// Attributes initialization
		structLetter ()
		{
			mLetter = 0;
			mOffsetX, mOffsetY, mWidthChar, mHeightChar = 0;
		}
	};
	typedef struct structLetter LETTER;

	// FONT
	struct structFont
	{
		LETTER *mLetters;				// Letter array
		int		mNumChars;				// Num of chars
		std::string mName;					// Font name
		IND_Surface *mSurface;// Bitmap (IND_Surface object) with the letters
		structFont () 
		{
			mLetters=0;
			mNumChars=0;
			mName		= "";
			mSurface=0;
		}
	};
	typedef struct structFont FONT;

	FONT mFont;

	// ----- Private sets ------

	void		SetLetters			(LETTER *pLetters)		{mFont.mLetters = pLetters;			}
	void		SetNumChars			(int pNumChars)			{mFont.mNumChars = pNumChars;		}	
	void		SetFileName			(const char *pName)			{mFont.mName = pName;				}
	void		SetSurface			(IND_Surface *pSurface)	{mFont.mSurface = pSurface;			}

	// ----- Private gets ------

	LETTER		*GetLetters			()						{ return mFont.mLetters;			}		
	IND_Surface *GetSurface			()						{ return mFont.mSurface;			}
	const char		*GetName			()						{ return mFont.mName.c_str();				}
	
	// ----- Friends -----
	
	friend class IND_FontManager;
	friend class IND_Render;
};
/*@}*/

#endif // _IND_FONT_
