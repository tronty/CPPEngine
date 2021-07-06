#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.MiddleScreenX=0
		self.MiddleScreenY=0

		self.Camera0PosX=0
		self.Camera1PosX=0
		self.Camera2PosX=0

		self.Layer0Speed		= 50
		self.Layer1Speed		= 162
		self.Layer2Speed		= 250
		self.mText=""
		self.fElapsedTime = 0.0

		self.timer=IndieLib.IND_Factory.GetTimer()
		self.SurfaceSky=IndieLib.IND_Factory.GetSurface()
		self.SurfaceLayer1=IndieLib.IND_Factory.GetSurface()
		self.SurfaceLayer2=IndieLib.IND_Factory.GetSurface()
		self.mFontSmall=IndieLib.IND_Factory.GetFont()
		self.mTextSmallWhite=IndieLib.IND_Factory.GetEntity2d()
		self.SkyEntity2d=IndieLib.IND_Factory.GetEntity2d()
		self.CaveEntity2d=IndieLib.IND_Factory.GetEntity2d()
		self.CaveNearEntity2d=IndieLib.IND_Factory.GetEntity2d()
		self.pCameras=[]
		self.CameraText=IndieLib.IND_Factory.GetCamera2d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("IndieLib-ParallaxScroll")

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.SurfaceSky, "/IndieLib-ParallaxScroll/sky.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.SurfaceLayer1, "/IndieLib-ParallaxScroll/cave.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.SurfaceLayer2, "/IndieLib-ParallaxScroll/cave_near.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.mFontSmall, "/IndieLib-ParallaxScroll/font_small.png", "/IndieLib-ParallaxScroll/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0


		IndieLib.CIndieLib.Instance().Entity2dManager.Add		(3, self.mTextSmallWhite)
		self.mTextSmallWhite.SetFont			(self.mFontSmall)
		self.mTextSmallWhite.SetLineSpacing	(18)
		self.mTextSmallWhite.SetCharSpacing	(-8)
		self.mTextSmallWhite.SetPosition		(5, 5, 1)
		self.mTextSmallWhite.SetAlign		(IndieLib.IND_LEFT)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (0, self.SkyEntity2d)
		self.SkyEntity2d.SetSurface (self.SurfaceSky)
		self.SkyEntity2d.SetPosition(600, 0, 0)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (1, self.CaveEntity2d)
		self.CaveEntity2d.SetSurface (self.SurfaceLayer1)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (2, self.CaveNearEntity2d)
		self.CaveNearEntity2d.SetSurface (self.SurfaceLayer2)

		self.MiddleScreenX = IndieLib.CIndieLib.Instance().Window.GetWidth() / 2
		self.MiddleScreenY = IndieLib.CIndieLib.Instance().Window.GetHeight() / 2

		self.Camera0PosX = self.MiddleScreenX
		self.Camera1PosX = self.MiddleScreenX
		self.Camera2PosX = self.MiddleScreenX

		self.pCameras.append(IndieLib.IND_Factory.GetCamera2d(self.Camera0PosX, self.MiddleScreenY))
		self.pCameras.append(IndieLib.IND_Factory.GetCamera2d(self.Camera1PosX, self.MiddleScreenY))
		self.pCameras.append(IndieLib.IND_Factory.GetCamera2d(self.Camera2PosX, self.MiddleScreenY))

		self.CameraText=IndieLib.IND_Factory.GetCamera2d(IndieLib.CIndieLib.Instance().Window.GetWidth() / 2, IndieLib.CIndieLib.Instance().Window.GetHeight() / 2)

		self.MiddleScreenX = IndieLib.CIndieLib.Instance().Window.GetWidth() / 2
		self.MiddleScreenY = IndieLib.CIndieLib.Instance().Window.GetHeight() / 2

		self.Camera0PosX = self.MiddleScreenX
		self.Camera1PosX = self.MiddleScreenX
		self.Camera2PosX = self.MiddleScreenX

		self.timer.Start()
		return 0

	def render(self):

		IndieLib.CIndieLib.Instance().Input.Update ()

		self.mText="Use right and left arrow keys for moving the camera\n"
		self.mTextSmallWhite.SetText	 (self.mText)

		self.fElapsedTime = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed(IndieLib.STX_KEY_RIGHT):
		
			if self.Camera2PosX < self.SurfaceLayer2.GetWidth() - self.MiddleScreenX:
			
				self.Camera0PosX += self.Layer0Speed * self.fElapsedTime
				self.Camera1PosX += self.Layer1Speed * self.fElapsedTime
				self.Camera2PosX += self.Layer2Speed * self.fElapsedTime
			
		
		elif IndieLib.CIndieLib.Instance().Input.IsKeyPressed(IndieLib.STX_KEY_LEFT):
		
			if self.Camera2PosX > self.MiddleScreenX:
			
				self.Camera0PosX -= self.Layer0Speed * self.fElapsedTime
				self.Camera1PosX -= self.Layer1Speed * self.fElapsedTime
				self.Camera2PosX -= self.Layer2Speed * self.fElapsedTime
			
		

		self.pCameras[0].SetPosition(self.Camera0PosX, self.MiddleScreenY)
		self.pCameras[1].SetPosition(self.Camera1PosX, self.MiddleScreenY)
		self.pCameras[2].SetPosition(self.Camera2PosX, self.MiddleScreenY)

		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60)
		IndieLib.CIndieLib.Instance().Render.SetViewPort2d (0, 0, IndieLib.CIndieLib.Instance().Window.GetWidth(), IndieLib.CIndieLib.Instance().Window.GetHeight())

		for i in range(0,3):
		
			IndieLib.CIndieLib.Instance().Render.SetCamera2d(self.pCameras[i])
			IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d (i)
		

		IndieLib.CIndieLib.Instance().Render.SetCamera2d(self.CameraText)
		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d(3)

		#IndieLib.CIndieLib.Instance().Render.ShowFpsInWindowTitle()
		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

