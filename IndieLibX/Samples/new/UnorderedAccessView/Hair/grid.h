/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
// Copyright (c) 2011 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, NONINFRINGEMENT,IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA 
// OR ITS SUPPLIERS BE  LIABLE  FOR  ANY  DIRECT, SPECIAL,  INCIDENTAL,  INDIRECT,  OR  
// CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS 
// OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY 
// OTHER PECUNIARY LOSS) ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, 
// EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
// Please direct any bugs or questions to SDKFeedback@nvidia.com

#ifndef GRID_H
#define GRID_H

#include "Common.h"

struct VS_INPUT_FLUIDSIM_STRUCT;

class Grid
{
public:
    Grid();
    virtual ~Grid( void );

    int Initialize( int gridWidth, int gridHeight, int gridDepth,stx_Effect::stx_Technique* technique );

    void DrawSlices        ( void );
    void DrawSlice         ( int slice );
    void DrawSlicesToScreen( void );
    void DrawBoundaryQuads ( void );
    void DrawBoundaryLines ( void );
    
    int  GetCols(){return m_cols;};
    int  GetRows(){return m_rows;};

    int  GetDimX() {return m_dim[0]; }
    int  GetDimY() {return m_dim[1]; }
    int  GetDimZ() {return m_dim[2]; }

protected:

    
    int CreateVertexBuffers  (stx_Effect::stx_Technique* technique);
    void InitScreenSlice        (VS_INPUT_FLUIDSIM_STRUCT** vertices, int z, int& index);
    void InitSlice             (int z, VS_INPUT_FLUIDSIM_STRUCT** vertices, int& index);
    void InitLine              (float x1, float y1, float x2, float y2, int z,
                                    VS_INPUT_FLUIDSIM_STRUCT** vertices, int& index);
    void InitBoundaryQuads     (VS_INPUT_FLUIDSIM_STRUCT** vertices, int& index);
    void InitBoundaryLines     (VS_INPUT_FLUIDSIM_STRUCT** vertices, int& index);

    int CreateLayout( FormatDesc* layoutDesc, unsigned int numElements, stx_Effect::stx_Technique* technique, VertexFormatID   layout);
    int CreateVertexBuffer( int ByteWidth, unsigned int bindFlags, exBufferID   vertexBuffer,VS_INPUT_FLUIDSIM_STRUCT* vertices,int numVertices);

    // D3D11 helper functions
    void DrawPrimitive( D3D11_PRIMITIVE_TOPOLOGY PrimitiveType, VertexFormatID  layout, exBufferID   vertexBuffer,unsigned int* stride, unsigned int* offset, unsigned int StartVertex, unsigned int VertexCount );

    // Internal State
    //===============
    
    

    VertexFormatID          m_layout;
    exBufferID               m_renderQuadBuffer;
    exBufferID               m_slicesBuffer;
    exBufferID               m_boundarySlicesBuffer;
    exBufferID               m_boundaryLinesBuffer;

    int m_numVerticesRenderQuad;
    int m_numVerticesSlices;
    int m_numVerticesBoundarySlices;
    int m_numVerticesBoundaryLines;
    int m_cols;
    int m_rows;

    int m_dim[3];
};

#endif

