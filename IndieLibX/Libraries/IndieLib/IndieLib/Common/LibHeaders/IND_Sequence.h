/*****************************************************************************************
/* File: IND_Sequence.h
/* Desc: Sequence object
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

#ifndef _IND_SEQUENCE_
#define _IND_SEQUENCE_

#include <vector>
#include "IND_Timer.h"
#include "Defines.h"


// --------------------------------------------------------------------------------
//										IND_Sequence
// --------------------------------------------------------------------------------

class IND_Sequence
{
private:

	// ----- Structures ------

	struct structFrameTime
	{
		int mPos;							// Position in vector frames
		int mTime;							// Frame duration
		structFrameTime ()
		{
			mPos = 0;
			mTime = 150;
		}
	};
	typedef struct structFrameTime FRAME_TIME;
	
	// Sequence (list of frames) 
	struct structSequence
	{
		IND_Timer mTimer;					// Sequence timer
		int i;								// Pointer to actual frame			
		int mWidth;							// With of the wider frame of the sequence
		int mHeight;						// Height of the wider frame of the sequence
		int mNumFrames;						// Number of frames of the sequence
		std::string mName;						// Sequence name
		vector <FRAME_TIME*> *mListFrames;	// List of frames with their times	
		bool mIsActive;						// Flage
		structSequence ()
		{
			i = 0;
			mIsActive = 0;
			mWidth = mHeight = mNumFrames =  0;
			mName = "";
			mListFrames = new vector <FRAME_TIME*>;
		}
	};
	typedef struct structSequence SEQUENCE;

	SEQUENCE mSequence;

	// ----- Private sets ------

	void					SetActualFramePos		(int pPos)				{ mSequence.i = pPos;										}
	void					SetHighWidth			(int pWidth)			{ mSequence.mWidth = pWidth;								}
	void					SetHighHeight			(int pHeight)			{ mSequence.mHeight = pHeight;								}
	void					SetNumFrames			(int pNumFrames)		{ mSequence.mNumFrames = pNumFrames;						}
	void					SetName					(const char *pName)			{ mSequence.mName = pName;									}
	void					SetPosInVectorFrame		(int pPos, int pNewPos)	{ (*mSequence.mListFrames) [pPos]->mPos = pNewPos;			}
	void					SetTimeInVectorFrame	(int pPos, int pTime)	{ (*mSequence.mListFrames) [pPos]->mTime = pTime;			}

	void					SetIsActive				(bool pAct)				{ mSequence.mIsActive = pAct;								}

	// ----- Private gets ------

	IND_Timer				*GetSequenceTimer		()						{ return &mSequence.mTimer;									}
	int						GetActualFramePos		()						{ return mSequence.i;										}
	int						GetHighWidth			()						{ return mSequence.mWidth;									}
	int						GetHighHeight			()						{ return mSequence.mHeight;									}	
	
	bool					GetIsActive				()						{ return mSequence.mIsActive;								}

	int						GetNumFrames			()						{ return mSequence.mNumFrames;								}
	const char					*GetName				()						{ return mSequence.mName.c_str();									}
	vector <FRAME_TIME*>	*GetListFrames			()						{ return mSequence.mListFrames;								}

	int						GetActualFramePosInVec	()						{ return (*GetListFrames ()) [GetActualFramePos()]->mPos;	}	
	int						GetActualFrameTime		()						{ return (*GetListFrames ()) [GetActualFramePos()]->mTime;	}	

	
	// ----- Friends -----

	friend class IND_Animation;
	friend class IND_AnimationManager;
};

#endif // _IND_FRAME_
