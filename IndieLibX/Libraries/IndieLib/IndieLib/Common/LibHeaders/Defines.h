/*****************************************************************************************
 * File: Defines.h
 * Desc: Types, defines, modos, etc
 *****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (info@pixelartgames.com)

This library is free software; you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place,
Suite 330, Boston, MA 02111-1307 USA 
*/
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
	
*/

/*!
\mainpage
\section main_general Menu
<A HREF="modules.html">Click here to access the Menu</A>
*/

/**
 * \defgroup Classes
 *
 */


/**
 * \defgroup Init_End Initialization / Destruction
 * \ingroup Classes
 */

/**
 * \defgroup IndieLib_Init_End Init / End
 * \ingroup Init_End
 */


/**
 * \defgroup Main Main
 * \ingroup Classes
 */

/**
 * \defgroup Main Main
 * \ingroup Classes
 */

/**
 * \defgroup IND_Window IND_Window
 * \ingroup Main
 */

/**
 * \defgroup IND_Render IND_Render
 * \ingroup Main
 */


/**
 * \defgroup EntityManagers Entity Objects and Entity Managers (the most important concept of IndieLib)
 * \ingroup Classes
 */

/**
 * \defgroup Managers Managers
 * \ingroup Classes
 */

/**
 * \defgroup Objects Graphical Objects (that can be inserted into the entities)
 * \ingroup Classes
 */

/**
 * \defgroup Cameras Cameras
 * \ingroup Classes
 */

/**
 * \defgroup Input Input from keyboard and mouse
 * \ingroup Classes
 */

/**
 * \defgroup Timer Timer
 * \ingroup Classes
 */

/**
 * \defgroup Advances Advances methods for bliting directly to the screen without using entities. It uses IND_Render methods (recommended only for advanced IndieLib users)
 */

/**
 * \defgroup Primitives Bliting Primitives
 * \ingroup Advances
 */

/**
 * \defgroup Graphical_Objects Bliting Surfaces, Animations and Fonts and setting the transformations directly
 * \ingroup Advances
With these methods you can set the 2d transformations (using IND_Render::Set2dTransform) and color
attributes (using IND_Render::SetRainbow()) and blit directly
to the screen surfaces, animations, and fonts using IND_Render class.

Remember that you can use ::IND_Entity2d together with ::IND_Entity2dManager for drawing these graphical objects
to the screen without having to use this advanced method directly. This method is only useful for advanced
users for really concrete purposes.
 */


/**
 * \defgroup Graphical_3d_Objects Bliting 3d Animated 3d Models and setting the transformations directly
 * \ingroup Advances
With these methods you can set the 3d transformations (using IND_Render::Set3dTransform) and color
attributes (using IND_Render::SetRainbow()) and to blit directly
to the screen 3d animated meshes using IND_Render class.

Remember that you can use ::IND_Entity3d together with ::IND_Entity3dManager for drawing these graphical objects
to the screen without having to use this advanced method directly. This method is only useful for advanced
users for really concrete purposes.
 */


/**
 * \defgroup Types
 */



#ifndef _DEFINES_
#define _DEFINES_

#include <Framework3/IRenderer.h>

#define INDIELIBAPIENTRY
#define INDIELIBAPI

// ----- Lib export -----
#define INLINE
#define DLL_EXP

