//-----------------------------------------------------------------------------
// File: Framework\GUI.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
#include <Util/GUI.h>

VArray <Menu *> BaseApp::menus;
MenuID BaseApp::currMenu=-1;
FONT3D *BaseApp::font=0;
#if 0
MenuListener *BaseApp::menuListener=0;// ??? new MenuListener(0.0f, 0.0f, font /* , this */);
MenuSystem *BaseApp::mainMenu=0; // ??? new MenuSystem(400, 36, font, /* this */);
#else
MenuListener *BaseApp::menuListener=new MenuListener();
MenuSystem *BaseApp::mainMenu=new MenuSystem();
#endif
Queue <Widget *> BaseApp::widgets;

float GUI_cpp__getStringWidth(const char* aText){return 0.0f;}

#if 0
FONT3D::FONT3D(){
	memset(chars, 0, sizeof(chars));

	spacingX = 0.1f;
	spacingY = 0.1f;

	vertices = NULL;
	indices  = NULL;
}

FONT3D::~FONT3D(){
	delete vertices;
	delete indices;
}

bool FONT3D::loadFromFile(const char *fileName){
	std::string fn=stx_convertpath(fileName);
	FILE *file = fopen(fn.c_str(), "rb");
	if (file == NULL) return false;

	unsigned int nChars = 0;
	fread(&nChars, 4, 1, file);

	unsigned char *fChars = new unsigned char[nChars];
	fread(fChars, nChars, 1, file);

	fread(&minY, 4, 1, file);
	fread(&maxY, 4, 1, file);

	nVertices = 0;
	nIndices = 0;
	for (unsigned int j = 0; j < nChars; j++){
		unsigned int c = fChars[j];

		fread(&chars[c], sizeof(Char), 1, file);

		nVertices += chars[c].nVertices;
		nIndices += chars[c].nIndices;
	}

	vertices = new FontVertex[nVertices];
	fread(vertices, nVertices, sizeof(FontVertex), file);

	indices = new ushort[nIndices];
	fread(indices, nIndices, sizeof(ushort), file);

	fclose(file);
	delete fChars;

	chars[32].ratio = 0.25f;

	return false;
}

bool FONT3D::uploadVertexBuffers( const bool d3dStyle){
	VertexAttribute attribs[] = {
		TYPE_VERTEX, FORMAT_FLOAT, 3,
		TYPE_COLOR, d3dStyle? FORMAT_D3DCOLOR : FORMAT_UBYTE, 4,
	};
	if ((vertexFormat = IRenderer::GetRendererInstance()->addVertexFormat(attribs, elementsOf(attribs))) == VF_NONE) return false;

	if ((vertexBuffer = IRenderer::GetRendererInstance()->addVertexBuffer(nVertices * sizeof(FontVertex), vertices)) == VB_NONE) return false;
	if ((indexBuffer = IRenderer::GetRendererInstance()->addIndexBuffer(nIndices, 2, indices)) == IB_NONE) return false;

	return true;
}

void FONT3D::getExtents(char *string, float x, float y, const float charWidth, const float charHeight, const float skewX, const float skewY, const FONT_HADJUST hAdjust, const FONT_VADJUST vAdjust, float *left, float *right, float *top, float *bottom){
	char *str = string;
	int n = 0;
	float fw = 0;
	// Count the number of lines in the string and find maximum width
	while (true){
		char *newline = str;
		while (*newline && *newline != '\n') newline++;

		float w = GUI_cpp__getStringWidth(str, int(newline - str));
		if (w > fw) fw = w;

		n++;
		if (!*newline) break;

		str = newline + 1;
	}
	fw += fabsf(skewX);
	fw *= charWidth;

	// Compute total height
	float fh = (n * (1 + spacingY) + fabsf(skewY)) * charHeight;
	// Adjust y coordinate according to vertical adjust mode
	if (skewY < 0){
		if (vAdjust == VA_TOP_UNDERLINE) y += charHeight * skewY;
	} else {
		if (vAdjust == VA_BOTTOM_UNDERLINE) y += charHeight * skewY;
	}
	if (vAdjust == VA_TOP_UNDERLINE) y += charHeight * minY;
	if (vAdjust == VA_BOTTOM) y -= fh;
	if (vAdjust == VA_BOTTOM_UNDERLINE) y -= fh - maxY * charHeight;
	if (vAdjust == VA_CENTER) y -= 0.5f * fh;

	// Adjust x coordinate according to horizontal adjust mode
	float lShiftFactor = (hAdjust == HA_LEFT)? 0 : (hAdjust == HA_CENTER)? 0.5f : 1;
	float l = x - fw * lShiftFactor;
	float r = l + fw;
	float t = y;
	float b = y + fh;

	if (left   != NULL) *left   = l;
	if (right  != NULL) *right  = r;
	if (top    != NULL) *top    = t;
	if (bottom != NULL) *bottom = b;
}

float FONT3D::GUI_cpp__getStringWidth(char *string, int length) const {
	if (length < 0) length = (int) strlen(string);

	float w = 0;
	if (length > 0){
		for (int i = 0; i < length; i++){
			w += chars[(unsigned char) string[i]].ratio;
		}
		w += (length - 1) * spacingX;
	}

	return w;
}

float FONT3D::getLineWidth(char *string) const {
	char *str = strchr(string, '\n');
	return GUI_cpp__getStringWidth(string, (str != NULL)? int(str - string) : -1);
}

