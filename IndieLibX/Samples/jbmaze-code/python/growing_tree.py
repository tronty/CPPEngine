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
class GrowingTree

	def on(grid, start_at: grid.random_cell):
		active = []
		active.append(start_at)

		while len(active):
			cell = yield active
			# growing_tree.rb:      available_neighbors = cell.neighbors.select { |n| n.links.empty? }
			available_neighbors = []
			for c in cell.m_neighbors:
				if c.links.empty:
					available_neighbors.append(c)

			if len(available_neighbors):
				neighbor = available_neighbors.sample
				cell.link(neighbor)
				active.append(neighbor)
			else:
				active.remove(cell)
			
		

		return grid
	


