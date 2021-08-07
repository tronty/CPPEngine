#include <CIndieLib.h>

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Image mImageBugOriginal;
		IND_Image mImageBugGaussian;
		IND_Image mImageBugPixelize;
		IND_Image mImageBugContrast;
		IND_Surface mSurfaceBack;
		IND_Surface mOriginalSurface;
		IND_Surface mGaussianSurface;
		IND_Surface mPixelizeSurface;
		IND_Surface mContrastSurface;
		IND_Entity2d mBack;
		IND_Entity2d mOriginal;
		IND_Entity2d mGaussian;
		IND_Entity2d mPixelize;
		IND_Entity2d mContrast;
		virtual int init(const char* aTitle)
		{
			STX_INIT_SEGVCATCH;
			STX_TRY;
			g_pIndieLib = mI = CIndieLib::Instance();

	// ----- IndieLib intialization -----

#if 0
	//CIndieLib * mI = CIndieLib::Instance();
	if (!CIndieLib::Instance()->Init (aTitle)) return 0;
#else

	
//CIndieLib * mI = CIndieLib::Instance();

	if (!CIndieLib::Instance()->
		Init (aTitle
		
		)) return -1;

#endif
	// ----- Loading of the original image and making 4 duplicates -----

	////////////////////////////IND_Entity2d mContrast;
	if (!mI->ImageManager->Add (&mImageBugOriginal, "/Enemy_Bug.png")) return 0;

	////////////////////////////IND_Entity2d mContrast;
	if (!mI->ImageManager->Clone (&mImageBugGaussian, &mImageBugOriginal)) return 0;

	////////////////////////////IND_Entity2d mContrast;
	if (!mI->ImageManager->Clone (&mImageBugPixelize, &mImageBugOriginal)) return 0;

	////////////////////////////IND_Entity2d mContrast;
	if (!mI->ImageManager->Clone (&mImageBugContrast, &mImageBugOriginal)) return 0;

	// ----- Applying filters -----

	mImageBugGaussian.GaussianBlur	(5);
	mImageBugPixelize.Pixelize		(5);
	mImageBugContrast.Contrast		(20);

	// ----- Surface creation -----

	// Loading Background
	////////////////////////////IND_Entity2d mContrast;
	if (!mI->SurfaceManager->Add (&mSurfaceBack, "/blue_background.jpg", IND_OPAQUE, IND_32)) return 0;

	// Creating the "Original Bug" surface from the IND_Image
	////////////////////////////IND_Entity2d mContrast;
	if (!mI->SurfaceManager->Add (&mOriginalSurface, &mImageBugOriginal, IND_ALPHA, IND_32)) return 0;

	// Creating the "Gaussian bug" surface from the IND_Image
	////////////////////////////IND_Entity2d mContrast;
	if (!mI->SurfaceManager->Add (&mGaussianSurface, &mImageBugGaussian, IND_ALPHA, IND_32)) return 0;

	// Creating the "Pixelize bug" surface from the IND_Image
	////////////////////////////IND_Entity2d mContrast;
	if (!mI->SurfaceManager->Add (&mPixelizeSurface, &mImageBugPixelize, IND_ALPHA, IND_32)) return 0;

	// Creating the "Contrast bug" surface from the IND_Image
	////////////////////////////IND_Entity2d mContrast;
	if (!mI->SurfaceManager->Add (&mContrastSurface, &mImageBugContrast, IND_ALPHA, IND_32)) return 0;

	// ----- Deleting of images -----
#ifndef OMITDELETE
	mI->ImageManager->Delete (&mImageBugOriginal);
	mI->ImageManager->Delete (&mImageBugGaussian);
	mI->ImageManager->Delete (&mImageBugPixelize);
	mI->ImageManager->Delete (&mImageBugContrast);
#endif
	// ----- Set the surfaces into the 2d entities -----

	// Creating 2d entity for the background
	////////////////////////////IND_Entity2d mContrast;
	mI->Entity2dManager->Add (&mBack);						// Entity adding
	mBack.SetSurface (&mSurfaceBack);						// Set the surface into the entity

	// Creating 2d entity for the "Original bug"
	////////////////////////////IND_Entity2d mContrast;
	mI->Entity2dManager->Add (&mOriginal);					// Entity adding
	mOriginal.SetSurface (&mOriginalSurface);				// Set the surface into the entity

	// Creating 2d entity for the "Gaussian bug"
	////////////////////////////IND_Entity2d mContrast;
	mI->Entity2dManager->Add (&mGaussian);					// Entity adding
	mGaussian.SetSurface (&mGaussianSurface);				// Set the surface into the entity

	// Creating 2d entity for the "pixelize bug"
	////////////////////////////IND_Entity2d mContrast;
	mI->Entity2dManager->Add (&mPixelize);					// Entity adding
	mPixelize.SetSurface (&mPixelizeSurface);				// Set the surface into the entity

	// Creating 2d entity for the "Contrast bug"
	////////////////////////////IND_Entity2d mContrast;
	mI->Entity2dManager->Add (&mContrast);					// Entity adding
	mContrast.SetSurface (&mContrastSurface);				// Set the surface into the entity

	// ----- Changing the attributes of the 2d entities -----

	mOriginal.SetPosition (290, 90, 0);
	mGaussian.SetPosition (50, 230, 0);
	mPixelize.SetPosition (290, 230, 0);
	mContrast.SetPosition (520, 230, 0);

	// ----- Main Loop -----
	STX_CATCH;
	return 0;

	}
virtual void render()
{
	STX_TRY;
		mI->Input->Update ();
		mI->Render->BeginScene ();
		mI->Entity2dManager->RenderEntities2d ();
		mI->Render->EndScene ();
		STX_CATCH;
	}

	// ----- Free -----

	};
GameLogic gl;
IGameLogic* glp=&gl;
int init(const char* aTitle){return glp->init(aTitle);}
void render(){return glp->render();}
int IndieLib()
{
	glp->init("a_03_Ind_Image");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

