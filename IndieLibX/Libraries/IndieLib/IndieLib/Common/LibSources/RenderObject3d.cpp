#if defined( BUILD_MESHRENDERER) 
#include <CIndieLib.h>
/*****************************************************************************************
 * File: RenderObject3d.cpp
 * Desc: Blitting of 3d objects using D3D
 * Modification of the Keith Ditchburn class
 *****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (javierlopezpro@gmail.com)

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
  Copyright (c) 2015 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
	
*/

// ----- Includes -----
#define __MESHRENDERER__ 1
#include <CIndieLib.h>


#include <IND_SurfaceManager.h>
#include <IND_Render.h>

// --------------------------------------------------------------------------------
//							         Public methods
// --------------------------------------------------------------------------------

/*!
\defgroup Graphical_3d_Objects Bliting 3d Animated 3d Models and setting the transformations directly
\ingroup Advances
*/
/*@{*/

/*!
\b Parameters:

\arg \b p3dMesh						Pointer to a ::IND_3dMesh object

\b Operation:

This function blits directly to the screen a ::IND_3dMesh object.

In order to change the transformations
and color attributes of the 3d mesh you have to use the IND_Render::SetTransform3d() and IND_Render::SetRainbow()
methods before calling to this function. Remember that you can use IND_Entity3d object for drawing 3d meshes to
the screen without having to use this advanced methods directly. This method is only useful for advanced users
with really concrete purposes.

Using this method is equivalent to using:
- IND_Entity3d::Set3dMesh()
*/
INLINE void IND_Render::Blit3dMesh	(/*int Getab(),*/IND_3dMesh *p3dMesh)
{
	D3DXFROMWINEMATRIX mMatrix;
	mInfo.mDevice->GetTransform (TS_WORLD, mMatrix);

	FrameMove (p3dMesh, GetFrameTime() * 0.001f , &mMatrix);
#if 0
	if (p3dMesh->m3dMesh.m_frameRoot)
		DrawFrame (p3dMesh->m3dMesh.m_frameRoot);
#else//???
	//mInfo.mDevice->renderAssimpMesh(p3dMesh->m3dMesh.mesh)
	if (p3dMesh->m3dMesh)
		DrawFrame (p3dMesh);
#endif
}


/*!
\b Parameters:

\arg \b p3dMesh						Pointer to a ::IND_3dMesh object
\arg \b pIndex						Index of the animation

\b Operation:

This function sets the animation sequence that will be rendered. When you use this method, the transition between the
current animation and the following will not be abrubt. IndieLib will fade out the effect of the first animation
and fade in the second. Use IND_3dMesh::SetTransitionSpeed() in order set the speed of that transition.
*/
void IND_Render::Set3dMeshSequence (/*int Getab(),*/IND_3dMesh *p3dMesh, unsigned int pIndex)
{
	if(!p3dMesh->m3dMesh->m_pAnimator)
		return;

	if (pIndex == p3dMesh->m3dMesh->m_pAnimator->CurrentAnimIndex())
		return;

	if (pIndex >= (unsigned int) p3dMesh->m3dMesh-> /* m_pAnimator-> */ GetNumAnimations() )
		pIndex = 0;

	// Remember current animation
//	p3dMesh->m3dMesh.m_currentAnimationSet = pIndex;
	p3dMesh->m3dMesh->m_pAnimator->SetAnimIndex(pIndex);

#if 0
	// Get the animation set from the controller
	LPD3DXFROMWINEANIMATIONSET set
	p3dMesh->m3dMesh.m_animController->GetAnimationSet(p3dMesh->m3dMesh.m_currentAnimationSet, &set )

	// Note: for a smooth transition between animation sets we can use two tracks and assign the new set to the track
	// not currently playing then insert Keys into the KeyTrack to do the transition between the tracks
	// tracks can be mixed together so we can gradually change into the new animation

	// Alternate tracks
	__DWORD__ newTrack = ( p3dMesh->m3dMesh.m_currentTrack == 0 ? 1 : 0 )

	// Assign to our track
	p3dMesh->m3dMesh.m_animController->SetTrackAnimationSet( newTrack, set )
    set->Release()

	// Clear any track events currently assigned to our two tracks
	p3dMesh->m3dMesh.m_animController->UnkeyAllTrackEvents( p3dMesh->m3dMesh.m_currentTrack )
    p3dMesh->m3dMesh.m_animController->UnkeyAllTrackEvents( newTrack )

	// Add an event key to disable the currently playing track kMoveTransitionTime seconds in the future
    p3dMesh->m3dMesh.m_animController->KeyTrackEnable( p3dMesh->m3dMesh.m_currentTrack, FALSE, p3dMesh->m3dMesh.m_currentTime + p3dMesh->m3dMesh.kMoveTransitionTime )
	// Add an event key to change the speed right away so the animation completes in kMoveTransitionTime seconds
    p3dMesh->m3dMesh.m_animController->KeyTrackSpeed( p3dMesh->m3dMesh.m_currentTrack, 0.0f, p3dMesh->m3dMesh.m_currentTime, p3dMesh->m3dMesh.kMoveTransitionTime, D3DXFROMWINETRANSITION_LINEAR )
	// Add an event to change the weighting of the current track (the effect it has blended with the secon track)
    p3dMesh->m3dMesh.m_animController->KeyTrackWeight( p3dMesh->m3dMesh.m_currentTrack, 0.0f, p3dMesh->m3dMesh.m_currentTime, p3dMesh->m3dMesh.kMoveTransitionTime, D3DXFROMWINETRANSITION_LINEAR )

	// Enable the new track
    p3dMesh->m3dMesh.m_animController->SetTrackEnable( newTrack, TRUE )
	// Add an event key to set the speed of the track
    p3dMesh->m3dMesh.m_animController->KeyTrackSpeed( newTrack, 1.0f, p3dMesh->m3dMesh.m_currentTime, p3dMesh->m3dMesh.kMoveTransitionTime, D3DXFROMWINETRANSITION_LINEAR )
	// Add an event to change the weighting of the current track (the effect it has blended with the first track)
	// As you can see this will go from 0 effect to total effect(1.0f) in kMoveTransitionTime seconds and the first track goes from
	// total to 0.0f in the same time.
    p3dMesh->m3dMesh.m_animController->KeyTrackWeight( newTrack, 1.0f, p3dMesh->m3dMesh.m_currentTime, p3dMesh->m3dMesh.kMoveTransitionTime, D3DXFROMWINETRANSITION_LINEAR )

	// Remember current track
    p3dMesh->m3dMesh.m_currentTrack = newTrack
#else//???

#endif
}
/*@}*/

