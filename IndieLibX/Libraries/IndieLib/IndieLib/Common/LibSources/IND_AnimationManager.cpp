/*****************************************************************************************
 * File: IND_AnimationManager.cpp
 * Desc: Manager de animaciones
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
  Copyright (c) 2015 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
	
*/

// ----- Includes -----
#include <CIndieLib.h>

#include <tinyxml/tinyxml.h>
#include <IND_AnimationManager.h>

// ----- Defines -----

#define MAX_TOKEN 1024


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pImageManager			Pointer to the manager IND_ImageManager correctly initialized
\arg \b pSurfaceManager			Pointer to the manager IND_SurfaceManager correctly initialized

\b Operation:

This function returns 1 (true) if the adminstrator is successfully initialized.
Must be called before using any method.
*/
bool IND_AnimationManager::Init (IND_ImageManager *pImageManager, IND_SurfaceManager *pSurfaceManager)
{
	End ();
	InitVars ();

	GetDebugAPI()->Header ("Initializing AnimationManager", 5);

	if (pSurfaceManager->IsOk())
	{
		GetDebugAPI()->Header ("SurfaceManager Ok", 1);
		mSurfaceManager = pSurfaceManager;

		GetDebugAPI()->Header ("ImageManager Ok", 1);
		mImageManager = pImageManager;

		mCollisionParser = CollisionParser::Instance();

		mOk = true;

		GetDebugAPI()->Header ("AnimationManager OK", 6);
	}
	else
	{
		GetDebugAPI()->Header ("SurfaceManager is not correctly initialized", 2);
		mOk = false;
	}

	return mOk;
}