#if !defined(SWIG) && !defined(SWIG_)
#if defined(ANDROID)
#define IndieLib() mainX(int argc, char *argv[])
#elif defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
#define IndieLib() mainX (int argc, char *argv[])
#elif defined(WIN_PHONE_APP)
#define IndieLib() mainX (int argc, char *argv[])
#elif defined(_MSC_VER)
#pragma comment(linker, "/ENTRY:WinMainCRTStartup")
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#if 0//!defined(_CINDIELIB_INTERNAL_) && defined(_CINDIELIB_)
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\Wbemuuid.lib")
#if 1//defined(__32_BIT__)
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\kernel32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\user32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\gdi32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\winspool.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\comdlg32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\advapi32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\shell32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\ole32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\oleaut32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\uuid.lib")
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\odbc32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\odbccp32.lib")  
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\kernel32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\user32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\gdi32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\winspool.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\comdlg32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\advapi32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\shell32.lib") 
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\ole32.lib")
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\oleaut32.lib")
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\uuid.lib")
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\odbc32.lib")
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\odbccp32.lib")
#pragma comment(lib, "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\winmm.lib")
#elif 0//defined(__64_BIT__)
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\kernel32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\user32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\gdi32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\winspool.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\comdlg32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\advapi32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\shell32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\ole32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\oleaut32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\uuid.lib")
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\odbc32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\odbccp32.lib")  
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\kernel32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\user32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\gdi32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\winspool.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\comdlg32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\advapi32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\shell32.lib") 
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\ole32.lib")
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\oleaut32.lib")
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\uuid.lib")
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\odbc32.lib")
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\odbccp32.lib")
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft Platform SDK for Windows Server 2003 R2\\Lib\\winmm.lib")
#endif
#pragma comment(lib, "C:\\IndieLibX\\Dependencies\\Cg\\lib\\cg.lib")
#pragma comment(lib, "C:\\IndieLibX\\Dependencies\\Cg\\lib\\cgGL.lib")
#pragma comment(lib, "C:\\IndieLibX\\Dependencies\\Cg\\lib\\cgD3D8.lib")
#pragma comment(lib, "C:\\IndieLibX\\Dependencies\\Cg\\lib\\cgD3D9.lib")
#pragma comment(lib, "C:\\IndieLibX\\Dependencies\\Cg\\lib\\cgD3D10.lib")
#pragma comment(lib, "C:\\IndieLibX\\Dependencies\\Cg\\lib\\cgD3D11.lib")
#pragma comment(lib, "C:\\IndieLibX\\Dependencies\\OpenGL\\lib\\OPENGL32.lib")
#pragma comment(lib, "C:\\IndieLibX\\Dependencies\\OpenGL\\lib\\glut32.lib")
#pragma comment(lib, "C:\\IndieLibX\\Dependencies\\OpenGL\\lib\\GLU32.lib")
#if 0//defined(__64_BIT__)
#pragma comment(lib,	"C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x64\\d3d9.lib")
#pragma comment(lib,	"C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x64\\d3dx9.lib")
#elif 1//defined(__32_BIT__)
#pragma comment(lib,	"C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\d3d9.lib")
#pragma comment(lib,	"C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\d3dx9.lib")
#endif
#pragma comment(lib, "C:\\IndieLibX\\Dependencies\\SDL-1.2.14\\lib\\SDL.lib")
//pragma comment(lib, "C:\\IndieLibX\\lib\\segvcatch.lib")
//pragma comment(lib, "C:\\IndieLibX\\lib\\whirlgif_x.lib")
#pragma comment(lib, "C:\\IndieLibX\\lib\\png_x.lib")
#pragma comment(lib, "C:\\IndieLibX\\lib\\zlib_x.lib")
#pragma comment(lib, "C:\\IndieLibX\\lib\\jpeg_x.lib")
#pragma comment(lib, "C:\\IndieLibX\\lib\\IL_x.lib")
#pragma comment(lib, "C:\\IndieLibX\\lib\\ILU_x.lib")
#pragma comment(lib, "C:\\IndieLibX\\lib\\STX.lib")
#pragma comment(lib, "C:\\IndieLibX\\lib\\STXCPP.lib")
//pragma comment(lib, "C:\\IndieLibX\\lib\\hlsl2glsl.lib")
#pragma comment(lib, "C:\\IndieLibX\\lib\\Framework3.lib")
#ifdef BUILD_MESHRENDERER
#pragma comment(lib, "C:\\IndieLibX\\lib\\assimp.lib")
#pragma comment(lib, "C:\\IndieLibX\\lib\\MeshRenderer2.lib")
#endif
#pragma comment(lib, "C:\\IndieLibX\\lib\\tinyxml.lib")
#pragma comment(lib, "C:\\IndieLibX\\lib\\IndieLib_Static.lib")
#endif
#define IndieLib() WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#elif !defined(_MSC_VER)
#define IndieLib() main(int argc, char *argv[])
#endif
#endif

#if !defined(_MSC_VER) 
#define INDIELIB(X) GameLogic gl; \
IGameLogic* glp=&gl; \
int init(const char* aTitle){return glp->init(X);} \
int render(){return glp->render();} \
int main(int argc, char *argv[]) \
{ \
	glp->init(X); \
	while (!STX_Service::GetInputInstance()->OnKeyPress (KEY_ESCAPE) && !STX_Service::GetInputInstance()->Quit()) \
		glp->render(); \
	CIndieLib::Instance()->End(); \
	return 0; \
}
#else
#define INDIELIB(X) GameLogic gl; \
IGameLogic* glp=&gl; \
int init(const char* aTitle){return glp->init(X);} \
int render(){return glp->render();} \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) \
{ \
	glp->init(X); \
	while (!STX_Service::GetInputInstance()->OnKeyPress (KEY_ESCAPE) && !STX_Service::GetInputInstance()->Quit()) \
		glp->render(); \
	CIndieLib::Instance()->End(); \
	return 0; \
}
#endif

