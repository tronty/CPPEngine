/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include "Grid.h"
#include "HeightMap.h"
#include "Vertex.h"

#include <assert.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <stdlib.h>

extern int dprintf(const char* formatString, ...);

Grid::Grid(void)
{
  resize(1, 1);
}

Grid::Grid(int width, int height)
{
  resize(width, height);
}

void Grid::clear(float value)
{
  int i, size;

  size = (int)_data.size();
  for(i=0; i<size; i++)
    {
      _data[i] = value;
    }
}

void Grid::copy(const Grid& grid)
{
  assert(this != &grid);

  _data = grid._data; 
}

void Grid::createVertexAndIndexBuffers(IDirect3DDevice9* device, IDirect3DVertexBuffer9*& vertexBuffer, IDirect3DIndexBuffer9*& indexBuffer,
                                       const D3DXFROMWINEVECTOR3& scale, const D3DXFROMWINEVECTOR3& offset, HeightMap* heightMap)
{
  assert(device != 0);
  assert(heightMap != 0);

  int       hr;
  int           x, y;
  int           x0, x1;
  const int     swathWidth = 10;
  float         s, t;
  float         widthOverTwo, heightOverTwo;
  float         waterDepth;
  Vertex*       vertices;
  unsigned int* indices;
  int           bufferIndex;
  int           vertexIndex;
  D3DXFROMWINEVECTOR3   position;
  D3DCOLOR      diffuse;



  // Create and fill the vertex buffer
  hr = device->CreateVertexBuffer(_width * _height * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &vertexBuffer, 0);
  if(hr != D3D_OK)
    {
      DXTRACE_ERR("Grid::createVertexAndIndexBuffers()  CreateVertexBuffer() failed\n", hr);
      return;
    }

  hr = vertexBuffer->Lock(0, 0, (void**)&vertices, 0);
  if(hr != D3D_OK)
    {
      DXTRACE_ERR("Grid::createVertexAndIndexBuffers()  vertexBuffer->Lock() failed\n", hr);
      return;
    }

  widthOverTwo = (float)_width / 2.0f;
  heightOverTwo = (float)_height / 2.0f;
  for(y=0; y<_height; y++)
    {
      for(x=0; x<_width; x++)
        {
          position = D3DXFROMWINEVECTOR3((float)x / widthOverTwo - 1.0f, (*this)(x, y), (float)y / heightOverTwo - 1.0f);
          position.x *= scale.x;
          position.y *= scale.y;
          position.z *= scale.z;
          position += offset;
          diffuse = D3DCOLOR_RGBA(0, 0, 128 + stx_rand() % 128, 255);
          s = (float)x / (float)_width;
          t = (float)y / (float)_height;
          //dprintf("%3d, %3d:  %8.2f, %8.2f, %8.2f\n", x, y, position.x, position.y, position.z);
          vertexIndex = y * _width + x;
          vertices[vertexIndex].position = position;
          vertices[vertexIndex].diffuse = diffuse;
          vertices[vertexIndex].tex0[0] = s;
          vertices[vertexIndex].tex0[1] = t;

          waterDepth = -heightMap->height(s, t, false, true);
          if(waterDepth < 0.0f)
            {
              waterDepth = 0.0f;
            }

          // Compute the distortion factors
          vertices[vertexIndex].tex1[0] = smoothStep(0.0f, 0.0f, 5.0f, 5.0f, waterDepth);   // Reflection factor for distant objects
          vertices[vertexIndex].tex1[1] = smoothStep(0.0f, 0.0f, 5.0f, 5.0f, waterDepth);   // Refraction factor for distant objects
          vertices[vertexIndex].tex1[2] = smoothStep(0.0f, 0.0f, 1.0f, 0.5f, waterDepth);   // Reflection factor for objects in water
          vertices[vertexIndex].tex1[3] = smoothStep(0.0f, 0.0f, 1.0f, 0.5f, waterDepth);   // Refraction factor for objects in water

          // TODO remove begin
          vertices[vertexIndex].tex1[0] = smoothStep(0.0f, 0.0f, 5.0f, 2.0f, waterDepth);   // Reflection factor for distant objects
          vertices[vertexIndex].tex1[1] = smoothStep(0.0f, 0.0f, 5.0f, 5.0f, waterDepth);   // Refraction factor for distant objects
          vertices[vertexIndex].tex1[2] = smoothStep(0.0f, 0.0f, 1.0f, 0.2f, waterDepth);   // Refraction factor for objects in water
          vertices[vertexIndex].tex1[3] = smoothStep(0.0f, 0.0f, 1.0f, 0.2f, waterDepth);   // Refraction factor for objects in water
          // TODO remove end
        }
    }

  vertexBuffer->Unlock();
  



  // Create and fill the index buffer
  hr = device->CreateIndexBuffer((_width - 1) * (_height - 1) * 2 * 3 * sizeof(unsigned int), D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, 
                                 D3DPOOL_MANAGED, &indexBuffer, 0);
  if(hr != D3D_OK)
    {
      DXTRACE_ERR("Grid::createVertexAndIndexBuffers()  CreateIndexBuffer() failed\n", hr);
      return;
    }

  hr = indexBuffer->Lock(0, 0, (void**)&indices, 0);
  if(hr != D3D_OK)
    {
      DXTRACE_ERR("Grid::createVertexAndIndexBuffers()  indexBuffer->Lock() failed\n", hr);
      return;
    }

  bufferIndex = 0;
  for(x0=0; x0<_width - 1; x0 += swathWidth)
    {
      x1 = x0 + swathWidth;

      for(y=0; y <_height - 1; y++)
        {
          for(x=x0; (x < _width - 1) && (x < x1); x++)
            {
              indices[bufferIndex + 0] = (y + 0) * _width + (x + 0);
              indices[bufferIndex + 1] = (y + 0) * _width + (x + 1);
              indices[bufferIndex + 2] = (y + 1) * _width + (x + 1);
          
              indices[bufferIndex + 3] = (y + 0) * _width + (x + 0);
              indices[bufferIndex + 4] = (y + 1) * _width + (x + 1);
              indices[bufferIndex + 5] = (y + 1) * _width + (x + 0);
              
              bufferIndex += 6;
            }
        }
    }

  indexBuffer->Unlock();

}