// --------------------------------------------------------------------------------
//							         Private methods
// --------------------------------------------------------------------------------

/*
*************************
Called to render a frame in the hierarchy
device - the Direct3D device object
frame - frame to render
*************************
*/
void IND_Render::DrawFrame(IND_3dMesh *p3dMesh)
{
#if 0
	// Draw all mesh containers in this frame
    LPD3DXFROMWINEMESHCONTAINER meshContainer = pFrame->pMeshContainer
    while (meshContainer)
    {
        DrawMeshContainer(meshContainer, pFrame)
        meshContainer = meshContainer->pNextMeshContainer
    }
#elif 1
	//p3dMesh->drawAiScene(p3dMesh->m3dMesh.m_frameRoot,w)
	p3dMesh->m3dMesh->render();//(mInfo.mDevice,p3dMesh->m3dMesh->m_pAnimator->mRootNode)
#endif
}


/*
*************************
Called to render a mesh
device - the Direct3D device object
meshContainerBase - the mesh container
frameBase - frame containing the mesh
*************************
*/
void IND_Render::DrawMeshContainer(IND_3dMesh *p3dMesh)
{
#if 0
	// Cast to our extended frame type
	D3DXFROMWINEFRAME_EXTENDED *frame = (D3DXFROMWINEFRAME_EXTENDED*)frameBase

	// Cast to our extended mesh container
	D3DXFROMWINEMESHCONTAINER_EXTENDED *meshContainer = (D3DXFROMWINEMESHCONTAINER_EXTENDED*)meshContainerBase

	// Set the world transform
    GetDevice()->SetTransform(TS_WORLD, frame->exCombinedTransformationMatrix)

	// Loop through all the materials in the mesh rendering each subset
    for (unsigned int iMaterial = 0 iMaterial < meshContainer->mNumMaterials
		 iMaterial++)
    {
		// use the material in our extended data rather than the one in meshContainer->pMaterials[iMaterial].MatD3D
#if 0
		GetDevice()->SetMaterial( &meshContainer->exMaterials[iMaterial] )
#endif
#if 0
		GetDevice()->setTexture ("Sampler", meshContainer->exTextures[iMaterial],mInfo.mDevice->
			//GetbilinearClamp
			Getlinear
			() )
		// Select the mesh to draw, if there is skin then use the skinned mesh else the normal one
		IND_3dMesh * pDrawMesh = (meshContainer->pSkinInfo) ? meshContainer->exSkinMesh: meshContainer->MeshData.pMesh

		// Finally Call the mesh draw function
        pDrawMesh->DrawSubset(iMaterial)
#endif
    }
#elif 1
	//p3dMesh->drawAiScene(/*???meshContainerBase*/ p3dMesh->m3dMesh.m_frameRoot,w)
	p3dMesh->m3dMesh->render();//p3dMesh->DrawScene(mInfo.mDevice,p3dMesh->m3dMesh->m_pAnimator->mRootNode)
#endif

}


/*
*************************
Called each frame update with the time and the current world matrix
elapsedTime - time passed
matWorld - current world matrix for the model
*************************
*/
void IND_Render::FrameMove (IND_3dMesh *p3dMesh, float elapsedTime, const D3DXFROMWINEMATRIX *matWorld)
{
	//D3DXFROMWINEMATRIX mMatrix;
	mInfo.mDevice->SetTransform (TS_WORLD, *matWorld);
}


/*
*************************
Called to update the frame matrices in the hierarchy to reflect current animation stage
frameBase - frame being looked at
parentMatrix - the matrix of our parent (if we have one)
*************************
*/
#if 0
void IND_Render::UpdateFrameMatrices(const aiScene*frameBase, const D3DXFROMWINEMATRIX *parentMatrix)
{

}
#endif
#endif
