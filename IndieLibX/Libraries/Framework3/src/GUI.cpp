#include <string.h>//QT_BUILD//???
#include <STX/STXCPP.h>
#include <Framework3/IRenderer.h>
#include <Framework3/GUI.h>
#include <STX/Matrix.h>
#if 0
//-----------------------------------------------------------------------------
// File: Framework\GUI.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
Font3D::Font3D(){
	stx_memset(chars, 0, sizeof(chars));

	spacingX = 0.1f;
	spacingY = 0.1f;

	vertices = 0;
	indices  = 0;
}

Font3D::~Font3D(){
	delete vertices;
	delete indices;
}

bool Font3D::loadFromFile(const char *fileName0){

	char buf[1024]={0};
	if(fileName0)
	{
        stx_strlcpy(buf,fileName0,1024);
	}
	else
	{	
	buf[0]=0;
	stx_strlcpy(buf,"/Framework3/Default.font",1024);
	}
	std::string f=buf;
    std::string fileName=stx_convertpath(f);
    stx_fileExists(fileName.c_str());
	LOG_PRINT("Font3D::loadFromFile:fopen=%s\n",fileName.c_str());
    FILE *file = fopen(fileName.c_str(), "rb");
	if (file == 0) return false;

	unsigned int nChars = 0;
    fread(&nChars, 4, 1, file);

	unsigned char *fChars = new unsigned char[nChars];
    fread(fChars, nChars, 1, file);

	float* f1=&minY;
	float* f2=&maxY;

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
	delete[] fChars;

	chars[32].ratio = 0.25f;

	uploadVertexBuffers(IRenderer::GetRendererInstance(),false);
	return false;
}

bool Font3D::uploadVertexBuffers(IRenderer *renderer, const bool d3dStyle){


{
	{
#if defined(ANDROID)
const char *plainSHD =
"[Vertex shader]\n"
"vec4 scaleBias=vec4(0.003,-0.003,-1,1);\n"
"attribute mediump vec3 aPosition;\n"
"attribute mediump vec4 aColor;\n"
"//varying mediump vec4 vPosition;\n"
"varying mediump vec4 vColor;\n"
"//ROW_MAJOR mediump mat4 ModelViewMatrix MVPSEMANTIC;\n"
"uniform mediump mat4 ModelViewMatrix;\n"
"void main(){\n"
	"	//gl_Position.xy = gl_Position.xy * scaleBias.xy + scaleBias.zw;\n"
	"	gl_Position=ModelViewMatrix*vec4(gl_Position,1);\n"
	"	gl_Position.w= WSIGN 1;\n"
	"	vColor=aColor;\n"
	
	"}\n"
"[Fragment shader]\n"
"//varying mediump vec4 vPosition;\n"
"varying mediump vec4 vColor;\n"
"void main(VsOut In): COLOR{\n"
	"	gl_FragColor = vColor;\n"
	"}\n";
#elif defined(USE_HLSL_SHADERS)
const char *plainSHD =
"[Vertex shader]\n"
"float4 scaleBias=float4(0.003,-0.003,-1,1);\n"
"float3 aPosition;\n"
"float4 aColor;\n"
"//float4 vPosition;\n"
"float4 vColor;\n"
"//ROW_MAJOR float4x4 ModelViewMatrix MVPSEMANTIC;\n"
"float4x4 ModelViewMatrix;\n"
"void main(){\n"
	"	//gl_Position.xy = gl_Position.xy * scaleBias.xy + scaleBias.zw;\n"
	"	gl_Position=ModelViewMatrix*float4(gl_Position,1);\n"
	"	gl_Position.w= WSIGN 1;\n"
	"	vColor=aColor;\n"
	
	"}\n"
"[Fragment shader]\n"
"//float4 vPosition;\n"
"float4 vColor;\n"
"void main(VsOut In): COLOR{\n"
	"	gl_FragColor = vColor;\n"
	"}\n";
#else
const char *plainSHD =
"[Vertex shader]\n"
"vec4 scaleBias=vec4(0.003,-0.003,-1,1);\n"
"attribute vec3 aPosition;\n"
"attribute vec4 aColor;\n"
"//varying vec4 vPosition;\n"
"varying vec4 vColor;\n"
"//ROW_MAJOR mat4 ModelViewMatrix MVPSEMANTIC;\n"
"uniform mat4 ModelViewMatrix;\n"
"void main(){\n"
	"	//gl_Position.xy = gl_Position.xy * scaleBias.xy + scaleBias.zw;\n"
	"	gl_Position=ModelViewMatrix*vec4(gl_Position,1);\n"
	"	gl_Position.w= WSIGN 1;\n"
	"	vColor=aColor;\n"
	
	"}\n"
"[Fragment shader]\n"
"//varying vec4 vPosition;\n"
"varying vec4 vColor;\n"
"void main(VsOut In): COLOR{\n"
	"	gl_FragColor = vColor;\n"
	"}\n";
#endif

		shader = IRenderer::GetRendererInstance()->addShader(plainSHD, "main", "main");
		FormatDesc format[] =
		{
		0,TYPE_VERTEX, FORMAT_FLOAT, 3,
		0,TYPE_TEXCOORD, FORMAT_FLOAT, 4,
		}
		;
		vertexFormat = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shader);
	}
}

	FontVertex4* vertices4=new FontVertex4[nVertices];
	for(unsigned int i=0;i<nVertices;i++)
	{
		vertices4->pos=vertices->pos;
		vertices4->color=D3DXFROMWINEVECTOR4((vertices->color&0xFF0000)*1.0f,(vertices->color&0x00FF00)*1.0f,(vertices->color&0x0000FF)*1.0f,1.0f);
	}
	if ((vertexBuffer = IRenderer::GetRendererInstance()->addVertexBuffer(nVertices * sizeof(FontVertex4),STATIC, vertices4)) == VB_NONE) return false;
	delete[] vertices4;
	if ((indexBuffer = IRenderer::GetRendererInstance()->addIndexBuffer(nIndices, 2,STATIC, indices)) == IB_NONE) return false;

	return true;
}

