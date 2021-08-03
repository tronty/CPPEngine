#
# Copyright (c) 2018 Tommi Roenty   http:#www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http:#opensource.org/licenses/LGPL-2.1
#
import IndieLib

class AppPy:
	def __init__(self):
		meshCube3D=IndieLib.Shape3D()
		meshCantorDust3D=IndieLib.Shape3D()
		texID=-1
		Niterations=2
		epsilon=0.005
		dy=2.0

	def CantorDust(aPosition, aSize, aIterations):
		if not aIterations:
			if False:
				LOG_PRINT="aPosition.x=%f\n" % aPosition.x
				print LOG_PRINT
				LOG_PRINT="aPosition.y=%f\n" % aPosition.y
				print LOG_PRINT
				LOG_PRINT="aPosition.z=%f\n" % aPosition.z
				print LOG_PRINT
				LOG_PRINT="aSize.x=%f\n" % aSize.x
				print LOG_PRINT
				LOG_PRINT="aSize.y=%f\n" % aSize.y
				print LOG_PRINT
				LOG_PRINT="aSize.z=%f\n" % aSize.z
				print LOG_PRINT
				LOG_PRINT="aIterations=%d\n" % aIterations
				print LOG_PRINT
				LOG_PRINT="epsilon=%f\n" % epsilon
				print LOG_PRINT
			return
	
		if aSize.x < epsilon:
			return

		f1=1.0/3.0
		f2=2.0/3.0
		size_=IndieLib.D3DXFROMWINEVECTOR3(f1*aSize.x,f1*aSize.y,f1*aSize.z)
		if False:
			position=IndieLib.D3DXFROMWINEVECTOR3(aPosition.x+f2*size_.x, aPosition.y, aPosition.z)
	 		CantorDust(position, size_, aIterations-1)
		else:
			position=IndieLib.D3DXFROMWINEVECTOR3(aPosition.x, aPosition.y+dy, aPosition.z)
	 		CantorDust(position, size_, aIterations-1)
			position=IndieLib.D3DXFROMWINEVECTOR3(aPosition.x+f2*aSize.x, aPosition.y+dy,aPosition.z+f2*aSize.z)
	 		CantorDust(position, size_, aIterations-1)
			position=IndieLib.D3DXFROMWINEVECTOR3(aPosition.x+f2*aSize.x, aPosition.y+dy,aPosition.z)
	 		CantorDust(position, size_, aIterations-1)
			position=IndieLib.D3DXFROMWINEVECTOR3(aPosition.x, aPosition.y+dy,aPosition.z+f2*aSize.z)
	 		CantorDust(position, size_, aIterations-1)

		if aIterations>1:
			return

		R=IndieLib.D3DXFROMWINEMATRIX()
		S=IndieLib.D3DXFROMWINEMATRIX()
		T=IndieLib.D3DXFROMWINEMATRIX()
		W=IndieLib.D3DXFROMWINEMATRIX()
		IndieLib.D3DXFROMWINEMatrixRotationYawPitchRoll(R, 0.0, 0.0, 0.0)
		IndieLib.D3DXFROMWINEMatrixScaling(S, aSize.x, aSize.y, aSize.z)
		#IndieLib.D3DXFROMWINEMatrixScaling(S, f1, f1, f1)
		IndieLib.D3DXFROMWINEMatrixTranslation(T, aPosition.x, aPosition.y, aPosition.z)
		W=R*S*T
			
		if meshCube3D.indices.size():
			for j in range(0,meshCube3D.indices.size()):
				v=meshCube3D.vertices[meshCube3D.indices[j]]
				v.Position=IndieLib.stx_Vec3Transform(W, v.Position)
				meshCantorDust3D.vertices.push_back(v)
				meshCantorDust3D.indices.push_back(meshCantorDust3D.vertices.size()-1)
	
		else:
			for j in range(0,meshCube3D.vertices.size()):
				v=meshCube3D.vertices[j]
				v.Position=IndieLib.stx_Vec3Transform(W, v.Position)
				meshCantorDust3D.vertices.push_back(v)

	def init(aTitle):
		ss=IndieLib.IRenderer.GetRendererInstance().Getlinear()
		self.texID=IndieLib.IRenderer.GetRendererInstance().addImageLibTexture("/test.bmp",False,ss)

		IRenderer.GetRendererInstance().InitTex3(IRenderer.GetRendererInstance().GettexShader3(), IRenderer.GetRendererInstance().GettexVF3())
		IRenderer.GetRendererInstance().setShader(IRenderer.GetRendererInstance().GettexShader3())
		IRenderer.GetRendererInstance().setVertexFormat(IRenderer.GetRendererInstance().GettexVF3())
		if True:
			awidth=1.0
			aheight=1.0
			adepth=1.0
			meshCube3D.CreateTexturedBox(awidth, aheight, adepth, eSimpleShader)
			meshCantorDust3D.CreateTexturedBox(awidth, aheight, adepth, eSimpleShader)
		if False:
			meshCantorDust3D=meshCube3D
		elif True:
			position=IndieLib.D3DXFROMWINEVECTOR3(0.0, 0.0, 0.0)
			size=IndieLib.D3DXFROMWINEVECTOR3(1.0, 1.0, 1.0)
			meshCantorDust3D.vertices.clear()
			meshCantorDust3D.indices.clear()
			CantorDust(position, size, Niterations)
			LOG_PRINT="meshCube3D.vertices.size()=%d\n" % meshCube3D.vertices.size()
			print LOG_PRINT
			LOG_PRINT="meshCube3D.indices.size()=%d\n" % meshCube3D.indices.size()
			print LOG_PRINT
			LOG_PRINT="meshCantorDust3D.vertices.size()=%d\n" % meshCantorDust3D.vertices.size()
			print LOG_PRINT
			LOG_PRINT="meshCantorDust3D.indices.size()=%d\n" % meshCantorDust3D.indices.size()
			print LOG_PRINT
		return 0
	
	def render(self):
		f=128.0
		IndieLib.IRenderer.GetRendererInstance().Clear ( True,False,IndieLib.D3DXFROMWINEVECTOR4 (f/256.0, f/256.0, f/256.0,1.0))

		IndieLib.IRenderer.GetRendererInstance().BeginScene()
		
		mAngleX=45.0
		mAngleY=45.0
		#IndieLib.stx_GetAngles(mAngleX, mAngleY)

		R=IndieLib.D3DXFROMWINEMATRIX()
		S=IndieLib.D3DXFROMWINEMATRIX()
		T=IndieLib.D3DXFROMWINEMATRIX()
		W=IndieLib.D3DXFROMWINEMATRIX()
		IndieLib.D3DXFROMWINEMatrixRotationYawPitchRoll(R, mAngleX, mAngleY, 0.0)
		IndieLib.D3DXFROMWINEMatrixScaling(S, 0.25, 0.25, 0.25)
		#IndieLib.D3DXFROMWINEMatrixScaling(S, 1.0, 1.0, 1.0)
		IndieLib.D3DXFROMWINEMatrixTranslation(T, 0.0, 0.0, 0.0)
		W=R*S*T

		if True:
			IndieLib.meshCantorDust3D.Draw(W, texID)
		else:
			IndieLib.meshCantorDust3D.render(W)
		IndieLib.IRenderer.GetRendererInstance().EndScene ()
		return 0

if __name__=="__main__":
	app=AppPy()
	IndieLib.IRenderer.GetRendererInstance("CantorDust3D")		
	while ((not IndieLib.STX_Service.GetInputInstance().OnKeyPress (IndieLib.STX_KEY_ESCAPE)) and (not IndieLib.STX_Service.GetInputInstance().Quit())):
		IndieLib.STX_Service.GetInputInstance().Update()	
		app.render()

