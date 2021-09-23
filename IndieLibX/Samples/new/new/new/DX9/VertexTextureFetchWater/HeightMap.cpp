/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#include "HeightMap.h"
#include "Vertex.h"

#include "dxstdafx.h"

extern int    dprintf(const char* formatString, ...);
extern char* toUnicode(const char* string);
extern  gDirect3DDevice9;

HeightMap::HeightMap(const char* heightFilename, const D3DXFROMWINEVECTOR3& scale, const D3DXFROMWINEVECTOR3& offset)
{
  assert(heightFilename != 0);

  _scale = scale;
  _offset = offset;
  _vertexBuffer = 0;
  _indexBuffer = 0;
  _texture = 0;

  // Load the height map
  load(heightFilename);
}

HeightMap::~HeightMap(void)
{
  ////////////////SAFE_RELEASE(_vertexBuffer);
  ////////////////SAFE_RELEASE(_indexBuffer);
  ////////////////SAFE_RELEASE(_texture);
}

void HeightMap::createIndexBuffer( device)
{
  assert(device != 0);

  int       hr;
  unsigned int* indices;
  int           x, z;
  int           bufferIndex;

  // Release the index buffer if there already is one
  ////////////////SAFE_RELEASE(_indexBuffer);

  // Create the index buffer
  hr = device->CreateIndexBuffer((_width - 1) * (_depth - 1) * 2 * 3 * sizeof(unsigned int), D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, 
                                 D3DPOOL_MANAGED, &_indexBuffer, 0);
  if(hr != D3D_OK)
    {
      DXTRACE_ERR("HeightMap::createIndexBuffer()  CreateIndexBuffer() failed\n", hr);
      return;
    }


  // Lock the index buffer
  hr = _indexBuffer->Lock(0, 0, (void**)&indices, 0);
  if(hr != D3D_OK)
    {
      DXTRACE_ERR("HeightMap::createIndexBuffer()  _indexBuffer->Lock() failed\n", hr);
      return;
    }


  // Fill the index buffer
  bufferIndex = 0;
  for(z=0; z <_depth - 1; z++)
    {
      for(x=0; x <_width - 1; x++)
        {
          indices[bufferIndex + 0] = computeIndex(x + 0, z + 0);
          indices[bufferIndex + 1] = computeIndex(x + 1, z + 0);
          indices[bufferIndex + 2] = computeIndex(x + 1, z + 1);
          
          indices[bufferIndex + 3] = computeIndex(x + 0, z + 0);
          indices[bufferIndex + 4] = computeIndex(x + 1, z + 1);
          indices[bufferIndex + 5] = computeIndex(x + 0, z + 1);
          
          bufferIndex += 6;
        }
    }

  _indexBuffer->Unlock();
  
}

void HeightMap::createVertexBuffer( device)
{
  assert(device != 0);

  int hr;
  Vertex* vertices;
  int     x, z;
  int     index;

  // Release the vertex buffer if there already is one
  ////////////////SAFE_RELEASE(_indexBuffer);


  // Create the vertex buffer
  hr = device->CreateVertexBuffer(_width * _depth * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &_vertexBuffer, 0);
  if(hr != D3D_OK)
    {
      DXTRACE_ERR("HeightMap::createVertexBuffer()  CreateVertexBuffer() failed\n", hr);
      return;
    }
  
  
  // Lock the vertex buffer
  hr = _vertexBuffer->Lock(0, 0, (void**)&vertices, 0);
  if(hr != D3D_OK)
    {
      DXTRACE_ERR("HeightMap::createVertexBuffer()  Lock() failed\n", hr);
      return;
    }
  

  // Fill the vertex buffer
  for(z=0; z<_depth; z++)
    {
      for(x=0; x<_width; x++)
        {
          index = computeIndex(x, z);
          computePosition(x, z, vertices[index].position );
          //TODO compute normal
          //TODO remove random color
          vertices[index].diffuse = D3DCOLOR_RGBA(stx_rand() & 0xFF, stx_rand() & 0xFF, stx_rand() & 0xFF, 0x80);
          vertices[index].tex0[0] = (float)x / (float)_width;
          vertices[index].tex0[1] = (float)z / (float)_depth;
        }
    }

  _vertexBuffer->Unlock();
}

void HeightMap::createHeightTexture( device, TextureID& texture)
{
  assert(device != 0);

  int         hr;
  D3DLOCKED_RECT  lockedRect;
  unsigned short* pixel;
  float           value;
  int             x, z;

  hr = D3DXFROMWINECreateTexture(device, _width, _depth, 1, 0, D3DFMT_G16R16, D3DPOOL_MANAGED, &texture);
  assert(hr == D3D_OK);
  assert(texture != 0);

  hr = texture->LockRect(0, &lockedRect, 0, 0);
  assert(hr == D3D_OK);
  
  for(z=0; z<_depth; z++)
    {
      for(x=0; x<_width; x++)
        {
          pixel = (unsigned short*)((unsigned char*)lockedRect.pBits + (z * lockedRect.Pitch + x * 4));
          value = (*this)(x, z) * 256.0f + 32768.0f;
          //value = 32768.0f;
          if((value < 0.0f) || (value >= 65535.0f))
            {
              dprintf("%2d, %2d = %8.3f\n", x, z, value);
            }
          pixel[0] = (unsigned short)value;
          pixel[1] = (unsigned short)value;
        }
    }
  texture->UnlockRect(0);
}


