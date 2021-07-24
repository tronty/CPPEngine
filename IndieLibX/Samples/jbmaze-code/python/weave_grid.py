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
import weave_cells

class WeaveGrid(grid.Grid):
	def __init__(self, rows, columns):
		m_under_cells = [] 
		super
	

	def prepare_grid(self):
		m_grid = [[0 for x in range(grid.Grid.m_columns)] for y in range(grid.Grid.m_rows)] 
		for row in range(grid.Grid.m_rows):
			for column in range(grid.Grid.m_columns):
				OverCell(row, column, self) 
			
		
	

	def tunnel_under(over_cell) :
		under_cell = UnderCell(over_cell)
		m_under_cells.append(under_cell)
	

	def each_cell(self):
		super

		for cell in m_under_cells: 
			yield cell
		
	

	def to_png(cell_size=10, inset=0.1):
		super
	

	def to_png_with_inset(img, cell, mode, cell_size, wall, x, y, inset):
		if cell.is_a==OverCell:
			super
		else:
			x1, x2, x3, x4, y1, y2, y3, y4 = cell_coordinates_with_inset(x, y, cell_size, inset)

			if cell.vertical_passage:
				img.line(x2, y1, x2, y2, wall)
				img.line(x3, y1, x3, y2, wall)
				img.line(x2, y3, x2, y4, wall)
				img.line(x3, y3, x3, y4, wall)
			else:
				img.line(x1, y2, x2, y2, wall)
				img.line(x1, y3, x2, y3, wall)
				img.line(x3, y2, x4, y2, wall)
				img.line(x3, y3, x4, y3, wall)
			
		
	

