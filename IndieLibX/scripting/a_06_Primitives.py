#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		self.mPixel=IndieLib.IND_Factory.GetEntity2d()
		self.mRegularPoly=IndieLib.IND_Factory.GetEntity2d()
		self.mLine=IndieLib.IND_Factory.GetEntity2d()
		self.mRectangle=IndieLib.IND_Factory.GetEntity2d()
		self.mFillRectangle=IndieLib.IND_Factory.GetEntity2d()
		self.mPoly2d=IndieLib.IND_Factory.GetEntity2d()

	def init(self, width, height):
		IndieLib.CIndieLib.Instance()
		IndieLib.CIndieLib.Instance().Init("a_06_Primitives")
		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mPixel)
		self.mPixel.SetPrimitive2d (IndieLib.IND_PIXEL)
		self.mPixel.SetPosition (200, 75, 0)
		self.mPixel.SetTint (255, 255, 255)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mRegularPoly)
		self.mRegularPoly.SetPrimitive2d (IndieLib.IND_REGULAR_POLY)
		self.mRegularPoly.SetPosition (200, 75, 0)
		self.mRegularPoly.SetNumSides (5)
		self.mRegularPoly.SetRadius (50)
		self.mRegularPoly.SetTint (255, 0, 0)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mLine)
		self.mLine.SetPrimitive2d (IndieLib.IND_LINE)
		self.mLine.SetLine (100, 100, 50, 50)
		self.mLine.SetTint (0, 255, 0)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mRectangle)
		self.mRectangle.SetPrimitive2d (IndieLib.IND_RECTANGLE)
		self.mRectangle.SetRectangle (350, 50, 400, 100)
		self.mRectangle.SetTint (0, 0, 255)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mFillRectangle)
		self.mFillRectangle.SetPrimitive2d (IndieLib.IND_FILL_RECTANGLE)
		self.mFillRectangle.SetRectangle (550, 40, 650, 110)
		self.mFillRectangle.SetTint (255, 0, 0)
		self.mFillRectangle.SetTransparency (50)

		IndieLib.CIndieLib.Instance().Entity2dManager.Add (self.mPoly2d)
		self.mPoly2d.SetPrimitive2d (IndieLib.IND_POLY2D)
		self.mVertPoly2 = IndieLib.new_D3DXFROMWINEVECTOR2Array(4)
		IndieLib.D3DXFROMWINEVECTOR2Array_setitem(self.mVertPoly2, 0, IndieLib.D3DXFROMWINEVECTOR2(440, 200 ))
		IndieLib.D3DXFROMWINEVECTOR2Array_setitem(self.mVertPoly2, 1, IndieLib.D3DXFROMWINEVECTOR2(480, 100 ))
		IndieLib.D3DXFROMWINEVECTOR2Array_setitem(self.mVertPoly2, 2, IndieLib.D3DXFROMWINEVECTOR2(450, 10  ))
		IndieLib.D3DXFROMWINEVECTOR2Array_setitem(self.mVertPoly2, 3, IndieLib.D3DXFROMWINEVECTOR2(470, 220 ))
		#self.mPoly2d.SetPolyPoints (self.mVertPoly2)
		IndieLib.delete_D3DXFROMWINEVECTOR2Array(self.mVertPoly2)
		self.mPoly2d.SetNumLines	(3)
		self.mPoly2d.SetTint (255, 128, 255)
		return 0
	
	def render(self):
		IndieLib.CIndieLib.Instance().Input.Update ()

		IndieLib.CIndieLib.Instance().Render.ClearViewPort (0, 0, 0)
		IndieLib.CIndieLib.Instance().Render.BeginScene ()

		i = 0
		while i < 400:
		
			IndieLib.CIndieLib.Instance().Render.BlitLine		(70, 150, i * 2, 500, i, 255 -i, 255, 255)
			IndieLib.CIndieLib.Instance().Render.BlitRegularPoly	(600, 600, i, 70, 0, 255 - i, i, i*4, 0xff)
			i = i + 5
		

		IndieLib.CIndieLib.Instance().Entity2dManager.RenderEntities2d()
		IndieLib.CIndieLib.Instance().Render.EndScene ()
		return 0

	def run(self):
		self.init(0,0)
		while not IndieLib.CIndieLib.Instance().Input.IsKeyUp(IndieLib.STX_KEY_ESCAPE):
			self.render()

if __name__=="__main__":
	app=AppPy()
	app.run()

