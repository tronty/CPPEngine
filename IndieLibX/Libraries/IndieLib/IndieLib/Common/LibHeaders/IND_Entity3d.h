#if defined( BUILD_MESHRENDERER) 
/*****************************************************************************************
/* File: IND_Entity3d.h
/* Desc: Entity 3d object
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

#ifndef _IND_ENTITY3D_
#define _IND_ENTITY3D_

// ----- Includes -----

#include "IND_SurfaceManager.h"


// --------------------------------------------------------------------------------
//									 IND_Entity3d
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Entity3d IND_Entity3d
\ingroup EntityManagers
3d entity class for drawing graphical objects into the screen and manipulating their attributes, click in ::IND_Entity3d to see all the methods of this class. 
*/
/*@{*/

/*!
\b ::IND_Entity3d is an 3d entity object of the class ::IND_Entity3dManager. This class, together with ::IND_Entity2d (object
of ::IND_Entity2dManager), are the most important classes of IndieLib.

An entity is an object that can have a graphical object associated to it. In this case this graphical objects
are 3d meshes. Using the methods of this class you will be able to change
the attributes of that 3d meshes.

For example, in a game, a 3d monster can be a ::IND_Entity3d that has a ::IND_3dMesh associated to it. You can have as many monsters
(::IND_3dMesh objects) in the screen as you want, all of them sharing the same ::IND_3dMesh (the 3d model). In system memory you will
have only one 3d mesh loaded, the rest will be instances. The cool thing is that you can
change the attributes of each of these different ::IND_Entity3d objects. For example, you can change the size, rotation, color or transparency
of the monsters. So, having only one 3d mesh, you can have lot of different monsters in the screen, with different sizes, colors, positions, etc.
*/
class DLL_EXP IND_Entity3d
{
public:

	// ----- Init -----

	IND_Entity3d		() { InitAttrib (); }

	// ----- Public sets ------

	/** @name Graphical objects assignation to the 3d entity
	* 
	*/
	//@{
	void Set3dMesh (IND_3dMesh *pNewMesh);
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
	bool	SetSequence			(int pS);
	void	SetAnimationSpeed	(float pSpeed);
	void	SetTransitionSpeed	(float pSpeed);
	//@}

	/** @name Space transformations
	*   
	*/
	//@{
	void	SetPosition			(float pX, float pY, float pZ);											
	void	SetAngleXYZ			(float pAnX, float pAnY, float pAnZ);
	void	SetScale			(float pSx, float pSy, float pSz);											
	void	SetFilter			(IND_Filter pF);
	//@}

	/** @name Color transformations, blending and back face culling
	*   
	*/
	//@{
	void	SetBackCull			(bool pCull);
	void	FlipNormals			(bool pFlipNormals);
	void	SetTint				(unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA);
	void	SetTransparency		(unsigned char pA);	
	void	SetFade				(unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA);			
	void	SetBlendSource		(IND_BlendingType pSo);
	void	SetBlendDest		(IND_BlendingType pDs);
	//@}	


	// ----- Public gets ------

