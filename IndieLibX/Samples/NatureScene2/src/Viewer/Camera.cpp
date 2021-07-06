#if 0
#include <Framework3/IRenderer.h>
#include "Camera.h"

	void NSCamera::updateClipPlanes(float nearClip, float farClip)
	{
		D3DXFROMWINEMatrixPerspectiveFovLH(&mProjectionMatrix, 90.0f, 1.0f*IRenderer::GetRendererInstance()->GetViewportWidth()/IRenderer::GetRendererInstance()->GetViewportHeight(), nearClip, farClip);
	}
	const D3DXFROMWINEMATRIX &NSCamera::getProjectionMatrix() const{return mProjectionMatrix;}

	const D3DXFROMWINEMATRIX &NSCamera::GetMVP(D3DXFROMWINEMATRIX* matWorld)
	{
		D3DXFROMWINEMATRIX mvp;
		if(matWorld)
			if(MVPInvert)
				mvp = mProjectionMatrix*viewMatrix*(*matWorld);
			else
				mvp = (*matWorld)*viewMatrix*mProjectionMatrix;
  		else
			if(MVPInvert)
				mvp = mProjectionMatrix*viewMatrix;
			else
				mvp = viewMatrix*mProjectionMatrix;
  		if(MVPTranspose)
			D3DXFROMWINEMatrixTranspose(&mvp, &mvp);
		return mvp;
	}

NSCamera::NSCamera()
{
  NSCamera(510.0f, 68.0f, 2210.0f,
               652.0f, 95.0f, 1670.0f,
                 0.0f,  1.0f,    0.0f);
}

NSCamera::NSCamera(float x , float y , float z,
               float xf, float yf, float zf,
               float xu, float yu, float zu)

{

  MVPTranspose=1;
  MVPInvert=0;

  mouseSensitivity = 0.25f;
  currentXRotation = 0.0f;
  speed            = 0.0f;

  mouseLocked      = false;
  active           = true;

  stx_memset(activeKeys, false, 6);
  setup(D3DXFROMWINEVECTOR3(x, y, z), D3DXFROMWINEVECTOR3(xf, yf, zf), D3DXFROMWINEVECTOR3(xu, yu, zu));
  setTranslationSpeed(0.5f);
}

void NSCamera::setup(const D3DXFROMWINEVECTOR3 &positionArg,
                   const D3DXFROMWINEVECTOR3 &focusArg,
                   const D3DXFROMWINEVECTOR3 &upArg)
{
  position = positionArg;
  focus    = focusArg;
  up       = upArg;
}

void NSCamera::setup(float x , float y , float z,
                   float xf, float yf, float zf,
                   float xu, float yu, float zu)
{
  position.set(x , y , z );
  focus.set   (xf, yf, zf);
  up.set      (xu, yu, zu);
}

void NSCamera::update(float frameInt)
{
  D3DXFROMWINEVECTOR3 temp  = focus;
          temp -= position;

  strafe.crossProduct(temp, up);
  strafe.normalize();

  speed  = frameInt*translationSpeed;

  setViewByMouse();
  processKeyboardEvents();
 
  //viewMatrix.lookAt(position, focus, up);
  D3DXFROMWINEMatrixLookAtLH(&viewMatrix, &position, &focus, &up);
  look();
}

void NSCamera::setMouseSensitivity(float sensitivity)
{
  mouseSensitivity = clamp(sensitivity, 0.0f, 1.0f);
}

void NSCamera::setTranslationSpeed(float speed)
{
  translationSpeed  = clamp(speed, 0.0f, 1.0f);
  translationSpeed *= 300.0f;
}

void NSCamera::lockMouse(bool value)
{
  if(value)
    mouseLockedPosition = mousePosition;

  mouseLocked = value;
}

void NSCamera::move(float speed)
{
  D3DXFROMWINEVECTOR3 temp = focus;

  temp -=position;
  temp.normalize();

  temp *= speed;

  position += temp;
  focus    += temp;
}

