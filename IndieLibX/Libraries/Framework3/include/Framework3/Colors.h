#ifndef _colors_H_
#define _colors_H_

// https://www.w3schools.com/colors/colors_names.asp
/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
// Color Names Supported by All Browsers:
enum colors {
eAliceBlue  	=0xF0F8FF,
eAntiqueWhite  	=0xFAEBD7,
eAqua  	=0x00FFFF,
eAquamarine  	=0x7FFFD4,
eAzure  	=0xF0FFFF,
eBeige  	=0xF5F5DC,
eBisque  	=0xFFE4C4,
eBlack  	=0x000000,
eBlanchedAlmond  	=0xFFEBCD,
eBlue  	=0x0000FF,
eBlueViolet  	=0x8A2BE2,
eBrown  	=0xA52A2A,
eBurlyWood  	=0xDEB887,
eCadetBlue  	=0x5F9EA0,
eChartreuse  	=0x7FFF00,
eChocolate  	=0xD2691E,
eCoral  	=0xFF7F50,
eCornflowerBlue  	=0x6495ED,
eCornsilk  	=0xFFF8DC,
eCrimson  	=0xDC143C,
eCyan  	=0x00FFFF,
eDarkBlue  	=0x00008B,
eDarkCyan  	=0x008B8B,
eDarkGoldenRod  	=0xB8860B,
eDarkGray  	=0xA9A9A9,
eDarkGrey  	=0xA9A9A9,
eDarkGreen  	=0x006400,
eDarkKhaki  	=0xBDB76B,
eDarkMagenta  	=0x8B008B,
eDarkOliveGreen  	=0x556B2F,
eDarkOrange  	=0xFF8C00,
eDarkOrchid  	=0x9932CC,
eDarkRed  	=0x8B0000,
eDarkSalmon  	=0xE9967A,
eDarkSeaGreen  	=0x8FBC8F,
eDarkSlateBlue  	=0x483D8B,
eDarkSlateGray  	=0x2F4F4F,
eDarkSlateGrey  	=0x2F4F4F,
eDarkTurquoise  	=0x00CED1,
eDarkViolet  	=0x9400D3,
eDeepPink  	=0xFF1493,
eDeepSkyBlue  	=0x00BFFF,
eDimGray  	=0x696969,
eDimGrey  	=0x696969,
eDodgerBlue  	=0x1E90FF,
eFireBrick  	=0xB22222,
eFloralWhite  	=0xFFFAF0,
eForestGreen  	=0x228B22,
eFuchsia  	=0xFF00FF,
eGainsboro  	=0xDCDCDC,
eGhostWhite  	=0xF8F8FF,
//eGold  	=0xFFD700, // original
eGold  		=0x856D4A,
eGoldenRod  	=0xDAA520,
eGray  	=0x808080,
eGrey  	=0x808080,
eGreen  	=0x008000,
eGreenYellow  	=0xADFF2F,
eHoneyDew  	=0xF0FFF0,
eHotPink  	=0xFF69B4,
eIndianRed   	=0xCD5C5C,
eIndigo   	=0x4B0082,
eIvory  	=0xFFFFF0,
eKhaki  	=0xF0E68C,
eLavender  	=0xE6E6FA,
eLavenderBlush  	=0xFFF0F5,
eLawnGreen  	=0x7CFC00,
eLemonChiffon  	=0xFFFACD,
eLightBlue  	=0xADD8E6,
eLightCoral  	=0xF08080,
eLightCyan  	=0xE0FFFF,
eLightGoldenRodYellow  	=0xFAFAD2,
eLightGray  	=0xD3D3D3,
eLightGrey  	=0xD3D3D3,
eLightGreen  	=0x90EE90,
eLightPink  	=0xFFB6C1,
eLightSalmon  	=0xFFA07A,
eLightSeaGreen  	=0x20B2AA,
eLightSkyBlue  	=0x87CEFA,
eLightSlateGray  	=0x778899,
eLightSlateGrey  	=0x778899,
eLightSteelBlue  	=0xB0C4DE,
eLightYellow  	=0xFFFFE0,
eLime  	=0x00FF00,
eLimeGreen  	=0x32CD32,
eLinen  	=0xFAF0E6,
eMagenta  	=0xFF00FF,
eMaroon  	=0x800000,
eMediumAquaMarine  	=0x66CDAA,
eMediumBlue  	=0x0000CD,
eMediumOrchid  	=0xBA55D3,
eMediumPurple  	=0x9370DB,
eMediumSeaGreen  	=0x3CB371,
eMediumSlateBlue  	=0x7B68EE,
eMediumSpringGreen  	=0x00FA9A,
eMediumTurquoise  	=0x48D1CC,
eMediumVioletRed  	=0xC71585,
eMidnightBlue  	=0x191970,
eMintCream  	=0xF5FFFA,
eMistyRose  	=0xFFE4E1,
eMoccasin  	=0xFFE4B5,
eNavajoWhite  	=0xFFDEAD,
eNavy  	=0x000080,
eOldLace  	=0xFDF5E6,
eOlive  	=0x808000,
eOliveDrab  	=0x6B8E23,
eOrange  	=0xFFA500,
eOrangeRed  	=0xFF4500,
eOrchid  	=0xDA70D6,
ePaleGoldenRod  	=0xEEE8AA,
ePaleGreen  	=0x98FB98,
ePaleTurquoise  	=0xAFEEEE,
ePaleVioletRed  	=0xDB7093,
ePapayaWhip  	=0xFFEFD5,
ePeachPuff  	=0xFFDAB9,
ePeru  	=0xCD853F,
ePink  	=0xFFC0CB,
ePlum  	=0xDDA0DD,
ePowderBlue  	=0xB0E0E6,
ePurple  	=0x800080,
eRebeccaPurple  	=0x663399,
eRed  	=0xFF0000,
eRosyBrown  	=0xBC8F8F,
eRoyalBlue  	=0x4169E1,
eSaddleBrown  	=0x8B4513,
eSalmon  	=0xFA8072,
eSandyBrown  	=0xF4A460,
eSeaGreen  	=0x2E8B57,
eSeaShell  	=0xFFF5EE,
eSienna  	=0xA0522D,
eSilver  	=0xC0C0C0,
eSkyBlue  	=0x87CEEB,
eSlateBlue  	=0x6A5ACD,
eSlateGray  	=0x708090,
eSlateGrey  	=0x708090,
eSnow  	=0xFFFAFA,
eSpringGreen  	=0x00FF7F,
eSteelBlue  	=0x4682B4,
eTan  	=0xD2B48C,
eTeal  	=0x008080,
eThistle  	=0xD8BFD8,
eTomato  	=0xFF6347,
eTurquoise  	=0x40E0D0,
eViolet  	=0xEE82EE,
eWheat  	=0xF5DEB3,
eWhite  	=0xFFFFFF,
eWhiteSmoke  	=0xF5F5F5,
eYellow  	=0xFFFF00,
eYellowGreen  	=0x9ACD32
}; // Colors

