/*****************************************************************************************
 * File: IND_Image.cpp
 * Desc: Image object
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

// ----- Includes -----
#define __ImageLib__ 1
#include <CIndieLib.h>
#include <IND_Image.h>

// --------------------------------------------------------------------------------
//										Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pR                             Byte R (Red)
\arg \b pG                             Byte G (Green)
\arg \b pB                             Byte B (Blue)
\arg \b pA                             Byte A (Transparency)

\b Operation:

Clear the image to the chosen color. Returns 0 if there is no image loaded.
*/
bool IND_Image::Clear (unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA)
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	// Image pointer
	unsigned char *mPtrImage = GetPointer();

	switch (GetFormatInt())
	{
		case IND_RGB:
		{
			for (int i = 0; i < GetWidth() * GetHeight(); i++)
			{
				// Pixel
				mPtrImage [0] = pR;
				mPtrImage [1] = pG;
				mPtrImage [2] = pB;

				mPtrImage += 3;
			}

			break;
		}

		case IND_BGR:
		{
			for (int i = 0; i < GetWidth() * GetHeight(); i++)
			{
				// Pixel
				mPtrImage [2] = pR;
				mPtrImage [1] = pG;
				mPtrImage [0] = pB;

				mPtrImage += 3;
			}

			break;
		}

		case IND_RGBA:
		{
			for (int i = 0; i < GetWidth() * GetHeight(); i++)
			{
				// Pixel
				mPtrImage [0] = pR;
				mPtrImage [1] = pG;
				mPtrImage [2] = pB;
				mPtrImage [3] = pA;

				mPtrImage += 4;
			}

			break;
		}

		case IND_BGRA:
		{
			for (int i = 0; i < GetWidth() * GetHeight(); i++)
			{
				// Pixel
				mPtrImage [3] = pA;
				mPtrImage [2] = pR;
				mPtrImage [1] = pG;
				mPtrImage [0] = pB;

				mPtrImage += 4;
			}

			break;
		}

		case IND_LUMINANCE:
		{
			for (int i = 0; i < GetWidth() * GetHeight(); i++)
			{
				// Pixel
				mPtrImage [0] = pA;

				mPtrImage += 1;
			}

			break;
		}
	}

	return 1;
}


/*!
\b Parameters:

\arg \b pX                  X coordanate.
\arg \b pY                  Y coordanate.
\arg \b pR                  Byte R (Red)
\arg \b pG                  Byte G (Green)
\arg \b pB                  Byte B (Blue)
\arg \b pA                  Byte A (Transparency)

\b Operation:

Returns by reference the colour of the specified pixel (in RGBA). Returns 0 if there is no image loaded.

If the image format (see ::IND_Format) doesn't have alpha value (pA) this parameter can be
omitted.

In the special case of ::IND_LUMINANCE type images, the pixel colour will be returned in pA.
*/
bool IND_Image::GetPixel	(int pX, int pY, unsigned char *pR, unsigned char *pG, unsigned char *pB, unsigned char *pA)
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	// Out of range
	if (pX >= GetWidth  ())		return 0;
	if (pY >= GetHeight ())		return 0;
	if (pX < 0)					return 0;
	if (pY < 0)					return 0;

	// Image pointer
	unsigned char *mPtrImage = GetPointer ();

	// Pixel offset
	int mOffset;

	switch (GetFormatInt())
	{
		case IND_RGB:
		{
			// Calculate the pixel offset
			mOffset = ( (pX + ( ( GetHeight () - pY - 1) * GetWidth ())) * 3);

			// Pixel
			*pR = mPtrImage [mOffset    ];
			*pG = mPtrImage [mOffset + 1];
			*pB = mPtrImage [mOffset + 2];
			*pA = 0;

			break;
		}

		case IND_BGR:
		{
			// Calculate the pixel offset
			mOffset = ( (pX + ( ( GetHeight () - pY - 1) * GetWidth ())) * 3);

			// Pixel
			*pA = 0;
			*pR = mPtrImage [mOffset + 2];
			*pG = mPtrImage [mOffset + 1];
			*pB = mPtrImage [mOffset    ];

			break;
		}

		case IND_RGBA:
		{
			// Calculate the pixel offset
			mOffset = ( (pX + ( ( GetHeight () - pY - 1) * GetWidth ())) * 4);

			// Pixel
			*pR = mPtrImage [mOffset    ];
			*pG = mPtrImage [mOffset + 1];
			*pB = mPtrImage [mOffset + 2];
			*pA = mPtrImage [mOffset + 3];

			break;
		}

		case IND_BGRA:
		{
			// Calculate the pixel offset
			mOffset = ( (pX + ( ( GetHeight () - pY - 1) * GetWidth ())) * 4);

			// Pixel
			*pA = mPtrImage [mOffset + 3];
			*pR = mPtrImage [mOffset + 2];
			*pG = mPtrImage [mOffset + 1];
			*pB = mPtrImage [mOffset    ];

			break;
		}

		case IND_LUMINANCE:
		{
			// Calculate the pixel offset
			mOffset = ( (pX + ( ( GetHeight () - pY - 1) * GetWidth ())));

			// Pixel
			*pA = mPtrImage [mOffset];
			*pR = 0;
			*pG = 0;
			*pB = 0;

			break;
		}

		// Unknown
		default:
		{
			*pA = 0;
			*pR = 0;
			*pG = 0;
			*pB = 0;
			return 0;
		}
	}

	return 1;
}