// ----- Const -----

#ifndef PI
#define PI 3.1415f
#endif


// --------------------------------------------------------------------------------
//									Pixel and vertex formats
// --------------------------------------------------------------------------------

//typedef int VertexFormatID;
typedef int ShaderID;
typedef int VertexFormatID;
//(D3DFVF_XYZ | D3DFVF_DIFFUSE)

// Vertex format
struct INDIELIBAPI structVertex2d
{
	float mX, mY, mZ;			// Position
	float mU, mV;				// Mapping coordinates
};
typedef struct structVertex2d CUSTOMVERTEX2D;
//(D3DFVF_XYZ | D3DFVF_TEX1)

VertexFormatID& GetPIXELVertexFormat();
ShaderID& GetPIXELShader();
VertexFormatID& GetCUSTOMVertexFormat2D();
VertexFormatID& GetCUSTOMVertexFormat2DW();
ShaderID& GetCUSTOMShader2D();
ShaderID& GetCUSTOMShader2DW();

struct IndieLibShadersFactory
{
	static std::string GetShader(const char* aShaderName);
};

// --------------------------------------------------------------------------------
//										Structures
// --------------------------------------------------------------------------------

/**
 * \defgroup Additional structures
 * \ingroup Types
 */
/*@{*/

// Matrix
struct INDIELIBAPI structMatrix
{
#if 0
	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
	float _41, _42, _43, _44;
#else
	float m[4][4];
#endif
	structMatrix ()
	{
//		_11 = _12 = _13 = _14 = _21 = _22 = _23 = _24 = _31 = _32 = _33 = _34 = _41 = _42 = _43 = _44 = 0;
	stx_memset(m,0,sizeof(float)*16);
	}

};
//! Matrix 4x4\n (_11, _12, _13, _14,\n _21, _22, _23, _24,\n _31, _32, _33, _34\n _41, _42, _43, _44)
typedef struct structMatrix IND_Matrix;


// Point
struct INDIELIBAPI structPoint
{
	int x, y;
};
//! 2d Point 2d\n (x, y)
typedef struct structPoint IND_Point;
/*@}*/
// Pixel

// --------------------------------------------------------------------------------
//									Free memory operations
// --------------------------------------------------------------------------------

#define Dispose(x)		if (x)	{ delete	x;  x = 0; }
#define DisposeArray(x) 	if (x)	{ delete [] x;  x = 0; }


// --------------------------------------------------------------------------------
//									Color Formats
// --------------------------------------------------------------------------------

/**
 * \defgroup IND_Format
 * \ingroup Types
 */
/*@{*/

//! Color formats

/*!
IND_Image objects can have different formats depending on the number of colors. To have \b truecolor, the range of colors
that the human eye can perceive,  3 bytes per pixel are necessary. These are   \b R (Red), \b G (Green)
and \b B (Blue) bytes.

When we want images to have per pixel transparency, it is necessary an extra
unsigned char. The A unsigned char, also called alpha chanel. This unsigned char doesn't show a color, but a level of transparency
 (0 full transparent, 255 full opaque).



It is also possible to define images with less colors and to obtain good results.
For example using gray range (they only use one unsigned char per pixel).

<b>Types IND_Format</b>

\arg ::IND_RGB
\arg ::IND_RGBA
\arg ::IND_BGR
\arg ::IND_BGRA
\arg ::IND_LUMINANCE
\arg ::IND_COLOR_INDEX
*/
typedef int IND_Format;
typedef int IND_Align;

#define IND_COLOUR_INDEX					1
//! Colour palette.
#define IND_COLOR_INDEX						1
//! Real color, using 3 bytes per pixel.
#define IND_RGB								2
//! Real color, using 4 bytes per pixel. The fourth unsigned char is used to the transparency variable.
#define IND_RGBA							3
//! Real colour using 3 bytes per pixel.
#define IND_BGR								4
//! Real color using 3 bytes per pixel. The fourth unsigned char is used to the transparency variable.
#define IND_BGRA							5
//! Gray range using 1 unsigned char per pixel.
#define IND_LUMINANCE						6
//#define IND_LUMINANCE_ALPHA					7
/*@}*/


// --------------------------------------------------------------------------------
//								     Colour quality
// --------------------------------------------------------------------------------

/**
 * \defgroup IND_Quality
 * \ingroup Types
 */
/*@{*/

//! Colour quality

/*!
\b IND_Quality shows the quality of the entity that it references.

You can consume less memory by adjusting this parameter, and as a result losing some quality in the colors.

This atributte alone let us to put entities (fonts, surfaces, etc) in gray color range.
*/
typedef int IND_Quality;

