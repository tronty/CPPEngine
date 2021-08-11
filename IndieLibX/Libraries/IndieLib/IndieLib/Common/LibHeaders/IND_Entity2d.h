/*****************************************************************************************
/* File: IND_Entity2d.h
/* Desc: Entity 2d object
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

#ifndef _IND_ENTITY2D_
#define _IND_ENTITY2D_

// ----- Includes -----

#include "IND_SurfaceManager.h"


// --------------------------------------------------------------------------------
//									 IND_Entity2d
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Entity2d IND_Entity2d
\ingroup EntityManagers
2d entity class managed by IND_Entity2dManager for drawing graphical objects into the screen and manipulating their attributes, click in ::IND_Entity2d to see all the methods of this class.
*/
/*@{*/	

/*!
\b IND_Entity2d is an 2d entity object of the class ::IND_Entity2dManager. This class, together with ::IND_Entity3d (object
of ::IND_Entity3dManager), are the most important classes of IndieLib.

An entity is an object that can have a graphical object associated to it. Using the methods of this class you will be able to change
the attributes of the graphical object that it contains.

For example, in a game, a bullet can be a ::IND_Entity2d that has a ::IND_Surface associated to it. You can have as many bullets
(::IND_Entity2d objects) in the screen as you want, all of them sharing the same IND_Surface (the sprite). In system memory 
you will have only one 3d mesh loaded, the rest will be instances. The cool thing is that you can
change the attributes of each of these different ::IND_Entity2d objects. For example, you can change the size, rotation, color or transparency
of the bullets. So, having only one sprite, you can have lot of different bullets in the screen, with different sizes, colors, positions, etc.

IND_Entity2d can have so many different graphical objects associated to it:
- Surfaces (Sprites, scrolls, images, etc)
- Animations
- Fonts
- Primitives (lines, rectangles, circles, polys, etc)
*/
class DLL_EXP IND_Entity2d
{
public:

	// ----- Init -----

	IND_Entity2d		();
	~IND_Entity2d		();

	// ----- Public sets ------

	/** @name Graphical objects assignation to the 2d entity
	* 
	*/
	//@{
	void	SetSurface			(IND_Surface *pNewSurface);	
	void	SetAnimation		(IND_Animation *pNewAnimation);
	void	SetPrimitive2d		(IND_Primitive2d pPri2d);
	void	SetFont				(IND_Font *pFont);
	//@}

	/** @name Showing
	*   
	*/
	//@{
	void	SetShow				(bool pShow);
	//@}

	/** @name Animations
	*   
	*/
	//@{
	void	SetSequence			(int pSequence);
	void	SetNumReplays		(int pNumReplays);	
	//@}

	/** @name Primitives
	*   
	*/
	//@{
	void	SetLine				(int pX1, int pY1, int pX2, int pY2);	
	void	SetRectangle		(int pX1, int pY1, int pX2, int pY2);	
	void	SetRadius			(int pRadius);		
	void	SetNumSides			(int pNumSides);		
	void	SetPolyAngle		(float pPolyAngle);			
	void	SetPolyPoints		(IND_Point *pPolyPoints);	
	void	SetNumLines			(int pNumLines);
	//@}

	/** @name Fonts
	*   
	*/
	//@{	
	void	SetAlign			(IND_Align pAlign);		
	void	SetCharSpacing		(int pCharSpacing);
	void	SetLineSpacing		(int pLineSpacing);
	void	SetText				(const char *pText);
	//@}


	/** @name Space transformations (Some of these methods dont affect the fonts or the primitives)
	*   
	*/
	//@{
	void	SetPosition			(float pX, float pY, int pZ);													
	void	SetAngleXYZ			(float pAnX, float pAnY, float pAnZ);				
	void	SetScale			(float pSx, float pSy);									
	void	SetMirrorX			(bool pMx);						
	void	SetMirrorY			(bool pMy);									
	void	SetFilter			(IND_Filter pF);						
	bool	SetHotSpot			(float pX, float pY);	
	bool	SetRegion			(int pOffX, int pOffY, int pRegionWidth, int pRegionHeight);	
	bool	ToggleWrap			(bool pWrap);	
	void	SetWrapDisplacement	(float pUDisplace, float pVDisplace);
	//@}