void Font3D::getExtents(const char *string, float x, float y, const float charWidth, const float charHeight, const float skewX, const float skewY, const FONT_HADJUST hAdjust, const FONT_VADJUST vAdjust, float *left, float *right, float *top, float *bottom){
	char *str = (char *)string;
	int n = 0;
	float fw = 0;
	// Count the number of lines in the string and find maximum width
	while (true){
		char *newline = str;
		while (*newline && *newline != '\n') newline++;

		float w = getStringWidth(str, int(newline - str));
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

	if (left   != 0) *left   = l;
	if (right  != 0) *right  = r;
	if (top    != 0) *top    = t;
	if (bottom != 0) *bottom = b;
}

float Font3D::getStringWidth(const char *string, int length) const {
    if (length < 0) length = (int) stx_strlen(string);

	float w = 0;
	if (length > 0){
		for (int i = 0; i < length; i++){
			w += chars[(unsigned char) string[i]].ratio;
		}
		w += (length - 1) * spacingX;
	}

	return w;
}

float Font3D::getLineWidth(const char *string) const {
    char *str = stx_strchr(string, '\n');
	return getStringWidth(string, (str != 0)? int(str - string) : -1);
}

int Font3D::getMaxCharsFitOnLine(const char *string, const float width, const bool wordBreak) const {
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

void Font3D::setSpacing(const float sx, const float sy){
	spacingX = sx;
	spacingY = sy;
}

void Font3D::drawText(IRenderer *renderer, const char *string, float x, float y, const float charWidth, const float charHeight, const float skewX, const float skewY, const FONT_HADJUST hAdjust, const FONT_VADJUST vAdjust){
#if 1
		IRenderer::GetRendererInstance()->
drawText(string, x, y-13,
		 //15, 18,
		//charWidth/2, charHeight/2,
		charWidth, charHeight,
		IRenderer::GetRendererInstance()->GetdefaultFont(),
		IRenderer::GetRendererInstance()->GetlinearClamp(),
		IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
		IRenderer::GetRendererInstance()->GetnoDepthTest());

			//DrawText( x,y,string);
#else
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
	skew.elem(0,2) = skewX;
	skew.elem(1,2) = skewY;


	while (*string){
		if (*string == '\n'){
			y += (1 + spacingY) * charHeight;
			x = startX;
			if (hAdjust != HA_LEFT) x -= lShiftFactor * getLineWidth(string + 1) * charWidth;
		} else {
			Char chr = chars[*(ubyte *) string];
			float cw = charWidth * chr.ratio;

			if (chr.nIndices > 0){
	IRenderer::GetRendererInstance()->setShader(this->shader);
	//IRenderer::GetRendererInstance()->texShader());

	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias", IRenderer::GetRendererInstance()->GetscaleBias2D());

	IRenderer::GetRendererInstance()->setVertexFormat(vertexFormat);
	IRenderer::GetRendererInstance()->setVertexBuffer(0,vertexBuffer);
	IRenderer::GetRendererInstance()->setIndexBuffer(indexBuffer);

				mat4 mat = translate(D3DXFROMWINEVECTOR3(x, y, 0)) * scale/*4???*/(charWidth, charHeight, 1) * skew;
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("ModelViewMatrix",mat);

				//IRenderer::GetRendererInstance()->drawElements(PRIM_TRIANGLES, chr.firstIndex, chr.nIndices, chr.firstVertex, chr.nVertices);
				IRenderer::GetRendererInstance()->DrawIndexedPrimitive(
					PRIM_TRIANGLES,0,chr.firstVertex,chr.nVertices,chr.firstIndex,chr.nIndices/3
					//chr.firstIndex, chr.nIndices, chr.firstVertex, chr.nVertices);
					);
			}

			x += cw + charWidth * spacingX;
		}
		string++;
	}

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("ModelViewMatrix",identity4());
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////

void Widget::setPosition(const float newX, const float newY){
	xPos = newX;
	yPos = newY;
}
void Menu::initCurrentItem(){
	for (unsigned int i = 0; i < items.getCount(); i++){
		if (items[i].rbGroup){
			if (items[i].flags & MENU_CHECKED){
				currItem = i;
				break;
			}
		} else break;
	}
}

MenuItemID Menu::addItem(const char *text, unsigned int flags){
	MenuItem menuItem;
	menuItem.flags = flags;
	menuItem.rbGroup = 0;

	color = D3DXFROMWINEVECTOR4(0, 0.5f, 1, 0.4f);

    size_t length = stx_strlen(text);
	menuItem.text = new char[length + 1];
    stx_strlcpy(menuItem.text, text,length + 1);

	nVisibleItems++;//???
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
void Menu::setCheckBoxGroup(const MenuItemID first, const MenuItemID last, const unsigned int group){
	for (unsigned int i = first; i <= last; i++){
		items[i].rbGroup = 0;
	}
}
bool Menu::getCheckedCheckBox(const unsigned int group, MenuItemID id) const{
	//for (unsigned int i = 0; i < items.getCount(); i++)
	{
		if (items[id].rbGroup == 0){
			if (items[id].flags & MENU_CHECKED) return true;
		}
	}
	return false;
}
void Menu::checkItem(const MenuItemID menuItem, const bool value){
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

void Menu::setItemText(const MenuItemID menuItem, const char *text){
	delete items[menuItem].text;

    items[menuItem].text = new char[stx_strlen(text) + 1];
    stx_strlcpy(items[menuItem].text, text,stx_strlen(text) + 1);
}

void Menu::draw(IRenderer *renderer){
	#if 0
	IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setBlending(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	#else
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());
	#endif
	

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
	D3DXFROMWINEVECTOR4 black(0, 0, 0, 1);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());
	IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos, xRight, yBottom, 8, black, 3);

	if (nVisibleItems < items.getCount()){
		float xMid = xPos + mWidth / 2;
		if (firstVisibleItem > 0){
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());

			IRenderer::GetRendererInstance()->drawTriangle(xMid, yPos + 4, xMid + 7, yPos + 11, xMid - 7, yPos + 11, black);
		}
		if (firstVisibleItem + nVisibleItems < items.getCount()){
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());

			IRenderer::GetRendererInstance()->drawTriangle(xMid, yBottom - 4, xMid - 7, yBottom - 11, xMid + 7, yBottom - 11, black);
		}
	}

	//IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());

	
	for (unsigned int i = 0; i < nVisibleItems; i++){
		MenuItemID item = firstVisibleItem + i;

		D3DXFROMWINEVECTOR4 col;
		if (items[item].flags & MENU_DISABLED){
			col = D3DXFROMWINEVECTOR4(0.5f, 0.5f, 0.5f, 1);
		} else {
			col = D3DXFROMWINEVECTOR4(1, 1, 1, 1);
		}
		if (items[item].flags & MENU_CHECKABLE){
			if (items[item].rbGroup){
				// Radio button
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());

				IRenderer::GetRendererInstance()->drawCircle(xPos + 17, yTop + 20 + i * 32, 7, col, 2);

				if (items[item].flags & MENU_CHECKED){
					// Radio button check
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());

					IRenderer::GetRendererInstance()->drawCircle(xPos + 17, yTop + 20 + i * 32, 4, col);
				}
			} else {
				// Checkbox
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());

				IRenderer::GetRendererInstance()->drawRect(xPos + 8, yTop + 11 + i * 32, xPos + 24, yTop + 28 + i * 32, col, 2);
				if (items[item].flags & MENU_CHECKED){
					// Check
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());

					IRenderer::GetRendererInstance()->drawLine(xPos + 11, yTop + 18 + i * 32, xPos + 15, yTop + 25 + i * 32, col, 2);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());

					IRenderer::GetRendererInstance()->drawLine(xPos + 15, yTop + 25 + i * 32, xPos + 21, yTop + 14 + i * 32, col, 2);
				}
			}
		}
	}

	// Menu item separators
	D3DXFROMWINEVECTOR4 darkBlue(0, 0, 0.3f, 1);
	for (unsigned int i = 0; i < separators.getCount(); i++){
		if (separators[i] >= firstVisibleItem && separators[i] < firstVisibleItem + nVisibleItems - 1){
			float y = yTop + (separators[i] - firstVisibleItem) * 32 + 36;
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());

			IRenderer::GetRendererInstance()->drawLine(xPos + 11, y, xRight - 11, y, darkBlue, 2);
		}
	}


	//IRenderer::GetRendererInstance()->setCullFace(BACK);
	IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullBack());
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());//setBlending(CONSTANT_COLOR, ZERO);
	//IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());

	

	for (unsigned int i = 0; i < nVisibleItems; i++){
		MenuItemID item = firstVisibleItem + i;

		if (currItem == item){
			if (items[item].flags & MENU_DISABLED){
				IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(0.5f, 0.15f, 0.2f, 0));
			} else {
				IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1, 0.2f, 0.3f, 0));
			}
		} else {
			if (items[item].flags & MENU_DISABLED){
				IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(0.5f, 0.5f, 0.5f, 0));
			} else {
				IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1, 1, 1, 0));
			}
		}