//! It uses 8 bits for gray range. This is the attribute that wastes less memory.
#define IND_GREY_8							1
//! This is like  ::IND_GREY_8 but it uses 16 bits for gray range, getting better quality for images that we want to render in "black and white".
#define IND_GREY_16							2
//! It uses 16 bits colour. Sometimes the diference between this attribute and  ::IND_32 is indiscernible, but this one wastes less memory.
#define IND_16								3//???2
//! It uses a 32 bits of real colour. This is the maximum colour quality that can be get with this library and consistently the one that wastes more memory.
#define IND_32								4
/*@}*/


// --------------------------------------------------------------------------------
//								     Light types
// --------------------------------------------------------------------------------

/**
 * \defgroup IND_LightType
 * \ingroup Types
 */
/*@{*/

//! Light types

/*!
\b IND_LightType is the type of the light

You can define different type of lights using this type.
*/
typedef int IND_LightType;

/*!
Ambient light represents the background light in a scene. In the real world light bounces off many objects and creates
a low light level so even geometry facing away from any directional light is still partially lit. Ambient light is very
inexpensive in terms of frame rate - almost free. It is applied to all surfaces irrespective of the direction they are
facing. There can be only one ambient light enabled at a time.
*/
#define IND_AMBIENT_LIGHT							100

/*!
Directional light is used to simulate distant light sources like the sun. It has a direction but no position. e.g.
you could set it to point downward (0,-1,0) which would simulate the sun being directly overhead. Directional light is
relatively inexpensive although if you add lots of them you may find your frame rate dropping somewhat. The lighting
effect on objects depends on which way they are facing relative to the light direction (defined in the vertex normal).
So a triangle with normal pointing straight up (0,1,0) would be lit by our sun (0,-1,0) fully - it would receive the
maximum amount of light. As the angle of the normal to light increases less light is applied. If the triangle normal
faces away from the light it is not lit at all.

Methods you can use for changing the attributes of these type of lights:
- IND_Light::SetDirection()
- IND_Light::SetRange()
- IND_Light::SetColor()
*/
#define IND_DIRECTIONAL_LIGHT						101

/*!
A point light represents a point source. It has a position in the world and radiates light in all directions. A bare
bulb on a stand would be an example of this. It radiates light in every direction and has a position in the world. This
is a more expensive type of light in terms of frame rate than a directional light.

A point light has an attenuation (use IND_Light::SetAttenuation()) that defines how the light level decreases over distance
and a range. The range (use IND_Light::SetRange()) is the maximum distance the light will travel.

Methods you can use for changing the attributes of these type of lights:
- IND_Light::SetPosition()
- IND_Light::SetRange()
- IND_Light::SetColor()
- IND_Light::SetAttenuation()
- IND_Light::SetRange()
*/
#define IND_POINT_LIGHT								102

/*!
Spot light is is the most complex light and is the most expensive type in
terms of frame rate so use sparingly. A spot light has a position in space and a direction. It emits a cone of light
with two degrees of intensity. It has a central brightly lit section and a surrounding dimly lit section that merges
with the surrounding shadow. Only objects within the light cone are illuminated.

To set a spot light you need to provide position, direction, cone size and a number of parameters determining the spread of
the cone. Theta (use IND_Light::SetTheta()) is the angle that defines the inner cone while Phi (use IND_Light::SetPhi())
defines the outer cone. Falloff (use IND_Light::SetFalloff()) defines the decrease in illumination between the outside of
the inner cone and the outside of the outer cone

Methods you can use for changing the attributes of these type of lights:
- IND_Light::SetPosition()
- IND_Light::SetRange()
- IND_Light::SetColor()
- IND_Light::SetAttenuation()
- IND_Light::SetRange()
- IND_Light::SetFalloff()
- IND_Light::SetPhi()
- IND_Light::SetTheta()
*/
#define IND_SPOT_LIGHT								103
/*@}*/



// --------------------------------------------------------------------------------
//							          Surface types
// --------------------------------------------------------------------------------

/**
 * \defgroup IND_Type
 * \ingroup Types
 */
/*@{*/

//! Surface types

/*!
\b IND_Type (type of surface).

Specifying the type  (opaque or transparent), you can consume less memory. For example,
when drawing the background of a game, that background usually doesn't
need alpha chanel for transparency (dosent't have transparent pixels), so it can be loaded
as  ::IND_OPAQUE.

However, another entity of your game could need transparency in some pixels of the image, then
you have to use the type
::IND_ALPHA. This type consumes more memory that ::IND_OPAQUE but both will be equally fast
when rendering.

<b>Types IND_Type</b>

\arg ::IND_OPAQUE
\arg ::IND_ALPHA
*/
typedef int IND_Type;

