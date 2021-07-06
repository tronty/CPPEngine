#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mSurfaceBack=IndieLib.IND_Factory.GetSurface()
		self.mAnimationCharacter1=IndieLib.IND_Factory.GetAnimation()
		self.mAnimationCharacter2=IndieLib.IND_Factory.GetAnimation()
		self.mAnimationDust=IndieLib.IND_Factory.GetAnimation()
		self.mBack=IndieLib.IND_Factory.GetEntity2d()
		self.mPlayer1=IndieLib.IND_Factory.GetEntity2d()
		self.mPlayer2=IndieLib.IND_Factory.GetEntity2d()
		self.mDust=IndieLib.IND_Factory.GetEntity2d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("a_04_IND_Animation")

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBack, "/blue_background.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32): 
			return 0

		if not IndieLib.CIndieLib.Instance().AnimationManager.AddToSurface (self.mAnimationCharacter1, "/animations/character1.xml", IndieLib.IND_ALPHA, IndieLib.IND_32, 0, 48, 152): 
			return 0

		if not IndieLib.CIndieLib.Instance().AnimationManager.AddToSurface (self.mAnimationCharacter2, "/animations/character2.xml", IndieLib.IND_ALPHA, IndieLib.IND_32, 0, 48, 152): 
			return 0

		if not IndieLib.CIndieLib.Instance().AnimationManager.AddToSurface (self.mAnimationDust, "/animations/dust.xml", IndieLib.IND_ALPHA, IndieLib.IND_16, 255, 0, 255): 
			return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mBack)
		self.mBack.SetSurface (self.mSurfaceBack)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mPlayer1)
		self.mPlayer1.SetAnimation (self.mAnimationCharacter1)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mPlayer2)
		self.mPlayer2.SetAnimation (self.mAnimationCharacter2)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mDust)
		self.mDust.SetAnimation (self.mAnimationDust)
		self.mPlayer1.SetSequence (0)
		self.mPlayer1.SetPosition (140, 200, 0)
		self.mPlayer1.SetMirrorX (True)

		self.mDust.SetPosition (360, 250, 0)

		self.mPlayer2.SetSequence (0)
		self.mPlayer2.SetPosition (550, 200, 0)
		self.mPlayer2.SetNumReplays (3)

		return 0
	
	def render(self):
		IndieLib.CIndieLib.Instance().Input.Update ()
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

