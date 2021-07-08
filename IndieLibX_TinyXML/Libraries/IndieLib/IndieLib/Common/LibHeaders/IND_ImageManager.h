/*****************************************************************************************
/* File: IND_ImageManager.h
/* Desc: Image Manager
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


#ifndef _IND_IMAGEMANAGER_
#define _IND_IMAGEMANAGER_

// ----- Includes -----

#include "IND_Image.h"
#include <list>

// ----- Defines -----

#define MAX_EXT_IMAGE 6


// --------------------------------------------------------------------------------
//									IND_ImageManager
// --------------------------------------------------------------------------------

/*!
\defgroup IND_ImageManager IND_ImageManager
\ingroup Managers
Manager of IND_Image objects. Click in ::IND_ImageManager to see all the methods of this class.
*/
/*@{*/

/*!
This manager that stores IND_Image objects works as a mediator between the image (bitmap) loaded
in memory and the IND_Surface object which finally appears in the screen. The IND_Image objects 
which are stored in this class can not be rendered directly to the screen.
But this class is very useful for applying <b>filters</b> (like in Gimp or Photoshop programs) to 
loaded images, and later they can be converted to IND_Surface objects, which can be directly 
rendered.

Other possible uses of this class are: 
- <b>Hardness maps</b>. Used for making collisions or path finding.
- GetPixel() / PutPixel() 
- <b>Colorkey</b>. By changing a certain color into transparent using SetAlpha().
- Generation of new images (procedurally).
- Convert formats. Changing the number of bytes per pixel using Convert() 
- Direct accesing to the bitmap. Using IND_Image::GetPointer()
*/
class DLL_EXP IND_ImageManager
{
public:
	
	// ----- Init/End -----

	IND_ImageManager		(): mOk(false)	{ }
	~IND_ImageManager		()				{ End(); }
	IND_ImageManager *mImageManager;

	bool	Init    ();
	void	End     ();
	bool	IsOk	()	const { return mOk; }
					
	// ----- Public methods -----

	bool Add			(IND_Image *pNewImage, const char *pName);
	bool Add			(IND_Image *pNewImage, int pWidth, int pHeight, IND_Format pFormat);
	bool Delete			(IND_Image *pIm);
	bool Save			(IND_Image *pIm, const char *pName);
	bool Clone			(IND_Image *pNewImage, IND_Image *pOldImage);
	
private:

	// ----- Private -----

	bool mOk;								// Manager initialization flag
	char *mSupportedExt [MAX_EXT_IMAGE];	// File extensions supported
	
	// ----- Containers -----

	list <IND_Image*> *mListImages;

	// ----- Private Methods -----

	const char *GetExtensionFromName		(const char *pName);
	bool CheckExtImage				(const char *pExtImage);

	void AddToList					(IND_Image *pNewImage);
	void DelFromList				(IND_Image *pIm);
	void WriteMessage				();
	void InitVars					();
	void FreeVars					();
};
/*@}*/
	
#endif // _IND_IMAGEMANAGER_