//! It  consumes less memory than IND_ALPHA, but it doesn't allow transparency. Usually it is used for backgrounds or entities that haven't transparent pixels inside the image. This method isn't faster when rendering than ::IND_ALPHA.
#define IND_OPAQUE							200
//! It allows per pixel transparency, but consumes more memory that IND_OPAQUE.
#define IND_ALPHA							202
/*@}*/



// --------------------------------------------------------------------------------
//									Primitives
// --------------------------------------------------------------------------------

/**
 * \defgroup IND_Primitive2d
 * \ingroup Types
 */
/*@{*/

//! Type of primitives

/*!
IND_Primitive2d defines a primitive (lines, circles, rectangles, etc).
*/
typedef int IND_Primitive2d;

//! Pixel (dot).
#define IND_PIXEL							400
//! Line.
#define IND_LINE							401
//! Rectangle.
#define IND_RECTANGLE						402
//! Polygon. The vertex list is an array of ::IND_Point, that type consist on two coordinates (x, y).
#define IND_POLY2D							403
//! Regular polygon with n sides.
#define IND_REGULAR_POLY					404
//! Filled rectangle.
#define IND_FILL_RECTANGLE					405
/*@}*/


// --------------------------------------------------------------------------------
//										Blending
// --------------------------------------------------------------------------------

/**
 * \defgroup IND_BlendingType
 * \ingroup Types
 */
/*@{*/

//! Blending type

/*!
Type \b IND_BlendigType define the type of blend (to do alphablending). Alphablending is a powerful tool that can be used
to create lots of graphical efects ( burst, electricity, speck efects, rain, etc).

The more usual factors of  blending are:

\arg (source, destination) = (::IND_SRCALPHA, ::IND_INVSRCALPHA) => This is the usual method of  blending to draw picture with alpha channel. It regards the alpha channel to draw pictures that have variable transparency.
\arg (source, destination) = (::IND_SRCALPHA, ::IND_ONE) => It is used to speck effects. In the source picture it has black areas than later will be transparent in the screen.

You can try diferents values to the source and to the destination and you can get a big amount of
diverse efects as much additive as subtractive (relative to RGBA values) and a great visual impact.
*/
typedef int IND_BlendingType;

//! Blend factor  (0, 0, 0, 0).
#define IND_ZERO							500
//! Blend factor (1, 1, 1, 1).
#define IND_ONE								501
//! Blend factor (Rs,Gs,Bs,As).
#define IND_SRCCOLOR						502
//! Blend factor (1 - Rs, 1 - Gs, 1 - Bs, 1 - As).
#define IND_INVSRCCOLOR						503
//! Blend factor (As, As, As, As).
#define IND_SRCALPHA						504
//! Blend factor ( 1 - As, 1 - As, 1 - As, 1 - As).
#define IND_INVSRCALPHA						505
//! Blend factor (Ad, Ad, Ad, Ad).
#define IND_DESTALPHA						506
//! Blend factor (1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad).
#define IND_INVDESTALPHA					507
//! Blend factor (Rd, Gd, Bd, Ad).
#define IND_DESTCOLOR						508
//! Blend factor (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad).
#define IND_INVDESTCOLOR					509
//! Blend factor (f, f, f, 1); f = min(A, 1 - Ad).
#define IND_SRCALPHASAT						510
//! Obsoleto.
#define IND_BOTHSRCALPHA					511
//! Blend factor (1 - As, 1 - As, 1 - As, 1 - As), y el destino (As, As, As, As);
#define IND_BOTHINVSRCALPHA					512
//! Blend colour.
#define IND_BLENDFACTOR						513
//! Blend opposite colour.
#define IND_INVBLENDFACTOR					514
/*@}*/


// --------------------------------------------------------------------------------
//									   Filters
// --------------------------------------------------------------------------------

/**
 * \defgroup IND_Filter
 * \ingroup Types
 */
/*@{*/

//! TextureOTL filters

/*!
These filters affects the surfaces when they are rendered on the screen. When we rotate or
we resize a surface, the quality of the image will be better or worse depending on the filter used.

<b>Types IND_Filter</b>

\arg ::IND_FILTER_NONE
\arg ::IND_FILTER_POINT
\arg ::IND_FILTER_LINEAR
*/
typedef int IND_Filter;

//! No filter applied.
#define IND_FILTER_NONE						0
//! Nearest point filter. This is faster than  IND_FILTER_LINEAR, but has less quality.
#define IND_FILTER_POINT					1
//! Bilinear interpolation filter. This is quite fast, and has better quality than IND_FILTER_POINT.
#define IND_FILTER_LINEAR					2
/*@}*/

