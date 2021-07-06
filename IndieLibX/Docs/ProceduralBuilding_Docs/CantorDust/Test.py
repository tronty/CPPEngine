#
# Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#	 http://opensource.org/licenses/LGPL-2.1
#

import math
import IndieLib

def Vec3TransformCoord(v, m):
	norm = m[3] * v[0] + m[4+3] * v[1] + m[8+3] *v[2] + m[12+3]
	outx = (m[0] * v[0] + m[4+0] * v[1] + m[8+0] * v[2] + m[12+0]) / norm
	outy = (m[1] * v[0] + m[4+1] * v[1] + m[8+1] * v[2] + m[12+1]) / norm
	outz = (m[2] * v[0] + m[4+2] * v[1] + m[8+2] * v[2] + m[12+2]) / norm
	return [outx, outy, outz]

def MatrixIdentity():
	m=[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
	return m

def MatrixMultiply(m1, m2):
	m=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
	for i in range(0,4):
		for j in range(0,4):
			m[4*i+j] = m1[4*i] * m2[j] + m1[4*i+1] * m2[4+j] + m1[4*i+2] * m2[8+j] + m1[4*i+3] * m2[12+j]
        return m

def DumpMatrix(m):
	for i in range(0,4):
		for j in range(0,4):
			l='m[%d,%d]=%f' % (i,j,m[4*i+j])
			print l

def DumpVec3(v):
	for i in range(0,3):
		l='v3[%d]=%f' % (i,v[i])
		print l

def MatrixScaling(sx, sy, sz):
	m=MatrixIdentity()
	m[0] = sx
	m[5] = sy
    	m[10] = sz
    	return m

def MatrixTranslation(x, y, z):
	m=MatrixIdentity()
	m[3] = x
	m[7] = y
	m[11] = z
	return m

def MatrixRotationX(aangle):
	angle=aangle*math.pi/180.0
	m=MatrixIdentity()
	m[5] = math.cos(angle)
	m[10] = math.cos(angle)
	m[6] = math.sin(angle)
	m[9] = -math.sin(angle)
	return m

def MatrixRotationY(aangle):
	angle=aangle*math.pi/180.0
	m=MatrixIdentity()
	m[0] = math.cos(angle)
	m[10] = math.cos(angle)
	m[2] = -math.sin(angle)
	m[8] = math.sin(angle)
	return m

def MatrixRotationZ(aangle):
	angle=aangle*math.pi/180.0
	m=MatrixIdentity()
	m[0] = math.cos(angle)
	m[5] = math.cos(angle)
	m[1] = math.sin(angle)
	m[4] = -math.sin(angle)
	return m

def MatrixRotationYawPitchRoll(roll, pitch, yaw):
	mout=MatrixIdentity()
	m=MatrixRotationZ(roll)
	mout=MatrixMultiply(mout, m)
	m=MatrixRotationX(pitch)
	mout=MatrixMultiply(mout, m)
	m=MatrixRotationY(yaw)
	mout=MatrixMultiply(mout, m)
	return mout

def GetPivotMatrix(aR, aT):
	mRotation=MatrixRotationYawPitchRoll(aR[0], aR[1], aR[2])
	mTranslation=MatrixTranslation(aT[0], aT[1], aT[2])
	return MatrixMultiply(mRotation,mTranslation)

if __name__ == '__main__':
	'''
	v3=IndieLib.D3DXFROMWINEVECTOR3(0.0, 0.0, 0.0)
	v=IndieLib.D3DXFROMWINEVECTOR3(1.0, 1.0, 1.0)
	m=IndieLib.D3DXFROMWINEMATRIX()
	IndieLib.D3DXFROMWINEMatrixIdentity(m)
	IndieLib.D3DXFROMWINEVec3TransformCoord(v3, v, m)
	IndieLib.D3DXFROMWINEDumpVec3(v3)
	v=[1,1,1]
	m=[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
	v3=Vec3TransformCoord(v, m)
	DumpVec3(v3)

	mo=IndieLib.D3DXFROMWINEMATRIX()
	m1=IndieLib.D3DXFROMWINEMATRIX()
	m2=IndieLib.D3DXFROMWINEMATRIX()
	IndieLib.D3DXFROMWINEMatrixIdentity(mo)
	IndieLib.D3DXFROMWINEMatrixIdentity(m1)
	IndieLib.D3DXFROMWINEMatrixIdentity(m2)
	IndieLib.D3DXFROMWINEMatrixMultiply(mo, m1, m2)
	IndieLib.D3DXFROMWINEDumpMatrix(mo)
	m1=[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
	m2=[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]
	mo=MatrixMultiply(m1, m2)
	DumpMatrix(mo)

	mo=IndieLib.D3DXFROMWINEMATRIX()
	IndieLib.D3DXFROMWINEMatrixIdentity(mo)
	s=1.0
	IndieLib.D3DXFROMWINEMatrixScaling(mo, s, s, s)
	IndieLib.D3DXFROMWINEDumpMatrix(mo)
	mo=MatrixScaling(s, s, s)
	DumpMatrix(mo)

	mo=IndieLib.D3DXFROMWINEMATRIX()
	IndieLib.D3DXFROMWINEMatrixIdentity(mo)
	IndieLib.D3DXFROMWINEMatrixTranslation(mo, 0, 0, 0)
	IndieLib.D3DXFROMWINEDumpMatrix(mo)
	mo=MatrixTranslation(0, 0, 0)
	DumpMatrix(mo)

	mo=IndieLib.D3DXFROMWINEMATRIX()
	IndieLib.D3DXFROMWINEMatrixIdentity(mo)
	IndieLib.D3DXFROMWINEMatrixRotationX(mo, 45*math.pi/180.0)
	IndieLib.D3DXFROMWINEDumpMatrix(mo)
	mo=MatrixRotationX(45)
	DumpMatrix(mo)

	mo=IndieLib.D3DXFROMWINEMATRIX()
	IndieLib.D3DXFROMWINEMatrixIdentity(mo)
	IndieLib.D3DXFROMWINEMatrixRotationY(mo, 45*math.pi/180.0)
	IndieLib.D3DXFROMWINEDumpMatrix(mo)
	mo=MatrixRotationY(45)
	DumpMatrix(mo)

	mo=IndieLib.D3DXFROMWINEMATRIX()
	IndieLib.D3DXFROMWINEMatrixIdentity(mo)
	IndieLib.D3DXFROMWINEMatrixRotationZ(mo, 45*math.pi/180.0)
	IndieLib.D3DXFROMWINEDumpMatrix(mo)
	mo=MatrixRotationZ(45)
	DumpMatrix(mo)

	mo=IndieLib.D3DXFROMWINEMATRIX()
	IndieLib.D3DXFROMWINEMatrixIdentity(mo)
	IndieLib.D3DXFROMWINEMatrixRotationYawPitchRoll(mo, 45*math.pi/180.0, 45*math.pi/180.0, 45*math.pi/180.0)
	IndieLib.D3DXFROMWINEDumpMatrix(mo)
	mo=MatrixRotationYawPitchRoll(45, 45, 45)
	DumpMatrix(mo)
	'''
	'''
	aR=IndieLib.D3DXFROMWINEMATRIX()
	aT=IndieLib.D3DXFROMWINEMATRIX()
	IndieLib.D3DXFROMWINEMatrixIdentity(aR)
	IndieLib.D3DXFROMWINEMatrixIdentity(aT)
	mo=GetPivotMatrix(aR, aT)
	IndieLib.D3DXFROMWINEDumpMatrix(mo)
	'''


	aR=MatrixRotationYawPitchRoll(0.0, 0.0, 0.0)
	aT=MatrixTranslation(0.0, 0.0, 0.0)
	mo=GetPivotMatrix(aR, aT)
	DumpMatrix(mo)

	'''
	aR=IndieLib.D3DXFROMWINEMATRIX()
	IndieLib.D3DXFROMWINEMatrixIdentity(aR)
	IndieLib.D3DXFROMWINEMatrixRotationYawPitchRoll(aR, 0.0, 0.0, 0.0)

	aT=IndieLib.D3DXFROMWINEMATRIX()
	IndieLib.D3DXFROMWINEMatrixIdentity(aT)
	IndieLib.D3DXFROMWINEMatrixTranslation(aT, 0, 0, 0)

	mo=IndieLib.stx_GetPivotMatrix(aR, aT)
	IndieLib.D3DXFROMWINEDumpMatrix(mo)
	'''

