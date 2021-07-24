// Desc:	Demonstrates simple physics for entity movement.  Uses acceleration and deceleration.
//
//			Controls:
//			mouse - tank movement and aiming
//			mouse left button - fire weapon
//			advancing enemies (fixed number)
//			explosions
//-------------------------------------------------------------------------------------------

#include <CIndieLib.h>
#include <time.h>
//include "AudioManager.h"
#include "Bullet.h"
#include "PropFighter.h"
#include "explosion.h"

#include <vector>

#define DIR_LEFT	0x1
#define DIR_RIGHT	0x2
#define MAX_LAYERS  3

//---------------
// Prototypes
//---------------
void Player_Move				( const D3DXFROMWINEVECTOR3& vecShift, bool Velocity );
void Player_Move				( ULONG Direction, float Distance, bool Velocity );
void Player_Update				( float fElapsedTime );
void Player_CollisionDetection	( );
void Update_Bullets				( float fElapsedTime );
void Bury_Bullets				( );
void Spawn_PropFighter			( );
void Update_PropFighters		( float fElapsedTime );
void Bury_PropFighters			( );
//void Free_Memory				( );
void Update_Explosions			( float fElapsedTime );
void Bury_Explosions			( );

struct TSoundName
{
	std::string szName;
	std::string szFilename;
};
typedef TSoundName SoundName;

#define MAX_SOUNDS 6
#define SOUNDEXT ".wav"
SoundName g_SoundNames[MAX_SOUNDS] =
{
	{"tankfire",	"/IndieLib-TankEnemies/sounds/tankfire" SOUNDEXT},
	{"tankfire1",	"/IndieLib-TankEnemies/sounds/tankfire1" SOUNDEXT},
	{"tankfire2",	"/IndieLib-TankEnemies/sounds/tankfire2" SOUNDEXT},
	{"tankfire3",	"/IndieLib-TankEnemies/sounds/cached_missile" SOUNDEXT},
	{"bigexplode",	"/IndieLib-TankEnemies/sounds/bigexplode" SOUNDEXT},
	{"bullethit",	"/IndieLib-TankEnemies/sounds/bullethit" SOUNDEXT}
	//,{"AtomicTank",  "music/LoveTheme" SOUNDEXT}
};

//-----------
// Globals
//-----------
//CIndieLib * g_pIndieLib = 0;
//IAudio * g_pAudioManager			= 0;
D3DXFROMWINEVECTOR3 g_vecPlayer_Pos				= D3DXFROMWINEVECTOR3( 400.0f, 500.0f, 0.0f );
D3DXFROMWINEVECTOR3 g_vecPlayerShadow_Pos		= D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );
D3DXFROMWINEVECTOR3 g_vecPlayer_Right			= D3DXFROMWINEVECTOR3( 1.0f, 0.0f, 0.0f );
D3DXFROMWINEVECTOR3 g_vecPlayer_Velocity		= D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );

D3DXFROMWINEVECTOR3 g_vecPropFighter_Pos		= D3DXFROMWINEVECTOR3( 400.0f, 50.0f, 0.0f );
D3DXFROMWINEVECTOR3 g_vecPropFighter_Velocity	= D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );

float		g_fSpeed					= 1100.0f;
float		g_fMaxVelocityX				= 180.0f;
float		g_fFriction					= 950.0f;
float		g_Epsilon					= 0.05f;
float		g_fMuzzleFlashTimer			= 0.0f;
bool		g_bToggleFreezeTank			= false;
float		g_fGunAngle					= 0.0f;	// in degrees
D3DXFROMWINEVECTOR3 g_vecBarrel					= D3DXFROMWINEVECTOR3( 0.0f, 0.0f, 0.0f );
const float fPI							= 3.14159f;
const float BARREL_LENGTH				= 30.0f;
const float BULLET_SPEED				= 500.0f;
const float MAX_GUN_ANGLE				= 80.0;		// in degrees from 90 degrees vertical (straight up)
const float FIRE_REPEAT_DELAY			= 0.15f;	// seconds until another bullet is allowed to be fired.
const float BIG_FIRE_REPEAT_DELAY		= 0.25f;	// seconds until another bullet is allowed to be fired.
const float SMALL_FIRE_REPEAT_DELAY		= 0.10f;	// seconds until another bullet is allowed to be fired.
const float MISL_FIRE_REPEAT_DELAY		= 0.8f;		// seconds until another bullet is allowed to be fired.
float		g_FireDelayTimer			= 0.0f;
float		g_FireDelayTime				= 0.0f;
IND_Surface*	g_BulletSurface			= 0;
IND_Animation*	g_pAnimExplosion		= 0 ;
string			g_szBulletSound;
int				g_nCurrRound				= 0;
float			g_PropFighterSpawnTimer		= 0.0f;
const float		SPAWN_PROPFIGHTER_TIME		= 1.5f;