inline CONST D3DXFROMWINEVECTOR4 stx_GetRGBA(CONST __DWORD__ col)
{
    CONST FLOAT f = 1.0f / 255.0f;
    CONST FLOAT r = f * (FLOAT)(unsigned char)(col >> 16);
    CONST FLOAT g = f * (FLOAT)(unsigned char)(col >>  8);
    CONST FLOAT b = f * (FLOAT)(unsigned char)col;
    CONST FLOAT a = f * (FLOAT)(unsigned char)(col >> 24);
	return  D3DXFROMWINEVECTOR4(r,g,b,a);
}
inline CONST D3DXFROMWINEVECTOR3 stx_GetRGB(CONST __DWORD__ col)
{
    CONST FLOAT f = 1.0f / 255.0f;
    CONST FLOAT r = f * (FLOAT)(unsigned char)(col >> 16);
    CONST FLOAT g = f * (FLOAT)(unsigned char)(col >>  8);
    CONST FLOAT b = f * (FLOAT)(unsigned char)col;
	return  D3DXFROMWINEVECTOR3(r,g,b);
}
#if 0

// The OpenGL Programming Guide - The Redbook
// File: IndieLibX/Docs/The OpenGL Programming Guide - The Redbook/teapots_8c.cpp

void renderTeapot(GLfloat x, GLfloat y,
   GLfloat ambr, GLfloat ambg, GLfloat ambb,
   GLfloat difr, GLfloat difg, GLfloat difb,
   GLfloat specr, GLfloat specg, GLfloat specb, GLfloat shine)
