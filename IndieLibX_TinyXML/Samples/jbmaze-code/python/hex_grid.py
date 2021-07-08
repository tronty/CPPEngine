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
import hex_cell

class HexGrid(grid.Grid):
	def prepare_grid(self):
		m_grid = [[0 for x in range(grid.Grid.m_columns)] for y in range(grid.Grid.m_rows)] 
		for row in range(grid.Grid.m_rows):
			for column in range(grid.Grid.m_columns):
				HexCell(row, column) 

	def configure_cells(self):
		for cell in each_cell:
			row, col = cell.row, cell.column

			if col.even:
				north_diagonal = row - 1
				south_diagonal = row
			else:
				north_diagonal = row
				south_diagonal = row + 1
			 

			cell.northwest = self[north_diagonal, col - 1] 
			cell.north		 = self[row - 1, col]
			cell.northeast = self[north_diagonal, col + 1]
			cell.southwest = self[south_diagonal, col - 1]
			cell.south		 = self[row + 1, col]
			cell.southeast = self[south_diagonal, col + 1] 
		
	

	def to_png(size=10):
		a_size = size / 2.0 
		b_size = size * Math.sqrt(3) / 2.0
		width = size * 2
		height = b_size * 2 

		img_width = (3 * a_size * columns + a_size + 0.5).to_i 
		img_height = (height * rows + b_size + 0.5).to_i 

		background = (255,255,255)
		wall = (0,0,0)

		img = ChunkyPy.Image(img_width + 1, img_height + 1, background)

		for backgrounds, walls in mode:
			for cell in each_cell:
				cx = size + 3 * cell.column * a_size 
				cy = b_size + cell.row * height
				if cell.column.odd:
					cy += b_size 

				# f/n = far/near
				# n/s/e/w = north/south/east/west
				x_fw = (cx - size).to_i 
				x_nw = (cx - a_size).to_i
				x_ne = (cx + a_size).to_i
				x_fe = (cx + size).to_i

				# m = middle
				y_n = (cy - b_size).to_i
				y_m = cy.to_i
				y_s = (cy + b_size).to_i 

				if mode==backgrounds:
					color = background_color_for(cell)
					if color:
						points = [[x_fw, y_m], [x_nw, y_n], [x_ne, y_n],
											[x_fe, y_m], [x_ne, y_s], [x_nw, y_s]]
						img.polygon(points, color, color)
					
				else:
					if not cell.southwest:
						img.line(x_fw, y_m, x_nw, y_s, wall) 
					if not cell.northwest:
						img.line(x_fw, y_m, x_nw, y_n, wall) 
					if not cell.north:
						img.line(x_nw, y_n, x_ne, y_n, wall) 
					if not cell.linked==cell.northeast:
						img.line(x_ne, y_n, x_fe, y_m, wall)
					if not cell.linked==cell.southeast:
						img.line(x_fe, y_m, x_ne, y_s, wall)
					if not cell.linked==cell.south:
						img.line(x_ne, y_s, x_nw, y_s, wall)
				
			
		

		return img
	

