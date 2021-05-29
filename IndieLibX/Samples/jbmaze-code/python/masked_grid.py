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

class MaskedGrid(Grid):

	def __init__(self, mask):
		m_mask = mask
		super(m_mask.rows, m_mask.columns) 
	

	def prepare_grid(self):
		m_grid = [[0 for x in range(grid.Grid.m_columns)] for y in range(grid.Grid.m_rows)] 
		for row in range(grid.Grid.m_rows):
			for column in range(grid.Grid.m_columns):
				if m_mask[row, column]:
					Cell(row, column)  
			
		
	

	def random_cell(self):
		row, col = m_mask.random_location
		self[row, col]
	

	def size(self):
		m_mask.count
	

