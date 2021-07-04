#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		
		self.v = IndieLib.new_floatArray(16)
		self.effect2=-1
		self.texture=-1
		self.texture2=-1
		self.texture3=-1
		self.texture4=-1

	def init(self, width,  height):
		ss=IndieLib.IRenderer.GetRendererInstance().GetlinearClamp()
		texture=IndieLib.IRenderer.GetRendererInstance().addImageLibTexture("/test.bmp",False,ss)
		texture2=IndieLib.IRenderer.GetRendererInstance().addImageLibTexture("/dx8sdk/tree01S.tga",False,ss)
		texture3=IndieLib.IRenderer.GetRendererInstance().addImageLibTexture("/AdvAniCh12/Particle_People1.png",False,ss)
		texture4=IndieLib.IRenderer.GetRendererInstance().addImageLibTexture("/NSGUISWIG/GUIElements.png",False,ss)

	def drawTexture(self, texture, x, y, sx, sy, InScene, tx, ty, stx, sty):
		IndieLib.IRenderer.GetRendererInstance().InitTex(IndieLib.IRenderer.GetRendererInstance().GettexShader(),IndieLib.IRenderer.GetRendererInstance().GettexVF())

		qx=x
		qy=y
		dx=sx
		dy=sy

		fq=((tx==0.0)and(ty==0.0)and(stx==1.0)and(sty==1.0))
		if fq:
			IndieLib.floatArray_setitem(self.v, 0,  qx+dx)
			IndieLib.floatArray_setitem(self.v, 1,  qy)
			IndieLib.floatArray_setitem(self.v, 2,  1.0)
			IndieLib.floatArray_setitem(self.v, 3,  0.0)
			IndieLib.floatArray_setitem(self.v, 4,  qx+dx)
			IndieLib.floatArray_setitem(self.v, 5,  qy+dy)
			IndieLib.floatArray_setitem(self.v, 6,  1.0)
			IndieLib.floatArray_setitem(self.v, 7,  1.0)
			IndieLib.floatArray_setitem(self.v, 8,  qx)
			IndieLib.floatArray_setitem(self.v, 9,  qy)
			IndieLib.floatArray_setitem(self.v, 10,  0.0)
			IndieLib.floatArray_setitem(self.v, 11,  0.0)
			IndieLib.floatArray_setitem(self.v, 12,  qx)
			IndieLib.floatArray_setitem(self.v, 13,  qy+dy)
			IndieLib.floatArray_setitem(self.v, 14,  0.0)
			IndieLib.floatArray_setitem(self.v, 15,  1.0)
		else:
			IndieLib.floatArray_setitem(self.v, 0,  qx+dx)
			IndieLib.floatArray_setitem(self.v, 1,  qy)
			IndieLib.floatArray_setitem(self.v, 2,  tx+stx)
			IndieLib.floatArray_setitem(self.v, 3,  ty)
			IndieLib.floatArray_setitem(self.v, 4,  qx+dx)
			IndieLib.floatArray_setitem(self.v, 5,  qy+dy)
			IndieLib.floatArray_setitem(self.v, 6,  tx+stx)
			IndieLib.floatArray_setitem(self.v, 7,  ty+sty)
			IndieLib.floatArray_setitem(self.v, 8,  qx)
			IndieLib.floatArray_setitem(self.v, 9,  qy)
			IndieLib.floatArray_setitem(self.v, 10,  tx)
			IndieLib.floatArray_setitem(self.v, 11,  ty)
			IndieLib.floatArray_setitem(self.v, 12,  qx)
			IndieLib.floatArray_setitem(self.v, 13,  qy+dy)
			IndieLib.floatArray_setitem(self.v, 14,  tx)
			IndieLib.floatArray_setitem(self.v, 15,  ty+sty)

		if not InScene:
			IndieLib.IRenderer.GetRendererInstance().BeginScene()
		IndieLib.IRenderer.GetRendererInstance().setShader(IndieLib.IRenderer.GetRendererInstance().GettexShader())
		IndieLib.IRenderer.GetRendererInstance().setVertexFormat(IndieLib.IRenderer.GetRendererInstance().GettexVF())
		IndieLib.IRenderer.GetRendererInstance().setTexture("Base", texture)
		IndieLib.IRenderer.GetRendererInstance().setShaderConstant4f("colorRGBA", IndieLib.D3DXFROMWINEVECTOR4(1.0,1.0,1.0,1.0))
		IndieLib.IRenderer.GetRendererInstance().setBlendState(IndieLib.IRenderer.GetRendererInstance().GetblendSrcAlpha())
		IndieLib.IRenderer.GetRendererInstance().setDepthState(IndieLib.IRenderer.GetRendererInstance().GetnoDepthTest())
		IndieLib.IRenderer.GetRendererInstance().setup2DMode(0, IndieLib.IRenderer.GetRendererInstance().GetViewportWidth(), 0, IndieLib.IRenderer.GetRendererInstance().GetViewportHeight())
		IndieLib.IRenderer.GetRendererInstance().setShaderConstant4f("scaleBias", IndieLib.IRenderer.GetRendererInstance().GetscaleBias2D())

		IndieLib.IRenderer.GetRendererInstance().DrawPrimitiself.veUP(IndieLib.PRIM_TRIANGLE_STRIP, IndieLib.IRenderer.GetRendererInstance().getPrimitiveCount(IndieLib.PRIM_TRIANGLE_STRIP, 4), self.v, self.v, 16)
		if not InScene:
			IndieLib.IRenderer.GetRendererInstance().EndScene()

	def drawBlending(self):
		s=0
		drawTexture(texture, 0, 0, 400, 300, True, 0.0, 0.0, 1.0, 1.0)
		drawTexture(texture2, 400, 300, 400, 300, True, 0.0, 0.0, 1.0, 1.0)

	def renderframe(self, aposition, asize, texid, atex=IndieLib.D3DXFROMWINEVECTOR2(0.0, 0.0), atexsize=IndieLib.D3DXFROMWINEVECTOR2(1.0, 1.0)):
		x=aposition.x
		y=aposition.y

		IndieLib.floatArray_setitem(self.v, 0, x + asize.x)
		IndieLib.floatArray_setitem(self.v, 1, y)
		IndieLib.floatArray_setitem(self.v, 2, atex.x + atexsize.x)
		IndieLib.floatArray_setitem(self.v, 3, atex.y)
		IndieLib.floatArray_setitem(self.v, 4, x + asize.x)
		IndieLib.floatArray_setitem(self.v, 5, y + asize.y)
		IndieLib.floatArray_setitem(self.v, 6, atex.x + atexsize.x)
		IndieLib.floatArray_setitem(self.v, 7, atex.y + atexsize.y)
		IndieLib.floatArray_setitem(self.v, 8, x)
		IndieLib.floatArray_setitem(self.v, 9, y)
		IndieLib.floatArray_setitem(self.v, 10, atex.x)
		IndieLib.floatArray_setitem(self.v, 11, atex.y)
		IndieLib.floatArray_setitem(self.v, 12, x)
		IndieLib.floatArray_setitem(self.v, 13, y + asize.y)
		IndieLib.floatArray_setitem(self.v, 14, atex.x)
		IndieLib.floatArray_setitem(self.v, 15, atex.y + atexsize.y)
		IndieLib.IRenderer.GetRendererInstance().drawTexturedV6(IndieLib.PRIM_TRIANGLE_STRIP, self.v, 2, texid, IndieLib.IRenderer.GetRendererInstance().GetblendSrcAlpha(), IndieLib.IRenderer.GetRendererInstance().GetnoDepthTest())

	def render5(self, texture, qx, qy, dx, dy, tx=0.0, ty=0.0, stx=1.0, sty=1.0):
		c=IndieLib.D3DXFROMWINEVECTOR4(1.0, 1.0, 1.0, 1.0)
		IndieLib.IRenderer.GetRendererInstance().Begin(IndieLib.PRIM_TRIANGLE_STRIP)

		sizeX=dx
		sizeY=dy

		IndieLib.IRenderer.GetRendererInstance().TexCoord2f( sty, sty)
		IndieLib.IRenderer.GetRendererInstance().Vertex2f(sizeX,-sizeY)
		IndieLib.IRenderer.GetRendererInstance().TexCoord2f( sty, 0)
		IndieLib.IRenderer.GetRendererInstance().Vertex2f(sizeX,sizeY)

		IndieLib.IRenderer.GetRendererInstance().TexCoord2f( 0, sty)
		IndieLib.IRenderer.GetRendererInstance().Vertex2f( -sizeX,-sizeY)

		IndieLib.IRenderer.GetRendererInstance().TexCoord2f( 0, 0)
		IndieLib.IRenderer.GetRendererInstance().Vertex2f( -sizeX,sizeY)

		IndieLib.IRenderer.GetRendererInstance().End(texture)
	def render4(self, texture, qx, qy, dx, dy, tx=0.0, ty=0.0, stx=1.0, sty=1.0):
		c=IndieLib.D3DXFROMWINEVECTOR4(1.0, 1.0, 1.0, 1.0)
		IndieLib.IRenderer.GetRendererInstance().Begin(IndieLib.PRIM_TRIANGLE_STRIP)

		IndieLib.IRenderer.GetRendererInstance().TexCoord2f(1, 0)
		IndieLib.IRenderer.GetRendererInstance().Vertex2f(qx,qy+dy)
		IndieLib.IRenderer.GetRendererInstance().TexCoord2f(1, 1)
		IndieLib.IRenderer.GetRendererInstance().Vertex2f(qx,qy)
		IndieLib.IRenderer.GetRendererInstance().TexCoord2f(0, 0)
		IndieLib.IRenderer.GetRendererInstance().Vertex2f(qx+dx,qy+dy)
		IndieLib.IRenderer.GetRendererInstance().TexCoord2f(0, 1)
		IndieLib.IRenderer.GetRendererInstance().Vertex2f(qx+dx,qy)

		IndieLib.IRenderer.GetRendererInstance().End(texture)
	def render3(self, texture, qx, qy, dx, dy, tx=0.0, ty=0.0, stx=1.0, sty=1.0):
		c=IndieLib.D3DXFROMWINEVECTOR4(1.0, 1.0, 1.0, 1.0)

		xPos=qx
		yPos=qy
		height=dx
		
		IndieLib.IRenderer.GetRendererInstance().drawTexture(texture, qx, qy, dx, dy, True, tx, ty, stx, sty)

		
		IndieLib.floatArray_setitem(self.v, 0, qx+dx)
		IndieLib.floatArray_setitem(self.v, 1, qy)
		IndieLib.floatArray_setitem(self.v, 2, tx+stx)
		IndieLib.floatArray_setitem(self.v, 3, ty)
		IndieLib.floatArray_setitem(self.v, 4, qx+dx)
		IndieLib.floatArray_setitem(self.v, 5, qy+dy)
		IndieLib.floatArray_setitem(self.v, 6, tx+stx)
		IndieLib.floatArray_setitem(self.v, 7, ty+sty)
		IndieLib.floatArray_setitem(self.v, 8, qx)
		IndieLib.floatArray_setitem(self.v, 9, qy)
		IndieLib.floatArray_setitem(self.v, 10, tx)
		IndieLib.floatArray_setitem(self.v, 11, ty)
		IndieLib.floatArray_setitem(self.v, 12, qx)
		IndieLib.floatArray_setitem(self.v, 13, qy+dy)
		IndieLib.floatArray_setitem(self.v, 14, tx)
		IndieLib.floatArray_setitem(self.v, 15, ty+sty)

		IndieLib.IRenderer.GetRendererInstance().drawTexturedV7(IndieLib.PRIM_TRIANGLE_STRIP, self.v, 4, texture, IndieLib.IRenderer.GetRendererInstance().GetblendSrcAlpha(), IndieLib.IRenderer.GetRendererInstance().GetnoDepthTest(), c)

	def render(self):
		f=128.0
		IndieLib.IRenderer.GetRendererInstance().Clear ( True,False,IndieLib.D3DXFROMWINEVECTOR4 (f/256.0, f/256.0, f/256.0,1))

		IndieLib.IRenderer.GetRendererInstance().BeginScene()

		#drawBlending()

		IndieLib.IRenderer.GetRendererInstance().drawLine(0,0,10,10,IndieLib.D3DXFROMWINEVECTOR4(1,1,1,1))
		IndieLib.IRenderer.GetRendererInstance().drawRect(10,10,20,20,IndieLib.D3DXFROMWINEVECTOR4(1,1,1,1))#,1.0)
		IndieLib.IRenderer.GetRendererInstance().drawRoundRect(20,20,30,30,5.0,IndieLib.D3DXFROMWINEVECTOR4(1,1,1,1))#,1.0)
		IndieLib.IRenderer.GetRendererInstance().drawTriangle(30,30,40,40,50,30,IndieLib.D3DXFROMWINEVECTOR4(1,1,1,1))#,1.0)
		IndieLib.IRenderer.GetRendererInstance().drawQuad(40,40,40,50,50,50,50,40,IndieLib.D3DXFROMWINEVECTOR4(1,1,1,1))
		IndieLib.IRenderer.GetRendererInstance().drawCircle(100,100,5.0,IndieLib.D3DXFROMWINEVECTOR4(1,1,1,1))#,1.0)

		self.renderframe(IndieLib.D3DXFROMWINEVECTOR2(100.0,100.0), IndieLib.D3DXFROMWINEVECTOR2(20.0,20.0), self.texture2)
		self.renderframe(IndieLib.D3DXFROMWINEVECTOR2(250.0,250.0), IndieLib.D3DXFROMWINEVECTOR2(30.0,30.0), self.texture3)
		self.renderframe(IndieLib.D3DXFROMWINEVECTOR2(400.0,400.0), IndieLib.D3DXFROMWINEVECTOR2(50.0,50.0), self.texture)

		self.render5(texture4, 150.0,160.0,50.0,50.0,0.0, 0.0,1.0, 1.0)
		self.render5(texture4, 140.0,150.0,10.0,10.0,0.86, 1.0,0.8, 1.0)

		IndieLib.IRenderer.GetRendererInstance().EndScene()
		IndieLib.IRenderer.GetRendererInstance().Present( )
	def clear(self):
		IndieLib.delete_floatArray(self.v)
if __name__=="__main__":
	app=AppPy()
	IndieLib.IRenderer.GetRendererInstance("Blending")		
	while ((not IndieLib.STX_Service.GetInputInstance().OnKeyPress (IndieLib.STX_KEY_ESCAPE)) and (not IndieLib.STX_Service.GetInputInstance().Quit())):
		IndieLib.STX_Service.GetInputInstance().Update()	
		app.render()
	app.clear()

