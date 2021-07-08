/*****************************************************************************************
 * File: IND_Entity2dManager.cpp
 * Desc: Manager de entidades. Una entidad contiene el un objeto gráfico y gestiona
 * sus atributos.
 *****************************************************************************************/

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

// ----- Includes -----

#include <CIndieLib.h>
#include <tinyxml/tinyxml.h>
#include <IND_Entity2dManager.h>


// For sorting the vector
bool ZisLess (IND_Entity2d *pLhs, IND_Entity2d *pRhs)
{
	if (pLhs->GetPosZ() < pRhs->GetPosZ())
		return 1;
	else
		return 0;
}


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pRender					Pointer to the manager IND_Render initialized correctly

\b Operation:

This function returns 1(true) if the manager is initialized.
It must be called before using any method.
*/
bool IND_Entity2dManager::Init (IND_Render *pRender)
{
	End ();
	InitVars ();

	GetDebugAPI()->Header ("Initializing Entity2dManager", 5);
	GetDebugAPI()->Header ("Preparing Entity2dManager", 1);

	mOk = true;

	// Checking IND_Render
	if (pRender->IsOk())
	{
		GetDebugAPI()->Header (" Checking IND_Render", 1);
		mRender = pRender;
	}
	else
	{
		GetDebugAPI()->Header ("IND_Render is not correctly initialized", 2);
		mOk = false;
		return mOk;
	}

	GetDebugAPI()->Header ("Entity2dManager OK", 6);

	return mOk;
}


