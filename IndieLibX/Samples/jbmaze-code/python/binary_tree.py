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
class BinaryTree:

	def on(grid):
		for cell in grid.each_cell:
			m_neighbors = []
			if cell.north:
				m_neighbors.append(cell.north)
			if cell.east: 
				m_neighbors.append(cell.east) 

			index = randint(0, m_neighbors.length) 
			neighbor = m_neighbors[index] 

			if neighbor:
				cell.link(neighbor) 
		

		return grid
	