#if 1
// --------------------------------------------------------------------------------
//										  Keys
// --------------------------------------------------------------------------------

/**
 * \defgroup IND_Key
 * \ingroup Types
 */
/*@{*/

//! Keys

/*!
Input keys of the keyboard.
*/
typedef int IND_Key;
#if 0
enum
{
	//! a
	IND_A=SDLK_a,
	//! b
	IND_B=SDLK_b,
	//! c
	IND_C=SDLK_c,
	//! d
	IND_D=SDLK_d,
	//! e
	IND_E=SDLK_e,
	//! f
	IND_F=SDLK_f,
	//! g
	IND_G=SDLK_g,
	//! h
	IND_H=SDLK_h,
	//! i
	IND_I=SDLK_i,
	//! j
	IND_J=SDLK_j,
	//! k
	IND_K=SDLK_k,
	//! l
	IND_L=SDLK_l,
	//! m
	IND_M=SDLK_m,
	//! n
	IND_N=SDLK_n,
	//! o
	IND_O=SDLK_o,
	//! p
	IND_P=SDLK_p,
	//! q
	IND_Q=SDLK_q,
	//! r
	IND_R=SDLK_r,
	//! s
	IND_S=SDLK_s,
	//! t
	IND_T=SDLK_t,
	//! u
	IND_U=SDLK_u,
	//! v
	IND_V=SDLK_v,
	//! w
	IND_W=SDLK_w,
	//! x
	IND_X=SDLK_x,
	//! y
	IND_Y=SDLK_y,
	//! z
	IND_Z=SDLK_z,

	//! 0
	IND_0=SDLK_0,
	//! 1
	IND_1=SDLK_1,
	//! 2
	IND_2=SDLK_2,
	//! 3
	IND_3=SDLK_3,
	//! 4
	IND_4=SDLK_4,
	//! 5
	IND_5=SDLK_5,
	//! 6
	IND_6=SDLK_6,
	//! 7
	IND_7=SDLK_7,
	//! 8
	IND_8=SDLK_8,
	//! 9
	IND_9=SDLK_9,

	//! backspace key
	IND_BACKSPACE=SDLK_BACKSPACE,
	//! Tab
	IND_TAB=SDLK_TAB,
	//! Clear
	IND_CLEAR=SDLK_CLEAR,
	//! Return
	IND_RETURN=SDLK_RETURN,
	//! Pause
	IND_PAUSE=SDLK_PAUSE,
	//! Escape
	IND_ESCAPE=SDLK_ESCAPE,
	//! Space
	IND_SPACE=SDLK_SPACE,
	//! Exclaim
	IND_EXCLAIM=SDLK_EXCLAIM,
	//! Exclaim
	IND_QUOTEDBL=SDLK_QUOTEDBL,
	//! hash
	IND_HASH=SDLK_HASH,
	//! Dollar
	IND_DOLLAR=SDLK_DOLLAR,
	//! ampersand
	IND_AMPERSAND=SDLK_AMPERSAND,
	//! quote
	IND_QUOTE=SDLK_QUOTE,
	//! left parenthesis
	IND_LEFTPAREN=SDLK_LEFTPAREN,
	//! right parenthesis
	IND_RIGHTPAREN=SDLK_RIGHTPAREN,
	//! Asterisk
	IND_ASTERISK=SDLK_ASTERISK,
	//! plus
	IND_PLUS=SDLK_PLUS,
	//! Comma
	IND_COMMA=SDLK_COMMA,
	//! Minus
	IND_MINUS=SDLK_MINUS,
	//! Period
	IND_PERIOD=SDLK_PERIOD,
	//! slash
	IND_SLASH=SDLK_SLASH,

	//! colon
	IND_COLON=SDLK_COLON,
	//! semicolon
	IND_SEMICOLON=SDLK_SEMICOLON,
	//! less than
	IND_LESS=SDLK_LESS,
	//! equals
	IND_EQUALS=SDLK_EQUALS,
	//! greater than
	IND_GREATER=SDLK_GREATER,
	//! question
	IND_QUESTION=SDLK_QUESTION,
	//! at
	IND_AT=SDLK_AT,
	//! left bracket
	IND_LEFTBRACKET=SDLK_LEFTBRACKET,
	//! Back slash
	IND_BACKSLASH=SDLK_BACKSLASH,
	//! right bracket
	IND_RIGHTBRACKET=SDLK_RIGHTBRACKET,
	//! caret
	IND_CARET=SDLK_CARET,
	//! underscore
	IND_UNDERSCORE=SDLK_UNDERSCORE,
	//! back quote
	IND_BACKQUOTE=SDLK_BACKQUOTE,

