#include "Event.h"


MouseEvent::MouseEvent()
{
  scrolling = 0;
  buttonID  = NOBUTTON;
  x         = 0;
  y         = 0;
  yInverse  = 0;
}

MouseEvent::MouseEvent(int bID, int xMouse, int yMouse, int invYMouse, int scroll)
{
  scrolling = scroll,
  buttonID  = bID;
  x         = xMouse;
  y         = yMouse;
  yInverse  = invYMouse;
}

MouseEvent::MouseEvent(const MouseEvent& copy)
{
  this->operator =(copy);
}

MouseEvent &MouseEvent::operator= (const MouseEvent& copy)
{
  if(this != &copy)
  {
    scrolling = copy.scrolling;
    buttonID  = copy.buttonID;
    yInverse  = copy.yInverse;
    x         = copy.x;
    y         = copy.y;
  }
  return *this;
}

void MouseEvent::consume()
{
  scrolling = 0;
  buttonID  = NOBUTTON;
  x         = 0;
  y         = 0;
  yInverse  = 0;
}

int MouseEvent::getScrolling(){ if(STX_Service::GetInputInstance()->OnMouseButtonPress(MBUTTON_WHEELUP))
					return SCROLLING_UP;
				else if(STX_Service::GetInputInstance()->OnMouseButtonPress(MBUTTON_WHEELDOWN))
					return SCROLLING_DOWN;
				return NOSCROLLING; }
int MouseEvent::getButtonID() { if(STX_Service::GetInputInstance()->OnMouseButtonPress(MBUTTON_LEFT))
					return BUTTON1;
				else if(STX_Service::GetInputInstance()->OnMouseButtonPress(MBUTTON_MIDDLE))
					return BUTTON2;
				else if(STX_Service::GetInputInstance()->OnMouseButtonPress(MBUTTON_RIGHT))
					return BUTTON3;
				return NOBUTTON;   }
int MouseEvent::getYInverse() { return yInverse;   }
int MouseEvent::getX()        { return STX_Service::GetInputInstance()->GetMouseX();          }
int MouseEvent::getY()        { return STX_Service::GetInputInstance()->GetMouseY();          }
