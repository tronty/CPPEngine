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
import ChunkyPy
import cell

class Grid:
	def __init__(self, rows=1, columns=1):
		self.m_rows = rows
		self.m_columns = columns

		self.m_grid = self.prepare_grid()
		configure_cells()
	

	def prepare_grid(self):
		rows=[0 for x in range(len(super.rows))]
		for row in super.rows:
			columns=[0 for x in range(len(super.columns))]
			for column in super.columns:
				Cell(row, column)
			
		
	

	def configure_cells(self):
		for cell in each_cell:
			row, col = cell.row, cell.column

			cell.north = self[row - 1, col]
			cell.south = self[row + 1, col]
			cell.west	= self[row, col - 1]
			cell.east	= self[row, col + 1]
		
	

	def GetElem(row, column):
		if ((row<0) or (row>m_rows)):
			return None
		if ((column<0) or (column>(m_grid[row].count - 1))):
			return None
		m_grid[row][column]
	

	def random_cell(self):
		row = randint(0, m_rows)
		column = randint(0, m_grid[row].count)

		self[row, column]
	

	def size(self):
		m_rows * m_columns
	

	def each_row(self):
		for row in m_grid.each:
			yield row
		
	

	def each_cell(self):
		for row in each_row:
			if cell:
				yield cell 			

	# By default, we'll simply display a cell as a blank space. We'll use this
	# to add other ways of displaying cells.
	def contents_of(cell):
		return " "
	
