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
class RecursiveBacktracker:

	def on(self, grid, start_at):#=grid.random_cell):
		stack = [] 
		stack.append(start_at)

		while len(stack):
			current = stack.last  
			# neighbors = current.neighbors.select { |n| n.links.empty? } 
			m_neighbors = []
			for c in current.m_neighbors:
				if c.links.empty:
					m_neighbors.append(c)

			if m_neighbors.empty:
				stack.pop 
			else:
				neighbor = m_neighbors.sample 
				current.link(neighbor) 
				stack.append(neighbor) 
			
		

		return grid
	

	def recursively_on(self, grid, start_at):#=grid.random_cell):
		walk_from(start_at)
		return grid
	

	def walk_from(self, cell) :
		for neighbor in cell.m_neighbors.shuffle.each: 
			if neighbor.links.empty:
				cell.link(neighbor) 
				walk_from(neighbor) 
			
		
	