fnt->
			drawText( renderer,items[item].text,
				xPos + checkSpace + 9.0f, yTop + i * 32 + 21.0f,
				20, 26, 0.15f, -0.1f, HA_LEFT, VA_CENTER);
/*
IRenderer::GetRendererInstance()->DrawText( xPos + checkSpace + 9.0f, yTop + i * 32 + 21.0f,
items[item].text);
*/
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
			case STX_KEY_RETURN:
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
				for (unsigned int i = 0; i < currItem; i++){
					if ((items[i].flags & MENU_DISABLED) == 0) currItem = i;
				}
				break;
			case STX_KEY_END:
			case STX_KEY_PAGEDOWN:
				for (unsigned int i = items.getCount() - 1; i > currItem; i--){
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
	Font3D* Widget::fnt=0;
float Menu::getWidth() const {
	float mWidth = 0;
	for (unsigned int i = 0; i < items.getCount(); i++){
		float w = 20 *
			fnt->getStringWidth(items[i].text);

			//IRenderer::GetRendererInstance()->getStringWidth(0,items[i].text,0);

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

MenuSystem::MenuSystem(const float ix, const float iy, Font3D *afnt, MenuListener *listener){

	fnt = afnt;
	xPos = ix;
	yPos = iy;
	currMenu = 0;
	menuListener = listener;

	color = D3DXFROMWINEVECTOR4(0, 0.5f, 1, 0.4f);

	capture = true;

#if 0
#if 0
	static bool inited=false;
	if(inited)
		return;
	inited=true;
	// Setup global menus

	mainMenu = new MenuSystem(400, 36, &mainFont, this);
	mainMenu->setVisible(false);
	addWidget(mainMenu);

	optionsMenu = mainMenu->addMenu("Options");
	optionsShowFPS = optionsMenu->addItem("Show framerate", MENU_CHECKABLE | MENU_CHECKED);
	optionsVSync = optionsMenu->addItem("VSync", MENU_CHECKABLE);
	optionsMenu->addSeparatorAfter(optionsVSync);
#ifndef NO_CAPTURE_MOUSE
	optionsInvertMouse = optionsMenu->addItem("Invert mouse", MENU_CHECKABLE);

	// These get their text assigned later
	optionsForwardKey  = optionsMenu->addItem("");
	optionsBackwardKey = optionsMenu->addItem("");
	optionsLeftKey     = optionsMenu->addItem("");
	optionsRightKey    = optionsMenu->addItem("");
	optionsUpKey       = optionsMenu->addItem("");
	optionsDownKey     = optionsMenu->addItem("");
	optionsResetKey      = optionsMenu->addItem("");

	optionsFramerateKey  = optionsMenu->addItem("");
	optionsMenuKey       = optionsMenu->addItem("");
	optionsScreenshotKey = optionsMenu->addItem("");

	optionsMenu->addSeparatorAfter(optionsScreenshotKey);
#endif

	optionsScreenshot = optionsMenu->addItem("Snap screenshot");

	helpMenu = mainMenu->addMenu("Help");
	helpAbout = helpMenu->addItem("About");

	for (uint i = 0; i < mainMenu->getMenuCount(); i++){
		mainMenu->getMenu(i)->initCurrentItem();

	}
	if (1){
			mainMenu->setVisible(true);
			widgets.goToObject(mainMenu);
			widgets.moveCurrentToTop();

			return true;
		}
#endif
#endif
}

MenuSystem::~MenuSystem(){
	for (unsigned int i = 0; i < menus.getCount(); i++){
		delete menus[i];
	}
}

Menu *MenuSystem::addMenu(const char *text){
	Menu *menu = new Menu(0, yPos + 32, 0, text, menuListener);
	menus.add(menu);

	setMenuPositions();

	return menu;
}

void MenuSystem::setPosition(const float newX, const float newY){
	Widget::setPosition(newX, newY);

	setMenuPositions();
}

void MenuSystem::draw(IRenderer *renderer){
	int mWidth = getHalfWidth();
	float mx0 = xPos - mWidth;
	float mx1 = xPos + mWidth;

	// Background
#if 0
	IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setBlending(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	#else
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());
	#endif

	

	IRenderer::GetRendererInstance()->drawRoundRect(mx0, yPos - 28, mx1, yPos + 28, 12, color);

	// Border
	//IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());


	D3DXFROMWINEVECTOR4 black(0, 0, 0, 1);
	IRenderer::GetRendererInstance()->drawRoundRect(mx0, yPos - 28, mx1, yPos + 28, 12, black, 3);
if(1)
{
	//IRenderer::GetRendererInstance()->setShader(shader);
	// Main menu
	//IRenderer::GetRendererInstance()->setCullFace(BACK);
	IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullBack());
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());//setBlending(CONSTANT_COLOR, ZERO);
//	IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	

	float mx = float(mx0 + 12);
	for (unsigned int i = 0; i < menus.getCount(); i++){
		if (currMenu == i){
			IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1, 0.2f, 0.3f, 0));
		} else {
			IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1, 1, 0.3f, 0));
		}
		fnt->drawText(renderer, const_cast<char*>(menus[i]->getTitle().c_str()), mx, (float) yPos, 32, 40, 0.15f, -0.1f, HA_LEFT, VA_CENTER);
		mx += 32 * fnt->getStringWidth(const_cast<char*>(menus[i]->getTitle().c_str())) + 20;
