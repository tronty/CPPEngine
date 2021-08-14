#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib
import cmath

class AppPy:
	def __init__(self):
		self.mNumBlocksX= 0
		self.mNumBlocksY= 0
		self.mWidthBlock= 0
		self.mHeightBlock= 0
		self.mShowGrid = 0
		self.mAngle = 0.0
		self.mT = 0.0
		self.mText = ""

		self.mSurfaceBack=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceDraco=IndieLib.IND_Factory.GetSurface()
		self.mFontSmall=IndieLib.IND_Factory.GetFont()
		self.mTextSmallWhite=IndieLib.IND_Factory.GetEntity2d()
		self.mBack=IndieLib.IND_Factory.GetEntity2d()
		self.mDraco=IndieLib.IND_Factory.GetEntity2d()
		self.mTimer=IndieLib.IND_Factory.GetTimer()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("b_01_IND_Surface_grids")
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBack, "/twist.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceDraco, "/draco.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.mFontSmall, "/font_small.png", "/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 0

		IndieLib.CIndieLib.Instance().Entity2dManager.Add(self.mTextSmallWhite)
		self.mTextSmallWhite.SetFont			(self.mFontSmall)
		self.mTextSmallWhite.SetLineSpacing	(18)
		self.mTextSmallWhite.SetCharSpacing	(-8)
		self.mTextSmallWhite.SetPosition		(5, 5, 1)
		self.mTextSmallWhite.SetAlign		(IndieLib.IND_LEFT)

		self.mSurfaceDraco.SetGrid (8, 8)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mBack)
		self.mBack.SetSurface (self.mSurfaceBack)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mDraco)
		self.mDraco.SetSurface (self.mSurfaceDraco)

		self.mBack.SetHotSpot (0.5, 0.5)
		self.mBack.SetPosition (400, 300, 0)
		self.mBack.SetScale (1.7, 1.7)

		self.mDraco.SetPosition (150, 50, 1)

		self.mText = ""
	
		self.mNumBlocksX = self.mSurfaceDraco.GetBlocksX()
		self.mNumBlocksY = self.mSurfaceDraco.GetBlocksY()
		self.mWidthBlock = self.mSurfaceDraco.GetWidthBlock()
		self.mHeightBlock = self.mSurfaceDraco.GetHeightBlock()
		return 0

	def render(self):

		IndieLib.CIndieLib.Instance().Input.Update ()

		self.mText = "Press space to see the grid in action. This is really cool, isn't it?"
		self.mTextSmallWhite.SetText	 (self.mText)

		if IndieLib.CIndieLib.Instance().Input.OnKeyPress (IndieLib.STX_KEY_SPACE):
			if self.mShowGrid: 
				self.mShowGrid = 0
			else: 
				self.mShowGrid = 1
		
		self.mAngle += 0.1
		self.mBack.SetAngleXYZ (0, 0, self.mAngle)

		self.mT = self.mTimer.GetTicks() / 1000.0

		for i in range(1, self.mNumBlocksX):
			for j in range(1, self.mNumBlocksY):
				self.mSurfaceDraco.SetVertexPos (j, i, ((j * self.mHeightBlock + cmath.cos (self.mT * 10 + (i + j) / 2) * 5)), ((i * self.mWidthBlock	+ cmath.sin (self.mT * 10 + (i + j) / 2) * 5)))

		IndieLib.CIndieLib.Instance().Render.BeginScene ()
		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60)
		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d ()
		if mShowGrid:
			IndieLib.CIndieLib.Instance().Entity2dManager.RenderGridAreas (0, 0, 0, 255)
		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