int FONT3D::getMaxCharsFitOnLine(char *string, const float width, const bool wordBreak) const {
	int lastWhiteSpace = 0;
	int i = 0;
	float w = 0;

	while (string[i]){
		if (string[i] == '\n') return i;
		if (string[i] == ' ' || string[i] == '\t'){
			lastWhiteSpace = i;
		}
		w += chars[(unsigned char) string[i]].ratio;
		if (w > width){
			return (lastWhiteSpace == 0 || !wordBreak)? i : lastWhiteSpace;
		}

		w += spacingX;
		i++;
	}
	return i;
}

void FONT3D::setSpacing(const float sx, const float sy){
	spacingX = sx;
	spacingY = sy;
}

void FONT3D::drawText( char *string, float x, float y, const float charWidth, const float charHeight, const float skewX, const float skewY, const FONT_HADJUST hAdjust, const FONT_VADJUST vAdjust){
	float lShiftFactor = (hAdjust == HA_LEFT)? 0 : (hAdjust == HA_CENTER)? 0.5f : 1;

	// Adjust x and y according to adjust modes
	if (skewX < 0){
		x -= (1 - lShiftFactor) * skewX * charWidth;
	} else {
		x -= lShiftFactor * skewX * charWidth;
	}
	if (skewY < 0){
		if (vAdjust == VA_TOP) y -= skewY * charHeight;
	} else {
		if (vAdjust == VA_BOTTOM) y -= skewY * charHeight;
	}
	if (vAdjust == VA_CENTER) y -= 0.5f * skewY * charHeight;

	float startX = x;
	if (hAdjust != HA_LEFT) x -= lShiftFactor * getLineWidth(string) * charWidth;
	if (vAdjust == VA_TOP) y -= charHeight * minY;
	if (vAdjust == VA_BOTTOM) y -= charHeight * maxY;
	if (vAdjust >= VA_BOTTOM){
		int n = 0;
		char *str = string;
		while (*str){
			if (*str == '\n') n++;
			str++;
		}
		if (vAdjust == VA_CENTER){
			y -= (n * (1 + spacingY) + minY + maxY) * charHeight / 2;
		} else {
			y -= n * (1 + spacingY) * charHeight;
		}
	}

	mat4 skew = identity4();
	skew.elem[0][2] = skewX;
	skew.elem[1][2] = skewY;

	IRenderer::GetRendererInstance()->changeVertexFormat(vertexFormat);
	IRenderer::GetRendererInstance()->changeVertexBuffer(vertexBuffer);
	IRenderer::GetRendererInstance()->changeIndexBuffer(indexBuffer);

	while (*string){
		if (*string == '\n'){
			y += (1 + spacingY) * charHeight;
			x = startX;
			if (hAdjust != HA_LEFT) x -= lShiftFactor * getLineWidth(string + 1) * charWidth;
		} else {
			Char chr = chars[*(ubyte *) string];
			float cw = charWidth * chr.ratio;

			if (chr.nIndices > 0){
				mat4 mat = translate(vec3(x, y, 0)) * scale4(charWidth, charHeight, 1) * skew;
				IRenderer::GetRendererInstance()->changeModelViewMatrix(mat);

				IRenderer::GetRendererInstance()->drawElements(TRIANGLES, chr.firstIndex, chr.nIndices, chr.firstVertex, chr.nVertices);
			}

			x += cw + charWidth * spacingX;
		}
		string++;
	}

	IRenderer::GetRendererInstance()->changeModelViewMatrix(identity4());
}
#else

void Font3D__drawText(const char *string, float x, float y, const float charWidth, const float charHeight, const float skewX, const float skewY, const FONT_HADJUST hAdjust, const FONT_VADJUST vAdjust)
{
		IRenderer::GetRendererInstance()->
drawText(string, x, y-13,
		 //15, 18,
		//charWidth/2, charHeight/2,
		charWidth, charHeight,
		IRenderer::GetRendererInstance()->GetdefaultFont(),
		IRenderer::GetRendererInstance()->GetlinearClamp(),
		IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
		IRenderer::GetRendererInstance()->GetnoDepthTest());}
#endif
#if 1
////////////////////////////////////////////////////////////////////////////////////////////

void Widget::setPosition(const float newX, const float newY){
	xPos = newX;
	yPos = newY;
}

Menu::Menu(const float ix, const float iy, FONT3D *fnt, char *text /* , MenuListener *listener */ ) : title(text?text:""){
	currItem = 0;
	firstVisibleItem = 0;
	nVisibleItems = 0;
	font = fnt;

	xPos = ix;
	yPos = iy;

	menuListener = (MenuListener*)this;// ??? listener;
}

Menu::~Menu(){
	for (uint i = 0; i < items.getCount(); i++){
		delete items[i].text;
	}
}

void Menu::initCurrentItem(){
	for (uint i = 0; i < items.getCount(); i++){
		if (items[i].rbGroup){
			if (items[i].flags & MENU_CHECKED){
				currItem = i;
				break;
			}
		} else break;
	}
}

MenuItemID Menu::addItem(char *text, unsigned int flags){
	MenuItem menuItem;
	menuItem.flags = flags;
	menuItem.rbGroup = 0;

	color = vec4(0, 0.5f, 1, 0.4f);

	size_t length = strlen(text);
	menuItem.text = new char[length + 1];
	strcpy(menuItem.text, text);

	return items.add(menuItem);
}

void Menu::addSeparatorBefore(const MenuItemID before){
	separators.add(before - 1);
}

void Menu::addSeparatorAfter(const MenuItemID after){
	separators.add(after);
}

void Menu::setRadioButtonGroup(const MenuItemID first, const MenuItemID last, const unsigned int group){
	for (unsigned int i = first; i <= last; i++){
		items[i].rbGroup = group;
	}
}

