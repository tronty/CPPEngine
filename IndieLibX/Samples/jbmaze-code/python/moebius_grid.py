#
# Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

#---
# Excerpted from "Mazes for Programmers",
# published by The Pragmatic Bookshelf.
# Copyrights apply to this code. It may not be used to create training material, 
# courses, books, articles, and the like. Contact us if you are in doubt.
# We make no guarantees that this code is fit for any purpose. 
# Visit http://www.pragmaticprogrammer.com/titles/jbmaze for more book information.
#---
import cylinder_grid

class MoebiusGrid(CylinderGrid):
	def __init__(self, rows, columns):
		super(rows, columns*2) 
	

	def to_png(self, cell_size=10, inset=0):
		grid_height = cell_size * rows
		mid_point = columns / 2 

		img_width = cell_size * mid_point 
		img_height = grid_height * 2

		inset = (cell_size * inset).to_i

		background = (255,255,255)
		wall = (0,0,0)

		img = ChunkyPy.Image(img_width + 1, img_height + 1, background)

		for backgrounds, walls in mode:
			for cell in each_cell:
				x = (cell.column % mid_point) * cell_size 
				y = cell.row * cell_size
				if cell.column >= mid_point:
					y += grid_height  

				if inset > 0:
					to_png_with_inset(img, cell, mode, cell_size, wall, x, y, inset)
				else:
					to_png_without_inset(img, cell, mode, cell_size, wall, x, y)
				
			
		

		return img
	