/*!
\b Operation:

This function frees the manager and all the objects that it contains.
*/
void IND_Entity2dManager::End()
{
	if (mOk)
	{
		GetDebugAPI()->Header ("Finalizing Entity2dManager", 5);
		GetDebugAPI()->Header ("Freeing 2d entities" , 5);
		FreeVars ();
		GetDebugAPI()->Header ("Entities freed", 6);
		GetDebugAPI()->Header ("IND_Entity2dManager finalized", 6);

		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//										Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pNewEntity2d				Pointer to an entity object.

\b Operation:

This function returns 1 (true) if the entity object passed as parameter exists
and is added to the manager successfully.

This method is equivalent to adding the entity to layer number 0.
*/
bool IND_Entity2dManager::Add (IND_Entity2d *pNewEntity2d)
{
	GetDebugAPI()->Header ("Adding 2d entity", 5);
	GetDebugAPI()->Header ("Name:", 3);
	//GetDebugAPI()->DataInt (int(pNewEntity2d), 1);

	if (!mOk)
	{
		WriteMessage ();
		return 0;
	}

	// ----- Puts the entity into the manager -----

	AddToList (0, pNewEntity2d);

	// ----- GetDebugAPI() -----

	GetDebugAPI()->Header("2d entity added", 6);

	return 1;
}


/*!
\b Parameters:

\arg \b pLayer						Number of layers (0 - 63 layers allowed)
\arg \b pNewEntity2d				Pointer to an entity object.

\b Operation:

This function returns 1 (true) if the entity object passed as parameter exists
and is added to the manager successfully in a concrete layer.

Layers are useful for having graphical objects in different levels. For example, you can have a terrain with buildings,
trees, etc; all in the same layer, and to have the score and gui buttons in a different layer that will be blitted always
upon the terrain. In this case, the terrain an all the objects on it could be in Layer[0]. The buttons and score could be
in an higher layer, for example Layer[1].

The z values of the objects (see ::SetPosition()) will be taken in count only between objects of the same layer.

There are 64 different layers allowed (0 - 63).
*/
bool IND_Entity2dManager::Add (int pLayer, IND_Entity2d *pNewEntity2d)
{
	GetDebugAPI()->Header ("Adding 2d entity", 5);
	GetDebugAPI()->Header ("Name:", 3);
	//GetDebugAPI()->DataInt (int(pNewEntity2d), 1);
	
	GetDebugAPI()->Header ("Layer:", 3);
	//GetDebugAPI()->DataInt (int(pLayer), 1);

	if (!mOk)
	{
		WriteMessage ();
		return 0;
	}

	// Only allow NUM_LAYERS
	if (pLayer < 0 || pLayer > NUM_LAYERS - 1) return 0;

	// ----- Puts the entity into the manager -----

	AddToList (pLayer, pNewEntity2d);

	// ----- GetDebugAPI() -----

	GetDebugAPI()->Header("2d entity added", 6);

	return 1;
}


/*!
\b Parameters:

\arg \b pNewEntity2d				Pointer to an entity object.

\b Operation:

This function returns 1(true) if the entity object passed as parameter exists
and is deleted from the manager successfully.
*/
bool IND_Entity2dManager::Delete (IND_Entity2d *pEn)
{
	GetDebugAPI()->Header ("Freeing 2d entity", 5);
	GetDebugAPI()->Header ("Name:", 3);
	//GetDebugAPI()->DataInt (int(pEn), 1);

	if (!mOk || !pEn)
	{
		WriteMessage ();
		return 0;
	}

	// Search object in all the layers
	for (int i = 0; i < NUM_LAYERS; i++)
	{
		bool mIs = 0;
		vector <IND_Entity2d*>::iterator mListIter;
		for (mListIter  = mListEntities2d[i]->begin();
			 mListIter  != mListEntities2d[i]->end();
			 mListIter++)
		{
			if ((*mListIter) == pEn)
			{
				mIs = 1;
				break;
			}
		}

		// Found
		if (mIs)
		{
			// ----- Delete object from list -----

			// Quit from list
			mListEntities2d[i]->erase (mListIter);

			GetDebugAPI()->Header ("Ok", 6);

			return 1;
		}
	}

	GetDebugAPI()->Header ("Entity not found", 6);
	return 0;
}


/*!
\b Operation:

This function renders (draws on the screen) all the entities of the manager.

Using this method is equivalent to rendering the entities of layer number 0.
*/
#ifdef BUILD_REPLAY
static bool m_bReplay=false;
static std::list < ReplayFrame > m_lReplayFrames;
static std::list <ReplayFrame>::iterator m_iReplayIndex;
static IND_Entity2d mTextTime;
#endif
INLINE
void IND_Entity2dManager::RenderEntities2d ()
{
#ifdef BUILD_REPLAY
	static IND_Font mFontBig;
	if(CIndieLib::Instance()->Input->OnKeyPress (KEY_r))
	{
		m_bReplay=!m_bReplay;
		static bool init=true;
		if(m_bReplay) 
		{
			if(init)
			{
				init=false;
				if (!CIndieLib::Instance()->FontManager->Add (&mFontBig, "..\\resources\\font_big.png", "..\\resources\\font_big.xml", IND_ALPHA, IND_32)) return;
	Add (NUM_LAYERS-1,&mTextTime);
	mTextTime.SetFont (&mFontBig);
	mTextTime.SetLineSpacing		(18);
	mTextTime.SetCharSpacing		(-8);
	mTextTime.SetAlign				(IND_LEFT);
	mTextTime.SetPosition			(280, 20, 1);
			}
			m_iReplayIndex = m_lReplayFrames.begin();
			for(unsigned int i=0;i<NUM_LAYERS;i++)	
				mListEntities2d0[i]=mListEntities2d[i];
		}
		else 
		{
			m_iReplayIndex = m_lReplayFrames.end();
			for(unsigned int i=0;i<NUM_LAYERS;i++)	
				mListEntities2d[i]=mListEntities2d0[i];
		}
	}
#endif
	int pLayer=0;
	RenderEntities2d (pLayer);
	//RenderEntities2d (NUM_LAYERS-1);
}


/*!
\b Operation:

This function renders (draws on the screen) all the entities of the manager of a concrete layer.
*/
#ifdef BUILD_REPLAY
extern IND_Viewport g_Viewport2d;
extern IND_Camera2d g_Camera2d;
#endif
INLINE 
void IND_Entity2dManager::RenderEntities2d (int pLayer)
{

	if (!mOk || mListEntities2d[pLayer]->empty()) return;

#if 0
	char buf[1024];
	stx_snprintf(buf, 1024, "Layer %d",pLayer);
	if(pLayer<NUM_LAYERS-1)
{
	if(m_bReplay&&(m_iReplayIndex != m_lReplayFrames.end()))
	{
		char buf[1024];
		buf[0]='\0';
		static unsigned int iii=0;
		stx_snprintf(buf, 1024, "Replay %d/%d",iii++,m_lReplayFrames.size());
		mTextTime.SetText(buf);		

		for(unsigned int i=0;i<NUM_LAYERS;i++)	
		mListEntities2d[i]=&(*m_iReplayIndex).mEntities2d[i];
		
		//CIndieLib::Instance()->Render->BeginScene ();
		CIndieLib::Instance()->Render->ClearViewPort (60, 60, 60);
		CIndieLib::Instance()->Render->SetViewPort2d ((*m_iReplayIndex).mViewport2d.X, (*m_iReplayIndex).mViewport2d.Y, (*m_iReplayIndex).mViewport2d.Width, (*m_iReplayIndex).mViewport2d.Height);
		CIndieLib::Instance()->Render->SetCamera2d (&(*m_iReplayIndex).mCamera2d);
		//CIndieLib::Instance()->Entity2dManager->RenderEntities2d ();
		//CIndieLib::Instance()->Render->EndScene();
		
		m_iReplayIndex++;
		if (m_iReplayIndex == m_lReplayFrames.end())
		{
#if 0
			m_iReplayIndex = m_lReplayFrames.begin();
			iii=0;
#else
			for(unsigned int i=0;i<NUM_LAYERS;i++)	
				mListEntities2d[i]=mListEntities2d0[i];
			m_bReplay=false;
#endif
		}

		//return;
	}
	else
	{
		if(
		m_lReplayFrames.size()>=10
		)
		m_lReplayFrames.pop_front();
		ReplayFrame f;
		for(unsigned int i=0;i<NUM_LAYERS;i++)
		f.mEntities2d[i]=*mListEntities2d[i];
		f.mViewport2d=g_Viewport2d;
		f.mCamera2d=g_Camera2d;
		m_lReplayFrames.push_back(f);
	}
}
#endif

//LOG_PRINT("mListEntities2d[%d]->size()=%d\n",pLayer,mListEntities2d[pLayer]->size());
	// Sort the list by z value ONLY if the z value of an entity has changed
	// TODO: How to know if an entity has changed z-value from here int order to avoid sorting?
	sort (mListEntities2d[pLayer]->begin(), mListEntities2d[pLayer]->end(), ZisLess);

	// Set culling region
	mRender->CalculeFrustumPlanes ();

	// Iterates the list
	vector <IND_Entity2d*>::iterator mIter;
/*
	LOG_PRINT("pLayer=%d\n",pLayer);
	LOG_PRINT("mListEntities2d[%d]=%d\n",pLayer,mListEntities2d[pLayer]);
	LOG_PRINT("size=%d\n",mListEntities2d[pLayer]->size());
*/
	for (mIter  = mListEntities2d[pLayer]->begin();
		 mIter != mListEntities2d[pLayer]->end();
		 mIter++)
	{
;
		// Only render it if "show" flag is true
		if ((*mIter)->mShow)
		{
;
			// If it has an animation or a surface assigned
			if ((*mIter)->mSu || (*mIter)->mAn)
			{
;
				// Set transformations ONLY if the entity space attributes has been modified
				if ((*mIter)->mUpdateTransFlag)
				{
;
					(*mIter)->mUpdateTransFlag = 0;

					int mWidthTemp, mHeightTemp;

					// ---- We obtain the width and height of the animatior or the surface -----

					if ((*mIter)->mSu)
					{

						mWidthTemp  = (*mIter)->mSu->GetWidth  ();
						mHeightTemp = (*mIter)->mSu->GetHeight ();
					}
					else
					{

						mWidthTemp  = (*mIter)->mAn->GetActualSurface((*mIter)->GetSequence())->GetWidth();
						mHeightTemp = (*mIter)->mAn->GetActualSurface((*mIter)->GetSequence())->GetHeight();
					}

					// ----- Transformations -----

					mRender->SetTransform2d	((int) (*mIter)->mX,
											(int) (*mIter)->mY,
											(*mIter)->mAngleX,
											(*mIter)->mAngleY,
											(*mIter)->mAngleZ,
											(*mIter)->mScaleX,
											(*mIter)->mScaleY,
											(*mIter)->mAxisCalX,
											(*mIter)->mAxisCalY,
											(*mIter)->mMirrorX,
											(*mIter)->mMirrorY,
											mWidthTemp,
											mHeightTemp,
											&(*mIter)->mMat);

				}
				else
				{

					D3DXFROMWINEMATRIX mMat;
					mRender->GetD3DMatrix ((*mIter)->mMat, &mMat);
					mRender->GetDevice()->SetTransform (TS_WORLD, mMat);

				}

				// ----- Color attributes -----


				int ab=mRender->SetRainbow2d	((*mIter)->mType,
										(*mIter)->mCull,
										(*mIter)->mMirrorX,
										(*mIter)->mMirrorY,
										(*mIter)->mFilter,
										(*mIter)->mR,
										(*mIter)->mG,
										(*mIter)->mB,
										(*mIter)->mA,
										(*mIter)->mFadeR,
										(*mIter)->mFadeG,
										(*mIter)->mFadeB,
										(*mIter)->mFadeA,
										(*mIter)->mSo,
										(*mIter)->mDs);

				// ----- Surface blitting -----

				if ((*mIter)->mSu)
				{
					// Surface region specified
					if ( ( (*mIter)->mRegionWidth > 0) && ((*mIter)->mRegionHeight > 0))
					{
						// X or Y wrapping
						if ((*mIter)->mWrap)
						{
							mRender->BlitWrapSurface	(/*ab,*/(*mIter)->mSu,
														(*mIter)->mRegionWidth,
														(*mIter)->mRegionHeight,
														(*mIter)->mUDisplace,
														(*mIter)->mVDisplace);
						}
						// No wrapping
						else
						{
							mRender->BlitRegionSurface	(/*ab,*/(*mIter)->mSu,
														(*mIter)->mOffX,
														(*mIter)->mOffY,
														(*mIter)->mRegionWidth,
														(*mIter)->mRegionHeight);
						}
					}
					// Blits all the surface
					else
{

						mRender->BlitSurface	(/*ab,*/(*mIter)->mSu);

}
				}

				// ----- Animation blitting -----

				else
				{
					// Animation is looping
					if ((*mIter)->mNumReplays == -1)
					{
						// Blits the animation, returns -1 when finishes
						if (mRender->BlitAnimation	(/*ab,*/(*mIter)->mAn,
													(*mIter)->mSequence,
													(*mIter)->mOffX,
													(*mIter)->mOffY,
													(*mIter)->mRegionWidth,
													(*mIter)->mRegionHeight,
													(*mIter)->mWrap,
													(*mIter)->mUDisplace,
													(*mIter)->mVDisplace) == -1)
						{
							// Reset the animation
							(*mIter)->mAn->SetActualFramePos ((*mIter)->mSequence, 0);
						}
					}
					else
					// If there is a stablished number or replays
					{
						// Blits the animation, returns -1 when finishes
						if (mRender->BlitAnimation	(/*ab,*/(*mIter)->mAn,
													(*mIter)->mSequence,
													(*mIter)->mOffX,
													(*mIter)->mOffY,
													(*mIter)->mRegionWidth,
													(*mIter)->mRegionHeight,
													(*mIter)->mWrap,
													(*mIter)->mUDisplace,
													(*mIter)->mVDisplace) == -1)
						{
							// There are replays
							if ((*mIter)->mNumReplays > 0)
							{
								// Reset animation
								(*mIter)->mAn->SetActualFramePos ((*mIter)->mSequence, 0);

								// Decrease the number of replays
								(*mIter)->mNumReplays--;
							}
							// There are no replays
							else
							{
								// Blits the last frame
								(*mIter)->mAn->SetActualFramePos ((*mIter)->mSequence, (*mIter)->mAn->GetNumFrames ((*mIter)->mSequence) - 1);
							}
						}
					}
				}
			}
			else
			{
				int ab=-1;//???

			// If it has a 2d primitive assigned
			if ((*mIter)->mPri2d)
			{
				switch ((*mIter)->mPri2d)
				{
					// Pixel
					case IND_PIXEL:
					{

						mRender->BlitPixel		(/*ab,*/(int) (*mIter)->mX,
												(int) (*mIter)->mY,
												(*mIter)->mR,
												(*mIter)->mG,
												(*mIter)->mB,
												(*mIter)->mA);
						break;
					}

					// Regular polygon
					case IND_REGULAR_POLY:
					{

						mRender->BlitRegularPoly(/*ab,*/(int) (*mIter)->mX,
												(int) (*mIter)->mY,
												(*mIter)->mRadius,
												(*mIter)->mNumSides,
												(*mIter)->mPolyAngle,
												(*mIter)->mR,
												(*mIter)->mG,
												(*mIter)->mB,
												(*mIter)->mA);
						break;
					}

					// Rectangle
					case IND_RECTANGLE:
					{

						mRender->BlitRectangle	(/*ab,*/(*mIter)->mX1,
												(*mIter)->mY1,
												(*mIter)->mX2,
												(*mIter)->mY2,
												(*mIter)->mR,
												(*mIter)->mG,
												(*mIter)->mB,
												(*mIter)->mA);
						break;
					}

					// Fill rectangle
					case IND_FILL_RECTANGLE:
					{
						mRender->BlitFillRectangle	(/*ab,*/(*mIter)->mX1,
													(*mIter)->mY1,
													(*mIter)->mX2,
													(*mIter)->mY2,
													(*mIter)->mR,
													(*mIter)->mG,
													(*mIter)->mB,
													(*mIter)->mA);
						break;
					}

					// Poly2d
					case IND_POLY2D:
					{

						mRender->BlitPoly2d		(/*ab,*/(*mIter)->mPolyPoints,
												(*mIter)->mNumLines,
												(*mIter)->mR,
												(*mIter)->mG,
												(*mIter)->mB,
												(*mIter)->mA);
						break;
					}

					// Line
					case IND_LINE:
					{

						mRender->BlitLine		(/*ab,*/(*mIter)->mX1,
												(*mIter)->mY1,
												(*mIter)->mX2,
												(*mIter)->mY2,
												(*mIter)->mR,
												(*mIter)->mG,
												(*mIter)->mB,
												(*mIter)->mA);
						break;
					}
				}
			}
			else
			// If it has a font assigned
			if ((*mIter)->mFont)
			{
				mRender->BlitText	(/*ab,*/(*mIter)->mFont,
									(char*)(*mIter)->mText,
									(int) (*mIter)->mX,
									(int) (*mIter)->mY,
									(*mIter)->mCharSpacing,
									(*mIter)->mLineSpacing,
									(*mIter)->mR,
									(*mIter)->mG,
									(*mIter)->mB,
									(*mIter)->mA,
									(*mIter)->mFadeR,
									(*mIter)->mFadeG,
									(*mIter)->mFadeB,
									(*mIter)->mFadeA,
									(*mIter)->mFilter,
									(*mIter)->mSo,
									(*mIter)->mDs,
									(*mIter)->mAlign);
			}
			;
			}
			;
		}
		;
	}
	;
}


/*!
\b Operation:

This function renders (blits on the screen) all the collision areas of the entities. It's good to use this method
in order to check that our collision areas are accurate.

This method is equivalent to rendering the collision areas of layer number 0.
*/
INLINE
void	IND_Entity2dManager::RenderCollisionAreas	(unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA)
{
	RenderCollisionAreas (0, pR, pG, pB, pA);
}


/*!
\b Operation:

This function renders (blits on the screen) all the collision areas of the entities of a certain layer. It's good to use
this method in order to check that our collision areas are accurate.
*/
INLINE
void	IND_Entity2dManager::RenderCollisionAreas	(int pLayer, unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA)
{
	if (!mOk || mListEntities2d[pLayer]->empty()) return;

	// Iterate the list
	vector <IND_Entity2d*>::iterator mIter;
	for (mIter  = mListEntities2d[pLayer]->begin();
		 mIter != mListEntities2d[pLayer]->end();
		 mIter++)
	{
		// If it has an animation or a surface assigned
		if ((*mIter)->mSu || (*mIter)->mAn)
		{
			if ((*mIter)->IsShowCollisionAreas())
			{
				list <BOUNDING_COLLISION*> *mBoundingListToRender;

				// Bounding list of a surface
				if ((*mIter)->mSu)
				{
					mBoundingListToRender = (*mIter)->mListBoundingCollision;
				}

				// Bounding list of a frame in an animation
				if ((*mIter)->mAn)
				{
					mBoundingListToRender = (*(*mIter)->mAn->GetVectorFrames()) [(*mIter)->mAn->GetActualFramePos((*mIter)->GetSequence())]->GetListBoundingCollision();
				}

				if (!mBoundingListToRender->empty())
				{
					int ab=-1;
					list <BOUNDING_COLLISION*>::iterator mListIter;
					for (mListIter  = mBoundingListToRender->begin();
						 mListIter != mBoundingListToRender->end();
						 mListIter++)
					{
						switch ((*mListIter)->mType)
						{
							// Triangle
							case 0:
							{
								mRender->BlitCollisionLine (/*ab,*/(*mListIter)->mAx, (*mListIter)->mAy, (*mListIter)->mBx, (*mListIter)->mBy, pR, pG, pB, pA, (*mIter)->mMat);
								mRender->BlitCollisionLine (/*ab,*/(*mListIter)->mBx, (*mListIter)->mBy, (*mListIter)->mCx, (*mListIter)->mCy, pR, pG, pB, pA, (*mIter)->mMat);
								mRender->BlitCollisionLine (/*ab,*/(*mListIter)->mCx, (*mListIter)->mCy, (*mListIter)->mAx, (*mListIter)->mAy, pR, pG, pB, pA, (*mIter)->mMat);
								break;
							}

							// Circle
							case 1:
							{
								mRender->BlitCollisionCircle (/*ab,*/(*mListIter)->mPosX,  (*mListIter)->mPosY, (*mListIter)->mRadius, (*mIter)->mScaleX, pR, pG, pB, pA, (*mIter)->mMat);
								break;
							}
						}
					}
				}
			}
		}
	}
}


/*!
\b Operation:

This function renders (blits on the screen) all the grid areas of the entities.

This method is equivalent to rendering the grid areas of layer number 0.
*/
INLINE 
void	IND_Entity2dManager::RenderGridAreas	(unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA)
{
	RenderGridAreas (0, pR, pG, pB, pA);
}


/*!
\b Operation:

This function renders (blits on the screen) all the grid areas of the entities of a certain layer.
*/
INLINE
void	IND_Entity2dManager::RenderGridAreas	(int pLayer, unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA)
{
	if (!mOk || mListEntities2d[pLayer]->empty()) return;

	// Iterate the list
	vector <IND_Entity2d*>::iterator mIter;
	for (mIter  = mListEntities2d[pLayer]->begin();
		 mIter != mListEntities2d[pLayer]->end();
		 mIter++)
	{
		// If it has an animation or a surface assigned
		if ((*mIter)->mSu || (*mIter)->mAn)
		{
			if ((*mIter)->IsShowGridAreas())
			{
				IND_Surface *mSu;

				// Surface
				if ((*mIter)->mSu)
				{
					mSu = (*mIter)->mSu;
				}

				// Surface of current frame
				if ((*mIter)->mAn)
				{
					mSu = (*mIter)->mAn->GetActualSurface((*mIter)->GetSequence());
				}

				int mWidthTemp  = mSu->GetWidth();
				int mHeightTemp = mSu->GetHeight();

				mRender->SetTransform2d	((int) (*mIter)->mX,
										(int) (*mIter)->mY,
										(*mIter)->mAngleX,
										(*mIter)->mAngleY,
										(*mIter)->mAngleZ,
										(*mIter)->mScaleX,
										(*mIter)->mScaleY,
										(*mIter)->mAxisCalX,
										(*mIter)->mAxisCalY,
										(*mIter)->mMirrorX,
										(*mIter)->mMirrorY,
										mWidthTemp,
										mHeightTemp,
										&(*mIter)->mMat);

				mRender->BlitGrid (mSu, pR, pG, pB, pA);
			}
		}
	}
}



/*!
\b Parameters:

\arg \b pEn1				Pointer to an entity object
\arg \b pId1				Id of a group of collison areas. Use "*" for checking all the groups.
\arg \b pEn2				Pointer to an entity object
\arg \b pId2				Id of a group of collison areas. Use "*" for checking all the groups.

\b Operation:

This function checks if two entities have collided in any of their bounding areas.
Returns 1 if there is a collision. The id is a string of characters that identifies a collision group.
For creating the bounding areas and defining collision groups you can use the following methods:
- IND_Entity2d::SetBoundingAreas()
- IND_Entity2d::SetBoundingCircle()
- IND_Entity2d::SetBoundingTriangle()
- IND_Entity2d::SetBoundingRectangle()

Important: This function only works with surfaces or animation entities.

Rotations allowed of the object: only in the z axis.
Scaling allowed of the object: yes.

Use IND_Entity2d::ShowCollisionAreas() method in order to show or hide the collision areas of an entity.
In order to render all the collision areas you should use the
IND_Entity2dManager::RenderCollisionAreas() method.
*/
INLINE
bool IND_Entity2dManager::IsCollision (IND_Entity2d *pEn1, const char *pId1, IND_Entity2d *pEn2, const char *pId2)
{
/*
//LOG_PRINT("pId1=%s\n",pId1);
//LOG_PRINT("pId2=%s\n",pId2);
STX_STACK_TRACE
stx_exit(0);
*/
//
	if (!pEn1->mSu && !pEn1->mAn) return 0;
	if (!pEn2->mSu && !pEn2->mAn) return 0;

	// Apply transformation if the entities dont have a world matrix
	if (pEn1->IsNullMatrix() || pEn2->IsNullMatrix())
	{
		int mWidthTemp, mHeightTemp;
		if (pEn1->mSu)
		{
			mWidthTemp  = pEn1->mSu->GetWidth  ();
			mHeightTemp = pEn1->mSu->GetHeight ();
		}
		else
		{
			mWidthTemp  = pEn1->mAn->GetActualSurface(pEn1->GetSequence())->GetWidth();
			mHeightTemp = pEn1->mAn->GetActualSurface(pEn1->GetSequence())->GetHeight();
		}

		IND_Matrix mMat1;
		mRender->SetTransform2d	((int) pEn1->mX,
								(int) pEn1->mY,
								pEn1->mAngleX,
								pEn1->mAngleY,
								pEn1->mAngleZ,
								pEn1->mScaleX,
								pEn1->mScaleY,
								pEn1->mAxisCalX,
								pEn1->mAxisCalY,
								pEn1->mMirrorX,
								pEn1->mMirrorY,
								mWidthTemp,
								mHeightTemp,
								&mMat1);
		pEn1->mMat = mMat1;


		if (pEn2->mSu)
		{
			mWidthTemp  = pEn2->mSu->GetWidth  ();
			mHeightTemp = pEn2->mSu->GetHeight ();
		}
		else
		{
			mWidthTemp  = pEn2->mAn->GetActualSurface(pEn2->GetSequence())->GetWidth();
			mHeightTemp = pEn2->mAn->GetActualSurface(pEn2->GetSequence())->GetHeight();
		}

		IND_Matrix mMat2;
		mRender->SetTransform2d	((int) pEn2->mX,
								(int) pEn2->mY,
								pEn2->mAngleX,
								pEn2->mAngleY,
								pEn2->mAngleZ,
								pEn2->mScaleX,
								pEn2->mScaleY,
								pEn2->mAxisCalX,
								pEn2->mAxisCalY,
								pEn2->mMirrorX,
								pEn2->mMirrorY,
								mWidthTemp,
								mHeightTemp,
								&mMat2);
		pEn2->mMat = mMat2;
	}

	list <BOUNDING_COLLISION*> *mBoundingList1;
	list <BOUNDING_COLLISION*> *mBoundingList2;

	// Is a surface
	if (pEn1->mSu)
	{
		mBoundingList1 = pEn1->mListBoundingCollision;
	}
	// Is an animation
	else
	{
		mBoundingList1 = (*(pEn1->mAn->GetVectorFrames())) [pEn1->mAn->GetActualFramePos(pEn1->GetSequence())]->GetListBoundingCollision();
	}

	// Is a surface
	if (pEn2->mSu)
	{
		mBoundingList2 = pEn2->mListBoundingCollision;
	}
	// Is an animation
	else
	{
		mBoundingList2 = (*(pEn2->mAn->GetVectorFrames())) [pEn2->mAn->GetActualFramePos(pEn2->GetSequence())]->GetListBoundingCollision();
	}
//
	if (IsCollision (mBoundingList1, mBoundingList2,
					pId1, pId2,
					pEn1->mMat, pEn2->mMat,
					pEn1->mScaleX, pEn2->mScaleX))
	{
		//STX_STACK_TRACE;
		//stx_exit(0);
		return 1;
	}

	return 0;
}


// --------------------------------------------------------------------------------
//									 Private methods
// --------------------------------------------------------------------------------

/*
*************************
Check the collision between bounding areas
*************************
*/
INLINE bool IND_Entity2dManager::IsCollision	(list <BOUNDING_COLLISION*> *pBoundingList1, list <BOUNDING_COLLISION*> *pBoundingList2,
												const char *pId1, const char *pId2,
												IND_Matrix pMat1, IND_Matrix pMat2,
												float pScale1, float pScale2)
{
	list <BOUNDING_COLLISION*>::iterator i;
	list <BOUNDING_COLLISION*>::iterator j;

	bool mCollision = 0;
//
	int iii=0;
	int jjj=0;		
	// Check all the collisions
	for (i  = pBoundingList1->begin();
		i != pBoundingList1->end();
		i++)
	{
//
//LOG_PRINT("%d\n",iii++);
		for (j  = pBoundingList2->begin();
			j != pBoundingList2->end();
			j++)
		{
//LOG_PRINT("%d\n",jjj++);
//
//LOG_PRINT("pId1=%s\n(*i)->mId=%s\npId2=%s\n(*j)->mId=%s\n",pId1, (*i)->mId, pId2, (*j)->mId);
			// Check only if the group is correct
			if ( (!strcmp (pId1, (*i)->mId.c_str())	&& !strcmp (pId2, (*j)->mId.c_str()))	||	// "group1", "group2"
				 (!strcmp (pId1, "*")		&& !strcmp ((*j)->mId.c_str(), pId2))	||	// "*", "group"
				 (!strcmp ((*i)->mId.c_str(), pId1)	&& !strcmp (pId2 , "*"))		||	// "group", "*"
				 (!strcmp (pId1, "*"))		&& !strcmp (pId2 , "*"))			// "*", "*"
			{
				//
				// Triangle to triangle
				if ((*i)->mType == 0 && (*j)->mType == 0)
				{
					if (mRender->IsTriangleToTriangleCollision ((*i), pMat1, (*j), pMat2))
						mCollision = 1;
				}

				// Circle to triangle
				if ((*i)->mType == 1 && (*j)->mType == 0)
				{
					if (mRender->IsCircleToTriangleCollision ((*i), pMat1, pScale1, (*j), pMat2))
						mCollision = 1;
				}

				// Triangle to circle
				if ((*i)->mType == 0 && (*j)->mType == 1)
				{
					if (mRender->IsCircleToTriangleCollision ((*j), pMat2, pScale2, (*i), pMat1))
						mCollision = 1;
				}

				// Circle to circle
				if ((*i)->mType == 1 && (*j)->mType == 1)
				{
					if (mRender->IsCircleToCircleCollision ((*i), pMat1, pScale1, (*j), pMat2, pScale2))
						mCollision = 1;
				}
			}
		}
	}/*
	if(mCollision)
	{
	STX_STACK_TRACE;
	stx_exit(0);
	}*/
	return mCollision;
}


/*
*************************
Checks if the matrix has all its member equal to zero
*************************
*/
bool IND_Entity2dManager::IsNullMatrix (IND_Matrix pMat)
{
	if (!pMat.m[0][0] && !pMat.m[0][1]  && !pMat.m[0][2] && !pMat.m[0][3] &&
		!pMat.m[1][0] && !pMat.m[1][1]  && !pMat.m[1][2] && !pMat.m[1][3] &&
		!pMat.m[2][0] && !pMat.m[2][1]  && !pMat.m[2][2] && !pMat.m[2][3] &&
		!pMat.m[3][0] && !pMat.m[3][1]  && !pMat.m[3][2] && !pMat.m[3][3])
		return 1;

	return 0;
}


/*
*************************
Inserts object into the manager in a certain layer
*************************
*/
void IND_Entity2dManager::AddToList (int pLayer, IND_Entity2d *pNewEntity2d)
{
	mListEntities2d[pLayer]->push_back (pNewEntity2d);
}


/*
*************************
Initialization error message
*************************
*/
void IND_Entity2dManager::WriteMessage ()
{
	GetDebugAPI()->Header ("This operation can not be done", 3);
	GetDebugAPI()->DataChar ("", 1);
	GetDebugAPI()->Header ("Invalid Id or IND_Entity2dManager not correctly initialized", 2);
}


/*
*************************
Init manager vars
*************************
*/
void IND_Entity2dManager::InitVars ()
{
	for (int i = 0; i < NUM_LAYERS; i++)
		mListEntities2d [i] = new vector <IND_Entity2d*>;
}


/*
*************************
Free manager memory
*************************
*/
void IND_Entity2dManager::FreeVars ()
{
	// Deletes all the manager entities
	vector <IND_Entity2d*>::iterator mEntityListIter;

	for (int i = 0; i < NUM_LAYERS; i++)
	{
		for (mEntityListIter  = mListEntities2d[i]->begin();
			 mEntityListIter != mListEntities2d[i]->end();
			 mEntityListIter++)
		{
			GetDebugAPI()->Header	("Freeing 2d entity:", 3);

			// Delete all the bounding areas
			(*mEntityListIter)->DeleteBoundingAreas	("*");

			//GetDebugAPI()->DataInt (int( (*mEntityListIter)), 1);
		}

		// Clear list
		mListEntities2d[i]->clear ();

		// Free list
		Dispose (mListEntities2d[i]);
	}
}

