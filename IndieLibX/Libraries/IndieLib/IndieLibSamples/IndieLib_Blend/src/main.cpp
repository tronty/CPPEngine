#define __ImageLib__ 1
#include <CIndieLib.h>

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
	// --- Variables ---
	float fElapsedTime = 0.0f;
	bool bShowBounds = false;
	bool bToggleRes = false;
	float speed = 1500.0f;
	int btnMissionAlpha = 0;
	int btnSurvivalAlpha = 0;
	int buttonBrightness = 180; // this is really just the alpha level
	float fScreenFlashTimer = 0.0f;
	float fMAX_FLASH = 0.15f;
	char szText [128];
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Timer timer;
		IND_Surface surfaceRed;
		IND_Surface surfaceBG;
		IND_Surface surfaceHightlight;
		IND_Surface surfaceCursor;
		IND_Font mFontSmall;
		IND_Entity2d TextSmallWhite;
		IND_Entity2d entityBG;
		IND_Entity2d entityRed;
		IND_Entity2d btnTargetMission;
		IND_Entity2d btnTargetSurvival;
		IND_Entity2d entityCursor;
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
		))
		return -1;
#endif
	////////////////////////IND_Entity2d entityCursor;

	////////////////////////IND_Entity2d entityCursor;
	mI->SurfaceManager->Add(&surfaceRed, "/IndieLib-Blend/red.png", IND_ALPHA, IND_32);

	////////////////////////IND_Entity2d entityCursor;
	mI->SurfaceManager->Add(&surfaceBG, "/IndieLib-Blend/mainmenu.jpg", IND_OPAQUE, IND_32);

	////////////////////////IND_Entity2d entityCursor;
	mI->SurfaceManager->Add(&surfaceHightlight, "/IndieLib-Blend/mainbigbtn.png", IND_ALPHA, IND_32);

	////////////////////////IND_Entity2d entityCursor;
	mI->SurfaceManager->Add(&surfaceCursor, "/IndieLib-Blend/cursor_arrow.png", IND_ALPHA, IND_32);

	// ----- Font creation -----
	////////////////////////IND_Entity2d entityCursor;
	if (!mI->FontManager->Add (&mFontSmall, "/IndieLib-Blend/font_small.png", "/IndieLib-Blend/font_small.xml", IND_ALPHA, IND_32)) return 0;

	////////////////////////IND_Entity2d entityCursor;
	mI->Entity2dManager->Add (2, &TextSmallWhite);			// Entity adding in layer 2 (gui)
	TextSmallWhite.SetFont (&mFontSmall);					// Set the font into the entity
	TextSmallWhite.SetLineSpacing	(18);
	TextSmallWhite.SetCharSpacing	(-8);
	TextSmallWhite.SetPosition		(15, 15, 4);
	TextSmallWhite.SetAlign			(IND_LEFT);


	// --- Background ---
	////////////////////////IND_Entity2d entityCursor;
	mI->Entity2dManager->Add(&entityBG);
	entityBG.SetSurface(&surfaceBG);

	// --- Red flash screen thing ---
	////////////////////////IND_Entity2d entityCursor;
	mI->Entity2dManager->Add(2, &entityRed);		// gui layer 2
	entityRed.SetSurface(&surfaceRed);
	entityRed.SetShow(false);
	entityRed.SetTransparency(127);

	// --- "Mission" Button ---
	////////////////////////IND_Entity2d entityCursor;
	mI->Entity2dManager->Add(&btnTargetMission);
	btnTargetMission.SetSurface(&surfaceHightlight);
	btnTargetMission.SetTransparency(0);
	btnTargetMission.SetPosition(100, 250, 0); // location of "Mission" button in background image
	btnTargetMission.SetHotSpot(0.5f, 0.5f);
	// !! Circle Bounds x and y are relative to the upper, left corner of the surface NOT to
	// the HotSpot
	btnTargetMission.SetBoundingCircle("btnMission", 64, 64, 50);	// 64 is half of surface width and height
	btnTargetMission.SetBoundingRectangle("no collision", 0, 0, 128, 128); 	// surface bounds, not used in collision

	// --- "Survival" Button ---
	////////////////////////IND_Entity2d entityCursor;
	mI->Entity2dManager->Add(&btnTargetSurvival);
	btnTargetSurvival.SetSurface(&surfaceHightlight);
	btnTargetSurvival.SetTransparency(0);
	btnTargetSurvival.SetPosition(230, 380, 0);	// location of "Survival" button in background image
	btnTargetSurvival.SetHotSpot(0.5f, 0.5f);
	// !! Circle Bounds x and y are relative to the upper, left corner of the surface NOT to
	// the HotSpot
	btnTargetSurvival.SetBoundingCircle("btnSurvival", 64, 64, 50); // 64 is half of surface width and height
	btnTargetSurvival.SetBoundingRectangle("no collision", 0, 0, 128, 128);	// surface bounds, not used in collision

	// --- Cursor ---
	////////////////////////IND_Entity2d entityCursor;
	mI->Entity2dManager->Add(&entityCursor);
	entityCursor.SetSurface(&surfaceCursor);
	entityCursor.SetBoundingCircle("", 27, 27, 3);
	entityCursor.SetHotSpot(0.5f, 0.5f);




	// ----- Main Loop -----
	timer.Start();
	STX_CATCH;
	return 0;
	}
