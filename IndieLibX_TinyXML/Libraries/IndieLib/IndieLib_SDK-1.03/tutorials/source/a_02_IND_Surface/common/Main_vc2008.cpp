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
		IND_Surface mSurfaceBack;
		IND_Surface mSurfaceWarrior;
		IND_Surface mSurfaceStar;
		IND_Entity2d mBack;
		IND_Entity2d mWarrior;
		IND_Entity2d mStar1;
		IND_Entity2d mStar2;
		IND_Entity2d mStar3;
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

	// ----- Surface loading -----

	// Loading Background
	////////////////IND_Entity2d mStar3;

	if (!mI->SurfaceManager->Add (&mSurfaceBack, "/blue_background.jpg", IND_OPAQUE, IND_32)) return 0;

	// Loading sprite of a warrior
	////////////////IND_Entity2d mStar3;
	if (!mI->SurfaceManager->Add (&mSurfaceWarrior, "/derekyu_sprite.png", IND_ALPHA, IND_32, 255, 0, 255)) return 0;

	// Loading sprite of a star
	////////////////IND_Entity2d mStar3;
	if (!mI->SurfaceManager->Add (&mSurfaceStar, "/star.png", IND_ALPHA, IND_32)) return 0;


	// ----- Set the surfaces into 2d entities -----

	// Creating 2d entity for the background
	////////////////IND_Entity2d mStar3;
	mI->Entity2dManager->Add (&mBack);						// Entity adding
	mBack.SetSurface (&mSurfaceBack);						// Set the surface into the entity

	// Creating 2d entity for the warrior
	////////////////IND_Entity2d mStar3;
	mI->Entity2dManager->Add (&mWarrior);					// Entity adding
	mWarrior.SetSurface (&mSurfaceWarrior);					// Set the surface into the entity

	// Creating 2d entity for the star 1
	////////////////IND_Entity2d mStar3;
	mI->Entity2dManager->Add (&mStar1);						// Entity adding
	mStar1.SetSurface (&mSurfaceStar);						// Set the surface into the entity

	// Creating 2d entity for the star 2 (big and a bit orange)
	////////////////IND_Entity2d mStar3;
	mI->Entity2dManager->Add (&mStar2);						// Entity adding
	mStar2.SetSurface (&mSurfaceStar);						// Set the surface into the entity

	// Creating 2d entity for the star 3
	////////////////IND_Entity2d mStar3;
	mI->Entity2dManager->Add (&mStar3);						// Entity adding
	mStar3.SetSurface (&mSurfaceStar);						// Set the surface into the entity

	// ----- Changing the attributes of the 2d entities -----

	// Warrior
	mWarrior.SetPosition (400, 170, 0);					// Set the position of the entity

	// Original Star without chaning it's attributes
	mStar1.SetPosition (100, 270, 0);					// Set the position of the entity

	// We change the attributes of this entity in order
	// to create a big rotated semitransparent star with
	// an orange tint
	mStar2.SetPosition (280, 200, 0);					// Set the position of the entity
	mStar2.SetScale	(2, 2);								// Set the scale of the entity
	mStar2.SetTint	(240, 160, 230);					// Set tint to color RGB = (240, 160, 230)
	mStar2.SetTransparency (200);						// Level of transparency 200 (255 will be opaque)
	mStar2.SetAngleXYZ	(0, 0, 45);						// Rotation in Z angle = 45º

	// A bigger star than the original, faded to pink. We only draw a region of 50x50 pixels
	mStar3.SetHotSpot (0.5f, 0.5f);						// We change the reference point of the entity
	mStar3.SetPosition (400, 470, 0);					// Set the position of the entity
	mStar3.SetScale	(1.5f, 1.5f);						// Set the scale of the entity
	mStar3.SetFade	(230, 0, 230, 128);					// Set fade to pink color, the amout of fade is 128 (255 will be complety pink)
	mStar3.SetRegion (20, 20, 50, 50);					// Region we want to draw


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
	glp->init("a_02_IND_Surface");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

