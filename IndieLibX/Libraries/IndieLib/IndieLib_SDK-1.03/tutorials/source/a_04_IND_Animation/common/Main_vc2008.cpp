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
		IND_Animation mAnimationCharacter1;
		IND_Animation mAnimationCharacter2;
		IND_Animation mAnimationDust;
		IND_Entity2d mBack;
		IND_Entity2d mPlayer1;
		IND_Entity2d mPlayer2;
		IND_Entity2d mDust;
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
	////////////////IND_Entity2d mDust;
	if (!mI->SurfaceManager->Add (&mSurfaceBack, "/blue_background.jpg", IND_OPAQUE, IND_32)) return 0;

	// ----- Animations loading -----

	// Characters animations, we apply a color key of (0, 48, 152)
	////////////////IND_Entity2d mDust;
	if (!mI->AnimationManager->AddToSurface (&mAnimationCharacter1, "/animations/character1.xml", IND_ALPHA, IND_32, 0, 48, 152)) return 0;

	// Characters animations, we apply a color key of (0, 48, 152)
	////////////////IND_Entity2d mDust;
	if (!mI->AnimationManager->AddToSurface (&mAnimationCharacter2, "/animations/character2.xml", IND_ALPHA, IND_32, 0, 48, 152)) return 0;

	// Dust animation, we apply a color key of (255, 0, 255)
	////////////////IND_Entity2d mDust;
	if (!mI->AnimationManager->AddToSurface (&mAnimationDust, "/animations/dust.xml", IND_ALPHA, IND_16, 255, 0, 255)) return 0;

	// ----- Set the surface and animations into 2d entities -----

	// Creating 2d entity for the background
	////////////////IND_Entity2d mDust;
	mI->Entity2dManager->Add (&mBack);							// Entity adding
	mBack.SetSurface (&mSurfaceBack);							// Set the surface into the entity

	// Character 1
	////////////////IND_Entity2d mDust;
	mI->Entity2dManager->Add (&mPlayer1);						// Entity adding
	mPlayer1.SetAnimation (&mAnimationCharacter1);				// Set the animation into the entity

	// Character 2
	////////////////IND_Entity2d mDust;
	mI->Entity2dManager->Add (&mPlayer2);						// Entity adding
	mPlayer2.SetAnimation (&mAnimationCharacter2);				// Set the animation into the entity

	// Dust explosion
	////////////////IND_Entity2d mDust;
	mI->Entity2dManager->Add (&mDust);							// Entity adding
	mDust.SetAnimation (&mAnimationDust);						// Set the animation into the entity

	// ----- Changing the attributes of the 2d entities -----

	// Player 1
	mPlayer1.SetSequence (0);									// Choose sequence
	mPlayer1.SetPosition (140, 200, 0);
	mPlayer1.SetMirrorX (1);									// Horizontal mirroring

	// Dust explosion
	mDust.SetPosition (360, 250, 0);

	// Player 2
	mPlayer2.SetSequence (0);									// Choose sequence
	mPlayer2.SetPosition (550, 200, 0);
	mPlayer2.SetNumReplays (3);									// The animation will be displayed 3 times

	// ----- Main Loop -----
//
	STX_CATCH;
	return 0;
	}
virtual void render()
{
	STX_TRY;
	/*
	
	LOG_PRINT("ww=%d\n",STX_Service::GetWindowInstance()->GetWidth());
	LOG_PRINT("wh=%d\n",STX_Service::GetWindowInstance()->GetHeight());
	LOG_PRINT("rw=%d\n",IRenderer::GetRendererInstance()->GetWidth());
	LOG_PRINT("rh=%d\n",IRenderer::GetRendererInstance()->GetHeight());
*/
		mI->Input->Update ();
		mI->Render->BeginScene ();
		mI->Entity2dManager->RenderEntities2d ();
		mI->Render->EndScene ();
//
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
	glp->init("a_04_IND_Animation");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

