#include <CIndieLib.h>
#define __MESHRENDERER__ 1
#if defined( BUILD_MESHRENDERER) 
/*****************************************************************************************
 * File: IND_3dMesh.cpp
 * Desc: 3d Mesh object
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


#include <IND_3dMesh.h>
// --------------------------------------------------------------------------------
//									 Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pSpeed                    Value higher than 0 that sets the animation speed

\b Operation:

If you change the animation sequence of a 3d Mesh when Indielib is rendering(using IND_Entity3d::SetSequence())
the change will not be abrupt. IndieLib will fade out the effect of the first animation and fade in the second.

This function sets the animation speed of this transition animation. The default value is 0.25f. To speed up the
transition choose values between 0.25f and 0.0f. The fastest transition possible is 0.0f. In order to slow down the
animation speed, choose values higher than 0.25f, the higher the value, the slower the animation.

Default: 0.25f
*/
	void IND_3dMesh::SetTransform3d	(float mX,
										float mY,
										float mZ,
										float mAngleX,
										float mAngleY,
										float mAngleZ,
										float mScaleX,
										float mScaleY,
										float mScaleZ)
	{
		this->m3dMesh->SetTX(mX);
		this->m3dMesh->SetTY(mY);
		this->m3dMesh->SetTZ(mZ);
		this->m3dMesh->SetSX(mScaleX);
		this->m3dMesh->SetSY(mScaleY);
		this->m3dMesh->SetSZ(mScaleZ);
		this->m3dMesh->SetSpinX(mAngleY);
		this->m3dMesh->SetSpinY(mAngleX);
		this->m3dMesh->SetSpinZ(mAngleZ);
	}

void IND_3dMesh::SetTransitionSpeed		(float pSpeed)
{
#if 0
	if (pSpeed < 0.001f) pSpeed = 0.001f

	m3dMesh.kMoveTransitionTime = pSpeed
#endif
}


/*!
\b Parameters:

\arg \b pSpeed                    Value higher than 0 that sets the animation speed

\b Operation:

This function sets the animation speed when changing from one sequence to another. The default value is 1.0f.
To speed up the animation choose values between 1.0f and 0.0f. The fastest animation possible is 0.0f. In order to
slow down the animation speed, choose values higher than 1.0f, the higher the value, the slower the animation.

Default: 1.0f
*/
void IND_3dMesh::SetAnimationSpeed		(float pSpeed)
{
#if 0
	if (pSpeed < 0.001f) pSpeed = 0.001f

	m3dMesh.m_speedAdjust = pSpeed
#endif
}
#if 1

void IND_3dMesh::SetPlay(bool a){m3dMesh->SetPlay(a);}
#ifdef MESHRENDERER2_ORIGINAL
void IND_3dMesh::SetAnimIndex( size_t pAnimIndex){m3dMesh->GetAsset()->m_pAnimator->SetAnimIndex(pAnimIndex);}

	//! Return the name of the loaded 3d Mesh
	//char				*IND_3dMesh::GetMeshName			()	{ return m3dMesh.mAttributes.mName				}
	//! Return the number of the sequence that is being rendered at this moment
	unsigned int		IND_3dMesh::GetCurrentSequence		()	{ return m3dMesh->GetAsset()->m_pAnimator->CurrentAnimIndex();			}
	//! Return the number of sequence animations of the 3d Mesh
	int					IND_3dMesh::GetNumSequences			()	{ return m3dMesh->GetAsset()->m_pAnimator->mScene->mNumAnimations;		}
#else
void IND_3dMesh::SetAnimIndex( size_t pAnimIndex){m3dMesh->m_pAnimator->SetAnimIndex(pAnimIndex);}

	//! Return the name of the loaded 3d Mesh
	//char				*IND_3dMesh::GetMeshName			()	{ return m3dMesh.mAttributes.mName				}
	//! Return the number of the sequence that is being rendered at this moment
	unsigned int		IND_3dMesh::GetCurrentSequence		()	{ return m3dMesh->m_pAnimator->CurrentAnimIndex();			}
	//! Return the number of sequence animations of the 3d Mesh
	int					IND_3dMesh::GetNumSequences			()	{ return m3dMesh->GetNumAnimations();		}
#endif
	//! Return the animation speed value
	//float				IND_3dMesh::GetAnimationSpeed		()	{ return m_speedAdjust					}
	//! Return the transition speed value
	//float				IND_3dMesh::GetTransitionSpeed		()	{ return kMoveTransitionTime			}


#if 0
	// ----- Private ------
/*
	// Attributes
	struct structAttributes
	{
		char *mName
		int mNumSequences

		structAttributes ()
		{
			mName = 0
			mNumSequences = 0
		}
	}
	typedef struct structAttributes ATTRIBUTES
*/
	// TYPE
	struct struct3dMesh
	{
		struct3dMesh()
		{mesh=0}
		#if 0
		// Direct3D objects required for animation
		LPD3DXFROMWINEFRAME                 m_frameRoot
		LPD3DXFROMWINEANIMATIONCONTROLLER   m_animController
		D3DXFROMWINEMESHCONTAINER_EXTENDED *m_firstMesh
#elif 0
#endif
#if 0
		// Bone data
		//D3DXFROMWINEMATRIX *m_boneMatrices
		//int m_maxBones

		// Animation variables
		unsigned int m_currentAnimationSet
		unsigned int m_currentTrack
		float m_currentTime
		float m_speedAdjust

		// The time to change from one animation set to another
		// To see how the merging works - increase this time value to slow it down
		float kMoveTransitionTime

		// Attributes
		ATTRIBUTES		mAttributes

		struct3dMesh ()
		{
			m_maxBones = m_currentAnimationSet = m_currentTrack = 0
			m_speedAdjust = 1.0f
			m_currentTime = 0.0f
			m_boneMatrices = 0
			kMoveTransitionTime = 0.25f
//			m_firstMesh = 0
		}
#endif
	}
	typedef struct struct3dMesh MESH3D
	MESH3D m3dMesh
#else
#endif
void IND_3dMesh::SetRenderingMode(unsigned int i)
{
	m3dMesh->SetRenderingMode(i);
}
void IND_3dMesh::GetBoundingSphereRadius(float& r)
{
	m3dMesh->GetBoundingSphereRadius(r);
}
void IND_3dMesh::GetBounds(D3DXFROMWINEVECTOR3& ____min, D3DXFROMWINEVECTOR3& ____max)
{
	m3dMesh->GetBounds(____min, ____max);
}
#endif
#endif
