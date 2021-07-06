#
# Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

import math
import pygame
import sys

window=(640,512)
    
'''
cantordust
{
	instance "box"
	move [0, 120%, 0]
	divide x
		[width/3] cantordust
		[width/3] {}
		[width/3] cantordust
}

cantordust : width < 0.5 {}
'''

def CantorDust(screen, x, width, y,height):
	if  width < 0.5:
		return

	CantorDust(screen, x, 			width/3.0, y+height,height)
	CantorDust(screen, x+2.0*width/3.0, 	width/3.0, y+height,height)

	#start = [x, y]
	#end = [x+width, y ]
	pygame.draw.rect(screen, (0,0,0), (x,y,width,height), 1)

pygame.init()
pygame.display.set_caption('CantorDust2D')
screen = pygame.display.set_mode((window[0],window[1]))
screen.fill((255,255,255))

while (True):
	for event in pygame.event.get():
        	if event.type == pygame.QUIT:
             		pygame.quit()
			sys.exit()
	CantorDust(screen, 0.0, window[0], 0.0, 10.0)
	pygame.display.update()

