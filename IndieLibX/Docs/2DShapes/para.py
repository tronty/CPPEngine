import pygame
import sys
from pygame.locals import *
import math

WIDTH = 640
HEIGHT = 480

pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT), 0, 32)
color = (255, 0, 0)

pxarray = pygame.PixelArray(screen)
first = True
while True:
    for event in pygame.event.get():
        if event.type == QUIT:
            pygame.quit()
            sys.exit()
    screen.fill((0,0,0))

    for y, py in enumerate(pxarray):
        for x, px in enumerate(py):
            if int(x) == (int(y)*int(y)) - 30*int(y) + 450:
                pxarray[y][x] = 0xFFFFFF

                if first:
                    first = False
                    prev_x, prev_y = x, y
                    continue

                pygame.draw.line(screen, color, (prev_y, prev_x), (y, x))
                prev_x, prev_y = x, y

    first = True
    pygame.display.flip()

