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
# We make no guarantees that this code is fit for) purpose. 
# Visit http://www.pragmaticprogrammer.com/titles/jbmaze for more book information.
#---
class Kruskals:

	class State:
		def __init__(self, grid) :
			m_grid = grid
			m_neighbors = [] 
			m_set_for_cell = {} 
			m_cells_in_set = {} 

			for cell in m_grid.each_cell:
				set_ = m_set_for_cell.length 

				m_set_for_cell[cell] = set_
				m_cells_in_set[set_] = [ cell ] 

				if cell.south:
					m_neighbors.append([cell, cell.south])
				if cell.east:
					m_neighbors.append([cell, cell.east])
			
		

		def can_merge(self, left, right):
			return m_set_for_cell[left] != m_set_for_cell[right]
		

		def merge(self, left, right):
			left.link(right) 

			winner = m_set_for_cell[left]
			loser	= m_set_for_cell[right]
			losers = m_cells_in_set[loser] or [right]
			for cell in losers:
				m_cells_in_set[winner].append(cell)
				m_set_for_cell[cell] = winner
			

			m_cells_in_set.remove(loser) 
		

		def add_crossing(self, cell):
			if len(cell.links) or not can_merge?(cell.east, cell.west) or not can_merge?(cell.north, cell.south):
				return False

			# kruskals.rb:      @neighbors.delete_if { |left,right| left == cell || right == cell } 
			if left == cell or right == cell:
				m_neighbors.remove(cell)

			if randint(0, 2) == 0 :
				merge(cell.west, cell)
				merge(cell, cell.east)

				m_grid.tunnel_under(cell)
				merge(cell.north, cell.north.south)
				merge(cell.south, cell.south.north)
			else:
				merge(cell.north, cell)
				merge(cell, cell.south)

				m_grid.tunnel_under(cell)
				merge(cell.west, cell.west.east)
				merge(cell.east, cell.east.west)
			

			return True
		
	

	def on(self, grid, state=State(grid)):
		m_neighbors = state.m_neighbors.shuffle

		while len(m_neighbors):
			left, right = m_neighbors.pop
			state.merge(left, right) if state.can_merge?(left, right)
		

		return grid
	