/*
IRenderer::GetRendererInstance()->DrawText( mx, (float) yPos,
menus[i]->getTitle().c_str());
		mx += 32 * IRenderer::GetRendererInstance()->getStringWidth(0,const_cast<char*>(menus[i]->getTitle().c_str()),0) + 20;
*/

	}
}
	if(menus.getCount())
		menus[currMenu]->draw(renderer);//???
}

bool MenuSystem::isInWidget(const int x, const int y) const {
	if(menus.getCount())
	return isInMainMenu(x, y) || menus[currMenu]->isInWidget(x, y);
	else

		return false;//???
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
		//???visible = false;
	}
	return false;
}

bool MenuSystem::onMouseMove(const int x, const int y, const bool lButton, const bool mButton, const bool rButton){
	if (isInMainMenu(x, y)){
		int mWidth = getHalfWidth();
		float rx = float(xPos - mWidth + 12);

		for (unsigned int i = 0; i < menus.getCount(); i++){
			float lx = rx;
			rx += 32 * fnt->getStringWidth(const_cast<char*>(menus[i]->getTitle().c_str()));
			if (x >= lx && x <= rx){
				currMenu = i;
				break;
			}
			rx += 20;
		}
		return true;
	} else {
		if(menus.getCount())
		return menus[currMenu]->onMouseMove(x, y, lButton, mButton, rButton);
		else
			return false;
	}
}

