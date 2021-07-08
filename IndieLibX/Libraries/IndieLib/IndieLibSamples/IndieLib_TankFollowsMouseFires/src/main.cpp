//-------------------------------------------------------------------------------------------
// Desc:	Demonstrates simple physics for entity movement.  Uses acceleration and deceleration.
//			Controls:
//			cursor keys - left/right
//			Added bonus is the mouse cursor is a surface.
//			Just allows shooting one bullet for simplicity.
//-------------------------------------------------------------------------------------------

#include <CIndieLib.h>
//include "AudioManager.h"

#define DIR_LEFT	0x1
#define DIR_RIGHT	0x2

//--------
// Globals
//--------
//CIndieLib * g_pIndieLib = 0;
D3DXFROMWINEVECTOR3 g_vecPlayer_Pos				= D3DXFROMWINEVECTOR3( 400.0f, 500.0f, 0.0f );
D3DXFROMWINEVECTOR3 g_vecPlayerShadow_Pos		= D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );
D3DXFROMWINEVECTOR3 g_vecPlayer_Right			= D3DXFROMWINEVECTOR3( 1.0f, 0.0f, 0.0f );
D3DXFROMWINEVECTOR3 g_vecPlayer_Velocity		= D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );
D3DXFROMWINEVECTOR3 g_vecBullet_Pos				= D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );
D3DXFROMWINEVECTOR3 g_vecBullet_Velocity		= D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );
bool		g_bBulletDead				= true;
float		g_fSpeed					= 1100.0f;
float		g_fMaxVelocityX				= 200.0f;
float		g_fFriction					= 950.0f;
float		g_Epsilon					= 0.05f;
float		g_fMuzzleFlashTimer			= 0.0f;
bool		g_bToggleFreezeTank			= false;
float		g_fGunAngle					= 0.0f;	// in degrees
D3DXFROMWINEVECTOR3 g_vecBarrel					= D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );
const float fPI							= 3.14159f;
const float BARREL_LENGTH				= 30.0f;
const float BULLET_SPEED				= 500.0f;
const float MAX_GUN_ANGLE				= 80.0;	// in degrees from 90 degrees vertical (straight up)

//-----------
// Prototypes
//-----------
void Player_Move(const D3DXFROMWINEVECTOR3& vecShift, bool Velocity);
void Player_Move(ULONG Direction, float Distance, bool Velocity);
void Player_Update( float fElapsedTime );
void Player_CollisionDetection();
void Bullet_Update(float fElapsedTime, IND_Entity2d* pBullet);


struct TSoundName
{
	std::string szName;
	std::string szFilename;
};
typedef TSoundName SoundName;

#define MAX_SOUNDS	5
#define SOUNDEXT ".wav"
SoundName g_SoundNames[MAX_SOUNDS] =
{
	{"tankfire", "/IndieLib-TankFollowsMouseFires/sounds/tankfire" SOUNDEXT},
	{"tankfire1", "/IndieLib-TankFollowsMouseFires/sounds/tankfire1" SOUNDEXT},
	{"tankfire2", "/IndieLib-TankFollowsMouseFires/sounds/tankfire2" SOUNDEXT},
	{"tankfire3", "/IndieLib-TankFollowsMouseFires/sounds/tankfire3" SOUNDEXT},
	{"tankfire4", "/IndieLib-TankFollowsMouseFires/sounds/tankfire4" SOUNDEXT},
};
//CIndieLib * mI = 0;
	float	fElapsedTime = 0.0f;
	char	szText [2048];
	char	szBuf  [128];
