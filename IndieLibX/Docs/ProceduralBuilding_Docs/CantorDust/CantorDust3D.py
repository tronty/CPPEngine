#
# Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#	 http://opensource.org/licenses/LGPL-2.1
#

import math
import pygame
from pygame.locals import *

try:
	from OpenGL.GL import *
	from OpenGL.GLU import *
except:
	print ('The GLCUBE example requires PyOpenGL')
	raise SystemExit

window=(640,512)

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
			l='m[%d,%d]=%f\n' % (i,j,m[4*i+j])
			print l

def DumpVec3(v):
	for i in range(0,3):
		l='v3[%d]=%f\n' % (i,v[i])
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

def drawarrays(vertices, texcoords, colors):
	glLoadMatrixf(w)
	glBegin(GL_TRIANGLES)
	for i in range(0,len(vertices)/3):
		glColor3f   (colors   [3*i][0], colors   [3*i+1][1], colors[3*i+2][2])
		glTexCoord2f(texcoords[3*i][0], texcoords[3*i+1][1])
		glVertex3f  (vertices [3*i][0], vertices [3*i+1][1], vertices[3*i+2][2])
	glEnd()

vertices=[]
texcoords=[]
colors=[]
def appendquad2arrays(w, color, size):
	texcoords.append([0, 0]);vertices.append(Vec3TransformCoord([-size[0],  size[1], 0]));colors.append(color)
	texcoords.append([1, 0]);vertices.append(Vec3TransformCoord([ size[0],  size[1], 0]));colors.append(color)
	texcoords.append([1, 1]);vertices.append(Vec3TransformCoord([ size[0], -size[1], 0]));colors.append(color)
	texcoords.append([0, 1]);vertices.append(Vec3TransformCoord([-size[0], -size[1], 0]));colors.append(color)
	texcoords.append([1, 1]);vertices.append(Vec3TransformCoord([ size[0], -size[1], 0]));colors.append(color)
	texcoords.append([0, 0]);vertices.append(Vec3TransformCoord([-size[0],  size[1], 0]));colors.append(color)

def drawquad(w, color, size):
	if False:
		appendquad2arrays(w, color, size)
	else:
		glLoadMatrixf(w)
		glBegin(GL_TRIANGLES)
		glColor3f(color[0], color[1], color[2])
		glTexCoord2f(0, 0);glVertex3f(-size[0],  size[1], 0)
		glTexCoord2f(1, 0);glVertex3f( size[0],  size[1], 0)
		glTexCoord2f(1, 1);glVertex3f( size[0], -size[1], 0)
		glTexCoord2f(0, 1);glVertex3f(-size[0], -size[1], 0)
		glTexCoord2f(1, 1);glVertex3f( size[0], -size[1], 0)
		glTexCoord2f(0, 0);glVertex3f(-size[0],  size[1], 0)
		glEnd()

def drawcube(position, angle):
	size=[1.0, 1.0]
	size=[0.3, 0.3]
	
	vR=[0*90.0+angle[0], 0.0+angle[1], 0.0]
	vP=[0.0+position[0], 0.0+position[1], -1.0+position[2]]
	color=[0.0, 1.0, 0.0]
	w=GetPivotMatrix(vR, vP)
	drawquad(w, color, size)

	vR=[0*90.0+angle[0], 0.0+angle[1], 0.0]
	vP=[0.0+position[0], 0.0+position[1], 1.0+position[2]]
	color=[0.0, 1.0, 0.0]
	w=GetPivotMatrix(vR, vP)
	drawquad(w, color, size)

	vR=[1*90.0+angle[0], 0.0+angle[1], 0.0]
	vP=[1.0+position[0], 0.0+position[1], 0.0+position[2]]
	color=[1.0, 0.0, 0.0]
	w=GetPivotMatrix(vR, vP)
	drawquad(w, color, size)

	vR=[1*90.0+angle[0], 0.0+angle[1], 0.0]
	vP=[-1.0+position[0], 0.0+position[1], 0.0+position[2]]
	color=[1.0, 0.0, 0.0]
	w=GetPivotMatrix(vR, vP)
	drawquad(w, color, size)

	vR=[0.0+angle[0], 1*90.0+angle[1], 0.0]
	vP=[0.0+position[0], 1.0+position[1], 0.0+position[2]]
	color=[0.0, 0.0, 1.0]
	w=GetPivotMatrix(vR, vP)
	drawquad(w, color, size)

	vR=[0.0+angle[0], -1*90.0+angle[1], 0.0]
	vP=[0.0+position[0], -1.0+position[1], -0.0+position[2]]
	color=[0.0, 0.0, 1.0]
	w=GetPivotMatrix(vR, vP)
	drawquad(w, color, size)
	return

def main():
	pygame.init()
	pygame.display.set_caption('CantorDust3D')
	screen = pygame.display.set_mode((window[0],window[1]), OPENGL|DOUBLEBUF)
	glClearColor(1,1,1,1)
	glEnable(GL_DEPTH_TEST) 

	glMatrixMode(GL_PROJECTION)
	glLoadIdentity()
	glMatrixMode(GL_MODELVIEW)
	glLoadIdentity()

	#gluPerspective(45.0,window[0]/window[1],0.1,100.0)

	angleX=0.0

	while 1:
		event = pygame.event.poll()
		if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
			break

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)

		if False:
			drawarrays(vertices, texcoords, colors)
		elif True:
			drawcube([0.0, 0.0, -3.0], [45+angleX, 45])
		else:
			drawcube([0.0, 0.0, 0.0], [0, 0])

		pygame.display.flip()
		#pygame.time.wait(10)
		angleX+=0.5

if __name__ == '__main__':
	main()

