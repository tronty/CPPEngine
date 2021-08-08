#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mTimeString = "" 
		self.mTempTime = ""
		self.mX = 0
		self.mSecond = 0

		self.mFontBig=IndieLib.IND_Factory.GetFont()
		self.mSurfaceDraco=IndieLib.IND_Factory.GetSurface()
		self.mTextTime=IndieLib.IND_Factory.GetEntity2d()
		self.mDraco=IndieLib.IND_Factory.GetEntity2d()
		self.mTimer=IndieLib.IND_Factory.GetTimer()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("a_16_IND_Timer")
		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.mFontBig, "/font_big.png", "/font_big.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceDraco, "/draco.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mTextTime)
		self.mTextTime.SetFont (self.mFontBig)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mDraco)
		self.mDraco.SetSurface (self.mSurfaceDraco)
		self.mDraco.SetHotSpot (0.5, 0.5)
		self.mDraco.SetPosition (400, 330, 1)

		self.mTextTime.SetLineSpacing		(18)
		self.mTextTime.SetCharSpacing		(-8)
		self.mTextTime.SetPosition			(280, 20, 1)
		self.mTextTime.SetAlign			(IndieLib.IND_LEFT)

		self.mTimer.Start()
		self.mTimeString = ""
		return 0

	def render(self):
		IndieLib.CIndieLib.Instance().Input.Update ()

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress (IndieLib.STX_KEY_SPACE):
			if self.mTimer.IsPaused():
				self.mTimer.Unpause() 
			else:
				self.mTimer.Pause()
		

		self.mSecond = self.mTimer.GetTicks() / 1000.0

		self.mTimeString = "Seconds: %d" % self.mSecond

		self.mTextTime.SetText	 (self.mTimeString)

		self.mDraco.SetAngleXYZ (0, 0, self.mSecond + 10)

		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60)
		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d ()
		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

