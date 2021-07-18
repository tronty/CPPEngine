#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib
import math

class AppPy:
	def __init__(self):
		self.MiddleScreenX= 0# = IndieLib.CIndieLib.Instance().Window.GetWidth() / 2
		self.MiddleScreenY= 0# = IndieLib.CIndieLib.Instance().Window.GetHeight() / 2
		self.CamPosX= 0# = MiddleScreenX

		self.bShowBoundsToggle = False
		self.fElapsedTime = 0.0
		self.x = 0.0
		self.y = 450.0

		self.VelocityX = 0.0
		self.VelocityY = 0.0
		self.AccelX	= 0.0
		self.MaxVelocityX = 300

		self.slideFriction = 100
		self.Dir = 0
		self.fJumpForce = 750
		self.speed = 700
		self.gravity = 2000
		self.jumpAccel = 0
		self.walkDecel = 0
		self.playerJumping = False

		self.timer=IndieLib.IND_Factory.GetTimer()
		self.mSurfaceBack=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceBeetle=IndieLib.IND_Factory.GetSurface()
		self.AnimationCharacter=IndieLib.IND_Factory.GetAnimation()
		self.mBack=IndieLib.IND_Factory.GetEntity2d()
		self.Player=IndieLib.IND_Factory.GetEntity2d()
		self.Beetle=IndieLib.IND_Factory.GetEntity2d()
		self.Cameras2d=IndieLib.IND_Factory.GetCamera2d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("IndieLib-Animation-Collision")

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBack, "/IndieLib-Animation-Collision/cave.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBeetle, "/IndieLib-Animation-Collision/beetleship.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().AnimationManager.AddToSurface (self.AnimationCharacter, "/IndieLib-Animation-Collision/mario.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0



		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mBack)
		self.mBack.SetSurface (self.mSurfaceBack)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (1, self.Player)
		self.Player.SetAnimation (self.AnimationCharacter)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (0, self.Beetle)
		self.Beetle.SetSurface (self.mSurfaceBeetle)
		self.Beetle.SetPosition(500, 460, 0)
		self.Beetle.SetHotSpot (0.5, 0.5)
		self.Beetle.SetMirrorX (True)
		self.Beetle.SetBoundingTriangle ("beetle_head", 160, 105, 160, 170, 190, 135)
		self.Beetle.SetBoundingCircle ("beetle_boy_head", 85, 52, 55)



		self.MiddleScreenX = IndieLib.CIndieLib.Instance().Window.GetWidth() / 2
		self.MiddleScreenY = IndieLib.CIndieLib.Instance().Window.GetHeight() / 2
		self.CamPosX = self.MiddleScreenX
		Cameras2d=IndieLib.IND_Factory.GetCamera2d(self.MiddleScreenX, self.MiddleScreenY)

		self.timer.Start()



		self.Player.SetSequence (0)
		self.Player.SetPosition (self.x , self.y, 0)
		self.Player.SetHotSpot (0.5, 0.5)
		self.Player.SetMirrorX (False)
		self.Player.SetBoundingRectangle("entire", 0, 0, 48, 48)
		return 0

	def render(self):
		IndieLib.CIndieLib.Instance().Input.Update ()

		self.fElapsedTime = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress(IndieLib.STX_KEY_p):
			bShowBoundsToggle = not bShowBoundsToggle

		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed(IndieLib.STX_KEY_LEFT):
			self.VelocityX -= speed * self.fElapsedTime
			if not self.playerJumping:
				Player.SetSequence(0)
			self.Player.SetMirrorX(True)

		else:
			if IndieLib.CIndieLib.Instance().Input.IsKeyPressed(IndieLib.STX_KEY_RIGHT):
				self.VelocityX += speed * self.fElapsedTime
				if not playerJumping:
					Player.SetSequence(0)
				Player.SetMirrorX(False)
			else:
				if not self.playerJumping:
					self.Player.SetSequence(1)
				self.VelocityX = 0.0

			if IndieLib.CIndieLib.Instance().Input.OnKeyPress(IndieLib.STX_KEY_SPACE) and not playerJumping:
				self.VelocityY -= fJumpForce
				self.Player.SetSequence(2)
				self.playerJumping = True
		
			self.VelocityY += self.gravity * self.fElapsedTime
			self.y += self.VelocityY * self.fElapsedTime

			if self.y > 500:
				self.VelocityY = 0
				self.y = 500
				self.playerJumping = False

			Length = math.sqrt(self.VelocityX * self.VelocityX)
			if (Length > self.MaxVelocityX) :
				self.VelocityX *= ( self.MaxVelocityX / Length )
		
			self.x += self.VelocityX * self.fElapsedTime
			if (self.x < 0):
				self.x = 0

			self.Player.SetPosition(self.x, self.y, 0)

			Dec = -self.Dir * self.slideFriction * self.fElapsedTime

			if IndieLib.CIndieLib.Instance().Entity2dManager.IsCollision (self.Player, "/IndieLib-Animation-Collision/entire", self.Beetle, "/IndieLib-Animation-Collision/beetle_boy_head"):
				VelocityY = 80
				VelocityX = 0

			if (self.x > (IndieLib.CIndieLib.Instance().Window.GetWidth() / 2)):
				Cameras2d.SetPosition(self.x, MiddleScreenY)


			IndieLib.CIndieLib.Instance().Render.BeginScene ()
			IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60)
			IndieLib.CIndieLib.Instance().Render.SetViewPort2d (0, 0, IndieLib.CIndieLib.Instance().Window.GetWidth(), IndieLib.CIndieLib.Instance().Window.GetHeight())
			IndieLib.CIndieLib.Instance().Render.SetCamera2d (self.Cameras2d)

			for i in range(0,16):
				IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d ( i )
				if self.bShowBoundsToggle:
					IndieLib.CIndieLib.Instance().Entity2dManager.RenderCollisionAreas (i, 255, 0, 0, 255)
		
			#???IndieLib.CIndieLib.Instance().Render->ShowFpsInWindowTitle("")
			IndieLib.CIndieLib.Instance().Render.EndScene ()
			return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

