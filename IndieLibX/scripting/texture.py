#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		
		self.v = IndieLib.new_floatArray(16)
		self.shd=-1
		self.texture=-1
		self.vf=-1
		self.shdTxt='''[Vertex shader]
struct VsIn {
	float2 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
float4 scaleBias=float4(0.003,-0.003,-1,1);
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
	Out.position.xy = In.position.xy * scaleBias.xy + scaleBias.zw;
	Out.position.w= WSIGN 1;
	Out.texCoord = In.texCoord;
	return Out;
}
[Fragment shader]
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
sampler2D Base;
float4 colorRGBA=float4(1,1,1,1);
float4 main(VsOut IN): COLOR {
	return tex2D(Base, IN.texCoord)* colorRGBA;
}'''

	def init(self, width,  height):
		self.shd = IndieLib.IRenderer.GetRendererInstance().addHLSLShader(self.shdTxt, main, main)
		a = IndieLib.new_intArray(8)
		IndieLib.intArray_setitem(a, 0, 0)
		IndieLib.intArray_setitem(a, 1, IndieLib.TYPE_VERTEX)
		IndieLib.intArray_setitem(a, 2, IndieLib.FORMAT_FLOAT)
		IndieLib.intArray_setitem(a, 3, 2)
		IndieLib.intArray_setitem(a, 4, 0)
		IndieLib.intArray_setitem(a, 5, IndieLib.TYPE_TEXCOORD)
		IndieLib.intArray_setitem(a, 6, IndieLib.FORMAT_FLOAT)
		IndieLib.intArray_setitem(a, 7, 2)
		self.vf = IndieLib.IRenderer.GetRendererInstance().addVertexFormatV(a, 2, self.shd)
		IndieLib.delete_intArray(a)
		ss=IndieLib.IRenderer.GetRendererInstance().GetlinearClamp()

		self.texture=IndieLib.IRenderer.GetRendererInstance().addImageLibTexture("/NSGUISWIG/GUIElements.png",False,ss)
	def RenderTex2(self, aposition, asize, texposition=IndieLib.D3DXFROMWINEVECTOR2(0.0, 0.0), texsize=IndieLib.D3DXFROMWINEVECTOR2(1.0, 1.0)):
		col=IndieLib.D3DXFROMWINEVECTOR4(1.0, 1.0, 1.0, 1.0)
		x=aposition.x
		y=aposition.y
		IndieLib.floatArray_setitem(self.v, 0, x + asize.x)
		IndieLib.floatArray_setitem(self.v, 1, y)
		IndieLib.floatArray_setitem(self.v, 2, texposition.x+texsize.x)
		IndieLib.floatArray_setitem(self.v, 3, texposition.y)
		IndieLib.floatArray_setitem(self.v, 4, x + asize.x)
		IndieLib.floatArray_setitem(self.v, 5, y + asize.y)
		IndieLib.floatArray_setitem(self.v, 6, texposition.x+texsize.x)
		IndieLib.floatArray_setitem(self.v, 7, texposition.y+texsize.y)
		IndieLib.floatArray_setitem(self.v, 8, x)
		IndieLib.floatArray_setitem(self.v, 9, y)
		IndieLib.floatArray_setitem(self.v, 10, texposition.x)
		IndieLib.floatArray_setitem(self.v, 11, texposition.y)
		IndieLib.floatArray_setitem(self.v, 12, x)
		IndieLib.floatArray_setitem(self.v, 13, y + asize.y)
		IndieLib.floatArray_setitem(self.v, 14, texposition.x)
		IndieLib.floatArray_setitem(self.v, 15, texposition.y+texsize.y)
		IndieLib.IRenderer.GetRendererInstance().drawTexturedV7(IndieLib.PRIM_TRIANGLE_STRIP, self.v, 2, self.texture, IndieLib.IRenderer.GetRendererInstance().GetblendSrcAlpha(), IndieLib.IRenderer.GetRendererInstance().GetnoDepthTest(), col)
	def RenderTex(self, id, aposition, asize, texposition=IndieLib.D3DXFROMWINEVECTOR2(0.0, 0.0), texsize=IndieLib.D3DXFROMWINEVECTOR2(1.0, 1.0)):
		col=IndieLib.D3DXFROMWINEVECTOR4(1.0, 1.0, 1.0, 1.0)
		x=aposition.x
		y=aposition.y
		IndieLib.floatArray_setitem(self.v, 0, x + asize.x)
		IndieLib.floatArray_setitem(self.v, 1, y)
		IndieLib.floatArray_setitem(self.v, 2, texposition.x+texsize.x)
		IndieLib.floatArray_setitem(self.v, 3, texposition.y)
		IndieLib.floatArray_setitem(self.v, 4, x + asize.x)
		IndieLib.floatArray_setitem(self.v, 5, y + asize.y)
		IndieLib.floatArray_setitem(self.v, 6, texposition.x+texsize.x)
		IndieLib.floatArray_setitem(self.v, 7, texposition.y+texsize.y)
		IndieLib.floatArray_setitem(self.v, 8, x)
		IndieLib.floatArray_setitem(self.v, 9, y)
		IndieLib.floatArray_setitem(self.v, 10, texposition.x)
		IndieLib.floatArray_setitem(self.v, 11, texposition.y)
		IndieLib.floatArray_setitem(self.v, 12, x)
		IndieLib.floatArray_setitem(self.v, 13, y + asize.y)
		IndieLib.floatArray_setitem(self.v, 14, texposition.x)
		IndieLib.floatArray_setitem(self.v, 15, texposition.y+texsize.y)
		IndieLib.IRenderer.GetRendererInstance().setShader(self.shd)
		IndieLib.IRenderer.GetRendererInstance().setup2DMode(0, IndieLib.IRenderer.GetRendererInstance().GetViewportWidth(), 0, IndieLib.IRenderer.GetRendererInstance().GetViewportHeight())
		IndieLib.IRenderer.GetRendererInstance().setShaderConstant4f("scaleBias", IndieLib.IRenderer.GetRendererInstance().GetscaleBias2D())
		IndieLib.IRenderer.GetRendererInstance().setShaderConstant4f("colorRGBA", col)
		IndieLib.IRenderer.GetRendererInstance().setTexture("Base", id)
		IndieLib.IRenderer.GetRendererInstance().setBlendState(IndieLib.IRenderer.GetRendererInstance().GetblendSrcAlpha())
		IndieLib.IRenderer.GetRendererInstance().setDepthState(IndieLib.IRenderer.GetRendererInstance().GetnoDepthTest())
		IndieLib.IRenderer.GetRendererInstance().setVertexFormat(self.vf)
		IndieLib.IRenderer.GetRendererInstance().DrawPrimitiveUP(IndieLib.PRIM_TRIANGLE_STRIP, 2, self.v, self.v, 16)
	def render(self):
		f=128.0
		IndieLib.IRenderer.GetRendererInstance().Clear ( True,False,IndieLib.D3DXFROMWINEVECTOR4 (f/256.0, f/256.0, f/256.0,1.0))

		IndieLib.IRenderer.GetRendererInstance().BeginScene()

		self.RenderTex(self.texture, IndieLib.D3DXFROMWINEVECTOR2(0.0,0.0), IndieLib.D3DXFROMWINEVECTOR2(100.0,100.0))
	

		self.RenderTex(self.texture, IndieLib.D3DXFROMWINEVECTOR2(0.0,100.0), IndieLib.D3DXFROMWINEVECTOR2(25.0,100.0), IndieLib.D3DXFROMWINEVECTOR2(0.0,0.57), IndieLib.D3DXFROMWINEVECTOR2(0.242,1.1))
	

		self.RenderTex(self.texture, IndieLib.D3DXFROMWINEVECTOR2(0.0,150.0), IndieLib.D3DXFROMWINEVECTOR2(25.0,100.0), IndieLib.D3DXFROMWINEVECTOR2(0.26,0.57), IndieLib.D3DXFROMWINEVECTOR2(0.242,1.1))

		self.RenderTex(self.texture, IndieLib.D3DXFROMWINEVECTOR2(0.0,200.0), IndieLib.D3DXFROMWINEVECTOR2(25.0,100.0), IndieLib.D3DXFROMWINEVECTOR2(0.53,0.57), IndieLib.D3DXFROMWINEVECTOR2(0.121,0.55))

		self.RenderTex(self.texture, IndieLib.D3DXFROMWINEVECTOR2(0.0,250.0), IndieLib.D3DXFROMWINEVECTOR2(25.0,100.0), IndieLib.D3DXFROMWINEVECTOR2(0.66,0.57), IndieLib.D3DXFROMWINEVECTOR2(0.121,0.55))

		self.RenderTex(self.texture, IndieLib.D3DXFROMWINEVECTOR2(0.0,300.0), IndieLib.D3DXFROMWINEVECTOR2(25.0,100.0), IndieLib.D3DXFROMWINEVECTOR2(0.79,0.57), IndieLib.D3DXFROMWINEVECTOR2(0.121,0.55))

		col=IndieLib.D3DXFROMWINEVECTOR4(1.0, 1.0, 1.0, 1.0)
		xPos = 100.0
		yTop = 100.0
		IndieLib.IRenderer.GetRendererInstance().setDepthState(IndieLib.IRenderer.GetRendererInstance().GetnoDepthTest())
		IndieLib.IRenderer.GetRendererInstance().setBlendState(IndieLib.IRenderer.GetRendererInstance().GetblendSrcAlpha())
		# Radio button
		IndieLib.IRenderer.GetRendererInstance().drawCircle(xPos + 17, yTop + 20, 7, col, 2)
		IndieLib.IRenderer.GetRendererInstance().drawCircle(xPos + 17, yTop + 20, 4, col)
		yTop = 200.0
		# Checkbox
		IndieLib.IRenderer.GetRendererInstance().drawRect(xPos + 8, yTop + 11, xPos + 24, yTop + 28, col, 2)
		# Check
		IndieLib.IRenderer.GetRendererInstance().drawLine(xPos + 11, yTop + 18, xPos + 15, yTop + 25, col, 2)
		IndieLib.IRenderer.GetRendererInstance().drawLine(xPos + 15, yTop + 25, xPos + 21, yTop + 14, col, 2)

		self.RenderTex2(IndieLib.D3DXFROMWINEVECTOR2(200.0,200.0), IndieLib.D3DXFROMWINEVECTOR2(25.0,100.0), IndieLib.D3DXFROMWINEVECTOR2(0.53,0.57), IndieLib.D3DXFROMWINEVECTOR2(0.121,0.55))

		IndieLib.IRenderer.GetRendererInstance().EndScene()
		IndieLib.IRenderer.GetRendererInstance().Present( )
	def clear(self):
		IndieLib.delete_floatArray(self.v)

if __name__=="__main__":
	app=AppPy()
	IndieLib.IRenderer.GetRendererInstance("texture")		
	while ((not IndieLib.STX_Service.GetInputInstance().OnKeyPress (IndieLib.STX_KEY_ESCAPE)) and (not IndieLib.STX_Service.GetInputInstance().Quit())):
		IndieLib.STX_Service.GetInputInstance().Update()	
		app.render()
	app.clear()

