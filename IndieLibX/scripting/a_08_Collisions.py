#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mSurfaceBack=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceRocket=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceBeetle=IndieLib.IND_Factory.GetSurface()
		self.mSwordMasterAnimation=IndieLib.IND_Factory.GetAnimation()
		self.mFontSmall=IndieLib.IND_Factory.GetFont()
		self.mBack=IndieLib.IND_Factory.GetEntity2d()
		self.mRocket=IndieLib.IND_Factory.GetEntity2d()
		self.mBeetle=IndieLib.IND_Factory.GetEntity2d()
		self.mSwordMaster=IndieLib.IND_Factory.GetEntity2d()
		self.mTextSmallWhite=IndieLib.IND_Factory.GetEntity2d()

		self.mAngle = 0.0
		self.mScale = 1.0
		self.mSpeedRotation = 5
		self.mSpeedScaling = 1
		self.mDelta = 0.0

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("a_08_Collisions")
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBack, "/twist.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceRocket, "/rocket.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBeetle, "/beetleship.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().AnimationManager.AddToSurface (self.mSwordMasterAnimation, "/animations/sword_master.xml", IndieLib.IND_ALPHA, IndieLib.IND_16, 0, 255, 0):
			return 0

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.mFontSmall, "/font_small.png", "/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mBack)
		self.mBack.SetSurface (self.mSurfaceBack)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mRocket)
		self.mRocket.SetSurface (self.mSurfaceRocket)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mBeetle)
		self.mBeetle.SetSurface (self.mSurfaceBeetle)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mSwordMaster)
		self.mSwordMaster.SetAnimation (self.mSwordMasterAnimation)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mTextSmallWhite)
		self.mTextSmallWhite.SetFont (self.mFontSmall)

		self.mBack.SetHotSpot (0.5, 0.5)
		self.mBack.SetPosition (400, 300, 0)
		self.mBack.SetScale (1.7, 1.7)

		self.mBeetle.SetHotSpot (0.5, 0.5)
		self.mBeetle.SetMirrorX (True)
		self.mBeetle.SetBoundingTriangle ("beetle_head", 160, 105, 160, 170, 190, 135)
		self.mBeetle.SetBoundingCircle ("beetle_boy_head", 85, 52, 55)

		self.mRocket.SetHotSpot (0.5, 0.5)
		self.mRocket.SetPosition (200, 450, 1)
		self.mRocket.SetBoundingAreas ("/rocket_collisions.xml")
		self.mRocket.DeleteBoundingAreas ("engines")

		self.mSwordMaster.SetHotSpot (0.5, 0.5)
		self.mSwordMaster.SetPosition (400, 220, 3)

		self.mTextSmallWhite.SetLineSpacing	(18)
		self.mTextSmallWhite.SetCharSpacing	(-7)
		self.mTextSmallWhite.SetPosition		(400, 30, 10)
		self.mTextSmallWhite.SetAlign		(IndieLib.IND_CENTER)

		return 0

	def render(self):
		IndieLib.CIndieLib.Instance().Input.Update ()

		self.mDelta = IndieLib.CIndieLib.Instance().Render.GetFrameTime() / 1000.0

		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_RIGHT):
			self.mScale = self.mScale + self.mSpeedScaling * self.mDelta
		if IndieLib.CIndieLib.Instance().Input.IsKeyPressed (IndieLib.STX_KEY_LEFT):
			self.mScale = self.mScale - self.mSpeedScaling * self.mDelta
		self.mAngle = self.mAngle + self.mSpeedRotation * self.mDelta
		if self.mScale < 0:
				self.mScale = 0

		self.mBack.SetAngleXYZ (0, 0, -self.mAngle)
		self.mRocket.SetAngleXYZ (0, 0, self.mAngle)
		self.mRocket.SetScale (self.mScale, self.mScale)
		self.mBeetle.SetPosition (IndieLib.CIndieLib.Instance().Input.GetMouseX(), IndieLib.CIndieLib.Instance().Input.GetMouseY(), 5)

		self.mTextSmallWhite.SetText	("No collision between the groups we are checking")

		if IndieLib.CIndieLib.Instance().Entity2dManager.IsCollision (self.mRocket, "engines", self.mBeetle, "beetle_boy_head"):
			self.mTextSmallWhite.SetText	("Collision between rocket boy head and engines")

		if IndieLib.CIndieLib.Instance().Entity2dManager.IsCollision (self.mRocket, "rocket_head", self.mBeetle, "beetle_head"):
			self.mTextSmallWhite.SetText	("Collision between rokect head and beetle head")

		if IndieLib.CIndieLib.Instance().Entity2dManager.IsCollision (self.mRocket, "rocket_boy_head", self.mBeetle, "beetle_boy_head"):
			self.mTextSmallWhite.SetText	("Collision between rocket boy head and beetle boy head")

		if IndieLib.CIndieLib.Instance().Entity2dManager.IsCollision (self.mBeetle, "beetle_boy_head", self.mSwordMaster, "sword"):
			self.mTextSmallWhite.SetText	("Collision between beetle boy head and the sword")

		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d ()
		IndieLib.CIndieLib.Instance().Entity2dManager.RenderCollisionAreas (255, 0, 0, 255)
		#IndieLib.CIndieLib.Instance().Entity2dManager.RenderGridAreas (255, 255, 0, 255)
		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

