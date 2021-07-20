#ifndef __CAMERA__H__
#define __CAMERA__H__

class NSCamera
{
  public:

	void updateClipPlanes(float nearClip, float farClip);
	const D3DXFROMWINEMATRIX &getProjectionMatrix() const;
	const D3DXFROMWINEMATRIX &GetMVP(D3DXFROMWINEMATRIX* matWorld=0);
    NSCamera();
    NSCamera(float positionX , float positionY , float positionZ ,
           float focusX    , float focusY    , float focusZ    ,
           float upVectorX , float upVectorY, float upVectorZ);

    void setup(const D3DXFROMWINEVECTOR3 &position,
               const D3DXFROMWINEVECTOR3 &focus,
               const D3DXFROMWINEVECTOR3 &up);

    void setup(float positionX, float positionY, float positionZ,
               float focusX   , float focusY   , float focusZ   ,
               float upVectorX, float upVectorY, float upVectorZ);

    void lockMouse(bool);
    void activate();
    void deactivate();

    void setKeyboardInput(bool active);
    void update(float timeFrame);
    void look();

    void     setMouseInfo(D3DXFROMWINEVECTOR2 &mousePosition);
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

    D3DXFROMWINEMATRIX viewMatrix, mProjectionMatrix;
	unsigned int MVPTranspose;
	unsigned int MVPInvert;

  private:
    void processKeyboardEvents();
    void rotateAroundPoint(D3DXFROMWINEVECTOR3 &vCenter, float x, float y, float z);
    void setViewByMouse();
    void rotateView(float angle, float X, float Y, float Z);
    void strafeMove(float speed);
    void move      (float speed);

    D3DXFROMWINEVECTOR2  mouseLockedPosition,
             mousePosition;

    float    mouseSensitivity,
             translationSpeed,
             currentXRotation,
             speed;

    D3DXFROMWINEVECTOR3  position,
             strafe,
             focus,
             up;

    bool     activeKeys[6],
             mouseLocked,
             active;
};

#endif