bool MenuSystem::onMouseWheel(const int x, const int y, const int scroll){
	if (!isInMainMenu(x, y)){
		return menus[currMenu]->onMouseWheel(x, y, scroll);
	}
	return true;
}

bool MenuSystem::onKeyPress(const unsigned int key, const bool pressed){
	if (pressed){
		if (key == STX_KEY_F1){
			visible ^= true;
		} else {
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
		x += 32 *
			fnt->getStringWidth(const_cast<char*>(menus[i]->getTitle().c_str()));
//IRenderer::GetRendererInstance()->
//getStringWidth(0,const_cast<char*>(menus[i]->getTitle().c_str()),0);



	}
}

int MenuSystem::getHalfWidth() const {
	float hWidth = 14.5f;
	for (unsigned int i = 0; i < menus.getCount(); i++){
		float w = 16 *
			fnt->getStringWidth(const_cast<char*>(menus[i]->getTitle().c_str()));
//IRenderer::GetRendererInstance()->
//getStringWidth(0,const_cast<char*>(menus[i]->getTitle().c_str()),0);

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
	color = D3DXFROMWINEVECTOR4(1, 1, 0, 0.5f);
	sliderListener = 0;
}

void Slider::draw(IRenderer *renderer){
#if 0
	IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setBlending(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	#else
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());
	#endif

	

	// Background
	IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos, xPos + width, yPos + height, 10, color);

	//IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());


	

	D3DXFROMWINEVECTOR4 black(0, 0, 0, 1);
	// Border
	IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos, xPos + width, yPos + height, 10, black, 3);

	if (isHorizontal){
		float vx = lerp_(xPos + 12.0f, xPos + width - 12.0f, (value - r0) / (r1 - r0)) + 0.5f;
			IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());

			IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
			//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());

		IRenderer::GetRendererInstance()->drawLine(xPos + 12, yPos + 0.5f * height, xPos + width - 12, yPos + 0.5f * height, black, 3);
			IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
			IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
			//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());

		IRenderer::GetRendererInstance()->drawRoundRect(vx - 5, yPos + 3, vx + 5, yPos + height - 3, 4, black);
	} else {
		float vy = lerp_(yPos + 12.0f, yPos + height - 12.0f, (r1 - value) / (r1 - r0)) + 0.5f;
			IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
			IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
			//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());

		IRenderer::GetRendererInstance()->drawLine(xPos + 0.5f * width, yPos + 12, xPos + 0.5f * width, yPos + height - 12, black, 3);
			IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
			IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
			//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullFront());

		IRenderer::GetRendererInstance()->drawRoundRect(xPos + 3, vy - 5, xPos + width - 3, vy + 5, 4, black);
	}
}

