/*****************************************************************************************
/* File: IND_Frame.h
/* Desc: Frame object
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

#ifndef _IND_FRAME_
#define _IND_FRAME_

#include <list>
#include "IND_Image.h"
#include "IND_Surface.h"
#include "Defines.h"

// --------------------------------------------------------------------------------
//									 IND_Frame
// --------------------------------------------------------------------------------

class IND_Frame
{
private:

	// ----- Structures ------

	// Frame
	struct structFrame
	{
		std::string mName;
		IND_Image *mImage;
		IND_Surface *mSurface;	
		int mOffsetX;
		int mOffsetY;
		list <BOUNDING_COLLISION*> *mListBoundingCollision;	
		structFrame () 
		{
			mName		= "";
			mImage		= 0;
			mSurface	= 0;
			mOffsetX = mOffsetY	= 0;
		}
	};
	typedef struct structFrame A_FRAME;

	A_FRAME mFrame;

	// ----- Private sets ------

	void SetName		(const char *pName)		{ mFrame.mName = pName;				}
	void SetImage		(IND_Image *pIm)	{ mFrame.mImage = pIm;				}	
	void SetSurface		(IND_Surface *pSu)	{ mFrame.mSurface = pSu;			}
	void SetOffsetX		(int pOffsetX)		{ mFrame.mOffsetX = pOffsetX;		}
	void SetOffsetY		(int pOffsetY)		{ mFrame.mOffsetY = pOffsetY;		}

	// ----- Private gets ------

	const char		*GetName	()	{ return mFrame.mName.c_str();		}
	IND_Image	*GetImage	()	{ return mFrame.mImage;		}
	IND_Surface *GetSurface	()	{ return mFrame.mSurface;	}
	int			GetOffsetX	()	{ return mFrame.mOffsetX;	}
	int			GetOffsetY	()	{ return mFrame.mOffsetY;	}
	list <BOUNDING_COLLISION*> *GetListBoundingCollision () { return mFrame.mListBoundingCollision; }	
	
	// ----- Friends -----

	friend class IND_Animation;
	friend class IND_AnimationManager;
	friend class IND_Entity2dManager;
};

#endif // _IND_SEQUENCE_