void Grid::resize(int width, int height)
{
  assert(width > 0);
  assert(height > 0);

  _width = width;
  _height = height;
  _data.resize(_width * _height);
}

int Grid::height(void) const
{
  return(_height);
}

int Grid::size(void) const
{
  return((int)_data.size());
}

int Grid::width(void) const
{
  return(_width);
}

int Grid::primitiveCount(void) const
{
  return(2 * (_width - 1) * (_height - 1));
}

float& Grid::operator ()(int x, int y)
{
  assert(x >= 0);
  assert(x < _width);
  assert(y >= 0);
  assert(y < _height);

  return(_data[y * _width + x]);
}

float Grid::operator ()(int x, int y) const
{
  assert(x >= 0);
  assert(x < _width);
  assert(y >= 0);
  assert(y < _height);

  return(_data[y * _width + x]);
}

void Grid::operator += (float value)
{
  int i;

  for(i=0; i<(int)_data.size(); i++)
    {
      _data[i] += value;
    }
}

void Grid::operator *= (float value)
{
  int i;

  for(i=0; i<(int)_data.size(); i++)
    {
      _data[i] *= value;
    }
}

float Grid::smoothStep(float t0, float f0, float t1, float f1, float t)
{
  assert((t1 - t0) * (t1 - t0) > .000001f);   // Make sure that t0 != t1

  float f;             // f evaluated at t
  float tNormalized;   // t in [0,1]

  // Normalize t from [t0, t1] to [0, 1]
  tNormalized = (t - t0) / (t1 - t0);
  if(tNormalized < 0.0f)
    {
      tNormalized = 0.0f;
    }
  if(tNormalized > 1.0f)
    {
      tNormalized = 1.0f;
    }

  // Evaluate smooth step cubic spline
  f = -2.0f * tNormalized * tNormalized * tNormalized + 3.0f * tNormalized * tNormalized;
  
  // Scaled and bias to [f0, f1]
  f = f0 + (f - f0) * (f1 - f0);

  return(f);
}