bool Slider::isInWidget(const int x, const int y) const {
	bool r=(x >= xPos && x < xPos + width && y >= yPos && y < yPos + height);
	return r;
}

bool Slider::onMouseClick(const int x, const int y, const unsigned int button, const bool pressed){
	if (isInWidget(x,y)&&(button == LEFT_BUTTON)){
		drag = capture = pressed;
		if (pressed) computeValue(x, y);
		return true;
	}/*
	else if(button == RIGHT_BUTTON)
		{
			drag =
				//capture =
				false;
	return true;
		}*/
	return false;
}

bool Slider::onMouseMove(const int x, const int y, const bool lButton, const bool mButton, const bool rButton){
	if (drag){
		if(isInWidget(x,y))
		{
		computeValue(x, y);
		return true;
		}/*
		else
		{
			drag = capture = false;
	return true;
		}*/
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
	setValue(lerp_(r0, r1, k));
	if (sliderListener) sliderListener->onSliderChange(this);
}


////////////////////////////////////////////////////////////////////////////////////////////

Label::Label(const float ix, const float iy, Font3D *fnt, const char *str, const float chrWidth, const float chrHeight,
			 const float skewx, const float skewy, FONT_HADJUST hAdj, FONT_VADJUST vAdj) : text(str){
	xPos = ix;
	yPos = iy;
	//fnt = fnt;
	charWidth  = chrWidth;
	charHeight = chrHeight;
	skewX = skewx;
	skewY = skewy;
	hAdjust = hAdj;
	vAdjust = vAdj;
	color = D3DXFROMWINEVECTOR4(1, 1, 0, 1);
}

Label::~Label(){
}

void Label::setText(const char *str){
	text = str;
}

void Label::draw(IRenderer *renderer){
	//IRenderer::GetRendererInstance()->setCullFace(BACK);
	IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullBack());
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());//setBlending(CONSTANT_COLOR, ZERO);
	//IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	

	//IRenderer::GetRendererInstance()->changeBlendConstantColor(
		IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", color);

	fnt->drawText(renderer,const_cast<char*>( text.c_str()), xPos, yPos, charWidth, charHeight, skewX, skewY, hAdjust, vAdjust);
//IRenderer::GetRendererInstance()->DrawText( xPos, yPos,
//text.c_str());

}

////////////////////////////////////////////////////////////////////////////////////////////

TextBox::TextBox(const float ix, const float iy, const float w, const float h, Font3D *fnt, const char *str) : text(str){
	xPos = ix;
	yPos = iy;
	width  = w;
	height = h;
	//fnt = fnt;
	color = D3DXFROMWINEVECTOR4(1, 0, 0, 0.5f);
}

TextBox::~TextBox(){
}

void TextBox::setText(const char *str){
	text = str;
}

void TextBox::draw(IRenderer *renderer){
#if 0
	IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setBlending(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	#else
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	#endif

	

	// Background
	IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos, xPos + width, yPos + height, 7, color);

	// Border
	//IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());

	

	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());

	IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos, xPos + width, yPos + height, 7, D3DXFROMWINEVECTOR4(0, 0, 0, 1), 3);

	//IRenderer::GetRendererInstance()->setCullFace(BACK);
	IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullBack());
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());//setBlending(CONSTANT_COLOR, ZERO);
//	IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	

	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(0.7f, 1, 1, 0));

	fnt->drawText(renderer, const_cast<char*>(text.c_str()), xPos + 0.5f * width, yPos + 0.5f * height + 2, 16, 20, 0.15f, -0.1f, HA_CENTER, VA_CENTER);
