#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mDisp = 0.0
		self.mSpeed = 0.5
		self.mDelta = 0.0

		self.mAnimationSun=IndieLib.IND_Factory.GetAnimation()
		self.mSun=IndieLib.IND_Factory.GetEntity2d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("a_11_Animated_Tile_Scrolling")

		if not IndieLib.CIndieLib.Instance().AnimationManager.AddToSurface (self.mAnimationSun, "/animations/sun.xml", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mSun)
		self.mSun.SetAnimation (self.mAnimationSun)

		self.mSun.ToggleWrap (True)
		self.mSun.SetRegion (0, 0, 800, 600)

		return 0

	def render(self):
		IndieLib.CIndieLib.Instance().Input.Update ()
		self.mDelta = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		self.mDisp = self.mDisp + self.mSpeed * self.mDelta
		self.mSun.SetWrapDisplacement (self.mDisp, self.mDisp)

		IndieLib.CIndieLib.Instance().Render.BeginScene ()
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

