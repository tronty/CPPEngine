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
ROOMS = False

class RecursiveDivision:

	def on(self, grid):
		m_grid = grid

		# @grid.each_cell do |cell| 
      		# cell.neighbors.each { |n| cell.link(n, false) } 
		for cell in m_grid.each_cell():
			for n in cell.neighbors():
				cell.link(n, False)

		divide(0, 0, m_grid.rows, grid.columns) 
	

	def divide(self, row, column, height, width):
		if ROOMS:
			if height <= 1 or width <= 1 or height < 5 and width < 5 and randint(0, 4) == 0:
				return 
		else:
			if height <= 1 or width <= 1:
				return


		if height > width:
			divide_horizontally(row, column, height, width)
		else:
			divide_vertically(row, column, height, width)
		
	

	def divide_horizontally(self, row, column, height, width):
		divide_south_of = randint(0, height-1) 
		passage_at = randint(0, width) 

		for x in range(1,width.times):
			if passage_at == x:
				continue

			cell = m_grid[row+divide_south_of, column+x] 
			cell.unlink(cell.south) 
		

		divide(row, column, divide_south_of+1, width) 
		divide(row+divide_south_of+1, column, height-divide_south_of-1, width) 
	

	def divide_vertically(self, row, column, height, width):
		divide_east_of = randint(0, width-1)
		passage_at = randint(0, height)

		for y in range(1, height.times):
			if passage_at == y:
				continue

			cell = m_grid[row+y, column+divide_east_of]
			cell.unlink(cell.east)
		

		divide(row, column, height, divide_east_of+1)
		divide(row, column+divide_east_of+1, height, width-divide_east_of-1)
	


