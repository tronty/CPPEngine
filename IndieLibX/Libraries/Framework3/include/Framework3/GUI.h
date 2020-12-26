//-----------------------------------------------------------------------------
// File: Framework\GUI.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#if 0//ndef _GUI_H_
#define _GUI_H_

/** \file
    GUI utilities.  This file contains the definitions of various GUI elements used by all the samples.
*/
#include <STX/Queue.h>
#include <Framework3/IRenderer.h>

#define LEFT_BUTTON   0
#define MIDDLE_BUTTON 1
#define RIGHT_BUTTON  2

/** Vertex structure for a 3D font */
struct FontVertex {
	D3DXFROMWINEVECTOR3 pos;
	unsigned int color;
};
struct FontVertex4 {
	D3DXFROMWINEVECTOR3 pos;
	D3DXFROMWINEVECTOR4 color;
};
/** A single character mesh */
struct Char {
	float ratio;

	unsigned int firstVertex;
	unsigned int firstIndex;
	unsigned int nVertices;
	unsigned int nIndices;
};

/** Defines horizontal text alignment when rendering out of the framework font classes */
enum FONT_HADJUST { HA_LEFT, HA_RIGHT, HA_CENTER };

/** Defines vertical text alignment when rendering out of the framework font classes */
enum FONT_VADJUST { VA_TOP, VA_TOP_UNDERLINE, VA_BOTTOM, VA_BOTTOM_UNDERLINE, VA_CENTER };

/** A 3D font mesh */
class RENDERERAPI Font3D {
public:
	Font3D();
	~Font3D();

	float getStringWidth(const char *string, int length = -1) const;
	float getLineWidth(const char *string) const;
	int getMaxCharsFitOnLine(const char *string, const float width, const bool wordBreak = false) const;

	void setSpacing(const float sx, const float sy);

	bool loadFromFile(const char *fileName);

	bool uploadVertexBuffers(IRenderer *renderer, const bool d3dStyle);

	void getExtents(const char *string, float x, float y, const float charWidth, const float charHeight, const float skewX = 0.0f, const float skewY = 0.0f, const FONT_HADJUST hAdjust = HA_LEFT, const FONT_VADJUST vAdjust = VA_TOP, float *left = 0, float *right = 0, float *top = 0, float *bottom = 0);
	void drawText(IRenderer *renderer, const char *string, float x, float y, const float charWidth, const float charHeight, const float skewX = 0.0f, const float skewY = 0.0f, const FONT_HADJUST hAdjust = HA_LEFT, const FONT_VADJUST vAdjust = VA_TOP);

protected:
	ShaderID shader;
	VertexFormatID vertexFormat;
	VertexBufferID vertexBuffer;
	IndexBufferID indexBuffer;

	FontVertex *vertices;
	ushort *indices;
	unsigned int nVertices;
	unsigned int nIndices;

	float spacingX, spacingY;
	float minY, maxY;

	Char chars[256];
};

//---------------------------------------------------------------------------------------------

/** The base class of all GUI widgets */
class RENDERERAPI Widget {
public:
	static Font3D* fnt;
	Widget(){
		visible = true;
		capture = false;
		dead = false;
	}
	virtual ~Widget(){}

	virtual void draw(IRenderer *renderer) = 0;
	virtual bool isInWidget(const int x, const int y) const = 0;
	virtual bool onMouseClick(const int x, const int y, const unsigned int button, const bool pressed){ return false; }
	virtual bool onMouseMove (const int x, const int y, const bool lButton, const bool mButton, const bool rButton){ return false; }
	virtual bool onMouseWheel(const int x, const int y, const int scroll){ return false; }
	virtual bool onKeyPress(const unsigned int key, const bool pressed){ return false; }
	virtual void onSize(const int w, const int h, const int oldW, const int oldH){}

