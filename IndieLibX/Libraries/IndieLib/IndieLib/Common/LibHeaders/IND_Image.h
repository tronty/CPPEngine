/*****************************************************************************************
/* File: IND_Image.h
/* Desc: Image object
/*****************************************************************************************/

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

#ifndef _IND_IMAGE_
#define _IND_IMAGE_


// --------------------------------------------------------------------------------
//									 IND_Image
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Image IND_Image
\ingroup Objects
IND_Image class managed by IND_ImageManager for loading, saving and applying filters to images, click in IND_Image to see all the methods of this class.
*/
/*@{*/

/*!
\b IND_Image is an image object from the class ::IND_ImageManager. Read the explanation in ::IND_ImageManager for more details.
*/
class DLL_EXP IND_Image
{
public:

	// ----- Public methods ------

	bool Clear			(unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA);
	bool GetPixel		(int pX, int pY, unsigned char *pR, unsigned char *pG, unsigned char *pB, unsigned char *pA);
	bool PutPixel		(int pX, int pY, unsigned char pR, unsigned char pG, unsigned char pB, unsigned char pA);
	bool FlipH			();
	bool FlipV			();
	bool Convert		(IND_Format pFormat);
	bool SetAlpha		(unsigned char pR, unsigned char pG, unsigned char pB);
	bool PasteImage		(IND_Image *pIm, int pX, int pY);
	bool GaussianBlur	(int pIter);
	bool Contrast		(float pCont);
	bool Equalize		();
	bool SetGamma		(float pGamma);
	bool Negative		();
	bool Noisify		(float pNoise);
	bool Pixelize		(int pPixSize);
	bool Sharpen		(float pFactor, int pIter);
	bool Scale			(int pSizeX, int pSizeY); 
	bool EdgeDetect1	();
	bool EdgeDetect2	();
	bool Emboss			();
	bool Rotate			(float pAngle);
	bool InvertAlpha	();
	bool Saturation		(float pSaturation);

	// ----- Public gets ------

	//! This function returns the width in pixels of the image.
	#if 0	
	int IsImageLoaded	()	{ if (mImage.mDevilId == -1) return 0; else return 1;	}
	#else
	int IsImageLoaded	()	{ return mImage.mImg.IsImageLoaded();	}
	#endif
	//! This function returns the width in pixels of the image.
	int GetWidth		()	{ return mImage.mImg.getWidth();						}
	//! This function returns the height in pixels of the image
	int GetHeight		()	{ return mImage.mImg.getHeight();					}	
	//! This function returns the bytes per pixel of the image.
	int GetBpp			()	{ return mImage.mImg.getBpp();						}
	//! This function returns the image format in an integer. See ::IND_Format.
	int	GetFormatInt	()	{ return mImage.mImg.getFormat();					}
	// !This function returns the format from the image in a text string. See ::IND_Format.
	const char *GetFormatChar ()	{ return FormatToChar (GetFormatInt	());				}
	//! This function returns the extension that had the image when it was stored in a file. For example if the image was "loover.jpg" the function will return "jpg".
	const char *GetExtension	()	{ return mImage.mExt.c_str();						}
	//! This function returns the pointer to the memory bytes which forms the image. It is a function very useful for accessing to the image directy and for modifying it or reading it.     
	unsigned char *GetPointer	()	{ return mImage.mImg.getPixels();}    
	void SetPointer	(unsigned char * data)	{ mImage.mImg.setPixels(data);}
	//! This function returns the name of the image in a string of chars.
	const char *GetName		()	{ return mImage.mName.c_str();						}
	
private:

	// ----- Structures ------

	//TYPE
	struct structImage
	{	
		Image3		mImg;
		IND_Format	mFormat;			// Image format
		std::string	mExt;
		std::string mName;

		structImage ()
		{
			mFormat = 0;
			mExt = "";
			mName = "";
		}
	};
	typedef struct structImage IMAGE;

	IMAGE mImage;

	// ----- Private sets ------

	void SetFormatInt	(int pFormat)	{ mImage.mFormat = pFormat;		}
	void SetExtension	(const char *pExt)	{ mImage.mExt = pExt;			}
	void SetName		(const char *pName)	{ mImage.mName = pName;			}
	#if 0
	void SetDevilId		(UINT pDevId) 	{ mImage.mDevilId = pDevId;					}
	UINT GetDevilId	()					{ return mImage.mDevilId;					}
	#elif 0
	void SetImageLibId		(unsigned int pDevId) 	
	{ 
		ResetImageLibId	();
		mImage.mImageLibId = pDevId;					
	}
	UINT GetImageLibId	()					{ return mImage.mImageLibId;}
	void ResetImageLibId	()					
	{ 
		if(0)//mImage.mImageLibId)// ??? Heap corruption revelation
		{ 
			//???ilDeleteImages(1, &mImage.mImageLibId);
			mImage.mImageLibId=0;
		}
	}
#endif
	// ----- Private methods -----

	void SetAlphaChannel				(unsigned char pR, unsigned char pG, unsigned char pB);
	const char *FormatToChar			(IND_Format pFormat);
	void ConvertImage(IND_Format pFormat);
	
	// ----- Friends -----
	
	friend class IND_ImageManager;
};
/*@}*/

#endif // _IND_IMAGE_
