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
class Ellers:

	class RowState:
		def __init__(self, starting_set=0):
			m_cells_in_set = {}
			m_set_for_cell = []
			m_next_set = starting_set
		

		def record(self, set_, cell):
			m_set_for_cell[cell.column] = set_

			if not m_cells_in_set[set_]:
				m_cells_in_set[set_] = [] 
			m_cells_in_set[set_].append(cell)
		

		def set_for(self, cell):
			if not m_set_for_cell[cell.column]:
				record(m_next_set, cell)
				m_next_set += 1
			

			m_set_for_cell[cell.column]
		

		def merge(self, winner, loser):
			for cell in m_cells_in_set[loser]:
				m_set_for_cell[cell.column] = winner
				m_cells_in_set[winner].append(cell)
			

			m_cells_in_set.remove(loser)
		

		def next(self):
			RowState(m_next_set)
		

		def each_set(self):
			# ellers.rb:      @cells_in_set.each { |set, cells| yield set, cells }
			for set_, cells in m_cells_in_set:
				yield set_, cells
			return self
		
	

	def on(self, grid):
		row_state = RowState.new

		for row in grid.each_row:
		#for row in 			: :
				if not cell.west:
					continue 

				set_ = row_state.set_for(cell) 
				prior_set = row_state.set_for(cell.west) 

				should_link = set_ != prior_set and (cell.south.nil or randint(0, 2) == 0) 

				if should_link:
					cell.link(cell.west)
					row_state.merge(prior_set, set_) 
				
			

			if row[0].south :
				next_row = row_state.next 

				for set_, list_ in row_state.each_set: 
					for cell, index in list_.shuffle.each_with_index: 
						if index == 0 or randint(0, 3) == 0 :
							cell.link(cell.south) 
							next_row.record(row_state.set_for(cell), cell.south) 
						
					
				

				row_state = next_row 
			
		
	