void NSCamera::look()
{
  //glMatrixMode(GL_MODELVIEW);
  //glLoadMatrixf(viewMatrix);
}

void NSCamera::strafeMove(float speed)
{
  position.x += strafe.x * speed;
  position.z += strafe.z * speed;

  focus.x += strafe.x * speed;
  focus.z += strafe.z * speed;
}

void NSCamera::rotateView(float angle, float x, float y, float z)
{
  D3DXFROMWINEVECTOR3 newView,
          vView = focus;

  float cosTheta = cos(angle),
        sinTheta = sin(angle);

  vView -= position;

  // Find the new x position for the new rotated point
  newView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
  newView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* vView.y;
  newView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* vView.z;

  // Find the new y position for the new rotated point
  newView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.x;
  newView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
  newView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.z;

  // Find the new z position for the new rotated point
  newView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.x;
  newView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.y;
  newView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;

  // Now we just add the newly rotated vector to our position to set
  // our new rotated view of our camera.
  focus  = position;
  focus += newView;
}

void NSCamera::elevate(float speed)
{
  position.y  += speed;
  focus.y     += speed;
}

void NSCamera::setViewByMouse()
{
  float          angleY        = 0.0f,
                 angleZ        = 0.0f;

  if((mousePosition == mouseLockedPosition) || !mouseLocked)
	  return;

  angleY = float(mouseLockedPosition.x - mousePosition.x)*DEG2RAD*mouseSensitivity;
  angleZ = float(mouseLockedPosition.y - mousePosition.y)*DEG2RAD*mouseSensitivity;

  currentXRotation -= angleZ;
  D3DXFROMWINEVECTOR3 vAxis     = focus;
          vAxis    -= position;
         (vAxis ^= up).normalize();

  rotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
  rotateView(angleY, 0, 1, 0);

  if(mouseLocked)
    mouseLockedPosition = mousePosition;
}

void NSCamera::setMouseInfo(D3DXFROMWINEVECTOR2 &mousePosition)
{
  setMouseInfo(mousePosition.x, mousePosition.y);
}

void NSCamera::setMouseInfo(int x, int y)
{
  mousePosition.set(x, y);
}

void NSCamera::processKeyboardEvents()
{
  if(activeKeys[0]) move( speed);
  if(activeKeys[1]) move(-speed);
  if(activeKeys[2]) strafeMove(-speed);
  if(activeKeys[3]) strafeMove( speed);
  if(activeKeys[4]) elevate( speed);
  if(activeKeys[5]) elevate(-speed);
}

void NSCamera::setKeyboardInput(bool active)
{
  if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_w) ||
  STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_UP)) activeKeys[0] = active;
  if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_s) ||
  STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_DOWN)) activeKeys[1] = active;
  if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_a) ||
  STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_LEFT)) activeKeys[2] = active;
  if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_d) ||
  STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_RIGHT)) activeKeys[3] = active;
  if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_e)) activeKeys[4] = active;
  if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_q)) activeKeys[5] = active;

}

void NSCamera::setViewerPosition(const D3DXFROMWINEVECTOR3 &position_){ position = position_; }
void NSCamera::setFocusPosition (const D3DXFROMWINEVECTOR3 &focus_)   { focus    = focus_;    }
void NSCamera::setUpDirection   (const D3DXFROMWINEVECTOR3 &up_)      { up       = up_;       }

const D3DXFROMWINEMATRIX &NSCamera::getModelViewMatrix()const { return viewMatrix; }
const D3DXFROMWINEVECTOR3  &NSCamera::getViewerPosition() const { return position;   }
const D3DXFROMWINEVECTOR3  &NSCamera::getFocusPosition()  const { return focus;      }
const D3DXFROMWINEVECTOR3  &NSCamera::getUpDirection()    const { return up;         }
#endif