	/** @name Color transformations, blending and back face culling (Some of these methods dont affect the fonts or the primitives)
	*   
	*/
	//@{
	void	SetBackCull			(bool pCull);	
	void	SetTint				(unsigned char pR, unsigned char pG, unsigned char pB);		
	void	SetTransparency		(unsigned char pA);		
	void	SetFade				(unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA);
	void	SetBlendSource		(IND_BlendingType pSo);
	void	SetBlendDest		(IND_BlendingType pDs);
	//@}	


	/** @name Collisions
	*   
	*/
	//@{
	bool SetBoundingAreas		(const char *pFile);
	bool SetBoundingTriangle	(const char *pId, int pAx, int pAy, int pBx, int pBy, int pCx, int pCy);		
	bool SetBoundingCircle		(const char *pId, int pOffsetX, int pOffsetY, int pRadius);		
	bool SetBoundingRectangle	(const char *pId, int pOffsetX, int pOffsetY, int pWidth, int pHeight);
	bool DeleteBoundingAreas	(const char *pId);
	void ShowCollisionAreas		(bool pShowCollisionAreas);
	void ShowGridAreas			(bool pShowGridAreas);
	//@}


	// ----- Public Gets ------

	/** @name Gets
	* 
	*/
	//@{

	//! If the entity has a surface assigned, it returns a pointer to this surface.
	IND_Surface		*GetSurface			()		{ return mSu;					}
	//! If the entity has a animation assigned, it returns a pointer to this animation.	
	IND_Animation	*GetAnimation		()		{ return mAn;					}
	//! Returns true if the entity is being showed, false if not
	bool	IsShow						()		{ return mShow;					}
	//! Returns the sequence number that has been assigned to the animation.
	int		GetSequence					()		{ return mSequence;				}
	//! Returns the number of repetitions the animation has to do. If this value is equal or less than zero, it indicates that the amination is looping.
	int		GetNumReplays				()		{ return mNumReplays;			}
	//! Returns X position of the entity.
	float	GetPosX						()		{ return mX; 					}
	//! Returns Y position of the entity.
	float	GetPosY						()		{ return mY; 					}
	//! Returns Z position of the entity.
	int		GetPosZ						()		{ return mZ;					}
	//! Returns the angle in the X axis of the entity.	
	float	GetAngleX					()		{ return mAngleX;				}
	//! Returns the angle in the Y axis of the entity.		
	float	GetAngleY					()		{ return mAngleY;				}
	//! Returns the angle in the Z axis of the entity.		
	float	GetAngleZ					()		{ return mAngleZ;				}	
	//! Returns the X scale of the entity.		
	float	GetScaleX					()		{ return mScaleX; 				}
	//! Returns the Y scale of the entity.	
	float	GetScaleY					()		{ return mScaleY; 				}
	//! Indicate if the entity is making backface culling 0 = No, 1 = Yes.		
	bool	GetBackCull					()		{ return mCull;					}
	//!Indicate if the entity makes horizontal mirror. 0 = No, 1 = Yes.		
	bool	GetMirrorX					()		{ return mMirrorX;				}
	//! Indicate if the entity makes vertical mirror. 0 = No, 1 = Yes.	
	bool	GetMirrorY					()		{ return mMirrorY;				}									
	//! Returns the type of filter ::IND_Filter which uses the graphical object assignated to the entity.	
	IND_Filter	GetFilter				()		{ return mFilter;				}
	//! Returns the type ::IND_Type which uses the graphical object assignated to the entity.		
	IND_Type	GetType					()		{ return mType;					}
	//! Returns the tint level in the R (red) channel of the entity.	
	unsigned char	GetTintR						()		{ return mR;					}
	//! Returns the tint level in the G (green) channel of the entity.	
	unsigned char	GetTintG						()		{ return mG;					}
	//! Returns the tint level in the B (blue) channel of the entity.	
	unsigned char	GetTintB						()		{ return mB;					}
	//! Returns the transparency level of the entity
	unsigned char	GetTransparency						()		{ return mA;				}
	//! Returns the fade level in R (red) channel of the entity.	
	unsigned char	GetFadeR					()		{ return mFadeR;				}
	//! Returns the fade level in G (green) channel of the entity.	
	unsigned char	GetFadeG					()		{ return mFadeG;				}
	//! Returns the fade level in B (blue) channel of the entity.	
	unsigned char	GetFadeB					()		{ return mFadeB;				}
	//! Returns the fade level of the entity.
	unsigned char	GetFadeA					()		{ return mFadeA;				}
	//! Returns the blending type ::IND_BlendingType for the source
	IND_BlendingType	GetBlendSource	()		{ return mSo;					}
	//! Returns the blending type ::IND_BlendingType for the destination	
	IND_BlendingType	GetDestSource	()		{ return mDs;					}
	//! Returns the X HotSpot of the entity.	
	float	GetHotSpotX					()		{ return mHotSpotX;				}
	//! Returns the Y HotSpot of the entity.	
	float	GetHotSpotY					()		{ return mHotSpotY;				}
	//! Returns the X position of the upper left corner of the region assigned to the entity by IND_Entity2d::SetRegion()	
	int		GetRegionX					()		{ return mOffX;					}
	//! Returns the Y position of the upper left corner of the region assigned to the entity by IND_Entity2d::SetRegion()	
	int		GetRegionY					()		{ return mOffY;					}
	//! Returns the width of the region assignated to the entity by IND_Entity2d::SetRegion()	
	int		GetRegionWidth				()		{ return mRegionWidth;			}
	//! Returns the height of the region assignated to the entity by IND_Entity2d::SetRegion()	
	int		GetRegionHeight				()		{ return mRegionHeight;			}
	//! Returns the position 1 (true) if the image is being repeated in the X,Y axis	
	bool	IfWrap					()			{ return mWrap;				}
	//! Returns the position in x from the first point of a primitive line.
	int		GetLineX1					()		{ return mX1;}
	//! Returns the position in y from the first point of a primitive line.
	int		GetLineY1					()		{ return mY1;					}
	//! Returns the position in x from the second point of a primitive line.	
	int		GetLineX2					()		{ return mX2;					}
	//! Returns the position in y from the second point of a primitive line.	
	int		GetLineY2					()		{ return mY2;					}
	//! Returns the radius of the primitives  ::IND_REGULAR_POLY		
	int		GetRadius					()		{ return mRadius;				}
	//! Returns the numbers of sides of the primitive ::IND_REGULAR_POLY 	
	int		GetNumSides					()		{ return mNumSides;				}
	//! Returns the primitive angle ::IND_REGULAR_POLY		
	float	GetPolyAngle				()		{ return mPolyAngle;			}
	//!  Returns the  points array which draw the primitive ::IND_POLY2D			
	IND_Point *GetPolyPoints			()		{ return mPolyPoints;			}
	//! Returns the number of lines which draw the primitive ::IND_POLY2D	
	int		GetNumLines					()		{ return mNumLines;				}
	//! Returns the align ::IND_Align of the text 
	IND_Align GetAlign					()		{ return mAlign;				}
	//! Returns the additional space between characters when using IND_Font	
	int		GetCharSpacing				()		{ return mCharSpacing;			}
	//!  Returns the line spacing between lines when using IND_Font	
	int		GetLineSpacing				()		{ return mLineSpacing;			}
	//! Returns the text string of the entity		
	char	*GetText					()		{ return mText;					}
	//! Returs true if the collision areas are being shown		
	bool	IsShowCollisionAreas		()		{ return mShowCollisionAreas;	}
	//! Returs true if the grid areas are being shown		
	bool	IsShowGridAreas		()				{ return mShowGridAreas;	}
	//@}
	
private:

	// ----- Private ------

	// ----- Objects -----

	CollisionParser *mCollisionParser;

	// ----- Entity attributes -----
	
	IND_Surface		*mSu;	// Pointer to a surface
	IND_Animation	*mAn;	// Pointer to an animation
	IND_Primitive2d mPri2d;	// Primitive type
	IND_Font		*mFont; // Fonts

	// Show
	bool mShow;				// Flag for showing / hiding the entity

	// Space transformation attributes
	bool mUpdateTransFlag;	// Flag for knowing when to recalculate the transformation matrix of an entity
	float mX;				// x Coordinate
	float mY;				// y Coordinate
	int mZ;					// Depth (indicates which object will be blitted upon which other)
	float mAngleX;			// Angle in the x axis
	float mAngleY;			// Angle in the y axis
	float mAngleZ;			// Angle in the z axis
	float mScaleX;			// Horizontal scaling
	float mScaleY;			// Vertical scaling	
	bool mCull;				// Back Face Culling ON / OFF
	float mHotSpotX;		// Hotspot x
	float mHotSpotY;		// Hotspot y
	int mAxisCalX;			// Horizontal hotspot precalculated
	int mAxisCalY;			// Vertical hotspot precalculated
	bool mMirrorX;			// Horizontal mirroring
	bool mMirrorY;			// Vertical mirroring
	IND_Filter mFilter;		// Filter type (0 = Nearest Point || 1 = Linear)
	IND_Matrix	mMat;		// World matrix