/**
 *  First column:  emerald, jade, obsidian, pearl, ruby, turquoise
 *  2nd column:  brass, bronze, chrome, copper, gold, silver
 *  3rd column:  black, cyan, green, red, white, yellow plastic
 *  4th column:  black, cyan, green, red, white, yellow rubber
 */
0.0215, 0.1745, 0.0215, 0.07568, 0.61424, 0.07568, 0.633, 0.727811, 0.633, 0.6
0.135, 0.2225, 0.1575,0.54, 0.89, 0.63, 0.316228, 0.316228, 0.316228, 0.1
0.05375, 0.05, 0.06625, 0.18275, 0.17, 0.22525, 0.332741, 0.328634, 0.346435, 0.3
0.25, 0.20725, 0.20725, 1, 0.829, 0.829, 0.296648, 0.296648, 0.296648, 0.088
0.1745, 0.01175, 0.01175, 0.61424, 0.04136, 0.04136, 0.727811, 0.626959, 0.626959, 0.6
0.1, 0.18725, 0.1745, 0.396, 0.74151, 0.69102, 0.297254, 0.30829, 0.306678, 0.1
0.329412, 0.223529, 0.027451, 0.780392, 0.568627, 0.113725, 0.992157, 0.941176, 0.807843, 0.21794872
0.2125, 0.1275, 0.054, 0.714, 0.4284, 0.18144, 0.393548, 0.271906, 0.166721, 0.2
0.25, 0.25, 0.25, 0.4, 0.4, 0.4, 0.774597, 0.774597, 0.774597, 0.6
0.19125, 0.0735, 0.0225, 0.7038, 0.27048, 0.0828, 0.256777, 0.137622, 0.086014, 0.1
0.24725, 0.1995, 0.0745, 0.75164, 0.60648, 0.22648, 0.628281, 0.555802, 0.366065, 0.4
0.19225, 0.19225, 0.19225, 0.50754, 0.50754, 0.50754, 0.508273, 0.508273, 0.508273, 0.4
0.0, 0.0, 0.0, 0.01, 0.01, 0.01, 0.50, 0.50, 0.50, .25
0.0, 0.1, 0.06, 0.0, 0.50980392, 0.50980392, 0.50196078, 0.50196078, 0.50196078, .25
0.0, 0.0, 0.0, 0.1, 0.35, 0.1, 0.45, 0.55, 0.45, .25
0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.7, 0.6, 0.6, .25
0.0, 0.0, 0.0, 0.55, 0.55, 0.55, 0.70, 0.70, 0.70, .25
0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.60, 0.60, 0.50, .25
0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.4, 0.4, 0.4, .078125
0.0, 0.05, 0.05, 0.4, 0.5, 0.5, 0.04, 0.7, 0.7, .078125
0.0, 0.05, 0.0, 0.4, 0.5, 0.4, 0.04, 0.7, 0.04, .078125
0.05, 0.0, 0.0, 0.5, 0.4, 0.4, 0.7, 0.04, 0.04, .078125
0.05, 0.05, 0.05, 0.5, 0.5, 0.5, 0.7, 0.7, 0.7, .078125
0.05, 0.05, 0.0, 0.5, 0.5, 0.4, 0.7, 0.7, 0.04, .078125
#endif
#endif