//IRenderer::GetRendererInstance()->DrawText( xPos + 0.5f * width, yPos + 0.5f * height + 2,
//text.c_str());

}

bool TextBox::isInWidget(const int x, const int y) const {
	return (x >= xPos && x < xPos + width && y >= yPos && y < yPos + height);
}

////////////////////////////////////////////////////////////////////////////////////////////

PushButton::PushButton(const float ix, const float iy, const float w, const float h, Font3D *fnt, const char *text) : TextBox(ix, iy, w, h, fnt, text){
	pushed = false;
	pushButtonListener = 0;
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
		color.x = 1.0f - color.x;
		color.y = 1.0f - color.y;
		color.z = 1.0f - color.z;
		color.w = 1.0f - color.w;
		pushed = val;
		if (!pushed && pushButtonListener != 0) pushButtonListener->onPushButtonPress(this);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

KeyWaiter::KeyWaiter(const float ix, const float iy, Font3D *fnt, KeyWaiterListener *listener){
	xPos = ix;
	yPos = iy;

	//fnt = fnt;

	keyWaiterListener = listener;

	color = D3DXFROMWINEVECTOR4(0, 0.75f, 1.0f, 0.7f);
	capture = true;
}

KeyWaiter::~KeyWaiter(){
}

void KeyWaiter::draw(IRenderer *renderer){
	float x0 = xPos - 125;
	float y0 = yPos - 30;
	float x1 = xPos + 125;
	float y1 = yPos + 30;

#if 0
	IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setBlending(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	#else
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	#endif

	

	// Background
	IRenderer::GetRendererInstance()->drawRoundRect(x0, y0, x1, y1, 16, color);

	// Border
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());

	
	IRenderer::GetRendererInstance()->drawRoundRect(x0, y0, x1, y1, 16, D3DXFROMWINEVECTOR4(0, 0, 0, 1), 3);

	//IRenderer::GetRendererInstance()->setCullFace(BACK);
	IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullBack());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());//setBlending(CONSTANT_COLOR, ZERO);
	//IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());


	

	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1, 0.6f, 0, 0));

	fnt->drawText(renderer, "Press desired key ...", xPos, yPos + 2, 24, 36, 0.15f, -0.1f, HA_CENTER, VA_CENTER);
//IRenderer::GetRendererInstance()->DrawText( xPos, yPos + 2,
//"Press desired key ...");

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

MsgBox::MsgBox(const float ix, const float iy, Font3D *fnt, const char *str) : text(str){
	xPos = ix;
	yPos = iy;
	//fnt = fnt;

	float x0, y0, x1, y1;
	getRect(&x0, &y0, &x1, &y1);

	okButton = new PushButton(ix - 22, y1 - 48, 44, 36, fnt, "OK");

	color = D3DXFROMWINEVECTOR4(0, 0.2f, 0.7f, 0.5f);

	capture = true;
}

MsgBox::~MsgBox(){
	delete okButton;
}

void MsgBox::draw(IRenderer *renderer){
	float x0, y0, x1, y1;
	getRect(&x0, &y0, &x1, &y1);

#if 0
	IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setBlending(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	#else
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	#endif

	

	// Background
	IRenderer::GetRendererInstance()->drawRoundRect(x0, y0, x1, y1, 20, color);

	// Border
//	IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
		IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());

	IRenderer::GetRendererInstance()->drawRoundRect(x0, y0, x1, y1, 20, D3DXFROMWINEVECTOR4(0, 0, 0, 1), 3);

//	IRenderer::GetRendererInstance()->changeCullFace(BACK);
	IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullBack());
		IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());

	fnt->drawText(renderer, const_cast<char*>(text.c_str()), xPos, yPos - 20, 24, 36, 0.15f, -0.1f, HA_CENTER, VA_CENTER);
//IRenderer::GetRendererInstance()->DrawText( xPos, yPos - 20,
//text.c_str());

	okButton->draw(renderer);
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
	if (key == STX_KEY_RETURN || key == STX_KEY_SPACE || key == STX_KEY_ESCAPE){
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
	IRenderer::GetRendererInstance()->getExtentsVrtl(0,const_cast<char*>(text.c_str()), float(xPos), float(yPos), 24, 36, 0.15f, -0.1f,
//		HA_CENTER, VA_CENTER,
		x0, x1, y0, y1);
	*x0 -= 10;
	*x1 += 10;
	*y0 -= 30;
	*y1 += 30;
}

