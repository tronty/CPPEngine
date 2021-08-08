#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib
import cmath
import math

DIR_LEFT	=1
DIR_RIGHT	=2

class AppPy:
	def __init__(self):
		self.g_vecPlayer_Pos		= IndieLib.D3DXFROMWINEVECTOR3( 400.0, 500.0, 0.0 )
		self.g_vecPlayerShadow_Pos	= IndieLib.D3DXFROMWINEVECTOR3( 0.0, 0.0, 0.0 )
		self.g_vecPlayer_Right		= IndieLib.D3DXFROMWINEVECTOR3( 1.0, 0.0, 0.0 )
		self.g_vecPlayer_Velocity	= IndieLib.D3DXFROMWINEVECTOR3( 0.0, 0.0, 0.0 )
		self.g_vecBullet_Pos		= IndieLib.D3DXFROMWINEVECTOR3( 0.0, 0.0, 0.0 )
		self.g_vecBullet_Velocity	= IndieLib.D3DXFROMWINEVECTOR3( 0.0, 0.0, 0.0 )
		self.g_bBulletDead		= True
		self.g_fSpeed			= 1100.0
		self.g_fMaxVelocityX		= 200.0
		self.g_fFriction		= 950.0
		self.g_Epsilon			= 0.05
		self.g_fMuzzleFlashTimer	= 0.0
		self.g_bToggleFreezeTank	= False
		self.g_fGunAngle		= 0.0
		self.g_vecBarrel		= IndieLib.D3DXFROMWINEVECTOR3( 0.0, 0.0, 0.0 )
		self.fPI			= 3.14159
		self.BARREL_LENGTH		= 30.0
		self.BULLET_SPEED		= 500.0
		self.MAX_GUN_ANGLE		= 80.0
		self.fElapsedTime = 0.0
		self.szText=""
		self.szBuf=""

		self.timer=IndieLib.IND_Factory.GetTimer()
		self.surfaceSky=IndieLib.IND_Factory.GetSurface()
		self.surfaceGnd=IndieLib.IND_Factory.GetSurface()
		self.animTank=IndieLib.IND_Factory.GetAnimation()
		self.surfaceBarrel=IndieLib.IND_Factory.GetSurface()
		self.surfaceCursor=IndieLib.IND_Factory.GetSurface()
		self.surfaceBullet=IndieLib.IND_Factory.GetSurface()
		self.surfaceTankShadow=IndieLib.IND_Factory.GetSurface()
		self.surfaceMuzzleFlash=IndieLib.IND_Factory.GetSurface()
		self.sky=IndieLib.IND_Factory.GetEntity2d()
		self.ground=IndieLib.IND_Factory.GetEntity2d()
		self.tank=IndieLib.IND_Factory.GetEntity2d()
		self.barrel=IndieLib.IND_Factory.GetEntity2d()
		self.bullet=IndieLib.IND_Factory.GetEntity2d()
		self.cursor=IndieLib.IND_Factory.GetEntity2d()
		self.tankShadow=IndieLib.IND_Factory.GetEntity2d()
		self.muzzleFlash=IndieLib.IND_Factory.GetEntity2d()
		self.FontSmall=IndieLib.IND_Factory.GetFont()
		self.mTextSmallWhite=IndieLib.IND_Factory.GetEntity2d()

	def Player_Move4(self, Direction, Distance, Velocity):
		vecShift = IndieLib.D3DXFROMWINEVECTOR3(0, 0, 0)

		if Direction and DIR_RIGHT:
			vecShift += self.g_vecPlayer_Right * Distance
		if Direction and DIR_LEFT:
			vecShift -= self.g_vecPlayer_Right * Distance

		if Direction:
			self.Player_Move3 ( vecShift, Velocity )

	def Player_Move3(self, vecShift, Velocity):

		if Velocity:
	
			self.g_vecPlayer_Velocity += vecShift
	
		else:
	
			self.g_vecPlayer_Pos += vecShift

	def Player_Update(self, fElapsedTime):

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
		if self.g_vecPlayer_Pos.x < 0.0:
			self.g_vecPlayer_Pos.x = 0.0
		elif self.g_vecPlayer_Pos.x > IndieLib.CIndieLib.Instance().Window.GetWidth():
			self.g_vecPlayer_Pos.x = IndieLib.CIndieLib.Instance().Window.GetWidth()

	def Bullet_Update(self, fElapsedTime, pBullet):
 		self.g_vecBullet_Pos += self.g_vecBullet_Velocity * fElapsedTime
		if pBullet:
	
			if (pBullet.GetPosX() > IndieLib.CIndieLib.Instance().Window.GetWidth() or pBullet.GetPosX() < 0 or pBullet.GetPosY() > IndieLib.CIndieLib.Instance().Window.GetHeight() or pBullet.GetPosY() < 0 ):
		
				self.g_bBulletDead = True
		

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("IndieLib-TankFollowsMouseFires")
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceSky, "/IndieLib-TankFollowsMouseFires/images/frigistan_sky.png", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceGnd, "/IndieLib-TankFollowsMouseFires/images/frigistan_ground.png", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().AnimationManager.AddToSurface (self.animTank, "/IndieLib-TankFollowsMouseFires/images/tank.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceBarrel, "/IndieLib-TankFollowsMouseFires/images/barrel.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceCursor, "/IndieLib-TankFollowsMouseFires/images/crosshairs1.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceBullet, "/IndieLib-TankFollowsMouseFires/images/bullet.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceTankShadow, "/IndieLib-TankFollowsMouseFires/images/tankshadow.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add(self.surfaceMuzzleFlash, "/IndieLib-TankFollowsMouseFires/images/muzzleflash.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0


		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.sky)
		self.sky.SetSurface(self.surfaceSky)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.ground)
		self.ground.SetSurface(self.surfaceGnd)
		self.ground.SetPosition(0, IndieLib.CIndieLib.Instance().Window.GetHeight() - self.surfaceGnd.GetHeight(), 0)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.tank)
		self.tank.SetAnimation(self.animTank)
		self.tank.SetHotSpot(0.5, 0.1)
		self.tank.SetPosition(self.g_vecPlayer_Pos.x, self.g_vecPlayer_Pos.y, 0)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.barrel)
		self.barrel.SetSurface(self.surfaceBarrel)
		self.barrel.SetHotSpot(0.5, 1.0)
		self.barrel.SetScale(1.5, 1.5)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.bullet)
		self.bullet.SetSurface(self.surfaceBullet)
		self.bullet.SetHotSpot(0.5, 0.5)
		self.bullet.SetShow(False)

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


		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.FontSmall, "/IndieLib-TankFollowsMouseFires/font_small.png", "/IndieLib-TankFollowsMouseFires/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(1, self.mTextSmallWhite)
		self.mTextSmallWhite.SetFont			(self.FontSmall)
		self.mTextSmallWhite.SetLineSpacing	(18)
		self.mTextSmallWhite.SetCharSpacing	(-8)
		self.mTextSmallWhite.SetPosition		(5, 5, 1)
		self.mTextSmallWhite.SetAlign		(IND_LEFT)

		self.timer.Start()

		IndieLib.IND_Factory.GetAudioInstance().Init()

		IndieLib.IND_Factory.GetAudioInstance().Load("/IndieLib-TankFollowsMouseFires/sounds/tankfire.wav",  "tankfire")
		IndieLib.IND_Factory.GetAudioInstance().Load("/IndieLib-TankFollowsMouseFires/sounds/tankfire1.wav",  "tankfire1")
		IndieLib.IND_Factory.GetAudioInstance().Load("/IndieLib-TankFollowsMouseFires/sounds/tankfire2.wav",  "tankfire2")
		IndieLib.IND_Factory.GetAudioInstance().Load("/IndieLib-TankFollowsMouseFires/sounds/tankfire3.wav",  "tankfire3")
		IndieLib.IND_Factory.GetAudioInstance().Load("/IndieLib-TankFollowsMouseFires/sounds/tankfire4.wav",  "tankfire4")
		return 0

	def render(self):
	
		IndieLib.CIndieLib.Instance().Input.Update ()

		self.fElapsedTime = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		self.szText="Move the mouse left and right and watch the self.tank try to follownot \n"
		self.szText+="Press Left Mouse button to firenot \n"
		self.szText+="Press 'f' to toggle freezing the self.tank movement (debug)\n"
		self.szBuf = "%d, %d\n" % (IndieLib.CIndieLib.Instance().Input.GetMouseX(), IndieLib.CIndieLib.Instance().Input.GetMouseY()) 
		self.szText+=self.szBuf
		self.szBuf = "%.3f, %.3f\n" % (self.g_vecPlayer_Pos.x, self.g_vecPlayer_Pos.y)
		self.szText+=self.szBuf
		self.szBuf = "Muzzle angle %.3f\n" % self.g_fGunAngle
		self.szText+=self.szBuf
		self.szBuf = "Tank velocity %.3f\n" % self.g_vecPlayer_Velocity.x
		self.szText+=self.szBuf
		self.mTextSmallWhite.SetText	 (self.szText)

		self.cursor.SetPosition(IndieLib.CIndieLib.Instance().Input.GetMouseX(), IndieLib.CIndieLib.Instance().Input.GetMouseY(), 1)

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress(IndieLib.STX_KEY_f):
			self.g_bToggleFreezeTank = not self.g_bToggleFreezeTank


		Direction = 0

		deltaX = self.g_vecPlayer_Pos.x - IndieLib.CIndieLib.Instance().Input.GetMouseX()

		if not self.g_bToggleFreezeTank and abs(deltaX) > self.g_Epsilon:
		
			if deltaX < 0:
			
				Direction |= DIR_RIGHT
				self.tank.SetSequence(1)
			
			else:
				deltaX > 0
			
				Direction |= DIR_LEFT
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


		if 5 and IndieLib.CIndieLib.Instance().Input.OnMouseButtonPress(IndieLib.STX_MBUTTON_LEFT):		
			angleDegrees = self.barrel.GetAngleZ() + 90.0
			angleRadians = angleDegrees * self.fPI / 180.0
			fX = self.barrel.GetPosX() - cosf(angleRadians) * self.BARREL_LENGTH
			fY = self.barrel.GetPosY() - sinf(angleRadians) * self.BARREL_LENGTH
			self.bullet.SetAngleXYZ(0, 0, angleDegrees - 90.0)
			self.g_vecBullet_Pos = IndieLib.D3DXFROMWINEVECTOR3(fX, fY, 0.0)
			self.bullet.SetPosition(self.g_vecBullet_Pos.x, self.g_vecBullet_Pos.y, 0)

			mtxRotate=IndieLib.D3DXFROMWINEMATRIX()
			vecVelDir = IndieLib.D3DXFROMWINEVECTOR3(-1.0, 0.0, 0.0)
			D3DXFROMWINEMatrixRotationZ( mtxRotate, angleRadians)
			D3DXFROMWINEVec3TransformNormal( vecVelDir, vecVelDir, mtxRotate )
			self.g_vecBullet_Velocity = vecVelDir * self.BULLET_SPEED

			self.g_fMuzzleFlashTimer = 0.05
			self.muzzleFlash.SetPosition(self.g_vecBullet_Pos.x, self.g_vecBullet_Pos.y, 0)
			self.g_bBulletDead = False

			IndieLib.IND_Factory.GetAudioInstance().Play("tankfire1")
		

		if not self.g_bBulletDead:
		
			self.bullet.SetShow(True)
			Bullet_Update( self.fElapsedTime, self.bullet )
			self.bullet.SetPosition(self.g_vecBullet_Pos.x, self.g_vecBullet_Pos.y, 0)
		
		else:
		
			self.bullet.SetShow(False)
		

		self.g_fMuzzleFlashTimer -= self.fElapsedTime
		if self.g_fMuzzleFlashTimer > 0.0:
		
			self.muzzleFlash.SetShow(True)
		
		else:
		
			self.muzzleFlash.SetShow(False)
			self.g_fMuzzleFlashTimer = 0.0
		


		IndieLib.CIndieLib.Instance().Render.BeginScene ()

		IndieLib.CIndieLib.Instance().Render.ClearViewPort (160, 160, 160)

		IndieLib.CIndieLib.Instance().Render.SetViewPort2d (0, 0, IndieLib.CIndieLib.Instance().Window.GetWidth(), IndieLib.CIndieLib.Instance().Window.GetHeight())

		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d ( )

		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d (1)
		#IndieLib.CIndieLib.Instance().Render.ShowFpsInWindowTitle()

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