//-----------------
// Main
//-----------------
CIndieLib * mI = 0;
CIndieLib * g_pIndieLib = 0;
class GameLogic : public IGameLogic
{
	public:
		GameLogic():IGameLogic(){}
		virtual ~GameLogic(){}
		IND_Timer timer;
		IND_Surface surfaceSky;
		IND_Surface surfaceGnd;
		IND_Animation animTank;
		IND_Surface surfaceBarrel;
		IND_Surface surfaceCursor;
		IND_Surface surfaceBullet;
		IND_Surface surfaceTankShadow;
		IND_Surface surfaceMuzzleFlash;
		IND_Entity2d sky;
		IND_Entity2d ground;
		IND_Entity2d tank;
		IND_Entity2d barrel;
		IND_Entity2d bullet;
		IND_Entity2d cursor;
		IND_Entity2d tankShadow;
		IND_Entity2d muzzleFlash;
		IND_Font FontSmall;
		IND_Entity2d mTextSmallWhite;
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
	
	mI = g_pIndieLib = CIndieLib::Instance();
	if (!mI->
		Init (aTitle
		)) return -1;
#endif

	//////////////////////////////////////IND_Entity2d mTextSmallWhite;

	// Characters animations
	//////////////////////////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->SurfaceManager->Add(&surfaceSky, "/IndieLib-TankFollowsMouseFires/images/frigistan_sky.png", IND_OPAQUE, IND_32)) return 0;

	//////////////////////////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->SurfaceManager->Add(&surfaceGnd, "/IndieLib-TankFollowsMouseFires/images/frigistan_ground.png", IND_OPAQUE, IND_32)) return 0;

	//////////////////////////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->AnimationManager->AddToSurface (&animTank, "/IndieLib-TankFollowsMouseFires/images/tank.xml", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->SurfaceManager->Add(&surfaceBarrel, "/IndieLib-TankFollowsMouseFires/images/barrel.png", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->SurfaceManager->Add(&surfaceCursor, "/IndieLib-TankFollowsMouseFires/images/crosshairs1.png", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->SurfaceManager->Add(&surfaceBullet, "/IndieLib-TankFollowsMouseFires/images/bullet.png", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->SurfaceManager->Add(&surfaceTankShadow, "/IndieLib-TankFollowsMouseFires/images/tankshadow.png", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////IND_Entity2d mTextSmallWhite;
	if (!mI->SurfaceManager->Add(&surfaceMuzzleFlash, "/IndieLib-TankFollowsMouseFires/images/muzzleflash.png", IND_ALPHA, IND_32)) return 0;


	//////////////////////////////////////IND_Entity2d mTextSmallWhite;
	mI->Entity2dManager->Add(&sky);
	sky.SetSurface(&surfaceSky);

	//////////////////////////////////////IND_Entity2d mTextSmallWhite
	mI->Entity2dManager->Add(&ground);
	ground.SetSurface(&surfaceGnd);
	ground.SetPosition(0, mI->Window->GetHeight() - surfaceGnd.GetHeight(), 0);

	//////////////////////////////////////IND_Entity2d mTextSmallWhite
	mI->Entity2dManager->Add(&tank);
	tank.SetAnimation(&animTank);
	tank.SetHotSpot(0.5f, 0.1f);
	tank.SetPosition(g_vecPlayer_Pos.x, g_vecPlayer_Pos.y, 0);  // initial position

	//////////////////////////////////////IND_Entity2d mTextSmallWhite
	mI->Entity2dManager->Add(&barrel);
	barrel.SetSurface(&surfaceBarrel);
	barrel.SetHotSpot(0.5f, 1.0f);
	barrel.SetScale(1.5f, 1.5f);

	//////////////////////////////////////IND_Entity2d mTextSmallWhite
	mI->Entity2dManager->Add(&bullet);
	bullet.SetSurface(&surfaceBullet);
	bullet.SetHotSpot(0.5f, 0.5f);
	bullet.SetShow(false);

	//////////////////////////////////////IND_Entity2d mTextSmallWhite
	mI->Entity2dManager->Add(1, &cursor);		// GUI layer
	cursor.SetSurface(&surfaceCursor);
	cursor.SetHotSpot(0.5f, 0.5f);

	//////////////////////////////////////IND_Entity2d mTextSmallWhite
	mI->Entity2dManager->Add(0, &tankShadow);
	tankShadow.SetSurface(&surfaceTankShadow);
	tankShadow.SetHotSpot(0.5f, 0.7f);

