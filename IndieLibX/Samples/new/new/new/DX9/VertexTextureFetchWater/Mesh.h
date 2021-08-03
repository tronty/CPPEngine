/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>

class Mesh
{
public:
  Mesh(void);
  ~Mesh(void);

  D3DXFROMWINEMATRIX& getWorldMatrix(void);
  void        initialize(const char* filename,  d3dDevice);
  void        release(void);
  void        render(bool disableStateSetup = false);
  void        setWorldMatrix(D3DXFROMWINEMATRIX& worldMatrix);
  
protected:
           _d3dDevice;
  LPD3DXMESH                _mesh;
  std::vector<D3DMATERIAL9> _materials;
  D3DXFROMWINEMATRIX                _worldMatrix;
};