MenuItemID Menu::getCheckedRadioButton(const unsigned int group) const {
	for (unsigned int i = 0; i < items.getCount(); i++){
		if (items[i].rbGroup == group){
			if (items[i].flags & MENU_CHECKED) return i;
		}
	}
	return MENU_NONE;
}

void Menu::checkItem(const MenuItemID menuItem, const bool value){
	if(menuItem<0)
		return;
	if(menuItem>=items.getCount())
		return;
	if (value){
		items[menuItem].flags |= MENU_CHECKED;
	} else {
		items[menuItem].flags &= ~MENU_CHECKED;
	}
}

void Menu::toggleItemCheck(const MenuItemID menuItem){
	if (items[menuItem].flags & MENU_CHECKABLE){
		unsigned int group = items[menuItem].rbGroup;
		if (group != 0){
			// Set checked to false for all other items in the group
			for (unsigned int i = 0; i < items.getCount(); i++){
				if (items[i].rbGroup == group){
					items[i].flags &= ~MENU_CHECKED;
				}
			}
			items[menuItem].flags |= MENU_CHECKED;
		} else {
			items[menuItem].flags ^= MENU_CHECKED;
		}
	}
}

void Menu::enableItem(const MenuItemID menuItem, const bool value){
	if(menuItem<0)
		return;
	if(menuItem>=items.getCount())
		return;
	if (value){
		items[menuItem].flags &= ~MENU_DISABLED;
	} else {
		items[menuItem].flags |= MENU_DISABLED;
	}
}

void Menu::enableGroup(const unsigned int group, const bool value){
	for (unsigned int i = 0; i < items.getCount(); i++){
		if (items[i].rbGroup == group){
			enableItem(i, value);
		}
	}
}

void Menu::setItemText(const MenuItemID menuItem, char *text){
	delete items[menuItem].text;

	items[menuItem].text = new char[strlen(text) + 1];
	strcpy(items[menuItem].text, text);
}

void Menu::draw(){
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	IRenderer::GetRendererInstance()->setBlending(eSRC_ALPHA, eONE_MINUS_SRC_ALPHA);

	float mWidth = getWidth();
	float checkSpace = getCheckSpace();

	float xRight = xPos + mWidth;
	float yTop = yPos;
	float yBottom = yPos + nVisibleItems * 32 + 10;
	if (nVisibleItems < items.getCount()){
		yTop += 12;
		yBottom += 24;
	}

	// Menu background
	IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos, xRight, yBottom, 8, color);
	vec4 black(0, 0, 0, 1);
	IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos, xRight, yBottom, 8, black, 3);

	if (nVisibleItems < items.getCount()){
		float xMid = xPos + mWidth / 2;
		if (firstVisibleItem > 0){
			IRenderer::GetRendererInstance()->drawTriangle(xMid, yPos + 4, xMid + 7, yPos + 11, xMid - 7, yPos + 11, black);
		}
		if (firstVisibleItem + nVisibleItems < items.getCount()){
			IRenderer::GetRendererInstance()->drawTriangle(xMid, yBottom - 4, xMid - 7, yBottom - 11, xMid + 7, yBottom - 11, black);
		}
	}

	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	
	for (unsigned int i = 0; i < nVisibleItems; i++){
		MenuItemID item = firstVisibleItem + i;

		vec4 col;
		if (items[item].flags & MENU_DISABLED){
			col = vec4(0.5f, 0.5f, 0.5f, 1);
		} else {
			col = vec4(1, 1, 1, 1);
		}
		if (items[item].flags & MENU_CHECKABLE){
			if (items[item].rbGroup){
				// Radio button
				IRenderer::GetRendererInstance()->drawCircle(xPos + 17, yTop + 20 + i * 32, 7, col, 2);

				if (items[item].flags & MENU_CHECKED){
					// Radio button check
					IRenderer::GetRendererInstance()->drawCircle(xPos + 17, yTop + 20 + i * 32, 4, col);
				}
			} else {
				// Checkbox
				IRenderer::GetRendererInstance()->drawRect(xPos + 8, yTop + 11 + i * 32, xPos + 24, yTop + 28 + i * 32, col, 2);
				if (items[item].flags & MENU_CHECKED){
					// Check
					IRenderer::GetRendererInstance()->drawLine(xPos + 11, yTop + 18 + i * 32, xPos + 15, yTop + 25 + i * 32, col, 2);
					IRenderer::GetRendererInstance()->drawLine(xPos + 15, yTop + 25 + i * 32, xPos + 21, yTop + 14 + i * 32, col, 2);
				}
			}
		}
	}

	// Menu item separators
	vec4 darkBlue(0, 0, 0.3f, 1);
	for (unsigned int i = 0; i < separators.getCount(); i++){
		if (separators[i] >= firstVisibleItem && separators[i] < firstVisibleItem + nVisibleItems - 1){
			float y = yTop + (separators[i] - firstVisibleItem) * 32 + 36;
			IRenderer::GetRendererInstance()->drawLine(xPos + 11, y, xRight - 11, y, darkBlue, 2);
		}
	}


	IRenderer::GetRendererInstance()->setCullFace(eBACK);
	IRenderer::GetRendererInstance()->setBlending(eCONSTANT_COLOR, eZERO);
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);

	for (unsigned int i = 0; i < nVisibleItems; i++){
		MenuItemID item = firstVisibleItem + i;

		if (currItem == item){
			if (items[item].flags & MENU_DISABLED){
				IRenderer::GetRendererInstance()->changeBlendConstantColor(vec4(0.5f, 0.15f, 0.2f, 0));
			} else {
				IRenderer::GetRendererInstance()->changeBlendConstantColor(vec4(1, 0.2f, 0.3f, 0));
			}
		} else {
			if (items[item].flags & MENU_DISABLED){
				IRenderer::GetRendererInstance()->changeBlendConstantColor(vec4(0.5f, 0.5f, 0.5f, 0));
			} else {
				IRenderer::GetRendererInstance()->changeBlendConstantColor(vec4(1, 1, 1, 0));
			}
		}
		Font3D__drawText(items[item].text, xPos + checkSpace + 9.0f, yTop + i * 32 + 21.0f, 20, 26, 0.15f, -0.1f, HA_LEFT, VA_CENTER);
		
		
		
		
	}
}

