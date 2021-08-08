#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib
import cmath
import math
import random

class Bullet:
	def __init__(self):
		self.entity=IndieLib.IND_Factory.GetEntity2d()
		self.vecPos=IndieLib.D3DXFROMWINEVECTOR3()
		self.vecVel=IndieLib.D3DXFROMWINEVECTOR3()
		self.bIsDead=False
		self.RADIUS_SCALE_FACTOR	= 0.65

	def __init__(self, pSurface, pos, vel, angle):
		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.entity)
		self.entity.SetSurface(pSurface)
		self.entity.SetPosition(pos.x, pos.y, 0)
		self.entity.SetHotSpot(0.5, 0.5)
		self.entity.SetAngleXYZ(0, 0, angle)
		self.entity.SetBoundingCircle("", pSurface.GetWidth() / 2, pSurface.GetHeight()/2,pSurface.GetWidth()/2 * self.RADIUS_SCALE_FACTOR)

		self.vecVel = vel
		self.vecPos = pos
		self.bIsDead = False

	def Update(self, fElapsedTime):
		self.vecPos += self.vecVel * fElapsedTime
		self.entity.SetPosition(self.vecPos.x, self.vecPos.y, 0)

class PropFighter:
	def __init__(self, pAnim=IndieLib.IND_Factory.GetAnimation(), pos=IndieLib.D3DXFROMWINEVECTOR3(0, 0, 0), vel=IndieLib.D3DXFROMWINEVECTOR3(0, 0, 0)):
		health=0.0
		self.entity=IndieLib.IND_Factory.GetEntity2d()
		self.vecPos=pos
		self.vecVel=vel
		self.bIsDead=False
		self.RADIUS_SCALE_FACTOR	= 0.65

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.entity)

		self.entity.SetAnimation(pAnim)

		self.entity.SetHotSpot(0.5, 0.5)

		self.entity.SetPosition(pos.x, pos.y, 0)

		self.vecVel = vel
		self.vecPos = pos
		self.bIsDead = False
		health = 1.0

	def Update(self, fElapsedTime):
		self.vecPos += self.vecVel * fElapsedTime
		self.entity.SetPosition(self.vecPos.x, self.vecPos.y, 0)

class Explosion:
	def __init__(self):
		self.entity=IndieLib.IND_Factory.GetEntity2d()
		self.AnimExplosion=IndieLib.IND_Factory.GetAnimation()
		self.vecPos=IndieLib.D3DXFROMWINEVECTOR3()
		self.vecVel=IndieLib.D3DXFROMWINEVECTOR3()
		self.bIsDead=False
		self.numFrames=0
		self.RADIUS_SCALE_FACTOR	= 0.65

	def __init__(self, szXmlAnimationFilename, pos, vel):
		IndieLib.CIndieLib.Instance().AnimationManager.AddToSurface (self.AnimExplosion, szXmlAnimationFilename, IndieLib.IND_ALPHA, IndieLib.IND_32)
		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.entity)
		self.entity.SetAnimation(self.AnimExplosion)
		self.entity.SetHotSpot(0.5, 0.5)
		self.entity.SetPosition(pos.x, pos.y, 0)
		self.entity.SetSequence(0)
		self.numFrames = self.AnimExplosion.GetNumFrames(0)

		self.vecPos = pos
		self.vecVel = vel
		self.bIsDead = False

	def Update(self, fElapsedTime):

		self.vecPos += self.vecVel * fElapsedTime
		self.entity.SetPosition(self.vecPos.x, self.vecPos.y, 0)

		pAnim = self.entity.GetAnimation()
		pos = pAnim.GetActualFramePos(0)

		if pos >= self.numFrames - 1:
	
			self.bIsDead = True
	