	//////////////////////////////////////IND_Entity2d mTextSmallWhite
	mI->Entity2dManager->Add(0, &muzzleFlash);
	muzzleFlash.SetSurface(&surfaceMuzzleFlash);
	muzzleFlash.SetHotSpot(0.5f, 0.5f);
	muzzleFlash.SetShow(false);


	// Font
	//////////////////////////////////////IND_Entity2d mTextSmallWhite
	if (!mI->FontManager->Add (&FontSmall, "/IndieLib-TankFollowsMouseFires/font_small.png", "/IndieLib-TankFollowsMouseFires/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Font creation -----

	//////////////////////////////////////IND_Entity2d mTextSmallWhite
	mI->Entity2dManager->Add(1, &mTextSmallWhite);		// Entity adding (Layer 1)
	mTextSmallWhite.SetFont			(&FontSmall);						// Set the font into the entity
	mTextSmallWhite.SetLineSpacing	(18);
	mTextSmallWhite.SetCharSpacing	(-8);
	mTextSmallWhite.SetPosition		(5, 5, 1);
	mTextSmallWhite.SetAlign		(IND_LEFT);


	timer.Start();



	// global audio manager pointer variable automatically set
	//IAudio* pAudioManager = STX_Service::GetAudioInstance()
	STX_Service::GetAudioInstance()->Init();

#if 1
	//------------------------------
	// Load all sounds
	//------------------------------
	for (int i = 0; i < MAX_SOUNDS; i++)
	{
		STX_Service::GetAudioInstance()->Load(g_SoundNames[i].szFilename.c_str(), g_SoundNames[i].szName.c_str());
	}
#endif
	STX_CATCH;
	return 0;


	// ----- Main Loop -----
	}
virtual void render()
{
	STX_TRY;
		mI->Input->Update ();

		// ----- Delta Time -----
		fElapsedTime = mI->Render->GetFrameTime() / 1000.0f;

			// ----- Text -----
		stx_strlcpy (szText, "Move the mouse left and right and watch the tank try to follow!\n", 2048);
		stx_strlcat (szText, "Press Left Mouse button to fire!\n", 2048);
		stx_strlcat (szText, "Press 'f' to toggle freezing the tank movement (debug)\n", 2048);
		stx_snprintf(szBuf, 2048, "%d, %d\n", mI->Input->GetMouseX(), mI->Input->GetMouseY() );
		stx_strlcat (szText, szBuf, 2048);
		stx_snprintf(szBuf, 2048, "%.3f, %.3f\n", g_vecPlayer_Pos.x, g_vecPlayer_Pos.y);
		stx_strlcat (szText, szBuf, 2048);
		stx_snprintf(szBuf, 2048, "Muzzle angle %.3f\n", g_fGunAngle);
		stx_strlcat (szText, szBuf, 2048);
		stx_snprintf(szBuf, 2048, "Tank velocity %.3f\n", g_vecPlayer_Velocity.x);
		stx_strlcat (szText, szBuf, 2048);
		mTextSmallWhite.SetText	 (szText);

		// Position the mouse cursor
		cursor.SetPosition((float)mI->Input->GetMouseX(), (float)mI->Input->GetMouseY(), 1);

		// "Freeze" tank except allow gun to rotate
		if (mI->Input->OnKeyPress(STX_KEY_f))
			g_bToggleFreezeTank = !g_bToggleFreezeTank;


		ULONG        Direction = 0;

		float deltaX = g_vecPlayer_Pos.x - mI->Input->GetMouseX();

		if (!g_bToggleFreezeTank && fabs(deltaX) > g_Epsilon)
		{
			if (deltaX < 0)
			{
				Direction |= DIR_RIGHT;
				tank.SetSequence(1);
			}
			else
			if (deltaX > 0)
			{
				Direction |= DIR_LEFT;
				tank.SetSequence(2);
			}
		}
		else
		{
			// Stop the animation if not moving
			tank.SetSequence(0);
		}

		// Any movement?
		if ( Direction && !g_bToggleFreezeTank )
		{
			Player_Move( Direction, g_fSpeed * fElapsedTime, true );
		}

		//---------------------------
		// Update the player
		//---------------------------
		Player_Update( fElapsedTime );

		//---------------------------
		// Move and rotate the barrel
		// Note: there would normally be a class representing the whole tank
		//---------------------------
		float fMouse2TankX = tank.GetPosX() - mI->Input->GetMouseX();
		float fMouse2TankY = tank.GetPosY() - mI->Input->GetMouseY();	// y position going down

		// Clamp. Mouse is below tank
		if (fMouse2TankY < 0.0f)
			fMouse2TankY = 0.0f;
		g_fGunAngle = atan2(fMouse2TankY, fMouse2TankX) * 180.0f / fPI;			// in degrees
		g_fGunAngle -= 90.0f;	// barrel is initially pointing up (in image).  atan2() w.r.t x-axis (pointing right)
		barrel.SetAngleXYZ(0.0f, 0.0f, g_fGunAngle);

		// Set position of barrel (child) on the tank (parent)
		g_vecBarrel.x = tank.GetPosX();
		g_vecBarrel.y = tank.GetPosY();
		barrel.SetPosition(g_vecBarrel.x, g_vecBarrel.y, 0);

		// Move the tank (player)
		tank.SetPosition(g_vecPlayer_Pos.x, g_vecPlayer_Pos.y, 1);
		tankShadow.SetPosition(tank.GetPosX(), tank.GetPosY() + 50, 0);	// location is tank HotSpot plus an offset of 50


		//---------------------------
		// Fire weapon!!
		//---------------------------
		if (5 && mI->Input->OnMouseButtonPress(STX_MBUTTON_LEFT))
		{
			// Initial bullet position (following code is rather verbose and could be combined into less lines
			// but did it that way to explain the math.
			float angleDegrees = barrel.GetAngleZ() + 90.0f;
			float angleRadians = angleDegrees * fPI / 180.0f;
			float fX = barrel.GetPosX() - cosf(angleRadians) * BARREL_LENGTH;
			float fY = barrel.GetPosY() - sinf(angleRadians) * BARREL_LENGTH;
			bullet.SetAngleXYZ(0, 0, angleDegrees - 90.0f);
			g_vecBullet_Pos = D3DXFROMWINEVECTOR3(fX, fY, 0.0f);
			bullet.SetPosition(g_vecBullet_Pos.x, g_vecBullet_Pos.y, 0);

			// Initial bullet velocity
			D3DXFROMWINEMATRIX mtxRotate;
			D3DXFROMWINEVECTOR3 vecVelDir = D3DXFROMWINEVECTOR3(-1.0f, 0.0f, 0.0f);
			D3DXFROMWINEMatrixRotationZ( &mtxRotate, angleRadians);
			D3DXFROMWINEVec3TransformNormal( &vecVelDir, &vecVelDir, &mtxRotate );
			g_vecBullet_Velocity = vecVelDir * BULLET_SPEED;

			// Show blast!
			g_fMuzzleFlashTimer = 0.05f;
			muzzleFlash.SetPosition(g_vecBullet_Pos.x, g_vecBullet_Pos.y, 0);
			g_bBulletDead = false;

			// Make sound! (no sound lib hooked up but this is where you would do it.
			STX_Service::GetAudioInstance()->Play("tankfire1");
		}

		// Update the single bullet
		if (!g_bBulletDead)
		{
			bullet.SetShow(true);
			Bullet_Update( fElapsedTime, &bullet );
			bullet.SetPosition(g_vecBullet_Pos.x, g_vecBullet_Pos.y, 0);
		}
		else
		{
			bullet.SetShow(false);
		}

		//---------------------------
		// Render muzzle flash effect
		//---------------------------
		g_fMuzzleFlashTimer -= fElapsedTime;
		if (g_fMuzzleFlashTimer > 0.0f)
		{
			muzzleFlash.SetShow(true);
		}
		else
		{
			muzzleFlash.SetShow(false);
			g_fMuzzleFlashTimer = 0.0f;
		}


		//-----------------
		// Render the scene
		//-----------------
		mI->Render->BeginScene ();

		mI->Render->ClearViewPort (160, 160, 160);

		mI->Render->SetViewPort2d (0, 0, mI->Window->GetWidth(), mI->Window->GetHeight());

		mI->Entity2dManager->RenderEntities2d ( );

		// Render GUI layer
		mI->Entity2dManager->RenderEntities2d (1);
		//???mI->Render->ShowFpsInWindowTitle();

		// MUST call this to update FMOD
		STX_Service::GetAudioInstance()->Update();

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
	glp->init("IndieLib_TankFollowsMouseFires");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();
	// ----- Free -----
	//STX_Service::GetAudioInstance()->DeleteInstance();

	return 0;
}
void Player_Move(ULONG Direction, float Distance, bool Velocity)
{
	D3DXFROMWINEVECTOR3 vecShift = D3DXFROMWINEVECTOR3(0, 0, 0);

	// Which direction are we moving?
	if ( Direction & DIR_RIGHT )
		vecShift += g_vecPlayer_Right * Distance;
	if ( Direction & DIR_LEFT )
		vecShift -= g_vecPlayer_Right * Distance;

	if ( Direction )
		Player_Move ( vecShift, Velocity );
}

void Player_Move(const D3DXFROMWINEVECTOR3& vecShift, bool Velocity)
{
	if ( Velocity )
	{
		g_vecPlayer_Velocity += vecShift;
	}
	else
	{
		g_vecPlayer_Pos += vecShift;
	}
}

void Player_Update( float fElapsedTime )
{

    // Clamp the velocity to our max velocity vector
    if ( fabs(g_vecPlayer_Velocity.x) > g_fMaxVelocityX )
	{
		if (g_vecPlayer_Velocity.x > 0)
			g_vecPlayer_Velocity.x = g_fMaxVelocityX;
		else
			g_vecPlayer_Velocity.x = -g_fMaxVelocityX;
	}

	// Move the player
	Player_Move (g_vecPlayer_Velocity * fElapsedTime, false );

	// Apply collision detection.
	Player_CollisionDetection();


	//--------------
	// Deceleration!
	//--------------
	// Calculate the reverse of the velocity direction
    D3DXFROMWINEVECTOR3 vecDec = -g_vecPlayer_Velocity;
    D3DXFROMWINEVec3Normalize( &vecDec, &vecDec );

    // Retrieve the actual velocity length
    float Length = D3DXFROMWINEVec3Length( &g_vecPlayer_Velocity );

    // Calculate total deceleration based on friction values
    float Dec = (g_fFriction * fElapsedTime);
    if ( Dec > Length ) Dec = Length;

    // Apply the friction force
    g_vecPlayer_Velocity += vecDec * Dec;
}

void Player_CollisionDetection()
{
	if (g_vecPlayer_Pos.x < 0.0f)
		g_vecPlayer_Pos.x = 0.0f;
	else
	if (g_vecPlayer_Pos.x > mI->Window->GetWidth())
		g_vecPlayer_Pos.x = (float)mI->Window->GetWidth();

}

void Bullet_Update( float fElapsedTime, IND_Entity2d* pBullet )
{
 	// Move the bullet
	g_vecBullet_Pos += g_vecBullet_Velocity * fElapsedTime;

	// Collision
	if (pBullet)
	{
		if (pBullet->GetPosX() > mI->Window->GetWidth() ||
			pBullet->GetPosX() < 0 ||
			pBullet->GetPosY() > mI->Window->GetHeight() ||
			pBullet->GetPosY() < 0 )
		{
			g_bBulletDead = true;
		}
	}
}

