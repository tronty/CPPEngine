#ifdef LINUX
//-----------------------------------------------------------------------------
// File: Framework\GUI.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _Framework_GUI_H_
#define _Framework_GUI_H_

/** \file
    GUI utilities.  This file contains the definitions of various GUI elements used by all the samples.
*/

#include <Framework3/IRenderer.h>
#if 1//def R2VB_GUI
#define String std::string
#define Array VArray

#define LEFT_BUTTON   0
#define MIDDLE_BUTTON 1
#define RIGHT_BUTTON  2

/** Vertex structure for a 3D font */
struct FontVertex {
	vec3 pos;
	unsigned int color;
};

/** A single character mesh */
struct Char {
	float ratio;

	uint firstVertex;
	uint firstIndex;
	uint nVertices;
	uint nIndices;
};

/** Defines horizontal text alignment when rendering out of the framework font classes */
enum FONT_HADJUST { HA_LEFT, HA_RIGHT, HA_CENTER };

/** Defines vertical text alignment when rendering out of the framework font classes */
enum FONT_VADJUST { VA_TOP, VA_TOP_UNDERLINE, VA_BOTTOM, VA_BOTTOM_UNDERLINE, VA_CENTER };

#if 0
/** A 3D font mesh */
class FONT3D {
public:
	FONT3D();
	~FONT3D();

	float getStringWidth(char *string, int length = -1) const;
	float getLineWidth(char *string) const;
	int getMaxCharsFitOnLine(char *string, const float width, const bool wordBreak = false) const;

	void setSpacing(const float sx, const float sy);

	bool loadFromFile(const char *fileName);

	bool uploadVertexBuffers( const bool d3dStyle);

	void getExtents(char *string, float x, float y, const float charWidth, const float charHeight, const float skewX = 0.0f, const float skewY = 0.0f, const FONT_HADJUST hAdjust = HA_LEFT, const FONT_VADJUST vAdjust = VA_TOP, float *left = NULL, float *right = NULL, float *top = NULL, float *bottom = NULL);
	void drawText( char *string, float x, float y, const float charWidth, const float charHeight, const float skewX = 0.0f, const float skewY = 0.0f, const FONT_HADJUST hAdjust = HA_LEFT, const FONT_VADJUST vAdjust = VA_TOP);

protected:
	Char chars[256];

	float spacingX, spacingY;
	float minY, maxY;

	VertexFormatID vertexFormat;
	VertexBufferID vertexBuffer;
	IndexBufferID indexBuffer;

	FontVertex *vertices;
	ushort *indices;
	uint nVertices;
	uint nIndices;
};
#else
#define FONT3D Font
#endif
//---------------------------------------------------------------------------------------------

/** The base class of all GUI widgets */
class Widget {
public:
	Widget(){
		visible = true;
		capture = false;
		dead = false;
	}
	virtual ~Widget(){}

	virtual void draw() = 0;
	virtual bool isInWidget(const int x, const int y) const = 0;
	virtual bool onMouseClick(const int x, const int y, const unsigned int button, const bool pressed){ return false; }
	virtual bool onMouseMove (const int x, const int y, const bool lButton, const bool mButton, const bool rButton){ return false; }
	virtual bool onMouseWheel(const int x, const int y, const int scroll){ return false; }
	virtual bool onKeyPress(const unsigned int key, const bool pressed){ return false; }
	virtual void onSize(const int w, const int h, const int oldW, const int oldH){}

	void setColor(const vec4 &col){ color = col; }
	virtual void setPosition(const float newX, const float newY);
	void setVisible(const bool isVisible){ visible = isVisible; }
	bool isVisible() const { return visible; }
	bool isCapturing() const { return capture; }
	bool isDead() const { return dead; }
protected:
	vec4 color;
	float xPos, yPos;
	bool visible;
	bool capture;
	bool dead;
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

class Menu;

/** A menu listener listens for framework callbacks on menu interaction */
class MenuListener {
public:
	virtual void onMenuItemPress(Menu *menu, MenuItemID item){}
};

/** Basic menu item structure */
struct MenuItem {
	char *text;
	unsigned int flags;
	unsigned int rbGroup;
};

/** This menu class represents a standard file/edit/view style menu */
class Menu : public Widget {
public:
	Menu(const float ix=0.0f, const float iy=0.0f, FONT3D *fnt=0, char *text=0 /*, MenuListener *listener=0 */);
	~Menu();

	void initCurrentItem();

	MenuItemID addItem(char *text, unsigned int flags = 0);
	void addSeparatorBefore(const MenuItemID before);
	void addSeparatorAfter(const MenuItemID after);
	void setRadioButtonGroup(const MenuItemID first, const MenuItemID last, const unsigned int group);
	MenuItemID getCheckedRadioButton(const unsigned int group) const;

	bool isItemChecked(const MenuItemID menuItem) const { return ((items[menuItem].flags & MENU_CHECKED) != 0); }
	void checkItem(const MenuItemID menuItem, const bool value);
	void toggleItemCheck(const MenuItemID menuItem);