/*!
\b Parameters:

\arg \b pX                  X coordenate.
\arg \b pY                  Y coordanate.
\arg \b pR                  Byte R (Red)
\arg \b pG                  Byte G (Green)
\arg \b pB                  Byte B (Blue)
\arg \b pA                  Byte A (Transparency)

\b Operation:

Puts a pixel in the position and color specified as parameters (in RGBA). Returns 0 if there is no image loaded.

If the image format (see ::IND_Format) doesn't have alpha value (pA) this parameter can be
omitted.

In the special case of ::IND_LUMINANCE type images, the pixel color must be specified in pA.
*/
bool IND_Image::PutPixel	(int pX, int pY, unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA)
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	// Out of range
	if (pX >= GetWidth  ())		return 0;
	if (pY >= GetHeight ())		return 0;
	if (pX < 0)					return 0;
	if (pY < 0)					return 0;

	// Image pointer
	unsigned char *mPtrImage = GetPointer ();

	// Pixel offset
	int mOffset;

	switch (GetFormatInt())
	{
		case IND_RGB:
		{
			// Calculate the pixel offset
			mOffset = ( (pX + ( ( GetHeight () - pY - 1) * GetWidth ())) * 3);

			// Pixel
			mPtrImage [mOffset    ] = pR;
			mPtrImage [mOffset + 1] = pG;
			mPtrImage [mOffset + 2] = pB;

			break;
		}

		case IND_BGR:
		{
			// Calculate the pixel offset
			mOffset = ( (pX + ( ( GetHeight () - pY - 1) * GetWidth ())) * 3);

			// Pixel
			mPtrImage [mOffset + 2] = pR;
			mPtrImage [mOffset + 1] = pG;
			mPtrImage [mOffset    ] = pB;

			break;
		}

		case IND_RGBA:
		{
			// Displacement
			mOffset = ( (pX + ( ( GetHeight () - pY - 1) * GetWidth ())) * 4);

			// Pixel
			mPtrImage [mOffset    ] = pR;
			mPtrImage [mOffset + 1] = pG;
			mPtrImage [mOffset + 2] = pB;
			mPtrImage [mOffset + 3] = pA;

			break;
		}

		case IND_BGRA:
		{
			// Calculate the pixel offset
			mOffset = ( (pX + ( ( GetHeight () - pY - 1) * GetWidth ())) * 4);

			// Pixel
			mPtrImage [mOffset + 3] = pA;
			mPtrImage [mOffset + 2] = pR;
			mPtrImage [mOffset + 1] = pG;
			mPtrImage [mOffset    ] = pB;

			break;
		}

		case IND_LUMINANCE:
		{
			// Displacement
			mOffset = ( (pX + ( ( GetHeight () - pY - 1) * GetWidth ())));

			// Pixel
			mPtrImage [mOffset] = pA;

			break;
		}

		// Unknown
		default:
		{
			return 0;
		}
	}

	return 1;
}