	void setColor(const D3DXFROMWINEVECTOR4 &col){ color = col; }
	virtual void setPosition(const float newX, const float newY);
	void setVisible(const bool isVisible){ visible = isVisible; }
	bool isVisible() const { return visible; }
	bool isCapturing() const { return capture; }
	bool isDead() const { return dead; }
protected:
	D3DXFROMWINEVECTOR4 color;
	float xPos, yPos;
	bool capture;
	bool dead;
public:
		bool visible;

};

//---------------------------------------------------------------------------------------------

#define MENU_NONE ((unsigned int) (-1))

#define MENU_CHECKED   0x1
#define MENU_CHECKABLE 0x2
#define MENU_DISABLED  0x4

/** \defgroup hMenuRes Menu Resource Handles
    Handles to menu resources.
    @{
*/
typedef unsigned int MenuID;        /**< Menu handle. */
typedef unsigned int MenuItemID;    /**< Menu item handle. */
//@}

class RENDERERAPI Menu;

/** A menu listener listens for framework callbacks on menu interaction */
class RENDERERAPI MenuListener {
public:
	virtual void onMenuItemPress(Menu *menu, MenuItemID item) = 0;
};

/** Basic menu item structure */
struct RENDERERAPI MenuItem {
	char *text;
	unsigned int flags;
	unsigned int rbGroup;
};

/** This menu class represents a standard file/edit/view style menu */
class RENDERERAPI Menu : public Widget {
public:
	IRenderer* renderer;
	
	Menu(const float ix, const float iy, Font3D *fnt, const char *text, MenuListener *listener);
	~Menu();

	void initCurrentItem();

	MenuItemID addItem(const char *text, unsigned int flags = 0);
	void addSeparatorBefore(const MenuItemID before);
	void addSeparatorAfter(const MenuItemID after);
	void setRadioButtonGroup(const MenuItemID first, const MenuItemID last, const unsigned int group);
	MenuItemID getCheckedRadioButton(const unsigned int group) const;

	void setCheckBoxGroup(const MenuItemID first, const MenuItemID last, const unsigned int group);
	bool getCheckedCheckBox(const unsigned int group, MenuItemID id) const;

	bool isItemChecked(const MenuItemID menuItem) const
	{
		#ifdef ___GUIDUMMYTEST__
		return false;
		#endif 
		return ((items[menuItem].flags & MENU_CHECKED) != 0); 
	}
	void checkItem(const MenuItemID menuItem, const bool value);
	void toggleItemCheck(const MenuItemID menuItem);

	bool isItemEnabled(const MenuItemID menuItem) const
	{
		#ifdef ___GUIDUMMYTEST__
		return false;
		#endif 
		return ((items[menuItem].flags & MENU_DISABLED) == 0);
	}
	void enableItem(const MenuItemID menuItem, const bool value);
	void enableGroup(const unsigned int group, const bool value);

	std::string &getTitle(){ return title; }
	void setItemText(const MenuItemID menuItem, const char *text);

	void draw(IRenderer *renderer);
	bool isInWidget(const int x, const int y) const;

	bool onMouseClick(const int x, const int y, const unsigned int button, const bool pressed);
	bool onMouseMove (const int x, const int y, const bool lButton, const bool mButton, const bool rButton);
	bool onMouseWheel(const int x, const int y, const int scroll);
	bool onKeyPress(const unsigned int key, const bool pressed);
	void onSize(const int w, const int h, const int oldW, const int oldH);

protected:
	float getWidth() const;
	float getCheckSpace() const;

	VArray <MenuItem> items;
	VArray <MenuItemID> separators;
	std::string title;
	MenuItemID currItem;

	//Font3D *font;
	MenuListener *menuListener;

	unsigned int firstVisibleItem, nVisibleItems;
};

//---------------------------------------------------------------------------------------------

/** A menu system manages a collection of menus */
class RENDERERAPI MenuSystem : public Widget {
public:
	MenuSystem(const float ix, const float iy, Font3D *fnt, MenuListener *listener);
	~MenuSystem();

	Menu *getMenu(const MenuID menu) const { return menus[menu]; }
	unsigned int getMenuCount() const { return menus.getCount(); }