////////////////////////////////////////////////////////////////////////////////////////////

DropDownList::DropDownList(const float ix, const float iy, const float w, const float h, Font3D *fnt){
	xPos = ix;
	yPos = iy;
	width  = w;
	height = h;
	//fnt = fnt;

	dropDownListener = 0;

	selectedItem = -1;
	droppedDown = false;
	incDown = decDown = false;

	color = D3DXFROMWINEVECTOR4(0.1f, 1, 0.2f, 0.5f);
}

DropDownList::~DropDownList(){
	for (unsigned int i = 0; i < items.getCount(); i++){
		delete items[i];
	}
}

ListItemID DropDownList::addItem(const char *string){
    char *str = new char[stx_strlen(string) + 1];
    stx_strlcpy(str, string,stx_strlen(string) + 1);
	return items.add(str);
}

void DropDownList::setSelectedItem(ListItemID item){
	if (item != selectedItem){
		selectedItem = item;
		if (dropDownListener) dropDownListener->onDropDownListChange(this);
	}
}

void DropDownList::draw(IRenderer *renderer){
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

#if 0
	IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setBlending(SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
	#else
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	#endif

	

	D3DXFROMWINEVECTOR4 clickCol(0.5f * color.x,0.5f * color.y,0.5f * color.z, color.w);

	// Background
	IRenderer::GetRendererInstance()->drawRoundRect(xPos,   yPos, xEdge0, yPos + height, 8, color);
		IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());

	IRenderer::GetRendererInstance()->drawRoundRect(xEdge1, yPos, xRight, yPos + 0.5f * height - 1, 5, decDown? clickCol : color);
		IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());

	IRenderer::GetRendererInstance()->drawRoundRect(xEdge1, yPos + 0.5f * height + 1, xRight, yPos + height, 5, incDown? clickCol : color);

	// Borders
	D3DXFROMWINEVECTOR4 black(0, 0, 0, 1);
		IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());

	IRenderer::GetRendererInstance()->drawRoundRect(xPos,   yPos, xEdge0, yPos + height, 8, black, 3);
		IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());

	IRenderer::GetRendererInstance()->drawRoundRect(xEdge1, yPos, xRight, yPos + 0.5f * height - 1, 5, black, 3);
		IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());

	IRenderer::GetRendererInstance()->drawRoundRect(xEdge1, yPos + 0.5f * height + 1, xRight, yPos + height, 5, black, 3);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());

	IRenderer::GetRendererInstance()->drawTriangle(
		xEdge1 + 0.5f * height,     yPos + 0.1f * height,
		xEdge1 + 0.8f * height - 1, yPos + 0.4f * height - 1,
		xEdge1 + 0.2f * height + 1, yPos + 0.4f * height - 1, black);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());

	IRenderer::GetRendererInstance()->drawTriangle(
		xEdge1 + 0.5f * height,     yPos + 0.9f * height,
		xEdge1 + 0.2f * height + 1, yPos + 0.6f * height + 1,
		xEdge1 + 0.8f * height - 1, yPos + 0.6f * height + 1, black);

	if (droppedDown){
			IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
			IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());

		IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos - (firstItem + selectedItem) * height, xEdge0, yPos + (lastItem - selectedItem) * height, 8, color);
			IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
			IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());

		IRenderer::GetRendererInstance()->drawRoundRect(xPos, yPos - (firstItem + selectedItem) * height, xEdge0, yPos + (lastItem - selectedItem) * height, 8, black, 3);
	}

	//IRenderer::GetRendererInstance()->setCullFace(BACK);
	IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullBack());
		IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());//setBlending(CONSTANT_COLOR, ZERO);
//	IRenderer::GetRendererInstance()->setDepthFunc(DEPTH_NONE);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	

	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(0.3f, 0.3f, 0.9f, 0));

	for (int i = firstItem; i < lastItem; i++){
		fnt->drawText(renderer, items[i], xPos + 0.3f * height, yPos + (i - selectedItem + 0.5f) * height, 0.6f * height, 0.7f * height, 0.15f, -0.1f, HA_LEFT, VA_CENTER);

//IRenderer::GetRendererInstance()->DrawText( xPos + 0.3f * height, yPos + (i - selectedItem + 0.5f) * height,
//items[i]);

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
#elif 0
VArray <Menu *> BaseApp::menus;
MenuID BaseApp::currMenu=-1;
FONT3D *BaseApp::font=0;
MenuListener *BaseApp::menuListener=0;
MenuSystem *BaseApp::mainMenu=0;
Queue <Widget *> BaseApp::widgets;
#endif