/*!
\b Parameters:

\b Operation:

Horizontal inversion. Returns 0 if there is no image loaded.

\image html original.png Original
\image html fliph.png  Horizontal mirror
*/
bool IND_Image::FlipH ()
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluMirror ();
	mImage.mImg.flipX();
	//SetPointer (GetPointer ());

	return 1;
}


/*!

\b Parameters:

\b Operation:

Vertical inversion. Returns 0 if there is no image loaded.

\image html original.png Original
\image html fliph.png  Vertical mirror
*/
bool IND_Image::FlipV ()
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluFlipImage ();
	mImage.mImg.flipY();
	//SetPointer (ilGetData ());

	return 1;
}

void IND_Image::ConvertImage(IND_Format pFormat)
{
	FORMAT fmt=FORMAT_NONE;
	switch(pFormat)
	{
		case IND_RGB:
			fmt=FORMAT_RGB8;
			break;
		case IND_RGBA:
			fmt=FORMAT_RGBA8;
			break;	
		case IND_BGR:
			fmt=FORMAT_RGB8;
			mImage.mImg.swap(0, 2);
			break;	
		case IND_BGRA:
			fmt=FORMAT_RGBA8;
			mImage.mImg.swap(0, 2);
			break;
		case IND_LUMINANCE:
			fmt=FORMAT_I8;
			break;
		default :
			fmt=FORMAT_RGBA8;
			break;	
	};
	mImage.mImg.convert(fmt);
}
/*!
\b Parameters:

\arg \b pFormat        Format of the image. See ::IND_Format.

\b Operation:

Converts the image to the specified format. Returns 0 if there is no image loaded.
*/
bool IND_Image::Convert (IND_Format pFormat)
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());

	ConvertImage (pFormat);
	SetFormatInt (pFormat);
	//SetBpp (ilGetInteger (IL_IMAGE_BITS_PER_PIXEL));
	//SetPointer (ilGetData ());

	return 1;
}


/*!
\b Parameters:

\arg \b pR                  Byte R (Red)
\arg \b pG                  Byte G (Green)
\arg \b pB                  Byte B (Blue)

\b Operation:

Sets the alpha value of all pixels of the specified color to transparent. This is know as "color key". Returns 0 if
there is no image loaded.
*/
bool IND_Image::SetAlpha (unsigned char pR, unsigned char pG, unsigned char pB)
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());

	// We add alpha channel if the image hasen't.
	if (GetFormatInt () != IND_RGBA)
	{
		ConvertImage (IND_RGBA);
		SetFormatInt (IND_RGBA);
		//SetBpp (ilGetInteger (IL_IMAGE_BITS_PER_PIXEL));
		//SetPointer (ilGetData ());
	}

	SetAlphaChannel (pR, pG, pB);

	return 1;
}


/*!
\b Parameters:

\arg \b pIm				  Pointer to an image object
\arg \b pX                X Coordenate
\arg \b pY                Y Coordenate

\b Operation:

Pastes the image passed as parameter over the image that calls the method in the specified position. Returns 0 if there
is no image loaded.
*/
bool IND_Image::PasteImage (IND_Image *pIm, int pX, int pY)
{
	// No image loaded
	if (!IsImageLoaded() || !pIm->IsImageLoaded() ) return 0;

	//ilBindImage (pIm->GetImageLibId ());
	//ilOverlayImage (GetImageLibId (), pX, pY, 0);
	pIm->SetPointer (GetPointer ());

	return 1;

}