	Menu *addMenu(const char *text);
	void setPosition(const float newX, const float newY);

	void draw(IRenderer *renderer);
	bool isInWidget(const int x, const int y) const;
	bool isInMainMenu(const int x, const int y) const;

	bool onMouseClick(const int x, const int y, const unsigned int button, const bool pressed);
	bool onMouseMove (const int x, const int y, const bool lButton, const bool mButton, const bool rButton);
	bool onMouseWheel(const int x, const int y, const int scroll);
	bool onKeyPress(const unsigned int key, const bool pressed);
	void onSize(const int w, const int h, const int oldW, const int oldH);
	void setListener(MenuListener *listener){ menuListener = listener; }

protected:
	void setMenuPositions();
	int getHalfWidth() const;

	VArray <Menu *> menus;
	MenuID currMenu;

	////Font3D *font;

	MenuListener *menuListener;
};

//---------------------------------------------------------------------------------------------

class RENDERERAPI Slider;

/** A slider listener listens for framework callbacks on slider interaction */
class RENDERERAPI SliderListener {
public:
	virtual void onSliderChange(Slider *slider) = 0;
};

/** A basic slider bar widget */
class RENDERERAPI Slider : public Widget {
public:
	Slider(const float ix, const float iy, const float w, const float h, const bool horizontal = true, const float ir0 = 0, const float ir1 = 1, const float val = 0, const float stepSize = 0);

	void draw(IRenderer *renderer);
	bool isInWidget(const int x, const int y) const;
	bool onMouseClick(const int x, const int y, const unsigned int button, const bool pressed);
	bool onMouseMove (const int x, const int y, const bool lButton, const bool mButton, const bool rButton);

	void setRange(const float ir0, const float ir1){
		r0 = ir0;
		r1 = ir1;
	}
	void setSize(const float w, const float h){
        width  = w;
		height = h;
	}

	float getValue() const { return value; }
	void setValue(const float val);
	void setListener(SliderListener *listener){ sliderListener = listener; }

protected:
	void computeValue(const int x, const int y);

	float width, height;

	float r0, r1;
	float value;
	float step;

	SliderListener *sliderListener;

	bool isHorizontal;
	bool drag;
};

//---------------------------------------------------------------------------------------------

/** A simple text label */
class RENDERERAPI Label : public Widget {
public:
	Label(const float ix, const float iy, Font3D *fnt, const char *str, const float chrWidth = 24, const float chrHeight = 32,
	      const float skewx = 0, const float skewy = 0, FONT_HADJUST hAdj = HA_LEFT, FONT_VADJUST vAdj = VA_TOP);
	virtual ~Label();

	void setText(const char *str);

	void draw(IRenderer *renderer);
	bool isInWidget(const int x, const int y) const { return false; } // Accepts no input
protected:
	float charHeight, charWidth, skewX, skewY;
	FONT_HADJUST hAdjust;
	FONT_VADJUST vAdjust;
	std::string text;
	//Font3D *font;
};

//---------------------------------------------------------------------------------------------

/** A standard text box widget */
class RENDERERAPI TextBox : public Widget {
public:
	TextBox(const float ix, const float iy, const float w, const float h, Font3D *fnt, const char *str);
	virtual ~TextBox();

	void setText(const char *str);

	void draw(IRenderer *renderer);
	bool isInWidget(const int x, const int y) const;
protected:
	float width, height;
	std::string text;

	//Font3D *font;
};

//---------------------------------------------------------------------------------------------

class RENDERERAPI PushButton;

/** A pushbutton listener listens for framework callbacks on pushbutton interaction */
class RENDERERAPI PushButtonListener {
public:
	virtual void onPushButtonPress(PushButton *pushButton) = 0;
};

/** A standard button widget */
class RENDERERAPI PushButton : public TextBox {
public:
	PushButton(const float ix, const float iy, const float w, const float h, Font3D *fnt, const char *text);

