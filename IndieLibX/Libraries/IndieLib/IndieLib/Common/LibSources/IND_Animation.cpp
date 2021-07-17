/*****************************************************************************************
 * File: IND_Animation.h
 * Desc: Animation object
 *****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (info@pixelartgames.com)

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


#include <IND_Animation.h>


// --------------------------------------------------------------------------------
//										Public methods
// --------------------------------------------------------------------------------


IND_Image *IND_Animation::GetImage (int pFrame)
{
	return (*GetVectorFrames ()) [pFrame]->GetImage ();
}


IND_Surface	*IND_Animation::GetSurface (int pFrame)
{
	return (*GetVectorFrames ()) [pFrame]->GetSurface ();
}


int	IND_Animation::GetHighWidth	(int pSequence)
{
	return (*GetListSequences ()) [pSequence]->GetHighWidth ();
}


int	IND_Animation::GetHighHeight (int pSequence)
{
	return (*GetListSequences ()) [pSequence]->GetHighHeight ();
}


int	IND_Animation::GetNumFrames	(int pSequence)
{
	return (*GetListSequences ()) [pSequence]->GetNumFrames ();
}


const char *IND_Animation::GetName (int pSequence)
{
	return (*GetListSequences ()) [pSequence]->GetName ();
}


int	IND_Animation::GetActualFramePos	(int pSequence)
{
	return (*GetListSequences ()) [pSequence]->GetActualFramePos();
}


int	IND_Animation::GetActualFrameTime	(int pSequence)
{
	return (*GetListSequences ()) [pSequence]->GetActualFrameTime();
}


int	IND_Animation::GetActualOffsetX	(int pSequence)
{
	return (*GetVectorFrames ()) [(*GetListSequences ()) [pSequence]->GetActualFramePosInVec ()]->GetOffsetX ();
}


int	IND_Animation::GetActualOffsetY	(int pSequence)
{
	return (*GetVectorFrames ()) [(*GetListSequences ()) [pSequence]->GetActualFramePosInVec ()]->GetOffsetY ();
}


IND_Surface	*IND_Animation::GetActualSurface	(int pSequence)
{
	return (*GetVectorFrames ()) [(*GetListSequences ()) [pSequence]->GetActualFramePosInVec ()]->GetSurface ();
}


void IND_Animation::SetActualFramePos (int pSequence, int pPos)
{
	(*GetListSequences ()) [pSequence]->SetActualFramePos (pPos);
}


IND_Timer *IND_Animation::GetSequenceTimer	(int pSequence)
{
	return (*GetListSequences ()) [pSequence]->GetSequenceTimer ();
}


bool IND_Animation::GetIsActive (int pSequence)
{
	return (*GetListSequences ()) [pSequence]->GetIsActive ();
}


void IND_Animation::SetIsActive	(int pSequence, bool pAct)
{
	(*GetListSequences ()) [pSequence]->SetIsActive (pAct);
}


void IND_Animation::SetImage (int pFrame, IND_Image *pNewImage)
{
	(*GetVectorFrames ()) [pFrame]->SetImage (pNewImage);
}


void IND_Animation::SetSurface (int pFrame, IND_Surface *pNewSurface)
{
	(*GetVectorFrames ()) [pFrame]->SetSurface (pNewSurface);
}

