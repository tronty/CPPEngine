#include <CIndieLib.h>
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
	int MiddleScreenX;// = mI->Window->GetWidth() / 2;
	int MiddleScreenY;// = mI->Window->GetHeight() / 2;

	float Camera0PosX;// = (float)MiddleScreenX;
	float Camera1PosX;// = (float)MiddleScreenX;
	float Camera2PosX;// = (float)MiddleScreenX;

	// --- Some variables ---
	int		Layer0Speed		= 50;
	int		Layer1Speed		= 162;
	int		Layer2Speed		= 250;
	char	mText [2048];
	float	fElapsedTime = 0.0f;
/*
==================
Main
==================
*/

class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Timer timer;
		IND_Surface SurfaceSky;
		IND_Surface SurfaceLayer1;
		IND_Surface SurfaceLayer2;
		IND_Font mFontSmall;
		IND_Entity2d mTextSmallWhite;
		IND_Entity2d SkyEntity2d;
		IND_Entity2d CaveEntity2d;
		IND_Entity2d CaveNearEntity2d;
		IND_Camera2d* pCameras[3];
		IND_Camera2d CameraText;
		virtual int init(const char* aTitle)
		{
			STX_INIT_SEGVCATCH;
			STX_TRY;
			g_pIndieLib = mI = CIndieLib::Instance();
	// ----- IndieLib intialization -----

#if 0
	//CIndieLib * mI = CIndieLib::Instance();
	if (!CIndieLib::Instance()->Init ()) return 0;
#else
	
//CIndieLib * mI = CIndieLib::Instance();
	if (!CIndieLib::Instance()->
		Init (aTitle
		)) return -1;
#endif
	//////////////////IND_Entity2d CaveNearEntity2d;



	mText [0]				= 0;
	
	// ----- Surface creation -----

	// Loading sky sackground (layer 0)
	//////////////////IND_Entity2d CaveNearEntity2d;
	if (!mI->SurfaceManager->Add (&SurfaceSky, "/IndieLib-ParallaxScroll/sky.jpg", IND_OPAQUE, IND_32)) return 0;

	// Loading layer 1
	//////////////////IND_Entity2d CaveNearEntity2d;
	if (!mI->SurfaceManager->Add (&SurfaceLayer1, "/IndieLib-ParallaxScroll/cave.png", IND_ALPHA, IND_32)) return 0;

	// Loading layer 2
	//////////////////IND_Entity2d CaveNearEntity2d;
	if (!mI->SurfaceManager->Add (&SurfaceLayer2, "/IndieLib-ParallaxScroll/cave_near.png", IND_ALPHA, IND_32)) return 0;

	// Font (layer 3)
	//////////////////IND_Entity2d CaveNearEntity2d;
	if (!mI->FontManager->Add (&mFontSmall, "/IndieLib-ParallaxScroll/font_small.png", "/IndieLib-ParallaxScroll/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Font creation -----

	//////////////////IND_Entity2d CaveNearEntity2d;
	mI->Entity2dManager->Add		(3, &mTextSmallWhite);		// Entity adding (Layer 3)
	mTextSmallWhite.SetFont			(&mFontSmall);				// Set the font into the entity
	mTextSmallWhite.SetLineSpacing	(18);
	mTextSmallWhite.SetCharSpacing	(-8);
	mTextSmallWhite.SetPosition		(5, 5, 1);
	mTextSmallWhite.SetAlign		(IND_LEFT);

	// ----- Entities -----
	// The layer may be treated as some kind of Z coordinate.
	// The entity with the biggest layer value will be the top-most entity on the screen.

	// Creating 2d entity for the sky
	//////////////////IND_Entity2d CaveNearEntity2d;
	mI->Entity2dManager->Add (0, &SkyEntity2d);				// Entity adding, !!! Note the first parameter is layer #
	SkyEntity2d.SetSurface (&SurfaceSky);					// Set the surface into the entity
	SkyEntity2d.SetPosition(600, 0, 0);


	// Creating 2d entity for the cave (middle layer)
	//////////////////IND_Entity2d CaveNearEntity2d;
	mI->Entity2dManager->Add (1, &CaveEntity2d);			// Entity adding !!! Note the first parameter is layer #
	CaveEntity2d.SetSurface (&SurfaceLayer1);				// Set the surface into the entity

	// Creating 2d entity for the cave near (front layer, layer 2)
	//////////////////IND_Entity2d CaveNearEntity2d;
	mI->Entity2dManager->Add (2, &CaveNearEntity2d);			// Entity adding, !!! Note the first parameter is layer #
	CaveNearEntity2d.SetSurface (&SurfaceLayer2);				// Set the surface into the entity

	// ----- Setting up Cameras -----
	// That means that to make parallax you have to load graphics into layered entities.
	// Then create a separate cameras for each of the layers and move the cameras with
	// a different speed.
	MiddleScreenX = mI->Window->GetWidth() / 2;
	MiddleScreenY = mI->Window->GetHeight() / 2;

	Camera0PosX = (float)MiddleScreenX;
	Camera1PosX = (float)MiddleScreenX;
	Camera2PosX = (float)MiddleScreenX;

	pCameras[0] = new IND_Camera2d((int)Camera0PosX, MiddleScreenY);
	pCameras[1] = new IND_Camera2d((int)Camera1PosX, MiddleScreenY);
	pCameras[2] = new IND_Camera2d((int)Camera2PosX, MiddleScreenY);

	CameraText=IND_Camera2d((int) mI->Window->GetWidth() / 2, mI->Window->GetHeight() / 2);

	MiddleScreenX = mI->Window->GetWidth() / 2;
	MiddleScreenY = mI->Window->GetHeight() / 2;

	Camera0PosX = (float)MiddleScreenX;
	Camera1PosX = (float)MiddleScreenX;
	Camera2PosX = (float)MiddleScreenX;

	// ----- Main Loop -----
	timer.Start();
	STX_CATCH;
	return 0;

	}
virtual void render()
{
		STX_TRY;
		mI->Input->Update ();

		// ----- Text -----
		stx_strlcpy (mText, "Use right and left arrow keys for moving the camera\n", 2048);
		mTextSmallWhite.SetText	 (mText);

		// ----- Delta Time -----
		fElapsedTime = mI->Render->GetFrameTime() / 1000.0f;

		// Move cameras when pressing right key
		if (mI->Input->IsKeyPressed(STX_KEY_RIGHT))
		{
			// Check limits
			if (Camera2PosX < SurfaceLayer2.GetWidth() - MiddleScreenX)
			{
				Camera0PosX += Layer0Speed * fElapsedTime;
				Camera1PosX += Layer1Speed * fElapsedTime;
				Camera2PosX += Layer2Speed * fElapsedTime;
			}
		}
		else
		if (mI->Input->IsKeyPressed(STX_KEY_LEFT))
		{
			// Check limits
			if (Camera2PosX > MiddleScreenX)
			{
				Camera0PosX -= Layer0Speed * fElapsedTime;
				Camera1PosX -= Layer1Speed * fElapsedTime;
				Camera2PosX -= Layer2Speed * fElapsedTime;
			}
		}

		// ---- Updating cameras -----
		pCameras[0]->SetPosition((int)Camera0PosX, MiddleScreenY);
		pCameras[1]->SetPosition((int)Camera1PosX, MiddleScreenY);
		pCameras[2]->SetPosition((int)Camera2PosX, MiddleScreenY);

		mI->Render->BeginScene ();
		mI->Render->ClearViewPort (60, 60, 60);
		mI->Render->SetViewPort2d (0, 0, mI->Window->GetWidth(), mI->Window->GetHeight());

		// Render layers (0-2)
		for (int i = 0; i < 3; i++)
		{
			mI->Render->SetCamera2d(pCameras[i]);
			mI->Entity2dManager->RenderEntities2d (i);
		}

		// Render the text
		mI->Render->SetCamera2d(&CameraText);
		mI->Entity2dManager->RenderEntities2d(3);

		//???mI->Render->ShowFpsInWindowTitle();
		mI->Render->EndScene ();
		STX_CATCH;
	}

	};

GameLogic gl;
IGameLogic* glp=&gl;
int init(const char* aTitle){return glp->init(aTitle);}
void render(){return glp->render();}
int IndieLib()
{
	glp->init("IndieLib-ParallaxScroll");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();

	// ----- Free -----
	//???for (int i = 0; i < 3; i++)
	//	delete pCameras[i];

	return 0;
}