	bool onMouseClick(const int x, const int y, const unsigned int button, const bool pressed);

	void setListener(PushButtonListener *listener){ pushButtonListener = listener; }

	void setPushed(const bool val);
	bool isPushed() const { return pushed; }
protected:
	PushButtonListener *pushButtonListener;

	bool pushed;
};

//---------------------------------------------------------------------------------------------

class RENDERERAPI KeyWaiter;

/** A pushbutton listener listens for framework callbacks on pushbutton interaction */
class RENDERERAPI KeyWaiterListener {
public:
	virtual void onKeyWaiterKeyPress(const unsigned int key) = 0;
};

/** A widget that waits for key input */
class RENDERERAPI KeyWaiter : public Widget {
public:
	KeyWaiter(const float ix, const float iy, Font3D *fnt, KeyWaiterListener *listener);
	virtual ~KeyWaiter();

	void draw(IRenderer *renderer);
	bool isInWidget(const int x, const int y) const;
	bool onKeyPress(const unsigned int key, const bool pressed);
protected:
	//Font3D *font;
	KeyWaiterListener *keyWaiterListener;
};

//---------------------------------------------------------------------------------------------

/** A standard message box widget */
class RENDERERAPI MsgBox : public Widget {
public:
	MsgBox(const float ix, const float iy, Font3D *fnt, const char *str);
	virtual ~MsgBox();

	void draw(IRenderer *renderer);
	bool isInWidget(const int x, const int y) const;
	bool onMouseClick(const int x, const int y, const unsigned int button, const bool pressed);
	bool onKeyPress(const unsigned int key, const bool pressed);
protected:
	void getRect(float *x0, float *y0, float *x1, float *y1) const;

	PushButton *okButton;
	std::string text;
	//Font3D *font;
};

//---------------------------------------------------------------------------------------------

/** \addtogroup hMenuRes
    @{
*/
typedef int ListItemID; /**<  A handle to a single item in a drop-down list */
//@}

class RENDERERAPI DropDownList;

/** A dropdown listener listens for framework callbacks on drop down list interaction */
class RENDERERAPI DropDownListener {
public:
	virtual void onDropDownListChange(DropDownList *dropDownList) = 0;
};

/** A standard drop-down list widget */
class RENDERERAPI DropDownList : public Widget {
public:
	DropDownList(const float ix, const float iy, const float w, const float h, Font3D *fnt);
	virtual ~DropDownList();

	ListItemID addItem(const char *string);
	ListItemID getSelectedItem() const { return selectedItem; }
	void setSelectedItem(ListItemID item);
	const char *getItem(const ListItemID item) const { return items[item]; }

	void setListener(DropDownListener *listener){ dropDownListener = listener; }

	void draw(IRenderer *renderer);
	bool isInWidget(const int x, const int y) const;
	bool onMouseClick(const int x, const int y, const unsigned int button, const bool pressed);
	bool onMouseWheel(const int x, const int y, const int scroll);
	bool onKeyPress(const unsigned int key, const bool pressed);
	void init();
protected:
	float width, height;
	//Font3D *font;

	VArray <char *> items;

	ListItemID selectedItem;
	ListItemID firstItem, lastItem;
	bool droppedDown;
	bool incDown, decDown;

	DropDownListener *dropDownListener;
	MenuSystem *mainMenu;

	Menu *optionsMenu, *helpMenu;
	MenuItemID optionsShowFPS, optionsVSync, optionsScreenshot, helpAbout;
#ifdef NO_CAPTURE_MOUSE
    bool invertMouse;
#else
	MenuItemID optionsInvertMouse;
	MenuItemID optionsForwardKey, optionsBackwardKey, optionsLeftKey, optionsRightKey, optionsUpKey, optionsDownKey;
	MenuItemID optionsResetKey, optionsFramerateKey, optionsMenuKey, optionsScreenshotKey;
	MenuItemID waitingForKeyItem;
#endif
};

#endif // _GUI_H_