bool Menu::isInWidget(const int x, const int y) const {
	return (x >= xPos && x <= xPos + getWidth() && y >= yPos && y <= yPos + (int) items.getCount() * 32 + 10);
}

bool Menu::onMouseClick(const int x, const int y, const unsigned int button, const bool pressed){
	if (pressed && button == LEFT_BUTTON){
		if (nVisibleItems < items.getCount()){
			int ny = y - int(yPos) - 5;
			ny -= 12;
			if (ny < 0) return true;
			unsigned int item = ny / 32;
			if (item >= nVisibleItems) return true;
		}

		if (isInWidget(x, y) && ((items[currItem].flags & MENU_DISABLED) == 0)){
			toggleItemCheck(currItem);
			menuListener->onMenuItemPress(this, currItem);
		}
	}
	return true;
}

bool Menu::onMouseMove(const int x, const int y, const bool lButton, const bool mButton, const bool rButton){
	if (isInWidget(x, y)){
		int ny = y - int(yPos) - 5;
		if (nVisibleItems < items.getCount()) ny -= 12;
		if (ny < 0){
			if (firstVisibleItem) firstVisibleItem--;
		} else {
			unsigned int item = ny / 32;
			if (item >= nVisibleItems){
				if (firstVisibleItem + nVisibleItems < items.getCount()) firstVisibleItem++;
			} else {
				currItem = item + firstVisibleItem;
			}
		}
	}
	return true;
}

bool Menu::onMouseWheel(const int x, const int y, const int scroll){
	if (scroll > (int) firstVisibleItem){
		firstVisibleItem = 0;
	} else {
		firstVisibleItem = min(firstVisibleItem - scroll, items.getCount() - nVisibleItems);
	}

	return true;
}

bool Menu::onKeyPress(const unsigned int key, const bool pressed){
	if (pressed){
		MenuItemID startItem = currItem;
		switch (key){
			case STX_KEY_SPACE:
			//case STX_KEY_ENTER:
				if ((items[currItem].flags & MENU_DISABLED) == 0){
					toggleItemCheck(currItem);
					menuListener->onMenuItemPress(this, currItem);
				}
				break;
			case STX_KEY_UP:
				do {
					if (currItem == 0){
						currItem = startItem;
						if (firstVisibleItem) firstVisibleItem--;
						break;
					}
					currItem--;
				} while (items[currItem].flags & MENU_DISABLED);
				if (currItem < firstVisibleItem){
					firstVisibleItem = currItem;
				}
				break;
			case STX_KEY_DOWN:
				do {
					currItem++;
					if (currItem == items.getCount()){
						currItem = startItem;
						if (firstVisibleItem + nVisibleItems < items.getCount()) firstVisibleItem++;
						break;
					}
				} while (items[currItem].flags & MENU_DISABLED);
				if (currItem >= firstVisibleItem + nVisibleItems){
					firstVisibleItem = currItem - nVisibleItems + 1;
				}
				break;
			case STX_KEY_HOME:
			case STX_KEY_PAGEUP:
				for (uint i = 0; i < currItem; i++){
					if ((items[i].flags & MENU_DISABLED) == 0) currItem = i;
				}
				break;
			case STX_KEY_END:
			case STX_KEY_PAGEDOWN:
				for (uint i = items.getCount() - 1; i > currItem; i--){
					if ((items[i].flags & MENU_DISABLED) == 0) currItem = i;
				}
				break;
			default:
				return false;
		}
		return true;
	}
	return false;
}

void Menu::onSize(const int w, const int h, const int oldW, const int oldH){
	if ((unsigned int) (h - yPos - 10) / 32 >= items.getCount()){
		nVisibleItems = items.getCount();
	} else {
		nVisibleItems = (h - int(yPos) - 24 - 10) / 32;
	}
}

float Menu::getWidth() const {
	float mWidth = 0;
	for (unsigned int i = 0; i < items.getCount(); i++){
		float w = 20 * /* font-> */ GUI_cpp__getStringWidth(items[i].text);
		mWidth = max(w, mWidth);
	}
	return mWidth + 22 + getCheckSpace();
}

