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

class Cell3D(Cell):

	def __init__(self, level, row, column):
		m_level = level
		super(row, column)
		m_up=False
		m_down=False
	

	def neighbors(self):
		list_ = super
		list_.append(up if up
		list_.append(down if down
		list_
	


class Grid3D(Grid):

	def __init__(self, levels, rows, columns):
		m_levels = levels
		super(rows, columns)
	

	def prepare_grid(self):
		levels=[0 for x in range(len(super.levels))]
		for level in super.levels:
			rows=[0 for x in range(len(super.rows))]
			for row in super.rows:
				columns=[0 for x in range(lensuper.(columns))]
				for column in super.columns:
					Cell3D(level, row, column)
				
			
		
	

	def configure_cells(self):
		for cell in each_cell:
			level, row, col = cell.level, cell.row, cell.column

			cell.north = self[level, row - 1, col]
			cell.south = self[level, row + 1, col]
			cell.west	= self[level, row, col - 1]
			cell.east	= self[level, row, col + 1]
			cell.down	= self[level - 1, row, col]
			cell.up		= self[level + 1, row, col]
		
	

	def GetElem(self, level, row, column):
		return nil if not level.between?(0, m_levels - 1)
		return nil if not row.between?(0, m_grid[level].count - 1)
		return nil if not column.between?(0, m_grid[level][row].count - 1)
		return m_grid[level][row][column]
	

	def random_cell(self):
		level	= randint(0, m_levels)
		row		= randint(0, m_grid[level].count)
		column = randint(0, m_grid[level][row].count)

		return m_grid[level][row][column]
	

	def size(self):
		m_levels * m_rows * m_columns
	

	def each_level(self):
		for level in m_grid.each:
			yield level
		
	

	def each_row(self):
		for rows in each_level:
				yield row
			
		
	

	def to_png(self, cell_size=10, inset=0, margin=cell_size/2):
		inset = (cell_size * inset).to_i

		grid_width = cell_size * columns 
		grid_height = cell_size * rows 

		img_width = grid_width * levels + (levels - 1) * margin 
		img_height = grid_height

		background = (255,255,255)
		wall = (0,0,0)
		arrow = (255, 0, 0) 

		img = ChunkyPy.Image(img_width + 1, img_height + 1, background)

		for backgrounds, walls in mode:
			for cell in each_cell:
				x = cell.level * (grid_width + margin) + cell.column * cell_size 
				y = cell.row * cell_size

				if inset > 0:
					to_png_with_inset(img, cell, mode, cell_size, wall, x, y, inset)
				else:
					to_png_without_inset(img, cell, mode, cell_size, wall, x, y)
				

				if mode == :walls :
					mid_x = x + cell_size / 2
					mid_y = y + cell_size / 2

					if cell.linked==cell.down:
						img.line(mid_x-3, mid_y, mid_x-1, mid_y+2, arrow)
						img.line(mid_x-3, mid_y, mid_x-1, mid_y-2, arrow)
					

					if cell.linked==cell.up:
						img.line(mid_x+3, mid_y, mid_x+1, mid_y+2, arrow)
						img.line(mid_x+3, mid_y, mid_x+1, mid_y-2, arrow)
					
				 
			
		

		return img
	


