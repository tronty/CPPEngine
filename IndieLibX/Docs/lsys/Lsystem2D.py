#
# Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

import math
import pygame
import sys

axiom="F"
angle = 60*math.pi/180
distance = 5
rules = ['F',['F','+','F','-','-','F','+','F']]
numIters=4
window=(640,512)

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

def processArray(oldArr, rules):
    newarr = []
    for i in oldArr:
	for ii in applyRules(i, rules):
        	newarr.append(ii) 

    return newarr

def createLSystem(numIters,axiom, rules):
    startArray = [axiom]
    endArray = []
    for i in range(numIters):
        endArray = processArray(startArray, rules)
        startArray = endArray

    return endArray

class Turtle:
    def __init__(self, x=window[0]/2, y=window[1]/2, a=angle):
	self.angle = a
	self.x = x
	self.y = y
	self.stack=[]

    def forward(self, screen, distance):
        start = [self.x, self.y]
        self.x += distance * math.cos(self.angle)
        self.y += distance * math.sin(self.angle)
        end = [self.x, self.y ]
        pygame.draw.line(screen, (0,0,0), start, end, 1)
	#print start
	#print end

    def turn(self, angle):
        self.angle += angle

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

