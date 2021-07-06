#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.fElapsedTime = 0.0
		self.szText=""
		self.iiiAlpha = 255
		self.SurfaceSnake=IndieLib.IND_Factory.GetSurface()
		self.SurfacePool=IndieLib.IND_Factory.GetSurface()
		self.FontSmall=IndieLib.IND_Factory.GetFont()
		self.TextSmallWhite=IndieLib.IND_Factory.GetEntity2d()
		self.PoolEntity2d=IndieLib.IND_Factory.GetEntity2d()
		self.SnakeEntity2d=IndieLib.IND_Factory.GetEntity2d()
		self.timer=IndieLib.IND_Factory.GetTimer()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("IndieLib-Fading")
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.SurfaceSnake, "/IndieLib-Fading/snake.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.SurfacePool, "/IndieLib-Fading/pool.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.FontSmall, "/IndieLib-Fading/font_small.png", "/IndieLib-Fading/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add		(1, self.TextSmallWhite)
		self.TextSmallWhite.SetFont			(self.FontSmall)
		self.TextSmallWhite.SetLineSpacing	(18)
		self.TextSmallWhite.SetCharSpacing	(-8)
		self.TextSmallWhite.SetPosition		(5, 5, 1)
		self.TextSmallWhite.SetAlign		(IndieLib.IND_LEFT)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (0, self.PoolEntity2d)
		self.PoolEntity2d.SetSurface (self.SurfacePool)
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (0, self.SnakeEntity2d)
		self.SnakeEntity2d.SetSurface (self.SurfaceSnake)
	
		self.timer.Start()
		return 0

	def render(self):
		IndieLib.CIndieLib.Instance().Input.Update ()

		self.fElapsedTime = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60)
		IndieLib.CIndieLib.Instance().Render.SetViewPort2d (0, 0, IndieLib.CIndieLib.Instance().Window.GetWidth(), IndieLib.CIndieLib.Instance().Window.GetHeight())

		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d ( 0 )

		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_z):
			self.iiiAlpha=self.iiiAlpha-1
			if self.iiiAlpha < 0:
				self.iiiAlpha = 0
			elif IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_x):
				self.iiiAlpha=self.iiiAlpha+1
				if self.iiiAlpha > 255:
					self.iiiAlpha = 255

			self.SnakeEntity2d.SetTransparency(self.iiiAlpha)

		self.szText = "Z to fade out, X to fade in\n"
		self.szText += "ESC to quit"
		self.TextSmallWhite.SetText	 (self.szText)

		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d ( 1 )

		#???IndieLib.CIndieLib.Instance().Render.ShowFpsInWindowTitle()
		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

