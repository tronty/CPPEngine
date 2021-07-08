#include "Camera.h"
#include <Framework3/IRenderer.h>

NSCamera::NSCamera(float x , float y , float z,
               float xf, float yf, float zf,
               float xu, float yu, float zu)

{

  mouseSensitivity = 0.25f;
  currentXRotation = 0.0f;
  speed            = 0.0f;

  mouseLocked      = false;
  active           = true;

  stx_memset(activeKeys, false, 6);
  setup(x, y, z, xf, yf, zf, xu, yu, zu);
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

  setKeyboardInput();
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
  D3DXFROMWINEMatrixIdentity(&viewMatrix);
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

  float cosTheta = cosf(angle),
        sinTheta = sinf(angle);

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

void NSCamera::setMouseInfo(Tuple2i &mousePosition)
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

void NSCamera::setKeyboardInput()
{
  
  activeKeys[0] = false;
  activeKeys[1] = false;
  activeKeys[2] = false;
  activeKeys[3] = false;
  activeKeys[4] = false;
  activeKeys[5] = false;

  if(STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_w) || STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_UP))
	activeKeys[0] = true;
  if(STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_s) || STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_DOWN))
	activeKeys[1] = true;
  if(STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_a) || STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_LEFT))
	activeKeys[2] = true;
  if(STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_d) || STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_RIGHT))
	activeKeys[3] = true;
  if(STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_e))
	activeKeys[4] = true;
  if(STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_q))
	activeKeys[5] = true;
  
}

void NSCamera::setViewerPosition(const D3DXFROMWINEVECTOR3 &position_){ position = position_; }
void NSCamera::setFocusPosition (const D3DXFROMWINEVECTOR3 &focus_)   { focus    = focus_;    }
void NSCamera::setUpDirection   (const D3DXFROMWINEVECTOR3 &up_)      { up       = up_;       }

const D3DXFROMWINEMATRIX &NSCamera::getModelViewMatrix()const { return viewMatrix; }
const D3DXFROMWINEVECTOR3  &NSCamera::getViewerPosition() const { return position;   }
const D3DXFROMWINEVECTOR3  &NSCamera::getFocusPosition()  const { return focus;      }
const D3DXFROMWINEVECTOR3  &NSCamera::getUpDirection()    const { return up;         }