	//! Delete
	IND_DELETE=SDLK_DELETE,

	//! 0 (numeric keyboard area)
	IND_K0=SDLK_,
	//! 1 (numeric keyboard area)
	IND_K1=SDLK_,
	//! 2 (numeric keyboard area)
	IND_K2=SDLK_,
	//! 3 (numeric keyboard area)
	IND_K3=SDLK_,
	//! 4 (numeric keyboard area)
	IND_K4=SDLK_,
	//! 5 (numeric keyboard area)
	IND_K5=SDLK_,
	//! 6 (numeric keyboard area)
	IND_K6=SDLK_,
	//! 7 (numeric keyboard area)
	IND_K7=SDLK_,
	//! 8 (numeric keyboard area)
	IND_K8=SDLK_,
	//! 9 (numeric keyboard area)
	IND_K9=SDLK_,

	//! Period (numeric keyboard area)
	IND_KPERIOD=SDLK_,
	//! Divide (numeric keyboard area)
	IND_KDIVIDE=SDLK_,
	//! Multiply (numeric keyboard area)
	IND_KMULTIPLY=SDLK_,
	//! Minus (numeric keyboard area)
	IND_KMINUS=SDLK_,
	//! Plus (numeric keyboard area)
	IND_KPLUS=SDLK_,
	//! Enter (numeric keyboard area)
	IND_KENTER=SDLK_,
	//! Equals (numeric keyboard area)
	IND_KEQUALS=SDLK_,

	//! Key up
	IND_KEYUP=SDLK_KEYUP,
	//! Key down
	IND_KEYDOWN=SDLK_KEYDOWN,
	//! Key right
	IND_KEYRIGHT=SDLK_KEYRIGHT,
	//! Key left
	IND_KEYLEFT=SDLK_KEYLEFT,

	//! Insert
	IND_INSERT=SDLK_INSERT,
	//! Home
	IND_HOME=SDLK_HOME,
	/*
	//! End
	IND_END=SDLK_,
	//! Page up
	IND_PAGEUP=SDLK_,
	//! Page down
	IND_PAGEDOWN=SDLK_,

	//! F1
	IND_F1=SDLK_,
	//! F2
	IND_F2=SDLK_,
	//! F3
	IND_F3=SDLK_,
	//! F4
	IND_F4=SDLK_,
	//! F5
	IND_F5=SDLK_,
	//! F6
	IND_F6=SDLK_,
	//! F7
	IND_F7=SDLK_,
	//! F8
	IND_F8=SDLK_,
	//! F9
	IND_F9=SDLK_,
	//! F10
	IND_F10=SDLK_,
	//! F11
	IND_F11=SDLK_,
	//! F12
	IND_F12=SDLK_,
	//! F13
	IND_F13=SDLK_,
	//! F14
	IND_F14=SDLK_,
	//! F15
	IND_F15=SDLK_,

	//! Numlock
	IND_NUMLOCK=SDLK_,
	//! Capslock
	IND_CAPSLOCK=SDLK_,
	//! Scrollock
	IND_SCROLLOCK=SDLK_,
	//! Right shift
	IND_RSHIFT=SDLK_,
	//! Left shift
	IND_LSHIFT=SDLK_,
	//! Right control
	IND_RCTRL=SDLK_,
	//! Left control
	IND_LCTRL=SDLK_,
	//! Right Alt

	IND_RALT=SDLK_,
	//! Left Alt
	IND_LALT=SDLK_,
	//! Right Meta
	IND_RMETA=SDLK_,
	//! Left Meta
	IND_LMETA=SDLK_,
	//! F15
	IND_LSUPER=SDLK_,
	//! Right Windows button
	IND_RSUPER=SDLK_,
	//! Mode shift
	IND_MODE=SDLK_,
	//! Help
	IND_HELP=SDLK_,
	//! Print
	IND_PRINT=SDLK_,
	//! Sysreq
	IND_SYSREQ=SDLK_,
	//! Break
	IND_BREAK=SDLK_,
	//! Menu
	IND_MENU=SDLK_,
	//! Power
	IND_POWER=SDLK_,
	//! Euro
	IND_EURO
	*/
};
#endif
/*@}*/


// --------------------------------------------------------------------------------
//									   Key State
// --------------------------------------------------------------------------------

/**
 * \defgroup IND_KeyState
 * \ingroup Types
 */
/*@{*/

//! Key state

/*!
Key state.
*/
typedef int IND_KeyState;

//! Pressed key
#define IND_KEY_PRESSED 2
//! Not pressed key