/*!
\b Operation:

This function frees the manager and all the objects that it contains.
*/
void IND_AnimationManager::End()
{
	if (mOk)
	{
		GetDebugAPI()->Header ("Finalizing AnimationManager", 5);
		GetDebugAPI()->Header ("Freeing animations" , 5);
		FreeVars ();
		GetDebugAPI()->Header ("Animations freed", 6);
		GetDebugAPI()->Header ("AnimationManager finalized", 6);

		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//									Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pNewAnimation			Pointer to a new animation object.
\arg \b mAnimation				Name of the animation file.
\arg \b pType					Surface type (see ::IND_Type)
\arg \b pQuality				Surface quality (see ::IND_Quality)

\b Operation:

This function returns 1 (true) if the animation file exists and is added successfully
to the manager loading the frames as IND_Surface objects. So, we will be able to draw
this animation directly.
*/
bool IND_AnimationManager::AddToSurface		(IND_Animation *pNewAnimation,
                                            const char *pAnimation,
											IND_Type pType,
											IND_Quality	pQuality)
{
	if (!AddToImage (pNewAnimation, pAnimation))
		return 0;

	for (int i = 0; i < pNewAnimation->GetNumTotalFrames (); i++)
	{
		// Pointer to the image
		IND_Image *ActualImage = pNewAnimation->GetImage (i);

		// Creation of the surface
		IND_Surface *mNewSurface = new IND_Surface;
		mSurfaceManager->Add (mNewSurface, ActualImage, pType, pQuality);
		pNewAnimation->SetSurface (i, mNewSurface);
		// Free the image
		//???
		#ifndef OMITDELETE
		mImageManager->Delete (ActualImage);
#endif
		pNewAnimation->SetImage (i, 0);
	}

	return 1;
}


/*!
\b Parameters:

\arg \b pNewAnimation			Pointer to a new animation object.
\arg \b mAnimation				Name of the animation XML script.
\arg \b pType					Surface type (see ::IND_Type)
\arg \b pQuality				Surface quality (see ::IND_Quality)
\arg <b>pR, pG, pB</b>			Color where the colorkey will be applied, this area will become transparent.

\b Operation:

This function returns 1 (true) if the animation file exists and is added successfully
to the manager, loading the frames with IND_Surface objects. So, we will be able to draw
this animation directly. Furthermore, it uses the color passed as a parameter to apply
a colorkey to the image. So, the zones coloured with this colour will be
transparent.
*/
bool IND_AnimationManager::AddToSurface		(IND_Animation *pNewAnimation,
                                            const char *pAnimation,
											IND_Type pType,
											IND_Quality	pQuality,
											unsigned char pR,
											unsigned char pG,
											unsigned char pB)
{
	if (!AddToImage (pNewAnimation, pAnimation))
		return 0;


	for (int i = 0; i < pNewAnimation->GetNumTotalFrames (); i++)
	{
		// Pointer to the image
		IND_Image *mCurrentImage = pNewAnimation->GetImage (i);

		// Color key
		mCurrentImage->SetAlpha (pR, pG, pB);

		// Creation of the surface
		IND_Surface *mNewSurface = new IND_Surface;
		mSurfaceManager->Add (mNewSurface, mCurrentImage, pType, pQuality);
		pNewAnimation->SetSurface (i, mNewSurface);

		// Free the image
		#ifndef OMITDELETE
		mImageManager->Delete (mCurrentImage);
#endif
		pNewAnimation->SetImage (i, 0);
	}

	return 1;
}


/*!
\b Parameters:

\arg \b pNewAnimation			Pointer to a new animation object.
\arg \b pAnimation				Name of the animation file.
\arg \b pBlockSize				Width of the blocks
\arg \b pType					Surface type (see ::IND_Type)
\arg \b pQuality				Surface quality (see ::IND_Quality)

\b Operation:

This function returns 1 (true) if the animation file exists and is added successfully
to the manager loading the frames as IND_Surface objects. So, we will be able to draw
this animation directly.
*/
bool IND_AnimationManager::	AddToSurface		(IND_Animation *pNewAnimation,
                                                const char *pAnimation,
												int pBlockSize,
												IND_Type pType,
												IND_Quality	pQuality)
{
	if (!AddToImage (pNewAnimation, pAnimation))
		return 0;

	for (int i = 0; i < pNewAnimation->GetNumTotalFrames (); i++)
	{
		// Pointer to the image
		IND_Image *ActualImage = pNewAnimation->GetImage (i);

		// Creation of the surface
		IND_Surface *mNewSurface = new IND_Surface;
		mSurfaceManager->Add (mNewSurface, ActualImage, pBlockSize,  pType, pQuality);
		pNewAnimation->SetSurface (i, mNewSurface);

		// Free the image
		#ifndef OMITDELETE
		mImageManager->Delete (ActualImage);
#endif
		pNewAnimation->SetImage (i, 0);
	}

	return 1;
}


/*!
\b Parameters:

\arg \b pNewAnimation			Pointer to a new animation object.
\arg \b pAnimation				Name of the animation XML script.
\arg \b pBlockSize				Width of the blocks
\arg \b pType					Surface type (see ::IND_Type)
\arg \b pQuality				Surface quality (see ::IND_Quality)
\arg <b>pR, pG, pB</b>			Color where the colorkey will be applied, this areas will become transparent.

\b Operation:

This function returns 1 (true) if the animation file exists and is added successfully
to the manager, loading the frames with IND_Surface objects. So, we will be able to draw
this animation directly. Furthermore, it uses the color passed as a parameter to apply
a colorkey to the image. So, the areas coloured with this color will become
transparent.
*/
bool IND_AnimationManager::AddToSurface		(IND_Animation *pNewAnimation,
                                            const char *pAnimation,

											int pBlockSize,
											IND_Type pType,
											IND_Quality	pQuality,
											unsigned char pR,
											unsigned char pG,
											unsigned char pB)
{
	if (!AddToImage (pNewAnimation, pAnimation))
		return 0;


	for (int i = 0; i < pNewAnimation->GetNumTotalFrames (); i++)
	{
		// Pointer to the image
		IND_Image *mCurrentImage = pNewAnimation->GetImage (i);

		// Color key
		mCurrentImage->SetAlpha (pR, pG, pB);

		// Creation of the surface
		IND_Surface *mNewSurface = new IND_Surface;
		mSurfaceManager->Add (mNewSurface, mCurrentImage, pBlockSize, pType, pQuality);
		pNewAnimation->SetSurface (i, mNewSurface);

		// Free the image
		#ifndef OMITDELETE
		mImageManager->Delete (mCurrentImage);
#endif
		pNewAnimation->SetImage (i, 0);
	}

	return 1;
}


/*!
\b Parameters:

\arg \b pNewAnimation			Pointer to a new animation object.
\arg \b pAnimation				Name of the animation XML script.

\b Operation:

This function returns 1 (true) if the animation file exists and is added successfully
to the manager, loading the frames as IND_Image objects. So this animation won't be able
to be drawn to the screen directly, but you will be able to modify the IND_Image objects
(for applying a filter, for example) accessing them trough the ::GetImage() function.

Once they have been modified, they can be converted to ::IND_Surfaces and applied to the animation
using ::SetSurface() and then the animation will be able to be drawn correctly
*/
bool IND_AnimationManager::AddToImage (IND_Animation *pNewAnimation, const char *apAnimation)
{
	std::string pAnimation=(apAnimation);

	GetDebugAPI()->Header ("Parsing and loading animation", 5);
	GetDebugAPI()->Header ("File name:", 3);
	GetDebugAPI()->DataChar (pAnimation.c_str(), 1);

	if (!mOk)
	{
		WriteMessage ();
		return 0;
	}

	// ----- Animation file parsing -----



	if (!ParseAnimation (pNewAnimation, (char*)pAnimation.c_str()))
	{
		GetDebugAPI()->Header ("Fatal error, cannot load the animation xml file", 2);
		return 0;
	}

	pNewAnimation->mAnimation.mName = (char*)pAnimation.c_str();


	// ----- Put the object into the manager  -----

	AddToList (pNewAnimation);

	// ----- GetDebugAPI() -----

	GetDebugAPI()->Header("Animation parsed and loaded", 6);

	return 1;
}


/*!
\b Parameters:

\arg \b pAn			Pointer to an animation object.

\b Operation:

This function returns 1 (true) if the animation object passed as a parameter
exists and it is successfully eliminated.
*/
bool IND_AnimationManager::Delete (IND_Animation *pAn)
{
	if (Delete (pAn, 0))
		return 1;

	return 0;
}


// --------------------------------------------------------------------------------
//									Private methods
// --------------------------------------------------------------------------------

/*
*************************
This function returns 1 (true) if the animation object passed as a parameter exists
and returns in pAxisCalX and pAxisCalY two values which will be necessary for the hotspot
specification when we have to call to the function IND_Render::SetTransform() (before
drawing the surface).

So this function is used for changing the reference point of the image.
When we talk about reference point or hotspot, we are refering to an imaginary point
from where the translation, rotation and scaling transformation will be applied. In this way,
we can choose that an animation rotates over its center, over one of its vertex or over
any middle position.

For establishing the hotspot, we use pAxisX and pAxisY, which in a range from 0
until 1, make reference to horizontal and vertical surface position respectivily
In this way (0.5f,0.5f) would be the image centre, (1.0f,0.0f) would be the upper right
corner, etc...

The resulting values (pAxisCalX y pAxisCalY) must be saved and passed as parameter to
the function IND_Render::SetTransform(),so , when we draw the sequence the specified
hotspot will be used.

If this method is not used and we pass to IND_Render::SetTransform() two null values
as the parameters pAxisCalX and pAxisCalY, the transformations will be applied by default
from the left upper corner of the object.

One way of using this function is through ::IND_Entity2dManager, and can be used for
managing all the IndieLib objects and its attributes in a very efficient way.
Using this function separately for each animation can be very uncomfortble.
*/
bool IND_AnimationManager::CalculeAxis	(IND_Animation *pAn,
										 int pSequence,
										float pAxisX,
										float pAxisY,
										int *pAxisCalX,
										int *pAxisCalY)
{
	if (!mOk || !pAn)
	{
		return 0;
	}

	if (pAxisX < 0.0f || pAxisX > 1.0f)
		pAxisX = 0.0f;

	if (pAxisY < 0.0f || pAxisY > 1.0f)
		pAxisY = 0.0f;

	*pAxisCalX = (int) (pAxisX * pAn->GetHighWidth  (pSequence)  * -1);
	*pAxisCalY = (int) (pAxisY * pAn->GetHighHeight (pSequence)  * -1);

	return 1;
}


/*
*************************
Load and image an returns the pointer
*************************
*/
IND_Image *IND_AnimationManager::LoadImage (const char *pName)
{
	IND_Image *mNewImage = new IND_Image;

	if (!mImageManager->Add (mNewImage, pName))
		return 0;

	return mNewImage;
}


/*
*************************
Parses and XML animation file
Uses Tinyxml
*************************
*/
bool IND_AnimationManager::ParseAnimation (IND_Animation *pNewAnimation, const char *pAnimationName)
{
	std::string pAnimation2=(pAnimationName);
	/*
	char* c=(char*)pAnimation2.c_str();
	while(*c)
	{
		if(*c=='\\') *c='/';//???
		c++;
	}*/
#ifdef _MSC_VER
	pAnimation2=replaceAll(pAnimation2,"/","\\");
#else
	pAnimation2=replaceAll(pAnimation2,"\\","/");
#endif
	//LOG_PRINT("pAnimation2=%s\n",pAnimation2);
	TiXmlDocument	*mXmlDoc = new TiXmlDocument(stx_convertpath(pAnimation2).c_str());

	char* c =stx_strrchr(pAnimation2.c_str(),
#ifdef _MSC_VER
		'\\'
#else
		'/'
#endif
		);
	pAnimation2.resize(c-pAnimation2.c_str()+1);
	//LOG_PRINT("pAnimation2.c_str()=%s\n",pAnimation2.c_str());

	// Fatal error, cannot load
	if (!mXmlDoc->LoadFile())
		return 0;

	// Document root
	TiXmlElement *mXAnimation = 0;
	mXAnimation = mXmlDoc->FirstChildElement("animation");

	if (!mXAnimation)
	{
		GetDebugAPI()->Header ("Invalid name for document root, should be <animation>", 2);
		mXmlDoc->Clear();
		delete mXmlDoc;
		return 0;
	}

	// ----------------- Parse frames and create the images -----------------

	// Frames
	TiXmlElement *mXFrames = 0;
	mXFrames = mXAnimation->FirstChildElement("frames");

	if (!mXFrames)
	{
		GetDebugAPI()->Header ("Invalid name for frames child, should be <frames>", 2);
		mXmlDoc->Clear();
		delete mXmlDoc;
		return 0;
	}

	TiXmlElement *mXFrame = 0;
	mXFrame = mXFrames->FirstChildElement("frame");

	if (!mXFrame)
	{
		GetDebugAPI()->Header ("There are no frames to parse", 2);
		mXmlDoc->Clear();
		delete mXmlDoc;
		return 0;
	}

	pNewAnimation->mVectorFrames = new vector <IND_Frame*>;

	// Parse all the frames
	while (mXFrame)
	{
		IND_Frame *mNewFrame = new IND_Frame;
		mNewFrame->mFrame.mListBoundingCollision = new list <BOUNDING_COLLISION*>;

		// Frame name attribute
		if (mXFrame->Attribute("name"))
		{
			mNewFrame->SetName ( (char*)mXFrame->Attribute("name"));
		}
		else
		{
			GetDebugAPI()->Header ("The frame doesn't have a \"name\" attribute", 2);
			mXmlDoc->Clear();
			delete mNewFrame;
			delete mXmlDoc;
			return 0;
		}

		// Frame image file attribute
		if (mXFrame->Attribute("file"))
		{
            const char *f=(const char *) mXFrame->Attribute("file");
#if 1
			std::string buf="";
			buf.append(pAnimation2.c_str());
			buf.append(f);
#ifdef _MSC_VER
	buf=replaceAll(buf,"/","\\");
#else
	buf=replaceAll(buf,"\\","/");
#endif
			//LOG_PRINT("file buf.c_str()=%s\n",buf.c_str());
#else
			char buf[1024];
			stx_snprintf(buf,1024,"%s%s",pAnimation2.c_str(),f);
#endif
			//LOG_PRINT("LoadImage %s\n",buf);
			// Loading image
			mNewFrame->SetImage (LoadImage ((const char *)buf.c_str()));
			//LOG_PRINT("LoadImage2 %s\n",buf);
			if (mNewFrame->GetImage () == 0)
				return 0;
		}
		else
		{
			GetDebugAPI()->Header ("The frame doesn't have a \"file\" attribute", 2);
			mXmlDoc->Clear();
			delete mXmlDoc;
			return 0;
		}

		// Frame offset_x attribute (optional)
		if (mXFrame->Attribute("offset_x"))
		{
			int ox=atoi (mXFrame->Attribute("offset_x"));
			//LOG_PRINT("ox=%d\n",ox);
			mNewFrame->SetOffsetX (ox);
		}

		// Frame offset_y attribute (optional)
		if (mXFrame->Attribute("offset_y"))
		{
			int oy=atoi (mXFrame->Attribute("offset_y"));
			//LOG_PRINT("oy=%d\n",oy);
			mNewFrame->SetOffsetY (oy);
		}

		// Path to a collision file (optional)
		if (mXFrame->Attribute("collision"))
		{
			// ----- Parsing collision file -----

			GetDebugAPI()->Header ("Parsing collision file", 5);
			GetDebugAPI()->Header ("File name:", 3);
			GetDebugAPI()->DataChar ((const char *) mXFrame->Attribute("collision"), 1);

			std::string buf;
			buf=pAnimation2;
			buf.append((const char *) mXFrame->Attribute("collision"));
			//LOG_PRINT("collision buf.c_str()=%s\n",buf.c_str());

			if (!mCollisionParser->ParseCollision (mNewFrame->mFrame.mListBoundingCollision, (const char *) (buf.c_str())))
			{
				GetDebugAPI()->Header ("Fatal error, cannot load the collision xml file", 2);
				return 0;
			}

			// ----- GetDebugAPI() -----

			GetDebugAPI()->Header("Collision file loaded", 6);
		}

		// Push the frame into the frames vector
		pNewAnimation->mVectorFrames->push_back (mNewFrame); //peta

		// Move to the next frame declaration
		mXFrame = mXFrame->NextSiblingElement("frame");
	}

	// ----------------- Parse sequences -----------------

	// Sequences
	TiXmlElement *mXSequences = 0;
	mXSequences = mXFrames->NextSiblingElement("sequences");

	if (!mXSequences)
	{
		GetDebugAPI()->Header ("Invalid name for sequences child, should be <sequences>", 2);
		mXmlDoc->Clear();
		delete mXmlDoc;
		return 0;
	}

	TiXmlElement *mXSequence = 0;
	mXSequence = mXSequences->FirstChildElement("sequence");

	if (!mXSequence)
	{
		GetDebugAPI()->Header ("There are no sequences to parse", 2);
		mXmlDoc->Clear();
		delete mXmlDoc;
		return 0;
	}

	pNewAnimation->mAnimation.mListSequences = new 	vector <IND_Sequence*>;

	// Parse all the sequences
	while (mXSequence)
	{
		IND_Sequence *mNewSequence;

		if (mXSequence->Attribute("name"))
		{
			mNewSequence = new IND_Sequence();
			mNewSequence->GetSequenceTimer()->Start();
			mNewSequence->SetName ((char*)mXSequence->Attribute("name"));
		}
		else
		{
			GetDebugAPI()->Header ("The sequence doesn't have a \"name\" attribute", 2);
			mXmlDoc->Clear();
			delete mXmlDoc;
			return 0;
		}

		TiXmlElement *mXSequenceFrame = 0;
		mXSequenceFrame = mXSequence->FirstChildElement("frame");

		if (!mXSequenceFrame)
		{
			GetDebugAPI()->Header ("There are no frames in the sequence to parse", 2);
			mXmlDoc->Clear();
			delete mXmlDoc;
			return 0;
		}

		if (mXSequenceFrame->Attribute("name"))
		{
			while (mXSequenceFrame)
			{
				// We have to check in the name of this frame has been already declared in
				// frame declaration section
				int mPos = 0;
				if (IsDeclaredFrame ((const char *) mXSequenceFrame->Attribute("name"), pNewAnimation, &mPos))
				{
					IND_Sequence::FRAME_TIME *mNewFrameTime = new IND_Sequence::FRAME_TIME;

					// Frame time points to the frame vector
					mNewFrameTime->mPos = mPos;

					int mWidth	= (*pNewAnimation->GetVectorFrames()) [mPos]->GetImage()->GetWidth();
					int mHeight = (*pNewAnimation->GetVectorFrames()) [mPos]->GetImage()->GetHeight();

					if (mWidth > mNewSequence->GetHighWidth())
					mNewSequence->SetHighWidth (mWidth);

					if (mHeight > mNewSequence->GetHighHeight())
					mNewSequence->SetHighHeight(mHeight);

					// Frame time (optional)
					if (mXSequenceFrame->Attribute("time"))
					{
						mNewFrameTime->mTime = atoi (mXSequenceFrame->Attribute("time"));
					}

					// Push the frame reference into the sequence
					mNewSequence->mSequence.mListFrames->push_back (mNewFrameTime);
					int mNumFrames = mNewSequence->GetNumFrames ();
					mNewSequence->SetNumFrames (mNumFrames + 1);

					// Move to the next frame reference
					mXSequenceFrame = mXSequenceFrame->NextSiblingElement("frame");
				}
				else //if(0)//???
				{
					GetDebugAPI()->Header ("Unknown frame in sequences", 2);
					mXmlDoc->Clear();
					delete mXmlDoc;
					//stx_exit(0);
					return 0;
				}
			}

			// Push the sequence into the animation
			pNewAnimation->mAnimation.mListSequences->push_back (mNewSequence);
			pNewAnimation->mAnimation.mNumSequences++;

			// Move to the next sequence
			mXSequence = mXSequence->NextSiblingElement("sequence");
		}
		else
		{
			GetDebugAPI()->Header ("The frame sequence doesn't have a \"name\" attribute", 2);
			mXmlDoc->Clear();
			delete mXmlDoc;
			return 0;
		}
	}

	// Delete our allocated document and return success ;)
  	mXmlDoc->Clear();
	delete mXmlDoc;

	return 1;
}


/*
*************************
Deletes an animation
*************************
*/
bool IND_AnimationManager::Delete (IND_Animation *pAn, bool pType)
{
	GetDebugAPI()->Header ("Freeing animation", 5);

	if (!mOk)
	{
		WriteMessage ();
		return 0;
	}

	// Search object
	bool mIs = 0;
	list <IND_Animation*>::iterator mAnimationListIter;
	for (mAnimationListIter  = mListAnimations->begin();
		 mAnimationListIter != mListAnimations->end();
		 mAnimationListIter++)
	{
		if ((*mAnimationListIter) == pAn)
		{
			mIs = 1;
			break;
		}
	}

	// Not found
	if (!mIs)
	{
		WriteMessage ();
		return 0;
	}

	// ----- Free object -----

	GetDebugAPI()->Header ("Name:", 3);
	GetDebugAPI()->DataChar ((char*)pAn->mAnimation.mName.c_str(), 1);

	// ------ Free all the surfaces and animations -----

	vector <IND_Frame*>::iterator mVectorFramesIter;
	for	(mVectorFramesIter  = pAn->mVectorFrames->begin();
		mVectorFramesIter  != pAn->mVectorFrames->end();
		mVectorFramesIter++)
	{
#ifndef OMITDELETE
		// Free image
		if ((*mVectorFramesIter)->GetImage())
			mImageManager->Delete ((*mVectorFramesIter)->GetImage());

		// Free surface
		if ((*mVectorFramesIter)->GetSurface())
			mSurfaceManager->Delete ((*mVectorFramesIter)->GetSurface());
#endif
		// Free frame
		//STX_PRINT("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
		delete ((*mVectorFramesIter));// ??? Heap corruption revelation

		//STX_PRINT("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
	}

	// Clear frame vector
	pAn->mVectorFrames->clear ();

	// ----- Free frames-----

	for (int m = 0; m < pAn->GetNumSequences (); m++)
	{
		// Free all the pointers to FRAME_TIME
		vector <IND_Sequence::FRAME_TIME*>::iterator mVectorFrameTimeIter;
		for	(mVectorFrameTimeIter  = (*pAn->mAnimation.mListSequences) [m]->mSequence.mListFrames->begin();
			mVectorFrameTimeIter  != (*pAn->mAnimation.mListSequences) [m]->mSequence.mListFrames->end();
			mVectorFrameTimeIter++)
		{
				// Free pointer
				delete ((*mVectorFrameTimeIter));
		}

		// Free frame list of each sequence
		(*pAn->mAnimation.mListSequences) [m]->mSequence.mListFrames->clear ();
	}

	// ----- Free sequences -----

	// Free all the pointers to SEQUENCE
	vector <IND_Sequence*>::iterator mVectorSequenceIter;
	for	(mVectorSequenceIter  = pAn->mAnimation.mListSequences->begin();
		mVectorSequenceIter  != pAn->mAnimation.mListSequences->end();
		mVectorSequenceIter++)
	{
			// Free pointer
			delete ((*mVectorSequenceIter));
	}

	// Free sequence list
	pAn->mAnimation.mListSequences->clear ();

	if (pType) return 1;

	// Quit from list
	DelFromList (pAn);

	GetDebugAPI()->Header ("Ok", 6);

	return 1;
}


/*
*************************
Method for checking if a frame name has been already declared or not in the frames section
*************************
*/
bool IND_AnimationManager::IsDeclaredFrame (const char *pFrameName, IND_Animation *pNewAnimation, int *pPos)
{
	int mPos = 0;

	vector <IND_Frame*>::iterator mVectorFrameIter;

	for	(mVectorFrameIter  = pNewAnimation->mVectorFrames->begin();
		mVectorFrameIter  != pNewAnimation->mVectorFrames->end();
		mVectorFrameIter++)
	{
		if (!strcmp (pFrameName, (*mVectorFrameIter)->GetName()))
		{
			*pPos = mPos;
			return 1;
		}
		mPos++;
	 }

	return 0;
}


/*
*************************
Inserts object into the manager
*************************
*/
void IND_AnimationManager::AddToList (IND_Animation *pNewAnimation)
{
	mListAnimations->push_back (pNewAnimation);
}


/*
*************************
Deletes object from the manager
*************************
*/
void IND_AnimationManager::DelFromList (IND_Animation *pAn)
{
	mListAnimations->remove (pAn);
	pAn = 0;
}


/*
*************************
Initialization error message
*************************
*/
void IND_AnimationManager::WriteMessage ()
{
	GetDebugAPI()->Header ("This operation can not be done", 3);
	GetDebugAPI()->DataChar ("", 1);
	GetDebugAPI()->Header ("Invalid Id or not correctly initialized AnimationManager", 2);
}


/*
*************************
Init manager vars
*************************
*/
void IND_AnimationManager::InitVars ()
{
	mListAnimations = new list <IND_Animation*>;
}



/*
*************************
Free manager memory
*************************
*/
void IND_AnimationManager::FreeVars ()
{
	// Releases everything in the anim
	list <IND_Animation*>::iterator mAnimationListIter;
	for (mAnimationListIter  = mListAnimations->begin();
		 mAnimationListIter != mListAnimations->end();
		 mAnimationListIter++)
	{
		// We released the animation(indicates no release pointer (Google Translate)
		Delete ((*mAnimationListIter), 1);

		GetDebugAPI()->Header ("Ok", 6);
	}

	// Clear list
	mListAnimations->clear ();

	// Free list
	Dispose (mListAnimations);
}