	bool isItemEnabled(const MenuItemID menuItem) const { return ((items[menuItem].flags & MENU_DISABLED) == 0); }
	void enableItem(const MenuItemID menuItem, const bool value);
	void enableGroup(const unsigned int group, const bool value);

	String &getTitle(){ return title; }
	void setItemText(const MenuItemID menuItem, char *text);

	void draw();
	bool isInWidget(const int x, const int y) const;

	bool onMouseClick(const int x, const int y, const unsigned int button, const bool pressed);
	bool onMouseMove (const int x, const int y, const bool lButton, const bool mButton, const bool rButton);
	bool onMouseWheel(const int x, const int y, const int scroll);
	bool onKeyPress(const unsigned int key, const bool pressed);
	void onSize(const int w, const int h, const int oldW, const int oldH);

protected:
	float getWidth() const;
	float getCheckSpace() const;

	Array <MenuItem> items;
	Array <MenuItemID> separators;
	String title;
	MenuItemID currItem;

	FONT3D *font;
	MenuListener *menuListener;

	uint firstVisibleItem, nVisibleItems;
};

//---------------------------------------------------------------------------------------------

/** A menu system manages a collection of menus */
class MenuSystem : public // ??? Widget
				Menu {
public:
	MenuSystem(const float ix=0.0f, const float iy=0.0f, FONT3D *fnt=0, MenuListener *listener=0);
	~MenuSystem();
	void addWidget(Widget *widget){ widgets.resize(1);widgets[0]=widget; } // ???

	Menu *getMenu(const MenuID menu) const { return menus[menu]; }
	uint getMenuCount() const { return menus.getCount(); }

	Menu *addMenu(char *text);
	void setPosition(const float newX, const float newY);

	void draw();
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

	Array <Menu *> menus;
	Array <Widget *> widgets; // ???
	MenuID currMenu;

	FONT3D *font;

	MenuListener *menuListener;
};

//---------------------------------------------------------------------------------------------

class Slider;

/** A slider listener listens for framework callbacks on slider interaction */
class SliderListener {
public:
	virtual void onSliderChange(Slider *slider) = 0;
};

/** A basic slider bar widget */
class Slider : public Widget {
public:
	Slider(const float ix, const float iy, const float w, const float h, const bool horizontal = true, const float ir0 = 0, const float ir1 = 1, const float val = 0, const float stepSize = 0);

	void draw();
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
class Label : public Widget {
public:
	Label(const float ix, const float iy, FONT3D *fnt, const char *str, const float chrWidth = 24, const float chrHeight = 32,
	      const float skewx = 0, const float skewy = 0, FONT_HADJUST hAdj = HA_LEFT, FONT_VADJUST vAdj = VA_TOP);
	virtual ~Label();

	void setText(char *str);

	void draw();
	bool isInWidget(const int x, const int y) const { return false; } // Accepts no input
protected:
	float charHeight, charWidth, skewX, skewY;
	FONT_HADJUST hAdjust;
	FONT_VADJUST vAdjust;
	String text;
	FONT3D *font;
};

//---------------------------------------------------------------------------------------------

/** A standard text box widget */
class TextBox : public Widget {
public:
	TextBox(const float ix, const float iy, const float w, const float h, FONT3D *fnt, char *str);
	virtual ~TextBox();

	void setText(char *str);

	void draw();
	bool isInWidget(const int x, const int y) const;
protected:
	float width, height;
	String text;

	FONT3D *font;
};

//---------------------------------------------------------------------------------------------

class PushButton;

/** A pushbutton listener listens for framework callbacks on pushbutton interaction */
class PushButtonListener {
public:
	virtual void onPushButtonPress(PushButton *pushButton) = 0;
};

/** A standard button widget */
class PushButton : public TextBox {
public:
	PushButton(const float ix, const float iy, const float w, const float h, FONT3D *fnt, char *text);

	bool onMouseClick(const int x, const int y, const unsigned int button, const bool pressed);

	void setListener(PushButtonListener *listener){ pushButtonListener = listener; }

	void setPushed(const bool val);
	bool isPushed() const { return pushed; }
protected:
	PushButtonListener *pushButtonListener;

	bool pushed;
};

//---------------------------------------------------------------------------------------------

class KeyWaiter;

/** A pushbutton listener listens for framework callbacks on pushbutton interaction */
class KeyWaiterListener {
public:
	virtual void onKeyWaiterKeyPress(const uint key) = 0;
};

/** A widget that waits for key input */
class KeyWaiter : public Widget {
public:
	KeyWaiter(const float ix, const float iy, FONT3D *fnt, KeyWaiterListener *listener);
	virtual ~KeyWaiter();

