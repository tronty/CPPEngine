#ifndef _VOXELIZER_H_
#define _VOXELIZER_H_
/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//----------------------------------------------------------------------------------
// File:   Voxelizer.h
// Author: Ignacio Llamas
// Email:  sdkfeedback@nvidia.com
// 
// Copyright (c) 2007 NVIDIA_Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA OR ITS SUPPLIERS
// BE  LIABLE  FOR  ANY  SPECIAL,  INCIDENTAL,  INDIRECT,  OR  CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//
//----------------------------------------------------------------------------------

#include "SkinnedMesh.h"

/*=============================================================================
  Arbitrary Mesh Voxelizer.
  Takes as input an arbitrary mesh and matrix transformation and generates a
   voxelized representation of the mesh in the destination texture, where voxels 
   outside the object have 0 value and voxels inside have non-zero value.
  
  The matrix transform is the object to voxel-volume transform, i.e. it is the
   object transform with respect to the coordinate system of the voxel-volume, 
   which has its origin at 0,0,0.

  Proposed Algorithm:

    StencilClipVolume: 
        per-slice stencil XOR or Non-Zero IN/OUT test with moving clip-plane.

     The object is drawn into each slice in the volume, setting an orthogonal
     projection such that the near plane matches the slice in question and the far
     plane is at infinity. The left, right, top and bottom clip planes are set to
     match the volume's side walls.

     A stencil buffer is used in each mesh drawing pass (the same buffer may be used
     for all slices).
     When drawing the mesh the stencil functions are set as follows:
      A) if using the XOR rule, we simply invert the value (STENCIL_OP_INVERT)
      B) if using the Non-Zero rule we increment (STENCIL_OP_INCR) for front faces 
      and decrement for back faces  (D3D10_STENCIL_OP_DECR).
      
      In both cases the end result is that non-zero values are found at those pixels 
      of the slice (voxels) that are inside the mesh.

     This works well only for  closed non-manifold (watertight) meshes.
     For non-closed meshes the results will vary depending on the object orientation 
     with respect to the volume.
     For example a noncapped cylinder, if aligned such that its axis is 
     perpendicular to the slices will yield no voxels inside.
     Conversely, if the axis is parallel to the slices we would get voxels for the
     inside of the same cylinder if it was capped.

  ============================================================================= */


class Voxelizer
{
public:
    Voxelizer(void);
    virtual ~Voxelizer(void);
   
    int SetDestination( 
        TextureID *pDstInOutTexture3D, TextureID *pDstVelocityTexture3D = 0);
    
    int Voxelize(D3DXFROMWINEMATRIX& objToVolumeXForm, SkinnedMesh *pMesh, 
        int srcSoBuf, int prevSrcSoBuf, float timeStep);

    void SetVelocityMultiplier(float f)     { m_velocityMultiplier = f;     }
    float GetVelocityMultiplier(void)       { return m_velocityMultiplier;  }
    // enable disable different types of voxelization
    void SetEnableInOutVoxelize(bool b)     { m_bInOutEnabled = b;      }
    void SetEnableVelocityVoxelize(bool b)  { m_bVelocityEnabled = b;   }
    bool GetEnableInOutVoxelize(void)       { return m_bInOutEnabled;      }
    bool GetEnableVelocityVoxelize(void)    { return m_bVelocityEnabled;   }
    

private:
    int Initialize(void);
    int InitTextures(void);
    int InitShaders(void);
    int InitSlices(void);
    void Cleanup(void);

    void DrawSlices(void);

    int DoVoxelization(void);
    int StencilClipVolume(void);
    int VoxelizeVelocity(void);
    int RenderClippedMesh(float zNear, float zFar, stx_Technique *pTechnique);

    // INTERNAL STATE
    bool                        m_bInOutEnabled;
    bool                        m_bVelocityEnabled;
    // Destination state
    TextureID             *m_pDstInOutTexture3D;
    TextureID      *m_pDstInOutTexRTView;
    TextureID             *m_pDstVelTexture3D;
    TextureID      *m_pDstVelTexRTView;

    unsigned int                        m_width;
    unsigned int                        m_height;
    unsigned int                        m_depth;

    //  for flat 3D texture
    int                         m_cols;
    int                         m_rows;
    
    // Source state
    SkinnedMesh                 *m_pSkinnedMesh;
    int                         m_srcSoBuffer;
    int                         m_prevSrcSoBuffer;
    float                       m_timeStep;
    float                       m_velocityMultiplier;

    D3DXFROMWINEMATRIX                  m_objToVolumeXForm;

    // Other state
    bool                        m_initialized;

    // The depth-stencil buffer
    TextureID             *m_pDSTex2D;
    TextureID      *m_pDSTex2DDSView;
    TextureID    *m_pDSTex2DSRView;

    // Effect/shader state
    EffectID                *m_pVoxEffect;
    stx_Technique       *m_etNZ;
    stx_Technique       *m_etResolveWithPS;
    stx_Technique       *m_etGenVelocityWireframe;

    VertexFormatID           *m_pSOInputLayout;

    stx_Variable          *m_evWorldViewProjectionMatrix;
    stx_Variable          *m_evProjSpacePixDim;
    stx_Variable          *m_evGridDim;
    stx_Variable          *m_evRecTimeStep;
    stx_Variable          *m_evVelocityMultiplier;
    stx_Variable          *m_evSliceIdx;
    stx_Variable          *m_evSliceZ;
    stx_ShaderResourceVariableID  *m_evTexture_depthStencil;


    // Slices state
    VertexFormatID           *m_pSlicesLayout;
    VertexBufferID                *m_pSlicesVB;

};

#endif // _VOXELIZER_H_

