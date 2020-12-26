#include <Framework3/IRenderer.h>
#include <STX/STXCPP.h>
#include <STX/CInput.h>
#include "GUITextBox.h"
#include "GUIFontManager.h"
#include "GUIFrame.h"

GUITextBox::GUITextBox(NSString callback,
                       NSString text) : GUIAlphaElement(callback)

{
  setBordersColor(0.3f, 0.3f, 0.3f);
  setDimensions(80, 18);
  setPadding(2, 2);
  setColor(0.5f, 0.5f, 0.5f);
  label.setString(text);

  blinkerPosition = 2;
  blinkerTimer    = 0.0f;
  blinkerOn       = false;

  textStartIndex  = 0;
  textEndIndex    = 0;

  dimensions.y    = GUIFontManager::getDefaultFont() ?
                    GUIFontManager::getDefaultFont()->getFontObject()->getHeight() + padding.y : dimensions.y;

  LOG_FNLN_NONE;
  LOG_PRINT_NONE("dimensions.y = %d\n", dimensions.y);

  widgetType      = TEXT_BOX;
}

bool GUITextBox::loadXMLSettings(XMLElement *element)
{
  if(!element || element->getName() != "TextBox")
    return false;//LOG_PRINT_NONE("Need a TextBox node in the xml file");

  XMLElement *child    = 0,
             *subChild = 0;

  if(child = element->getChildByName("BordersColor"))
    XMLElement::loadRX_GY_BZf(*child, bordersColor);

  setBordersColor(bordersColor);
  return   GUIAlphaElement::loadXMLSettings(element);
}

void  GUITextBox::setBordersColor(const D3DXFROMWINEVECTOR3& color)
{
  setBordersColor(color.x, color.y, color.z);
}

const D3DXFROMWINEVECTOR3 &GUITextBox::getBordersColor() const
{
  return bordersColor;
}

void  GUITextBox::setBordersColor(float r, float g, float b)
{
  bordersColor=D3DXFROMWINEVECTOR3(clampNS(r, 0.0f, 255.0f),
                   clampNS(g, 0.0f, 255.0f),
                   clampNS(b, 0.0f, 255.0f));
  bordersColor.x /= (bordersColor.x > 1.0) ? 255.0f : 1.0f;
  bordersColor.y /= (bordersColor.y > 1.0) ? 255.0f : 1.0f;
  bordersColor.z /= (bordersColor.z > 1.0) ? 255.0f : 1.0f;
}

void  GUITextBox::setPadding(const Tuple2i& p)
{
  setPadding(p.x, p.y);
}

void  GUITextBox::setPadding(int x, int y)
{
  padding.set(clampNS(x, 0, 25),
              clampNS(y, 0, 25));
}

const Tuple2i &GUITextBox::getPadding() const
{
  return padding;
}

void GUITextBox::render(float clockTick)
{
  if(!parent || !visible)
    return;

  modifyCurrentAlpha(clockTick);
  getWindowBounds();

  int realBlinkerPosition = clampNS(blinkerPosition,
                                  windowBounds.x  + padding.x,
                                  windowBounds.z  - padding.x);
  blinkerTimer += clockTick;
  blinkerTimer -= blinkerTimer > 1.0f ? 1.0f : 0.0f;

  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  IRenderer::GetRendererInstance()->Color4fv(color);

  IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLE_STRIP);
#if 1
  IRenderer::GetRendererInstance()->Vertex2i(windowBounds.x, windowBounds.y);
  IRenderer::GetRendererInstance()->Vertex2i(windowBounds.x, windowBounds.w);
  IRenderer::GetRendererInstance()->Vertex2i(windowBounds.z + padding.x, windowBounds.y);
  IRenderer::GetRendererInstance()->Vertex2i(windowBounds.z + padding.x, windowBounds.w);
#else
    IRenderer::GetRendererInstance()->Vertex2i(windowBounds.x, windowBounds.w);
  IRenderer::GetRendererInstance()->Vertex2i(windowBounds.x, windowBounds.y);
  IRenderer::GetRendererInstance()->Vertex2i(windowBounds.z + padding.x, windowBounds.w);
  IRenderer::GetRendererInstance()->Vertex2i(windowBounds.z + padding.x, windowBounds.y);
#endif

  IRenderer::GetRendererInstance()->End();

#if 0
  D3DXFROMWINEVECTOR3 col(1.0f, 1.0f, 1.0f);
#else
  D3DXFROMWINEVECTOR3 col=bordersColor;
#endif

#if 0//???
  IRenderer::GetRendererInstance()->Color3fv(&col.x);
  IRenderer::GetRendererInstance()->Begin(PRIM_LINE_LOOP);
  IRenderer::GetRendererInstance()->Vertex2i(windowBounds.x, windowBounds.y);
  IRenderer::GetRendererInstance()->Vertex2i(windowBounds.x, windowBounds.w);
  IRenderer::GetRendererInstance()->Vertex2i(windowBounds.z + padding.x, windowBounds.w);
  IRenderer::GetRendererInstance()->Vertex2i(windowBounds.z + padding.x, windowBounds.y);

  IRenderer::GetRendererInstance()->End();
#endif
  //glDisable(GL_BLEND);

  label.print(windowBounds.x + padding.x, windowBounds.y, textStartIndex, textEndIndex);

  //if(blinkerOn && (blinkerTimer > 0.5f))
  {

#if 0
  D3DXFROMWINEVECTOR3 col(0.0f, 0.0f, 0.0f);
#else
  D3DXFROMWINEVECTOR3 col=label.getColor();
#endif

#if 0//???
    IRenderer::GetRendererInstance()->Color3fv(&col.x);
    IRenderer::GetRendererInstance()->Begin(PRIM_LINES);
    IRenderer::GetRendererInstance()->Vertex2i(realBlinkerPosition, windowBounds.y + padding.y/2);
    IRenderer::GetRendererInstance()->Vertex2i(realBlinkerPosition, windowBounds.w - padding.y);

    IRenderer::GetRendererInstance()->End();
#endif
    IRenderer::GetRendererInstance()->Color3f(1,1,1);
  }
}