	void draw();
	bool isInWidget(const int x, const int y) const;
	bool onKeyPress(const unsigned int key, const bool pressed);
protected:
	FONT3D *font;
	KeyWaiterListener *keyWaiterListener;
};

//---------------------------------------------------------------------------------------------

/** A standard message box widget */
class MsgBox : public Widget {
public:
	MsgBox(const float ix, const float iy, FONT3D *fnt, char *str);
	virtual ~MsgBox();

	void draw();
	bool isInWidget(const int x, const int y) const;
	bool onMouseClick(const int x, const int y, const unsigned int button, const bool pressed);
	bool onKeyPress(const unsigned int key, const bool pressed);
protected:
	void getRect(float *x0, float *y0, float *x1, float *y1) const;

	PushButton *okButton;
	String text;
	FONT3D *font;
};

//---------------------------------------------------------------------------------------------

/** \addtogroup hMenuRes
    @{
*/
typedef int ListItemID; /**<  A handle to a single item in a drop-down list */
//@}

class DropDownList;

/** A dropdown listener listens for framework callbacks on drop down list interaction */
class DropDownListener {
public:
	virtual void onDropDownListChange(DropDownList *dropDownList) = 0;
};

/** A standard drop-down list widget */
class DropDownList : public Widget {
public:
	DropDownList(const float ix, const float iy, const float w, const float h, FONT3D *fnt);
	virtual ~DropDownList();

	ListItemID addItem(const char *string);
	ListItemID getSelectedItem() const { return selectedItem; }
	void setSelectedItem(ListItemID item);
	const char *getItem(const ListItemID item) const { return items[item]; }

	void setListener(DropDownListener *listener){ dropDownListener = listener; }

	void draw();
	bool isInWidget(const int x, const int y) const;
	bool onMouseClick(const int x, const int y, const unsigned int button, const bool pressed);
	bool onMouseWheel(const int x, const int y, const int scroll);
	bool onKeyPress(const unsigned int key, const bool pressed);
protected:
	float width, height;
	FONT3D *font;

	Array <char *> items;

	ListItemID selectedItem;
	ListItemID firstItem, lastItem;
	bool droppedDown;
	bool incDown, decDown;

	DropDownListener *dropDownListener;
};

#include <Util/Queue.h>

/** Base application class */
class BaseApp : public MenuListener, public KeyWaiterListener {
public:
	BaseApp();
	virtual ~BaseApp();

    /** Add a widget to the collection of widgets */
	static void addWidget(Widget *widget)
	{
		widgets.addFirst(widget);
	}

    /** Menu handler. This routine serves as the catch point for listening for menu item interaction.
        \param menu A pointer to the active menu
        \param item A (unique) handle to the active menu item
    */
	virtual void onMenuItemPress(Menu *menu, MenuItemID item);

    /** Key waiter handler. This routine serves as the catch point for key selection.
        \param key The key that was selected
    */
	virtual void onKeyWaiterKeyPress(const uint key);

    /** Mouse handler. This routine catches mouse events.
        \param x X position of the mouse in standard windowing (2D) coordinates
        \param y Y position of the mouse in standard windowing (2D) coordinates
        \param button The ID of the button clicked
        \param pressed Whether the button is depressed or released
    */
	virtual bool onMouseClick(const int x, const int y, const unsigned int button, const bool pressed);

    /** Mouse handler. This routine catches mouse movement events.
        \param x X position of the mouse in standard windowing (2D) coordinates
        \param y Y position of the mouse in standard windowing (2D) coordinates
        \param lButton Whether the left mouse button is pressed
        \param mButton Whether the middle mouse button is pressed
        \param rButton Whether the right mouse button is pressed
    */
	virtual bool onMouseMove (const int x, const int y, const bool lButton, const bool mButton, const bool rButton);

    /** Mouse handler. This routine catches mouse wheel events.
        \param x X position of the mouse in standard windowing (2D) coordinates
        \param y Y position of the mouse in standard windowing (2D) coordinates
        \param scroll Scroll direction
    */
	virtual bool onMouseWheel(const int x, const int y, const int scroll);

    /** Keyboard handler. This routine catches keyboard events.
        \param key Virtual key ID of active key
        \param pressed Whether the key is depressed
    */
	virtual bool onKeyPress(const unsigned int key, const bool pressed);

    /** Window sizing event handler.
        \param w New window width
        \param h New window height
    */
	virtual void onSize(const int w, const int h);

    /** Window repositioning event handler.
        \param xp New window horizontal position on screen
        \param yp New window vertical position on screen
    */
	virtual void onPosition(const int xp, const int yp);

	static MenuSystem* getMenu(){return mainMenu;}
	static void initGUI()
	{
		currMenu=-1;//???
		font=0;//???
	}
    //static void addWidget(Widget* aWidget){}
    static int getMainFont(){return font;}
protected:
	void setMenuPositions();
	int getHalfWidth() const;

	static VArray <Menu *> menus;
	static MenuID currMenu;

	static FONT3D *font;

	static MenuListener *menuListener;

	static MenuSystem *mainMenu;
    static Queue <Widget *> widgets;
};
#endif
#endif // _GUI_H_
#endif