	// Color, transperency and fading attributes
	IND_Type mType;			// IND_Type: IND_Opaque, IND_Alpha
	unsigned char mR;				// R component for tinting
	unsigned char mG;				// G component for tinting
	unsigned char mB;				// B component for tinting
	unsigned char mA;				// Transparency
	unsigned char mFadeR;			// R component for the fade to color
	unsigned char mFadeG;			// G component for the fade to color
	unsigned char mFadeB;			// B component for the fade to color
	unsigned char mFadeA;			// Amount of fade
	IND_BlendingType mSo;	// Blending source
	IND_BlendingType mDs;	// Blending destination

	// Region
	int mOffX;				// x coordinate
	int mOffY;				// y coordinate
	int mRegionWidth;		// Witdh
	int mRegionHeight;		// Height

	// Tiling
	bool mWrap;				// Wrapping
	float mUDisplace;		// U Coordinate displacement
	float mVDisplace;		// V Coordinate displacement

	// Animation attributes
	int mSequence;			// Index of the sequence
	int mNumReplays;		// Num of replays of the sequence
	int mFirstTime;			// Flag

	// 2d primitive attributes
	int mX1, mY1, mX2, mY2;	// Line
	int mRadius;			// Radius
	IND_Point *mPolyPoints;	// Point list
	int mNumLines;			// Number of sides of the polygon
	int mNumSides;			// Number of sides of the regular polygon
	float mPolyAngle;		// Regular polygon angle

	// Fonts attributes
	IND_Align mAlign;		// Font align
	int mCharSpacing;		// Additional space between letters
	int mLineSpacing;		// Space between lines
	char *mText;			// Text

	// Collision attributes
	bool mShowCollisionAreas;

	// Grid areas attributes
	bool mShowGridAreas;

	// Collision list for surfaces (the collision list for animations is in IND_AnimationManager.h)
	list <BOUNDING_COLLISION*> *mListBoundingCollision;	// Vector of bounding areas for collision checking	

	// ----- Private methods -----

	void	InitAttrib						();
	bool	IsNullMatrix					();

	// ----- Friends -----
	
	friend class IND_Entity2dManager;
};
/*@}*/

#endif // _IND_ENTITY2D_