/*!
\b Parameters:

\arg \b pIter               Iterations

\b Operation:

Applies a gaussian filter which intensity depends on the pIter parameter. Returns 0 if there
is no image loaded.

\image html original.png Original
\image html gauss.png Gaussian blur
*/
bool IND_Image::GaussianBlur (int pIter)
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluBlurGaussian (pIter);
	//SetPointer (ilGetData ());

	return 1;
}


/*!
\b Parameters:

\arg \b pCont            Contrast

\b Operation:

Applies a contrast filter which intensity depends of the pCont parameter. Returns 0 if there is no image loaded.

\image html original.png Original
\image html cont.png Contrast
*/
bool IND_Image::Contrast (float pCont)
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluContrast (pCont);

	return 1;
}


/*!
\b Parameters:

\b Operation:

Applies a equalizator filter. Returns 0 if there is no image loaded.

\image html original.png Original
\image html equa.png Equalizator
*/
bool IND_Image::Equalize ()
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluEqualize ();

	return 1;
}


/*!
\b Parameters:

\arg \b pGamma       Gamma quality

\b Operation:

Applies a gamma filter which intensity depends on the pGamma parameter. Values lower than 1
makes the image darker, and values greater than 1 makes it clearer. Returns 0 if there is no image loaded.

\image html original.png Original
\image html gamma.png Gamma
*/
bool IND_Image::SetGamma (float pGamma)
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluGammaCorrect (pGamma);

	return 1;
}


/*!
\b Parameters:

\b Operation:

Applies a negative filter. Returns 0 if there is no image loaded.

\image html original.png Original
\image html negat.png Negative
*/
bool IND_Image::Negative ()
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluNegative ();

	return 1;
}


/*!
\b Parameters:

\arg \b pNoise        Noise quantity

\b Operation:

Applies a noise filter which intensity depends on the pNoise parameter. The greater this value is,
the more noise is applied to the image. Returns 0 if there is no image loaded.

\image html original.png Original
\image html noise.png Noise
*/
bool IND_Image::Noisify (float pNoise)
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluNoisify (pNoise);

	return 1;
}


/*!
\b Parameters:

\arg \b pPixSize      Size of each pixel

\b Operation:

Applies a pixelation filter. The size of each pixel is specified by the parameter
pPixSize. Returns 0 if there is no image loaded.

\image html original.png Original
\image html pixel.png Pixeled
*/
bool IND_Image::Pixelize (int pPixSize)
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluPixelize (pPixSize);

	return 1;
}


/*!
\b Parameters:

\arg \b pFactor       "Definition" factor
\arg \b pIter         Numbers of iterations

\b Operation:

Applies a definition filter that depends of the parameters pFactor (the greater this
parameter, the more "defined" the image will be) and the pIter parameter that indicates the number of times that
the filter is applied consecutivity. This filter can improve the quality in images that
have been deteriorated. Returns 0 if there is no image loaded.

\image html original.png Original
\image html shar.png Sharpen
*/
bool IND_Image::Sharpen (float pFactor, int pIter)
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluSharpen (pFactor, pIter);

	return 1;
}


/*!
\b Parameters:

\arg \b pSizeX			Width in píxels
\arg \b pSizeY			Height in píxels

\b Operation:

Applies a scale to the height and width specified as paramteres. Returns 0 if there is no image loaded.

\image html original.png Original
\image html scale.png Scaled
*/
bool IND_Image::Scale (int pSizeX, int pSizeY)
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
/*
	// Scaling filter (when minimizing => Bilinear | when maximizing => ScaleBell)
	if (pSizeX < GetWidth () || pSizeY < GetHeight ())
		iluImageParameter (ILU_FILTER, ILU_BILINEAR);
	else
		iluImageParameter (ILU_FILTER, ILU_SCALE_BELL);
*/

	//iluScale (pSizeX, pSizeY, GetBpp ());
	//SetPointer (ilGetData ());
	////SetWidth (pSizeX);
	//SetHeight (pSizeY);

	return 1;
}


