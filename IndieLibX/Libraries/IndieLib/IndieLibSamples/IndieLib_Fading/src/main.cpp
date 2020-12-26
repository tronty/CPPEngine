#include <CIndieLib.h>


/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
	float fElapsedTime = 0.0f;
	std::string szText;
	int iiiAlpha = 255;	// start off fully opaque

class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Surface SurfaceSnake;
		IND_Surface SurfacePool;
		IND_Font FontSmall;
		IND_Entity2d TextSmallWhite;
		IND_Entity2d PoolEntity2d;
		IND_Entity2d SnakeEntity2d;
		IND_Timer timer;
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

	// Loading snake image into a surface
	//////////////IND_Timer timer;
	if (!mI->SurfaceManager->Add (&SurfaceSnake, "/IndieLib-Fading/snake.png", IND_ALPHA, IND_32)) return 0;

	// Loading lpool image into a surface
	//////////////IND_Timer timer;
	if (!mI->SurfaceManager->Add (&SurfacePool, "/IndieLib-Fading/pool.jpg", IND_OPAQUE, IND_32)) return 0;

	// ----- Font creation -----
	// Font (layer 3)
	//////////////IND_Timer timer;
	if (!mI->FontManager->Add (&FontSmall, "/IndieLib-Fading/font_small.png", "/IndieLib-Fading/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// Create the Font
	//////////////IND_Timer timer;
	mI->Entity2dManager->Add		(1, &TextSmallWhite);		// Entity adding (Layer 3)
	TextSmallWhite.SetFont			(&FontSmall);				// Set the font into the entity
	TextSmallWhite.SetLineSpacing	(18);
	TextSmallWhite.SetCharSpacing	(-8);
	TextSmallWhite.SetPosition		(5, 5, 1);
	TextSmallWhite.SetAlign		(IND_LEFT);

	// Creating 2d entities for the pool and snake
	//////////////IND_Timer timer;
	mI->Entity2dManager->Add (0, &PoolEntity2d);			// Entity adding !!! Note the first parameter is layer #
	PoolEntity2d.SetSurface (&SurfacePool);				// Set the surface into the entity

	//////////////IND_Timer timer;
	mI->Entity2dManager->Add (0, &SnakeEntity2d);			// Entity adding !!! Note the first parameter is layer #
	SnakeEntity2d.SetSurface (&SurfaceSnake);				// Set the surface into the entity


	//////////////IND_Timer timer;


	// ----- Main Loop -----
	timer.Start();
	STX_CATCH;
	return 0;

	}
virtual void render()
{
	STX_TRY;
		mI->Input->Update ();

		fElapsedTime = mI->Render->GetFrameTime() / 1000.0f;

		mI->Render->BeginScene ();
		mI->Render->ClearViewPort (60, 60, 60);
		mI->Render->SetViewPort2d (0, 0, mI->Window->GetWidth(), mI->Window->GetHeight());

		mI->Entity2dManager->RenderEntities2d ( 0 );

		if (mI->Input->IsKeyPressed (STX_KEY_z))
		{
			iiiAlpha --;
			if (iiiAlpha < 0)
				iiiAlpha = 0;
		}
		else
		if (mI->Input->IsKeyPressed (STX_KEY_x))
		{
			iiiAlpha ++;
			if (iiiAlpha > 255)
				iiiAlpha = 255;
		}

		// Set the snake's transparency
		SnakeEntity2d.SetTransparency(iiiAlpha);


		// ----- Text -----
		szText = "Z to fade out, X to fade in\n";
		szText += "ESC to quit";
		TextSmallWhite.SetText	 ((char*)szText.c_str());

		mI->Entity2dManager->RenderEntities2d ( 1 );

		//???mI->Render->ShowFpsInWindowTitle();
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
	glp->init("IndieLib-Fading");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

