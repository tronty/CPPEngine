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
import grid
import triangle_cell

class TriangleGrid(grid.Grid):
	def prepare_grid(self):
		m_grid = [[0 for x in range(grid.Grid.m_columns)] for y in range(grid.Grid.m_rows)] 
		for row in range(grid.Grid.m_rows):
			for column in range(grid.Grid.m_columns):
				triangle_cell.TriangleCell(row, column)

	def configure_cells(self):
		for cell in each_cell:
			row, col = cell.row, cell.column

			cell.west = self[row, col - 1]
			cell.east = self[row, col + 1]

			if cell.upright:
				cell.south = self[row + 1, col]
			else:
				cell.north = self[row - 1, col]
			 
		
	

	def to_png(self, size=16):
		half_width = size / 2.0 
		height = size * Math.sqrt(3) / 2.0
		half_height = height / 2.0 

		img_width = "%d" % (size * (columns + 1) / 2.0).to_i 
		img_height = "%d" % (height * rows).to_i 

		background = (255,255,255)
		wall = (0,0,0)

		img = ChunkyPy.Image(img_width + 1, img_height + 1, background)

		for backgrounds, walls in mode:
			for cell in each_cell:
				cx = half_width + cell.column * half_width 
				cy = half_height + cell.row * height

				west_x = "%d" % (cx - half_width).to_i
				mid_x = "%d" % cx.to_i
				east_x = "%d" % (cx + half_width).to_i

				if cell.upright:
					apex_y = "%d" % (cy - half_height).to_i
					base_y = "%d" % (cy + half_height).to_i
				else:
					apex_y = "%d" % (cy + half_height).to_i
					base_y = "%d" % (cy - half_height).to_i
				 

				if mode == backgrounds:
					color = background_color_for(cell)
					if color:
						points = [[west_x, base_y], [mid_x, apex_y], [east_x, base_y]]
						img.polygon(points, color, color)
					
				else:
					if not cell.west:
						img.line(west_x, base_y, mid_x, apex_y, wall)
					

					if not cell.linked==cell.east:
						img.line(east_x, base_y, mid_x, apex_y, wall)
					

					no_south = cell.upright and cell.south.nil 
					not_linked = cell.upright and (cell.linked==cell.north)

					if no_south or not_linked :
						img.line(east_x, base_y, west_x, base_y, wall)
					
				
			
		

		return img
	
