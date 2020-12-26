#include <CIndieLib.h>

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
	float	mAngle = 0;
	float	mScale = 1.0f;
	int		mSpeedRotation = 5;
	int		mSpeedScaling = 1;
	float	mDelta;
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Surface mSurfaceBack;
		IND_Surface mSurfaceRocket;
		IND_Surface mSurfaceBeetle;
		IND_Animation mSwordMasterAnimation;
		IND_Font mFontSmall;
		IND_Entity2d mBack;
		IND_Entity2d mRocket;
		IND_Entity2d mBeetle;
		IND_Entity2d mSwordMaster;
		IND_Entity2d mTextSmallWhite;
		virtual int init(const char* aTitle)
		{
			STX_INIT_SEGVCATCH;
			STX_TRY;
			g_pIndieLib = mI = CIndieLib::Instance();
	// ----- IndieLib intialization -----
#if 0
	//CIndieLib * mI = CIndieLib::Instance();
	if (!mI->Init (aTitle)) return 0;
#else
	
//CIndieLib * mI = CIndieLib::Instance();
	if (!CIndieLib::Instance()->
		Init (aTitle
		
		)) return -1;
#endif
	// ----- Surface loading -----

	// Loading Background
	////////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->SurfaceManager->Add (&mSurfaceBack, "/twist.jpg", IND_OPAQUE, IND_32)) return 0;

	// Loading Rocket
	////////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->SurfaceManager->Add (&mSurfaceRocket, "/rocket.png", IND_ALPHA, IND_32)) return 0;

	// Loading Beetleship
	////////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->SurfaceManager->Add (&mSurfaceBeetle, "/beetleship.png", IND_ALPHA, IND_32)) return 0;

	// Sword Master animation, we apply a color key of (0, 255, 0)
	////////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->AnimationManager->AddToSurface (&mSwordMasterAnimation, "/animations/sword_master.xml", IND_ALPHA, IND_16, 0, 255, 0)) return 0;

	// ----- Font loading -----

	// Font
	////////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->FontManager->Add (&mFontSmall, "/font_small.png", "/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Set the surfaces into 2d entities -----

	// Creating 2d entity for the background
	////////////////////IND_Entity2d mTextSmallWhite;
	mI->Entity2dManager->Add (&mBack);						// Entity adding
	mBack.SetSurface (&mSurfaceBack);						// Set the surface into the entity

	// Creating 2d entity for the Rocket
	////////////////////IND_Entity2d mTextSmallWhite;
	mI->Entity2dManager->Add (&mRocket);					// Entity adding
	mRocket.SetSurface (&mSurfaceRocket);					// Set the surface into the entity

	// Creating 2d entity for the Beetleship
	////////////////////IND_Entity2d mTextSmallWhite;
	mI->Entity2dManager->Add (&mBeetle);					// Entity adding
	mBeetle.SetSurface (&mSurfaceBeetle);					// Set the surface into the entity

	// Creating 2d entity for the Sword Master animation
	////////////////////IND_Entity2d mTextSmallWhite;
	mI->Entity2dManager->Add (&mSwordMaster);				// Entity adding
	mSwordMaster.SetAnimation (&mSwordMasterAnimation);		// Set the animation into the entity

	// Text small white
	////////////////////IND_Entity2d mTextSmallWhite;
	mI->Entity2dManager->Add (&mTextSmallWhite);			// Entity adding
	mTextSmallWhite.SetFont (&mFontSmall);					// Set the font into the entity

	// ----- Changing the attributes of the 2d entities -----

	// Background
	mBack.SetHotSpot (0.5f, 0.5f);
	mBack.SetPosition (400, 300, 0);
	mBack.SetScale (1.7f, 1.7f);

	// Beetle
	mBeetle.SetHotSpot (0.5f, 0.5f);
	mBeetle.SetMirrorX (1);
	mBeetle.SetBoundingTriangle ("beetle_head", 160, 105, 160, 170, 190, 135);
	mBeetle.SetBoundingCircle ("beetle_boy_head", 85, 52, 55);

	// Rocket
	mRocket.SetHotSpot (0.5f, 0.5f);
	mRocket.SetPosition (200, 450, 1);
	mRocket.SetBoundingAreas ("/rocket_collisions.xml");
	//mRocket.DeleteBoundingAreas ("engines");

	// Sword Master Animation
	mSwordMaster.SetHotSpot (0.5f, 0.5f);
	mSwordMaster.SetPosition (400, 220, 3);

	// Text
	mTextSmallWhite.SetLineSpacing	(18);
	mTextSmallWhite.SetCharSpacing	(-7);
	mTextSmallWhite.SetPosition		(400, 30, 10);
	mTextSmallWhite.SetAlign		(IND_CENTER);

	// ----- Main Loop -----
	STX_CATCH;
	return 0;



	}
virtual void render()
{
	STX_TRY;
		// ----- Input update ----

		mI->Input->Update ();

		// ----- Input ----

		mDelta = mI->Render->GetFrameTime() / 1000.0f;

		if (mI->Input->IsKeyPressed (STX_KEY_RIGHT))	mScale += mSpeedScaling * mDelta;
		if (mI->Input->IsKeyPressed (STX_KEY_LEFT))	mScale -= mSpeedScaling * mDelta;
		mAngle += mSpeedRotation * mDelta;
		if (mScale < 0) mScale = 0;

		// ----- Updating entities attributes  -----

		mBack.SetAngleXYZ (0, 0, -mAngle);
		mRocket.SetAngleXYZ (0, 0, mAngle);
		mRocket.SetScale (mScale, mScale);
		mBeetle.SetPosition ((float) mI->Input->GetMouseX(), (float) mI->Input->GetMouseY(), 5);

		// ----- Check collisions -----

		mTextSmallWhite.SetText	("No collision between the groups we are checking");

		if (mI->Entity2dManager->IsCollision (&mRocket, "engines", &mBeetle, "beetle_boy_head"))
			mTextSmallWhite.SetText	("Collision between rocket boy head and engines");

		if (mI->Entity2dManager->IsCollision (&mRocket, "rocket_head", &mBeetle, "beetle_head"))
			mTextSmallWhite.SetText	("Collision between rokect head and beetle head");

		if (mI->Entity2dManager->IsCollision (&mRocket, "rocket_boy_head", &mBeetle, "beetle_boy_head"))
			mTextSmallWhite.SetText	("Collision between rocket boy head and beetle boy head");

		if (mI->Entity2dManager->IsCollision (&mBeetle, "beetle_boy_head", &mSwordMaster, "sword"))
			mTextSmallWhite.SetText	("Collision between beetle boy head and the sword");

		// ----- Render  -----

		mI->Render->BeginScene ();
		mI->Entity2dManager->RenderEntities2d ();
		mI->Entity2dManager->RenderCollisionAreas (255, 0, 0, 255);
		//mI->Entity2dManager->RenderGridAreas (255, 255, 0, 255);
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
	glp->init("a_08_Collisions");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