float Menu::getCheckSpace() const {
	for (unsigned int i = 0; i < items.getCount(); i++){
		if (items[i].flags & MENU_CHECKABLE) return 20;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////

MenuSystem::MenuSystem(const float ix, const float iy, FONT3D *fnt, MenuListener *listener=0){
	font = fnt;
	xPos = ix;
	yPos = iy;
	currMenu = 0;
	#if 0
	menuListener = this;// ??? listener;
	#else
	menuListener = (MenuListener*)this;
	#endif

	color = vec4(0, 0.5f, 1, 0.4f);

	capture = true;
}

MenuSystem::~MenuSystem(){
	for (unsigned int i = 0; i < menus.getCount(); i++){
		delete menus[i];
	}
}

Menu *MenuSystem::addMenu(char *text){
	Menu *menu = new Menu(0, yPos + 32, font, text /*, menuListener */);
	menus.add(menu);

	setMenuPositions();

	return menu;
}

void MenuSystem::setPosition(const float newX, const float newY){
	Widget::setPosition(newX, newY);

	setMenuPositions();
}

void MenuSystem::draw(){
	int mWidth = getHalfWidth();
	float mx0 = xPos - mWidth;
	float mx1 = xPos + mWidth;

	// Background
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	IRenderer::GetRendererInstance()->setBlending(eSRC_ALPHA, eONE_MINUS_SRC_ALPHA);
	

	IRenderer::GetRendererInstance()->drawRoundRect(mx0, yPos - 28, mx1, yPos + 28, 12, color);

	// Border
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	

	vec4 black(0, 0, 0, 1);
	IRenderer::GetRendererInstance()->drawRoundRect(mx0, yPos - 28, mx1, yPos + 28, 12, black, 3);

	// Main menu
	IRenderer::GetRendererInstance()->setCullFace(eBACK);
	IRenderer::GetRendererInstance()->setBlending(eCONSTANT_COLOR, eZERO);
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	

	float mx = float(mx0 + 12);
	for (unsigned int i = 0; i < menus.getCount(); i++){
		if (currMenu == i){
			IRenderer::GetRendererInstance()->changeBlendConstantColor(vec4(1, 0.2f, 0.3f, 0));
		} else {
			IRenderer::GetRendererInstance()->changeBlendConstantColor(vec4(1, 1, 0.3f, 0));
		}
		Font3D__drawText(menus[i]->getTitle().c_str(), mx, (float) yPos, 32, 40, 0.15f, -0.1f, HA_LEFT, VA_CENTER);
		mx += 32 * /* font-> */ GUI_cpp__getStringWidth(menus[i]->getTitle().c_str()) + 20;
	}

	menus[currMenu]->draw();
}

bool MenuSystem::isInWidget(const int x, const int y) const {
	return isInMainMenu(x, y) || menus[currMenu]->isInWidget(x, y);
}

bool MenuSystem::isInMainMenu(const int x, const int y) const {
	int hWidth = getHalfWidth();
	int x0 = x - hWidth;
	int x1 = x + hWidth;

	return (x >= x0 && x <= x1 && y >= yPos - 28 && y <= yPos + 28);
}

bool MenuSystem::onMouseClick(const int x, const int y, const unsigned int button, const bool pressed){
	if (isInWidget(x, y)){
		return menus[currMenu]->onMouseClick(x, y, button, pressed);
	} else {
		visible = false;
	}
	return false;
}

bool MenuSystem::onMouseMove(const int x, const int y, const bool lButton, const bool mButton, const bool rButton){
	if (isInMainMenu(x, y)){
		int mWidth = getHalfWidth();
		float rx = float(xPos - mWidth + 12);

		for (unsigned int i = 0; i < menus.getCount(); i++){
			float lx = rx;
			rx += 32 * /* font-> */ GUI_cpp__getStringWidth(menus[i]->getTitle().c_str());
			if (x >= lx && x <= rx){
				currMenu = i;
				break;
			}
			rx += 20;
		}
		return true;
	} else {
		return menus[currMenu]->onMouseMove(x, y, lButton, mButton, rButton);
	}
}

bool MenuSystem::onMouseWheel(const int x, const int y, const int scroll){
	if (!isInMainMenu(x, y)){
		return menus[currMenu]->onMouseWheel(x, y, scroll);
	}
	return true;
}

bool MenuSystem::onKeyPress(const unsigned int key, const bool pressed){
	if (pressed){ /*
		if (key == menuKey){
			visible = false;
		} else */ {
			switch (key){
				case STX_KEY_LEFT:
					if (currMenu == 0) currMenu = menus.getCount();
					currMenu--;
					break;
				case STX_KEY_RIGHT:
					currMenu++;
					if (currMenu == menus.getCount()) currMenu = 0;
					break;
				case STX_KEY_ESCAPE:
					visible = false;
					break;
				default:
					return menus[currMenu]->onKeyPress(key, pressed);
			}
		}
		return true;
	}
	return false;
}

void MenuSystem::onSize(const int w, const int h, const int oldW, const int oldH){
	for (unsigned int i = 0; i < menus.getCount(); i++){
		menus[i]->onSize(w, h, oldW, oldH);
	}
}

void MenuSystem::setMenuPositions(){
	float x = float(xPos - getHalfWidth());
	for (unsigned int i = 0; i < menus.getCount(); i++){
		menus[i]->setPosition(x + 20 * i, yPos + 32);
		x += 32 * /* font-> */ GUI_cpp__getStringWidth(menus[i]->getTitle().c_str());
	}
}

int MenuSystem::getHalfWidth() const {
	float hWidth = 14.5f;
	for (unsigned int i = 0; i < menus.getCount(); i++){
		float w = 16 * /* font-> */ GUI_cpp__getStringWidth(menus[i]->getTitle().c_str());
		hWidth += w;
	}

	return int(hWidth) + 10 * (menus.getCount() - 1);
}

////////////////////////////////////////////////////////////////////////////////////////////

Slider::Slider(const float ix, const float iy, const float w, const float h, const bool horizontal, const float ir0, const float ir1, const float val, const float stepSize){
	drag = false;
	xPos = ix;
	yPos = iy;
	width  = w;
	height = h;
	isHorizontal = horizontal;
	step = stepSize;
	setRange(ir0, ir1);
	setValue(val);
	color = vec4(1, 1, 0, 0.5f);
	sliderListener = NULL;
}

void Slider::draw(){
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	IRenderer::GetRendererInstance()->setBlending(eSRC_ALPHA, eONE_MINUS_SRC_ALPHA);
	

	// Background
	IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos, xPos + width, yPos + height, 10, color);

	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	

	vec4 black(0, 0, 0, 1);
	// Border
	IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos, xPos + width, yPos + height, 10, black, 3);

	if (isHorizontal){
		float vx = lerp(xPos + 12.0f, xPos + width - 12.0f, (value - r0) / (r1 - r0)) + 0.5f;
		IRenderer::GetRendererInstance()->drawLine(xPos + 12, yPos + 0.5f * height, xPos + width - 12, yPos + 0.5f * height, black, 3);
		IRenderer::GetRendererInstance()->drawRoundRect(vx - 5, yPos + 3, vx + 5, yPos + height - 3, 4, black);
	} else {
		float vy = lerp(yPos + 12.0f, yPos + height - 12.0f, (r1 - value) / (r1 - r0)) + 0.5f;
		IRenderer::GetRendererInstance()->drawLine(xPos + 0.5f * width, yPos + 12, xPos + 0.5f * width, yPos + height - 12, black, 3);
		IRenderer::GetRendererInstance()->drawRoundRect(xPos + 3, vy - 5, xPos + width - 3, vy + 5, 4, black);
	}
}

