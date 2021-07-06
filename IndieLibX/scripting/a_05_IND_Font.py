#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mSurfaceBack=IndieLib.IND_Factory.GetSurface()
		self.mSurfaceTip=IndieLib.IND_Factory.GetSurface()
		self.mFontSmall=IndieLib.IND_Factory.GetFont()
		self.mFontBig=IndieLib.IND_Factory.GetFont()
		self.mBack=IndieLib.IND_Factory.GetEntity2d()
		self.mTip=IndieLib.IND_Factory.GetEntity2d()
		self.mTextTitle=IndieLib.IND_Factory.GetEntity2d()
		self.mTextSmallBlack=IndieLib.IND_Factory.GetEntity2d()
		self.mTextSmallWhite=IndieLib.IND_Factory.GetEntity2d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("a_05_IND_Font")
		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceBack, "/blue_background.jpg", IndieLib.IND_OPAQUE, IndieLib.IND_32):
			return 

		if not IndieLib.CIndieLib.Instance().SurfaceManager.Add (self.mSurfaceTip, "/tip.png", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.mFontSmall, "/font_small.png", "/font_small.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 

		if not IndieLib.CIndieLib.Instance().FontManager.Add (self.mFontBig, "/font_big.png", "/font_big.xml", IndieLib.IND_ALPHA, IndieLib.IND_32):
			return 

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mBack)
		self.mBack.SetSurface (self.mSurfaceBack)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mTip)
		self.mTip.SetSurface (self.mSurfaceTip)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mTextTitle)
		self.mTextTitle.SetFont (self.mFontBig)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mTextSmallBlack)
		self.mTextSmallBlack.SetFont (self.mFontSmall)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mTextSmallWhite)
		self.mTextSmallWhite.SetFont (self.mFontSmall)

		self.mTip.SetHotSpot (0.5, 0.5)
		self.mTip.SetPosition (250, 300, 0)

		self.mTextTitle.SetText				("IndieLib Fonts")
		self.mTextTitle.SetLineSpacing		(18)
		self.mTextTitle.SetCharSpacing		(-8)
		self.mTextTitle.SetPosition			(400, 40, 1)
		self.mTextTitle.SetAlign				(IndieLib.IND_CENTER)

		self.mTextSmallBlack.SetText			("This paragraph is\ncentered using.\nIND_CENTER\n\nThis font has\nblack color using\nSetTint() method.\n\nRocks!")
		self.mTextSmallBlack.SetLineSpacing	(18)
		self.mTextSmallBlack.SetCharSpacing	(-8)
		self.mTextSmallBlack.SetPosition		(242, 230, 1)
		self.mTextSmallBlack.SetAlign		(IndieLib.IND_CENTER)
		self.mTextSmallBlack.SetTint			(0, 0, 0)

		self.mTextSmallWhite.SetText			("This is a another paragraph. This time\nusing the font without changing the color\nand with a IND_LEFT alignment.\n\nThis is a really easy way of writing\ntexts, isn't it?")
		self.mTextSmallWhite.SetLineSpacing	(18)
		self.mTextSmallWhite.SetCharSpacing	(-7)
		self.mTextSmallWhite.SetPosition		(400, 230, 1)
		self.mTextSmallWhite.SetAlign		(IndieLib.IND_LEFT)
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