	/** @name Gets
	* 
	*/
	//@{
	//! If the entity has a surface assigned, it returns a pointer to this surface.
	IND_3dMesh		*Get3dMesh			()		{ return mMe;								}
	//! Returns true if the entity is being showed, false if not
	bool	IsShow						()		{ return mShow;								}
	//! Returns the sequence number that has been assigned to the animation.	
	int		GetSequence					()		{ return mSequence;							}
	//! Return the animation speed value	
	//float	GetAnimationSpeed			()		{ return mMe->GetAnimationSpeed();			}
	//! Return the transition speed value
	//float	GetTransitionSpeed			()		{ return mMe->GetTransitionSpeed();			}
	//! Returns X position of the entity.
	float		GetPosX					()		{ return mX; 								}
	//! Returns Y position of the entity.
	float		GetPosY					()		{ return mY; 								}
	//! Returns Z position of the entity.
	float		GetPosZ					()		{ return mZ;								}
	//! Returns the angle in the X axis of the entity.	
	float	GetAngleX					()		{ return mAngleX;							}
	//! Returns the angle in the Y axis of the entity.		
	float	GetAngleY					()		{ return mAngleY;							}
	//! Returns the angle in the Z axis of the entity.	
	float	GetAngleZ					()		{ return mAngleZ;							}	
	//! Returns the X scale of the entity.	
	float	GetScaleX					()		{ return mScaleX; 							}
	//! Returns the Y scale of the entity.
	float	GetScaleY					()		{ return mScaleY; 							}
	//! Returns the Z scale of the entity.
	float	GetScaleZ					()		{ return mScaleZ; 							}
	//! Indicates if the entity is making backface culling 0 = No, 1 = Yes.		
	bool	GetBackCull					()		{ return mCull;								}							
	//! Returns the type of filter ::IND_Filter which uses the graphical object assignated to the entity.
	IND_Filter	GetFilter				()		{ return mFilter;							}
	//! Returns the tint level in the R (red) channel of the entity.	
	unsigned char	GetTintR					()		{ return mR;								}
	//! Returns the tint level in the G (green) channel of the entity.
	unsigned char	GetTintG					()		{ return mG;								}
	//! Returns the tint level in the B (blue) channel of the entity.
	unsigned char	GetTintB					()		{ return mB;								}
	//! Returns the transparency level of the entity
	unsigned char	GetTransparency				()		{ return mA;								}
	//! Returns the fade level in R (red) channel of the entity.	
	unsigned char	GetFadeR					()		{ return mFadeR;							}
	//! Returns the fade level in G (green) channel of the entity.	
	unsigned char	GetFadeG					()		{ return mFadeG;							}
	//! Returns the fade level in B (blue) channel of the entity.		
	unsigned char	GetFadeB					()		{ return mFadeB;							}
	//! Returns the fade level of the entity.	
	unsigned char	GetFadeA					()		{ return mFadeA;							}
	//! Returns the blending type ::IND_BlendingType for the source
	IND_BlendingType	GetBlendSource	()		{ return mSo;								}
	//! Returns the blending type ::IND_BlendingType for the destination		
	IND_BlendingType	GetDestSource	()		{ return mDs;								}
	//@}

private:

	// ----- Private ------

	// ----- Entity attributes -----
	
	IND_3dMesh		*mMe;		// Pointer to a mesh

	// Show
	bool mShow;					// Flag for showing / hiding the entity

	// Space transformation attributes
	bool mFlipNormals;			// Flip normals 0 / 1 of / off
	float mX;					// x Coordinate
	float mY;					// y Coordinate
	float mZ;					// z Coordinate 
	float mAngleX;				// x Axis angle
	float mAngleY;				// y Axis angle
	float mAngleZ;				// z Axis angle
	float mScaleX;				// x Scaling
	float mScaleY;				// y Scaling	
	float mScaleZ;				// z Scaling
	bool mCull;					// Back Face Culling (1 = Activated, 0 = Deactivated)
	IND_Filter mFilter;			// Tipo filtro (0 = Nearest Point || 1 = Linear)
	IND_Matrix	mMat;			// Global Matrix

	// Color, transperency and fading attributes
	unsigned char mR;					// Tint R
	unsigned char mG;					// Tint G
	unsigned char mB;					// Tint B
	unsigned char mA;					// Tint A
	unsigned char mFadeR;				// Fade to color R
	unsigned char mFadeG;				// Fade to color G
	unsigned char mFadeB;				// Fade to color B
	unsigned char mFadeA;				// Fade to color A (quantity of fading)
	IND_BlendingType mSo;		// Blending Type
	IND_BlendingType mDs;		// Blending Source

	// Animations attributes
	bool mChangeSequenceFlag;	// Activated when changing the sequence
	int mSequence;				// Number of sequences to draw

	// ----- Private methods -----

	void	InitAttrib					 ();

	// ----- Friends -----
	
	friend class IND_Entity3dManager;
};
/*@}*/

#endif // _IND_ENTITY3D_
#endif