bool Slider::isInWidget(const int x, const int y) const {
	return (x >= xPos && x < xPos + width && y >= yPos && y < yPos + height);
}

bool Slider::onMouseClick(const int x, const int y, const unsigned int button, const bool pressed){
	if (button == LEFT_BUTTON){
		drag = capture = pressed;
		if (pressed) computeValue(x, y);
		return true;
	}
	return false;
}

bool Slider::onMouseMove(const int x, const int y, const bool lButton, const bool mButton, const bool rButton){
	if (drag){
		computeValue(x, y);
		return true;
	}
	return false;
}

void Slider::setValue(const float val){
	value = clamp(val, r0, r1);
	if (step > 0){
		float s = (value - r0) / step;
		s = floor(s + 0.5f);
		value = min(r0 + s * step, r1);
	}
}

void Slider::computeValue(const int x, const int y){
	float k = isHorizontal? (x - (xPos + 12.0f)) / (width - 24.0f) : ((yPos + height - 12.0f) - y) / (height - 24.0f);
	setValue(lerp(r0, r1, k));
	if (sliderListener) sliderListener->onSliderChange(this);
}


////////////////////////////////////////////////////////////////////////////////////////////

Label::Label(const float ix, const float iy, FONT3D *fnt, const char *str, const float chrWidth, const float chrHeight,
			 const float skewx, const float skewy, FONT_HADJUST hAdj, FONT_VADJUST vAdj) : text(str){
	xPos = ix;
	yPos = iy;
	font = fnt;
	charWidth  = chrWidth;
	charHeight = chrHeight;
	skewX = skewx;
	skewY = skewy;
	hAdjust = hAdj;
	vAdjust = vAdj;
	color = vec4(1, 1, 0, 1);
}

Label::~Label(){
}

void Label::setText(char *str){
	if(str)
		text = str;
	else
		text="";
}

void Label::draw(){
	IRenderer::GetRendererInstance()->setCullFace(eBACK);
	IRenderer::GetRendererInstance()->setBlending(eCONSTANT_COLOR, eZERO);
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	

	IRenderer::GetRendererInstance()->changeBlendConstantColor(color);

	Font3D__drawText(text.c_str(), xPos, yPos, charWidth, charHeight, skewX, skewY, hAdjust, vAdjust);
}

////////////////////////////////////////////////////////////////////////////////////////////

TextBox::TextBox(const float ix, const float iy, const float w, const float h, FONT3D *fnt, char *str) : text(str){
	xPos = ix;
	yPos = iy;
	width  = w;
	height = h;
	font = fnt;
	color = vec4(1, 0, 0, 0.5f);
}

TextBox::~TextBox(){
}

void TextBox::setText(char *str){
	if(str)
	text = str;
	else
	text="";
}

void TextBox::draw(){
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	IRenderer::GetRendererInstance()->setBlending(eSRC_ALPHA, eONE_MINUS_SRC_ALPHA);
	

	// Background
	IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos, xPos + width, yPos + height, 7, color);

	// Border
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	
	IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos, xPos + width, yPos + height, 7, vec4(0, 0, 0, 1), 3);

	IRenderer::GetRendererInstance()->setCullFace(eBACK);
	IRenderer::GetRendererInstance()->setBlending(eCONSTANT_COLOR, eZERO);
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	

	IRenderer::GetRendererInstance()->changeBlendConstantColor(vec4(0.7f, 1, 1, 0));

	Font3D__drawText(text.c_str(), xPos + 0.5f * width, yPos + 0.5f * height + 2, 16, 20, 0.15f, -0.1f, HA_CENTER, VA_CENTER);
}

bool TextBox::isInWidget(const int x, const int y) const {
	return (x >= xPos && x < xPos + width && y >= yPos && y < yPos + height);
}

////////////////////////////////////////////////////////////////////////////////////////////

PushButton::PushButton(const float ix, const float iy, const float w, const float h, FONT3D *fnt, char *text) : TextBox(ix, iy, w, h, fnt, text){
	pushed = false;
	pushButtonListener = NULL;
}