class AppPy:
	def __init__(self):
		self.DIR_LEFT	=1
		self.DIR_RIGHT	=2
		self.MAX_LAYERS =3
		self.g_vecPlayer_Pos		= IndieLib.D3DXFROMWINEVECTOR3( 400.0, 500.0, 0.0 )
		self.g_vecPlayerShadow_Pos	= IndieLib.D3DXFROMWINEVECTOR3( 0.0, 0.0, 0.0 )
		self.g_vecPlayer_Right		= IndieLib.D3DXFROMWINEVECTOR3( 1.0, 0.0, 0.0 )
		self.g_vecPlayer_Velocity	= IndieLib.D3DXFROMWINEVECTOR3( 0.0, 0.0, 0.0 )

		self.g_vecPropFighter_Pos	= IndieLib.D3DXFROMWINEVECTOR3( 400.0, 50.0, 0.0 )
		self.g_vecPropFighter_Velocity	= IndieLib.D3DXFROMWINEVECTOR3( 0.0, 0.0, 0.0 )

		self.g_fSpeed					= 1100.0
		self.g_fMaxVelocityX				= 180.0
		self.g_fFriction					= 950.0
		self.g_Epsilon					= 0.05
		self.g_fMuzzleFlashTimer			= 0.0
		self.g_bToggleFreezeTank			= False
		self.g_fGunAngle				= 0.0
		self.g_vecBarrel		= IndieLib.D3DXFROMWINEVECTOR3( 0.0, 0.0, 0.0 )
		self.fPI							= 3.14159
		self.BARREL_LENGTH				= 30.0
		self.BULLET_SPEED				= 500.0
		self.MAX_GUN_ANGLE				= 80.0
		self.FIRE_REPEAT_DELAY			= 0.15
		self.BIG_FIRE_REPEAT_DELAY		= 0.25
		self.SMALL_FIRE_REPEAT_DELAY		= 0.10
		self.MISL_FIRE_REPEAT_DELAY		= 0.8
		self.g_FireDelayTimer			= 0.0
		self.g_FireDelayTime				= 0.0
		self.g_BulletSurface=IndieLib.IND_Factory.GetSurface()
		self.g_pAnimExplosion=IndieLib.IND_Factory.GetAnimation()
		self.g_szBulletSound="tankfire"
		self.g_nCurrRound				= 0
		self.g_PropFighterSpawnTimer		= 0.0
		self.SPAWN_PROPFIGHTER_TIME		= 1.5

		self.g_Bullets=[]
		self.g_PropFighters=[]
		self.g_Explosions=[]

		self.g_szAnimationFile = "/IndieLib-TankEnemies/images/explosion.xml"

		self.fElapsedTime = 0.0
		self.szText=""
		self.szBuf=""
		self.bToggleShowBounds = False

		self.timer=IndieLib.IND_Factory.GetTimer()
		self.surfaceSky=IndieLib.IND_Factory.GetSurface()
		self.surfaceGnd=IndieLib.IND_Factory.GetSurface()
		self.surfaceBG=IndieLib.IND_Factory.GetSurface()
		self.animTank=IndieLib.IND_Factory.GetAnimation()
		self.animExplosion=IndieLib.IND_Factory.GetAnimation()
		self.surfaceTankTracks=IndieLib.IND_Factory.GetSurface()
		self.surfaceBarrel=IndieLib.IND_Factory.GetSurface()
		self.surfaceCursor=IndieLib.IND_Factory.GetSurface()
		self.surfaceBullet=IndieLib.IND_Factory.GetSurface()
		self.surfaceBulletBig=IndieLib.IND_Factory.GetSurface()
		self.surfaceBulletSmall=IndieLib.IND_Factory.GetSurface()
		self.surfaceBulletMissile=IndieLib.IND_Factory.GetSurface()
		self.surfaceTankShadow=IndieLib.IND_Factory.GetSurface()
		self.surfaceMuzzleFlash=IndieLib.IND_Factory.GetSurface()
		self.sky=IndieLib.IND_Factory.GetEntity2d()
		self.ground=IndieLib.IND_Factory.GetEntity2d()
		self.BG=IndieLib.IND_Factory.GetEntity2d()
		self.tank=IndieLib.IND_Factory.GetEntity2d()
		self.tankTracks=IndieLib.IND_Factory.GetEntity2d()
		self.barrel=IndieLib.IND_Factory.GetEntity2d()
		self.cursor=IndieLib.IND_Factory.GetEntity2d()
		self.tankShadow=IndieLib.IND_Factory.GetEntity2d()
		self.muzzleFlash=IndieLib.IND_Factory.GetEntity2d()
		self.FontSmall=IndieLib.IND_Factory.GetFont()
		self.mTextSmallWhite=IndieLib.IND_Factory.GetEntity2d()
		self.g_AnimPropFighter=IndieLib.IND_Factory.GetAnimation()

	def Player_Move4(self, Direction, Distance, Velocity):

		vecShift = IndieLib.D3DXFROMWINEVECTOR3(0, 0, 0)

		if Direction and self.DIR_RIGHT: 
			vecShift += self.g_vecPlayer_Right * Distance
		if Direction and self.DIR_LEFT: 
			vecShift -= self.g_vecPlayer_Right * Distance

		if Direction:
			self.Player_Move3 ( vecShift, Velocity )


	def Player_Move3(self, vecShift, Velocity):

		if Velocity:
	
			self.g_vecPlayer_Velocity += vecShift
	
		else:
	
			self.g_vecPlayer_Pos += vecShift
	


	def Player_Update(self, fElapsedTime ):


		if abs(self.g_vecPlayer_Velocity.x) > self.g_fMaxVelocityX:
	
			if self.g_vecPlayer_Velocity.x > 0:
				self.g_vecPlayer_Velocity.x = self.g_fMaxVelocityX
			else:
				self.g_vecPlayer_Velocity.x = -self.g_fMaxVelocityX
	

		self.Player_Move3 (self.g_vecPlayer_Velocity * fElapsedTime, False )

		self.Player_CollisionDetection()


		vecDec = -self.g_vecPlayer_Velocity
    		IndieLib.D3DXFROMWINEVec3Normalize( vecDec, vecDec )

	   	Length = IndieLib.D3DXFROMWINEVec3Length( self.g_vecPlayer_Velocity )

	    	Dec = (self.g_fFriction * fElapsedTime)
	    	if Dec > Length:
			Dec = Length

		    	self.g_vecPlayer_Velocity += vecDec * Dec


	def Player_CollisionDetection(self):

		if self.g_vecPlayer_Pos.x - 32 < 0.0:
			self.g_vecPlayer_Pos.x = 32.0
		elif self.g_vecPlayer_Pos.x + 32 > IndieLib.CIndieLib.Instance().Window.GetWidth():
			self.g_vecPlayer_Pos.x = IndieLib.CIndieLib.Instance().Window.GetWidth() - 32.0



	def Update_Bullets(self, fElapsedTime ):
		for pBullet in self.g_Bullets:

			if pBullet:
		
				pBullet.Update( fElapsedTime )

				if pBullet.self.entity.GetPosX() > IndieLib.CIndieLib.Instance().Window.GetWidth() or pBullet.self.entity.GetPosX() < 0 or pBullet.self.entity.GetPosY() > IndieLib.CIndieLib.Instance().Window.GetHeight() or pBullet.self.entity.GetPosY() < 0:
			
					pBullet.self.bIsDead = True

	def Bury_Bullets(self):
		idx=0
		for pBullet in self.g_Bullets:
	
			if pBullet.bIsDead:
		
				self.g_Bullets[idx]=0
			idx+=1

	def Spawn_PropFighter(self):

		vecVelDir = IndieLib.D3DXFROMWINEVECTOR3(1.0, 0.0, 0.0)

		speed = (random.random() % (200 - 80) + 80)
		Vel = vecVelDir * speed

		altitude = (random.random() % (200 - 40) + 40)
		Pos = IndieLib.D3DXFROMWINEVECTOR3(-50.0, altitude, 0.0)

		pNewPropFighter = PropFighter(self.g_AnimPropFighter, Pos, Vel)

		self.g_PropFighters.push_back( pNewPropFighter )



	def Update_PropFighters(self, fElapsedTime):

		for pEnemy in self.g_PropFighters:
	
			if pEnemy:
		
				pEnemy.Update( fElapsedTime )

				for i in range(0,self.g_Bullets.size()):
			
					#if IndieLib.CIndieLib.Instance().Entity2dManager.IsCollision(pEnemy.self.entity, "/IndieLib-TankEnemies/", self.g_Bullets[i].self.entity, "/IndieLib-TankEnemies/"):
					if IndieLib.CIndieLib.Instance().Entity2dManager.IsCollision(pEnemy.self.entity, "", self.g_Bullets[i].self.entity, ""):
				
				
						pEnemy.self.bIsDead = True
						self.g_Bullets[i].self.bIsDead = True
						IndieLib.IND_Factory.GetAudioInstance().Play("bigexplode")

						pos = IndieLib.D3DXFROMWINEVECTOR3( pEnemy.self.entity.GetPosX(), pEnemy.self.entity.GetPosY(), pEnemy.self.entity.GetPosZ())
						vel = IndieLib.D3DXFROMWINEVECTOR3( pEnemy.self.vecVel.x, pEnemy.self.vecVel.y, pEnemy.self.vecVel.z)
						pNewExplosion = Explosion("/IndieLib-TankEnemies/images/explosion.xml", pos, vel)

						self.g_Explosions.push_back(pNewExplosion)
						break
				
	def Bury_PropFighters(self):
		idx=0
		for pPropFighter in self.g_PropFighters:

			if pPropFighter.bIsDead:
		
				g_PropFighters[idx]=0
			idx+=1
		
	


	def Update_Explosions(self, fElapsedTime):

		for pExplosion in self.g_Explosions:

			pExplosion.Update( fElapsedTime )
	



	def Bury_Explosions(self):

		idx=0
		for pExplosion in self.g_Explosions:
	
			if pExplosion.self.bIsDead:
				self.g_Explosions[idx]=0
		
			idx+=1

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("IndieLib-TankEnemies")

		IndieLib.IND_Factory.GetAudioInstance().Init()
		IndieLib.IND_Factory.GetAudioInstance().Load("/IndieLib-TankEnemies/sounds/tankfire.wav", "tankfire")
		IndieLib.IND_Factory.GetAudioInstance().Load("/IndieLib-TankEnemies/sounds/tankfire1.wav", "tankfire1")
		IndieLib.IND_Factory.GetAudioInstance().Load("/IndieLib-TankEnemies/sounds/tankfire2.wav", "tankfire2")
		IndieLib.IND_Factory.GetAudioInstance().Load("/IndieLib-TankEnemies/sounds/cached_missile.wav", "tankfire3")
		IndieLib.IND_Factory.GetAudioInstance().Load("/IndieLib-TankEnemies/sounds/bigexplode.wav", "bigexplode")
		IndieLib.IND_Factory.GetAudioInstance().Load("/IndieLib-TankEnemies/sounds/bullethit.wav", "bullethit")
		#IndieLib.IND_Factory.GetAudioInstance().Load("music/LoveTheme", "AtomicTank")

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceSky, "/IndieLib-TankEnemies/images/frigistan_sky.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			 return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceGnd, "/IndieLib-TankEnemies/images/frigistan_ground.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			 return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceBG, "/IndieLib-TankEnemies/images/frigistan_bg.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			 return 0

		if not IndieLib.CIndieLib.Instance().AnimationManager.AddToSurface (self.animTank, "/IndieLib-TankEnemies/images/tank.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			 return 0
		#if not IndieLib.CIndieLib.Instance().AnimationManager.AddToSurface (self.animTank, "/IndieLib-TankEnemiesParallax/images/tank.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
		#return 0

		if not IndieLib.CIndieLib.Instance().AnimationManager.AddToSurface (self.animExplosion, "/IndieLib-TankEnemies/images/explosion.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceTankTracks, "/IndieLib-TankEnemies/images/tracks.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			 return 0

	
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceBarrel, "/IndieLib-TankEnemies/images/barrel.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			 return 0

	
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceCursor, "/IndieLib-TankEnemies/images/crosshairs1.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			 return 0

	
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceBullet, "/IndieLib-TankEnemies/images/bullet.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			 return 0

	
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceBulletBig, "/IndieLib-TankEnemies/images/bullet_big.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			 return 0

	
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceBulletSmall, "/IndieLib-TankEnemies/images/bullet_small.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			 return 0

	
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceBulletMissile, "/IndieLib-TankEnemies/images/missile.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			 return 0

	
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceTankShadow, "/IndieLib-TankEnemies/images/tankshadow.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			 return 0

	
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceMuzzleFlash, "/IndieLib-TankEnemies/images/muzzleflash.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			 return 0

		if not IndieLib.CIndieLib.Instance().AnimationManager.AddToSurface (self.g_AnimPropFighter, "/IndieLib-TankEnemies/images/propfighter.xml", IndieLib.IND_ALPHA, IndieLib.IND_32): 
			return 0


	
		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.sky)
		self.sky.SetSurface(self.surfaceSky)

	
		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.ground)
		self.ground.SetSurface(self.surfaceGnd)
		self.ground.SetPosition(0, (IndieLib.CIndieLib.Instance().Window.GetHeight() - self.surfaceGnd.GetHeight()), 0)

	
		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.BG)
		self.BG.SetSurface(self.surfaceBG)
		self.BG.SetPosition(0, IndieLib.CIndieLib.Instance().Window.GetHeight()- self.surfaceGnd.GetHeight() - self.surfaceBG.GetHeight(), 0)

	
		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.tank)
		self.tank.SetAnimation(self.animTank)
		self.tank.SetHotSpot(0.5, 0.1)
		self.tank.SetPosition(self.g_vecPlayer_Pos.x, self.g_vecPlayer_Pos.y, 0)

	
		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.tankTracks)
		self.tankTracks.SetSurface(self.surfaceTankTracks)
		self.tankTracks.SetPosition(0, 540, 0)
		self.tankTracks.SetHotSpot(0.0, 0.5)

	
		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.barrel)
		self.barrel.SetSurface(self.surfaceBarrel)
		self.barrel.SetHotSpot(0.5, 1.0)
		self.barrel.SetScale(1.5, 1.5)

	
		IndieLib.CIndieLib.Instance().Entity2dManager.Add(1, self.cursor)
		self.cursor.SetSurface(self.surfaceCursor)
		self.cursor.SetHotSpot(0.5, 0.5)

	
		IndieLib.CIndieLib.Instance().Entity2dManager.Add(0, self.tankShadow)
		self.tankShadow.SetSurface(self.surfaceTankShadow)
		self.tankShadow.SetHotSpot(0.5, 0.7)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(0, self.muzzleFlash)
		self.muzzleFlash.SetSurface(self.surfaceMuzzleFlash)
		self.muzzleFlash.SetHotSpot(0.5, 0.5)
		self.muzzleFlash.SetShow(False)

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.FontSmall, "/IndieLib-TankEnemies/font_small.png", "/IndieLib-TankEnemies/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			 return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(1, self.mTextSmallWhite)
		self.mTextSmallWhite.SetFont		(self.FontSmall)
		self.mTextSmallWhite.SetLineSpacing	(18)
		self.mTextSmallWhite.SetCharSpacing	(-8)
		self.mTextSmallWhite.SetPosition	(5, 5, 1)
		self.mTextSmallWhite.SetAlign		(IndieLib.IND_LEFT)

		self.g_BulletSurface = self.surfaceBullet
		self.g_pAnimExplosion = self.animExplosion
		self.g_FireDelayTime = self.FIRE_REPEAT_DELAY
		self.g_szBulletSound = "tankfire"
		self.g_nCurrRound = 1
		self.g_PropFighterSpawnTimer = 2.0

		#IndieLib.IND_Factory.GetAudioInstance().Play("AtomicTank")

		srand(time(0))

		self.timer.Start()
		return 0

	
	def render(self):

		IndieLib.CIndieLib.Instance().Input.Update ()

		self.fElapsedTime = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		self.szText  = "Aim and move self.tank with the mouse.\n"
		self.szText += "Press Left Mouse button to firenot \n"
		self.szText += "Press 'f' to toggle freezing the self.tank movement (debug)\n"
		self.szText += "Press 1 = Med Round\nPress 2 = Small Round\nPress 3 = Big Round\nPress 4 = Missile\n"
		self.szBuf = "Round Type: %d\n" % self.g_nCurrRound
		self.szText += self.szBuf
		self.szBuf = "Muzzle angle %.3f\n" % self.g_fGunAngle
		self.szText += self.szBuf
		self.szBuf = "Live Rounds: %d\n" % len(self.g_Bullets)
		self.szText += self.szBuf
		if self.g_bToggleFreezeTank:
		
			self.szText += "Tank is frozennot \n"
		
		self.szBuf = "mouse %d, %d\n" % (IndieLib.CIndieLib.Instance().Input.GetMouseX(), IndieLib.CIndieLib.Instance().Input.GetMouseY())
		self.szText += self.szBuf
		self.szBuf = "self.tank %.3f, %.3f\n" % (self.g_vecPlayer_Pos.x, self.g_vecPlayer_Pos.y)
		self.szText += self.szBuf
		self.mTextSmallWhite.SetText	 (self.szText)

		self.cursor.SetPosition(IndieLib.CIndieLib.Instance().Input.GetMouseX(), IndieLib.CIndieLib.Instance().Input.GetMouseY(), 1)

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress(IndieLib.STX_KEY_f):
			self.g_bToggleFreezeTank = not self.g_bToggleFreezeTank

		Direction = 0

		inside = False

		if IndieLib.CIndieLib.Instance().Input.GetMouseY() > (self.g_vecPlayer_Pos.y - 32) and IndieLib.CIndieLib.Instance().Input.GetMouseX() < (self.g_vecPlayer_Pos.x + 36) and IndieLib.CIndieLib.Instance().Input.GetMouseX() > (self.g_vecPlayer_Pos.x - 36) and IndieLib.CIndieLib.Instance().Input.GetMouseY() < (self.g_vecPlayer_Pos.y + 55):
		
			inside = True
		
		deltaX = self.g_vecPlayer_Pos.x - IndieLib.CIndieLib.Instance().Input.GetMouseX()
		if not self.g_bToggleFreezeTank and (abs(deltaX) > self.g_Epsilon) and not inside:
			if deltaX < 0:
				Direction |= self.DIR_RIGHT
				self.tank.SetSequence(1)
			elif deltaX > 0:
				Direction |= self.DIR_LEFT
				self.tank.SetSequence(2)
		else:
			self.tank.SetSequence(0)
		if Direction and not self.g_bToggleFreezeTank:
			self.Player_Move4( Direction, self.g_fSpeed * self.fElapsedTime, True )
		self.Player_Update( self.fElapsedTime )

		fMouse2TankX = self.tank.GetPosX() - IndieLib.CIndieLib.Instance().Input.GetMouseX()
		fMouse2TankY = self.tank.GetPosY() - IndieLib.CIndieLib.Instance().Input.GetMouseY()

		if fMouse2TankY < 0.0:
			fMouse2TankY = 0.0
		self.g_fGunAngle = math.atan2(fMouse2TankY, fMouse2TankX) * 180.0 / self.fPI
		self.g_fGunAngle -= 90.0
		self.barrel.SetAngleXYZ(0.0, 0.0, self.g_fGunAngle)

		self.g_vecBarrel.x = self.tank.GetPosX()
		self.g_vecBarrel.y = self.tank.GetPosY()
		self.barrel.SetPosition(self.g_vecBarrel.x, self.g_vecBarrel.y, 0)

		self.tank.SetPosition(self.g_vecPlayer_Pos.x, self.g_vecPlayer_Pos.y, 1)
		self.tankShadow.SetPosition(self.tank.GetPosX(), self.tank.GetPosY() + 50, 0)

		if IndieLib.CIndieLib.Instance().Input.IsMouseButtonPressed(IndieLib.STX_MBUTTON_LEFT) and self.g_FireDelayTimer == 0.0:
			
			angleDegrees = self.barrel.GetAngleZ() + 90.0
			angleRadians = angleDegrees * self.fPI / 180.0
			fX = self.barrel.GetPosX() - cosf(angleRadians) * self.BARREL_LENGTH
			fY = self.barrel.GetPosY() - sinf(angleRadians) * self.BARREL_LENGTH
			angle = angleDegrees - 90.0
			Pos = IndieLib.D3DXFROMWINEVECTOR3(fX, fY, 0.0)

			mtxRotate
			vecVelDir = IndieLib.D3DXFROMWINEVECTOR3(-1.0, 0.0, 0.0)
			D3DXFROMWINEMatrixRotationZ( mtxRotate, angleRadians)
			D3DXFROMWINEVec3TransformNormal( vecVelDir, vecVelDir, mtxRotate )
			Vel = vecVelDir * self.BULLET_SPEED

			pBullet = Bullet(self.g_BulletSurface, Pos, Vel, angle)

			self.g_fMuzzleFlashTimer = 0.05
			self.muzzleFlash.SetPosition(Pos.x, Pos.y, 0)

			self.g_Bullets.push_back(pBullet)

			IndieLib.IND_Factory.GetAudioInstance().Play(self.g_szBulletSound)

			self.g_FireDelayTimer = self.g_FireDelayTime
		

		if self.g_FireDelayTimer > 0.0:
		
			self.g_FireDelayTimer -= self.fElapsedTime
		
		else:
		
			self.g_FireDelayTimer = 0.0
		

		self.Update_Bullets( self.fElapsedTime )

		self.Bury_Bullets()


		self.g_fMuzzleFlashTimer -= self.fElapsedTime
		if self.g_fMuzzleFlashTimer > 0.0:
		
			self.muzzleFlash.SetShow(True)
		
		else:
		
			self.muzzleFlash.SetShow(False)
			self.g_fMuzzleFlashTimer = 0.0
		

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress(IndieLib.STX_KEY_1):
		
			self.g_FireDelayTime = self.FIRE_REPEAT_DELAY
			self.g_BulletSurface = self.surfaceBullet
			self.g_szBulletSound = "tankfire"
			self.g_nCurrRound = 1
		
		elif IndieLib.CIndieLib.Instance().Input.OnKeyPress(IndieLib.STX_KEY_2):
		
			self.g_BulletSurface = self.surfaceBulletSmall
			self.g_FireDelayTime = self.SMALL_FIRE_REPEAT_DELAY
			self.g_szBulletSound = "tankfire1"
			self.g_nCurrRound = 2
		
		elif IndieLib.CIndieLib.Instance().Input.OnKeyPress(IndieLib.STX_KEY_3):
		
			self.g_BulletSurface = self.surfaceBulletBig
			self.g_FireDelayTime = self.BIG_FIRE_REPEAT_DELAY
			self.g_szBulletSound = "tankfire2"
			self.g_nCurrRound = 3
		
		elif IndieLib.CIndieLib.Instance().Input.OnKeyPress(IndieLib.STX_KEY_4):
		
			self.g_BulletSurface = self.surfaceBulletMissile
			self.g_FireDelayTime = self.MISL_FIRE_REPEAT_DELAY
			self.g_szBulletSound = "tankfire3"
			self.g_nCurrRound = 4
		

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress(IndieLib.STX_KEY_b):
			self.bToggleShowBounds = not self.bToggleShowBounds

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress(IndieLib.STX_KEY_F1):
		
			IndieLib.CIndieLib.Instance().Render.ToggleFullScreen()
		

		self.g_PropFighterSpawnTimer -= self.fElapsedTime
		if self.g_PropFighterSpawnTimer < 0.0:
		

			self.Spawn_PropFighter()

			self.g_PropFighterSpawnTimer = self.SPAWN_PROPFIGHTER_TIME

		

		self.Update_PropFighters ( self.fElapsedTime )
		self.Bury_PropFighters	( )

		self.Update_Explosions	( self.fElapsedTime )
		self.Bury_Explosions		( )

		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Render.ClearViewPort (160, 160, 160)
		IndieLib.CIndieLib.Instance().Render.SetViewPort2d (0, 0, IndieLib.CIndieLib.Instance().Window.GetWidth(), IndieLib.CIndieLib.Instance().Window.GetHeight())

		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d ( )

		#???IndieLib.CIndieLib.Instance().Render.ShowFpsInWindowTitle()

		for i in range(0,self.MAX_LAYERS):
		
			IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d (i)

			if self.bToggleShowBounds:
				IndieLib.CIndieLib.Instance().Entity2dManager.RenderCollisionAreas (i, 255, 0, 0, 255)

		

		IndieLib.IND_Factory.GetAudioInstance().Update()

		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

