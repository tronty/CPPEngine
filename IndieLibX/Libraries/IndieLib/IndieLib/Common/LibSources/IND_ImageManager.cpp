/*****************************************************************************************
 * File: IND_ImageManager.cpp
 * Desc: Image Manager
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
#include <IND_ImageManager.h>

    const char *mSupportedExt[] =
	{
		"bmp","png","tga","jpg","jpeg","pcx",""
	};
// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*!
This function returns 1 (true) if the adminstrator is successfully initialized.
Must be called before using any method.
*/
bool IND_ImageManager::Init ()
{
	End ();
	InitVars ();

	GetDebugAPI()->Header ("Initializing ImageManager", 5);
	GetDebugAPI()->Header ("Preparing ImageManager", 1);
	mOk = true;

	GetDebugAPI()->Header ("ImageManager OK", 6);

	return mOk;
}


/*!
\b Operation:

This function frees the manager and all the objects that it contains.
*/
void IND_ImageManager::End()
{
	if (mOk)
	{
		GetDebugAPI()->Header ("Finalizing ImageManager", 5);
		GetDebugAPI()->Header ("Freeing images" , 5);
		FreeVars ();
		GetDebugAPI()->Header ("Images freed", 6);
		GetDebugAPI()->Header ("ImageManager finalized", 6);

		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//									Public methods
// --------------------------------------------------------------------------------

/*!
\b Parameters:

\arg \b pNewImage             Pointer no a new image object.
\arg \b pName                 Image name

\b Operation:

This function returns 1 (true) if the image object passed as a parameter
exists and is added successfully to the manager.

It supports the following graphics formats (thanks to http://openil.sourceforge.net):
bmp, png, tga, jpg and pcx.
*/
bool IND_ImageManager::Add (IND_Image *pNewImage, const char *apName)
{

	//LOG_PRINT("apName = %s\n", apName);
	std::string pName=(apName);
	//stx_fileExists(pName.c_str());

	//LOG_PRINT("pName=%s\n",pName.c_str());


	GetDebugAPI()->Header ("Loading Image", 5);

	GetDebugAPI()->Header ("File name:", 3);


	GetDebugAPI()->DataChar (pName.c_str(), 1);
		
	if (!mOk)
	{
		WriteMessage ();
		return 0;
	}

	// ----- Obtaining and checking file extension -----

#if 0

    const char *mExt = GetExtensionFromName (pName);

//LOG_PRINT("mExt=%s\n",mExt);

	if (CheckExtImage (mExt))
		pNewImage->SetExtension (mExt);
	else
	{
		

		GetDebugAPI()->Header ("Unknown extension", 2);


		return 0;
	}



	GetDebugAPI()->Header ("Extension:", 3);


	GetDebugAPI()->DataChar (mExt, 1);
#endif

	std::string fn=stx_convertpath(pName);

	//LOG_PRINT("IND_ImageManager::Add:loadImageLibImage=%s\n",fn.c_str());

	Image3 img;
	img.loadImageLibImage(fn.c_str());
	//img.createMipMaps();
	if(img.getFormat()!=FORMAT_RGBA8)
		img.convert(FORMAT_RGBA8);

	pNewImage->mImage.mImg=img;

	// Attributes
	//pNewImage->SetWidth		(ilGetInteger (IL_IMAGE_WIDTH));


	//pNewImage->SetHeight	(ilGetInteger (IL_IMAGE_HEIGHT));


	pNewImage->SetFormatInt	(img.getFormat());


	//pNewImage->SetBpp		(ilGetInteger (IL_IMAGE_BITS_PER_PIXEL));


	//pNewImage->SetPointer	(ilGetData ());


//LOG_PRINT("this=%x\n",this);
//LOG_PRINT("pNewImage=%x\n",pNewImage);
//LOG_PRINT("pName.c_str()=%s\n",pName.c_str());
	pNewImage->SetName	(pName.c_str());


	pNewImage->FlipV();



	// ImageLib image ID
	//pNewImage->SetImageLibId (mImageName);




	// ----- Puts the object into the manager -----
#if 1
	AddToList (pNewImage);

#else
	mListImages->push_back (pNewImage);
#endif



	// ----- GetDebugAPI() -----

	GetDebugAPI()->Header ("Size:", 3);


	//GetDebugAPI()->DataInt (pNewImage->GetWidth (), 0);


	GetDebugAPI()->DataChar ("x", 0);


	//GetDebugAPI()->DataInt (pNewImage->GetHeight (), 1);



	GetDebugAPI()->Header ("Bpp:", 3);


	//GetDebugAPI()->DataInt (pNewImage->GetBpp (), 1);


	GetDebugAPI()->Header ("Fomart:", 3);


	GetDebugAPI()->DataChar (pNewImage->GetFormatChar(), 1);



	GetDebugAPI()->Header("Image loaded", 6);



	//LOG_PRINT("IL_IMAGE_BITS_PER_PIXEL=%d\n",ilGetInteger (IL_IMAGE_BITS_PER_PIXEL));
	//stx_exit(0);
	
	return 1;
}


/*!
\b Parameters:

\arg \b pNewImage             Pointer to a new image object
\arg \b pWidth                Width of the new image
\arg \b pHeight               Height of the new image
\arg \b pFormat               New image format. See ::IND_Format.

\b Operation:

This function returns 1 (true) if the image object passed as a parameter
exists and is added successfully to the manager creating a new procedural image.

By default, the new image is not initialized to any color, you should used the Clear()
method in order to assing a valid colour.

*/
bool IND_ImageManager::Add (IND_Image *pNewImage, int pWidht, int pHeight, IND_Format pFormat)
{
	GetDebugAPI()->Header ("Creating Image", 5);

	if (!mOk)
	{
		WriteMessage ();
		return 0;
	}

	// ----- Creating imagen using ImageLib -----
/*
	// New image instance
	ILuint mImageName;
	ilGenImages (1, &mImageName);
	ilBindImage (mImageName);

	// New image sizing
	iluImageParameter (ILU_FILTER, ILU_NEAREST);
	*/
	//iluScale (pWidht, pHeight, 3);
	pNewImage->ConvertImage (pFormat);

	// Attributes
	//pNewImage->SetWidth		(ilGetInteger (IL_IMAGE_WIDTH));
	//pNewImage->SetHeight	(ilGetInteger (IL_IMAGE_HEIGHT));
	//pNewImage->SetFormatInt	(ilGetInteger (IL_IMAGE_FORMAT));
	//pNewImage->SetBpp		(ilGetInteger (IL_IMAGE_BITS_PER_PIXEL));
	//pNewImage->SetPointer	(ilGetData ());

	// Name
	pNewImage->SetName		("Procedural");
	pNewImage->FlipV();

	// ImageLib image ID
	//pNewImage->SetImageLibId (mImageName);

	// ----- Puts the image in the list  -----

	AddToList (pNewImage);

	// ----- GetDebugAPI() -----

	GetDebugAPI()->Header ("File name:", 3);
	GetDebugAPI()->DataChar (pNewImage->GetName (), 1);

	GetDebugAPI()->Header ("Size:", 3);
	//GetDebugAPI()->DataInt (pNewImage->GetWidth (), 0);
	GetDebugAPI()->DataChar ("x", 0);
	//GetDebugAPI()->DataInt (pNewImage->GetHeight (), 1);

	GetDebugAPI()->Header ("Bpp:", 3);
	//GetDebugAPI()->DataInt (pNewImage->GetBpp (), 1);

	GetDebugAPI()->Header ("Format:", 3);
	GetDebugAPI()->DataChar (pNewImage->GetFormatChar(), 1);

	GetDebugAPI()->Header("Image created", 6);

	return 1;
}


/*!
\b Parameters:

\arg \b pIm                 Pointer to an image object.

\b Operation:

This function returns 1(true) if the image object passed as parameter exists and it is
deleted from the manager successfully.
*/
bool IND_ImageManager::Delete (IND_Image *pIm)
{
	GetDebugAPI()->Header ("Freeing image", 5);

	if (!mOk || !pIm)
	{
		WriteMessage ();
		return 0;
	}


	// Search object
	bool mIs = 0;
	list <IND_Image*>::iterator mImageListIter;
	for (mImageListIter  = mListImages->begin();
		 mImageListIter != mListImages->end();
		 mImageListIter++)
	{
		if ((*mImageListIter) == pIm)
		{
			mIs = 1;
			break;
		}
	}

	// Not found
	if (!mIs)
	{
		WriteMessage ();
		return 0;

	}




	// ----- Free object -----



	GetDebugAPI()->Header ("File name:", 3);
	GetDebugAPI()->DataChar (pIm->GetName (), 1);

	// Quit from list
	DelFromList (pIm);

	// Free image
	pIm->mImage.mImg.clear ();	

	// Free the string of the extension
	//delete[] pIm->mImage.mExt;



	GetDebugAPI()->Header ("Ok", 6);

	return 1;
}


/*!
\b Parameters:

\arg \b pNewImage             Pointer to a new image object
\arg \b pOldImage             Pointer to the image we want to clone

\b Operation:

This function returns 1 (true) if the image object passed as a parameter
exists and is added successfully to the manager creating a new image cloning from a previous one.
*/
bool IND_ImageManager::Clone (IND_Image *pNewImage, IND_Image *pOldImage)
{
	GetDebugAPI()->Header ("Duplicating Image", 5);

	/*
	if(mOk)
		//LOG_PRINT("mOk\n");
	else
		//LOG_PRINT("not mOk\n");
	//LOG_PRINT("pNewImage=%x\n",pNewImage);
	//LOG_PRINT("pOldImage=%x\n",pOldImage);
	
	*/
	if (!mOk || !pNewImage || !pOldImage)
	{
		WriteMessage ();
		return 0;
	}

	//LOG_PRINT("pOldImage->GetImageLibId ()=%x\n",pOldImage->GetImageLibId ());
	if (!pOldImage->mImage.mImg.getPixels ())
	{
		WriteMessage ();
		return 0;
	}

	// ----- Image creation using ImageLib -----
#if 0

	// New image instance
	ILuint mImageName;
	ilGenImages (1, &mImageName);
	ilBindImage (mImageName);

	// Copy the new image
	ilCopyImage (pOldImage->GetImageLibId());

	// ImageLib image ID
	//pNewImage->SetImageLibId (mImageName);
#else
	pNewImage->mImage.mImg=pOldImage->mImage.mImg;
#endif

	// Attributes
	//pNewImage->SetWidth		(pOldImage->GetWidth());
	//pNewImage->SetHeight	(pOldImage->GetHeight());
	pNewImage->SetFormatInt	(pOldImage->GetFormatInt());
	//pNewImage->SetBpp		(pOldImage->GetBpp());
	//pNewImage->SetPointer	(ilGetData ());
	pNewImage->SetName		("Image copied");
	//???pNewImage->FlipV();

	// ----- Put the object into the manager -----

	AddToList (pNewImage);

	// ----- GetDebugAPI() -----

	GetDebugAPI()->Header ("File name:", 3);
	GetDebugAPI()->DataChar (pOldImage->GetName (), 1);
	GetDebugAPI()->Header("Image duplicated", 6);

	return 1;
}


/*!
\b Parameters:

\arg \b pName          Image name

\b Operation:

This function returns 1 (true) if the image object passed as a parameter exists and
saves this image in a file with extension and type specified in the name.

It supports the following graphics formats (thanks to http://openil.sourceforge.net):
bmp, png, tga, jpg and pcx.
*/
bool IND_ImageManager::Save (IND_Image *pIm, const char *apName)
{
	std::string pName=(apName);
	
	GetDebugAPI()->Header ("Saving image", 5);

	if (!mOk || !pIm || !pIm->IsImageLoaded())
	{
		WriteMessage ();
		return 0;
	}

	// ----- Checking extension -----
#if 0
    const char *mExt = GetExtensionFromName (pName);
	if (CheckExtImage (mExt))
	{
		pIm->SetExtension (mExt);
	}
	else
	{
		GetDebugAPI()->Header ("Unknown extension", 2);
		return 0;
	}

	GetDebugAPI()->Header ("Image type:", 3);
	GetDebugAPI()->DataChar (mExt, 1);
#endif
	//ilBindImage (pIm->GetImageLibId ());

	GetDebugAPI()->Header ("File name:", 3);
	GetDebugAPI()->DataChar (pName.c_str(), 1);

	GetDebugAPI()->Header ("Size:", 3);
	//GetDebugAPI()->DataInt (pIm->GetWidth (), 0);
	GetDebugAPI()->DataChar ("x", 0);
	//GetDebugAPI()->DataInt (pIm->GetHeight (), 1);



	pIm->mImage.mImg.saveImage(pName.c_str());

	GetDebugAPI()->Header ("Ok", 6);
	return 1;
}


// --------------------------------------------------------------------------------
//									Private methods
// --------------------------------------------------------------------------------

/*
*************************
Obtain file extension
Return "" if the file hasn't got an extension
*************************
*/
const char *IND_ImageManager::GetExtensionFromName (const char *pName)
{
	int i;

	// The extension can be 127 chars large (1 char is for '/0')
	static std::string mExtImage;
	//mExtImage.resize(128);

	// The search starts at the end of the name
	for (i = stx_strlen (pName); i > -1; i--)
		if (pName [i] == '.') break;

	// Image without extension
	if (pName [i] != '.')
		return "";

	// Copy the extension
	mExtImage=std::string(&pName [i+1]);

    return (const char*)mExtImage.c_str();
}


/*
*************************
Check if it is a known extension
*************************
*/
bool IND_ImageManager::CheckExtImage (const char *pExtImage)
{
	return 1;//???
	//LOG_PRINT("pExtImage=%s\n",pExtImage);
	for (int i = 0; i < MAX_EXT_IMAGE; i++)
	{
        //mSupportedExt [i];
		//LOG_PRINT("mSupportedExt [%d]=%c%c%c\n", i, mSupportedExt [i]+0, mSupportedExt [i]+1, mSupportedExt [i]+2);
        //LOG_PRINT("mSupportedExt [%d]=%d\n", i, mSupportedExt [i]);
		//LOG_PRINT("mSupportedExt [%d]=%s\n", i, mSupportedExt [i]);
		if (!strcmp (pExtImage, mSupportedExt [i]))
			return 1;
	}

	return 0;
}


/*
*************************
Inserts object into the manager
*************************
*/
void IND_ImageManager::AddToList (IND_Image *pNewImage)
{
	mListImages->push_back (pNewImage);
}


/*
*************************
Deletes object from the manager
*************************
*/
void IND_ImageManager::DelFromList	(IND_Image *pIm)
{
	mListImages->remove (pIm);
	pIm = 0;
}


/*
*************************
Initialization error message
*************************
*/
void IND_ImageManager::WriteMessage ()
{
	GetDebugAPI()->Header ("This operation can not be done", 3);
	GetDebugAPI()->DataChar ("", 1);
	GetDebugAPI()->Header ("Invalid Id or IND_ImageManager not correctly initialized", 2);
	//STX_STACK_TRACE;
	//stx_exit(0);
}


/*
*************************
Init manager vars
*************************
*/
/*
void IND_ImageManager::Check ()
{
	
	return;
	if(mListImages&&mListImages->size()!=0) DBG_HALT;
	if(mListImages)
	{
	if(stx_strcmp(mSupportedExt [0], "bmp")!=0) DBG_HALT;
	if(stx_strcmp(mSupportedExt [1], "png")!=0) DBG_HALT;
	if(stx_strcmp(mSupportedExt [2], "tga")!=0) DBG_HALT;
	if(stx_strcmp(mSupportedExt [3], "jpg")!=0) DBG_HALT;
	if(stx_strcmp(mSupportedExt [4], "jpeg")!=0) DBG_HALT;
	if(stx_strcmp(mSupportedExt [5], "pcx")!=0) DBG_HALT;
	}
}
*/
void IND_ImageManager::InitVars ()
{

	// Supported extensions
#if 0
	for (int i = 0; i < MAX_EXT_IMAGE; i++)
	{
		//LOG_PRINT("mSupportedExt [%d]=%c%c%c\n", i, mSupportedExt [i]+0, mSupportedExt [i]+1, mSupportedExt [i]+2);
		//LOG_PRINT("mSupportedExt [%d]=%s\n", i, mSupportedExt [i]);
		//LOG_PRINT("mSupportedExt [%d]=%d\n", i, mSupportedExt [i]);
	}
#endif
	//if(!mListImages)
	mListImages = new list <IND_Image*>;

}


/*
*************************
Free manager memory
*************************
*/
void IND_ImageManager::FreeVars ()
{
	// Deletes all the manager entities
	list <IND_Image*>::iterator mImageListIter;
	for (mImageListIter  = mListImages->begin();
		 mImageListIter != mListImages->end();
		 mImageListIter++)
	{
		if (1)
		{
			GetDebugAPI()->Header	("Freeing image:", 3);
			GetDebugAPI()->DataChar ((*mImageListIter)->GetName (), 1);
			// Free image
			(*mImageListIter)->mImage.mImg.clear();
			// Free the string of the extension
			//DisposeArray ((*mImageListIter)->mImage.mExt);
		}
	}

	// Clear list
	mListImages->clear ();

	// Free list
	Dispose (mListImages);
}

