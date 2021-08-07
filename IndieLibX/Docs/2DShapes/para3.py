import pygame
import sys
from pygame.locals import *
import math

WIDTH = 640
HEIGHT = 480

def drawParabola(screen, p, h, k):
	first = True
	color = (255, 0, 0)
	for y in range(int(k),HEIGHT):
		x = (y-h)*(y-h)*0.25/p+k
                if first:
                    	first = False
                    	prev_x, prev_y = x, y
                    	continue

                pygame.draw.line(screen, color, (prev_y, prev_x), (y, x))
                #pygame.draw.line(screen, color, (prev_y, -prev_x), (y, -x))
                prev_x, prev_y = x, y

def main():
	pygame.init()
	screen = pygame.display.set_mode((WIDTH, HEIGHT), 0, 32)
	while True:
    		for event in pygame.event.get():
        		if event.type == QUIT:
            			pygame.quit()
            			sys.exit()
    		screen.fill((0,0,0))

		drawParabola(screen, 1.25, 100.0, 300.0)
		drawParabola(screen, 1.5625, 150.0, 300.0)
		drawParabola(screen, 2.0833, 200.0, 300.0)
		drawParabola(screen, 3.125, 250.0, 300.0)
		drawParabola(screen, 6.25, 300.0, 300.0)

    		pygame.display.flip()

main()

