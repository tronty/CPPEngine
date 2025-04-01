/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#include "Mesh.h"
#include "Registers.h"
#include "Vertex.h"

#include <assert.h>

extern char* toUnicode(const char* string);

Mesh::Mesh(void)
{
  _d3dDevice = 0;
  _mesh = 0;
  D3DXFROMWINEMatrixIdentity(&_worldMatrix);
 
  D3DXFROMWINEMatrixScaling(&_worldMatrix, 0.1f, 0.1f, 0.1f);

  _worldMatrix = D3DXFROMWINEMATRIX(0.1f, 0.0f, 0.0f, 0.0f,
                            0.0f, 0.1f, 0.0f, 0.0f,
                            0.0f, 0.0f, 0.1f, 0.0f,
                            20.0f, 0.5f, 20.0f, 1.0f);
}

Mesh::~Mesh(void)
{
  release();
}

D3DXFROMWINEMATRIX& Mesh::getWorldMatrix(void)
{
  return(_worldMatrix);
}

void Mesh::initialize(const char* filename,  d3dDevice)
{
  assert(filename != 0);
  assert(d3dDevice != 0);

  int       hr;
  DWORD         numMaterials;
  int           i;
  LPD3DXBUFFER  materialBuffer;
  LPD3DXMESH    mesh;
  D3DXFROMWINEMATERIAL* materials;

  // If we already have mesh stuff, release it
  release();

  // Store the D3D device
  _d3dDevice = d3dDevice;

  // Load the mesh into system memory
  hr = D3DXFROMWINELoadMeshFromX(toUnicode(filename), D3DXFROMWINEMESH_SYSTEMMEM, d3dDevice, 0, &materialBuffer, 0, &numMaterials, &mesh);
  assert(hr == D3D_OK);

  // Create a clone of the mesh using our FVF
  hr = mesh->CloneMeshFVF(D3DXFROMWINEMESH_MANAGED, Vertex::FVF, d3dDevice, &_mesh);
  assert(hr == D3D_OK);

  // Throw away the system memory mesh
  if(mesh != 0)
    {
      mesh->Release();
      mesh = 0;
    }
  
  // Extract the materials
  materials = (D3DXFROMWINEMATERIAL*)materialBuffer->GetBufferPointer();

  for(i=0; i<(int)numMaterials; i++)
    {
      D3DMATERIAL9 material;
      
      material = materials[i].MatD3D;
      material.Ambient = material.Diffuse;

      _materials.push_back(material);
    }

  // Release the material buffer
  materialBuffer->Release();
}

void Mesh::release(void)
{
  if(_mesh != 0)
    {
      _mesh->Release();
    }
  _mesh = 0;
}

void Mesh::render(bool disableStateSetup)
{
  assert(_d3dDevice != 0);
  assert(_mesh != 0);
  
  int   i;
  
  // Render each subset of the mesh
  for(i=0; i<(int)_materials.size(); i++)
    {
      if(disableStateSetup)
        {
          _d3dDevice->SetMaterial(&_materials[i]);
        }

      _d3dDevice->SetVertexShaderConstantF(VS_MATERIAL_DIFFUSE, (float*)&_materials[i].Diffuse, 1);
      _d3dDevice->SetVertexShaderConstantF(VS_MATERIAL_AMBIENT, (float*)&_materials[i].Ambient, 1);

      _mesh->DrawSubset(i);
    }
}

void Mesh::setWorldMatrix(D3DXFROMWINEMATRIX& worldMatrix)
{
  _worldMatrix = worldMatrix;
}
