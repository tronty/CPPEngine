#
# Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		
		self.shdTxt='''[Vertex shader]
struct VsIn {
	float2 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main(VsIn In){
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position.x, In.position.y, 0.0, 1.0));
	Out.texCoord = In.texCoord;
	return Out;
}
[Fragment shader]
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
sampler2D Base;
float4 main(VsOut IN): COLOR {
	return tex2D(Base, IN.texCoord);
}
		'''
		self.shd = IndieLib.IRenderer.GetRendererInstance().addHLSLShader(self.shdTxt, "main", "main")
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
		ss=IndieLib.IRenderer.GetRendererInstance().Getlinear()
		self.texture=IndieLib.IRenderer.GetRendererInstance().addImageLibTexture("/test.bmp", False, ss)
		self.v = IndieLib.new_floatArray(16)
		IndieLib.floatArray_setitem(self.v, 0,  1.0)
		IndieLib.floatArray_setitem(self.v, 1, -1.0)
		IndieLib.floatArray_setitem(self.v, 2,  1.0)
		IndieLib.floatArray_setitem(self.v, 3,  1.0)
		IndieLib.floatArray_setitem(self.v, 4,  1.0)
		IndieLib.floatArray_setitem(self.v, 5,  1.0)
		IndieLib.floatArray_setitem(self.v, 6,  1.0)
		IndieLib.floatArray_setitem(self.v, 7,  0.0)
		IndieLib.floatArray_setitem(self.v, 8, -1.0)
		IndieLib.floatArray_setitem(self.v, 9, -1.0)
		IndieLib.floatArray_setitem(self.v, 10,  0.0)
		IndieLib.floatArray_setitem(self.v, 11,  1.0)

		IndieLib.floatArray_setitem(self.v, 12, -1.0)
		IndieLib.floatArray_setitem(self.v, 13,  1.0)
		IndieLib.floatArray_setitem(self.v, 14,  0.0)
		IndieLib.floatArray_setitem(self.v, 15,  0.0)
	def clear(self):
		IndieLib.delete_floatArray(self.v)

	def render(self):
		f=128.0/256.0
		IndieLib.IRenderer.GetRendererInstance().Clear(True, False, IndieLib.D3DXFROMWINEVECTOR4 (f, f, f, 1.0))
		IndieLib.IRenderer.GetRendererInstance().BeginScene()
		IndieLib.IRenderer.GetRendererInstance().setShader(self.shd)
		IndieLib.IRenderer.GetRendererInstance().setVertexFormat(self.vf)
		IndieLib.IRenderer.GetRendererInstance().setTexture("Base", self.texture)
		I=IndieLib.D3DXFROMWINEMATRIX()
		IndieLib.D3DXFROMWINEMatrixIdentity(I)
		IndieLib.IRenderer.GetRendererInstance().setShaderConstant4x4f("worldViewProj", I)
		IndieLib.IRenderer.GetRendererInstance().DrawPrimitiveUP(IndieLib.PRIM_TRIANGLE_STRIP, 2, self.v, self.v, 16)		
		IndieLib.IRenderer.GetRendererInstance().EndScene()
		IndieLib.IRenderer.GetRendererInstance().Present( )

if __name__=="__main__":
	app=AppPy()
	IndieLib.IRenderer.GetRendererInstance("SimpleTexture")		
	while ((not IndieLib.STX_Service.GetInputInstance().OnKeyPress (IndieLib.STX_KEY_ESCAPE)) and (not IndieLib.STX_Service.GetInputInstance().Quit())):
		IndieLib.STX_Service.GetInputInstance().Update()	
		app.render()
	app.clear()

