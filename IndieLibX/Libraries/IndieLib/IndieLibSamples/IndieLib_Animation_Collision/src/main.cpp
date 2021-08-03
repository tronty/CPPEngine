#include <CIndieLib.h>

/*
==================
Main
==================
*/
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
	// ----- Camera -----
	int MiddleScreenX;// = mI->Window->GetWidth() / 2;
	int MiddleScreenY;// = mI->Window->GetHeight() / 2;
	int CamPosX;// = MiddleScreenX;


	bool bShowBoundsToggle = false;
	float fElapsedTime = 0.0f;
	float x = 0.0f;
	float y = 450.0f;

	float VelocityX = 0.0f;
	float VelocityY = 0.0f;
	float AccelX	= 0.0f;
	float MaxVelocityX = 300;


	float slideFriction = 100;
	int Dir = 0;	// stand 1=right, -1=left
	float fJumpForce = 750;
	float speed = 700;
	float gravity = 2000;
	float jumpAccel = 0;		// TODO - use Vector2d instead for walk/jump accel
	float walkDecel = 0;
	bool playerJumping = false;

class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Timer timer;
		IND_Surface mSurfaceBack;
		IND_Surface mSurfaceBeetle;
		IND_Animation AnimationCharacter;
		IND_Entity2d mBack;
		IND_Entity2d Player;
		IND_Entity2d Beetle;
		IND_Camera2d  Cameras2d;
		virtual int init(const char* aTitle)
		{
			STX_INIT_SEGVCATCH;
			STX_TRY;
			g_pIndieLib = mI = CIndieLib::Instance();
	// ----- IndieLib intialization -----

#if 0
	//CIndieLib * mI = CIndieLib::Instance();
	if (!mI->Init ()) return 0;
#else
	
//CIndieLib * mI = CIndieLib::Instance();
	if (!mI->
		Init (aTitle
		)) return -1;
#endif
	//IND_Entity2d Beetle

	// Loading Background
	//IND_Entity2d Beetle
	if (!mI->SurfaceManager->Add (&mSurfaceBack, "/IndieLib-Animation-Collision/cave.png", IND_ALPHA, IND_32))
		return 0;

	// Loading Beetleship
	//IND_Entity2d Beetle
	if (!mI->SurfaceManager->Add (&mSurfaceBeetle, "/IndieLib-Animation-Collision/beetleship.png", IND_ALPHA, IND_32))
		return 0;

	// ----- Animations loading -----

	// Characters animations
	//IND_Entity2d Beetle
	if (!mI->AnimationManager->AddToSurface (&AnimationCharacter, "/IndieLib-Animation-Collision/mario.xml", IND_ALPHA, IND_32))
		return 0;



	// ----- Set the surface and animations into 2d entities -----

	// Creating 2d entity for the background
	//IND_Entity2d Beetle
	mI->Entity2dManager->Add (&mBack);						// Entity adding (layer 0)
	mBack.SetSurface (&mSurfaceBack);						// Set the surface into the entity

	// Character 1
	//IND_Entity2d Beetle
	mI->Entity2dManager->Add (1, &Player);					// Entity adding (layer 1)
	Player.SetAnimation (&AnimationCharacter);				// Set the animation into the entity

	// Beetle
	//IND_Entity2d Beetle
	mI->Entity2dManager->Add (0, &Beetle);					// (layer 1)
	Beetle.SetSurface (&mSurfaceBeetle);					// Set the surface into the entity
	Beetle.SetPosition(500, 460, 0);
	Beetle.SetHotSpot (0.5f, 0.5f);
	Beetle.SetMirrorX (1);
	Beetle.SetBoundingTriangle ("beetle_head", 160, 105, 160, 170, 190, 135);
	Beetle.SetBoundingCircle ("beetle_boy_head", 85, 52, 55);



	MiddleScreenX = mI->Window->GetWidth() / 2;
	MiddleScreenY = mI->Window->GetHeight() / 2;
	CamPosX = MiddleScreenX;
	Cameras2d=IND_Camera2d(MiddleScreenX, MiddleScreenY);

	// ----- Main Loop -----

	timer.Start();



	// Init Player
	Player.SetSequence (0);			// standing
	Player.SetPosition (x , y, 0);
	Player.SetHotSpot (0.5f, 0.5f);
	Player.SetMirrorX (0);
	Player.SetBoundingRectangle("entire", 0, 0, 48, 48);
	STX_CATCH;
	return 0;

	}
virtual void render()
{
		STX_TRY;
		mI->Input->Update ();

		// ----- Delta Time -----
		fElapsedTime = mI->Render->GetFrameTime() / 1000.0f;

		// Toggle show bounding boxes
		if (mI->Input->OnKeyPress(STX_KEY_p))
			bShowBoundsToggle = !bShowBoundsToggle;

		// Walk Left!
		if (mI->Input->IsKeyPressed(STX_KEY_LEFT))
		{
			VelocityX -= speed * fElapsedTime;
			if (!playerJumping)
				Player.SetSequence(0);		//walk
			Player.SetMirrorX(1);
		}
		// Walk Right!
		else
		if (mI->Input->IsKeyPressed(STX_KEY_RIGHT))
		{
			VelocityX += speed * fElapsedTime;
			if (!playerJumping)
				Player.SetSequence(0);		//walk
			Player.SetMirrorX(0);

		}
		else
		{
			if (!playerJumping)
				Player.SetSequence(1);		// stand
			VelocityX = 0.0f;			// 0 = no slide
		}

		// JUMP!
		if(mI->Input->OnKeyPress(STX_KEY_SPACE) && !playerJumping)
		{
			VelocityY -= fJumpForce;	// jump force (minus is up)
			Player.SetSequence(2);		// jump
			playerJumping = true;
		}

		// TODO - if no key press then apply friction.  We want the player to move instantly
		// yet slide to a stop.

		VelocityY += gravity * fElapsedTime;	// apply gravity (value is gravity)
		y += VelocityY * fElapsedTime;			// position

		// Check falling limits (poor man collision detection!)
		if (y > 500)
		{
			VelocityY = 0;
			y = 500;
			playerJumping = false;
		}

		// Clamp walking velocity
		float Length = sqrtf(VelocityX * VelocityX);
		if ( Length > MaxVelocityX )
		{
			VelocityX *= ( MaxVelocityX / Length );
		} // End if clamp X velocity

		x += VelocityX * fElapsedTime;
		if (x < 0)
			x = 0;

		// Move the player
		Player.SetPosition(x, y, 0);

		// Apply slide friction
		float Dec = -Dir * slideFriction * fElapsedTime;

		if (mI->Entity2dManager->IsCollision (&Player, "/IndieLib-Animation-Collision/entire", &Beetle, "/IndieLib-Animation-Collision/beetle_boy_head"))
		{
			VelocityY = 80;	// force him downward
			VelocityX = 0;
		}

		if (x > mI->Window->GetWidth() / 2)
			Cameras2d.SetPosition((int)x, MiddleScreenY);


		mI->Render->BeginScene ();
		mI->Render->ClearViewPort (60, 60, 60);
		mI->Render->SetViewPort2d (0, 0, mI->Window->GetWidth(), mI->Window->GetHeight());
		mI->Render->SetCamera2d (&Cameras2d);

		for (int i = 0; i < 16; i++)
		{
			mI->Entity2dManager->RenderEntities2d ( i );
			if (bShowBoundsToggle)
				mI->Entity2dManager->RenderCollisionAreas (i, 255, 0, 0, 255);
		}
		//???mI->Render->ShowFpsInWindowTitle("");
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
	glp->init("IndieLib-Animation-Collision");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();


	return 0;
}