virtual void render()
{
	STX_TRY;
		stx_strlcpy (szText, "Press 'b' to toggle display bounds\n", 128);
		stx_strlcat (szText, "Press 'f' to red flash the screen\n", 128);
		stx_strlcat (szText, "Press ESC' to quit", 128);
		TextSmallWhite.SetText	 (szText);

		mI->Input->Update ();

		// ----- Delta Time -----
		fElapsedTime = mI->Render->GetFrameTime() / 1000.0f;

		entityCursor.SetPosition((float)mI->Input->GetMouseX(), (float)mI->Input->GetMouseY(), 1);

		// Set off screen flash effect
		if (mI->Input->OnKeyPress(STX_KEY_f))
		{
			fScreenFlashTimer = fMAX_FLASH;
			entityRed.SetShow(true);
		}

		// Show bounds
		if (mI->Input->OnKeyPress(STX_KEY_b))
			bShowBounds = !bShowBounds;

		// --- Is mouse over "Mission" button? ---
		// Check collisions with mouse and button collision zone(s)
		if (mI->Entity2dManager->IsCollision(&entityCursor, "", &btnTargetMission, "btnMission"))
		{
			btnTargetMission.SetShow(true);

			entityBG.SetBlendSource(IND_SRCALPHA);
			entityBG.SetBlendDest(IND_INVSRCALPHA);
			btnTargetMission.SetBlendSource(IND_SRCALPHA);
			btnTargetMission.SetBlendDest(IND_ONE);

			btnMissionAlpha += (int)(fElapsedTime * speed);
			if (btnMissionAlpha > buttonBrightness)
				btnMissionAlpha = buttonBrightness;

			// Apply the alpha value
			btnTargetMission.SetTransparency(btnMissionAlpha);
		}
		else
		{
			btnTargetMission.SetShow(false);
			btnMissionAlpha = 0;
		}

		// --- Is mouse over "Survival" button? ---
		// Check collisions with mouse and button collision zone(s)
		if (mI->Entity2dManager->IsCollision(&entityCursor, "", &btnTargetSurvival, "btnSurvival"))
		{
			btnTargetSurvival.SetShow(true);

			entityBG.SetBlendSource(IND_SRCALPHA);
			entityBG.SetBlendDest(IND_INVSRCALPHA);
			btnTargetSurvival.SetBlendSource(IND_SRCALPHA);
			btnTargetSurvival.SetBlendDest(IND_ONE);

			btnSurvivalAlpha += (int)(fElapsedTime * speed);
			if (btnSurvivalAlpha > buttonBrightness)
				btnSurvivalAlpha = buttonBrightness;

			// Apply the alpha value
			btnTargetSurvival.SetTransparency(btnSurvivalAlpha);
		}
		else
		{
			btnTargetSurvival.SetShow(false);
			btnSurvivalAlpha = 0;
		}

		// --- Render the scene ----
		mI->Render->BeginScene ();
		mI->Render->ClearViewPort (60, 60, 60);
		mI->Render->SetViewPort2d (0, 0, mI->Window->GetWidth(), mI->Window->GetHeight());

		mI->Entity2dManager->RenderEntities2d ( );

		if (bShowBounds)
			mI->Entity2dManager->RenderCollisionAreas(255, 0, 0, 255);

		//---------------------------
		// Render screen flash effect
		//---------------------------
		fScreenFlashTimer -= fElapsedTime;
		if (fScreenFlashTimer > 0.0f)
		{
			int alpha = (int)(127 - (127 * (fMAX_FLASH - fScreenFlashTimer)));
			if (alpha < 0)
				alpha = 0;
			entityRed.SetTransparency(alpha);
		}
		else
		{
			entityRed.SetShow(false);
			fScreenFlashTimer = 0.0f;
		}

		mI->Entity2dManager->RenderEntities2d (1);
		mI->Entity2dManager->RenderEntities2d (2);	// GUI layer
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
	glp->init("IndieLib-Blend");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