void GUITextBox::checkMouseEvents(int extraInfo, bool reservedBits)
{
  bool nRelease   = STX_Service::GetInputInstance()->IsMouseButtonReleased();

  GUIRectangle::checkMouseEvents(extraInfo, true);

  if(mouseOver && (STX_Service::GetInputInstance()->IsMouseButtonPressed()))
    setupBlinker(STX_Service::GetInputInstance()->GetMouseX());

  clicked   = false;
  released  = false;
  blinkerOn = focused ? blinkerOn : false;
}

void GUITextBox::checkKeyboardEvents(int extraInfo)
{
	#if 0
  if(!focused || !active)
    return;

  if((evt->getKeyID() >= 32 && evt->getKeyID() <= 127) ||
     (evt->getKeyID() == 8));
  else
    extraInfo = 0;

  if(evt->OnKeyPress(KEY_UNKNOWN+evt->getKeyID()))
	  //extraInfo == KEY_PRESSED)
  {
    int length = label.getString().getLength();

//???    if(evt->displayable())
      setupText(INSERT_CHAR, evt->getKeyChar());

    if(length)
    {
      switch(evt->getKeyID())
      {
        case   8:  setupText(BACKSPACE_DELETE); break;
        case 127:  setupText(SIMPLE_DELETE);    break;
      }
    }
  }

  label.forceUpdate(true);
  update = true;
  #endif
}

void    GUITextBox::setupBlinker(int x)
{
  if(!focused || !active)
    return;

  GUIFont *font           = GUIFontManager::getFont(label.getFontIndex());
  const    NSString &string = label.getString();
  const    int    *spaces = 0;
  blinkerPosition         = getWindowBounds().x + padding.x;
  x -= 1;

  if(font)
  {
     spaces = font->getFontObject()->getCharHorizontalGlyphs();

     for(size_t i = 0; i < string.getLength(); i++)
     if(blinkerPosition < x)
       blinkerPosition += spaces[int(string.getBytes()[i])];
  }

  blinkerOn = true;
}

void GUITextBox::setupText(int type, char Char)
{
  GUIFont *font           = GUIFontManager::getFont(label.getFontIndex());
  const    int    *spaces = font ? font->getFontObject()->getCharHorizontalGlyphs() : 0,
                   length = label.getString().getLength();

  LOG_FNLN_NONE;
  LOG_PRINT_NONE("spaces = %x\n", font->getFontObject()->getCharHorizontalGlyphs());
  LOG_PRINT_NONE("spaces = %d\n", *font->getFontObject()->getCharHorizontalGlyphs());
  NSString   temp;
  int      start  = windowBounds.x + padding.x,
           index  = 0;

  if(!spaces)
    return;

  for(int t = 0; t < length; t++)
  {
    if(blinkerPosition > start)
    {
      index++;
      start += spaces[label.getCharString()[t]];
    }
  }

  if(type == INSERT_CHAR)
  {
    if(index != length && length)
    {
      NSString leftSide;
      leftSide.set(label.getCharString(), index);
      leftSide += Char;
      temp.set(label.getCharString() + index, length - index);
      label.setString(leftSide + temp);
    }
    else
    {
      temp  = label.getString();
      temp += Char;
      label.setString(temp);
    }
    blinkerPosition  = blinkerPosition + GUIFontManager::getCharacterWidth(Char, font);
  }

  if(type == BACKSPACE_DELETE && (blinkerPosition != windowBounds.x + padding.x))
  {
    if(index != length)
    {
      NSString leftSide;
      setupBlinker(blinkerPosition - GUIFontManager::getCharacterWidth(label.getCharString()[index -1],
                                                                       label.getFontIndex()));

      leftSide.set(label.getCharString(), index - 1);
      temp.set(label.getCharString() + index, length - index);
      label.setString(leftSide + temp);
      return;
    }

    setupBlinker(blinkerPosition - GUIFontManager::getCharacterWidth(label.getCharString()[length -1],
                                                                     font));

    temp.set(label.getCharString(), length - 1);
    if(temp.getLength())
      label.setString(temp);
    else
    {
      label.clear();
      blinkerPosition = windowBounds.x + padding.x;
    }
  }

  if(type == SIMPLE_DELETE && length)
  {
    if((blinkerPosition == windowBounds.x + padding.x) && (length == 1))
    {
      label.clear();
      return;
    }

    if(index < length)
    {
      NSString leftSide;
      leftSide.set(label.getCharString(), index);
      temp.set(label.getCharString() + index + 1, length - index - 1);
      label.setString(leftSide + temp);
    }
  }
  if(type == PARSE_VISIBLE)
     textEndIndex = font->getFontObject()->getMaxFittingLength(label.getString(), getWidth());
}

const Tuple4i &GUITextBox::getWindowBounds()
{
  if(parent && update)
  {
    label.computeDimensions();
    dimensions.y    = label.getHeight() ? label.getHeight() + padding.y : dimensions.y;

  LOG_FNLN_NONE;
  LOG_PRINT_NONE("dimensions.y = %d\n", dimensions.y);

    GUIRectangle::computeWindowBounds();
    blinkerPosition = !blinkerPosition ? windowBounds.x + padding.x : blinkerPosition;
    setupText(PARSE_VISIBLE);
  }
  return windowBounds;
}
