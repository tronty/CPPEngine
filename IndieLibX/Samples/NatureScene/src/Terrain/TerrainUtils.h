#ifndef TERRAIN_UTILS_H
#define TERRAIN_UTILS_H

#include <Tools/MathUtils.h>
#include <Viewer/Frustum.h>
//include "../../Image3/Image3.h"

#define TERRAIN_MAX_HEIGHT   450.0f
#define WATER_MIN_HEIGHT    -150.0f

#define TERRAIN_DEPTH_SCALE   20.0f
#define TERRAIN_WIDTH_SCALE   20.0f

#define TILE_COLUMN_COUNT 17
#define TILE_ROW_COUNT    17
#define TILE_LOD_COUNT     1
#define TILE_COUNT        TILE_ROW_COUNT*TILE_COLUMN_COUNT
                           
#define CELL_COLUMN_COUNT 16
#define CELL_ROW_COUNT    16
#define CELL_COUNT        (CELL_COLUMN_COUNT*CELL_ROW_COUNT)

#define HEIGHT_MAP_DEPTH  256
#define HEIGHT_MAP_WIDTH  256

#define TERRAIN_Z_OFFSET 128
#define TERRAIN_X_OFFSET 128
 
#define GRASS_LOWER_THRESHOLD   -1.0f
#define GRASS_UPPER_THRESHOLD  70.0f

#define GRASS_HEIGHT_OFFSET     20.0f
#define GRASS_HEIGHT            55.0f
#define GRASS_WIDTH             30.0f

#define INDEX_COUNT ((TILE_ROW_COUNT - 1)*(TILE_COLUMN_COUNT - 1)* 6 )

#ifndef OFFSET
  #define OFFSET(x) ((char *)0+(x))
#endif

inline float sgn(float a)
{
	if (a > 0.0F) return (1.0F);
	if (a < 0.0F) return (-1.0F);
	return (0.0F);
}

inline float Tuple4fDot(const D3DXFROMWINEVECTOR4 &v1,  const D3DXFROMWINEVECTOR4 &v2)
{
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

inline void modifyProjectionMatrix(const D3DXFROMWINEVECTOR4& clipPlane)
{
	//float	matrix[16];
	D3DXFROMWINEMATRIX mmatrix;
	float* matrix=&mmatrix[0];
	D3DXFROMWINEVECTOR4	q;

	// Grab the current projection matrix from OpenGL
	//glGetFloatv(GL_PROJECTION_MATRIX, matrix);
	IRenderer::GetRendererInstance()->GetTransform( TS_PROJECTION, mmatrix );
	
	// Calculate the clip-space corner point opposite the clipping plane
	// as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
	// transform it into camera space by multiplying it
	// by the inverse of the projection matrix
	
	q.x = (sgn(clipPlane.x) + matrix[8]) / matrix[0];
	q.y = (sgn(clipPlane.y) + matrix[9]) / matrix[5];
	q.z = -1.0F;
	q.w = (1.0F + matrix[10]) / matrix[14];
	
	// Calculate the scaled plane vector
	D3DXFROMWINEVECTOR4 c = D3DXFROMWINEVECTOR4(clipPlane) * (2.0F / Tuple4fDot(clipPlane, q));
	
	// Replace the third row of the projection matrix
	matrix[2] = c.x;
	matrix[6] = c.y;
	matrix[10] = c.z + 1.0F;
	matrix[14] = c.w;

	// Load it back into OpenGL
#if 0
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(matrix);
	glMatrixMode(GL_MODELVIEW);
#else
	IRenderer::GetRendererInstance()->SetTransform( TS_PROJECTION, mmatrix );
#endif
}
#endif
