#
# Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

# Douglas Dunham:"An Algorithm to Generate Repeating Hyperbolic Patterns.pdf"
# http://www.d.umn.edu/~ddunham/

import os, re, string
import math, pygame, sys

window=(640,512)
edgeTran=[[[1,0,0,0],[0,1,0,0],[0,0,1,0]],[[1,0,0,0],[0,1,0,0],[0,0,1,0]],[[1,0,0,0],[0,1,0,0],[0,0,1,0]],[[1,0,0,0],[0,1,0,0],[0,0,1,0]]]
v=[[-1, -1], [1, -1], [0, 1]]
maxLayers=3
p=4
q=[3,3,3,3]


def drawMotif ( motif, initialTran ):
	print ':1'
	start=v[0]
	end=v[1]
	for i in range(1,len(v)):
		print 'i2:', i
		end=v[i]
		pygame.draw.line(screen, (0,0,0), start, end, 1)
		start=end
	return

def matMult ( m1, m2 ):
	print ':2'
	om=[[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]]
    	for i in range(0, 4):
        	for j in range(0, 4):
            		om[i][j] = m1[i][0] * m2[0][j] + m1[i][1] * m2[1][j] + m1[i][2] * m2[2][j] + m1[i][3] * m2[3][j];
	return om

def GetRotV(T):
	out=[0,0,0,1]
	if T[0][0] == 1.0:
        	out[0] = math.atan2(T[0][2], T[1][2]);
        elif T[0][0] == -1.0:
        	out[0] = math.atan2(T[0][2], T[1][2]);
        else:
        	out[0] = math.atan2(-T[2][0],T[0][0]);
            	out[1] = math.asin(T[2][1]);
            	out[2] = math.atan2(-T[1][2],T[1][1]);
	return out

def computeTran ( T, shift ):
	print ':3,T:',T
	pPosition=[T[2][0],T[2][1],T[2][2],1]
	orientation=GetRotV(T)
	newEdge = ( pPosition + orientation * shift ) % p 
	return matMult ( tran, edgeTran[newEdge] ) 

def addToTran ( T, shift ):
	print ':4'
	if ( shift % p == 0 ):
		return T
	#else:
	return computeTran ( T, shift )

pShiftArray=[1, 0]  
verticesToSkipArray=[3, 2]  
qShiftArray=[0, -1]
polygonsToSkipArray=[2, 3]
MAX_EXP=1#???
MIN_EXP=0#???
exposureArray=[MAX_EXP, MIN_EXP]

# where drawMotif(): simply multiplies each of the point vectors of the motif by the transformation and draws the transformed motif.
def replicateMotif ( motif, initialTran, layer, exposure ):
	print ':5'
	drawMotif ( motif, initialTran )  # Draw the transformed motif
	if ( layer < maxLayers ):
		pShift = pShiftArray[exposure]  # Which vertex to start at
		verticesToDo = p - verticesToSkipArray[exposure] 
		for i in range( 1, verticesToDo ): # Iterate over vertices
			pTran = computeTran ( initialTran, pShift ) 
			first_i = ( i == 1 ) 
			qTran = addToTran ( pTran, qShiftArray[first_i] ) 
			if ( pTran.orientation > 0 ):
				vertex = (pTran.pposition-1) % p 
			else:
				vertex = pTran.pposition 
			polygonsToDo = q[vertex] - polygonsToSkipArray[first_i] 
			for j in range( 1, polygonsToDo ): # Iterate about a vertex
				first_j = ( j == 1 ) 
				newExposure = exposureArray[first_j] 
				replicateMotif ( motif, qTran, layer + 1, newExposure ) 
				qTran = addToTran ( qTran, -1 ) 
			pShift = (pShift + 1) % p 
			# Advance to next vertex

def replicate ( motif ):
	print ':6'
	#print 'q[1]', q[1]
	for j in range( 1, q[1] ):
		qTran = edgeTran[1] 
		# qTran = transformation about the origin
		replicateMotif ( motif, qTran, 2, MAX_EXP )
		qTran = addToTran ( qTran, -1 )
		# -1 ==> anticlockwise order

def render():
	print ':7'
	motif=[[-1, -1], [1, -1], [0, 1]]
	replicate ( motif )
	
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