std::vector < Bullet* >	g_Bullets;				// array of bullets
std::vector < PropFighter* > g_PropFighters;	// array of enemies
//////////////////////////////////////////////////

std::vector < Explosion* >	 g_Explosions;

std::string g_szAnimationFile = "/IndieLib-TankEnemies/images/explosion.xml";

//CIndieLib * mI = 0;
//----------------
	// Local Variables
	//----------------
	float	fElapsedTime = 0.0f;
	char	szText [2048];
	char	szBuf  [128];
	bool	bToggleShowBounds = false;
//-----------------
// Main - is very Fat!
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
		IND_Surface surfaceBG;
		IND_Animation animTank;
		IND_Animation animExplosion;
		IND_Surface surfaceTankTracks;
		IND_Surface surfaceBarrel;
		IND_Surface surfaceCursor;
		IND_Surface surfaceBullet;
		IND_Surface surfaceBulletBig;
		IND_Surface surfaceBulletSmall;
		IND_Surface surfaceBulletMissile;
		IND_Surface surfaceTankShadow;
		IND_Surface surfaceMuzzleFlash;
		IND_Entity2d sky;
		IND_Entity2d ground;
		IND_Entity2d BG;
		IND_Entity2d tank;
		IND_Entity2d tankTracks;
		IND_Entity2d barrel;
		IND_Entity2d cursor;
		IND_Entity2d tankShadow;
		IND_Entity2d muzzleFlash;
		IND_Font FontSmall;
		IND_Entity2d mTextSmallWhite;
		IND_Animation g_AnimPropFighter;// HACK just for access!
		virtual int init(const char* aTitle)
		{
			STX_INIT_SEGVCATCH;
			STX_TRY;
			g_pIndieLib = mI = CIndieLib::Instance();
	// ----- IndieLib intialization -----

#if 0
	//CIndieLib * mI = mI;
	if (!mI->Init ()) return 0;
#else
	

	if (!mI->Init (aTitle)) return -1;
#endif
	//////////////////////////////////////////////////

	// global audio manager pointer variable automatically set
	//g_pAudioManager = AudioManager::Instance();
	STX_Service::GetAudioInstance()->Init();

	//------------------------------
	// Load all sounds
	//------------------------------
	for (int i = 0; i < MAX_SOUNDS; i++)
	{
		STX_Service::GetAudioInstance()->Load(g_SoundNames[i].szFilename.c_str(), g_SoundNames[i].szName.c_str());
	}

	// Characters animations
	//////////////////////////////////////////////////
	if (!mI->SurfaceManager->Add(&surfaceSky, "/IndieLib-TankEnemies/images/frigistan_sky.jpg", IND_OPAQUE, IND_32)) return 0;

	//////////////////////////////////////////////////
	if (!mI->SurfaceManager->Add(&surfaceGnd, "/IndieLib-TankEnemies/images/frigistan_ground.jpg", IND_OPAQUE, IND_32)) return 0;

	//////////////////////////////////////////////////
	if (!mI->SurfaceManager->Add(&surfaceBG, "/IndieLib-TankEnemies/images/frigistan_bg.png", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////////////////
	if (!mI->AnimationManager->AddToSurface (&animTank, "/IndieLib-TankEnemies/images/tank.xml", IND_ALPHA, IND_32)) return 0;
	//if (!mI->AnimationManager->AddToSurface (&animTank, "/IndieLib-TankEnemiesParallax/images/tank.xml", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////////////////
	if (!mI->AnimationManager->AddToSurface (&animExplosion, "/IndieLib-TankEnemies/images/explosion.xml", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////////////////
	if (!mI->SurfaceManager->Add(&surfaceTankTracks, "/IndieLib-TankEnemies/images/tracks.png", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////////////////
	if (!mI->SurfaceManager->Add(&surfaceBarrel, "/IndieLib-TankEnemies/images/barrel.png", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////////////////
	if (!mI->SurfaceManager->Add(&surfaceCursor, "/IndieLib-TankEnemies/images/crosshairs1.png", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////////////////
	if (!mI->SurfaceManager->Add(&surfaceBullet, "/IndieLib-TankEnemies/images/bullet.png", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////////////////
	if (!mI->SurfaceManager->Add(&surfaceBulletBig, "/IndieLib-TankEnemies/images/bullet_big.png", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////////////////
	if (!mI->SurfaceManager->Add(&surfaceBulletSmall, "/IndieLib-TankEnemies/images/bullet_small.png", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////////////////
	if (!mI->SurfaceManager->Add(&surfaceBulletMissile, "/IndieLib-TankEnemies/images/missile.png", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////////////////
	if (!mI->SurfaceManager->Add(&surfaceTankShadow, "/IndieLib-TankEnemies/images/tankshadow.png", IND_ALPHA, IND_32)) return 0;

	//////////////////////////////////////////////////
	if (!mI->SurfaceManager->Add(&surfaceMuzzleFlash, "/IndieLib-TankEnemies/images/muzzleflash.png", IND_ALPHA, IND_32)) return 0;

	if (!mI->AnimationManager->AddToSurface (&g_AnimPropFighter, "/IndieLib-TankEnemies/images/propfighter.xml", IND_ALPHA, IND_32)) 
		return 0;


	//////////////////////////////////////////////////
	mI->Entity2dManager->Add(&sky);
	sky.SetSurface(&surfaceSky);

	//////////////////////////////////////////////////
	mI->Entity2dManager->Add(&ground);
	ground.SetSurface(&surfaceGnd);
	ground.SetPosition(0, (float)(mI->Window->GetHeight() - surfaceGnd.GetHeight()), 0);

	//////////////////////////////////////////////////
	mI->Entity2dManager->Add(&BG);
	BG.SetSurface(&surfaceBG);
	BG.SetPosition(0, (float)mI->Window->GetHeight()- surfaceGnd.GetHeight() - surfaceBG.GetHeight(), 0);

	//////////////////////////////////////////////////
	mI->Entity2dManager->Add(&tank);
	tank.SetAnimation(&animTank);
	tank.SetHotSpot(0.5f, 0.1f);
	tank.SetPosition(g_vecPlayer_Pos.x, g_vecPlayer_Pos.y, 0);  // initial position

	//////////////////////////////////////////////////
	mI->Entity2dManager->Add(&tankTracks);
	tankTracks.SetSurface(&surfaceTankTracks);
	tankTracks.SetPosition(0, 540, 0);
	tankTracks.SetHotSpot(0.0f, 0.5f);

	//////////////////////////////////////////////////
	mI->Entity2dManager->Add(&barrel);
	barrel.SetSurface(&surfaceBarrel);
	barrel.SetHotSpot(0.5f, 1.0f);
	barrel.SetScale(1.5f, 1.5f);

	//////////////////////////////////////////////////
	mI->Entity2dManager->Add(1, &cursor);		// GUI layer
	cursor.SetSurface(&surfaceCursor);
	cursor.SetHotSpot(0.5f, 0.5f);

	//////////////////////////////////////////////////
	mI->Entity2dManager->Add(0, &tankShadow);
	tankShadow.SetSurface(&surfaceTankShadow);
	tankShadow.SetHotSpot(0.5f, 0.7f);

	//////////////////////////////////////////////////
	mI->Entity2dManager->Add(0, &muzzleFlash);
	muzzleFlash.SetSurface(&surfaceMuzzleFlash);
	muzzleFlash.SetHotSpot(0.5f, 0.5f);
	muzzleFlash.SetShow(false);


	// Font
	//////////////////////////////////////////////////
	if (!mI->FontManager->Add (&FontSmall, "/IndieLib-TankEnemies/font_small.png", "/IndieLib-TankEnemies/font_small.xml", IND_ALPHA, IND_32)) return 0;

	// ----- Font creation -----

	//////////////////////////////////////////////////
	mI->Entity2dManager->Add(1, &mTextSmallWhite);		// Entity adding (Layer 1)
	mTextSmallWhite.SetFont			(&FontSmall);				// Set the font into the entity
	mTextSmallWhite.SetLineSpacing	(18);
	mTextSmallWhite.SetCharSpacing	(-8);
	mTextSmallWhite.SetPosition		(5, 5, 1);
	mTextSmallWhite.SetAlign		(IND_LEFT);

	//-----------
	// Initialize
	//-----------
	g_BulletSurface = &surfaceBullet;
	g_pAnimExplosion = &animExplosion;
	g_FireDelayTime = FIRE_REPEAT_DELAY;
	g_szBulletSound = g_SoundNames[0].szName;
	g_nCurrRound = 1;
	g_PropFighterSpawnTimer = 2.0f;

	//STX_Service::GetAudioInstance()->Play("AtomicTank");

	stx_srand((unsigned int)time(0));



	timer.Start();
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
		stx_strlcpy (szText, "Aim and move tank with the mouse.\n", 2048);
		stx_strlcat (szText, "Press Left Mouse button to fire!\n", 2048);
		stx_strlcat (szText, "Press 'f' to toggle freezing the tank movement (debug)\n", 2048);
		stx_strlcat (szText, "Press 1 = Med Round\nPress 2 = Small Round\nPress 3 = Big Round\nPress 4 = Missile\n", 2048);
		stx_snprintf(szBuf, 2048, "Round Type: %d\n", g_nCurrRound);
		stx_strlcat (szText, szBuf, 2048);
		stx_snprintf(szBuf, 2048, "Muzzle angle %.3f\n", g_fGunAngle);
		stx_strlcat (szText, szBuf, 2048);
		stx_snprintf(szBuf, 2048, "Live Rounds: %d\n", g_Bullets.size());
		stx_strlcat (szText, szBuf, 2048);
		if (g_bToggleFreezeTank)
		{
			stx_strlcat (szText, "Tank is frozen!\n", 2048);
		}
		stx_snprintf(szBuf, 2048, "mouse %d, %d\n", mI->Input->GetMouseX(), mI->Input->GetMouseY());
		stx_strlcat (szText, szBuf, 2048);
		stx_snprintf(szBuf, 2048, "tank %.3f, %.3f\n", g_vecPlayer_Pos.x, g_vecPlayer_Pos.y);
		stx_strlcat (szText, szBuf, 2048);
		mTextSmallWhite.SetText	 (szText);

		// Position the mouse cursor
		cursor.SetPosition((float)mI->Input->GetMouseX(), (float)mI->Input->GetMouseY(), 1);

		// "Freeze" tank except allow gun to rotate
		if (mI->Input->OnKeyPress(STX_KEY_f))
			g_bToggleFreezeTank = !g_bToggleFreezeTank;


		ULONG Direction = 0;

		// Is mouse inside the tank bounds?
		bool inside = false;
		if ( mI->Input->GetMouseY() > (g_vecPlayer_Pos.y - 32)	&&
			 mI->Input->GetMouseX() < (g_vecPlayer_Pos.x + 36) &&
			 mI->Input->GetMouseX() > (g_vecPlayer_Pos.x - 36)	&&
			 mI->Input->GetMouseY() < (g_vecPlayer_Pos.y + 55))
		{
			inside = true;
		}

		// Delta in x axis between mouse cursor hotspot and tank hotspot.
		// Movement allowed if mouse is OUTSIDE of its rectangular bounding box .
		float deltaX = g_vecPlayer_Pos.x - mI->Input->GetMouseX();
		if (!g_bToggleFreezeTank &&			// tank is not frozen
			(fabs(deltaX) > g_Epsilon) &&	// delta is large enough to move
			!inside)
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
		// Barrel hotspot position matches tank hotspot position
		g_vecBarrel.x = tank.GetPosX();
		g_vecBarrel.y = tank.GetPosY();
		barrel.SetPosition(g_vecBarrel.x, g_vecBarrel.y, 0);

		// Move the tank (player)
		tank.SetPosition(g_vecPlayer_Pos.x, g_vecPlayer_Pos.y, 1);
		tankShadow.SetPosition(tank.GetPosX(), tank.GetPosY() + 50, 0);	// location is tank HotSpot plus an offset of 50


		//---------------------------
		// Fire weapon!!
		//---------------------------
		if (mI->Input->IsMouseButtonPressed(STX_MBUTTON_LEFT) && g_FireDelayTimer == 0.0f)
		{
			// Initial bullet position (following code is rather verbose and could be combined into less lines
			// but did it that way to explain the math.
			float angleDegrees = barrel.GetAngleZ() + 90.0f;
			float angleRadians = angleDegrees * fPI / 180.0f;
			float fX = barrel.GetPosX() - cosf(angleRadians) * BARREL_LENGTH;
			float fY = barrel.GetPosY() - sinf(angleRadians) * BARREL_LENGTH;
			float angle = angleDegrees - 90.0f;
			D3DXFROMWINEVECTOR3 Pos = D3DXFROMWINEVECTOR3(fX, fY, 0.0f);

			// Initial bullet velocity
			D3DXFROMWINEMATRIX mtxRotate;
			D3DXFROMWINEVECTOR3 vecVelDir = D3DXFROMWINEVECTOR3(-1.0f, 0.0f, 0.0f);
			D3DXFROMWINEMatrixRotationZ( &mtxRotate, angleRadians);
			D3DXFROMWINEVec3TransformNormal( &vecVelDir, &vecVelDir, &mtxRotate );
			D3DXFROMWINEVECTOR3 Vel = vecVelDir * BULLET_SPEED;

			Bullet* pBullet = new Bullet(g_BulletSurface, Pos, Vel, angle);

			// Show blast!
			g_fMuzzleFlashTimer = 0.05f;
			muzzleFlash.SetPosition(Pos.x, Pos.y, 0);

			// Add new bullet to vector
			g_Bullets.push_back(pBullet);

			// Make sound! (no sound lib hooked up but this is where you would do it.
			STX_Service::GetAudioInstance()->Play(g_szBulletSound.c_str());

			g_FireDelayTimer = g_FireDelayTime;
		}

		// Update repeat timer.  When 0, its ok to fire again.
		if (g_FireDelayTimer > 0.0f)
		{
			g_FireDelayTimer -= fElapsedTime;
		}
		else
		{
			g_FireDelayTimer = 0.0f;
		}

		// Update the single bullet
		Update_Bullets( fElapsedTime );

		// "Bury" any dead bullets!  Get rid of 'em!
		Bury_Bullets();


		//---------------------------
		// Render muzzle flash
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

		//--------------------
		// Change bullet types
		//--------------------
		if (mI->Input->OnKeyPress(STX_KEY_1))
		{
			g_FireDelayTime = FIRE_REPEAT_DELAY;
			g_BulletSurface = &surfaceBullet;
			g_szBulletSound = g_SoundNames[0].szName;
			g_nCurrRound = 1;
		}
		else
		if (mI->Input->OnKeyPress(STX_KEY_2))
		{
			g_BulletSurface = &surfaceBulletSmall;
			g_FireDelayTime = SMALL_FIRE_REPEAT_DELAY;
			g_szBulletSound = g_SoundNames[1].szName;
			g_nCurrRound = 2;
		}
		else
		if (mI->Input->OnKeyPress(STX_KEY_3))
		{
			g_BulletSurface = &surfaceBulletBig;
			g_FireDelayTime = BIG_FIRE_REPEAT_DELAY;
			g_szBulletSound = g_SoundNames[2].szName;
			g_nCurrRound = 3;
		}
		else
		if (mI->Input->OnKeyPress(STX_KEY_4))
		{
			g_BulletSurface = &surfaceBulletMissile;
			g_FireDelayTime = MISL_FIRE_REPEAT_DELAY;
			g_szBulletSound = g_SoundNames[3].szName;
			g_nCurrRound = 4;
		}

		// Toggle show bounds
		if (mI->Input->OnKeyPress(STX_KEY_b))
			bToggleShowBounds = !bToggleShowBounds;

		// Toggle fullscreen
		if (mI->Input->OnKeyPress(STX_KEY_F1))
		{
			mI->Render->ToggleFullScreen();
		}

		g_PropFighterSpawnTimer -= fElapsedTime;
		if (g_PropFighterSpawnTimer < 0.0f)
		{

			Spawn_PropFighter();

			g_PropFighterSpawnTimer = SPAWN_PROPFIGHTER_TIME;

		}

		Update_PropFighters ( fElapsedTime );
		Bury_PropFighters	( );

		Update_Explosions	( fElapsedTime );
		Bury_Explosions		( );

		//-----------------
		// Render the scene
		//-----------------
		mI->Render->BeginScene ();
		mI->Render->ClearViewPort (160, 160, 160);
		mI->Render->SetViewPort2d (0, 0, mI->Window->GetWidth(), mI->Window->GetHeight());

		mI->Entity2dManager->RenderEntities2d ( );

		// Render GUI layer
		//???mI->Render->ShowFpsInWindowTitle();

		for (int i = 0; i < MAX_LAYERS; i++)
		{
			mI->Entity2dManager->RenderEntities2d (i);

			if (bToggleShowBounds)
				mI->Entity2dManager->RenderCollisionAreas (i, 255, 0, 0, 255);

		}

		// MUST call this to update FMOD
		STX_Service::GetAudioInstance()->Update();

		mI->Render->EndScene ();
		STX_CATCH;
	}
	};
GameLogic gl;
IGameLogic* glp=&gl;
#if 0//defined(ANDROID) || defined(OS_IPHONE) || defined(IPHONE_SIMULATOR)
static int init_thread(void *arg)
{
	glp->init();
}
static int render_thread(void *arg)
{
	glp->render();
}
int init(const char* aTitle){return glp->init(aTitle);}
void render(){return glp->render();}
int IndieLib()
{
	SDL_Thread * init_tid = SDL_CreateThread(init_thread, is);
	SDL_WaitThread(init_tid, 0);
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		SDL_Thread * init_tid = SDL_CreateThread(render_thread, is);
		SDL_WaitThread(render_tid, 0);
	}
	mI->End();

	// ----- Free -----
//	Free_Memory ();
	//STX_Service::GetAudioInstance()->DeleteInstance();
	return 0;
};
#else
int init(const char* aTitle){return glp->init(aTitle);}
void render(){glp->render();}
int IndieLib()
{
	glp->init("IndieLib_TankEnemies");
	while (!mI->Input->OnKeyPress (STX_KEY_ESCAPE) && !mI->Input->Quit())
	{
		glp->render();
	}
	mI->End();

	// ----- Free -----
//	Free_Memory ();
	//STX_Service::GetAudioInstance()->DeleteInstance();
	return 0;
}
#endif

//-----------------------------------
//-----------------------------------
// Local Functions
//-----------------------------------
//-----------------------------------
/*
void Free_Memory()
{
	std::vector < Bullet* >::iterator iter;

	for (iter = g_Bullets.begin();
		iter != g_Bullets.end();
		 iter++)
	{
		delete (*iter);
		(*iter) = 0;
	}
	g_Bullets.clear();


	std::vector < PropFighter* >::iterator iterPF;

	for (iterPF = g_PropFighters.begin();
		 iterPF != g_PropFighters.end();
		 iterPF++)
	{
		delete (*iterPF);
		(*iterPF) = 0;
	}
	g_PropFighters.clear();

	//mI->AnimationManager->Delete(g_pAnimExplosion);
	//mI->SurfaceManager->Delete(g_BulletSurface);
}

*/
//-----------------------------------
// Handle movement input then call
// move.
//----------------------------------
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

//-----------------------------------
// Move the player (tank)
// Velocity based or direct position
// based move.
//----------------------------------
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

//-----------------------------------
// Update player (tank) movement.
// Perform collision detection.
//-----------------------------------
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

//-----------------------------------
// Player collision detection.
// Hack!  Adding hardcoded sprite width.
//-----------------------------------
void Player_CollisionDetection()
{
	if (g_vecPlayer_Pos.x - 32 < 0.0f)
		g_vecPlayer_Pos.x = 32.0f;
	else
	if (g_vecPlayer_Pos.x + 32 > mI->Window->GetWidth())
		g_vecPlayer_Pos.x = (float)mI->Window->GetWidth() - 32.0f;

}

//-----------------------------------
// Move bullets.  Do collision detection.
//-----------------------------------
void Update_Bullets( float fElapsedTime )
{
	std::vector < Bullet* >::iterator iter;

	// For all live rounds
	for (iter = g_Bullets.begin();
		iter != g_Bullets.end();
		 iter++)
	{
		Bullet* pBullet = (*iter);

		if (pBullet)
		{
			// Update the bullet
			pBullet->Update( fElapsedTime );

			// Collision with view extents
			if (pBullet->entity.GetPosX() > mI->Window->GetWidth() ||
				pBullet->entity.GetPosX() < 0 ||
				pBullet->entity.GetPosY() > mI->Window->GetHeight() ||
				pBullet->entity.GetPosY() < 0 )
			{
				pBullet->bIsDead = true;
			}
		}
	}
}

//-----------------------------------
// Delete any bullets that have died!
//-----------------------------------
void Bury_Bullets()
{
	std::vector< Bullet* >::iterator iter = g_Bullets.begin();

	while ( iter != g_Bullets.end())
	{
		if ((*iter)->bIsDead)
		{
			delete (*iter);
			(*iter) = 0;
			iter = g_Bullets.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}


void Spawn_PropFighter( )
{
	// stx_rand() % (high - low) + low;
	// set facing direction
	// set position (off screen)
	// set velocity (includes random speed within a range)

	D3DXFROMWINEVECTOR3 vecVelDir = D3DXFROMWINEVECTOR3(1.0f, 0.0f, 0.0f);

	float speed = (float)(stx_rand() % (200 - 80) + 80);
	D3DXFROMWINEVECTOR3 Vel = vecVelDir * speed;

	// Set position with random height within a range
	float altitude = (float)(stx_rand() % (200 - 40) + 40);
	D3DXFROMWINEVECTOR3 Pos = D3DXFROMWINEVECTOR3(-50.0f, altitude, 0.0f);

	PropFighter* pNewPropFighter = new PropFighter(&gl.g_AnimPropFighter, Pos, Vel);

	// Add to vector
	g_PropFighters.push_back( pNewPropFighter );

}

void Update_PropFighters( float fElapsedTime )
{
	std::vector < PropFighter* >::iterator iter;

	// For all live rounds
	for (iter = g_PropFighters.begin();
		iter != g_PropFighters.end();
		iter++)
	{
		PropFighter* pEnemy = (*iter);

		if (pEnemy)
		{
			// Update the bullet
			pEnemy->Update( fElapsedTime );

			// Check collisions of each enemy with all bullets
			for (int i = 0; i < (int)g_Bullets.size(); i++)
			{
				#if 0
				if (mI->Entity2dManager->IsCollision(&pEnemy->entity, "/IndieLib-TankEnemies/", &g_Bullets[i]->entity, "/IndieLib-TankEnemies/"))
				#else
				if (mI->Entity2dManager->IsCollision(&pEnemy->entity, "", &g_Bullets[i]->entity, ""))
				#endif
				{
					// Boom!
					pEnemy->bIsDead = true;
					g_Bullets[i]->bIsDead = true;
					STX_Service::GetAudioInstance()->Play("bigexplode");	// maybe this should be included in the explosion.  Pass sound name as ctor parm.

					D3DXFROMWINEVECTOR3 pos = D3DXFROMWINEVECTOR3( pEnemy->entity.GetPosX(), pEnemy->entity.GetPosY(), (float)pEnemy->entity.GetPosZ());
					D3DXFROMWINEVECTOR3 vel = D3DXFROMWINEVECTOR3( pEnemy->vecVel.x, pEnemy->vecVel.y, pEnemy->vecVel.z);
					Explosion* pNewExplosion = new Explosion("/IndieLib-TankEnemies/images/explosion.xml", pos, vel);

					g_Explosions.push_back(pNewExplosion);
					break;
				}
			}
		}
	}
}


//-----------------------------------
// Delete any bullets that have died!
//-----------------------------------
void Bury_PropFighters()
{
	std::vector< PropFighter* >::iterator iter = g_PropFighters.begin();

	while ( iter != g_PropFighters.end())
	{
		if ((*iter)->bIsDead)
		{
			delete (*iter);
			(*iter) = 0;
			iter = g_PropFighters.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void Update_Explosions( float fElapsedTime )
{
	std::vector < Explosion* >::iterator iter;

	// For all live explosions
	for (iter = g_Explosions.begin();
		iter != g_Explosions.end();
		iter++)
	{
		Explosion* pExplosion = (*iter);

		pExplosion->Update( fElapsedTime );
	}
}


//-----------------------------------
// Delete any explosions that have died!
//-----------------------------------
void Bury_Explosions()
{
	std::vector< Explosion* >::iterator iter = g_Explosions.begin();

	while ( iter != g_Explosions.end())
	{
		if ((*iter)->bIsDead)
		{
			delete (*iter);
			(*iter) = 0;
			iter = g_Explosions.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