#define IND_KEY_NOT_PRESSED	3
/*@}*/


// --------------------------------------------------------------------------------
//									   Mouse Button
// --------------------------------------------------------------------------------

/**
 * \defgroup IND_MouseButton
 * \ingroup Types
 */
/*@{*/

//! Mouse Buttons

/*!
Mouse input buttons.
*/
typedef int IND_MouseButton;

//! Left mouse button
#define IND_MBUTTON_LEFT 0
//! Right mouse button
#define IND_MBUTTON_RIGHT 1
//! Middle mouse button
#define IND_MBUTTON_MIDDLE 2
//! Wheelup mouse button
#define IND_MBUTTON_WHEELUP 3
//! Wheeldown mouse button
#define IND_MBUTTON_WHEELDOWN 4
/*@}*/


// --------------------------------------------------------------------------------
//									   Mouse Button State
// --------------------------------------------------------------------------------

/**
 * \defgroup IND_MouseButtonState
 * \ingroup Types
 */
/*@{*/

//! Mouse button states

/*!
States of mouse buttons.
*/
typedef int IND_MouseButtonState;

//! Button pressed
#define IND_MBUTTON_PRESSED 1
//! Button not pressed
#define IND_MBUTTON_NOT_PRESSED 0
/*@}*/

#define IND_HAT_CENTERED	0x00
#define IND_HAT_UP		0x01
#define IND_HAT_RIGHT		0x02
#define IND_HAT_DOWN		0x04
#define IND_HAT_LEFT		0x08
#define IND_HAT_RIGHTUP		(SDL_HAT_RIGHT|SDL_HAT_UP)
#define IND_HAT_RIGHTDOWN	(SDL_HAT_RIGHT|SDL_HAT_DOWN)
#define IND_HAT_LEFTUP		(SDL_HAT_LEFT|SDL_HAT_UP)
#define IND_HAT_LEFTDOWN	(SDL_HAT_LEFT|SDL_HAT_DOWN)

#define IND_JOYBUTTONDOWN 10		/**< Joystick button pressed */
#define IND_JOYBUTTONUP 11		/**< Joystick button released */
#endif

// --------------------------------------------------------------------------------
//										 GetDebugAPI()
// --------------------------------------------------------------------------------

/**
 * \defgroup IND_Debug
 * \ingroup Types
 */
/*@{*/

//! GetDebugAPI()

/*!
Using IndieLib::Init() method, the library can be initialized with the modes:
::IND_DEBUG_MODE (debug mode) , ::IND_RELEASE_MODE (release mode).

::IND_DEBUG_MODE starts the library in debug mode. In this mode at the end of the execution
a file named debug.txt is generated. It contains details about the project execution. Here you can verify
if the resources are correctly loaded/freed. Please check this file if any problem happen after the execution of
an IndieLib application.

::IND_RELEASE_MODE is the mode that should be chosen for a finished project.
*/
typedef int IND_InitializationMode;

//! Iniatializes the engine in debug mode.
#define IND_DEBUG_MODE						1
//! Iniatializes the engine in release mode.
#define IND_RELEASE_MODE					0
/*@}*/

#include <string>
// --------------------------------------------------------------------------------
//								 Bounding collision
// --------------------------------------------------------------------------------

struct INDIELIBAPI structBoundingCollision
{
	int mType;							// 0 = Triange, 1 = Circle
	std::string mId;							// Group Id for grouping bounding areas
	int mPosX, mPosY;					// Position
	int mRadius;						// Radius of the circle
	int mAx, mAy, mBx, mBy, mCx, mCy;	// Vertices of the triangle

	structBoundingCollision ()
	{
		mType = mPosX = mPosY = mRadius = mAx = mAy = mBx = mBy = mCx = mCy = 0;mId="";
	}
};
typedef struct structBoundingCollision BOUNDING_COLLISION;

// --------------------------------------------------------------------------------
//								 Global constants
// --------------------------------------------------------------------------------
const int MAX_CHARS_IN_INT32_STR = (10+1); // +1 is for '\0' character
const int MAX_CHARS_IN_INT64_STR = (20+1);

//! Center alignment.
#define IND_CENTER							300
//! Right alignment.
#define IND_RIGHT							301
//! Left alignment.
#define IND_LEFT							302

#include <GUI/GUIUtils.h>
class IGameLogic : public STXGUI
{
	public:
		IGameLogic():STXGUI(){}
		virtual ~IGameLogic(){}
		virtual void render()=0;
		virtual int init(const char* aTitle="")=0;
		virtual void deinit(){}
		virtual void actionPerformed(GUIEvent &evt){}
};

#endif // _DEFINES_
