/*****************************************************************************************
/* File: IND_Animation.h
/* Desc: Animation object
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

#ifndef _IND_ANIMATION_
#define _IND_ANIMATION_

// ----- Includes -----

#include "IND_Frame.h"
#include "IND_Sequence.h"


// --------------------------------------------------------------------------------
//									IND_Animation
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Animation IND_Animation
\ingroup Objects
Class for storing sequences of frames (animations). Click in ::IND_Animation to see all the methods of this class.
*/
/*@{*/

/*!
IND_Animation objects are loaded trough the methods of IND_AnimationManager. In order to load
an animation it is necessary to create an animation script in xml format. The structure of the script is quite 
easy to understand.

In each animation script you first define the frames (each of the sprites) that you will use for
the sequences that you want to create. Later, you create sequences, using references to these frames, 
and specifying the time delay between frames. 

So, you can set different sequences using the same frames.

The schema of one animation would be as follows:

\code
Animation
{
	Frames {Frame_1, Frame_2, Frame_3... Frame_n}

	Sequences 
	{
		Sequence_1 (list of n frames) 
		Sequence_2 (list of n frames)
		Sequence_3 (list of n frames)
		...
		Sequence_n
	}
}
\endcode

The  \b tokens or <b>keywords</b> of a \b IndieLib animation file are:

<b><< Relative to frames >></b> 

\arg <b><frames></b>					Frame section
\arg <b><name></b>						Name assigned to the frames (later it will be used in the sequences). 
\arg <b><file></b>						Path to the image file.
\arg <b><offset_x></b>					X offset of the frame.    (Optional, 0 by default).
\arg <b><offset_y></b>					Y offset of the frame.    (Optional, 0 by default).

<b><< Relative to sequences >></b> 

\arg <b><sequences></b>				Indicates that the sequences are going to be defined in the section between brackets.
\arg <b><name></b>					Name assigned to the sequence.
\arg <b><frame name></b>			Name of the frame that we are refering to. Here we have to use one of the names we assigned to the loaded frames.
\arg <b><time></b>					Time, in milliseconds, that the frame will be displayed. (Optional, 150 by default).

\b Note: "//"  can be used before a phrase for writing comments.

\image html surfa1.jpg Animations example in IndieLib.
*/
class DLL_EXP IND_Animation
{
public:

	// ----- Public gets ------

	// ----- Relative to lobal animation ------

	//! This function returns the animation script file name in a string of characters.
	const char					*GetName			()				{ return mAnimation.mName.c_str();																}
	//! This function returns the total number of animation sequences.
	int						GetNumSequences		()				{ return mAnimation.mNumSequences;														}
	//! This function returns the total number of animation frames.
	int						GetNumTotalFrames	()				{ return mVectorFrames->size();															}
	//! This function returns the pointer to the object ::IND_Image that contains the frame received or 0 in case that the object has not been loaded.
	IND_Image				*GetImage			(int pFrame);
	//! This function returns the pointer to the object ::IND_Surface that contains the frame received or 0 in case that the object has not been loaded. 
	IND_Surface				*GetSurface			(int pFrame);

	// ----- Relative to a concrete sequence ------

	//! This function returns the maximum width of the sequence. It means, the width of the widest frame of the sequence.
	int						GetHighWidth		(int pSequence);
	//! This function returns the maximum height of the sequence. It means, the height of the highest frame of the sequence.
	int						GetHighHeight		(int pSequence);
	//! This function returns the number of frames which are referenced by the sequeence received as a parameter.
	int						GetNumFrames		(int pSequence);			
	//! This function returns the name, in a string of characters, of the sequence received as a paramater.
	const char					*GetName			(int pSequence);

	// NOT DOCUMENTED
	int						GetActualFramePos	(int pSequence);
	int						GetActualFrameTime	(int pSequence);
	int						GetActualOffsetX	(int pSequence);
	int						GetActualOffsetY	(int pSequence);
	IND_Surface				*GetActualSurface	(int pSequence);
	void					SetActualFramePos	(int pSequence, int pPos);					
	IND_Timer				*GetSequenceTimer	(int pSequence);
	
	bool					GetIsActive			(int pSequence);
	void					SetIsActive			(int pSequence, bool pAct);
	
	// ----- Public sets ------

	//! This function establishes a IND_Image object in a frame. It can be useful for modifying some frames of animations.Note: It is convenient to eliminate any IND_Image object that would be there before setting the actual one. 
	void					SetImage			(int pFrame, IND_Image *pNewImage);
	//! This function establishes a IND_Surface object in a frame. It can be useful for modifying some frames of animations.Note: It is convenient to eliminate any IND_Surface object that would be there before setting the actual one. 
	void					SetSurface			(int pFrame, IND_Surface *pNewSurface);
	
private:

	// ----- Structures ------

	vector <IND_Frame*> *mVectorFrames;		// Vector of frames

	// Animation (list of sequences)
	struct structAnimation
	{
		std::string mName;							// Animation name
		int mNumSequences;						// Number of sequences
		vector <IND_Sequence*> *mListSequences;	// Sequence list
		structAnimation ()
		{
			mName = "";
			mNumSequences = 0;
		}
		void SetName		(const char *pName)	{ mName = pName;			}
	};
	typedef struct structAnimation ANIMATION;

	ANIMATION mAnimation;

	// ----- Private sets ------

	void					SetName				(const char *pName)				{ mAnimation.mName = pName;																					}
	void					SetNumSequences		(int pNum)					{ mAnimation.mNumSequences = pNum;																			}	

	// ----- Private gets ------

	vector <IND_Sequence*>	*GetListSequences	()							{ return mAnimation.mListSequences;																			}
	vector <IND_Frame*>		*GetVectorFrames	()							{ return mVectorFrames;																						}

	// ----- Friends -----

	friend class IND_AnimationManager;
	friend class IND_Render;
	friend class IND_Entity2dManager;
};
/*@}*/

#endif // _IND_ANIMATION_