float HeightMap::height(float x, float z, bool useWorldSpace, bool useNormalizedSpace) const
{
  float y;
  int   xInteger, zInteger;
  float xFraction, zFraction;
  float samples[4];

  if(useWorldSpace)
    {
      // Convert from object space to local coordinate space
      x = ((float)_width * (x - _offset.x)) / _scale.x;
      z = ((float)_depth * (z - _offset.z)) / _scale.z;
    }
  if(useNormalizedSpace)
    {
      //NOTE: It makes no sense to have useWorldSpace = true and useNormalizedSpace = true
      x *= (float)_width;
      z *= (float)_depth;
    }

  // Bilinear blend samples
  xInteger = (int)floorf(x);
  zInteger = (int)floorf(z);
  xFraction = x - (float)xInteger;
  zFraction = z - (float)zInteger;
  assert((xFraction >= 0.0f) && (xFraction < 1.0f));
  assert((zFraction >= 0.0f) && (zFraction < 1.0f));
  samples[0] = (*this)(xInteger + 0, zInteger + 0, true);
  samples[1] = (*this)(xInteger + 1, zInteger + 0, true);
  samples[2] = (*this)(xInteger + 1, zInteger + 1, true);
  samples[3] = (*this)(xInteger + 0, zInteger + 1, true);

  y = ((1.0f - xFraction) * (1.0f - zFraction) * samples[0] +
       (       xFraction) * (1.0f - zFraction) * samples[1] +
       (       xFraction) * (       zFraction) * samples[2] +
       (1.0f - xFraction) * (       zFraction) * samples[3]);
  
  assert("this has not been tested\n");

  return(y);
}


void HeightMap::render( device, bool disableStateSetup)
{
  assert(device != 0);

  if(disableStateSetup == false)
    {
      device->SetTexture(0, _texture);
      device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
      device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
      device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
      device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
      device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
      device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
      device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
    }

  device->SetStreamSource(0, _vertexBuffer, 0, sizeof(Vertex));
  device->SetIndices(_indexBuffer);
  device->SetFVF(Vertex::FVF);
  device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (int)_data.size(), 0, 2 * (_width - 1) * (_depth - 1));

  if(disableStateSetup)
    {
      device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
    }
}

void HeightMap::setTexture(TextureID texture)
{
  assert(texture != 0);

  _texture = texture;
}

void HeightMap::computePosition(int x, int z, D3DXFROMWINEVECTOR3& position)
{
  position.x = (_scale.x * x / ((float)_width)) + _offset.x;
  position.y = (*this)(x, z);
  position.z = (_scale.z * z / ((float)_depth)) + _offset.z;
}

bool HeightMap::load(const char* filename)
{
  assert(filename != 0);
  assert(gDirect3DDevice9 != 0);

  TextureID texture;
  D3DSURFACE_DESC    description;
  int                width, depth;
  D3DFORMAT          format;
  D3DLOCKED_RECT     lockedRect;
  int            hr;
  int                x, z;
  unsigned char      *bits;
  unsigned char      sample;
  

  // Load the image and create a texture
  hr = D3DXFROMWINECreateTextureFromFileEx(gDirect3DDevice9, toUnicode(filename), 0, 0, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM,
                                   D3DXFROMWINE_DEFAULT, D3DXFROMWINE_DEFAULT, 0, 0, 0, &texture);
  if(hr != D3D_OK)
    {
      return(false);
    }

  // Get some info about the texture
  texture->GetLevelDesc(0, &description);
  format = description.Format;
  width = (int)description.Width;
  depth = (int)description.Height;

  // This code only works with this format
  assert(format == D3DFMT_A8R8G8B8);


  // Lock the data in the texture so we can read it
  hr = texture->LockRect(0, &lockedRect, 0, D3DLOCK_READONLY);
  assert(hr == D3D_OK);
  bits = (unsigned char *)lockedRect.pBits;


  // Copy the data into our internal data structures
  resize(width, depth);
  for(z=0; z<depth; z++)
    {
      for(x=0; x<width; x++)
        {
          sample = bits[(depth - z - 1) * lockedRect.Pitch + 4 * x];

          _data[z * width + x] = (((float)sample) / 255.0f - 0.5f) * _scale.y + _offset.y;
        }
    }

  // Remember to unlock the texture
  hr = texture->UnlockRect(0);

  // Remember to release, we don't need it anymore
  texture->Release();

  return(true);
}

void HeightMap::resize(int width, int depth)
{
  assert(width > 0);
  assert(depth > 0);

  _width = width;
  _depth = depth;
  _data.resize(_width * _depth);
}