bool PushButton::onMouseClick(const int x, const int y, const unsigned int button, const bool pressed){
	if (button == LEFT_BUTTON){
		setPushed(capture = pressed);
		return true;
	}
	return false;
}

void PushButton::setPushed(const bool val){
	if (val != pushed){
		color = vec4(1.0f, 1.0f, 1.0f, 1.0f) - color;
		pushed = val;
		if (!pushed && pushButtonListener != NULL) pushButtonListener->onPushButtonPress(this);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

KeyWaiter::KeyWaiter(const float ix, const float iy, FONT3D *fnt, KeyWaiterListener *listener){
	xPos = ix;
	yPos = iy;
	font = fnt;

	keyWaiterListener = listener;

	color = vec4(0, 0.75f, 1.0f, 0.7f);
	capture = true;
}

KeyWaiter::~KeyWaiter(){
}

void KeyWaiter::draw(){
	float x0 = xPos - 125;
	float y0 = yPos - 30;
	float x1 = xPos + 125;
	float y1 = yPos + 30;

	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	IRenderer::GetRendererInstance()->setBlending(eSRC_ALPHA, eONE_MINUS_SRC_ALPHA);
	

	// Background
	IRenderer::GetRendererInstance()->drawRoundRect(x0, y0, x1, y1, 16, color);

	// Border
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	
	IRenderer::GetRendererInstance()->drawRoundRect(x0, y0, x1, y1, 16, vec4(0, 0, 0, 1), 3);

	IRenderer::GetRendererInstance()->/* changeCullFace */ setCullFace(eBACK);

	IRenderer::GetRendererInstance()->setCullFace(eBACK);
	IRenderer::GetRendererInstance()->setBlending(eCONSTANT_COLOR, eZERO);
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	

	IRenderer::GetRendererInstance()->changeBlendConstantColor(vec4(1, 0.6f, 0, 0));

	Font3D__drawText("Press desired key ...", xPos, yPos + 2, 24, 36, 0.15f, -0.1f, HA_CENTER, VA_CENTER);
}

bool KeyWaiter::isInWidget(const int x, const int y) const {
	return (x >= xPos - 125 && x <= xPos + 125 && y >= yPos - 30 && y <= yPos + 30);
}

bool KeyWaiter::onKeyPress(const unsigned int key, const bool pressed){
	if (pressed){
		keyWaiterListener->onKeyWaiterKeyPress(key);
		dead = true;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////

MsgBox::MsgBox(const float ix, const float iy, FONT3D *fnt, char *str) : text(str){
	xPos = ix;
	yPos = iy;
	font = fnt;

	float x0, y0, x1, y1;
	getRect(&x0, &y0, &x1, &y1);

	okButton = new PushButton(ix - 22, y1 - 48, 44, 36, fnt, "OK");

	color = vec4(0, 0.2f, 0.7f, 0.5f);

	capture = true;
}

MsgBox::~MsgBox(){
	delete okButton;
}

void MsgBox::draw(){
	float x0, y0, x1, y1;
	getRect(&x0, &y0, &x1, &y1);

	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	IRenderer::GetRendererInstance()->setBlending(eSRC_ALPHA, eONE_MINUS_SRC_ALPHA);

	// Background
	IRenderer::GetRendererInstance()->drawRoundRect(x0, y0, x1, y1, 20, color);

	// Border
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	IRenderer::GetRendererInstance()->drawRoundRect(x0, y0, x1, y1, 20, vec4(0, 0, 0, 1), 3);

	IRenderer::GetRendererInstance()->/* changeCullFace */ setCullFace(eBACK);

	Font3D__drawText(text.c_str(), xPos, yPos - 20, 24, 36, 0.15f, -0.1f, HA_CENTER, VA_CENTER);

	okButton->draw();
}

bool MsgBox::isInWidget(const int x, const int y) const {
	float x0, y0, x1, y1;
	getRect(&x0, &y0, &x1, &y1);

	return (x >= x0 && x <= x1 && y >= y0 && y <= y1);
}

bool MsgBox::onMouseClick(const int x, const int y, const unsigned int button, const bool pressed){
	if (button == LEFT_BUTTON){
		bool pushed = okButton->isPushed();
		bool inW = okButton->isInWidget(x, y);

		if (pushed || inW){
			okButton->onMouseClick(x, y, button, pressed);
			dead = pushed && inW;
		}
		return true;
	}
	return false;
}

bool MsgBox::onKeyPress(const unsigned int key, const bool pressed){
	if (//key == STX_KEY_ENTER || 
	key == STX_KEY_SPACE || key == STX_KEY_ESCAPE){
		if (pressed){
			okButton->setPushed(true);
		} else {
			dead = okButton->isPushed();
		}
		return true;
	}
	return false;
}

void MsgBox::getRect(float *x0, float *y0, float *x1, float *y1) const {
#if 0
	font->getExtents(text, float(xPos), float(yPos), 24, 36, 0.15f, -0.1f, HA_CENTER, VA_CENTER, x0, x1, y0, y1);
#else
	*x0=0.0f;
	*x1=0.0f;
	*y0=0.0f;
	*y1=0.0f;
#endif
	*x0 -= 10;
	*x1 += 10;
	*y0 -= 30;
	*y1 += 30;
}

////////////////////////////////////////////////////////////////////////////////////////////

DropDownList::DropDownList(const float ix, const float iy, const float w, const float h, FONT3D *fnt){
	xPos = ix;
	yPos = iy;
	width  = w;
	height = h;
	font = fnt;

	dropDownListener = NULL;

	selectedItem = -1;
	droppedDown = false;
	incDown = decDown = false;

	color = vec4(0.1f, 1, 0.2f, 0.5f);
}

DropDownList::~DropDownList(){
	for (uint i = 0; i < items.getCount(); i++){
		delete items[i];
	}
}

ListItemID DropDownList::addItem(const char *string){
	char *str = new char[strlen(string) + 1];
	strcpy(str, string);
	return items.add(str);
}

void DropDownList::setSelectedItem(ListItemID item){
	if (item != selectedItem){
		selectedItem = item;
		if (dropDownListener) dropDownListener->onDropDownListChange(this);
	}
}

void DropDownList::draw(){
	float xRight = xPos + width;
	float xEdge0 = xRight - height - 5;
	float xEdge1 = xRight - height;

	if (droppedDown){
		firstItem = 0;
		lastItem = items.getCount();
	} else {
		firstItem = selectedItem;
		lastItem = firstItem + (selectedItem >= 0);
	}

	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	IRenderer::GetRendererInstance()->setBlending(eSRC_ALPHA, eONE_MINUS_SRC_ALPHA);

	vec4 clickCol(0.5f * color.x, 0.5f * color.y, 0.5f * color.z, color.w);

	// Background
	IRenderer::GetRendererInstance()->drawRoundRect(xPos,   yPos, xEdge0, yPos + height, 8, color);
	IRenderer::GetRendererInstance()->drawRoundRect(xEdge1, yPos, xRight, yPos + 0.5f * height - 1, 5, decDown? clickCol : color);
	IRenderer::GetRendererInstance()->drawRoundRect(xEdge1, yPos + 0.5f * height + 1, xRight, yPos + height, 5, incDown? clickCol : color);

	// Borders
	vec4 black(0, 0, 0, 1);
	IRenderer::GetRendererInstance()->drawRoundRect(xPos,   yPos, xEdge0, yPos + height, 8, black, 3);
	IRenderer::GetRendererInstance()->drawRoundRect(xEdge1, yPos, xRight, yPos + 0.5f * height - 1, 5, black, 3);
	IRenderer::GetRendererInstance()->drawRoundRect(xEdge1, yPos + 0.5f * height + 1, xRight, yPos + height, 5, black, 3);

	IRenderer::GetRendererInstance()->drawTriangle(
		xEdge1 + 0.5f * height,     yPos + 0.1f * height,
		xEdge1 + 0.8f * height - 1, yPos + 0.4f * height - 1,
		xEdge1 + 0.2f * height + 1, yPos + 0.4f * height - 1, black);

	IRenderer::GetRendererInstance()->drawTriangle(
		xEdge1 + 0.5f * height,     yPos + 0.9f * height,
		xEdge1 + 0.2f * height + 1, yPos + 0.6f * height + 1,
		xEdge1 + 0.8f * height - 1, yPos + 0.6f * height + 1, black);

	if (droppedDown){
		IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos - (firstItem + selectedItem) * height, xEdge0, yPos + (lastItem - selectedItem) * height, 8, color);
		IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos - (firstItem + selectedItem) * height, xEdge0, yPos + (lastItem - selectedItem) * height, 8, black, 3);
	}

	IRenderer::GetRendererInstance()->setCullFace(eBACK);
	IRenderer::GetRendererInstance()->setBlending(eCONSTANT_COLOR, eZERO);
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);

	IRenderer::GetRendererInstance()->changeBlendConstantColor(vec4(0.3f, 0.3f, 0.9f, 0));

	for (int i = firstItem; i < lastItem; i++){
		Font3D__drawText(items[i], xPos + 0.3f * height, yPos + (i - selectedItem + 0.5f) * height, 0.6f * height, 0.7f * height, 0.15f, -0.1f, HA_LEFT, VA_CENTER);
	}
}

bool DropDownList::isInWidget(const int x, const int y) const {
	if (x < xPos || x > xPos + width) return false;

	if (droppedDown && x < xPos + width - height){
		float yt = yPos - (firstItem + max(selectedItem, 0)) * height;
		float yb = yPos + (lastItem  - selectedItem) * height;

		return (y >= yt && y < yb);
	} else {
		return (y >= yPos && y < yPos + height);
	}
}

bool DropDownList::onMouseClick(const int x, const int y, const unsigned int button, const bool pressed){
	if (pressed){
		if (button == LEFT_BUTTON && isInWidget(x, y)){
			if (x < xPos + width - height){
				if (droppedDown){
					int item = int(floorf((y - yPos) / height)) + selectedItem;
					if (item >= 0 && item < (int) items.getCount()) setSelectedItem(item);
				}
				capture = droppedDown = !droppedDown;
			} else {
				if (y < yPos + 0.5f * height){
					if (selectedItem > 0) setSelectedItem(selectedItem - 1);
					decDown = true;
				} else {
					if (selectedItem < (int) items.getCount() - 1) setSelectedItem(selectedItem + 1);
					incDown = true;
				}
				capture = true;
			}
		} else {
			capture = droppedDown = false;
		}
	} else {
		if (button == LEFT_BUTTON){
			incDown = decDown = false;
			capture = false;
		}
	}

	return true;
}

bool DropDownList::onMouseWheel(const int x, const int y, const int scroll){
	if (selectedItem >= 0 || scroll < 0){
		int item = selectedItem - scroll;

		if (item < 0) item = 0;
		if (item >= (int) items.getCount()) item = items.getCount() - 1;

		setSelectedItem(item);
	}
	return true;
}

bool DropDownList::onKeyPress(const unsigned int key, const bool pressed){
	return false;
}
#endif

