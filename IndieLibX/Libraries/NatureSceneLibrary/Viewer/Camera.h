#ifndef CAMERA_H
#define CAMERA_H

//include "../Math/MathUtils.h"
//include "../Events/KeyEvent.h"
#include <STX/d3dx9mathFromWine.h>
#include <Tools/MathUtils.h>

class NSCamera
{
  public:
    NSCamera(float positionX = 0.0f, float positionY = 0.0f, float positionZ = 0.0f,
           float focusX    = 0.0f, float focusY    = 0.0f, float focusZ    = 0.0f,
           float upVectorX = 0.0f, float upVectorY = 1.0f, float upVectorZ = 0.0f);

    void setup(const D3DXFROMWINEVECTOR3 &position,
               const D3DXFROMWINEVECTOR3 &focus,
               const D3DXFROMWINEVECTOR3 &up);

    void setup(float positionX, float positionY, float positionZ,
               float focusX   , float focusY   , float focusZ   ,
               float upVectorX, float upVectorY, float upVectorZ);

    void lockMouse(bool);
    void activate();
    void deactivate();

    void setKeyboardInput();
    void update(float timeFrame);
    void look();

    void     setMouseInfo(Tuple2i &mousePosition);
    void     setMouseInfo(int x = 0, int y = 0);

    void     setViewerPosition(const D3DXFROMWINEVECTOR3 &position);
    void     setFocusPosition (const D3DXFROMWINEVECTOR3 &focus);
    void     setUpDirection   (const D3DXFROMWINEVECTOR3 &up);

    void     setMouseSensitivity(float zeroToOne);
    void     setTranslationSpeed(float zeroToOne);

    const D3DXFROMWINEMATRIX &getModelViewMatrix() const;
    const D3DXFROMWINEVECTOR3  &getViewerPosition()  const;
    const D3DXFROMWINEVECTOR3  &getFocusPosition()   const;
    const D3DXFROMWINEVECTOR3  &getUpDirection()     const;
    void  elevate   (float speed);

  private:
    void processKeyboardEvents();
    void rotateAroundPoint(D3DXFROMWINEVECTOR3 &vCenter, float x, float y, float z);
    void setViewByMouse();
    void rotateView(float angle, float X, float Y, float Z);
    void strafeMove(float speed);
    void move      (float speed);

    Tuple2i  mouseLockedPosition,
             mousePosition;

    float    mouseSensitivity,
             translationSpeed,
             currentXRotation,
             speed;

    D3DXFROMWINEMATRIX viewMatrix;

    D3DXFROMWINEVECTOR3  position,
             strafe,
             focus,
             up;

    bool     activeKeys[6],
             mouseLocked,
             active;
};

#endif