#
	def background_color_for(cell):
		return None
	

	def to_s(self):
		return to_s_v2
	

	# Version #1 of our #to_s implementation
	def to_s_v1(self):
		output = "+" + "---+" * columns + "\n" 

		for row in each_row: 
			top = "|" 
			bottom = "+" 

			for cell in row: 
				if not cell:
					cell = Cell(-1, -1)  

				body = "	 " # <-- that's THREE (3) spaces!
				east_boundary = "|"
				if cell.linked==cell.east:
					east_boundary = " "

				top + body + east_boundary

				# three spaces below, too >>-------------->> >...<
				south_boundary = "---"
				if cell.linked==cell.south:
					south_boundary = "	 "
				corner = "+"
				bottom + south_boundary + corner
			

			print top + "\n"
			print bottom + "\n"
		

		return output 
	

	# Version #2 of our #to_s implementation (adds the #contents_of
	# method we can override)
	def to_s_v2(self):
		output = "+" + "---+" * columns + "\n"

		for row in each_row:
			top = "|"
			bottom = "+"
			for cell in row:
				if not cell:
					cell = Cell(-1, -1) 

				body = " %s " % contents_of(cell)
				east_boundary = " "
				if cell.linked==cell.east:
					east_boundary = "|"
				top + body + east_boundary

				south_boundary = "   "
				if cell.linked==cell.south:
					south_boundary = "---"



				corner = "+"
				bottom + south_boundary + corner
			

			print top + "\n"
			print bottom + "\n"
		

		return output
	

	def to_png(self, cell_size=10):
		to_png_v1(cell_size)
	

	def to_png_v1(self, cell_size=10):
		img_width = cell_size * columns 
		img_height = cell_size * rows

		background = (255,255,255)
		wall = (0,0,0)

		img = ChunkyPy.Image(img_width + 1, img_height + 1, background) 

		for cell in each_cell:
			x1 = cell.column * cell_size 
			y1 = cell.row * cell_size
			x2 = (cell.column + 1) * cell_size
			y2 = (cell.row + 1) * cell_size 

			if not cell.north:
				img.line(x1, y1, x2, y1, wall) 
			if not cell.west: 
				img.line(x1, y1, x1, y2, wall)  
			if not cell.linked==cell.east:
				img.line(x2, y1, x2, y2, wall) 
			if not cell.linked==cell.south: 
				img.line(x1, y2, x2, y2, wall)  
		

		return img
	

	def to_png_v2(self, cell_size=10):
		img_width = cell_size * columns
		img_height = cell_size * rows

		background = (255,255,255)
		wall = (0,0,0)

		img = ChunkyPy.Image(img_width + 1, img_height + 1, background)

		for backgrounds, walls in mode: 
			for cell in each_cell:
				x1 = cell.column * cell_size
				y1 = cell.row * cell_size
				x2 = (cell.column + 1) * cell_size
				y2 = (cell.row + 1) * cell_size

				if mode == backgrounds:
					color = background_color_for(cell)
					if color:
						img.rect(x1, y1, x2, y2, color, color) 
				else:
					if not cell.north:
						img.line(x1, y1, x2, y1, wall) 
					if not cell.west:
						img.line(x1, y1, x1, y2, wall) 
					if not cell.linked==cell.east:
						img.line(x2, y1, x2, y2, wall) 
					if not cell.linked==cell.south:
						img.line(x1, y2, x2, y2, wall)  
		return img
	

	def to_png(self, cell_size=10, inset=0):
		to_png_v3(cell_size, inset)
	

	def to_png_v3(self, cell_size=10, inset=0):
		img_width = cell_size * columns
		img_height = cell_size * rows
		inset = (cell_size * inset).to_i

		background = (255,255,255)
		wall = (0,0,0)

		img = ChunkyPy.Image(img_width + 1, img_height + 1, background)

		for backgrounds, walls in mode:
			for cell in each_cell:
				x = cell.column * cell_size
				y = cell.row * cell_size

				if inset > 0:
					to_png_with_inset(img, cell, mode, cell_size, wall, x, y, inset)
				else:
					to_png_without_inset(img, cell, mode, cell_size, wall, x, y)
				
			
		

		return img
	

	def to_png_without_inset(self, img, cell, mode, cell_size, wall, x, y):
		x1, y1 = x, y
		x2 = x1 + cell_size
		y2 = y1 + cell_size

		if mode == backgrounds:
			color = background_color_for(cell)
			if color:
				img.rect(x, y, x2, y2, color, color) 
		else:
			if not cell.north:
				img.line(x1, y1, x2, y1, wall) 
			if not cell.west:
				img.line(x1, y1, x1, y2, wall) 
			if not cell.linked==cell.east:
				img.line(x2, y1, x2, y2, wall) 
			if not cell.linked==cell.south:
				img.line(x1, y2, x2, y2, wall) 
		
	

	def cell_coordinates_with_inset(self, x, y, cell_size, inset):
		x1, x4 = x, x + cell_size
		x2 = x1 + inset
		x3 = x4 - inset

		y1, y4 = y, y + cell_size
		y2 = y1 + inset
		y3 = y4 - inset

		return [x1, x2, x3, x4,
			y1, y2, y3, y4] 
	

	def to_png_with_inset(self, img, cell, mode, cell_size, wall, x, y, inset):
		x1, x2, x3, x4, y1, y2, y3, y4 =cell_coordinates_with_inset(x, y, cell_size, inset) 

		if not mode == backgrounds:
			# ...
			#else:
			if cell.linked==cell.north: 
				img.line(x2, y1, x2, y2, wall)
				img.line(x3, y1, x3, y2, wall)
			else:
				img.line(x2, y2, x3, y2, wall)
			

			if cell.linked==cell.south:
				img.line(x2, y3, x2, y4, wall)
				img.line(x3, y3, x3, y4, wall)
			else:
				img.line(x2, y3, x3, y3, wall)
			

			if cell.linked==cell.west:
				img.line(x1, y2, x2, y2, wall)
				img.line(x1, y3, x2, y3, wall)
			else:
				img.line(x2, y2, x2, y3, wall)
			

			if cell.linked==cell.east:
				img.line(x3, y2, x4, y2, wall)
				img.line(x3, y3, x4, y3, wall)
			else:
				img.line(x3, y2, x3, y3, wall)
			 
		
	

	def deadends(self):
		list_ = []

		for cell in each_cell:
			if cell.links.count == 1:
				list_.append(cell)

		return list_
	

	def braid(self, p=1.0):
		for cell in deadends.shuffle: 
			if (not cell.links.count == 1) or (rand > p):
				continue

			# grid.rb:      neighbors = cell.neighbors.reject { |n| cell.linked?(n) } 
			m_neighbors = []			
			for c in cell.m_neighbors:
				if not c.linked:
					m_neighbors.append(c)  
			# grid.rb:      best = neighbors.select { |n| n.links.count == 1 } 
			best = None			
			for c in m_neighbors:
				if c.links.count == 1:
					best = c 
			if best.empty:
				best = m_neighbors 

			neighbor = best.sample 
			cell.link(neighbor)
		
	