/*!
\b Parameters:

\b Operation:

Applies a filter of border detection (type 1). Returns 0 if there is no image loaded.

\image html original.png Original
\image html edge1.png Border detection type 1
*/
bool IND_Image::EdgeDetect1 ()
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluEdgeDetectP ();

	return 1;
}


/*!
\b Parameters:

\b Operation:

Applies a filter of border detection (type 2). Returns 0 if there is no image loaded.

\image html original.png Original
\image html edge2.png Border detection type 2
*/
bool IND_Image::EdgeDetect2 ()
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluEdgeDetectS ();

	return 1;
}


/*!
\b Parameters:

\b Operation:

Applies a emboss filter. Returns 0 if there is no image loaded.

\image html original.png Original
\image html emboss.png Emboss
*/
bool IND_Image::Emboss ()
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluEmboss ();
	//SetPointer (ilGetData ());

	return 1;
}


/*!
\b Parameters:

\arg \b pAngle          Rotation angle

\b Operation:

Applies a rotation in an angle which is specified as the pAngle parameter. Returns 0 if there is no image loaded.

\image html original.png Original
\image html rotate.png Rotation
*/
bool IND_Image::Rotate (float pAngle)
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluRotate (pAngle);
	//SetWidth (ilGetInteger (IL_IMAGE_WIDTH));
	//SetHeight (ilGetInteger (IL_IMAGE_HEIGHT));
	//SetPointer (ilGetData ());

	return 1;
}


/*!
\b Parameters:

\b Operation:

Inverts the alpha values (transparency) of the image. Returns 0 if there is no image loaded.
*/
bool IND_Image::InvertAlpha ()
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluInvertAlpha ();

	return 1;
}


/*!
\b Parameters:

\arg \b pSaturation		Saturation quantity of the image.

\b Operation:

Applies a saturation filter which depends of the paramenter pSaturation. The greater
this value is, the more saturated the image will be. Returns 0 if there is no image loaded.

\image html original.png Original
\image html satur.png Saturation
*/
bool IND_Image::Saturation (float pSaturation)
{
	// No image loaded
	if (!IsImageLoaded()) return 0;

	//ilBindImage (GetImageLibId ());
	//iluSaturate1f  (pSaturation);

	return 1;
}


// --------------------------------------------------------------------------------
//									Private methods
// --------------------------------------------------------------------------------

/*
*************************
ColorKey
*************************
*/
void IND_Image::SetAlphaChannel (unsigned char pR, unsigned char pG, unsigned char pB)
{
	unsigned char *mImage = GetPointer ();
	int mSize  = GetWidth () * GetHeight() * 4; // 4 bytes RGBA

	for (int i = 0; i < mSize; i += 4)
	{
		if (mImage [ i ] == pR &&
			mImage [i+1] == pG &&
			mImage [i+2] == pB)
		{
			mImage [i+3] = 0x00;
		}
	}
}


/*
*************************
Id (integer) format to string
*************************
*/
const char *IND_Image::FormatToChar (IND_Format pFormat)
{
	switch (pFormat)
	{
		case IND_COLOUR_INDEX:		return "IND_COLOUR_INDEX";
		case IND_RGB:				return "IND_RGB";
		case IND_RGBA:				return "IND_RGBA";
		case IND_BGR:				return "IND_BGR";
		case IND_BGRA:				return "IND_BGRA";
		case IND_LUMINANCE:			return "IND_LUMINANCE";

		default:					return "FORMAT_NOT_IDENTIFIED";
	}
}

