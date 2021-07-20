#
# Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

import math
import pygame
import sys

'''
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
'''

class Image():
	def __init__(self, dx, dy, acolor=(255, 255, 255)):
		self.screen=0
		self.pi=math.pi
		self.PI=math.pi
		self.color=(255, 255, 255)
		self.thickness=1.0
		pygame.init() # ???
		self.screen = pygame.display.set_mode((dx, dy))
		self.screen.fill(acolor)
		return self.screen

	def line(cx, cy, dx, dy, acolor):
		#pointlist=[(cx, cy), (cx+dx, cy+dy)]
		#pygame.draw.lines(self.screen, acolor, False, pointlist, self.thickness)

        	pygame.draw.line(self.screen, acolor, [cx, cy], [cx+dx, cy+dy ], self.thickness)

	def rect(x,y,width,height, acolor):
		pygame.draw.rect(self.screen, acolor, (x,y,width,height), self.thickness)
		
	def circle(x,y, radius, acolor):
		pygame.draw.circle(self.screen, acolor, (x,y), radius, self.thickness)

