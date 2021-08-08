#
# Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

# Douglas Dunham and John Lindgren:"Creating Repeating Hyperbolic Patterns"
# Computer Graphics Volume 15 Number 3 Aug 1981
# http://www.d.umn.edu/~ddunham/

import os, re, string
import math, pygame, sys


def matMult(m1, m2):
	#print ':1'
	om=[[1,0,0],[0,1,0],[0,0,1]]
    	for i in range(0, 3):
        	for j in range(0, 3):
            		om[i][j] = m1[i][0] * m2[0][j] + m1[i][1] * m2[1][j] + m1[i][2] * m2[2][j]
	return om

def vecMult(m, v):
	#print ':2'
	ov=[[1,0,0],[0,1,0],[0,0,1]]
	norm = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] *v[2];
	ov[0] = (m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2]) / norm;
	ov[1] = (m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2]) / norm;
	ov[2] = (m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2]) / norm;
	return ov

window=(640,512)
Color=(255,255,255)
Edge=0
Vertex=1
Identity=[[1,0,0],[0,1,0],[0,0,1]]

nLayers=4
p=4
q=4

coshb=math.cos(math.pi/q)/math.sin(math.pi/p)
cosh2b=2.0*coshb*coshb-1.0
sinh2b=0#math.sqrt(cosh2b*cosh2b-1.0)#ValueError: math domain error

ReflectEdgeBisector=[[1,0,0],[0,-1,0],[0,0,1]]
ReflectPgonEdge=[[-cosh2b,0,sinh2b],[0,1,0],[-sinh2b,0,cosh2b]]
ReflectHypotenuse=[[math.cos(2*math.pi/p),math.sin(2*math.pi/p),0],[math.sin(2*math.pi/p),-math.cos(2*math.pi/p),0],[0,0,1]]

RotateP=matMult(ReflectEdgeBisector,ReflectHypotenuse)
RotateQ=matMult(ReflectHypotenuse,ReflectPgonEdge)
RotateEdge=matMult(ReflectPgonEdge,ReflectEdgeBisector)

def DrawPgonPattern(T):
	#print ':3,T:',T
	SumSquare=0.0
	Tx=0.0
	Ty=0.0
	Z=[0.0,0.0,0.0]
	P=[Tx, Ty]

	nPgonActions=0#???
	X=range(0, nPgonActions)#???
	Y=range(0, nPgonActions)#???
	Action=range(0, nPgonActions)#???

	for i in range(1, nPgonActions):
		SumSquare=X[i]*X[i]+Y[i]*Y[i]
		Z[0]=2.0*X[i]/(1.0-SumSquare)
		Z[1]=2.0*Y[i]/(1.0-SumSquare)
		Z[2]=(1.0+SumSquare)/(1.0-SumSquare)
		#print 'T:',T
		#print 'Z:',Z
		Z=vecMult(T,Z)
		Tx=Z[0]/(1.0+Z[2])
		Ty=Z[1]/(1.0+Z[2])
		#print 'Action[i]:',Action,i
		if Action[i]=='Move':
			P=[Tx, Ty]
		elif Action[i]=='Draw':
			pygame.draw.line(screen, Color, P, [Tx, Ty], 1)
			P=[Tx, Ty]
		elif Action[i]=='Black':
			Color=(0,0,0)
		elif Action[i]=='Blue':
			Color=(0,0,255)
		elif Action[i]=='Red':
			Color=(255,0,0)
		elif Action[i]=='Yellow':
			Color=(255,255,0)

def ReplicatePattern(InitialTransform, Layerdiff, Adjacency):
	#print ':4'
	DrawPgonPattern(InitialTransform)
	if Layerdiff>0:
		ExposedEdges=0
		if Adjacency==Edge:
			ExposedEdges=p-3
		elif Adjacency==Vertex:
			ExposedEdges=p-2
		RotateCenter=matMult(RotateP,matMult(RotateP,InitialTransform))
		for i in range(1, ExposedEdges):
			T=matMult(RotateEdge,RotateCenter)
			ReplicatePattern(T, Layerdiff-1, Edge)
			RotateCenter=matMult(RotateP,RotateCenter)
			PgonsPerVertex=0
			if i < ExposedEdges:
				PgonsPerVertex=q-3
			elif i == ExposedEdges:
				PgonsPerVertex=q-4
			RotateVertex=matMult(RotateQ,RotateP)
			for j in range(1, PgonsPerVertex):
				ReplicatePattern(matMult(RotateVertex,T), Layerdiff-1, Vertex)
				RotateVertex=matMult(RotateP,RotateVertex)
			
		
def render():
	#print ':5'
	RotateCenter=Identity
	for i in range(1, p):
		T=matMult(RotateEdge,RotateCenter)
		ReplicatePattern(T, nLayers-1, Edge)
		RotateCenter=matMult(RotateP,RotateCenter)
		RotateVertex=matMult(RotateQ,RotateP)
		for j in range(1, q-3):
			ReplicatePattern(matMult(RotateVertex,T), nLayers-1, Vertex)
			RotateVertex=matMult(RotateQ,RotateVertex)
	
if __name__ == '__main__':
	pygame.init()
	screen = pygame.display.set_mode((window[0],window[1]))
	screen.fill((255,255,255))

	while (True):
		for event in pygame.event.get():
        		if event.type == pygame.QUIT:
             			pygame.quit()
				sys.exit()
		render()
		pygame.display.update()

