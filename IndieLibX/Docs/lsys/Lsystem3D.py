#
# Copyright (c) 2018 Tuomas Roenty   http://www.tuomasronty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

import sys
import math
import pygame

axiom="F"
angle = 60*math.pi/180
distance = 5
rules = ['F',['F','+','F','-','-','F','+','F']]
numIters=4
window=(640,400)
ORIGINX = 0
ORIGINY = 0

def draw_3dline(surface, color, a, b):
	"""Convert 3D coordinates to 2D and draw line.""" 
	ax, ay = a[0]+(a[2]*0.3)+ORIGINX, a[1]+(a[2]*0.3)+ORIGINY
	bx, by = b[0]+(b[2]*0.3)+ORIGINX, b[1]+(b[2]*0.3)+ORIGINY
	pygame.draw.line(surface, color, (ax, ay), (bx, by))

def createLSystem(numIters,axiom, rules):
    startArray = [axiom]
    endArray = []
    for i in range(numIters):
        endArray = processArray(startArray, rules)
        startArray = endArray

    return endArray

def processArray(oldArr, rules):
    newarr = []
    for i in oldArr:
	for ii in applyRules(i, rules):
        	newarr.append(ii) 

    return newarr

def applyRules(i, rules):
    newarr = []
    for ii in range(0, len(rules)/2):
	key=rules[2*ii+0]
	rule=rules[2*ii+1]
    	if i == key:
        	newarr = rule
    	else:
        	newarr = i

    return newarr

class Turtle:
    def __init__(self, x=window[0]/2, y=window[1]/2, z=0, ax=angle, ay=0, az=0):
	self.x = x
	self.y = y
	self.z = z
	self.ax = ax 
	self.ay = ay
	self.az = az 
	self.stack=[]

    def forward(self, screen, distance):
        start = [self.x, self.y, self.z]
	self.x += cos((self.ax)) * cos((self.ay))*distance
	self.y += sin((self.ax)) * cos((self.ay))*distance
	self.z += sin((self.ay))*distance
        end = [self.x, self.y, self.z]
        draw_3dline(screen, (0,0,0), start, end)
	#print start
	#print end

    def turn(self, angle):
        """Turn left by angle."""
        self.ax += angle

    def push(turtle):
		self.stack.append(turtle)
    def pop():
	if len(self.stack):
		i=self.stack[len(self.stack)-1]
		self.stack=self.stack[0:len(self.stack)-2]
		return i
	return None

def drawLsystem(screen, aTurtle, inst, angle, distance):
	for symbol in inst:
		if symbol == "[":
			aTurtle.push(turtle)
		elif symbol == "]":
			aTurtle=turtle.pop()
		elif symbol == "F":
			aTurtle.forward(screen, distance)
		elif symbol == "B":
			aTurtle.forward(screen, -distance)
		elif symbol == "+":
			aTurtle.turn(angle)
		elif symbol == "-":
			aTurtle.turn(-angle)

pygame.init()
screen = pygame.display.set_mode((window[0],window[1]))
screen.fill((255,255,255))
#turtle=Turtle(window[0]/2, window[1]/2, angle)
inst = createLSystem(numIters, axiom, rules)

while (True):
	for event in pygame.event.get():
        	if event.type == pygame.QUIT:
             		pygame.quit()
			sys.exit()
	turtle=Turtle(window[0]/2, window[1]/2, angle)
	drawLsystem(screen, turtle, inst, angle, distance)
	pygame.display.update()

