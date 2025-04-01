/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#ifndef VIEWER_H
#define VIEWER_H

#include <d3dx9.h>

class Viewer
{
public:
  Viewer(void);

  D3DXFROMWINEMATRIX& computeViewMatrix(D3DXFROMWINEMATRIX& viewMatrix);
  void        moveForward(float distance);
  void        moveRight(float distance);
  void        rotateLeftRight(float angle);
  void        rotateUpDown(float angle);
  

  // Accessor methods
  D3DXFROMWINEVECTOR3 getPosition(void) const;
  void        set(const D3DXFROMWINEVECTOR3& position, float yaw, float pitch);
  void        setHeight(float y);
  void        setPosition(const D3DXFROMWINEVECTOR3& position);

protected:
  D3DXFROMWINEVECTOR3 _position;
  float       _yaw;
  float       _pitch;
};

#endif
