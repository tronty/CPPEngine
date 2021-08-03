# https://eskerda.com/bsp-dungeon-generation/

#
# Copyright (c) 2018 Tommi Roenty   http:#www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http:#opensource.org/licenses/LGPL-2.1
#

import random
import math
import pygame
import sys
from xml.etree import ElementTree

screen = None
window=(640,512)
g_node = None

MAP_SIZE         = 500
N_ITERATIONS     = 9

SQUARE           = window[0] / MAP_SIZE
DISCARD_BY_RATIO = True
H_RATIO          = 0.45
W_RATIO          = 0.45

def random_(min, max):
	return math.floor(random.uniform(0, 1) * (max - min + 1) + min) # ???

class Tree():
	def __init__(self, leaf=None):
    		self.leaf = leaf
    		self.lchild = None
    		self.rchild = None

	def getLeafs(self):
    		if (self.lchild == None and self.rchild == None):
        		return [self.leaf]
    		else:
       			return (self.lchild.getLeafs() + self.rchild.getLeafs()) # ??? concat -> join

	def paint(self, c):
    		self.leaf.paint(c)
    		if (not self.lchild ==None):
        		self.lchild.paint(c)
    		if (not self.rchild ==None):
        		self.rchild.paint(c)

class Point():
	def __init__(self, x, y):
    		self.x = x
    		self.y = y

class Container():
	def __init__(self, x, y, w, h):
    		self.x = x
    		self.y = y
    		self.w = w
    		self.h = h
    		self.center = Point(self.x + (self.w/2), self.y + (self.h/2))

	def paint(self, c):


		#pygame.draw.rect(screen, (0,0,0), (this.x * SQUARE, this.y * SQUARE, this.w * SQUARE, this.h * SQUARE)
		#pygame.draw.rect(screen, (0,0,0), (this.x, this.y, this.w * SQUARE, this.h * SQUARE)

		if False:
			pygame.draw.rect(screen, (0,0,0), (self.x * SQUARE, self.y * SQUARE, self.w * SQUARE, self.h * SQUARE), 2)
		else:
			pygame.draw.rect(screen, (0,0,0), (self.x, self.y, self.w * SQUARE, self.h * SQUARE), 2)

	def split_container(self, container, iter):
    		root = Tree(container)
    		if (not iter == 0):
        		sr = self.random_split(container)
        		root.lchild = self.split_container(sr[0], iter-1)
        		root.rchild = self.split_container(sr[1], iter-1)
    		return root

	def random_split_(self, container):
    		r1=None
		r2=None
    		if (random_(0, 1) == 0):
        		# Vertical
        		r1 = Container(container.x, container.y, random_(1, container.w), container.h)
            		r2 = Container(container.x + r1.w, container.y, container.w - r1.w, container.h)
    		else:
        		# Horizontal
        		r1 = Container(container.x, container.y, container.w, random_(1, container.h))
                        r2 = Container(container.x, container.y + r1.h, container.w, container.h - r1.h)
	
		mergedlist = []
		mergedlist.append(r1)
		mergedlist.append(r2)
		return mergedlist

	def random_split(self, container):
    		r1=None
		r2=None
    		if (random_(0, 1) == 0):
        		# Vertical
        		r1 = Container(container.x, container.y, random_(1, container.w), container.h)
			r2 = Container(container.x + r1.w, container.y, container.w - r1.w, container.h)

        		if (DISCARD_BY_RATIO):
            			r1_w_ratio = r1.w / r1.h
            			r2_w_ratio = r2.w / r2.h
            			if (r1_w_ratio < W_RATIO or r2_w_ratio < W_RATIO):
                			return self.random_split(container)
    		else:
        		# Horizontal
        		r1 = Container(container.x, container.y, container.w, random_(1, container.h))
        		r2 = Container(container.x, container.y + r1.h, container.w, container.h - r1.h)

        		if (DISCARD_BY_RATIO):
            			r1_h_ratio = r1.h / r1.w
            			r2_h_ratio = r2.h / r2.w
            			if (r1_h_ratio < H_RATIO or r2_h_ratio < H_RATIO):
                			return self.random_split(container)
	
		mergedlist = []
		mergedlist.append(r1)
		mergedlist.append(r2)
		return mergedlist

pygame.init()
pygame.display.set_caption('BSPDungeon2D')
screen = pygame.display.set_mode((window[0],window[1]))
screen.fill((255,255,255))

with open('../../../IndieLib_resources/BSPDungeon/BSPDungeon.xml', 'rt') as f:
	tree = ElementTree.parse(f)
	xml=tree.getroot()
	MAP_SIZE=int(xml.find('./BSP').attrib['MAP_SIZE'])
	N_ITERATIONS=int(xml.find('./BSP').attrib['N_ITERATIONS'])
	print 'MAP_SIZE=',MAP_SIZE
	print 'N_ITERATIONS=',N_ITERATIONS

main_container = Container(0, 0, window[0], window[1])
container_tree = main_container.split_container(main_container, N_ITERATIONS)

while (True):
	for event in pygame.event.get():
        	if event.type == pygame.QUIT:
             		pygame.quit()
			sys.exit()
	container_tree.paint(screen)
	pygame.display.update()

